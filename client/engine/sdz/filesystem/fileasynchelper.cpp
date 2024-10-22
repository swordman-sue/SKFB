
#include "fileasync.h"
#include "filehelper.h"
#include "setting.h"
#include "zqcommon/xmlhelper.h"
#include "zqcommon/logutil.h"
#include "common/tinyxml/tinyxml.h"
#include "tinyxml2/tinyxml2.h"
#include "common/platform/system.h"
#include "zip/zip.h"

#include "particle/strconv.h"

bool FileAsync::WriteSettingFile(const std::string &file_full_path,const GameType::StrKVMap &setting_map)
{
	tinyxml2::XMLDocument document;
	tinyxml2::XMLElement* root_elem = document.NewElement("setting");
	for (GameType::StrKVMap_CIter iter = setting_map.begin(); iter != setting_map.end(); ++iter)
	{
		tinyxml2::XMLElement* setting = document.NewElement("setting");
		setting->SetAttribute("name", iter->first.c_str());
		tinyxml2::XMLText* value = document.NewText(iter->second.c_str());
		setting->InsertEndChild(value);

		root_elem->InsertEndChild(setting);
	}
	document.InsertFirstChild(document.NewDeclaration());
	document.InsertEndChild(root_elem);
	document.SaveFile((file_full_path + ".tmp").c_str());

	FileHelper::Rename(file_full_path+".tmp",file_full_path);

	return true;
}

bool FileAsync::WriteVersionFile(const std::string& download_path, const GameType::StrUIntKVMap& filepath_to_version_map)
{
	std::string config_file_path = download_path + "version.xml";
	if (config_file_path=="")
	{
		return false;
	}

	tinyxml2::XMLDocument document;
	tinyxml2::XMLElement* root_elem = document.NewElement("config");

	for (GameType::StrUIntKVMap_CIter iter = filepath_to_version_map.begin();iter != filepath_to_version_map.end(); ++iter)
	{
		tinyxml2::XMLElement* res = document.NewElement("res");
		res->SetAttribute("path", iter->first.c_str());
		std::string tmp_str;
		ZQ::StrUtil::BaseTypeToString(iter->second, tmp_str);
		tinyxml2::XMLText* value = document.NewText(tmp_str.c_str());
		res->InsertEndChild(value);

		root_elem->InsertEndChild(res);
	}
	document.InsertFirstChild(document.NewDeclaration());
	document.InsertEndChild(root_elem);
	document.SaveFile((config_file_path + ".tmp").c_str());

	FileHelper::Rename(config_file_path+".tmp",config_file_path);

	return true;
}

bool FileAsync::ReadVersionXml(const TiXmlDocument *xml, GameType::StrUIntKVMap& file_version_map, std::string& app_version, std::string& svn_app_version)
{
	const TiXmlElement* root_elem = xml->RootElement();
	if (0 == root_elem)
	{
		return false;
	}

	// 获取客户端执行文件版本
	const char* tmp_cpp_version = root_elem->Attribute("ver");
	if (0 != tmp_cpp_version)
	{
		app_version = tmp_cpp_version;
	}

	// 远程svn最新版本
	const char* tmp_svn_version = root_elem->Attribute("svn_ver");
	if (0 != tmp_svn_version)
	{
		svn_app_version = tmp_svn_version;
	}

	// 读取各个文件的版本呢
	const TiXmlElement *cur_elem = root_elem->FirstChildElement("res");
	if (0 == cur_elem)
	{
		return false;
	}

	while(0 != cur_elem)
	{
		std::string path;
		unsigned int svn_version;
		//unsigned int res_size;
		if (!GetNodeAttr(cur_elem, "path", path))
		{
			return false;
		}
		if (!GetNodeValue(cur_elem, svn_version))
		{
			return false;
		}
		//if (!GetNodeAttr(cur_elem, "s", res_size))
		//{
		//	res_size = 0;
		//}

		cur_elem = cur_elem->NextSiblingElement();
		file_version_map.insert(std::make_pair(path, svn_version));
	}
	return true;
}

bool FileAsync::ReadServerVersionXml(const TiXmlDocument *xml, FileVersion::FilePathToVersionInfoMap& file_version_map, std::string& app_version, std::string& svn_app_version)
{
	const TiXmlElement* root_elem = xml->RootElement();
	if (0 == root_elem)
	{
		return false;
	}

	// 获取客户端执行文件版本
	const char* tmp_cpp_version = root_elem->Attribute("ver");
	if (0 != tmp_cpp_version)
	{
		app_version = tmp_cpp_version;
	}

	// 远程svn最新版本
	const char* tmp_svn_version = root_elem->Attribute("svn_ver");
	if (0 != tmp_svn_version)
	{
		svn_app_version = tmp_svn_version;
	}

	// 读取各个文件的版本呢
	const TiXmlElement *cur_elem = root_elem->FirstChildElement("res");
	if (0 == cur_elem)
	{
		return false;
	}

	while (0 != cur_elem)
	{
		std::string path;
		FileVersion::FileVerInfo fvi;
		if (!GetNodeAttr(cur_elem, "path", path))
		{
			return false;
		}
		if (!GetNodeValue(cur_elem, fvi.version))
		{
			return false;
		}
		if (!GetNodeAttr(cur_elem, "s", fvi.size))
		{
			return false;
		}

		cur_elem = cur_elem->NextSiblingElement();
		file_version_map.insert(std::make_pair(path, fvi));
	}
	return true;
}

bool FileAsync::ReadVersionFromXml(const std::string& config_file_path, GameType::StrUIntKVMap& filepath_to_version_map, std::string& error_msg)
{
	if (config_file_path.empty())
	{
		error_msg = "[FileAsync::ReadVersionFromXml] invalid config_file_path";
		LogUtil::LogError("%s", error_msg.c_str());
		return false;
	}

	TiXmlDocument document;
	if (!document.LoadFile(config_file_path))
	{
		error_msg = "[FileAsync::ReadVersionFromXml] parse xml failed error: ";
		error_msg += document.ErrorDesc();
		LogUtil::LogError("%s", error_msg.c_str());
		return false;
	}

	std::string app_ver_str, svn_ver_str;
	bool ret = ReadVersionXml(&document, filepath_to_version_map, app_ver_str, svn_ver_str);
	if (!ret)
	{
		error_msg = "[FileAsync::ReadVersionFromXml] read version.xml failed";
		LogUtil::LogError("%s", error_msg.c_str());
	}
	return ret;
}

bool FileAsync::ReadResVersionFromXml(const std::string& config_file_path, GameType::StrUIntKVMap& filepath_to_version_map)
{
	if (config_file_path == "")
	{
		return false;
	}

	TiXmlDocument document;
	if (!document.LoadFile(config_file_path))
	{
		LogUtil::LogError("[FileAsync::ReadResVersionFromXml] %s", document.ErrorDesc());
		return false;
	}

	const TiXmlElement* root_elem = document.RootElement();
	while (0 != root_elem)
	{
		std::string path;
		unsigned int svn_version;
		if (!GetNodeAttr(root_elem, "path", path))
		{
			return false;
		}
		if (!GetNodeValue(root_elem, svn_version))
		{
			return false;
		}

		root_elem = root_elem->NextSiblingElement();
		filepath_to_version_map.insert(std::make_pair(path, svn_version));
	}

	return true;
}

bool FileAsync::ReadVersionFromIos(const std::string &config_file_path, FileVersion::FilePathToVersionInfoMap& filepath_to_version_map_server,
	std::string& app_version, std::string& svn_version, std::string& error_msg)
{
	Zip temp_zip;
	void *buffer=0;
	int unzip_buf_size;

	if (!temp_zip.Unzip(config_file_path.c_str(), 0))
	{
		error_msg = "[FileAsync::ReadVersionFromIos] unzip failed zip_file: " + config_file_path;
		return false;
	}
	if(!temp_zip.GetUzFileData("version.xml", &buffer, &unzip_buf_size))
	{
		error_msg = "[FileAsync::ReadVersionFromIos] get version.xml failed zip_file: " + config_file_path;
		return false;
	}

	TiXmlDocument document;
	if (!document.Parse((const char *)buffer))
	{
		error_msg = "[FileAsync::ReadVersionFromIos] parse version.xml failed error: ";
		error_msg += document.ErrorDesc();
		return false;
	}

	bool ret = ReadServerVersionXml(&document, filepath_to_version_map_server, app_version, svn_version);
	if (!ret)
	{
		error_msg = "[FileAsync::ReadVersionFromIos] read version.xml failed";
		error_msg += document.ErrorDesc();
	}
	return ret;
}

bool FileAsync::UnzipVersionIosFromPackage(const std::string &initial_res_path, GameType::StrUIntKVMap& filepath_to_version_map, std::string& error_msg)
{
	void *buffer = 0;
	unsigned int len = 0;
	if(!FileHelper::UnzipAndroidFile(initial_res_path, "version.xml", &buffer, &len))
	{
		error_msg = "[FileAsync::UnzipVersionIosFromPackage] unzip version.xml from apk error apk_path: " + initial_res_path;
		LogUtil::LogError("%s", error_msg.c_str());
		return false;
	}

	TiXmlDocument document;
	if (!document.Parse((const char *)buffer))
	{
		error_msg = "[FileAsync::UnzipVersionIosFromPackage] parse version.xml failed error: ";
		error_msg += document.ErrorDesc();
		LogUtil::LogError("%s", error_msg.c_str());
		return false;
	}

	std::string app_ver_str, svn_ver_str;
	bool ret = ReadVersionXml(&document, filepath_to_version_map, app_ver_str, svn_ver_str);
	if (!ret)
	{
		error_msg = "[FileAsync::UnzipVersionIosFromPackage] read version.xml failed";
		LogUtil::LogError("%s", error_msg.c_str());
	}
	free(buffer);
	return ret;
}

bool FileAsync::UnzipVersionIosFromExpansion(const std::string &expansion_path, GameType::StrUIntKVMap& file_vesion_expansion, std::string& error_msg)
{
	void *buffer = 0;
	unsigned int len = 0;
	if(!FileHelper::UnzipFile(expansion_path, "version.xml", &buffer, &len))
	{
		LogUtil::LogError("[FileAsync::UnzipVersionIosFromExpansion] UnzipFile error :%s", expansion_path.c_str());
		return false;
	}

	TiXmlDocument document;
	if (!document.Parse((const char *)buffer))
	{
		LogUtil::LogError("[FileAsync::UnzipVersionIosFromExpansion] Parse xml error :%s", expansion_path.c_str());
		return false;
	}

	std::string app_ver_str, svn_ver_str;
	bool ret = ReadVersionXml(&document, file_vesion_expansion, app_ver_str, svn_ver_str);

	free(buffer);
	return ret;
}

bool FileAsync::LoadAllVersionFile(const std::string &initial_res_path, const std::string &download_path, const std::string &expansion_path,
	std::string& app_version, std::string& svn_version,
	GameType::StrUIntKVMap& file_version_map_init, 
	GameType::StrUIntKVMap& file_version_map_download, 
	FileVersion::FilePathToVersionInfoMap& file_version_map_server,
	GameType::StrUIntKVMap& file_vesion_expansion,
	std::string& op_result_desc)
{
	std::string tmp_version;
	std::string version_file;

	// 读取安装包文件版本
#if defined(__ANDROID__)
	if (!UnzipVersionIosFromPackage(initial_res_path, file_version_map_init, op_result_desc))
	{
		op_result_desc = "[FileAsync::LoadAllVersionFile] read initial version list failed " + op_result_desc;
		LogUtil::LogError("%s", op_result_desc.c_str());
		return false;
	}	
#else
	version_file = initial_res_path + "version.xml";
	if (!ReadVersionFromXml(version_file, file_version_map_init, op_result_desc))
	{
		op_result_desc = "[FileAsync::LoadAllVersionFile] read initial version list failed " + op_result_desc;
		LogUtil::LogError("%s", op_result_desc.c_str());
		return false;
	}	
#endif

	// 读取扩展包文件版本
	if(expansion_path.length() > 0)
	{
#if defined(__ANDROID__)
		if (FileHelper::IsExsit(expansion_path))
		{
			bool ret = UnzipVersionIosFromExpansion(expansion_path, file_vesion_expansion, op_result_desc);
			if (!ret)
			{
				op_result_desc = "[FileAsync::LoadAllVersionFile] read expansion version list failed " + op_result_desc;
				LogUtil::LogError("%s", op_result_desc.c_str());
				return false;
			}
		}
#else
		version_file = expansion_path + "version.xml";
		if (FileHelper::IsExsit(version_file))
		{
			bool ret = ReadVersionFromXml(version_file, file_vesion_expansion, op_result_desc);
			if (!ret)
			{
				op_result_desc = "[FileAsync::LoadAllVersionFile] read expansion version list failed " + op_result_desc;
				LogUtil::LogError("%s", op_result_desc.c_str());
				return false;
			}	
		}
#endif
	}

	// 读取已下载文件版本
	std::string downloaded_version_list_file = download_path + "version.xml";
	if (FileHelper::IsExsit(downloaded_version_list_file))
	{
		if (!ReadVersionFromXml(downloaded_version_list_file, file_version_map_download, op_result_desc))
		{
			op_result_desc = "[FileAsync::LoadAllVersionFile] read downloaded version list failed " + op_result_desc;
			LogUtil::LogError("%s", op_result_desc.c_str());

			// 删除无效文件
			FileHelper::Remove(downloaded_version_list_file);
			return false;
		}
	}
	else
	{
		// 第一次启动，下载目录没有版本文件
	}

	// 读取服务器文件版本
    bool bIsSkipUpdate = cocos2d::StrConv::parseInt(Setting::GetInstance()->Get("IsSkipUpdate")) == 1;
	if (!bIsSkipUpdate && !ReadVersionFromIos(download_path + "version.ios", file_version_map_server, app_version, svn_version, op_result_desc))
	{
		op_result_desc = "[FileAsync::LoadAllVersionFile] read server version list failed " + op_result_desc;
		LogUtil::LogError("%s", op_result_desc.c_str());
		return false;
	}

	return true;
}


