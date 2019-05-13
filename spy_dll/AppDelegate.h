#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cobins.h"
USING_NS_COB;
USING_NS_LL;

class AppDelegate : public cob::Application
{
public:
    AppDelegate();

protected:
    virtual void onCreate(void* params) override;
    virtual void onStart() override;
    virtual void onStop() override;
    virtual void onDestroy() override;

protected:
    cob::Bin mBin;
};

#endif
