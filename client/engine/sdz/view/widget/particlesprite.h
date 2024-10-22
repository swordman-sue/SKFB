#ifndef MID_PARTICLE_H
#define MID_PARTICLE_H
#include "particle/particlesystem.h"
#include "luatinker/lua_tinker.h"
#include "zqcommon/typedef.h"
#include "cocos2d.h"

class ParticleSprite : cocos2d::Node
{
public:
	virtual ~ParticleSprite(void);
	static ParticleSprite* Create();
	static ParticleSprite* Create(const char* template_name);
	static ParticleSprite* CreateAsync(const char* template_name);
	//预加载粒子模板，参数t为需要加载的模板名称集合(lua table)，如：{"template_name0", "template_name1"}
	static bool PreloadTemplates(lua_tinker::table t, GameType::UINT32 lua_fun_id);						
	//删除粒子模板，用于清理模板。参数t与PreloadTemplates相同。
	static bool DeleteTemplates(lua_tinker::table t);						

private:
	ParticleSprite();

public:
	void SetCycle(bool cycle);
	void SetScale(float x,float y,float z);
	void SetFlipX(bool is_flip);
	void SetFlipY(bool is_flip);
	void SetEnabled(bool enabled);
	void SetMansualUpdate(bool is_stop);
	void SetTimeScale(float time);
	void SetFramePreSecond(int frame_num);					//设置粒子系统每秒更新的帧数,需小于60
	void SetPlayEndListener(GameType::UINT32 lua_fun_id);
	void MakeGrey(bool is_grey);

	float GetTimeScale();
	float GetSinceStartTime();
	cocos2d::Size GetContentSize();
	virtual std::string getDescription() const;

protected:
	virtual cocos2d::Node* createCloneInstance();
	virtual void copySpecialProperties(cocos2d::Node* model);

private:
	void InitWithTemplate(const char* template_name, bool is_async = false);
	void OnCycleCallback();

private:
	cocos2d::MyParticleSystem* m_particle;
	GameType::UINT32 m_play_end_listener_id;		//播放结束后调用的函数
	GameType::UINT32 m_down_fun_id;
	GameType::UINT32 m_move_fun_id;
	GameType::UINT32 m_up_fun_id;

public:
	const char* m_template_name;
};
#endif
