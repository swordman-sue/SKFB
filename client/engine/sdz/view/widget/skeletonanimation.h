#ifndef SKELETON_ANIMATION_H
#define SKELETON_ANIMATION_H

#include "zqcommon/typedef.h"
#include "spine/spine-cocos2dx.h"
#include <map>

class SkeletonData;

class SkeletonAnimationEx : public spine::SkeletonAnimation
{
public:
	struct EventInfo
	{
		EventInfo()
		:event_type(0)
		, int_value(0)
		, float_value(0)
		, string_value(" ")
		{}
		std::string animation_name;
		unsigned int event_type;
		std::string event_name;
		int int_value;
		float float_value;
		std::string string_value;
	};
	typedef std::vector<EventInfo> EventInfoList;
public:;
	   static SkeletonAnimationEx* CreateWithFile(const char* skeletonDataFile, const char* atlasFile);
	   static SkeletonAnimationEx* CreateWithFile(const char* skeletonZipFile);
	   static void SetSkeletonFps(float new_fps){ fps_time = new_fps; }

	   SkeletonAnimationEx(SkeletonData *skeleton_data);
	   ~SkeletonAnimationEx();

	   virtual void update(float delta_time);
	   virtual void onEnter();
	   virtual void onAnimationStateEvent(int trackIndex, spEventType type, spEvent* event, int loopCount);
public:
	//�滻��������
	void ResetSkeleton(const char* skeletonDataFile, const char* atlasFile);
	//�滻������Դssdz
	void ResetSkeleton(const char* ssdz_file);
	//���õ�ǰ���Ŷ���
	void SetAnimation(int trackIndex, const char* name, bool loop);
	//��Ӳ��Ŷ���
	void AddAnimation(int trackIndex, const char* name, bool loop, float delay);
	//���ö����л���Ļ��
	void SetMix(const char* fromAnimation, const char* toAnimation, float duration);
	//���ò����ٶ�
	void SetTimeScale(float var);
	//���ö���
	void SetToSetupPose();
	//�����Ƿ����
	void SetHighLight(bool high_light);
	//�����Ƿ�ҶȻ�
	void SetGray(bool graying);
	//���û����ɫ
	void SetColor(unsigned char r, unsigned char g, unsigned char b);
	//���÷�תX
	void SetFlipX(bool flip_x);
	//���÷�תY
	void SetFlipY(bool flip_y);
	//��ȡX��ת
	bool IsFlipX(){ return m_flip_x; }
	bool IsFlipY(){ return m_flip_y; }
	//��ͣ����
	void StopAnimation(){ m_run_animation = false; }
	//�ָ�����
	void ResumeAnimation(){ m_run_animation = true; }
	//�����Ƿ���ͣ
	void SetRunAnimationEx(bool var){ m_run_animation_ex = var; }
	//�����Ƿ����
	bool IsAnimationExist(const char* name);
	//��ȡ��ǰ���ڲ��ŵĶ���
	std::string GetPlayAnimationName();
	//�@ȡ��ǰ���ڲ��ńӮ��ĕr�g
	float GetPlayAnimationTime();
	//ǿ�ƶ�λ�������Ĳ���ʱ��
	void SetToPlayTime(float play_time);
	void SetSkin(const char* name);
	void SetAttachment(const char* slotName, const char* attachmentName);
	float GetWidth();
	float GetHeight();
	float GetOriginX();
	float GetOriginY();

	//���ö��������¼��Ļص�����
	void SetAnimationListener(GameType::UINT32 animation_listener_fun_id);
	//����ص�
	void ClearAnimationListener();
	//��ù��ؽڵ�
	cocos2d::Node* GetNodeBySlotName(const char* name);
	void SetSlotNodeOffSet(float x, float y);

	void UpdateEvent();
public:
	static float fps_time;

private:
	EventInfoList m_callback_list;

	//���ù�������
	void ResetSkeletonData(SkeletonData*);

private:
	bool						m_run_animation_ex;
	bool						m_run_animation;
	bool						m_flip_x, m_flip_y;
	bool						m_highlight_enable;
	bool						m_grey_enable;
	SkeletonData				*m_skeleton_data;
	spAnimationStateData		*m_sp_animation_state_data;
	GameType::UINT32			m_animation_listener_fun_id;
	float						m_delta_time;
	cocos2d::GLProgram			*m_shader_program;
	bool						m_state_dirty;
};

#endif
