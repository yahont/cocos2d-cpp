#ifndef _TOUCHES_TEST__H_
#define _TOUCHES_TEST__H_

#include "2d/CCNode.h"
#include "../BaseTest.h"


DEFINE_TEST_SUITE(TouchesTests);

class PongScene : public TestCase
{
public:
    static PongScene* create()
    {
        auto ret = new PongScene;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual bool init() override;
};

class Ball;
class Paddle;
class PongLayer : public cocos2d::Layer
{
private:
    Ball*       _ball;
    std::vector<cocos2d::node_ptr<Paddle>>  _paddles;
    cocos2d::Vec2     _ballStartingVelocity; 
public:
    static PongLayer* create()
    {
        auto ret = new PongLayer;
        ret->init();
        ret->autorelease();
        return ret;
    }
    PongLayer();
    ~PongLayer();

    void resetAndScoreBallForPlayer(int player);
    void doStep(float delta);
};

class ForceTouchTest : public TestCase
{
public:
    static ForceTouchTest* create()
    {
        auto ret = new ForceTouchTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
protected:
    ForceTouchTest();
    virtual ~ForceTouchTest();

    cocos2d::Label * _infoLabel;
};

#endif
