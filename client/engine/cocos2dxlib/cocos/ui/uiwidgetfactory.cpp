#include "uiwidgetfactory.h"

NS_CC_BEGIN

UIWidgetFactory::UIWidgetFactory()
{
}

UIWidgetFactory* UIWidgetFactory::GetInstance()
{
	static UIWidgetFactory ui_widget_factory;
	return &ui_widget_factory;
}

void UIWidgetFactory::AddCreateRedirectFunc(const std::string& class_name, UI_Create_Func create_func)
{
	m_widget_create_func_map[class_name] = create_func;
}

cocos2d::Ref* UIWidgetFactory::CreateUIWidget(const std::string& widget_name)
{
	std::map<std::string, UI_Create_Func>::iterator widget_create_func_iter = m_widget_create_func_map.find(widget_name);

	if (widget_create_func_iter == m_widget_create_func_map.end())
		return nullptr;

	return widget_create_func_iter->second();
}

NS_CC_END
