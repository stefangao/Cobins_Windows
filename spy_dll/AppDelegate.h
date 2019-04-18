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
    virtual void onCreate(const lianli::Context& context) override;
    virtual void onStart() override;
    virtual void onStop() override;
    virtual void onDestroy(const lianli::Context& context) override;

protected:
    cob::Bin mBin;
};

#endif
