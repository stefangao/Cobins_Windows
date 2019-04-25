#include "MemSpy.h"

NS_COB_BEGIN

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

void MemSpy::hello(lianli::EvtStream& evtData)
{
    std::string words;
    evtData >> words;
    COBLOG("words=%s\n", words.c_str());

    //MessageBox(getBin()->getMainWnd(), words.c_str(), "Cobins", MB_OK);
    //SetWindowText(getBin()->getMainWnd(), words.c_str());
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
