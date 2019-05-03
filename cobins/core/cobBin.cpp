#include "winutils/wndshm.h"
#include "cobBin.h"
#include "cobProbe.h"
#include "cobRobot.h"

NS_COB_BEGIN

Bin::Bin()
{
    m_nRpcFrameNo = 0;
}

Bin::~Bin()
{
    for (auto iter = mProbeMap.begin(); iter != mProbeMap.end(); iter++)
    {
        auto probe = iter->second;
        delete probe;
    }
    mProbeMap.clear();

    for (auto iter = mRobotMap.begin(); iter != mRobotMap.end(); iter++)
    {
        auto roobt = iter->second;
        delete roobt;
    }
    mRobotMap.clear();
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
        //COBLOG("Bin::onPipeReceiveData E thread=%x [%x]\n", GetCurrentThreadId(), GetCurrentProcessId());
        RpcMsgHeader_t msgHeader;
        int headerLen = RpcRecvDataEx((PBYTE)&msgHeader, sizeof(msgHeader));
        if (headerLen != sizeof(msgHeader) || msgHeader.start != 0xA2B3C4E5)
        {
            BYTE bTemBuf[256];
            int nLen = 0;
            do
            {
                nLen = RpcRecvDataEx(bTemBuf, sizeof(bTemBuf));
            } while (nLen == sizeof(bTemBuf));

            COBLOG("onPipeReceiveData: error: start=%x\n", msgHeader.start);
            return;
        }

        //COBLOG("msgHeader=(%d,%d)", msgHeader.ctrcode, msgHeader.frameno);

        PBYTE pMsgInfo = NULL;
        int nMsgInfoLen = 0;
        int nRecvLen = 0;
        int nLen = 0;

        nMsgInfoLen = sizeof(RpcMsgHeader_t) + msgHeader.rcvrlen + msgHeader.namelen + msgHeader.datalen;
        pMsgInfo = (PBYTE)malloc(nMsgInfoLen);
        if (pMsgInfo == NULL)
        {
            COBLOG("error");
            return;
        }

        memcpy(pMsgInfo, &msgHeader, sizeof(msgHeader));
        nRecvLen += sizeof(msgHeader);

        nLen = RpcRecvDataEx(pMsgInfo + nRecvLen, nMsgInfoLen - nRecvLen);
        if (nLen != nMsgInfoLen - nRecvLen)
        {
            COBLOG("error\n");
            return;
        }
        m_RpcPipe.ReceiveEnd();

        if (msgHeader.ctrcode != RMFL_ANSWER)
        {
            m_MsgCb.post([this, pMsgInfo]() {
                onRpcReceived(pMsgInfo);
                delete pMsgInfo;
            });
        }
        else
        {
            PostMessage(m_MsgCb.getMainWnd(), WM_GMBS_RPCANSWER, (WPARAM)pMsgInfo, 0);
        }
    }
}

void Bin::onRpcReceived(PBYTE pMsgInfo)
{
    RpcMsgHeader_t msgHeader;
    memcpy(&msgHeader, pMsgInfo, sizeof(msgHeader));

    const char* recvName = MSGRCVR(pMsgInfo);
    const char* evtName = MSGNAME(pMsgInfo);
    const char* recvData = MSGDATA(pMsgInfo);
    //COBLOG("Bin::onRpcReceived: recv=%s, msg=%s, data=%s\n", recvName, evtName, recvData);

    auto iter = mProbeMap.find(recvName);
    if (iter != mProbeMap.end())
    {
        auto probe = iter->second;
        if (msgHeader.ctrcode == RMFL_SYNC)
        {
            lianli::EvtStream evtData, retData;
            evtData.write(recvData, msgHeader.datalen);
            probe->onRequest(evtName, evtData, retData);
            RpcSendEvent(MSGRCVR(pMsgInfo), MSGNAME(pMsgInfo), retData, RMFL_ANSWER, msgHeader.frameno);
        }
        else
        {
            lianli::EvtStream evtData;
            evtData.write(recvData, msgHeader.datalen);
            probe->onNotify(evtName, evtData);
        }
    }
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
        ret = pipeConnect((DWORD)hWnd);
        COBLOG("Bin::bind: connect ret = %d\n", ret);
    }
    return ret;
}

bool Bin::unbind()
{
    m_dllManager.eject();
    return true;
}

bool Bin::addProbe(Probe& probe)
{
    auto iter = mProbeMap.find(probe.getName());
    COBASSERT(iter == mProbeMap.end(), "install failed: probe existed already");

	mProbeMap.insert(std::make_pair(probe.getName(), &probe));
    probe.mBin = this;
	return true;
}

bool Bin::removeProbe(const std::string& probeName)
{
    auto iter = mProbeMap.find(probeName);
    if (iter == mProbeMap.end())
        return false;

    mProbeMap.erase(iter);
	return true;
}

bool Bin::addRobot(Robot& robot)
{
    auto iter = mRobotMap.find(robot.getName());
    COBASSERT(iter == mRobotMap.end(), "install failed: robot existed already");

    mRobotMap.insert(std::make_pair(robot.getName(), &robot));
    robot.mBin = this;
	return true;
}

bool Bin::removeRobot(const std::string& robotName)
{
    auto iter = mRobotMap.find(robotName);
    if (iter == mRobotMap.end())
        return false;

    mRobotMap.erase(iter);
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
    auto iter = mRobotMap.find(robotName);
    if (iter == mRobotMap.end())
        return nullptr;

	return iter->second;
}

bool Bin::RpcSend(const std::string& probeName, const std::string& evtName, const lianli::EvtStream& evtData, lianli::EvtStream& resultData)
{
    if (RpcSendEvent(probeName, evtName, evtData, RMFL_SYNC, ++m_nRpcFrameNo) > 0)
    {
        if (RpcRecvAnswer(probeName, evtName, resultData) == 0)
        {

        }
    }
    return true;
}

int Bin::RpcRecvAnswer(const std::string& probeName, const std::string& evtName, lianli::EvtStream& retData)
{
    MSG   msg;
    while (GetMessage(&msg, NULL, WM_GMBS_RPCANSWER, WM_GMBS_RPCANSWER + 1))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_GMBS_RPCANSWER)
            break;
    }

    int result = 0;
    if (msg.message == WM_GMBS_RPCANSWER)
    {
        PBYTE pMsgInfo = (PBYTE)msg.wParam;
        RpcMsgHeader_t* pMsgHeader = (RpcMsgHeader_t*)pMsgInfo;

        if (probeName == MSGRCVR(pMsgInfo) &&
            evtName == MSGNAME(pMsgInfo) && pMsgHeader->frameno == m_nRpcFrameNo)
        {
            retData.write(MSGDATA(pMsgInfo), pMsgHeader->datalen);
            result = 0;
        }
        else
        {
            result = -1;

            COBLOG("RpcRecvAnswer: error (%s)!=%s (%s)!=%s\n (%d)!=%d ctr=%x\n", MSGRCVR(pMsgInfo), probeName.c_str(),
                MSGNAME(pMsgInfo), evtName.c_str(), pMsgHeader->frameno, m_nRpcFrameNo, pMsgHeader->ctrcode);
        }
        delete pMsgInfo;
    }

    return 0;
}

bool Bin::RpcPost(const std::string& probeName, const std::string& evtName, const lianli::EvtStream& evtData)
{
	int lResult = 0;

	if (RpcSendEvent(probeName, evtName, evtData, 0, 0))
		lResult = -1;

	return lResult;
}

int Bin::RpcReturn(const lianli::EvtStream& evtData, bool bRightNow)
{
    return evtData.size();
}

int Bin::RpcSendEvent(const std::string& probeName, const std::string& evtName, const lianli::EvtStream& evtData, DWORD ctrCode, DWORD frameNo)
{
    RpcMsgHeader_t MsgHeader;
    int nMsgLen = 0;

    MsgHeader.start = 0xA2B3C4E5;
    MsgHeader.rcvrlen = probeName.size() + 1;
    MsgHeader.namelen = evtName.size() + 1;
    MsgHeader.datalen = evtData.size();
    MsgHeader.ctrcode = ctrCode;
    MsgHeader.frameno = frameNo;

    memcpy(m_RpcMsgTxdBuf, &MsgHeader, sizeof(MsgHeader));
    nMsgLen += sizeof(MsgHeader);

    memcpy(m_RpcMsgTxdBuf + nMsgLen, probeName.c_str(), MsgHeader.rcvrlen);
    nMsgLen += MsgHeader.rcvrlen;

    memcpy(m_RpcMsgTxdBuf + nMsgLen, evtName.c_str(), MsgHeader.namelen);
    nMsgLen += MsgHeader.namelen;

    if (!evtData.empty() && MsgHeader.datalen > 0)
    {
        //memcpy(m_RpcMsgTxdBuf + nMsgLen, evtData.getData(), MsgHeader.datalen);
        ((lianli::EvtStream&)evtData).read((char*)m_RpcMsgTxdBuf + nMsgLen, RPCBUF_MAXLEN - nMsgLen);
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

int Bin::RpcSendEvent(const std::string& probeName, const std::string& evtName, PBYTE data, int dataLen, DWORD ctrCode, DWORD frameNo)
{
	RpcMsgHeader_t MsgHeader;
	int nMsgLen = 0;

	MsgHeader.start = 0xA2B3C4E5;
	MsgHeader.rcvrlen = probeName.size() + 1;
	MsgHeader.namelen = evtName.size() + 1;
	MsgHeader.datalen = dataLen;
	MsgHeader.ctrcode = ctrCode;
	MsgHeader.frameno = frameNo;

	memcpy(m_RpcMsgTxdBuf, &MsgHeader, sizeof(MsgHeader));
	nMsgLen += sizeof(MsgHeader);

	memcpy(m_RpcMsgTxdBuf + nMsgLen, probeName.c_str(), MsgHeader.rcvrlen);
	nMsgLen += MsgHeader.rcvrlen;

	memcpy(m_RpcMsgTxdBuf + nMsgLen, evtName.c_str(), MsgHeader.namelen);
	nMsgLen += MsgHeader.namelen;

	if (data != NULL && MsgHeader.datalen > 0)
	{
		memcpy(m_RpcMsgTxdBuf + nMsgLen, data, MsgHeader.datalen);
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
			COBLOG("Bin::RpcRecvDataEx: no more data\n");
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
