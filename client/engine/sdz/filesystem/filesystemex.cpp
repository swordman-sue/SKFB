
#include "filesystemex.h"
#include "platform/systemtools.h"
#if defined(__ANDROID__)
	#include "platform/android/fileasynchandler.h"
#endif

#include "filehelper.h"
#include "fileversion.h"
#include "setting.h"

#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include "luaengine/luaengine.h"
#include "cocos2d.h"

static FileSystemEx* s_instance = nullptr;

FileSystemEx* FileSystemEx::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new FileSystemEx;
	}
	return s_instance;
}

void FileSystemEx::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

FileSystemEx::FileSystemEx()
{
#if defined(__APPLE__)
	m_initial_res_path = SystemTools::GetAppPackPath();
	m_download_path = SystemTools::GetAppPath()+"Library/res/";
	m_nonversion_path = SystemTools::GetAppPath()+"Library/nonversion/";
	m_tmp_path = SystemTools::GetAppPath()+"tmp/";
	m_packagedownload_path = SystemTools::GetAppPath()+"Documents/package/";
	m_log_path = m_nonversion_path;

#elif defined (__ANDROID__)
	m_initial_res_path = SystemTools::GetAppPackPath();
	m_download_path = SystemTools::GetAppPath();
	m_nonversion_path = SystemTools::GetAppPath()+"nonversion/";
	m_tmp_path = SystemTools::GetAppPath()+"tmp/";
	m_packagedownload_path = SystemTools::GetAppPath()+"package/";
	m_expansion_res_path = SystemTools::GetApkExpansionPath();
	m_log_path = m_nonversion_path;

#elif defined (WP8)
	m_initial_res_path = SystemTools::GetAppPackPath();
	m_download_path = SystemTools::GetAppPath()+"res/";
	m_nonversion_path = SystemTools::GetAppPath()+"nonversion/";
	m_tmp_path = SystemTools::GetAppPath()+"tmp/";
	m_packagedownload_path = SystemTools::GetAppPath()+"package/";
	m_log_path = m_nonversion_path;

#else
	m_initial_res_path = SystemTools::GetAppPackPath();
	m_download_path = SystemTools::GetAppPath()+"Library/res/";
	m_nonversion_path = SystemTools::GetAppPath()+"Library/nonversion/";
	m_tmp_path = SystemTools::GetAppPath()+"Library/tmp/";
	m_packagedownload_path = SystemTools::GetAppPath()+"Library/package/";
	m_expansion_res_path = SystemTools::GetApkExpansionPath();
	m_log_path = m_nonversion_path;
#endif

	m_initial_res_path = ConvertPathFormatToUnixStyle(m_initial_res_path);
	m_download_path = ConvertPathFormatToUnixStyle(m_download_path);
	m_nonversion_path = ConvertPathFormatToUnixStyle(m_nonversion_path);
	m_tmp_path = ConvertPathFormatToUnixStyle(m_tmp_path);
	m_packagedownload_path = ConvertPathFormatToUnixStyle(m_packagedownload_path);
	m_expansion_res_path = ConvertPathFormatToUnixStyle(m_expansion_res_path);
	m_log_path = ConvertPathFormatToUnixStyle(m_log_path);

	FileHelper::CreateDir(m_download_path);
	FileHelper::CreateDir(m_nonversion_path);
	FileHelper::CreateDir(m_tmp_path);
	FileHelper::CreateDir(m_packagedownload_path);
}

FileSystemEx::~FileSystemEx()
{
}

bool FileSystemEx::Start()
{
	//增加文件搜索路径

	//安装包下载目录
	cocos2d::CCFileUtils::getInstance()->addSearchPath(m_packagedownload_path, true);
	//随包资源目录
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	cocos2d::CCFileUtils::getInstance()->addSearchPath("assets/", true);
#else
	cocos2d::CCFileUtils::getInstance()->addSearchPath(m_initial_res_path, true);
#endif
	//下载资源目录
	if (LuaEngine::GetInstance()->GetConfig("IsLocalRes") == "true" && CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		cocos2d::CCFileUtils::getInstance()->addSearchPath(m_initial_res_path + "../../../cdnfiles/", true);
	}
	else
	{
		cocos2d::CCFileUtils::getInstance()->addSearchPath(m_download_path, true);
	}

	return true;
}

std::string FileSystemEx::GetInitialPath()
{
	return m_initial_res_path;
}

std::string FileSystemEx::GetExpansionPath()
{
	return m_expansion_res_path;
}

std::string FileSystemEx::GetNonVersionPath()
{
	return m_nonversion_path;
}

std::string FileSystemEx::GetTmpPath()
{
	return m_tmp_path;
}

std::string FileSystemEx::GetPackageDownloadPath()
{
	return m_packagedownload_path;
}

std::string FileSystemEx::GetDownLoadPath()
{
	return m_download_path;
}

std::string FileSystemEx::GetDownLoadResPath(const std::string &file_path)
{
	if (file_path.find(m_download_path) != std::string::npos)
		return file_path;

	return m_download_path + file_path;
}

std::string FileSystemEx::GetResourcePath(const std::string &file_path)
{
	return cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(file_path);
}

void FileSystemEx::SetAsyncMemLimit(unsigned int mem_limit)
{
	m_file_async.SetMemLimit(mem_limit);
#if defined(__ANDROID__)
	FileAsyncHandler::SetMemLimit(mem_limit);
#endif
}

void FileSystemEx::CancelAsync(unsigned int async_id)
{
	m_file_async.Cancel(async_id);
#if defined(__ANDROID__)
	FileAsyncHandler::Cancel(async_id);
#endif
}

void FileSystemEx::ClearPackageDownloadPath()
{
	if (!FileHelper::Remove(m_packagedownload_path))
	{
		FileHelper::CreateDir(m_packagedownload_path);
	}
}

bool FileSystemEx::IsDownloadPathFileExist(const std::string &file_path)
{
	if (ZQ::StrUtil::Trim(file_path)=="")
	{
		return false;
	}

	return FileHelper::IsExsit(m_download_path+file_path);
}

bool FileSystemEx::ReadInitialFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const
{
#if defined(__ANDROID__)
	return FileAsyncHandler::ReadFile(file_full_path, data_ptr, data_len);
#else
	std::string tmp_path = m_initial_res_path + file_full_path;
	return ReadFile(tmp_path, data_ptr, data_len);
#endif
}

//unsigned int FileSystemEx::ReadInitialFileAsync(const std::string &file_full_path, FileListener *listener)
//{
//#if defined(__ANDROID__)
//	FileAsync::FileInfo fi(FileAsync::FOT_READ, file_full_path, listener);
//	unsigned int async_id = GetAsyncID();
//	fi.async_id = async_id;
//	bool ret =  FileAsyncHandler::Add(fi);
//	if(ret)
//	{
//		return async_id;
//	}
//#else
//	std::string tmp_path = m_initial_res_path + file_full_path;
//	return ReadFileAsync(tmp_path, listener);
//#endif
//
//}

bool FileSystemEx::ReadDownloadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const
{
	std::string tmp_path = m_download_path + file_full_path;
	return ReadFile(tmp_path, data_ptr, data_len);
}

//unsigned int FileSystemEx::ReadDownloadFileAsync(const std::string &file_full_path, FileListener *listener)
//{
//	std::string tmp_path = m_download_path + file_full_path;
//	return ReadFileAsync(tmp_path, listener);
//}
//
//bool FileSystemEx::WriteDownloadFile(const std::string &file_full_path, const char *data_ptr, unsigned int data_len) const
//{
//	std::string tmp_path = m_download_path + file_full_path;
//	return WriteFile(tmp_path, data_ptr, data_len);
//}
//
//unsigned int FileSystemEx::WriteDownloadFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener)
//{
//	std::string tmp_path = m_download_path + file_full_path;
//	return WriteFileAsync(tmp_path, data_ptr, data_len, listener);
//}


bool FileSystemEx::CopyDirFromInitialToDownload(const std::string &src_path, const std::string &dest_path, bool cover) const
{
#if defined(__ANDROID__)
	return FileHelper::UnzipAndroidDirTo(m_initial_res_path, src_path, m_download_path + dest_path, cover);
#else
	std::string tmp_src_path = m_initial_res_path + src_path;
	std::string tmp_dest_path = m_download_path + dest_path;
	return FileHelper::CopyDir(tmp_src_path, tmp_dest_path, cover);
#endif
}

bool FileSystemEx::CopyFileFromInitialToDownload(const std::string &src_file_name, const std::string &dest_file_name, char** data_ptr, unsigned int *data_len) const
{
#if defined(__ANDROID__)
	std::string tmp_dest_path = m_download_path + dest_file_name;
	return FileAsyncHandler::Copy(src_file_name.c_str(), tmp_dest_path.c_str());
#else
	std::string tmp_src_path = m_initial_res_path + src_file_name;
	std::string tmp_dest_path = m_download_path + dest_file_name;
	return CopyFile(tmp_src_path, tmp_dest_path, data_ptr, data_len);
#endif
}

unsigned int FileSystemEx::CopyFileFromInitialToDownloadAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener)
{
#if defined(__ANDROID__)
	unsigned int async_id = GetAsyncID();
	std::string tmp_dest_path = m_download_path + dest_file_name;
	FileAsync::FileInfo fi(FileAsync::FOT_COPY, src_file_name, listener);
	fi.dest_path = tmp_dest_path;
	fi.async_id = async_id;
	FileAsyncHandler::Add(fi);
	return async_id;
#else
	std::string tmp_src_path = m_initial_res_path + src_file_name;
	std::string tmp_dest_path = m_download_path + dest_file_name;
	return CopyFileAsync(tmp_src_path, tmp_dest_path, listener);
#endif

}

bool FileSystemEx::CopyFileFromExpansionToDownload(const std::string &src_file_name, const std::string &dest_file_name, char** data_ptr, unsigned int *data_len) const
{
#if defined(__ANDROID__)
	void *buffer = 0;
	unsigned int len = 0;
	if(!FileHelper::UnzipFile(m_expansion_res_path, src_file_name, &buffer, &len))
	{
		LogUtil::LogError("[FileSystemEx::CopyFileFromExpansionToDownload UnzipFile error :%s]", src_file_name.c_str());
		return false;
	}

	std::string tmp_dest_path = m_download_path + dest_file_name;
	FileHelper::Write(tmp_dest_path, (const char*)buffer, len);
	free(buffer);
#else
	std::string tmp_src_path = m_expansion_res_path + src_file_name;
	std::string tmp_dest_path = m_download_path + dest_file_name;
	return CopyFile(tmp_src_path, tmp_dest_path, data_ptr, data_len);
#endif
	return true;
}

unsigned int FileSystemEx::CopyFileFromExpansionToDownloadAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener)
{
#if defined(__ANDROID__)
	unsigned int async_id = GetAsyncID();
	std::string tmp_dest_path = m_download_path + dest_file_name;
	FileAsync::FileInfo fi(FileAsync::FOT_UNZIP, src_file_name, listener);
	fi.dest_path = tmp_dest_path;
	fi.expansion_path = m_expansion_res_path;
	fi.async_id = async_id;
	FileAsyncHandler::Add(fi);
	return async_id;
#else
	std::string tmp_src_path = m_expansion_res_path + src_file_name;
	std::string tmp_dest_path = m_download_path + dest_file_name;
	return CopyFileAsync(tmp_src_path, tmp_dest_path, listener);
#endif
}

//
//bool FileSystemEx::ReadNonVersionFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const
//{
//	std::string tmp_path = m_nonversion_path + file_full_path;
//	return ReadFile(tmp_path, data_ptr, data_len);
//}
//
//unsigned int FileSystemEx::ReadNonVersionFileAsync(const std::string &file_full_path, FileListener *listener)
//{
//	std::string tmp_path = m_nonversion_path + file_full_path;
//	return ReadFileAsync(tmp_path, listener);
//}
//
//bool FileSystemEx::WriteNonVersionFile(const std::string &file_full_path, char* data_ptr, unsigned int data_len) const
//{
//	std::string tmp_path = m_nonversion_path + file_full_path;
//	return WriteFile(tmp_path, data_ptr, data_len);
//}
//
//unsigned int FileSystemEx::WriteNonVersionFileAsync(const std::string &file_full_path, char *data_ptr, unsigned int data_len, FileListener *listener)
//{
//	std::string tmp_path = m_nonversion_path + file_full_path;
//	return WriteFileAsync(tmp_path, data_ptr, data_len, listener);
//}
//
//bool FileSystemEx::ReadTmpFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const
//{
//	std::string tmp_path = m_tmp_path + file_full_path;
//	return ReadFile(tmp_path, data_ptr, data_len);
//}
//
//unsigned int FileSystemEx::ReadTmpFileAsync(const std::string &file_full_path, FileListener *listener)
//{
//	std::string tmp_path = m_tmp_path + file_full_path;
//	return ReadFileAsync(tmp_path, listener);
//}
//
//bool FileSystemEx::WriteTmpFile(const std::string &file_full_path, char* data_ptr, unsigned int data_len) const
//{
//	std::string tmp_path = m_tmp_path + file_full_path;
//	return WriteFile(tmp_path, data_ptr, data_len);
//}
//
//unsigned int FileSystemEx::WriteTmpFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener)
//{
//	std::string tmp_path = m_tmp_path + file_full_path;
//	return WriteFileAsync(tmp_path, data_ptr, data_len, listener);
//}


unsigned int FileSystemEx::LoadVersionFileAsync(FileListener *listener)
{
	FileAsync::FileInfo fi(FileAsync::FOT_VERSION_FILE_LOAD, m_initial_res_path, listener);
	fi.path = m_initial_res_path;
	fi.dest_path = m_download_path;
	fi.expansion_path = m_expansion_res_path;
	return m_file_async.Add(fi);
}

unsigned int FileSystemEx::WriteVersionFileAsync(const GameType::StrUIntKVMap &version_map)
{
	FileAsync::FileInfo fi(FileAsync::FOT_VERSION_FILE_WRITE, m_download_path, 0);
	fi.version_file_download = version_map;
	return m_file_async.Add(fi);
}

unsigned int FileSystemEx::WriteSettingFileAsync(const std::string &file_full_path, GameType::StrKVMap setting_map)
{
	std::string tmp_path = m_download_path + file_full_path;
	FileAsync::FileInfo fi(FileAsync::FOT_SETTING_FILE_WRITE, tmp_path, 0);
	fi.setting_file_specail_info = setting_map;
	return m_file_async.Add(fi);
}

unsigned int FileSystemEx::ReadScriptIosAsync(const GameType::StrVect &file_path_list, FileListener *listener)
{
	return UnzipPackageAsync(FileAsync::FOT_SCRIPT_IOS_UNZIP, file_path_list, listener);
}

unsigned int FileSystemEx::UnzipPackageAsync(FileAsync::FileOpType type, const GameType::StrVect &file_path_list, FileListener *listener, const std::string& dest_dir)
{
	FileAsync::FileInfo fi(type, "", listener);
	fi.unzip_file_list = file_path_list;
	if (dest_dir != "")
	{
		fi.dest_path = dest_dir;
	}
	return m_file_async.Add(fi);
}

unsigned int FileSystemEx::GetDataFrmFileAsync(const std::string &file_full_path, FileListener *listener)
{
	FileAsync::FileInfo fi(FileAsync::FOT_GET_DATA_FRM_FILE, file_full_path, listener);
	fi.max_retry_count = 2;
	return m_file_async.Add(fi);
}

bool FileSystemEx::ReadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const
{
	return FileHelper::Read(file_full_path, data_ptr, data_len);
}

unsigned int FileSystemEx::ReadFileAsync(const std::string &file_full_path, FileListener *listener)
{
	FileAsync::FileInfo fi(FileAsync::FOT_READ, file_full_path, listener);
	return m_file_async.Add(fi);
}

bool FileSystemEx::WriteFile(const std::string &file_full_path, const char* data_ptr, unsigned int data_len) const
{
	return FileHelper::Write(file_full_path, data_ptr, data_len);
}

unsigned int FileSystemEx::WriteFileAsync(const std::string &file_full_path, char *data_ptr, unsigned int data_len, FileListener *listener)
{
	FileAsync::FileInfo fi(FileAsync::FOT_WRITE, file_full_path, listener);
	fi.ptr = data_ptr;
	fi.len = data_len;
	return m_file_async.Add(fi);
}

bool FileSystemEx::CopyFile(const std::string &src_full_path, const std::string &dest_full_path, char **data_ptr, unsigned int *data_len) const
{
	return FileHelper::Copy(src_full_path, dest_full_path, data_ptr, data_len);
}

unsigned int FileSystemEx::CopyFileAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener)
{
	FileAsync::FileInfo fi(FileAsync::FOT_COPY, src_file_name, listener);
	fi.dest_path = dest_file_name;
	return m_file_async.Add(fi);
}

bool FileSystemEx::Update(unsigned int now_time)
{
	FileAsync::FileInfoList tmp_file_list = m_file_async.Get();
	for (FileAsync::FileInfoList::iterator iter = tmp_file_list.begin(); iter != tmp_file_list.end(); ++iter)
	{
		if (iter->listener == 0)
			continue;

		if (iter->op_result != FileAsync::FOR_SUCC && iter->CanRetry())
		{
			++iter->retry_count;
			m_file_async.Add(*iter);
		}
		else
		{
			iter->listener->OnCallBack(iter->async_id, *iter);
			iter->listener->Free();
		}
	}
#if defined(__ANDROID__)
	FileAsync::FileInfoList tmp_android_file_list = FileAsyncHandler::Get();
	for (FileAsync::FileInfoList::const_iterator iter = tmp_android_file_list.begin(); iter != tmp_android_file_list.end(); ++iter)
	{
		if (iter->listener == 0) continue;

		iter->listener->OnCallBack(iter->async_id, *iter);
		iter->listener->Free();
	}
#endif
	FileVersion::GetInstance()->Save(now_time);
	Setting::GetInstance()->Save(now_time);

	return true;
}

bool FileSystemEx::Stop()
{
	m_file_async.Stop();

	return true;
}

bool FileSystemEx::Restart()
{
	FileVersion::GetInstance()->Restart();
	return true;
}

std::string FileSystemEx::GetLogPath()
{
	return m_log_path;
}

std::string FileSystemEx::ConvertPathFormatToUnixStyle(const std::string& path)
{
	std::string ret = path;
	int len = ret.length();
	for (int i = 0; i < len; ++i)
	{
		if (ret[i] == '\\')
		{
			ret[i] = '/';
		}
	}
	return ret;
}