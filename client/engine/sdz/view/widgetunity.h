#ifndef WIDGET_UNITY_H
#define WIDGET_UNITY_H

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UIHelper.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"
#include "cocos-ext.h"

//游戏Widget控件的公用函数库
class WidgetUnity
{
public:
	//根据名字获取对象的子控件
	static cocos2d::Node* GetWidgetByName(cocos2d::ui::Widget *target_widget, const std::string &widget_name);
};

#endif
