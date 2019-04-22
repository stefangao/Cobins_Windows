//============================================================================
// Name        : cobProbe.cpp
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include "cobProbe.h"

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

bool Probe::request(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData)
{
    COBASSERT(mBin, "bin is null");

    mBin->RpcSend(mName, evtName, evtData, retData);
	return true;
}

bool Probe::response(const lianli::EvtData& resultData)
{
    COBASSERT(mBin, "bin is null");

    mBin->RpcReturn(resultData);
    return true;
}

bool Probe::notify(const std::string& evtName, const lianli::EvtData& evtData)
{
    COBASSERT(mBin, "bin is null");

    mBin->RpcPost(mName, evtName, evtData);
    return true;
}

void Probe::onRequest(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData)
{
    auto iter = mEvtRequestProcMap.find(evtName);
    if (iter != mEvtRequestProcMap.end())
    {
        auto& evtRequestProc = iter->second;
        evtRequestProc(evtData, retData);
        response(retData);
    }
}

void Probe::onNotify(const std::string& evtName, const lianli::EvtData& evtData)
{
    auto iter = mEvtNotifyProcMap.find(evtName);
    if (iter != mEvtNotifyProcMap.end())
    {
        auto& evtNotifyProc = iter->second;
        evtNotifyProc(evtData);
    }
}

void Probe::addEvtRequestProc(const std::string evtName, const onEvtRequestProc& evtRequestProc)
{
    mEvtRequestProcMap.insert(std::pair（"readValue", evtRequestProc));
}

void Probe::addEvtNotifyProc(const std::string evtName, const onEvtRequestProc& evtRequestProc)
{
    mEvtNotifyProcMap.insert(std::pair（"readValue", evtRequestProc));

}

NS_COB_END
