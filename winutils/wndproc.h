#ifndef __WNDPROCESS_H__
#define __WNDPROCESS_H__

#include <windows.h>

/** @addtogroup groupwndbase
 *  @{
 */
DWORD WBS_GetProcessId(const char* szExeFile);
DWORD WBS_GetProcessId2(LPCSTR szExeFilePart1, LPCSTR szExeFilePart2);
DWORD WBS_GetThreadId(DWORD OwnerProcessId, LPCSTR lpWndTitle, HWND &hMainWnd);
int   WBS_GetProcessWindows(LPCSTR szExeFile, LPCSTR lpWndTitle, DWORD wndArray[], int nArraySize);
/** @} */ // end of groupwndbase

#endif
