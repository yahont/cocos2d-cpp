/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.

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

#ifndef __cocos2d_libs__CCMouseEvent__
#define __cocos2d_libs__CCMouseEvent__

#include "base/CCEvent.h"
#include "math/CCGeometry.h"

#define MOUSE_BUTTON_LEFT       0
#define MOUSE_BUTTON_RIGHT      1
#define MOUSE_BUTTON_MIDDLE     2
#define MOUSE_BUTTON_4          3
#define MOUSE_BUTTON_5          4
#define MOUSE_BUTTON_6          5
#define MOUSE_BUTTON_7          6
#define MOUSE_BUTTON_8          7

/**
 * @addtogroup base
 * @{
 */

namespace cocos2d {

/** @class EventMouse
 * @brief The mouse event.
 */
class CC_DLL EventMouse : public Event
{
public:
    /**
    * MouseEventType Different types of MouseEvent.
    */
    enum class MouseEventType
    {
        MOUSE_NONE,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        MOUSE_SCROLL,
    };

    /** Constructor.
     * @param mouseEventCode A given mouse event type.
     */
    EventMouse(MouseEventType mouseEventCode);

    /** Set mouse scroll data.
     * 
     * @param scrollX The scroll data of x axis.
     * @param scrollY The scroll data of y axis.
     */
    void setScrollData(float scrollX, float scrollY) { _scrollX = scrollX; _scrollY = scrollY; }

    /** Get mouse scroll data of x axis.
     * @return The scroll data of x axis.
     */
    float getScrollX() const { return _scrollX; }

    /** Get mouse scroll data of y axis.
     * @return The scroll data of y axis.
     */
    float getScrollY() const { return _scrollY; }

    /** Set the cursor position.
     * @param x The x coordinate of cursor position.
     * @param y The y coordinate of cursor position.
     */
    void setCursorPosition(float x, float y) { 
        _x = x;
        _y = y;
        _prevPoint = _point;
        _point.x = x;
        _point.y = y;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
        }
    }

    /** Set mouse button.
     * @param button a given mouse button.
     */
    void setMouseButton(int button) { _mouseButton = button; }

    /** Get mouse button.
     * @return The mouse button.
     */
    int getMouseButton() const { return _mouseButton; }

    /** Get the cursor position of x axis.
     * @return The x coordinate of cursor position.
     */

    float getCursorX() const { return _x; }

    /** Get the cursor position of y axis.
     * @return The y coordinate of cursor position.
     */
    float getCursorY() const { return _y; }

    /** Returns the current touch location in OpenGL coordinates.
     * @return The current touch location in OpenGL coordinates.
     */
    Vec2 getLocation() const;
    
    /** Returns the previous touch location in OpenGL coordinates.
     * @return The previous touch location in OpenGL coordinates.
     */
    Vec2 getPreviousLocation() const;

    /** Returns the start touch location in OpenGL coordinates.
     * @return The start touch location in OpenGL coordinates.
     */
    Vec2 getStartLocation() const;

    /** Returns the delta of 2 current touches locations in screen coordinates.
     * @return The delta of 2 current touches locations in screen coordinates.
     */
    Vec2 getDelta() const;

    /** Returns the current touch location in screen coordinates.
     * @return The current touch location in screen coordinates.
     */
    Vec2 getLocationInView() const;

    /** Returns the previous touch location in screen coordinates.
     * @return The previous touch location in screen coordinates.
     */
    Vec2 getPreviousLocationInView() const;

    /** Returns the start touch location in screen coordinates.
     * @return The start touch location in screen coordinates.
     */
    Vec2 getStartLocationInView() const;


private:
    MouseEventType _mouseEventType;
    int _mouseButton;
    float _x;
    float _y;
    float _scrollX;
    float _scrollY;

    bool _startPointCaptured;
    Vec2 _startPoint;
    Vec2 _point;
    Vec2 _prevPoint;

    friend class EventListenerMouse;
};

} // namespace cocos2d

// end of base group
/// @}

#endif /* defined(__cocos2d_libs__CCMouseEvent__) */
