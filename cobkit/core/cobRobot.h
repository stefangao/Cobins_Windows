#ifndef __COB_ROBOT_H__
#define __COB_ROBOT_H__

#include "lianli.h"
#include "cobMacros.h"
#include "cobContext.h"
#include "cobProbe.h"

NS_COB_BEGIN

class Robot : public lianli::FSM
{
public:
	Robot();
    bool create(const std::string& name, Context& context = Context::DEFAULT);

    Probe* getProbe(const std::string& probeName);
    Robot* getRobot(const std::string& robotName);

protected:


private:

};

NS_COB_END

#endif
