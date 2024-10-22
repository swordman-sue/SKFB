#ifndef RESMANAGER_H
#define RESMANAGER_H

#include "httpmanager/httpmanager.h"
#include "zqcommon/typedef.h"
#include "luatinker/lua_tinker.h"
#include "renderer/CCTextureCache.h"
#include <string>
#include <set>

class LuaResDownloadHttpLister;
class LuaCallUrlHttpLister;
class LuaCopyFileListener;

class ResManager
{
	friend class LuaResDownloadHttpLister;
	friend class LuaCallUrlHttpLister;
	friend class LuaCopyFileListener;
	friend class LuaUnzipFileListener;

public:
	static ResManager* GetInstance();
	static void DelInstance();
	~ResManager(){};

	bool Init();
	bool Stop();
	bool Release();

private:
	ResManager(){};

public:
	// ����ƽ̨url����Ϸurl����Դurl
	void SetPlatAndCdnURL(const std::string& plat_url, const std::string& res_url);
	void SetGameURL(const std::string& game_url);

	// �����������
	void SetConnectTimeout(int timeout);
	void SetKeepAlive(bool is_keepalive);

	// ����url�����ַ���
	bool CallPlatApi(const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	bool CallGameApi(const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	bool CallDirectApi(const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	bool CallDirectApiEx(const std::string &url, lua_tinker::table header_list, const std::string &post_param, const std::string &get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);

	// �ϴ��ļ���������
	bool SendFile2Server(const std::string& url, const std::string& filePath, const char* contentType, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	bool SendFile2Server(const std::string& url, const char* fileData, unsigned int fileSize, const char* contentType, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);

	bool LuaCallUrlHelper(int conn_id, const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	bool LuaCallUrlHelper(int conn_id, const std::string &url, lua_tinker::table header_list, const std::string &post_param, const std::string &get_param, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	bool LuaCallUrlHelper(int conn_id, const std::string &url, lua_tinker::table post_param, lua_tinker::table get_param, ZQ::INT32 time_out, HttpListener *listener);

	// �����ļ�
	bool UpdateFileList(lua_tinker::table file_list, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out, lua_tinker::table httpid_list, lua_tinker::table asyncid_list);
	bool UpdateFileList(lua_tinker::table file_list, ZQ::UINT32 file_list_len, ZQ::UINT32 callback_func_id, ZQ::INT32 time_out);
	void CancelUpdateFileList(lua_tinker::table httpid_list, lua_tinker::table asyncid_list);

	// ��ȡ�ļ��汾״̬ edit by chenyingpeng
	int GetFileVersionStatus(const std::string& file_path);

	// ��ȡ�ļ���С edit by chenyingpeng
	unsigned int GetFileSize(const std::string& file_path);

	// ��ȡ��Ҫ���µ��ļ��б�
	bool GetUpdateFileList(lua_tinker::table, lua_tinker::table, lua_tinker::table);

	void SetSetting(const std::string &key, const std::string &value);
	std::string GetSetting(const std::string &key);

	// ��̬��ѹ�����Դ
	virtual void UnzipInitResDynamically(const char* file_name);

private:
	void OnDownLoadFinish(unsigned int http_id, unsigned int func_id, const std::string &file_name, unsigned int version, bool is_abort, bool result, int ret_code, int http_ret_code, int op_code, const char *data, unsigned int len);
	void OnCopyCallBack(unsigned int async_id, unsigned int func_id, bool ret, const std::string &file_name, ZQ::UINT32 version, int op_result, const std::string& op_result_desc, unsigned int len);
	void OnUnzipCallBack(unsigned int async_id, unsigned int func_id, bool ret, const std::string &file_name, ZQ::UINT32 version, int op_result, const std::string& op_result_desc, unsigned int len);

private:
	// ��ѹ�е������Դ����
	std::set<std::string> m_unziping_init_res_set;
};

#endif
