#include "uilayoutex.h"
#include "luaengine/luaengine.h"
#include "view/viewhelper.h"
#include "view/struct.h"

using namespace cocos2d;
using namespace cocos2d::ui;

IMPLEMENT_CLASS_GUI_INFO(UILayoutEx)

UILayoutEx::UILayoutEx()
:m_down_fun_id(0)
,m_move_fun_id(0)
,m_up_fun_id(0)
,m_cancle_fun_id(0)
,m_update_fun_id(0)
{}

UILayoutEx::~UILayoutEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_down_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_move_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_up_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_cancle_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_update_fun_id);
}

UILayoutEx* UILayoutEx::create()
{
	UILayoutEx* widget = new UILayoutEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

void UILayoutEx::onExit()
{
	unschedule(schedule_selector(UILayoutEx::Update));
	Layout::onExit();
}

void UILayoutEx::SetBackGroundColor(GameType::UINT32 red, GameType::UINT32 green, GameType::UINT32 blue)
{
	Layout::setBackGroundColor(cocos2d::Color3B(red, green, blue));
}

void UILayoutEx::SetBackGroundImage(const char* fileName, int texType)
{
	std::string full_path = ViewHelper::GetResoursePath(fileName);
	if (TextureResType::LOCAL == (TextureResType)texType && !ViewHelper::LoadImageResource(full_path))
		return;

	Layout::setBackGroundImage(full_path.c_str(), (Widget::TextureResType)texType);
}

void UILayoutEx::SetBackGroundImage(const ImageData& image_data)
{
	if (image_data.clip_image_name.c_str() == "")
		return;

	Layout::setBackGroundImage(image_data.clip_image_name.c_str(), Widget::TextureResType::PLIST);
}

void UILayoutEx::SetEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id)
{
	m_down_fun_id = down_fun_id;
	m_move_fun_id = move_fun_id;
	m_up_fun_id = up_fun_id;
	Widget::addTouchEventListener(CC_CALLBACK_2(UILayoutEx::OnTouchEvent, this));
}

void UILayoutEx::SetEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancle_fun_id)
{
	m_down_fun_id = down_fun_id;
	m_move_fun_id = move_fun_id;
	m_up_fun_id = up_fun_id;
	m_cancle_fun_id = cancle_fun_id;
	Widget::addTouchEventListener(CC_CALLBACK_2(UILayoutEx::OnTouchEvent, this));
}

void UILayoutEx::OnTouchEvent(cocos2d::Ref* target, Widget::TouchEventType event_type)
{
	switch (event_type)
	{
	case Widget::TouchEventType::BEGAN:
		{
			cocos2d::Point point = Layout::getTouchStartPos();
			LuaEngine::GetInstance()->CallFunction(m_down_fun_id, "dd" ,point.x, point.y);
		}
		break;
	case Widget::TouchEventType::MOVED:
		{
			cocos2d::Point point = Layout::getTouchMovePos();
			LuaEngine::GetInstance()->CallFunction(m_move_fun_id, "dd", point.x, point.y);
		} 
		break;
	case Widget::TouchEventType::ENDED:
		{
			cocos2d::Point point = Layout::getTouchEndPos();
			LuaEngine::GetInstance()->CallFunction(m_up_fun_id, "dd", point.x, point.y);
			LuaEngine::GetInstance()->CallFunction("PlaySystemOperEffect", "ii", _nTouchSoundIndex, _nTouchSoundSubIndex);
		}
		break;
	case Widget::TouchEventType::CANCELED:
		{
			cocos2d::Vec2 point = Widget::getTouchMovePosition();
			LuaEngine::GetInstance()->CallFunction(m_cancle_fun_id, "dd", point.x, point.y);
		}
		break;
	default:
		break;
	}
}

void UILayoutEx::SetClippingSize(float width, float height)
{
	Layout::setStencilClippingSize(cocos2d::Size(width, height));
}

void UILayoutEx::SetClippingType(int clipping_type)
{
	Layout::setClippingType((ClippingType)clipping_type);
}

void UILayoutEx::SetClippingEnable(bool enable)
{
	Layout::setClippingEnabled(enable);
}

cocos2d::ui::Widget* UILayoutEx::createCloneInstance()
{
	return UILayoutEx::create();
}

void UILayoutEx::SetBackGroundColorType(int type)
{
	Layout::setBackGroundColorType((BackGroundColorType)type);
}

void UILayoutEx::SetLayoutType(int layout_type)
{
	Layout::setLayoutType((cocos2d::ui::Layout::Type)layout_type);
}

void UILayoutEx::SetSizeType(int size_type)
{
	Layout::setSizeType((cocos2d::ui::Layout::SizeType)size_type);
}

void UILayoutEx::SetSizePercent(const float percent_w, const float percent_h)
{
	Layout::setSizePercent(percent_w, percent_h);
}

void UILayoutEx::SetBackGroundImageCapInsets(float x, float y, float w, float h)
{
	Layout::setBackGroundImageCapInsets(cocos2d::CCRectMake(x, y, w, h));
}

void UILayoutEx::SetLuaUpdate(GameType::UINT32 update_fun_id, float interval)
{
	if (0 != m_update_fun_id)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_update_fun_id);
	}
	m_update_fun_id = update_fun_id;

	SEL_SCHEDULE selector = schedule_selector(UILayoutEx::Update);
	if (0 != m_update_fun_id)
	{
		if (isScheduled(selector))
		{
			unschedule(selector);
		}
		schedule(selector, interval);
	}
	else
	{
		unschedule(selector);
	}
}

void UILayoutEx::Update(float dt)
{
	LuaEngine::GetInstance()->CallFunction(m_update_fun_id, "d", dt);
}
