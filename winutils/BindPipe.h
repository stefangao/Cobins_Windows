#if !defined(BINDPIPE_H__)
#define BINDPIPE_H__

#include "Pipe.h"

typedef void (*OnPipeReceiveProc_t)(void*, int);

class  __declspec(dllexport) CBindPipe : public CPipe
{

typedef struct
{
    int datalen;
    int keep;
    
}DataHeader_t;

private:
    HWND  m_hBindWnd;
    OnPipeReceiveProc_t m_OnPipeReceiveProc;
    void *m_UserData;

public:
    CBindPipe();

public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

    void BindWindow(HWND hWnd) {m_hBindWnd = hWnd;};
    BOOL RegisterRxd(OnPipeReceiveProc_t OnPipeReceiveProc, void* userdata);

    long RecvMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen);
};

#endif
