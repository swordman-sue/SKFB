/**\file
平台服Http请求管理器
\author Kevin
\date 2016/8/30
\note
*/

#pragma once

#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/ThreadPool.h"
#include "../ToolKit/CriticalSection.h"
#include "../ToolKit/Linkedlist.h"
#include <string>

class HttpQueryResult;
class CHttpRequest;

typedef void (*fnCallBackQueryResult)( DWORD dwIndex, HttpQueryResult * pData );

class WinHttpClient;

class HttpRequestManager
{
	friend class QueryExecuteProcessor;
public:
	HttpRequestManager(void);
	~HttpRequestManager(void);

	BOOL	Initialize(const std::wstring &HostName, DWORD dwHttpPort, DWORD wProcessNum, fnCallBackQueryResult fnQueryResult);
	VOID	Release();

	// 由主线程调用(处理响应结果)
	VOID	Update();

	// http请求入口
	BOOL	HttpQuery(HttpQueryResult * pQuery);

	// 取待处理的请求数量
	DWORD	GetBufferedQueryNum() { return m_pQueryList->GetCount(); }

private:
	inline	VOID SwitchQueryResultList() 
	{	
		CLinkedlist<HttpQueryResult *> * pTmpList = m_pQueryResultPushList; 
		m_pQueryResultPushList = m_pQueryResultPopList; 
		m_pQueryResultPopList = pTmpList;	
	}

private:
	WinHttpClient**			m_ppHttpInstance;		// todo::目前已不使用
	ThreadPool *			m_pThreadPool;
	fnCallBackQueryResult	m_fnQueryResult;		// 第三方Http请求返回结果回调
	WORD					m_wQueryProcessorNum;	// 处理http的线程数量
	BOOL					m_bEndProcess;
	HANDLE					m_hQEPWakeupEvent;		// 唤醒http请求事件
	CriticalSection *		m_pQueryCS;
	CriticalSection *		m_pQueryResultCS;

	CLinkedlist<HttpQueryResult*>*	m_pQueryList;			 // 待请求处理列表(多条线程从里面取数据来处理)
	CLinkedlist<HttpQueryResult*>*	m_pQueryResultPushList;  // 处理返回结果列表(线程之间交互的中介列表)
	CLinkedlist<HttpQueryResult*>*	m_pQueryResultPopList;	 // 处理返回结果列表(主线程处理的列表)	
};


// 执行请求操作的逻辑类(一个一条线程)
class QueryExecuteProcessor : public ThreadBase
{
public:
	QueryExecuteProcessor(HttpRequestManager& HttpReqMgr, WinHttpClient * pHttpInstance);
	virtual ~QueryExecuteProcessor(void);

	virtual VOID	EndThread();
	virtual DWORD	run();

public:
	int m_nIndex;	// 测试使用

private:
	HttpRequestManager&	m_HttpReqMgr;
	HANDLE m_hEndThreadEvent;		// 线程结束事件
	WinHttpClient*	m_pHttpInstance;
	CHttpRequest* m_pHttpRequest;
};
