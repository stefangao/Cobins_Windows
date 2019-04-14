#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cobApplication.h"
USING_NS_COB;

class AppDelegate : public cobins::Application
{
public:
    AppDelegate();
    bool create(HWND hWnd, const std::string& name, cobins::Context& context = cobins::Context::DEFAULT);

protected:
    virtual void onCreate(const lianli::Context& context) override;
    virtual void onStart() override;
    virtual void onStop() override;
    virtual void onDestroy(const lianli::Context& context) override;

protected:
    cobins::Bin mBin;
    HWND mhMainWnd;

public:
    enum
    {
        DAEMON, TEST1
    };

    class Daemon : public State
    {
    protected:
        virtual void onEnter() override
        {
            State::onEnter();

            COBLOG("AppDelegate: Daemon onEnter\n");
        }
        virtual void onExit() override
        {
            State::onExit();
        }
        virtual bool onEventProc(const std::string& evtName, lianli::EvtData& evtData) override
        {
            State::onEventProc(evtName, evtData);
            return true;
        }

        DECLARE_STATE_FACTORY(Daemon, AppDelegate)
    };

    DECLARE_STATE_TABLE()
    DECLARE_TRANS_TABLE()
};

#endif
