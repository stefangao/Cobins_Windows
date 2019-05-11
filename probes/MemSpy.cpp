#include "MemSpy.h"
#include "base/cobApiHook.h"
USING_NS_LL;

NS_COB_BEGIN

static ApiHook m_Send_Hook;

void MemSpy::onCreate(const lianli::Context& context)
{
    addEvtRequestProc("readValue", COB_BIND2(MemSpy::readValue, this));

    addEvtNotifyProc("hello", COB_BIND1(MemSpy::on_hello, this));
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

void MemSpy::on_hello(lianli::EvtStream& evtData)
{
    std::string words;
    evtData >> words;
    COBLOG("on_hello: words=%s\n", words.c_str());

    //MessageBox(getBin()->getMainWnd(), words.c_str(), "Cobins", MB_OK);
    //SetWindowText(getBin()->getMainWnd(), words.c_str());

    //m_Send_Hook.HookFunc("Ws2_32.dll", "send", (PROC)GetMemberFuncAddr(hook_send), 10);
}


void MemSpy::Daemon::onEnter()
{
    State::onEnter();

    COBLOG("MemSpy::Daemon::onEnter()\n");

    //startHeartBeat(30);
}

void MemSpy::Daemon::onExit()
{
    State::onExit();

    stopHeartBeat();
}

bool MemSpy::Daemon::onEventProc(const std::string& evtName, EvtStream& evtData)
{
    int value;
    std::string text;

    evtData >> value >> text;

    COBLOG("AutoRefresh: evtName=%s value=%d text=%s\n", evtName.c_str(), value, text.c_str());

    auto bin = self()->getBin();
    auto memSpy = (IMemSpy*)bin->getProbe("memspy");
    if (memSpy)
    {
        int value = memSpy->readValue(0x12345678);
        COBLOG("AutoRefresh: readValue: value=%d\n", value);
    }
    return true;
}

void MemSpy::Daemon::onHeartBeat()
{
    lianli::EvtStream evtData;

    evtData << "I come from embed side!";
    self()->notify("embed_voice", evtData);
}

BEGIN_STATE_TABLE(MemSpy)
STATE_ENTRY(DAEMON, Daemon, S_ROOT, SFL_ACTIVE)
END_STATE_TABLE()

BEGIN_TRANS_TABLE(MemSpy, Prober)
TRANS_ENTRY(DAEMON, "TestEvt1", S_NONE)
END_TRANS_TABLE()

/////////////////////////////////////////////////////////////////////

IMemSpy::IMemSpy(const std::string& name)
    : IProbe(name)
{
    addEvtNotifyProc("embed_voice", COB_BIND1(IMemSpy::on_embed_voice, this));
}

void IMemSpy::on_embed_voice(lianli::EvtStream& evtData)
{
    std::string words;
    evtData >> words;
    COBLOG("on_embed_voice: voice=%s\n", words.c_str());
}

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
