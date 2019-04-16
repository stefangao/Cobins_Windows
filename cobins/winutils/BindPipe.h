#if !defined(BINDPIPE_H__)
#define BINDPIPE_H__

#include "Pipe.h"
#include <functional>

class CBindPipe : public CPipe
{

typedef struct
{
    int datalen;
    int keep;
    
}DataHeader_t;

private:
    HWND  m_hBindWnd;
    std::function<void(int)> m_OnPipeReceiveDataProc;

public:
    CBindPipe();

public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

    void BindWindow(HWND hWnd) {m_hBindWnd = hWnd;};
    BOOL RegisterRxd(std::function<void(int)> callback);

    long RecvMessage(LPCSTR receiver, LPCSTR msgname, PBYTE msgdata, int msgdatalen);
};

#endif
