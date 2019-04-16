#include "stdio.h"
#include "Pipe.h"
#include "wtermin.h"

NS_COB_BEGIN

CPipe::CPipe() {
	m_hPipe = INVALID_HANDLE_VALUE;
	m_hEvent = CreateEvent(0, FALSE, FALSE, NULL);
	m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hListenEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bConnected = FALSE;
	m_hListenThread = NULL;
	m_OnPipeReceiveDataProc = NULL;
}

CPipe::~CPipe() {
	CloseHandle (m_hEvent);
	CloseHandle (m_hSendEvent);
	CloseHandle (m_hRecvEvent);
	CloseHandle (m_hListenEvent);
}

static DWORD WINAPI PipeClientListenProc(LPVOID lParam)
{
	CPipe* pPipe;
	pPipe = (CPipe*)lParam;

	SetEvent(pPipe->m_hEvent);  //make parent thread continue
	pPipe->Listen();

	WT_Trace("PipeClientListenProc: Exit\n");

	return 0;
}

static DWORD WINAPI PipeServerListenProc(LPVOID lParam)
{
	CPipe* pPipe = (CPipe*)lParam;

	SetEvent (pPipe->m_hEvent);  //make parent thread continue

	while (pPipe->ConnectPipe())//�ȴ�client����
	{
		pPipe->Listen();            //�ȴ����ݵ���
		WT_Trace("PipeServerListenProc listen end");
	}

	WT_Trace("PipeServerListenProc: Exit\n");

	return 0;
}

int CPipe::Send(BYTE *pBuf, int nSize) {
	BOOL bSuccess;
	OVERLAPPED ol;
	DWORD dwWritten = -1;

	if (m_hPipe == INVALID_HANDLE_VALUE || !m_bConnected) {
		WT_Error("CPipe::Send: ��Ч�����δ����: hPipe=%x\n", m_hPipe);
		OnReceive(-1);
		return -1;
	}

	if (OVERLAPPED_IO) {
		memset(&ol, 0, sizeof(ol));
		ol.hEvent = m_hSendEvent;
	}

	bSuccess = WriteFile(m_hPipe, pBuf, nSize, &dwWritten,
			OVERLAPPED_IO ? &ol : NULL);
	if (!bSuccess) {
		if (GetLastError() == ERROR_IO_PENDING) {
			//����3��
			//WT_Trace("pipe[%x] write: wait 3000 ...\n", this);
			DWORD dwResult = WaitForSingleObject(ol.hEvent, 5000);
			if (dwResult != WAIT_OBJECT_0) {
				WT_Error("pipe write: wait error\n");
				BOOL bCancel = CancelIo(m_hPipe); //remove it to avoid losing frame
				//ClosePipe();
				//OnReceive(-1);
				return 0;
			}
			//WT_Trace("pipe[%x] write: wait ok\n", this);

			bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwWritten, FALSE);
			if (!bSuccess && !PipeCheck()) {
				WT_Error("CPipe::Send: error\n");
				dwWritten = -1;
				OnReceive(-1);
			}

			//WT_Trace("pipe[%x] write: finished\n", this);
		} else {
			WT_Trace("PIPE: send error\n");
		}
	}

	return dwWritten;
}

int CPipe::Receive(BYTE *pBuf, int nBufLen) {
	BOOL bSuccess;
	OVERLAPPED ol;
	DWORD dwReadLen = -1;

	if (m_hPipe == INVALID_HANDLE_VALUE || !m_bConnected) {
		WT_Error("CPipe::Receive: ��Ч�����δ����: hPipe=%x\n", m_hPipe);
		OnReceive(-1);
		return -1;
	}

	if (OVERLAPPED_IO) {
		memset(&ol, 0, sizeof(ol));
		ol.hEvent = m_hRecvEvent;
	}

	bSuccess = ReadFile(m_hPipe, pBuf, nBufLen, &dwReadLen,
			OVERLAPPED_IO ? &ol : NULL);
	if (!bSuccess) {
		if (GetLastError() == ERROR_IO_PENDING) {
			//����3��
			//WT_Trace("pipe[%x] read: wait 3000 ...\n", this);
			DWORD dwResult = WaitForSingleObject(ol.hEvent, 5000);
			if (dwResult != WAIT_OBJECT_0) {
				BOOL bCancel = CancelIo(m_hPipe);
				WT_Error("CPipe::Recv: delayed: bCancel=%d\n", bCancel);
				//ClosePipe();
				//OnReceive(-1);
				//return 0;
			}
			//WT_Trace("pipe[%x] read: wait ok\n", this);

			bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwReadLen, FALSE);
			if (!bSuccess && !PipeCheck()) {
				WT_Error("CPipe::Send: error\n");
				dwReadLen = -1;
				OnReceive(-1);
			}
		}
	}

	SetEvent (m_hEvent);

	return dwReadLen;
}

BOOL CPipe::PipeCheck() {
	DWORD dwLastError = GetLastError();
	WT_Trace("PipeCheck��lastErr=%d", dwLastError);

	if (dwLastError == ERROR_BROKEN_PIPE || dwLastError == ERROR_NO_DATA
			|| dwLastError == ERROR_PIPE_NOT_CONNECTED) {
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

BOOL CPipe::ConnectPipe() {
	BOOL bSuccess;
	OVERLAPPED ol;
	DWORD dwRead;
	BOOL bResult = FALSE;
	DWORD lasterr = 0;

	WT_Trace("ConnectPipe��E\n");

	if (m_hPipe == INVALID_HANDLE_VALUE)
		return FALSE;

	if (OVERLAPPED_IO) {
		memset(&ol, 0, sizeof(ol));
		ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	bSuccess = ConnectNamedPipe(m_hPipe, OVERLAPPED_IO ? &ol : NULL);
	if (bSuccess) {
		m_bConnected = TRUE;
		bResult = TRUE;
	} else {
		DWORD dwLastErr = GetLastError();
		if (dwLastErr == ERROR_IO_PENDING) {
			bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwRead, TRUE);
			if (bSuccess) {
				m_bConnected = TRUE;
				bResult = TRUE;
			} else {
				WT_Trace("CPipe::ConnectPipe()111: dwLastErr=%d\n",
						GetLastError());
			}
		} else if (dwLastErr == ERROR_PIPE_CONNECTED) {
			m_bConnected = TRUE;
			bResult = TRUE;
		} else {
			WT_Trace("CPipe::ConnectPipe()222: dwLastErr=%d\n", dwLastErr);
		}
	}

	if (OVERLAPPED_IO) {
		CloseHandle(ol.hEvent);
	}
	WT_Trace("ConnectPipe��X ret=%d\n", bResult);
	return bResult;
}

BOOL CPipe::Listen() {
	BOOL bSuccess;
	DWORD dwReadLen;
	OVERLAPPED ol;

	if (OVERLAPPED_IO) /*if overlapped, prepare OVERLAPPED structure */
	{
		memset(&ol, 0, sizeof(ol));
		ol.hEvent = m_hListenEvent;
	}

	while (m_bConnected) {
		bSuccess = ReadFile(m_hPipe, NULL, 0, &dwReadLen,
				OVERLAPPED_IO ? &ol : NULL);
		if (bSuccess && GetDataSize() > 0) {
			OnReceive(0);
			WaitForSingleObject(m_hEvent, 10000);
		} else if (GetLastError() == ERROR_IO_PENDING) {
			bSuccess = GetOverlappedResult(m_hPipe, &ol, &dwReadLen, TRUE);
			WT_Trace("GetOverlappedResult��bSucc=%d, len=%d [%x](%x)\n",
					bSuccess, GetDataSize(), GetCurrentProcessId(),
					GetCurrentThreadId());
			if (bSuccess && GetDataSize() > 0) {
				OnReceive(0);
				WaitForSingleObject(m_hEvent, 10000);
			}
		}

		if (dwReadLen > 0) {
			WT_Error("CPipe::Listen: fatal error: dwReadLen > 0\n");
		}

		if (!bSuccess && !PipeCheck()) {
			WT_Error("CPipe::Listen: Pipe check error\n");
			OnReceive(-1);
			break;
		}
	}

	WT_Trace("Listen end [%x]\n", GetCurrentProcessId());

	return -1;
}

int CPipe::GetDataSize() {
	DWORD dwSizeLow, dwSizeHigh;
	dwSizeLow = GetFileSize(m_hPipe, &dwSizeHigh);
	return dwSizeLow;
}

BOOL CPipe::CreatePipe(const char* pipename) {
	if (m_hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle (m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}

	m_hPipe = CreateNamedPipe(pipename,
			PIPE_ACCESS_DUPLEX | (OVERLAPPED_IO ? FILE_FLAG_OVERLAPPED : 0),
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES, 0, 0, 60000, NULL);

	if (m_hPipe == INVALID_HANDLE_VALUE) {
		WT_Error("CPipe::CreatePipe: lasterr=%d\n", GetLastError());
		return FALSE;
	}

	m_hListenThread = CreateThread(0, 0, PipeServerListenProc, this, 0, 0);
	WT_Trace("CPipe::CreatePipe: listen thread=%x\n", m_hListenThread);
	WaitForSingleObject(m_hEvent, 2000);    //wait child thread running

	return TRUE;
}

BOOL CPipe::OpenPipe(const char* pipename) {
	if (m_hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle (m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}

	m_hPipe = ::CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, OVERLAPPED_IO ? FILE_FLAG_OVERLAPPED : 0, NULL);

	if (m_hPipe == INVALID_HANDLE_VALUE) {
		WT_Error("CPipe::CreatePipe: lasterr=%d\n", GetLastError());
		return FALSE;
	}

	m_bConnected = TRUE;
	m_hListenThread = CreateThread(0, 0, PipeClientListenProc, this, 0, 0);
	WaitForSingleObject(m_hEvent, 2000);        //wait child thread running

	return TRUE;
}

BOOL CPipe::CreatePipe(DWORD dwPortId) {
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);

	//WT_Trace("CreatePipe: name=%s\n", pipename);

	return CreatePipe(pipename);
}

BOOL CPipe::OpenPipe(DWORD dwPortId) {
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	//WT_Trace("OpenPipe: name=%s\n", pipename);

	return OpenPipe(pipename);
}

BOOL CPipe::BindPipe(DWORD dwPortId) {
	char pipename[256];

	sprintf_s(pipename, PIPENAME, dwPortId);
	WT_Trace("BindPipe: name=%s\n", pipename);

	if (!WaitNamedPipe(pipename, NMPWAIT_USE_DEFAULT_WAIT)) {
		return CreatePipe(dwPortId);
	} else {
		return OpenPipe(dwPortId);
	}
}

BOOL CPipe::DestroyPipe() {
	WT_Trace("DestroyPipe111: m_hPipe=%x\n", m_hPipe);
	if (m_hPipe != INVALID_HANDLE_VALUE) {
		OnClose(0);
		SetEvent (m_hListenEvent);
		WT_Trace("CPipe::DestroyPipe->111: listen thread=%x", m_hListenThread);
		//WaitForSingleObject(m_hListenThread, INFINITE);
		//TerminateThread(m_hListenThread, 0);
		WT_Trace("CPipe::DestroyPipe->222: listen thread=%x", m_hListenThread);

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

BOOL CPipe::ClosePipe() {
	WT_Trace("SetEvent listen: E\n");
	m_bConnected = FALSE;
	CloseHandle (m_hPipe);
	SetEvent (m_hListenEvent);
	WT_Trace("SetEvent listen: X\n");
	SetEvent (m_hEvent);
	WaitForSingleObject(m_hListenThread, INFINITE);
	WT_Trace("WaitForSingleObject End proc=%x\n", GetCurrentProcessId());
	/*
	 if (m_hPipe != INVALID_HANDLE_VALUE)
	 {
	 OnClose(0);

	 CloseHandle(m_hPipe);
	 m_hPipe = INVALID_HANDLE_VALUE;
	 m_bConnected = FALSE;
	 SetEvent(m_hEvent);

	 WT_Trace("ClosePipe\n");
	 }*/

	return TRUE;
}

BOOL CPipe::RegisterRxd(std::function<void(int)> callback) {
	m_OnPipeReceiveDataProc = callback;
	return TRUE;
}

void CPipe::OnReceive(int nErrorCode) {
	if (m_OnPipeReceiveDataProc != nullptr) {
		m_OnPipeReceiveDataProc(nErrorCode);
	} else //test
	{
		char buf[256];
		int len = Receive((BYTE*) buf, 256);
		if (len > 0)
			WT_Trace("[TEST] OnReceive: len=%d buf=%s\n", len, buf);
	}
}

NS_COB_END
