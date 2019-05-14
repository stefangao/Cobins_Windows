#include "cobWndThread.h"
#include "base/cobUtils.h"

NS_COB_BEGIN

WndThread::WndThread()
{
	m_hMainWnd = NULL;
}

WndThread::~WndThread()
{
    DestroyWindow(m_hMainWnd);
    PostQuitMessage(0);
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd,wMsg,wParam,lParam);
}
static DWORD WINAPI ThreadProc(PVOID pArg)
{
	WndThread* pThread = (WndThread*)pArg;
    HMODULE hInst = GetModuleHandle(NULL);

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

    pThread->m_hMainWnd = CreateWindowEx(0,
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

    COBLOG("WndThread::ThreadProc() exit\n");
    return 0;
}

HWND WndThread::create()
{
    HANDLE hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
    CloseHandle(hThread);

    m_hEvent = CreateEvent(0, FALSE, FALSE, NULL);
    WaitForSingleObject(m_hEvent, INFINITE);
    CloseHandle(m_hEvent);

    COBLOG("WndThread::create() End: hMainWnd=%x\n", m_hMainWnd);
    return m_hMainWnd;
}

NS_COB_END