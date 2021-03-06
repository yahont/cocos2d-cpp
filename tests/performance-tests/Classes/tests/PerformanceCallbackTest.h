//
//  PerformanceCallbackTest.h

#ifndef __PERFORMANCE_CALLBACK_TEST_H__
#define __PERFORMANCE_CALLBACK_TEST_H__

#include "BaseTest.h"

DEFINE_TEST_SUITE(PerformceCallbackTests);

class PerformanceCallbackScene : public TestCase
{
public:
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onUpdate(float /*dt*/) {};
    
    void dumpProfilerInfo(float dt);
protected:
    
    std::string _profileName;
    int _placeHolder; // To avoid compiler optimization
    static const int LOOP_COUNT = 10000;
};

class SimulateNewSchedulerCallbackPerfTest : public PerformanceCallbackScene
{
public:
    static SimulateNewSchedulerCallbackPerfTest* create()
    {
        auto ret = new SimulateNewSchedulerCallbackPerfTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    // overrides
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onUpdate(float dt) override;
    
    virtual void update(float /*dt*/) override { _placeHolder = 100; };
    
    void simulateSchedulePerFrame(const std::function<void(float)>& callback)
    {
        _callback = callback;
    }
    
    template <class T>
    void simulateScheduleUpdate(T* target)
    {
        target->simulateSchedulePerFrame([target](float dt){
            target->update(dt);
        });
    }
    
private:
    std::function<void(float)> _callback;
};


// InvokeMemberFunctionPerfTest
class InvokeMemberFunctionPerfTest : public PerformanceCallbackScene
{
public:
    static InvokeMemberFunctionPerfTest* create()
    {
        auto ret = new InvokeMemberFunctionPerfTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onUpdate(float dt) override;
    
    virtual void update(float /*dt*/) override { _placeHolder = 200; };
    
private:
    InvokeMemberFunctionPerfTest* _target;
    void (InvokeMemberFunctionPerfTest::*_selector)(float);
};

// InvokeStdFunctionPerfTest
class InvokeStdFunctionPerfTest : public PerformanceCallbackScene
{
public:
    static InvokeStdFunctionPerfTest* create()
    {
        auto ret = new InvokeStdFunctionPerfTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    // overrides
    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onUpdate(float dt) override;
    
    virtual void update(float /*dt*/) override { _placeHolder = 100; };
private:
    std::function<void(float)> _callback;
};

#endif /* __PERFORMANCE_CALLBACK_TEST_H__ */
