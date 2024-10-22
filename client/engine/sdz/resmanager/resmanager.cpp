
#include "resmanager.h"

#include "luaengine/luaengine.h"
#include "httpmanager/httpmanager.h"
#include "gameengine/gameengine.h"
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include "platform/systemtools.h"
#include "filesystem/fileversion.h"
#include "filesystem/setting.h"
#include "filesystem/filesystemex.h"
#include "filesystem/setting.h"
#include "zip/zip.h"
#include "view/skeletondatacache.h"

#include "cocos2dxext/texturecahceex.h"
#include "particle/particledatamanager.h"
#include "particle/strutil.h"

class LuaResDownloadHttpLister : public HttpListener
{
public:
	LuaResDownloadHttpLister(std::string file_name, ZQ::UINT32 version, ZQ::UINT32 func_id)
		: m_file_name(file_name)
		, m_version(version)
		, m_func_id(func_id)
	{

	}

	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task)
	{
		ResManager::GetInstance()->OnDownLoadFinish(
			http_id, m_func_id, 
			m_file_name, m_version,
			request_task.is_abort,
			request_task.result, 
			request_task.ret_code, 
			request_task.http_ret_code,
			request_task.op_code,
			request_task.data_ptr,
			request_task.data_len);
	}
	virtual void Free(){ delete this;}

private:
	std::string m_file_name;
	ZQ::UINT32 m_version;
	ZQ::UINT32 m_func_id;
};

class LuaCopyFileListener : public FileListener
{
public:
	LuaCopyFileListener(const std::string &file_name, unsigned int version, unsigned int func_id = 0)
		: m_file_name(file_name)
		, m_version(version)
		, m_func_id(func_id)
	{

	}
	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info)
	{
		bool ret = file_info.op_result == FileAsync::FOR_SUCC;
		ResManager::GetInstance()->OnCopyCallBack(
			async_id, m_func_id, 
			ret, m_file_name,
			m_version, file_info.op_result, 
			file_info.op_result_desc, file_info.len);
	};
	virtual void Free()
	{
		delete this;
	}

protected:
	std::string m_file_name;
	ZQ::UINT32 m_version;
	ZQ::UINT32 m_func_id;
};

class LuaUnzipFileListener : public FileListener
{
public:
	LuaUnzipFileListener(const std::string &file_name, unsigned int version, unsigned int func_id = 0)
		: m_file_name(file_name)
		, m_version(version)
		, m_func_id(func_id)
	{}

	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info)
	{
		bool ret = file_info.op_result == FileAsync::FOR_SUCC;
		ResManager::GetInstance()->OnUnzipCallBack(
			async_id, m_func_id, 
			ret, m_file_name, 
			m_version, file_info.op_result, 
			file_info.op_result_desc, file_info.len);
	};

	virtual void Free()
	{
		delete this;
	}

protected:
	std::string m_file_name;
	ZQ::UINT32 m_version;
	ZQ::UINT32 m_func_id;
};

class LuaCallUrlHttpLister : public HttpListener
{
public:
	LuaCallUrlHttpLister(std::string url, ZQ::UINT32 func_id)
		:m_url(url), m_func_id(func_id)
	{

	}

	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task)
	{
		LuaEngine::GetInstance()->CallUrlCallBack(
			http_id, m_func_id, request_task.is_abort, 
			request_task.result, request_task.ret_code, m_url, 
			request_task.data_ptr, request_task.data_len, request_task.http_ret_code, request_task.op_code);
	}
	virtual void Free(){ delete this; }

private:
	std::string m_url;
	ZQ::UINT32 m_func_id;
};

static ResManager* s_instance = nullptr;

ResManager* ResManager::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new ResManager;
	}
	return s_instance;
}

void ResManager::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

bool ResManager::Init()
{
	m_unziping_init_res_set.clear();

	//初始化自定义纹理缓存
	TextureCacheEx::GetInstance()->Init();

	//初始化粒子系统目录
	std::string config_path = "resource/effects/particle/config/";
	std::string texture_path = "resource/effects/particle/texture/";
	std::string mesh_path = "resource/effects/particle/mesh/";
	std::string texture_suffix = "sdz";
	cocos2d::PSManager::GetInstance()->Init(config_path, texture_path, mesh_path, texture_suffix);

	return true;
}

bool ResManager::Stop()
{
	SkeletonDataCache::GetInstance()->Stop();
	cocos2d::PSManager::GetInstance()->Stop();
	TextureCacheEx::GetInstance()->Stop();

	return true;
}

bool ResManager::Release()
{
	SkeletonDataCache::DelInstance();
	cocos2d::PSManager::DelInstance();
	TextureCacheEx::DelInstance();
	return true;
}

void ResManager::SetPlatAndCdnURL(const std::string& plat_url, const std::string& cdn_url)
{
	if (ZQ::StrUtil::EndsWith(plat_url, "/"))
	{
		LuaEngine::GetInstance()->SetConfig("Plat", plat_url);
	}
	else
	{
		LuaEngine::GetInstance()->SetConfig("Plat", plat_url+ "/");
	}

	if (ZQ::StrUtil::EndsWith(cdn_url, "/"))
	{
		LuaEngine::GetInstance()->SetConfig("Cdn", cdn_url);
	}
	else
	{
		LuaEngine::GetInstance()->SetConfig("Cdn", cdn_url+ "/");
	}
}

void ResManager::SetGameURL(const std::string& game_url)
{
	if (ZQ::StrUtil::EndsWith(game_url, "/"))
	{
		LuaEngine::GetInstance()->SetConfig("Game", game_url);
	}
	else
	{
		LuaEngine::GetInstance()->SetConfig("Game", game_url+ "/");
	}
}

void ResManager::SetConnectTimeout(int timeout)
{
	HttpManager::GetInstance()->SetConnectTimeout(0, timeout);
}

void ResManager::SetKeepAlive(bool is_keepalive)
{
	HttpManager::GetInstance()->SetKeepAlive(0, is_keepalive);
}

bool ResManager::CallPlatApi(const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, GameType::UINT32 callback_func_id, GameType::INT32 time_out)
{
	std::string plat_url = LuaEngine::GetInstance()->GetConfig("Plat") + url;
	if (plat_url == "")
	{
		return false;
	}
	return LuaCallUrlHelper(HttpManager::HM_CONN_ID_FOR_PLAT_MANAGER_PHP, plat_url, post_param, get_param, callback_func_id, time_out);
}

bool ResManager::CallGameApi(const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, GameType::UINT32 callback_func_id, GameType::INT32 time_out)
{
	std::string game_url = LuaEngine::GetInstance()->GetConfig("Game");
	return LuaCallUrlHelper(HttpManager::HM_CONN_ID_FOR_GAMETOOL_PHP, game_url+url, post_param, get_param, callback_func_id, time_out);
}

bool ResManager::CallDirectApi(const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, GameType::UINT32 callback_func_id, GameType::INT32 time_out)
{
	if(url == "")
	{
		return false;
	}
	return LuaCallUrlHelper(HttpManager::HM_CONN_ID_FOR_OTHER_DEFAULT ,url, post_param, get_param, callback_func_id, time_out);
}

bool ResManager::CallDirectApiEx(const std::string &url, lua_tinker::table header_list, const std::string &post_param, const std::string &get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out)
{
	if (url == "")
	{
		return false;
	}
	return LuaCallUrlHelper(HttpManager::HM_CONN_ID_FOR_OTHER_DEFAULT, url, header_list, post_param, get_param, callback_func_id, time_out);
}

bool ResManager::SendFile2Server(const std::string& url, const std::string& filePath, const char* contentType, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out)
{
	HttpInfo http_info;
	http_info.url = url;
	http_info.isSendFile = true;
	http_info.sendFilePath = FileSystemEx::GetInstance()->GetResourcePath(filePath);
	http_info.sendFileContentType = (char*)contentType;
	http_info.time_out_milliseconds = time_out;
	LuaCallUrlHttpLister* listener = new LuaCallUrlHttpLister(url, callback_func_id);
	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, http_info, (HttpListener*)listener);
	return true;
}

bool ResManager::SendFile2Server(const std::string& url, const char* fileData, unsigned int fileSize, const char* contentType, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out)
{
	HttpInfo http_info;
	http_info.url = url;
	http_info.isSendFile = true;
	http_info.sendFileData = (char*)fileData;
	http_info.sendFileContentType = (char*)contentType;
	http_info.time_out_milliseconds = time_out;
	LuaCallUrlHttpLister* listener = new LuaCallUrlHttpLister(url, callback_func_id);
	HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, http_info, (HttpListener*)listener);
	return true;
}

bool ResManager::LuaCallUrlHelper(int conn_id, const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, GameType::INT32 time_out, HttpListener *listener)
{
	if (url == "" || conn_id == HttpManager::HM_CONN_ID_FOR_INVILID)
	{
		CC_SAFE_DELETE(listener);
		return false;
	}

	HttpInfo http_info;
	http_info.url = url;
	http_info.url_id = rand();
	http_info.time_out_milliseconds = time_out;
	http_info.low_speed = 250.0f;

	GameType::INT32 count=0;

	if (lua_tinker::is_table(post_param))
	{
		count=0;

		lua_State* L = post_param.m_obj->m_L;
		lua_pushnil(L);
		while (0 != lua_next(L, post_param.m_obj->m_index))
		{
			lua_pushvalue(L, -2);
			std::string key = lua_tostring(L, -1);
			std::string value = lua_tostring(L, -2);

			if (MAX_URL_PARAM_NUM<=count
				|| MAX_URL_PARAM_LEN <= key.length()
				|| MAX_URL_PARAM_VALUE_LEN <= value.length())
			{
				CC_SAFE_DELETE(listener);
				LogUtil::LogError("[ResManager::LuaCallUrlHelper PostParamError][param_index:%d key_len:%d value_len:%d]", count, key.length(), value.length());
				return false;
			}

			memcpy(http_info.post_param_names[count], key.c_str(), key.length());
			memcpy(http_info.post_param_values[count], value.c_str(), value.length());
			++count;

			lua_pop(L, 2);
		}
		http_info.post_param_num = count;
	}

	if (lua_tinker::is_table(get_param))
	{
		count=0;

		lua_State* L = get_param.m_obj->m_L;
		lua_pushnil(L);
		while (0 != lua_next(L, get_param.m_obj->m_index))
		{
			lua_pushvalue(L, -2);
			std::string key = lua_tostring(L, -1);
			std::string value = lua_tostring(L, -2);

			if (MAX_URL_PARAM_NUM <= count
				|| MAX_URL_PARAM_LEN <= key.length()
				|| MAX_URL_PARAM_VALUE_LEN <= value.length())
			{
				CC_SAFE_DELETE(listener);
				LogUtil::LogError("[ResManager::LuaCallUrlHelper GetParamError][param_index:%d key_len:%d value_len:%d]", count, key.length(), value.length());
				return false;
			}

			memcpy(http_info.get_param_names[count], key.c_str(), key.length());
			memcpy(http_info.get_param_values[count], value.c_str(), value.length());
			++count;

			lua_pop(L, 2);
		}
		http_info.get_param_num = count;
	}

	HttpManager::GetInstance()->Request(conn_id, http_info, listener);
	return true;
}

bool ResManager::LuaCallUrlHelper(int conn_id, const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, GameType::UINT32 callback_func_id, GameType::INT32 time_out)
{
	LuaCallUrlHttpLister* ucl = new LuaCallUrlHttpLister(url, callback_func_id);
	LuaCallUrlHelper(conn_id, url, post_param, get_param, time_out, ucl);
	return true;
}

bool ResManager::LuaCallUrlHelper(int conn_id, const std::string &url, lua_tinker::table header_list, const std::string &post_param, const std::string &get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out)
{
	if (url == "" || conn_id == HttpManager::HM_CONN_ID_FOR_INVILID)
	{
		return false;
	}

	std::vector<std::string> header_vec;
	if (lua_tinker::is_table(header_list))
	{
		lua_State* L = header_list.m_obj->m_L;
		lua_pushnil(L);
		while (0 != lua_next(L, header_list.m_obj->m_index))
		{
			lua_pushvalue(L, -2);
			std::string value = lua_tostring(L, -2);
			header_vec.push_back(value);

			lua_pop(L, 2);
		}
	}

	HttpInfo http_info;
	http_info.url = url;
	http_info.url_id = rand();
	http_info.time_out_milliseconds = time_out;
	http_info.post_param_str = post_param;
	http_info.get_param_str = get_param;
	http_info.header_vec = header_vec;
	http_info.low_speed = 250.0f;

	LuaCallUrlHttpLister* listener = new LuaCallUrlHttpLister(url, callback_func_id);
	HttpManager::GetInstance()->Request(conn_id, http_info, listener);
	return true;
}

bool ResManager::UpdateFileList(lua_tinker::table file_list, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out, lua_tinker::table httpid_list, lua_tinker::table asyncid_list)
{
	if (!lua_tinker::is_table(file_list))
	{
		return false;
	}

	bool is_need_httpid = true;
	if (!lua_tinker::is_table(httpid_list))
	{
		is_need_httpid = false;
	}
	bool is_need_asyncid = true;
	if (!lua_tinker::is_table(asyncid_list))
	{
		is_need_asyncid = false;
	}

	std::string cdn_url = LuaEngine::GetInstance()->GetConfig("Cdn");

	lua_State* L = file_list.m_obj->m_L;
	lua_pushnil(L);
	while (0 != lua_next(L, file_list.m_obj->m_index))
	{
		std::string file_path;
		file_path = lua_tostring(L, -1);
		FileVersion::FileVerInfo file_ver_info = FileVersion::GetInstance()->CheckFileVersion(file_path);
		bool is_need_download = false;
		
		switch(file_ver_info.status)
		{
		//服务器资源是最新的
		case FileVersion::VS_SERVER_NEW:
			{
				is_need_download = true;
				break;
			}

		//安装包里面是最新的
		case FileVersion::VS_INIT_NEW:
			{
				//从安装包中解压资源到下载目录
				LuaCopyFileListener *listener = new LuaCopyFileListener(file_path, file_ver_info.version, callback_func_id);
				unsigned int async_id = FileSystemEx::GetInstance()->CopyFileFromInitialToDownloadAsync(file_path, file_path, listener);
				if (is_need_asyncid)
				{
					asyncid_list.set(file_path.c_str(), async_id);
				}
				break;
			}

		//拓展包里的是最新的
		case FileVersion::VS_INIT_EX_NEW:
			{
				if(FileSystemEx::GetInstance()->GetExpansionPath().length() > 0)
				{
					//从扩展包中解压资源到下载目录
					LuaUnzipFileListener *listener = new LuaUnzipFileListener(file_path, file_ver_info.version, callback_func_id);
					unsigned int async_id = FileSystemEx::GetInstance()->CopyFileFromExpansionToDownloadAsync(file_path, file_path, listener);
					if (is_need_asyncid)
					{
						asyncid_list.set(file_path.c_str(), async_id);
					}
				}
				else
				{
					is_need_download = true;
				}

				break;
			}

		//下载目录里面是最新的
		case FileVersion::VS_DOWNLOAD_NEW:
			{
				LuaEngine::GetInstance()->UpdateFileListCallBack(0, callback_func_id, file_path, false, true, 0, 0, 0, "", file_ver_info.size);
				break;
			}

		default:
			LogUtil::LogError("[ResManager::UpdateFileList FileVerStatusError][FileName:%s]", file_path.c_str());
			break;
		}

		//从资源服务器上下载资源到下载目录
		if (is_need_download)
		{
			std::string file_path_noext, ext, version_str;
			ZQ::StrUtil::BaseTypeToString(file_ver_info.version, version_str);
			cocos2d::StrUtil::splitBaseFilename(file_path, file_path_noext, ext);

			HttpInfo http_info;
			http_info.url = cdn_url + file_path_noext + "_v" + version_str + "." + ext;
			http_info.expect_size = file_ver_info.size;
			http_info.time_out_milliseconds = time_out;
			http_info.save_path = FileSystemEx::GetInstance()->GetDownLoadResPath(file_path);

			LuaResDownloadHttpLister* rl = new LuaResDownloadHttpLister(file_path, file_ver_info.version, callback_func_id);
			unsigned int http_id = HttpManager::GetInstance()->Request(HttpManager::HM_CONN_ID_FOR_RESOURCE, http_info, rl);
			if (is_need_httpid && http_id > 0)
			{
				httpid_list.set(file_path.c_str(), http_id);
			}

			std::string is_print_download_log = LuaEngine::GetInstance()->GetConfig("PrintDownloadLog");
			if (is_print_download_log == "true")
			{
				LogUtil::LogInfo("[ResManager::UpdateFileList dowload_file:%s]", file_path.c_str());
			}
		}

		lua_pop(L, 1);
	}

	return true;
}
bool ResManager::UpdateFileList(lua_tinker::table file_list, ZQ::UINT32 file_list_len, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out)
{
	lua_State* L = file_list.m_obj->m_L;
	lua_newtable(L);
	lua_tinker::table httpid_list(L, lua_gettop(L));
	lua_newtable(L);
	lua_tinker::table asyncid_list(L, lua_gettop(L));
	return UpdateFileList(file_list, callback_func_id, time_out, httpid_list, asyncid_list);
}

void ResManager::CancelUpdateFileList(lua_tinker::table httpid_list, lua_tinker::table asyncid_list)
{
	if (lua_tinker::is_table(httpid_list))
	{
		lua_State* L = httpid_list.m_obj->m_L;
		lua_pushnil(L);
		while (0 != lua_next(L, httpid_list.m_obj->m_index))
		{
			unsigned int httpid = lua_tointeger(L, -1);
			HttpManager::GetInstance()->AbortRequest(httpid, true);
			lua_pop(L, 1);
		}
	}

	if (lua_tinker::is_table(asyncid_list))
	{
		lua_State* L = asyncid_list.m_obj->m_L;
		lua_pushnil(L);
		while (0 != lua_next(L, asyncid_list.m_obj->m_index))
		{
			unsigned int async_id = lua_tointeger(L, -1);
			FileSystemEx::GetInstance()->CancelAsync(async_id);
			lua_pop(L, 1);
		}
	}
}

// 获取文件版本状态 edit by chenyingpeng
int ResManager::GetFileVersionStatus(const std::string& file_path)
{
	FileVersion::FileVerInfo fileVerInfo = FileVersion::GetInstance()->CheckFileVersion(file_path);
	return fileVerInfo.status;
}

// 获取文件大小 edit by chenyingpeng
unsigned int ResManager::GetFileSize(const std::string& file_path)
{
	return FileVersion::GetInstance()->GetFileSize(file_path);
}

void ResManager::OnDownLoadFinish(
	unsigned int http_id, unsigned int func_id, 
	const std::string &file_name, unsigned int version, 
	bool is_abort, bool result, int ret_code, int http_ret_code, int op_code, const char *data, unsigned int len)
{	
	std::string custom_error_msg;

	while(true)
	{
		if(ret_code != 0 || !result)
			break;

		unsigned int version_file_size = GetFileSize(file_name);
		if(len != version_file_size)
		{
			result = false;

			char temp_error_msg[512];
			sprintf(temp_error_msg, "[ResManager::OnDownLoadFinish] Download File %s Size %d Not Equal Version Size %d", file_name.c_str(), len, version_file_size);
			custom_error_msg = temp_error_msg;
			LogUtil::LogInfo("%s", temp_error_msg);
			break;
		}
		FileVersion::GetInstance()->UpdateVersion(file_name, version);
		break;
	}
	
	LuaEngine::GetInstance()->UpdateFileListCallBack(http_id, func_id, file_name, is_abort, result, ret_code, http_ret_code, op_code, custom_error_msg.c_str(), len);
}

void ResManager::OnCopyCallBack(
	unsigned int async_id, unsigned int func_id, 
	bool ret, const std::string &file_name, 
	ZQ::UINT32 version, int op_result, const std::string& op_result_desc, unsigned int len)
{
	m_unziping_init_res_set.erase(file_name);

	if (ret)
	{
		FileVersion::GetInstance()->UpdateVersion(file_name, version);
	}
	LuaEngine::GetInstance()->UpdateFileListCallBack(async_id, func_id, file_name, false, ret, 0, 0, op_result, op_result_desc.c_str(), len);
}

void ResManager::OnUnzipCallBack(
	unsigned int async_id, unsigned int func_id, 
	bool ret, const std::string &file_name, 
	ZQ::UINT32 version, int op_result, const std::string& op_result_desc, unsigned int len)
{
	m_unziping_init_res_set.erase(file_name);

	if (ret)
	{
		FileVersion::GetInstance()->UpdateVersion(file_name, version);
	}
	LuaEngine::GetInstance()->UpdateFileListCallBack(async_id, func_id, file_name, false, ret, 0, 0, op_result, op_result_desc.c_str(), len);
}

void ResManager::SetSetting(const std::string &key, const std::string &value)
{
	Setting::GetInstance()->Set(key,value);
}

std::string ResManager::GetSetting(const std::string &key)
{
	return Setting::GetInstance()->Get(key);
}

// 获取需要更新的文件列表
bool ResManager::GetUpdateFileList(lua_tinker::table download_file_list, lua_tinker::table unzip_file_list, lua_tinker::table must_unzip_file_list)
{
	if(!lua_tinker::is_table(download_file_list) || !lua_tinker::is_table(unzip_file_list) || !lua_tinker::is_table(must_unzip_file_list))
		return false;

	std::string district_language = Setting::GetInstance()->Get("DistrictLanguage");
	std::string language = Setting::GetInstance()->Get("Language");

	std::string lang_search_tag = "_language_";
	std::string lang_tag = "_language_" + language;
	std::string lang_config_search_tag = "resource/language/";
	std::string lang_config_tag = "resource/language/" + district_language;
	std::string lang_uires_search_tag = "resource/ui_res/language/";
	std::string lang_uires_tag = "resource/ui_res/language/" + language;

	//检测语言相关资源的有效性
#define CHECK_LANG_FILE(file_name)\
	if ((std::string::npos != file_name.find(lang_search_tag) && std::string::npos == file_name.find(lang_tag)) ||\
		(std::string::npos != file_name.find(lang_config_search_tag) && std::string::npos == file_name.find(lang_config_tag)) ||\
		(std::string::npos != file_name.find(lang_uires_search_tag) && std::string::npos == file_name.find(lang_uires_tag)))\
	{\
		continue;\
	}\

 	FileVersion::FilePathToVersionInfoMap file_version_map = FileVersion::GetInstance()->GetAllFileVersion();
	FileVersion::FilePathToVersionInfoMap::const_iterator file_version_iter = file_version_map.begin();
	for(; file_version_iter != file_version_map.end(); ++file_version_iter)
	{
		const std::string& file_name = file_version_iter->second.file_name;

		switch(file_version_iter->second.status)
		{
		//服务器资源是最新的
		case FileVersion::VS_SERVER_NEW:
		{
			//检测语言相关资源的有效性
			CHECK_LANG_FILE(file_name)

			download_file_list.set(file_version_iter->second.file_name.c_str(), GetFileSize(file_version_iter->second.file_name));
		}break;

		//安装包/扩展包资源是最新的
		case FileVersion::VS_INIT_NEW:
		case FileVersion::VS_INIT_EX_NEW:
		{
			//检测语言相关资源的有效性
			CHECK_LANG_FILE(file_name)

			unzip_file_list.set(file_version_iter->second.file_name.c_str(), GetFileSize(file_version_iter->second.file_name));

			if (
				//扩展包中的资源必须在进游戏前解压出来(由于目前不支持读取扩展包中的资源)
				FileVersion::VS_INIT_EX_NEW == file_version_iter->second.status ||
				//处理覆盖安装后，下载目录的文件没有更新的问题
				FileVersion::GetInstance()->IsDownLoadFileVersionExist(file_version_iter->second.file_name))
			{
				must_unzip_file_list.set(file_version_iter->second.file_name.c_str(), 1);
			}
		}break;

		//下载目录里面是最新的
		case FileVersion::VS_DOWNLOAD_NEW:
			break;

		default:
			break;
		}
	}

	return true;
}

// 动态解压随包资源
void ResManager::UnzipInitResDynamically(const char* file_name)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
	if (m_unziping_init_res_set.find(file_name) != m_unziping_init_res_set.end())
	{
		//解压中
		return;
	}

	if (!FileVersion::GetInstance()->IsLoaded())
	{
		return;
	}

	FileVersion::FileVerInfo file_ver_info = FileVersion::GetInstance()->CheckFileVersion(file_name);
	switch (file_ver_info.status)
	{
		// 异步解压随包资源
		case FileVersion::VS_INIT_NEW:
		{
    		m_unziping_init_res_set.insert(file_name);
			LuaCopyFileListener *listener = new LuaCopyFileListener(file_name, file_ver_info.version);
			FileSystemEx::GetInstance()->CopyFileFromInitialToDownloadAsync(file_name, file_name, listener);
			break;
		}

		// 异步解压扩展包资源
		case FileVersion::VS_INIT_EX_NEW:
		{
			if (FileSystemEx::GetInstance()->GetExpansionPath().length() > 0)
			{
				m_unziping_init_res_set.insert(file_name);
				LuaUnzipFileListener *listener = new LuaUnzipFileListener(file_name, file_ver_info.version);
				FileSystemEx::GetInstance()->CopyFileFromExpansionToDownloadAsync(file_name, file_name, listener);
			}
			break;
		}
		default:
			break;
	}
#endif
}
