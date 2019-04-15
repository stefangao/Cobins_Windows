#ifndef __MSG_CALLBACK_H__
#define __MSG_CALLBACK_H__

#include <windows.h>
#include <functional>
#include <map>

#define WM_POST_CALLBACK    WM_USER + 1001
#define WM_SEND_CALLBACK    WM_USER + 1002
#define WM_WAIT_TIMEOUT     WM_USER + 1003

typedef const std::function<void(void)> CallbackFunc;

class MsgCallback
{
private:
    HWND m_hMainWnd;

public:
    MsgCallback() {};

public:
    void post(CallbackFunc& callback);
    void send(CallbackFunc& callback);
    void wait(UINT uTimeout, UINT uTargetMsg, const CallbackFunc& callback);

    void SetWndProc(HWND hWnd);
    void ResetWndProc();
};

#endif
