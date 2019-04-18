#include "stdafx.h"
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

}

bool AppDelegate::onEventProc(const std::string& evtName, lianli::EvtData& evtData)
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

    return true;
}

void AppDelegate::onStop()
{

}

void AppDelegate::onDestroy(const lianli::Context& context)
{

}

BEGIN_TRANS_TABLE(AppDelegate, FSM)
    TRANS_ENTRY(S_ROOT, "BindEvt", S_NONE)
    TRANS_ENTRY(S_ROOT, "UnbindtEvt", S_NONE)
END_TRANS_TABLE()
