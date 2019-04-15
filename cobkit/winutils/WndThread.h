#ifndef __WND_THREAD_H__
#define __WND_THREAD_H__

#include <windows.h>

class WndThread
{
	typedef const std::function<void(void)> CallbackFunc;
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
