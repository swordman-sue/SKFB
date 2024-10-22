
#ifndef FILEASYNC_H
#define FILEASYNC_H

#include "zqcommon/typedef.h"
#include "filesystem/fileversion.h"
#include "common/platform/thread/mutex.h"
#include "common/platform/thread/thread.h"
#include "common/platform/thread/semaphorex.h"
#include "base/CCData.h"

unsigned int GetAsyncID();
DWORD FileHandleFunc(void* pData);
class FileListener;
class Zip;
class TiXmlDocument;

class FileAsync
{
	friend DWORD FileHandleFunc(void* pData);

public:
	FileAsync();
	~FileAsync();

	enum FileOpType
	{
		FOT_INVALID = 0,
		FOT_READ,
		FOT_WRITE,
		FOT_COPY,
		FOT_VERSION_FILE_LOAD,		// Ϊ�汾�ļ����ض��ӵ�
		FOT_VERSION_FILE_WRITE,		// Ϊ�汾�ļ����ӵ�
		FOT_SETTING_FILE_WRITE,		// Ϊ�����ļ����ӵ�
		FOT_SCRIPT_IOS_UNZIP,		// Ϊ��ѹscript.ios���ӵ�
		FOT_UNZIP,					// ��ѹ��չ����Դ
		FOT_CLOSE_ZIP_FILE,			// �ر���չ��(ֻ��԰�׿)
		FOT_UNZIP_RESPACKAGE,		// ��ѹ������Դ��
		FOT_GET_DATA_FRM_FILE,		// ΪFileUtils::getDataFromFile���ӵ�

		FOT_CONUT,
	};

	enum FileOpResult
	{
		FOR_SUCC = 0,
		FOR_OP_TYPE_ERR,
		FOR_READ_ERR,
		FOR_WRITE_ERR,
		FOR_COPY_ERR,
		FOR_VERSION_FILE_LOAD_ERR,
		FOR_VERSION_FILE_WRITE_ERR,
		FOR_SETTING_FILE_WRITE_ERR,
        FOR_SCRIPT_IOS_LOAD_ERR,
		FOR_SCRIPT_IOS_UNZIP_ERR,
		FOR_UNZIP_ERR,
		FOR_UNZIP_FILE_INVALID_ERR,
		FOR_UNZIP_IO_ERR,
		FOR_UNZIP_FILENOTFOUND_ERR,
		FOR_OPEN_ZIP_ERR,
		FOR_CLOSE_ZIP_ERR,
		FOR_GET_DATA_FRM_FILE_ERR,

		FOR_CONUT,
	};

	struct FileInfo 
	{
		FileInfo(FileOpType type, std::string p, FileListener *l)
			:op_type(type)
			,op_result(FOR_SUCC)
			,async_id(0)
			,listener(l)
			,path(p)
			,ptr(0)
			,len(0)
			,retry_count(0)
			,max_retry_count(0)
		{
		}
		
		FileInfo(unsigned int async_id)
			:op_type(FOT_INVALID)
			,op_result(FOR_SUCC)
			,async_id(async_id)
			,listener(0)
			,ptr(0)
			,len(0)
			,retry_count(0)
			,max_retry_count(0)
		{
		}

		bool operator==(const FileInfo &fi) const
		{
			return async_id == fi.async_id;
		}

		bool CanRetry() const
		{
			return max_retry_count > 0 && retry_count < max_retry_count;
		}

		int op_type;
		int op_result;
		std::string op_result_desc;
		int retry_count;
		int max_retry_count;
		unsigned int async_id;
		FileListener *listener;

		std::string path;
		std::string dest_path;
		std::string expansion_path;

		char *ptr;
		unsigned int len;
		std::string app_version_str;					// ������Դ�ļ��汾
		std::string svn_version_str;					// Զ������svn�汾��
		FileVersion::FilePathToVersionInfoMap version_file_server;		// �������ļ��汾
		GameType::StrUIntKVMap version_file_download;	// �������ļ��汾
		GameType::StrUIntKVMap version_file_init;		// ��װ���ļ��汾
		GameType::StrUIntKVMap version_file_expansion;	// ��չ��Դ���ļ��汾
		GameType::StrKVMap setting_file_specail_info;	// setting�ļ�
		GameType::StrVect unzip_file_list;				// ��Ҫѹ�����ļ�����
		Zip *script_ios_zip;							// Ϊscript.ios��ѹ����,�ڱ������̴߳�����Ȼ�󽻸����̲߳������߳��ͷ�
		cocos2d::Data file_data;						// ΪFileUtils::getDataFromFile���ӵ�
	};
	typedef std::vector<FileInfo>	FileInfoList;

	unsigned int Add(FileInfo fi);
	void Cancel(unsigned int async_id);
	FileInfoList Get();

	void SetMemLimit(unsigned int mem_limit);

	void Stop();

private:
	bool WriteSettingFile(const std::string &file_full_path,const GameType::StrKVMap &setting_map);
	bool WriteVersionFile(const std::string& download_path, const GameType::StrUIntKVMap& filepath_to_version_map);

	bool ReadVersionXml(const TiXmlDocument *xml, GameType::StrUIntKVMap& file_version_map, std::string& app_version, std::string& svn_app_version);
	bool ReadServerVersionXml(const TiXmlDocument *xml, FileVersion::FilePathToVersionInfoMap& file_version_map, std::string& app_version, std::string& svn_app_version);
	bool ReadVersionFromXml(const std::string& config_file_path, GameType::StrUIntKVMap& filepath_to_version_map, std::string& error_msg);
	bool ReadVersionFromIos(const std::string &config_file_path, FileVersion::FilePathToVersionInfoMap& filepath_to_version_map_server, std::string& app_version, std::string& svn_version, std::string& error_msg);
	bool ReadResVersionFromXml(const std::string& config_file_path, GameType::StrUIntKVMap& filepath_to_version_map);

	bool UnzipVersionIosFromPackage(const std::string &initial_res_path, GameType::StrUIntKVMap& filepath_to_version_map, std::string& error_msg);
	bool UnzipVersionIosFromExpansion(const std::string &expansion_path, GameType::StrUIntKVMap& file_vesion_expansion, std::string& error_msg);

	bool LoadAllVersionFile(const std::string &initial_res_path, const std::string &download_path, const std::string &expansion_path,
		std::string& app_version, std::string& svn_version,
		GameType::StrUIntKVMap& file_version_map_init, 
		GameType::StrUIntKVMap& file_version_map_download, 
		FileVersion::FilePathToVersionInfoMap& file_version_map_server,
		GameType::StrUIntKVMap& file_vesion_expansion,
		std::string& op_result_desc);

private:
	void ThreadFun();

private:
	bool volatile m_is_stoped;		// ģ���Ƿ��Ѿ�ֹͣ����

	//���ƶ�ȡ�����ڴ��С
	int volatile m_total_mem_len;
	int volatile m_used_mem_len;

	// �߳�
	Thread m_thread;

	FileInfoList m_unhandle_list;		// δ��������
	Mutex m_mutex_for_unhandle_list;	// δ�����������
	Semaphore m_sem_for_unhandle_list;	// δ����������ź���

	FileInfoList m_handled_list;		// ������ɶ���
	Mutex m_mutex_for_handled_list;		// ������ɶ��е���
};

#endif
