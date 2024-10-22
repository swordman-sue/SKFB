#ifndef INSTALLER_H
#define INSTALLER_H

#include "zqcommon/typedef.h"
#include "luatinker/lua_tinker.h"

class InstallerHttpLister;

//更新包下载回调接口
class NewPackageInfoInterface
{
public:
	virtual void OnDownLoadFinish_GetNewPackageInfo(bool result, int ret_code, const char* data_ptr, unsigned int data_len) = 0;
	virtual void OnDownLoadFinish_NewPackage(bool result, int ret_code, const std::string &package_name) = 0;
};

class Installer
{
	friend class InstallerHttpLister;
private:
	Installer(){};

public:
	static void GotoDownloadUrl();
	static void RequestPackageUrl(NewPackageInfoInterface* package_interface);
	static bool GetDownloadUrl(bool result, int ret_code, const char *data_ptr, unsigned int data_len, std::string& url, std::string& file_name);

	static unsigned int GetNewPackInfo(const std::string& url, unsigned int time_out, unsigned int lua_callback_func_id, NewPackageInfoInterface* package_interface);
	static unsigned int GetNewPackInfoForLua(const std::string& url, unsigned int time_out, unsigned int lua_callback_func_id);
	
	static bool CheckHasDownload(std::string package_name);
	static unsigned int DownloadNewPack(const std::string& url, unsigned int time_out, const std::string &save_file_name, unsigned int lua_callback_func_id, NewPackageInfoInterface* package_interface);
	static unsigned int DownloadNewPackForLua(const std::string& url, unsigned int time_out, const std::string &save_file_name, unsigned int lua_callback_func_id);
	static bool CheckDownloadProgress(unsigned int http_id, unsigned int &total_len, unsigned  int &cur_len);
	static bool CheckDownloadProgressForLua(unsigned int http_id, lua_tinker::table progress_params);
	static bool Install(const std::string &filename);
	static void Cancle(unsigned int http_id);

private:
	static void OnFinish_GetNewPackInfo(unsigned int http_id, unsigned int func_id, NewPackageInfoInterface* package_interface, bool is_abort, bool result, int ret_code, const char *data_ptr, unsigned int data_len);
	static void OnFinish_DownloadNewPack(unsigned int http_id, unsigned int func_id, NewPackageInfoInterface* package_interface, bool is_abort, bool result, int ret_code, const std::string &save_file_name);
};

#endif
