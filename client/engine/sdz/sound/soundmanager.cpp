
#include "soundmanager.h"
#include "SimpleAudioEngine.h"
#include "filesystem/filesystemex.h"
#include "zqcommon/logutil.h"
#include "luaengine/luaengine.h"
#include "platform/CCPlatformMacros.h"
#include "cocos2d.h"

using namespace cocos2d;

static SoundManager* s_instance = nullptr;

SoundManager* SoundManager::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new SoundManager;
	}
	return s_instance;
}

void SoundManager::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
	Release();
}

void SoundManager::PreloadBGMusic(const std::string &str_filename)
{
	if("" == str_filename) return;

	std::string tempPath = FileSystemEx::GetInstance()->GetResourcePath(str_filename);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(tempPath.c_str());
}

void SoundManager::PlayBGMusic(const std::string &str_filename, bool loop)
{
	if("" == str_filename) return;

	std::string tempPath = FileSystemEx::GetInstance()->GetResourcePath(str_filename);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(tempPath.c_str(), loop);
}

void SoundManager::StopBGMusic(bool releaseData)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(releaseData);
}

void SoundManager::PauseBGMusic()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void SoundManager::ResumeBGMusic()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void SoundManager::RewindBGMusic()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->rewindBackgroundMusic();
}

bool SoundManager::IsBGMusicPlaying()
{
	return CocosDenshion::SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
}

float SoundManager::GetBGMusicVolume()
{
	return CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
}

void SoundManager::SetBGMusicVolume(float volume)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volume);
}

unsigned int SoundManager::PlayEffect(const std::string &strFileName, bool loop, float pitch, float pan, float gain)
{
	if("" == strFileName) return 0;

	std::string tempPath = FileSystemEx::GetInstance()->GetResourcePath(strFileName);
	return CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(tempPath.c_str(), loop, pitch, pan, gain);
}

void SoundManager::PauseEffect(ZQ::UINT32 sound_id)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseEffect(sound_id);
}

void SoundManager::PauseAllEffects()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

void SoundManager::ResumeEffect(ZQ::UINT32 sound_id)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeEffect(sound_id);
}

void SoundManager::ResumeAllEffects()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

void SoundManager::StopEffect(ZQ::UINT32 sound_id)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopEffect(sound_id);
}

void SoundManager::StopAllEffects()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
}

void SoundManager::PreloadEffect(const std::string &str_filename)
{
	if("" == str_filename) return;

	std::string print_load_log = LuaEngine::GetInstance()->GetConfig("PrintLoadLog");
	if (print_load_log == "true")
	{
		CCLOG("SoundManager::PreloadEffect file:%s", str_filename.c_str());
	}

	std::string tempPath = FileSystemEx::GetInstance()->GetResourcePath(str_filename);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(tempPath.c_str());
}

void SoundManager::UnloadEffect(const std::string &str_filename)
{
	if("" == str_filename) return;

	std::string tempPath = FileSystemEx::GetInstance()->GetResourcePath(str_filename);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(tempPath.c_str());
}

float SoundManager::GetEffectsVolume()
{
	return CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume();
}

void SoundManager::SetEffectsVolume(float volume)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(volume);
}

bool SoundManager::Pause()
{
    // IOS版本，进入后台再切回前台，大概率出现音效无法播放，屏蔽PauseBGMusic和ResumeBGMusic可解决
#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
	PauseBGMusic();
#endif
	PauseAllEffects();

	return true;
}

bool SoundManager::Resume()
{
    // IOS版本，进入后台再切回前台，大概率出现音效无法播放，屏蔽PauseBGMusic和ResumeBGMusic可解决
#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
    ResumeBGMusic();
#endif
	ResumeAllEffects();

	return true;
}

bool SoundManager::Release()
{	
	return true;
}

bool SoundManager::Stop()
{
	StopBGMusic(true);
	StopAllEffects();

	return true;
}






