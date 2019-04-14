#include "MsgCallback.h"
#include "wtermin.h"

WNDPROC MsgCallback::mOldWndProc = NULL;
std::function<void(void)> MsgCallback::mPostCallback = nullptr;
MsgCallback::MsgCallback(HWND hWnd)
  : mhMainWnd(hWnd)
{
    mSendCallback = nullptr;
    ::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);
}

void MsgCallback::SetWndProc(HWND hWnd)
{
    mhMainWnd = hWnd;
    ::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);
}

void MsgCallback::ResetWndProc()
{
    if (mhMainWnd)
    {
        ::SetWindowLong(mhMainWnd, GWL_WNDPROC, (LONG)mOldWndProc);
    }
}

LRESULT CALLBACK MsgCallback::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //WT_Trace(" MsgCallback::WindowProc\n");
    if (uMsg == WM_POST_CALLBACK)
    {
        if (mPostCallback)
        {
            mPostCallback();
            return 0;
        }
    }
    return CallWindowProc(mOldWndProc, hWnd, uMsg, wParam, lParam);
}

void MsgCallback::post(const std::function<void(void)>& callback)
{
    mPostCallback = callback;
    ::PostMessage(mhMainWnd, WM_POST_CALLBACK, 0, 0);
}

void MsgCallback::send(const std::function<void(void)>& callback)
{
    mSendCallback = callback;

}