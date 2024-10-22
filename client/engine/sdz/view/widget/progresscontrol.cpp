#include "progresscontrol.h"
#include "luaengine/luaengine.h"
#include "view/struct.h"
#include "filesystem/filesystemex.h"
#include "view/viewhelper.h"
#include "zqcommon/logutil.h"

using namespace cocos2d;

ProgressControl::ProgressControl()
:m_current_volume(0.0), 
m_max_volume(100.0), 
m_width(0), 
m_height(0), 
m_loading_sprite(NULL), 
m_dx(0), 
m_dy(0), 
m_background_sprite(NULL), 
m_flip(false)
{
	setCascadeOpacityEnabled(true);
}

ProgressControl::~ProgressControl()
{
}

ProgressControl* ProgressControl::Node()
{
	ProgressControl *progress = new ProgressControl();

	if(!progress->init())
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}
	progress->autorelease();
	return progress;
}

void ProgressControl::onEnter()
{
	Layer::onEnter();
	schedule(schedule_selector(ProgressControl::update), 0.04f);
}

void ProgressControl::onExit()
{
	unschedule(schedule_selector(ProgressControl::update));
	Layer::onExit();
}

//设置进度条图片
bool ProgressControl::SetProgressImage(const std::string &loading_image_file)
{
	if(m_loading_sprite)
	{
		removeChild(m_loading_sprite, true);
		m_loading_sprite = NULL;
	}

	m_loading_sprite = SpriteEx::CreateWithImageFile(loading_image_file);
	if(!m_loading_sprite)
		return false;

	m_width = m_loading_sprite->getTextureRect().size.width;
	m_height = m_loading_sprite->getTextureRect().size.height;
	m_loading_sprite->setAnchorPoint(Vec2(0.0, 0.5));
	m_loading_sprite->setPosition(Vec2(-m_width / 2, 0));
	addChild(m_loading_sprite, 1);

	return true;
}

bool ProgressControl::SetProgressImage(const ImageData &loading_image)
{
	if(m_loading_sprite)
	{
		removeChild(m_loading_sprite, true);
		m_loading_sprite = NULL;
	}

	m_loading_sprite = SpriteEx::CreateWithImageFile(loading_image);
	if(!m_loading_sprite)
		return false;

	m_dx = loading_image.x;
	m_dy = loading_image.y;
	m_width = loading_image.width;
	m_height = loading_image.height;
	m_loading_sprite->setAnchorPoint(Vec2(0.0, 0.5));
	m_loading_sprite->setPosition(Vec2(-m_width / 2, 0));
	addChild(m_loading_sprite, 1);

	return true;
}

//设置进度条背景图片
bool ProgressControl::SetBkImage(const std::string &background_image_file)
{
	if(m_background_sprite)
	{
		removeChild(m_background_sprite, true);
		m_background_sprite = NULL;
	}

	if(!ViewHelper::LoadImageResource(background_image_file))
		return false;

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(background_image_file);
	Texture2D *background_texture = cocos2d::Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	if(NULL == background_texture)
		return false;
	
	m_background_sprite = SpriteEx::CreateWithImageFile(background_image_file);
	if(!m_background_sprite)
		return false;

	m_background_sprite->setAnchorPoint(Vec2(0.0, 0.5));
	m_background_sprite->setPosition(Vec2(-m_background_sprite->getTextureRect().size.width / 2, 0));
	addChild(m_background_sprite, 0);
	
	return true;
}

bool ProgressControl::SetBkImage(const ImageData &background_image)
{
	if(m_background_sprite)
	{
		removeChild(m_background_sprite, true);
		m_background_sprite = NULL;
	}

	if(!ViewHelper::LoadImageResource(background_image.image_path))
		return false;

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(background_image.image_path);
	Texture2D *background_texture = cocos2d::Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	Rect rect = Rect(background_image.x, background_image.y, background_image.width, background_image.height);
	if(NULL == background_texture)
		return false;

	m_background_sprite = SpriteEx::CreateWithImageFile(background_image);
	if(!m_background_sprite)
		return false;

	m_background_sprite->setAnchorPoint(Vec2(0.0, 0.5));
	m_background_sprite->setPosition(Vec2(-background_image.width / 2, 0));
	addChild(m_background_sprite, 0);
	return true;
}

bool ProgressControl::InitWithImage(const std::string &loading_image_file, const std::string &background_image_file)
{	
	bool ret = SetProgressImage(loading_image_file);
	if("" != background_image_file)
	{
		ret = ret && SetBkImage(background_image_file);
	}

	return ret;
}

bool ProgressControl::InitWithImage(const ImageData &loading_image, const ImageData &background_image)
{
	bool ret = SetProgressImage(loading_image);

	if("" != background_image.image_path)
	{
		ret = ret && SetBkImage(background_image);
	}

	return ret;
}

ProgressControl* ProgressControl::CreateWithImage(const std::string &loading_image_file, const std::string &background_image_file)
{
	ProgressControl *progress = new ProgressControl();

	if(!progress->InitWithImage(loading_image_file, background_image_file))
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}

	progress->autorelease();
	return progress;
}

ProgressControl* ProgressControl::CreateWithImage(const ImageData &loading_image, const ImageData &background_image)
{
	ProgressControl *progress = new ProgressControl();

	if(!progress->InitWithImage(loading_image, background_image))
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}

	progress->autorelease();
	return progress;
}

float ProgressControl::AddProgress(float volume)
{
	float pre_volume = m_current_volume;

	m_current_volume += volume;

	SetCurrentProgress();
	return pre_volume;
}

void ProgressControl::SetProgress(float volume)
{
	if(volume < 0 || volume > m_max_volume)
	{
		LogUtil::LogError("ProgressControl::SetProgress volume is invalid");
		return;
	}
	m_current_volume = volume;
	SetCurrentProgress();
}

void ProgressControl::SetCurrentProgress()
{
	if (m_current_volume >= m_max_volume)
	{
		m_current_volume = m_max_volume;
	}
	else if (m_current_volume < 0)
	{
		m_current_volume = 0;
	}

	if (m_flip)
	{
		float width = (1 - m_current_volume) / m_max_volume * m_width;
		float height = m_height;
		m_loading_sprite->setTextureRect(Rect(m_dx + width, m_dy, m_width - width, height));
		m_loading_sprite->setPosition(Vec2(-m_width / 2 + width, 0));
	}
	else
	{
		float width = (m_current_volume / m_max_volume) * m_width;
		float height = m_height;
		m_loading_sprite->setTextureRect(Rect(m_dx, m_dy, width, height));
	}
}

void ProgressControl::update(float dt)
{
	SetCurrentProgress();
}

//设置旋转
void ProgressControl::SetFlip(bool flip)
{
	m_flip = flip;
}
