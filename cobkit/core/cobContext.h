//============================================================================
// Name        : cobContext.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_CONTEXT_H__
#define __COB_CONTEXT_H__

#include "llfsm/lianli.h"
#include "cobMacros.h"

NS_COB_BEGIN

class Bin;
class Context : public lianli::Context
{
public:
    Context(const std::string& name);
    static const Context& getDefault() {return DEFAULT;};

    Bin* getBin(const std::string& name);

protected:
    std::list<lianli::FSM*> mBinList;

    virtual void add(lianli::FSM* fsm);
    virtual void remove(lianli::FSM* fsm);

public:
    static Context DEFAULT;
};

NS_COB_END

#endif
