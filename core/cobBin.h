#ifndef __COB_BIN_H__
#define __COB_BIN_H__

#include "lianli.h"
#include "cobConst.h"
#include "cobContext.h"

NS_COB_BEGIN

class Probe;
class Robot;
class Bin : public lianli::FSM
{
public:
	Bin();
    bool create(const std::string& name, Context& context = Context::DEFAULT);

    bool connect();
    bool disconnect();

    bool install(Probe& probe);
    bool uninstall(const std::string& probeName);

    Probe* getProbe(const std::string& probeName);
    Robot* getRobot(const std::string& robotName);

protected:
    std::map<const std::string, Probe*> mProbeMap;


private:

};

NS_COB_END

#endif
