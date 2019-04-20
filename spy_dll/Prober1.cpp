#include "Prober1.h"

NS_COB_BEGIN

Prober1::Prober1(const std::string& name)
  : Prober(name)
{

}

void Prober1::onRequest(const std::string& evtName, const lianli::EvtData& evtData, lianli::EvtData& retData)
{
    COBLOG("Prober1::onRequest: evtName=%s\n", evtName.c_str());
    char buf[] = "ABCD7890";
    getBin()->RpcReturn((PBYTE)buf, sizeof(buf), TRUE);
}

NS_COB_END
