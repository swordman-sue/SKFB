
#include "fileversion.h"
#include "filesystemex.h"
#include "filehelper.h"
#include "setting.h"

#include "platform/systemtools.h"
#include "httpmanager/httpmanager.h"

#include "zqcommon/logutil.h"
#include "zqcommon/strutil.h"
#include "zqcommon/xmlhelper.h"
#include "common/tinyxml/tinyxml.h"
#include "zip/zip.h"

#include "particle/strconv.h"

bool FileVersion::Load(const GameType::StrUIntKVMap &init_version_map, 
	const GameType::StrUIntKVMap &download_version_map, 
	const FilePathToVersionInfoMap &server_version_map,
	const std::string &app_version, const std::string &svn_version,
	const GameType::StrUIntKVMap &expansion_version_map)
{
	// 读取安装包文件版本
	m_init_version_map = init_version_map;

	// 读取已下载文件版本
	m_download_version_map = download_version_map;

	// 读取服务器文件版本
	m_server_version_map = server_version_map;
	m_server_app_version = app_version;
	m_server_svn_version = svn_version;
	
	// 拓展包资源版本
	m_expansion_map = expansion_version_map;

    bool bIsSkipUpdate = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsSkipUpdate")) == 1;

	// 缓存script.ios文件列表
	GameType::StrVect temp_script_ios_list;
	for (int i = 0; true; ++i)
	{
        //脚本压缩包文件名
		std::string tmp_id = "";
		ZQ::StrUtil::BaseTypeToString(i, tmp_id);
		std::string tmp_path_name = "resource/script/script";
		tmp_path_name += tmp_id+".ios";

        // 更新下载目录中的脚本压缩包
        if (!bIsSkipUpdate)
        {
            FilePathToVersionInfoMap_CIter server_iter = m_server_version_map.find(tmp_path_name);
            if (server_iter == m_server_version_map.end()) break;
            
            temp_script_ios_list.push_back(tmp_path_name);
            
            GameType::StrUIntKVMap_CIter download_iter = m_download_version_map.find(tmp_path_name);
            GameType::StrUIntKVMap_CIter init_iter = m_init_version_map.find(tmp_path_name);
            GameType::StrUIntKVMap_CIter expansion_iter = m_expansion_map.find(tmp_path_name);
            
            if (download_iter != m_download_version_map.end() && download_iter->second == server_iter->second.version)
            {
                // 下载目录里已经是最新
                // donothing
            }
            else
            {
                // 安装包里是最新版本，从安装包拷贝
                if (init_iter != m_init_version_map.end() && server_iter->second.version == init_iter->second)
                {
                    if (!FileSystemEx::GetInstance()->CopyFileFromInitialToDownload(tmp_path_name, tmp_path_name, 0, 0))
                    {
                        LogUtil::LogError("[FileVersion::Load] CopyFileFromInitialToDownload error [script_name:%s]", tmp_path_name.c_str());
                        return false;
                    }
                    
                    // 更新版本记录
                    m_download_version_map[tmp_path_name] = server_iter->second.version;
                }
                
                // 扩展包里是最新版本，从拓展包拷贝
                else if (expansion_iter != m_expansion_map.end() && server_iter->second.version == expansion_iter->second)
                {
                    if (!FileSystemEx::GetInstance()->CopyFileFromExpansionToDownload(tmp_path_name, tmp_path_name, 0, 0))
                    {
                        LogUtil::LogError("[FileVersion::Load] CopyFileFromExpansionToDownload error [script_name:%s]", tmp_path_name.c_str());
                        return false;
                    }
                    
                    // 更新版本记录
                    m_download_version_map[tmp_path_name] = server_iter->second.version;
                }
            }
        }
        
        // 跳过更新
        else
        {
            GameType::StrUIntKVMap_CIter init_iter = m_init_version_map.find(tmp_path_name);
            if (init_iter == m_init_version_map.end()) break;
            
            temp_script_ios_list.push_back(tmp_path_name);
        }
	}
	m_script_ios_list = temp_script_ios_list;
	
	m_is_loaded = true;
	return true;
}

int FileVersion::CheckAppVersion()
{
	assert(m_is_loaded);

	std::string local_app_version = SystemTools::GetAppVersion();
	std::size_t local_version_dot_pos = local_app_version.find_last_of(".");
	std::size_t remote_version_dot_pos = m_server_app_version.find_last_of(".");

	std::string base_app_version = local_app_version.substr(0, local_version_dot_pos);
	std::string remote_base_app_version = m_server_app_version.substr(0, remote_version_dot_pos);
	base_app_version = ZQ::StrUtil::ReplaceAll(base_app_version, ".", "");
	remote_base_app_version = ZQ::StrUtil::ReplaceAll(remote_base_app_version, ".", "");
	int base_app_version_num = 0;
	int remote_base_app_version_num = 0;
	ZQ::StrUtil::StringToBaseType(base_app_version, base_app_version_num);
	ZQ::StrUtil::StringToBaseType(remote_base_app_version, remote_base_app_version_num);
	if (base_app_version < remote_base_app_version)
	{
		return AVS_NEED_UPDATE;
	}

	std::string date_app_version = local_app_version.substr(local_version_dot_pos+1, local_app_version.length());
	std::string remote_date_app_version = m_server_app_version.substr(remote_version_dot_pos+1, m_server_app_version.length());
	int date_app_version_num = 0;
	int remote_date_app_version_num = 0;
	ZQ::StrUtil::StringToBaseType(date_app_version, date_app_version_num);
	ZQ::StrUtil::StringToBaseType(remote_date_app_version, remote_date_app_version_num);
	if (date_app_version_num < remote_date_app_version_num)
	{
		return AVS_CAN_IGNORE;
	}

	return AVS_UNUPDATE;
}

void FileVersion::UpdateVersion(std::string file_path, unsigned int version)
{
	assert(m_is_loaded);

	m_download_version_map[file_path] = version;
	m_is_changed = true;
}

void FileVersion::UpdateDownloadVersion(const GameType::StrUIntKVMap& file_list)
{
    for (auto& it : file_list)
    {
        m_download_version_map[it.first] = it.second;
        m_is_changed = true;
    }
}

// 本地下载文件版本号是否最新
bool FileVersion::IsDownLoadFileVersionNew(const std::string& file_path)
{
	assert(m_is_loaded);

	FilePathToVersionInfoMap_CIter server_ver_iter = m_server_version_map.find(file_path);
	GameType::StrUIntKVMap_CIter download_ver_iter = m_download_version_map.find(file_path);

	// 在已下载文件版本列表中找不到
	if (download_ver_iter == m_download_version_map.end())
	{
		return false;
	}

	// 已下载文件版本不是最新的
	if (server_ver_iter->second.version != download_ver_iter->second)
	{
		return false;
	}

	// 已下载文件版本是最新的，但文件丢失了
	if (!FileSystemEx::GetInstance()->IsDownloadPathFileExist(file_path))
	{
		return false;
	}

	return true;
}

// 安装包文件版本号是否最新
bool FileVersion::IsInitFileVersionNew(const std::string& file_path)
{
	assert(m_is_loaded);

	FilePathToVersionInfoMap_CIter server_ver_iter = m_server_version_map.find(file_path);
	GameType::StrUIntKVMap_CIter init_ver_iter = m_init_version_map.find(file_path);

	if(init_ver_iter == m_init_version_map.end())
	{
		return false;
	}

	if(init_ver_iter->second != server_ver_iter->second.version)
	{
		return false;
	}

	return true;
}

// 拓展包文件版本号是否最新
bool FileVersion::IsExpansionVersionNew(const std::string& file_path)
{
	assert(m_is_loaded);

	FilePathToVersionInfoMap_CIter server_ver_iter = m_server_version_map.find(file_path);
	GameType::StrUIntKVMap_CIter expansion_ver_iter = m_expansion_map.find(file_path);

	if(expansion_ver_iter == m_expansion_map.end())
	{
		return false;
	}

	if(expansion_ver_iter->second != server_ver_iter->second.version)
	{
		return false;
	}

	return true;
}

//本地下载文件版本号是否存在
bool FileVersion::IsDownLoadFileVersionExist(const std::string& file_path)
{
	assert(m_is_loaded);

	GameType::StrUIntKVMap_CIter download_ver_iter = m_download_version_map.find(file_path);
	return download_ver_iter != m_download_version_map.end();
}

// 在版本相同的情况下，返回值的优先级是 VS_DOWNLOAD_NEW > VS_INIT_NEW > VS_SERVER_NEW
FileVersion::FileVerInfo FileVersion::CheckFileVersion(const std::string& file_path)
{
	assert(m_is_loaded);

	FileVerInfo tmp_file_version_info;
	tmp_file_version_info.file_name = file_path;
	tmp_file_version_info.version = 0;

	FilePathToVersionInfoMap_CIter server_ver_iter = m_server_version_map.find(file_path);
	GameType::StrUIntKVMap_CIter download_ver_iter = m_download_version_map.find(file_path);
	GameType::StrUIntKVMap_CIter init_ver_iter = m_init_version_map.find(file_path);
	GameType::StrUIntKVMap_CIter expansion_ver_iter = m_expansion_map.find(file_path);

	//服务器版本号列表上没这个文件
	if (server_ver_iter == m_server_version_map.end())
	{
		tmp_file_version_info.status = VS_NOT_EXIST;
		return tmp_file_version_info;
	}

	//本地下载文件最新
	if(IsDownLoadFileVersionNew(file_path))
	{
		tmp_file_version_info.status = VS_DOWNLOAD_NEW;
		tmp_file_version_info.version = download_ver_iter->second;
		return tmp_file_version_info;
	}

	//安装包文件最新
	if(IsInitFileVersionNew(file_path))
	{
		tmp_file_version_info.status = VS_INIT_NEW;
		tmp_file_version_info.version = init_ver_iter->second;
		return tmp_file_version_info;
	}

	//拓展包文件最新
	if(IsExpansionVersionNew(file_path))
	{
		tmp_file_version_info.status = VS_INIT_EX_NEW;
		tmp_file_version_info.version = server_ver_iter->second.version;
		tmp_file_version_info.size = server_ver_iter->second.size;
		return tmp_file_version_info;
	}

	//服务器上版本号最新
	tmp_file_version_info.status = VS_SERVER_NEW;
	tmp_file_version_info.version = server_ver_iter->second.version;
	tmp_file_version_info.size = server_ver_iter->second.size;
	return tmp_file_version_info;
}

FileVersion::FilePathToVersionInfoMap FileVersion::CheckFileListVersion(const GameType::StrVect &file_path)
{
	assert(m_is_loaded);

	FilePathToVersionInfoMap tmp_filepath_to_version_map;
	for (GameType::StrVect_CIter file_iter = file_path.begin(); file_iter != file_path.end(); ++file_iter)
	{
		FileVerInfo tmp_ver_info = CheckFileVersion(*file_iter);
		tmp_filepath_to_version_map.insert(std::make_pair(*file_iter, tmp_ver_info));
	}
	return tmp_filepath_to_version_map;
}

void FileVersion::GetNeedToDownloadScriptIosVersion(std::vector<FileVerInfo> &ver_info)
{
	for (GameType::StrVect_CIter iter=m_script_ios_list.begin();iter!=m_script_ios_list.end();++iter)
	{
		FileVerInfo tmp_ver_info = CheckFileVersion(*iter);
		if (tmp_ver_info.status == VS_SERVER_NEW)
		{
			ver_info.push_back(tmp_ver_info);
		}
	}
}

unsigned int FileVersion::GetFileSize(const std::string& file_path) const
{
	FilePathToVersionInfoMap_CIter server_ver_iter = m_server_version_map.find(file_path);
	if (server_ver_iter != m_server_version_map.end())
	{
		return server_ver_iter->second.size;
	}

	return 0;
}

void FileVersion::Save(unsigned int now_time)
{
	if (!m_is_loaded)
	{
		return;
	}

	if (!m_is_changed || m_last_save_time + 5000 > now_time)
	{
		return;
	}

	FileSystemEx::GetInstance()->WriteVersionFileAsync(m_download_version_map);

	m_is_changed = false;
	m_last_save_time = now_time;
	return;
}

//获取所有文件的本地版本
FileVersion::FilePathToVersionInfoMap FileVersion::GetAllFileVersion()
{
	assert(m_is_loaded);
	
	FileVerInfo filever_info;
	FilePathToVersionInfoMap filepath_to_version_map;

	FilePathToVersionInfoMap_CIter version_iter = m_server_version_map.begin();
	for(; version_iter != m_server_version_map.end(); ++version_iter)
	{
		filever_info = CheckFileVersion(version_iter->first);
		filepath_to_version_map.insert(std::make_pair(version_iter->first, filever_info));
	}
	return filepath_to_version_map;
}

//获取本地version.ios的md5码
std::string FileVersion::GetLocalVersionMd5()
{
	std::string local_version_ios_md5 = "";
	char *file_buf;
	unsigned int file_size;

	if(!FileSystemEx::GetInstance()->ReadDownloadFile("version.ios", &file_buf, &file_size))
	{
		return local_version_ios_md5;
	}

	local_version_ios_md5 = ZQ::StrUtil::Md5((const char*)file_buf, file_size);
	free(file_buf);

	return local_version_ios_md5;
}
