#ifndef __COB_ROBOT_H__
#define __COB_ROBOT_H__

#include "llfsm/lianli.h"
#include "cobMacros.h"
#include "cobProbe.h"

NS_COB_BEGIN

class Robot : public lianli::FSM
{
public:
	Robot();
	Robot& create(const std::string& name, lianli::Context& context = lianli::Context::DEFAULT);

    Probe* getProbe(const std::string& probeName);
    Robot* getRobot(const std::string& robotName);

protected:


private:

};

NS_COB_END

#endif
