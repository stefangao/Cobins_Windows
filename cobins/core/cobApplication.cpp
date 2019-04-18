#include "cobApplication.h"

NS_COB_BEGIN

Application::Application()
{
    m_hMainWnd = NULL;
}

Application& Application::create(HWND hWnd, const std::string& name, lianli::Context& context)
{
	m_hMainWnd = hWnd;

    return (Application&)lianli::FSM::create(name, context);
}

NS_COB_END
