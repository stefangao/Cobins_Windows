#include "AppDelegate.h"
#include "probes/MemSpy.h"

#pragma comment(lib,"Ws2_32.lib ")

AppDelegate::AppDelegate()
    : mBin("embed")
{
}

void AppDelegate::onCreate(void* params)
{
    mBin.create(m_hMainWnd);

    //auto prober1 = new Prober1("prober1");
    //mBin.install(*prober1);

    auto memSpy = new MemSpy();
    memSpy->create("memspy", mBin);
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

void AppDelegate::onDestroy()
{
    mBin.destroy();
}
