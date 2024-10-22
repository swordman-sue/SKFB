#include "uilistviewex.h"
#include "luaengine/luaengine.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UIListViewEx)

UIListViewEx::UIListViewEx() :
m_scroll_fun_id(0), 
scroll_percent(0), 
m_create_fun_id(0)
{
}

UIListViewEx::~UIListViewEx()
{
	scroll_percent = 0;
	LuaEngine::GetInstance()->DelLuaFuncID(m_scroll_fun_id);
	LuaEngine::GetInstance()->DelLuaFuncID(m_create_fun_id);
}

UIListViewEx* UIListViewEx::create()
{
	UIListViewEx* widget = new UIListViewEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

const cocos2d::Vec2& UIListViewEx::GetInnerContainerPos()
{
	return this->_innerContainer->getPosition();
}

void UIListViewEx::AddEventListener(GameType::UINT32 scroll_fun_id)
{
	m_scroll_fun_id = scroll_fun_id;
	ScrollView::addEventListener(CC_CALLBACK_2(UIListViewEx::OnTouchEvent, this));
}

void UIListViewEx::AddCreateEventListener(GameType::UINT32 create_fun_id)
{
	m_create_fun_id = create_fun_id;
}

void UIListViewEx::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{
	Layout::visit(renderer, parentTransform, parentFlags);
	if (m_create_fun_id >0)
	{
		LuaEngine::GetInstance()->CallFunction(m_create_fun_id,"");
		m_create_fun_id = 0;
	}
}

void UIListViewEx::StartAutoScrollToDestination(const Vec2& des, float timeInSec, bool attenuated)
{
	ScrollView::startAutoScrollToDestination(des, timeInSec, attenuated);
}

void UIListViewEx::StartAutoScroll(const Vec2& des, float timeInSec, bool attenuated)
{
	ScrollView::startAutoScroll(des, timeInSec, attenuated);
}

void UIListViewEx::OnTouchEvent(Ref *pSender, ScrollView::EventType type)
{
	LuaEngine::GetInstance()->CallFunction(m_scroll_fun_id,"");
}

float UIListViewEx::GetScrollPercent()
{
	float x =  this->GetInnerContainerPos().x;
	float y =  this->GetInnerContainerPos().y;
	switch(this->getDirection())
	{
		case ScrollView::Direction::VERTICAL: // vertical
		{
			float minY = _contentSize.height - _innerContainer->getContentSize().height;
			float h = - minY;
			if(minY !=0)
			{
				scroll_percent = 100 -(y)*100.0f/minY;
			}else
			{
				scroll_percent = 100;
			}
			break;
		}
		case ScrollView::Direction::HORIZONTAL: 
		{
			float w = _innerContainer->getContentSize().width - _contentSize.width;
			if(w !=0)

			{
				scroll_percent = -x*100.0f/w;
			}else
			{
				scroll_percent = 100;
			}
			break;
		}
		default:
			break;
	}
	return scroll_percent;
}

cocos2d::ui::Widget* UIListViewEx::createCloneInstance()
{
	return UIListViewEx::create();
}

void UIListViewEx::SetDirection(int dir)
{
	ListView::setDirection((Direction)dir);
}

// 添加获取条目数量的函数 add by swordman sue 2016-9-26
int UIListViewEx::GetItemNum()
{
	return _items.size();
}
// end by swordman sue