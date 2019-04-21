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
	else if (evtName == "SendDataEvt")
	{
		std::string text;
		//evtData >> text;
		//mBin.RpcSendData((PBYTE)text.c_str(), text.size() + 1);

        lianli::EvtData resultBuf;
        int len;
        //mBin.RpcPost("testEngine", "TestEvt1", (PBYTE)text.c_str(), text.size() + 1);
        mBin.RpcSend("testEngine", "TestEvt1", evtData, resultBuf);
	}
    else if (evtName == "PostDataEvt")
    {
        mBin.RpcPost("testEngine", "TestEvt1", evtData);
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
