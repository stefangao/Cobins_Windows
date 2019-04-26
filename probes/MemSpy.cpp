#include "MemSpy.h"
#include "base/cobApiHook.h"

NS_COB_BEGIN

static ApiHook m_Send_Hook;

MemSpy::MemSpy(const std::string& name)
  : Prober(name)
{
    addEvtRequestProc("readValue", COB_BIND2(MemSpy::readValue, this));

    addEvtNotifyProc("hello", COB_BIND1(MemSpy::hello, this));
}

void MemSpy::readValue(lianli::EvtStream& evtData, lianli::EvtStream& retData)
{
    unsigned long address;
    evtData >> address;
    COBLOG("addr=%x\n", address);
    retData << 23;
}

static int WINAPI hook_send(SOCKET s, const char FAR *buf, int len, int flags)
{
    int result = -1;

    COBLOG("hook_send: s=%x, buf=%x, len=%x, flags=%x\n", s, buf, len, flags);

    m_Send_Hook.HookPause();
    result = send(s, buf, len, flags);
    m_Send_Hook.HookResume();

    /*
    void *func = m_Send_Hook.GetOriginFunc();
    if (func != NULL)
    {
        __asm
        {
            push flags
            push len
            push buf
            push s
            call func                     //call orginal api
            mov  result, eax
        }
    }*/

    return result;
}

void MemSpy::hello(lianli::EvtStream& evtData)
{
    std::string words;
    evtData >> words;
    COBLOG("words=%s\n", words.c_str());

    //MessageBox(getBin()->getMainWnd(), words.c_str(), "Cobins", MB_OK);
    //SetWindowText(getBin()->getMainWnd(), words.c_str());

    m_Send_Hook.HookFunc("Ws2_32.dll", "send", (PROC)GetMemberFuncAddr(hook_send), 10);
}

/////////////////////////////////////////////////////////////////////

int IMemSpy::readValue(unsigned long address)
{
    lianli::EvtStream evtData, retData;

    evtData << address;
    request("readValue", evtData, retData);

    int value;
    retData >> value;

    return value;
}

void IMemSpy::hello()
{
    lianli::EvtStream evtData;

    evtData << "Hello World!";
    notify("hello", evtData);
}


NS_COB_END
