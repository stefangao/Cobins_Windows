#ifndef __MSG_CALLBACK_H__
#define __MSG_CALLBACK_H__

#include <windows.h>
#include <functional>
#include <map>
#include "cobConst.h"

#define WM_POST_CALLBACK    WM_COBMSG_MIN + 101
#define WM_SEND_CALLBACK    WM_COBMSG_MIN + 102
#define WM_WAIT_TIMEOUT     WM_COBMSG_MIN + 103

typedef std::function<void(UINT)> CallbackFunc;

class MsgCallback
{
private:
    HWND m_hMainWnd;
    BOOL m_bNeedReset;

public:
    MsgCallback();
    virtual ~MsgCallback();

public:
    void post(const CallbackFunc& callback);
    void send(const CallbackFunc& callback);
    void wait(UINT uTimeout);
    bool waitMessage(UINT uTargetMsg, const CallbackFunc& callback, UINT uTimeout = INFINITE);

    void SetWndProc(HWND hWnd);
    void ResetWndProc();

    inline HWND getMainWnd() { return m_hMainWnd; }
};

#endif
