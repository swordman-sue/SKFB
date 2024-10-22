
#include "httpmanager.h"
#include "CCPlatformDefine.h"
#include "httpclient/httpclient.h"
#include "zqcommon/strutil.h"
#include "zqcommon/logutil.h"
#include "common/platform/system.h"

#include <algorithm>
#include <assert.h>

#if defined(__APPLE__) || defined(__ANDROID__)
inline long InterlockedIncrement( volatile long *val )
{
	//__gnu_cxx::__exchange_and_add((volatile int *)val,1);
	__sync_fetch_and_add((volatile int *)val,1);
	return *val;

}
#endif

#define RESOURCE_THREAD_START_CONNID  10000

ZQ::UINT32 GetHttpID()
{
	static long http_id = 0;
	http_id = (ZQ::UINT32)(InterlockedIncrement(&http_id));
	return http_id;
}

//std::string GetDomain(const std::string& url)
//{
//	std::string path = url;
//	// 去除头部
//	size_t pos = path.find("http://");
//	if (pos != path.npos && pos == 0)
//	{
//		path = path.substr(7);
//	}
//
//	// 获取最后
//	pos = path.find('/');
//	if (pos != path.npos)
//	{
//		path = path.substr(0, pos);
//		//m_root = path.substr(pos + 1);
//	}
//
//	// 去除端口
//	pos = path.find(':');
//	if (pos != path.npos)
//	{
//		//m_port = m_hostname.substr(pos+1);
//		path = path.substr(0, pos);
//	}
//
//	//char url[MAX_URL_LENGTH];
//	//m_hostname = UrlEncode(m_hostname.c_str(), url, MAX_URL_LENGTH, ":%./");
//	return path;
//}

static HttpManager* s_instance = nullptr;

HttpManager* HttpManager::GetInstance()
{
	if (nullptr == s_instance)
	{
		s_instance = new HttpManager;
	}
	return s_instance;
}

void HttpManager::DelInstance()
{
	if (nullptr != s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

HttpManager::~HttpManager()
{
}

bool HttpManager::Init()
{
	if (!HttpClient::Init())
	{
		LogUtil::LogError("[HttpManager::Init IHttpClientInitError]");
		return false;
	}

	m_connid_to_cnum_map[HM_CONN_ID_FOR_RESOURCE] = HM_CONN_MAXNUM_FOR_RESOURCE;
	m_connid_to_cnum_map[HM_CONN_ID_FOR_PLAT_MANAGER_PHP] = HM_CONN_MAXNUM_FOR_PLAT_MANAGER_PHP;
	m_connid_to_cnum_map[HM_CONN_ID_FOR_GAMETOOL_PHP] = HM_CONN_MAXNUM_FOR_GAMETOOL_PHP;
	m_connid_to_cnum_map[HM_CONN_ID_FOR_OTHER_DEFAULT] = HM_CONN_MAXNUM_FOR_OTHER_DEFAULT;

	return true;
}

bool HttpManager::Start()
{
	return true;
}

bool HttpManager::Pause()
{
	return true;
}

bool HttpManager::Resume()
{
	return true;
}

bool HttpManager::Update(float time)
{
	// 更新处理中的请求
	for (ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.begin(); connid_to_httpconn_iter != m_connid_to_httpconn_map.end(); ++connid_to_httpconn_iter)
	{
		// 更新进度
		ZQ::UINT32 now_http_id = 0;
		ZQ::UINT32 total_len = 0;
		ZQ::UINT32 cur_len = 0;
		connid_to_httpconn_iter->second->GetProgress(&now_http_id, &total_len, &cur_len);
		
		if (now_http_id!=0)
		{
			HttpIDToReqtaskMap::iterator httpid_to_reqtask_iter = m_httpid_to_reqtask_map.find(now_http_id);
			
			//低于最低网速的情况超过一定时长将会移除当前http请求
			if(httpid_to_reqtask_iter != m_httpid_to_reqtask_map.end())
			{
				httpid_to_reqtask_iter->second.pre_time += time * 1000;
				httpid_to_reqtask_iter->second.total_len = total_len;
				httpid_to_reqtask_iter->second.cur_len = cur_len;
				if (httpid_to_reqtask_iter->second.pre_time > 1000)
				{
					HttpInfo& http_info = httpid_to_reqtask_iter->second.http_info;
					float cur_speed = (cur_len - httpid_to_reqtask_iter->second.pre_len) * 1000 / httpid_to_reqtask_iter->second.pre_time;
					if (http_info.low_speed_timeout_milliseconds != 0)
					{
						if (http_info.low_speed > cur_speed)
						{
							http_info.cur_low_speed_timeout_milliseconds += httpid_to_reqtask_iter->second.pre_time;
						}
						else
						{
							http_info.cur_low_speed_timeout_milliseconds = 0;
						}

						if (http_info.cur_low_speed_timeout_milliseconds > http_info.low_speed_timeout_milliseconds)
						{
							connid_to_httpconn_iter->second->RemoveEasyHandle();
						}
					}
					httpid_to_reqtask_iter->second.pre_time = 0;
					httpid_to_reqtask_iter->second.pre_len = cur_len;
				}
			}
			else
			{
				// 在请求队列里面有，在任务队列里却没有
				LogUtil::LogError("[HttpManager::Update] httpid_to_reqtask_iter == m_httpid_to_reqtask_map.end()");
			}
		}

		// 获取完成队列
		RequestInfoMap handled_map = connid_to_httpconn_iter->second->GetHandledRequest();
		for(RequestInfoMap::iterator requestinfo_iter = handled_map.begin(); requestinfo_iter!=handled_map.end(); requestinfo_iter++)
		{
			ZQ::UINT32 handled_http_id = requestinfo_iter->second.http_id;

			HttpIDToHttpConnMap::iterator httpid_to_httpconn_iter = m_httpid_to_httpconn_map.find(handled_http_id);
			if(httpid_to_httpconn_iter == m_httpid_to_httpconn_map.end())
			{
				LogUtil::LogError("[HttpManager::Update] httpid_to_httpconn_iter == m_httpid_to_httpconn_map.end()");
				continue;
			}

			HttpConn* http_conn = httpid_to_httpconn_iter->second;
			if(http_conn != connid_to_httpconn_iter->second)
			{
				LogUtil::LogError("[HttpManager::Update] http_conn != connid_to_httpconn_iter->second");
				continue;
			}			
			
			//已中断，跳过
			HttpIDToReqtaskMap::iterator httpid_to_reqtask_iter = m_httpid_to_reqtask_map.find(handled_http_id);
			if(httpid_to_reqtask_iter == m_httpid_to_reqtask_map.end())
			{
				//释放请求信息
				http_conn->ReleaseRequestInfo(&(requestinfo_iter->second));
				continue;
			}

			RequestTask& task = httpid_to_reqtask_iter->second;
			task.data_len = requestinfo_iter->second.data_len;
			task.data_ptr = requestinfo_iter->second.data_ptr;
			task.result = requestinfo_iter->second.result;
			task.ret_code = requestinfo_iter->second.ret_code;
			task.http_ret_code = requestinfo_iter->second.http_ret_code;
			task.op_code = requestinfo_iter->second.op_code;

			PathToHttpIDMap::iterator path_to_httpid_iter = m_path_to_httpid_map.find(task.http_info.GetHashInfo());
			CC_ASSERT(path_to_httpid_iter != m_path_to_httpid_map.end());
			if (path_to_httpid_iter != m_path_to_httpid_map.end())
			{
				m_path_to_httpid_map.erase(path_to_httpid_iter);
			}

			// 立刻进行回调
			for (RequestTask::CALLBACKLIST::iterator cb_iter = task.callback_list.begin(); cb_iter != task.callback_list.end(); ++cb_iter)
			{
				if (*cb_iter)
				{
					(*cb_iter)->OnCallBack(task.http_id, task);
					(*cb_iter)->Free();
				}
			}

			AbortRequest(handled_http_id);

			http_conn->ReleaseRequestInfo(&(requestinfo_iter->second));
		}
	}

	return true;
}

bool HttpManager::Stop()
{
	for (ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.begin();connid_to_httpconn_iter != m_connid_to_httpconn_map.end();++connid_to_httpconn_iter)
	{
		HttpConn *http_conn = connid_to_httpconn_iter->second;
		http_conn->Stop();
		delete http_conn;
	}
	m_connid_to_httpconn_map.clear();

	return true;
}

bool HttpManager::Release()
{
	HttpClient::CleanUp();
	return true;
}

void HttpManager::SetConnectTimeout(ZQ::UINT32 connid, int timeout)
{
	if (connid==0)
	{
		for (ConnIDToHttpConnMap::iterator connid_to_httpconn_iter=m_connid_to_httpconn_map.begin();connid_to_httpconn_iter!=m_connid_to_httpconn_map.end();++connid_to_httpconn_iter)
		{
			connid_to_httpconn_iter->second->SetConnectTimeout(timeout);
		}
	}
	else
	{
		std::vector<ZQ::UINT32> connids = GetConnIds(connid);
		for (std::vector<ZQ::UINT32>::iterator iter = connids.begin();iter != connids.end();++iter)
		{
			ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.find(*iter);
			if (connid_to_httpconn_iter!=m_connid_to_httpconn_map.end())
			{
				connid_to_httpconn_iter->second->SetConnectTimeout(timeout);
			}
		}
	}
}

void HttpManager::SetKeepAlive(ZQ::UINT32 connid, bool is_keepalive)
{
	if (connid==0)
	{
		for (ConnIDToHttpConnMap::iterator connid_to_httpconn_iter=m_connid_to_httpconn_map.begin();connid_to_httpconn_iter!=m_connid_to_httpconn_map.end();++connid_to_httpconn_iter)
		{
			connid_to_httpconn_iter->second->SetKeepAlive(is_keepalive);
		}
	}
	else
	{
		std::vector<ZQ::UINT32> connids = GetConnIds(connid);
		for (std::vector<ZQ::UINT32>::iterator iter = connids.begin();iter != connids.end();++iter)
		{
			ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.find(*iter);
			if (connid_to_httpconn_iter!=m_connid_to_httpconn_map.end())
			{
				connid_to_httpconn_iter->second->SetKeepAlive(is_keepalive);
			}
		}
	}
}

void HttpManager::SetMemLimit(ZQ::UINT32 connid, int total_mem)
{
	if (connid==0)
	{
		for (ConnIDToHttpConnMap::iterator connid_to_httpconn_iter=m_connid_to_httpconn_map.begin();connid_to_httpconn_iter!=m_connid_to_httpconn_map.end();++connid_to_httpconn_iter)
		{
			connid_to_httpconn_iter->second->SetMemLimit(total_mem);
		}
	}
	else
	{
		std::vector<ZQ::UINT32> connids = GetConnIds(connid);
		for (std::vector<ZQ::UINT32>::iterator iter = connids.begin();iter != connids.end();++iter)
		{
			ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.find(*iter);
			if (connid_to_httpconn_iter!=m_connid_to_httpconn_map.end())
			{
				connid_to_httpconn_iter->second->SetMemLimit(total_mem);
			}
		}
	}
}

void HttpManager::SetResourceConnNum(ZQ::UINT32 thread_num)
{
	if(thread_num <= 0 || thread_num >= RESOURCE_THREAD_MAX)
		return;

	if(m_resource_conn_num > thread_num)
	{
		for (ZQ::UINT32 num = thread_num; num < m_resource_conn_num; ++num)
		{
			ZQ::UINT32 connid = RESOURCE_THREAD_START_CONNID + num;
			ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.find(connid);
			if (connid_to_httpconn_iter != m_connid_to_httpconn_map.end())
			{
				HttpConn *http_conn = connid_to_httpconn_iter->second;
				http_conn->Stop();
				delete http_conn;
				m_connid_to_httpconn_map.erase(connid_to_httpconn_iter);
			}
		}
	}
	m_resource_conn_num = thread_num;
}

ZQ::UINT32 HttpManager::GetConvertConnId(ZQ::UINT32 connid, ZQ::UINT32 http_id)
{
	//如果是资源下载
	if (connid == HM_CONN_ID_FOR_RESOURCE)
	{
		connid = (http_id % GetResourceConnNum()) + RESOURCE_THREAD_START_CONNID;
	}
	return connid;
}

std::vector<ZQ::UINT32> HttpManager::GetConnIds(ZQ::UINT32 connid)
{
	std::vector<ZQ::UINT32> connids;
	if (connid == HM_CONN_ID_FOR_RESOURCE)
	{
		for(ZQ::UINT32 num = 0; num < GetResourceConnNum(); ++num)
		{
			connid = num + RESOURCE_THREAD_START_CONNID;
			connids.push_back(connid);
		}
	}else
	{
		connids.push_back(connid);
	}
	return connids;
}

ZQ::UINT32 HttpManager::Request(ZQ::UINT32 connid, const HttpInfo& http_info, HttpListener* listner)
{
	CC_ASSERT(connid > HM_CONN_ID_FOR_INVILID && connid < HM_CONN_ID_END);
	if (connid <= HM_CONN_ID_FOR_INVILID || connid >= HM_CONN_ID_END)
	{
		return 0;
	}

	// 获取url路径信息
	StrHashInfo str_hash_info = http_info.GetHashInfo();

	// 检查是否重复请求
	PathToHttpIDMap::iterator path_to_httpid_iter = m_path_to_httpid_map.find(str_hash_info);
	if(path_to_httpid_iter != m_path_to_httpid_map.end())
	{
		ZQ::UINT32 tmp_httpid = path_to_httpid_iter->second;
		HttpIDToReqtaskMap::iterator httpid_to_reqtask_iter = m_httpid_to_reqtask_map.find(tmp_httpid);
		if (httpid_to_reqtask_iter != m_httpid_to_reqtask_map.end())
		{
			// 追加回调
			httpid_to_reqtask_iter->second.callback_list.push_back(listner);
			return tmp_httpid;
		}
		else
		{
			// url路径有对应的httpid，却没有reqtask，肯定有问题
			CC_ASSERT(false);
			return 0;
		}
	}

	HttpConn* http_conn = 0;
	ZQ::UINT32 http_id = GetHttpID();
	ZQ::INT32 convert_connid = GetConvertConnId(connid, http_id);

	ConnIDToHttpConnMap::iterator connid_to_httpconn_iter = m_connid_to_httpconn_map.find(convert_connid);
	if (connid_to_httpconn_iter == m_connid_to_httpconn_map.end())
	{
		http_conn = new HttpConn(convert_connid, m_connid_to_cnum_map[connid]);
		m_connid_to_httpconn_map.insert(std::make_pair(convert_connid, http_conn));
	}
	else
	{
		http_conn = connid_to_httpconn_iter->second;
	}
	http_conn->Start();
	http_conn->AddReqest(http_id, http_info);

	m_httpid_to_httpconn_map.insert(std::make_pair(http_id, http_conn));

	m_path_to_httpid_map.insert(std::make_pair(str_hash_info, http_id));

	RequestTask task;
	task.http_id = http_id;
	task.http_info = http_info;
	task.callback_list.push_back(listner);
	m_httpid_to_reqtask_map.insert(std::make_pair(http_id, task));

	return http_id;
}

bool HttpManager::GetProgress(ZQ::UINT32 http_id, unsigned int &total_len, unsigned  int &cur_len)
{
	HttpIDToReqtaskMap::iterator httpid_to_reqtask_iter = m_httpid_to_reqtask_map.find(http_id);
	if(httpid_to_reqtask_iter == m_httpid_to_reqtask_map.end())
	{
		return false;
	}
	total_len = httpid_to_reqtask_iter->second.total_len;
	cur_len = httpid_to_reqtask_iter->second.cur_len;
	return true;
}

HttpProgressInfo HttpManager::GetProgress(ZQ::UINT32 http_id)
{
	HttpProgressInfo info;
	memset(&info, 0, sizeof(info));
	info.ret = GetProgress(http_id, info.total_len, info.cur_len);
	return info;
}

void HttpManager::AbortRequest(ZQ::UINT32 http_id, bool releaseReqInfo)
{
	PathToHttpIDMap::iterator path_to_httpid_iter = m_path_to_httpid_map.begin();
	while(path_to_httpid_iter != m_path_to_httpid_map.end())
	{
		ZQ::UINT32 tmp_httpid = path_to_httpid_iter->second;
		if(tmp_httpid == http_id)
		{
			m_path_to_httpid_map.erase(path_to_httpid_iter);
			break;
		}
		++path_to_httpid_iter;
	}

	HttpIDToReqtaskMap::iterator httpid_to_reqtask_iter = m_httpid_to_reqtask_map.find(http_id);
	if(httpid_to_reqtask_iter != m_httpid_to_reqtask_map.end())
	{
		m_httpid_to_reqtask_map.erase(httpid_to_reqtask_iter);
	}

	HttpIDToHttpConnMap::iterator httpid_to_httpconn_iter = m_httpid_to_httpconn_map.find(http_id);
	if (httpid_to_httpconn_iter != m_httpid_to_httpconn_map.end())
	{
		httpid_to_httpconn_iter->second->AbortRequest(http_id, releaseReqInfo);
		m_httpid_to_httpconn_map.erase(httpid_to_httpconn_iter);
	}
}

#define HTTP_CONNECT_SLEEP_MS		500

void HttpManager::HttpConn::Start()
{
	for(int i = 0; i < max_connnum; ++i)
	{
		if (!httpconnection[i].IsStarted())
		{
			httpconnection[i].SetConnectSleep(i * HTTP_CONNECT_SLEEP_MS);
			httpconnection[i].Start();
		}
	}
}

void HttpManager::HttpConn::Stop()
{
	if (httpid_list.size() > 0)
	{
		HttpConn::HttpIDList::iterator http_id_iter = httpid_list.begin();
		HttpConn::HttpIDList::iterator http_id_end = httpid_list.end();

		for (; http_id_iter != http_id_end; ++http_id_iter)
			for (int i = 0; i < max_connnum; ++i)
				httpconnection[i].AbortRequest(*http_id_iter);

		httpid_list.clear();
	}

	in_requesting = false;
	requestinfo_map.clear();

	for(int i = 0; i < max_connnum; ++i)
		httpconnection[i].Stop();
}

void HttpManager::HttpConn::AddReqest(unsigned int http_id, const HttpInfo& http_info)
{
	HttpIDList::iterator http_iter = find(httpid_list.begin(), httpid_list.end(), http_id);
	if(http_iter != httpid_list.end())
	{
		LogUtil::LogError("[HttpManager::HttpConn::AddReqest] Repeat Add %d %s", http_id, http_info.url.c_str());
		return;
	}

	httpid_list.push_back(http_id);

	if (!in_requesting)
	{
		//不在请求中，执行新请求
		in_requesting = true;
		for (int i = 0; i < max_connnum; ++i)
			httpconnection[i].AddReqest(http_id, http_info);
	}
	else
	{
		//已在请求中，缓存新请求
		RequestInfo request_info;
		request_info.http_id = http_id;
		request_info.http_info = http_info;
		requestinfo_map.insert(std::make_pair(http_id, request_info));
	}
}

//中断请求
void HttpManager::HttpConn::AbortRequest(unsigned int http_id, bool releaseReqInfo)
{
	std::vector<ZQ::UINT32>::iterator httpid_index_iter = std::find(httpid_list.begin(), httpid_list.end( ), http_id);
	if(httpid_index_iter == httpid_list.end())
	{
		LogUtil::LogError("[HttpManager::HttpConn::AbortRequest] httpid_index_iter == httpid_list.end()");
		return;
	}
	
	//移除请求
	httpid_list.erase(httpid_index_iter);

	//移除缓存的请求
	RequestInfoMap::iterator requestinfo_iter = requestinfo_map.find(http_id);
	if (requestinfo_iter != requestinfo_map.end())
	{
		requestinfo_map.erase(requestinfo_iter);
	}

	//中断HttpConnection
	for(int i = 0; i < max_connnum; ++i)
		httpconnection[i].AbortRequest(http_id);

	//释放已完成队列中的RequestInfo
	if (releaseReqInfo)
	{
		RequestInfoMap handled_map = GetHandledRequest();
		if (handled_map.empty())
		{
			CheckDoNextRequestion();
		}
		else
		{
			for (RequestInfoMap::iterator requestinfo_iter = handled_map.begin(); requestinfo_iter != handled_map.end(); requestinfo_iter++)
			{
				ReleaseRequestInfo(&(requestinfo_iter->second));
			}
		}
	}
}

void HttpManager::HttpConn::GetProgress(unsigned int *http_id, unsigned int *total_len, unsigned int *cur_len)
{
	httpconnection[0].GetProgress(http_id, total_len, cur_len);
}

void HttpManager::HttpConn::SetConnectTimeout(int timeout)
{
	for(int i = 0; i < max_connnum; ++i)
		httpconnection[i].SetConnectTimeout(timeout);
}

void HttpManager::HttpConn::SetKeepAlive(bool is_keepalive)
{
	for(int i = 0; i < max_connnum; ++i)
		httpconnection[i].SetKeepAlive(is_keepalive);
}

void HttpManager::HttpConn::SetMemLimit(int total_mem)
{
	for(int i = 0; i < max_connnum; ++i)
		httpconnection[i].SetMemLimit(total_mem);
}

RequestInfoMap HttpManager::HttpConn::GetHandledRequest()
{
	RequestInfoMap handled_request_map;

	// 获取完成队列
	for (int i = 0; i < max_connnum; ++i)
	{
		RequestInfoList handled_list = httpconnection[i].GetHandledRequest();
		for (RequestInfoList::iterator requestinfo_iter = handled_list.begin(); requestinfo_iter != handled_list.end(); requestinfo_iter++)
		{
			// 该请求已经被中断或前一条线程已经返回处理结果，废弃掉
			HttpIDList::iterator http_iter = std::find(httpid_list.begin(), httpid_list.end(), requestinfo_iter->http_id);
			if (http_iter == httpid_list.end() || handled_request_map.find(requestinfo_iter->http_id) != handled_request_map.end())
			{
				httpconnection[i].ReleaseRequestInfo(&(*requestinfo_iter));
				continue;
			}

			// 该请求成功或最后一条线程返回处理结果
			if ((requestinfo_iter->result && requestinfo_iter->ret_code == 0) || i == max_connnum - 1)
			{
				handled_request_map.insert(std::make_pair(requestinfo_iter->http_id, *requestinfo_iter));
			}
		}
	}

	return handled_request_map;
}

void HttpManager::HttpConn::ReleaseRequestInfo(RequestInfo *request_info)
{
	for(int i = 0; i <max_connnum; ++i)
	{
		httpconnection[i].ReleaseRequestInfo(request_info);
	}

	//当前请求已完成，检测执行下一个请求
	CheckDoNextRequestion();
}

void HttpManager::HttpConn::RemoveEasyHandle()
{
	for (int i = 0; i < max_connnum; i++)
	{
		httpconnection[i].RemoveEasyHandle();
	}
}

//当前请求已完成，检测执行下一个请求
void HttpManager::HttpConn::CheckDoNextRequestion()
{	
	int i = 0;
	while (i < httpid_list.size())
	{
		ZQ::UINT32 http_id = httpid_list.at(i++);
		RequestInfoMap::iterator requestinfo_iter = requestinfo_map.find(http_id);
		if (requestinfo_iter != requestinfo_map.end())
		{
			in_requesting = true;
			RequestInfo requestinfo = requestinfo_iter->second;
			for (int i = 0; i < max_connnum; ++i)
			{
				httpconnection[i].AddReqest(requestinfo.http_id, requestinfo.http_info);
			}
			return;
		}
	}
	in_requesting = false;

}
