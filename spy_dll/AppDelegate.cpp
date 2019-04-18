#include "AppDelegate.h"

AppDelegate::AppDelegate()
{

}

void AppDelegate::onCreate(const lianli::Context& context)
{
    mBin.create(m_hMainWnd);
}

void AppDelegate::onStart()
{
    mBin.pipeListen(0x1234);

}

void AppDelegate::onStop()
{

}

void AppDelegate::onDestroy(const lianli::Context& context)
{
    mBin.destroy();
}
