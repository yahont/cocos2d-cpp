// Application application header file.
 
// Original file name: HelloWorldApp.h
// Generated by TOPS Builder:Project wizard,Date:2010-8-3
 

#ifndef  __HelloWorld_App_H__
#define  __HelloWorld_App_H__
#include "CCXApplication.h"
#include "CCXEGLView.h"

class  THelloWorldApp  :  public  cocos2d::CCXApplication
{
public:
	THelloWorldApp();
	~THelloWorldApp();

    virtual Boolean initCocos2d();

public:
	virtual Boolean EventHandler(EventType * pEvent);

protected:
    cocos2d::CCXEGLView * m_pMainWnd;
};
 

#endif

