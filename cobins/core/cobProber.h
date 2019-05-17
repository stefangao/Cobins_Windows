#ifndef __COB_PROBER_H__
#define __COB_PROBER_H__

#include "cobProbe.h"
#include "llfsm/lianli.h"

NS_COB_BEGIN

class Prober : public Probe
{
public:
    Prober& create(const std::string& name, lianli::Context& context = lianli::Context::DEFAULT);

protected:


private:
};

NS_COB_END

#endif
