#include "AppDelegate.h"

AppDelegate::AppDelegate()
{

}

bool AppDelegate::create(HWND hWnd, const std::string& name, Context& context)
{
    if (!Application::create(name, context))
        return false;

    mhMainWnd = hWnd;
    mBin.create(mhMainWnd, 1234);
    return true;
}

void AppDelegate::onCreate(const lianli::Context& context)
{
    
}

void AppDelegate::onStart()
{

}

void AppDelegate::onStop()
{

}

void AppDelegate::onDestroy(const lianli::Context& context)
{
    mBin.destroy();
}

BEGIN_STATE_TABLE(AppDelegate)
    STATE_ENTRY(DAEMON, Daemon, lianli::S_ROOT, lianli::SFL_ACTIVE)
END_STATE_TABLE()

BEGIN_TRANS_TABLE(AppDelegate, FSM)
    TRANS_ENTRY(DAEMON, "UnhookEvt", lianli::S_NONE)
END_TRANS_TABLE()