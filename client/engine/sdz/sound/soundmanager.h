#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "zqcommon/typedef.h"
#include "string"

class SoundManager
{
public:
	static SoundManager* GetInstance();
	static void DelInstance();
	~SoundManager();

private:
	SoundManager();

public:
	bool Init(){return true;};
	bool Start(){return true;};
	bool Update(){return true;};
	//引擎调用
	bool Pause();
	bool Resume();
	//删除已加载所有资源
	bool Release();
	bool Stop();

public:
	//bg音乐预加载为空函数
	void PreloadBGMusic(const std::string &str_filename);

	//background music op.
	void PlayBGMusic(const std::string &str_filename, bool loop);
	void StopBGMusic(bool releaseData);
	void PauseBGMusic();
	void ResumeBGMusic();
	void RewindBGMusic();

	bool IsBGMusicPlaying();
	
	//0.0 ~ 1.0
	float GetBGMusicVolume();
	void SetBGMusicVolume(float volume);

	//effect sounds op.

	//return soundId
	ZQ::UINT32 PlayEffect(const std::string &str_filename, bool loop, float pitch, float pan, float gain);
	void PauseEffect(ZQ::UINT32 sound_id);
	void PauseAllEffects();
	void ResumeEffect(ZQ::UINT32 sound_id);
	void ResumeAllEffects();
	void StopEffect(ZQ::UINT32 sound_id);
	void StopAllEffects();
	
	void PreloadEffect(const std::string &str_filename);
	void UnloadEffect(const std::string &str_filename);

	float GetEffectsVolume();
	void SetEffectsVolume(float volume);


};


#endif

