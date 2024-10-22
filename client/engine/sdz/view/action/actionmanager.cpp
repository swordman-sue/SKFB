#include "actionmanager.h"
#include "luaengine/luaengine.h"

ActionCallBack::ActionCallBack()
	:m_fun_id(0){}

void ActionCallBack::RunCallBack()
{
	if(m_fun_id != 0)
	{
		LuaEngine::GetInstance()->CallFunction(m_fun_id, "");
		//LuaEngine::GetInstance()->DelLuaFuncID(m_fun_id);
	}
}

ActionCallBack* ActionCallBack::Node()
{
	ActionCallBack *call_back = new ActionCallBack();

	if(call_back)
	{
		call_back->autorelease();
	}

	return call_back;
}

ActionManager::ActionManager()
{}

void ActionManager::MoveTo(cocos2d::Node *node, float time, float x, float y)
{
	MoveTo(node, time, x, y, 0);
}

void ActionManager::MoveTo(cocos2d::Node *node, float time, float x, float y, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *move = cocos2d::MoveTo::create(time, cocos2d::Vec2(x, y));
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *move_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *move_action = cocos2d::Sequence::create(move, move_func, NULL);
	node->runAction(move_action);
}

void ActionManager::MoveBy(cocos2d::Node *node, float time, float x, float y)
{
	MoveBy(node, time, x, y, 0);
}

void ActionManager::MoveBy(cocos2d::Node *node, float time, float x, float y, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *move = cocos2d::MoveBy::create(time, cocos2d::Vec2(x, y));
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *move_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *move_action = cocos2d::Sequence::create(move, move_func, NULL);
	node->runAction(move_action);
}

void ActionManager::JumpTo(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency)
{
	JumpTo(node, time, x, y, h, frequency, 0);
}

void ActionManager::JumpTo(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *jump = cocos2d::JumpTo::create(time, cocos2d::Vec2(x, y), h, frequency);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *jump_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *jump_action = cocos2d::Sequence::create(jump, jump_func, NULL);
	node->runAction(jump_action);
}

void ActionManager::JumpBy(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency)
{
	JumpBy(node, time, x, y, h, frequency, 0);
}

void ActionManager::JumpBy(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *jump = cocos2d::JumpBy::create(time, cocos2d::Vec2(x, y), h, frequency);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *jump_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *jump_action = cocos2d::Sequence::create(jump, jump_func, NULL);
	node->runAction(jump_action);
}

void ActionManager::RotateBy(cocos2d::CCNode *node, float time, float angle)
{
	RotateBy(node, time, angle, 0);
}

void ActionManager::RotateBy(cocos2d::CCNode *node, float time, float angle, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *rotate = cocos2d::RotateBy::create(time, angle);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *rotate_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *rotate_action = cocos2d::Sequence::create(rotate, rotate_func, NULL);
	node->runAction(rotate_action);
}

void ActionManager::RotateTo(cocos2d::CCNode *node, float time, float angle)
{
	RotateTo(node, time, angle, 0);
}

void ActionManager::RotateTo(cocos2d::CCNode *node, float time, float angle, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *rotate = cocos2d::RotateTo::create(time, angle);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *rotate_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *rotate_action = cocos2d::Sequence::create(rotate, rotate_func, NULL);
	node->runAction(rotate_action);
}

void ActionManager::FadeOut(cocos2d::CCNode *node, float time)
{
	FadeOut(node, time, 0);
}

//淡出效果
void ActionManager::FadeOut(cocos2d::CCNode *node, float time, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *fade_out = cocos2d::FadeOut::create(time);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *fade_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *fade_action = cocos2d::Sequence::create(fade_out, fade_func, NULL);
	node->runAction(fade_action);
}

void ActionManager::FadeIn(cocos2d::CCNode *node, float time)
{
	FadeIn(node, time, 0);
}

//淡入效果
void ActionManager::FadeIn(cocos2d::CCNode *node, float time, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *fade_in = cocos2d::FadeIn::create(time);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *fade_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *fade_action = cocos2d::Sequence::create(fade_in, fade_func, NULL);
	node->runAction(fade_action);
}

void ActionManager::FadeTo(cocos2d::CCNode *node, GLubyte opacity,  float time)
{
	FadeTo(node, opacity, time, 0);
}

//淡入效果
void ActionManager::FadeTo(cocos2d::CCNode *node, GLubyte opacity, float time, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *fade_in = cocos2d::FadeTo::create(time, opacity);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *fade_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *fade_action = cocos2d::Sequence::create(fade_in, fade_func, NULL);
	node->runAction(fade_action);
}

//放大缩小
void ActionManager::ScaleTo(cocos2d::CCNode *node, float time, float factor)
{
	ScaleTo(node, time, factor, 0);
}

void ActionManager::ScaleTo(cocos2d::CCNode *node, float time, float factor, ZQ::UINT32 call_back_id)
{
	cocos2d::ActionInterval *scale_to = cocos2d::ScaleTo::create(time, factor);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *scale_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *scale_action = cocos2d::Sequence::create(scale_to, scale_func, NULL);
	node->runAction(scale_action);
}

//放大缩小XY
void ActionManager::ScaleToXY(cocos2d::CCNode *node, float time, float factor_x, float factor_y)
{
	ScaleToXY(node, time, factor_x, factor_y,0);
}

void ActionManager::ScaleToXY(cocos2d::CCNode *node, float time, float factor_x,float factor_y, ZQ::UINT32 call_back_id)
{
	cocos2d::ActionInterval *scale_to = cocos2d::ScaleTo::create(time, factor_x, factor_y);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *scale_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *scale_action = cocos2d::Sequence::create(scale_to, scale_func, NULL);
	node->runAction(scale_action);
}

//SkewBy
void ActionManager::SkewByXY(cocos2d::CCNode *node, float time, float factor_x, float factor_y)
{
	SkewByXY(node, time, factor_x, factor_y, 0);
}

void ActionManager::SkewByXY(cocos2d::CCNode *node, float time, float factor_x,float factor_y, ZQ::UINT32 call_back_id)
{
	cocos2d::ActionInterval *skew_by = cocos2d::SkewBy::create(time, factor_x, factor_y);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *skew_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *skew_action = cocos2d::Sequence::create(skew_by, skew_func, NULL);
	node->runAction(skew_action);
}

//SkewTo
void ActionManager::SkewToXY(cocos2d::CCNode *node, float time, float factor_x, float factor_y)
{
	SkewToXY(node, time, factor_x, factor_y, 0);
}

void ActionManager::SkewToXY(cocos2d::CCNode *node, float time, float factor_x,float factor_y, ZQ::UINT32 call_back_id)
{
	cocos2d::ActionInterval *skew_to = cocos2d::SkewTo::create(time, factor_x, factor_y);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *skew_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	cocos2d::FiniteTimeAction *skew_action = cocos2d::Sequence::create(skew_to, skew_func, NULL);
	node->runAction(skew_action);
}


//左右或者上下移动
void ActionManager::ShakeLoop(cocos2d::CCNode *node, float time, float factor, float angle)
{	
	ShakeLoop(node, time, factor, angle, 0);
}

void ActionManager::ShakeLoop(cocos2d::CCNode *node, float time, float factor, float angle, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *callback_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));

	cocos2d::FiniteTimeAction *action1 = cocos2d::ScaleTo::create(time * 2, (1 + factor));
	cocos2d::FiniteTimeAction *action2 = cocos2d::ScaleTo::create(time, (1 - factor));
	cocos2d::FiniteTimeAction *action3 = cocos2d::RotateTo::create(time, angle);
	cocos2d::FiniteTimeAction *action4 = cocos2d::RotateTo::create(time, -angle);
	cocos2d::FiniteTimeAction *action5 = cocos2d::RotateTo::create(time, 0.0f);

	cocos2d::FiniteTimeAction *sequence1 = cocos2d::Sequence::create(action3, action4, action3, action4, action3, action4, action3, action4, action5, callback_func, NULL);
	node->runAction(cocos2d::RepeatForever::create(static_cast<cocos2d::Sequence *>(cocos2d::Sequence::create(action1, sequence1, action2, NULL))));
}

void ActionManager::Jump(cocos2d::CCNode *node, float time, float h, float factor, ZQ::UINT32 count)
{
	Jump(node, time, h, factor, count, 0);
}

void ActionManager::Jump(cocos2d::CCNode *node, float time, float h, float factor, ZQ::UINT32 count, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::Vec2 pt = cocos2d::Vec2(0, 0);

	cocos2d::FiniteTimeAction *pNow;
	cocos2d::FiniteTimeAction *pPrev = cocos2d::JumpBy::create(time, pt, h, 1);

	for(int i = 1; i < (int)count; ++i)
	{
		time *= factor;
		h *= factor;

		pNow = cocos2d::JumpBy::create(time, pt, h, 1);
		pPrev = cocos2d::Sequence::createWithTwoActions(pPrev, pNow);
	}

	if(call_back_id != 0)
	{
		ActionCallBack *call_back = ActionCallBack::Node();
		call_back->SetLuaCallBackFun(call_back_id);
		cocos2d::CallFunc *callback_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
		pPrev = cocos2d::Sequence::createWithTwoActions(pPrev, callback_func);
	}

	node->runAction(cocos2d::RepeatForever::create(static_cast<cocos2d::Sequence *>(pPrev)));
}

//延迟执行
void ActionManager::DelayTo(cocos2d::CCNode *node, float time, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ActionInterval *delay_interval = cocos2d::DelayTime::create(time);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *fade_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	node->runAction(cocos2d::Sequence::createWithTwoActions(delay_interval, fade_func));
}

void ActionManager::Rollover(cocos2d::CCNode *in, cocos2d::CCNode *out, float time, ZQ::UINT32 call_back_id)
{
	if (NULL == in || NULL == out)
	{
		return;
	}

	cocos2d::ActionInterval* openAnimIn = (cocos2d::ActionInterval*)cocos2d::Sequence::create(cocos2d::DelayTime::create(time / 2), cocos2d::Show::create(), cocos2d::OrbitCamera::create(time / 2, 1, 0, 270, 90, 0, 0), NULL);
	cocos2d::ActionInterval* openAnimOut = (cocos2d::ActionInterval *)cocos2d::Sequence::create(cocos2d::OrbitCamera::create(time / 2, 1, 0, 0, 90, 0, 0), cocos2d::Hide::create(), cocos2d::DelayTime::create(time / 2), NULL);

	if (NULL != call_back_id)
	{
		ActionCallBack *call_back = ActionCallBack::Node();
		call_back->SetLuaCallBackFun(call_back_id);
		cocos2d::CallFunc *callback_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
		openAnimIn = cocos2d::Sequence::createWithTwoActions(openAnimIn, callback_func);
	}

	in->stopAllActions();
	out->stopAllActions();
	in->runAction(openAnimIn);
	out->runAction(openAnimOut);
}

//执行进度
void ActionManager::ProgressTo(cocos2d::CCNode *node, float time, float percent)
{
	if(node == NULL)
		return;

	ProgressTo(node, time, percent, 0);
}

//执行进度
void ActionManager::ProgressTo(cocos2d::CCNode *node, float time, float percent, ZQ::UINT32 call_back_id)
{
	if(node == NULL)
		return;

	cocos2d::ProgressTo *progress_to = cocos2d::ProgressTo::create(time, percent);
	ActionCallBack *call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc *progress_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	node->runAction(cocos2d::Sequence::createWithTwoActions(progress_to, progress_func));
}

void ActionManager::BezierBy(cocos2d::Node *node, float time, cocos2d::Vec2 end_point, cocos2d::Vec2 ctrl_point1, cocos2d::Vec2 ctrl_point2, ZQ::UINT32 call_back_id)
{
	if (nullptr == node)
		return;

	cocos2d::ccBezierConfig c;
	c.controlPoint_1 = ctrl_point1;
	c.controlPoint_2 = ctrl_point2;
	c.endPosition = end_point;
	cocos2d::BezierBy* bezierby_action = cocos2d::BezierBy::create(time, c);

	ActionCallBack* call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc* bezierby_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	node->runAction(cocos2d::Sequence::createWithTwoActions(bezierby_action, bezierby_func));
}

void ActionManager::BezierTo(cocos2d::Node *node, float time, cocos2d::Vec2 end_point, cocos2d::Vec2 ctrl_point1, cocos2d::Vec2 ctrl_point2, ZQ::UINT32 call_back_id)
{
	if (nullptr == node)
		return;

	cocos2d::ccBezierConfig c;
	c.controlPoint_1 = ctrl_point1;
	c.controlPoint_2 = ctrl_point2;
	c.endPosition = end_point;
	cocos2d::BezierTo* bezierto_action = cocos2d::BezierTo::create(time, c);

	ActionCallBack* call_back = ActionCallBack::Node();
	call_back->SetLuaCallBackFun(call_back_id);
	cocos2d::CallFunc* bezierto_func = cocos2d::CallFunc::create(call_back, callfunc_selector(ActionCallBack::RunCallBack));
	node->runAction(cocos2d::Sequence::createWithTwoActions(bezierto_action, bezierto_func));
}

void ActionManager::StopAllActions(cocos2d::CCNode *node)
{
	if(node == NULL)
		return;

	node->stopAllActions();
}

void ActionManager::PauseAction(cocos2d::CCNode *node)
{
	if(node == NULL)
		return;

	node->pauseSchedulerAndActions();
}

void ActionManager::ResumeAction(cocos2d::CCNode *node)
{
	if(node == NULL)
		return;

	node->resumeSchedulerAndActions();
}
