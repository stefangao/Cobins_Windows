#ifndef __AUTO_REFRESH_H__
#define __AUTO_REFRESH_H__

#include "cobins.h"

NS_COB_BEGIN

class AutoRefresh : public Robot
{
public:
    AutoRefresh();
    virtual ~AutoRefresh();

public:
    enum
    {
        DAEMON, TEST1, TEST2, TEST3, TEST4
    };

    class Daemon: public State
    {
    protected:
        virtual void onEnter() override;
        virtual void onExit() override;
        virtual bool onEventProc(const std::string& evtName, lianli::EvtStream& evtData) override;

        DECLARE_STATE_FACTORY(Daemon, AutoRefresh)
    };

    DECLARE_STATE_TABLE()
    DECLARE_TRANS_TABLE()
};

NS_COB_END

#endif
