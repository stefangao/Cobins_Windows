#ifndef __MSG_CALLBACK_H__
#define __MSG_CALLBACK_H__

#include <windows.h>
#include <functional>

#define WM_POST_CALLBACK    WM_USER + 1001
#define WM_SEND_CALLBAＩCK  WM_USER + 1002
#define WM_WAIT_TIMEOUT     WM_USER + 1003

class MsgCallback
{
	typedef const std::function<void(void)> CallbackFunc;
private:
    HWND m_hMainWnd;
    WNDPROC m_OldWndProc;

public:
    MsgCallback() {};

public:
    void post(CallbackFunc& callback);
    void send(CallbackFunc& callback);
    void wait(UINT uTimeout, UINT uTargetMsg, CallbackFunc& callback);

    void SetWndProc(HWND hWnd);
    void ResetWndProc();
};

#endif
