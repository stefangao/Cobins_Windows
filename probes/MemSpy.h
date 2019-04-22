#ifndef __MEM_SPY_H__
#define __MEM_SPY_H__

#include "cobins.h"
#include "llfsm/lianli.h"

NS_COB_BEGIN

class MemSpy : public Prober
{
public:
    MemSpy(const std::string& name);

protected:
    void readValue(lianli::EvtStream& evtData, lianli::EvtStream& retData);
    void hello(lianli::EvtStream& evtData);
};

class IMemSpy : public IProbe
{
public:
    IMemSpy(const std::string& name) : IProbe(name) {}
    
    int readValue(unsigned long address);
    void hello();
};

NS_COB_END

#endif
