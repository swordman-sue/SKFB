#include "widgetunity.h"
#include "ui/UIWidget.h"

using namespace cocos2d;
using namespace cocos2d::ui;

Node* WidgetUnity::GetWidgetByName(Widget *target_widget, const std::string &widget_name)
{
	if(!target_widget || widget_name == "")
		return NULL;

	CCASSERT(dynamic_cast<Widget*>(target_widget) != NULL, "[WidgetUnity::GetWidgetByName] target_widget not a Widget");

	return cocos2d::ui::Helper::seekWidgetByName(target_widget, widget_name.c_str());
}
