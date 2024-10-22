#include "uitextfieldex.h"
#include "luaengine/luaengine.h"
#include "view/font/stringconv.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UITextFieldEx)

UITextFieldEx::UITextFieldEx()
:m_attach_event_func_id(0), 
m_detach_event_func_id(0), 
m_insert_text_event_func_id(0),
m_delete_backward_event_func_id(0),
m_text_changed_id(0),
m_filter_str(" \n\r\n"),
m_only_view_touch_area(false),
m_cursor_sprite(nullptr),
is_filter(false)
{
}

UITextFieldEx::~UITextFieldEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_attach_event_func_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_detach_event_func_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_insert_text_event_func_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_delete_backward_event_func_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_text_changed_id);
}

void UITextFieldEx::onEnter()
{
	TextField::onEnter();
	addEventListener(CC_CALLBACK_2(UITextFieldEx::OnTouchEvent, this));
	Widget::addTouchEventListener(CC_CALLBACK_2(UITextFieldEx::OnTouchEventEx, this));
}

void UITextFieldEx::OnTouchEvent(Ref *target, ui::TextField::EventType type)
{
	switch (type)
	{
	case ui::TextField::EventType::ATTACH_WITH_IME:
		if (m_cursor_sprite)
		{
			m_cursor_sprite->setVisible(true);
		}
		this->ResetCursorPosition();
		LuaEngine::GetInstance()->CallFunction(m_attach_event_func_id, "");
		break;

	case ui::TextField::EventType::DETACH_WITH_IME:
		if (m_cursor_sprite)
		{
			m_cursor_sprite->setVisible(false);
		}
		this->ResetCursorPosition();
		LuaEngine::GetInstance()->CallFunction(m_detach_event_func_id, "");
		break;

	case ui::TextField::EventType::INSERT_TEXT:
		is_filter = false;
		this->ResetCursorPosition();
		LuaEngine::GetInstance()->CallFunction(m_insert_text_event_func_id, "");
		break;

	case ui::TextField::EventType::DELETE_BACKWARD:
		this->ResetCursorPosition();
		LuaEngine::GetInstance()->CallFunction(m_delete_backward_event_func_id, "");
		break;
	default:
		break;
	}
}

void UITextFieldEx::OnTouchEventEx(cocos2d::Ref* target, Widget::TouchEventType event_type)
{
	switch (event_type)
	{
	case Widget::TouchEventType::ENDED:
		{
			LuaEngine::GetInstance()->CallFunction("PlaySystemOperEffect", "ii", _nTouchSoundIndex, _nTouchSoundSubIndex);
		}
		break;

	default:
		break;
	}
}

bool UITextFieldEx::onTextFieldInsertText(TextFieldTTF *pSender, const char *text, int nLen)
{
	if (nLen == 1 && strcmp(text, "\n") == 0)
	{
		didNotSelectSelf();
		return false;
	}
	if (is_filter)
	{
		return false;
	}
	std::string filter_text;
	std::string now_text = GetText();

	GameType::UINT32 unicode_len = StringConvHelper::unicode_length(text);
	GameType::UINT32 index = 0;
	while (index < unicode_len)
	{
		std::string index_str = StringConvHelper::unicode_at(text, index);
		if (now_text.length() + filter_text.length() + index_str.length() > getMaxLength())
		{
			break;
		}
		filter_text = filter_text + index_str;
		++index;
	}

	if (filter_text.length() == 0)
	{
		return true;
	}
	//过滤字符
	std::string::iterator cur_iter = filter_text.begin();
	while (cur_iter != filter_text.end())
	{
		if (m_filter_str.find(*cur_iter) != std::string::npos)
		{
			cur_iter = filter_text.erase(cur_iter);
		}
		else
		{
			++cur_iter;
		}
	}
	is_filter = true;
	now_text.append(filter_text);
	TextField::setText(now_text);
	LuaEngine::GetInstance()->CallFunction(m_text_changed_id, "s", filter_text.c_str());
	return true;
}

//创建光标精灵
void UITextFieldEx::CreateCursorSprite()
{
	if (m_cursor_sprite)
	{
		this->removeChild(m_cursor_sprite, true);
	}

	const GameType::UINT32 cursor_width = 2;
	GameType::UINT32 cursor_height = (GameType::UINT32)(TextField::getFontSize() * CC_CONTENT_SCALE_FACTOR());
	int total = cursor_width * cursor_height;
	unsigned int* data = new unsigned int[total];
	for (int i = 0; i < total; ++i)
	{
		data[i] = 0xfffffff0;
	}

	Texture2D *texture = new Texture2D();
	texture->initWithData(data, total, Texture2D::PixelFormat::RGB888, cursor_width, cursor_height, cocos2d::Size(cursor_width, cursor_height));
	texture->autorelease();

	delete[] data;

	m_cursor_sprite = Sprite::createWithTexture(texture);
	m_cursor_sprite->setPosition(0, 0);
	//m_cursor_sprite->setPosition(0, Widget::getContentSize().height / 2);
	m_cursor_sprite->setAnchorPoint(Vec2(0.0, 0.0));
	this->addChild(m_cursor_sprite);
	m_cursor_sprite->setVisible(false);

	RepeatForever *cursor_action = RepeatForever::create((ActionInterval *)Sequence::create(FadeOut::create(0.5f), FadeIn::create(0.5f), NULL));
	m_cursor_sprite->runAction(cursor_action);
}

//重置光标精灵的位置
void UITextFieldEx::ResetCursorPosition()
{
	float pos_x;

	if (std::string(GetText()).length() > 0)
	{
		pos_x = getContentSize().width;
	}
	else
	{
		pos_x = 0;
		Widget::setContentSize(cocos2d::Size(getContentSize().width, getFontSize()));
	}
	if (m_cursor_sprite)
	{
		m_cursor_sprite->setPosition(pos_x, m_cursor_sprite->getPosition().y);
	}

	ResetVisibleSection();
}

//重设可视区域
void UITextFieldEx::ResetVisibleSection()
{
	if (!m_only_view_touch_area)
	{
		return;
	}
	float width = getContentSize().width;
	float offset_x = 0.0f, offset_y = 0.0f;
	//重置位置
	if (width > _touchWidth)
	{
		offset_x = (m_origin_pos.x - (width - _touchWidth)) * (1 - getAnchorPoint().x);
	}
	else
	{
		offset_x = m_origin_pos.x * (1 - getAnchorPoint().x);
	}
	Node::setPosition(offset_x, m_origin_pos.y);
}

UITextFieldEx* UITextFieldEx::create()
{
	UITextFieldEx* widget = new UITextFieldEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

UITextFieldEx* UITextFieldEx::CreateWithString(const std::string &place_holder, const std::string &font_name, int font_size)
{
	UITextFieldEx* widget = create();
	if (!widget)
		return nullptr;

	widget->SetFontName(font_name);
	widget->SetFontSize(font_size);
	widget->SetPlaceHolderText(place_holder);
	return widget;
}

std::string UITextFieldEx::GetText()
{
	return TextField::getStringValue();
}

void UITextFieldEx::SetText(const std::string& text)
{
	TextField::setText(text);
	this->ResetCursorPosition();
}

void UITextFieldEx::SetFilterStr(const std::string &filter_str)
{
	m_filter_str = filter_str;
}

void UITextFieldEx::SetTouchSize(float width, float height)
{
	this->setTouchSize(cocos2d::Size(width, height));
}

void UITextFieldEx::SetOnlyVisibleTouchArea(bool visible)
{
	m_only_view_touch_area = visible;
}

void UITextFieldEx::SetTextAreaSize(float width, float height)
{
	TextField::setTextAreaSize(cocos2d::Size(width, height));
}

void UITextFieldEx::SetFontName(const std::string &font_name)
{
	TextField::setFontName(font_name);
}

void UITextFieldEx::SetFontSize(int font_size)
{
	TextField::setFontSize(font_size);
	this->CreateCursorSprite();
}

void UITextFieldEx::SetTextColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue)
{
	TextField::setColor(ccc3(red, green, blue));
}

void UITextFieldEx::SetTextHorizontalAlign(int align)
{
	TextField::setTextHorizontalAlignment((cocos2d::TextHAlignment)align);
}

void UITextFieldEx::SetTextVertiacalAlign(int align)
{
	TextField::setTextVerticalAlignment((cocos2d::TextVAlignment)align);
}

void UITextFieldEx::SetEventListener(GameType::UINT32 attach_event_id, GameType::UINT32 detach_event_id, GameType::UINT32 insert_event_id, GameType::UINT32 delete_event_id)
{
	m_attach_event_func_id = attach_event_id;
	m_detach_event_func_id = delete_event_id;
	m_insert_text_event_func_id = insert_event_id;
	m_delete_backward_event_func_id = delete_event_id;
}

//设置输入框提示文字
void UITextFieldEx::SetPlaceHolderText(const std::string &text)
{
	TextField::setPlaceHolder(text);
}

//设置输入框提示文字的颜色
void UITextFieldEx::SetPlaceHolderColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue)
{
	((UICCTextField*)TextField::getVirtualRenderer())->setColorSpaceHolder(ccc3(red, green, blue));
}

//设置最大输入文字
void UITextFieldEx::SetMaxLength(GameType::INT32 max_length)
{
	TextField::setMaxLength(max_length);
}

//设置密码模式
void UITextFieldEx::SetPasswordMode(bool is_password)
{
	TextField::setPasswordEnabled(is_password);
}

cocos2d::ui::Widget* UITextFieldEx::createCloneInstance()
{
	return UITextFieldEx::create(); 
}
