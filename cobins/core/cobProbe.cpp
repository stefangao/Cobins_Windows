//============================================================================
// Name        : cobProbe.cpp
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include "cobProbe.h"
#include "base/cobUtils.h"
#include "core/cobBin.h"

NS_COB_BEGIN

Probe::Probe(const std::string& name)
  : mName(name)
{
	mBin = nullptr;
}

bool Probe::bind(const std::string& config)
{
	return true;
}

bool Probe::unbind()
{
	return true;
}

bool Probe::request(const std::string& evtName, const lianli::EvtStream& evtData, lianli::EvtStream& retData)
{
    COBASSERT(mBin, "bin is null");

    mBin->RpcSend(mName, evtName, evtData, retData);
	return true;
}

bool Probe::notify(const std::string& evtName, const lianli::EvtStream& evtData)
{
    COBASSERT(mBin, "bin is null");

    mBin->RpcPost(mName, evtName, evtData);
    return true;
}

NS_COB_END
