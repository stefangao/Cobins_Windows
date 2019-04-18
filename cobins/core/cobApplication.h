#ifndef __COB_APPLICATION_H__
#define __COB_APPLICATION_H__

#include <windows.h>
#include "llfsm/lianli.h"
#include "cobMacros.h"
#include "cobBin.h"

NS_COB_BEGIN

class Application : public lianli::FSM
{
public:
    Application();
    Application& create(HWND hWnd, const std::string& name, lianli::Context& context = lianli::Context::DEFAULT);

protected:
    HWND m_hMainWnd;

};

NS_COB_END

#endif
