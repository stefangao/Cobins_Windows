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

void Probe::onRequest(const std::string& evtName, lianli::EvtStream& evtData, lianli::EvtStream& retData)
{
    auto iter = mEvtRequestProcMap.find(evtName);
    if (iter != mEvtRequestProcMap.end())
    {
        auto& evtRequestProc = iter->second;
        evtRequestProc(evtData, retData);
    }
}

void Probe::onNotify(const std::string& evtName, lianli::EvtStream& evtData)
{
    auto iter = mEvtNotifyProcMap.find(evtName);
    if (iter != mEvtNotifyProcMap.end())
    {
        auto& evtNotifyProc = iter->second;
        evtNotifyProc(evtData);
    }
}

void Probe::addEvtRequestProc(const std::string& evtName, const OnEvtRequestProc& evtRequestProc)
{
    mEvtRequestProcMap.insert(std::make_pair(evtName, evtRequestProc));
}

void Probe::addEvtNotifyProc(const std::string& evtName, const OnEvtNotifyProc& evtNotifyProc)
{
    mEvtNotifyProcMap.insert(std::make_pair(evtName, evtNotifyProc));
}

NS_COB_END
