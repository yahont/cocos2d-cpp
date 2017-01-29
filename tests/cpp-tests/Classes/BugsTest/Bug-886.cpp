//
// Bug-886
// http://code.google.com/p/cocos2d-iphone/issues/detail?id=886
//

#include "Bug-886.h"

#include "2d/CCSprite.h"

using namespace cocos2d;

bool Bug886Layer::init()
{
    if(BugsTestBase::init())
    {
        auto sprite = Sprite::create("Images/bugs/bug886.jpg");
        sprite->setAnchorPoint(Vec2::ZERO);
        sprite->setPosition(Vec2::ZERO);
        sprite->setScaleX(0.6f);
        addChild(sprite);

        auto sprite2 = Sprite::create("Images/bugs/bug886.png");
        sprite2->setAnchorPoint(Vec2::ZERO);
        sprite2->setScaleX(0.6f);
        sprite2->setPosition(sprite->getContentSize().width * 0.6f + 10, 0);
        addChild(sprite2);

        return true;
    }

    return false;
}
