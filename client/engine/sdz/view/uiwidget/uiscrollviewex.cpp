#include "uiscrollviewex.h"
#include "luaengine/luaengine.h"
#include <algorithm>

using namespace cocos2d;
using namespace cocos2d::ui;
using std::min;
using std::max;
IMPLEMENT_CLASS_GUI_INFO(UIScrollViewEx)

UIScrollViewEx::UIScrollViewEx():
m_scroll_fun_id(0)
{}

void UIScrollViewEx::onEnter()
{
	ScrollView::onEnter();
	ScrollView::addEventListener(CC_CALLBACK_2(UIScrollViewEx::OnTouchEvent, this));
	ScrollView::setTouchEnabled(true);
}

void UIScrollViewEx::onExit()
{
	ScrollView::onExit();
	if(m_scroll_fun_id != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_scroll_fun_id);
	}
}

UIScrollViewEx* UIScrollViewEx::create()
{
	UIScrollViewEx* widget = new UIScrollViewEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

void UIScrollViewEx::StartAutoScrollToDestination(const Vec2& des, float timeInSec, bool attenuated)
{
	ScrollView::startAutoScrollToDestination(des, timeInSec, attenuated);
}

void UIScrollViewEx::SetInnerContainerSize(float w,float h)
{
	ScrollView::setInnerContainerSize(cocos2d::Size(w,h));
}

void UIScrollViewEx::SetEventListener(GameType::UINT32 scroll_fun_id)
{
	if(m_scroll_fun_id != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_scroll_fun_id);
	}

	m_scroll_fun_id = scroll_fun_id;
}

void UIScrollViewEx::SetScrollDirection(int dir)
{
	ScrollView::setDirection((Direction)dir);
}

void UIScrollViewEx::OnTouchEvent(cocos2d::Ref *pSender, ScrollView::EventType type)
{
	LuaEngine::GetInstance()->CallFunction(m_scroll_fun_id, "");
}

void UIScrollViewEx::ScrollToBottom(float time, bool attenuated)
{
	ScrollView::scrollToBottom(time, attenuated);
}

void UIScrollViewEx::ScrollToTop(float time, bool attenuated)
{
	ScrollView::scrollToTop(time, attenuated);
}

void UIScrollViewEx::ScrollToLeft(float time, bool attenuated)
{
	ScrollView::scrollToLeft(time, attenuated);
}

void UIScrollViewEx::ScrollToRight(float time, bool attenuated)
{
	ScrollView::scrollToRight(time, attenuated);
}

void UIScrollViewEx::SetBackGroundColor(GameType::UINT32 red, GameType::UINT32 green, GameType::UINT32 blue)
{
	ScrollView::setBackGroundColor(cocos2d::Color3B(red, green, blue));
}

void UIScrollViewEx::SetScrollBarEnabled(bool var)
{
	ScrollView::setScrollBarEnabled(var);
}

void UIScrollViewEx::SetScrollVerticalVolume(float volume, float time, bool attenuated)
{
	volume = min(0.0f, volume);
	volume = max(_contentSize.height - _innerContainer->getSize().height, volume);
	ScrollView::startAutoScrollToDestination(cocos2d::Point(_innerContainer->getPosition().x, volume), time, attenuated);
}

void UIScrollViewEx::SetScrollHorizontalVolume(float volume, float time, bool attenuated)
{
	volume = min(0.0f, volume);
	volume = max(_contentSize.width - _innerContainer->getSize().width, volume);
	ScrollView::startAutoScrollToDestination(cocos2d::Point(volume, _innerContainer->getPosition().y), time, attenuated);
}

float UIScrollViewEx::GetHorizontalScrollVolume()
{
	return _innerContainer->getPosition().x;
}

float UIScrollViewEx::GetVerticalScrollVolume()
{
	return _innerContainer->getPosition().y;
}

int UIScrollViewEx::GetScrollPercentVertical()
{
	float minY = ScrollView::getContentSize().height - ScrollView::getInnerContainer()->getContentSize().height;
	float offset_y = ScrollView::getInnerContainer()->getPositionY();
	int percent = (int)offset_y / minY * 100;
	return 100 - percent;
}

int UIScrollViewEx::GetScrollPercentHorizontal()
{
	return 0;
}

void UIScrollViewEx::SetLayoutType(int type)
{
	ScrollView::setLayoutType((Layout::Type)type);
}

cocos2d::ui::Widget* UIScrollViewEx::createCloneInstance()
{
	return UIScrollViewEx::create();
}

void UIScrollViewEx::MoveInnerContainer(const cocos2d::Vec2& deltaMove, bool canStartBounceBack)
{
	moveInnerContainer(deltaMove, canStartBounceBack);
}