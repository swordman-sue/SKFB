#ifndef SELECT_LANGUAGE_LAYER_H
#define SELECT_LANGUAGE_LAYER_H

#include <list> 
#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "view/widget/labelex.h"
#include "view/uiwidget/uicheckboxex.h"
#include "view/uiwidget/uibuttonex.h"

#define LANGUAGE_LEN 10 

using namespace cocos2d;

typedef void(*selectlanguage_callback)(std::string);

class SelectLanguageLayer : public Layer
{
public:
	SelectLanguageLayer();
	~SelectLanguageLayer();

	static SelectLanguageLayer* Create();

	void show(std::string language_list_param[], selectlanguage_callback call_back);
	void OnSelectEvent(cocos2d::Ref *pSender, cocos2d::ui::CheckBox::EventType eventType);
	void OnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	//通知php选择了哪种语言
	void NoticePhp();
	virtual void onEnter();
	virtual void onExit();

private:
	UICheckBoxEx* selectedcheckbox;
	selectlanguage_callback call_back_fun;
	UIButtonEx *surebtn;
	std::string language_list[LANGUAGE_LEN];
};

#endif
