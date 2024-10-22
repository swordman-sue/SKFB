#include "uitouchgroupex.h"
#include "ui/UIWidget.h"
#include "ui/UIHelper.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UITouchGroupEx)

UITouchGroupEx::UITouchGroupEx() 
:m_swallow(false),
m_pRootWidget(nullptr)
{}

UITouchGroupEx::~UITouchGroupEx(void)
{
	if (m_pRootWidget)
	{
		this->removeChild(m_pRootWidget);
	}
}

UITouchGroupEx* UITouchGroupEx::create()
{
	UITouchGroupEx *node = new UITouchGroupEx();
	if (node && node->init())
	{
		node->autorelease();
		node->SetSwallow(true);
		return node;
	}
	else
	{
		CC_SAFE_DELETE(node);
		return NULL;
	}
}

bool UITouchGroupEx::init()
{
	if (!Layer::init())
	{
		return false;
	}
	m_pRootWidget = cocos2d::ui::Widget::create();
	this->addChild(m_pRootWidget);

	return true;
}

void UITouchGroupEx::onEnter()
{
	Layer::onEnter();
	
	int parent_touch_priority = TOUCH_GROUP_PRIORITY;
	SetTouchPriority(parent_touch_priority);
}

void UITouchGroupEx::AddWidget( cocos2d::ui::Widget* widget )
{
	m_pRootWidget->addChild(widget);
}

void UITouchGroupEx::AddWidget(cocos2d::ui::Widget* widget, int z_order)
{
	m_pRootWidget->addChild(widget, z_order);
}

void UITouchGroupEx::AddWidget(cocos2d::ui::Widget* widget, int z_order, int tag)
{
	m_pRootWidget->addChild(widget, z_order, tag);
}

void UITouchGroupEx::RemoveWidget(cocos2d::ui::Widget* widget)
{
	m_pRootWidget->removeChild(widget);
}

cocos2d::ui::Widget* UITouchGroupEx::GetWidgetByName(const std::string& name)
{
	if (!m_pRootWidget)
	{
		return NULL;
	}
	return dynamic_cast<Widget*>(cocos2d::ui::Helper::seekWidgetByName(m_pRootWidget, name));
}

void UITouchGroupEx::SetSwallow(bool swallow)
{
	if(m_swallow != swallow) 
	{
		m_swallow = swallow;
		setSwallowsTouches(swallow);
	}
}

void UITouchGroupEx::SetTouchPriority(GameType::INT32 priority)
{
}

GameType::INT32 UITouchGroupEx::GetTouchPriority()
{
	return this->getLocalZOrder();
}
