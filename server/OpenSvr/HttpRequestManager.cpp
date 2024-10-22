#include "stdafx.h"
#include "HttpRequestManager.h"
#include "WinHttpClient.h"
#include "HttpQueryResult.h"
#include "HttpRequest.h"
#include "..\Common\CommonLogic.h"

#define HTTP_QUERY_POOL_SIZE	500

HttpRequestManager::HttpRequestManager(void)
:	m_ppHttpInstance ( NULL ),
	m_pThreadPool ( NULL ),
	m_fnQueryResult ( NULL ),
	m_pQueryList ( NULL ),
	m_pQueryCS ( NULL ),
	m_pQueryResultCS ( NULL ),
	m_pQueryResultPushList ( NULL ),
	m_pQueryResultPopList ( NULL ),
	m_hQEPWakeupEvent ( NULL ),
	m_wQueryProcessorNum ( 0 ),
	m_bEndProcess( FALSE )
		
{

}

HttpRequestManager::~HttpRequestManager(void)
{
}


VOID HttpRequestManager::Release()
{
	m_bEndProcess = TRUE;

	SAFE_DELETE( m_pThreadPool );

	CloseHandle( m_hQEPWakeupEvent );
	
	SAFE_DELETE( m_pQueryList );
	SAFE_DELETE( m_pQueryResultPushList );
	SAFE_DELETE( m_pQueryResultPopList );
	
	if( m_ppHttpInstance )
	{
		for( WORD i = 0 ; i < m_wQueryProcessorNum ; ++i )
		{
			delete m_ppHttpInstance[i];
			m_ppHttpInstance[i] = NULL;
		}
		delete [] m_ppHttpInstance;
		m_ppHttpInstance = NULL;
	}
	SAFE_DELETE( m_pQueryCS );
	SAFE_DELETE( m_pQueryResultCS );
}

BOOL HttpRequestManager::Initialize(const std::wstring &HostName, DWORD dwHttpPort, DWORD wProcessNum, fnCallBackQueryResult fnQueryResult)
{
	m_fnQueryResult			= fnQueryResult;
	m_wQueryProcessorNum	= wProcessNum;

	m_ppHttpInstance = new WinHttpClient*[m_wQueryProcessorNum];

	for( WORD i = 0 ; i < m_wQueryProcessorNum ; ++i )
	{
		m_ppHttpInstance[i] = new WinHttpClient;
	}

	for( WORD i = 0 ; i < m_wQueryProcessorNum ; ++i )
	{
		if( !m_ppHttpInstance[i]->InitHttpConnect( HostName, dwHttpPort ) )
			return FALSE;
	}


	m_hQEPWakeupEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	m_pQueryCS				= new CriticalSection;
	m_pQueryResultCS		= new CriticalSection;

	m_pQueryList			= new CLinkedlist<HttpQueryResult *>( HTTP_QUERY_POOL_SIZE );
	m_pQueryResultPushList  = new CLinkedlist<HttpQueryResult *>( HTTP_QUERY_POOL_SIZE );
	m_pQueryResultPopList  = new CLinkedlist<HttpQueryResult *>( HTTP_QUERY_POOL_SIZE );

	m_pThreadPool = new ThreadPool;
	
	ThreadBase * pThread = NULL;
	for( WORD i = 0 ; i < m_wQueryProcessorNum ; ++i )
	{
		pThread = new QueryExecuteProcessor( *this, m_ppHttpInstance[i] );

		((QueryExecuteProcessor*)pThread)->m_nIndex = i; // todo::测试使用

		m_pThreadPool->AddThread( pThread, TRUE );
	}

	m_pThreadPool->BatchStartThread();

	m_bEndProcess = FALSE;

	return TRUE;
}

BOOL HttpRequestManager::HttpQuery(HttpQueryResult * pQuery)
{
	if (!pQuery)
	{
		return FALSE;
	}

	if( m_bEndProcess )
		return FALSE;

	m_pQueryCS->Lock();
	m_pQueryList->AddTail( pQuery );
	m_pQueryCS->Unlock();

	SetEvent( m_hQEPWakeupEvent );

	return TRUE;
}

VOID HttpRequestManager::Update()
{
	if( !m_pQueryResultPushList || (m_pQueryResultPushList && m_pQueryResultPushList->GetCount() == 0 ))
		return ;
	
	m_pQueryResultCS->Lock();

	SwitchQueryResultList();

	m_pQueryResultCS->Unlock();

	BOOL bInfiniteLoop = TRUE;
	while( bInfiniteLoop )
	{
		HttpQueryResult* pQResult = m_pQueryResultPopList->DeleteHead();

		if (pQResult==NULL)
		{
			break;
		}

		m_fnQueryResult( pQResult->GetIndex(), pQResult );
	}
}

QueryExecuteProcessor::QueryExecuteProcessor(HttpRequestManager& HttpReqMgr, WinHttpClient * pHttpInstance)
: m_HttpReqMgr(HttpReqMgr),
	m_pHttpInstance ( pHttpInstance )
{
	m_hEndThreadEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_pHttpRequest = new CHttpRequest();
}

QueryExecuteProcessor::~QueryExecuteProcessor(void)
{
	CloseHandle( m_hEndThreadEvent );

	if (m_pHttpRequest)
		delete m_pHttpRequest;
}

VOID QueryExecuteProcessor::EndThread()
{
	SetEvent( m_hEndThreadEvent );
}

DWORD QueryExecuteProcessor::run()
{
	HttpQueryResult * pQuery = NULL;
	HANDLE hEvent[2];
	hEvent[0] = m_HttpReqMgr.m_hQEPWakeupEvent;
	hEvent[1] = m_hEndThreadEvent;

	CriticalSection *& qcs = m_HttpReqMgr.m_pQueryCS;
	CriticalSection *& qrcs = m_HttpReqMgr.m_pQueryResultCS;

	BOOL bEventInfiniteLoop = TRUE;
	while(bEventInfiniteLoop)
	{
		// 同时在监听两个事件(http请求事件,线程结束事件)
		if( WaitForMultipleObjects( 2, hEvent, FALSE, INFINITE ) == WAIT_OBJECT_0 + 1 )
			break;

		BOOL bQueryInfiniteLoop = TRUE;
		while(bQueryInfiniteLoop)
		{
			DWORD dwQueryCount = m_HttpReqMgr.m_pQueryList->GetCount();
			if (dwQueryCount == 0)
				break;

			qcs->Lock();
			pQuery = m_HttpReqMgr.m_pQueryList->DeleteHead();
			qcs->Unlock();

			if( !pQuery )
				break;

			int nRequestType = pQuery->GetRequestType();
			if (HTTP_REQUEST_GET == nRequestType || HTTP_REQUEST_POST == nRequestType)
			{
				UINT64 lCurTickCount = g_GetCurrentTime();

				bool isRequestSucce;
				if (HTTP_REQUEST_GET == nRequestType)
					isRequestSucce = m_pHttpRequest->SendRequest_Get(pQuery->GetHost(), pQuery->GetPort(), pQuery->GetPath(), pQuery->GetContext());
				else
					isRequestSucce = m_pHttpRequest->SendRequest_Post(pQuery->GetHost(), pQuery->GetPort(), pQuery->GetPath(), pQuery->GetContext());

				DWORD dwUsedTime = g_GetCurrentTime() - lCurTickCount;

				// todo::bug定位
				OPENLOG(GUI_LEVEL, "http请求[Cost:%d,GuidId:%d,Index:%d,QueryCount:%d,Host:%s]", 
					dwUsedTime, pQuery->GetKey(), m_nIndex, dwQueryCount, pQuery->GetHost());

				if (isRequestSucce)
				{
					if (200 != m_pHttpRequest->GetStateCode())
					{
						pQuery->SetQueryResult(0);
					}
					else
					{
						pQuery->SetQueryResult(1);
						pQuery->SetHttpResult(m_pHttpRequest->GetResponseData().c_str());
					}
				}
			}
			else
			{
				//处理http请求
				char szHttpBuff[MAX_MSG_BUFF_LEN];
				wchar_t wszHttpBuff[MAX_MSG_BUFF_LEN];

				UINT64 lCurTickCount = g_GetCurrentTime();

				// 创建请求URL
				pQuery->BuildHttpRequest(szHttpBuff);

				// 字符格转换
				pQuery->ChangeToWildChar(szHttpBuff, wszHttpBuff, sizeof(wszHttpBuff) / sizeof(wchar_t));

				// 发出操作请求
				DWORD dwResult = m_pHttpInstance->SendHttpRequestEx(szHttpBuff);

				DWORD dwUsedTime = g_GetCurrentTime() - lCurTickCount;

				//#ifdef _DEBUG
				OPENLOG(GUI_LEVEL, "[cost:%d,index:%d]http请求[%s]", dwUsedTime, m_nIndex, szHttpBuff);
				//#endif
				// 记录请求返回结果
				pQuery->SetQueryResult(dwResult);
				if (dwResult)
				{
					pQuery->SetHttpResult(m_pHttpInstance->GetResponseContent());
				}
			}

			qrcs->Lock();
			m_HttpReqMgr.m_pQueryResultPushList->AddTail(pQuery);
			qrcs->Unlock();
		}
	}
	return 0;
}