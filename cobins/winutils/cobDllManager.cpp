#include "cobDllManager.h"

NS_COB_BEGIN

DllManager::DllManager()
{
    m_hHook = NULL;
}

BOOL DllManager::inject(HWND hTargetWnd, const std::string& dllPath)
{
    HHOOK hHook = NULL;
    if (hTargetWnd && m_hHook == NULL)
     {
         HINSTANCE hInst = LoadLibraryA(dllPath.c_str());
         if (hInst)
         {
             typedef HHOOK(*HookWndFunc) (HWND);
             HookWndFunc hookWnd = (HookWndFunc)GetProcAddress(hInst, "HookWnd");
             if (hookWnd)
             {
                 hHook = hookWnd(hTargetWnd);
             }
             FreeLibrary(hInst);
         }
     }
    m_hHook = hHook;
    return hHook != NULL;
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
