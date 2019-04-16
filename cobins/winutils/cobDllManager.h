#ifndef __DLL_MANAGER__
#define __DLL_MANAGER__

#include <windows.h>
#include <string>
#include "cobMacros.h"
#include "base/cobUtils.h"

NS_COB_BEGIN

class DllManager
{
private:
    HHOOK  m_hHook;

public:
    DllManager();
    virtual ~DllManager() {};

public:
    BOOL inject(HWND hTargetWnd, const std::string& dllPath);
    BOOL eject();
    BOOL isInjected() {return m_hHook != NULL;};
};

NS_COB_END

#endif
