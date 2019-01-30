//============================================================================
// Name        : cobContext.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_CONTEXT_H__
#define __COB_CONTEXT_H__

#include "lianli.h"
#include "cobConst.h"

NS_COB_BEGIN

class Context : public lianli::Context
{
public:
    Context(const std::string& name);

protected:
    std::list<lianli::FSM*> mBinList;

private:
};

NS_COB_END

#endif
