#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cobApplication.h"

NS_COB_BEGIN

class AppDelegate : public Application
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
    Bin mBin;
};

NS_COB_END

#endif
