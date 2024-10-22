#ifndef STARTLOGIC_H
#define STARTLOGIC_H

#include "filesystem/fileversion.h"
#include "zqcommon/typedef.h"
#include "httpmanager/httpmanager.h"
#include "resmanager/installer.h"
#include "platform/systemtools.h"
#include <queue>
#include <map>

class StartLogicDownLoadListener;
class LoadScriptIosListener;
class LoadVersionIosListener;

class StartLogic : public NewPackageInfoInterface
{
	friend class StartLogicDownLoadListener;
	friend class LoadScriptIosListener;
	friend class LoadVersionIosListener;

public:
	enum LogicProgress
	{
		LP_Invalid = 0,
		LP_Cdn_DownLoading,
		LP_VersionIos_DownLoading,
		LP_VersionIos_Loading,
		LP_NewPackage_WaitingForCheck,
		LP_NewPackage_GetInfo,
		LP_NewPackage_DownLoading,
		LP_ResPackage_DownLoading,
		LP_ResPackage_Unziping,
		LP_ScriptIos_DownLoading,
		LP_ScriptIos_Loading,
		LP_EnterWithoutUpdate,

		LP_StopProgressGoing,
		LP_COUNT,
	};
	enum SubProgress
	{
		SP_Invalid = 0,
		SP_Start,
		SP_End_Fail,
		SP_End_Succ,
	};

	struct LogicProgressItem
	{
	public:
		LogicProgressItem():progresss(LP_Invalid),sub_progress(SP_Invalid)
		{}

		LogicProgressItem(LogicProgress p, SubProgress sp, ZQ::INT32 rt = 0, ZQ::INT32 hrt = 0, ZQ::INT32 oc = 0, const char* ocd = ""):
			progresss(p),
			sub_progress(sp), 
			ret_code(rt),
			http_ret_code(hrt),
			op_code(oc),
			op_code_desc(ocd)
		{}

		LogicProgress progresss;
		SubProgress sub_progress;
		ZQ::INT32 ret_code;
		ZQ::INT32 http_ret_code;
		ZQ::INT32 op_code;
		std::string op_code_desc;
	};

private:
	struct ResPackageInfo
	{
		std::string name;
		std::string md5;
		int size;
		ResPackageInfo(const std::string& n, const std::string& m, int s) :
			name(n), md5(m), size(s)
		{}
	};

public:
	StartLogic();
	~StartLogic();
	void Start();
	void Restart(LogicProgress logic_progress);
	bool IsStarted() { return m_bStarted; }
	void update(float delta);

	bool GetLogicProgress(LogicProgressItem& item);
	void CancelCurProgress();

	// 由界面调用
	void StartDownLoad_NewPackage();		

	// 如果版本可以不用强更，也可能被界面调用
	void CheckScriptIos();						

	// 获取下载安装包文件的进度
	bool GetDownLoadNewPackageProgress(unsigned int &http_id, unsigned int &total_len, unsigned  int &cur_len);	

	bool GetCurDownLoadProgress(unsigned int &http_id, unsigned int &total_len, unsigned  int &cur_len);
	void InstallSteupPackage();
	unsigned int GetNeedDownloadScriptNum(){ return m_need_download_script_ios_list.size(); }
	unsigned int GetHadDownloadScriptIndex();
	unsigned int GetNeedDownloadResPackage();
	unsigned int GetHadDwonloadResPackage();
	void DeleteResPackage();
	void StartDownLoad_ResPackage();
	void OnUnzipFinish_ResPackage(bool result, const GameType::StrVect& unzip_filelist);
    
	void CheckNetworkState();

private:
	void StartDownLoad_Cdn();
	void OnDownLoadFinish_Cdn(bool result, int ret_code, int http_ret_code, int op_code, const char* data_ptr, unsigned int len);

	unsigned int GetVersionIosDownloadProGress();
	void StartDownLoad_VersionIos(bool ignoreCheckExamine = false);
	void OnDownLoadFinish_VersionIos(bool result, int ret_code, int http_ret_code, int op_code, const char* data_ptr, unsigned int len);

	void StartLoad_VersionIos(bool ignoreCheckExamine = false);
	void OnLoadFinish_VersionIos(bool ret, int op_result, const std::string& op_result_desc);

	void OnDownLoadFinish_GetNewPackageInfo(bool result, int ret_code, const char* data_ptr, unsigned int data_len);
	void OnDownLoadFinish_NewPackage(bool result, int ret_code, const std::string & package_name);

	void OnDownLoadFinish_ResPackage(bool result, int ret_code, int http_ret_code, int op_code, const std::string& package_name, const std::string& md5);
	void StartUnzipResPackage();

	void StartDownLoad_ScriptIos(const std::string& script_name, unsigned int size, unsigned int version);
	void OnDownLoadFinish_ScriptIos(bool result, int ret_code, int http_ret_code, int op_code, const std::string &script_ios_name, unsigned int script_ios_version, const char* data_ptr, unsigned int len);

	void StartUnzip_ScriptIos();
	void OnUnzipFinish_ScriptIos(bool result, const std::string& unzip_error_scriptios = "");

	void CheckNetworkValid(float delta);

private:
	static void OnDownloadResPackageDlgMsg(int);
    static void OnStartDownLoadVersionIosDlgMsg(int);
    static void OnStartLoadVersionIosDlgMsg(int);

private:
	static StartLogic* m_start_logic;

private:
	bool m_bStarted;
	unsigned int m_cdn_http_id;
	unsigned int m_md5_http_id;
	std::string m_md5_str;
	unsigned int m_version_ios_http_id;
	unsigned int m_new_package_http_id;
	unsigned int m_script_ios_http_id;
	unsigned int m_res_package_http_id;
	unsigned int *m_cur_http_id;
    std::string m_steup_filename;

	std::queue<LogicProgressItem> m_progress_queue;
	std::vector<FileVersion::FileVerInfo> m_need_download_script_ios_list;
	unsigned int m_had_download_script_index;
	std::vector<ResPackageInfo> m_need_download_res_package;
	unsigned int m_had_download_respackage_index;
	std::vector<std::string> m_need_unzip_res_package_list;
	bool m_bCanResPackageAlert;
	LogicProgress m_cur_progress;

    float m_checkNetworkValidDelayTime;
	float m_checkNetworkValidElapseTime;
	bool m_bInNetworkAlert;

	SystemTools::NETWORKSTATE m_eNetworkState;
};

#endif
