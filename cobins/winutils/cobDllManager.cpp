#include "cobDllManager.h"

NS_COB_BEGIN

DllManager::DllManager()
{
    m_hHook = NULL;
}

BOOL DllManager::inject(HWND hTargetWnd, const std::string& dllPath)
{
    if (hTargetWnd && m_hHook == NULL)
     {
         HINSTANCE hInst = LoadLibraryA("spy_dll.dll");
         if (hInst)
         {
             typedef HHOOK(*HookWndFunc) (HWND);
             HookWndFunc hookWnd = (HookWndFunc)GetProcAddress(hInst, "HookWnd");
             if (hookWnd)
             {
                 m_hHook = hookWnd(hTargetWnd);
                 COBLOG("HookWnd result=%x\n", m_hHook);
             }
             FreeLibrary(hInst);
         }
     }
    return true;
}

BOOL DllManager::eject()
{
    BOOL ret = false;
    if (m_hHook != NULL)
    {
        ret = UnhookWindowsHookEx(m_hHook);
        m_hHook = NULL;
    }
    return ret;
}

NS_COB_END
