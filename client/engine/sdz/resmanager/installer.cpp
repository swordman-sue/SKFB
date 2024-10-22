
#include "installer.h"
#include "view/startup/startlogic.h"
#include "httpmanager/httpmanager.h"
#include "filesystem/filesystemex.h"
#include "filesystem/filehelper.h"
#include "luaengine/luaengine.h"
#include "zqcommon/logutil.h"
#include "platform/sdktools.h"
#include "platform/systemtools.h"

class InstallerHttpLister : public HttpListener
{
public:
	enum RequestType
	{
		RT_GET_NEW_PACKAGE_INFO,
		RT_DOWNLOAD_NEW_PACKAGE,
	};

	InstallerHttpLister(int type, ZQ::UINT32 func_id, NewPackageInfoInterface* package_interface, std::string file_path_name)
		: m_type(type)
		, m_func_id(func_id)
		, m_package_interface(package_interface)
		, m_file_path_name(file_path_name)
	{

	}

	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task)
	{
		switch(m_type)
		{
		case RT_GET_NEW_PACKAGE_INFO:
			Installer::OnFinish_GetNewPackInfo(http_id, m_func_id, m_package_interface, request_task.is_abort, request_task.result, request_task.ret_code, request_task.data_ptr, request_task.data_len);
			break;
		case RT_DOWNLOAD_NEW_PACKAGE:
			Installer::OnFinish_DownloadNewPack(http_id, m_func_id, m_package_interface, request_task.is_abort, request_task.result, request_task.ret_code, m_file_path_name);
			break;
		default:
			LogUtil::LogError("[InstallerHttpLister::OnCallBack SwitchError][type:%d http_id:%d url:%s]", m_type, http_id, request_task.http_info.url.c_str());
			break;
		}
	}
	virtual void Free(){ delete this;}

private:
	int m_type;
	unsigned int m_func_id;
	NewPackageInfoInterface* m_package_interface;
	std::string m_file_path_name;
};

class DefualtPackageInfoInterface : public NewPackageInfoInterface
{
	virtual void OnDownLoadFinish_GetNewPackageInfo(bool result, int ret_code, const char* data_ptr, unsigned int data_len)
	{
		std::string url, m_steup_filename;
		if(!Installer::GetDownloadUrl(result, ret_code, data_ptr, data_len, url, m_steup_filename))
		{
			delete this;
			return;
		}
		
		SdkTools::GotoBrowser(url.c_str());
		delete this;
	}

	virtual void OnDownLoadFinish_NewPackage(bool result, int ret_code, const std::string &package_name){}
};

unsigned int Installer::GetNewPackInfo(const std::string& url, unsigned int time_out, unsigned int lua_callback_func_id, NewPackageInfoInterface* package_interface)
{
	HttpInfo http_info;
	http_info.url = url;
	http_info.time_out_milliseconds = time_out;
	InstallerHttpLister* rl = new InstallerHttpLister(InstallerHttpLister::RT_GET_NEW_PACKAGE_INFO, lua_callback_func_id, package_interface, "");
	unsigned int http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, http_info, rl);
	return http_id;
}

unsigned int Installer::GetNewPackInfoForLua(const std::string& url, unsigned int time_out, unsigned int lua_callback_func_id)
{
	return GetNewPackInfo(url, time_out, lua_callback_func_id, 0);
}

bool Installer::CheckHasDownload(std::string package_name)
{
	std::string package_path = FileSystemEx::GetInstance()->GetPackageDownloadPath() + package_name;
	std::string tmp_path = package_path + ".tmp";
	return FileHelper::IsExsit(package_path) && !FileHelper::IsExsit(tmp_path);
}

void Installer::OnFinish_GetNewPackInfo(unsigned int http_id, unsigned int func_id, NewPackageInfoInterface* package_interface, bool is_abort, bool result, int ret_code, const char *data_ptr, unsigned int data_len)
{
	if (func_id!=0)
	{
		// 回调给lua
		LuaEngine::GetInstance()->CallFunction(func_id, "Ibbis", http_id, is_abort, result, ret_code, data_ptr);
		return;
	}
	else if (package_interface!=0)
	{
		// 回调给启动模块
		package_interface->OnDownLoadFinish_GetNewPackageInfo(result, ret_code, data_ptr, data_len);
		return;
	}
	else
	{
		LogUtil::LogError("[Installer::OnFinish_GetNewPackInfo NoCallBack][http_id:%d]", http_id);
	}
}

unsigned int Installer::DownloadNewPack(const std::string& url, unsigned int time_out, const std::string &save_file_name, unsigned int lua_callback_func_id, NewPackageInfoInterface* package_interface)
{
	LogUtil::LogInfo("Installer::DownloadNewPack");

	HttpInfo http_info;
	http_info.url = url;
	http_info.time_out_milliseconds = time_out;
	http_info.save_path = FileSystemEx::GetInstance()->GetPackageDownloadPath() + save_file_name;
	http_info.is_resume_recv = true;
	http_info.write_type = true;
	InstallerHttpLister* rl = new InstallerHttpLister(InstallerHttpLister::RT_DOWNLOAD_NEW_PACKAGE, lua_callback_func_id, package_interface, save_file_name);
	unsigned int http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, http_info, rl);
	return http_id;
}

unsigned int Installer::DownloadNewPackForLua(const std::string& url, unsigned int time_out, const std::string &save_file_name, unsigned int lua_callback_func_id)
{
	return DownloadNewPack(url, time_out, save_file_name, lua_callback_func_id, 0);
}

bool Installer::CheckDownloadProgress(unsigned int http_id, unsigned int &total_len, unsigned  int &cur_len)
{
	return HttpManager::GetInstance()->GetProgress(http_id, total_len, cur_len);
}

bool Installer::CheckDownloadProgressForLua(ZQ::UINT32 http_id, lua_tinker::table progress_params)
{
	unsigned int total_len, cur_len;
	HttpManager::GetInstance()->GetProgress(http_id, total_len, cur_len);

	if (lua_tinker::is_table(progress_params))
	{
		progress_params.set("total_len", total_len);
		progress_params.set("cur_len", cur_len);
		return true;
	}

	return false;
}

bool Installer::Install(const std::string &filename)
{
	return SdkTools::Install( FileSystemEx::GetInstance()->GetPackageDownloadPath() +  filename);
}

void Installer::OnFinish_DownloadNewPack(unsigned int http_id, unsigned int func_id, NewPackageInfoInterface* package_interface, bool is_abort, bool result, int ret_code, const std::string &save_file_name)
{
	if (func_id!=0)
	{
		// 回调给lua
		LuaEngine::GetInstance()->CallFunction(func_id, "Ibbis", http_id, is_abort, result, ret_code, save_file_name.c_str());
		return;
	}
	else if (package_interface!=0)
	{
		// 回调给启动模块
		package_interface->OnDownLoadFinish_NewPackage(result, ret_code, save_file_name);
	}
	else
	{
		LogUtil::LogError("[Installer::OnFinish_GetNewPackInfo NoCallBack][http_id:%d save_file_name:%d]", http_id, save_file_name.c_str());
	}
}

void Installer::Cancle(unsigned int http_id)
{
	HttpManager::GetInstance()->AbortRequest(http_id, true);
}

void Installer::GotoDownloadUrl()
{
	DefualtPackageInfoInterface *package_interface = new DefualtPackageInfoInterface();
	RequestPackageUrl(package_interface);
}

void Installer::RequestPackageUrl(NewPackageInfoInterface* package_interface)
{
	if(NULL == package_interface)
		return;

	if (SystemTools::IsApptorePackage())
	{
		// 苹果官方版本直接打开appstore
		std::string apple_pkg_url = LuaEngine::GetInstance()->GetConfig("ApplePkgUrl");
		SdkTools::GotoBrowser(apple_pkg_url.c_str());
	}
	else
	{
		// 苹果越狱或者安卓则进行下载
		std::string url = LuaEngine::GetInstance()->GetConfig("PackageInfo") + "?" + SdkTools::GetUpdateUrlParams();
		LogUtil::LogInfo(url.c_str());
		GetNewPackInfo(url, 60 * 60 * 1000, 0, package_interface);
	}
}

bool Installer::GetDownloadUrl(bool result, int ret_code, const char *data_ptr, unsigned int data_len, std::string& url, std::string& file_name)
{
	if (!result || ret_code != 0)
	{
		return false;
	}

	std::string special_name = std::string(data_ptr, 32);
	std::string download_url = std::string(data_ptr, 32, data_len-32);
	std::string special_md5 = "00000000000000000000000000000000";
	std::string error_md5 = "11111111111111111111111111111111";

	// 错误检测
	if (error_md5 == special_name)
	{
		LogUtil::LogError("[Installer::GetDownloadUrl GetErrorMd5Code][ErrorInfo:%s]", download_url.c_str());
		return false;
	}

	// 修改安装包下载地址获取方式 add by swordman sue 2017-3-8
	// 特殊平台包，跳转浏览器打开
	if (special_md5 == special_name)
	{
		url = download_url;
		file_name = special_name;
		return true;
	}

	std::string data_str = data_ptr;
	int pos = data_str.find("http");
	file_name = data_str.substr(0, pos);
	url = data_str.substr(pos, data_len - pos);
	// end

	return true;
}
