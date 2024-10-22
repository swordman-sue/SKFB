//
//  gameengine.cpp 
//
//  Created by swordman sue
//  Copyright (c) ZLGame All rights reserved.
//

#include "gameengine.h"

#include "httpmanager/httpmanager.h"
#include "filesystem/filesystemex.h"
#include "luaengine/luaengine.h"
#include "netadapter/netadapter.h"

#include "sound/soundmanager.h"
#include "sound/voicemanager.h"
#include "location/location.h"
#include "sdkhandle/sdkhandle.h"
#include "msgpush/msgpush.h"

#include "resmanager/resmanager.h"
#include "common/platform/system.h"
#include "zqcommon/logutil.h"

#include "view/viewhelper.h"

#include "ui/uiwidgetfactory.h"
#include "view/uiwidget/uibuttonex.h"
#include "view/uiwidget/uicheckboxex.h"
#include "view/uiwidget/uiimageviewex.h"
#include "view/uiwidget/uitextex.h"
#include "view/uiwidget/uitextatlasex.h"
#include "view/uiwidget/uiloadingbarex.h"
#include "view/uiwidget/uisliderex.h"
#include "view/uiwidget/uitextfieldex.h"
#include "view/uiwidget/uilayoutex.h"
#include "view/uiwidget/uiscrollviewex.h"
#include "view/uiwidget/uitouchgroupex.h"
#include "view/uiwidget/uilistviewex.h"
#include "view/uiwidget/uipageviewex.h"
#include "view/widget/spriteex.h"
#include "filesystem/setting.h"
#include "view/scenemanager.h"

#include "view/action/actiontimelineex.h"

static GameEngine* s_instance = nullptr;

GameEngine::GameEngine()
{
	m_http_manager = HttpManager::GetInstance();
	m_filesystem = FileSystemEx::GetInstance();
	m_lua_engine = LuaEngine::GetInstance();
	m_netadapter = NetAdapter::GetInstance();

	m_sound_manager = SoundManager::GetInstance();
	m_voice_manager = VoiceManager::GetInstance();

	m_location = Location::GetInstance();
	m_msgpush = MsgPush::GetInstance();
	m_sdk = SdkHandle::GetInstance();

	m_status = ES_UNINIT;

	m_start_time_milli = GetNowTimeMilli();

	//设置"游戏是否启动过"的标记
	bool have_started = Setting::GetInstance()->Get("HaveStarted") == "1";
	if (!have_started)
	{
		Setting::GetInstance()->Set("HaveStarted", "1");
	}
	//设置"是否首次启动"的标记
	Setting::GetInstance()->Set("IsFirstStarted", have_started ? "0" : "1");
	Setting::GetInstance()->Set("ReportEventTrackForLossRateByEngine", "1");

	LogUtil::Init();

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_START_GAME);
}

GameEngine::~GameEngine()
{
	HttpManager::DelInstance();
	FileSystemEx::DelInstance();
	LuaEngine::DelInstance();
	NetAdapter::DelInstance();
	SoundManager::DelInstance();
	VoiceManager::DelInstance();
	Location::DelInstance();
	MsgPush::DelInstance();
	SdkHandle::DelInstance();

	ResManager::DelInstance();
}

GameEngine* GameEngine::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new GameEngine;
	}
	return s_instance;
}

void GameEngine::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool GameEngine::Init()
{
	srand(PITime());
	bool ret = m_http_manager->Init();
	ret = ret && m_filesystem->Init();
	ret = ret && m_lua_engine->Init();
	ret = ret && m_netadapter->Init();
	ret = ret && m_sound_manager->Init();
	ret = ret && m_voice_manager->Init();
	ret = ret && m_location->Init();
	ret = ret && m_msgpush->Init();
	ret = ret && m_sdk->Init();
	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Init Error]");
		return ret;
	}

	ResManager::GetInstance()->Init();

	m_status = ES_INITED;
	return ret;
}

bool GameEngine::Start()
{
	bool ret = m_http_manager->Start();
	ret = ret && m_filesystem->Start();
	ret = ret && m_lua_engine->Start();
	ret = ret && m_netadapter->Start();
	ret = ret && m_sound_manager->Start();
	ret = ret && m_voice_manager->Start();
	ret = ret && m_location->Start();
	ret = ret && m_msgpush->Start();
	ret = ret && m_sdk ->Start();
	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Start Error]");
		return ret;
	}

	//注册自定义Widget
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("Button", (cocos2d::UI_Create_Func)UIButtonEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("CheckBox", (cocos2d::UI_Create_Func)UICheckBoxEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("ImageView", (cocos2d::UI_Create_Func)UIImageViewEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("Text", (cocos2d::UI_Create_Func)UITextEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("TextAtlas", (cocos2d::UI_Create_Func)UITextAtlasEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("LoadingBar", (cocos2d::UI_Create_Func)UILoadingBarEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("Slider", (cocos2d::UI_Create_Func)UISliderEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("TextField", (cocos2d::UI_Create_Func)UITextFieldEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("Layout", (cocos2d::UI_Create_Func)UILayoutEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("ScrollView", (cocos2d::UI_Create_Func)UIScrollViewEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("ListView", (cocos2d::UI_Create_Func)UIListViewEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("PageView", (cocos2d::UI_Create_Func)UIPageViewEx::create);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("Sprite", (cocos2d::UI_Create_Func)SpriteEx::Node);
	cocos2d::UIWidgetFactory::GetInstance()->AddCreateRedirectFunc("ActionTimeline", (cocos2d::UI_Create_Func)ActionTimelineEx::create);

	m_status = ES_STARTED;
	return ret;
}

bool GameEngine::RunPreFrame(float delta_time)
{
	if (m_status != ES_STARTED)
	{
		return true;
	}

	m_http_manager->Update(delta_time);
	m_filesystem->Update(GetNowTimeMilli());
	m_lua_engine->Update(GetNowTime(), GetNowTimeMilli(), delta_time);
	m_netadapter->Update();

	m_sound_manager->Update();
	m_voice_manager->Update(delta_time);

	m_location->Update(delta_time);
	m_msgpush->Update(delta_time);
	m_sdk->Update(delta_time);

	return true;
}

bool GameEngine::Stop()
{
	if (m_status == ES_STOPED || m_status == ES_RELEASED)
	{
		return true;
	}

	bool ret = m_lua_engine->Stop();
	ret = m_http_manager->Stop() && ret;
	ret = m_filesystem->Stop() && ret;
	ret = m_netadapter->Stop() && ret;

	ret = m_sound_manager->Stop() && ret;
	ret = m_voice_manager->Stop() && ret;

	ret = m_location->Stop() && ret;
	ret = m_msgpush->Stop() && ret;
	ret = m_sdk->Stop() && ret;

	ResManager::GetInstance()->Stop();

	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Stop Error]");
	}
	
	m_status = ES_STOPED;

	return ret;
}

bool GameEngine::Release()
{
	if (m_status == ES_RELEASED)
	{
		return true;
	}

	bool ret = m_http_manager->Release();
	ret = m_filesystem->Release() && ret;
	ret = m_lua_engine->Release() && ret;
	ret = m_netadapter->Release() && ret;

	ret = m_sound_manager->Release() && ret;
	ret = m_voice_manager->Release() && ret;
	
	ret = m_location->Release() && ret;
	ret = m_msgpush->Release() && ret;
	ret = m_sdk->Release() && ret;

	ResManager::GetInstance()->Release();

	LogUtil::Release();

	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Release Error]");
	}

	m_status = ES_RELEASED;

	return ret;
}

bool GameEngine::Pause()
{
	bool ret = m_lua_engine->Pause();
	ret = m_sound_manager->Pause() && ret;
	ret = m_voice_manager->Pause() && ret;
	ret = m_netadapter->Pause() && ret;
	ret = m_http_manager->Pause() && ret;
	ret = m_filesystem->Pause() && ret;
	ret = m_location->Pause() && ret;
	ret = m_msgpush->Pause() && ret;
	ret = m_sdk->Pause() && ret;
	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Pause Error]");
		return ret;
	}

	return ret;
}

bool GameEngine::Resume()
{
	bool ret = m_sound_manager->Resume();
	ret = m_http_manager->Resume() && ret;
	ret = m_voice_manager->Resume() && ret;
	ret = m_netadapter->Resume() && ret;
	ret = m_lua_engine->Resume() && ret;
	ret = m_filesystem->Resume() && ret;
	ret = m_location->Resume() && ret;
	ret = m_msgpush->Resume() && ret;
	ret = m_sdk->Resume() && ret;
	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Resume Error]");
		return ret;
	}

	return ret;
}

bool GameEngine::Restart()
{
	bool ret = m_filesystem->Restart();
	ret = ret && m_lua_engine->Restart();
	if (!ret)
	{
		LogUtil::LogError("[GameEngine::Restart Error]");
		CloseApp();
		return false;
	}

	SceneManager::GetInstance()->RunStartUpScene();

	return true;
}

double GameEngine::GetNowTime()
{
	return PITime() * 0.001;
}

double GameEngine::GetNowTimeMilli()
{
	return PITime();
}

void GameEngine::CloseApp()
{
	Stop();
	cocos2d::CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	cocos2d::CCDirector::sharedDirector()->mainLoop();
	exit(0);
#endif
}

//切换语言版本并重登
void GameEngine::SwitchLanguageAndRestart(const std::string& district_language)
{
	Setting::GetInstance()->Set("DistrictLanguage", district_language);

	Restart();
}
