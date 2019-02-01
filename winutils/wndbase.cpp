#include "wndbase.h"
#include <math.h>
/*#include "cntt_util.h"
#include "cntt_string.h"*/
#include "wtermin.h"
#include "MusicPlayer.h"

static CMusicPlayer m_AlertPlayer;

BOOL WBS_GetWindowPixel(HWND hWnd, POINT pt, COLORREF& clr)
{
	HDC hDevDC;
	
	if (hWnd == NULL)
		return FALSE;

	hDevDC = GetDC(hWnd);

	clr = GetPixel(hDevDC, pt.x, pt.y);

	ReleaseDC(hWnd, hDevDC);

	return TRUE;
}

BOOL WBS_GetScreenPixel(POINT pt, COLORREF& clr)
{
	HDC hdc;

	hdc = CreateDC("DISPLAY",NULL,NULL,NULL);

	clr = GetPixel( hdc, pt.x, pt.y );
    if (CLR_INVALID == clr)
    {
        DeleteDC(hdc);
        return FALSE;
    }

    DeleteDC(hdc);

	return TRUE;
}

COLORREF WBS_SetScreenPixel(POINT pt, COLORREF clr)
{
	HDC hdc;
    COLORREF newclr;
    
    hdc = CreateDC("DISPLAY",NULL,NULL,NULL);

	newclr = SetPixel( hdc, pt.x, pt.y, clr );

    DeleteDC(hdc);

    return newclr;
}

BOOL WBS_RGB2YUV(COLORREF rgb, YUV_t &yuv)
{
	unsigned char r, g, b;

	r = (int)rgb&0xff;
	g = (int)(rgb>>8)&0xff;
	b = (int)(rgb>>16)&0xff;

	yuv.y = (int)(0.299*r + 0.587*g + 0.114*b);
	yuv.u = (int)(-0.147*r - 0.289*g + 0.436*b);
	yuv.v = (int)(0.615*r - 0.515*g - 0.100*b);

	return TRUE;
}

BOOL WBS_YUV2RGB(YUV_t yuv, COLORREF &rgb)
{
	unsigned char r, g, b;

    r = (unsigned char)(yuv.y + 1.14*yuv.v);
    g = (unsigned char)(yuv.y - 0.39*yuv.u - 0.58*yuv.v);
    b = (unsigned char)(yuv.y + 2.03*yuv.u);

    rgb = r + g*256 + b*256*256;		

	return TRUE;
}

BOOL WBS_RGB2HSV(COLORREF rgb, HSV_t &hsv)
{
    double var_R, var_G, var_B;
    double var_Min, var_Max, del_Max;
    double var_H, var_S, var_V;
    double del_R, del_G, del_B;
    
    var_R = GetRValue(rgb) / 255.; 
    var_G = GetGValue(rgb) / 255.;
    var_B = GetBValue(rgb) / 255.;
    
    var_Min = min3(var_R, var_G, var_B);
    var_Max = max3(var_R, var_G, var_B);
    del_Max = var_Max - var_Min;
    
    var_V = (var_Max + var_Min)/2.0;
    
    if (del_Max == 0)    //This is a gray, no chroma...
    {
        var_H = 0; 
        var_S = 0;
    }
    else                   //Chromatic data...
    {
        
        del_R = ( ( ( var_Max - var_R ) / 6. ) + ( del_Max / 2. ) ) / del_Max;
        del_G = ( ( ( var_Max - var_G ) / 6. ) + ( del_Max / 2. ) ) / del_Max;
        del_B = ( ( ( var_Max - var_B ) / 6. ) + ( del_Max / 2. ) ) / del_Max;
        
        if      ( var_R == var_Max ) var_H = del_B - del_G;
        else if ( var_G == var_Max ) var_H = ( 1.0 / 3.0 ) + del_R - del_B;
        else if ( var_B == var_Max ) var_H = ( 2.0 / 3.0 ) + del_G - del_R;
        
        if ( var_H < 0. )  var_H += 1;
        if ( var_H > 1. )  var_H -= 1;

        var_S = del_Max / (1.0 - fabs(1-2*var_V));
    }     
    
    var_H = var_H*240;
    var_S = var_S*240;
    var_V = var_V*240;

    hsv.h = (BYTE)(var_H + 0.5);
    hsv.s = (BYTE)(var_S + 0.5);
    hsv.v = (BYTE)(var_V + 0.5);

    return TRUE;
}

//TODO: WBS_HSV2RGB doesnot work
BOOL WBS_HSV2RGB(HSV_t hsv, COLORREF &rgb)
{
    double var_R, var_G, var_B;
    double var_i, var_1, var_2, var_3;
    double var_H, var_S, var_V;
    
    var_H = hsv.h / 240.; 
    var_S = hsv.s / 240.;
    var_V = hsv.v / 240.;
    
    if (var_S == 0)
    {
        var_R = var_V * 255;
        var_G = var_V * 255;
        var_B = var_V * 255;
    }
    else
    {
        var_H = var_H * 6;
        if (var_H == 6)  var_H = 0;       //H must be < 1
        
        var_i = int(var_H);               //Or ... var_i = floor( var_h )
        var_1 = var_V * ( 1 - var_S );
        var_2 = var_V * ( 1 - var_S * ( var_H - var_i ) );
        var_3 = var_V * ( 1 - var_S * ( 1 - ( var_H - var_i ) ) );
        
        if      ( var_i == 0 ) { var_R = var_V ; var_G = var_3 ; var_B = var_1 ;}
        else if ( var_i == 1 ) { var_R = var_2 ; var_G = var_V ; var_B = var_1 ;}
        else if ( var_i == 2 ) { var_R = var_1 ; var_G = var_V ; var_B = var_3 ;}
        else if ( var_i == 3 ) { var_R = var_1 ; var_G = var_2 ; var_B = var_V ;}
        else if ( var_i == 4 ) { var_R = var_3 ; var_G = var_1 ; var_B = var_V ;}
        else                   { var_R = var_V ; var_G = var_1 ; var_B = var_2 ;}
        
        var_R = var_R * 255;
        var_G = var_G * 255;
        var_B = var_B * 255;
    }

    return TRUE;
}

/** \brief Fetch the handle of child window on specified point
    \param hwndParent The handle of the timer to kill
    \param point The handle of the timer to kill
    \param uFlags Specifies which child windows to skip. This parameter can be a combination of the following values:
    CWP_ALL -- Do not skip any child windows 
    CWP_SKIPINVISIBLE -- Skip invisible child windows 
    CWP_SKIPDISABLED Skip -- disabled child windows 
    CWP_SKIPTRANSPARENT Skip -- transparent child windows 
    \return The window handle if succeed; otherwise NULL.
 */
HWND WBS_GetWindowFromPoint(HWND hwndParent, POINT point, UINT uFlags )
{
	if(hwndParent != NULL)
		return ::ChildWindowFromPointEx(hwndParent, point, uFlags);
	//返回光标(point)所在点的子窗口句柄

	// Find the smallest "window" still containing the point
	// Doing this prevents us from stopping at the first window containing the point
	RECT rect, rectSearch;
	HWND pWnd, hWnd, hSearchWnd;

	hWnd = ::WindowFromPoint(point);//得到光标(point)所在点的窗口句柄
	if(hWnd != NULL)
	{
		// Get the size and parent for compare later
		::GetWindowRect(hWnd, &rect);	//得到整个窗口在屏幕上的矩形框位置
		pWnd = ::GetParent(hWnd);       //得到父窗口句柄

		// We only search further if the window has a parent
		if(pWnd != NULL)
		{
			// Search from the window down in the Z-Order
			hSearchWnd = hWnd;
			do{
				hSearchWnd = ::GetWindow(hSearchWnd, GW_HWNDNEXT);//如果再也找不到这样的窗口，该函数就会返回NULL
				//GetWindow得到和句柄为hSearchWnd(即首次循环为hWnd)的窗口相关的窗口，其关系由GW_HWNDNEXT决定，这里是寻找兄弟窗口

				// Does the search window also contain the point, have the same parent, and is visible?
				::GetWindowRect(hSearchWnd, &rectSearch);
				if(::PtInRect(&rectSearch, point) && ::GetParent(hSearchWnd) == pWnd && ::IsWindowVisible(hSearchWnd))
				{
					// It does, but is it smaller?比较看谁的面积最小
					if(((rectSearch.right - rectSearch.left) * (rectSearch.bottom - rectSearch.top)) < ((rect.right - rect.left) * (rect.bottom - rect.top)))
					{
						// Found new smaller window, update compare window
						hWnd = hSearchWnd;
						::GetWindowRect(hWnd, &rect);
					}
				}
			}
			while(hSearchWnd != NULL);
		}
	}
	return hWnd;
}

/*
void WBS_LButtonDown(HWND hWnd, int xPos, int yPos)
{
    PostMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(xPos, yPos));
}

void WBS_LButtonUp(HWND hWnd, int xPos, int yPos)
{
    PostMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(xPos, yPos));
}*/

void WBS_LButtonDown( )
{
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
}

void WBS_LButtonUp( )
{
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
}

void WBS_MouseMove(DWORD dx, DWORD dy)
{
    mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}

void WBS_RButtonDown( )
{
    mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
}

void WBS_RButtonUp( )
{
    mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
}

void WBS_RButtonDownMove(POINT pt, int dx, int dy)
{
    SetCursorPos(pt.x, pt.y);
    mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_MOVE,dx,dy,0,0);
    SetCursorPos(pt.x+dx, pt.y+dy);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
    WBS_GUIWait(50);
}


BOOL WBS_SetCursorPos(int x, int y)
{
    return SetCursorPos(x, y);
}

BOOL WBS_GetCursorPos(LPPOINT lpPoint)
{
    return GetCursorPos(lpPoint);
}

BOOL WBS_SetWinCursorPos(int x, int y, HWND hWnd)
{
    POINT pt = {x, y};

    if (hWnd != NULL)
    {
        ClientToScreen(hWnd, &pt);
    }

    return SetCursorPos(pt.x, pt.y);
}

BOOL WBS_GetCursorWinPos(LPPOINT lpPoint)
{
    HWND hWnd = NULL;

    GetCursorPos(lpPoint);

    hWnd = WBS_GetWindowFromPoint( NULL, *lpPoint, 0 );
    if (!hWnd)
        return FALSE;
    
    return ScreenToClient(hWnd, lpPoint);
}

void WBS_MouseLClick()
{
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
}

void WBS_MouseLDClick()
{
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    WBS_GUIWait(60);
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
}

void WBS_MouseRDClick()
{
    mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
    WBS_GUIWait(60);
    mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
}

void WBS_MouseRClick()
{
    mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
    WBS_GUIWait(50);
    mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
}

void WBS_PressKey(BYTE key, unsigned long combinkey)
{
    if (combinkey & WBS_KEY_SHIFT)
        keybd_event(VK_SHIFT, 0, 0, 0);

    if (combinkey & WBS_KEY_CTL)
        keybd_event(VK_CONTROL, 0, 0, 0);

    if (combinkey & WBS_KEY_ALT)
        keybd_event(VK_MENU, 0, 0, 0);
    
    keybd_event(key, 0, 0 ,0);
    keybd_event(key, 0, KEYEVENTF_KEYUP,0);

    if (combinkey & WBS_KEY_SHIFT)
        keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);

    if (combinkey & WBS_KEY_CTL)
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

    if (combinkey & WBS_KEY_ALT)
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

}


BOOL WBS_MakeWndMark(HWND hWnd, POINT pt, int width)
{
    HDC hdc;
    RECT rect;

    if (!hWnd || !IsWindow(hWnd))
        return FALSE;

    hdc = GetDC(hWnd);
    if (!hdc)
        return FALSE;

    rect.left = pt.x - width/2;
    rect.top = pt.y - width/2;
    rect.right = pt.x + width/2;
    rect.bottom = pt.y + width/2;

    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    ReleaseDC(hWnd, hdc);

    return TRUE;
}

BOOL WBS_MakeScreenMark(POINT pt, int width)
{
    HDC hdc;
    RECT rect;

	hdc = CreateDC("DISPLAY",NULL,NULL,NULL);
    if (!hdc)
        return FALSE;

    rect.left = pt.x - width/2;
    rect.top = pt.y - width/2;
    rect.right = pt.x + width/2;
    rect.bottom = pt.y + width/2;

    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    DeleteDC(hdc);

    return TRUE;
}

void WBS_AlertPlay(LPCTSTR lpszWaveFileName)
{
    m_AlertPlayer.Play(lpszWaveFileName);
}

void WBS_AlertStop()
{
    m_AlertPlayer.Stop();
}

BOOL WBS_IsAlertPlaying()
{
    if (m_AlertPlayer.GetPlayStatus() == MCI_MODE_PLAY)
        return TRUE;
    
    return FALSE;
}

void WBS_Alert(LPCSTR lpAudioPath, BOOL bRestart)
{
    if (!bRestart && WBS_IsAlertPlaying())
        return;

    WBS_AlertPlay(lpAudioPath);
}

///////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    int timeout;
    int matched;
    MatchFunc_T matchfunc;
	unsigned long param;
	int result;

    UINT waittmid;
    UINT chktmid;

}GUIWaitInfo_T;

static void  GUIWait_TimeOut(CNTT_HANDLE handle, unsigned long ulParam)
{
    GUIWaitInfo_T *waitinfo = (GUIWaitInfo_T*)ulParam;
    if (!waitinfo)
        return;

    waitinfo->timeout = 1;
}

static void  GUIWait_CheckProc(CNTT_HANDLE handle, unsigned long ulParam)
{
    GUIWaitInfo_T *waitinfo = (GUIWaitInfo_T*)ulParam;
    if (!waitinfo)
        return;

    if (waitinfo->matchfunc(waitinfo->param) == waitinfo->result)
    {
        cntt_KillTimer(handle);
        waitinfo->matched = 1;
        return;
    }
}

static BOOL GUIWait(DWORD dwMilliseconds, MatchFunc_T matchfunc, unsigned long param, int result)
{
  	MSG msg;
    CNTT_HANDLE waithdl = NULL, chkhdl = NULL;

    GUIWaitInfo_T *waitinfo = (GUIWaitInfo_T*)malloc(sizeof(GUIWaitInfo_T));
    memset(waitinfo, 0, sizeof(GUIWaitInfo_T));

    if (!dwMilliseconds && !matchfunc)
        return FALSE;

    if (dwMilliseconds > 0)
    {
        if ((waithdl = cntt_SetTimer(dwMilliseconds, GUIWait_TimeOut, (unsigned long)waitinfo, CNTT_TIME_ONESHOT)) == NULL)
        {
            free(waitinfo);
            return FALSE;
        }

        waitinfo->timeout = 0;
        waitinfo->waittmid = *((UINT*)waithdl);
    }

    if (matchfunc)
    {
        if ((chkhdl = cntt_SetTimer(5, GUIWait_CheckProc, (unsigned long)waitinfo, 0)) == NULL)
        {
            free(waitinfo);
            return FALSE;
        }

        waitinfo->matchfunc = matchfunc;
        waitinfo->matched = 0;
        waitinfo->param = param;
        waitinfo->result = result;
        waitinfo->chktmid = *((UINT*)chkhdl);
    }

    while (1) 
	{
        if (waitinfo->matched)
        {
            if (!waitinfo->timeout && waithdl)
                cntt_KillTimer(waithdl);

            free(waitinfo);
            {
               return TRUE;
            }
        }
        else if (waitinfo->timeout)
        {
            if (!waitinfo->matched && chkhdl)
                cntt_KillTimer(chkhdl);

            free(waitinfo);

            return FALSE;
        }

        if (GetMessage(&msg, NULL, 0, 0))
        {
            //CWinThread *pThread = AfxGetThread();
            //if (pThread && !pThread->PreTranslateMessage(&msg))
            {
                if (msg.message == WM_TIMER && 
                    msg.wParam != waitinfo->chktmid &&
                    msg.wParam != waitinfo->waittmid)
                        continue;
                
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

}

BOOL WBS_GUIWaitTrue(DWORD dwMilliseconds, MatchFunc_T matchfunc, unsigned long param)
{
	return GUIWait(dwMilliseconds, matchfunc, param, TRUE);
}

BOOL WBS_GUIWaitFalse(DWORD dwMilliseconds, MatchFunc_T matchfunc, unsigned long param)
{
	return GUIWait(dwMilliseconds, matchfunc, param, FALSE);
}

BOOL WBS_GUIWait(DWORD dwMilliseconds)
{
	return GUIWait(dwMilliseconds, 0, 0, 0);
}

/*
static BOOL GUIWaitM(DWORD dwMilliseconds, DWORD dwMsg, WaitMFunc_T waitmfunc, unsigned long userdata)
{
  	MSG msg;
    CNTT_HANDLE waithdl = NULL, chkhdl = NULL;

    GUIWaitInfo_T *waitinfo = (GUIWaitInfo_T*)malloc(sizeof(GUIWaitInfo_T));
    cntt_memset(waitinfo, 0, sizeof(GUIWaitInfo_T));

    if (!dwMilliseconds || !waitmfunc || !dwMsg)
        return FALSE;

    if (dwMilliseconds > 0)
    {
        if ((waithdl = cntt_SetTimer(dwMilliseconds, GUIWait_TimeOut, (unsigned long)waitinfo, CNTT_TIMER_ONCE)) == NULL)
        {
            free(waitinfo);
            return FALSE;
        }

        waitinfo->timeout = 0;
        waitinfo->waittmid = *((UINT*)waithdl);
    }

    while (1) 
	{
        if (GetMessage(&msg, NULL, 0, 0))
        {
            CWinThread *pThread = AfxGetThread();
            if (pThread && !pThread->PreTranslateMessage(&msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (msg.message == dwMsg)
            {
                if (waitmfunc(userdata, msg.wParam, msg.lParam))
                {
                    return TRUE;
                }
            }
        }

        if (waitinfo->timeout)
        {
            if (!waitinfo->matched && chkhdl)
                cntt_KillTimer(chkhdl);

            free(waitinfo);
            return FALSE;
        }
    }
}

BOOL WBS_WaitMessage(DWORD dwMilliseconds, DWORD dwMsg, WaitMFunc_T pWaitMFunc, DWORD dwUserData)
{
	return GUIWaitM(dwMilliseconds, dwMsg, pWaitMFunc, dwUserData);
}*/

////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    int timeout;

}GUIWaitMsg_t;

static void  GUIWaitMsg_TimeOut(CNTT_HANDLE handle, unsigned long ulParam)
{
    GUIWaitMsg_t *waitmsg = (GUIWaitMsg_t*)ulParam;
    if (waitmsg == NULL)
        return;

    waitmsg->timeout = 1;
}

BOOL WBS_WaitMessage(DWORD dwMilliSeconds, UINT uMessage, LPMSG lpMsg)
{
  	MSG msg;
    CNTT_HANDLE waithdl = NULL;

    GUIWaitMsg_t *waitmsg = (GUIWaitMsg_t*)malloc(sizeof(GUIWaitMsg_t));
    waitmsg->timeout = 0;

    if (dwMilliSeconds > 0)
    {
        if ((waithdl = cntt_SetTimer(dwMilliSeconds, GUIWaitMsg_TimeOut, (unsigned long)waitmsg, CNTT_TIME_ONESHOT)) == NULL)
        {
            free(waitmsg);
            return FALSE;
        }
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        CWinThread *pThread = AfxGetThread();
        if (pThread && !pThread->PreTranslateMessage(&msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == uMessage)
        {
            if (lpMsg != NULL)
            {
                *lpMsg = msg;
            }

            if (!waitmsg->timeout)
            {
                cntt_KillTimer(waithdl);
            }

            free(waitmsg);
            return TRUE;
        }
        else if (waitmsg->timeout)
        {
            free(waitmsg);
            return FALSE;
        }
    }

    return FALSE;
}

static DWORD WINAPI GUIBlock_CheckProc(LPVOID lpParameter)
{
	GUIWaitThread_T* pThread = (GUIWaitThread_T*)lpParameter;
	DWORD dwResult = FALSE;

    WT_Trace("GUIWait_CheckProc Begin\n");

	cntt_assert(pThread);

	while (!pThread->timeout)
	{
    	if (pThread->matchfunc(pThread->param) == pThread->result)
		{
		    SetEvent((HANDLE)pThread->hEvent);
			dwResult = TRUE;
			break;
		}

		Sleep(100);

        WT_Trace("GUIWait_Checking...\n");
	}

	WT_Trace("GUIWait_CheckProc End\n");
	
	return dwResult;
}


static BOOL GUIBlock(DWORD dwMilliseconds, MatchFunc_T matchfunc, unsigned long param, int result)
{
	GUIWaitThread_T *pThread;
	
	pThread = (GUIWaitThread_T*)malloc(sizeof(GUIWaitThread_T));
	if(pThread == NULL)
	{
		return FALSE;
	}
	
	pThread->hEvent = CreateEvent(0, 1, 0, NULL);
	if(pThread->hEvent == 0)
	{
		free(pThread);
		return FALSE;
	}

	if (matchfunc)
	{
		pThread->result = result;
    	pThread->timeout = FALSE;
 		pThread->matchfunc = matchfunc;
		pThread->param = param;
		
		pThread->hThread = CreateThread(NULL, 0, GUIBlock_CheckProc, (LPVOID)pThread, 0, &pThread->threadid);
		if(pThread->hThread == 0)
		{
			free(pThread);
			return FALSE;
		}
	}
	
	WaitForSingleObject((HANDLE) pThread->hEvent, dwMilliseconds);

	if (matchfunc)
	{
		pThread->timeout = TRUE;
		WaitForSingleObject((HANDLE) pThread->hThread, 5000);
		CloseHandle((HANDLE) pThread->hThread);
	}

    CloseHandle((HANDLE) pThread->hEvent);
 
    free(pThread);

    return TRUE;  //be probem, TBD
}

BOOL WBS_GUIBlockTrue(DWORD dwMilliseconds, MatchFunc_T matchfunc, unsigned long param)
{
	return GUIBlock(dwMilliseconds, matchfunc, param, TRUE);
}

BOOL WBS_GUIBlockFalse(DWORD dwMilliseconds, MatchFunc_T matchfunc, unsigned long param)
{
	return GUIBlock(dwMilliseconds, matchfunc, param, FALSE);
}

BOOL WBS_GUIBlock(DWORD dwMilliseconds)
{
	return GUIBlock(dwMilliseconds, 0, 0, 0);
}

void WBS_WinShutdown(UINT ShutdownFlag)
{
    OSVERSIONINFO oi;
    oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&oi);
    //如果是NT/2000下需先设置相关权限
    if (oi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
    {
        HANDLE handle;
        TOKEN_PRIVILEGES tkp;
        
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle);
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
        
        tkp.PrivilegeCount = 1;  // one privilege to set    
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
        
        AdjustTokenPrivileges(handle, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    }
    
	//WIN98下关机或注销时可直接调用下面涵数既可
    ::ExitWindowsEx(ShutdownFlag,0);
}

BOOL WBS_PreTranslateMessage(MSG &msg)
{
    CWinThread *pThread = AfxGetThread();
    if (!pThread)
        return FALSE;
        
    return pThread->PreTranslateMessage(&msg);
}

HWND WBS_FindWindow(HWND hwndParent, LPCSTR lpTitle, LPCSTR lpClass)
{
    return FindWindowEx(hwndParent, NULL, lpClass, lpTitle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WBS_ReadStream(HANDLE hProcess, const void *lpBaseAddress, BYTE *pBuf, int nBufLen)
{
    DWORD dwReadLen;

    if (!ReadProcessMemory(hProcess, lpBaseAddress, pBuf, nBufLen, &dwReadLen))
        return -1;

    return dwReadLen;
}

BOOL WBS_WriteStream(HANDLE hProcess, void *lpBaseAddress, BYTE *pData, DWORD nDataLen)
{
    DWORD dwWrittenLen;

    if (!WriteProcessMemory(hProcess, lpBaseAddress, pData, nDataLen, &dwWrittenLen))
        return FALSE;
    
    if (dwWrittenLen != nDataLen)
        return FALSE;

    return TRUE;
}

DWORD WBS_ReadLong(HANDLE hProcess, const void *lpBaseAddress)
{
    BYTE  Buf[4];
    DWORD dwReadLen;

    if (!ReadProcessMemory(hProcess, lpBaseAddress, Buf, 4, &dwReadLen))
        return 0;

    return MAKELONG(MAKEWORD(Buf[0], Buf[1]), MAKEWORD(Buf[2], Buf[3]));
}

float WBS_ReadFloat(HANDLE hProcess, const void *lpBaseAddress)
{
    BYTE  Buf[4];
    DWORD dwReadLen;

    if (!ReadProcessMemory(hProcess, lpBaseAddress, Buf, 4, &dwReadLen))
        return 0;

    return *(float*)Buf;
}

xmlXPathObjectPtr WBS_GetXPathNodeset(xmlDocPtr doc, const xmlChar *xpath)
{  
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(doc);
    if (context == NULL) 
    {
        printf("context is NULL\n");
        return NULL; 
    }
    
    result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    if (result == NULL) 
    {
        printf("xmlXPathEvalExpression return NULL\n"); 
        return NULL;  
    }
    
    if (xmlXPathNodeSetIsEmpty(result->nodesetval))
    {
        xmlXPathFreeObject(result);
        printf("nodeset is empty\n");
        return NULL;
    }
    
    return result;
}

//截图
HBITMAP WBS_CopyWindowToBitmap(HWND hWnd, LPRECT lpRect) //lpRect 代表选定区域
{
    HDC hScrDC, hMemDC;   
    // 屏幕和内存设备描述表
    HBITMAP hBitmap,hOldBitmap;   
    // 位图句柄
    int nX, nY, nX2, nY2;   
    // 选定区域坐标
    int nWidth, nHeight;   
    // 位图宽度和高度
    int xScrn, yScrn;   
    // 屏幕分辨率
    // 确保选定区域不为空矩形
    if (IsRectEmpty(lpRect))
        return NULL;
    //为屏幕创建设备描述表
    hScrDC = ::GetDC(hWnd);
    //为屏幕设备描述表创建兼容的内存设备描述表
    hMemDC = CreateCompatibleDC(hScrDC);
    // 获得选定区域坐标
    nX = lpRect->left;
    nY = lpRect->top;
    nX2 = lpRect->right;
    nY2 = lpRect->bottom;
    // 获得屏幕分辨率
    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);
    //确保选定区域是可见的
    if (nX < 0)
        nX = 0;
    if (nY < 0)
        nY = 0;
    if (nX2 > xScrn)
        nX2 = xScrn;
    if (nY2 > yScrn)
        nY2 = yScrn;
    nWidth = nX2 - nX;
    nHeight = nY2 - nY;
    // 创建一个与屏幕设备描述表兼容的位图
    hBitmap=CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
    // 把新位图选到内存设备描述表中
    hOldBitmap=(HBITMAP)SelectObject(hMemDC, hBitmap);
    // 把屏幕设备描述表拷贝到内存设备描述表中
    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);
    //得到屏幕位图的句柄
    hBitmap=(HBITMAP)SelectObject(hMemDC, hOldBitmap);
    //清除
    ::ReleaseDC(hWnd, hScrDC);
    DeleteDC(hMemDC);
    // 返回位图句柄
    return hBitmap;
}

HBITMAP WBS_CopyScreenToBitmap(LPRECT lpRect) //lpRect 代表选定区域
{
    HDC hScrDC, hMemDC;   
    // 屏幕和内存设备描述表
    HBITMAP hBitmap,hOldBitmap;   
    // 位图句柄
    int nX, nY, nX2, nY2;   
    // 选定区域坐标
    int nWidth, nHeight;   
    // 位图宽度和高度
    int xScrn, yScrn;   
    // 屏幕分辨率
    // 确保选定区域不为空矩形
    if (IsRectEmpty(lpRect))
        return NULL;
    //为屏幕创建设备描述表
    hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    //为屏幕设备描述表创建兼容的内存设备描述表
    hMemDC = CreateCompatibleDC(hScrDC);
    // 获得选定区域坐标
    nX = lpRect->left;
    nY = lpRect->top;
    nX2 = lpRect->right;
    nY2 = lpRect->bottom;
    // 获得屏幕分辨率
    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);
    //确保选定区域是可见的
    if (nX < 0)
        nX = 0;
    if (nY < 0)
        nY = 0;
    if (nX2 > xScrn)
        nX2 = xScrn;
    if (nY2 > yScrn)
        nY2 = yScrn;
    nWidth = nX2 - nX;
    nHeight = nY2 - nY;
    // 创建一个与屏幕设备描述表兼容的位图
    hBitmap=CreateCompatibleBitmap(hScrDC,nWidth,nHeight);
    // 把新位图选到内存设备描述表中
    hOldBitmap=(HBITMAP)SelectObject(hMemDC,hBitmap);
    // 把屏幕设备描述表拷贝到内存设备描述表中
    BitBlt(hMemDC,0,0, nWidth,nHeight,hScrDC, nX, nY, SRCCOPY);
    //得到屏幕位图的句柄
    hBitmap=(HBITMAP)SelectObject(hMemDC,hOldBitmap);
    //清除
    DeleteDC(hScrDC);
    DeleteDC(hMemDC);
    // 返回位图句柄
    return hBitmap;
}

//函数2 : SaveBitmapToFile 将图片存成文件
BOOL WBS_SaveBitmapToFile(HBITMAP hBitmap, LPCSTR lpFileName) //hBitmap 为刚才的屏幕位图句柄
{ //lpFileName 为位图文件名
    HDC hDC;   
    //设备描述表
    int iBits;   
    //当前显示分辨率下每个像素所占字节数
    WORD wBitCount;   
    //位图中每个像素所占字节数
    //定义调色板大小， 位图中像素字节大小 ， 位图文件大小 ， 写入文件字节数
    DWORD dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
    BITMAP Bitmap;   
    //位图属性结构
    BITMAPFILEHEADER bmfHdr;   
    //位图文件头结构
    BITMAPINFOHEADER bi;   
    //位图信息头结构
    LPBITMAPINFOHEADER lpbi;   
    //指向位图信息头结构
    HANDLE fh, hDib, hPal;
    HPALETTE hOldPal=NULL;
    //定义文件，分配内存句柄，调色板句柄
    
    //计算位图文件每个像素所占字节数
    hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) *
        GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 32;
    //计算调色板大小
    if (wBitCount <= 8)
        dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);
    
    //设置位图信息头结构
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    
    dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
    //为位图内容分配内存
    
    /*xxxxxxxx计算位图大小分解一下(解释一下上面的语句)xxxxxxxxxxxxxxxxxxxx  
    //每个扫描行所占的字节数应该为4的整数倍，具体算法为:
    int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
    if((Bitmap.bmWidth*wBitCount) % 32)
    biWidth++; //不是整数倍的加1
    biWidth *= 4;//到这里，计算得到的为每个扫描行的字节数。
    dwBmBitsSize = biWidth * Bitmap.bmHeight;//得到大小
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/
    
    
    hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;
    // 处理调色板   
    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = ::GetDC(NULL);
        hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
        RealizePalette(hDC);
    }
    // 获取该调色板下新的像素值
    GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi,DIB_RGB_COLORS);
    //恢复调色板   
    if (hOldPal)
    {
        SelectPalette(hDC, hOldPal, TRUE);
        RealizePalette(hDC);
        ::ReleaseDC(NULL, hDC);
    }
    //创建位图文件   
    fh=CreateFile(lpFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (fh==INVALID_HANDLE_VALUE)
        return FALSE;
    // 设置位图文件头
    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;  
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
    // 写入位图文件头
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    // 写入位图文件其余内容
    WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize , &dwWritten, NULL);
    //清除   
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//计算平方差的函数
static int PFC(int color1, int color2)
{
	int x, y, z;
	x = (color1 & 0xf) - (color2 & 0xf);
	y = ((color1>>4) & 0xf) - ((color2>>4) & 0xf);
	z = ((color1>>8) & 0xf) - ((color2>>8) & 0xf);
	return (x*x + y*y + z*z);
};

//直接插入排序
static int Sort1(int *src, int *attach, int n)
{
	int cur, cur1;
	int i = 0, j = 0, k = 0;
	for (i = 1; i < n; i++)
	{
		cur = src[i];
		cur1 = attach[i];
		for (j = i - 1; j >= 0; j--)
		{
			if (cur > src[j])
			{
				src[j+1]    = src[j];
				attach[j+1] = attach[j];
			}
			else
			{
				break;
			}
		}
		src[j+1]  = cur;
		attach[j+1] = cur1;
	}
	return 0;
}

//快速排序
static int Sort2(int *src, int *attach, int n)
{
	if (n <= 12)
	return Sort1(src, attach, n);
	int low = 1, high = n - 1;
	int tmp;
	while (low <= high)
	{
		while (src[low] >= src[0])
		{
			if (++low > n - 1)
				break;
		}
		while (src[high] < src[0])
		{
			if (--high < 1)
				break;
		}
		if (low > high)
			break;
		{
		tmp = src[low];
		src[low] = src[high];
		src[high] = tmp;
		tmp = attach[low];
		attach[low] = attach[high];
		attach[high] = tmp;
		}
		low++;
		high--;
	}
	{
		tmp = src[low - 1];
		src[low - 1] = src[0];
		src[0] = tmp;
		tmp = attach[low - 1];
		attach[low - 1] = attach[0];
		attach[0] = tmp;
	}
	if (low > 1)
		Sort2(src, attach, low - 1);
	if (low < n)
		Sort2(&src[low], &attach[low], n - low);
	return 0;
}

//将24bit的象素颜色数据转换为256色图的图像数据(即索引值)
static int Transfer(WORD *color24bit, int len, BYTE *Index, RGBQUAD *mainColor)
{
	int usedTimes[4096] = {0};
	int miniColor[4096];
	int i;
	for (i=0; i<4096; i++)
		miniColor[i] = i;
	i = 0;
	for (i=0; i<len; i++)
	{
		cntt_assert(color24bit[i] < 4096);
		usedTimes[color24bit[i]]++;
	}
	int numberOfColors = 0;
	for (i=0; i<4096; i++)
	{
		if (usedTimes[i] > 0)
			numberOfColors++;
	}
	//对usedTimes进行排序，排序过程中minColor数组(保存了颜色值)也作与useTimes
	//数组相似的交换
	Sort2(usedTimes, miniColor, 4096);
	//usedTimes数组中是各颜色使用频率，从高到低排列，显然第numberOfColor个之后的都为0
	//miniColor数组中是相应的颜色数据</font>
	//将前256个颜色数据保存到256色位图的调色盘中
	for (i=0; i < 256; i++)
	{
		mainColor[i].rgbBlue = (BYTE)((miniColor[i]>>8)<<4);
		mainColor[i].rgbGreen = (BYTE)(((miniColor[i]>>4) & 0xf)<<4);
		mainColor[i].rgbRed = (BYTE)((miniColor[i] & 0xf)<<4);
		mainColor[i].rgbReserved = 0;
	}
	int *colorIndex = usedTimes;//用原来的useTimes数组来保存索引值
	memset(colorIndex, 0, sizeof(int) * 4096);
	if (numberOfColors <= 256)
	{
		for (i=0; i<numberOfColors; i++)
			colorIndex[miniColor[i]] = i;
	}
	else//为第256之后的颜色在前256种颜色中找一个最接近的
	{
		for (i = 0; i < 256; i++)
			colorIndex[miniColor[i]] = i;
		int index, tmp, tmp1;
		for (i=256; i<numberOfColors; i++)
		{
			tmp = PFC(miniColor[0], miniColor[i]);
			index = 0;
			for (int j=1; j<256; j++)
			{
				tmp1 = PFC(miniColor[j], miniColor[i]);
				if (tmp > tmp1)
				{
				tmp = tmp1;
				index = j;
				}
			}
			colorIndex[miniColor[i]] = index;
		}
	}
	//记录各点颜色数据的索引值，即256色位图的颜色数据
	for (i=0; i<len; i++)
	{
		cntt_assert(colorIndex[color24bit[i]] < 256);
		Index[i] = colorIndex[color24bit[i]];
	}
	return 1;
}

void SaveToFile(LPBITMAPINFOHEADER pInfoHeader, RGBQUAD *pPalette, PBYTE pData, LPCSTR szFileName)
{
    BITMAPFILEHEADER FileHeader;
    int nPaletteLen = 0;

    if (pInfoHeader->biBitCount <= 8)
        nPaletteLen = (1 << pInfoHeader->biBitCount)*sizeof(RGBQUAD);

    //进行文件头设置  
    FileHeader.bfType = ((WORD)('M' << 8) | 'B'); //"BM"   
    FileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nPaletteLen + pInfoHeader->biSizeImage;    
    FileHeader.bfReserved1 = 0;     
    FileHeader.bfReserved2 = 0;     
    FileHeader.bfOffBits = (DWORD)(sizeof(BITMAPFILEHEADER) + pInfoHeader->biSize + nPaletteLen);
    
    //进行位图写入    
    FILE *bmpFile = NULL;     
    bmpFile = fopen(szFileName, "wb"); //szFile为文件名     
    if (bmpFile != NULL)     
    {        
        fwrite(&FileHeader, 1, sizeof(BITMAPFILEHEADER), bmpFile); //位图文件头         
        fwrite(pInfoHeader, 1, sizeof(BITMAPINFOHEADER), bmpFile); //位图信息头
        
        if (pPalette && nPaletteLen > 0)
        {
            fwrite(pPalette, 1, nPaletteLen, bmpFile);    //颜色表         
        }

        fwrite(pData, 1, pInfoHeader->biSizeImage, bmpFile); //位图示?    
    }    
    fclose(bmpFile); 
}

static int Convert24to8(LPBITMAPINFOHEADER pInfoHeader, PBYTE pData, RGBQUAD *pMainColor, PBYTE pIndex)
{
	long nData = pInfoHeader->biSizeImage;
	/************  24bit到256色的颜色数据转换  *****************/
	int nPad = pInfoHeader->biWidth % 4;
	int w = pInfoHeader->biWidth;
	int h = pInfoHeader->biHeight;
	if (nPad != 0)
	{
        // 去掉数据区中填充的0
		nPad = 4 - nPad;
		BYTE* pNewData = new BYTE[w * h * 3];
		for (int i = 0; i < h; i++)
		{
			memcpy(pNewData + i * w * 3, pData + i * (w * 3 + nPad), w * 3);
		}
        memcpy(pData, pNewData, w * h * 3);
		delete [] pNewData;
	}

	long nNewData = w*h;
	memset(pMainColor, 0, sizeof(RGBQUAD)*256);
	WORD* shortColor = new WORD[nNewData];
	int iRed, iGreen, iBlue;
	for (int i = 0; i < nNewData; i++)
	{
        //取RGB颜色的高4位
		iRed    = pData[i*3]>>4;
		iGreen    = pData[i*3+1]>>4;
		iBlue    = pData[i*3+2]>>4;
		shortColor[i] = (iRed<<8) + (iGreen<<4) + iBlue;
	}

	//调用转换函数
	Transfer(shortColor, nNewData, pIndex, pMainColor);
	//256色位图的调色盘数据(保存在mainColor)和图像数据区的数据(保存在Index中)
	delete []shortColor;

	pInfoHeader->biBitCount = 8;          //颜色位数改为8
	pInfoHeader->biSizeImage = nData/3;   //  

    return 0;
}

static void Convert32to24(LPBITMAPINFOHEADER pInfoHeader, PBYTE pData)
{
    //颜色空间转换，如果为32位位图，转换为24
    DWORD dwSize24 = 0;
    DWORD dwSize32 = pInfoHeader->biSizeImage;
    dwSize24 = (dwSize32*3)/4;   //RGB32与RGB24的像素点空间差一个字节
    BYTE* pImg24 = new BYTE[dwSize24]; //存放RGB24存储空间
    BYTE* pImg24Temp = pImg24; //设定临时指针，后续进行数据管理
    BYTE* pImg32 = pData; //跳过文件头，定位到数据部分
    bool  isSize24 = false;
    int   nPaletteSize = 0;

    //每字节数行4对齐
    int biNewWidth = pInfoHeader->biWidth - pInfoHeader->biWidth%4;
    
    if (pInfoHeader->biBitCount == 32)
    {
        isSize24 = true;
        
        for (int j = 0; j < pInfoHeader->biHeight; j++)
        {
            for (int i = 0; i < pInfoHeader->biWidth; i++)
            {
                if (i < biNewWidth)
                {
                    unsigned char r = *(pImg32++);
                    unsigned char g = *(pImg32++);
                    unsigned char b = *(pImg32++);
                    pImg32++;    //ignore alpha 
                    *(pImg24++) = r;
                    *(pImg24++) = g;
                    *(pImg24++) = b;
                }
                else
                {
                    pImg32 += 4;
                }
            }
        }
        
        pInfoHeader->biBitCount = 24;
        pInfoHeader->biSizeImage = biNewWidth*pInfoHeader->biHeight*3;
        pInfoHeader->biWidth = biNewWidth;
    }
    
    if (pInfoHeader->biBitCount <= 8)
        nPaletteSize = (1 << pInfoHeader->biBitCount)*sizeof(RGBQUAD);

    memcpy(pData, pImg24Temp, pInfoHeader->biSizeImage);
    
    delete pImg24Temp; 
}

//函数2 : SaveBitmapToFile 将图片存成文件
BOOL WBS_SaveBitmap8ToFile(HBITMAP hBitmap, LPCSTR lpFileName)  //hBitmap 为刚才的屏幕位图句柄
{
    //lpFileName 为位图文件名
    HDC hDC;   
    //设备描述表
    int iBits;   
    //当前显示分辨率下每个像素所占字节数
    WORD wBitCount;   
    //位图中每个像素所占字节数
    //定义调色板大小， 位图中像素字节大小 ， 位图文件大小 ， 写入文件字节数
    DWORD dwBmBitsSize;
    BITMAP Bitmap;

    //位图文件头结构
    BITMAPINFOHEADER InfoHeader;   
    //位图信息头结构
    LPBITMAPINFOHEADER lpbi;   
    //指向位图信息头结构
    HANDLE hDib, hPal;
    HPALETTE hOldPal=NULL;
    RGBQUAD maincolor[256];
    PBYTE pIndex = NULL;

    //计算位图文件每个像素所占字节数
    hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 32;

    //设置位图信息头结构
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;

    InfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    InfoHeader.biWidth = Bitmap.bmWidth;
    InfoHeader.biHeight = Bitmap.bmHeight;
    InfoHeader.biPlanes = 1;
    InfoHeader.biBitCount = wBitCount;
    InfoHeader.biCompression = BI_RGB;
    InfoHeader.biSizeImage = dwBmBitsSize;
    InfoHeader.biXPelsPerMeter = 0;
    InfoHeader.biYPelsPerMeter = 0;
    InfoHeader.biClrUsed = 0;
    InfoHeader.biClrImportant = 0;
    
    hDib = GlobalAlloc(GHND, dwBmBitsSize);
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    // 处理调色板   
    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = ::GetDC(NULL);
        hOldPal=SelectPalette(hDC,(HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }

    // 获取该调色板下新的像素值
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi, (BITMAPINFO*)&InfoHeader, DIB_RGB_COLORS);
    //恢复调色板   
    if (hOldPal)
    {
        SelectPalette(hDC, hOldPal, TRUE);
        RealizePalette(hDC);
        ::ReleaseDC(NULL, hDC);
    }

    if (InfoHeader.biBitCount == 32)
        Convert32to24(&InfoHeader, (PBYTE)lpbi);
    
    if (InfoHeader.biBitCount == 24)
    {
        pIndex = new BYTE[Bitmap.bmWidth*Bitmap.bmHeight];
        Convert24to8(&InfoHeader, (PBYTE)lpbi, maincolor, pIndex);
        SaveToFile(&InfoHeader, maincolor, pIndex, lpFileName);
        delete pIndex;
    }
    else
    {
        SaveToFile(&InfoHeader, NULL, (PBYTE)lpbi, lpFileName);
    }

    GlobalUnlock(hDib);
    GlobalFree(hDib);

    return TRUE;
}