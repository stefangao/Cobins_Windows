#include "winutils/wndshm.h"
#include "cobBin.h"
#include "cobProbe.h"
#include "cobRobot.h"

NS_COB_BEGIN

Bin::Bin()
{

}

bool Bin::create(HWND hWnd, int portId)
{
    bool ret = false;
    if (!m_RpcPipe.IsPipe())
    {
        ret = m_RpcPipe.CreatePipe(portId);
        if (ret)
        {
            std::function<void(int)> callback = std::bind(&Bin::onPipeReceiveData, this, std::placeholders::_1);
            m_RpcPipe.RegisterRxd(callback);
        }
    }
    m_hMainWnd = hWnd;
    m_MsgCallback.SetWndProc(hWnd);
    return ret;
}

bool Bin::connect(int portId)
{
    bool ret = false;
    if (!m_RpcPipe.IsPipe())
    {
        ret = m_RpcPipe.OpenPipe(portId);
        if (ret)
        {
            std::function<void(int)> callback = std::bind(&Bin::onPipeReceiveData, this, std::placeholders::_1);
            m_RpcPipe.RegisterRxd(callback);

            COBLOG("Bin::connect: success");
        }
        else
        {
            COBLOG("Bin::connect: failed");
        }
    }
    return ret;
}

bool Bin::disconnect()
{
	return true;
}

bool Bin::install(Probe& probe)
{
	mProbeMap.insert(std::make_pair(probe.getName(), &probe));
	return true;
}

bool Bin::uninstall(const std::string& probeName)
{
	return true;
}

Probe* Bin::getProbe(const std::string& probeName)
{

	return nullptr;
}

Robot* Bin::getRobot(const std::string& robotName)
{
	return nullptr;
}

int Bin::RpcSend(LPCSTR strEngineName, LPCSTR lpMsgName, PBYTE pMsgData, int nMsgDataLen, PBYTE& pResultData, int& nResultDataLen)
{
	MSG msg;

	//WT_Printf("[%s]RpcSend0000\n", m_strPrefix);

	nResultDataLen = 0;

	if (m_RpcReturnCntx.bNeed && !m_RpcReturnCntx.bDone)
	{
		RpcReturn(NULL, 0, TRUE);
		COBLOG("------------------------ RpcSend: 解锁------------------------\n");
	}

	RpcSendLock();
	if (GetRpcState() == 0)
	{
		SetRpcState(1);
		RpcSendUnlock();
	}
	else
	{
		RpcSendUnlock();

		//WT_Printf("%s: wait rpcsend\n", m_strPrefix);
		while (GetMessage(&msg, NULL, WM_GMBS_RPCSEND, WM_GMBS_EXIT))
		{
			//WT_Printf("%s: recv rpcsend\n", m_strPrefix);

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_GMBS_EXIT)
				return -1;

			RpcSendLock();
			if (GetRpcState() == 0)
			{
				SetRpcState(1);
				RpcSendUnlock();
				break;
			}
			else
			{
				RpcSendUnlock();
				//COBLOG("============================== 仍然在锁定 ==============================\n");
			}
		}
	}

	//WT_Printf("[%s]RpcSend1111\n", m_strPrefix);

	//如果运行到这里一定是锁住了RPC
	int lResult = -1;
	if (RpcSendMessage(strEngineName, lpMsgName, pMsgData, nMsgDataLen, RMFL_SYNC, ++m_nRpcFrameNo) > 0)
	{
		if (RpcRecvAnswer(strEngineName, lpMsgName, pResultData, nResultDataLen) == 0)
		{
			if (strncmp((LPCSTR)pResultData, "Error", 5) != 0)
				lResult = 0; //成功
		}
		else
		{
			COBLOG("[%s]RpcSend: Wrong Answer (nResultDataLen=%d)\n", m_strPrefix, nResultDataLen);
		}
	}

	if (lResult == -1)
	{
		RpcSendLock();
		SetRpcState(0);
		RpcSendUnlock();
		COBLOG("[%s]RpcSend: 执行失败\n", m_strPrefix);
	}

	//WT_Printf("[%s]RpcSend2222\n", m_strPrefix);

	return lResult;
}

int Bin::RpcPost(LPCSTR strEngineName, LPCSTR lpMsgName, PBYTE pMsgData, int nMsgDataLen)
{
	int lResult = 0;

	if (RpcSendMessage(strEngineName, lpMsgName, pMsgData, nMsgDataLen) < 0)
		lResult = -1;

	return lResult;
}


int Bin::RpcReturn(PBYTE pResultData, int nResultDataLen, BOOL bRightNow)
{
	if (pResultData != NULL && nResultDataLen > 0)
	{
		if (m_RpcReturnCntx.nDataLen + nResultDataLen <= RPCRETURNBUF_MAXLEN)
		{
			memcpy(m_RpcReturnCntx.pData + m_RpcReturnCntx.nDataLen, pResultData, nResultDataLen);
			m_RpcReturnCntx.nDataLen += nResultDataLen;
		}
		else
		{
			COBLOG("Bin::RpcReturn: Buffer不够\n");
			return -1;
		}
	}

	if (bRightNow && m_RpcReturnCntx.bNeed && !m_RpcReturnCntx.bDone)
	{
		RpcSendLock();
		SetRpcState(0);
		RpcSendMessage(m_RpcReturnCntx.lpEngName, m_RpcReturnCntx.lpMsgName, m_RpcReturnCntx.pData, m_RpcReturnCntx.nDataLen, RMFL_ANSWER, m_RpcReturnCntx.nFrameNo);
		m_RpcReturnCntx.nDataLen = 0;
		m_RpcReturnCntx.bDone = TRUE;
		RpcSendUnlock();
	}

	return nResultDataLen;
}

BOOL Bin::RpcSendLock()
{
	if (WaitForSingleObject(m_hRpcSendMutex, INFINITE) == WAIT_OBJECT_0)
		return TRUE;

	return FALSE;
}

BOOL Bin::RpcSendUnlock()
{
	return ReleaseMutex(m_hRpcSendMutex);
}

int Bin::RpcSendMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen, DWORD ctrcode)
{
	RpcMsgHeader_t MsgHeader;
	int nMsgLen = 0;

	MsgHeader.start = 0xA2B3C4E5;
	MsgHeader.rcvrlen = strlen(receiver) + 1;
	MsgHeader.namelen = strlen(msgname) + 1;
	MsgHeader.datalen = msgdatalen;
	MsgHeader.ctrcode = ctrcode;
	MsgHeader.frameno = ++m_nRpcFrameNo;

	memcpy(m_RpcMsgTxdBuf, &MsgHeader, sizeof(MsgHeader));
	nMsgLen += sizeof(MsgHeader);

	memcpy(m_RpcMsgTxdBuf + nMsgLen, receiver, MsgHeader.rcvrlen);
	nMsgLen += MsgHeader.rcvrlen;

	memcpy(m_RpcMsgTxdBuf + nMsgLen, msgname, MsgHeader.namelen);
	nMsgLen += MsgHeader.namelen;

	if (msgdata != NULL && MsgHeader.datalen > 0)
	{
		memcpy(m_RpcMsgTxdBuf + nMsgLen, msgdata, MsgHeader.datalen);
		nMsgLen += MsgHeader.datalen;
	}

	int nSentLen = RpcSendData(m_RpcMsgTxdBuf, nMsgLen);
	if (nSentLen != nMsgLen)
	{
		COBLOG("RpcSendMessage: error (%d != %d\n", nSentLen, nMsgLen);
		nSentLen = -1;
	}

	return nSentLen;
}

int Bin::RpcSendMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen, DWORD ctrcode, DWORD frameno)
{
	RpcMsgHeader_t MsgHeader;
	int nMsgLen = 0;

	MsgHeader.start = 0xA2B3C4E5;
	MsgHeader.rcvrlen = strlen(receiver) + 1;
	MsgHeader.namelen = strlen(msgname) + 1;
	MsgHeader.datalen = msgdatalen;
	MsgHeader.ctrcode = ctrcode;
	MsgHeader.frameno = frameno;

	memcpy(m_RpcMsgTxdBuf, &MsgHeader, sizeof(MsgHeader));
	nMsgLen += sizeof(MsgHeader);

	memcpy(m_RpcMsgTxdBuf + nMsgLen, receiver, MsgHeader.rcvrlen);
	nMsgLen += MsgHeader.rcvrlen;

	memcpy(m_RpcMsgTxdBuf + nMsgLen, msgname, MsgHeader.namelen);
	nMsgLen += MsgHeader.namelen;

	if (msgdata != NULL && MsgHeader.datalen > 0)
	{
		memcpy(m_RpcMsgTxdBuf + nMsgLen, msgdata, MsgHeader.datalen);
		nMsgLen += MsgHeader.datalen;
	}

	int nSentLen = RpcSendData(m_RpcMsgTxdBuf, nMsgLen);
	if (nSentLen != nMsgLen)
	{
		COBLOG("RpcSendMessage: error (%d != %d\n", nSentLen, nMsgLen);
		nSentLen = -1;
	}

	return nSentLen;
}


int Bin::RpcRecvAnswer(LPCSTR receiver, LPCSTR msgname, PBYTE &msgdata, int &msgdatalen)
{
	MSG   msg;
	int   nResult = 0;

	DWORD dwStartTicks = GetTickCount();
	while (!PeekMessage(&msg, m_hMainWnd, WM_GMBS_EXIT, WM_GMBS_RPCANSWER, PM_REMOVE))
	{
		if (GetTickCount() > dwStartTicks + 8000)
		{
			COBLOG("[%s]:=====等待应答超时=====\n", m_strPrefix);
			msgdatalen = 0;
			return -1;
		}
		Sleep(10);
	}

	//保存收到的应答(消息处理函数会释放pMsgData)
	if (msg.message == WM_GMBS_RPCANSWER)
	{
		PBYTE pMsgInfo = (PBYTE)msg.wParam;
		RpcMsgHeader_t* pMsgHeader = (RpcMsgHeader_t*)pMsgInfo;

		if (strcmp(MSGRCVR(pMsgInfo), receiver) == 0 &&
			strcmp(MSGNAME(pMsgInfo), msgname) == 0 &&
			pMsgHeader->frameno == m_nRpcFrameNo)
		{
			memcpy(m_RpcMsgRxdBuf, MSGDATA(pMsgInfo), pMsgHeader->datalen);
			msgdata = m_RpcMsgRxdBuf;
			msgdatalen = pMsgHeader->datalen;
			nResult = 0;

			//WT_Printf("[%s]:RpcRecvAnswer=%s %s %d\n", m_strPrefix, MSGRCVR(pMsgInfo), msgname, pMsgHeader->frameno);
		}
		else
		{
			msgdatalen = 0;
			nResult = -1;

			COBLOG("[%s]:RpcRecvAnswer: 错误应答: (%s)!=%s (%s)!=%s\n (%d)!=%d ctr=%x\n", m_strPrefix, MSGRCVR(pMsgInfo), receiver,
				MSGNAME(pMsgInfo), msgname, pMsgHeader->frameno, m_nRpcFrameNo, pMsgHeader->ctrcode);
		}
	}
	else
	{
		msgdatalen = 0;
		nResult = -1;
	}

	//分发消息并处理
	TranslateMessage(&msg);
	DispatchMessage(&msg);

	return nResult;
}


int  Bin::GetRpcState()
{
	if (m_hPlatformCntx == NULL)
		return NULL;

	int *pnRpcState = (int*)WBS_Shm_GetDataEx(m_hPlatformCntx, cntt_offsetof(nRpcSendState, GmbsPlatformCntx_t));
	return *pnRpcState;
}

void Bin::SetRpcState(int state)
{
	if (m_hPlatformCntx == NULL)
		return;

	WBS_Shm_SetDataEx(m_hPlatformCntx, &state, cntt_offsetof(nRpcSendState, GmbsPlatformCntx_t), sizeof(state));
}


int Bin::RpcSendData(PBYTE pData, int nDataLen)
{
	int nTotalLen = 0, nLen = 0;

	while (nTotalLen < nDataLen)
	{
		nLen = m_RpcPipe.Send(pData + nTotalLen, nDataLen - nTotalLen);
		if (nLen < 0)
		{
			COBLOG("Pipe异常\n");
			return -2;
		}
		else if (nLen == 0)
		{
			COBLOG("Pipe发送失败\n");
			return -3;
		}

		nTotalLen += nLen;
	}

	return nTotalLen;
}

int Bin::RpcRecvData(PBYTE pDataBuf, int nBufLen)
{
	int nRecvLen = 0;

	if (m_RpcPipe.GetDataSize() <= 0)
	{
		COBLOG("----------------RpcRecvData无数据-------------------\n");
	}

	nRecvLen = m_RpcPipe.Receive(pDataBuf, nBufLen);
	return nRecvLen;
}

int Bin::RpcRecvDataEx(PBYTE pDataBuf, int nBufLen)
{
	int nLen = 0, nRecvLen = 0;

	do
	{
		nLen = m_RpcPipe.Receive(pDataBuf + nRecvLen, nBufLen - nRecvLen);
		if (nLen <= 0)
		{
			COBLOG("CGmbsThread::RpcRecvDataEx: error\n");
			break;
		}

		nRecvLen += nLen;

	} while (nRecvLen < nBufLen);

	return nRecvLen;
}

void Bin::onPipeReceiveData(int nErrCode)
{
    if (nErrCode == 0)
    {
        COBLOG("-*-postCallback begin mainwnd=%x\n", m_hMainWnd);
        m_MsgCallback.post([this]() {
            char buf[256];
            int len = m_RpcPipe.Receive((BYTE*)buf, 256);
            WT_Trace("Bin::onPipeReceiveData: len=%d buf=%s\n", len, buf);
            if (strcmp(buf, "cmd_unhook") == 0)
            {
                COBLOG("Bin::destroy: begin\n");
                destroy();
                COBLOG("Bin::destroy: end\n");
            }
        });


#if 0
        lianli::postCallback([this](const void* userData) {
            char buf[256];
            int len = m_RpcPipe.Receive((BYTE*)buf, 256);
            if (len > 0)
            {
                WT_Trace("Bin::onPipeReceiveData: len=%d buf=%s\n", len, buf);
                if (strcmp(buf, "cmd_unhook") == 0)
                {
                    COBLOG("Bin::destroy: begin\n");
                    destroy();
                    COBLOG("Bin::destroy: end\n");
                }
            }
        });
#endif
    }

#if 0
    CGmbsThread *pGmbsThread;
    RpcMsgHeader_t MsgHeader;
    PBYTE pMsgInfo = NULL;
    int nMsgInfoLen = 0;
    int nRecvLen = 0;
    int nLen = 0;

    if (nErrCode == -1)
    {
        PostMessage(pGmbsThread->m_hMainWnd, WM_GMBS_PIPEBROKEN, NULL, NULL);
        PostMessage(pGmbsThread->m_hMainWnd, WM_GMBS_EXIT, NULL, NULL);

        WT_Error("_OnPipeReceiveProc: Pipe broken\n");
        return;
    }

    nLen = pGmbsThread->RpcRecvDataEx((PBYTE)&MsgHeader, sizeof(MsgHeader));
    if (nLen != sizeof(MsgHeader))
    {
        WT_Error("----------丢弃Pipe当前所有数据[MsgHead长度不够]-------------\n");
        return;
    }

    if (MsgHeader.start != 0xA2B3C4E5)
    {
        BYTE bTemBuf[256];
        do
        {
            nLen = pGmbsThread->RpcRecvDataEx(bTemBuf, sizeof(bTemBuf));

        } while (nLen == sizeof(bTemBuf));

        WT_Error("----------丢弃Pipe当前所有数据[启动码不对start=%x]-------------\n", MsgHeader.start);

        return;
    }

    nMsgInfoLen = sizeof(RpcMsgHeader_t) + MsgHeader.rcvrlen + MsgHeader.namelen + MsgHeader.datalen;
    pMsgInfo = (PBYTE)cntt_malloc(nMsgInfoLen);
    if (pMsgInfo == NULL)
    {
        WT_Error("_OnPipeReceiveProc: 内存不足\n");
        return;
    }

    memcpy(pMsgInfo, &MsgHeader, sizeof(MsgHeader));
    nRecvLen += sizeof(MsgHeader);

    nLen = pGmbsThread->RpcRecvDataEx(pMsgInfo + nRecvLen, nMsgInfoLen - nRecvLen);
    if (nLen != nMsgInfoLen - nRecvLen)
    {
        WT_Error("----------丢弃Pipe当前所有数据[消息长度不够]-------------\n");
        return;
    }

    if ((MsgHeader.ctrcode & RMFL_SYNC) == RMFL_SYNC)
    {
        //WT_Printf("%s:-pipedata_send-[%d]\n", ((CGmbsContainer*)pGmbsThread)->m_strPrefix, MsgHeader.frameno);
        PostMessage(pGmbsThread->m_hMainWnd, WM_GMBS_RPCSEND, (WPARAM)pMsgInfo, nMsgInfoLen);
    }
    else if ((MsgHeader.ctrcode & RMFL_ANSWER) == RMFL_ANSWER)
    {
        //WT_Printf("%s:-pipedata_answ-[%d]\n", ((CGmbsContainer*)pGmbsThread)->m_strPrefix, MsgHeader.frameno);
        PostMessage(pGmbsThread->m_hMainWnd, WM_GMBS_RPCANSWER, (WPARAM)pMsgInfo, nMsgInfoLen);
    }
    else if ((MsgHeader.ctrcode & RMFL_USERMSG) == RMFL_USERMSG)
    {
        PostMessage(pGmbsThread->m_hMainWnd, WM_GMBS_USERMSG, (WPARAM)pMsgInfo, nMsgInfoLen);
    }
    else
    {
        PostMessage(pGmbsThread->m_hMainWnd, WM_GMBS_RPCPOST, (WPARAM)pMsgInfo, nMsgInfoLen);
    }
#endif
}

void Bin::destroy()
{
    //m_RpcPipe.DestroyPipe();
    m_RpcPipe.ClosePipe();
    m_MsgCallback.ResetWndProc();
}

NS_COB_END
