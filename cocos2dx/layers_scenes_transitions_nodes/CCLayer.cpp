/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#include "CCLayer.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include <stdarg.h>


// CCLayer
CCLayer::CCLayer()
{
	/// @todo director
// 	if( (self=[super init]) ) {
// 
// 		CGSize s = [[CCDirector sharedDirector] winSize];
// 		anchorPoint_ = ccp(0.5f, 0.5f);
// 		[self setContentSize:s];
// 		self.isRelativeAnchorPoint = NO;
// 
// 		isTouchEnabled = NO;
// 		isAccelerometerEnabled = NO;
// 	}
// 
// 	return self;
}

CCLayer::~CCLayer()
{
}


/// Touch and Accelerometer related

void CCLayer::registerWithTouchDispatcher()
{
	CCTouchDispatcher::getSharedDispatcher()->addStandardDelegate(this,0);
}


/// isTouchEnabled getter
bool CCLayer::getIsTouchEnabled()
{
	return m_bIsTouchEnabled;
}
/// isTouchEnabled setter
void CCLayer::setIsTouchEnabled(bool enabled)
{
	if (m_bIsTouchEnabled != enabled)
	{
		m_bIsTouchEnabled = enabled;
		if (m_bIsRunning)
		{
			if (enabled)
			{
				this->registerWithTouchDispatcher();
			}
			else
			{
				/// @todo param this error
				//CCTouchDispatcher::getSharedDispatcher()->removeDelegate(this);
			}
		}
	}
	
	/** objc
	if( m_bIsTouchEnabled != enabled ) {
		m_bIsTouchEnabled = enabled;
	if( isRunning_ ) {
	if( enabled )
	[self registerWithTouchDispatcher];
	else
	[[CCTouchDispatcher sharedDispatcher] removeDelegate:self];
	}
	}*/
}

/// isAccelerometerEnabled getter
bool CCLayer::getIsAccelerometerEnabled()
{
	return m_bIsAccelerometerEnabled;
}
/// isAccelerometerEnabled setter
void CCLayer::setIsAccelerometerEnabled(bool enabled)
{
	/** @todo UIAccelerometer
	if( enabled != isAccelerometerEnabled ) {
		isAccelerometerEnabled = enabled;
		if( isRunning_ ) {
			if( enabled )
				[[UIAccelerometer sharedAccelerometer] setDelegate:self];
			else
				[[UIAccelerometer sharedAccelerometer] setDelegate:nil];
		}
	}*/
}


/// Callbacks
void CCLayer::onEnter()
{
	/** @todo UIAccelerometer
	// register 'parent' nodes first
	// since events are propagated in reverse order
	if (isTouchEnabled)
		[self registerWithTouchDispatcher];

	// then iterate over all the children
	[super onEnter];

	if( isAccelerometerEnabled )
		[[UIAccelerometer sharedAccelerometer] setDelegate:self];*/
}

void CCLayer::onExit()
{
	/**
	if( isTouchEnabled )
		[[CCTouchDispatcher sharedDispatcher] removeDelegate:self];

	if( isAccelerometerEnabled )
		[[UIAccelerometer sharedAccelerometer] setDelegate:nil];

	[super onExit];*/
}

/** @todo UITouch
-(BOOL) ccTouchBegan:(UITouch *)touch withEvent:(UIEvent *)event
{
	NSAssert(NO, @"Layer#ccTouchBegan override me");
	return YES;
}*/

/// ColorLayer


CCColorLayer::CCColorLayer()
{
}
CCColorLayer::~CCColorLayer()
{
}

// Opacity and RGB color protocol
/// opacity getter
GLubyte CCColorLayer::getOpacity()
{
	return m_cOpacity;
}
/// opacity setter
void CCColorLayer::setOpacity(GLubyte var)
{
	m_cOpacity = var;
	updateColor();
}

/// color getter
ccColor3B CCColorLayer::getColor()
{
	return m_tColor;
}

/// color setter
void CCColorLayer::setColor(ccColor3B var)
{
	m_tColor = var;
	updateColor();
}


/// blendFunc getter
ccBlendFunc CCColorLayer::getBlendFunc()
{
	return m_tBlendFunc;
}
/// blendFunc setter
void CCColorLayer::setBlendFunc(ccBlendFunc var)
{
	m_tBlendFunc = var;
}


CCColorLayer * CCColorLayer::layerWithColorWidthHeight(ccColor4B color, GLfloat width, GLfloat height)
{
	CCColorLayer * pLayer = new CCColorLayer();
	pLayer->initWithColorWidthHeight(color,width,height);
	pLayer->autorelease();
	return pLayer;
}
CCColorLayer * CCColorLayer::layerWithColor(ccColor4B color)
{
	CCColorLayer * pLayer = new CCColorLayer();
	pLayer->initWithColor(color);
	pLayer->autorelease();
	return pLayer;
}

CCColorLayer* CCColorLayer::initWithColorWidthHeight(ccColor4B color, GLfloat width, GLfloat height)
{
	// default blend function
	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;

	m_tColor.r = color.r;
	m_tColor.g = color.g;
	m_tColor.b = color.b;
	m_cOpacity = color.a;

	for (UINT32 i=0; i<sizeof(m_pSquareVertices) / sizeof(m_pSquareVertices[0]); i++ )
		m_pSquareVertices[i] = 0.0f;

	this->updateColor();
	this->setContentSize(CGSizeMake(width,height));
	return this;
}

CCColorLayer * CCColorLayer::initWithColor(ccColor4B color)
{
	/** @todo director
	CGSize s = [[CCDirector sharedDirector] winSize];
	return [self initWithColor:color width:s.width height:s.height];*/
	return NULL;
}

/// override contentSize
void CCColorLayer::setContentSize(CGSize size)
{
	m_pSquareVertices[2] = size.width;
	m_pSquareVertices[5] = size.height;
	m_pSquareVertices[6] = size.width;
	m_pSquareVertices[7] = size.height;

	this->setContentSize(size);
}

void CCColorLayer::changeWidthAndHeight(GLfloat w ,GLfloat h)
{
	this->setContentSize(CGSizeMake(w, h));
}

void CCColorLayer::changeWidth(GLfloat w)
{
	this->setContentSize(CGSizeMake(w, m_tContentSize.height));
}

void CCColorLayer::changeHeight(GLfloat h)
{
	this->setContentSize(CGSizeMake(m_tContentSize.width, h));
}

void CCColorLayer::updateColor()
{
	for( UINT32 i=0; i < sizeof(m_pSquareColors) / sizeof(m_pSquareColors[0]); i++ )
	{
		if( i % 4 == 0 )
			m_pSquareColors[i] = m_tColor.r;
		else if( i % 4 == 1)
			m_pSquareColors[i] = m_tColor.g;
		else if( i % 4 ==2  )
			m_pSquareColors[i] = m_tColor.b;
		else
			m_pSquareColors[i] = m_cOpacity;
	}
}

void CCColorLayer::draw()
{		
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, GL_COLOR_ARRAY
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	glVertexPointer(2, GL_FLOAT, 0, m_pSquareVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_pSquareColors);

	bool newBlend = false;
	if( m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST ) {
		newBlend = true;
		glBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);
	}
	else if( m_cOpacity != 255 ) {
		newBlend = true;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if( newBlend )
		glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);

	// restore default GL state
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
}


/// MultiplexLayer

CCMultiplexLayer::CCMultiplexLayer()
{
}
CCMultiplexLayer::~CCMultiplexLayer()
{
	m_pLayers->release();
}

CCMultiplexLayer * CCMultiplexLayer::layerWithLayers(CCLayer * layer, ...)
{
	va_list args;
	va_start(args,layer);

	CCMultiplexLayer * pMultiplexLayer = new CCMultiplexLayer();
	pMultiplexLayer->initWithLayers(layer, args);
	pMultiplexLayer->autorelease();

	va_end(args);
	return pMultiplexLayer;
}

CCMultiplexLayer * CCMultiplexLayer::initWithLayers(CCLayer *layer, va_list params)
{
	m_pLayers = new NSMutableArray<CCLayer*>(5);
	m_pLayers->retain();

	m_pLayers->addObject(layer);

	CCLayer *l = va_arg(params,CCLayer*);
	while( l ) {
		m_pLayers->addObject(l);
		l = va_arg(params,CCLayer*);
	}

	m_nEnabledLayer = 0;
	this->addChild(m_pLayers->getObjectAtIndex(m_nEnabledLayer));

	return this;
}


void CCMultiplexLayer::switchTo(unsigned int n)
{
	NSAssert( n < m_pLayers->count(), "Invalid index in MultiplexLayer switchTo message" );

	this->removeChild(m_pLayers->getObjectAtIndex(m_nEnabledLayer), true);

	m_nEnabledLayer = n;

	this->addChild(m_pLayers->getObjectAtIndex(n));
}

void CCMultiplexLayer::switchToAndReleaseMe(unsigned int n)
{
	NSAssert( n < m_pLayers->count(), "Invalid index in MultiplexLayer switchTo message" );

	this->removeChild(m_pLayers->getObjectAtIndex(m_nEnabledLayer), true);

	//[layers replaceObjectAtIndex:enabledLayer withObject:[NSNull null]];
	m_pLayers->replaceObjectAtIndex(m_nEnabledLayer, NULL);

	m_nEnabledLayer = n;

	this->addChild(m_pLayers->getObjectAtIndex(n));
}
