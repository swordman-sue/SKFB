#ifndef __UI_WIDGET_FATORY_H__
#define __UI_WIDGET_FATORY_H__

#include <string>
#include <map>
#include "platform/CCPlatformMacros.h"
#include "base/CCRef.h"

NS_CC_BEGIN

typedef Ref* (*UI_Create_Func)(void);

// 自定义UI工厂
class CC_DLL UIWidgetFactory
{
public:
	static UIWidgetFactory* GetInstance();
	void AddCreateRedirectFunc(const std::string& class_name, UI_Create_Func create_func);

	Ref* CreateUIWidget(const std::string&);

private:
	UIWidgetFactory();

private:
	std::map<std::string, UI_Create_Func>	m_widget_create_func_map;
};

NS_CC_END

#endif
