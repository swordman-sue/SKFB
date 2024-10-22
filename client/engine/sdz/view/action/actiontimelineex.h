#ifndef ACTION_TIMELINE_EX_H
#define ACTION_TIMELINE_EX_H

#include "cocostudio/ActionTimeline/CCActionTimeline.h"
#include "zqcommon/typedef.h"
#include "ui/GUIDefine.h"

class ActionTimelineEx : public cocostudio::timeline::ActionTimeline
{
	//add by swordman sue 2016-12-8
	DECLARE_CLASS_GUI_INFO

public:
	~ActionTimelineEx();

	static ActionTimelineEx* create();

	//用于lua_tinker的向下搜索子类函数，add by swordman sue 2016-12-8
	virtual std::string getDescription() const override { return "ActionTimelineEx"; }

	//设置动画结束回调
	void SetAnimationEndListener(const std::string animationName, GameType::UINT32 func_id);

	//清空动画结束回调
	void ClearAnimationEndListener(const std::string animationName);

protected:
	// 添加克隆时创建新对象的接口 add by swordman sue 2016-12-8
	virtual ActionTimelineEx* createCloneInstance() const override;

private:
	void AnimationEndListener(const std::string animationName);

private:
	typedef std::map<std::string, GameType::UINT32> AnimationEndFuncMap;

	//动画结束回调表
	AnimationEndFuncMap m_endFuncMap;
};

#endif