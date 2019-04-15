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

static void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
   switch(nTimerid)
   {
   case 1:
	   ::PostMessage(m_hMainWnd, WM_WAIT_TIMEOUT, 0, 0);
       break;
   default:
	   break;
   }
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

void MsgCallback::wait(UINT uTimeout, UINT uTargetMsg, CallbackFunc& callback);
{
	if (uTimeout != INFINITE)
	{
　　　　　　　　SetTimer(m_hMainWnd, 1, uTimeout, TimerProc);
	}

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
    	if (msg.message == uTargetMsg)
    	{
    		callback();
    		break;
    	}
    	else if (msg.message == WM_WAIT_TIMEOUT)
    	{
    		callback();
    		break;
    	}
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

