
#include "progress9control.h"
#include "view/struct.h"
#include "view/viewhelper.h"
//#include "luaengine/luaengine.h"
//#include "filesystem/filesystemex.h"
#include "2d/CCClippingNode.h"
#include "2d/CCDrawNode.h"

using namespace cocos2d;

Progress9Control::Progress9Control()
: m_current_volume(100.0)
, m_max_volume(100.0) 
, m_loading_sprite(NULL)
{
	m_bVolumeDirty = true;

	setCascadeOpacityEnabled(true);
	schedule(schedule_selector(Progress9Control::update));

	//创建模板相关
	m_pClippingNode = cocos2d::ClippingNode::create();
	m_pClippingNode->setCascadeOpacityEnabled(true);
	addChild(m_pClippingNode);

	m_pClippingStencil = cocos2d::DrawNode::create();
	m_pClippingNode->setStencil(m_pClippingStencil);
}

Progress9Control::~Progress9Control()
{
	m_loading_sprite = NULL;
	unschedule(schedule_selector(Progress9Control::update));
}

Progress9Control* Progress9Control::Node()
{
	Progress9Control *progress = new Progress9Control();

	if(!progress->init())
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}

	progress->autorelease();
	return progress;
}

//设置进度条图片
bool Progress9Control::SetProgressImage(const std::string &loading_image_file)
{
	if(m_loading_sprite)
	{
		removeChild(m_loading_sprite, true);
		m_loading_sprite = NULL;
	}

	m_loading_sprite = Scale9SpriteEx::CreateWithImageFile(loading_image_file);
	if(!m_loading_sprite)
	{
		return false;
	}

	m_loading_sprite->setAnchorPoint(Vec2(0.0, 0.0));
	m_loading_sprite->setCascadeOpacityEnabled(true);
	m_pClippingNode->addChild(m_loading_sprite, 1);

	return true;
}

bool Progress9Control::SetProgressImage(const ImageData &loading_image)
{
	if(m_loading_sprite)
	{
		removeChild(m_loading_sprite, true);
		m_loading_sprite = NULL;
	}

	m_loading_sprite = Scale9SpriteEx::CreateWithImageFile(loading_image);
	if(!m_loading_sprite)
	{
		return false;
	}

	m_loading_sprite->setAnchorPoint(Vec2(0.0, 0.0));
	m_pClippingNode->addChild(m_loading_sprite, 1);

	return true;
}

Progress9Control* Progress9Control::CreateWithImage(const std::string &loading_image_file)
{
	Progress9Control *progress = new Progress9Control();

	if(!progress->SetProgressImage(loading_image_file))
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}

	progress->autorelease();
	return progress;
}

Progress9Control* Progress9Control::CreateWithImage(const ImageData &loading_image)
{
	Progress9Control *progress = new Progress9Control();

	if(!progress->SetProgressImage(loading_image))
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}

	progress->autorelease();
	return progress;
}

float Progress9Control::AddProgress(float volume)
{
	float pre_volume = m_current_volume;

	SetProgress(m_current_volume + volume);

	return pre_volume;
}

void Progress9Control::update(float dt)
{
	if (!m_bVolumeDirty)
	{
		return;
	}

	if (!m_loading_sprite)
	{
		return;
	}

	m_loading_sprite->SetStretchSize(_contentSize.width, _contentSize.height);

	//边界处理
	if(m_current_volume >= m_max_volume)
	{
		m_current_volume = m_max_volume;
	}
	else if(m_current_volume < 0)
	{
		m_current_volume = 0;
	}

	//重画模板
	float stencilWidth = (m_current_volume / m_max_volume) * _contentSize.width;
	float stencilHeight = _contentSize.height;
	Vec2 rect[4];
	rect[1].set(stencilWidth, 0.0f);
	rect[2].set(stencilWidth, stencilHeight);
	rect[3].set(0.0f, stencilHeight);
	Color4F green(0.0f, 1.0f, 0.0f, 1.0f);
	m_pClippingStencil->clear();
	m_pClippingStencil->drawPolygon(rect, 4, green, 0, green);

	m_bVolumeDirty = false;
}
