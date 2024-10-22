#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "httpstruct.h"

#include "common/platform/thread/mutex.h"
#include "common/platform/thread/thread.h"
#include "common/platform/thread/semaphorex.h"

#include <map>
#include <vector>

class HttpClient;

DWORD ResourceHandleFunc(void* pData);
int ProgressCallBack(void *p, size_t recv_len, size_t total_len);

class HttpConnection
{
	friend DWORD ResourceHandleFunc(void* pData);
	friend int ProgressCallBack(void *p, size_t recv_len, size_t total_len);

private:
	typedef std::vector<unsigned int> AbortHttpIDList;

public:
	HttpConnection();
	~HttpConnection();

	void Start();
	void Stop();

	void AddReqest(unsigned int http_id, const HttpInfo& http_info);
	void AbortRequest(unsigned int http_id);
	void GetProgress(unsigned int *http_id, unsigned int *total_len, unsigned int *cur_len);
	RequestInfoList GetHandledRequest();
	void ReleaseRequestInfo(RequestInfo *request_info);

	void SetConnectTimeout(int timeout);
	void SetKeepAlive(bool is_keepalive);
	void SetMemLimit(int total_mem);
	void SetConnectSleep(int ms);
	bool IsStarted() const { return m_is_started; }
	void RemoveEasyHandle();

private:
	// �����߳�ִ�еĺ���
	void ThreadFun();
	void ExcuteHttpRequest(RequestInfo *request_info, HttpClient* http_client);

private:
	bool m_is_started;				// ģ���Ƿ��Ѿ���ʼ����
	bool volatile m_is_stoped;		// ģ���Ƿ��Ѿ�ֹͣ����

	int volatile m_httpclient_connect_timeout;
	bool volatile m_httpclient_is_keepalive;
	int volatile m_total_mem_len;
	int volatile m_used_mem_len;

	int volatile m_httpclient_connect_sleep;	// ִ������ǰ����ʱ�� add by chenyingpeng

	// �߳�
	Thread m_thread;

private:
	RequestInfoMap m_unhandle_map;			// δ��������
	Mutex m_mutex_for_unhandle_map;			// δ�����������
	Semaphore m_sem_for_unhandle_map;		// δ����������ź���

	unsigned int volatile m_requesting_http_id;
	unsigned int volatile m_requesting_http_total_len;
	unsigned int volatile m_requesting_http_cur_len;

	RequestInfoList m_handled_queue;		// ������ɶ���
	Mutex m_mutex_for_handled_queue;		// ������ɶ��е���

	RequestInfoMap m_release_map;			//δ�ͷŵ���������
	HttpClient* m_http_client;
};

#endif
