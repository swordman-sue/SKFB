#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "cocos2d.h"

class HttpManager;
class FileSystemEx;
class LuaEngine;
class NetAdapter;

class SoundManager;
class VoiceManager;

class ResManager;

class Location;
class MsgPush;
class SdkHandle;

class GameEngine : public cocos2d::PreFrameHandler
{
public:
	static GameEngine* GetInstance();
	static void DelInstance();
	~GameEngine();

private:
	GameEngine();

public:
	bool Init();
	bool Start();
	bool RunPreFrame(float time);
	bool Pause();
	bool Resume();
	bool Stop();
	bool Release();
	bool Restart();

	// ����ʱ��
	inline double GetStartTimeMilli() { return m_start_time_milli; }

	// ��ǰʱ��
	double GetNowTime();
	double GetNowTimeMilli();

	//�˳�Ӧ��
	void CloseApp();

	//�л����԰汾���ص�
	void SwitchLanguageAndRestart(const std::string& district_language);

private:
	enum ENGINESTATUS
	{
		ES_UNINIT = 0,
		ES_INITED,
		ES_STARTED,
		ES_STOPED,
		ES_RELEASED,
	};

private:
	short			m_status;
	double			m_start_time_milli;

	HttpManager		*m_http_manager;			// httpģ��
	FileSystemEx	*m_filesystem;				// �ļ�ϵͳģ��
	LuaEngine		*m_lua_engine;				// luaģ��
	NetAdapter		*m_netadapter;				// ����ģ��

	SoundManager	*m_sound_manager;			// ����ģ��
	VoiceManager	*m_voice_manager;			// ����ģ��

	Location		*m_location;
	MsgPush			*m_msgpush;
	SdkHandle       *m_sdk;
};

#endif
