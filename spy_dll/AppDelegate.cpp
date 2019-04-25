#include "AppDelegate.h"
#include "probes/MemSpy.h"

AppDelegate::AppDelegate()
{

}

void AppDelegate::onCreate(const lianli::Context& context)
{
    mBin.create(m_hMainWnd);

    //auto prober1 = new Prober1("prober1");
    //mBin.install(*prober1);

    auto memSpy = new MemSpy("memspy");
    mBin.addProbe(*memSpy);
}

void AppDelegate::onStart()
{
    mBin.pipeListen((DWORD)m_hMainWnd);
}

void AppDelegate::onStop()
{

}

void AppDelegate::onDestroy(const lianli::Context& context)
{
    mBin.destroy();
}
