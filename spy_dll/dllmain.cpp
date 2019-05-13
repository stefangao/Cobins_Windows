#include "windows.h"
#include "cobins.h"
#include "AppDelegate.h"

#define WM_HOOKWND_DONE  (WM_USER + 0x123)

extern "C"
{
static HHOOK g_hHook = NULL;	     //the handle to the hook procedure
static HINSTANCE g_hinstDll = NULL;  //the handle to DLL module
static HANDLE g_hEvent = NULL;
static const char* g_sEventName = "__HookWndEvent__";
static AppDelegate* g_pAppDelegate = NULL;
static HWND g_hMainWnd = NULL;

static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (g_hMainWnd == NULL)
    {
        MSG *lpMsg = (MSG*)lParam;
        if (lpMsg->message == WM_HOOKWND_DONE)
        {
            WT_Trace("OnHooked: process=%x, gameWnd=%x\n", GetCurrentProcessId(), lpMsg->hwnd);

            g_hMainWnd = lpMsg->hwnd;
            g_pAppDelegate = new AppDelegate();
            g_pAppDelegate->create(lpMsg->hwnd, "embed123").start();

            g_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, g_sEventName);
            if (g_hEvent != NULL)
                SetEvent(g_hEvent);
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

_declspec(dllexport) HHOOK HookWnd(HWND hTargetWnd)
{
    HHOOK hHook = NULL;
    DWORD dwProcessId = NULL;
    DWORD dwThreadId = NULL;

    if (g_hHook != NULL || hTargetWnd == NULL)
        return NULL;

    dwThreadId = GetWindowThreadProcessId(hTargetWnd, &dwProcessId);
    if (dwThreadId == NULL)
        return NULL;

    COBLOG("HookWnd: Target thread=0x%08x, hTargetWnd=%x\n", dwThreadId, hTargetWnd);

    g_hEvent = CreateEvent(NULL, FALSE, FALSE, g_sEventName);
    hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hinstDll, dwThreadId);
    if (hHook != NULL)
    {
        COBLOG("HookWnd: Hook Done!\n");
        PostMessage(hTargetWnd, WM_HOOKWND_DONE, 0, 0);
        if (g_hEvent)
        {
            WaitForSingleObject(g_hEvent, INFINITE);
            CloseHandle(g_hEvent);
            g_hEvent = NULL;
        }
    }
    else
    {
        COBLOG("HookWnd: Hook falied!\n");
    }
    g_hHook = hHook;
    return hHook;
}
} //end of extern "C"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        COBLOG("DLL_PROCESS_ATTACH: process=%x, hinstDLL=%x\n", GetCurrentProcessId(), hinstDLL);
        g_hinstDll = hinstDLL;
    }
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        COBLOG("DLL_PROCESS_DETACH: process=%x, g_hMainWnd=%x\n", GetCurrentProcessId(), g_hMainWnd);
        if (g_hMainWnd && g_pAppDelegate)
        {
            g_pAppDelegate->destroy();
            CloseHandle(g_hEvent);
            g_pAppDelegate = NULL;
            g_hMainWnd = NULL;
        }
        break;

    default:
        break;
    }

    return TRUE;
}
