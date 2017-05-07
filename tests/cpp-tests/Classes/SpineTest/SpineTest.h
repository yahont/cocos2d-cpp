/*******************************************************************************
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef _SPINETEST_H_
#define _SPINETEST_H_

#include "../BaseTest.h"
#include <spine/spine-cocos2dx.h>

DEFINE_TEST_SUITE(SpineTests);

class SpineTestLayer : public TestCase
{
protected:
    std::string    _title;
};

class SpineTestLayerNormal: public SpineTestLayer {
private:
	spine::SkeletonAnimation* skeletonNode;
    
public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "Normal Spine";
    }
	virtual bool init () override;
	virtual void update (float deltaTime) override;
    //    void animationStateEvent (spine::SkeletonAnimation* node, int trackIndex, spEventType type, spEvent* event, int loopCount);
    
	static SpineTestLayerNormal* create()
	{
	    auto ret = new SpineTestLayerNormal;
	    ret->init();
	    ret->autorelease();
	    return ret;
	}
};

class SpineTestLayerFFD: public SpineTestLayer {
private:
	spine::SkeletonAnimation* skeletonNode;
    
public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "FFD Spine";
    }
	virtual bool init () override;
	virtual void update (float deltaTime) override;
    //    void animationStateEvent (spine::SkeletonAnimation* node, int trackIndex, spEventType type, spEvent* event, int loopCount);
    
	static SpineTestLayerFFD* create()
	{
	    auto ret = new SpineTestLayerFFD;
	    ret->init();
	    ret->autorelease();
	    return ret;
	}
};

class SpineTestLayerRapor: public SpineTestLayer
{
public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "Raptor Test";
    }
    virtual bool init () override;
    
    static SpineTestLayerRapor* create()
    {
        auto ret = new SpineTestLayerRapor;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
private:
    spine::SkeletonAnimation* skeletonNode;
};

class SpineTestPerformanceLayer: public SpineTestLayer
{
public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "Performance Test for Spine";
    }
	virtual bool init () override;
	virtual void update (float deltaTime) override;
    //    void animationStateEvent (spine::SkeletonAnimation* node, int trackIndex, spEventType type, spEvent* event, int loopCount);
    
	static SpineTestPerformanceLayer* create()
	{
	    auto ret = new SpineTestPerformanceLayer;
	    ret->init();
	    ret->autorelease();
	    return ret;
	}
};


class SpineTestLayerNormalBin: public SpineTestLayer {
private:
    spine::SkeletonAnimation* skeletonNode;

public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "Normal Spine - Binary file";
    }
    virtual bool init () override;
    virtual void update (float deltaTime) override;
    //    void animationStateEvent (spine::SkeletonAnimation* node, int trackIndex, spEventType type, spEvent* event, int loopCount);

    static SpineTestLayerNormalBin* create()
    {
        auto ret = new SpineTestLayerNormalBin;
        ret->init();
        ret->autorelease();
        return ret;
    }
};

class SpineTestLayerFFDBin: public SpineTestLayer {
private:
    spine::SkeletonAnimation* skeletonNode;

public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "FFD Spine - Binary File";
    }
    virtual bool init () override;
    virtual void update (float deltaTime) override;
    //    void animationStateEvent (spine::SkeletonAnimation* node, int trackIndex, spEventType type, spEvent* event, int loopCount);

    static SpineTestLayerFFDBin* create()
    {
        auto ret = new SpineTestLayerFFDBin;
        ret->init();
        ret->autorelease();
        return ret;
    }
};

class SpineTestLayerRaporBin: public SpineTestLayer
{
public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "Raptor Test - Binary File";
    }
    virtual bool init () override;

    static SpineTestLayerRaporBin* create()
    {
        auto ret = new SpineTestLayerRaporBin;
        ret->init();
        ret->autorelease();
        return ret;
    }

private:
    spine::SkeletonAnimation* skeletonNode;
};

class SpineTestPerformanceLayerBin: public SpineTestLayer
{
public:
    virtual std::string title() const override
    {
        return "Spine Test";
    }
    virtual std::string subtitle() const override
    {
        return "Performance Test for Spine - Binary File";
    }
    virtual bool init () override;
    virtual void update (float deltaTime) override;
    //    void animationStateEvent (spine::SkeletonAnimation* node, int trackIndex, spEventType type, spEvent* event, int loopCount);

    static SpineTestPerformanceLayerBin* create()
    {
        auto ret = new SpineTestPerformanceLayerBin;
        ret->init();
        ret->autorelease();
        return ret;
    }
};

#endif // _EXAMPLELAYER_H_
