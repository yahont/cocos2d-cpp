/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2017 Iakov Sergeev <yahont@github>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "NewRendererTest.h"

#include "2d/CCCamera.h"
#include "2d/CCClippingNode.h"
#include "2d/CCLabel.h"
#include "2d/CCLabelAtlas.h"
#include "2d/CCMenu.h"
#include "2d/CCMenuItem.h"
#include "2d/CCSprite.h"
#include "2d/CCFastTMXTiledMap.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/ccUtils.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGroupCommand.h"
#include "renderer/CCRenderer.h"
#include "renderer/ccShaders.h"
#include "renderer/CCTextureCache.h"
#include "platform/CCFileUtils.h"

using namespace cocos2d;

NewRendererTests::NewRendererTests()
{
    ADD_TEST_CASE(NewSpriteTest);
    ADD_TEST_CASE(GroupCommandTest);
    ADD_TEST_CASE(NewClippingNodeTest);
    ADD_TEST_CASE(NewDrawNodeTest);
    ADD_TEST_CASE(NewCullingTest);
    ADD_TEST_CASE(VBOFullTest);
    ADD_TEST_CASE(CaptureScreenTest);
    ADD_TEST_CASE(CaptureNodeTest);
    ADD_TEST_CASE(BugAutoCulling);
    ADD_TEST_CASE(RendererBatchQuadTri);
    ADD_TEST_CASE(RendererUniformBatch);
    ADD_TEST_CASE(RendererUniformBatch2);
};

std::string MultiSceneTest::title() const
{
    return "New Renderer";
}

std::string MultiSceneTest::subtitle() const
{
    return "MultiSceneTest";
}

NewSpriteTest::NewSpriteTest()
{
    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesEnded = CC_CALLBACK_2(NewSpriteTest::onTouchesEnded, this);

    createSpriteTest();
    createNewSpriteTest();
}

NewSpriteTest::~NewSpriteTest()
{

}

void NewSpriteTest::createSpriteTest()
{
    Size winSize = Director::getInstance()->getWinSize();

    Sprite* parent = Sprite::create("Images/grossini.png");
    parent->setPosition(winSize.width/4, winSize.height/2);
    Sprite* child1 = Sprite::create("Images/grossinis_sister1.png");
    child1->setPosition(0.0f, -20.0f);
    Sprite* child2 = Sprite::create("Images/grossinis_sister2.png");
    child2->setPosition(20.0f, -20.0f);
    Sprite* child3 = Sprite::create("Images/grossinis_sister1.png");
    child3->setPosition(40.0f, -20.0f);
    Sprite* child4 = Sprite::create("Images/grossinis_sister2.png");
    child4->setPosition(60.0f, -20.0f);
    Sprite* child5 = Sprite::create("Images/grossinis_sister2.png");
    child5->setPosition(80.0f, -20.0f);
    Sprite* child6 = Sprite::create("Images/grossinis_sister2.png");
    child6->setPosition(100.0f, -20.0f);
    Sprite* child7 = Sprite::create("Images/grossinis_sister2.png");
    child7->setPosition(120.0f, -20.0f);

    parent->addChild(child1);
    parent->addChild(child2);
    parent->addChild(child3);
    parent->addChild(child4);
    parent->addChild(child5);
    parent->addChild(child6);
    parent->addChild(child7);
    addChild(parent);
}

void NewSpriteTest::createNewSpriteTest()
{
    Size winSize = Director::getInstance()->getWinSize();

    Sprite* parent = Sprite::create("Images/grossini.png");
    parent->setPosition(winSize.width*2/3, winSize.height/2);
    Sprite* child1 = Sprite::create("Images/grossinis_sister1.png");
    child1->setPosition(0.0f, -20.0f);
    Sprite* child2 = Sprite::create("Images/grossinis_sister2.png");
    child2->setPosition(20.0f, -20.0f);
    Sprite* child3 = Sprite::create("Images/grossinis_sister1.png");
    child3->setPosition(40.0f, -20.0f);
    Sprite* child4 = Sprite::create("Images/grossinis_sister2.png");
    child4->setPosition(60.0f, -20.0f);
    Sprite* child5 = Sprite::create("Images/grossinis_sister2.png");
    child5->setPosition(80.0f, -20.0f);
    Sprite* child6 = Sprite::create("Images/grossinis_sister2.png");
    child6->setPosition(100.0f, -20.0f);
    Sprite* child7 = Sprite::create("Images/grossinis_sister2.png");
    child7->setPosition(120.0f, -20.0f);

    parent->addChild(child1);
    parent->addChild(child2);
    parent->addChild(child3);
    parent->addChild(child4);
    parent->addChild(child5);
    parent->addChild(child6);
    parent->addChild(child7);
    addChild(parent);
}

void NewSpriteTest::onTouchesEnded(const std::vector<Touch *> &, Event*)
{

}

std::string NewSpriteTest::title() const
{
    return "Renderer";
}

std::string NewSpriteTest::subtitle() const
{
    return "SpriteTest";
}

class SpriteInGroupCommand : public Sprite
{
protected:
    GroupCommand _spriteWrapperCommand;
public:
    static SpriteInGroupCommand* create(const std::string& filename);
    
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
};

SpriteInGroupCommand* SpriteInGroupCommand::create(const std::string &filename)
{
    SpriteInGroupCommand* sprite = new (std::nothrow) SpriteInGroupCommand();
    sprite->initWithFile(filename);
    sprite->autorelease();
    return sprite;
}

void SpriteInGroupCommand::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    CCASSERT(renderer, "Render is null");
    _spriteWrapperCommand.init(_globalZOrder);
    renderer->addCommand(&_spriteWrapperCommand);
    renderer->pushGroup(_spriteWrapperCommand.getRenderQueueID());
    Sprite::draw(renderer, transform, flags);
    renderer->popGroup();
}

GroupCommandTest::GroupCommandTest()
{
    auto sprite = SpriteInGroupCommand::create("Images/grossini.png");
    Size winSize = Director::getInstance()->getWinSize();
    sprite->setPosition(winSize.width/2,winSize.height/2);
    addChild(sprite);
}

GroupCommandTest::~GroupCommandTest()
{
}

std::string GroupCommandTest::title() const
{
    return "Renderer";
}

std::string GroupCommandTest::subtitle() const
{
    return "GroupCommandTest: You should see a sprite";
}

NewClippingNodeTest::NewClippingNodeTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto clipper = ClippingNode::create();
    clipper->setTag( kTagClipperNode );
    clipper->setContentSize(  Size(200, 200) );
    clipper->setAnchorPoint(  Vec2(0.5, 0.5) );
    clipper->setPosition( Vec2(s.width / 2, s.height / 2) );

    clipper->runAction( std::make_unique<RepeatForever>( std::make_unique<RotateBy>(1, 45)));
    this->addChild(clipper);

    //Test with alpha Test
    clipper->setAlphaThreshold(0.05f);
    auto stencil = Sprite::create("Images/grossini.png");
    stencil->setPosition(s.width/2, s.height/2);
    clipper->setStencil(stencil);

    auto content = Sprite::create("Images/background2.png");
    content->setTag( kTagContentNode );
    content->setAnchorPoint(  Vec2(0.5, 0.5) );
    content->setPosition( Vec2(clipper->getContentSize().width / 2, clipper->getContentSize().height / 2) );
    clipper->addChild(content);

    _scrolling = false;

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(NewClippingNodeTest::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(NewClippingNodeTest::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(NewClippingNodeTest::onTouchesEnded, this);
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

NewClippingNodeTest::~NewClippingNodeTest()
{

}

std::string NewClippingNodeTest::title() const
{
    return "New Render";
}

std::string NewClippingNodeTest::subtitle() const
{
    return "ClipNode";
}

void NewClippingNodeTest::onTouchesBegan(const std::vector<Touch *> &touches, Event *)
{
    Touch *touch = touches[0];
    auto clipper = this->getChildByTag(kTagClipperNode);
    Vec2 point = clipper->convertToNodeSpace(Director::getInstance()->convertToGL(touch->getLocationInView()));
    auto rect = Rect(0, 0, clipper->getContentSize().width, clipper->getContentSize().height);
    _scrolling = rect.containsPoint(point);
    _lastPoint = point;
}

void NewClippingNodeTest::onTouchesMoved(const std::vector<Touch *> &touches, Event *)
{
    if (!_scrolling) return;
    Touch *touch = touches[0];
    auto clipper = this->getChildByTag(kTagClipperNode);
    auto point = clipper->convertToNodeSpace(Director::getInstance()->convertToGL(touch->getLocationInView()));
    Vec2 diff = point - _lastPoint;
    auto content = clipper->getChildByTag(kTagContentNode);
    content->setPosition(content->getPosition() + diff);
    _lastPoint = point;
}

void NewClippingNodeTest::onTouchesEnded(const std::vector<Touch *> &, Event *)
{
    if (!_scrolling) return;
    _scrolling = false;
}

/**
* NewDrawNode
*/
NewDrawNodeTest::NewDrawNodeTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto parent = Node::create();
    parent->setPosition(s.width/2, s.height/2);
    addChild(parent);

    auto rectNode = DrawNode::create();
    Vec2 rectangle[4];
    rectangle[0] = Vec2(-50, -50);
    rectangle[1] = Vec2(50, -50);
    rectangle[2] = Vec2(50, 50);
    rectangle[3] = Vec2(-50, 50);

    Color4F white(1, 1, 1, 1);
    rectNode->drawPolygon(rectangle, 4, white, 1, white);
    parent->addChild(rectNode);
}

NewDrawNodeTest::~NewDrawNodeTest()
{

}

std::string NewDrawNodeTest::title() const
{
    return "New Render";
}

std::string NewDrawNodeTest::subtitle() const
{
    return "DrawNode";
}

NewCullingTest::NewCullingTest()
{
    Size size = Director::getInstance()->getWinSize();
    auto sprite = Sprite::create("Images/btn-about-normal-vertical.png");
    sprite->setRotation(5);
    sprite->setPosition(Vec2(size.width/2,size.height/3));
    sprite->setScale(2);
    addChild(sprite);

    auto sprite2 = Sprite::create("Images/btn-about-normal-vertical.png");
    sprite2->setRotation(-85);
    sprite2->setPosition(Vec2(size.width/2,size.height * 2/3));
    sprite2->setScale(2);
    addChild(sprite2);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(NewCullingTest::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(NewCullingTest::onTouchMoved, this);
    
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
}

bool NewCullingTest::onTouchBegan(Touch* touch, Event*)
{
    auto pos = touch->getLocation();
    _lastPos = pos;
    return true;
}

void NewCullingTest::onTouchMoved(Touch* touch, Event*)
{    
    auto pos = touch->getLocation();
    
    auto offset = pos - _lastPos;
    
    auto layerPos = getPosition();
    auto newPos = layerPos + offset;
    
    setPosition(newPos);
    _lastPos = pos;
}

NewCullingTest::~NewCullingTest()
{
    
}

std::string NewCullingTest::title() const
{
    return "New Render";
}

std::string NewCullingTest::subtitle() const
{
    return "Drag the layer to test the result of culling";
}

VBOFullTest::VBOFullTest()
{
    Size s = Director::getInstance()->getWinSize();
    Node* parent = Node::create();
    parent->setPosition(0,0);
    addChild(parent);
    
    for (int i=0; i< Renderer::VBO_SIZE / 3.9; ++i)
    {
        Sprite* sprite = Sprite::create("Images/grossini_dance_01.png");
        sprite->setScale(0.1f, 0.1f);
        float x = ((float)std::rand()) /RAND_MAX;
        float y = ((float)std::rand()) /RAND_MAX;
        sprite->setPosition(Vec2(x * s.width, y * s.height));
        parent->addChild(sprite);
    }
}

VBOFullTest::~VBOFullTest()
{
    
}

std::string VBOFullTest::title() const
{
    return "New Renderer";
}

std::string VBOFullTest::subtitle() const
{
    return "VBO full Test, everthing should render normally";
}

CaptureScreenTest::CaptureScreenTest()
{
    Size s = Director::getInstance()->getWinSize();
    Vec2 left(s.width / 4, s.height / 2);
    Vec2 right(s.width / 4 * 3, s.height / 2);
	
    auto sp1 = Sprite::create("Images/grossini.png");
    sp1->setPosition(left);
    auto move1 = std::make_unique<MoveBy>(1, Vec2(s.width/2, 0));
    auto move1_reverse = std::unique_ptr<MoveBy>(move1->reverse());

    auto seq1 = std::make_unique<RepeatForever>(
        std::make_unique<Sequence>(
            std::move(move1),
            std::move(move1_reverse)
        )
    );
    addChild(sp1);
    sp1->runAction( std::move( seq1));
    auto sp2 = Sprite::create("Images/grossinis_sister1.png");
    sp2->setPosition(right);
    auto move2 = std::make_unique<MoveBy>(1, Vec2(-s.width/2, 0));
    auto move2_reverse = std::unique_ptr<MoveBy>(move2->reverse());

    auto seq2 = std::make_unique<RepeatForever>(
        std::make_unique<Sequence>(
            std::move(move2),
            std::move(move2_reverse)
        )
    );
    addChild(sp2);
    sp2->runAction( std::move( seq2));

    auto label1 = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "capture all");
    auto mi1 = MenuItemLabel::create(label1, CC_CALLBACK_1(CaptureScreenTest::onCaptured, this));
    auto menu = Menu::create(mi1, nullptr);
    addChild(menu);
    menu->setPosition(s.width / 2, s.height / 4);

    _filename = "";
}

CaptureScreenTest::~CaptureScreenTest()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
}

std::string CaptureScreenTest::title() const
{
    return "New Renderer";
}

std::string CaptureScreenTest::subtitle() const
{
    return "Capture screen test, press the menu items to capture the screen";
}

void CaptureScreenTest::onCaptured(Ref*)
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
    removeChildByTag(childTag);
    _filename = "CaptureScreenTest.png";
    utils::captureScreen(CC_CALLBACK_2(CaptureScreenTest::afterCaptured, this), _filename);
}

void CaptureScreenTest::afterCaptured(bool succeed, const std::string& outputFile)
{
    if (succeed)
    {
        auto sp = Sprite::create(outputFile);
        addChild(sp, 0, childTag);
        Size s = Director::getInstance()->getWinSize();
        sp->setPosition(s.width / 2, s.height / 2);
        sp->setScale(0.25);
        _filename = outputFile;
    }
    else
    {
        log("Capture screen failed.");
    }
}

CaptureNodeTest::CaptureNodeTest()
{
    Size s = Director::getInstance()->getWinSize();
    Vec2 left(s.width / 4, s.height / 2);
    Vec2 right(s.width / 4 * 3, s.height / 2);

    auto sp1 = Sprite::create("Images/grossini.png");
    sp1->setPosition(left);
    auto move1 = std::make_unique<MoveBy>(1, Vec2(s.width / 2, 0));
    auto move1_reverse = std::unique_ptr<MoveBy>(move1->reverse());
    auto seq1 = std::make_unique<RepeatForever>(
        std::make_unique<Sequence>(
            std::move(move1),
            std::move(move1_reverse)
        )
    );
    addChild(sp1);
    sp1->runAction( std::move( seq1));
    auto sp2 = Sprite::create("Images/grossinis_sister1.png");
    sp2->setPosition(right);
    auto move2 = std::make_unique<MoveBy>(1, Vec2(-s.width / 2, 0));
    auto move2_reverse = std::unique_ptr<MoveBy>(move2->reverse());
    auto seq2 = std::make_unique<RepeatForever>(
        std::make_unique<Sequence>(
            std::move(move2),
            std::move(move2_reverse)
        )
    );
    addChild(sp2);
    sp2->runAction( std::move( seq2));

    auto label1 = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "capture this scene");
    auto mi1 = MenuItemLabel::create(label1, CC_CALLBACK_1(CaptureNodeTest::onCaptured, this));
    auto menu = Menu::create(mi1, nullptr);
    addChild(menu);
    menu->setPosition(s.width / 2, s.height / 4);

    _filename = "";
}

CaptureNodeTest::~CaptureNodeTest()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
}

std::string CaptureNodeTest::title() const
{
    return "New Renderer";
}

std::string CaptureNodeTest::subtitle() const
{
    return "Capture node test, press the menu items to capture this scene with scale 0.5";
}

void CaptureNodeTest::onCaptured(Ref*)
{ 
    Director::getInstance()->getTextureCache()->removeTextureForKey(_filename);
    removeChildByTag(childTag);
    
    _filename = FileUtils::getInstance()->getWritablePath() + "/CaptureNodeTest.png";

    // capture this
    auto image = utils::captureNode(this, 0.5);

    // create a sprite with the captured image directly
    auto sp = Sprite::create(Director::getInstance()->getTextureCache()->addImage(image, _filename));
    addChild(sp, 0, childTag);
    Size s = Director::getInstance()->getWinSize();
    sp->setPosition(s.width / 2, s.height / 2);

    // store to disk
    image->saveToFile(_filename);

    // release the captured image
    image->release();
}

BugAutoCulling::BugAutoCulling()
{
    Size s = Director::getInstance()->getWinSize();
    auto fastmap = cocos2d::experimental::TMXTiledMap::create("TileMaps/orthogonal-test2.tmx");
    this->addChild(fastmap);
    for (int i = 0; i < 30; i++) {
        auto sprite = Sprite::create("Images/grossini.png");
        sprite->setPosition(s.width/2 + s.width/10 * i, s.height/2);
        this->addChild(sprite);
        auto label = Label::createWithTTF(TTFConfig("fonts/arial.ttf"), "Label");
        label->setPosition(s.width/2 + s.width/10 * i, s.height/2);
        this->addChild(label);
    }

    auto lambda_autoculling_bug = [=](float){
        auto camera = Director::getInstance()->getRunningScene()->getCameras().front();
        auto move  = std::make_unique<MoveBy>(2.0, Vec2(2 * s.width, 0));
        auto move_reverse = std::unique_ptr<MoveBy>(move->reverse());
        camera->runAction(
            std::make_unique<Sequence>(
                std::move(move),
                std::move(move_reverse)
            )
        );
    };

    Director::getInstance()->getScheduler().schedule(
        TimedJob(this, lambda_autoculling_bug, 0)
            .delay(1.0f)
            .repeat(0)
            .paused(isPaused())
    );
}

std::string BugAutoCulling::title() const
{
    return "Bug-AutoCulling";
}

std::string BugAutoCulling::subtitle() const
{
    return "Moving the camera to the right instead of moving the layer";
}

//
// RendererBatchQuadTri
//

RendererBatchQuadTri::RendererBatchQuadTri()
{
    Size s = Director::getInstance()->getWinSize();

    for (int i=0; i<250; i++)
    {
        int x = CCRANDOM_0_1() * s.width;
        int y = CCRANDOM_0_1() * s.height;

        auto label = LabelAtlas::create("This is a label", "fonts/tuffy_bold_italic-charmap.plist");
        label->setColor(Color3B::RED);
        label->setPosition(Vec2(x,y));
        addChild(label);

        auto sprite = Sprite::create("fonts/tuffy_bold_italic-charmap.png");
        sprite->setTextureRect(Rect(0,0,100,100));
        sprite->setPosition(Vec2(x,y));
        sprite->setColor(Color3B::BLUE);
        addChild(sprite);
    }
}

std::string RendererBatchQuadTri::title() const
{
    return "RendererBatchQuadTri";
}

std::string RendererBatchQuadTri::subtitle() const
{
    return "QuadCommand and TriangleCommands are batched together";
}


//
//
// RendererUniformBatch
//

RendererUniformBatch::RendererUniformBatch()
{
    Size s = Director::getInstance()->getWinSize();

    auto glBlurState = createBlurGLProgramState();
    auto glSepiaState = createSepiaGLProgramState();

    auto x_inc = s.width / 20;
    auto y_inc = s.height / 6;

    for (int y=0; y<6; ++y)
    {
        for (int x=0; x<20; ++x)
        {
            auto sprite = Sprite::create("Images/grossini.png");
            sprite->setPosition(Vec2(x * x_inc, y * y_inc));
            sprite->setScale(0.4);
            addChild(sprite);

            if (y>=4) {
                sprite->setGLProgramState(glSepiaState);
            } else if(y>=2) {
                sprite->setGLProgramState(glBlurState);
            }
        }
    }
}

GLProgramState* RendererUniformBatch::createBlurGLProgramState()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
    const std::string shaderName("Shaders/example_Blur.fsh");
#else
    const std::string shaderName("Shaders/example_Blur_winrt.fsh");
#endif
    // outline shader
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename(shaderName);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
    auto glprogramstate = (glprogram == nullptr ? nullptr : GLProgramState::getOrCreateWithGLProgram(glprogram));

    glprogramstate->setUniformVec2("resolution", Vec2(85,121));
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
    glprogramstate->setUniformFloat("blurRadius", 10);
    glprogramstate->setUniformFloat("sampleNum", 5);
#endif

    return glprogramstate;
}

GLProgramState* RendererUniformBatch::createSepiaGLProgramState()
{
    const std::string shaderName("Shaders/example_Sepia.fsh");

    // outline shader
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename(shaderName);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
    auto glprogramstate = (glprogram == nullptr ? nullptr : GLProgramState::getOrCreateWithGLProgram(glprogram));

    return glprogramstate;
}

std::string RendererUniformBatch::title() const
{
    return "RendererUniformBatch";
}

std::string RendererUniformBatch::subtitle() const
{
    return "Only 9 draw calls should appear";
}


//
// RendererUniformBatch2
//

RendererUniformBatch2::RendererUniformBatch2()
{
    Size s = Director::getInstance()->getWinSize();

    auto glBlurState = createBlurGLProgramState();
    auto glSepiaState = createSepiaGLProgramState();

    auto x_inc = s.width / 20;
    auto y_inc = s.height / 6;

    for (int y=0; y<6; ++y)
    {
        for (int x=0; x<20; ++x)
        {
            auto sprite = Sprite::create("Images/grossini.png");
            sprite->setPosition(Vec2(x * x_inc, y * y_inc));
            sprite->setScale(0.4);
            addChild(sprite);

            auto r = CCRANDOM_0_1();
            if (r < 0.33)
                sprite->setGLProgramState(glSepiaState);
            else if (r < 0.66)
                sprite->setGLProgramState(glBlurState);
        }
    }
}

GLProgramState* RendererUniformBatch2::createBlurGLProgramState()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
    const std::string shaderName("Shaders/example_Blur.fsh");
#else
    const std::string shaderName("Shaders/example_Blur_winrt.fsh");
#endif
    // outline shader
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename(shaderName);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
    auto glprogramstate = (glprogram == nullptr ? nullptr : GLProgramState::getOrCreateWithGLProgram(glprogram));

    glprogramstate->setUniformVec2("resolution", Vec2(85,121));
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
    glprogramstate->setUniformFloat("blurRadius", 10);
    glprogramstate->setUniformFloat("sampleNum", 5);
#endif

    return glprogramstate;
}

GLProgramState* RendererUniformBatch2::createSepiaGLProgramState()
{
    const std::string shaderName("Shaders/example_Sepia.fsh");

    // outline shader
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename(shaderName);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
    auto glprogramstate = (glprogram == nullptr ? nullptr : GLProgramState::getOrCreateWithGLProgram(glprogram));

    return glprogramstate;
}

std::string RendererUniformBatch2::title() const
{
    return "RendererUniformBatch 2";
}

std::string RendererUniformBatch2::subtitle() const
{
    return "Mixing different shader states should work ok";
}
