#include <stdio.h>
#include "cobPipe.h"
#include "wtermin.h"
#include "base/cobUtils.h"

NS_COB_BEGIN

Pipe::Pipe()
{
    m_hPipe = INVALID_HANDLE_VALUE;
    m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hListenEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_bConnected = FALSE;
    m_hListenThread = NULL;
    m_OnPipeReceiveDataProc = NULL;
}

Pipe::~Pipe()
{
    CloseHandle(m_hEvent);
    CloseHandle(m_hSendEvent);
    CloseHandle(m_hRecvEvent);
    CloseHandle(m_hListenEvent);
    CloseHandle(m_hAcceptEvent);
}

static DWORD WINAPI PipeClientListenProc(LPVOID lParam)
{
    Pipe* pPipe;
    pPipe = (Pipe*)lParam;

    SetEvent(pPipe->m_hEvent);  //notify parent thread continue
    pPipe->ListenData();

    COBLOG("PipeClientListenProc: Exit\n");

    return 0;
}

static DWORD WINAPI PipeServerListenProc(LPVOID lParam)
{
    Pipe* pPipe = (Pipe*)lParam;

    SetEvent (pPipe->m_hEvent);  //notify parent thread continue
    while (pPipe->AcceptConnect())
    {
        pPipe->ListenData();
    }

    COBLOG("PipeServerListenProc: Exit\n");
    return 0;
}

int Pipe::Send(BYTE *pBuf, int nSize)
{
    BOOL bSuccess;
    OVERLAPPED ol;
    DWORD dwWritten = -1;

    if (m_hPipe == INVALID_HANDLE_VALUE || !m_bConnected)
    {
        COBLOG("Pipe::Send: Error hPipe=%x\n", m_hPipe);
        OnReceive(-1);
        return -1;
    }

    if (OVERLAPPED_IO)
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = m_hSendEvent;
    }

    bSuccess = WriteFile(m_hPipe, pBuf, nSize, &dwWritten, OVERLAPPED_IO ? &ol : NULL);
    if (!bSuccess)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            //COBLOG("pipe[%x] write: wait 3000 ...\n", this);
            DWORD dwResult = WaitForSingleObject(ol.hEvent, INFINITE);
            if (dwResult != WAIT_OBJECT_0)
            {
                COBLOG("pipe write: wait error\n");
                BOOL bCancel = CancelIo(m_hPipe); //remove it to avoid losing frame
                return 0;
            }
            //COBLOG("pipe[%x] write: wait ok\n", this);

            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwWritten, TRUE);
            if (!bSuccess && !PipeCheck())
            {
                COBLOG("Pipe::Send: error\n");
                dwWritten = -1;
                OnReceive(-1);
            }

            //COBLOG("pipe[%x] write: finished\n", this);
        }
        else
        {
            COBLOG("PIPE: send error\n");
        }
    }

    return dwWritten;
}

int Pipe::Receive(BYTE *pBuf, int nBufLen)
{
    BOOL bSuccess;
    OVERLAPPED ol;
    DWORD dwReadLen = -1;

    if (m_hPipe == INVALID_HANDLE_VALUE || !m_bConnected)
    {
        COBLOG("Pipe::Receive: hPipe=%x\n", m_hPipe);
        OnReceive(-1);
        return -1;
    }

    if (OVERLAPPED_IO)
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = m_hRecvEvent;
    }

    bSuccess = ReadFile(m_hPipe, pBuf, nBufLen, &dwReadLen, OVERLAPPED_IO ? &ol : NULL);
    if (!bSuccess)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwReadLen, FALSE);
            if (!bSuccess && !PipeCheck())
            {
                COBLOG("Pipe::Send: error\n");
                dwReadLen = -1;
                OnReceive(-1);
            }
        }
    }

    return dwReadLen;
}

void Pipe::ReceiveEnd()
{
    SetEvent(m_hEvent);
}

BOOL Pipe::PipeCheck()
{
    BOOL ret = TRUE;
    DWORD dwLastError = GetLastError();
    COBLOG("PipeCheck: lasterr=%d, this=%p\n", dwLastError, this);
    if (dwLastError == ERROR_BROKEN_PIPE)
    {
        if (m_hPipe != INVALID_HANDLE_VALUE)
        {
            DisconnectNamedPipe(m_hPipe);
            m_bConnected = FALSE;
            m_hPipe = INVALID_HANDLE_VALUE;
        }
        ret = FALSE;
    }
    else if (dwLastError == ERROR_NO_DATA || dwLastError == ERROR_PIPE_NOT_CONNECTED)
    {
        if (m_hPipe != INVALID_HANDLE_VALUE)
        {
            DisconnectNamedPipe(m_hPipe);
            CloseHandle(m_hPipe);
            m_bConnected = FALSE;
            m_hPipe = INVALID_HANDLE_VALUE;
        }
        ret = FALSE;
    }
    return ret;
}

BOOL Pipe::AcceptConnect()
{
    BOOL bSuccess;
    OVERLAPPED ol;
    DWORD dwRead;
    BOOL bResult = FALSE;
    DWORD lasterr = 0;

    COBLOG("AcceptConnect E\n");

    if (m_hPipe == INVALID_HANDLE_VALUE)
        return FALSE;

    if (OVERLAPPED_IO)
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = m_hAcceptEvent;
    }

    bSuccess = ConnectNamedPipe(m_hPipe, OVERLAPPED_IO ? &ol : NULL);
    if (bSuccess)
    {
        m_bConnected = TRUE;
        bResult = TRUE;
    }
    else
    {
        DWORD dwLastErr = GetLastError();
        if (dwLastErr == ERROR_IO_PENDING)
        {
            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwRead, TRUE);
            COBLOG("Pipe::AcceptConnect(): bSuccess=%d\n", bSuccess);
            if (bSuccess)
            {
                m_bConnected = TRUE;
                bResult = TRUE;
            }
        }
        else if (dwLastErr == ERROR_PIPE_CONNECTED)
        {
            m_bConnected = TRUE;
            bResult = TRUE;
        }
        else
        {
            COBLOG("Pipe::AcceptConnect(): dwLastErr=%d\n", dwLastErr);
        }
    }

    COBLOG("AcceptConnect X: ret=%d\n", bResult);
    return bResult;
}

BOOL Pipe::ListenData()
{
    BOOL bSuccess;
    DWORD dwReadLen;
    OVERLAPPED ol;

    if (OVERLAPPED_IO) /*if overlapped, prepare OVERLAPPED structure */
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = m_hListenEvent;
    }

    while (m_bConnected)
    {
        bSuccess = ReadFile(m_hPipe, NULL, 0, &dwReadLen, OVERLAPPED_IO ? &ol : NULL);
        //COBLOG("ReadFile: [%x](%x) succ=%d\n", GetCurrentProcessId(), GetCurrentThreadId(), bSuccess);
        if (bSuccess && GetDataSize() > 0)
        {
            OnReceive(0);
            WaitForSingleObject(m_hEvent, 10000);
        }
        else if (GetLastError() == ERROR_IO_PENDING)
        {
            //COBLOG("GetOverlappedResult E: [%x](%x)\n", GetCurrentProcessId(), GetCurrentThreadId());
            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwReadLen, TRUE);
            //COBLOG("GetOverlappedResult X: bSucc=%d, len=%d [%x](%x)\n",
            //        bSuccess, GetDataSize(), GetCurrentProcessId(), GetCurrentThreadId());
            if (bSuccess && GetDataSize() > 0)
            {
                OnReceive(0);
                WaitForSingleObject(m_hEvent, 10000);
            }
        }

        if (dwReadLen > 0)
        {
            COBLOG("Pipe::ListenData: fatal error: dwReadLen > 0\n");
        }

        if (!bSuccess && !PipeCheck())
        {
            COBLOG("Pipe::ListenData: Pipe check error\n");
            OnReceive(-1);
            break;
        }
    }

    COBLOG("ListenData end [%x]\n", GetCurrentProcessId());
    return -1;
}

int Pipe::GetDataSize()
{
	DWORD dwSizeLow, dwSizeHigh;
	dwSizeLow = GetFileSize(m_hPipe, &dwSizeHigh);
	return dwSizeLow;
}

BOOL Pipe::CreatePipe(const char* pipeName)
{
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle (m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }

    m_hPipe = CreateNamedPipe(pipeName,
            PIPE_ACCESS_DUPLEX | (OVERLAPPED_IO ? FILE_FLAG_OVERLAPPED : 0),
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 1024, 1024, 5000, NULL);
    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        COBLOG("Pipe::CreatePipe: lasterr=%d\n", GetLastError());
        return FALSE;
    }

    COBLOG("Pipe: create ServerListenThread E\n");
    m_hListenThread = CreateThread(0, 0, PipeServerListenProc, this, 0, 0);
    WaitForSingleObject(m_hEvent, 2000);    //wait child thread running
    COBLOG("Pipe: create ServerListenThread X: listen thread=%x\n", m_hListenThread);

    return TRUE;
}

BOOL Pipe::OpenPipe(const char* pipeName)
{
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle (m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }

    m_hPipe = ::CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    OPEN_EXISTING, OVERLAPPED_IO ? FILE_FLAG_OVERLAPPED : 0, NULL);
    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        COBLOG("Pipe::OpenPipe: lasterr=%d\n", GetLastError());
        return FALSE;
    }

    COBLOG("Pipe: create ClientListenThread E\n");
    m_bConnected = TRUE;
    m_hListenThread = CreateThread(0, 0, PipeClientListenProc, this, 0, 0);
    WaitForSingleObject(m_hEvent, 2000);        //wait child thread running
    COBLOG("Pipe: create ClientListenThread X: listen thread=%x\n", m_hListenThread);

    return TRUE;
}

BOOL Pipe::CreatePipe(DWORD dwPortId)
{
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	COBLOG("CreatePipe: name=%s\n", pipename);

	return CreatePipe(pipename);
}

BOOL Pipe::OpenPipe(DWORD dwPortId)
{
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	COBLOG("OpenPipe: name=%s\n", pipename);

	return OpenPipe(pipename);
}

BOOL Pipe::BindPipe(DWORD dwPortId)
{
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	COBLOG("BindPipe: name=%s\n", pipename);

	if (!WaitNamedPipe(pipename, NMPWAIT_USE_DEFAULT_WAIT)) {
		return CreatePipe(dwPortId);
	} else {
		return OpenPipe(dwPortId);
	}
}

BOOL Pipe::ClosePipe()
{
    COBLOG("ClosePipe: E\n");

    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        m_bConnected = FALSE;
        CloseHandle(m_hPipe);
        SetEvent(m_hListenEvent);
        SetEvent(m_hEvent);
        SetEvent(m_hRecvEvent);
        SetEvent(m_hSendEvent);
        SetEvent(m_hAcceptEvent);
        WaitForSingleObject(m_hListenThread, INFINITE);
        m_hPipe = INVALID_HANDLE_VALUE;
    }

    COBLOG("ClosePipe: WaitForSingleObject End proc=%x\n", GetCurrentProcessId());
	return TRUE;
}

BOOL Pipe::RegisterRxd(const std::function<void(int)>& callback)
{
	m_OnPipeReceiveDataProc = callback;
	return TRUE;
}

void Pipe::OnReceive(int nErrorCode)
{
	if (m_OnPipeReceiveDataProc != nullptr)
	{
		m_OnPipeReceiveDataProc(nErrorCode);
	}
	else //test
	{
		char buf[256];
		int len = Receive((BYTE*) buf, 256);
		if (len > 0)
			COBLOG("[TEST] OnReceive: len=%d buf=%s\n", len, buf);
	}
}

NS_COB_END
