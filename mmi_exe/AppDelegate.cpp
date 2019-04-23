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

    //create all necessary probes
    IMemSpy* memSpy = new IMemSpy("memspy");
    mBin.addProbe(*memSpy);

    //create all needed robots
    auto autoRefresh = new AutoRefresh();
    autoRefresh->create("AutoRefresh");
    mBin.addRobot(*autoRefresh);
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
    else if (evtName == "StartRobotEvt")
	{
        std::string robotName;
        evtData >> robotName;
        auto robot = mBin.getRobot(robotName);
        if (robot)
        {
            robot->start();
        }
	}
    else if (evtName == "StopRobotEvt")
	{
        std::string robotName;
        evtData >> robotName;
        auto robot = mBin.getRobot(robotName);
        if (robot)
        {
            robot->stop();
        }
	}
    else if (evtName == "PostRobotEvt")
	{
        std::string robotName;
        evtData >> robotName;
        auto robot = mBin.getRobot(robotName);
        if (robot)
        {
            std::string embEvtName;
            EvtStream embEvtData;
            evtData >> embEvtName >> embEvtData;
            robot->postEvent(embEvtName, embEvtData);
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
    TRANS_ENTRY(S_ROOT, "StartRobotEvt", S_NONE)
    TRANS_ENTRY(S_ROOT, "StopRobotEvt", S_NONE)
    TRANS_ENTRY(S_ROOT, "PostRobotEvt", S_NONE)
END_TRANS_TABLE()
