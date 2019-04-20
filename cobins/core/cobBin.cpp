#include "winutils/wndshm.h"
#include "cobBin.h"
#include "cobProbe.h"
#include "cobRobot.h"

NS_COB_BEGIN

Bin::Bin()
{
    memset(&m_RpcReturnCntx, 0, sizeof(m_RpcReturnCntx));
}

bool Bin::create(HWND hWnd)
{
    m_hMainWnd = hWnd;
    m_MsgCb.SetWndProc(hWnd);
    return TRUE;
}

bool Bin::pipeListen(DWORD dwPortId)
{
    bool ret = false;
    if (!m_RpcPipe.IsPipe())
    {
        ret = m_RpcPipe.CreatePipe(dwPortId);
        if (ret)
        {
            std::function<void(int)> callback = std::bind(&Bin::onPipeReceiveData, this, std::placeholders::_1);
            m_RpcPipe.RegisterRxd(callback);

            COBLOG("Bin::pipeListen: [%d] success\n", dwPortId);
        }
    }
    return ret;
}

bool Bin::pipeConnect(DWORD dwPortId)
{
    bool ret = false;
    if (!m_RpcPipe.IsPipe())
    {
        ret = m_RpcPipe.OpenPipe(dwPortId);
        if (ret)
        {
            std::function<void(int)> callback = std::bind(&Bin::onPipeReceiveData, this, std::placeholders::_1);
            m_RpcPipe.RegisterRxd(callback);
            COBLOG("Bin::pipeConnect: success\n");
        }
        else
        {
            COBLOG("Bin::pipeConnect: failed\n");
        }
    }
    return ret;
}


void Bin::onPipeReceiveData(int nErrCode)
{
    if (nErrCode == 0)
    {
        COBLOG("Bin::onPipeReceiveData E thread=%x [%x]\n", GetCurrentThreadId(), GetCurrentProcessId());
        m_MsgCb.post([this]() {
            onRpcReceived();

            /*
            char buf[256];
            int len = m_RpcPipe.Receive((BYTE*)buf, 256);
            WT_Trace("Bin::onPipeReceiveData X: thread=%x [%x] len=%d buf=%s\n", GetCurrentThreadId(), GetCurrentProcessId(), len, buf);

            auto prober = getProbe("prober1");
            if (prober)
            {
                lianli::EvtData evtData, retData;
                evtData << buf;

                m_RpcReturnCntx.bNeed = true;
                m_RpcReturnCntx.bDone = false;
                m_RpcReturnCntx.nDataLen = 0;
                prober->onRequest("Test123", evtData, retData);
            }*/
        });
    }


#if 0
    char buf[256];
    int len = m_RpcPipe.Receive((BYTE*)buf, 256);
    WT_Trace("Bin::onPipeReceiveData X: thread=%x len=%d buf=%s\n", GetCurrentThreadId(), len, buf);
    if (strcmp(buf, "cmd_unhook") == 0)
    {
        COBLOG("Bin::destroy: begin\n");
        m_MsgCb.post([this]()
        {
            destroy();
        });
        COBLOG("Bin::destroy: end\n");
    }
    else if (strcmp(buf, "hello123") == 0)
    {
        MessageBox(m_hMainWnd, buf, "test", 0);
    }
#endif
}

void Bin::onRpcReceived()
{
    RpcMsgHeader_t MsgHeader;
    PBYTE pMsgInfo = NULL;
    int nMsgInfoLen = 0;
    int nRecvLen = 0;
    int nLen = 0;

    nLen = RpcRecvDataEx((PBYTE)&MsgHeader, sizeof(MsgHeader));
    if (nLen != sizeof(MsgHeader))
    {
        COBLOG("----------error-------------\n");
        return;
    }

    if (MsgHeader.start != 0xA2B3C4E5)
    {
        BYTE bTemBuf[256];
        do
        {
            nLen = RpcRecvDataEx(bTemBuf, sizeof(bTemBuf));

        } while (nLen == sizeof(bTemBuf));

        COBLOG("---------error: %d-------------\n", MsgHeader.start);

        return;
    }

    nMsgInfoLen = sizeof(RpcMsgHeader_t) + MsgHeader.rcvrlen + MsgHeader.namelen + MsgHeader.datalen;
    pMsgInfo = (PBYTE)malloc(nMsgInfoLen);
    if (pMsgInfo == NULL)
    {
        COBLOG("error");
        return;
    }

    memcpy(pMsgInfo, &MsgHeader, sizeof(MsgHeader));
    nRecvLen += sizeof(MsgHeader);

    nLen = RpcRecvDataEx(pMsgInfo + nRecvLen, nMsgInfoLen - nRecvLen);
    if (nLen != nMsgInfoLen - nRecvLen)
    {
        COBLOG("error\n");
        return;
    }

#if 1
    const char* recvName = MSGRCVR(pMsgInfo);
    const char* msgName = MSGNAME(pMsgInfo);
    const char* msgData = (const char*)MSGDATA(pMsgInfo);
    COBLOG("Bin::onRpcReceived: recv=%s, msg=%s, data=%s\n", recvName, msgName, msgData);

    auto prober = getProbe("prober1");
    if (prober)
    {
        lianli::EvtData evtData, retData;
        evtData << msgData;

        m_RpcReturnCntx.bNeed = true;
        m_RpcReturnCntx.bDone = false;
        m_RpcReturnCntx.nDataLen = 0;
        prober->onRequest(msgName, evtData, retData);
    }
#endif
}

bool Bin::pipeDisconnect()
{
    if (!m_RpcPipe.IsPipe())
    {
        m_RpcPipe.ClosePipe();
    }
	return true;
}

bool Bin::bind(HWND hWnd, const ValueMap& params)
{
    bool ret = m_dllManager.inject(hWnd, "spy_dll.dll");
	COBLOG("Bin::bind: inject ret = %d\n", ret);
    if (ret)
    {
        m_MsgCb.wait(2000, [this, hWnd]()
        {
            bool ret = pipeConnect(0x1234);
            COBLOG("Bin::bind: connect ret = %d\n", ret);
        });
    }
    return ret;
}

bool Bin::unbind()
{
    m_dllManager.eject();
    return true;
}

bool Bin::install(Probe& probe)
{
    auto iter = mProbeMap.find(probe.getName());
    COBASSERT(iter == mProbeMap.end(), "install failed: existed already");

	mProbeMap.insert(std::make_pair(probe.getName(), &probe));
    probe.mBin = this;
	return true;
}

bool Bin::uninstall(const std::string& probeName)
{
    auto iter = mProbeMap.find("probeName");
    if (iter == mProbeMap.end())
        return false;

    mProbeMap.erase(iter);
	return true;
}

Probe* Bin::getProbe(const std::string& probeName)
{
    auto iter = mProbeMap.find(probeName);
    if (iter == mProbeMap.end())
        return nullptr;

	return iter->second;
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
		COBLOG("------------------------ RpcSend:------------------------\n");
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
			}
		}
	}

	int lResult = -1;
	if (RpcSendMessage(strEngineName, lpMsgName, pMsgData, nMsgDataLen, RMFL_SYNC, ++m_nRpcFrameNo) > 0)
	{
        /*
		if (RpcRecvAnswer(strEngineName, lpMsgName, pResultData, nResultDataLen) == 0)
		{
			if (strncmp((LPCSTR)pResultData, "Error", 5) != 0)
				lResult = 0;
		}
		else
		{
			COBLOG("[%s]RpcSend: Wrong Answer (nResultDataLen=%d)\n", m_strPrefix, nResultDataLen);
		}*/
	}

	if (lResult == -1)
	{
		RpcSendLock();
		SetRpcState(0);
		RpcSendUnlock();
		//COBLOG("[%s]RpcSend: error\n", m_strPrefix);
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
			COBLOG("Bin::RpcReturn: Buffer error\n");
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
		if (GetTickCount() > dwStartTicks + 20000)
		{
			COBLOG("[%s]:error\n", m_strPrefix);
			msgdatalen = 0;
			return -1;
		}
		Sleep(10);
	}

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

			COBLOG("[%s]:RpcRecvAnswer: error (%s)!=%s (%s)!=%s\n (%d)!=%d ctr=%x\n", m_strPrefix, MSGRCVR(pMsgInfo), receiver,
				MSGNAME(pMsgInfo), msgname, pMsgHeader->frameno, m_nRpcFrameNo, pMsgHeader->ctrcode);
		}
	}
	else
	{
		msgdatalen = 0;
		nResult = -1;
	}

	TranslateMessage(&msg);
	DispatchMessage(&msg);
	return nResult;
}


int  Bin::GetRpcState()
{
    return 0;
	if (m_hPlatformCntx == NULL)
		return NULL;

	int *pnRpcState = (int*)WBS_Shm_GetDataEx(m_hPlatformCntx, cntt_offsetof(nRpcSendState, GmbsPlatformCntx_t));
	return *pnRpcState;
}

void Bin::SetRpcState(int state)
{
    return;
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
			COBLOG("RpcSendData: error1\n");
			return -2;
		}
		else if (nLen == 0)
		{
			COBLOG("RpcSendData: error2\n");
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
		COBLOG("RpcRecvData error\n");
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

void Bin::destroy()
{
    m_RpcPipe.ClosePipe();
    m_MsgCb.ResetWndProc();
}

NS_COB_END
