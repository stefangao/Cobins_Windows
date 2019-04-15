#include "WndThread.h"
#include "wtermin.h"

WndThread::WndThread()
{
	mhMainWnd = NULL;
}

WndThread::~WndThread()
{
    DestroyWindow(m_hMainWnd);
    PostQuitMessage();
}

static LRESULT WndProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    return DefWindowProc(hWnd,wMsg,wParam,lParam);
}
static DWORD ThreadProc(PVOID pArg)
{
	WndThread* pThread = (WndThread*)pArg;
	hInst = GetModuleHandle(NULL);

    WNDCLASS wc = {0};
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = TEXT("SimpleWindow");
    RegisterClass(&wc);

    m_hMainWnd = CreateWindowEx(0,
                TEXT("SimpleWindow"),
                TEXT("SimpleWindow"),
                WS_VISIBLE,
                0,
                0,
                200,
                200,
                NULL,
                NULL,
				hInst,
                0);

    SetEvent(pThread->m_hEvent);

    //The message loop
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    WT_Trace("WndThread::ThreadProc() exit\n");
    return 0;
}

HANDLE WndThread::create()
{
    HANDLE hThread = CreateThread(NULL, 0, ThreadProc,　this, 0, NULL);
    CloseHandle(hThread);

    m_hEvent = CreateEvent(0, FALSE, FALSE, NULL);
    WaitForSingleObject(m_hEvent, INFINITE);
    CloseHanle(m_hEvent);

    WT_Trace("WndThread::create() End: hMainWnd=%x\n", m_hMainWnd);
    return m_hMainWnd;
}
