#include "luaerrorreport.h"

#include <stdio.h>

#include "platform/CCPlatformMacros.h"
#include "math/Vec2.h"
#include "math/CCGeometry.h"
#include "2d/CCLabel.h"

#include "md5/md5.h"
#include "zqcommon/gamedef.h"
#include "zqcommon/strutil.h"
#include "zqcommon/gamedef.h"
#include "zqcommon/logutil.h"
#include "zqcommon/strutil.h"
#include "luaengine/luaengine.h"
#include "view/uiwidget/uitextex.h"
#include "view/uiwidget/uilayoutex.h"
#include "view/uiwidget/uiscrollviewex.h"
#include "view/uiwidget/uibuttonex.h"
#include "view/gamescene.h"
#include "view/scenemanager.h"
#include "view/startup/startup.h"

USING_NS_CC;
using namespace cocos2d::ui;


static LuaErrorReport* s_instance = nullptr;

LuaErrorReport* LuaErrorReport::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new LuaErrorReport;
	}
	return s_instance;
}

void LuaErrorReport::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

LuaErrorReport::LuaErrorReport(void)
: m_all_error_str("\n")
, m_pre_error_key("")
, m_error_elapse_time(0)
, m_view_layout(nullptr)
, m_error_txt(nullptr)
, m_error_txt_scv(nullptr)
{
}

LuaErrorReport::~LuaErrorReport()
{
	int i = 0;
}

bool LuaErrorReport::Stop()
{
	CloseView();
	return true;
}

void LuaErrorReport::Update(float elapse_time)
{
	if (nullptr != m_view_layout)
	{
		m_error_elapse_time += elapse_time;
		if (m_error_elapse_time >= 0.5)
		{
			m_error_elapse_time = 0;

			const Size& inner_size = m_error_txt_scv->getInnerContainerSize();
			float txt_height = m_error_txt->getContentHeight();
			if (txt_height > inner_size.height)
			{
				m_error_txt_scv->SetInnerContainerSize(inner_size.width, txt_height);
			}
		}
	}
}

void LuaErrorReport::Check(const std::string& error)
{	
	std::string key = MD5String(error.c_str());

	const std::string& lua_error_view_toggle = LuaEngine::GetInstance()->GetConfig("LuaErrorViewToggle");
	if (lua_error_view_toggle == "true")
	{
		// 显示报错信息界面
		Show(key, error);
	}
	else
	{
		if (LuaEngine::GetInstance()->IsLuaMainStart())
		{
			// 上传报错信息到服务端
			LuaEngine::GetInstance()->CallFunction("CacheLuaError", "ss", key.c_str(), error.c_str());
		}
		else
		{
			// 显示报错信息界面
			Show(key, error);
		}
	}
}

void LuaErrorReport::Show(const std::string& key, const std::string& error)
{
	if (key == m_pre_error_key)
	{
		return;
	}
	m_pre_error_key = key;

	// 输出报错信息到控制台
	std::string error_ex = ZQ::StrUtil::ReplaceAll(error, "%", "%%");
	LogUtil::LogError(error_ex.c_str());

	m_all_error_str += "[LUA ERROR]" + error;

	if (OpenView())
	{
		m_error_txt->SetText(m_all_error_str);
	}
	else
	{
		LogUtil::LogError(m_all_error_str.c_str());
		m_pre_error_key = "";
	}
}

bool LuaErrorReport::OpenView()
{
	if (nullptr == m_view_layout)
	{
		GameScene* scene = SceneManager::GetInstance()->GetRunningScene();
		if (nullptr != dynamic_cast<StartUpScene*>(scene))
		{
			return false;
		}

		COCOSize design_size;
		SceneManager::GetInstance()->GetDesignSize(&design_size);

		m_view_layout = UILayoutEx::create();
		m_view_layout->SetContentSize(design_size.width, design_size.height);
		m_view_layout->SetAnchorPoint(0.5, 0.5);
		m_view_layout->SetPosition(design_size.width / 2, design_size.height / 2);
		m_view_layout->setTouchEnabled(true);
		m_view_layout->setSwallowTouches(true);
		m_view_layout->retain();
		scene->addChild(m_view_layout, GameType::RenderQueue::GRQ_MAX);

		float layout_w = 853, layout_h = 640;
		UILayoutEx* layout = UILayoutEx::create();
		layout->SetContentSize(layout_w, layout_h);
		layout->SetAnchorPoint(0.5, 0.5);
		layout->SetBackGroundImage("resource/start/tipbk.png", (int)(Widget::TextureResType::LOCAL));
		layout->setBackGroundImageScale9Enabled(true);
		layout->SetPosition(design_size.width / 2, design_size.height / 2);
		layout->setPositionType(Widget::PositionType::PERCENT);
		layout->setPositionPercent(Vec2(0.5, 0.5));
		m_view_layout->addChild(layout);

		UIButtonEx* btn = UIButtonEx::CreateWithImage("resource/start/small_btn.png");
		btn->SetText("OK");
		btn->SetFontSize(24);
		btn->SetFontName(Label::DEF_FONT_NAME);
		btn->SetAnchorPoint(0.5, 0);
		btn->SetScale9Enabled(true);
		btn->SetContentSize(116, 40);
		btn->SetPosition(layout_w / 2, 8);
		btn->addTouchEventListener(CC_CALLBACK_2(LuaErrorReport::OnTouchBtn, this));
		layout->addChild(btn);

		m_error_txt_scv = UIScrollViewEx::create();
		m_error_txt_scv->SetContentSize(layout_w - 20, layout_h - 40);
		m_error_txt_scv->SetAnchorPoint(0.5, 1);
		m_error_txt_scv->setPositionType(Widget::PositionType::PERCENT);
		m_error_txt_scv->setPositionPercent(Vec2(0.5, 1));
		layout->addChild(m_error_txt_scv);

		m_error_txt = UITextEx::CreateWithString("", Label::DEF_FONT_NAME, 24);
		m_error_txt->setAnchorPoint(Vec2(0.5, 1));
		m_error_txt->SetTextAreaSize(layout_w - 20, 0);
		m_error_txt->setPositionType(Widget::PositionType::PERCENT);
		m_error_txt->setPositionPercent(Vec2(0.5, 1));
		m_error_txt_scv->addChild(m_error_txt);
	}

	return true;
}

void LuaErrorReport::CloseView()
{
	m_pre_error_key = "";
	m_all_error_str = "\n";
	m_error_elapse_time = 0;

	if (nullptr != m_view_layout)
	{
		m_view_layout->removeFromParent();
		m_view_layout->release();
		m_view_layout = nullptr;
		m_error_txt = nullptr;
		m_error_txt_scv = nullptr;
	}
}

void LuaErrorReport::OnTouchBtn(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		CloseView();
		break;
	
	default:
		break;
	}
}
