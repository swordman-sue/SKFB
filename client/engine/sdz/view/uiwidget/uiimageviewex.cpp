#include "uiimageviewex.h"
#include "view/viewhelper.h"
#include "view/struct.h"
#include "luaengine/luaengine.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramCache.h"
#include "filesystem/filesystemex.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UIImageViewEx)

UIImageViewEx::UIImageViewEx():
m_down_fun_id(0),
m_move_fun_id(0),
m_up_fun_id(0),
m_cancel_fun_id(0)
{}

UIImageViewEx::~UIImageViewEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_down_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_move_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_up_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_cancel_fun_id);
}

UIImageViewEx* UIImageViewEx::create()
{
	UIImageViewEx* widget = new UIImageViewEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

UIImageViewEx* UIImageViewEx::CreateWithImage(const std::string& image_file, int texType)
{
	UIImageViewEx* widget = create();

	if(!widget)
		return NULL;

	widget->SetImage(image_file, texType);
	return widget;
}

UIImageViewEx* UIImageViewEx::CreateWithImage(const ImageData& image_data)
{
	UIImageViewEx* widget = create();

	if(!widget)
		return NULL;

	widget->SetImage(image_data);
	return widget;
}

// 修改UIImageViewEx对PLIST和LOCAL的图片资源类型的处理 edit by JiaMiao 2016-5-25
void UIImageViewEx::SetImage(const std::string &image_file, int texType)
{
	std::string full_path = ViewHelper::GetResoursePath(image_file);
	if (TextureResType::LOCAL == (TextureResType)texType && !ViewHelper::LoadImageResource(full_path))
		return;

	ImageView::loadTexture(full_path.c_str(), (Widget::TextureResType)texType);
}

void UIImageViewEx::SetImage(const ImageData &image_data)
{
	if (image_data.clip_image_name.c_str() == "")
		return;

	ImageView::loadTexture(image_data.clip_image_name.c_str(), TextureResType::PLIST);
}

void UIImageViewEx::SetImageRect(const float x, const float y, const float w, const float h)
{
	cocos2d::Rect rect(x, y, w, h);
	ImageView::setTextureRect(rect);
}

void UIImageViewEx::SetScaleSize(float x, float y)
{
	setContentSize(cocos2d::Size(x, y));
}

void UIImageViewEx::SetScale9Enabled(bool enable)
{
	ImageView::setScale9Enabled(enable);
}

void UIImageViewEx::SetCapInsets(float x, float y, float w, float h)
{
	ImageView::setCapInsets(cocos2d::Rect(x, y, w, h));
}

void UIImageViewEx::AddEventListener(GameType::UINT32 down_fun_id, GameType::UINT32 move_fun_id, GameType::UINT32 up_fun_id, GameType::UINT32 cancel_fun_id)
{
	this->m_down_fun_id = down_fun_id;
	this->m_move_fun_id = move_fun_id;
	this->m_up_fun_id = up_fun_id;
	this->m_cancel_fun_id = cancel_fun_id;
	ImageView::addTouchEventListener(CC_CALLBACK_2(UIImageViewEx::OnTouchEvent, this));
}

void UIImageViewEx::OnTouchEvent(Ref* target, Widget::TouchEventType type)
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
		LuaEngine::GetInstance()->CallFunction("PlaySystemOperEffect", "ii", _nTouchSoundIndex, _nTouchSoundSubIndex);
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

void UIImageViewEx::setGLProgram(cocos2d::GLProgram *pShaderProgram)
{
	Node::setGLProgram(pShaderProgram);
	cocos2d::Vector<Node*> widget_children = this->getChildren();
	if (widget_children.size() > 0)
	{
		for (int i = 0; i < widget_children.size(); i++)
		{
			Widget* pNode = (Widget*)widget_children.at(i);
			pNode->setGLProgram(pShaderProgram);
		}
	}

	_imageRenderer->setGLProgram(pShaderProgram);

	//九宫格图片需要将其中子级设置shader
	//if (_scale9Enabled)
	//{
	//	cocos2d::Vector<Node*> pChildren = _imageRenderer->getChildren();
	//	if (pChildren.size() > 0)
	//	{
	//		for (int i = 0; i<pChildren.size(); i++)
	//		{
	//			Node* pChild = pChildren.at(i);
	//			cocos2d::Vector<Node*> insetChildren = pChild->getChildren();
	//			if (insetChildren.size() > 0)
	//			{
	//				for (int j = 0; j < insetChildren.size(); j++)
	//				{
	//					Node* insetChild = insetChildren.at(j);
	//					insetChild->setGLProgram(pShaderProgram);
	//				}
	//			}
	//		}
	//	}
	//}
}

void UIImageViewEx::SetGray(bool grey)
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
	this->setGLProgram(pShaderProgram);

	_imageRenderer->setState(grey ? Scale9Sprite::State::GRAY : Scale9Sprite::State::NORMAL);
}

cocos2d::Node* UIImageViewEx::getVirtualRenderer()
{
	return ImageView::getVirtualRenderer();
}

void UIImageViewEx::SetAliasTexParameters()
{
	cocos2d::Sprite* imageSprite = dynamic_cast<cocos2d::Sprite*>(this->getVirtualRenderer());
	if (imageSprite)
	{
		imageSprite->getTexture()->setAliasTexParameters();
	}
}

cocos2d::ui::Widget* UIImageViewEx::createCloneInstance()
{
	return UIImageViewEx::create();
}
