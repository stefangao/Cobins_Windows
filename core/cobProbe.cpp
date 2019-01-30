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


	return true;
}

void Probe::onRequest(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData)
{


}

void Probe::onResponse(const std::string& evtName, lianli::EvtData& retData)
{

}

void Probe::onNotify(const std::string& evtName, const lianli::EvtData& evtData)
{

}

NS_COB_END
