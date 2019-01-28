//============================================================================
// Name        : llContext.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_CONTEXT_H__
#define __COB_CONTEXT_H__

#include <string>
#include <list>
#include "cobConst.h"

NS_COB_BEGIN

class Context
{
public:
    Context(const std::string& name);
    static const Context& getDefault() {return DEFAULT;};

protected:
    std::list<FSM*> mFsmList;

private:
    static Context DEFAULT;
};

NS_COB_END

#endif
