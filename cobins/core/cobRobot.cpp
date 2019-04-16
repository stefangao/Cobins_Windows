#include "cobRobot.h"

NS_COB_BEGIN

Robot::Robot()
{

}

bool Robot::create(const std::string& name, lianli::Context& context)
{
    if (!lianli::FSM::create(name, context))
        return false;


    return true;
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
