#ifndef __WNDBASE_H__
#define __WNDBASE_H__

#include <windows.h>
#include <stdio.h>
/*
#include "cntt_timer.h"
#include <libxml\parser.h> 
#include <libxml\xpath.h>*/

/** @defgroup groupwndbase wndbase
 *  This is a module implementing utilities related to windows.
\n\n<b>Header:</b> Declared in wndbase.h, wndproc.h or wndshm.h.\n
    <b>Import Library:</b> Use wndbase.lib.\n
 *  @{
 */

#define WBS_KEY_SHIFT    0x0001
#define WBS_KEY_CTL      0x0002
#define WBS_KEY_ALT      0x0004

#define WBS_RED(x)       ((x) & 0xff)
#define WBS_GREEN(x)     (((x)>>8) & 0xff)
#define WBS_BLUE(x)      (((x)>>16) & 0xff)

#define min2(x, y)       ((x) < (y) ? (x):(y))
#define max2(x, y)       ((x) > (y) ? (x):(y))
#define min3(x, y, z)    min2(min2(x, y), z)
#define max3(x, y, z)    max2(max2(x, y), z)

typedef struct 
{
    int y;
	int u;
	int v;
}YUV_t;

typedef struct 
{
    int h;
	int s;
	int v;
}HSV_t;

typedef BOOL (*MatchFunc_T)(unsigned long);

typedef BOOL (*WaitMFunc_T)(DWORD, DWORD, DWORD);


typedef struct
{
    MatchFunc_T matchfunc;
	unsigned long param;
	int result;
	BOOL timeout;

	unsigned long threadid;
	void *hThread;
    void *hEvent;

}GUIWaitThread_T;

//add here after removing winshr
typedef enum
{
    WSH_MSG_MIN = WM_USER + 800,
    WSH_MSG_KEY,
    WSH_MSG_MOUSE,
    WSH_MSG_CURSOR,
    WSH_MSG_PUSHDATA,
    WSH_MSG_PULLDATA,

    WSH_MSG_MAX,

}WSH_CMD;

typedef void* CNTT_HANDLE;


BOOL WBS_GetScreenPixel( POINT pt, COLORREF& clr );
BOOL WBS_GetWindowPixel( HWND hWnd, POINT pt, COLORREF& clr);
BOOL WBS_RGB2YUV(COLORREF rgb, YUV_t &yuv);
BOOL WBS_YUV2RGB(YUV_t yuv, COLORREF& rgb);
BOOL WBS_RGB2HSV(COLORREF rgb, HSV_t &hsv);
BOOL WBS_HSV2RGB(HSV_t hsv, COLORREF& rgb );
HWND WBS_GetWindowFromPoint(HWND hwndParent, POINT point, UINT uFlags=CWP_SKIPINVISIBLE);
void WBS_LButtonDown();
void WBS_LButtonUp();
void WBS_MouseMove(DWORD dx, DWORD dy);
void WBS_LButtonDown(HWND hWnd, int xPos, int yPos);
void WBS_LButtonUp(HWND hWnd, int xPos, int yPos);

BOOL WBS_SetCursorPos(int x, int y);
BOOL WBS_GetCursorPos(LPPOINT lpPoint);
BOOL WBS_SetWinCursorPos(int x, int y, HWND hWnd);
BOOL WBS_GetCursorWinPos(LPPOINT lpPoint);
COLORREF WBS_SetScreenPixel( POINT pt, COLORREF clr );
void WBS_RButtonDown();
void WBS_RButtonUp();
void WBS_RButtonDownMove(POINT pt, int dx, int dy);
void WBS_MouseLClick();
void WBS_MouseRClick();
void WBS_MouseLDClick();
void WBS_MouseRDClick();
BOOL WBS_MakeWndMark(HWND hWnd, POINT pt, int width);
BOOL WBS_MakeScreenMark(POINT pt, int width);

void WBS_AlertPlay(LPCTSTR lpszWaveFileName);
void WBS_AlertStop();
BOOL WBS_IsAlertPlaying();
void WBS_Alert(LPCSTR lpAudioPath, BOOL bRestart = FALSE);

void WBS_PressKey(BYTE key, unsigned long combinkey = 0);

BOOL WBS_GUIWaitTrue(DWORD dwMilliseconds, MatchFunc_T matchfunc = NULL, unsigned long param = 0);
BOOL WBS_GUIWaitFalse(DWORD dwMilliseconds, MatchFunc_T matchfunc = NULL, unsigned long param = 0);
BOOL WBS_GUIWait(DWORD dwMilliseconds);
BOOL WBS_WaitMessage(DWORD dwMilliseconds, UINT uMessage, LPMSG lpMsg = NULL);

BOOL WBS_GUIBlockTrue(DWORD dwMilliseconds, MatchFunc_T matchfunc = NULL, unsigned long param = 0);
BOOL WBS_GUIBlockFalse(DWORD dwMilliseconds, MatchFunc_T matchfunc = NULL, unsigned long param = 0);
BOOL WBS_GUIBlock(DWORD dwMilliseconds);

void WBS_WinShutdown(UINT ShutdownFlag);

BOOL WBS_PreTranslateMessage(MSG &msg);

HWND WBS_FindWindow(HWND hwndParent, LPCSTR lpTitle, LPCSTR lpClass = NULL);

int   WBS_ReadStream(HANDLE hProcess, const void *lpBaseAddress, BYTE *pBuf, int nBufLen);
BOOL  WBS_WriteStream(HANDLE hProcess, void *lpBaseAddress, BYTE *pData, DWORD nDataLen);
DWORD WBS_ReadLong(HANDLE hProcess, const void *lpBaseAddress);
float WBS_ReadFloat(HANDLE hProcess, const void *lpBaseAddress);

//xmlXPathObjectPtr WBS_GetXPathNodeset(xmlDocPtr doc, const xmlChar *xpath);

HBITMAP WBS_CopyWindowToBitmap(HWND hWnd, LPRECT lpRect); //lpRect 代表选定区域
HBITMAP WBS_CopyScreenToBitmap(LPRECT lpRect); //lpRect 代表选定区域
BOOL    WBS_SaveBitmapToFile(HBITMAP hBitmap, LPCSTR lpFileName);   //hBitmap 为刚才的屏幕位图句柄
BOOL    WBS_SaveBitmap8ToFile(HBITMAP hBitmap, LPCSTR lpFileName);  //hBitmap 为刚才的屏幕位图句柄


/** @} */ // end of groupwndbase

#endif
