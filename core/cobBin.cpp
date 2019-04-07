#include "cobBin.h"
#include "cobProbe.h"
#include "cobRobot.h"

NS_COB_BEGIN

Bin::Bin()
{

}

bool Bin::create(const std::string& name, Context& context)
{
    if (!lianli::FSM::create(name, context))
        return false;


    return true;
}

bool Bin::connect()
{
	return true;
}

bool Bin::disconnect()
{
	return true;
}

bool Bin::install(Probe& probe)
{
	mProbeMap.insert(std::make_pair(probe.getName(), &probe));
	return true;
}

bool Bin::uninstall(const std::string& probeName)
{
	return true;
}

Probe* Bin::getProbe(const std::string& probeName)
{

	return nullptr;
}

Robot* Bin::getRobot(const std::string& robotName)
{
	return nullptr;
}

NS_COB_END
