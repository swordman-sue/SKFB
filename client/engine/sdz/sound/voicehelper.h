#ifndef VOICEHELPER_H
#define VOICEHELPER_H

#include "common/platform/thread/mutex.h"
#include "common/platform/thread/thread.h"
#include "common/platform/thread/semaphorex.h"
#include "luaengine/luamessageadapter.h"
#include "filesystem/filehelper.h"
#include <sys/stat.h>
#include <vector>
#include <string>

DWORD VoiceHandleFunc(void* pData);

class VoiceHelper
{
	friend DWORD VoiceHandleFunc(void* pData);
public:
	VoiceHelper();
	~VoiceHelper();

public:
	enum VoiceOpType
	{
		VOT_INVALID = 0,
		VOT_GET_MSG_FROM_FILE,
		VOT_SAVE_MSG_AS_FILE,
		VOT_DELETE_FILE,

		VOT_CONUT,
	};
	enum VoiceOpResult
	{
		VOR_SUCC= 0,
		VOR_TYPE_ERR,
		VOR_WAV_TO_AMR_ERR,
		VOR_AMR_TO_WAV_ERR,
		VOR_FROM_MESSAGE,
		VOR_TO_MESSAGE,
		VOR_RENAME,
		VOR_DELETE_FILE,

		VOR_CONUT,
	};

	struct VoiceInfo
	{
		VoiceInfo()
			:voice_id(0)
			,op_type(VOT_INVALID)
			,op_result(VOR_SUCC)
			,voice_length(0)
			,data_ptr(0)
			,data_size(0)
			,callback_id(0)
		{

		}
		unsigned int voice_id;
		int op_type;
		int op_result;
		std::string src_file_name;
		std::string dest_file_name;
		std::string rename_file_name;
		int voice_length;
		void* data_ptr;
		unsigned int data_size;
		unsigned int callback_id;
		
	};
	typedef std::vector<VoiceInfo> VoiceInfoList;


public:
	void Add(VoiceInfo vi);
	VoiceInfoList Get();
	bool Stop();

private:
	int MsgToFile(const std::string &src_file_name,void* data_ptr,unsigned int& data_size);
	int FileToMsg(const std::string &dest_file_name,void*& data_ptr,unsigned int& data_size);

	int CCAmrToWav(const std::string &src_file_name, const std::string &dest_file_name);
	int CCWavToAmr(const std::string &src_file_name, const std::string &dest_file_name);

	int RenameFile(const std::string &src_file_name,const std::string &dest_file_name, const std::string &rename_file_name);
	int DeleteFile(const std::string &src_file_name);

private:
	void ThreadFun();
private:
	bool volatile m_is_stoped;		// 模块是否已经停止工作
	// 线程
	Thread m_thread;

	VoiceInfoList m_unhandle_list;		// 未处理请求
	Mutex m_mutex_for_unhandle_list;	// 未处理请求的锁
	Semaphore m_sem_for_unhandle_list;	// 未处理请求的信号量
	VoiceInfoList m_handled_list;		// 处理完成队列
	Mutex m_mutex_for_handled_list;		// 处理完成队列的锁
	unsigned int m_file_max_size;		//文件最大长度
};

#endif
