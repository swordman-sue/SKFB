#ifndef NODE_UNITY_H
#define NODE_UNITY_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"

//游戏控件的公用函数库
class NodeUnity
{
public:

	//获取某节点的世界坐标
	static COCOPoint GetWorldPosition(cocos2d::Node *node);
	//将世界坐标转换成某节点里的坐标
	static COCOPoint ConvertToNodePosition(cocos2d::Node *node, COCOPoint point);
	//将某节点里的坐标转换成世界坐标
	static COCOPoint ConvertToWorldPosition(cocos2d::Node *node, COCOPoint point);
	//将某节点里的坐标转换成窗口坐标
	static COCOPoint ConvertToUISpace(cocos2d::Node *node, COCOPoint point);

	/*------------属性---------------*/
	
	//获取节点在父节点的位置
	static void SetPosition(cocos2d::Node *node, float x, float y);
	static COCOPoint GetPosition(cocos2d::Node *node);
	
	//获取描点
	static void SetAnchorPoint(cocos2d::Node *node, float x, float y);
	static COCOPoint GetAnchorPoint(cocos2d::Node *node);
	
	//x轴缩放倍数
	static void SetScaleXFactor(cocos2d::Node *node, float scale_x);
	static float GetScaleXFactor(cocos2d::Node *node);

	//y轴缩放倍数
	static void SetScaleYFactor(cocos2d::Node *node, float scale_y);
	static float GetScaleYFactor(cocos2d::Node *node);
	
	//x-y轴同等缩放倍数
	static void SetScaleFactor(cocos2d::Node *node, float scale);
	static float GetScaleFactor(cocos2d::Node *node);

	//获取设置宽高
	static void SetWidth(cocos2d::Node *node, float width);
	static void SetHeight(cocos2d::Node *node, float height);
	static float GetWidth(cocos2d::Node *node);
	static float GetHeight(cocos2d::Node *node);
	
	//旋转
	static void SetRotation(cocos2d::Node *node, float degree);
	static float GetRotation(cocos2d::Node *node);
	
	//是否可见
	static bool GetVisible(cocos2d::Node *node);
	static void SetVisible(cocos2d::Node *node, bool visible);

	//是否可见
	static ZQ::UINT32 GetZOrder(cocos2d::Node *node);
	static void SetZOrder(cocos2d::Node *node, ZQ::UINT32 order);

	//增加节点计数
	static ZQ::UINT32 Retain(cocos2d::Node *node);
	//减少节点计数
	static ZQ::UINT32 Release(cocos2d::Node *node);

	//设置灰度化
	static void SetGrayShader(cocos2d::Node *node, bool grey);
	
	//设置高亮
	static void SetBrightShader(cocos2d::Node *node, bool bright);
	//调整高亮数值 0.0-2.0
	static void SetBrightValue(cocos2d::Node *node, float bright_value);
	//获取高亮数值
	static float GetBrightValue(cocos2d::Node *node);
	
	//暂停
	static void Pause(cocos2d::Node *node);
	
	//恢复
	static void Resume(cocos2d::Node *node);

	//两个节点是否相同
	static bool IsNodeEqual(cocos2d::Node *node1, cocos2d::Node *node2);
};

#endif