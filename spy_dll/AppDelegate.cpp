#include "AppDelegate.h"
#include "probes/MemSpy.h"

#pragma comment(lib,"Ws2_32.lib ")

AppDelegate::AppDelegate()
{
}

void AppDelegate::onCreate(const lianli::Context& context)
{
    mBin.create(m_hMainWnd);

    //auto prober1 = new Prober1("prober1");
    //mBin.install(*prober1);

    auto memSpy = new MemSpy();
    memSpy->create("memspy");
    mBin.addProbe(*memSpy);
}

void AppDelegate::onStart()
{
    mBin.pipeListen((DWORD)m_hMainWnd);

    auto probe = (Prober*)mBin.getProbe("memspy");
    if (probe)
        probe->start();
}

void AppDelegate::onStop()
{
    auto probe = (Prober*)mBin.getProbe("memspy");
    if (probe)
        probe->stop();
}

void AppDelegate::onDestroy(const lianli::Context& context)
{
    mBin.destroy();
}
