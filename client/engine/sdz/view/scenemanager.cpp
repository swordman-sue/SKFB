#include "gameengine/gameengine.h"
#include "scenemanager.h"
#include "gamescene.h"
#include "struct.h"
#include "zqcommon/gamedef.h"
#include "zqcommon/logutil.h"
#include "zqcommon/strutil.h"
#include "luaengine/luaengine.h"
#include "view/startup/startup.h"
#include "resmanager/resmanager.h"
#include "platform/systemtools.h"
#include "filesystem/filesystemex.h"
#include "filesystem/setting.h"

//#include "keypad_dispatcher/CCKeypadDispatcher.h"
//#include "touch_dispatcher/CCTouchDispatcher.h"

DefaultKeyDelegate::DefaultKeyDelegate()
: m_is_enabled(false)
, m_listener(nullptr)
, m_lua_back_fun_id(0)
, m_lua_menu_fun_id(0)
, m_lua_arrow_fun_id(0)
, m_lua_enter_fun_id(0)
{
	SetKeypadEnabled(true);
}

DefaultKeyDelegate::~DefaultKeyDelegate()
{
	SetKeypadEnabled(false);
	// 	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_back_fun_id);
	// 	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_menu_fun_id);
}

void DefaultKeyDelegate::SetKeypadEnabled(bool is_enabled)
{
	if (is_enabled == m_is_enabled)
	{
		return;
	}
	m_is_enabled = is_enabled;

	if (nullptr != m_listener)
	{
		cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(m_listener);
		m_listener = nullptr;
	}

	if (m_is_enabled)
	{
		m_listener = cocos2d::EventListenerKeyboard::create();
		m_listener->retain();
		m_listener->onKeyPressed = CC_CALLBACK_2(DefaultKeyDelegate::onKeyPressed, this);
		m_listener->onKeyReleased = CC_CALLBACK_2(DefaultKeyDelegate::onKeyReleased, this);

		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_listener, -1025);
	}
}

void DefaultKeyDelegate::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event)
{
}

void DefaultKeyDelegate::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* unused_event)
{
	if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_BACK)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_back_fun_id, "");
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_MENU)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_menu_fun_id, "");
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_arrow_fun_id, "i", keyCode);
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_arrow_fun_id, "i", keyCode);
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_arrow_fun_id, "i", keyCode);
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_arrow_fun_id, "i", keyCode);
	}
	else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_ENTER)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_enter_fun_id, "");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////

DefaultTouchDelegate::DefaultTouchDelegate()
:m_is_enabled(false),
m_lua_down_fun_id(0),
m_lua_move_fun_id(0),
m_lua_up_fun_id(0),
m_lua_cancel_fun_id(0),
m_swallow(false), 
m_listener(nullptr)
{
	SetTouchEnable(true);
}

DefaultTouchDelegate::~DefaultTouchDelegate()
{
	SetTouchEnable(false);
	// 	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_down_fun_id);
	// 	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_move_fun_id);
	// 	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_up_fun_id);
	// 	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_cancel_fun_id);
}

bool DefaultTouchDelegate::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	cocos2d::Point pt = pTouch->getLocation();

	int ret = 0;
	LuaEngine::GetInstance()->CallFunction(m_lua_down_fun_id, "dd>i", pt.x, pt.y, &ret);
	
	return (ret != 0);
}

void DefaultTouchDelegate::onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	LuaEngine::GetInstance()->CallFunction(m_lua_move_fun_id, "");
}

void DefaultTouchDelegate::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	LuaEngine::GetInstance()->CallFunction(m_lua_up_fun_id, "");
}

void DefaultTouchDelegate::onTouchCancelled(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	LuaEngine::GetInstance()->CallFunction(m_lua_cancel_fun_id, "");
}

void DefaultTouchDelegate::SetTouchEnable(bool enable)
{
	if (enable == m_is_enabled)
	{
		return;
	}

	m_is_enabled = enable;

	if (nullptr != m_listener)
	{
		cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(m_listener);
		m_listener = nullptr;
	}

	if (m_is_enabled)
	{
		m_listener = cocos2d::EventListenerTouchOneByOne::create();
		m_listener->retain();
		m_listener->setSwallowTouches(m_swallow);
		m_listener->onTouchBegan = CC_CALLBACK_2(DefaultTouchDelegate::onTouchBegan, this);
		m_listener->onTouchMoved = CC_CALLBACK_2(DefaultTouchDelegate::onTouchMoved, this);
		m_listener->onTouchEnded = CC_CALLBACK_2(DefaultTouchDelegate::onTouchEnded, this);
		m_listener->onTouchCancelled = CC_CALLBACK_2(DefaultTouchDelegate::onTouchCancelled, this);

		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_listener, -1024);
	}
}

void DefaultTouchDelegate::SetSwallow(bool is_swallow)
{
	if(m_swallow == is_swallow)
		return;

	m_swallow = is_swallow;

	bool is_enable = m_is_enabled;
	SetTouchEnable(false);
	SetTouchEnable(is_enable);
}

SceneManager::SceneManager()
{
	m_director = cocos2d::Director::getInstance();
	m_director->setAnimationInterval(1.0 / 60);

	if (LuaEngine::GetInstance()->GetConfig("IsDisplayStats") == "true")
	{
		m_director->setDisplayStats(true);
	}

	// 初始化GLView
	auto glview = m_director->getOpenGLView();
	if (!glview) {
		glview = cocos2d::GLViewImpl::create("skfb");

#if defined(_WIN32)
		// 初始化窗口尺寸
		int width = 0, height = 0;
		ZQ::StrUtil::StringToBaseType(Setting::GetInstance()->Get("WindowWidth"), width);
		ZQ::StrUtil::StringToBaseType(Setting::GetInstance()->Get("WindowHeight"), height);
		!width ? width = GameType::SCREEN_WIDTH_DEF : 0;
		!height ? height = GameType::SCREEN_HEIGHT_DEF : 0;
		glview->setFrameSize(width, height);
#endif
	}
	m_director->setOpenGLView(glview);

	// 设置设计分辨率与适配方案
	// 横屏适配方案：FIXED_HEIGHT，竖屏适配方案：FIXED_WIDTH
	glview->setDesignResolutionSize(GameType::SCREEN_DESIGN_WIDTH_DEF, GameType::SCREEN_DESIGN_HEIGHT_DEF, ResolutionPolicy::FIXED_HEIGHT);

	// 获取屏幕尺寸与设计分辨率尺寸
	m_screen_size = glview->getFrameSize();
	m_design_size = glview->getDesignResolutionSize();
	
	// 监听触摸
	m_touch_delegate.SetTouchEnable(true);
	m_touch_delegate.SetSwallow(true);
}

SceneManager::~SceneManager()
{
}

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::PopScene()
{
	m_director->popScene();
}

void SceneManager::PushScene(GameScene* scene)
{
	m_director->pushScene(scene);
}

void SceneManager::RunWithScene(GameScene* scene)
{
	cocos2d::CCScene* running_scene = m_director->getRunningScene();
	if (nullptr != running_scene)
	{
		m_director->replaceScene(scene);
	}
	else
	{
		m_director->runWithScene(scene);
	}
}

void SceneManager::ReplaceScene(GameScene *scene)
{
	m_director->replaceScene(scene);
}

unsigned int SceneManager::GetResolutionPolicy()
{
	auto pGLView = m_director->getOpenGLView();
	return (unsigned int)pGLView->getResolutionPolicy();
}

void SceneManager::GetDesignSize(COCOSize* size)
{
	size->width = m_design_size.width;
	size->height = m_design_size.height;
}

void SceneManager::GetScreenSize(COCOSize* size)
{
	size->width = m_screen_size.width;
	size->height = m_screen_size.height;
}

void SceneManager::GetVisibleRect(cocos2d::Rect* rect)
{
	Rect safeAreaRect = m_director->getVisibleRect();
	(*rect) = safeAreaRect;
}

void SceneManager::GetSafeAreaRect(Rect* rect)
{
    Rect safeAreaRect = m_director->getSafeAreaRect();
    (*rect) = safeAreaRect;
}

GameScene* SceneManager::GetRunningScene()
{
	return (GameScene*)m_director->getRunningScene();
}

#ifdef _WIN32
std::string Utf8ToGBK(const char* strUtf8)
{
	//不是utf8编码，返回原串
	unsigned char code_mark = strUtf8[0];
	if(code_mark <= 0xE0)
	{
		return strUtf8;
	}

	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8, -1, NULL,0); 
	unsigned short * wszGBK = new unsigned short[len+1];       
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8, -1, (LPWSTR)wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK=new char[len + 1]; 
	memset(szGBK, 0, len + 1); 
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL);
	std::string gbkString = szGBK;
	delete[] wszGBK;
	delete[] szGBK;
	return gbkString;
}
#endif //

void SceneManager::ShowMessage(const std::string &msg, const std::string &title, ZQ::UINT32 fun_id)
{
#ifdef _WIN32
	std::string tmp_msg = Utf8ToGBK(msg.c_str());
	std::string tmp_title = Utf8ToGBK(title.c_str());
	cocos2d::MessageBox(tmp_msg.c_str(), tmp_title.c_str());
#else
	cocos2d::MessageBox(msg.c_str(), title.c_str());
#endif
	LuaEngine::GetInstance()->CallFunction(fun_id, "");
}

void SceneManager::SetEnableTouchEvent(bool enable)
{
	m_touch_delegate.SetTouchEnable(enable);
}

void SceneManager::SetLuaBackKey(ZQ::UINT32 back_fun_id)
{
	m_key_delegate.SetLuaBackKey(back_fun_id);
}

void SceneManager::SetLuaArrowKey(ZQ::UINT32 arrow_fun_id)
{
	m_key_delegate.keyArrowClicked(arrow_fun_id);
}

void SceneManager::SetLuaEnterKey(ZQ::UINT32 enter_fun_id)
{
	m_key_delegate.keyEnterClicked(enter_fun_id);
}

void SceneManager::SetLuaMenuKey(ZQ::UINT32 menu_fun_id)
{
	m_key_delegate.SetLuaMenuKey(menu_fun_id);
}

//设置是否响应键盘消息
void SceneManager::SetKeypadEnabled(bool bEnabled)
{
	m_key_delegate.SetKeypadEnabled(bEnabled);
}

//设置游戏帧数
void SceneManager::SetGameFrame(float frame)
{
	if(frame == 0)
	{
		return;
	}

	m_director->setAnimationInterval(1.0 / frame);
}

//获取游戏帧数
double SceneManager::GetGameFrame()
{
	return (1.0 / m_director->getAnimationInterval());
}

//运行开始界面
void SceneManager::RunStartUpScene()
{
	GameScene *startup_scene = StartUpScene::Node();
	RunWithScene(startup_scene);
}

void SceneManager::SetLuaTouchDown(ZQ::UINT32 down_fun_id)
{
	m_touch_delegate.SetLuaTouchDown(down_fun_id);
}

void SceneManager::SetLuaTouchMove(ZQ::UINT32 move_fun_id)
{
	m_touch_delegate.SetLuaTouchMove(move_fun_id);
}

void SceneManager::SetLuaTouchUp(ZQ::UINT32 up_fun_id)
{
	m_touch_delegate.SetLuaTouchUp(up_fun_id);
}

void SceneManager::SetLuaTouchCancel(ZQ::UINT32 cancel_fun_id)
{
	m_touch_delegate.SetLuaTouchCancel(cancel_fun_id);
}

void SceneManager::SetTouchSwallow(bool is_swallow)
{
	m_touch_delegate.SetSwallow(is_swallow);
}

void SceneManager::Pause()
{
	m_director->pause();
}

void SceneManager::Resume()
{
	m_director->resume();
}

void SceneManager::SetDeltaSpeedTime(float speed_time)
{
	m_director->SetDeltaSpeedTime(speed_time);
}

float SceneManager::GetDeltaSpeedTime()
{
	return m_director->GetDeltaSpeedTime();
}
