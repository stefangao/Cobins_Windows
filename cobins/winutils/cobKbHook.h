#ifndef __COB_KB_HOOK_H__
#define __COB_KB_HOOK_H__

#include "windows.h"
#include "cobMacros.h"

NS_COB_BEGIN
   
BOOL Kb_StartHook(HWND hHostWnd);
BOOL Kb_StopHook(HWND hHostWnd);

NS_COB_END

#endif
