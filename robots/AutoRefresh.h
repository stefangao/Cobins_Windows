#ifndef __AUTO_REFRESH_H__
#define __AUTO_REFRESH_H__

#include "cobins.h"
#include "probes/MemSpy.h"

NS_COB_BEGIN

class AutoRefresh : public Robot
{
public:
    AutoRefresh();
    virtual ~AutoRefresh();

protected:
    virtual void onStart() override;

    IMemSpy* mMemSpy;

public:
    enum
    {
        DAEMON
    };

    class Daemon: public State
    {
    protected:
        virtual void onEnter() override;
        virtual void onExit() override;
        virtual bool onEventProc(const std::string& evtName, lianli::EvtStream& evtData) override;
        virtual void onHeartBeat() override;

        DECLARE_STATE_FACTORY(Daemon, AutoRefresh)
    };

    DECLARE_STATE_TABLE()
    DECLARE_TRANS_TABLE()
};

NS_COB_END

#endif
