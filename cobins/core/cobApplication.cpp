#include "cobApplication.h"

NS_COB_BEGIN

Application::Application()
{
    m_hMainWnd = NULL;
}

Application& Application::create(HWND hWnd, const std::string& name, lianli::Context& context)
{
    lianli::FSM::create(name, context);

    m_hMainWnd = hWnd;
    return *this;
}

NS_COB_END
