﻿/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2013 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017      Iakov Sergeev <yahont@github>

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

// cocos2d includes
#include "base/CCDirector.h"

// standard includes
#include <string>

#include "platform/CCFileUtils.h"

#include "2d/CCActionManager.h"
#include "2d/CCFontFNT.h"
#include "2d/CCFontAtlasCache.h"
#include "2d/CCAnimationCache.h"
#include "2d/CCTransition.h"
#include "2d/CCFontFreeType.h"
#include "2d/CCLabelAtlas.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCGLProgramStateCache.h"
#include "renderer/CCTextureCache.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCRenderState.h"
#include "renderer/CCFrameBuffer.h"
#include "2d/CCCamera.h"
#include "base/CCUserDefault.h"
#include "base/ccFPSImages.h"
#include "base/CCScheduler.h"
#include "base/ccMacros.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventCustom.h"
#include "base/CCConsole.h"
#include "base/CCAutoreleasePool.h"
#include "base/CCConfiguration.h"
#include "base/CCAsyncTaskPool.h"
#include "platform/CCApplication.h"

/**
 Position of the FPS
 
 Default: 0,0 (bottom-left corner)
 */
#ifndef CC_DIRECTOR_STATS_POSITION
#define CC_DIRECTOR_STATS_POSITION Director::getInstance()->getVisibleOrigin()
#endif // CC_DIRECTOR_STATS_POSITION

using namespace std;

namespace cocos2d {
// FIXME: it should be a Director ivar. Move it there once support for multiple directors is added

// singleton stuff
static Director *s_SharedDirector = nullptr;

#define kDefaultFPS        60  // 60 frames per second
extern const char* cocos2dVersion(void);

const char *Director::EVENT_PROJECTION_CHANGED = "director_projection_changed";
const char *Director::EVENT_AFTER_DRAW = "director_after_draw";
const char *Director::EVENT_AFTER_VISIT = "director_after_visit";
const char *Director::EVENT_BEFORE_UPDATE = "director_before_update";
const char *Director::EVENT_AFTER_UPDATE = "director_after_update";
const char *Director::EVENT_RESET = "director_reset";

Director* Director::getInstance()
{
    if (!s_SharedDirector)
    {
        s_SharedDirector = new (std::nothrow) Director;
        CCASSERT(s_SharedDirector, "FATAL: Not enough memory");
    }

    return s_SharedDirector;
}

Director::Director()
: _runningScene()
, _isStatusLabelUpdated(true)
, _invalid(true)
{
    setDefaultValues();

    _runningScene.reset();

    _notificationNode = nullptr;

    // FPS
    _accumDt = 0.0f;
    _frameRate = 0.0f;
    _FPSLabel = _drawnBatchesLabel = _drawnVerticesLabel = nullptr;
    _totalFrames = 0;
    _lastUpdate = std::chrono::steady_clock::now();
    _secondsPerFrame = 1.0f;

    // paused ?
    _paused = false;

    // purge ?
    _purgeDirectorInNextLoop = false;
    
    // restart ?
    _restartDirectorInNextLoop = false;
    
    // invalid ?
    _invalid = false;

    _winSizeInPoints = Size::ZERO;

    _openGLView = nullptr;
    _defaultFBO = nullptr;
    
    _contentScaleFactor = 1.0f;

    _console = new (std::nothrow) Console;

    _actionManager.reset(new ActionManager);

    _scheduler.schedule(
        UpdateJob(_actionManager.get(), UpdateJob::SYSTEM_PRIORITY)
            .paused(false)
    );

    _eventDispatcher = new (std::nothrow) EventDispatcher();
    _eventAfterDraw = new (std::nothrow) EventCustom(EVENT_AFTER_DRAW);
    _eventAfterDraw->setUserData(this);
    _eventAfterVisit = new (std::nothrow) EventCustom(EVENT_AFTER_VISIT);
    _eventAfterVisit->setUserData(this);
    _eventBeforeUpdate = new (std::nothrow) EventCustom(EVENT_BEFORE_UPDATE);
    _eventBeforeUpdate->setUserData(this);
    _eventAfterUpdate = new (std::nothrow) EventCustom(EVENT_AFTER_UPDATE);
    _eventAfterUpdate->setUserData(this);
    _eventProjectionChanged = new (std::nothrow) EventCustom(EVENT_PROJECTION_CHANGED);
    _eventProjectionChanged->setUserData(this);
    _eventResetDirector = new (std::nothrow) EventCustom(EVENT_RESET);
    //init TextureCache
    initTextureCache();
    initMatrixStack();

    _renderer = new (std::nothrow) Renderer;
    RenderState::initialize();
}

Director::~Director()
{
    CCLOGINFO("deallocing Director: %p", this);

    CC_SAFE_RELEASE(_FPSLabel);
    CC_SAFE_RELEASE(_drawnVerticesLabel);
    CC_SAFE_RELEASE(_drawnBatchesLabel);

    _runningScene.reset();
    CC_SAFE_RELEASE(_notificationNode);
    _scheduler.unscheduleUpdateJob( _actionManager.get() );
    _scheduler.unscheduleAll();
    CC_SAFE_RELEASE(_defaultFBO);
    
    delete _eventBeforeUpdate;
    delete _eventAfterUpdate;
    delete _eventAfterDraw;
    delete _eventAfterVisit;
    delete _eventProjectionChanged;
    delete _eventResetDirector;

    delete _renderer;

    delete _console;


    CC_SAFE_RELEASE(_eventDispatcher);
    
    Configuration::destroyInstance();

    s_SharedDirector = nullptr;
}

void Director::setDefaultValues()
{
    Configuration *conf = Configuration::getInstance();

    // default FPS
    double fps = conf->getValue("cocos2d.x.fps", Value(kDefaultFPS)).asDouble();
    _oldAnimationInterval = _animationInterval = 1.0 / fps;

    // Display FPS
    _displayStats = conf->getValue("cocos2d.x.display_fps", Value(false)).asBool();

    // GL projection
    std::string projection = conf->getValue("cocos2d.x.gl.projection", Value("3d")).asString();
    if (projection == "3d")
        _projection = Projection::_3D;
    else if (projection == "2d")
        _projection = Projection::_2D;
    else
        CCASSERT(false, "Invalid projection value");

    // Default pixel format for PNG images with alpha
    std::string pixel_format = conf->getValue("cocos2d.x.texture.pixel_format_for_png", Value("rgba8888")).asString();
    if (pixel_format == "rgba8888")
        Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA8888);
    else if(pixel_format == "rgba4444")
        Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
    else if(pixel_format == "rgba5551")
        Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGB5A1);

    // PVR v2 has alpha premultiplied ?
    bool pvr_alpha_premultiplied = conf->getValue("cocos2d.x.texture.pvrv2_has_alpha_premultiplied", Value(false)).asBool();
    Image::setPVRImagesHavePremultipliedAlpha(pvr_alpha_premultiplied);
}

void Director::setGLDefaultValues()
{
    // This method SHOULD be called only after openGLView_ was initialized
    CCASSERT(_openGLView, "opengl view should not be null");

    setAlphaBlending(true);
    setDepthTest(false);
    setProjection(_projection);
}

// Draw the Scene
void Director::drawScene()
{
    // calculate "global" dt
    calculateDeltaTime();
    
    if (_openGLView)
    {
        _openGLView->pollEvents();
    }

    //tick before glClear: issue #533
    if (! _paused)
    {
        _eventDispatcher->dispatchEvent(_eventBeforeUpdate);
        _scheduler.update(_deltaTime);
        _eventDispatcher->dispatchEvent(_eventAfterUpdate);
    }

    _renderer->clear();
    experimental::FrameBuffer::clearAllFBOs();

    // if another Scene on the top of the stack is available
    if (_scenesStack.back())
    {
        setNextScene();
    }

    pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    
    if (_runningScene)
    {
#if (CC_USE_PHYSICS || (CC_USE_3D_PHYSICS && CC_ENABLE_BULLET_INTEGRATION) || CC_USE_NAVMESH)
        _runningScene->stepPhysicsAndNavigation(_deltaTime);
#endif
        //clear draw stats
        _renderer->clearDrawStats();
        
        //render the scene
        _openGLView->renderScene(_runningScene.get(), _renderer);
        
        _eventDispatcher->dispatchEvent(_eventAfterVisit);
    }

    // draw the notifications node
    if (_notificationNode)
    {
        _notificationNode->visit(_renderer, Mat4::IDENTITY, 0);
    }

    if (_displayStats)
    {
        showStats();
    }
    _renderer->render();

    _eventDispatcher->dispatchEvent(_eventAfterDraw);

    popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    _totalFrames++;

    // swap buffers
    if (_openGLView)
    {
        _openGLView->swapBuffers();
    }

    if (_displayStats)
    {
        calculateMPF();
    }
}

void Director::calculateDeltaTime()
{
    auto now = std::chrono::steady_clock::now();

    // new delta time. Re-fixed issue #1277
    if (_nextDeltaTimeZero)
    {
        _deltaTime = 0;
        _nextDeltaTimeZero = false;
    }
    else
    {
        _deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;
        _deltaTime = MAX(0, _deltaTime);
    }

#if COCOS2D_DEBUG
    // If we are debugging our code, prevent big delta time
    if (_deltaTime > 0.2f)
    {
        _deltaTime = 1 / 60.0f;
    }
#endif

    _lastUpdate = now;
}
float Director::getDeltaTime() const
{
    return _deltaTime;
}
void Director::setOpenGLView(GLView *openGLView)
{
    CCASSERT(openGLView, "opengl view should not be null");

    if (_openGLView != openGLView)
    {
        // Configuration. Gather GPU info
        Configuration *conf = Configuration::getInstance();
        conf->gatherGPUInfo();
        CCLOG("%s\n",conf->getInfo().c_str());

        if(_openGLView)
            _openGLView->release();
        _openGLView = openGLView;
        _openGLView->retain();

        // set size
        _winSizeInPoints = _openGLView->getDesignResolutionSize();

        _isStatusLabelUpdated = true;

        if (_openGLView)
        {
            setGLDefaultValues();
        }

        _renderer->initGLView();

        CHECK_GL_ERROR_DEBUG();

        if (_eventDispatcher)
        {
            _eventDispatcher->setEnabled(true);
        }
        
        _defaultFBO = experimental::FrameBuffer::getOrCreateDefaultFBO(_openGLView);
        _defaultFBO->retain();
    }
}

TextureCache* Director::getTextureCache() const
{
    return _textureCache;
}

void Director::initTextureCache()
{
    _textureCache = new (std::nothrow) TextureCache();
}

void Director::destroyTextureCache()
{
    if (_textureCache)
    {
        _textureCache->waitForQuit();
        CC_SAFE_RELEASE_NULL(_textureCache);
    }
}

void Director::setViewport()
{
    if (_openGLView)
    {
        _openGLView->setViewPortInPoints(0, 0, _winSizeInPoints.width, _winSizeInPoints.height);
    }
}

void Director::setNextDeltaTimeZero(bool nextDeltaTimeZero)
{
    _nextDeltaTimeZero = nextDeltaTimeZero;
}

//
// FIXME TODO
// Matrix code MUST NOT be part of the Director
// MUST BE moved outside.
// Why the Director must have this code ?
//
void Director::initMatrixStack()
{
    while (!_modelViewMatrixStack.empty())
    {
        _modelViewMatrixStack.pop();
    }

    _projectionMatrixStackList.clear();

    while (!_textureMatrixStack.empty())
    {
        _textureMatrixStack.pop();
    }

    _modelViewMatrixStack.push(Mat4::IDENTITY);
    std::stack<Mat4> projectionMatrixStack;
    projectionMatrixStack.push(Mat4::IDENTITY);
    _projectionMatrixStackList.push_back(projectionMatrixStack);
    _textureMatrixStack.push(Mat4::IDENTITY);
}

void Director::resetMatrixStack()
{
    initMatrixStack();
}

void Director::initProjectionMatrixStack(unsigned int stackCount)
{
    _projectionMatrixStackList.clear();
    std::stack<Mat4> projectionMatrixStack;
    projectionMatrixStack.push(Mat4::IDENTITY);
    for (unsigned int i = 0; i < stackCount; ++i)
        _projectionMatrixStackList.push_back(projectionMatrixStack);
}

unsigned int Director::getProjectionMatrixStackSize()
{
    return _projectionMatrixStackList.size();
}

void Director::popMatrix(MATRIX_STACK_TYPE type)
{
    if(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.pop();
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStackList[0].pop();
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.pop();
    }
    else
    {
        CCASSERT(false, "unknown matrix stack type");
    }
}

void Director::popProjectionMatrix(unsigned int index)
{
    _projectionMatrixStackList[index].pop();
}

void Director::loadIdentityMatrix(MATRIX_STACK_TYPE type)
{
    if(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() = Mat4::IDENTITY;
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStackList[0].top() = Mat4::IDENTITY;
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() = Mat4::IDENTITY;
    }
    else
    {
        CCASSERT(false, "unknown matrix stack type");
    }
}

void Director::loadProjectionIdentityMatrix(unsigned int index)
{
    _projectionMatrixStackList[index].top() = Mat4::IDENTITY;
}

void Director::loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
    if(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() = mat;
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStackList[0].top() = mat;
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() = mat;
    }
    else
    {
        CCASSERT(false, "unknown matrix stack type");
    }
}

void Director::loadProjectionMatrix(const Mat4& mat, unsigned int index)
{
    _projectionMatrixStackList[index].top() = mat;
}

void Director::multiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
    if(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() *= mat;
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStackList[0].top() *= mat;
    }
    else if(MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() *= mat;
    }
    else
    {
        CCASSERT(false, "unknown matrix stack type");
    }
}

void Director::multiplyProjectionMatrix(const Mat4& mat, unsigned int index)
{
    _projectionMatrixStackList[index].top() *= mat;
}

void Director::pushMatrix(MATRIX_STACK_TYPE type)
{
    if(type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
    {
        _modelViewMatrixStack.push(_modelViewMatrixStack.top());
    }
    else if(type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
    {
        _projectionMatrixStackList[0].push(_projectionMatrixStackList[0].top());
    }
    else if(type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
    {
        _textureMatrixStack.push(_textureMatrixStack.top());
    }
    else
    {
        CCASSERT(false, "unknown matrix stack type");
    }
}

void Director::pushProjectionMatrix(unsigned int index)
{
    _projectionMatrixStackList[index].push(_projectionMatrixStackList[index].top());
}

const Mat4& Director::getMatrix(MATRIX_STACK_TYPE type) const
{
    if(type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
    {
        return _modelViewMatrixStack.top();
    }
    else if(type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
    {
        return _projectionMatrixStackList[0].top();
    }
    else if(type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
    {
        return _textureMatrixStack.top();
    }

    CCASSERT(false, "unknown matrix stack type, will return modelview matrix instead");
    return  _modelViewMatrixStack.top();
}

const Mat4& Director::getProjectionMatrix(unsigned int index) const
{
    return _projectionMatrixStackList[index].top();
}

void Director::setProjection(Projection projection)
{
    Size size = _winSizeInPoints;

    if (size.width == 0 || size.height == 0)
    {
        CCLOGERROR("cocos2d: warning, Director::setProjection() failed because size is 0");
        return;
    }

    setViewport();

    switch (projection)
    {
        case Projection::_2D:
        {
            Mat4 orthoMatrix;
            Mat4::createOrthographicOffCenter(0, size.width, 0, size.height, -1024, 1024, &orthoMatrix);
            loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);
            loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
            break;
        }
            
        case Projection::_3D:
        {
            float zeye = this->getZEye();

            Mat4 matrixPerspective, matrixLookup;

            // issue #1334
            Mat4::createPerspective(60, (GLfloat)size.width/size.height, 10, zeye+size.height/2, &matrixPerspective);

            Vec3 eye(size.width/2, size.height/2, zeye), center(size.width/2, size.height/2, 0.0f), up(0.0f, 1.0f, 0.0f);
            Mat4::createLookAt(eye, center, up, &matrixLookup);
            Mat4 proj3d = matrixPerspective * matrixLookup;

            loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, proj3d);
            loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
            break;
        }

        default:
            CCLOG("cocos2d: Director: unrecognized projection");
            break;
    }

    _projection = projection;
    GL::setProjectionMatrixDirty();

    _eventDispatcher->dispatchEvent(_eventProjectionChanged);
}

void Director::purgeCachedData(void)
{
    FontFNT::purgeCachedData();
    FontAtlasCache::purgeCachedData();

    if (s_SharedDirector->getOpenGLView())
    {
        _spriteFrameCache.removeUnusedSpriteFrames();
        _textureCache->removeUnusedTextures();

        // Note: some tests such as ActionsTest are leaking refcounted textures
        // There should be no test textures left in the cache
        log("%s\n", _textureCache->getCachedTextureInfo().c_str());
    }
    FileUtils::getInstance()->purgeCachedEntries();
}

float Director::getZEye(void) const
{
    return (_winSizeInPoints.height / 1.1566f);
}

void Director::setAlphaBlending(bool on)
{
    if (on)
    {
        GL::blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        GL::blendFunc(GL_ONE, GL_ZERO);
    }

    CHECK_GL_ERROR_DEBUG();
}

void Director::setDepthTest(bool on)
{
    _renderer->setDepthTest(on);
}

void Director::setClearColor(const Color4F& clearColor)
{
    _renderer->setClearColor(clearColor);
    auto defaultFBO = experimental::FrameBuffer::getOrCreateDefaultFBO(_openGLView);
    
    if(defaultFBO) defaultFBO->setClearColor(clearColor);
}

static void GLToClipTransform(Mat4 *transformOut)
{
    if(nullptr == transformOut) return;
    
    Director* director = Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when setting matrix stack");

    auto projection = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    auto modelview = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    *transformOut = projection * modelview;
}

Vec2 Director::convertToGL(const Vec2& uiPoint)
{
    Mat4 transform;
    GLToClipTransform(&transform);

    Mat4 transformInv = transform.getInversed();

    // Calculate z=0 using -> transform*[0, 0, 0, 1]/w
    float zClip = transform.m[14]/transform.m[15];

    Size glSize = _openGLView->getDesignResolutionSize();
    Vec4 clipCoord(2.0f*uiPoint.x/glSize.width - 1.0f, 1.0f - 2.0f*uiPoint.y/glSize.height, zClip, 1);

    Vec4 glCoord;
    //transformInv.transformPoint(clipCoord, &glCoord);
    transformInv.transformVector(clipCoord, &glCoord);
    float factor = 1.0f / glCoord.w;
    return Vec2(glCoord.x * factor, glCoord.y * factor);
}

Vec2 Director::convertToUI(const Vec2& glPoint)
{
    Mat4 transform;
    GLToClipTransform(&transform);

    Vec4 clipCoord;
    // Need to calculate the zero depth from the transform.
    Vec4 glCoord(glPoint.x, glPoint.y, 0.0, 1);
    transform.transformVector(glCoord, &clipCoord);

	/*
	BUG-FIX #5506

	a = (Vx, Vy, Vz, 1)
	b = (a×M)T
	Out = 1 ⁄ bw(bx, by, bz)
	*/
	
	clipCoord.x = clipCoord.x / clipCoord.w;
	clipCoord.y = clipCoord.y / clipCoord.w;
	clipCoord.z = clipCoord.z / clipCoord.w;

    Size glSize = _openGLView->getDesignResolutionSize();
    float factor = 1.0f / glCoord.w;
    return Vec2(glSize.width * (clipCoord.x * 0.5f + 0.5f) * factor, glSize.height * (-clipCoord.y * 0.5f + 0.5f) * factor);
}

const Size& Director::getWinSize(void) const
{
    return _winSizeInPoints;
}

Size Director::getWinSizeInPixels() const
{
    return Size(_winSizeInPoints.width * _contentScaleFactor, _winSizeInPoints.height * _contentScaleFactor);
}

Size Director::getVisibleSize() const
{
    if (_openGLView)
    {
        return _openGLView->getVisibleSize();
    }
    else
    {
        return Size::ZERO;
    }
}

Vec2 Director::getVisibleOrigin() const
{
    if (_openGLView)
    {
        return _openGLView->getVisibleOrigin();
    }
    else
    {
        return Vec2::ZERO;
    }
}

// scene management

void Director::runWithScene(node_ptr<Scene> scene)
{
    CCASSERT(scene, "This command can only be used to start the Director. There is already a scene present.");
    CCASSERT(!_runningScene, "_runningScene should be null");

    pushScene( std::move(scene) );
    startAnimation();
}

void Director::replaceScene(node_ptr<Scene> scene)
{
    CCASSERT(scene, "the scene should not be null");
    
    if (!_runningScene) {
        runWithScene( std::move(scene) );
        return;
    }
    
    if (scene == _scenesStack.back())
        return;
    
    if (_scenesStack.back())
    {
        if (!_scenesStack.back()->isPaused())
        {
            _scenesStack.back()->onExit();
        }
        _scenesStack.back()->cleanup();
    }

    _sendCleanupToScene = true;
    
    _scenesStack.back() = std::move(scene);
}

void Director::pushScene(node_ptr<Scene> scene)
{
    CCASSERT(scene, "the scene should not null");

    _sendCleanupToScene = false;

    _scenesStack.push_back( std::move(scene) );
}

void Director::popScene()
{
    CCASSERT(_runningScene, "running scene should not be null");
    
    _scenesStack.pop_back();

    if (_scenesStack.empty())
    {
        end();
    }
    else
    {
        _sendCleanupToScene = true;
    }
}

void Director::popToRootScene()
{
    popToSceneStackLevel(1);
}

void Director::popToSceneStackLevel(size_t level)
{
    CCASSERT(_runningScene, "A running Scene is needed");

    // level 0? -> end
    if (level == 0)
    {
        end();
        return;
    }

    // current level or lower -> nothing
    if (level >= _scenesStack.size())
    {
        return;
    }

    // pop stack until reaching desired level
    for ( ; _scenesStack.size() > level; _scenesStack.pop_back())
    {
        if (_scenesStack.back())
        {
            if (!_scenesStack.back()->isPaused())
            {
                _scenesStack.back()->onExit();
            }

            _scenesStack.back()->cleanup();
        }
    }

    // cleanup running scene
    _sendCleanupToScene = true;
}

void Director::end()
{
    _purgeDirectorInNextLoop = true;
}

void Director::restart()
{
    _restartDirectorInNextLoop = true;
}

void Director::reset()
{
    if (_runningScene)
    {
        _runningScene->onExit();
        _runningScene->cleanup();
        _runningScene.reset();
    }
    
    _eventDispatcher->dispatchEvent(_eventResetDirector);
    
    // cleanup scheduler
    _scheduler.unscheduleUpdateJob( _actionManager.get() );
    _scheduler.unscheduleAll();
    
    // Remove all events
    if (_eventDispatcher)
    {
        _eventDispatcher->removeAllEventListeners();
    }
    
    if(_notificationNode)
    {
        _notificationNode->onExit();
        _notificationNode->cleanup();
        _notificationNode->release();
    }
    
    _notificationNode = nullptr;
    
    _scenesStack.clear();
    
    stopAnimation();
    
    CC_SAFE_RELEASE_NULL(_notificationNode);
    CC_SAFE_RELEASE_NULL(_FPSLabel);
    CC_SAFE_RELEASE_NULL(_drawnBatchesLabel);
    CC_SAFE_RELEASE_NULL(_drawnVerticesLabel);
    
    // purge bitmap cache
    FontFNT::purgeCachedData();
    FontAtlasCache::purgeCachedData();
    
    FontFreeType::shutdownFreeType();
    
    AnimationCache::destroyInstance();
    _spriteFrameCache.removeSpriteFrames();
    GLProgramCache::destroyInstance();
    GLProgramStateCache::destroyInstance();
    FileUtils::destroyInstance();
    AsyncTaskPool::destroyInstance();
    
    // cocos2d-x specific data structures
    UserDefault::destroyInstance();
    
    GL::invalidateStateCache();

    RenderState::finalize();
    
    destroyTextureCache();
}

void Director::purgeDirector()
{
    reset();

    CHECK_GL_ERROR_DEBUG();
    
    // OpenGL view
    if (_openGLView)
    {
        _openGLView->end();
        _openGLView = nullptr;
    }

    delete this;
}

void Director::restartDirector()
{
    reset();
    
    // RenderState need to be reinitialized
    RenderState::initialize();

    // Texture cache need to be reinitialized
    initTextureCache();
    
    _scheduler.schedule(
        UpdateJob(_actionManager.get(), UpdateJob::SYSTEM_PRIORITY).paused(false)
    );

    // release the objects
    PoolManager::getInstance()->getCurrentPool()->clear();

    // Restart animation
    startAnimation();
}

void Director::setNextScene()
{
    bool runningIsTransition = dynamic_cast<TransitionScene*>(_runningScene.get()) != nullptr;
    bool newIsTransition = dynamic_cast<TransitionScene*>(_scenesStack.back().get()) != nullptr;

    // If it is not a transition, call onExit/cleanup
    if (! newIsTransition)
    {
        if (_runningScene)
        {
            _runningScene->onExitTransitionDidStart();
            _runningScene->onExit();

            if (_sendCleanupToScene)
            {
                _runningScene->cleanup();
            }
        }
    }

    // return the running scene back to the stack unless it was rremoved
    auto iter = std::find(_scenesStack.rbegin(), _scenesStack.rend(),
                          node_ptr<Scene>());
    if (iter != _scenesStack.rend())
        *iter = std::move(_runningScene);

    // keep running scene out of the stack
    _runningScene = std::move(_scenesStack.back());

    if ( ! runningIsTransition)
    {
        _runningScene->onEnter();
        _runningScene->onEnterTransitionDidFinish();
    }
}

void Director::pause()
{
    if (_paused)
    {
        return;
    }

    _oldAnimationInterval = _animationInterval;

    // when paused, don't consume CPU
    setAnimationInterval(1 / 4.0);
    _paused = true;
}

void Director::resume()
{
    if (! _paused)
    {
        return;
    }

    setAnimationInterval(_oldAnimationInterval);

    _paused = false;
    _deltaTime = 0;
    // fix issue #3509, skip one fps to avoid incorrect time calculation.
    setNextDeltaTimeZero(true);
}

// display the FPS using a LabelAtlas
// updates the FPS every frame
void Director::showStats()
{
    if (_isStatusLabelUpdated)
    {
        createStatsLabel();
        _isStatusLabelUpdated = false;
    }

    static unsigned long prevCalls = 0;
    static unsigned long prevVerts = 0;
    static float prevDeltaTime  = 0.016f; // 60FPS
    static const float FPS_FILTER = 0.10f;

    _accumDt += _deltaTime;
    
    if (_displayStats && _FPSLabel && _drawnBatchesLabel && _drawnVerticesLabel)
    {
        char buffer[30];

        float dt = _deltaTime * FPS_FILTER + (1-FPS_FILTER) * prevDeltaTime;
        prevDeltaTime = dt;
        _frameRate = 1/dt;

        // Probably we don't need this anymore since
        // the framerate is using a low-pass filter
        // to make the FPS stable
        if (_accumDt > CC_DIRECTOR_STATS_INTERVAL)
        {
            sprintf(buffer, "%.1f / %.3f", _frameRate, _secondsPerFrame);
            _FPSLabel->setString(buffer);
            _accumDt = 0;
        }

        auto currentCalls = (unsigned long)_renderer->getDrawnBatches();
        auto currentVerts = (unsigned long)_renderer->getDrawnVertices();
        if( currentCalls != prevCalls ) {
            sprintf(buffer, "GL calls:%6lu", currentCalls);
            _drawnBatchesLabel->setString(buffer);
            prevCalls = currentCalls;
        }

        if( currentVerts != prevVerts) {
            sprintf(buffer, "GL verts:%6lu", currentVerts);
            _drawnVerticesLabel->setString(buffer);
            prevVerts = currentVerts;
        }

        const Mat4& identity = Mat4::IDENTITY;
        _drawnVerticesLabel->visit(_renderer, identity, 0);
        _drawnBatchesLabel->visit(_renderer, identity, 0);
        _FPSLabel->visit(_renderer, identity, 0);
    }
}

void Director::calculateMPF()
{
    static float prevSecondsPerFrame = 0;
    static const float MPF_FILTER = 0.10f;

    auto now = std::chrono::steady_clock::now();
    
    _secondsPerFrame = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;

    _secondsPerFrame = _secondsPerFrame * MPF_FILTER + (1-MPF_FILTER) * prevSecondsPerFrame;
    prevSecondsPerFrame = _secondsPerFrame;
}

// returns the FPS image data pointer and len
void Director::getFPSImageData(unsigned char** datapointer, ssize_t* length)
{
    // FIXME: fixed me if it should be used 
    *datapointer = cc_fps_images_png;
    *length = cc_fps_images_len();
}

void Director::createStatsLabel()
{
    Texture2D *texture = nullptr;
    std::string fpsString = "00.0";
    std::string drawBatchString = "000";
    std::string drawVerticesString = "00000";
    if (_FPSLabel)
    {
        fpsString = _FPSLabel->getString();
        drawBatchString = _drawnBatchesLabel->getString();
        drawVerticesString = _drawnVerticesLabel->getString();
        
        CC_SAFE_RELEASE_NULL(_FPSLabel);
        CC_SAFE_RELEASE_NULL(_drawnBatchesLabel);
        CC_SAFE_RELEASE_NULL(_drawnVerticesLabel);
        _textureCache->removeTextureForKey("/cc_fps_images");
        FileUtils::getInstance()->purgeCachedEntries();
    }

    Texture2D::PixelFormat currentFormat = Texture2D::getDefaultAlphaPixelFormat();
    Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
    unsigned char *data = nullptr;
    ssize_t dataLength = 0;
    getFPSImageData(&data, &dataLength);

    Image* image = new (std::nothrow) Image();
    bool isOK = image->initWithImageData(data, dataLength);
    if (! isOK) {
        CCLOGERROR("%s", "Fails: init fps_images");
        return;
    }

    texture = _textureCache->addImage(image, "/cc_fps_images");
    CC_SAFE_RELEASE(image);

    /*
     We want to use an image which is stored in the file named ccFPSImage.c 
     for any design resolutions and all resource resolutions. 
     
     To achieve this, we need to ignore 'contentScaleFactor' in 'AtlasNode' and 'LabelAtlas'.
     So I added a new method called 'setIgnoreContentScaleFactor' for 'AtlasNode',
     this is not exposed to game developers, it's only used for displaying FPS now.
     */
    float scaleFactor = 1 / CC_CONTENT_SCALE_FACTOR();

    _FPSLabel = LabelAtlas::create();
    _FPSLabel->retain();
    _FPSLabel->setIgnoreContentScaleFactor(true);
    _FPSLabel->initWithString(fpsString, texture, 12, 32 , '.');
    _FPSLabel->setScale(scaleFactor);

    _drawnBatchesLabel = LabelAtlas::create();
    _drawnBatchesLabel->retain();
    _drawnBatchesLabel->setIgnoreContentScaleFactor(true);
    _drawnBatchesLabel->initWithString(drawBatchString, texture, 12, 32, '.');
    _drawnBatchesLabel->setScale(scaleFactor);

    _drawnVerticesLabel = LabelAtlas::create();
    _drawnVerticesLabel->retain();
    _drawnVerticesLabel->setIgnoreContentScaleFactor(true);
    _drawnVerticesLabel->initWithString(drawVerticesString, texture, 12, 32, '.');
    _drawnVerticesLabel->setScale(scaleFactor);


    Texture2D::setDefaultAlphaPixelFormat(currentFormat);

    const int height_spacing = 22 / CC_CONTENT_SCALE_FACTOR();
    _drawnVerticesLabel->setPosition(Vec2(0, height_spacing*2) + CC_DIRECTOR_STATS_POSITION);
    _drawnBatchesLabel->setPosition(Vec2(0, height_spacing*1) + CC_DIRECTOR_STATS_POSITION);
    _FPSLabel->setPosition(Vec2(0, height_spacing*0)+CC_DIRECTOR_STATS_POSITION);
}

void Director::setContentScaleFactor(float scaleFactor)
{
    if (scaleFactor != _contentScaleFactor)
    {
        _contentScaleFactor = scaleFactor;
        _isStatusLabelUpdated = true;
    }
}

void Director::setNotificationNode(Node *node)
{
	if (_notificationNode != nullptr){
		_notificationNode->onExitTransitionDidStart();
		_notificationNode->onExit();
		_notificationNode->cleanup();
	}
	CC_SAFE_RELEASE(_notificationNode);

	_notificationNode = node;
	if (node == nullptr)
		return;
	_notificationNode->onEnter();
	_notificationNode->onEnterTransitionDidFinish();
    CC_SAFE_RETAIN(_notificationNode);
}

void Director::startAnimation()
{
    _lastUpdate = std::chrono::steady_clock::now();

    _invalid = false;

    _cocos2d_thread_id = std::this_thread::get_id();

    Application::getInstance()->setAnimationInterval(_animationInterval);

    // fix issue #3509, skip one fps to avoid incorrect time calculation.
    setNextDeltaTimeZero(true);
}

void Director::mainLoop()
{
    if (_purgeDirectorInNextLoop)
    {
        _purgeDirectorInNextLoop = false;
        purgeDirector();
    }
    else if (_restartDirectorInNextLoop)
    {
        _restartDirectorInNextLoop = false;
        restartDirector();
    }
    else if (! _invalid)
    {
        drawScene();
     
        // release the objects
        PoolManager::getInstance()->getCurrentPool()->clear();
    }
}

void Director::stopAnimation()
{
    _invalid = true;
}

void Director::setAnimationInterval(float interval)
{
    _animationInterval = interval;
    if (! _invalid)
    {
        stopAnimation();
        startAnimation();
    }    
}

} // namespace cocos2d

