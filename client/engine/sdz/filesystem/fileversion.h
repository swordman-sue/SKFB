
#ifndef FILEVERSION_H
#define FILEVERSION_H

#include "zqcommon/typedef.h"
#include <string>
#include <map>

class FileAsync;

// 版本管理模块，这个模块本身不负责资源下载
class FileVersion
{
public:
	enum VERSIONSTATUS
	{
		VS_INVALID = 0,
		VS_INIT_NEW,				// 安装包里面是最新的
		VS_INIT_EX_NEW,				// 拓展包里的是最新的
		VS_DOWNLOAD_NEW,			// 下载目录里面是最新的
		VS_SERVER_NEW,				// 服务器资源是最新的
		VS_NOT_EXIST,				// 文件不存在

		VS_CONUT,
	};
	enum APPVERSIONSTATUS
	{
		AVS_INVALID = 0,
		AVS_UNUPDATE,			// 无需更新
		AVS_NEED_UPDATE,		// 必须更新
		AVS_CAN_IGNORE,			// 可以忽略的更新

		AVS_CONUT,
	};
	struct FileVerInfo
	{
		FileVerInfo() :status(AVS_INVALID)
					, version(0)
					, size(0)
		{}

		std::string		file_name;
		int				status;
		unsigned int	version;
		unsigned int    size;
	};

	typedef std::map<std::string, FileVerInfo> FilePathToVersionInfoMap;
	typedef std::map<std::string, FileVerInfo>::const_iterator FilePathToVersionInfoMap_CIter;

	typedef std::vector<std::string> FilePathVec;
	typedef std::vector<std::string>::iterator FilePathVec_Iter;
	typedef std::map<int, FilePathVec>	 FileLevelToPathMap;
	typedef std::map<int, FilePathVec>::iterator FileLevelToPathMap_Iter;

public:
	static FileVersion *GetInstance()
	{
		static FileVersion instance;
		return &instance;
	}
	~FileVersion(){};

private:
	FileVersion() :m_is_loaded(false), m_is_changed(false), m_last_save_time(0){}

public:
	bool Load(const GameType::StrUIntKVMap &init_version_map, 
		const GameType::StrUIntKVMap &download_version_map, 
		const FilePathToVersionInfoMap &server_version_map,
		const std::string &app_version, const std::string &svn_version,
		const GameType::StrUIntKVMap &expansion_version_map);

	void Restart() { m_is_loaded = false; }

public:
	int CheckAppVersion();
	void UpdateVersion(std::string file_path, unsigned int version);
	FileVerInfo CheckFileVersion(const std::string& file_path);
	//获取传入的列表中的每个文件的版本
	FilePathToVersionInfoMap CheckFileListVersion(const GameType::StrVect &file_path);
	unsigned int GetFileSize(const std::string& file_path) const;
	//获取分隔的脚本代码文件列表
	GameType::StrVect GetScriptIosFileList()const { return m_script_ios_list; }
	//获取需要下载的脚本代码文件列
	void GetNeedToDownloadScriptIosVersion(std::vector<FileVerInfo> &ver_info);
	void Save(unsigned int now_time);
	//获取所有文件的本地版本
	FilePathToVersionInfoMap GetAllFileVersion();

	//本地下载文件版本号是否最新
	bool IsDownLoadFileVersionNew(const std::string& file_path);
	//安装包文件版本号是否最新
	bool IsInitFileVersionNew(const std::string& file_path);
	//拓展包文件版本号是否最新
	bool IsExpansionVersionNew(const std::string& file_path);

	//本地下载文件版本号是否存在
	bool IsDownLoadFileVersionExist(const std::string& file_path);

	//获取远程svn版本号
	std::string GetRemoteSvnVersion(){return m_server_svn_version;}

	//获取远程应用版本号
	std::string GetRemoteAppVersion(){return m_server_app_version;}

	//获取本地version.ios的md5码
	std::string GetLocalVersionMd5();

	//更新已经下载文件列表m_download_version_map
	void UpdateDownloadVersion(const GameType::StrUIntKVMap& file_list);

	bool IsLoaded() { return m_is_loaded; }

private:
	bool m_is_loaded;									// 是否完成加载
	bool m_is_changed;									// 是否变更，作为判断是否保存的脏位
	unsigned int m_last_save_time;						// 上次保存时间
	std::string m_server_app_version;					// 服务器版本
	std::string m_server_svn_version;					// svn版本

	GameType::StrVect m_script_ios_list;

	GameType::StrUIntKVMap m_init_version_map;			// 安装包文件的版本号
	GameType::StrUIntKVMap m_download_version_map;		// 已下载文件的版本号
	FilePathToVersionInfoMap m_server_version_map;		// 服务器文件的FileVerInfo
	GameType::StrUIntKVMap m_expansion_map;				// google play拓展包版本
};

#endif
