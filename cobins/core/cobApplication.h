#ifndef __COB_APPLICATION_H__
#define __COB_APPLICATION_H__

#include "llfsm/lianli.h"
#include "cobMacros.h"
#include "cobBin.h"

NS_COB_BEGIN

class Application : public lianli::FSM
{
public:
    Application();
    bool create(const std::string& name, lianli::Context& context = lianli::Context::DEFAULT);

protected:


};

NS_COB_END

#endif
