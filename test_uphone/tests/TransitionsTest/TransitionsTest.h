#ifndef _TRANSITIONS_TEST_H_
#define _TRANSITIONS_TEST_H_

#include "../testBasic.h"

using namespace cocos2d;

class TransitionsTestScene : public TestScene
{
public:
    virtual void runThisTest();
};

class TestLayer1 : public CCLayer
{
public:
    TestLayer1(void);
    ~TestLayer1(void);

    void restartCallback(NSObject* pSender);
    void nextCallback(NSObject* pSender);
    void backCallback(NSObject* pSender);

    void step(ccTime dt);
};

class TestLayer2 : public CCLayer
{
public:
    TestLayer2(void);
    ~TestLayer2(void);

    void restartCallback(NSObject* pSender);
    void nextCallback(NSObject* pSender);
    void backCallback(NSObject* pSender);

    void step(ccTime dt);
};

#endif
