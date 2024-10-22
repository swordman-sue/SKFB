#include "imagebutton.h"
#include "view/gamelayer.h"
#include "luaengine/luaengine.h"
#include "filesystem/filesystemex.h"
#include "view/viewhelper.h"
#include "spriteex.h"
#include "scale9spriteex.h"

using namespace cocos2d;

ImageButton::ImageButton()
:m_enable(true), 
m_lua_down_fun_id(0), 
m_lua_move_fun_id(0), 
m_lua_up_fun_id(0), 
m_lua_cancel_fun_id(0), 
m_normal_sprite(NULL), 
m_selected_sprite(NULL),
m_disable_sprite(NULL),
m_menu_text_item(NULL),
m_touch_overlap(false),
m_menuitem_sprite(NULL), 
m_stretch_width(0), 
m_stretch_height(0),
m_auto_gen(false),
m_flip_x(false),
m_flip_y(false)
{
	m_text_color = Color3B(255, 255, 255);
	m_disable_text_color = Color3B(128, 128, 128);
}

ImageButton::~ImageButton()
{
	if(m_lua_down_fun_id != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_lua_down_fun_id);
	}
	
	if(m_lua_up_fun_id != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_lua_up_fun_id);
	}

	if(m_lua_move_fun_id != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_lua_move_fun_id);
	}

	if(m_lua_cancel_fun_id != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_lua_cancel_fun_id);
	}
}

bool ImageButton::Init()
{
	if(!Menu::init())
		return false;

	setContentSize(Size(0, 0));
	m_menuitem_sprite = MenuItemSprite::create(NULL, NULL, NULL);
	addChild(m_menuitem_sprite);
	return true;
}

// 创建一个空的按钮
ImageButton* ImageButton::Node()
{
	ImageButton *button = new ImageButton();
	
	if(!button->Init())
	{
		CC_SAFE_DELETE(button);
		return NULL;
	}
	
	button->autorelease();
	return button;
}

bool ImageButton::InitWithImage(const std::string &image_file)
{
	if (!init())
		return false;
	
	if("" == image_file)
		return false;

	if(!ViewHelper::LoadImageResource(image_file))
		return false;

	SetNormalImage(image_file);

	setContentSize(Size(0, 0));
	addChild(m_menuitem_sprite, 0, 0);
	return true;
}

bool ImageButton::InitWithImage(const ImageData& image_data)
{
	if (!init())
		return false;

	if("" == image_data.image_path)	
		return false;

	if(!ViewHelper::LoadImageResource(image_data.image_path))
		return false;

	SetNormalImage(image_data);

	setContentSize(Size(0, 0));
	addChild(m_menuitem_sprite, 0, 0);
	return true;
}

ImageButton* ImageButton::CreateWithImages(const std::string &normal_image)
{
	ImageButton *button = new ImageButton();
	button->SetAutoGen(true);
	
	if(!button->InitWithImage(normal_image))
	{
		CC_SAFE_DELETE(button);
		return NULL;
	}

	button->autorelease();
	return button;
}

ImageButton* ImageButton::CreateWithImages(const std::string &normal_image, const std::string &down_image, const std::string &disable_image)
{
	ImageButton *button = new ImageButton();

	if(!button->InitWithImage(normal_image))
	{
		CC_SAFE_DELETE(button);
		return NULL;
	}

	button->SetSelectedImage(down_image);
	button->SetDisabledImage(disable_image);
	button->autorelease();

	return button;
}

ImageButton* ImageButton::CreateWithImages(const ImageData &normal_image)
{
	ImageButton *button = new ImageButton();
	button->SetAutoGen(true);

	if(!button->InitWithImage(normal_image))
	{
		CC_SAFE_DELETE(button);
		return NULL;
	}

	button->autorelease();
	return button;
}

ImageButton* ImageButton::CreateWithImages(const ImageData &normal_image, const ImageData &down_image, const ImageData &disable_image)
{
	ImageButton *button = new ImageButton();

	if(!button->InitWithImage(normal_image))
	{
		CC_SAFE_DELETE(button);
		return NULL;
	}

	button->SetSelectedImage(down_image);
	button->SetDisabledImage(disable_image);
	button->autorelease();

	return button;
}

template<class T> void ImageButton::SetNormalImageTemplate(T& image_file)
{
	m_normal_sprite = Scale9SpriteEx::CreateWithImageFile(image_file);
	m_normal_sprite->SetFlipX(m_flip_x);
	m_normal_sprite->SetFlipY(m_flip_y);

	if (!m_menuitem_sprite)
	{
		m_menuitem_sprite = MenuItemSprite::create(m_normal_sprite, NULL, NULL);
	}
	else
	{
		m_menuitem_sprite->setNormalImage(m_normal_sprite);
	}

	if (m_auto_gen)
	{
		m_selected_sprite = Scale9SpriteEx::CreateWithImageFile(image_file);
		m_selected_sprite->SetHighLight(true);
		m_selected_sprite->SetFlipX(m_flip_x);
		m_selected_sprite->SetFlipY(m_flip_y);
		m_menuitem_sprite->setSelectedImage(m_selected_sprite);

		m_disable_sprite = Scale9SpriteEx::CreateWithImageFile(image_file);
		m_disable_sprite->SetGray(true);
		m_disable_sprite->SetFlipX(m_flip_x);
		m_disable_sprite->SetFlipY(m_flip_y);
		m_menuitem_sprite->setDisabledImage(m_disable_sprite);
	}

	if (m_stretch_width != 0 && m_stretch_height != 0)
	{
		SetStretchSize(m_stretch_width, m_stretch_height);
	}
}

void ImageButton::SetNormalImage(const std::string &image_file)
{
	if ("" == image_file)
		return;

	if (!ViewHelper::LoadImageResource(image_file))
		return;

	SetNormalImageTemplate(image_file);
}

void ImageButton::SetNormalImage(const ImageData& image_data)
{
	if ("" == image_data.image_path)
		return;

	if (!ViewHelper::LoadImageResource(image_data.image_path))
		return;

	SetNormalImageTemplate(image_data);
}

void ImageButton::SetSelectedImage(const std::string &image_file)
{
	if("" == image_file)
	{	
		return;
	}

	if(!ViewHelper::LoadImageResource(image_file))
	{
		return;
	}

	m_selected_sprite = Scale9SpriteEx::CreateWithImageFile(image_file);
	m_selected_sprite->SetFlipX(m_flip_x);
	m_selected_sprite->SetFlipY(m_flip_y);
	if (m_stretch_width != 0 && m_stretch_height != 0)
	{
		m_selected_sprite->SetStretchSize(m_stretch_width, m_stretch_height);
	}
	m_menuitem_sprite->setSelectedImage(m_selected_sprite);
}

void ImageButton::SetSelectedImage(const ImageData &image_data)
{
	if("" == image_data.image_path)
	{	
		return;
	}

	if(!ViewHelper::LoadImageResource(image_data.image_path))
	{
		return;
	}

	m_selected_sprite = Scale9SpriteEx::CreateWithImageFile(image_data);
	m_selected_sprite->SetFlipX(m_flip_x);
	m_selected_sprite->SetFlipY(m_flip_y);
	if (m_stretch_width != 0 && m_stretch_height != 0)
	{
		m_selected_sprite->SetStretchSize(m_stretch_width, m_stretch_height);
	}
	m_menuitem_sprite->setSelectedImage(m_selected_sprite);
}

void ImageButton::SetDisabledImage(const std::string &image_file)
{
	if("" == image_file)
	{	
		return;
	}

	if(!ViewHelper::LoadImageResource(image_file))
	{
		return;
	}

	m_disable_sprite = Scale9SpriteEx::CreateWithImageFile(image_file);
	m_disable_sprite->SetFlipX(m_flip_x);
	m_disable_sprite->SetFlipY(m_flip_y);
	if (m_stretch_width != 0 && m_stretch_height != 0)
	{
		m_disable_sprite->SetStretchSize(m_stretch_width, m_stretch_height);
	}
	m_menuitem_sprite->setDisabledImage(m_disable_sprite);
}

void ImageButton::SetDisabledImage(const ImageData& image_data)
{
	if("" == image_data.image_path)
	{	return;}

	if(!ViewHelper::LoadImageResource(image_data.image_path))
	{
		return;
	}

	m_disable_sprite = Scale9SpriteEx::CreateWithImageFile(image_data);
	m_disable_sprite->SetFlipX(m_flip_x);
	m_disable_sprite->SetFlipY(m_flip_y);
	if (m_stretch_width != 0 && m_stretch_height != 0)
	{
		m_disable_sprite->SetStretchSize(m_stretch_width, m_stretch_height);
	}
	m_menuitem_sprite->setDisabledImage(m_disable_sprite);
}

void ImageButton::SetIsEnabled(bool enable)
{
	m_enable = enable;
	m_menuitem_sprite->setEnabled(m_enable);
	
	if(NULL != m_menu_text_item)
	{
		m_menu_text_item->setEnabled(enable);
	}
}

bool ImageButton::onTouchBegan(Touch* touch, Event* event)
{
	//必须点击在父窗口的有效区内才生效
	cocos2d::Node *parent = Menu::getParent();
	if(parent && typeid(*parent) == typeid(GameLayer))
	{
		GameLayer* parent_layer = dynamic_cast<GameLayer*>(parent);
		if(!parent_layer->IsValidTouchPos(touch))
		{
			return false;
		}
	}

	if (!Menu::onTouchBegan(touch, event))
	{	
		return false;
	}

	if (m_touch_overlap)
		m_menuitem_sprite->getNormalImage()->setVisible(true);

	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);
	touch_point = convertToNodeSpace(touch_point);

	if(m_lua_down_fun_id != 0)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_down_fun_id, "dd", touch_point.x, touch_point.y);
	}
	return true;
}

void ImageButton::onTouchEnded(Touch* touch, Event* event)
{
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);
	touch_point = convertToNodeSpace(touch_point);

	Menu::onTouchEnded(touch, event);
	
	if (_selectedItem)
	{
		if(m_lua_up_fun_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(m_lua_up_fun_id, "dd", touch_point.x, touch_point.y);
		}
	}
	else
	{
		if(m_lua_cancel_fun_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(m_lua_cancel_fun_id, "dd", touch_point.x, touch_point.y);
		}
	}
}

void ImageButton::onTouchMoved(Touch* touch, Event* event)
{
	Point touch_point = touch->getLocationInView();
	touch_point = Director::getInstance()->convertToGL(touch_point);
	touch_point = convertToNodeSpace(touch_point);
	Menu::onTouchMoved(touch, event);

	if(m_lua_move_fun_id != 0)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_move_fun_id, "dd", touch_point.x, touch_point.y);
	}
}

//  Lua版本的回调函数
void ImageButton::SetLuaCallBack(ZQ::UINT32 down_call_fun, ZQ::UINT32 move_call_fun, ZQ::UINT32 up_call_fun, ZQ::UINT32 cancel_call_func)
{
	m_lua_down_fun_id = down_call_fun;
	m_lua_move_fun_id = move_call_fun;
	m_lua_up_fun_id = up_call_fun;
	m_lua_cancel_fun_id = cancel_call_func;
}

// 设置按钮的文字
void ImageButton::SetButtonText(const std::string &text, const std::string &font_name, float font_size)
{
	if(NULL != m_menu_text_item)
	{
		m_menu_text_item->setString(text.c_str());
	}
	else
	{
		LabelTTF *text_label = LabelTTF::create(text.c_str(), font_name.c_str(), font_size);
		m_menu_text_item = MenuItemLabel::create(text_label);
		m_menu_text_item->setColor(m_text_color);
		m_menu_text_item->setDisabledColor(m_disable_text_color);
		m_menu_text_item->setAnchorPoint(CCPointMake(0.5, 0.5));
		addChild(m_menu_text_item, 0, 0);
	}
}

//设置按钮文字的颜色
void ImageButton::SetButtonTextColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue)
{
	m_text_color = ccc3(red, green, blue);
	if(NULL != m_menu_text_item)
	{
		m_menu_text_item->setColor(m_text_color);
	}
}

//设置按钮变化后文字的颜色
void ImageButton::SetDisableButtonTextColor(ZQ::UINT8 red, ZQ::UINT8 green, ZQ::UINT8 blue)
{
	m_disable_text_color = ccc3(red, green, blue);
	if(NULL != m_menu_text_item)
	{
		m_menu_text_item->setDisabledColor(m_disable_text_color);
	}
}

//获取宽高
float ImageButton::GetWidth()
{
	return m_menuitem_sprite->getContentSize().width;
}

float ImageButton::GetHeight()
{
	return m_menuitem_sprite->getContentSize().height;
}

void ImageButton::SetStretchSize(float width, float height)
{
	if(width == 0 || height == 0)
		return;

	m_menuitem_sprite->setContentSize(Size(width, height));

	if(m_normal_sprite)
	{
		m_normal_sprite->SetStretchSize(width, height);
	}

	if(m_selected_sprite)
	{
		m_selected_sprite->SetStretchSize(width, height);
	}

	if(m_disable_sprite)
	{
		m_disable_sprite->SetStretchSize(width, height);
	}

	m_stretch_width = width;
	m_stretch_height = height;
}

void ImageButton::SetFlipX(bool flip)
{
	if (m_flip_x == flip)
		return;

	if (m_normal_sprite)
		m_normal_sprite->SetFlipX(flip);
	
	if (m_selected_sprite)
		m_selected_sprite->SetFlipX(flip);
	
	if (m_disable_sprite)
		m_disable_sprite->SetFlipX(flip);

	m_flip_x = flip;
}

void ImageButton::SetFlipY(bool flip)
{
	if (m_flip_y == flip)
		return;

	if (m_normal_sprite)
		m_normal_sprite->SetFlipY(flip);
	
	if (m_selected_sprite)
		m_selected_sprite->SetFlipY(flip);
	
	if (m_disable_sprite)
		m_disable_sprite->SetFlipY(flip);

	m_flip_y = flip;
}

//获取正常状态下的图片控件
Scale9SpriteEx* ImageButton::GetNormalImageNode()
{
	if(!m_normal_sprite)
		return NULL;

	return m_normal_sprite;
}

//获取下按状态下的图片控件
Scale9SpriteEx* ImageButton::GetSelectedImageNode()
{
	if(!m_selected_sprite)
		return NULL;

	return m_selected_sprite;
}

//获取无状态下的图片控件
Scale9SpriteEx* ImageButton::GetDisableImageNode()
{
	if(!m_disable_sprite)
		return NULL;
	
	return m_disable_sprite;
}
