#ifndef __MEM_SPY_H__
#define __MEM_SPY_H__

#include "cobins.h"
#include "llfsm/lianli.h"

NS_COB_BEGIN

class MemSpy : public Prober
{
protected:
    void onCreate(void* params) override;
    bool readValue(lianli::EvtStream& evtData, lianli::EvtStream& retData);
    bool on_hello(lianli::EvtStream& evtData);

public:
    enum {DAEMON};

    class Daemon : public State
    {
    protected:
        virtual void onEnter() override;
        virtual void onExit() override;
        virtual bool onEventProc(const std::string& evtName, lianli::EvtStream& evtData) override;
        virtual void onHeartBeat() override;

        DEFINE_STATE_FACTORY_OF_FSM(Daemon, MemSpy)
    };

    DECLARE_STATE_TABLE()
    DECLARE_TRANS_TABLE()
};

class IMemSpy : public IProbe
{
public:
    IMemSpy(const std::string& name);
    int readValue(unsigned long address);
    void hello();
    bool on_embed_voice(lianli::EvtStream& evtData);

protected:
    void onCreate(void* params);
};

NS_COB_END

#endif
