#include "stdafx.h"
#include "AppDelegate.h"
#include "probes/MemSpy.h"
#include "robots/AutoRefresh.h"

AppDelegate::AppDelegate()
{

}

void AppDelegate::onCreate(const lianli::Context& context)
{
    mBin.create(m_hMainWnd);

    IMemSpy* memSpy = new IMemSpy("memspy");
    mBin.install(*memSpy);
}

void AppDelegate::onStart()
{

}

bool AppDelegate::onEventProc(const std::string& evtName, lianli::EvtStream& evtData)
{
    if (evtName == "BindEvt")
    {
        HWND hTargetWnd;
        evtData >> (DWORD&)hTargetWnd;
        if (hTargetWnd)
        {
            mBin.bind(hTargetWnd);
        }
    }
    else if (evtName == "UnbindEvt")
    {
        
        
    }
    else if (evtName == "PostDataEvt")
    {
        auto memSpy = (IMemSpy*)mBin.getProbe("memspy");
        if (memSpy)
        {
            memSpy->hello();
        }
    }
	else if (evtName == "SendDataEvt")
	{
        lianli::EvtStream retData;
        //mBin.RpcSend("testEngine", "TestEvt1", evtData, retData);

        auto memSpy = (IMemSpy*)mBin.getProbe("memspy");
        if (memSpy)
        {
            int value = memSpy->readValue(0x12345678);
            COBLOG("readValue: value=%d\n", value);
        }
	}


    return true;
}

void AppDelegate::onStop()
{
	mBin.destroy();
}

void AppDelegate::onDestroy(const lianli::Context& context)
{

}

BEGIN_TRANS_TABLE(AppDelegate, FSM)
    TRANS_ENTRY(S_ROOT, "BindEvt", S_NONE)
    TRANS_ENTRY(S_ROOT, "UnbindtEvt", S_NONE)
	TRANS_ENTRY(S_ROOT, "SendDataEvt", S_NONE)
    TRANS_ENTRY(S_ROOT, "PostDataEvt", S_NONE)
END_TRANS_TABLE()
