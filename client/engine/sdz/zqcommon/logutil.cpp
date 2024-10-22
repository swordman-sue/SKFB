
#include "logutil.h"

#include <assert.h>
#include <time.h>
#include <stdarg.h>

#include "cocos2d.h"
#include "particle/strconv.h"
#include "zqcommon/typedef.h"
#include "platform/systemtools.h"
#include "filesystem/filesystemex.h"
#include "httpmanager/httpmanager.h"
#include "luaengine/luaengine.h"
#include "platform/sdktools.h"
#include "filesystem/setting.h"

#if (defined _WIN32) && (!defined WP8)
#include <WinCon.h>
typedef HWND(WINAPI *PROCGETCONSOLEWINDOW)();
#endif

FILE* LogUtil::log_file = 0;

int LogUtil::s_data_id_of_event_track_for_loss_rate = time(NULL);
int LogUtil::s_cur_event_track_type_for_loss_rate = ET_UNKNOWN;

void LogUtil::Init()
{
	std::string log_path = FileSystemEx::GetInstance()->GetNonVersionPath() + "game.log";
	log_file = fopen(log_path.c_str() ,"wb");

#if (defined _WIN32) && (!defined WP8)
		AllocConsole();

		HMODULE hKernel32 = GetModuleHandle("kernel32");
		PROCGETCONSOLEWINDOW GetConsoleWindow = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32, "GetConsoleWindow");
		HWND hWnd = GetConsoleWindow();
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED);
		
		// 支持标准输入输出流 add by swordman sue 2016-6-13
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
#endif
}

void LogUtil::Release()
{
	if (0!=log_file)
	{
		fclose(log_file);
	}

	#if (defined _WIN32) && (!defined WP8)
		FreeConsole();
	#endif

}

void LogUtil::WriteLog(int log_level, const char *fmt, va_list argp)
{
	const static int BUFFER_MAX_LEN = 1024 * 16;
	assert(log_file != 0);

	static const char *LL_STR[LT_CONUT] = { "Error", "Info", };
	const char* level_str = ((log_level >= 0 && log_level < LT_CONUT) ? LL_STR[log_level] : "Unknown");

	char time_str[64] = { 0 };
	time_t now_time = time(0);
	struct tm *local_time = localtime(&now_time);
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

	static char buffer[BUFFER_MAX_LEN];
	memset(buffer, 0, sizeof(buffer));

	int head_length = ::sprintf(buffer, "[%s] (%s): ", time_str, level_str);
	int msg_length = ::vsprintf(buffer + head_length, fmt, argp);
	int tail_length = ::sprintf(buffer + head_length + msg_length, "\n");

	cocos2d::log("%s", buffer);

	fwrite(buffer, 1, head_length + msg_length + tail_length, log_file);
	fflush(log_file);
}

void LogUtil::LuaLogInfo(const char *str)
{
	LogInfo("%s", str);
}

void LogUtil::LuaLogError(const char *str)
{
	LogError("%s", str);
}

void LogUtil::LogError(const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	WriteLog(LT_ERROR, format, argp);
	va_end(argp);
}

void LogUtil::LogInfo(const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	WriteLog(LT_INFO, format, argp);
	va_end(argp);
}

//客户端事件跟踪(统计流失率)
void LogUtil::ReportEventTrackForLossRate(int type)
{
	std::string statistic_url = LuaEngine::GetInstance()->GetConfig("EventTrackForLossRateURL");
	if (statistic_url.empty())
	{
		return;
	}

	if (s_cur_event_track_type_for_loss_rate >= type)
	{
		return;
	}
	s_cur_event_track_type_for_loss_rate = type;

	if (ET_END_IN_ENGINE == s_cur_event_track_type_for_loss_rate)
	{
		Setting::GetInstance()->Set("DataIDOfEventTrackForLossRate", cocos2d::StrConv::toString(s_data_id_of_event_track_for_loss_rate));
		return;
	}

	HttpInfo hi;
	hi.url = statistic_url;
	hi.time_out_milliseconds = 5000;
	SetupHttpParamsForReport(&hi, type);
	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_OTHER_DEFAULT, hi, nullptr);
}

//客户端事件跟踪过程中，上报导致卡死的错误信息
void LogUtil::ReportErrorInfoForEventTrack(const char *arg_fmt, ...)
{
	std::string statistic_url = LuaEngine::GetInstance()->GetConfig("ErrorInfoForEventTrackURL");
	if (statistic_url.empty())
	{
		return;
	}

	if (ET_UNKNOWN == s_cur_event_track_type_for_loss_rate || ET_END_IN_ENGINE == s_cur_event_track_type_for_loss_rate)
	{
		return;
	}

	HttpInfo hi;
	hi.url = statistic_url;
	hi.time_out_milliseconds = 5000;
	SetupHttpParamsForReport(&hi, s_cur_event_track_type_for_loss_rate);

	//错误信息
	std::string error_info_key = "error_msg";
	static char error_info_val[4096];
	memset(error_info_val, 0, sizeof(error_info_val));
	va_list args;
	va_start(args, arg_fmt);
	vsprintf(error_info_val, arg_fmt, args);
	va_end(args);
	memcpy(hi.post_param_names[hi.post_param_num], error_info_key.c_str(), error_info_key.size());
	memcpy(hi.post_param_values[hi.post_param_num], error_info_val, MAX_URL_PARAM_VALUE_LEN);
	hi.post_param_num += 1;

	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_OTHER_DEFAULT, hi, nullptr);
}

//初始化HTTP参数
void LogUtil::SetupHttpParamsForReport(HttpInfo* pHttpInfo, int type)
{
	if (pHttpInfo == nullptr)
	{
		return;
	}

	std::string app_key_key = "app_key";
	std::string app_key_str = "20160603";

	//时间戳
	std::string data_id_key = "data_id";
	std::string data_id_str = cocos2d::StrConv::toString(s_data_id_of_event_track_for_loss_rate);

	//设备ID
	std::string deviceid_key = "deviceid";
	std::string deviceid_str = SystemTools::GetDeviceId();

	//自定义渠道号
	std::string cp_channel_type_key = "cp_channel_type";
	std::string cp_channel_type_str = SdkTools::GetChannelIdEx();
	cp_channel_type_str = !cp_channel_type_str.empty() ? cp_channel_type_str : SdkTools::GetChannelId();

	//是否首次启动
	std::string is_new_key = "is_new";
	std::string is_new_str = Setting::GetInstance()->Get("IsFirstStarted");

	//机型
	std::string device_model_key = "device_model";
	std::string device_model_str = SystemTools::GetDeviceModel();

	//可用内存
	std::string mem_size_key = "mem_size";
	std::string mem_size_str = cocos2d::StrConv::toString((int)SystemTools::GetMemorySize());

	//APP版本号
	std::string app_version_key = "app_version";
	std::string app_version_str = SystemTools::GetAppVersion();

	//操作系统
	std::string os_key = "os";
	std::string os_str = SystemTools::GetSystemType();

	//事件类型
	std::string type_key = "type";
	std::string type_str = cocos2d::StrConv::toString(type);

	pHttpInfo->post_param_num = 10;
	std::string param_key_arr[] = { app_key_key, data_id_key, deviceid_key, cp_channel_type_key, is_new_key, device_model_key, mem_size_key, app_version_key, os_key, type_key};
	std::string param_val_arr[] = { app_key_str, data_id_str, deviceid_str, cp_channel_type_str, is_new_str, device_model_str, mem_size_str, app_version_str, os_str, type_str };;
	for (int i = 0; i < 10; ++i)
	{
		memcpy(pHttpInfo->post_param_names[i], param_key_arr[i].c_str(), param_key_arr[i].size());
		memcpy(pHttpInfo->post_param_values[i], param_val_arr[i].c_str(), param_val_arr[i].size());
	}
}