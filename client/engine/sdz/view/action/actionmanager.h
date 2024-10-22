#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "zqcommon/typedef.h"
#include "view/struct.h"
#include "cocos2d.h"

//动作回调类
class ActionCallBack : public cocos2d::Node
{
public:
	static ActionCallBack* Node();
	void SetLuaCallBackFun(ZQ::UINT32 id){m_fun_id = id;}
	void RunCallBack();
private:
	ActionCallBack();
	ZQ::UINT32 m_fun_id;
};

//管理动作类
class ActionManager
{
public:
	//static ActionManager *GetInstance();
	//将一个对象在限定时间内匀速运动一定的距离
	//node:目标物件
	//time:移动时间
	//x, y移动距离
	//call_back 动作结束后lua的回调
	static void MoveBy(cocos2d::Node *node, float time, float x, float y);
	static void MoveBy(cocos2d::Node *node, float time, float x, float y, ZQ::UINT32 call_back);
	
	//将一个对象在限定时间内匀速运动到目标点
	//node:目标物件
	//time:移动时间
	//x, y:目标点
	//call_back:动作结束后lua的回调
	static void MoveTo(cocos2d::Node *node, float time, float x, float y);
	static void MoveTo(cocos2d::Node *node, float time, float x, float y, ZQ::UINT32 call_back);
	
	//将一个对象在限定时间内跳跃到目标点
	//node:目标物件
	//time:移动时间
	//x, y:目标点
	//h:跳跃的高度 
	//frequency:跳跃的次数
	//call_back:动作结束后lua的回调
	static void JumpTo(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency);
	static void JumpTo(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency, ZQ::UINT32 call_back);

	//将一个对象在限定时间内跳跃一定的距离
	//将一个对象在限定时间内匀速运动一定的距离
	//node:目标物件
	//time:移动时间
	//x, y移动距离
	//h:跳跃的高度
	//frequency:跳跃的次数
	//call_back 动作结束后lua的回调
	static void JumpBy(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency);
	static void JumpBy(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency, ZQ::UINT32 call_back);
	
	//将一个对象在限定时间内旋转一定的角度
	//node:目标物件
	//time:移动时间
	//angle:旋转的角度
	//call_back 动作结束后lua的回调
	static void RotateBy(cocos2d::Node *node, float time, float angle);
	static void RotateBy(cocos2d::Node *node, float time, float angle, ZQ::UINT32 call_back);
	
	//讲一个对象在限定时间内旋转到某个角度
	//node:目标物件
	//time:移动时间
	//angle:旋转到某一个角度
	//call_back 动作结束后lua的回调
	static void RotateTo(cocos2d::Node *node, float time, float angle);
	static void RotateTo(cocos2d::Node *node, float time, float angle, ZQ::UINT32 call_back);
	
	//淡出效果
	//node:目标物件
	//time:淡出时间
	//call_back 动作结束后lua的回调
	static void FadeOut(cocos2d::Node *node, float time);
	static void FadeOut(cocos2d::Node *node, float time, ZQ::UINT32 call_back);
	
	//淡入效果
	//node:目标物件
	//time:淡入时间
	//call_back 动作结束后lua的回调
	static void FadeIn(cocos2d::Node *node, float time);
	static void FadeIn(cocos2d::Node *node, float time, ZQ::UINT32 call_back);

	//渐变透明度
	//node:目标物件
	//time:渐变时间时间
	//opacity: 渐变至的透明度
	//call_back 动作结束后lua的回调
	static void FadeTo(cocos2d::Node *node, GLubyte opacity, float time);
	static void FadeTo(cocos2d::Node *node, GLubyte opacity, float time, ZQ::UINT32 call_back);

	//放大缩小
	//node:目标物件
	//time:渐变时间时间
	//factor:变化的倍数
	//call_back 动作结束后lua的回调
	static void ScaleTo(cocos2d::Node *node, float time, float factor);
	static void ScaleTo(cocos2d::Node *node, float time, float factor, ZQ::UINT32 call_back);

	//放大缩小
	//node:目标物件
	//time:渐变时间时间
	//factor_x:变化x的倍数
	//factor_y:变化y的倍数
	//call_back 动作结束后lua的回调
	static void ScaleToXY(cocos2d::Node *node, float time, float factor_x, float factor_y);
	static void ScaleToXY(cocos2d::Node *node, float time, float factor_x, float factor_y, ZQ::UINT32 call_back_id);

	//左右或者上下移动
	static void ShakeLoop(cocos2d::Node *node, float time, float factor, float angle);
	static void ShakeLoop(cocos2d::Node *node, float time, float factor, float angle, ZQ::UINT32 call_back);

	//跳跃
	static void Jump(cocos2d::Node *node, float time, float h, float factor, ZQ::UINT32 count);
	static void Jump(cocos2d::Node *node, float time, float h, float factor, ZQ::UINT32 count, ZQ::UINT32 call_back);

	//延迟执行
	static void DelayTo(cocos2d::Node *node, float time, ZQ::UINT32 call_back);

	//in 里面
	//out 背面
	//time 翻转时间
	static void Rollover(cocos2d::Node *in, cocos2d::CCNode *out, float time, ZQ::UINT32 call_back_id);

	//执行进度
	//time 执行时间
	//percent 目标进度
	static void ProgressTo(cocos2d::Node *node, float time, float percent);
	static void ProgressTo(cocos2d::Node *node, float time, float percent, ZQ::UINT32 call_back);

	static void SkewByXY(cocos2d::Node *node, float time, float factor_x, float factor_y);
	static void SkewByXY(cocos2d::Node *node, float time, float factor_x, float factor_y, ZQ::UINT32 call_back_id);

	static void SkewToXY(cocos2d::Node *node, float time, float factor_x, float factor_y);
	static void SkewToXY(cocos2d::Node *node, float time, float factor_x, float factor_y, ZQ::UINT32 call_back_id);

	//贝塞尔曲线
	static void BezierBy(cocos2d::Node *node, float time, cocos2d::Vec2 end_point, cocos2d::Vec2 ctrl_point1, cocos2d::Vec2 ctrl_point2, ZQ::UINT32 call_back_id);
	static void BezierTo(cocos2d::Node *node, float time, cocos2d::Vec2 end_point, cocos2d::Vec2 ctrl_point1, cocos2d::Vec2 ctrl_point2, ZQ::UINT32 call_back_id);

	static void StopAllActions(cocos2d::Node *node);
	static void PauseAction(cocos2d::Node *node);
	static void ResumeAction(cocos2d::Node *node);

protected:
private:
	ActionManager();
};

#endif