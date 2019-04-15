#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cobins.h"
USING_NS_COB;

class AppDelegate : public cobins::Application
{
public:
    AppDelegate();
    bool create(const std::string& name, Context& context = Context::DEFAULT);

protected:
    virtual void onCreate(const lianli::Context& context) override;
    virtual void onStart() override;
    virtual void onStop() override;
    virtual void onDestroy(const lianli::Context& context) override;

protected:
public:
    cobins::Bin mBin;
};

#endif
