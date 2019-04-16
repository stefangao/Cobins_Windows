#ifndef __COB_INJECT_DLL__
#define __COB_INJECT_DLL__

#include <windows.h>
#include "cobMacros.h"

NS_COB_BEGIN

class DllManager
{
private:
    HHOOK  m_hHook;

public:
    DllManager();
    virtual ~DllManager() {};

public:
    BOOL Inject(HWND hTargetWnd, const std::string& dllPath);
    BOOL Eject();
    BOOL isInjected() {return m_hHook != NULL;};
};

NS_COB_END

#endif
