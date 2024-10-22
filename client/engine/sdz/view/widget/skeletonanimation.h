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
	//替换骨骼数据
	void ResetSkeleton(const char* skeletonDataFile, const char* atlasFile);
	//替换骨骼资源ssdz
	void ResetSkeleton(const char* ssdz_file);
	//设置当前播放动画
	void SetAnimation(int trackIndex, const char* name, bool loop);
	//添加播放动画
	void AddAnimation(int trackIndex, const char* name, bool loop, float delay);
	//设置动画切换间的混合
	void SetMix(const char* fromAnimation, const char* toAnimation, float duration);
	//设置播放速度
	void SetTimeScale(float var);
	//重置动画
	void SetToSetupPose();
	//设置是否高亮
	void SetHighLight(bool high_light);
	//设置是否灰度化
	void SetGray(bool graying);
	//设置混合颜色
	void SetColor(unsigned char r, unsigned char g, unsigned char b);
	//设置翻转X
	void SetFlipX(bool flip_x);
	//设置翻转Y
	void SetFlipY(bool flip_y);
	//获取X翻转
	bool IsFlipX(){ return m_flip_x; }
	bool IsFlipY(){ return m_flip_y; }
	//暂停动画
	void StopAnimation(){ m_run_animation = false; }
	//恢复动画
	void ResumeAnimation(){ m_run_animation = true; }
	//设置是否暂停
	void SetRunAnimationEx(bool var){ m_run_animation_ex = var; }
	//动作是否存在
	bool IsAnimationExist(const char* name);
	//获取当前正在播放的动画
	std::string GetPlayAnimationName();
	//獲取當前正在播放動畫的時間
	float GetPlayAnimationTime();
	//强制定位到动画的播放时间
	void SetToPlayTime(float play_time);
	void SetSkin(const char* name);
	void SetAttachment(const char* slotName, const char* attachmentName);
	float GetWidth();
	float GetHeight();
	float GetOriginX();
	float GetOriginY();

	//设置动画播放事件的回调函数
	void SetAnimationListener(GameType::UINT32 animation_listener_fun_id);
	//清除回调
	void ClearAnimationListener();
	//获得挂载节点
	cocos2d::Node* GetNodeBySlotName(const char* name);
	void SetSlotNodeOffSet(float x, float y);

	void UpdateEvent();
public:
	static float fps_time;

private:
	EventInfoList m_callback_list;

	//重置骨骼数据
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
