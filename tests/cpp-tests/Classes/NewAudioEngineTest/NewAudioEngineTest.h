/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

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

#include "platform/CCPlatformConfig.h"

#ifndef __NEWAUDIOENGINE_TEST_H_
#define __NEWAUDIOENGINE_TEST_H_

#include "../BaseTest.h"

#include "audio/include/AudioEngine.h"

DEFINE_TEST_SUITE(AudioEngineTests);

class AudioEngineTestDemo : public TestCase
{
public:
    AudioEngineTestDemo();
    
    virtual std::string title() const override;
    virtual void onExit() override;
protected:
    std::shared_ptr<bool> _isDestroyed;
};

class AudioControlTest : public AudioEngineTestDemo
{
public:
    static AudioControlTest* create()
    {
        auto ret = new AudioControlTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual ~AudioControlTest();
    
    virtual bool init() override;
    
    virtual void update(float dt) override;
    
    virtual std::string title() const override;
    
private:
    int _audioID;
    bool _loopEnabled;
    float _volume;
    float _duration;
    float _timeRatio;
    
    void* _playItem;
    void* _timeSlider;
    bool _updateTimeSlider;
    bool _isStopped;
    cocos2d::Label* _playOverLabel;
};

class PlaySimultaneouslyTest : public AudioEngineTestDemo
{
public:
    static PlaySimultaneouslyTest* create()
    {
        auto ret = new PlaySimultaneouslyTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual ~PlaySimultaneouslyTest();
    
    virtual bool init() override;
    
    virtual std::string title() const override;
private:
    static const int TEST_COUNT = 10;
    std::string _files[TEST_COUNT];
    
    void* _playItem;
    int _playingcount;
};

class AudioProfileTest : public AudioEngineTestDemo
{
public:
    static AudioProfileTest* create()
    {
        auto ret = new AudioProfileTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual ~AudioProfileTest();
    
    virtual bool init() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    virtual void update(float dt) override;
    
private:
    static const int FILE_COUNT = 2;
    std::string _files[FILE_COUNT];
    cocos2d::experimental::AudioProfile _audioProfile;
    
    int _audioCount;
    cocos2d::Label* _showLabel;
    float _time;
    float _minDelay;
    void* _timeSlider;
};

class InvalidAudioFileTest : public AudioEngineTestDemo
{
public:
    static InvalidAudioFileTest* create()
    {
        auto ret = new InvalidAudioFileTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual ~InvalidAudioFileTest();
    
    virtual bool init() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
private:
   
};

class LargeAudioFileTest : public AudioEngineTestDemo
{
public:
    static LargeAudioFileTest* create()
    {
        auto ret = new LargeAudioFileTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual ~LargeAudioFileTest();
    
    virtual bool init() override;
    
    virtual std::string title() const override;
    
private:
    
};

class AudioLoadTest : public AudioEngineTestDemo
{
public:
    static AudioLoadTest* create()
    {
        auto ret = new AudioLoadTest;
        ret->init();
        ret->autorelease();
        return ret;
    }

    virtual bool init() override;

    virtual std::string title() const override;
};

class AudioIssue11143Test : public AudioEngineTestDemo
{
public:
    static AudioIssue11143Test* create()
    {
        auto ret = new AudioIssue11143Test;
        ret->init();
        ret->autorelease();
        return ret;
    }

    virtual bool init() override;

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AudioPerformanceTest : public AudioEngineTestDemo
{
public:
    static AudioPerformanceTest* create()
    {
        auto ret = new AudioPerformanceTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual bool init() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AudioSwitchStateTest : public AudioEngineTestDemo
{
public:
    static AudioSwitchStateTest* create()
    {
        auto ret = new AudioSwitchStateTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual bool init() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AudioSmallFileTest : public AudioEngineTestDemo
{
public:
    static AudioSmallFileTest* create()
    {
        auto ret = new AudioSmallFileTest;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual bool init() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AudioPauseResumeAfterPlay : public AudioEngineTestDemo
{
public:
    static AudioPauseResumeAfterPlay* create()
    {
        auto ret = new AudioPauseResumeAfterPlay;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual void onEnter() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AudioPreloadSameFileMultipleTimes : public AudioEngineTestDemo
{
public:
    static AudioPreloadSameFileMultipleTimes* create()
    {
        auto ret = new AudioPreloadSameFileMultipleTimes;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual void onEnter() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class AudioPlayFileInWritablePath : public AudioEngineTestDemo
{
public:
    static AudioPlayFileInWritablePath* create()
    {
        auto ret = new AudioPlayFileInWritablePath;
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual void onEnter() override;
    virtual void onExit() override;
    
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
private:
    std::vector<std::string> _oldSearchPaths;
};

#endif /* defined(__NEWAUDIOENGINE_TEST_H_) */
