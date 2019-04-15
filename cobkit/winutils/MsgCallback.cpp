#include "MsgCallback.h"
#include "wtermin.h"

void MsgCallback::SetWndProc(HWND hWnd)
{
    if (m_hMainWnd != NULL && hWnd)
    {
        m_hMainWnd = hWnd;
        m_OldWndProc = ::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);
    }
}

void MsgCallback::ResetWndProc()
{
    if (m_hMainWnd)
    {
        ::SetWindowLong(m_hMainWnd, GWL_WNDPROC, (LONG)m_OldWndProc);
        m_hMainWnd = NULL;
    }
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_POST_CALLBACK)
    {
        CallbackFunc* p = (CallbackFunc*)wParam;
        CallbackFunc& callback = *p;
        if (callback)
        {
            callback();
            delete p;
            return 0;
        }
    }
    return CallWindowProc(m_OldWndProc, hWnd, uMsg, wParam, lParam);
}

void MsgCallback::post(const std::function<void(void)>& callback)
{
    CallbackFunc **wParam = new (CallbackFunc*);
    ::PostMessage(m_hMainWnd, WM_POST_CALLBACK, (WPARAM)(*wParam), 0);
}

void MsgCallback::send(const std::function<void(void)>& callback)
{
    CallbackFunc **wParam = new (CallbackFunc*);
    ::SendMessage(m_hMainWnd, WM_POST_CALLBACK, (WPARAM)(*wParam), 0);
}
