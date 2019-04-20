#include "windows.h"
#include "winutils/cobPipe.h"
#include "cobins.h"
#include "AppDelegate.h"

extern "C"
{
static HHOOK g_hHook = NULL;	    //the handle to the hook procedure
static HINSTANCE g_hinstDll = NULL;     //the handle to DLL module
static BOOL g_hostFlag = FALSE;
static BOOL g_hooked = FALSE;
static cob::Pipe g_pipe;
static AppDelegate* g_pAppDelegate = NULL;
static HWND g_hMainWnd = NULL;

//消息回调函数
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (!g_hooked)
    {
        g_hooked = true;
        if (!g_hostFlag)
        {
            MSG *lpMsg;
            lpMsg = (MSG*)lParam;
            if (lpMsg->hwnd != NULL)
            {
				WT_Trace("OnHooked: process=%x, gameWnd=%x\n", GetCurrentProcessId(), lpMsg->hwnd);
				g_pAppDelegate = new AppDelegate();
				g_pAppDelegate->create(lpMsg->hwnd, "embed123").start();
            }
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

_declspec(dllexport) void triggerAppliation()
{
    if (!g_hooked)
    {
        WT_Trace("OnHooked: process=%x,hinstDLL=%x,g_hostFlag=%d\n", GetCurrentProcessId(), g_hinstDll, g_hostFlag);
        g_hooked = true;
        if (!g_hostFlag)
        {
			g_pAppDelegate->create(g_hMainWnd, "embed123");
        }
    }
}

_declspec(dllexport) HHOOK HookWnd(HWND hDestWnd)
{
    HHOOK hHook = NULL;
    DWORD dwProcessId = NULL;
    DWORD dwThreadId = NULL;

    if (g_hHook != NULL || hDestWnd == NULL)
    return NULL;

    dwThreadId = GetWindowThreadProcessId(hDestWnd, &dwProcessId);
    if (dwThreadId == NULL)
    return NULL;

    COBLOG("Target window thread = 0x%08x, g_hinstDll=%x\r\n", dwThreadId, g_hinstDll);

    g_hostFlag = TRUE;
    g_hMainWnd = hDestWnd;
    hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hinstDll, dwThreadId);
    if (hHook == NULL)
    {
        COBLOG("Hook error!\n");
        return NULL;
    }
    else
    {
        COBLOG("Hook target thread Successfully\r\n");
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
		if (g_hooked && !g_hostFlag && g_pAppDelegate)
		{
			g_pAppDelegate->destroy();
		}
		COBLOG("DLL_PROCESS_DETACH: process=%x, hinstDLL=%x\n", GetCurrentProcessId(), hinstDLL);
        break;

    default:
        break;
    }

    return TRUE;
}
