#include "httpclient.h"
#include <string>
#include <sys/stat.h>
#include <curl/curl.h>
#include "filesystem/filehelper.h"
#include "zqcommon/logutil.h"
#include "zqcommon/strutil.h"

static int OnDebug(CURL*, curl_infotype itype, char* pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	else
	{
		printf("[itype]%d\n", (int)itype);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lp_void)
{
	HttpClient* parent_client = (HttpClient*)lp_void;
	return parent_client->OnCurlWriteData(buffer, size, nmemb);
}

static int OnProgress(void* lp_void, double dltotal, double dlnow, double ultotal, double ulnow)
{
	HttpClient* parent_client = (HttpClient*)lp_void;
	return parent_client->OnCurlProgress(dltotal, dlnow, ultotal, ulnow);
}

bool HttpClient::Init()
{
	CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
	return (code == CURLE_OK);
}

void HttpClient::CleanUp()
{
	curl_global_cleanup();
}

HttpClient::HttpClient() 
	: m_is_debug(false)
	, m_easy_handle(0)
	, m_multi_handle(nullptr)
	, m_capacity(0)
	, m_data_ptr(0)
	, m_data_len(0)
	, m_last_http_ret_code(0)
	, m_last_curl_ret_code(-1)
	, m_progress_func(0)
	, m_progress_data(0)
	, m_is_long_connect(false)
	, m_is_redirect(false)
    , m_curl_list(NULL)
	, m_write_type(true)
	, m_stop_perform(false)
	, m_header_list(nullptr)
{
	m_multi_handle = curl_multi_init();
	m_easy_handle = curl_easy_init();

	CURL* easy_handle = (CURL*)m_easy_handle;

	curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);

	curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, OnProgress);
	curl_easy_setopt(easy_handle, CURLOPT_PROGRESSDATA, (void *)this);

	curl_easy_setopt(easy_handle, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void *)this);

	curl_easy_setopt(easy_handle, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(easy_handle, CURLOPT_CONNECTTIMEOUT_MS, 3000);
	curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT_MS, 10000);
	curl_easy_setopt(easy_handle, CURLOPT_FORBID_REUSE, 1L);
}

HttpClient::~HttpClient(void)
{
	if (nullptr != m_header_list)
	{
		curl_slist_free_all(m_header_list);
		m_header_list = NULL;
	}

	if (nullptr != m_easy_handle)
	{
		CURL* easy_handle = (CURL*)m_easy_handle;
		curl_easy_cleanup(easy_handle);
		m_easy_handle = nullptr;
	}

	if (nullptr != m_multi_handle)
	{
		CURLM* multi_handle = (CURLM*)m_multi_handle;
		curl_multi_cleanup(multi_handle);
		m_multi_handle = nullptr;
	}

	if(m_curl_list)
    {
        curl_slist_free_all(m_curl_list);
        m_curl_list = NULL;
    }

	delete[] m_data_ptr;
	m_data_ptr = 0;
	m_capacity = 0;
	m_data_len = 0;
}

bool HttpClient::Post(const std::string& url, std::vector<std::string>& header_vec, const std::string& post_str)
{
	m_data_len = 0;

	CURL* easy_handle = (CURL*)m_easy_handle;
	CURLM* multi_handle = (CURLM*)m_multi_handle;

	//初始化http头
	if (NULL != m_header_list)
	{
		curl_slist_free_all(m_header_list);
		m_header_list = NULL;
	}
	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator ite = header_vec.end();
	for (it = header_vec.begin(); it != ite; ++it)
	{
		m_header_list = curl_slist_append(m_header_list, it->c_str());
	}

	curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(easy_handle, CURLOPT_POST, 1);
	curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, post_str.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDSIZE, post_str.size());
	if (nullptr != m_header_list)
	{
		curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, m_header_list);
	}

	curl_multi_add_handle(multi_handle, easy_handle);

	//执行数据传输
	m_stop_perform = false;
	int running_handle_num = 0;
	do
	{
		m_last_curl_ret_code = curl_multi_perform(multi_handle, &running_handle_num);
		if (CURLM_CALL_MULTI_PERFORM == m_last_curl_ret_code)
		{
			continue;
		}

		if (CURLM_OK != m_last_curl_ret_code)
		{
			m_last_curl_ret_code = -m_last_curl_ret_code;
			break;
		}

		if (m_stop_perform)
		{
			m_last_curl_ret_code = CURLE_OPERATION_TIMEDOUT;
			break;
		}

	} while (CURLM_CALL_MULTI_PERFORM == m_last_curl_ret_code || running_handle_num > 0);

	//检测数据传输结果
	m_last_http_ret_code = 0;
	if (CURLM_OK == m_last_curl_ret_code)
	{
		int msgInQueue;
		CURLMsg* pCurlMsg = NULL;
		do
		{
			pCurlMsg = curl_multi_info_read(multi_handle, &msgInQueue);
			if (pCurlMsg && (pCurlMsg->msg == CURLMSG_DONE) && (pCurlMsg->easy_handle == easy_handle))
			{
				m_last_curl_ret_code = pCurlMsg->data.result;
				curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &m_last_http_ret_code);
				break;
			}
		} while (NULL != pCurlMsg);
	}

	curl_multi_remove_handle(multi_handle, easy_handle);

	if ( m_last_curl_ret_code != CURLE_OK || m_last_http_ret_code != 200 )
	{
		LogUtil::LogError("[HttpClient::Post CurlError][curl_ret_code:%d http_ret_code:%d]", m_last_curl_ret_code, m_last_http_ret_code);
		return false;
	}

	return true;
}

bool HttpClient::SendFile(const std::string& url, const std::string& filePath, const char* contentType)
{
	FILE * file;
	struct stat fileInfo;
	char *fileData = NULL;

	file = fopen(filePath.c_str(), "rb");
	if (NULL == file)
	{
		return false;
	}
	stat(filePath.c_str(), &fileInfo);// get file size
	unsigned int fileSize = fileInfo.st_size;
	fileData = (char*)malloc(fileSize);
	if (NULL == fileData) 
	{
		fclose(file);
		return false;
	}
	int readCount = fread(fileData, 1, fileSize, file);
	if (readCount != fileSize) 
	{
		fclose(file);
		free(fileData);
		return false;
	}
	fclose(file);

	SendFileData(url, fileData, fileSize, contentType);
	free(fileData);
	return true;
}

bool HttpClient::SendFileData(const std::string& url, const char* fileData, unsigned int fileSize, const char* contentType)
{
	m_data_len = 0;

	CURL* easy_handle = (CURL*)m_easy_handle;
	CURLM* multi_handle = (CURLM*)m_multi_handle;

	//通过表单方式上传二进制数据
	//referer http://curl.haxx.se/mail/lib-2003-08/0190.html
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	curl_formadd(&formpost, &lastptr,
		CURLFORM_COPYNAME, "file",
		CURLFORM_BUFFER, "unknown",
		CURLFORM_BUFFERPTR, fileData,
		CURLFORM_BUFFERLENGTH, fileSize,
		CURLFORM_CONTENTTYPE, contentType,
		CURLFORM_END);

	curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_HTTPPOST, formpost);

	/* initialize custom header list (stating that Expect: 100-continue is not wanted */
	struct curl_slist *headerlist = NULL;
	headerlist = curl_slist_append(headerlist, "Expect:");
	curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headerlist);

	curl_multi_add_handle(multi_handle, easy_handle);

	//执行数据传输
	m_stop_perform = false;
	int running_handle_num = 0;
	do
	{
		m_last_curl_ret_code = curl_multi_perform(multi_handle, &running_handle_num);
		if (CURLM_CALL_MULTI_PERFORM == m_last_curl_ret_code)
		{
			continue;
		}

		if (CURLM_OK != m_last_curl_ret_code)
		{
			m_last_curl_ret_code = -m_last_curl_ret_code;
			break;
		}

		if (m_stop_perform)
		{
			m_last_curl_ret_code = CURLE_OPERATION_TIMEDOUT;
			break;
		}

	} while (CURLM_CALL_MULTI_PERFORM == m_last_curl_ret_code || running_handle_num > 0);

	//检测数据传输结果
	m_last_http_ret_code = 0;
	if (CURLM_OK == m_last_curl_ret_code)
	{
		int msgInQueue;
		CURLMsg* pCurlMsg = NULL;
		do
		{
			pCurlMsg = curl_multi_info_read(multi_handle, &msgInQueue);
			if (pCurlMsg && (pCurlMsg->msg == CURLMSG_DONE) && (pCurlMsg->easy_handle == easy_handle))
			{
				m_last_curl_ret_code = pCurlMsg->data.result;
				curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &m_last_http_ret_code);
				break;
			}
		} while (NULL != pCurlMsg);
	}

	curl_formfree(formpost);
	curl_multi_remove_handle(multi_handle, easy_handle);

	if (m_last_curl_ret_code != CURLE_OK || m_last_http_ret_code != 200)
	{
		LogUtil::LogError("[HttpClient::PostFileData] curl error(curl_ret_code:%d http_ret_code:%d)", m_last_curl_ret_code, m_last_http_ret_code);
		return false;
	}

	return true;
}

bool HttpClient::Get(const std::string& url, int offset)
{
	m_data_len = offset;

	CURL* easy_handle = (CURL*)m_easy_handle;
	CURLM* multi_handle = (CURLM*)m_multi_handle;

	curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_POST, 0);
	curl_easy_setopt(easy_handle, CURLOPT_RESUME_FROM, offset);
	
	curl_multi_add_handle(multi_handle, easy_handle);	

	//执行数据传输
	m_stop_perform = false;
	int running_handle_num = 0;
	do 
	{
		m_last_curl_ret_code = curl_multi_perform(multi_handle, &running_handle_num);
		if (CURLM_CALL_MULTI_PERFORM == m_last_curl_ret_code)
		{
			continue;
		}

		if (CURLM_OK != m_last_curl_ret_code)
		{
			m_last_curl_ret_code = -m_last_curl_ret_code;
			break;
		}

		if (m_stop_perform)
		{
			m_last_curl_ret_code = CURLE_OPERATION_TIMEDOUT;
			break;
		}

	} while (CURLM_CALL_MULTI_PERFORM == m_last_curl_ret_code || running_handle_num > 0);

	//检测数据传输结果
	m_last_http_ret_code = 0;
	if (CURLM_OK == m_last_curl_ret_code)
	{
		int msgInQueue;
		CURLMsg* pCurlMsg = NULL;
		do
		{
			pCurlMsg = curl_multi_info_read(multi_handle, &msgInQueue);
			if (pCurlMsg && (pCurlMsg->msg == CURLMSG_DONE) && (pCurlMsg->easy_handle == easy_handle))
			{
				m_last_curl_ret_code = pCurlMsg->data.result;
				curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &m_last_http_ret_code);
				break;
			}
		} while (NULL != pCurlMsg);
	}

	curl_multi_remove_handle(multi_handle, easy_handle);
	
	//断点续传时返回码为206，表示部分内容
	if (m_last_curl_ret_code != CURLE_OK || (m_last_http_ret_code != 200 && m_last_http_ret_code != 206))
	{
		LogUtil::LogError("[HttpClient::Get CurlError][curl_ret_code:%d http_ret_code:%d]", m_last_curl_ret_code, m_last_http_ret_code);
		return false;
	}

	return true;
}

void HttpClient::Recapacity(size_t new_size)
{
	size_t need_size = get_next_pot(new_size);
	size_t need_cpy_len = std::min<size_t>(m_data_len, need_size);
	char* new_data_ptr = new char[need_size];
	memset(new_data_ptr, 0, need_size);
	if(m_data_ptr != nullptr && need_cpy_len > 0)
	{
		memcpy(new_data_ptr, m_data_ptr, need_cpy_len);
	}
	if(m_capacity > 0)
	{
		delete[] m_data_ptr;
	}
	m_capacity = need_size;
	m_data_ptr = new_data_ptr;
}

size_t HttpClient::OnCurlWriteData(void* buffer, size_t size, size_t nmemb)
{
	if(buffer == 0)
	{
		return -1;
	}
	else
	{
		size_t total_size = size * nmemb;

		if (m_save_path != "" && m_write_type)
		{
			if (m_data_len == 0 && FileHelper::IsExsit(m_save_path))
			{
				if (!FileHelper::Remove(m_save_path))
				{
					LogUtil::LogError("[HttpClient::OnCurlWriteData FileRemoveError][file:%s]", m_save_path.c_str());
					return -1;
				}
			}

			if (!FileHelper::WriteAdd(m_save_path, (const char*)buffer, total_size))
			{
				LogUtil::LogError("[HttpClient::OnCurlWriteData WriteAddError][file:%s]", m_save_path.c_str());
				return -1;
			}
		}
		else
		{
			if (total_size + m_data_len >= m_capacity)
			{
				Recapacity(total_size + m_data_len + 1);	// 预留最后位置\0
			}
			memcpy(m_data_ptr + m_data_len, buffer, total_size);
		}
		m_data_len += total_size;

		return total_size;
	}
}

int HttpClient::OnCurlProgress( double dltotal, double dlnow, double ultotal, double ulnow )
{
	int ret = 0;
	if(m_progress_func && dlnow != 0.0)
	{
		ret = (*m_progress_func)(m_progress_data, (size_t)dlnow, (size_t)dltotal);
	}
	return ret;
}

void HttpClient::SetProgressInfo(ProgressCallBackFunc func, void* progress_data)
{
	m_progress_func = func;
	m_progress_data = progress_data;
}

void HttpClient::RemoveEasyHandle()
{
	m_stop_perform = true;
}

void HttpClient::SetSavePath(const std::string& save_path)
{
	m_save_path = save_path;
}

void HttpClient::SetWriteType(bool type)
{
	m_write_type = type;
}

void HttpClient::SetDebug(bool is_debug)
{
	CURL* easy_handle = (CURL*)m_easy_handle;

	m_is_debug = is_debug;
	if(m_is_debug)
	{
		curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 1);
		//curl_easy_setopt(m_easy_handle, CURLOPT_HEADER, 1); 
		//curl_easy_setopt(m_easy_handle, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	else
	{
		curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 0);
		//curl_easy_setopt(m_easy_handle, CURLOPT_DEBUGFUNCTION, 0);
	}
}


void HttpClient::SetTimeOut(int connect_timer_out_ms, int time_out_ms)
{
	CURL* easy_handle = (CURL*)m_easy_handle;
	curl_easy_setopt(easy_handle, CURLOPT_CONNECTTIMEOUT_MS, (long)connect_timer_out_ms);
	curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT_MS, (long)time_out_ms);
}

void HttpClient::SetLongConnect(bool is_long_connect)
{
	CURL* easy_handle = (CURL*)m_easy_handle;

	m_is_long_connect = is_long_connect;
	if(m_is_long_connect)
	{
		curl_easy_setopt(easy_handle, CURLOPT_FORBID_REUSE, 0L);
        if(m_curl_list)
        {
            curl_slist_free_all(m_curl_list);
            m_curl_list = NULL;
        }
        m_curl_list = curl_slist_append(m_curl_list,"Connection:Keep-Alive");
		curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, m_curl_list);
	}
	else
	{
		curl_easy_setopt(easy_handle, CURLOPT_FORBID_REUSE, 1L);
	}
}

void HttpClient::SetRedirect(bool is_redirect)
{
	CURL* easy_handle = (CURL*)m_easy_handle;

	m_is_redirect = is_redirect;
	if(is_redirect)
	{
		curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
	}
	else
	{
		curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 0L);
	}
}

