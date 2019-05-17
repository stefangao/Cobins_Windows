//============================================================================
// Name        : cobContext.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_PROBE_H__
#define __COB_PROBE_H__

#include <functional>
#include <map>
#include <string>
#include "cobMacros.h"
#include "llfsm/lianli.h"

NS_COB_BEGIN

class Bin;
class Probe : public lianli::FSM
{
    friend class Bin;
public:
	Probe(const std::string& name = "");

    Bin* getBin() {return mBin;}
    const std::string& getName() const {return mName;}

    bool bind(const std::string& config);
    bool unbind();

    virtual bool request(const std::string& evtName, const lianli::EvtStream& evtData, lianli::EvtStream& retData);
    virtual bool notify(const std::string& evtName, const lianli::EvtStream& evtData);

protected:
    Bin* mBin;
    std::string mName;
};

NS_COB_END

#endif
