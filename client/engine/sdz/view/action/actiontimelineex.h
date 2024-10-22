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

	//����lua_tinker�������������ຯ����add by swordman sue 2016-12-8
	virtual std::string getDescription() const override { return "ActionTimelineEx"; }

	//���ö��������ص�
	void SetAnimationEndListener(const std::string animationName, GameType::UINT32 func_id);

	//��ն��������ص�
	void ClearAnimationEndListener(const std::string animationName);

protected:
	// ��ӿ�¡ʱ�����¶���Ľӿ� add by swordman sue 2016-12-8
	virtual ActionTimelineEx* createCloneInstance() const override;

private:
	void AnimationEndListener(const std::string animationName);

private:
	typedef std::map<std::string, GameType::UINT32> AnimationEndFuncMap;

	//���������ص���
	AnimationEndFuncMap m_endFuncMap;
};

#endif