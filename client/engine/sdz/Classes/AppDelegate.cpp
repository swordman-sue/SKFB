
#include "AppDelegate.h"
#include "AppMacros.h"
#include "gameengine/gameengine.h"

#include "view/font/fontmanager.h"
#include "view/scenemanager.h"
#include "view/render/batchrenderer.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
	// 监听EVENT_RENDERER_RECREATED事件并做相应处理 add by swordman sue 2017-10-20
#if CC_ENABLE_CACHE_TEXTURE_DATA
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

	_rendererRecreatedListener = cocos2d::EventListenerCustom::create(EVENT_RENDERER_RECREATED, CC_CALLBACK_1(AppDelegate::listenRendererRecreated, this));
	eventDispatcher->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
	// end
}

AppDelegate::~AppDelegate() 
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	m_engine->Stop();
#endif
	m_engine->Release();
	GameEngine::DelInstance();
}

void AppDelegate::initGLContextAttrs()
{
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

	GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() 
{
	m_engine = GameEngine::GetInstance();
	bool ret = m_engine->Init();
	ret = ret && m_engine->Start();
	if (ret)
	{
		Director::getInstance()->SetFrameHandler(m_engine);
		SceneManager::GetInstance()->RunStartUpScene();
	}
	return ret;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {

	m_engine->Pause();

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	Director::getInstance()->stopAnimation();
#endif

	FontManager::sharedFontManager()->clearAllFonts();
	BatchRendererManager::sharedRendererManager()->setDirty(true);

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {

	m_engine->Resume();

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	Director::getInstance()->startAnimation();
#endif

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

// add by swordman sue 2018-09-04
void AppDelegate::applicationWillTerminate()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    m_engine->Stop();
    m_engine->Release();
	cocos2d::CCDirector::sharedDirector()->end();
	cocos2d::CCDirector::sharedDirector()->mainLoop();
#endif
}

// add by swordman sue 2017-10-20
void AppDelegate::listenRendererRecreated(cocos2d::EventCustom *event)
{
	cocos2d::log("AppDelegate::listenRendererRecreated beg");

	FontManager::sharedFontManager()->clearAllFonts();
	BatchRendererManager::sharedRendererManager()->setDirty(true);

	cocos2d::log("AppDelegate::listenRendererRecreated end\n");
}
// end
