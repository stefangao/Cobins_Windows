#ifndef __COB_PROBER_H__
#define __COB_PROBER_H__

#include "llfsm/lianli.h"
#include "cobProbe.h"

NS_COB_BEGIN

class Prober : public Probe, public lianli::FSＭ
{
public:
	Prober(const std::string& name);

protected:


private:
};

NS_COB_END

#endif
