#ifndef __COB_PIPE__
#define __COB_PIPE__

#include <windows.h>
#include "cobMacros.h"

NS_COB_BEGIN

#define PIPENAME "\\\\.\\pipe\\GameBs_Ptfm_Pipe%x"       /* Default pipe name */
#define OVERLAPPED_IO  TRUE

class CPipe
{
	friend DWORD WINAPI PipeClientListenProc(LPVOID lParam);
	friend DWORD WINAPI PipeServerListenProc(LPVOID lParam);

public:
    HANDLE  m_hPipe;  //only for test


private:
	HANDLE  m_hEvent;
	BOOL    m_bConnected;
    HANDLE  m_hSendEvent;
    HANDLE  m_hRecvEvent;
    HANDLE  m_hListenEvent;
    HANDLE  m_hListenThread;

protected:
    BOOL Listen();
	BOOL ConnectPipe();
	BOOL PipeCheck();

public:
	CPipe();
   	virtual ~CPipe();

    BOOL IsPipe() {return (m_hPipe != INVALID_HANDLE_VALUE);};

// Attributes
public:
	BOOL CreatePipe(const char* pipename);
	BOOL OpenPipe(const char* pipename);

	BOOL CreatePipe(DWORD dwPortId);
	BOOL OpenPipe(DWORD dwPortId);
    BOOL BindPipe(DWORD dwPortId);
	
	int  Send(BYTE* pBuf,int nSize);
	int  Receive(BYTE *pBuf,int nBufLen);

    BOOL DestroyPipe();
    BOOL ClosePipe();

	BOOL IsConnected() {return m_bConnected;};

    int  GetDataSize( );

    BOOL RegisterRxd(const std::function<void(int)>& callback);


// Operations
protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode) {};
};

NS_COB_END

#endif
