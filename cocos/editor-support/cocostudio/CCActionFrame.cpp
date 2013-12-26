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

#include "CCActionFrame.h"
#include "CCActionEaseEx.h"

using namespace cocos2d;

namespace cocostudio {

ActionFrame::ActionFrame()
: _frameType(0)
, _frameIndex(0)
, _fTime(0.0f)
, _easingType(FrameEaseType::FrameEase_Linear)
{

}
ActionFrame::~ActionFrame()
{

}

void ActionFrame::setFrameIndex(int index)
{
	_frameIndex = index;
}
int ActionFrame::getFrameIndex()
{
	return _frameIndex;
}

void ActionFrame::setFrameTime(float fTime)
{
	_fTime = fTime;
}
float ActionFrame::getFrameTime()
{
	return _fTime;
}

void ActionFrame::setFrameType(int frameType)
{
	_frameType = frameType;
}
int ActionFrame::getFrameType()
{
	return _frameType;
}

void ActionFrame::setEasingType(int easingType)
{
	_easingType = (FrameEaseType)easingType;
}
int ActionFrame::getEasingType()
{
	return (int)_easingType;
}

ActionInterval* ActionFrame::getAction(float fDuration)
{
	log("Need a definition of <getAction> for ActionFrame");
	return nullptr;
}
ActionInterval* ActionFrame::getAction(float fDuration,ActionFrame* srcFrame)
{
	return this->getAction(fDuration);
}

void ActionFrame::setEasingParameter(std::vector<float>& parameter)
{
	_Parameter.clear();

	for (size_t i = 0; i<parameter.size(); i++)
	{
		_Parameter.push_back(parameter[i]);
	}
}

ActionInterval* ActionFrame::getEasingAction(ActionInterval* action)
{
	if (action == nullptr)
	{
		return nullptr;
	}

	switch (_easingType)
	{
	case FrameEaseType::FrameEase_Custom:
		{
			EaseBezierAction* cAction = EaseBezierAction::create(action);
			cAction->setBezierParamer(_Parameter[0],_Parameter[1],_Parameter[2],_Parameter[3]);
			return cAction;
		}
		break;
	case FrameEaseType::FrameEase_Linear:
		return action;
		break;
	case FrameEaseType::FrameEase_Sine_EaseIn:
		return EaseSineIn::create(action);
		break;
	case FrameEaseType::FrameEase_Sine_EaseOut:
		return EaseSineOut::create(action);
		break;
	case FrameEaseType::FrameEase_Sine_EaseInOut:
		return EaseSineInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Quad_EaseIn:
		return EaseQuadraticActionIn::create(action);
		break;
	case FrameEaseType::FrameEase_Quad_EaseOut:
		return EaseQuadraticActionOut::create(action);
		break;
	case FrameEaseType::FrameEase_Quad_EaseInOut:
		return EaseQuadraticActionInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Cubic_EaseIn:
		return EaseCubicActionIn::create(action);
		break;
	case FrameEaseType::FrameEase_Cubic_EaseOut:
		return EaseCubicActionOut::create(action);
		break;
	case FrameEaseType::FrameEase_Cubic_EaseInOut:
		return EaseCubicActionInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Quart_EaseIn:
		return EaseQuarticActionIn::create(action);
		break;
	case FrameEaseType::FrameEase_Quart_EaseOut:
		return EaseQuadraticActionOut::create(action);
		break;
	case FrameEaseType::FrameEase_Quart_EaseInOut:
		return EaseQuarticActionInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Quint_EaseIn:
		return EaseQuinticActionIn::create(action);
		break;
	case FrameEaseType::FrameEase_Quint_EaseOut:
		return EaseQuinticActionOut::create(action);
		break;
	case FrameEaseType::FrameEase_Quint_EaseInOut:
		return EaseQuinticActionInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Expo_EaseIn:
		return EaseExponentialIn::create(action);
		break;
	case FrameEaseType::FrameEase_Expo_EaseOut:
		return EaseExponentialOut::create(action);
		break;
	case FrameEaseType::FrameEase_Expo_EaseInOut:
		return EaseExponentialInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Circ_EaseIn:
		return EaseCircleActionIn::create(action);
		break;
	case FrameEaseType::FrameEase_Circ_EaseOut:
		return EaseCircleActionOut::create(action);
		break;
	case FrameEaseType::FrameEase_Circ_EaseInOut:
		return EaseCircleActionInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Elastic_EaseIn:
		{
			EaseElasticIn* cAction = EaseElasticIn::create(action);
			cAction->setPeriod(_Parameter[0]);
			return cAction;
		}
		break;
	case FrameEaseType::FrameEase_Elastic_EaseOut:
		{
			EaseElasticOut* cAction = EaseElasticOut::create(action);
			cAction->setPeriod(_Parameter[0]);
			return cAction;
		}
		break;
	case FrameEaseType::FrameEase_Elastic_EaseInOut:
		{
			EaseElasticInOut* cAction = EaseElasticInOut::create(action);
			cAction->setPeriod(_Parameter[0]);
			return cAction;
		}
		break;
	case FrameEaseType::FrameEase_Back_EaseIn:
		return EaseBackIn::create(action);
		break;
	case FrameEaseType::FrameEase_Back_EaseOut:
		return EaseBackOut::create(action);
		break;
	case FrameEaseType::FrameEase_Back_EaseInOut:
		return EaseBackInOut::create(action);
		break;
	case FrameEaseType::FrameEase_Bounce_EaseIn:
		return EaseBounceIn::create(action);
		break;
	case FrameEaseType::FrameEase_Bounce_EaseOut:
		return EaseBounceOut::create(action);
		break;
	case FrameEaseType::FrameEase_Bounce_EaseInOut:
		return EaseBounceInOut::create(action);
		break;
	default:
		return action;
		break;
	}
}
//////////////////////////////////////////////////////////////////////////

ActionMoveFrame::ActionMoveFrame()
	: _position(Point(0.0f,0.0f))
{
	_frameType = (int)kKeyframeMove;
}
ActionMoveFrame::~ActionMoveFrame()
{

}
void ActionMoveFrame::setPosition(Point pos)
{
	_position = pos;
}
Point ActionMoveFrame::getPosition()
{
	return _position;
}
ActionInterval* ActionMoveFrame::getAction(float fDuration)
{
	return this->getEasingAction(CCMoveTo::create(fDuration,_position));
}
//////////////////////////////////////////////////////////////////////////

ActionScaleFrame::ActionScaleFrame()
	: _scaleX(1.0f)
	, _scaleY(1.0f)
{
	_frameType = (int)kKeyframeScale;
}

ActionScaleFrame::~ActionScaleFrame()
{

}

void ActionScaleFrame::setScaleX(float scaleX)
{
	_scaleX = scaleX;
}

float ActionScaleFrame::getScaleX()
{
	return _scaleX;
}

void ActionScaleFrame::setScaleY(float scaleY)
{
	_scaleY = scaleY;
}

float ActionScaleFrame::getScaleY()
{
	return _scaleY;
}

ActionInterval* ActionScaleFrame::getAction(float fDuration)
{
	return this->getEasingAction(CCScaleTo::create(fDuration,_scaleX,_scaleY));
}

ActionRotationFrame::ActionRotationFrame()
	: _rotation(0.0f)
{
	_frameType = (int)kKeyframeRotate;
}

ActionRotationFrame::~ActionRotationFrame()
{

}

void ActionRotationFrame::setRotation(float rotation)
{
	_rotation = rotation;
}

float ActionRotationFrame::getRotation()
{
	return _rotation;
}

ActionInterval* ActionRotationFrame::getAction(float fDuration)
{
	return this->getEasingAction(CCRotateTo::create(fDuration,_rotation));
}
ActionInterval* ActionRotationFrame::getAction(float fDuration,ActionFrame* srcFrame)
{
	ActionRotationFrame* srcRotationFrame = static_cast<ActionRotationFrame*>(srcFrame);
	if (srcRotationFrame == nullptr)
	{
		return this->getAction(fDuration);
	}
	else
	{
		float diffRotation = _rotation - srcRotationFrame->_rotation;
		return this->getEasingAction(CCRotateBy::create(fDuration,diffRotation));
	}
}

ActionFadeFrame::ActionFadeFrame()
	: _opacity(255)
{
	_frameType = (int)kKeyframeFade;
}

ActionFadeFrame::~ActionFadeFrame()
{

}

void ActionFadeFrame::setOpacity(int opacity)
{
	_opacity = opacity;
}

int ActionFadeFrame::getOpacity()
{
	return _opacity;
}

ActionInterval* ActionFadeFrame::getAction(float fDuration)
{
	return this->getEasingAction(CCFadeTo::create(fDuration,_opacity));
}


ActionTintFrame::ActionTintFrame()
	: _color(Color3B(255,255,255))
{
	_frameType = (int)kKeyframeTint;
}

ActionTintFrame::~ActionTintFrame()
{

}

void ActionTintFrame::setColor(Color3B ccolor)
{
	_color = ccolor;
}

Color3B ActionTintFrame::getColor()
{
	return _color;
}

ActionInterval* ActionTintFrame::getAction(float fDuration)
{
	return this->getEasingAction(CCTintTo::create(fDuration,_color.r,_color.g,_color.b));
}


}