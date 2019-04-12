#include "wndshm.h"
#include "cobBin.h"
#include "cobProbe.h"
#include "cobRobot.h"

NS_COB_BEGIN

Bin::Bin()
{

}

bool Bin::create(const std::string& name, Context& context)
{
    if (!lianli::FSM::create(name, context))
        return false;


    return true;
}

bool Bin::connect()
{
	return true;
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

NS_COB_END
