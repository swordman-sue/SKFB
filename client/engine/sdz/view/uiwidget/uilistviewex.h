#ifndef UI_LISTVIEW_H
#define UI_LISTVIEW_H

#include "ui/UIListView.h"
#include "zqcommon/typedef.h"

class UIListViewEx : public cocos2d::ui::ListView
{
	DECLARE_CLASS_GUI_INFO
public:
	~UIListViewEx();
	
public:
	static UIListViewEx* create();
	
	void AddEventListener(GameType::UINT32 scroll_fun_id);
	void AddCreateEventListener(GameType::UINT32 create_fun_id);
	
	void SetDirection(int dir);
	float GetScrollPercent();
	const cocos2d::Vec2& GetInnerContainerPos();
	
	virtual std::string getDescription() const { return "UIListViewEx"; }
	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags);
	void StartAutoScrollToDestination(const cocos2d::Vec2& des, float timeInSec, bool attenuated);
	void StartAutoScroll(const cocos2d::Vec2& des, float timeInSec, bool attenuated);

	// 添加获取条目数量的函数 add by swordman sue 2016-9-26
	int GetItemNum();
	// end by swordman sue

protected:
	virtual cocos2d::ui::Widget* createCloneInstance();

private:
	UIListViewEx();
	GameType::UINT32 m_scroll_fun_id;
	GameType::UINT32 m_create_fun_id;
	float scroll_percent;
	void OnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::ScrollView::EventType type);
};

#endif

