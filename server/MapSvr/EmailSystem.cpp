#include "stdafx.h"
#include "EmailSystem.h"
#include "../Common/MsgDefine.h"
#include "Player.h"
#include "MapSvrQuery.h"
#include "PlayerManager.h"
#include "AccountInfo.h"
#include "../ToolKit/DBCFileManager.h"
#include "TimeoutManager.h"
#include"../ToolKit/Rand.h"
EmailSystem::EmailSystem()
{
	
}

EmailSystem::~EmailSystem()
{

}

SendEmailEntry* EmailSystem::GetSendEmailEntry( DWORD dwID )
{
	if (!dwID)
		return NULL;

	DBCFile* pDBCFile = DBCFileManager::Instance()->GetDBC(DBC_EMAILSQL_ENTRY);
	if (!pDBCFile)
		return NULL;

	return (SendEmailEntry*)pDBCFile->GetRecord(dwID);
}

EmailEntry* EmailSystem::GetEmailEntry( DWORD dwID )
{
	if (!dwID)
		return NULL;

	DBCFile* pDBCFile = DBCFileManager::Instance()->GetDBC(DBC_EMAIL_ENTRY);
	if (!pDBCFile)
		return NULL;

	return (EmailEntry*)pDBCFile->GetRecord(dwID);
}
void	EmailSystem::GetEmailAttachmentInfo(EmailEntry* pInfo,EmailInfo& Info)
{
// 	if ((0!= pInfo->m_nType1)&&(0!=pInfo->m_nNum1)  )
// 	{
// 		Info.m_Attachment[0].m_dwType = pInfo->m_nType1;
// 		Info.m_Attachment[0].m_dwNum = pInfo->m_nNum1;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType2)&&(0!=pInfo->m_nNum2)  )
// 	{
// 		Info.m_Attachment[1].m_dwType = pInfo->m_nType2;
// 		Info.m_Attachment[1].m_dwNum = pInfo->m_nNum2;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType3)&&(0!=pInfo->m_nNum3)  )
// 	{
// 		Info.m_Attachment[2].m_dwType = pInfo->m_nType3;
// 		Info.m_Attachment[2].m_dwNum = pInfo->m_nNum3;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType4)&&(0!=pInfo->m_nNum4)  )
// 	{
// 		Info.m_Attachment[3].m_dwType = pInfo->m_nType4;
// 		Info.m_Attachment[3].m_dwNum = pInfo->m_nNum4;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType5)&&(0!=pInfo->m_nNum5)  )
// 	{
// 		Info.m_Attachment[4].m_dwType = pInfo->m_nType5;
// 		Info.m_Attachment[4].m_dwNum = pInfo->m_nNum5;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType6)&&(0!=pInfo->m_nNum6)  )
// 	{
// 		Info.m_Attachment[5].m_dwType = pInfo->m_nType6;
// 		Info.m_Attachment[5].m_dwNum = pInfo->m_nNum6;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType7)&&(0!=pInfo->m_nNum7)  )
// 	{
// 		Info.m_Attachment[6].m_dwType = pInfo->m_nType7;
// 		Info.m_Attachment[6].m_dwNum = pInfo->m_nNum7;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType8)&&(0!=pInfo->m_nNum8)  )
// 	{
// 		Info.m_Attachment[7].m_dwType = pInfo->m_nType8;
// 		Info.m_Attachment[7].m_dwNum = pInfo->m_nNum8;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType9)&&(0!=pInfo->m_nNum9)  )
// 	{
// 		Info.m_Attachment[8].m_dwType = pInfo->m_nType9;
// 		Info.m_Attachment[8].m_dwNum = pInfo->m_nNum9;
// 		Info.m_IsHaveAttachment = 1;
// 	}
// 	if ((0!= pInfo->m_nType10)&&(0!=pInfo->m_nNum10)  )
// 	{
// 		Info.m_Attachment[9].m_dwType = pInfo->m_nType10;
// 		Info.m_Attachment[9].m_dwNum = pInfo->m_nNum10;
// 		Info.m_IsHaveAttachment = 1;
// 	}

}
void	EmailSystem::InsertEmailSyn(MAP_QUERY_INDEX nIdx,EmailInfo& Info,char* pInfo)
{
	if (NULL == pInfo )
	{
		return;
	}
	InsertEmailQuery* pQuery = InsertEmailQuery::ALLOC();
	pQuery->SetIndex(nIdx);
	pQuery->SetThreadIndex(0);
	pQuery->SetEmailInfo(Info);
	//pQuery->SetCharQuery(pInfo);	
	g_MapServer.MapDBQuery(pQuery);
}
void	EmailSystem::DeleteEmailByConditionSyn(DWORD dwCurrTime)
{
	DeleteEmailByConditionQuery* pQuery = DeleteEmailByConditionQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_DELETETIMEOUT_EMAIL);
	pQuery->SetThreadIndex(0);
	pQuery->SetCurrTime(dwCurrTime);
	g_MapServer.MapDBQuery(pQuery);
}

void	EmailSystem::SendEmail2Player(DWORD dwID,char* pPlayerName)
{
// 	if(true)
// 	{
// 		// Zhangy TODO GM操作的，暂时没用，注释掉
// 		return;
// 	}
// 	if (NULL == pPlayerName)
// 	{
// 		return;
// 	}
// 	EmailEntry*      pEmailEntry = EmailSystem::Instance()->GetEmailEntry(dwID);
// 	if (!pEmailEntry )
// 	{
// 		return ;
// 	}
// 	EmailInfo Info;
// 	Info.m_byAttachmentCount = 0;
// 	memset(&Info.m_Attachment,0,sizeof(Info.m_Attachment));
// 	EmailSystem::Instance()->GetEmailAttachmentInfo(pEmailEntry,Info);
// 	Info.m_bIsRead = 0;
// 	Info.m_dwInsertTime =(DWORD)TimeoutManager::Instance()->GetCurrentTime();
// 	Info.m_dwID = Info.m_dwInsertTime+(DWORD)Rand::Instance()->irand(2,1000000);;
// 	strncpy(Info.m_szTitle,pEmailEntry->m_szTitle,MAX_EMAIL_TITLE_LEN);
// 	strncpy(Info.m_Word,pEmailEntry->m_szWord,MAX_EMAIL_WORD);
// 	Info.m_IsReword=0;
// 	if (Info.m_byAttachmentCount)
// 	{
// 		Info.m_dwDeleteTime = Info.m_dwInsertTime + 15*SECOND_OF_DAY; 
// 	}
// 	else
// 	{
// 		Info.m_dwDeleteTime = Info.m_dwInsertTime + 3*SECOND_OF_DAY; 
// 
// 	}
// 
// 	//向数据库发送请求
// 	EmailSystem::Instance()->InsertEmailSyn(MAP_QUERY_INSERT_EMAIL_PLAYER,Info,pPlayerName);
}
