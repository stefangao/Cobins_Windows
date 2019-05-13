#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cobins.h"
#include <string>
USING_NS_COB;
USING_NS_LL;

class AppDelegate : public cob::Application
{
public:
    AppDelegate();
	virtual ~AppDelegate() {};

protected:
    virtual void onCreate(void* params) override;
    virtual void onStart() override;
    virtual bool onEventProc(const std::string& evtName, EvtStream& evtData) override;
    virtual void onStop() override;
    virtual void onDestroy() override;

protected:
    cob::Bin mBin;

    DECLARE_TRANS_TABLE()
};

#endif
