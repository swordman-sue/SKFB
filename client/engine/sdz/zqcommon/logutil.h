#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <stdio.h>
#include <string>

struct HttpInfo;

class LogUtil
{
private:
	LogUtil(){};

public:
	enum LogType
	{
		LT_ERROR = 0,
		//LT_Warning,
		LT_INFO,
		//LT_Debug,

		LT_CONUT,
	};

	enum EventType4LossRate
	{
		ET_UNKNOWN = 0,

		//��Ϸ����
		ET_START_GAME,
		//ȡ�汾��Ϣ
		ET_REQ_VERSION_INFO,
		//ȡ�汾��Ϣ�б�
		ET_REQ_VERSION_INFO_LIST,
		//���ذ汾��Ϣ�б�
		ET_LOAD_VERSION_INFO_LIST,
		//���ؽű�ѹ����
		ET_DOWNLOAD_SCRIPT_CP,
		//��ѹ�ű�ѹ����
		ET_UNZIP_SCRIPT_CP,

		//���������
		ET_END_IN_ENGINE,
	
		//�󼸸��ɽű�����
		//....
	};

	static void Init();
	static void Release();

	static void LogInfo(const char *format, ...);
	static void LogError(const char *format, ...);
	static void LuaLogInfo(const char *str);
	static void LuaLogError(const char *str);

	//�ͻ����¼�����(ͳ����ʧ��)
	static void ReportEventTrackForLossRate(int type);

	//�ͻ����¼����ٹ����У��ϱ����¿����Ĵ�����Ϣ
	static void ReportErrorInfoForEventTrack(const char *arg_fmt, ...);

private:
	static void WriteLog(int log_level, const char *fmt, va_list argp);

	static void SetupHttpParamsForReport(HttpInfo* pHttpInfo, int type);

private:
	static FILE *log_file;					
	static int s_data_id_of_event_track_for_loss_rate;
	static int s_cur_event_track_type_for_loss_rate;
};

#endif
