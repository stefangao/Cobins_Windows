#ifndef __COB_PROBER_H__
#define __COB_PROBER_H__

#include "cobProbe.h"
#include "llfsm/lianli.h"

NS_COB_BEGIN

class Prober : public Probe, public lianli::FSM
{
public:
	Prober(const std::string& name);

protected:


private:
};

NS_COB_END

#endif
