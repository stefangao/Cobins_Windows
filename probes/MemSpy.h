#ifndef __MEM_SPY_H__
#define __MEM_SPY_H__

#include "cobins.h"

NS_COB_BEGIN

class MemSpy : public Prober
{
public:
    MemSpy(const std::string& name);

protected:
    virtual void onRequest(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData) override;

    void readValue(const lianli::EvtData& evtData, lianli::EvtData& retData);

private:
};

class IMemSpy : public IProbe
{
public:
    IMemSpy(const std::string& name);

    int readValue(void* address);

protected:


private:
};

NS_COB_END

#endif
