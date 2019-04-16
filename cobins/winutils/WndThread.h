#ifndef __WND_THREAD_H__
#define __WND_THREAD_H__

#include <windows.h>
#include <functional>

class WndThread
{
    typedef const std::function<void(void)> CallbackFunc;
    friend static DWORD WINAPI ThreadProc(PVOID pArg);
public:
    WndThread();
    virtual ~WndThread();

    HWND create();
    HWND GetMainWnd() const {return m_hMainWnd;}

private:
    HWND m_hMainWnd;
	HANDLE m_hEvent;
};

#endif
