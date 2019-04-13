#include "AppDelegate.h"

AppDelegate::AppDelegate()
{

}

bool AppDelegate::create(const std::string& name, Context& context)
{
    if (!Application::create(name, context))
        return false;

    return true;
}

void AppDelegate::onCreate(const lianli::Context& context)
{
    mBin.create(1234);
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
