#include "AutoRefresh.h"
#include "probes/MemSpy.h"
USING_NS_LL;

NS_COB_BEGIN

AutoRefresh::AutoRefresh()
{

}

AutoRefresh::~AutoRefresh()
{

}

void AutoRefresh::onStart()
{
    mMemSpy = (IMemSpy*)getBin()->getProbe("memspy");
    COBASSERT(mMemSpy, "MemSpy is not found");
}

void AutoRefresh::Daemon::onEnter()
{
    State::onEnter();

    //startHeartBeat(20);

    self()->mMemSpy->hello();
}

void AutoRefresh::Daemon::onExit()
{
	State::onExit();

    stopHeartBeat();

    int value = self()->mMemSpy->readValue(0x12345678);
    COBLOG("readValue: value=%d\n", value);
 }

bool AutoRefresh::Daemon::onEventProc(const std::string& evtName, EvtStream& evtData)
{
	if (evtName == "TestEvt1")
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
	}
	else if (evtName == "PressKeyEvt")
	{
		char key;
		evtData >> key;
		self()->mMemSpy->pressKey(key);
	}
    return true;
}

void AutoRefresh::Daemon::onHeartBeat()
{
    auto bin = self()->getBin();
    auto memSpy = (IMemSpy*)bin->getProbe("memspy");
    if (memSpy)
    {
        int value = memSpy->readValue(0x12345678);
        COBLOG("AutoRefresh: readValue: value=%d\n", value);
    }
}

BEGIN_STATE_TABLE(AutoRefresh)
    STATE_ENTRY(DAEMON, Daemon, S_ROOT,    SFL_ACTIVE)
END_STATE_TABLE()

BEGIN_TRANS_TABLE(AutoRefresh, Robot)
    TRANS_ENTRY(DAEMON, "TestEvt1", S_NONE)
	TRANS_ENTRY(DAEMON, "PressKeyEvt", S_NONE)
END_TRANS_TABLE()

NS_COB_END
