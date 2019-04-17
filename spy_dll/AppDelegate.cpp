#include "AppDelegate.h"

AppDelegate::AppDelegate()
{

}

bool AppDelegate::create(HWND hWnd, const std::string& name, lianli::Context& context)
{
    if (!Application::create(name, context))
        return false;

    mhMainWnd = hWnd;
    //mBin.create(hWnd, 1234);
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
