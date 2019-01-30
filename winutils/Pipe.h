#if !defined(PIPE_H__)
#define PIPE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>

#define PIPENAME "\\\\.\\pipe\\GameBs_Ptfm_Pipe%x"       /* Default pipe name */
#define OVERLAPPED_IO  TRUE

class  __declspec(dllexport) CPipe
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


// Operations
protected:
	virtual void OnReceive(int nErrorCode){};
	virtual void OnClose(int nErrorCode) {};
};

#endif
