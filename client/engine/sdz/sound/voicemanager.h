#ifndef VOICEMANAGER_H
#define VOICEMANAGER_H

#include "voicehelper.h"
#include "luaengine/luaengine.h"
#include <string.h>

struct MessageMem;

//��Ƶ������
class VoiceManager
{
public:
	static VoiceManager* GetInstance();
	static void DelInstance();
	~VoiceManager() {};

public:
	enum VoiceCODE
	{
		VOICE_SUCCESS = 0,			// �ɹ�
		VOICE_ERROR,				// ʧ��
		VOICE_FILE_ERROR,			// �ļ�����
		VOICE_CODE_ERROR,			// ת�����
		VOICE_PARAMETER_ERROR,		// ��������
		NOT_SUPPORT_INIT_ERROR,		// ʵ��������
		VOICE_TOO_SMALL,			// �ļ�̫С
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
	int StopRecord(unsigned int voice_id,unsigned int lua_func_id);		// ��������

	int StartPlay(unsigned int voice_id, unsigned int lua_fun_id);
	int StopPlay(unsigned int voice_id);

	int ReleaseVoice();

	int DeleteFileByVoiceId(unsigned int voice_id,unsigned int lua_func_id);
private:
	// ͨ��voice id ��ȡ�ļ�ȫ·��
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
