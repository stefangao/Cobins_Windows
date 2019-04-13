#include "stdlib.h"
#include "BindPipe.h"
#include "wtermin.h"

CBindPipe::CBindPipe()
{
    m_OnPipeReceiveDataProc = NULL;
}

BOOL CBindPipe::RegisterRxd(std::function<void(int)> callback)
{
    m_OnPipeReceiveDataProc = callback;
    return TRUE;
}

void CBindPipe::OnReceive(int nErrorCode)
{
    if (m_OnPipeReceiveDataProc != nullptr)
    {
        m_OnPipeReceiveDataProc(nErrorCode);
    }
    else //test
    {
        char buf[256];
        int len = Receive((BYTE*)buf, 256);
        if (len > 0)
            WT_Trace("[TEST] OnReceive: len=%d buf=%s\n", len, buf);
    }
}

void CBindPipe::OnClose(int nErrorCode) 
{
  
}

long CBindPipe::RecvMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen)
{

    return 0;
}
