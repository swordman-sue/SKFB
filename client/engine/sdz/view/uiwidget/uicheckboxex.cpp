#include "uicheckboxex.h"
#include "luaengine/luaengine.h"
#include "view/viewhelper.h"
#include "view/struct.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UICheckBoxEx)

UICheckBoxEx::UICheckBoxEx()
:m_select_fun_id(0), 
m_unselect_fun_id(0)
{}

UICheckBoxEx::~UICheckBoxEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_select_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_unselect_fun_id);
}

UICheckBoxEx* UICheckBoxEx::create()
{
	UICheckBoxEx* widget = new UICheckBoxEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UICheckBoxEx* UICheckBoxEx::CreateWithImage(const std::string& backgroud_image_file, const std::string& cross_image_file)
{
	UICheckBoxEx* widget = new UICheckBoxEx();
	if (widget && widget->InitWithImage(backgroud_image_file, cross_image_file))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UICheckBoxEx* UICheckBoxEx::CreateWithImage(const ImageData& background_image_data, const ImageData& cross_image_data)
{
	UICheckBoxEx* widget = new UICheckBoxEx();
	if (widget && widget->InitWithImage(background_image_data, cross_image_data))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

bool UICheckBoxEx::InitWithImage(const std::string& backgroud_image_file, const std::string& cross_image_file)
{
	if(!CheckBox::init())
		return false;

	SetBackGroundImage(backgroud_image_file);
	SetCrossImage(cross_image_file);
	return true;
}

bool UICheckBoxEx::InitWithImage(const ImageData& background_image_data, const ImageData& cross_image_data)
{
	if(!CheckBox::init())
		return false;

	SetBackGroundImage(background_image_data);
	SetCrossImage(cross_image_data);
	return true;
}

void UICheckBoxEx::SetBackGroundImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	CheckBox::loadTextureBackGround(full_path.c_str(), TextureResType::LOCAL);
}

void UICheckBoxEx::SetBackGroundImage(const ImageData &image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	CheckBox::loadTextureBackGround(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UICheckBoxEx::SetBackGroundDisableImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	CheckBox::loadTextureBackGroundDisabled(full_path.c_str(), TextureResType::LOCAL);
}

void UICheckBoxEx::SetBackGroundDisableImage(const ImageData &image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	CheckBox::loadTextureBackGroundDisabled(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UICheckBoxEx::SetBackGroundSelectImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	CheckBox::loadTextureBackGroundSelected(full_path.c_str(), TextureResType::LOCAL);
}

void UICheckBoxEx::SetBackGroundSelectImage(const ImageData &image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	CheckBox::loadTextureBackGroundSelected(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UICheckBoxEx::SetCrossImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	CheckBox::loadTextureFrontCross(full_path.c_str(), TextureResType::LOCAL);
}

void UICheckBoxEx::SetCrossImage(const ImageData &image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	CheckBox::loadTextureFrontCross(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UICheckBoxEx::SetCrossDisableImage(const std::string &image_file)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	CheckBox::loadTextureFrontCrossDisabled(full_path.c_str(), TextureResType::LOCAL);
}

void UICheckBoxEx::SetCrossDisableImage(const ImageData &image_data)
{
	if(image_data.clip_image_name.c_str() == "")
		return;

	CheckBox::loadTextureFrontCrossDisabled(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UICheckBoxEx::SetSelectedState(bool selected)
{
	CheckBox::setSelectedState(selected);
}

bool UICheckBoxEx::GetSelectedState()
{
	return CheckBox::getSelectedState();
}

//重写onTouchEnded 修复在单选框外松开触碰点后，仍然触发选中回调的问题 add by swordman sue 2017-2-25
void UICheckBoxEx::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent)
{
	bool highlight = _highlight;
	AbstractCheckButton::onTouchEnded(touch, unusedEvent);
	if (!highlight)
	{
		return;
	}

	if (_isSelected)
	{
		setSelected(false);
		dispatchSelectChangedEvent(false);
	}
	else
	{
		setSelected(true);
		dispatchSelectChangedEvent(true);
	}

	LuaEngine::GetInstance()->CallFunction("PlaySystemOperEffect", "ii", _nTouchSoundIndex, _nTouchSoundSubIndex);
}

void UICheckBoxEx::SetEventListener(GameType::UINT32 select_fun_id, GameType::UINT32 unselect_fun_id)
{
	m_select_fun_id = select_fun_id;
	m_unselect_fun_id = unselect_fun_id;
	addEventListener(CC_CALLBACK_2(UICheckBoxEx::OnSelectEvent, this));
}

void UICheckBoxEx::OnSelectEvent(cocos2d::Ref *pSender, cocos2d::ui::CheckBox::EventType eventType)
{
	if (cocos2d::ui::CheckBox::EventType::SELECTED == eventType)
	{
		LuaEngine::GetInstance()->CallFunction(m_select_fun_id, "");
	}
	else
	{
		LuaEngine::GetInstance()->CallFunction(m_unselect_fun_id, "");
	}
}

void UICheckBoxEx::SetGray(bool grey)
{
	cocos2d::GLProgram *pShaderProgram;
	if (grey)
	{
		pShaderProgram = cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_NO_MVP);
	}
	else
	{
		pShaderProgram = cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP);
	}
	_backGroundBoxRenderer->setGLProgram(pShaderProgram);
	_backGroundSelectedBoxRenderer->setGLProgram(pShaderProgram);
	_frontCrossRenderer->setGLProgram(pShaderProgram);
	_backGroundBoxDisabledRenderer->setGLProgram(pShaderProgram);
	_frontCrossDisabledRenderer->setGLProgram(pShaderProgram);

	cocos2d::Vector<cocos2d::Node*> widget_children = this->getChildren();
	if (widget_children.size() > 0)
	{
		for (int i = 0; i < widget_children.size(); i++)
		{
			Widget* pNode = (Widget*)widget_children.at(i);
			pNode->setGLProgram(pShaderProgram);
		}
	}
}

cocos2d::ui::Widget* UICheckBoxEx::createCloneInstance()
{
	return UICheckBoxEx::create();
}
