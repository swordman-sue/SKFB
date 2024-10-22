#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <vector>
#include <algorithm>

struct curl_slist;
static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lp_void);
static int OnProgress(void* lp_void, double dltotal, double dlnow, double ultotal, double ulnow);

class HttpClient
{
	friend size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid);
	friend int OnProgress(void* lpVoid, double dltotal, double dlnow, double ultotal, double ulnow);
	
public:
	static bool Init();
	static void CleanUp();

	typedef int(*ProgressCallBackFunc)(void *p, size_t recv_len, size_t total_len);

public:
	HttpClient();
	~HttpClient(void);

	bool Post(const std::string& url, std::vector<std::string>& header_vec, const std::string& post_str);
	bool SendFile(const std::string& url, const std::string& filePath, const char* contentType);
	bool SendFileData(const std::string& url, const char* fileData, unsigned int fileSize, const char* contentType);
	bool Get(const std::string& url, int offset);

	unsigned int GetDataLen() const{return m_data_len;}
	const char* GetDataPtr(){return m_data_ptr;}
	long GetCurlRetCode() const { return m_last_curl_ret_code; }
	long GetHttpRetCode() const { return m_last_http_ret_code; }
	
public:
	void SetDebug(bool is_debug);
	void SetTimeOut(int connect_timer_out_ms, int time_out_ms);
	void SetLongConnect(bool is_long_connect);
	void SetRedirect(bool is_redirect);
	void SetProgressInfo(ProgressCallBackFunc func, void* progress_data);
	void SetSavePath(const std::string& save_path);
	void SetWriteType(bool type);
	void RemoveEasyHandle();

protected:
	int OnCurlProgress( double dltotal, double dlnow, double ultotal, double ulnow );

	void Recapacity(size_t new_size);
	size_t OnCurlWriteData(void* buffer, size_t size, size_t nmemb);

	static size_t get_next_pot( size_t x )
	{
		x = x - 1;
		x = x | (x >> 1);
		x = x | (x >> 2);
		x = x | (x >> 4);
		x = x | (x >> 8);
		x = x | (x >>16);
		return x + 1;
	}

private:
	bool m_is_debug;
	void* m_easy_handle;
	void* m_multi_handle;
	size_t m_capacity;
	char* m_data_ptr;
	size_t m_data_len;

	long			m_last_http_ret_code;
	long			m_last_curl_ret_code;

	void*			m_progress_data;
	ProgressCallBackFunc m_progress_func;

	bool			m_is_long_connect;
	bool			m_is_redirect;
    curl_slist*     m_curl_list;
	std::string		m_save_path;
	bool			m_write_type;		//写文件方式：true为边接收数据边写（默认），false为接收完数据一次性写
	bool			m_stop_perform;

	struct curl_slist* m_header_list;
};

#endif // HTTPCLIENT_H
