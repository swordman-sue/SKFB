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

		//游戏启动
		ET_START_GAME,
		//取版本信息
		ET_REQ_VERSION_INFO,
		//取版本信息列表
		ET_REQ_VERSION_INFO_LIST,
		//加载版本信息列表
		ET_LOAD_VERSION_INFO_LIST,
		//下载脚本压缩包
		ET_DOWNLOAD_SCRIPT_CP,
		//解压脚本压缩包
		ET_UNZIP_SCRIPT_CP,

		//引擎打点结束
		ET_END_IN_ENGINE,
	
		//后几个由脚本触发
		//....
	};

	static void Init();
	static void Release();

	static void LogInfo(const char *format, ...);
	static void LogError(const char *format, ...);
	static void LuaLogInfo(const char *str);
	static void LuaLogError(const char *str);

	//客户端事件跟踪(统计流失率)
	static void ReportEventTrackForLossRate(int type);

	//客户端事件跟踪过程中，上报导致卡死的错误信息
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
