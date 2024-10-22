

/************************************************************************/
/* 
 * Ŀ¼�ṹ˵��
 * 
 * ƻ��IOS
 *		<Home>						SystemTools::GetAppPath													[ϵͳ]Ӧ�ð�װĿ¼
 *		<Home>/AppName.app			SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			{��Ϸ}��Դ��Ŀ¼	[ϵͳ]����Ŀ¼�������޸ģ�iTunes������
 *		<Home>/Documents/																					[ϵͳ]����Ŀ¼��iTunes���ݡ�
 *		<Home>/Documents/package									FileSystemEx::GetPackageDownloadPath	{��Ϸ}�°汾��װ������Ŀ¼
 *		<Home>/Library/Preferences																			[ϵͳ]ƫ������Ŀ¼���ļ�ͨ��NSUserDefaults���CFPreferences API����д��iTunes���ݡ�
 *		<Home>/Library/Caches																				[ϵͳ]����cacheĿ¼���ɶ�д��iTunes�����ݣ�ˢ���ָ�ʱ�������
 *		<Home>/Library/res											FileSystemEx::GetDownLoadPath			{��Ϸ}��Դ����Ŀ¼
 *		<Home>/Library/nonversion									FileSystemEx::GetNonVersionPath			{��Ϸ}�ް汾�ļ�Ŀ¼
 *		<Home>/tmp/													FileSystemEx::GetTmpPath				{��Ϸ}��ʱ�ļ�Ŀ¼ [ϵͳ]����ʱ��ʱĿ¼���ɶ�д��iTunes�����ݣ������˳��󣬿��ܱ������
 *	
 *	��׿ANDROID
 *		/data/app/app_url_name.apk		SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			{��Ϸ}��ʼ��Դ��·��	[ϵͳ]����װ�ļ�(�����ļ�)
 *		/data/data/app_url_name/		SystemTools::GetAppPath													[ϵͳ]�����дĿ¼
 *		/data/data/app_url_name/										FileSystemEx::GetDownLoadPath			{��Ϸ}��Դ����Ŀ¼
 *		/data/data/app_url_name/tmp										FileSystemEx::GetTmpPath				{��Ϸ}��ʱ�ļ�Ŀ¼
 *		/data/data/app_url_name/nonversion								FileSystemEx::GetNonVersionPath			{��Ϸ}�ް汾�ļ�Ŀ¼
 *		/data/data/app_url_name/package									FileSystemEx::GetPackageDownloadPath	{��Ϸ}�°汾��װ������Ŀ¼
 *		/mnt/asec/app_url_name-1				[ϵͳ]Ӧ���Ƶ�sd����Ĵ��Ŀ¼
 *		/mnt/secure/asec/app_url_name-1.asec	[ϵͳ]Ӧ���Ƶ�sd�������ɵļ����ļ�
 *	
 *	WP8
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install\assets	SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			{��Ϸ}��ʼ��Դ��·��	[ϵͳ]����װ�ļ�(�����ļ�)
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install\		SystemTools::GetAppPath													[ϵͳ]�����дĿ¼
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install\res										FileSystemEx::GetDownLoadPath			{��Ϸ}��Դ����Ŀ¼
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install/tmp										FileSystemEx::GetTmpPath				{��Ϸ}��ʱ�ļ�Ŀ¼
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install/nonversion								FileSystemEx::GetNonVersionPath			{��Ϸ}�ް汾�ļ�Ŀ¼
 *		C:\Data\Programs\{2AC777CA-2D3B-41C8-98AC-B6ED1A4B0988}\Install/package									FileSystemEx::GetPackageDownloadPath	{��Ϸ}�°汾��װ������Ŀ¼
 *		
 *		
 * WIN�������� �� WINģ����
 *		<Home>						SystemTools::GetAppPath													��װĿ¼
 *		<Home>/sdz_app				SystemTools::GetAppPackPath		FileSystemEx::GetInitialResPath			��ʼ��ԴĿ¼
 *		<Home>/Library/res											FileSystemEx::GetDownLoadPath			��Դ����Ŀ¼
 *		<Home>/Library/tmp											FileSystemEx::GetTmpPath				��ʱ�ļ�Ŀ¼
 *		<Home>/Library/nonversion									FileSystemEx::GetNonVersionPath			�ް汾�ļ�Ŀ¼
 *		<Home>/Library/package										FileSystemEx::GetPackageDownloadPath	{��Ϸ}�°汾��װ������Ŀ¼
 *		<Home>/script																						�ű�Ŀ¼��������������
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
	// ��ȡһЩ����Ŀ¼
	std::string GetInitialPath();
	std::string GetExpansionPath();
	std::string GetDownLoadPath();
	std::string GetNonVersionPath();
	std::string GetTmpPath();
	std::string GetLogPath();
	//��ȡ��װ������·��
	std::string GetPackageDownloadPath();

	std::string GetResourcePath(const std::string &file_path);
	std::string GetDownLoadResPath(const std::string &file_path);

	void SetAsyncMemLimit(unsigned int mem_limit);	// �����첽�����ڴ�
	void CancelAsync(unsigned int async_id);		// ȡ���첽����
	void ClearPackageDownloadPath();				// �������и���ʱ����

// ����ӿ�û��android��ʵ�֣�Ϊ�˱������ã�ֱ�Ӹɵ�����
//#ifndef __ANDDROID__
//	// �жϰ�װ���ļ��Ƿ���ڣ�android�жϵĻ�����̫��
//	bool IsInitialPathFileExist(const std::string &file_path);
//#endif

	bool IsDownloadPathFileExist(const std::string &file_path);

	// ��ȡ��ʼ��װ���ļ�
	bool ReadInitialFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadInitialFileAsync(const std::string &file_full_path, FileListener *listener);

	// ��д����Ŀ¼�ļ�
	bool ReadDownloadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadDownloadFileAsync(const std::string &file_full_path, FileListener *listener);
//	bool WriteDownloadFile(const std::string &file_full_path, const char* data_ptr, unsigned int data_len) const;
//	unsigned int WriteDownloadFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);

	// �����ļ���
	bool CopyDirFromInitialToDownload(const std::string &src_path, const std::string &dest_path, bool cover = false) const;

	// ���Ƴ�ʼ��װ����Դ������Ŀ¼
	bool CopyFileFromInitialToDownload(const std::string &src_file_name, const std::string &dest_file_name, char** data_ptr, unsigned int *data_len) const;
	unsigned int CopyFileFromInitialToDownloadAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener);

	// ������չ����Դ������Ŀ¼
	bool CopyFileFromExpansionToDownload(const std::string &src_file_name, const std::string &dest_file_name, char** data_ptr, unsigned int *data_len) const;
	unsigned int CopyFileFromExpansionToDownloadAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener);

	// ��д�ް汾�ļ�
//	bool ReadNonVersionFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadNonVersionFileAsync(const std::string &file_full_path, FileListener *listener);
//	bool WriteNonVersionFile(const std::string &file_full_path, char* data_ptr, unsigned int data_len) const;
//	unsigned int WriteNonVersionFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);

	// ��д��ʱ�ļ�
//	bool ReadTmpFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
//	unsigned int ReadTmpFileAsync(const std::string &file_full_path, FileListener *listener);
//	bool WriteTmpFile(const std::string &file_full_path, char* data_ptr, unsigned int data_len) const;
//	unsigned int WriteTmpFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);

	// ��д�����κ��ļ�
	bool ReadFile(const std::string &file_full_path, char** data_ptr, unsigned int *data_len) const;
	unsigned int ReadFileAsync(const std::string &file_full_path, FileListener *listener);
	bool WriteFile(const std::string &file_full_path, const char* data_ptr, unsigned int data_len) const;
	unsigned int WriteFileAsync(const std::string &file_full_path, char* data_ptr, unsigned int data_len, FileListener *listener);
	bool CopyFile(const std::string &src_full_path, const std::string &dest_full_path, char **data_ptr, unsigned int *data_len) const;
	unsigned int CopyFileAsync(const std::string &src_file_name, const std::string &dest_file_name, FileListener *listener);

	// ����ӿ�
	unsigned int LoadVersionFileAsync(FileListener *listener);
	unsigned int WriteVersionFileAsync(const GameType::StrUIntKVMap &version_map);
	unsigned int WriteSettingFileAsync(const std::string &file_full_path, GameType::StrKVMap setting_map);
	unsigned int ReadScriptIosAsync(const GameType::StrVect &file_path_list, FileListener *listener);
	unsigned int UnzipPackageAsync(FileAsync::FileOpType type, const GameType::StrVect &file_path_list, FileListener *listener, const std::string& dest_dir = "");
	unsigned int GetDataFrmFileAsync(const std::string &file_full_path, FileListener *listener);

	// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
	std::string ConvertPathFormatToUnixStyle(const std::string& path);
	
private:
	std::string m_initial_res_path;		// �����Դ·��
	std::string m_download_path;		// ������Դ���Ŀ¼
	std::string m_nonversion_path;		// �ް汾�ļ�Ŀ¼���ɳ����Լ������Ƿ����
	std::string m_tmp_path;				// ��ʱ�ļ�Ŀ¼��ϵͳ���ܻ�������
	std::string m_packagedownload_path;	// ��װ������Ŀ¼
	std::string m_expansion_res_path;	// ��չ��Դ��·��
	std::string m_log_path;				// ��־·��

	FileAsync m_file_async;
};

#endif
