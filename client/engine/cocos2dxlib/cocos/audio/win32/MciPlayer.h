#ifndef _MCI_PLAYER_WIN32_H_
#define _MCI_PLAYER_WIN32_H_

#include "platform/CCStdC.h"
#include <mmsystem.h>
#include <string>
using namespace std;


namespace CocosDenshion {

class MciPlayer
{
public:
    MciPlayer();
    ~MciPlayer();

    void Close();

    /**
    @brief  Play sound file
    @param pFileName    Sound's file name,include the file path.
    @param nTimes    Play mode，default value is 1,paly once
    */
    void Open(const char* pFileName, UINT uId);

    void Play(UINT uTimes = 1);

    /**
    @brief Pause play
    */
    void Pause();

    /**
    @brief Resume play
    */
    void Resume();

    /**
    @brief Stop play
    */
    void Stop();

    /**
    @brief Replay
    */
    void Rewind();

    /**
    @brief Is Playing
    */
    bool IsPlaying();

    /**
    @brief  Get playing sound's ID
    @return Sound's ID
    */
    UINT GetSoundID();

	// 修改cocos2d支持win32下设置音乐音效音量 add by JiaMiao 2016-1-7
	// @volume value ranges from 0 .. 1000
	void Volume(UINT volume);

	// @return value ranges from 0 .. 1000
	UINT MciPlayer::Volume() const;

private:
    friend LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    void _SendGenericCommand(int nCommand, DWORD_PTR param1 = 0, DWORD_PTR parma2 = 0);

    HWND        _wnd;
    MCIDEVICEID _dev;
    UINT        _soundID;
    UINT        _times;
    bool        _playing;
	bool		_pausing;		// 修复win32下调用Resume接口会播放预加载音效的问题 add by JiaMiao 2016-4-26
    std::string strExt;
};

} // end of namespace CocosDenshion

#endif
