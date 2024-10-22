#ifndef PLATFORM_VOICE_H
#define PLATFORM_VOICE_H
class Voice
{
public:
	//开始录制
	static int StartRecord(const char* full_file_name,int secord);
	//返回秒
	static int StopRecord();
	//开始录音
	static int StartPlay(const char* full_file_name,unsigned int voice_id, unsigned int lua_fun_id);
	//返回秒
	static int StopPlay();
	//释放资源
	static int ReleaseVoice();
};
#endif
