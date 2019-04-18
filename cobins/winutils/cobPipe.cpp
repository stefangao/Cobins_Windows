#include <stdio.h>
#include "cobPipe.h"
#include "wtermin.h"

NS_COB_BEGIN

Pipe::Pipe()
{
    m_hPipe = INVALID_HANDLE_VALUE;
    m_hEvent = CreateEvent(0, FALSE, FALSE, NULL);
    m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hListenEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_bConnected = FALSE;
    m_hListenThread = NULL;
    m_OnPipeReceiveDataProc = NULL;
}

Pipe::~Pipe()
{
    CloseHandle (m_hEvent);
    CloseHandle (m_hSendEvent);
    CloseHandle (m_hRecvEvent);
    CloseHandle (m_hListenEvent);
}

static DWORD WINAPI PipeClientListenProc(LPVOID lParam)
{
    Pipe* pPipe;
    pPipe = (Pipe*)lParam;

    SetEvent(pPipe->m_hEvent);  //make parent thread continue
    pPipe->Listen();

    WT_Trace("PipeClientListenProc: Exit\n");

    return 0;
}

static DWORD WINAPI PipeServerListenProc(LPVOID lParam)
{
    Pipe* pPipe = (Pipe*)lParam;

    SetEvent (pPipe->m_hEvent);  //make parent thread continue

    while (pPipe->ConnectPipe())
    {
        pPipe->Listen();
        WT_Trace("PipeServerListenProc listen end");
    }

    WT_Trace("PipeServerListenProc: Exit\n");

    return 0;
}

int Pipe::Send(BYTE *pBuf, int nSize)
{
    BOOL bSuccess;
    OVERLAPPED ol;
    DWORD dwWritten = -1;

    if (m_hPipe == INVALID_HANDLE_VALUE || !m_bConnected)
    {
        WT_Error("Pipe::Send: ��Ч�����δ����: hPipe=%x\n", m_hPipe);
        OnReceive(-1);
        return -1;
    }

    if (OVERLAPPED_IO)
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = m_hSendEvent;
    }

    bSuccess = WriteFile(m_hPipe, pBuf, nSize, &dwWritten,
            OVERLAPPED_IO ? &ol : NULL);
    if (!bSuccess)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            //����3��
            //WT_Trace("pipe[%x] write: wait 3000 ...\n", this);
            DWORD dwResult = WaitForSingleObject(ol.hEvent, 5000);
            if (dwResult != WAIT_OBJECT_0)
            {
                WT_Error("pipe write: wait error\n");
                BOOL bCancel = CancelIo(m_hPipe); //remove it to avoid losing frame
                //ClosePipe();
                //OnReceive(-1);
                return 0;
            }
            //WT_Trace("pipe[%x] write: wait ok\n", this);

            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwWritten, FALSE);
            if (!bSuccess && !PipeCheck())
            {
                WT_Error("Pipe::Send: error\n");
                dwWritten = -1;
                OnReceive(-1);
            }

            //WT_Trace("pipe[%x] write: finished\n", this);
        }
        else
        {
            WT_Trace("PIPE: send error\n");
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
        WT_Error("Pipe::Receive: hPipe=%x\n", m_hPipe);
        OnReceive(-1);
        return -1;
    }

    if (OVERLAPPED_IO)
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = m_hRecvEvent;
    }

    bSuccess = ReadFile(m_hPipe, pBuf, nBufLen, &dwReadLen,
            OVERLAPPED_IO ? &ol : NULL);
    if (!bSuccess)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            //WT_Trace("pipe[%x] read: wait 3000 ...\n", this);
            DWORD dwResult = WaitForSingleObject(ol.hEvent, 5000);
            if (dwResult != WAIT_OBJECT_0)
            {
                BOOL bCancel = CancelIo(m_hPipe);
                WT_Error("Pipe::Recv: delayed: bCancel=%d\n", bCancel);
                //ClosePipe();
                //OnReceive(-1);
                //return 0;
            }
            //WT_Trace("pipe[%x] read: wait ok\n", this);

            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwReadLen, FALSE);
            if (!bSuccess && !PipeCheck())
            {
                WT_Error("Pipe::Send: error\n");
                dwReadLen = -1;
                OnReceive(-1);
            }
        }
    }

    SetEvent (m_hEvent);

    return dwReadLen;
}

BOOL Pipe::PipeCheck()
{
    DWORD dwLastError = GetLastError();
    WT_Trace("PipeCheck��lastErr=%d", dwLastError);

    if (dwLastError == ERROR_BROKEN_PIPE || dwLastError == ERROR_NO_DATA
            || dwLastError == ERROR_PIPE_NOT_CONNECTED)
    {
        BOOL bResult = DisconnectNamedPipe(m_hPipe);
        CloseHandle (m_hPipe);
        m_bConnected = FALSE;
        m_hPipe = INVALID_HANDLE_VALUE;

        WT_Error(
                "=============================PipeCheck: lasterr=%d, this=%p\n",
                dwLastError, this);
        return FALSE;
    }

    return TRUE;
}

BOOL Pipe::ConnectPipe()
{
    BOOL bSuccess;
    OVERLAPPED ol;
    DWORD dwRead;
    BOOL bResult = FALSE;
    DWORD lasterr = 0;

    WT_Trace("ConnectPipe E\n");

    if (m_hPipe == INVALID_HANDLE_VALUE)
        return FALSE;

    if (OVERLAPPED_IO)
    {
        memset(&ol, 0, sizeof(ol));
        ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
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
            if (bSuccess)
            {
                m_bConnected = TRUE;
                bResult = TRUE;
            }
            else
            {
                WT_Trace("Pipe::ConnectPipe()111: dwLastErr=%d\n",
                        GetLastError());
            }
        }
        else if (dwLastErr == ERROR_PIPE_CONNECTED)
        {
            m_bConnected = TRUE;
            bResult = TRUE;
        }
        else
        {
            WT_Trace("Pipe::ConnectPipe()222: dwLastErr=%d\n", dwLastErr);
        }
    }

    if (OVERLAPPED_IO)
    {
        CloseHandle(ol.hEvent);
    }
    WT_Trace("ConnectPipe End: ret=%d\n", bResult);
    return bResult;
}

BOOL Pipe::Listen()
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
        bSuccess = ReadFile(m_hPipe, NULL, 0, &dwReadLen,
                OVERLAPPED_IO ? &ol : NULL);
        if (bSuccess && GetDataSize() > 0)
        {
            OnReceive(0);
            WaitForSingleObject(m_hEvent, 10000);
        }
        else if (GetLastError() == ERROR_IO_PENDING)
        {
            bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwReadLen, TRUE);
            WT_Trace("GetOverlappedResult: bSucc=%d, len=%d [%x](%x)\n",
                    bSuccess, GetDataSize(), GetCurrentProcessId(),
                    GetCurrentThreadId());
            if (bSuccess && GetDataSize() > 0)
            {
                OnReceive(0);
                WaitForSingleObject(m_hEvent, 10000);
            }
        }

        if (dwReadLen > 0)
        {
            WT_Error("Pipe::Listen: fatal error: dwReadLen > 0\n");
        }

        if (!bSuccess && !PipeCheck())
        {
            WT_Error("Pipe::Listen: Pipe check error\n");
            OnReceive(-1);
            break;
        }
    }

    WT_Trace("Listen end [%x]\n", GetCurrentProcessId());

    return -1;
}

int Pipe::GetDataSize()
{
	DWORD dwSizeLow, dwSizeHigh;
	dwSizeLow = GetFileSize(m_hPipe, &dwSizeHigh);
	return dwSizeLow;
}

BOOL Pipe::CreatePipe(const char* pipename)
{
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle (m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }

    m_hPipe = CreateNamedPipe(pipename,
            PIPE_ACCESS_DUPLEX | (OVERLAPPED_IO ? FILE_FLAG_OVERLAPPED : 0),
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 0, 0, 60000, NULL);

    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        WT_Error("Pipe::CreatePipe: lasterr=%d\n", GetLastError());
        return FALSE;
    }

    m_hListenThread = CreateThread(0, 0, PipeServerListenProc, this, 0, 0);
    WT_Trace("Pipe::CreatePipe: listen thread=%x\n", m_hListenThread);
    WaitForSingleObject(m_hEvent, 2000);    //wait child thread running

    return TRUE;
}

BOOL Pipe::OpenPipe(const char* pipename)
{
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle (m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }

    m_hPipe = ::CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
            OPEN_EXISTING, OVERLAPPED_IO ? FILE_FLAG_OVERLAPPED : 0, NULL);

    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        WT_Error("Pipe::CreatePipe: lasterr=%d\n", GetLastError());
        return FALSE;
    }

    m_bConnected = TRUE;
    m_hListenThread = CreateThread(0, 0, PipeClientListenProc, this, 0, 0);
    WaitForSingleObject(m_hEvent, 2000);        //wait child thread running

    return TRUE;
}

BOOL Pipe::CreatePipe(DWORD dwPortId)
{
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);

	//WT_Trace("CreatePipe: name=%s\n", pipename);

	return CreatePipe(pipename);
}

BOOL Pipe::OpenPipe(DWORD dwPortId)
{
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	//WT_Trace("OpenPipe: name=%s\n", pipename);

	return OpenPipe(pipename);
}

BOOL Pipe::BindPipe(DWORD dwPortId)
{
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	WT_Trace("BindPipe: name=%s\n", pipename);

	if (!WaitNamedPipe(pipename, NMPWAIT_USE_DEFAULT_WAIT)) {
		return CreatePipe(dwPortId);
	} else {
		return OpenPipe(dwPortId);
	}
}

#if 0
BOOL Pipe::DestroyPipe()
{
	WT_Trace("DestroyPipe111: m_hPipe=%x\n", m_hPipe);
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		OnClose(0);
		SetEvent (m_hListenEvent);
		WT_Trace("Pipe::DestroyPipe->111: listen thread=%x", m_hListenThread);
		//WaitForSingleObject(m_hListenThread, INFINITE);
		//TerminateThread(m_hListenThread, 0);
		WT_Trace("Pipe::DestroyPipe->222: listen thread=%x", m_hListenThread);

		m_bConnected = FALSE;
		DisconnectNamedPipe (m_hPipe);
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
		SetEvent (m_hEvent);

		WT_Trace("DestroyPipe: this=%p\n", this);
	}
	WT_Trace("DestroyPipe222: m_hPipe=%x\n", m_hPipe);

	return TRUE;
}
#endif

BOOL Pipe::ClosePipe()
{
    WT_Trace("ClosePipe: E\n");

    m_bConnected = FALSE;
    CloseHandle (m_hPipe);
    SetEvent (m_hListenEvent);
    SetEvent (m_hEvent);
    SetEvent (m_hRecvEvent);
    SetEvent (m_hSendEvent);
    WaitForSingleObject(m_hListenThread, INFINITE);

    WT_Trace("ClosePipe: WaitForSingleObject End proc=%x\n", GetCurrentProcessId());
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
			WT_Trace("[TEST] OnReceive: len=%d buf=%s\n", len, buf);
	}
}

NS_COB_END
