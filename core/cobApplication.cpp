#include "cobApplication.h"

NS_COB_BEGIN

Application::Application()
{

}

bool Application::create(const std::string& name, Context& context)
{
    if (!lianli::FSM::create(name, context))
        return false;

    return true;
}

NS_COB_END
