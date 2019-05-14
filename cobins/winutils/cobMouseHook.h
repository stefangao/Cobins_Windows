#ifndef __MOUSEHOOK_H__
#define __MOUSEHOOK_H__

#define   _WIN32_WINNT   0x0500 

#include "windows.h"
#include "cobMacros.h"

NS_COB_BEGIN
   
/** @defgroup groupmousehook mousehook
 *  This is a module for hooking mouse.
\n\n<b>Header:</b> Declared in mousehook.h.\n
    <b>Import Library:</b> Use mousehook.lib.\n
 *  @{
 */
BOOL Mouse_StartHook(HWND hHostWnd);
BOOL Mouse_StopHook();
/** @} */ // end of groupmousehook

NS_COB_END

#endif
