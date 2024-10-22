#ifndef RESSTRUCT_H
#define RESSTRUCT_H

#include "zqcommon/typedef.h"
#include <string>
#include <vector>

#define MAX_URL_PARAM_LEN 512
#define MAX_URL_PARAM_VALUE_LEN 1280
#define MAX_URL_PARAM_NUM 64

class HttpListener;

// ��·���ַ�����ϣ��һ��3*4�ֽڽṹ
struct StrHashInfo
{
	StrHashInfo():hash0(0),hash1(0),hash2(0)
	{

	}
	ZQ::UINT32 hash0;
	ZQ::UINT32 hash1;
	ZQ::UINT32 hash2;

	bool operator < (const StrHashInfo& a) const
	{
		if (hash0 < a.hash0) return true;
		if (hash0 > a.hash0) return false;
		if (hash1 < a.hash1) return true;
		if (hash1 > a.hash1) return false;
		return hash2 < a.hash2;
	}

	bool operator == (const StrHashInfo& a) const
	{
		return (a.hash0==hash0)&&(a.hash1==hash1)&&(a.hash2==hash2);
	}
};

class PathTool
{
public:
	PathTool(){}
	~PathTool(){}

	static StrHashInfo GetPathHashInfo(const std::string& regular_path);

private:
	static void InitCryptTable();
	static ZQ::UINT32 HashString(ZQ::UINT32 type,const std::string& str_in );

private:
	static ZQ::UINT32 m_crypt_table[0x500];
	static bool m_is_crypt_table_init;
};

// http������Ϣ
struct HttpInfo
{
	HttpInfo()
		: expect_size(0)
		, url_id(0)
		, time_out_milliseconds(120000)
		, post_param_num(0)
		, get_param_num(0)
		, write_type(false)
		, is_resume_recv(false)
		, low_speed(1000.0f)
		, low_speed_timeout_milliseconds(10000.0f)
		, cur_low_speed_timeout_milliseconds(0.0f)
		, isSendFile(false)
		, sendFileData(nullptr)
		, sendFileSize(0)
		, sendFileContentType(nullptr)
	{
		for(int k=0; k<MAX_URL_PARAM_NUM; ++k)
		{
			post_param_names[k] = new char[MAX_URL_PARAM_LEN];
			memset(post_param_names[k], 0, MAX_URL_PARAM_LEN);
			post_param_values[k] = new char[MAX_URL_PARAM_VALUE_LEN];
			memset(post_param_values[k], 0, MAX_URL_PARAM_VALUE_LEN);
			get_param_names[k] = new char[MAX_URL_PARAM_LEN];
			memset(get_param_names[k], 0, MAX_URL_PARAM_LEN);
			get_param_values[k] = new char[MAX_URL_PARAM_VALUE_LEN];
			memset(get_param_values[k], 0, MAX_URL_PARAM_VALUE_LEN);
		}
	}

	HttpInfo(const HttpInfo &target)
	{
		for (int k = 0; k < MAX_URL_PARAM_NUM; ++k)
		{
			post_param_names[k] = new char[MAX_URL_PARAM_LEN];
			post_param_values[k] = new char[MAX_URL_PARAM_VALUE_LEN];
			get_param_names[k] = new char[MAX_URL_PARAM_LEN];
			get_param_values[k] = new char[MAX_URL_PARAM_VALUE_LEN];
		}
		sendFileData = nullptr;

		*this = target;
	}

	~HttpInfo()
	{
		for(int k=0; k<MAX_URL_PARAM_NUM; ++k)
		{
			delete post_param_names[k];
			delete post_param_values[k];
			delete get_param_names[k];
			delete get_param_values[k];
		}

		if (nullptr != sendFileData)
		{
			delete[] sendFileData;
			sendFileData = nullptr;
		}
	}

	HttpInfo& operator=(const HttpInfo &target)
	{
		if(this == &target)
			return *this;

		url = target.url;
		expect_size = target.expect_size;
		save_path = target.save_path;
		write_type = target.write_type;
		is_resume_recv = target.is_resume_recv;
		url_id = target.url_id;
		time_out_milliseconds = target.time_out_milliseconds;
		post_param_num = target.post_param_num;
		get_param_num = target.get_param_num;
		low_speed = target.low_speed;
		low_speed_timeout_milliseconds = target.low_speed_timeout_milliseconds;
		cur_low_speed_timeout_milliseconds = target.cur_low_speed_timeout_milliseconds;

		post_param_str = target.post_param_str;
		get_param_str = target.get_param_str;
		header_vec = target.header_vec;

		//�ϴ��ļ����
		isSendFile = target.isSendFile;
		sendFilePath = target.sendFilePath;
		sendFileContentType = target.sendFileContentType;
		if (nullptr != sendFileData)
		{
			delete[] sendFileData;
		}
		if (nullptr == target.sendFileData || 0 >= target.sendFileSize)
		{
			sendFileData = nullptr;
			sendFileSize = 0;
		}
		else
		{
			sendFileData = (char*)malloc(target.sendFileSize);
			sendFileSize = target.sendFileSize;
			memcpy(sendFileData, target.sendFileData, target.sendFileSize);
		}

		//get/post����
		for(int k=0; k<MAX_URL_PARAM_NUM; ++k)
		{
			memcpy(post_param_names[k], target.post_param_names[k], MAX_URL_PARAM_LEN);
			memcpy(post_param_values[k], target.post_param_values[k], MAX_URL_PARAM_VALUE_LEN);
			memcpy(get_param_names[k], target.get_param_names[k], MAX_URL_PARAM_LEN);
			memcpy(get_param_values[k], target.get_param_values[k], MAX_URL_PARAM_VALUE_LEN);
		}

		return *this;
	}

	std::string url;
	unsigned int expect_size; //�����ļ���С
	std::string save_path;
	bool write_type;		//д�ļ���ʽ��trueΪ�߽������ݱ�д��falseΪ����������һ����д��Ĭ�ϣ�
	bool is_resume_recv;	//�Ƿ�ʹ�öϵ�������Ĭ�ϲ�ʹ�öϵ�����
	ZQ::INT32 url_id;
	ZQ::UINT32 time_out_milliseconds;
	float low_speed;							//�������,Ĭ��1000B/s
	float low_speed_timeout_milliseconds;		//�������ʱ��������������ٳ�����ʱ�����Ƴ�����curl����Ĭ��10s
	float cur_low_speed_timeout_milliseconds;

	ZQ::UINT8 post_param_num;
	char *post_param_names[MAX_URL_PARAM_NUM];
	char *post_param_values[MAX_URL_PARAM_NUM];

	ZQ::UINT8 get_param_num;
	char *get_param_names[MAX_URL_PARAM_NUM];
	char *get_param_values[MAX_URL_PARAM_NUM];

	std::string post_param_str;
	std::string get_param_str;
	std::vector<std::string> header_vec;

	bool isSendFile;
	std::string sendFilePath;
	char* sendFileData;
	unsigned int sendFileSize;
	char* sendFileContentType;

	const StrHashInfo GetHashInfo() const;
};

//Http������Ϣ(����HttpConnection)
struct RequestInfo
{
	RequestInfo() :http_id(0), result(true), ret_code(0), data_ptr(0), data_len(0), http_ret_code(0)
	{}

	enum{
		OP_SUCCESS = 0,
		OP_NETWORK_FAILED = 1,
		OP_SIZE_NOT_EQUAL = 2,
		OP_SAVE_FAILED = 3,
		OP_UNZIP_FAILED = 4,
		OP_SIZE_INVALID = 5,
	};

	ZQ::UINT32 http_id;
	HttpInfo http_info;
	bool result;
	int op_code;		//0�ɹ����� -1����ʧ�� -2���ص��ļ���С��ƥ�� -3�ļ�����ʧ��
	int ret_code;
	int http_ret_code;
	char *data_ptr;
	unsigned int data_len;
};
typedef std::vector<RequestInfo> RequestInfoList;
typedef std::multimap<unsigned int, RequestInfo> RequestInfoMap;

//HTTP����������Ϣ(����HttpManager���ⲿ�ص�)
struct RequestTask
{
	RequestTask()
		:http_id(0)
		,is_abort(false)
		,total_len(0)
		,cur_len(0)
		,result(true)
		,ret_code(0)
		,data_ptr(0)
		,data_len(0)
		,pre_time(0)
		,pre_len(0)
		,http_ret_code(0)
		,op_code(0)
	{

	}

	HttpInfo	http_info;
	ZQ::UINT32	http_id;	
	bool		is_abort;
	ZQ::UINT32	total_len;
	ZQ::UINT32	cur_len;
	float		pre_time;
	ZQ::UINT32	pre_len;

	bool		result;			// ������
	int			ret_code;		// ������
	int			http_ret_code;	// HTTP������
	int			op_code;		// ������
	char*		data_ptr;		// ����ָ��
	ZQ::UINT32	data_len;		// ���ݳ���

	typedef std::vector<HttpListener*> CALLBACKLIST;
	CALLBACKLIST callback_list;
};

#endif

