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

//数据库描述信息
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

//数据库线程
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
	SqlDatabase*				m_pDatabase;			//数据库对象
	fnCallBackMessage			m_fnErrorMessage;		//查询错误消息处理函数指针
	fnCallBackQueryResult		m_fnQueryResult;		//查询结果处理函数指针
	BOOL						m_bEndProcess;			//线程结束标志
	CriticalSection *			m_pQueryCS;				//待查询列表互斥变量
	CriticalSection *			m_pQueryResultCS;		//查询结果列表互斥变量
	CLinkedlist<QueryResult*>*	m_pQueryList;			//待查询列表
	CLinkedlist<QueryResult*>*	m_pQueryResultList;		//查询结果列表
};

//数据库线程管理器
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
	DBThread*					m_pDBThread;				//处理线程列表
	DWORD						m_dwDBThreadNum;			//处理线程数量
	//std::hash_map<DWORD,fnProcessQuery>	m_ProcessQueryMap;	//查询处理函数列表
	std::map<DWORD,fnProcessQuery>	m_ProcessQueryMap;	//查询处理函数列表
};