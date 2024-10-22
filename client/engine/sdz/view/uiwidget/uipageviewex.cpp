#include "uipageviewex.h"
#include "uilayoutex.h"
#include "luaengine/luaengine.h"

using namespace cocos2d;
using namespace cocos2d::ui;
IMPLEMENT_CLASS_GUI_INFO(UIPageViewEx)

UIPageViewEx::UIPageViewEx() 
:m_page_fun_id(0)
{
}

UIPageViewEx::~UIPageViewEx()
{
	LuaEngine::GetInstance()->DelLuaFuncID(m_page_fun_id);
}

UIPageViewEx* UIPageViewEx::create()
{
	UIPageViewEx* widget = new UIPageViewEx();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}
void UIPageViewEx::AddWidgetToPage(Widget* widget, int pageIdx, bool forceCreate)
{
	PageView::addWidgetToPage(widget,pageIdx,forceCreate);
}
void UIPageViewEx::AddPage(UILayoutEx* page)
{
	PageView::addPage(page);
}
void UIPageViewEx::InsertPage(UILayoutEx* page, int idx)
{
	PageView::insertPage(page,idx);
}
void UIPageViewEx::RemovePage(UILayoutEx* page)
{
	PageView::removePage(page);
}
void UIPageViewEx::RemovePageAtIndex(int index)
{
	PageView::removePageAtIndex(index);
}
void UIPageViewEx::RemoveAllPages()
{
	PageView::removeAllPages();
}
void UIPageViewEx::ScrollToPage(int idx)
{
	PageView::scrollToPage(idx);
}
void UIPageViewEx::ScrollToPageByTime(int idx, float time)
{
	ListView::scrollToItem(idx, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE,time);
}
int UIPageViewEx::GetCurPageIndex() const
{
	return PageView::getCurPageIndex();
}

UILayoutEx* UIPageViewEx::GetCurPage()
{
	int page_index = PageView::getCurPageIndex();
	return dynamic_cast<UILayoutEx*>(PageView::getPage(page_index));
}

UILayoutEx* UIPageViewEx::GetPage(int index)
{
	return dynamic_cast<UILayoutEx*>(PageView::getPage(index));
}

UILayoutEx* UIPageViewEx::CreatePage()
{
	UILayoutEx* newPage = UILayoutEx::create();
	newPage->setContentSize(getContentSize());
	return newPage;
}

void UIPageViewEx::SetPageSize(float width, float height)
{
	cocos2d::ui::Layout::setContentSize(Size(width, height));
}

int UIPageViewEx::GetPageNum()
{
	return getItems().size();
}

void UIPageViewEx::AddEventListener(GameType::UINT32 page_fun_id)
{
	 m_page_fun_id = page_fun_id;
	 addEventListener(CC_CALLBACK_2(UIPageViewEx::OnTouchEvent, this));
}

void UIPageViewEx::OnTouchEvent(Ref *pSender, PageView::EventType type)
{
	LuaEngine::GetInstance()->CallFunction(m_page_fun_id,"i",type);
}

cocos2d::ui::Widget* UIPageViewEx::createCloneInstance()
{
	return UIPageViewEx::create();
}

std::string UIPageViewEx::getDescription() const
{
	return "UIPageViewEx";
}
