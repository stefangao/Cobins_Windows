#include "cobProber.h"

NS_COB_BEGIN

Prober& Prober::create(const std::string& name, lianli::Context& context)
{
    Probe::mName = name;
    FSM::create(name, context);
    return *this;
}

NS_COB_END
