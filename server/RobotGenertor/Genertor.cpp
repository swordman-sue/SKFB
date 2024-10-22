#include "stdafx.h"
#include "Genertor.h"
#include "../Common/DBCDefine.h"
#include "SqlDatabase.h"
#include <stdlib.h>
#include "../MapSvr/MapSvrQuery.h"
#include "data_define.pb.h"


Genertor::Genertor()
{
	m_pDatabase			 = NULL;
	m_pProcessCS		 = NULL;
	m_pProcessResultCS	 = NULL;
	m_pProcessList		 = NULL;
	m_pProcessResultList = NULL;
}

Genertor::~Genertor()
{

}

BOOL Genertor::Initialize(DatabaseDesc& desc, DWORD dwZoneID)
{
	if(!desc.pszUserName || !desc.pszUserPassword || !desc.pszDatabaseName || !desc.pszDatabaseIP)
		return FALSE;

	m_pDatabase = new SqlDatabase;

	if(!m_pDatabase->Connect(desc.pszDatabaseName,desc.pszDatabaseIP,desc.pszUserName,desc.pszUserPassword))
		return FALSE;

	m_dwZoneID = dwZoneID;

	//缓冲区互斥变量
	m_pProcessCS	      = new CriticalSection;
	m_pProcessResultCS	  = new CriticalSection;

	m_pProcessList		  = new CLinkedlist<RobotEntry*>(500);
	m_pProcessResultList  = new CLinkedlist<RobotEntry*>(500);

	m_bEndProcess = FALSE;

	//开始线程处理函数
	StartThread();

	return TRUE;
}

//该函数由上层逻辑线程调用，取出处理结果，调用回调函数处理
VOID Genertor::Update()
{
	if(!m_pProcessResultList ||  m_pProcessResultList->GetCount() == 0)
		return;

	BOOL bInfiniteLoop = TRUE;
	while( bInfiniteLoop )
	{
		m_pProcessResultCS->Lock();
		RobotEntry* pRobotEntry = m_pProcessResultList->DeleteHead();
		m_pProcessResultCS->Unlock();

		if (pRobotEntry == NULL)
			break;

		//此处回调管理器来处理结果
		GenertorManager::Instance()->ProcessResult(pRobotEntry);
	}
}

void Genertor::Release()
{
	m_bEndProcess = TRUE;

	//调用基类版本，等待线程结束
	ThreadBase::Release();

	//清除数据库连接
	if (m_pDatabase)
	{
		if(m_pDatabase->IsConnected())
			m_pDatabase->Disconnect();

		delete m_pDatabase;
	}

	//清除缓冲区互斥变量
	delete m_pProcessCS;
	delete m_pProcessResultCS;

	//清除缓冲区列表
	if (m_pProcessList)
	{
		m_pProcessList->DeleteAll();
		delete m_pProcessList;
	}

	if (m_pProcessResultList)
	{
		m_pProcessResultList->DeleteAll();
		delete m_pProcessResultList;
	}
}

//该函数由上层逻辑线程调用,把处理放到列表中
BOOL Genertor::DBProcess(RobotEntry* pRobotEntry)
{
	if(!pRobotEntry)
		return FALSE;

	if( m_bEndProcess )
		return FALSE;

	// 加入处理列表
	m_pProcessCS->Lock();
	m_pProcessList->AddTail(pRobotEntry);
	m_pProcessCS->Unlock();

	return TRUE;
}

//线程处理函数，负责对处理列表进行处理
DWORD Genertor::run()
{

	m_pDatabase->ThreadStart();

	RobotEntry* pRobotEntry = NULL;
	while(!m_bEndProcess)
	{
		BOOL bQueryInfiniteLoop = TRUE;
		while(bQueryInfiniteLoop)
		{
			if( m_pProcessList->GetCount() == 0 )	
				break;

			m_pProcessCS->Lock();
			pRobotEntry = m_pProcessList->DeleteHead();
			m_pProcessCS->Unlock();

			if(!pRobotEntry)
				break;

			//创建机器人
			CreateRobot(pRobotEntry);

			//把处理结果放到结果列表中
			m_pProcessResultCS->Lock();
			m_pProcessResultList->AddTail(pRobotEntry);
			m_pProcessResultCS->Unlock();
		}


		Sleep(1);
	}

	//退出线程函数
	m_pDatabase->ThreadEnd();

	return 0;
}

VOID Genertor::EndThread()
{

}

//创建机器人
BOOL Genertor::CreateRobot(RobotEntry* pRobotEntry)
{
 	if (!pRobotEntry || !m_pDatabase)
 		return FALSE;
 
 	DWORD dwRoleId;
	DWORD dwHeadImageId = 30300; // todo::暂时介么处理

	//更新玩家登陆时间
	CHAR szCreateTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szCreateTime);

	// 基本信息
	PB::RoleBaseInfo PBRoleBaseInfo;
	PBRoleBaseInfo.set_account(pRobotEntry->m_szAccID);
	PBRoleBaseInfo.set_role_name(pRobotEntry->m_szCharName);
	PBRoleBaseInfo.set_sex(1);
	PBRoleBaseInfo.set_level(pRobotEntry->m_dwLevel);
	PBRoleBaseInfo.set_head_image_id(dwHeadImageId);

	// 基本信息
	string strBaseInfoData("");
	if (!PBRoleBaseInfo.AppendToString(&strBaseInfoData) || strBaseInfoData.size() > MAX_ROLE_BASE_INFO_SIZE)
	{
		printf("[%s]角色基本数据系列化后过大[size:%d]", __FUNCTION__, strBaseInfoData.size());
		return FALSE;
	}

	// 基本信息
	char szBaseInfo[MAX_ROLE_BASE_INFO_SIZE * 2 + 1] = { 0 };
	m_pDatabase->escape_string(szBaseInfo, strBaseInfoData.c_str(), strBaseInfoData.size());

	sprintf(m_pDatabase->GetQuery()
		, "Insert Into Role(role_name,account,server_id,platform,sex,level,head_image_id,create_time,base_info) \
		VALUE ('%s','%s', %d, %d, %d, %d, %d,'%s','%s')"
		, pRobotEntry->m_szCharName
		, pRobotEntry->m_szAccID
		, m_dwZoneID
		, 1
		, 1
		, pRobotEntry->m_dwLevel
		, dwHeadImageId
		, szCreateTime
		, szBaseInfo);
	int nRet = m_pDatabase->Execute();
 	if (nRet == -1) 
 		return FALSE;
 
 	//得到插入的CharID
 	dwRoleId = m_pDatabase->GetInsertId();

	return TRUE;
}