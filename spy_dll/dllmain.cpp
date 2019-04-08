#include "windows.h"
#include "wtermin.h"

extern "C" 
{
    static HHOOK     g_hHook = NULL;	    //the handle to the hook procedure
    static HINSTANCE g_hinstDll = NULL;     //the handle to DLL module

    //消息回调函数
    LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        //WT_Trace("---GetMsgProc-----\n");
        return CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }

    _declspec(dllexport) BOOL HookThread(DWORD ThreadId)
    {
        if (g_hHook == NULL)
        {
            WT_Trace("Target window thread = 0x%08x, g_hinstDll=%x\r\n", ThreadId, g_hinstDll);

            g_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hinstDll, ThreadId);
            if (g_hHook == NULL)
            {
                WT_Trace("Hook error!\n");
                return FALSE;
            }
            else
            {
                WT_Trace("Hook target thread Successfully\r\n");
            }
        }

        return TRUE;
    }

    _declspec(dllexport) BOOL HookWnd(HWND hDestWnd)
    {
        DWORD dwProcessId = NULL;
        DWORD dwThreadId = NULL;

        if (g_hHook != NULL || hDestWnd == NULL)
            return NULL;

        dwThreadId = GetWindowThreadProcessId(hDestWnd, &dwProcessId);
        if (dwThreadId == NULL)
            return NULL;

        WT_Trace("Target window thread = 0x%08x, g_hinstDll=%x\r\n", dwThreadId, g_hinstDll);

        g_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hinstDll, dwThreadId);
        if (g_hHook == NULL)
        {
            WT_Trace("Hook error!\n");
            return FALSE;
        }
        else
        {
            WT_Trace("Hook target thread Successfully\r\n");
        }

        return TRUE;
    }

}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        WT_Trace("***** DLL_PROCESS_ATTACH: process=%x, hinstDLL=%x\n", GetCurrentProcessId(), hinstDLL);

        g_hinstDll = hinstDLL;
    }
    break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
    {
        WT_Trace("***** DLL_PROCESS_DETACH: process=%x, hinstDLL=%x\n", GetCurrentProcessId(), hinstDLL);
    }
    break;
    }

    return TRUE;
}