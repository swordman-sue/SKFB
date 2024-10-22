

/************************************************************************/
/* 
 * 目录结构说明
 * 
 * 苹果IOS
 *		<Home>						SystemTools::GetAppPath													[系统]应用安装目录
 *		<Home>/AppName.app			SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			{游戏}资源包目录	[系统]程序目录，不可修改，iTunes不备份
 *		<Home>/Documents/																					[系统]数据目录，iTunes备份。
 *		<Home>/Documents/package									FileSystemEx::GetPackageDownloadPath	{游戏}新版本安装包下载目录
 *		<Home>/Library/Preferences																			[系统]偏好设置目录，文件通过NSUserDefaults类或CFPreferences API来读写，iTunes备份。
 *		<Home>/Library/Caches																				[系统]数据cache目录，可读写，iTunes不备份，刷机恢复时被清除。
 *		<Home>/Library/res											FileSystemEx::GetDownLoadPath			{游戏}资源下载目录
 *		<Home>/Library/nonversion									FileSystemEx::GetNonVersionPath			{游戏}无版本文件目录
 *		<Home>/tmp/													FileSystemEx::GetTmpPath				{游戏}临时文件目录 [系统]运行时临时目录，可读写，iTunes不备份，程序退出后，可能被清除。
 *	
 *	安卓ANDROID
 *		/data/app/app_url_name.apk		SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			{游戏}初始资源包路径	[系统]程序安装文件(程序文件)
 *		/data/data/app_url_name/		SystemTools::GetAppPath													[系统]程序可写目录
 *		/data/data/app_url_name/										FileSystemEx::GetDownLoadPath			{游戏}资源下载目录
 *		/data/data/app_url_name/tmp										FileSystemEx::GetTmpPath				{游戏}临时文件目录
 *		/data/data/app_url_name/nonversion								FileSystemEx::GetNonVersionPath			{游戏}无版本文件目录
 *		/data/data/app_url_name/package									FileSystemEx::GetPackageDownloadPath	{游戏}新版本安装包下载目录
 *		/mnt/asec/app_url_name-1				[系统]应用移到sd卡后的存放目录
 *		/mnt/secure/asec/app_url_name-1.asec	[系统]应用移到sd卡后生成的加密文件
 *	
 *	WP8
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install\assets	SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			{游戏}初始资源包路径	[系统]程序安装文件(程序文件)
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install\		SystemTools::GetAppPath													[系统]程序可写目录
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install\res										FileSystemEx::GetDownLoadPath			{游戏}资源下载目录
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install/tmp										FileSystemEx::GetTmpPath				{游戏}临时文件目录
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install/nonversion								FileSystemEx::GetNonVersionPath			{游戏}无版本文件目录
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install/package									FileSystemEx::GetPackageDownloadPath	{游戏}新版本安装包下载目录
 *		
 *		
 * WIN开发环境 与 WIN模拟器
 *		<Home>						SystemTools::GetAppPath													安装目录
 *		<Home>/sdz_app				SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			初始资源目录
 *		<Home>/Library/res											FileSystemEx::GetDownLoadPath			资源下载目录
 *		<Home>/Library/tmp											FileSystemEx::GetTmpPath				临时文件目录
 *		<Home>/Library/nonversion									FileSystemEx::GetNonVersionPath			无版本文件目录
 *		<Home>/Library/package										FileSystemEx::GetPackageDownloadPath	{游戏}新版本安装包下载目录
 *		<Home>/script																						脚本目录【仅开发环境】
 * 
 * */
/************************************************************************/


#ifndef FILESYSTEMEX_H
#define FILESYSTEMEX_H

#include "fileasync.h"
#include <string>
#include <map>

class FileListener
{
public:
	virtual void OnCallBack(unsigned int async_id, const FileAsync::FileInfo& file_info) = 0;
	virtual void Free()=0;
};

class FileSystemEx
{
public:
	static FileSystemEx* GetInstance();
	static void DelInstance();
	~FileSystemEx();

private:
	FileSystemEx();

public:
	bool Init(){return true;};
	bool Start();
	bool Update(unsigned int now_time);
	bool Stop();
	bool Release(){return true;};
	bool Pause(){return true;};
	bool Resume(){return true;};
	bool Restart();

public:
	// 获取一些基础目录
	std::string GetInitialPath();
	std::string GetExpansionPath();
	std::string GetDownLoadPath();
	std::string GetNonVersionPath();
	std::string GetTmpPath();
	std::string GetLogPath();
	//获取安装包下载路径
	std::string GetPackageDownloadPath();

	std::string GetResourcePath(const std::string &file_path);
	std::string GetDownLoadResPath(const std::string &file_path);

	void SetAsyncMemLimit(unsigned int mem_limit);	// 设置异步可用内存
	void CancelAsync(unsigned int async_id);		// 取消异步操作
	void ClearPackageDownloadPath();				// 不能在有更新时操作

// 这个接口没在android下实现，为了避免误用，直接干掉算了
//#ifndef __ANDDROID__
//	// 判断安装包文件是否存在，android判断的话开销太大
//	bool IsInitialPathFileExist(const std::string &file_path);
//#endif

	bool IsDownloadPathFileExist(const std::string &file_path);

	// 读取初始安装包文件
	bool ReadInitialFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadInitialFileAsync(const std::string &file_full_path, FileListener *listener);

	// 读写下载目录文件
	bool ReadDownloadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadDownloadFileAsync(const std::string &file_full_path, FileListener *listener);
//	bool WriteDownloadFile(const std::string &file_full_path, const char* data_ptr, unsigned int data_len) const;
//	unsigned int WriteDownloadFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);

	// 复制文件夹
	bool CopyDirFromInitialToDownload(const std::string &src_path, const std::string &dest_path, bool cover = false) const;

	// 复制初始安装包资源到下载目录
	bool CopyFileFromInitialToDownload(const std::string &src_file_name, const std::string &dest_file_name, char** data_ptr, unsigned int *data_len) const;
	unsigned int CopyFileFromInitialToDownloadAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener);

	// 复制扩展包资源到下载目录
	bool CopyFileFromExpansionToDownload(const std::string &src_file_name, const std::string &dest_file_name, char** data_ptr, unsigned int *data_len) const;
	unsigned int CopyFileFromExpansionToDownloadAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener);

	// 读写无版本文件
//	bool ReadNonVersionFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadNonVersionFileAsync(const std::string &file_full_path, FileListener *listener);
//	bool WriteNonVersionFile(const std::string &file_full_path, char* data_ptr, unsigned int data_len) const;
//	unsigned int WriteNonVersionFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);

	// 读写临时文件
//	bool ReadTmpFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadTmpFileAsync(const std::string &file_full_path, FileListener *listener);
//	bool WriteTmpFile(const std::string &file_full_path, char* data_ptr, unsigned int data_len) const;
//	unsigned int WriteTmpFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);

	// 读写复制任何文件
	bool ReadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
	unsigned int ReadFileAsync(const std::string &file_full_path, FileListener *listener);
	bool WriteFile(const std::string &file_full_path, const char* data_ptr, unsigned int data_len) const;
	unsigned int WriteFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);
	bool CopyFile(const std::string &src_full_path, const std::string &dest_full_path, char **data_ptr, unsigned int *data_len) const;
	unsigned int CopyFileAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener);

	// 特殊接口
	unsigned int LoadVersionFileAsync(FileListener *listener);
	unsigned int WriteVersionFileAsync(const GameType::StrUIntKVMap &version_map);
	unsigned int WriteSettingFileAsync(const std::string &file_full_path, GameType::StrKVMap setting_map);
	unsigned int ReadScriptIosAsync(const GameType::StrVect &file_path_list, FileListener *listener);
	unsigned int UnzipPackageAsync(FileAsync::FileOpType type, const GameType::StrVect &file_path_list, FileListener *listener, const std::string& dest_dir = "");
	unsigned int GetDataFrmFileAsync(const std::string &file_full_path, FileListener *listener);

	// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
	std::string ConvertPathFormatToUnixStyle(const std::string& path);
	
private:
	std::string m_initial_res_path;		// 随包资源路径
	std::string m_download_path;		// 下载资源存放目录
	std::string m_nonversion_path;		// 无版本文件目录，由程序自己管理是否清除
	std::string m_tmp_path;				// 临时文件目录，系统可能会进行清楚
	std::string m_packagedownload_path;	// 安装包下载目录
	std::string m_expansion_res_path;	// 拓展资源包路径
	std::string m_log_path;				// 日志路径

	FileAsync m_file_async;
};

#endif
