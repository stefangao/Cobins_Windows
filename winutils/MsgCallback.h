#ifndef __MSG_CALLBACK_H__
#define __MSG_CALLBACK_H__

#include <windows.h>
#include <functional>

#define WM_POST_CALLBACK    WM_USER + 1001
#define WM_SEND_CALLBACK    WM_USER + 1002

class MsgCallback
{
private:
    HWND mhMainWnd;
    static WNDPROC mOldWndProc;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static std::function<void(void)> mPostCallback;
    std::function<void(void)> mSendCallback;

public:
    MsgCallback() {};
    MsgCallback(HWND hWnd);

public:
    void post(const std::function<void(void)>& callback);
    void send(const std::function<void(void)>& callback);

    void SetWndProc(HWND hWnd);
    void ResetWndProc();
};

#endif
