#include "LayerTest.h"
#include "../testResource.h"

#include "2d/CCLabel.h"
#include "2d/CCMenu.h"
#include "2d/CCMenuItem.h"
#include "2d/CCSprite.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"

using namespace cocos2d;

enum 
{
    kTagLayer = 1,
};

LayerTests::LayerTests()
{
    ADD_TEST_CASE(LayerTestCascadingOpacityA);
    ADD_TEST_CASE(LayerTestCascadingOpacityB);
    ADD_TEST_CASE(LayerTestCascadingOpacityC);
    ADD_TEST_CASE(LayerTestCascadingColorA);
    ADD_TEST_CASE(LayerTestCascadingColorB);
    ADD_TEST_CASE(LayerTestCascadingColorC);
    ADD_TEST_CASE(LayerTest1);
    ADD_TEST_CASE(LayerTest2);
    ADD_TEST_CASE(LayerTestBlend);
    ADD_TEST_CASE(LayerGradientTest);
    ADD_TEST_CASE(LayerGradientTest2);
    ADD_TEST_CASE(LayerIgnoreAnchorPointPos);
    ADD_TEST_CASE(LayerIgnoreAnchorPointRot);
    ADD_TEST_CASE(LayerIgnoreAnchorPointScale);
    ADD_TEST_CASE(LayerExtendedBlendOpacityTest);
    ADD_TEST_CASE(LayerBug3162A);
    ADD_TEST_CASE(LayerBug3162B);
    ADD_TEST_CASE(LayerColorOccludeBug);
}

// Cascading support extensions

static void setEnableRecursiveCascading(Node* node, bool enable)
{
    node->setCascadeColorEnabled(enable);
    node->setCascadeOpacityEnabled(enable);
    
    auto& children = node->getChildren();
    for(const auto & child : children) {
        setEnableRecursiveCascading(child.get(), enable);
    }
}

std::string LayerTest::title() const
{
    return "Layer Test";
}

// LayerTestCascadingOpacityA
void LayerTestCascadingOpacityA::onEnter()
{
    LayerTest::onEnter();
    
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = Layer::create();
    
    auto sister1 = Sprite::create("Images/grossinis_sister1.png");
    auto sister2 = Sprite::create("Images/grossinis_sister2.png");
    auto label = Label::createWithBMFont("fonts/bitmapFontTest.fnt", "Test");
    
    layer1->addChild(sister1);
    layer1->addChild(sister2);
    layer1->addChild(label);
    this->addChild( layer1, 0, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, s.height/2));
    sister2->setPosition( Vec2( s.width*2/3, s.height/2));
    label->setPosition( Vec2( s.width/2, s.height/2));
    
    layer1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<FadeTo>(4, 0),
                std::make_unique<FadeTo>(4, 255),
                std::make_unique<DelayTime>(1)
            )));

    sister1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<FadeTo>(2, 0),
                std::make_unique<FadeTo>(2, 255),
                std::make_unique<FadeTo>(2, 0),
                std::make_unique<FadeTo>(2, 255),
                std::make_unique<DelayTime>(1)
            )));
    
    // Enable cascading in scene
    setEnableRecursiveCascading(this, true);
}

std::string LayerTestCascadingOpacityA::subtitle() const
{
    return "Layer: cascading opacity";
}


//  LayerTestCascadingOpacityB
void LayerTestCascadingOpacityB::onEnter()
{
    LayerTest::onEnter();
        
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = LayerColor::create(Color4B(192, 0, 0, 255), s.width, s.height/2);
    layer1->setCascadeColorEnabled(false);
    
    layer1->setPosition( Vec2(0, s.height/2));
    
    auto sister1 = Sprite::create("Images/grossinis_sister1.png");
    auto sister2 = Sprite::create("Images/grossinis_sister2.png");
    auto label = Label::createWithBMFont("fonts/bitmapFontTest.fnt", "Test");
    
    layer1->addChild(sister1);
    layer1->addChild(sister2);
    layer1->addChild(label);
    this->addChild( layer1, 0, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, 0));
    sister2->setPosition( Vec2( s.width*2/3, 0));
    label->setPosition( Vec2( s.width/2, 0));
    
    layer1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<FadeTo>(4, 0),
                std::make_unique<FadeTo>(4, 255),
                std::make_unique<DelayTime>(1)
            )));
    
    sister1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<FadeTo>(2, 0),
                std::make_unique<FadeTo>(2, 255),
                std::make_unique<FadeTo>(2, 0),
                std::make_unique<FadeTo>(2, 255),
                std::make_unique<DelayTime>(1)
            )));

    // Enable cascading in scene
    setEnableRecursiveCascading(this, true);
}

std::string LayerTestCascadingOpacityB::subtitle() const
{
    return "CCLayerColor: cascading opacity";
}


// LayerTestCascadingOpacityC
void LayerTestCascadingOpacityC::onEnter()
{
    LayerTest::onEnter();
    
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = LayerColor::create(Color4B(192, 0, 0, 255), s.width, s.height/2);
    layer1->setCascadeColorEnabled(false);
    layer1->setCascadeOpacityEnabled(false);
    
    layer1->setPosition( Vec2(0, s.height/2));
    
    auto sister1 = Sprite::create("Images/grossinis_sister1.png");
    auto sister2 = Sprite::create("Images/grossinis_sister2.png");
    auto label = Label::createWithBMFont("fonts/bitmapFontTest.fnt", "Test");
    
    layer1->addChild(sister1);
    layer1->addChild(sister2);
    layer1->addChild(label);
    this->addChild( layer1, 0, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, 0));
    sister2->setPosition( Vec2( s.width*2/3, 0));
    label->setPosition( Vec2( s.width/2, 0));
    
    layer1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<FadeTo>(4, 0),
                std::make_unique<FadeTo>(4, 255),
                std::make_unique<DelayTime>(1)
            )));

    sister1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<FadeTo>(2, 0),
                std::make_unique<FadeTo>(2, 255),
                std::make_unique<FadeTo>(2, 0),
                std::make_unique<FadeTo>(2, 255),
                std::make_unique<DelayTime>(1)
            )));
}

std::string LayerTestCascadingOpacityC::subtitle() const
{
    return "CCLayerColor: non-cascading opacity";
}


//// Example LayerTestCascadingColor

// LayerTestCascadingColorA
void LayerTestCascadingColorA::onEnter()
{
    LayerTest::onEnter();
    
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = Layer::create();
    
    auto sister1 = Sprite::create("Images/grossinis_sister1.png");
    auto sister2 = Sprite::create("Images/grossinis_sister2.png");
    auto label = Label::createWithBMFont("fonts/bitmapFontTest.fnt", "Test");
    
    layer1->addChild(sister1);
    layer1->addChild(sister2);
    layer1->addChild(label);
    this->addChild( layer1, 0, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, s.height/2));
    sister2->setPosition( Vec2( s.width*2/3, s.height/2));
    label->setPosition( Vec2( s.width/2, s.height/2));
    
    layer1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<TintTo>(6, 255, 0, 255),
                std::make_unique<TintTo>(6, 255, 255, 255),
                std::make_unique<DelayTime>(1)
            )));

    sister1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<TintTo>(2, 255, 255, 0),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<TintTo>(2, 0, 255, 255),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<TintTo>(2, 255, 0, 255),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<DelayTime>(1)
            )));
    
    // Enable cascading in scene
    setEnableRecursiveCascading(this, true);
     
}

std::string LayerTestCascadingColorA::subtitle() const
{
    return "Layer: cascading color";
}


// LayerTestCascadingColorB
void LayerTestCascadingColorB::onEnter()
{
    LayerTest::onEnter();
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = LayerColor::create(Color4B(255, 255, 255, 255), s.width, s.height/2);
    
    layer1->setPosition( Vec2(0, s.height/2));
    
    auto sister1 = Sprite::create("Images/grossinis_sister1.png");
    auto sister2 = Sprite::create("Images/grossinis_sister2.png");
    auto label = Label::createWithBMFont("fonts/bitmapFontTest.fnt", "Test");
    
    layer1->addChild(sister1);
    layer1->addChild(sister2);
    layer1->addChild(label);
    this->addChild( layer1, 0, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, 0));
    sister2->setPosition( Vec2( s.width*2/3, 0));
    label->setPosition( Vec2( s.width/2, 0));
    
    layer1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<TintTo>(6, 255, 0, 255),
                std::make_unique<TintTo>(6, 255, 255, 255),
                std::make_unique<DelayTime>(1)
            )));

    sister1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<TintTo>(2, 255, 255, 0),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<TintTo>(2, 0, 255, 255),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<TintTo>(2, 255, 0, 255),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<DelayTime>(1)
            )));
    
    // Enable cascading in scene
    setEnableRecursiveCascading(this, true);
}

std::string LayerTestCascadingColorB::subtitle() const
{
    return "CCLayerColor: cascading color";
}


// LayerTestCascadingColorC
void LayerTestCascadingColorC::onEnter()
{
    LayerTest::onEnter();
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = LayerColor::create(Color4B(255, 255, 255, 255), s.width, s.height/2);
    layer1->setCascadeColorEnabled(false);
    layer1->setPosition( Vec2(0, s.height/2));
    
    auto sister1 = Sprite::create("Images/grossinis_sister1.png");
    auto sister2 = Sprite::create("Images/grossinis_sister2.png");
    auto label = Label::createWithBMFont("fonts/bitmapFontTest.fnt", "Test");
    
    layer1->addChild(sister1);
    layer1->addChild(sister2);
    layer1->addChild(label);
    this->addChild( layer1, 0, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, 0));
    sister2->setPosition( Vec2( s.width*2/3, 0));
    label->setPosition( Vec2( s.width/2, 0));
    
    layer1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<TintTo>(6, 255, 0, 255),
                std::make_unique<TintTo>(6, 255, 255, 255),
                std::make_unique<DelayTime>(1)
            )));

    sister1->runAction(
        std::make_unique<RepeatForever>(
            std::make_unique<Sequence>(
                std::make_unique<TintTo>(2, 255, 255, 0),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<TintTo>(2, 0, 255, 255),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<TintTo>(2, 255, 0, 255),
                std::make_unique<TintTo>(2, 255, 255, 255),
                std::make_unique<DelayTime>(1)
            )));
}

std::string LayerTestCascadingColorC::subtitle() const
{
    return "CCLayerColor: non-cascading color";
}

//------------------------------------------------------------------
//
// LayerTest1
//
//------------------------------------------------------------------
void LayerTest1::onEnter()
{
    LayerTest::onEnter();

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(LayerTest1::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(LayerTest1::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(LayerTest1::onTouchesEnded, this);
    
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto s = Director::getInstance()->getWinSize();
    auto layer = LayerColor::create( Color4B(0xFF, 0x00, 0x00, 0x80), 200, 200); 
    
    layer->setIgnoreAnchorPointForPosition(false);
    layer->setPosition( Vec2(s.width/2, s.height/2) );
    addChild(layer, 1, kTagLayer);
}

void LayerTest1::updateSize(Vec2 &touchLocation)
{    
    auto s = Director::getInstance()->getWinSize();
    
    auto newSize = Size( fabs(touchLocation.x - s.width/2)*2, fabs(touchLocation.y - s.height/2)*2);
    
    auto l = (LayerColor*) getChildByTag(kTagLayer);

    l->setContentSize( newSize );
}

void LayerTest1::onTouchesBegan(const std::vector<Touch*>& touches, Event  *event)
{
    onTouchesMoved(touches, event);
}

void LayerTest1::onTouchesMoved(const std::vector<Touch*>& touches, Event*)
{
    auto touchLocation = touches[0]->getLocation();

    updateSize(touchLocation);
}

void LayerTest1::onTouchesEnded(const std::vector<Touch*>& touches, Event  *event)
{
    onTouchesMoved(touches, event);
}

std::string LayerTest1::subtitle() const
{
    return "ColorLayer resize (tap & move)";
}

//------------------------------------------------------------------
//
// LayerTest2
//
//------------------------------------------------------------------
void LayerTest2::onEnter()
{
    LayerTest::onEnter();

    auto s = Director::getInstance()->getWinSize();
    auto layer1 = LayerColor::create( Color4B(255, 255, 0, 80), 100, 300);
    layer1->setPosition(Vec2(s.width/3, s.height/2));
    layer1->setIgnoreAnchorPointForPosition(false);
    addChild(layer1, 1);
    
    auto layer2 = LayerColor::create( Color4B(0, 0, 255, 255), 100, 300);
    layer2->setPosition(Vec2((s.width/3)*2, s.height/2));
    layer2->setIgnoreAnchorPointForPosition(false);
    addChild(layer2, 1);
    
    auto actionTint = std::make_unique<TintBy>(2, -255, -127, 0);
    auto actionTintBack = std::unique_ptr<TintBy>(actionTint->reverse());
    auto seq1 = std::make_unique<Sequence>( std::move( actionTint), std::move( actionTintBack) );
    layer1->runAction( std::move(seq1) );

    auto actionFade = std::make_unique<FadeOut>(2.0f);
    auto actionFadeBack = std::unique_ptr<FadeTo>(actionFade->reverse());
    auto seq2 = std::make_unique<Sequence>( std::move(actionFade), std::move( actionFadeBack) );        
    layer2->runAction( std::move(seq2) );
}

std::string LayerTest2::subtitle() const
{
    return "ColorLayer: fade and tint";
}

//------------------------------------------------------------------
//
// LayerTestBlend
//
//------------------------------------------------------------------

LayerTestBlend::LayerTestBlend()
{
    auto s = Director::getInstance()->getWinSize();
    auto layer1 = LayerColor::create( Color4B(255, 255, 255, 80) );
    
    auto sister1 = Sprite::create(s_pathSister1);
    auto sister2 = Sprite::create(s_pathSister2);
    
    addChild(sister1);
    addChild(sister2);
    addChild(layer1, 100, kTagLayer);
    
    sister1->setPosition( Vec2( s.width*1/3, s.height/2) );
    sister2->setPosition( Vec2( s.width*2/3, s.height/2) );

    Director::getInstance()->getScheduler().schedule(
        TimedJob(this, &LayerTestBlend::newBlend, 0).interval(1.0f).delay(1.0f).paused(isPaused())
    );
}

void LayerTestBlend::newBlend(float /*dt*/)
{
     auto layer = (LayerColor*)getChildByTag(kTagLayer);

    GLenum src;
    GLenum dst;

    if( layer->getBlendFunc().dst == GL_ZERO )
    {
        src = GL_SRC_ALPHA;
        dst = GL_ONE_MINUS_SRC_ALPHA;
    }
    else
    {
        src = GL_ONE_MINUS_DST_COLOR;
        dst = GL_ZERO;
    }

    BlendFunc bf = {src, dst};
    layer->setBlendFunc( bf );
}


std::string LayerTestBlend::subtitle() const
{
    return "ColorLayer: blend";
}

//------------------------------------------------------------------
//
// LayerGradientTest
//
//------------------------------------------------------------------
LayerGradientTest::LayerGradientTest()
{
    auto layer1 = LayerGradient::create(Color4B(255,0,0,255), Color4B(0,255,0,255), Vec2(0.9f, 0.9f));
    addChild(layer1, 0, kTagLayer);

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesMoved = CC_CALLBACK_2(LayerGradientTest::onTouchesMoved, this);
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    auto label1 = Label::createWithTTF("Compressed Interpolation: Enabled", "fonts/Marker Felt.ttf", 26);
    auto label2 = Label::createWithTTF("Compressed Interpolation: Disabled", "fonts/Marker Felt.ttf", 26);

    MenuItemToggle::items_container menuItems;
    menuItems.reserve(2);

    menuItems.push_back(
        to_node_ptr(
            static_cast<MenuItem*>(MenuItemLabel::create(label1))
        )
    );

    menuItems.push_back(
        to_node_ptr(
            static_cast<MenuItem*>(MenuItemLabel::create(label2))
        )
    );

    auto item = MenuItemToggle::createWithCallback(
        CC_CALLBACK_1(LayerGradientTest::toggleItem, this),
        std::move(menuItems)
    );

    auto menu = Menu::create(item, nullptr);
    addChild(menu);
    auto s = Director::getInstance()->getWinSize();
    menu->setPosition(Vec2(s.width / 2, 100));
}

void LayerGradientTest::toggleItem(Ref*)
{
    auto gradient = static_cast<LayerGradient*>( getChildByTag(kTagLayer) );
    gradient->setCompressedInterpolation(! gradient->isCompressedInterpolation());
}

void LayerGradientTest::onTouchesMoved(const std::vector<Touch*>& touches, Event*)
{
    auto s = Director::getInstance()->getWinSize();

    auto touch = touches[0];
    auto start = touch->getLocation();    

    auto diff =  Vec2(s.width/2,s.height/2) - start;
    diff = diff.getNormalized();

    auto gradient = static_cast<LayerGradient*>( getChildByTag(1) );
    gradient->setVector(diff);
}

std::string LayerGradientTest::title() const
{
    return "LayerGradientTest";
}

std::string LayerGradientTest::subtitle() const
{
    return "Touch the screen and move your finger";
}

//------------------------------------------------------------------
//
// LayerGradientTest2
//
//------------------------------------------------------------------
LayerGradientTest2::LayerGradientTest2()
{
    auto layer = LayerGradient::create(Color4B(255,0,0,255), Color4B(255,255,0,255));
    addChild(layer);
}

std::string LayerGradientTest2::title() const
{
    return "LayerGradientTest 2";
}

std::string LayerGradientTest2::subtitle() const
{
    return "You should see a gradient";
}

// LayerIgnoreAnchorPointPos

#define kLayerIgnoreAnchorPoint  1000

void LayerIgnoreAnchorPointPos::onEnter()
{
    LayerTest::onEnter();

    auto s = Director::getInstance()->getWinSize();

    auto l = LayerColor::create(Color4B(255, 0, 0, 255), 150, 150);

    l->setAnchorPoint(Vec2(0.5f, 0.5f));
    l->setPosition(Vec2( s.width/2, s.height/2));

    auto move = std::make_unique<MoveBy>(2, Vec2(100,2));
    auto back = std::unique_ptr<MoveBy>(move->reverse());
    auto seq = std::make_unique<Sequence>( std::move(move), std::move( back) );
    l->runAction(std::make_unique<RepeatForever>( std::move(seq) ));
    this->addChild(l, 0, kLayerIgnoreAnchorPoint);

    auto child = Sprite::create("Images/grossini.png");
    l->addChild(child);
    auto lsize = l->getContentSize();
    child->setPosition(Vec2(lsize.width/2, lsize.height/2));

    auto item = MenuItemFont::create("Toggle ignore anchor point", CC_CALLBACK_1(LayerIgnoreAnchorPointPos::onToggle, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);

    menu->setPosition(Vec2(s.width/2, s.height/2));
}

void LayerIgnoreAnchorPointPos::onToggle(Ref* /*pObject*/)
{
    auto layer = this->getChildByTag(kLayerIgnoreAnchorPoint);
    bool ignore = layer->isIgnoreAnchorPointForPosition();
    layer->setIgnoreAnchorPointForPosition(! ignore);
}

std::string LayerIgnoreAnchorPointPos::title() const
{
    return "IgnoreAnchorPoint - Position";
}

std::string LayerIgnoreAnchorPointPos::subtitle() const
{
    return "Ignoring Anchor Vec2 for position";
}

// LayerIgnoreAnchorPointRot

void LayerIgnoreAnchorPointRot::onEnter()
{
    LayerTest::onEnter();
    auto s = Director::getInstance()->getWinSize();

    auto l = LayerColor::create(Color4B(255, 0, 0, 255), 200, 200);

    l->setAnchorPoint(Vec2(0.5f, 0.5f));
    l->setPosition(Vec2( s.width/2, s.height/2));

    this->addChild(l, 0, kLayerIgnoreAnchorPoint);

    l->runAction(std::make_unique<RepeatForever>(std::make_unique<RotateBy>(2, 360)));

    auto child = Sprite::create("Images/grossini.png");
    l->addChild(child);
    auto lsize = l->getContentSize();
    child->setPosition(Vec2(lsize.width/2, lsize.height/2));

    auto item = MenuItemFont::create("Toogle ignore anchor point", CC_CALLBACK_1(LayerIgnoreAnchorPointRot::onToggle, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);

    menu->setPosition(Vec2(s.width/2, s.height/2));
}

void LayerIgnoreAnchorPointRot::onToggle(Ref* /*pObject*/)
{
    auto layer = this->getChildByTag(kLayerIgnoreAnchorPoint);
    bool ignore = layer->isIgnoreAnchorPointForPosition();
    layer->setIgnoreAnchorPointForPosition(! ignore);
}

std::string LayerIgnoreAnchorPointRot::title() const
{
    return "IgnoreAnchorPoint - Rotation";
}

std::string LayerIgnoreAnchorPointRot::subtitle() const
{
    return "Ignoring Anchor Vec2 for rotations";
}

// LayerIgnoreAnchorPointScale
void LayerIgnoreAnchorPointScale::onEnter()
{
    LayerTest::onEnter();
    
    auto s = Director::getInstance()->getWinSize();

    auto l = LayerColor::create(Color4B(255, 0, 0, 255), 200, 200);

    l->setAnchorPoint(Vec2(0.5f, 1.0f));
    l->setPosition(Vec2( s.width/2, s.height/2));


    auto scale = std::make_unique<ScaleBy>(2, 2);
    auto back = std::unique_ptr<ScaleBy>(scale->reverse());
    auto seq = std::make_unique<Sequence>( std::move(scale), std::move( back) );

    l->runAction(std::make_unique<RepeatForever>( std::move(seq) ));

    this->addChild(l, 0, kLayerIgnoreAnchorPoint);

    auto child = Sprite::create("Images/grossini.png");
    l->addChild(child);
    auto lsize = l->getContentSize();
    child->setPosition(Vec2(lsize.width/2, lsize.height/2));

    auto item = MenuItemFont::create("Toogle ignore anchor point", CC_CALLBACK_1(LayerIgnoreAnchorPointScale::onToggle, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);

    menu->setPosition(Vec2(s.width/2, s.height/2));
}

void LayerIgnoreAnchorPointScale::onToggle(Ref* /*pObject*/)
{
    auto layer = this->getChildByTag(kLayerIgnoreAnchorPoint);
    bool ignore = layer->isIgnoreAnchorPointForPosition();
    layer->setIgnoreAnchorPointForPosition(! ignore);
}

std::string LayerIgnoreAnchorPointScale::title() const
{
    return "IgnoreAnchorPoint - Scale";
}

std::string LayerIgnoreAnchorPointScale::subtitle() const
{
    return "Ignoring Anchor Vec2 for scale";
}

LayerExtendedBlendOpacityTest::LayerExtendedBlendOpacityTest()
{
    auto layer1 = LayerGradient::create(Color4B(255, 0, 0, 255), Color4B(255, 0, 255, 255));
    layer1->setContentSize(Size(80, 80));
    layer1->setPosition(Vec2(50,50));
    addChild(layer1);
    
    auto layer2 = LayerGradient::create(Color4B(0, 0, 0, 127), Color4B(255, 255, 255, 127));
    layer2->setContentSize(Size(80, 80));
    layer2->setPosition(Vec2(100,90));
    addChild(layer2);
    
    auto layer3 = LayerGradient::create();
    layer3->setContentSize(Size(80, 80));
    layer3->setPosition(Vec2(150,140));
    layer3->setStartColor(Color3B(255, 0, 0));
    layer3->setEndColor(Color3B(255, 0, 255));
    layer3->setStartOpacity(255);
    layer3->setEndOpacity(255);
    layer3->setBlendFunc( BlendFunc::ALPHA_NON_PREMULTIPLIED );
    addChild(layer3);
}

std::string LayerExtendedBlendOpacityTest::title() const
{
    return "Extended Blend & Opacity";
}

std::string LayerExtendedBlendOpacityTest::subtitle() const
{
    return "You should see 3 layers";
}

// LayerBug3162A
void LayerBug3162A::onEnter()
{
    LayerTest::onEnter();
    
    Size size = VisibleRect::getVisibleRect().size;
    size.width = size.width / 2;
    size.height = size.height / 3;
    Color4B color[3] = {Color4B(255, 0, 0, 255), Color4B(0, 255, 0, 255), Color4B(0, 0, 255, 255)};
    
    for (int i = 0; i < 3; ++i)
    {
        _layer[i] = LayerColor::create(color[i]);
        _layer[i]->setContentSize(size);
        _layer[i]->setPosition(Vec2(size.width/2, size.height/2) - Vec2(20, 20));
        _layer[i]->setOpacity(150);
        _layer[i]->setCascadeOpacityEnabled(true);
        if (i > 0)
        {
            _layer[i-1]->addChild(_layer[i]);
        }
    }
    
    this->addChild(_layer[0]);
    
    Director::getInstance()->getScheduler().schedule(
        TimedJob(this, &LayerBug3162A::step, 0).interval(0.5f).delay(0.5f).paused(isPaused())
    );
}

void LayerBug3162A::step(float /*dt*/)
{
    _layer[0]->setCascadeOpacityEnabled(!_layer[0]->isCascadeOpacityEnabled());
}

std::string LayerBug3162A::title() const
{
    return "Bug 3162 red layer cascade opacity eable/disable";
}

std::string LayerBug3162A::subtitle() const
{
    return "g and b layer opacity is effected/diseffected with r layer";
}

// LayerBug3162B
void LayerBug3162B::onEnter()
{
    LayerTest::onEnter();
    
    Size size = VisibleRect::getVisibleRect().size;
    size.width = size.width / 2;
    size.height = size.height / 3;
    Color4B color[3] = {Color4B(200, 0, 0, 255), Color4B(150, 0, 0, 255), Color4B(100, 0, 0, 255)};
    
    for (int i = 0; i < 3; ++i)
    {
        _layer[i] = LayerColor::create(color[i]);
        _layer[i]->setContentSize(size);
        _layer[i]->setPosition(Vec2(size.width/2, size.height/2) - Vec2(20, 20));
        //_layer[i]->setOpacity(150);
        if (i > 0)
        {
            _layer[i-1]->addChild(_layer[i]);
        }
    }
    
    this->addChild(_layer[0]);
    
    _layer[0]->setCascadeColorEnabled(true);
    _layer[1]->setCascadeColorEnabled(true);
    _layer[2]->setCascadeColorEnabled(true);
    
    Director::getInstance()->getScheduler().schedule(
        TimedJob(this, &LayerBug3162B::step, 0).interval(0.5f).delay(0.5f).paused(isPaused())
    );
}

void LayerBug3162B::step(float /*dt*/)
{
    _layer[0]->setCascadeColorEnabled(!_layer[0]->isCascadeColorEnabled());
}

std::string LayerBug3162B::title() const
{
    return "Bug 3162 bottom layer cascade color eable/disable";
}

std::string LayerBug3162B::subtitle() const
{
    return "u and m layer color is effected/diseffected with b layer";
}

std::string LayerColorOccludeBug::title() const
{
    return "Layer Color Occlude Bug Test";
}

std::string LayerColorOccludeBug::subtitle() const
{
    return  "Layer Color Should not occlude titles and any sprites";
}

void LayerColorOccludeBug::onEnter()
{
    LayerTest::onEnter();
    Director::getInstance()->setDepthTest(true);
    _layer = LayerColor::create(Color4B(0, 80, 95, 255));
    addChild(_layer);
}

void LayerColorOccludeBug::onExit()
{
    LayerTest::onExit();
    Director::getInstance()->setDepthTest(false);
}
