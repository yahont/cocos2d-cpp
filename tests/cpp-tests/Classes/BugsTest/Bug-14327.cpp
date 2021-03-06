//
//  Bug-14327.cpp
//  cocos2d_tests
//
//  Issue: https://github.com/cocos2d/cocos2d-x/pull/14327
//  Please test in Windows
//
//

#include "Bug-14327.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

using namespace cocos2d;

bool Bug14327Layer::init()
{
    if (BugsTestBase::init())
    {
        auto glview = Director::getInstance()->getOpenGLView();
        auto visibleOrigin = glview->getVisibleOrigin();
        auto visibleSize = glview->getVisibleSize();

        auto pBg = Sprite::create("Images/HelloWorld.png");
        pBg->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2));
        addChild(pBg);

        _removeTime = time(nullptr) + 20;

        _TTFShowTime = Label::createWithSystemFont("Edit control will be removed after 00:20!", "Arial", 20);
        _TTFShowTime->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 60));
        this->addChild(_TTFShowTime);


        auto editBoxSize = Size(visibleSize.width - 100, visibleSize.height * 0.1);

        std::string pNormalSprite = "extensions/green_edit.png";
        _edit = ui::EditBox::create(editBoxSize + Size(0, 20), ui::Scale9Sprite::create(pNormalSprite));
        _edit->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2));
        _edit->setFontColor(Color3B::RED);
        _edit->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        _edit->setDelegate(this);
        this->addChild(_edit);

        Director::getInstance()->getScheduler().schedule(UpdateJob(this).paused(isPaused()));
        return true;
    }

    return false;
}

void Bug14327Layer::update(float dt)
{
    long delta = _removeTime - time(nullptr);
    if (delta > 0)
    {
        ldiv_t ret = ldiv(delta, 60L);
        char str[100];
        snprintf(str, 100, "%s%.2ld:%.2ld", "Edit control will be removed after ", ret.quot, ret.rem);
        _TTFShowTime->setString(str);
    }
    else
    {
        _edit->removeFromParent();
        _edit = nullptr;
        _TTFShowTime->setString("Edit control has been removed!\nIt should not crash.");
        Director::getInstance()->getScheduler().unscheduleUpdateJob(this);
    }
}

void Bug14327Layer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
    log("editBox %p DidBegin !", editBox);
}

void Bug14327Layer::editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox)
{
    log("editBox %p DidEnd !", editBox);
}

void Bug14327Layer::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
    log("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void Bug14327Layer::editBoxReturn(ui::EditBox* editBox)
{
    log("editBox %p was returned !", editBox);
}

#endif
