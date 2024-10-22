#ifndef UI_PAGEVIEW_EX_H
#define UI_PAGEVIEW_EX_H

#include "ui/UIPageView.h"
#include "zqcommon/typedef.h"

class UILayoutEx;

class UIPageViewEx : public cocos2d::ui::PageView
{
	DECLARE_CLASS_GUI_INFO
public:
	~UIPageViewEx();

public:
	static UIPageViewEx* create();
	void AddWidgetToPage(cocos2d::ui::Widget* widget, int pageIdx, bool forceCreate);
	void AddPage(UILayoutEx* page);
	void InsertPage(UILayoutEx* page, int idx);
	void RemovePage(UILayoutEx* page);
	void RemovePageAtIndex(int index);
	void RemoveAllPages();
	void ScrollToPage(int idx);
	void ScrollToPageByTime(int idx,float time);
	int GetCurPageIndex() const;
	UILayoutEx* GetCurPage();
	UILayoutEx* GetPage(int index);
	UILayoutEx* CreatePage();
	void SetPageSize(float width, float height);
	int GetPageNum();
	void AddEventListener(GameType::UINT32 page_fun_id);
	virtual std::string getDescription() const;

private:
	UIPageViewEx();
	GameType::UINT32 m_page_fun_id;
	void OnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::PageView::EventType type);

protected:
	virtual cocos2d::ui::Widget* createCloneInstance();
};

#endif