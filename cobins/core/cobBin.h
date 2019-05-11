#ifndef __COB_BIN_H__
#define __COB_BIN_H__

#include <windows.h>
#include "llfsm/lianli.h"
#include "winutils/cobPipe.h"
#include "winutils/MsgCallback.h"
#include "winutils/cobDllManager.h"
#include "base/cobUtils.h"
#include "base/cobValue.h"

NS_COB_BEGIN

const int RPCRETURNBUF_MAXLEN = 1024 * 64;

#define ENGNAME_MAXLEN     128
#define ENGFILE_MAXLEN     128
#define MSGNAME_MAXLEN     256

const int WM_GMBSENGINEMSG_MIN = (WM_USER + 0x300);
const int WM_GMBSENGINEMSG_MAX = (WM_USER + 0x350);

const int WM_GMBS_INIT = (WM_GMBSENGINEMSG_MIN + 1);

const int WM_GMBS_TIMER = (WM_GMBSENGINEMSG_MIN + 3);

const int WM_GMBS_RPCSEND = (WM_GMBSENGINEMSG_MIN + 5);  //don't change
const int WM_GMBS_EXIT = (WM_GMBSENGINEMSG_MIN + 6);     //don't change
const int WM_GMBS_RPCANSWER = (WM_GMBSENGINEMSG_MIN + 7);//don't change

const int WM_GMBS_RPCPOST = (WM_GMBSENGINEMSG_MIN + 8);

const int RPCBUF_MAXLEN = 64 * 1024;

typedef struct
{
	DWORD start;
	int   rcvrlen;
	int   namelen;
	int   datalen;
	int   ctrcode;
	int   frameno;

} RpcMsgHeader_t;
#define RPCMSGHEADERLEN    sizeof(RpcMsgHeader_t)

#define RMFL_SYNC          0x00000001L
#define RMFL_USERMSG       0x00000002L
#define RMFL_TOHOST        0x00000004L
#define RMFL_ANSWER        0x00000008L

typedef struct
{
	BOOL  bNeed;
	BOOL  bDone;
	int   nFrameNo;
	BYTE  pData[RPCRETURNBUF_MAXLEN];
	int   nDataLen;
	char  lpEngName[ENGNAME_MAXLEN + 1];
	char  lpMsgName[MSGNAME_MAXLEN + 1];

} GmbsReturnCntx_t;

typedef struct
{
	HWND hGameWnd;
	HWND hHostWnd;
	HWND hUserWnd;
	int  nRpcSendState;
	DWORD dwCardId;
	DWORD dwProdId;
} GmbsPlatformCntx_t;

#define cntt_offsetof(member, type)  (unsigned long)&(((type *)0)->member)

class Probe;
class Robot;
class Bin
{
public:
    Bin();
    virtual ~Bin();

    bool create(HWND hWnd);
    void destroy();

    bool bind(HWND hWnd, const ValueMap& params = ValueMapNull);
    bool unbind();

    bool pipeListen(DWORD dwPortId);  //for pipe server
    bool pipeConnect(DWORD dwPortId); //for pipe client
    bool pipeDisconnect();

    bool addProbe(Probe& probe);
    bool removeProbe(const std::string& probeName);

    bool addRobot(Robot& robot);
    bool removeRobot(const std::string& robotName);

    Probe* getProbe(const std::string& probeName);
    Robot* getRobot(const std::string& robotName);

    bool RpcSend(const std::string& probeName, const std::string& evtName, const lianli::EvtStream& evtData, lianli::EvtStream& resultData);
    bool RpcPost(const std::string& probeName, const std::string& evtName, const lianli::EvtStream& evtData);
    bool RpcSendEvent(const std::string& probeName, const std::string& evtName, const lianli::EvtStream& evtData, DWORD ctrCode, DWORD frameNo);
    int  RpcRecvAnswer(const std::string& probeName, const std::string& evtName, lianli::EvtStream& retData);
    int  RpcReturn(const lianli::EvtStream& evtData, bool bRightNow = true);

	int  RpcSendEvent(const std::string& probeName, const std::string& evtName, PBYTE data, int dataLen, DWORD ctrCode, DWORD frameNo);

    inline HWND getMainWnd() const { return m_hMainWnd; }

protected:
    std::map<const std::string, Probe*> mProbeMap;
    std::map<const std::string, Robot*> mRobotMap;

    void onPipeReceiveData(int nErrCode);
    virtual void onRpcReceived(PBYTE pMsgInfo);

private:
	int m_nRpcFrameNo;

protected:
	Pipe m_RpcPipe;

    HWND m_hMainWnd;
    MsgCallback m_MsgCb;
    DllManager m_dllManager;

protected:
	BYTE m_RpcMsgTxdBuf[RPCBUF_MAXLEN];

public: //TBD
	int RpcSendData(PBYTE pData, int nDataLen);
	int RpcRecvData(PBYTE pDataBuf, int nBufLen);
	int RpcRecvDataEx(PBYTE pDataBuf, int nBufLen);
};

#define MSGRCVR(pMsgData) ((LPCSTR)(pMsgData + sizeof(RpcMsgHeader_t)))
#define MSGNAME(pMsgData) (MSGRCVR(pMsgData) + ((RpcMsgHeader_t*)pMsgData)->rcvrlen)
#define MSGDATA(pMsgData) ((LPCSTR)(MSGNAME(pMsgData) + ((RpcMsgHeader_t*)pMsgData)->namelen))  

NS_COB_END

#endif
