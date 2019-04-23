#include "AutoRefresh.h"
USING_NS_LL;

NS_COB_BEGIN

AutoRefresh::AutoRefresh()
{

}

AutoRefresh::~AutoRefresh()
{

}

void AutoRefresh::Daemon::onEnter()
{
    State::onEnter();

}

void AutoRefresh::Daemon::onExit()
{
	State::onExit();
}

bool AutoRefresh::Daemon::onEventProc(const std::string& evtName, EvtStream& evtData)
{
	State::onEventProc(evtName, evtData);
    return true;
}

BEGIN_STATE_TABLE(AutoRefresh)
    STATE_ENTRY(DAEMON, Daemon, S_ROOT,    SFL_ACTIVE)
END_STATE_TABLE()

BEGIN_TRANS_TABLE(AutoRefresh, Robot)
    TRANS_ENTRY(DAEMON, "TestEvt1", S_NONE)
END_TRANS_TABLE()

NS_COB_END
