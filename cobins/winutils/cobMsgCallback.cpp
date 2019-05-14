#include "cobMsgCallback.h"
#include <assert.h>
#include "wtermin.h"

struct CallbackWrapper{ CallbackFunc callback; };
static std::map<HWND, WNDPROC> m_WndMap;

MsgCallback::MsgCallback()
{
    m_hMainWnd = NULL;
    m_bNeedReset = FALSE;
}

MsgCallback::~MsgCallback()
{
    if (m_bNeedReset)
        ResetWndProc();
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_POST_CALLBACK || uMsg == WM_SEND_CALLBACK)
    {
        CallbackWrapper *wrapper = (CallbackWrapper*)wParam;
        if (wrapper)
        {
            if (wrapper->callback)
                wrapper->callback();

            delete wrapper;
            return 0;
        }
    }

    auto iter = m_WndMap.find(hWnd);
    assert(iter != m_WndMap.end());

    return CallWindowProc(iter->second, hWnd, uMsg, wParam, lParam);
}

void MsgCallback::SetWndProc(HWND hWnd)
{
    auto iter = m_WndMap.find(hWnd);
    if (iter != m_WndMap.end())
    {
        m_hMainWnd = hWnd;
        return;
    }

    if (m_hMainWnd == NULL && hWnd)
    {
        m_hMainWnd = hWnd;
        m_bNeedReset = TRUE;
        WNDPROC oldWndProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);
        m_WndMap.insert(std::make_pair(hWnd, oldWndProc));
    }
}

void MsgCallback::ResetWndProc()
{
    if (m_hMainWnd)
    {
        auto iter = m_WndMap.find(m_hMainWnd);
        assert(iter != m_WndMap.end());

        ::SetWindowLong(m_hMainWnd, GWL_WNDPROC, (LONG)iter->second);
        m_hMainWnd = NULL;
		m_WndMap.erase(iter);
    }
}

static void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
   switch(nTimerid)
   {
   case 1:
	   ::PostMessage(hWnd, WM_WAIT_TIMEOUT, 0, 0);
       KillTimer(hWnd, 1);
       break;
   default:
	   break;
   }
}

void MsgCallback::post(const CallbackFunc& callback)
{
    CallbackWrapper *wrapper = new CallbackWrapper();
    wrapper->callback = callback;
    ::PostMessage(m_hMainWnd, WM_POST_CALLBACK, (WPARAM)wrapper, 0);
}
void MsgCallback::send(const CallbackFunc& callback)
{
    CallbackWrapper *wrapper = new CallbackWrapper();
    wrapper->callback = callback;
    ::SendMessage(m_hMainWnd, WM_SEND_CALLBACK, (WPARAM)wrapper, 0);
}

void MsgCallback::wait(UINT uTimeout, const CallbackFunc& callback, UINT uTargetMsg)
{
	if (uTimeout != INFINITE)
	{
        SetTimer(m_hMainWnd, 1, uTimeout, TimerProc);
	}

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
    	if (msg.message == WM_WAIT_TIMEOUT)
    	{
    		callback();
    		break;
    	}
        else if (msg.message == uTargetMsg)
        {
            callback();
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

