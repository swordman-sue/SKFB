#include "uitextex.h"
#include "luaengine/luaengine.h"
#include "platform/systemtools.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UITextEx)

UITextEx::UITextEx() :
m_down_fun_id(0),
m_move_fun_id(0),
m_up_fun_id(0),
m_cancel_fun_id(0)
{}

UITextEx::~UITextEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_down_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_move_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_up_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_cancel_fun_id);
}

UITextEx* UITextEx::create()
{
	UITextEx* widget = new UITextEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UITextEx* UITextEx::CreateWithString(const std::string &text, const std::string &font_name, int font_size)
{
	UITextEx* widget = new UITextEx();
	if (widget && widget->InitWithString(text, font_name, font_size))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

bool UITextEx::InitWithString(const std::string &text, const std::string &font_name, int font_size)
{
	if(!ui::Text::init())
		return false;

	SetText(text);
	ui::Text::setFontName(font_name);
	ui::Text::setFontSize(font_size);
	return true;
}

std::string UITextEx::GetText()
{
	return ui::Text::getStringValue();
}

void UITextEx::SetText(const std::string& text)
{
	ui::Text::setText(SystemTools::ConvertUygurToEx(text));
}

Widget* UITextEx::createCloneInstance()
{
	return UITextEx::create();
}

void UITextEx::SetFontName(const std::string &font_name)
{
	ui::Text::setFontName(font_name);
}

void UITextEx::SetFontSize(int font_size)
{
	ui::Text::setFontSize(font_size);
}

void UITextEx::SetTextColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue)
{
	ui::Text::setColor(cocos2d::Color3B(red, green, blue));
}

void UITextEx::SetStroke(bool stroke)
{
	m_stroke = stroke;
}

void UITextEx::SetStrokeColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue, GameType::UINT8 aplha)
{
	cocos2d::Color4B color = cocos2d::Color4B(red, green, blue, aplha);
	enableOutline(color, 2.0);
}

void UITextEx::SetShadow(bool shadow)
{
	m_shadow = shadow;
}

void UITextEx::SetShadowColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue, GameType::UINT8 aplha)
{
	cocos2d::Color4B color = cocos2d::Color4B(red, green, blue, aplha);
	enableShadow(color);
}

void UITextEx::SetTextHorizontalAlign(int align)
{
	ui::Text::setTextHorizontalAlignment((cocos2d::TextHAlignment)align);
}

void UITextEx::SetTextVertiacalAlign(int align)
{
	ui::Text::setTextVerticalAlignment((cocos2d::TextVAlignment)align);
}

void UITextEx::SetTextAreaSize(float width, float height)
{
	ui::Text::setTextAreaSize(cocos2d::Size(width, height));
}

int UITextEx::GetLength()
{
	return getStringLength();
}

void UITextEx::DisableEffect()
{
	disableEffect();
}

void UITextEx::AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancel_fun_id)
{
	this->m_down_fun_id = down_fun_id;
	this->m_move_fun_id = move_fun_id;
	this->m_up_fun_id = up_fun_id;
	this->m_cancel_fun_id = cancel_fun_id;
	ui::Text::addTouchEventListener(CC_CALLBACK_2(UITextEx::addTouchEventListener, this));
}

void UITextEx::addTouchEventListener(Ref* target, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		LuaEngine::GetInstance()->CallFunction(m_down_fun_id, "");
		break;
	case Widget::TouchEventType::MOVED:
	{
		cocos2d::Vec2 point = Widget::getTouchMovePosition();
		LuaEngine::GetInstance()->CallFunction(m_move_fun_id, "dd", point.x, point.y);
	}
		break;
	case Widget::TouchEventType::ENDED:
		LuaEngine::GetInstance()->CallFunction(m_up_fun_id, "");
		break;
	case Widget::TouchEventType::CANCELED:
	{
		cocos2d::Vec2 point = Widget::getTouchMovePosition();
		LuaEngine::GetInstance()->CallFunction(m_cancel_fun_id, "dd", point.x, point.y);
	}
		break;
	default:
		break;
	}
}