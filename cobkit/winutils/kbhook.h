#ifndef __KBHOOK_H__
#define __KBHOOK_H__

//#define   _WIN32_WINNT   0x0500 

#include "windows.h"
   
/** @defgroup groupkbhook kbhook
 *  This is a module for hooking keyboard.
\n\n<b>Header:</b> Declared in kbhook.h.\n
    <b>Import Library:</b> Use kbhook.lib.\n
 *  @{
 */
BOOL KB_StartHook(HWND hHostWnd);
BOOL KB_StopHook(HWND hHostWnd);

/** @} */ // end of groupkbhook

#endif
