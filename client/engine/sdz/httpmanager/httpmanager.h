#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include "httpstruct.h"
#include "httplistener.h"
#include "httpconnection.h"

struct HttpProgressInfo
{
	bool ret;
	unsigned int total_len;
	unsigned int cur_len;
};

class HttpManager
{
public:
	enum
	{
		HM_CONN_ID_FOR_INVILID = 0,
		HM_CONN_ID_FOR_RESOURCE = 1,
		HM_CONN_ID_FOR_PLAT_MANAGER_PHP,
		HM_CONN_ID_FOR_GAMETOOL_PHP,
		HM_CONN_ID_FOR_OTHER_DEFAULT,
		HM_CONN_ID_END,
	};

	enum
	{
		HM_CONN_MAXNUM_FOR_INVILID			= 0,
		HM_CONN_MAXNUM_FOR_RESOURCE			= 1,
		HM_CONN_MAXNUM_FOR_PLAT_MANAGER_PHP	= 3,
		HM_CONN_MAXNUM_FOR_GAMETOOL_PHP		= 1,
		HM_CONN_MAXNUM_FOR_OTHER_DEFAULT	= 1,

		HM_CONN_MAXNUM						= 3,
	};

	static const unsigned int RESOURCE_THREAD_MAX = 8;
public:
	static HttpManager* GetInstance();
	static void DelInstance();
	~HttpManager();

	bool Init();
	bool Start();
	bool Update(float time);
	bool Pause();
	bool Resume();
	bool Stop();
	bool Release();

	ZQ::UINT32 Request(ZQ::UINT32 connid, const HttpInfo& http_info, HttpListener* listner);
	bool GetProgress(ZQ::UINT32 http_id, unsigned int &total_len, unsigned  int &cur_len);
	HttpProgressInfo GetProgress(ZQ::UINT32 http_id);
	void AbortRequest(ZQ::UINT32 http_id, bool releaseReqInfo = false);
	ZQ::UINT32 GetResourceConnNum(){return m_resource_conn_num;};
	void SetResourceConnNum(ZQ::UINT32 thread_num);
	void SetConnectTimeout(ZQ::UINT32 connid, int timeout);
	void SetKeepAlive(ZQ::UINT32 connid, bool is_keepalive);
	void SetMemLimit(ZQ::UINT32 connid, int total_mem);

private:
	struct HttpConn
	{
	private:
		typedef std::vector<ZQ::UINT32> HttpIDList;
		ZQ::UINT32		connid;
		int				max_connnum;
		HttpIDList		httpid_list;
		RequestInfoMap	requestinfo_map;
		HttpConnection	httpconnection[HM_CONN_MAXNUM];
		bool			in_requesting;

	public:
		HttpConn() :connid(0), max_connnum(0), in_requesting(false){}
		HttpConn(ZQ::UINT32 cid, ZQ::UINT32 cnum) :connid(cid), max_connnum(cnum), in_requesting(false){}

	public:

		void Start();
		void Stop();

		void AddReqest(unsigned int http_id, const HttpInfo& http_info);
		void AbortRequest(unsigned int http_id, bool releaseReqInfo = false);
		void GetProgress(unsigned int *http_id, unsigned int *total_len, unsigned int *cur_len);
		void RemoveEasyHandle();

		void SetConnectTimeout(int timeout);
		void SetKeepAlive(bool is_keepalive);
		void SetMemLimit(int total_mem);

		RequestInfoMap GetHandledRequest();
		void ReleaseRequestInfo(RequestInfo *request_info);

	private:
		void CheckDoNextRequestion();
	};

	typedef std::map<ZQ::UINT32, HttpConn*> ConnIDToHttpConnMap;
	ConnIDToHttpConnMap	m_connid_to_httpconn_map;

	typedef std::map<ZQ::UINT32, HttpConn*> HttpIDToHttpConnMap;
	HttpIDToHttpConnMap	m_httpid_to_httpconn_map;

	typedef std::map<ZQ::UINT32, RequestTask> HttpIDToReqtaskMap;
	HttpIDToReqtaskMap	m_httpid_to_reqtask_map;

	typedef std::map<StrHashInfo, ZQ::UINT32> PathToHttpIDMap;

private:
	HttpManager():m_resource_conn_num(4){};
	ZQ::UINT32 GetConvertConnId(ZQ::UINT32 connid,ZQ::UINT32 http_id);
	std::vector<ZQ::UINT32> GetConnIds(ZQ::UINT32 connid);

private:
	ZQ::UINT32 m_resource_conn_num;
	PathToHttpIDMap	m_path_to_httpid_map;
	std::map<int, int> m_connid_to_cnum_map;
};

#endif
