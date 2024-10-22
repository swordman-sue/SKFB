
#include "httpconnection.h"
#include "httpclient/httpclient.h"
#include "filesystem/filehelper.h"
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include "common/platform/socket/pisocket.h"
#include "common/platform/system.h"
#include "common/platform/thread/mutex.h"
#include "zip/zip.h"
#include "json/json.h"

#include <algorithm>
#include <assert.h>

DWORD ResourceHandleFunc(void *data_ptr)
{
	HttpConnection* res_module = (HttpConnection*)data_ptr;
	res_module->ThreadFun();
	return 0;
}

int ProgressCallBack(void *p, size_t recv_len, size_t total_len)
{
	int ret = 0;

	HttpConnection* http_conn = (HttpConnection*)p;
	http_conn->m_requesting_http_total_len = total_len;
	http_conn->m_requesting_http_cur_len = recv_len;

	return ret;
}

void HttpConnection::ThreadFun()
{
	if (m_http_client)
	{
		delete m_http_client;
		m_http_client = nullptr;
	}

	m_http_client = new HttpClient();
	m_http_client->SetProgressInfo(ProgressCallBack, this);

	while (!m_is_stoped)
	{
		//防止一帧时间内下载内存过大
		if (m_used_mem_len>m_total_mem_len)
		{
			PISleep(500);
			continue;
		}

		if (Semaphore::Succeed != m_sem_for_unhandle_map.TryDown(1000))
			continue;

		//延时链接休眠
		if(m_httpclient_connect_sleep > 0)
		{
			PISleep(m_httpclient_connect_sleep);
		}

		m_mutex_for_unhandle_map.Lock();
		if(m_unhandle_map.empty())
		{
			m_mutex_for_unhandle_map.Unlock();
			continue;
		}

		RequestInfoMap::iterator unhandle_request = m_unhandle_map.begin();
		RequestInfo request_info = unhandle_request->second;
		m_http_client->SetSavePath(request_info.http_info.save_path);
		m_http_client->SetWriteType(request_info.http_info.write_type);
		m_unhandle_map.erase(unhandle_request);
		m_mutex_for_unhandle_map.Unlock();

		m_requesting_http_id = request_info.http_id;
		m_requesting_http_total_len = 0;
		m_requesting_http_cur_len = 0;

		ExcuteHttpRequest(&request_info, m_http_client);

		m_mutex_for_handled_queue.Lock();

		if(m_requesting_http_id == request_info.http_id)
		{
			m_handled_queue.push_back(request_info);
		}
		else
		{
			//被中断
			if (nullptr != request_info.data_ptr)
			{
				free(request_info.data_ptr);
				request_info.data_ptr = nullptr;
			}
			m_used_mem_len -= request_info.data_len;
		}		

		m_requesting_http_id = 0;
		m_requesting_http_total_len = 0;
		m_requesting_http_cur_len = 0;

		m_mutex_for_handled_queue.Unlock();
	}

	delete m_http_client;
	m_http_client = 0;
}

void HttpConnection::ExcuteHttpRequest(RequestInfo *request_info, HttpClient *http_client)
{
	std::string url = request_info->http_info.url;
	std::string get_str = "";
	std::string post_str = "";

	//拼接get参数
	for (int i = 0; i < request_info->http_info.get_param_num; i++)
	{
		if (get_str != "")
		{
			get_str += "&";
		}
		get_str += ZQ::StrUtil::UrlEncode(request_info->http_info.get_param_names[i]);
		get_str += "=";
		get_str += ZQ::StrUtil::UrlEncode(request_info->http_info.get_param_values[i]);
	}
	if (request_info->http_info.get_param_str != "")
	{
		get_str = request_info->http_info.get_param_str;
	}
	if (get_str != "")
	{
		url +="?"+get_str;
	}

	//拼接post参数
	for (int i = 0; i < request_info->http_info.post_param_num; i++)
	{
		if (post_str!="")
		{
			post_str += "&";
		}
		post_str += ZQ::StrUtil::UrlEncode(request_info->http_info.post_param_names[i]);
		post_str += "=";
		post_str += ZQ::StrUtil::UrlEncode(request_info->http_info.post_param_values[i]);
	}
	if (request_info->http_info.post_param_str != "")
	{
		post_str = request_info->http_info.post_param_str;
	}

	//设置超时时间
	http_client->SetTimeOut(m_httpclient_connect_timeout, request_info->http_info.time_out_milliseconds);
	http_client->SetLongConnect(m_httpclient_is_keepalive);

	const std::string& tmp_path = request_info->http_info.save_path + ".tmp";

	if (request_info->http_info.isSendFile)
	{
		if (nullptr != request_info->http_info.sendFileData)
		{
			//上传文件(已知文件数据)
			request_info->result = http_client->SendFileData(
				url, request_info->http_info.sendFileData, request_info->http_info.sendFileSize, request_info->http_info.sendFileContentType);
		}
		else
		{
			//上传文件(已知文件路径)
			request_info->result = http_client->SendFile(url, request_info->http_info.sendFilePath, request_info->http_info.sendFileContentType);
		}
	}
	else
	{
		if (post_str != "")
		{
			//常规post
			request_info->result = http_client->Post(url, request_info->http_info.header_vec, post_str);
		}
		else
		{
			//常规get
			int offset = 0;
			if (request_info->http_info.is_resume_recv)
			{
				if (request_info->http_info.save_path != "" && FileHelper::IsExsit(tmp_path))
				{
					char *file_buf;
					unsigned int file_size = 0;
					FileHelper::Read(tmp_path, &file_buf, &file_size);

					Json::Reader json_reader;
					Json::Value root;
					std::string tmp_str(file_buf, file_size);
					if (json_reader.parse(tmp_str, root)
						&& root["url"].asString() == url
						&& FileHelper::IsExsit(request_info->http_info.save_path))
					{
						char *tmp_buf;
						unsigned int buf_size = 0;
						FileHelper::Read(request_info->http_info.save_path, &tmp_buf, &buf_size);

						offset = buf_size;
						if (buf_size > 0)
						{
							free(tmp_buf);
						}
					}
					if (file_size > 0)
					{
						free(file_buf);
					}
				}
				std::string json_str = "{\"url\":\"" + url + "\"}";
				FileHelper::Write(tmp_path, json_str.c_str(), json_str.length());
			}
			request_info->result = http_client->Get(url, offset);
		}
	}

	request_info->ret_code = http_client->GetCurlRetCode();
	request_info->http_ret_code = http_client->GetHttpRetCode();
	if (!request_info->result)
	{
		LogUtil::LogError("[HttpConnection::ExcuteHttpRequest] network failed result:%d ret_code:%d http_ret_code:%d url:%s post_str:%s", 
			request_info->result, request_info->ret_code, request_info->http_ret_code, url.c_str(), post_str.c_str());
		
		request_info->op_code = -RequestInfo::OP_NETWORK_FAILED;
		if (request_info->http_info.is_resume_recv && request_info->http_info.save_path != "" && FileHelper::IsExsit(tmp_path))
		{
			FileHelper::Remove(tmp_path);
		}
		return;
	}

	// 文件大小有效性检测
	const char* data = http_client->GetDataPtr();
	ZQ::UINT32 data_len = (ZQ::UINT32)http_client->GetDataLen();
	if (data_len < request_info->http_info.expect_size)
	{
		LogUtil::LogError("[HttpConnection::ExcuteHttpRequest] size not equal data_len:%d expect_size:%d url:%s post_str:%s", 
			data_len, request_info->http_info.expect_size, url.c_str(), post_str.c_str());

		request_info->op_code = -RequestInfo::OP_SIZE_NOT_EQUAL;
		request_info->result = false;
		return;
	}

	// 不保存到本地
	if (request_info->http_info.save_path == "")
	{
		if (data_len == 0 || data == 0)
		{
			LogUtil::LogError("[HttpConnection::ExcuteHttpRequest] size invalid data_len:%d data:%s url:%s post_str:%s",
				data_len, data, url.c_str(), post_str.c_str());

			request_info->op_code = -RequestInfo::OP_SIZE_INVALID;
			request_info->result = false;
			return;
		}

		request_info->data_ptr = (char*)malloc(sizeof(char)* data_len + 1);
		request_info->data_ptr[data_len] = 0;
		memcpy(request_info->data_ptr, data, data_len);
		m_used_mem_len += data_len;
	}
	// 保存到本地
	else if (!request_info->http_info.write_type)
	{
		if(!FileHelper::Write(request_info->http_info.save_path, data, data_len))
		{
			LogUtil::LogError("[HttpConnection::ExcuteHttpRequest] save failed file:%s", request_info->http_info.save_path.c_str());

			request_info->op_code = -RequestInfo::OP_SAVE_FAILED;
			request_info->result = false;
			return;
		}
	}
	request_info->data_len = data_len;

	// 移除断点续传中产生的临时文件
	if (request_info->http_info.is_resume_recv && request_info->http_info.save_path != "" && FileHelper::IsExsit(tmp_path))
	{
		FileHelper::Remove(tmp_path);
	}
}

HttpConnection::HttpConnection( )
	:m_is_started(false)
	,m_is_stoped(false)
	,m_httpclient_connect_timeout(10000)
	,m_httpclient_is_keepalive(true)
	,m_total_mem_len(1024*1024*10)
	,m_used_mem_len(0)
	,m_sem_for_unhandle_map(0)//TODO:Semaphore
	,m_requesting_http_id(0)
	,m_requesting_http_cur_len(0)
	,m_requesting_http_total_len(0)
	,m_httpclient_connect_sleep(0)
	,m_http_client(nullptr)
{
}

HttpConnection::~HttpConnection()
{
	if (m_http_client)
	{
		delete m_http_client;
		m_http_client = 0;
	}
}

void HttpConnection::Start()
{
	if(m_is_started)
		return;
	
	m_is_started = true;
	m_thread.Run(&ResourceHandleFunc, this);
}

void HttpConnection::Stop()
{
	m_is_stoped = true;

	RequestInfoMap::iterator iter;

	iter = m_unhandle_map.begin();
	while(iter != m_unhandle_map.end())
	{
		if (nullptr != iter->second.data_ptr)
		{
			free(iter->second.data_ptr);
		}
		++iter;
	}

	iter = m_release_map.begin();
	while(iter != m_release_map.end())
	{
		if (nullptr != iter->second.data_ptr)
		{
			free(iter->second.data_ptr);
		}
		++iter;
	}

	m_sem_for_unhandle_map.Up();
	m_thread.Join();
}

void HttpConnection::AddReqest(unsigned int http_id, const HttpInfo& http_info)
{
	RequestInfo req_info;
	req_info.http_id = http_id;
	req_info.http_info = http_info;

	m_mutex_for_unhandle_map.Lock();
	m_unhandle_map.insert(std::make_pair(http_id, req_info));
	m_mutex_for_unhandle_map.Unlock();
	
	m_sem_for_unhandle_map.Up();
}

void HttpConnection::AbortRequest(unsigned int http_id)
{
    RemoveEasyHandle();

	m_mutex_for_unhandle_map.Lock();
	RequestInfoMap::iterator iter = m_unhandle_map.find(http_id);
	if(iter != m_unhandle_map.end())
	{
		m_unhandle_map.erase(iter);
	}

	if(m_requesting_http_id == http_id)
	{
		m_requesting_http_id = 0;
		m_requesting_http_total_len = 0;
		m_requesting_http_cur_len = 0;
	}
	m_mutex_for_unhandle_map.Unlock();
}

void HttpConnection::GetProgress(unsigned int *http_id, unsigned int *total_len, unsigned int *cur_len)
{
	*http_id = m_requesting_http_id;
	*total_len = m_requesting_http_total_len;
	*cur_len = m_requesting_http_cur_len;
}

RequestInfoList HttpConnection::GetHandledRequest()
{
	RequestInfoList tmp_list;
	m_mutex_for_handled_queue.Lock();
	tmp_list.swap(m_handled_queue);
	m_mutex_for_handled_queue.Unlock();

	RequestInfoList::iterator iter = tmp_list.begin();
	for(; iter != tmp_list.end(); ++iter)
	{
		m_release_map.insert(std::make_pair(iter->http_id, *iter));
	}

	return tmp_list;
}

void HttpConnection::ReleaseRequestInfo(RequestInfo *request_info)
{
	assert(request_info != 0);

	RequestInfoMap::iterator iter = m_release_map.find(request_info->http_id);
	if(iter == m_release_map.end())
	{
		return;
	}

	if (nullptr != request_info->data_ptr)
	{
		free(request_info->data_ptr);
		m_used_mem_len -= request_info->data_len;
        request_info->data_ptr = nullptr;
	}
	m_release_map.erase(iter);
}

void HttpConnection::SetConnectTimeout(int timeout)
{
	m_httpclient_connect_timeout = timeout;
}

void HttpConnection::SetKeepAlive(bool is_keepalive)
{
	m_httpclient_is_keepalive = is_keepalive;
}

void HttpConnection::SetMemLimit(int total_mem)
{
	m_total_mem_len = total_mem;
}

void HttpConnection::SetConnectSleep(int ms)
{
	m_httpclient_connect_sleep = ms;
}

void HttpConnection::RemoveEasyHandle()
{
	if (m_http_client)
	{
		m_http_client->RemoveEasyHandle();
	}
}
