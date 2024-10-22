#ifndef WIDGET_UNITY_H
#define WIDGET_UNITY_H

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include "ui/UIHelper.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"
#include "cocos-ext.h"

//��ϷWidget�ؼ��Ĺ��ú�����
class WidgetUnity
{
public:
	//�������ֻ�ȡ������ӿؼ�
	static cocos2d::Node* GetWidgetByName(cocos2d::ui::Widget *target_widget, const std::string &widget_name);
};

#endif
