/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

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

#include "ui/UIScrollView.h"

NS_CC_BEGIN

namespace ui {

static const float AUTOSCROLLMAXSPEED = 1000.0f;

const Vec2 SCROLLDIR_UP(0.0f, 1.0f);
const Vec2 SCROLLDIR_DOWN(0.0f, -1.0f);
const Vec2 SCROLLDIR_LEFT(-1.0f, 0.0f);
const Vec2 SCROLLDIR_RIGHT(1.0f, 0.0f);

IMPLEMENT_CLASS_GUI_INFO(ScrollView)

ScrollView::ScrollView():
_innerContainer(nullptr),
_direction(Direction::VERTICAL),
_topBoundary(0.0f),
_bottomBoundary(0.0f),
_leftBoundary(0.0f),
_rightBoundary(0.0f),
_bounceTopBoundary(0.0f),
_bounceBottomBoundary(0.0f),
_bounceLeftBoundary(0.0f),
_bounceRightBoundary(0.0f),
_autoScroll(false),
_autoScrollAddUpTime(0.0f),
_autoScrollOriginalSpeed(0.0f),
_autoScrollAcceleration(-1000.0f),
_isAutoScrollSpeedAttenuated(false),
_needCheckAutoScrollDestination(false),
_bePressed(false),
_slidTime(0.0f),
_childFocusCancelOffset(5.0f),
_bounceEnabled(false),
_bouncing(false),
_bounceOriginalSpeed(0.0f),
_inertiaScrollEnabled(true),
_scrollViewEventListener(nullptr),
_scrollViewEventSelector(nullptr),
_eventCallback(nullptr)
{
    setTouchEnabled(true);
}

ScrollView::~ScrollView()
{
    _scrollViewEventListener = nullptr;
    _scrollViewEventSelector = nullptr;
}

ScrollView* ScrollView::create()
{
    ScrollView* widget = new (std::nothrow) ScrollView();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

void ScrollView::onEnter()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
            return;
    }
#endif

    Layout::onEnter();
    scheduleUpdate();
}

bool ScrollView::init()
{
    if (Layout::init())
    {
        setClippingEnabled(true);
        _innerContainer->setTouchEnabled(false);
        return true;
    }
    return false;
}

void ScrollView::initRenderer()
{
    Layout::initRenderer();
    _innerContainer = Layout::create();
    _innerContainer->setColor(Color3B(255,255,255));
    _innerContainer->setOpacity(255);
    _innerContainer->setCascadeColorEnabled(true);
    _innerContainer->setCascadeOpacityEnabled(true);
    addProtectedChild(_innerContainer, 1, 1);
}

void ScrollView::onSizeChanged()
{
    Layout::onSizeChanged();
    _topBoundary = _contentSize.height;
    _rightBoundary = _contentSize.width;
    float bounceBoundaryParameterX = _contentSize.width / 3.0f;
    float bounceBoundaryParameterY = _contentSize.height / 3.0f;
    _bounceTopBoundary = _contentSize.height - bounceBoundaryParameterY;
    _bounceBottomBoundary = bounceBoundaryParameterY;
    _bounceLeftBoundary = bounceBoundaryParameterX;
    _bounceRightBoundary = _contentSize.width - bounceBoundaryParameterX;
    Size innerSize = _innerContainer->getContentSize();
    float orginInnerSizeWidth = innerSize.width;
    float orginInnerSizeHeight = innerSize.height;
    float innerSizeWidth = MAX(orginInnerSizeWidth, _contentSize.width);
    float innerSizeHeight = MAX(orginInnerSizeHeight, _contentSize.height);
    _innerContainer->setContentSize(Size(innerSizeWidth, innerSizeHeight));
    _innerContainer->setPosition(Vec2(0, _contentSize.height - _innerContainer->getContentSize().height));
}

void ScrollView::setInnerContainerSize(const Size &size)
{
    float innerSizeWidth = _contentSize.width;
    float innerSizeHeight = _contentSize.height;
    Size originalInnerSize = _innerContainer->getContentSize();
    if (size.width < _contentSize.width)
    {
        CCLOG("Inner width <= scrollview width, it will be force sized!");
    }
    else
    {
        innerSizeWidth = size.width;
    }
    if (size.height < _contentSize.height)
    {
        CCLOG("Inner height <= scrollview height, it will be force sized!");
    }
    else
    {
        innerSizeHeight = size.height;
    }
    _innerContainer->setContentSize(Size(innerSizeWidth, innerSizeHeight));

    switch (_direction)
    {
        case Direction::VERTICAL:
        {
            Size newInnerSize = _innerContainer->getContentSize();
            float offset = originalInnerSize.height - newInnerSize.height;
            scrollChildren(0.0f, offset);
            break;
        }
        case Direction::HORIZONTAL:
        {
            if (_innerContainer->getRightBoundary() <= _contentSize.width)
            {
                Size newInnerSize = _innerContainer->getContentSize();
                float offset = originalInnerSize.width - newInnerSize.width;
                scrollChildren(offset, 0.0f);
            }
            break;
        }
        case Direction::BOTH:
        {
            Size newInnerSize = _innerContainer->getContentSize();
            float offsetY = originalInnerSize.height - newInnerSize.height;
            float offsetX = 0.0f;
            if (_innerContainer->getRightBoundary() <= _contentSize.width)
            {
                offsetX = originalInnerSize.width - newInnerSize.width;
            }
            scrollChildren(offsetX, offsetY);
            break;
        }
        default:
            break;
    }
    if (_innerContainer->getLeftBoundary() > 0.0f)
    {
        _innerContainer->setPosition(Vec2(_innerContainer->getAnchorPoint().x * _innerContainer->getContentSize().width,
                                          _innerContainer->getPosition().y));
    }
    if (_innerContainer->getRightBoundary() < _contentSize.width)
    {
         _innerContainer->setPosition(Vec2(_contentSize.width - ((1.0f - _innerContainer->getAnchorPoint().x) * _innerContainer->getContentSize().width),
                                           _innerContainer->getPosition().y));
    }
    if (_innerContainer->getPosition().y > 0.0f)
    {
        _innerContainer->setPosition(Vec2(_innerContainer->getPosition().x,
                                          _innerContainer->getAnchorPoint().y * _innerContainer->getContentSize().height));
    }
    if (_innerContainer->getTopBoundary() < _contentSize.height)
    {
        _innerContainer->setPosition(Vec2(_innerContainer->getPosition().x,
                                          _contentSize.height - (1.0f - _innerContainer->getAnchorPoint().y) * _innerContainer->getContentSize().height));
    }
}

const Size& ScrollView::getInnerContainerSize() const
{
	return _innerContainer->getContentSize();
}

void ScrollView::addChild(Node* child)
{
    ScrollView::addChild(child, child->getLocalZOrder(), child->getTag());
}

void ScrollView::addChild(Node * child, int localZOrder)
{
    ScrollView::addChild(child, localZOrder, child->getTag());
}

void ScrollView::addChild(Node *child, int zOrder, int tag)
{
    _innerContainer->addChild(child, zOrder, tag);
}

void ScrollView::addChild(Node* child, int zOrder, const std::string &name)
{
    _innerContainer->addChild(child, zOrder, name);
}

void ScrollView::removeAllChildren()
{
    removeAllChildrenWithCleanup(true);
}

void ScrollView::removeAllChildrenWithCleanup(bool cleanup)
{
    _innerContainer->removeAllChildrenWithCleanup(cleanup);
}

void ScrollView::removeChild(Node* child, bool cleanup)
{
	return _innerContainer->removeChild(child, cleanup);
}

Vector<Node*>& ScrollView::getChildren()
{
    return _innerContainer->getChildren();
}

const Vector<Node*>& ScrollView::getChildren() const
{
    return _innerContainer->getChildren();
}

ssize_t ScrollView::getChildrenCount() const
{
    return _innerContainer->getChildrenCount();
}

Node* ScrollView::getChildByTag(int tag) const
{
    return _innerContainer->getChildByTag(tag);
}

Node* ScrollView::getChildByName(const std::string& name)const
{
    return _innerContainer->getChildByName(name);
}

void ScrollView::moveChildren(float offsetX, float offsetY)
{
    _moveChildPoint = _innerContainer->getPosition() + Vec2(offsetX, offsetY);
    _innerContainer->setPosition(_moveChildPoint);
}

void ScrollView::autoScrollChildren(float dt)
{
    float lastTime = _autoScrollAddUpTime;
    _autoScrollAddUpTime += dt;
    if (_isAutoScrollSpeedAttenuated)
    {
        float nowSpeed = _autoScrollOriginalSpeed + _autoScrollAcceleration * _autoScrollAddUpTime;
        if (nowSpeed <= 0.0f)
        {
            stopAutoScrollChildren();
            checkNeedBounce();
        }
        else
        {
            float timeParam = lastTime * 2 + dt;
            float offset = (_autoScrollOriginalSpeed + _autoScrollAcceleration * timeParam * 0.5f) * dt;
            float offsetX = offset * _autoScrollDir.x;
            float offsetY = offset * _autoScrollDir.y;
            if (!scrollChildren(offsetX, offsetY))
            {
                stopAutoScrollChildren();
                checkNeedBounce();
            }
        }
    }
    else
    {
        if (_needCheckAutoScrollDestination)
        {
            float xOffset = _autoScrollDir.x * dt * _autoScrollOriginalSpeed;
            float yOffset = _autoScrollDir.y * dt * _autoScrollOriginalSpeed;
            bool notDone = checkCustomScrollDestination(&xOffset, &yOffset);
            bool scrollCheck = scrollChildren(xOffset, yOffset);
            if (!notDone || !scrollCheck)
            {
                stopAutoScrollChildren();
                checkNeedBounce();
            }
        }
        else
        {
            if (!scrollChildren(_autoScrollDir.x * dt * _autoScrollOriginalSpeed, _autoScrollDir.y * dt * _autoScrollOriginalSpeed))
            {
                stopAutoScrollChildren();
                checkNeedBounce();
            }
        }
    }
}

void ScrollView::bounceChildren(float dt)
{
    if (_bounceOriginalSpeed <= 0.0f)
    {
        stopBounceChildren();
    }
    if (!bounceScrollChildren(_bounceDir.x * dt * _bounceOriginalSpeed, _bounceDir.y * dt * _bounceOriginalSpeed))
    {
        stopBounceChildren();
    }
}

bool ScrollView::checkNeedBounce()
{
    if (!_bounceEnabled)
    {
        return false;
    }
	bool topBounceNeeded, bottomBounceNeeded, leftBounceNeeded, rightBounceNeeded;
    checkBounceBoundary(&topBounceNeeded, &bottomBounceNeeded, &leftBounceNeeded, &rightBounceNeeded);
	
    if (topBounceNeeded || bottomBounceNeeded || leftBounceNeeded || rightBounceNeeded)
    {
        if (topBounceNeeded && leftBounceNeeded)
        {
            Vec2 scrollVector = Vec2(0.0f, _contentSize.height) - Vec2(_innerContainer->getLeftBoundary(), _innerContainer->getTopBoundary());
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (topBounceNeeded && rightBounceNeeded)
        {
            Vec2 scrollVector = Vec2(_contentSize.width, _contentSize.height) - Vec2(_innerContainer->getRightBoundary(), _innerContainer->getTopBoundary());
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (bottomBounceNeeded && leftBounceNeeded)
        {
            Vec2 scrollVector = Vec2::ZERO - Vec2(_innerContainer->getLeftBoundary(), _innerContainer->getBottomBoundary());
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (bottomBounceNeeded && rightBounceNeeded)
        {
            Vec2 scrollVector = Vec2(_contentSize.width, 0.0f) - Vec2(_innerContainer->getRightBoundary(), _innerContainer->getBottomBoundary());
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (topBounceNeeded)
        {
            Vec2 scrollVector = Vec2(0.0f, _contentSize.height) - Vec2(0.0f, _innerContainer->getTopBoundary());
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (bottomBounceNeeded)
        {
            Vec2 scrollVector = Vec2::ZERO - Vec2(0.0f, _innerContainer->getBottomBoundary());
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (leftBounceNeeded)
        {
            Vec2 scrollVector = Vec2::ZERO - Vec2(_innerContainer->getLeftBoundary(), 0.0f);
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        else if (rightBounceNeeded)
        {
            Vec2 scrollVector = Vec2(_contentSize.width, 0.0f) - Vec2(_innerContainer->getRightBoundary(), 0.0f);
            float orSpeed = scrollVector.getLength()/(0.2f);
            _bounceDir = scrollVector.getNormalized();
            startBounceChildren(orSpeed);
        }
        return true;
    }
    return false;
}

void ScrollView::checkBounceBoundary(bool* pTopBounceNeeded, bool* pBottomBounceNeeded, bool* pLeftBounceNeeded, bool* pRightBounceNeeded)
{
    float icBottomPos = _innerContainer->getBottomBoundary();
    if (icBottomPos > _bottomBoundary)
    {
		processScrollEvent(MoveDirection::BOTTOM, false);
        (*pBottomBounceNeeded) = true;
    }
    else
    {
        (*pBottomBounceNeeded) = false;
    }
    float icTopPos = _innerContainer->getTopBoundary();
    if (icTopPos < _topBoundary)
    {
		processScrollEvent(MoveDirection::TOP, false);
        (*pTopBounceNeeded) = true;
    }
    else
    {
        (*pTopBounceNeeded) = false;
    }
    float icRightPos = _innerContainer->getRightBoundary();
    if (icRightPos < _rightBoundary)
    {
		processScrollEvent(MoveDirection::RIGHT, false);
        (*pRightBounceNeeded) = true;
    }
    else
    {
        (*pRightBounceNeeded) = false;
    }
    float icLeftPos = _innerContainer->getLeftBoundary();
    if (icLeftPos > _leftBoundary)
    {
		processScrollEvent(MoveDirection::LEFT, false);
        (*pLeftBounceNeeded) = true;
    }
    else
    {
        (*pLeftBounceNeeded) = false;
    }
}

void ScrollView::startBounceChildren(float v)
{
    _bounceOriginalSpeed = v;
    _bouncing = true;
}

void ScrollView::stopBounceChildren()
{
    _bouncing = false;
    _bounceOriginalSpeed = 0.0f;
}

void ScrollView::startAutoScrollChildrenWithOriginalSpeed(const Vec2& dir, float v, bool attenuated, float acceleration)
{
    stopAutoScrollChildren();
    _autoScrollDir = dir;
    _isAutoScrollSpeedAttenuated = attenuated;
    _autoScrollOriginalSpeed = v;
    _autoScroll = true;
    _autoScrollAcceleration = acceleration;
}

void ScrollView::startAutoScrollChildrenWithDestination(const Vec2& des, float second, bool attenuated)
{
    _needCheckAutoScrollDestination = false;
    _autoScrollDestination = des;
    Vec2 dis = des - _innerContainer->getPosition();
    Vec2 dir = dis.getNormalized();
    float orSpeed = 0.0f;
    float acceleration = -1000.0f;
    if (attenuated)
    {
        acceleration = (-(2 * dis.getLength())) / (second * second);
        orSpeed =  2 * dis.getLength() / second;
    }
    else
    {
        _needCheckAutoScrollDestination = true;
        orSpeed = dis.getLength() / second;
    }
    startAutoScrollChildrenWithOriginalSpeed(dir, orSpeed, attenuated, acceleration);
}

void ScrollView::jumpToDestination(const Vec2 &des)
{
    float finalOffsetX = des.x;
    float finalOffsetY = des.y;
    switch (_direction)
    {
        case Direction::VERTICAL:
            if (des.y <= 0)
            {
                finalOffsetY = MAX(des.y, _contentSize.height - _innerContainer->getContentSize().height);
            }
            break;
        case Direction::HORIZONTAL:
            if (des.x <= 0)
            {
                finalOffsetX = MAX(des.x, _contentSize.width - _innerContainer->getContentSize().width);
            }
            break;
        case Direction::BOTH:
            if (des.y <= 0)
            {
                finalOffsetY = MAX(des.y, _contentSize.height - _innerContainer->getContentSize().height);
            }
            if (des.x <= 0)
            {
                finalOffsetX = MAX(des.x, _contentSize.width - _innerContainer->getContentSize().width);
            }
            break;
        default:
            break;
    }
    _innerContainer->setPosition(Vec2(finalOffsetX, finalOffsetY));
}

void ScrollView::stopAutoScrollChildren()
{
    _autoScroll = false;
    _autoScrollOriginalSpeed = 0.0f;
    _autoScrollAddUpTime = 0.0f;
}

bool ScrollView::bounceScrollChildren(float touchOffsetX, float touchOffsetY)
{
    bool scrollenabled = true;
    if (touchOffsetX > 0.0f && touchOffsetY > 0.0f) //first quadrant //bounce to top-right
    {
        float realOffsetX = touchOffsetX;
        float realOffsetY = touchOffsetY;
        float icRightPos = _innerContainer->getRightBoundary();
        if (icRightPos + realOffsetX >= _rightBoundary)
        {
            realOffsetX = _rightBoundary - icRightPos;
            processScrollEvent(MoveDirection::RIGHT, true);
            scrollenabled = false;
        }
        float icTopPos = _innerContainer->getTopBoundary();
        if (icTopPos + touchOffsetY >= _topBoundary)
        {
            realOffsetY = _topBoundary - icTopPos;
			processScrollEvent(MoveDirection::TOP, true);
            scrollenabled = false;
        }
        moveChildren(realOffsetX, realOffsetY);
    }
    else if(touchOffsetX < 0.0f && touchOffsetY > 0.0f) //second quadrant //bounce to top-left
    {
        float realOffsetX = touchOffsetX;
        float realOffsetY = touchOffsetY;
        float icLefrPos = _innerContainer->getLeftBoundary();
        if (icLefrPos + realOffsetX <= _leftBoundary)
        {
            realOffsetX = _leftBoundary - icLefrPos;
            processScrollEvent(MoveDirection::LEFT, true);
            scrollenabled = false;
        }
        float icTopPos = _innerContainer->getTopBoundary();
        if (icTopPos + touchOffsetY >= _topBoundary)
        {
            realOffsetY = _topBoundary - icTopPos;
			processScrollEvent(MoveDirection::TOP, true);
            scrollenabled = false;
        }
        moveChildren(realOffsetX, realOffsetY);
    }
    else if (touchOffsetX < 0.0f && touchOffsetY < 0.0f) //third quadrant //bounce to bottom-left
    {
        float realOffsetX = touchOffsetX;
        float realOffsetY = touchOffsetY;
        float icLefrPos = _innerContainer->getLeftBoundary();
        if (icLefrPos + realOffsetX <= _leftBoundary)
        {
            realOffsetX = _leftBoundary - icLefrPos;
            processScrollEvent(MoveDirection::LEFT, true);
            scrollenabled = false;
        }
        float icBottomPos = _innerContainer->getBottomBoundary();
        if (icBottomPos + touchOffsetY <= _bottomBoundary)
        {
            realOffsetY = _bottomBoundary - icBottomPos;
            processScrollEvent(MoveDirection::BOTTOM, true);
            scrollenabled = false;
        }
        moveChildren(realOffsetX, realOffsetY);
    }
    else if (touchOffsetX > 0.0f && touchOffsetY < 0.0f) //forth quadrant //bounce to bottom-right
    {
        float realOffsetX = touchOffsetX;
        float realOffsetY = touchOffsetY;
        float icRightPos = _innerContainer->getRightBoundary();
        if (icRightPos + realOffsetX >= _rightBoundary)
        {
            realOffsetX = _rightBoundary - icRightPos;
            processScrollEvent(MoveDirection::RIGHT, true);
            scrollenabled = false;
        }
        float icBottomPos = _innerContainer->getBottomBoundary();
        if (icBottomPos + touchOffsetY <= _bottomBoundary)
        {
            realOffsetY = _bottomBoundary - icBottomPos;
            processScrollEvent(MoveDirection::BOTTOM, true);
            scrollenabled = false;
        }
        moveChildren(realOffsetX, realOffsetY);
    }
    else if (touchOffsetX == 0.0f && touchOffsetY > 0.0f) // bounce to top
    {
        float realOffsetY = touchOffsetY;
        float icTopPos = _innerContainer->getTopBoundary();
        if (icTopPos + touchOffsetY >= _topBoundary)
        {
            realOffsetY = _topBoundary - icTopPos;
			processScrollEvent(MoveDirection::TOP, true);
            scrollenabled = false;
        }
        moveChildren(0.0f, realOffsetY);
    }
    else if (touchOffsetX == 0.0f && touchOffsetY < 0.0f) //bounce to bottom
    {
        float realOffsetY = touchOffsetY;
        float icBottomPos = _innerContainer->getBottomBoundary();
        if (icBottomPos + touchOffsetY <= _bottomBoundary)
        {
            realOffsetY = _bottomBoundary - icBottomPos;
            processScrollEvent(MoveDirection::BOTTOM, true);
            scrollenabled = false;
        }
        moveChildren(0.0f, realOffsetY);
    }
    else if (touchOffsetX > 0.0f && touchOffsetY == 0.0f) //bounce to right
    {
        float realOffsetX = touchOffsetX;
        float icRightPos = _innerContainer->getRightBoundary();
        if (icRightPos + realOffsetX >= _rightBoundary)
        {
            realOffsetX = _rightBoundary - icRightPos;
            processScrollEvent(MoveDirection::RIGHT, true);
            scrollenabled = false;
        }
        moveChildren(realOffsetX, 0.0f);
    }
    else if (touchOffsetX < 0.0f && touchOffsetY == 0.0f) //bounce to left
    {
        float realOffsetX = touchOffsetX;
        float icLeftPos = _innerContainer->getLeftBoundary();
        if (icLeftPos + realOffsetX <= _leftBoundary)
        {
            realOffsetX = _leftBoundary - icLeftPos;
            processScrollEvent(MoveDirection::LEFT, true);
            scrollenabled = false;
        }
        moveChildren(realOffsetX, 0.0f);
    }
    return scrollenabled;
}

bool ScrollView::checkCustomScrollDestination(float* touchOffsetX, float* touchOffsetY)
{
    bool scrollenabled = true;
    switch (_direction)
    {
        case Direction::VERTICAL:
        {
            if (_autoScrollDir.y > 0)
            {
                float icBottomPos = _innerContainer->getBottomBoundary();
                if (icBottomPos + *touchOffsetY >= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icBottomPos;
                    scrollenabled = false;
                }
            }
            else
            {
                float icBottomPos = _innerContainer->getBottomBoundary();
                if (icBottomPos + *touchOffsetY <= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icBottomPos;
                    scrollenabled = false;
                }
            }
            break;
        }
        case Direction::HORIZONTAL:
        {
            if (_autoScrollDir.x > 0)
            {
                float icLeftPos = _innerContainer->getLeftBoundary();
                if (icLeftPos + *touchOffsetX >= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icLeftPos;
                    scrollenabled = false;
                }
            }
            else
            {
                float icLeftPos = _innerContainer->getLeftBoundary();
                if (icLeftPos + *touchOffsetX <= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icLeftPos;
                    scrollenabled = false;
                }
            }
            break;
        }
        case Direction::BOTH:
        {
            if (*touchOffsetX > 0.0f && *touchOffsetY > 0.0f) // up right
            {
                float icLeftPos = _innerContainer->getLeftBoundary();
                if (icLeftPos + *touchOffsetX >= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icLeftPos;
                    scrollenabled = false;
                }
                float icBottomPos = _innerContainer->getBottomBoundary();
                if (icBottomPos + *touchOffsetY >= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icBottomPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX < 0.0f && *touchOffsetY > 0.0f) // up left
            {
                float icRightPos = _innerContainer->getRightBoundary();
                if (icRightPos + *touchOffsetX <= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icRightPos;
                    scrollenabled = false;
                }
                float icBottomPos = _innerContainer->getBottomBoundary();
                if (icBottomPos + *touchOffsetY >= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icBottomPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX < 0.0f && *touchOffsetY < 0.0f) // down left
            {
                float icRightPos = _innerContainer->getRightBoundary();
                if (icRightPos + *touchOffsetX <= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icRightPos;
                    scrollenabled = false;
                }
                float icTopPos = _innerContainer->getTopBoundary();
                if (icTopPos + *touchOffsetY <= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icTopPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX > 0.0f && *touchOffsetY < 0.0f) // down right
            {
                float icLeftPos = _innerContainer->getLeftBoundary();
                if (icLeftPos + *touchOffsetX >= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icLeftPos;
                    scrollenabled = false;
                }
                float icTopPos = _innerContainer->getTopBoundary();
                if (icTopPos + *touchOffsetY <= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icTopPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX == 0.0f && *touchOffsetY > 0.0f) // up
            {
                float icBottomPos = _innerContainer->getBottomBoundary();
                if (icBottomPos + *touchOffsetY >= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icBottomPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX < 0.0f && *touchOffsetY == 0.0f) // left
            {
                float icRightPos = _innerContainer->getRightBoundary();
                if (icRightPos + *touchOffsetX <= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icRightPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX == 0.0f && *touchOffsetY < 0.0f) // down
            {
                float icTopPos = _innerContainer->getTopBoundary();
                if (icTopPos + *touchOffsetY <= _autoScrollDestination.y)
                {
                    *touchOffsetY = _autoScrollDestination.y - icTopPos;
                    scrollenabled = false;
                }
            }
            else if (*touchOffsetX > 0.0f && *touchOffsetY == 0.0f) // right
            {
                float icLeftPos = _innerContainer->getLeftBoundary();
                if (icLeftPos + *touchOffsetX >= _autoScrollDestination.x)
                {
                    *touchOffsetX = _autoScrollDestination.x - icLeftPos;
                    scrollenabled = false;
                }
            }
            break;
        }
        default:
            break;
    }
    return scrollenabled;
}

bool ScrollView::processScrollBottom(float* offsetYResult, float touchOffsetY)
{
	float boundary = (_bounceEnabled ? _bounceTopBoundary : _topBoundary);
	float icTopPos = _innerContainer->getTopBoundary();
	if (icTopPos + touchOffsetY <= boundary)
	{
		(*offsetYResult) = boundary - icTopPos;
		processScrollEvent(MoveDirection::TOP, false);
		return false;
	}
	return true;
}

bool ScrollView::processScrollTop(float* offsetYResult, float touchOffsetY)
{
	float boundary = (_bounceEnabled ? _bounceBottomBoundary : _bottomBoundary);
	float icBottomPos = _innerContainer->getBottomBoundary();
	if (icBottomPos + touchOffsetY >= boundary)
	{
		(*offsetYResult) = boundary - icBottomPos;
		processScrollEvent(MoveDirection::BOTTOM, false);
		return false;
	}
	return true;
}
	
bool ScrollView::processScrollRight(float* offsetXResult, float touchOffsetX)
{
	float boundary = (_bounceEnabled ? _bounceLeftBoundary : _leftBoundary);
	float icLeftPos = _innerContainer->getLeftBoundary();
	if (icLeftPos + touchOffsetX >= boundary)
	{
		(*offsetXResult) = boundary - icLeftPos;
		processScrollEvent(MoveDirection::LEFT, false);
		return false;
	}
	return true;
}

bool ScrollView::processScrollLeft(float* offsetXResult, float touchOffsetX)
{
	float boundary = (_bounceEnabled ? _bounceRightBoundary : _rightBoundary);
	float icRightPos = _innerContainer->getRightBoundary();
	if (icRightPos + touchOffsetX <= boundary)
	{
		(*offsetXResult) = boundary - icRightPos;
		processScrollEvent(MoveDirection::RIGHT, false);
		return false;
	}
	return true;
}

bool ScrollView::scrollChildren(float touchOffsetX, float touchOffsetY)
{
    processScrollingEvent();
	
	touchOffsetX = (_direction == Direction::VERTICAL ? 0 : touchOffsetX);
	touchOffsetY = (_direction == Direction::HORIZONTAL ? 0 : touchOffsetY);
	
	bool scrollenabled = true;
	float realOffsetX = touchOffsetX;
	float realOffsetY = touchOffsetY;
	
	if (touchOffsetY > 0.0f) // up
	{
		if (touchOffsetX > 0.0f) // right
		{
			scrollenabled = processScrollRight(&realOffsetX, touchOffsetX);
		}
		else if (touchOffsetX < 0.0f) // left
		{
			scrollenabled = processScrollLeft(&realOffsetX, touchOffsetX);
		}
		scrollenabled = processScrollTop(&realOffsetY, touchOffsetY);
	}
	else if (touchOffsetY < 0.0f) // down
	{
		if (touchOffsetX < 0.0f) // left
		{
			scrollenabled = processScrollLeft(&realOffsetX, touchOffsetX);
		}
		else if (touchOffsetX > 0.0f) // right
		{
			scrollenabled = processScrollRight(&realOffsetX, touchOffsetX);
		}
		scrollenabled = processScrollBottom(&realOffsetY, touchOffsetY);
	}
	else
	{
		if (touchOffsetX < 0.0f) // left
		{
			scrollenabled = processScrollLeft(&realOffsetX, touchOffsetX);
		}
		else if (touchOffsetX > 0.0f) // right
		{
			scrollenabled = processScrollRight(&realOffsetX, touchOffsetX);
		}
	}
	moveChildren(realOffsetX, realOffsetY);
	return scrollenabled;
}

void ScrollView::scrollToBottom(float second, bool attenuated)
{
    startAutoScrollChildrenWithDestination(Vec2(_innerContainer->getPosition().x, 0.0f), second, attenuated);
}

void ScrollView::scrollToTop(float second, bool attenuated)
{
    startAutoScrollChildrenWithDestination(Vec2(_innerContainer->getPosition().x,
                                                _contentSize.height - _innerContainer->getContentSize().height), second, attenuated);
}

void ScrollView::scrollToLeft(float second, bool attenuated)
{
    startAutoScrollChildrenWithDestination(Vec2(0.0f, _innerContainer->getPosition().y), second, attenuated);
}

void ScrollView::scrollToRight(float second, bool attenuated)
{
    startAutoScrollChildrenWithDestination(Vec2(_contentSize.width - _innerContainer->getContentSize().width,
                                                _innerContainer->getPosition().y), second, attenuated);
}

void ScrollView::scrollToTopLeft(float second, bool attenuated)
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    startAutoScrollChildrenWithDestination(Vec2(0.0f, _contentSize.height - _innerContainer->getContentSize().height), second, attenuated);
}

void ScrollView::scrollToTopRight(float second, bool attenuated)
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    startAutoScrollChildrenWithDestination(Vec2(_contentSize.width - _innerContainer->getContentSize().width,
                                                _contentSize.height - _innerContainer->getContentSize().height), second, attenuated);
}

void ScrollView::scrollToBottomLeft(float second, bool attenuated)
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    startAutoScrollChildrenWithDestination(Vec2::ZERO, second, attenuated);
}

void ScrollView::scrollToBottomRight(float second, bool attenuated)
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    startAutoScrollChildrenWithDestination(Vec2(_contentSize.width - _innerContainer->getContentSize().width, 0.0f), second, attenuated);
}

void ScrollView::scrollToPercentVertical(float percent, float second, bool attenuated)
{
    float minY = _contentSize.height - _innerContainer->getContentSize().height;
    float h = - minY;
    startAutoScrollChildrenWithDestination(Vec2(_innerContainer->getPosition().x, minY + percent * h / 100.0f), second, attenuated);
}

void ScrollView::scrollToPercentHorizontal(float percent, float second, bool attenuated)
{
    float w = _innerContainer->getContentSize().width - _contentSize.width;
    startAutoScrollChildrenWithDestination(Vec2(-(percent * w / 100.0f), _innerContainer->getPosition().y), second, attenuated);
}

void ScrollView::scrollToPercentBothDirection(const Vec2& percent, float second, bool attenuated)
{
    if (_direction != Direction::BOTH)
    {
        return;
    }
    float minY = _contentSize.height - _innerContainer->getContentSize().height;
    float h = - minY;
    float w = _innerContainer->getContentSize().width - _contentSize.width;
    startAutoScrollChildrenWithDestination(Vec2(-(percent.x * w / 100.0f), minY + percent.y * h / 100.0f), second, attenuated);
}

void ScrollView::jumpToBottom()
{
    jumpToDestination(Vec2(_innerContainer->getPosition().x, 0.0f));
}

void ScrollView::jumpToTop()
{
    jumpToDestination(Vec2(_innerContainer->getPosition().x,
                           _contentSize.height - _innerContainer->getContentSize().height));
}

void ScrollView::jumpToLeft()
{
    jumpToDestination(Vec2(0.0f, _innerContainer->getPosition().y));
}

void ScrollView::jumpToRight()
{
    jumpToDestination(Vec2(_contentSize.width - _innerContainer->getContentSize().width, _innerContainer->getPosition().y));
}

void ScrollView::jumpToTopLeft()
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    jumpToDestination(Vec2(0.0f, _contentSize.height - _innerContainer->getContentSize().height));
}

void ScrollView::jumpToTopRight()
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    jumpToDestination(Vec2(_contentSize.width - _innerContainer->getContentSize().width,
                           _contentSize.height - _innerContainer->getContentSize().height));
}

void ScrollView::jumpToBottomLeft()
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    jumpToDestination(Vec2::ZERO);
}

void ScrollView::jumpToBottomRight()
{
    if (_direction != Direction::BOTH)
    {
        CCLOG("Scroll diretion is not both!");
        return;
    }
    jumpToDestination(Vec2(_contentSize.width - _innerContainer->getContentSize().width, 0.0f));
}

void ScrollView::jumpToPercentVertical(float percent)
{
    float minY = _contentSize.height - _innerContainer->getContentSize().height;
    float h = - minY;
    jumpToDestination(Vec2(_innerContainer->getPosition().x, minY + percent * h / 100.0f));
}

void ScrollView::jumpToPercentHorizontal(float percent)
{
    float w = _innerContainer->getContentSize().width - _contentSize.width;
    jumpToDestination(Vec2(-(percent * w / 100.0f), _innerContainer->getPosition().y));
}

void ScrollView::jumpToPercentBothDirection(const Vec2& percent)
{
    if (_direction != Direction::BOTH)
    {
        return;
    }
    float minY = _contentSize.height - _innerContainer->getContentSize().height;
    float h = - minY;
    float w = _innerContainer->getContentSize().width - _contentSize.width;
    jumpToDestination(Vec2(-(percent.x * w / 100.0f), minY + percent.y * h / 100.0f));
}

void ScrollView::startRecordSlidAction()
{
    if (_autoScroll)
    {
        stopAutoScrollChildren();
    }
    if (_bouncing)
    {
        stopBounceChildren();
    }
    _slidTime = 0.0f;
}

void ScrollView::endRecordSlidAction()
{
    if (!checkNeedBounce() && _inertiaScrollEnabled)
    {
        if (_slidTime <= 0.016f)
        {
            return;
        }
        float totalDis = 0.0f;
        Vec2 dir;
        Vec2 touchEndPositionInNodeSpace = this->convertToNodeSpace(_touchEndPosition);
        Vec2 touchBeganPositionInNodeSpace = this->convertToNodeSpace(_touchBeganPosition);
        switch (_direction)
        {
            case Direction::VERTICAL:
                totalDis = touchEndPositionInNodeSpace.y - touchBeganPositionInNodeSpace.y;
                if (totalDis < 0.0f)
                {
                    dir = SCROLLDIR_DOWN;
                }
                else
                {
                    dir = SCROLLDIR_UP;
                }
                break;
            case Direction::HORIZONTAL:
                totalDis = touchEndPositionInNodeSpace.x - touchBeganPositionInNodeSpace.x;
                if (totalDis < 0.0f)
                {
                    dir = SCROLLDIR_LEFT;
                }
                else
                {
                    dir = SCROLLDIR_RIGHT;
                }
                break;
            case Direction::BOTH:
            {
                Vec2 subVector = touchEndPositionInNodeSpace - touchBeganPositionInNodeSpace;
                totalDis = subVector.getLength();
                dir = subVector.getNormalized();
                break;
            }
            default:
                break;
        }
        float orSpeed = MIN(fabs(totalDis)/(_slidTime), AUTOSCROLLMAXSPEED);
        startAutoScrollChildrenWithOriginalSpeed(dir, orSpeed, true, -1000);
        _slidTime = 0.0f;
    }
}

void ScrollView::handlePressLogic(Touch *touch)
{
    startRecordSlidAction();
    _bePressed = true;
}

void ScrollView::handleMoveLogic(Touch *touch)
{
    Vec2 touchPositionInNodeSpace = this->convertToNodeSpace(touch->getLocation());
    Vec2 previousTouchPositionInNodeSpace = this->convertToNodeSpace(touch->getPreviousLocation());
    Vec2 delta = touchPositionInNodeSpace - previousTouchPositionInNodeSpace;
    switch (_direction)
    {
        case Direction::VERTICAL: // vertical
        {
            scrollChildren(0.0f, delta.y);
            break;
        }
        case Direction::HORIZONTAL: // horizontal
        {
            scrollChildren(delta.x, 0.0f);
            break;
        }
        case Direction::BOTH: // both
        {
            scrollChildren(delta.x, delta.y);
            break;
        }
        default:
            break;
    }
}

void ScrollView::handleReleaseLogic(Touch *touch)
{
    endRecordSlidAction();
    _bePressed = false;
}

bool ScrollView::onTouchBegan(Touch *touch, Event *unusedEvent)
{
    bool pass = Layout::onTouchBegan(touch, unusedEvent);
    if (!_isInterceptTouch)
    {
        if (_hitted)
        {
            handlePressLogic(touch);
        }
    }
    return pass;
}

void ScrollView::onTouchMoved(Touch *touch, Event *unusedEvent)
{
    Layout::onTouchMoved(touch, unusedEvent);
    if (!_isInterceptTouch)
    {
        handleMoveLogic(touch);
    }
}

void ScrollView::onTouchEnded(Touch *touch, Event *unusedEvent)
{
    Layout::onTouchEnded(touch, unusedEvent);
    if (!_isInterceptTouch)
    {
        handleReleaseLogic(touch);
    }
    _isInterceptTouch = false;
}

void ScrollView::onTouchCancelled(Touch *touch, Event *unusedEvent)
{
    Layout::onTouchCancelled(touch, unusedEvent);
    if (!_isInterceptTouch)
    {
        handleReleaseLogic(touch);
    }
    _isInterceptTouch = false;
}

void ScrollView::update(float dt)
{
    if (_autoScroll)
    {
        autoScrollChildren(dt);
    }
    if (_bouncing)
    {
        bounceChildren(dt);
    }
    recordSlidTime(dt);
}

void ScrollView::recordSlidTime(float dt)
{
    if (_bePressed)
    {
        _slidTime += dt;
    }
}

void ScrollView::interceptTouchEvent(Widget::TouchEventType event, Widget *sender,Touch* touch)
{
    Vec2 touchPoint = touch->getLocation();
    switch (event)
    {
        case TouchEventType::BEGAN:
        {
            _isInterceptTouch = true;
            _touchBeganPosition = touch->getLocation();
            handlePressLogic(touch);
        }
        break;
        case TouchEventType::MOVED:
        {
            float offset = (sender->getTouchBeganPosition() - touchPoint).getLength();
            _touchMovePosition = touch->getLocation();
            if (offset > _childFocusCancelOffset)
            {
                sender->setHighlighted(false);
                handleMoveLogic(touch);
            }
        }
        break;

        case TouchEventType::CANCELED:
        case TouchEventType::ENDED:
        {
            _touchEndPosition = touch->getLocation();
            handleReleaseLogic(touch);
            if (sender->isSwallowTouches())
            {
                _isInterceptTouch = false;
            }
        }
        break;
    }
}

void ScrollView::processScrollEvent(MoveDirection dir, bool bounce)
{
	ScrollviewEventType scrollEventType;
	EventType eventType;
	switch(dir) {
		case MoveDirection::TOP:
		{
			scrollEventType = (bounce ? SCROLLVIEW_EVENT_BOUNCE_TOP : SCROLLVIEW_EVENT_SCROLL_TO_TOP);
			eventType = (bounce ? EventType::BOUNCE_TOP : EventType::SCROLL_TO_TOP);
			break;
		}
		case MoveDirection::BOTTOM:
		{
			scrollEventType = (bounce ? SCROLLVIEW_EVENT_BOUNCE_BOTTOM : SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM);
			eventType = (bounce ? EventType::BOUNCE_BOTTOM : EventType::SCROLL_TO_BOTTOM);
			break;
		}
		case MoveDirection::LEFT:
		{
			scrollEventType = (bounce ? SCROLLVIEW_EVENT_BOUNCE_LEFT : SCROLLVIEW_EVENT_SCROLL_TO_LEFT);
			eventType = (bounce ? EventType::BOUNCE_LEFT : EventType::SCROLL_TO_LEFT);
			break;
		}
		case MoveDirection::RIGHT:
		{
			scrollEventType = (bounce ? SCROLLVIEW_EVENT_BOUNCE_RIGHT : SCROLLVIEW_EVENT_SCROLL_TO_RIGHT);
			eventType = (bounce ? EventType::BOUNCE_RIGHT : EventType::SCROLL_TO_RIGHT);
			break;
		}
	}
	dispatchEvent(scrollEventType, eventType);
}

void ScrollView::processScrollingEvent()
{
	dispatchEvent(SCROLLVIEW_EVENT_SCROLLING, EventType::SCROLLING);
}

void ScrollView::dispatchEvent(ScrollviewEventType scrollEventType, EventType eventType)
{
	this->retain();
	if (_scrollViewEventListener && _scrollViewEventSelector)
	{
		(_scrollViewEventListener->*_scrollViewEventSelector)(this, scrollEventType);
	}
	if (_eventCallback)
	{
		_eventCallback(this, eventType);
	}
	if (_ccEventCallback)
	{
		_ccEventCallback(this, static_cast<int>(eventType));
	}
	this->release();
}

void ScrollView::addEventListenerScrollView(Ref *target, SEL_ScrollViewEvent selector)
{
    _scrollViewEventListener = target;
    _scrollViewEventSelector = selector;
}

void ScrollView::addEventListener(const ccScrollViewCallback& callback)
{
    _eventCallback = callback;
}

void ScrollView::setDirection(Direction dir)
{
    _direction = dir;
}

ScrollView::Direction ScrollView::getDirection()const
{
    return _direction;
}

void ScrollView::setBounceEnabled(bool enabled)
{
    _bounceEnabled = enabled;
}

bool ScrollView::isBounceEnabled() const
{
    return _bounceEnabled;
}

void ScrollView::setInertiaScrollEnabled(bool enabled)
{
    _inertiaScrollEnabled = enabled;
}

bool ScrollView::isInertiaScrollEnabled() const
{
    return _inertiaScrollEnabled;
}

Layout* ScrollView::getInnerContainer()const
{
    return _innerContainer;
}

void ScrollView::setLayoutType(Type type)
{
    _innerContainer->setLayoutType(type);
}

Layout::Type ScrollView::getLayoutType() const
{
    return _innerContainer->getLayoutType();
}

void ScrollView::doLayout()
{
    if (!_doLayoutDirty)
    {
        return;
    }
    _doLayoutDirty = false;
}

std::string ScrollView::getDescription() const
{
    return "ScrollView";
}

Widget* ScrollView::createCloneInstance()
{
    return ScrollView::create();
}

void ScrollView::copyClonedWidgetChildren(Widget* model)
{
    Layout::copyClonedWidgetChildren(model);
}

void ScrollView::copySpecialProperties(Widget *widget)
{
    ScrollView* scrollView = dynamic_cast<ScrollView*>(widget);
    if (scrollView)
    {
        Layout::copySpecialProperties(widget);
        setInnerContainerSize(scrollView->getInnerContainerSize());
        setDirection(scrollView->_direction);
        setBounceEnabled(scrollView->_bounceEnabled);
        setInertiaScrollEnabled(scrollView->_inertiaScrollEnabled);
        _scrollViewEventListener = scrollView->_scrollViewEventListener;
        _scrollViewEventSelector = scrollView->_scrollViewEventSelector;
        _eventCallback = scrollView->_eventCallback;
        _ccEventCallback = scrollView->_ccEventCallback;
    }
}

Widget* ScrollView::findNextFocusedWidget(cocos2d::ui::Widget::FocusDirection direction, cocos2d::ui::Widget *current)
{
    if (this->getLayoutType() == Layout::Type::VERTICAL
        || this->getLayoutType() == Layout::Type::HORIZONTAL)
    {
        return _innerContainer->findNextFocusedWidget(direction, current);
    }
    else
    {
        return Widget::findNextFocusedWidget(direction, current);
    }
}
}

NS_CC_END
