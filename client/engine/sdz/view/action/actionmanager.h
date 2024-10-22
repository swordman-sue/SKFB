#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "zqcommon/typedef.h"
#include "view/struct.h"
#include "cocos2d.h"

//�����ص���
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

//��������
class ActionManager
{
public:
	//static ActionManager *GetInstance();
	//��һ���������޶�ʱ���������˶�һ���ľ���
	//node:Ŀ�����
	//time:�ƶ�ʱ��
	//x, y�ƶ�����
	//call_back ����������lua�Ļص�
	static void MoveBy(cocos2d::Node *node, float time, float x, float y);
	static void MoveBy(cocos2d::Node *node, float time, float x, float y, ZQ::UINT32 call_back);
	
	//��һ���������޶�ʱ���������˶���Ŀ���
	//node:Ŀ�����
	//time:�ƶ�ʱ��
	//x, y:Ŀ���
	//call_back:����������lua�Ļص�
	static void MoveTo(cocos2d::Node *node, float time, float x, float y);
	static void MoveTo(cocos2d::Node *node, float time, float x, float y, ZQ::UINT32 call_back);
	
	//��һ���������޶�ʱ������Ծ��Ŀ���
	//node:Ŀ�����
	//time:�ƶ�ʱ��
	//x, y:Ŀ���
	//h:��Ծ�ĸ߶� 
	//frequency:��Ծ�Ĵ���
	//call_back:����������lua�Ļص�
	static void JumpTo(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency);
	static void JumpTo(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency, ZQ::UINT32 call_back);

	//��һ���������޶�ʱ������Ծһ���ľ���
	//��һ���������޶�ʱ���������˶�һ���ľ���
	//node:Ŀ�����
	//time:�ƶ�ʱ��
	//x, y�ƶ�����
	//h:��Ծ�ĸ߶�
	//frequency:��Ծ�Ĵ���
	//call_back ����������lua�Ļص�
	static void JumpBy(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency);
	static void JumpBy(cocos2d::Node *node, float time, float x, float y, float h, ZQ::UINT32 frequency, ZQ::UINT32 call_back);
	
	//��һ���������޶�ʱ������תһ���ĽǶ�
	//node:Ŀ�����
	//time:�ƶ�ʱ��
	//angle:��ת�ĽǶ�
	//call_back ����������lua�Ļص�
	static void RotateBy(cocos2d::Node *node, float time, float angle);
	static void RotateBy(cocos2d::Node *node, float time, float angle, ZQ::UINT32 call_back);
	
	//��һ���������޶�ʱ������ת��ĳ���Ƕ�
	//node:Ŀ�����
	//time:�ƶ�ʱ��
	//angle:��ת��ĳһ���Ƕ�
	//call_back ����������lua�Ļص�
	static void RotateTo(cocos2d::Node *node, float time, float angle);
	static void RotateTo(cocos2d::Node *node, float time, float angle, ZQ::UINT32 call_back);
	
	//����Ч��
	//node:Ŀ�����
	//time:����ʱ��
	//call_back ����������lua�Ļص�
	static void FadeOut(cocos2d::Node *node, float time);
	static void FadeOut(cocos2d::Node *node, float time, ZQ::UINT32 call_back);
	
	//����Ч��
	//node:Ŀ�����
	//time:����ʱ��
	//call_back ����������lua�Ļص�
	static void FadeIn(cocos2d::Node *node, float time);
	static void FadeIn(cocos2d::Node *node, float time, ZQ::UINT32 call_back);

	//����͸����
	//node:Ŀ�����
	//time:����ʱ��ʱ��
	//opacity: ��������͸����
	//call_back ����������lua�Ļص�
	static void FadeTo(cocos2d::Node *node, GLubyte opacity, float time);
	static void FadeTo(cocos2d::Node *node, GLubyte opacity, float time, ZQ::UINT32 call_back);

	//�Ŵ���С
	//node:Ŀ�����
	//time:����ʱ��ʱ��
	//factor:�仯�ı���
	//call_back ����������lua�Ļص�
	static void ScaleTo(cocos2d::Node *node, float time, float factor);
	static void ScaleTo(cocos2d::Node *node, float time, float factor, ZQ::UINT32 call_back);

	//�Ŵ���С
	//node:Ŀ�����
	//time:����ʱ��ʱ��
	//factor_x:�仯x�ı���
	//factor_y:�仯y�ı���
	//call_back ����������lua�Ļص�
	static void ScaleToXY(cocos2d::Node *node, float time, float factor_x, float factor_y);
	static void ScaleToXY(cocos2d::Node *node, float time, float factor_x, float factor_y, ZQ::UINT32 call_back_id);

	//���һ��������ƶ�
	static void ShakeLoop(cocos2d::Node *node, float time, float factor, float angle);
	static void ShakeLoop(cocos2d::Node *node, float time, float factor, float angle, ZQ::UINT32 call_back);

	//��Ծ
	static void Jump(cocos2d::Node *node, float time, float h, float factor, ZQ::UINT32 count);
	static void Jump(cocos2d::Node *node, float time, float h, float factor, ZQ::UINT32 count, ZQ::UINT32 call_back);

	//�ӳ�ִ��
	static void DelayTo(cocos2d::Node *node, float time, ZQ::UINT32 call_back);

	//in ����
	//out ����
	//time ��תʱ��
	static void Rollover(cocos2d::Node *in, cocos2d::CCNode *out, float time, ZQ::UINT32 call_back_id);

	//ִ�н���
	//time ִ��ʱ��
	//percent Ŀ�����
	static void ProgressTo(cocos2d::Node *node, float time, float percent);
	static void ProgressTo(cocos2d::Node *node, float time, float percent, ZQ::UINT32 call_back);

	static void SkewByXY(cocos2d::Node *node, float time, float factor_x, float factor_y);
	static void SkewByXY(cocos2d::Node *node, float time, float factor_x, float factor_y, ZQ::UINT32 call_back_id);

	static void SkewToXY(cocos2d::Node *node, float time, float factor_x, float factor_y);
	static void SkewToXY(cocos2d::Node *node, float time, float factor_x, float factor_y, ZQ::UINT32 call_back_id);

	//����������
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