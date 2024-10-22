#ifndef UI_SCROLLVIEWEX_H
#define UI_SCROLLVIEWEX_H

#include "ui/UIWidget.h"
#include "ui/UIScrollView.h"
#include "zqcommon/typedef.h"

class UIScrollViewEx: public cocos2d::ui::ScrollView
{
	DECLARE_CLASS_GUI_INFO
public:
	static UIScrollViewEx* create();

public:
	virtual void onEnter();
	virtual void onExit();

	void SetEventListener(GameType::UINT32 scroll_fun_id);
	void SetInnerContainerSize(float width, float height);
	void SetScrollDirection(int dir);
	void SetBackGroundColor(GameType::UINT32 red, GameType::UINT32 green, GameType::UINT32 blue);
    void ScrollToBottom(float time, bool attenuated);
    void ScrollToTop(float time, bool attenuated);
    void ScrollToLeft(float time, bool attenuated);
    void ScrollToRight(float time, bool attenuated);
	void SetScrollBarEnabled(bool var);
    void SetScrollVerticalVolume(float volume, float time, bool attenuated);
    void SetScrollHorizontalVolume(float volume, float time, bool attenuated);
	float GetHorizontalScrollVolume();
	float GetVerticalScrollVolume();
	int GetScrollPercentVertical();
	int GetScrollPercentHorizontal();
	void SetLayoutType(int type);
	void StartAutoScrollToDestination(const cocos2d::Vec2& des, float timeInSec, bool attenuated);
	void MoveInnerContainer(const cocos2d::Vec2& deltaMove, bool canStartBounceBack);

	virtual std::string getDescription() const { return "UIScrollViewEx"; }

protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
	UIScrollViewEx();
	void OnTouchEvent(cocos2d::Ref *pSender, ScrollView::EventType type);

private:
	GameType::UINT32 m_scroll_fun_id;
};

#endif

