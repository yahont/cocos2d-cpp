#include "CCApplication_bada.h"
#include "CCDirector.h"
#include "Gbk_Unicode.h"

using namespace Osp::App;
using namespace Osp::System;
using namespace Osp::Base;
using namespace Osp::Base::Runtime;

NS_CC_BEGIN;

static CCApplication * s_pApplication = NULL;
static long long s_nAnimationInterval = 1000/60;

CCApplication::CCApplication() :
		m_pTimer(null)
{
	CCLog("CCApplication constructor...");
	s_pApplication = this;

}

CCApplication::~CCApplication()
{
	s_pApplication = NULL;
}

CCApplication::Orientation CCApplication::setOrientation(CCApplication::Orientation orientation)
{
    // swap width and height
    CCEGLView * pView = CCDirector::sharedDirector()->getOpenGLView();
    if (pView)
    {
        return (Orientation)pView->setDeviceOrientation(orientation);
    }
    return (Orientation)CCDirector::sharedDirector()->getDeviceOrientation();
}

CCRect CCApplication::statusBarFrame(CCRect * rect)
{
    if (rect)
    {
        // bada doesn't have status bar.
        *rect = CCRectMake(0, 0, 0, 0);
    }
    return *rect;
}

//////////////////////////////////////////////////////////////////////////
/// Implement static class member
//////////////////////////////////////////////////////////////////////////
CCApplication& CCApplication::sharedApplication()
{
    CC_ASSERT(s_pApplication);
    return *s_pApplication;
}

ccLanguageType CCApplication::getCurrentLanguage()
{
    return kLanguageEnglish;
}

void CCApplication::setAnimationInterval(double interval)
{
    s_nAnimationInterval = (long long)(interval * 1000.0f);
}

bool CCApplication::OnAppInitializing(AppRegistry& appRegistry)
{
	result r = E_FAILURE;
	if (! initInstance() || !applicationDidFinishLaunching())
		return false;
	m_pTimer = new Timer;
	if (null == m_pTimer)
	{
		return E_FAILURE;
	}

	r = m_pTimer->Construct(*this);
	if (IsFailed(r))
	{
		delete m_pTimer;
		m_pTimer = null;
		return E_FAILURE;
	}
	m_pTimer->Start(s_nAnimationInterval);

	return true;
}


bool CCApplication::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	if (m_pTimer)
	{
		m_pTimer->Cancel();
		delete m_pTimer;
		m_pTimer = null;
	}
	return true;
}


void CCApplication::OnForeground(void)
{
	if (m_pTimer)
		m_pTimer->Start(s_nAnimationInterval);
}


void CCApplication::OnBackground(void)
{
	if (m_pTimer)
		m_pTimer->Cancel();
}


void CCApplication::OnLowMemory(void)
{
}


void CCApplication::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
}

void CCApplication::OnTimerExpired(Timer& timer)
{
	m_pTimer->Start(s_nAnimationInterval);
	CCDirector::sharedDirector()->mainLoop();
}

NS_CC_END;
