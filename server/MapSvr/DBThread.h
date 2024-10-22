#pragma once

#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/CriticalSection.h"
#include "../ToolKit/Linkedlist.h"
#include "../ToolKit/Singleton.h"
#include <hash_map>

class QueryResult;
class SqlDatabase;

typedef void (*fnCallBackMessage)( char * szMessage );
typedef void (*fnCallBackQueryResult)( DWORD dwIndex, QueryResult * pData );
typedef void (*fnProcessQuery)( SqlDatabase* pSqlDatabase, QueryResult * pData );

//���ݿ�������Ϣ
struct DatabaseDesc
{
	char *						pszDatabaseIP;
	char *						pszDatabaseName;
	char *						pszUserName;
	char *						pszUserPassword;
	int							nPort;	
	fnCallBackQueryResult		fnQueryResult;
	fnCallBackMessage			fnErrorMessage;
};

//���ݿ��߳�
class DBThread : public ThreadBase
{
public:
	DBThread(void);
	virtual ~DBThread(void);

public:
	BOOL						Initialize( DatabaseDesc & desc );
	VOID						Update();
	virtual void				Release();
	BOOL						DBQuery( QueryResult * pQuery );
	size_t						GetQueryListSize() { return m_pQueryList->GetCount(); }

protected:
	virtual void				EndThread();
	virtual DWORD				run();
	fnProcessQuery				Find(DWORD dwIndex);

private:
	SqlDatabase*				m_pDatabase;			//���ݿ����
	fnCallBackMessage			m_fnErrorMessage;		//��ѯ������Ϣ������ָ��
	fnCallBackQueryResult		m_fnQueryResult;		//��ѯ���������ָ��
	BOOL						m_bEndProcess;			//�߳̽�����־
	CriticalSection *			m_pQueryCS;				//����ѯ�б������
	CriticalSection *			m_pQueryResultCS;		//��ѯ����б������
	CLinkedlist<QueryResult*>*	m_pQueryList;			//����ѯ�б�
	CLinkedlist<QueryResult*>*	m_pQueryResultList;		//��ѯ����б�
};

//���ݿ��̹߳�����
class DBThreadManager : public Singleton<DBThreadManager>
{
public:
	DBThreadManager();
	~DBThreadManager();
public:
	BOOL						Initialize( DatabaseDesc & desc,DWORD dwDBThreadNum );
	void						Update();
	void						Release();
	BOOL						DBQuery( DWORD dwDBThreadIndex,QueryResult * pQuery );
	fnProcessQuery				Find(DWORD dwIndex);
	DBThread*					GetDBThread(DWORD& dwDBThreadNum) 
	{ 
		dwDBThreadNum = m_dwDBThreadNum; 
		return m_pDBThread; 
	}

private:
	void						Register();
	void						AddHandler(DWORD dwIndex, fnProcessQuery fnHandler);
	

private:
	DBThread*					m_pDBThread;				//�����߳��б�
	DWORD						m_dwDBThreadNum;			//�����߳�����
	//std::hash_map<DWORD,fnProcessQuery>	m_ProcessQueryMap;	//��ѯ�������б�
	std::map<DWORD,fnProcessQuery>	m_ProcessQueryMap;	//��ѯ�������б�
};