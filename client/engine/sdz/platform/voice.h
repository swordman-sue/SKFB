#ifndef PLATFORM_VOICE_H
#define PLATFORM_VOICE_H
class Voice
{
public:
	//��ʼ¼��
	static int StartRecord(const char* full_file_name,int secord);
	//������
	static int StopRecord();
	//��ʼ¼��
	static int StartPlay(const char* full_file_name,unsigned int voice_id, unsigned int lua_fun_id);
	//������
	static int StopPlay();
	//�ͷ���Դ
	static int ReleaseVoice();
};
#endif
