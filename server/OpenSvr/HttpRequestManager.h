/**\file
ƽ̨��Http���������
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

	// �����̵߳���(������Ӧ���)
	VOID	Update();

	// http�������
	BOOL	HttpQuery(HttpQueryResult * pQuery);

	// ȡ���������������
	DWORD	GetBufferedQueryNum() { return m_pQueryList->GetCount(); }

private:
	inline	VOID SwitchQueryResultList() 
	{	
		CLinkedlist<HttpQueryResult *> * pTmpList = m_pQueryResultPushList; 
		m_pQueryResultPushList = m_pQueryResultPopList; 
		m_pQueryResultPopList = pTmpList;	
	}

private:
	WinHttpClient**			m_ppHttpInstance;		// todo::Ŀǰ�Ѳ�ʹ��
	ThreadPool *			m_pThreadPool;
	fnCallBackQueryResult	m_fnQueryResult;		// ������Http���󷵻ؽ���ص�
	WORD					m_wQueryProcessorNum;	// ����http���߳�����
	BOOL					m_bEndProcess;
	HANDLE					m_hQEPWakeupEvent;		// ����http�����¼�
	CriticalSection *		m_pQueryCS;
	CriticalSection *		m_pQueryResultCS;

	CLinkedlist<HttpQueryResult*>*	m_pQueryList;			 // ���������б�(�����̴߳�����ȡ����������)
	CLinkedlist<HttpQueryResult*>*	m_pQueryResultPushList;  // �����ؽ���б�(�߳�֮�佻�����н��б�)
	CLinkedlist<HttpQueryResult*>*	m_pQueryResultPopList;	 // �����ؽ���б�(���̴߳�����б�)	
};


// ִ������������߼���(һ��һ���߳�)
class QueryExecuteProcessor : public ThreadBase
{
public:
	QueryExecuteProcessor(HttpRequestManager& HttpReqMgr, WinHttpClient * pHttpInstance);
	virtual ~QueryExecuteProcessor(void);

	virtual VOID	EndThread();
	virtual DWORD	run();

public:
	int m_nIndex;	// ����ʹ��

private:
	HttpRequestManager&	m_HttpReqMgr;
	HANDLE m_hEndThreadEvent;		// �߳̽����¼�
	WinHttpClient*	m_pHttpInstance;
	CHttpRequest* m_pHttpRequest;
};
