#include "cobRobot.h"

NS_COB_BEGIN

Robot::Robot()
{

}

Robot& Robot::create(const std::string& name, lianli::Context& context)
{
	lianli::FSM::create(name, context);

    return *this;
}


Probe* Robot::getProbe(const std::string& probeName)
{
    return nullptr;
}

Robot* Robot::getRobot(const std::string& robotName)
{
    return nullptr;
}

NS_COB_END
