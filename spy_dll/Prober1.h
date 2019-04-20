#ifndef __COB_PROBER1_H__
#define __COB_PROBER1_H__

#include "cobins.h"

NS_COB_BEGIN

class Prober1 : public Prober
{
public:
    Prober1(const std::string& name);

protected:
    virtual void onRequest(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData) override;


private:
};

NS_COB_END

#endif
