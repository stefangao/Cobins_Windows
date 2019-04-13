#ifndef __COB_BIN_H__
#define __COB_BIN_H__

#include <windows.h>
#include "lianli.h"
#include "cobConst.h"
#include "cobContext.h"
#include "BindPipe.h"

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

    bool create(int portId);
    void destroy();

    bool connect(int portId);
    bool disconnect();

    bool install(Probe& probe);
    bool uninstall(const std::string& probeName);

    Probe* getProbe(const std::string& probeName);
    Robot* getRobot(const std::string& robotName);

	int RpcSend(LPCSTR strEngineName, LPCSTR lpMsgName, PBYTE pMsgData, int nMsgDataLen, PBYTE& pResultData, int& nResultDataLen);
	int RpcPost(LPCSTR strEngineName, LPCSTR lpMsgName, PBYTE pMsgData, int nMsgDataLen);
	int RpcReturn(PBYTE pResultData, int nResultDataLen, BOOL bRightNow);
	BOOL RpcSendLock();
	BOOL RpcSendUnlock();
	int RpcRecvAnswer(LPCSTR receiver, LPCSTR msgname, PBYTE &msgdata, int &msgdatalen);
	int GetRpcState();
	void SetRpcState(int state);

	int  RpcSendMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen, DWORD ctrcode = 0);
	int  RpcSendMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen, DWORD ctrcode, DWORD frameno);

protected:
    std::map<const std::string, Probe*> mProbeMap;

    void onPipeReceiveData(int nErrCode);

private:
	GmbsReturnCntx_t m_RpcReturnCntx;
	HANDLE   m_hRpcSendMutex;

	BYTE    m_RpcMsgRxdBuf[RPCBUF_MAXLEN];
	int     m_nRpcFrameNo;


protected: //should be private
	CBindPipe m_RpcPipe;
	char    m_strPrefix[6];  //"Host" or "Embed"

	HANDLE m_hPlatformCntx;

	HWND      m_hMainWnd;

protected:
	BYTE m_RpcMsgTxdBuf[RPCBUF_MAXLEN];

public: //TBD
	int RpcSendData(PBYTE pData, int nDataLen);
	int RpcRecvData(PBYTE pDataBuf, int nBufLen);
	int RpcRecvDataEx(PBYTE pDataBuf, int nBufLen);

};

#define MSGRCVR(pMsgData) ((LPCSTR)(pMsgData + sizeof(RpcMsgHeader_t)))
#define MSGNAME(pMsgData) (MSGRCVR(pMsgData) + ((RpcMsgHeader_t*)pMsgData)->rcvrlen)
#define MSGDATA(pMsgData) ((PBYTE)(MSGNAME(pMsgData) + ((RpcMsgHeader_t*)pMsgData)->namelen))  

NS_COB_END

#endif
