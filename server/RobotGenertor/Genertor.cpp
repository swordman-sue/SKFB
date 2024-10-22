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

	//�������������
	m_pProcessCS	      = new CriticalSection;
	m_pProcessResultCS	  = new CriticalSection;

	m_pProcessList		  = new CLinkedlist<RobotEntry*>(500);
	m_pProcessResultList  = new CLinkedlist<RobotEntry*>(500);

	m_bEndProcess = FALSE;

	//��ʼ�̴߳�����
	StartThread();

	return TRUE;
}

//�ú������ϲ��߼��̵߳��ã�ȡ�������������ûص���������
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

		//�˴��ص���������������
		GenertorManager::Instance()->ProcessResult(pRobotEntry);
	}
}

void Genertor::Release()
{
	m_bEndProcess = TRUE;

	//���û���汾���ȴ��߳̽���
	ThreadBase::Release();

	//������ݿ�����
	if (m_pDatabase)
	{
		if(m_pDatabase->IsConnected())
			m_pDatabase->Disconnect();

		delete m_pDatabase;
	}

	//����������������
	delete m_pProcessCS;
	delete m_pProcessResultCS;

	//����������б�
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

//�ú������ϲ��߼��̵߳���,�Ѵ���ŵ��б���
BOOL Genertor::DBProcess(RobotEntry* pRobotEntry)
{
	if(!pRobotEntry)
		return FALSE;

	if( m_bEndProcess )
		return FALSE;

	// ���봦���б�
	m_pProcessCS->Lock();
	m_pProcessList->AddTail(pRobotEntry);
	m_pProcessCS->Unlock();

	return TRUE;
}

//�̴߳�����������Դ����б���д���
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

			//����������
			CreateRobot(pRobotEntry);

			//�Ѵ������ŵ�����б���
			m_pProcessResultCS->Lock();
			m_pProcessResultList->AddTail(pRobotEntry);
			m_pProcessResultCS->Unlock();
		}


		Sleep(1);
	}

	//�˳��̺߳���
	m_pDatabase->ThreadEnd();

	return 0;
}

VOID Genertor::EndThread()
{

}

//����������
BOOL Genertor::CreateRobot(RobotEntry* pRobotEntry)
{
 	if (!pRobotEntry || !m_pDatabase)
 		return FALSE;
 
 	DWORD dwRoleId;
	DWORD dwHeadImageId = 30300; // todo::��ʱ��ô����

	//������ҵ�½ʱ��
	CHAR szCreateTime[MAX_TIME_LEN] = { 0 };
	DATETIME CurTime(time(NULL));
	CurTime.toString(szCreateTime);

	// ������Ϣ
	PB::RoleBaseInfo PBRoleBaseInfo;
	PBRoleBaseInfo.set_account(pRobotEntry->m_szAccID);
	PBRoleBaseInfo.set_role_name(pRobotEntry->m_szCharName);
	PBRoleBaseInfo.set_sex(1);
	PBRoleBaseInfo.set_level(pRobotEntry->m_dwLevel);
	PBRoleBaseInfo.set_head_image_id(dwHeadImageId);

	// ������Ϣ
	string strBaseInfoData("");
	if (!PBRoleBaseInfo.AppendToString(&strBaseInfoData) || strBaseInfoData.size() > MAX_ROLE_BASE_INFO_SIZE)
	{
		printf("[%s]��ɫ��������ϵ�л������[size:%d]", __FUNCTION__, strBaseInfoData.size());
		return FALSE;
	}

	// ������Ϣ
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
 
 	//�õ������CharID
 	dwRoleId = m_pDatabase->GetInsertId();

	return TRUE;
}