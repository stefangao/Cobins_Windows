//============================================================================
// Name        : llContext.cpp
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================
#include "cobContext.h"

NS_COB_BEGIN

Context Context::DEFAULT = Context("CobinsContext");

Context::Context(const std::string& name)
  : lianli::Context(name)
{

}

Bin* Context::getBin(const std::string& name)
{

	return nullptr;
}

void Context::add(lianli::FSM* fsm)
{
	lianli::Context::add(fsm);
}

void Context::remove(lianli::FSM* fsm)
{
	lianli::Context::remove(fsm);
}
NS_COB_END
