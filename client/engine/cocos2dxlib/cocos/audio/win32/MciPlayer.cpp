#include "MciPlayer.h"
#include <Digitalv.h>
#include <tchar.h>
#include "platform/CCFileUtils.h"

#define WIN_CLASS_NAME        "CocosDenshionCallbackWnd"
#define BREAK_IF(cond)      if (cond) break;

namespace CocosDenshion {

static HINSTANCE s_hInstance;
static MCIERROR  s_mciError;

LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

MciPlayer::MciPlayer()
: _wnd(NULL)
, _dev(0L)
, _soundID(0)
, _times(0)
, _playing(false)
, _pausing(false)	// 修改win32下调用Resume接口会播放预加载音效的问题 add by JiaMiao 2016-4-26
, strExt("")
{
    if (! s_hInstance)
    {
        s_hInstance = GetModuleHandle( NULL );            // Grab An Instance For Our Window

        WNDCLASS  wc;        // Windows Class Structure

        // Redraw On Size, And Own DC For Window.
        wc.style          = 0;  
        wc.lpfnWndProc    = _SoundPlayProc;                    // WndProc Handles Messages
        wc.cbClsExtra     = 0;                              // No Extra Window Data
        wc.cbWndExtra     = 0;                                // No Extra Window Data
        wc.hInstance      = s_hInstance;                    // Set The Instance
        wc.hIcon          = 0;                                // Load The Default Icon
        wc.hCursor        = LoadCursor( NULL, IDC_ARROW );    // Load The Arrow Pointer
        wc.hbrBackground  = NULL;                           // No Background Required For GL
        wc.lpszMenuName   = NULL;                           // We Don't Want A Menu
        wc.lpszClassName  = _T(WIN_CLASS_NAME);                 // Set The Class Name

        if (! RegisterClass(&wc)
            && 1410 != GetLastError())
        {
            return;
        }
    }

    _wnd = CreateWindowEx(
        WS_EX_APPWINDOW,                                    // Extended Style For The Window
        _T(WIN_CLASS_NAME),                                        // Class Name
        NULL,                                        // Window Title
        WS_POPUPWINDOW,/*WS_OVERLAPPEDWINDOW*/               // Defined Window Style
        0, 0,                                                // Window Position
        0,                                                    // Window Width
        0,                                                    // Window Height
        NULL,                                                // No Parent Window
        NULL,                                                // No Menu
        s_hInstance,                                        // Instance
        NULL );
    if (_wnd)
    {
        SetWindowLongPtr(_wnd, GWLP_USERDATA, (LONG_PTR)this);
    }
}

MciPlayer::~MciPlayer()
{
    Close();
    DestroyWindow(_wnd);
}

void MciPlayer::Open(const char* pFileName, UINT uId)
{
//     WCHAR * pBuf = NULL;
    do 
    {
        BREAK_IF(! pFileName || ! _wnd);
        int nLen = (int)strlen(pFileName);
        BREAK_IF(! nLen);
//         pBuf = new WCHAR[nLen + 1];
//         BREAK_IF(! pBuf);
//         MultiByteToWideChar(CP_ACP, 0, pFileName, nLen + 1, pBuf, nLen + 1);

        strExt = cocos2d::FileUtils::getInstance()->getFileExtension(pFileName);

        Close();

        MCI_OPEN_PARMS mciOpen = {0};
        MCIERROR mciError;
		WCHAR* fileNameWideChar = new WCHAR[nLen + 1];
		BREAK_IF(! fileNameWideChar);
		MultiByteToWideChar(CP_ACP, 0, pFileName, nLen + 1, fileNameWideChar, nLen + 1);
        mciOpen.lpstrElementName = fileNameWideChar;

		//mciOpen.lpstrDeviceType = (LPCTSTR)MCI_ALL_DEVICE_ID;
		const char *deviceTypeStr = "MPEGVideo";
		int dtLen = (int)strlen(deviceTypeStr);
		WCHAR* typeStrWideChar = new WCHAR[dtLen + 1];
		MultiByteToWideChar(CP_ACP, 0, deviceTypeStr, dtLen + 1, typeStrWideChar, dtLen + 1);
		mciOpen.lpstrDeviceType = typeStrWideChar;

		mciError = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, reinterpret_cast<DWORD_PTR>(&mciOpen));
		CC_SAFE_DELETE_ARRAY(mciOpen.lpstrElementName);

        BREAK_IF(mciError);

        _dev = mciOpen.wDeviceID;
        _soundID = uId;
        _playing = false;
    } while (0);
}

void MciPlayer::Play(UINT uTimes /* = 1 */)
{
    if (! _dev)
    {
        return;
    }
    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = reinterpret_cast<DWORD_PTR>(_wnd);
    s_mciError = mciSendCommand(_dev,MCI_PLAY, MCI_FROM|MCI_NOTIFY,reinterpret_cast<DWORD_PTR>(&mciPlay));
    if (! s_mciError)
    {
        _playing = true;
		_pausing = false;		// 修复win32下调用Resume接口会播放预加载音效的问题 add by JiaMiao 2016-4-26
        _times = uTimes;
    }
}

void MciPlayer::Close()
{
    if (_playing)
    {
        Stop();
    }
    if (_dev)
    {
         _SendGenericCommand(MCI_CLOSE);
    }
    _dev = 0;
    _playing = false;
}

void MciPlayer::Pause()
{
    _SendGenericCommand(MCI_PAUSE);
	// 修复win32下调用Resume接口会播放预加载音效的问题 edit by JiaMiao 2016-4-26
	if (_playing) {
		_pausing = true;
	}
	_playing = false;
}

void MciPlayer::Resume()
{
    if (strExt == ".mid")
    {
        // midi not support MCI_RESUME, should get the position and use MCI_FROM
        MCI_STATUS_PARMS mciStatusParms;
        MCI_PLAY_PARMS   mciPlayParms;  
        mciStatusParms.dwItem = MCI_STATUS_POSITION;   
        _SendGenericCommand(MCI_STATUS, MCI_STATUS_ITEM, reinterpret_cast<DWORD_PTR>(&mciStatusParms)); // MCI_STATUS   
        mciPlayParms.dwFrom = mciStatusParms.dwReturn;  // get position  
        _SendGenericCommand(MCI_PLAY, MCI_FROM, reinterpret_cast<DWORD_PTR>(&mciPlayParms)); // MCI_FROM
    } 
    else
    {
		// 修复win32下调用Resume接口会播放预加载音效的问题 edit by JiaMiao 2016-4-26
		if (_pausing) {
			_SendGenericCommand(MCI_RESUME);
			_playing = true;
			_pausing = false;
		}
    }   
}

void MciPlayer::Stop()
{
    _SendGenericCommand(MCI_STOP);
    _playing = false;
    _times = 0;
}

void MciPlayer::Rewind()
{
    if (! _dev)
    {
        return;
    }
    mciSendCommand(_dev, MCI_SEEK, MCI_SEEK_TO_START, 0);

    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = reinterpret_cast<DWORD_PTR>(_wnd);
    _playing = mciSendCommand(_dev, MCI_PLAY, MCI_NOTIFY,reinterpret_cast<DWORD_PTR>(&mciPlay)) ? false : true;
	_pausing = false;		// 修复win32下调用Resume接口会播放预加载音效的问题 add by JiaMiao 2016-4-26
}

bool MciPlayer::IsPlaying()
{
    return _playing;
}

UINT MciPlayer::GetSoundID()
{
    return _soundID;
}

// 修改cocos2d支持win32下设置音乐音效音量 add by JiaMiao 2016-1-7
void MciPlayer::Volume(UINT volume)
{
	if (!_dev)
		return;
	MCI_DGV_SETAUDIO_PARMS mciParams = { 0 };
	mciParams.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciParams.dwValue = volume;
	mciSendCommand(_dev, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD)&mciParams);
}

// 修改cocos2d支持win32下设置音乐音效音量 add by JiaMiao 2016-1-7
UINT MciPlayer::Volume() const
{
	if (!_dev)
		return 0;
	MCI_STATUS_PARMS mciParams = { 0 };
	mciParams.dwItem = MCI_DGV_STATUS_VOLUME;
	mciSendCommand(_dev, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciParams);
	return mciParams.dwReturn;
}

//////////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////////
void MciPlayer::_SendGenericCommand( int nCommand, DWORD_PTR param1 /*= 0*/, DWORD_PTR parma2 /*= 0*/ )
{
    if (! _dev)
    {
        return;
    }
    mciSendCommand(_dev, nCommand, param1, parma2);
}


//////////////////////////////////////////////////////////////////////////
// static function
//////////////////////////////////////////////////////////////////////////

LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    MciPlayer * pPlayer = NULL;
    if (MM_MCINOTIFY == Msg 
        && MCI_NOTIFY_SUCCESSFUL == wParam
        &&(pPlayer = (MciPlayer *)GetWindowLongPtr(hWnd, GWLP_USERDATA)))
    {
        if (pPlayer->_times)
        {
            --pPlayer->_times;
        }

        if (pPlayer->_times)
        {
            mciSendCommand(lParam, MCI_SEEK, MCI_SEEK_TO_START, 0);

            MCI_PLAY_PARMS mciPlay = {0};
            mciPlay.dwCallback = reinterpret_cast<DWORD_PTR>(hWnd);
            mciSendCommand(lParam, MCI_PLAY, MCI_NOTIFY,reinterpret_cast<DWORD_PTR>(&mciPlay));
        }
        else
        {
            pPlayer->_playing = false;
        }
        return 0;
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

} // end of namespace CocosDenshion
