#ifndef LUAERRORREPORT_H
#define LUAERRORREPORT_H

#include <string>
#include "ui/UIWidget.h"

class UITextEx;
class UILayoutEx;
class UIScrollViewEx;

class LuaErrorReport
{
public:
	static LuaErrorReport* GetInstance();
	void DelInstance();
	~LuaErrorReport();
	
	bool Init() { return true; }
	bool Stop();
	void Update(float elapse_time);
	void Check(const std::string& error);
	
private:
	LuaErrorReport();

	void Show(const std::string& key, const std::string& error);
	bool OpenView();
	void CloseView();

	void OnTouchBtn(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	std::string m_all_error_str;
	std::string m_pre_error_key;
	float m_error_elapse_time;

	UILayoutEx* m_view_layout;
	UITextEx* m_error_txt;
	UIScrollViewEx* m_error_txt_scv;
};

#endif