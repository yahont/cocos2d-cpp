#ifndef _CLICK_AND_MOVE_TEST_H_
#define _CLICK_AND_MOVE_TEST_H_

#include "../BaseTest.h"

DEFINE_TEST_SUITE(ClickAndMoveTest);

class ClickAndMoveTestCase : public TestCase
{
public:
    static ClickAndMoveTestCase* create()
    {
        auto ret = new ClickAndMoveTestCase;
        ret->init();
        ret->autorelease();
        return ret;
    }

    ClickAndMoveTestCase();
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif
