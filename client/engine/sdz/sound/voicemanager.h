#ifndef VOICEMANAGER_H
#define VOICEMANAGER_H

#include "voicehelper.h"
#include "luaengine/luaengine.h"
#include <string.h>

struct MessageMem;

//音频管理器
class VoiceManager
{
public:
	static VoiceManager* GetInstance();
	static void DelInstance();
	~VoiceManager() {};

public:
	enum VoiceCODE
	{
		VOICE_SUCCESS = 0,			// 成功
		VOICE_ERROR,				// 失败
		VOICE_FILE_ERROR,			// 文件错误
		VOICE_CODE_ERROR,			// 转码错误
		VOICE_PARAMETER_ERROR,		// 参数错误
		NOT_SUPPORT_INIT_ERROR,		// 实例化错误
		VOICE_TOO_SMALL,			// 文件太小
	};
	enum FileType
	{
		AMR = 1,
		WAV,		
	};

	bool Init();
	bool Start();
	bool Update(float interval);
	bool Pause();
	bool Resume();
	bool Stop();
	bool Release();

public:
	int SaveMsgAsFile(char *voice_msg, unsigned int voice_msg_len, unsigned int voice_id, int voice_length, unsigned int lua_fun_id);

	int StartRecord(int secord);
	int StopRecord(unsigned int voice_id,unsigned int lua_func_id);		// 返回秒数

	int StartPlay(unsigned int voice_id, unsigned int lua_fun_id);
	int StopPlay(unsigned int voice_id);

	int ReleaseVoice();

	int DeleteFileByVoiceId(unsigned int voice_id,unsigned int lua_func_id);
private:
	// 通过voice id 获取文件全路径
	std::string GetFileNameByVoiceID(unsigned int voice_id,int file_type);

	VoiceManager(){};

private:
	VoiceHelper m_voice_helper;

	std::string m_voice_path;
	std::string m_tmp_amr_full_name;
	std::string m_tmp_wav_full_name;
	float m_update_interval;
};

#endif
