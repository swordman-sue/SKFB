#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

class GameEngine;
#include "cocos2d.h"

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by CCDirector.
*/
class  AppDelegate : private cocos2d::CCApplication
{
public:
    AppDelegate();
    virtual ~AppDelegate();

	virtual void initGLContextAttrs();

    /**
    @brief    Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();
    
    void applicationWillTerminate();

	void listenRendererRecreated(cocos2d::EventCustom *event);

private:
	GameEngine* m_engine;
	cocos2d::EventListenerCustom* _rendererRecreatedListener;
};

#endif // _APP_DELEGATE_H_

