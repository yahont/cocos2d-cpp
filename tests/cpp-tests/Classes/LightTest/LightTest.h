/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
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

#ifndef _LIGHT_TEST_H_
#define _LIGHT_TEST_H_

#include "../BaseTest.h"

#include "2d/CCLight.h"

namespace cocos2d {
class AmbientLight;
class DirectionLight;
class PointLight;
class SpotLight;
}

DEFINE_TEST_SUITE(LightTests);

class LightTest : public TestCase
{
public:
    static LightTest* create()
    {
        auto ret = new LightTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    LightTest();
    virtual ~LightTest();

    virtual std::string title() const override;

    virtual void update(float delta) override;

    void SwitchLight(cocos2d::Ref* sender, cocos2d::LightType lightType);

private:

    void addSprite();
    void addLights();

private:

    cocos2d::AmbientLight* _ambientLight;
    cocos2d::DirectionLight* _directionalLight;
    cocos2d::PointLight* _pointLight;
    cocos2d::SpotLight* _spotLight;

    cocos2d::Label* _ambientLightLabel;
    cocos2d::Label* _directionalLightLabel;
    cocos2d::Label* _pointLightLabel;
    cocos2d::Label* _spotLightLabel;
};

#endif

