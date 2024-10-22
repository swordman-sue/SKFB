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
	//Ԥ��������ģ�壬����tΪ��Ҫ���ص�ģ�����Ƽ���(lua table)���磺{"template_name0", "template_name1"}
	static bool PreloadTemplates(lua_tinker::table t, GameType::UINT32 lua_fun_id);						
	//ɾ������ģ�壬��������ģ�塣����t��PreloadTemplates��ͬ��
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
	void SetFramePreSecond(int frame_num);					//��������ϵͳÿ����µ�֡��,��С��60
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
	GameType::UINT32 m_play_end_listener_id;		//���Ž�������õĺ���
	GameType::UINT32 m_down_fun_id;
	GameType::UINT32 m_move_fun_id;
	GameType::UINT32 m_up_fun_id;

public:
	const char* m_template_name;
};
#endif
