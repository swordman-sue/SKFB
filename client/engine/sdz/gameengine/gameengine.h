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

	// 启动时间
	inline double GetStartTimeMilli() { return m_start_time_milli; }

	// 当前时间
	double GetNowTime();
	double GetNowTimeMilli();

	//退出应用
	void CloseApp();

	//切换语言版本并重登
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

	HttpManager		*m_http_manager;			// http模块
	FileSystemEx	*m_filesystem;				// 文件系统模块
	LuaEngine		*m_lua_engine;				// lua模块
	NetAdapter		*m_netadapter;				// 网络模块

	SoundManager	*m_sound_manager;			// 音乐模块
	VoiceManager	*m_voice_manager;			// 语音模块

	Location		*m_location;
	MsgPush			*m_msgpush;
	SdkHandle       *m_sdk;
};

#endif
