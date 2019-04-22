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
#include "llfsm/lianli.h"
#include "cobins.h"

NS_COB_BEGIN

typedef std::function<void(const lianli::EvtData& evtData, lianli::EvtData& retData)> onEvtRequestProc;
typedef std::function<void(const lianli::EvtData& evtData)> onEvtNotifyProc;

#define CC_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define CC_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)

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

    virtual bool request(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData);
	virtual bool response(const lianli::EvtData& resultData);
    virtual bool notify(const std::string& evtName, const lianli::EvtData& evtData);

protected:
    virtual void onRequest(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData);
    virtual void onNotify(const std::string& evtName, const lianli::EvtData& evtData);

    void addEvtRequestProc(const std::string evtName, const onEvtRequestProc& evtRequestProc);
    void addEvtNotifyProc(const std::string evtName, const onEvtRequestProc& evtRequestProc);

protected:
    Bin* mBin;
    std::string mName;
    std::map<const std::string evtName, onEvtRequestProc> mEvtRequestProcMap;
    std::map<const std::string evtName, onEvtNotifyProc> mEvtNotifyProcMap;
};

NS_COB_END

#endif
