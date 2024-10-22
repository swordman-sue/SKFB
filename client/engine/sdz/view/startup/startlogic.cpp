
#include "startlogic.h"
#include <stdlib.h>
#include <time.h>

#include "particle/strconv.h"
#include "particle/strutil.h"
#include "zqcommon/strutil.h"
#include "luaengine/luaengine.h"
#include "filesystem/filesystemex.h"
#include "filesystem/fileversion.h"
#include "filesystem/setting.h"
#include "filesystem/filehelper.h"
#include "platform/sdktools.h"
#include "platform/systemtools.h"
#include "resmanager/installer.h"
#include "zqcommon/logutil.h"
#include "json/json.h"
#include "gameengine/gameengine.h"

#define RESPACKAGESIZE_LIMIT 5*1024*1024   //资源包在非wifi环境下的大小限制提示

StartLogic* StartLogic::m_start_logic = nullptr;

void NetworkStateChangedFun(void* func_data)
{
	StartLogic* pStartLogic = (StartLogic*)func_data;
	pStartLogic->CheckNetworkState();
}

class StartLogicDownLoadListener : public HttpListener
{
public:
	enum DownLoadType
	{
		DLT_Cdn = 0,
		DLT_VersionIos,
		DLT_ResPackage,
		DLT_ScriptIos,
	};
	
	StartLogicDownLoadListener(StartLogic *logic, int download_type)
		:m_logic(logic)
		,m_download_type(download_type)
		,script_ios_version(0)
	{

	}
	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task)
	{
		switch(m_download_type)
		{
		case DLT_Cdn:
			m_logic->OnDownLoadFinish_Cdn(request_task.result, request_task.ret_code, request_task.http_ret_code, request_task.op_code, request_task.data_ptr, request_task.data_len);
			break;

		case DLT_VersionIos:
			m_logic->OnDownLoadFinish_VersionIos(request_task.result, request_task.ret_code, request_task.http_ret_code, request_task.op_code, request_task.data_ptr, request_task.data_len);
			break;

		case DLT_ResPackage:
			m_logic->OnDownLoadFinish_ResPackage(request_task.result, request_task.ret_code, request_task.http_ret_code, request_task.op_code, download_name, md5);
			break;

		case DLT_ScriptIos:
			m_logic->OnDownLoadFinish_ScriptIos(request_task.result, request_task.ret_code, request_task.http_ret_code, request_task.op_code, download_name, script_ios_version, request_task.data_ptr, request_task.data_len);
			break;
		}
	};
	virtual void Free()
	{
		delete this;
	}

public:
	std::string download_name;
	unsigned int script_ios_version;
	std::string md5;

protected:
	StartLogic *m_logic;
	int m_download_type;

};


// 进行初始化加载	注意：需要准备好远程md5文件和version.ios
class LoadVersionIosListener : public FileListener
{
public:
	LoadVersionIosListener(StartLogic *logic)
		:m_logic(logic)
	{

	}
	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info)
	{
		if (file_info.op_result != FileAsync::FOR_SUCC)
		{
			m_logic->OnLoadFinish_VersionIos(false, file_info.op_result, file_info.op_result_desc);
		}
		else
		{
			FileVersion::GetInstance()->Load(file_info.version_file_init,
				file_info.version_file_download,
				file_info.version_file_server,
				file_info.app_version_str,
				file_info.svn_version_str,
				file_info.version_file_expansion);
			m_logic->OnLoadFinish_VersionIos(true, file_info.op_result, "");
		}
	};
	virtual void Free()
	{
		delete this;
	}

protected:
	StartLogic *m_logic;
};

StartLogic::StartLogic()
	: m_bStarted(false)
    , m_cur_progress(LP_Invalid)
	, m_cdn_http_id(0)
	, m_md5_http_id(0)
	, m_version_ios_http_id(0)
	, m_new_package_http_id(0)
	, m_script_ios_http_id(0)
	, m_cur_http_id(0)
	, m_had_download_script_index(0)
	, m_had_download_respackage_index(0)
	, m_bCanResPackageAlert(true)
    , m_checkNetworkValidDelayTime(-1.0f)
    , m_checkNetworkValidElapseTime(-1.0f)
	, m_bInNetworkAlert(false)
	, m_eNetworkState(SystemTools::NETWORK_NONE)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    float epsilon = 1e-6;
    float version = SystemTools::GetIOSVersion();
    if (version - 10.0f > epsilon || fabs(version - 10.0f) <= epsilon)
    {
        //ios10以上系统，延迟检测网络有效性，目的是为了提高以下情况的用户体验（首次安装并启动后，需用户设置网络访问权限）
        m_checkNetworkValidDelayTime = 5.0f;
    }
#endif
        
}

StartLogic::~StartLogic()
{
	SystemTools::SetNetworkStateChangeNotifyFuncEx(nullptr, nullptr);
}

void StartLogic::Start()
{
	m_bStarted = true;
    m_eNetworkState = SystemTools::GetNetWorkState();
    if (SystemTools::NETWORK_NONE != m_eNetworkState && SystemTools::NETWORK_UNKNOW != m_eNetworkState)
    {
        StartDownLoad_Cdn();
    }

    Setting::GetInstance()->Set("IsInExamine", "0");
	SystemTools::SetNetworkStateChangeNotifyFuncEx(NetworkStateChangedFun, this);
}

void StartLogic::Restart(LogicProgress logic_progress)
{
	switch(logic_progress)
	{
	case LP_Cdn_DownLoading:
		StartDownLoad_Cdn();
		break;
	case LP_VersionIos_DownLoading:
		StartDownLoad_Cdn();
		break;
	case LP_VersionIos_Loading:
		StartDownLoad_VersionIos(true);
		break;
	case LP_NewPackage_GetInfo:
	case LP_NewPackage_DownLoading:
		StartDownLoad_NewPackage();
		break;
	case LP_ResPackage_DownLoading:
	case LP_ResPackage_Unziping:
		StartDownLoad_ResPackage();
		break;
	case LP_ScriptIos_DownLoading:
	case LP_ScriptIos_Loading:
		CheckScriptIos();
		break;
	default:
		break;
	}
}

void StartLogic::update(float delta)
{
	CheckNetworkValid(delta);
}

bool StartLogic::GetLogicProgress(LogicProgressItem& item)
{
	if (m_progress_queue.empty())
	{
		return false;
	}
	item = m_progress_queue.front();
	m_progress_queue.pop();
	return true;
}

void StartLogic::CancelCurProgress()
{
	if (m_cur_http_id == 0)
		return;

	HttpManager::GetInstance()->AbortRequest(*m_cur_http_id, true);
	LogicProgressItem lpi(m_cur_progress, SP_End_Fail, 0);
	m_progress_queue.push(lpi);
}

void StartLogic::StartDownLoad_Cdn()
{
	LogUtil::LogInfo("StartLogic::StartDownLoad_Cdn");

	if (LuaEngine::GetInstance()->GetConfig("IsLocalRes") == "true" && CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		//WIN32下，若配置读取本地资源，则跳过资源更新
		m_cur_progress = LP_EnterWithoutUpdate;
		m_progress_queue.push(LogicProgressItem(m_cur_progress, SP_Start));
	}
	else
	{
		HttpInfo hi;
		hi.url = LuaEngine::GetInstance()->GetConfig("VersionURL");
		hi.url_id = rand();
		hi.low_speed = 250.0f;
		hi.time_out_milliseconds = 15000;

		std::string channel_type_key = "channel_type";
		std::string channel_type_str = SdkTools::GetChannelId();
		channel_type_str = !channel_type_str.empty() ? channel_type_str : "0";

		std::string sub_channel_type_key = "sub_channel_type";
		std::string sub_channel_type_str = SdkTools::GetKeyDataChannelId();
		sub_channel_type_str = !sub_channel_type_str.empty() ? sub_channel_type_str : "1";

		std::string app_version_key = "app_version";
		std::string app_version = SystemTools::GetAppVersionCode();

		std::string check_examine_key = "check_examine";
		std::string check_examine = LuaEngine::GetInstance()->GetConfig("CheckExamine");
		check_examine = !check_examine.empty() ? check_examine : "0";

		hi.get_param_num = 4;
		memcpy(hi.get_param_names[0], channel_type_key.c_str(), channel_type_key.size());
		memcpy(hi.get_param_values[0], channel_type_str.c_str(), channel_type_str.size());
		memcpy(hi.get_param_names[1], sub_channel_type_key.c_str(), sub_channel_type_key.size());
		memcpy(hi.get_param_values[1], sub_channel_type_str.c_str(), sub_channel_type_str.size());
		memcpy(hi.get_param_names[2], app_version_key.c_str(), app_version_key.size());
		memcpy(hi.get_param_values[2], app_version.c_str(), app_version.size());
		memcpy(hi.get_param_names[3], check_examine_key.c_str(), check_examine_key.size());
		memcpy(hi.get_param_values[3], check_examine.c_str(), check_examine.size());

		StartLogicDownLoadListener *listener = new StartLogicDownLoadListener(this, StartLogicDownLoadListener::DLT_Cdn);
		m_cdn_http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_PLAT_MANAGER_PHP, hi, listener);
		m_cur_http_id = &m_cdn_http_id;

		m_cur_progress = LP_Cdn_DownLoading;
		LogicProgressItem lpi(m_cur_progress, SP_Start);
		m_progress_queue.push(lpi);
	}

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_REQ_VERSION_INFO);
}

void StartLogic::OnDownLoadFinish_Cdn(bool result, int ret_code, int http_ret_code, int op_code, const char* data_ptr, unsigned int len)
{
    m_cur_http_id = 0;

    LogUtil::LogInfo("StartLogic::OnDownLoadFinish_Cdn");
    
	if (!result || ret_code != 0)
	{	
		LogUtil::LogError("[OnDownLoadFinish_Cdn DownLoadError][result:%d ret_code:%d data_len:%d]", result, ret_code, len);
		LogicProgressItem lpi(LP_Cdn_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code);
		m_progress_queue.push(lpi);
		return;
	}

	Json::Reader json_reader;
	Json::Value root;
	std::string json_str(data_ptr, len);
	if(!json_reader.parse(json_str, root))
	{
		std::string error_msg = "[OnDownLoadFinish_Cdn] JsonDecodeError: ";
		error_msg += json_reader.getFormatedErrorMessages();
		LogicProgressItem lpi(LP_Cdn_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code, error_msg.c_str());
		m_progress_queue.push(lpi);
		LogUtil::LogError("%s", error_msg.c_str());
		return;
	}

	int json_ret_code = root["code"].asInt();
	std::string json_ret_str = root["str"].asString();
	if(json_ret_code < 0)
	{
		std::string error_msg = "[OnDownLoadFinish_Cdn] ReturnCodeError ret_code: ";
		error_msg += json_ret_code;
		LogicProgressItem lpi(LP_Cdn_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code, error_msg.c_str());
		m_progress_queue.push(lpi);
		LogUtil::LogError("%s", error_msg.c_str());
		return;
	}

	Json::Value& msg_root = root["msg"];
	std::string all_cdn_url = msg_root["cdn"].asString();

	//是否跳过资源更新
	int is_skip_update = msg_root["is_skip_update"].asInt();
	Setting::GetInstance()->Set("IsSkipUpdate", cocos2d::StrConv::toString(is_skip_update));

	//是否处于审核状态
	int is_in_examine = msg_root["is_in_examine"].asInt();
	Setting::GetInstance()->Set("IsInExamine", cocos2d::StrConv::toString(is_in_examine));

	if (!ZQ::StrUtil::StartsWith(all_cdn_url,"http://"))
	{
		all_cdn_url = std::string("http://") + all_cdn_url;
	}
	if (!ZQ::StrUtil::EndsWith(all_cdn_url,"/"))
	{
		all_cdn_url += std::string("/");
	}
	LuaEngine::GetInstance()->SetConfig("Cdn", all_cdn_url);

	m_md5_str = msg_root["md5"].asString();
	m_md5_str = m_md5_str.substr(0, 32);

	Json::Value& update_res_info = root["res_info"];
	Json::Value& packages = update_res_info["packages"];
	if (packages.isArray() && packages.size() > 0)
	{
		int count = packages.size();
		for (int i = 0; i < count; i++)
		{
			if (packages[i]["name"].isString() && packages[i]["md5"].isString() && packages[i]["size"].isInt())
			{
				const std::string& name = packages[i]["name"].asString();
				const std::string& md5 = packages[i]["md5"].asString().substr(0, 32);
				int size = packages[i]["size"].asInt();
				ResPackageInfo rp_info(name, md5, size);
				m_need_download_res_package.push_back(rp_info);
			}
			else
			{
				LogUtil::LogError("[OnDownLoadFinish_Cdn] json of res_info is error");
			}
		}
	}

    //检测本地version.ios是否需要更新
    if (is_skip_update != 1)
    {
        // 获取本地version.ios的md5
        if (!FileSystemEx::GetInstance()->IsDownloadPathFileExist("version.ios"))
        {
            if (!FileSystemEx::GetInstance()->CopyFileFromInitialToDownload("version.ios", "version.ios", 0, 0))
            {
                std::string error_msg = "[OnDownLoadFinish_Cdn] CopyFileFromInitialToDownload error]";
                LogicProgressItem lpi(LP_Cdn_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code, error_msg.c_str());
                m_progress_queue.push(lpi);
                return;
            }
        }
        
        char *file_buf;
        unsigned int file_size;
        std::string local_version_ios_md5 = "";
        if(FileSystemEx::GetInstance()->ReadDownloadFile("version.ios", &file_buf, &file_size))
        {
            local_version_ios_md5 = ZQ::StrUtil::Md5((const char*)file_buf, file_size);
            free(file_buf);
        }
        
        // 检测本地与远程version.ios的md5是否一致
        if (local_version_ios_md5 != m_md5_str)
        {
            StartDownLoad_VersionIos();
            return;
        }
    }
    
    StartLoad_VersionIos();
}

void StartLogic::StartDownLoad_VersionIos(bool ignoreCheckExamine)
{
    /*
    bool is_in_examine = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsInExamine")) == 1;
    if (is_in_examine && !ignoreCheckExamine)
    {
        StartLogic::m_start_logic = this;
        std::string strTips = LuaEngine::GetInstance()->GetStringConfig("tips");
        std::string strConfirmBtn = LuaEngine::GetInstance()->GetStringConfig("confirm");
        std::string strExamineDownloadTips = LuaEngine::GetInstance()->GetStringConfig("examine_download_tips");
        SdkTools::ShowDialog(strTips.c_str(), strExamineDownloadTips.c_str(), strConfirmBtn.c_str(), nullptr, OnStartDownLoadVersionIosDlgMsg);
        
        return;
    }
    */
    
	LogUtil::LogInfo("StartLogic::StartDownLoad_VersionIos");

	HttpInfo hi;
	hi.url = LuaEngine::GetInstance()->GetConfig("Cdn") + "version.ios";
	hi.save_path = FileSystemEx::GetInstance()->GetDownLoadResPath("version.ios");

	memcpy(hi.get_param_names[0], "v", 1);
	memcpy(hi.get_param_values[0], m_md5_str.c_str(), m_md5_str.length());
	hi.get_param_num = 1;

	StartLogicDownLoadListener *listener = new StartLogicDownLoadListener(this, StartLogicDownLoadListener::DLT_VersionIos);
	m_version_ios_http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, hi, listener);
	m_cur_http_id = &m_version_ios_http_id;
	m_cur_progress = LP_VersionIos_DownLoading;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_REQ_VERSION_INFO_LIST);
}

void StartLogic::OnDownLoadFinish_VersionIos(bool result, int ret_code, int http_ret_code, int op_code, const char* data_ptr, unsigned int len)
{
	m_cur_http_id = 0;
	if (!result || ret_code!=0)
	{
		LogUtil::LogError("[OnDownLoadFinish_VersionIos DownLoadError][result:%d ret_code:%d data_len:%d]", result, ret_code, len);
		LogicProgressItem lpi(LP_VersionIos_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code);
		m_progress_queue.push(lpi);
		return;
	}

	LogUtil::LogInfo("StartLogic::OnDownLoadFinish_VersionIos");

	// 检测新下载的version.ios有效性
	std::string local_version_ios_md5 = "";
	char *file_buf;
	unsigned int file_size;
	if (FileSystemEx::GetInstance()->IsDownloadPathFileExist("version.ios")
		&& FileSystemEx::GetInstance()->ReadDownloadFile("version.ios", &file_buf, &file_size))
	{
		local_version_ios_md5 = ZQ::StrUtil::Md5((const char*)file_buf, file_size);
		free(file_buf);
		if (local_version_ios_md5 == m_md5_str)
		{
			// 加载版本信息相关文件
			StartLoad_VersionIos(true);
			return;
		}
	}

	char error_msg[256];
	sprintf(error_msg, "[OnDownLoadFinish_VersionIos MD5Error][expect_md5:%s file_md5:%s]", m_md5_str.c_str(), local_version_ios_md5.c_str());
	LogUtil::LogError("%s", error_msg);
	LogicProgressItem lpi(LP_VersionIos_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code, error_msg);
	m_progress_queue.push(lpi);
}

void StartLogic::StartLoad_VersionIos(bool ignoreCheckExamine)
{
    /*
    bool is_in_examine = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsInExamine")) == 1;
    if (is_in_examine && !ignoreCheckExamine)
    {
        StartLogic::m_start_logic = this;
        std::string strTips = LuaEngine::GetInstance()->GetStringConfig("tips");
        std::string strConfirmBtn = LuaEngine::GetInstance()->GetStringConfig("confirm");
        std::string strExamineDownloadTips = LuaEngine::GetInstance()->GetStringConfig("examine_download_tips");
        SdkTools::ShowDialog(strTips.c_str(), strExamineDownloadTips.c_str(), strConfirmBtn.c_str(), nullptr, OnStartLoadVersionIosDlgMsg);
        
        return;
    }
    */

	LogUtil::LogInfo("StartLogic::StartLoad_VersionIos");

	m_cur_progress = LP_VersionIos_Loading;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);

	LoadVersionIosListener *listener = new LoadVersionIosListener(this);
	FileSystemEx::GetInstance()->LoadVersionFileAsync(listener);

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_LOAD_VERSION_INFO_LIST);
}

void StartLogic::OnLoadFinish_VersionIos(bool ret, int op_result, const std::string& op_result_desc)
{
	LogUtil::LogInfo("StartLogic::OnLoadFinish_VersionIos");

	if (!ret)
	{
		LogUtil::LogError("[OnLoadFinish_VersionIos LoadError]");
		LogicProgressItem lpi(LP_VersionIos_Loading, SP_End_Fail, 0, 0, op_result, op_result_desc.c_str());
		m_progress_queue.push(lpi);
		return;
	}

    bool bIsSkipUpdate = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsSkipUpdate")) == 1;
	bool bIsSkipAppVerUpdate = LuaEngine::GetInstance()->GetConfig("SkipAppVerUpdate") == "true";
	if (FileVersion::GetInstance()->CheckAppVersion() != FileVersion::AVS_UNUPDATE && !bIsSkipAppVerUpdate && !bIsSkipUpdate)
	{
		LogicProgressItem lpi(LP_NewPackage_WaitingForCheck, SP_Invalid);
		m_progress_queue.push(lpi);
		return;
	}

	StartDownLoad_ResPackage();
}

void StartLogic::StartDownLoad_NewPackage()
{
	LogUtil::LogInfo("StartLogic::StartDownLoad_NewPackage");

	Installer::RequestPackageUrl(this);
	m_cur_progress = LP_NewPackage_GetInfo;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);
}

void StartLogic::OnDownLoadFinish_GetNewPackageInfo(bool result, int ret_code, const char *data_ptr, unsigned int data_len)
{
	LogUtil::LogInfo("StartLogic::OnDownLoadFinish_GetNewPackageInfo");

	std::string url;	
	if(!Installer::GetDownloadUrl(result, ret_code, data_ptr, data_len, url, m_steup_filename))
	{
		LogicProgressItem lpi(LP_NewPackage_DownLoading, SP_End_Fail);
		m_progress_queue.push(lpi);
		return;
	}

	std::string special_md5 = "00000000000000000000000000000000";
	if(special_md5 == m_steup_filename)
	{
		// 特殊平台包，跳转浏览器打开
		SdkTools::GotoBrowser(url.c_str());
	}
	else if(!Installer::CheckHasDownload(m_steup_filename))
	{
		m_new_package_http_id = Installer::DownloadNewPack(url, 60 * 60 *1000, m_steup_filename, 0, this);
		m_cur_http_id = &m_new_package_http_id;
		m_cur_progress = LP_NewPackage_DownLoading;
		LogicProgressItem lpi(m_cur_progress, SP_Start);
		m_progress_queue.push(lpi);
	}
	else
	{
		m_cur_progress = LP_NewPackage_DownLoading;
		LogicProgressItem lpi(m_cur_progress, SP_Start);
		m_progress_queue.push(lpi);
		OnDownLoadFinish_NewPackage(true, 0, m_steup_filename);
	}
}

void StartLogic::OnDownLoadFinish_NewPackage(bool result, int ret_code, const std::string & package_name)
{
	LogUtil::LogInfo("StartLogic::OnDownLoadFinish_NewPackage");

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		// WIN32不进行安装
		StartDownLoad_ResPackage();
		return;
	}

	m_cur_http_id = 0;
	if (!result || ret_code!=0)
	{
		LogicProgressItem lpi(LP_NewPackage_DownLoading, SP_End_Fail, ret_code);
		m_progress_queue.push(lpi);
		m_new_package_http_id = 0;
		return;
	}

	LogicProgressItem lpi(LP_NewPackage_DownLoading, SP_End_Succ, 0);
    m_progress_queue.push(lpi);
}

void StartLogic::StartDownLoad_ResPackage()
{
    bool bIsSkipUpdate = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsSkipUpdate")) == 1;
	if (m_need_download_res_package.size() <= 0 || m_had_download_respackage_index >= m_need_download_res_package.size() || bIsSkipUpdate)
	{
		CheckScriptIos();
		return;
	}

	if (m_bCanResPackageAlert && SystemTools::GetNetWorkState() != SystemTools::NETWORK_WIFI && m_had_download_respackage_index == 0)
	{
		int total_size = 0;
		for (ResPackageInfo& rp_info : m_need_download_res_package)
		{
			total_size += rp_info.size;
		}
		if (total_size > RESPACKAGESIZE_LIMIT)
		{
			m_cur_progress = LP_StopProgressGoing;
			m_progress_queue.push(LogicProgressItem(m_cur_progress, SP_Start));

			StartLogic::m_start_logic = this;
			const std::string& tip = LuaEngine::GetInstance()->GetStringConfig("tips");
			const std::string& tip_info = LuaEngine::GetInstance()->GetStringConfig("download_tip");
			const std::string& confirm = LuaEngine::GetInstance()->GetStringConfig("confirm");
			const std::string& cancel = LuaEngine::GetInstance()->GetStringConfig("cancel");
			char tmp[128];
			sprintf(tmp, tip_info.c_str(), total_size / (1024.0f * 1024.0f));
			SdkTools::ShowDialog(tip.c_str(), tmp, confirm.c_str(), cancel.c_str(), OnDownloadResPackageDlgMsg);
			return;
		}
	}

	m_cur_progress = LP_StopProgressGoing;
	m_progress_queue.push(LogicProgressItem(m_cur_progress, SP_End_Succ));

	ResPackageInfo& rp_info = m_need_download_res_package[m_had_download_respackage_index];
	HttpInfo hi;
	const std::string& package_path = rp_info.name;
	std::string::size_type pos = package_path.find_last_of("/");
	std::string package_name = package_path;
	if (pos != std::string::npos)
	{
		package_name = package_path.substr(pos + 1, package_path.length() - pos - 1);
	}
	hi.url = LuaEngine::GetInstance()->GetConfig("Cdn") + package_path;
	hi.save_path = FileSystemEx::GetInstance()->GetDownLoadResPath(package_name);
	hi.time_out_milliseconds = 1800000;			//设置30分钟
	hi.expect_size = rp_info.size;
	hi.is_resume_recv = true;
	hi.write_type = true;
	memcpy(hi.get_param_names[0], "md5", 3);
	memcpy(hi.get_param_values[0], rp_info.md5.c_str(), rp_info.md5.length());
	hi.get_param_num = 1;

	StartLogicDownLoadListener* listener = new StartLogicDownLoadListener(this, StartLogicDownLoadListener::DLT_ResPackage);
	listener->download_name = hi.save_path;
	listener->md5 = rp_info.md5;
	m_res_package_http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, hi, listener);

	m_cur_http_id = &m_res_package_http_id;
	m_cur_progress = LP_ResPackage_DownLoading;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);
}

void StartLogic::OnDownLoadFinish_ResPackage(bool result, int ret_code, int http_ret_code, int op_code, const std::string& package_name, const std::string& md5)
{
	char* file_buff;
	unsigned int file_size;
	m_cur_http_id = 0;
	if (!result || ret_code != 0 || !FileHelper::IsExsit(package_name) || !FileHelper::Read(package_name, &file_buff, &file_size))
	{
		LogicProgressItem lpi(LP_ResPackage_DownLoading, SP_End_Fail, ret_code);
		m_progress_queue.push(lpi);
		return;
	}

	//验证下载资源包的md5
	const std::string& local_md5 = ZQ::StrUtil::Md5((const char*)file_buff, file_size);
	free(file_buff);
	if (local_md5 != md5)
	{
		LogicProgressItem lpi(LP_ResPackage_DownLoading, SP_End_Fail, ret_code);
		m_progress_queue.push(lpi);
		return;
	}
	
	m_need_unzip_res_package_list.push_back(package_name);
	m_had_download_respackage_index++;
	if (m_had_download_respackage_index < m_need_download_res_package.size())
	{
		StartDownLoad_ResPackage();
		return;
	}

	StartUnzipResPackage();
}

void StartLogic::DeleteResPackage()
{
	for (int i = 0; i < m_need_unzip_res_package_list.size(); i++)
	{
		if (FileHelper::IsExsit(m_need_unzip_res_package_list.at(i)))
		{
			FileHelper::Remove(m_need_unzip_res_package_list.at(i));
		}
	}
}

void StartLogic::OnUnzipFinish_ResPackage(bool result, const GameType::StrVect& unzip_filelist)
{
	if (!result)
	{
		m_had_download_respackage_index = 0;
		m_need_unzip_res_package_list = unzip_filelist;
		LogicProgressItem lpi(LP_ResPackage_Unziping, SP_End_Fail);
		m_progress_queue.push(lpi);
		return;
	}
	LogicProgressItem lpi(LP_ResPackage_Unziping, SP_End_Succ);
	m_progress_queue.push(lpi);

	CheckScriptIos();
}

class UnzipResPackageListener : public FileListener
{
public:
	UnzipResPackageListener(StartLogic *logic)
		:m_logic(logic)
	{}
	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info)
	{
		if (file_info.op_result != FileAsync::FOR_SUCC)
		{
			if (m_logic)
			{
				m_logic->OnUnzipFinish_ResPackage(false, file_info.unzip_file_list);
			}
		}
		else
		{
			if (m_logic)
			{
				m_logic->DeleteResPackage();
				FileVersion::GetInstance()->UpdateDownloadVersion(file_info.version_file_download);
				m_logic->OnUnzipFinish_ResPackage(true, file_info.unzip_file_list);
			}
		}
	};
	virtual void Free()
	{
		delete this;
	}

protected:
	StartLogic *m_logic;
};

void StartLogic::StartUnzipResPackage()
{
	UnzipResPackageListener *listener = new UnzipResPackageListener(this);
	FileSystemEx::GetInstance()->UnzipPackageAsync(FileAsync::FOT_UNZIP_RESPACKAGE, m_need_unzip_res_package_list, listener, FileSystemEx::GetInstance()->GetDownLoadPath());

	m_cur_progress = LP_ResPackage_Unziping;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);
}

void StartLogic::CheckScriptIos()
{
    bool bIsSkipUpdate = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsSkipUpdate")) == 1;
    if (!bIsSkipUpdate)
    {
        if (m_need_download_script_ios_list.size()==0)
        {
            FileVersion::GetInstance()->GetNeedToDownloadScriptIosVersion(m_need_download_script_ios_list);
        }
        
        // 判断是否有新版本
        if (m_had_download_script_index<m_need_download_script_ios_list.size())
        {
            FileVersion::FileVerInfo tmp_ver_info = m_need_download_script_ios_list[m_had_download_script_index];
            StartDownLoad_ScriptIos(tmp_ver_info.file_name, tmp_ver_info.size, tmp_ver_info.version);
            return;
        }
    }

	// 解压script.ios
	StartUnzip_ScriptIos();
}

void StartLogic::StartDownLoad_ScriptIos(const std::string& file_path, unsigned int size, unsigned int version)
{
	LogUtil::LogInfo("StartLogic::StartDownLoad_ScriptIos %s", file_path.c_str());

	std::string file_path_noext, ext, version_str;
	ZQ::StrUtil::BaseTypeToString(version, version_str);
	cocos2d::StrUtil::splitBaseFilename(file_path, file_path_noext, ext);

	HttpInfo hi;
	hi.url = LuaEngine::GetInstance()->GetConfig("Cdn") + file_path_noext + "_v" + version_str + "." + ext;
	hi.expect_size = size;
	hi.is_resume_recv = true;
	hi.write_type = true;
	hi.save_path = FileSystemEx::GetInstance()->GetDownLoadResPath(file_path);

	StartLogicDownLoadListener *listener = new StartLogicDownLoadListener(this, StartLogicDownLoadListener::DLT_ScriptIos);
	listener->download_name = file_path;
	listener->script_ios_version = version;

	m_script_ios_http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, hi, listener);
	m_cur_http_id = &m_script_ios_http_id;
	m_cur_progress = LP_ScriptIos_DownLoading;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_DOWNLOAD_SCRIPT_CP);
}

void StartLogic::OnDownLoadFinish_ScriptIos(bool result, int ret_code, int http_ret_code, int op_code, const std::string &script_ios_name, unsigned int script_ios_version, const char* data_ptr, unsigned int len)
{
	m_cur_http_id = 0;
	if (!result || ret_code!=0)
	{
		LogicProgressItem lpi(LP_ScriptIos_DownLoading, SP_End_Fail, ret_code, http_ret_code, op_code);
		m_progress_queue.push(lpi);
		return;
	}

	LogUtil::LogInfo("StartLogic::OnDownLoadFinish_ScriptIos %s", script_ios_name.c_str());

	FileVersion::GetInstance()->UpdateVersion(script_ios_name, script_ios_version);
	m_had_download_script_index++;

	CheckScriptIos();
}

void StartLogic::StartUnzip_ScriptIos()
{
	LogUtil::LogInfo("StartLogic::StartUnzip_ScriptIos");

	LuaEngine::GetInstance()->StartLoadScriptIos(this);
	m_cur_progress = LP_ScriptIos_Loading;
	LogicProgressItem lpi(m_cur_progress, SP_Start);
	m_progress_queue.push(lpi);

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_UNZIP_SCRIPT_CP);
}

void StartLogic::OnUnzipFinish_ScriptIos(bool result, const std::string& unzip_error_scriptios)
{
	if (!result)
	{
		m_had_download_script_index = 0;
		m_need_download_script_ios_list.clear();
		FileVersion::GetInstance()->UpdateVersion(unzip_error_scriptios, 0);
		LogicProgressItem lpi(LP_ScriptIos_Loading, SP_End_Fail);
		m_progress_queue.push(lpi);
		return;
	}

	LogUtil::LogInfo("StartLogic::OnUnzipFinish_ScriptIos");

	LogicProgressItem lpi(LP_ScriptIos_Loading, SP_End_Succ);
	m_progress_queue.push(lpi);

	//客户端事件跟踪(统计流失率)
	LogUtil::ReportEventTrackForLossRate(LogUtil::ET_END_IN_ENGINE);
}

bool StartLogic::GetDownLoadNewPackageProgress(unsigned int &http_id, unsigned int &total_len, unsigned  int &cur_len)
{
	if(m_new_package_http_id == 0)
	{
		return false;
	}
	http_id = m_new_package_http_id;
	return Installer::CheckDownloadProgress(m_new_package_http_id, total_len, cur_len);
}

bool StartLogic::GetCurDownLoadProgress(unsigned int &http_id, unsigned int &total_len, unsigned  int &cur_len)
{
	if(m_cur_http_id == 0 || *m_cur_http_id == 0)
	{
		return false;
	}

	http_id = *m_cur_http_id;
	return HttpManager::GetInstance()->GetProgress(*m_cur_http_id, total_len, cur_len);
}

void StartLogic::InstallSteupPackage()
{
	// 保存并进行安装
	Installer::Install(m_steup_filename);
}

unsigned int StartLogic::GetHadDownloadScriptIndex()
{
	return m_had_download_script_index;
}

unsigned int StartLogic::GetNeedDownloadResPackage()
{
	return m_need_download_res_package.size();
}

unsigned int StartLogic::GetHadDwonloadResPackage()
{
	return m_had_download_respackage_index;
}

void StartLogic::OnDownloadResPackageDlgMsg(int btn_id)
{
	if (m_start_logic == nullptr)
	{
		return;
	}

	if (btn_id)
	{
		m_start_logic->m_bCanResPackageAlert = false;
		m_start_logic->StartDownLoad_ResPackage();
		m_start_logic = nullptr;
	}
	else
	{
		GameEngine::GetInstance()->CloseApp();
	}
}

void StartLogic::OnStartDownLoadVersionIosDlgMsg(int btn_id)
{
    if (m_start_logic == nullptr)
    {
        return;
    }

    m_start_logic->StartDownLoad_VersionIos(true);
    m_start_logic = nullptr;
}

void StartLogic::OnStartLoadVersionIosDlgMsg(int btn_id)
{
    if (m_start_logic == nullptr)
    {
        return;
    }
    
    m_start_logic->StartLoad_VersionIos(true);
    m_start_logic = nullptr;
}

//检测网络有效性
void StartLogic::CheckNetworkValid(float delta)
{
    //延迟检测网络有效性
    if (m_checkNetworkValidDelayTime > 0.0f)
    {
        m_checkNetworkValidElapseTime = m_checkNetworkValidElapseTime >= 0.0f ? m_checkNetworkValidElapseTime : 0.0f;
        m_checkNetworkValidElapseTime += delta;
        if (m_checkNetworkValidElapseTime >= m_checkNetworkValidDelayTime)
        {
            m_checkNetworkValidDelayTime = -1.0f;
            m_checkNetworkValidElapseTime = -1.0f;
        }
        return;
    }
    
	float interval = 2;
	m_checkNetworkValidElapseTime = m_checkNetworkValidElapseTime >= 0.0f ? m_checkNetworkValidElapseTime : interval;
	m_checkNetworkValidElapseTime = m_checkNetworkValidElapseTime + delta;
	if (m_checkNetworkValidElapseTime < interval)
	{
		return;
	}
	m_checkNetworkValidElapseTime = m_checkNetworkValidElapseTime - interval;

	bool bAvailable = SystemTools::IsNetworkAvailable();
	if (bAvailable)
	{
		m_bInNetworkAlert = false;
		return;
	}

	if (m_bInNetworkAlert)
	{
		return;
	}
	m_bInNetworkAlert = true;

	//网络无效，中断未完成的下载链接
	CancelCurProgress();

	//提示网络无效弹窗
	SystemTools::CheckNetWork("", "", "");
}

//检测网络状态变化(如：网络制式变化)
void StartLogic::CheckNetworkState()
{
	SystemTools::NETWORKSTATE oldNetworkState = m_eNetworkState;
	m_eNetworkState = SystemTools::GetNetWorkState();
    
    if (oldNetworkState != m_eNetworkState)
    {
        //若启动后网络不连通，那么在首次连通后，立即调用StartDownLoad_Cdn
        if (LP_Invalid == m_cur_progress)
        {
            StartDownLoad_Cdn();
        }

        //从网络连通到不连通，中断未完成的下载链接
        if (SystemTools::NETWORK_NONE != oldNetworkState && SystemTools::NETWORK_UNKNOW != oldNetworkState)
        {
            CancelCurProgress();
        }
    }
}
