#ifndef UI_TOUCHGROUPEX_H
#define UI_TOUCHGROUPEX_H

#include "2d/CCLayer.h"
#include "ui/UIWidget.h"
#include "zqcommon/typedef.h"

#define TOUCH_GROUP_PRIORITY -128

class UITouchGroupEx : public cocos2d::Layer
{
	DECLARE_CLASS_GUI_INFO
public:
	~UITouchGroupEx();
	virtual bool init();
	virtual void onEnter();
public:
	static UITouchGroupEx* create();
	void AddWidget(cocos2d::ui::Widget* widget);
	void AddWidget(cocos2d::ui::Widget* widget, int z_order);
	void AddWidget(cocos2d::ui::Widget* widget, int z_order, int tag);
	void RemoveWidget(cocos2d::ui::Widget* widget);
	cocos2d::ui::Widget* GetWidgetByName(const std::string& name);
	void SetSwallow(bool swallow);
	void SetTouchPriority(GameType::INT32 priority);
	GameType::INT32 GetTouchPriority();

	virtual std::string getDescription() const { return "UITouchGroupEx"; }
private:
	UITouchGroupEx();

private:
	bool m_swallow;
	cocos2d::ui::Widget* m_pRootWidget;
};

#endif
