#include "MemSpy.h"

NS_COB_BEGIN

MemSpy::MemSpy(const std::string& name)
  : Prober(name)
{
    mEvtRequestProcMap.addEvtRequestProc("readValue", CC_CALLBACK_2(MemSpy::readValue, this));
}

void MemSpy::readValue(const lianli::EvtData& evtData, lianli::EvtData& retData)
{
    void* address;
    evtData >> address;
    int value = *(int*)address;
    retData << value;
}

/////////////////////////////////////////////////////////////////////

IMemSpy::IMemSpy(const std::string& name)
  : IProbe(name)
{

}

int IMemSpy::readValue(void* address)
{
    EvtData evtData, retData;

    evtData << address;
    request("readValue", evtData, retData);

    int value;
    retData >> value;

    return value;
}


NS_COB_END
