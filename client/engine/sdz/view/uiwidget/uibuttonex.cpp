#include "uibuttonex.h"
#include "luaengine/luaengine.h"
#include "view/struct.h"
#include "view/viewhelper.h"
#include "platform/systemtools.h"

using namespace cocos2d;
using namespace cocos2d::ui;

IMPLEMENT_CLASS_GUI_INFO(UIButtonEx);

UIButtonEx::UIButtonEx():
m_down_fun_id(0), 
m_move_fun_id(0), 
m_up_fun_id(0), 
m_cancle_fun_id(0),
m_is_enabled(true)
{
}

UIButtonEx::~UIButtonEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_down_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_move_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_up_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_cancle_fun_id);
}

UIButtonEx* UIButtonEx::create()
{
	UIButtonEx* widget = new UIButtonEx();
	if (widget && widget->init())
	{
		widget->initRenderer();
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UIButtonEx* UIButtonEx::CreateWithImage(const std::string& image_file, int texType)
{
	UIButtonEx* widget = new UIButtonEx();
	if (widget && widget->InitWithImage(image_file, texType))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UIButtonEx* UIButtonEx::CreateWithImage(const ImageData& image_data)
{
	UIButtonEx* widget = new UIButtonEx();
	if (widget && widget->InitWithImage(image_data))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

bool UIButtonEx::InitWithImage(const std::string& image_file, int texType)
{
	if(!init())
		return false;

	std::string full_path = ViewHelper::GetResoursePath(image_file);
	if (TextureResType::LOCAL == (TextureResType)texType && !ViewHelper::LoadImageResource(full_path))
		return false;

	Button::loadTextureNormal(full_path.c_str(), (Widget::TextureResType)texType);
	return true;
}

bool UIButtonEx::InitWithImage(const ImageData& image_data)
{
	if(!init())
		return false;

	Button::loadTextureNormal(image_data.clip_image_name.c_str(), TextureResType::PLIST);
	return true;
}

void UIButtonEx::SetNormalImage(const std::string &image_file, int texType)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	if (TextureResType::LOCAL == (TextureResType)texType && !ViewHelper::LoadImageResource(full_path))
		return;

	Button::loadTextureNormal(full_path.c_str(), (Widget::TextureResType)texType);
}

void UIButtonEx::SetNormalImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Button::loadTextureNormal(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UIButtonEx::SetSelectedImage(const std::string &image_file, int texType)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	if (TextureResType::LOCAL == (TextureResType)texType && !ViewHelper::LoadImageResource(full_path))
		return;

	Button::loadTexturePressed(full_path.c_str(), (Widget::TextureResType)texType);
}

void UIButtonEx::SetSelectedImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Button::loadTexturePressed(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UIButtonEx::SetDisabledImage(const std::string &image_file, int texType)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	if (TextureResType::LOCAL == (TextureResType)texType && !ViewHelper::LoadImageResource(full_path))
		return;

	Button::loadTextureDisabled(full_path.c_str(), (Widget::TextureResType)texType);
}

void UIButtonEx::SetDisabledImage(const ImageData& image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	Button::loadTextureDisabled(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UIButtonEx::SetScale9Enabled(bool able)
{
	Button::setScale9Enabled(able);
}

void UIButtonEx::SetCapInsets(float x, float y, float w, float h)
{
	Button::setCapInsets(cocos2d::CCRectMake(x,y,w,h));
}

void UIButtonEx::SetText(const std::string &text)
{
	Button::setTitleText(SystemTools::ConvertUygurToEx(text));
}

std::string UIButtonEx::GetText()
{
	return Button::getTitleText();
}

void UIButtonEx::SetFontName(const std::string &font_name)
{
	Button::setTitleFontName(font_name.c_str());
}

void UIButtonEx::SetFontSize(float font_size)
{
	Button::setTitleFontSize(font_size);
}

void UIButtonEx::SetTextColor(GameType::UINT8 red, GameType::UINT8 green, GameType::UINT8 blue)
{
	Button::setTitleColor(ccc3(red, green, blue));
}

void UIButtonEx::AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id)
{
	m_down_fun_id = down_fun_id;
	m_move_fun_id = move_fun_id;
	m_up_fun_id = up_fun_id;
	addTouchEventListener(CC_CALLBACK_2(UIButtonEx::OnTouchEvent, this));
}

void UIButtonEx::AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancle_fun_id)
{
	m_down_fun_id = down_fun_id;
	m_move_fun_id = move_fun_id;
	m_up_fun_id = up_fun_id;
	m_cancle_fun_id = cancle_fun_id;
	addTouchEventListener(CC_CALLBACK_2(UIButtonEx::OnTouchEvent, this));
}

void UIButtonEx::OnTouchEvent(cocos2d::Ref *pSender, Widget::TouchEventType type)
{
	if(!m_is_enabled)
		return;

	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		{
			cocos2d::Vec2 point = Button::getTouchStartPos();
			LuaEngine::GetInstance()->CallFunction(m_down_fun_id, "dd" ,point.x, point.y);
		}
		break;
	case Widget::TouchEventType::MOVED:
		{
			cocos2d::Vec2 point = Button::getTouchMovePos();
			LuaEngine::GetInstance()->CallFunction(m_move_fun_id, "dd", point.x, point.y);
		} 
		break;
	case Widget::TouchEventType::ENDED:
		{
			cocos2d::Vec2 point = Button::getTouchEndPos();
			LuaEngine::GetInstance()->CallFunction(m_up_fun_id, "dd", point.x, point.y);
			LuaEngine::GetInstance()->CallFunction("PlaySystemOperEffect", "ii", _nTouchSoundIndex, _nTouchSoundSubIndex);
	}
		break;
	case Widget::TouchEventType::CANCELED:
		{
			cocos2d::Vec2 point = Button::getTouchMovePosition();
			LuaEngine::GetInstance()->CallFunction(m_cancle_fun_id, "dd", point.x, point.y);
		}
		break;
	default:
		break;
	}
}

Widget* UIButtonEx::createCloneInstance()
{
	return UIButtonEx::create();
}

//设置按钮是否生效
void UIButtonEx::setEnabled(bool enable)
{
	if(enable == m_is_enabled)
		return;

	Widget::setEnabled(enable);

	m_is_enabled = enable;

	setTouchEnabled(m_is_enabled);
	setPressedActionEnabled(m_is_enabled);

	//cocos2d::GLProgram *pShaderProgram;
	//if (m_is_enabled)
	//{
	//	pShaderProgram = cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP);
	//}
	//else
	//{
	//	pShaderProgram = cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_NO_MVP);
	//}
	//_buttonNormalRenderer->setGLProgram(pShaderProgram);
	//_buttonClickedRenderer->setGLProgram(pShaderProgram);
	//_buttonDisabledRenderer->setGLProgram(pShaderProgram);
	//cocos2d::Vector<cocos2d::Node*> widget_children = this->getChildren();
	//if (widget_children.size() > 0)
	//{
	//	for (int i = 0; i < widget_children.size(); i++)
	//	{
	//		Widget* pNode = (Widget*)widget_children.at(i);
	//		pNode->setGLProgram(pShaderProgram);
	//	}
	//}
}

//获取按钮是否生效
bool UIButtonEx::GetEnable()
{
	return m_is_enabled;
}

void UIButtonEx::SetOutline(GLubyte r, GLubyte g, GLubyte b, GLubyte a, int out_line_size)
{
	cocos2d::Color4B color = cocos2d::Color4B(r, g, b, a);
	_titleRenderer->enableOutline(color, out_line_size);
}

void UIButtonEx::SetShadow(GLubyte r, GLubyte g, GLubyte b, GLubyte a, float w, float h, int blurRadius)
{
	cocos2d::Color4B color = cocos2d::Color4B(r, g, b, a);
	_titleRenderer->enableShadow(color, cocos2d::Size(w, h), blurRadius);
}

void UIButtonEx::DisableEffect()
{
	_titleRenderer->disableEffect();
}
