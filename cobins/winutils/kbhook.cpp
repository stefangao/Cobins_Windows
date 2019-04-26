#include "kbhook.h"
#include "wtermin.h"
#include "wndbase.h"

static HINSTANCE glhInstance=NULL;   //DLL实例句柄 
static HHOOK glhKeyboardHook = NULL; //安装的鼠标勾子句柄
static HWND  ghHostWnd = NULL;       //host window handle

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	WT_Trace("kbhook: DllMain glhInstance = %x", hInstance);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		glhInstance = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{

	}

	return 1;
}

//普通键盘钩子
/*
LRESULT WINAPI KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
    if (nCode>=0)
    {
        if (WSH_GetMimWnd() != NULL)
            WSH_PostCmd2Emb(WSH_MSG_KEY, wParam, lParam);
        else
            PostMessage(ghHostWnd, WSH_MSG_KEY, wParam, lParam);
    }
    
    return CallNextHookEx(glhKeyboardHook, nCode, wParam, lParam);
    //继续传递消息
}

BOOL KB_StartHook(HWND hHostWnd)
{
   	DWORD ThreadId = 0;

    WT_Trace( "KB_StartHook: instance = %x\n", glhInstance );

	if( !(glhKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, glhInstance, ThreadId)) )
	{
		WT_Trace( "Hook keyboard err = %d", GetLastError() );
		return FALSE;
	}

    ghHostWnd = hHostWnd;
	return TRUE; 
}*/

//Hook 底层键盘钩子(2009.10.1发现普通键盘钩子在家里台式机的联众四国军旗中失效)
LRESULT WINAPI KeyboardProc(int nCode,WPARAM wparam,LPARAM lparam)
{
	PKBDLLHOOKSTRUCT pKeyInfo  = (PKBDLLHOOKSTRUCT)lparam;
    if (nCode >= 0)
    {
        LPARAM lParam = 1;
        lParam += pKeyInfo->scanCode << 16;
        lParam += pKeyInfo->flags << 24;
        if (pKeyInfo->flags == 0x80)
        {
            lParam += 1 << 30; //Specifies the previous key state. The value is always 1 for a WM_KEYUP message. (MSDN)
        }

        //WT_Trace("KeyHook: curprocess=%x\n", GetCurrentProcessId());

        if (ghHostWnd != NULL)
        {
            PostMessage(ghHostWnd, WSH_MSG_KEY, (WPARAM)pKeyInfo->vkCode, lParam);
        }

        //WT_Trace("scancode=%x,flags=%x,time=%x, dwMsg=%x\n", pKeyInfo->scanCode, pKeyInfo->flags, pKeyInfo->time, lParam);
    }

	return CallNextHookEx(glhKeyboardHook, nCode, wparam, lparam);
}

BOOL KB_StartHook(HWND hHostWnd)
{
   	DWORD ThreadId = 0;
    
    if (glhKeyboardHook != NULL)
    {
        WT_Trace( "KB_StartHook: 钩子已存在\n");
        return FALSE;
    }
    
    if (NULL == (glhKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, glhInstance, ThreadId)))
    {
        WT_Trace( "KB_StartHook: Hook Keyboard error = %d", GetLastError() );
        return FALSE;
    }
    
    ghHostWnd = hHostWnd;
    
    WT_Trace("glhKeyboardHook=%x\n", glhKeyboardHook);
    
    return TRUE; 
}

BOOL KB_StopHook(HWND hHostWnd)
{
    if(glhKeyboardHook && UnhookWindowsHookEx(glhKeyboardHook))
    {
        glhKeyboardHook = NULL;
        return TRUE;
    }
    else
    {
        WT_Trace("卸载键盘钩子失败\n");
    }
    
    return FALSE;
}