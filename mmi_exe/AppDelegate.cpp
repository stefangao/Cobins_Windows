#include "stdafx.h"
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
    mBin.create(NULL, 1234);
}

void AppDelegate::onStart()
{

}

bool AppDelegate::onEventProc(const std::string& evtName, lianli::EvtData& evtData)
{
    if (evtName == "ConnectEvt")
    {
        
        
    }
    else if (evtName == "ConnectEvt")
    {
        
        
    }

    return true;
}

void AppDelegate::onStop()
{

}

void AppDelegate::onDestroy(const lianli::Context& context)
{

}

BEGIN_TRANS_TABLE(AppDelegate, FSM)
    TRANS_ENTRY(S_ROOT, "ConnectEvt", S_NONE)
    TRANS_ENTRY(S_ROOT, "DisconnectEvt", S_NONE)
END_TRANS_TABLE()
