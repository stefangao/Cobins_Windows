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

typedef std::function<void(lianli::EvtStream& evtData, lianli::EvtStream& retData)> OnEvtRequestProc;
typedef std::function<void(lianli::EvtStream& evtData)> OnEvtNotifyProc;

#define COB_BIND1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define COB_BIND2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)

class Bin;
class Probe
{
    friend class Bin;
public:
	Probe(const std::string& name);

    Bin* getBin() {return mBin;}
    const std::string& getName() const {return mName;}

    bool bind(const std::string& config);
    bool unbind();

    virtual bool request(const std::string& evtName, const lianli::EvtStream& evtData, lianli::EvtStream& retData);
    virtual bool notify(const std::string& evtName, const lianli::EvtStream& evtData);

protected:
    virtual void onRequest(const std::string& evtName, lianli::EvtStream& evtData, lianli::EvtStream& retData);
    virtual void onNotify(const std::string& evtName, lianli::EvtStream& evtData);

    void addEvtRequestProc(const std::string& evtName, const OnEvtRequestProc& evtRequestProc);
    void addEvtNotifyProc(const std::string& evtName, const OnEvtNotifyProc& evtNotifyProc);

protected:
    Bin* mBin;
    std::string mName;
    std::map<std::string, OnEvtRequestProc> mEvtRequestProcMap;
    std::map<std::string, OnEvtNotifyProc> mEvtNotifyProcMap;
};

NS_COB_END

#endif
