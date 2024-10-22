#include "StdAfx.h"
#include "DBThread.h"
#include "SqlDatabase.h"
#include "QueryResult.h"
#include "DBThreadHandler.h"

DBThread::DBThread(void)
:	m_pDatabase(NULL),
m_fnErrorMessage(NULL),
m_fnQueryResult(NULL),
m_bEndProcess(FALSE),
m_pQueryCS(NULL),
m_pQueryResultCS(NULL),
m_pQueryList(NULL),
m_pQueryResultList(NULL)
{
}

DBThread::~DBThread(void)
{
}

void DBThread::EndThread()
{

}

BOOL DBThread::Initialize( DatabaseDesc & desc )
{
	if (!desc.pszUserName || !desc.pszUserPassword || !desc.pszDatabaseName || !desc.pszDatabaseIP || !desc.nPort)
		return FALSE;

	m_fnErrorMessage		= desc.fnErrorMessage;
	m_fnQueryResult			= desc.fnQueryResult;

	m_pDatabase = new SqlDatabase;
	
	if (!m_pDatabase->Connect(desc.pszDatabaseName, desc.pszDatabaseIP, desc.pszUserName, desc.pszUserPassword, desc.nPort))
		return FALSE;

	//缓冲区互斥变量
	m_pQueryCS				= new CriticalSection;
	m_pQueryResultCS		= new CriticalSection;

	m_pQueryList			= new CLinkedlist<QueryResult *>(3000);
	m_pQueryResultList  = new CLinkedlist<QueryResult *>(3000);

	m_bEndProcess = FALSE;

	//开始线程处理函数
	StartThread();

	return TRUE;
}

//该函数由上层逻辑线程调用，取出查询结果，调用回调函数处理
VOID DBThread::Update()
{
	if( !m_pQueryResultList ||  m_pQueryResultList->GetCount() == 0 )
		return ;

	//WORD wLoopTimes = 0;
	BOOL bInfiniteLoop = TRUE;
	while( bInfiniteLoop )
	{
		m_pQueryResultCS->Lock();
		QueryResult* pQResult = m_pQueryResultList->DeleteHead();
		m_pQueryResultCS->Unlock();

		if (pQResult==NULL)
			break;

		m_fnQueryResult(pQResult->GetIndex(), pQResult);

		// todo:: add by Kevin 防止长期处理数据库回调,冷落主线程其他逻辑
// 		if (++wLoopTimes >= 100)
// 			break;
	}
}

void DBThread::Release()
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
	delete m_pQueryCS;
	delete m_pQueryResultCS;
	
	QueryResult* pQueryResult = NULL;
	//清除缓冲区列表
	if (m_pQueryList)
	{
		while((pQueryResult = m_pQueryList->DeleteHead())!=NULL)
		{
			pQueryResult->Release();
		}

		delete m_pQueryList;
	}

	if (m_pQueryResultList)
	{
		while((pQueryResult = m_pQueryResultList->DeleteHead())!=NULL)
		{
			pQueryResult->Release();
		}

		delete m_pQueryResultList;
	}

	//printf("DBThread::Release() \n");
}

//该函数由上层逻辑线程调用,把需要处理的查询放到列表中
BOOL DBThread::DBQuery( QueryResult * pQuery )
{
	if(!pQuery)
		return FALSE;

	if( m_bEndProcess )
		return FALSE;

	// 加入查询缓冲区
	m_pQueryCS->Lock();
	m_pQueryList->AddTail( pQuery );
	m_pQueryCS->Unlock();

	return TRUE;
}

//线程处理函数，负责对查询列表中的查询进行处理
DWORD DBThread::run()
{

	m_pDatabase->ThreadStart();

	QueryResult * pQuery = NULL;

	while(!m_bEndProcess)
	{
		BOOL bQueryInfiniteLoop = TRUE;
		while(bQueryInfiniteLoop)
		{
			if(m_pQueryList->GetCount() == 0)	
				break;

			m_pQueryCS->Lock();
			pQuery = m_pQueryList->DeleteHead();
			m_pQueryCS->Unlock();

			if( !pQuery )
				break;

			//执行查询
			fnProcessQuery pfnHandler =  Find(pQuery->GetIndex());

			UINT64 lCurTickCount = GetTickCount64();

			if(pfnHandler)
				pfnHandler(m_pDatabase,pQuery);
			else{
				//写日志
				if(m_fnErrorMessage)
					m_fnErrorMessage("Error,Can not find Database Process handler");
			}

			DWORD dwCostTime = GetTickCount64() - lCurTickCount;
			if (dwCostTime >= 100)
			{
				MAPLOG(ERROR_LEVEL, "[query_idx:%d]process db operation cost time[%d]ms", pQuery->GetIndex(), dwCostTime); 
			}

			//把查询结果放到结果缓冲区中
			m_pQueryResultCS->Lock();
			m_pQueryResultList->AddTail( pQuery );
			m_pQueryResultCS->Unlock();
		}
			

		Sleep(1);
	}
	
	//主线程退出处理完所有未完成的DB操作
	while (1)
	{
		m_pQueryCS->Lock();
		pQuery = m_pQueryList->DeleteHead();
		m_pQueryCS->Unlock();

		if( !pQuery )
			break;

		//执行查询
		fnProcessQuery pfnHandler =  Find(pQuery->GetIndex());

		if(pfnHandler)
			pfnHandler(m_pDatabase,pQuery);
		else
		{
			//写日志
			if(m_fnErrorMessage)
			{
				m_fnErrorMessage("Error,Can not find Database Process handler");
			}
		}

		//把查询结果放到结果缓冲区中
		m_pQueryResultCS->Lock();
		m_pQueryResultList->AddTail( pQuery );
		m_pQueryResultCS->Unlock();
	}

	//退出线程函数
	m_pDatabase->ThreadEnd();
	return 0;
}


fnProcessQuery DBThread::Find(DWORD dwIndex)
{
	return DBThreadManager::Instance()->Find(dwIndex);
}

DBThreadManager::DBThreadManager()
:	m_pDBThread(NULL),
m_dwDBThreadNum(0)
{

}

DBThreadManager::~DBThreadManager()
{

}

BOOL DBThreadManager::Initialize( DatabaseDesc & desc,DWORD dwDBThreadNum )
{
	Register();

	m_dwDBThreadNum = dwDBThreadNum;
	m_pDBThread = new DBThread[m_dwDBThreadNum];
	for (DWORD i = 0;i<m_dwDBThreadNum;i++)
	{
		if(!m_pDBThread[i].Initialize(desc))
			return FALSE;
	}

	return TRUE;
}

void DBThreadManager::Update()
{
	for (DWORD i = 0;i<m_dwDBThreadNum;i++)
	{
		m_pDBThread[i].Update();
	}
}

void DBThreadManager::Release()
{
	for (DWORD i = 0;i<m_dwDBThreadNum;i++)
	{
		m_pDBThread[i].Release();
	}

	delete[] m_pDBThread;
}

void DBThreadManager::Register()
{
	//在这里添加消息处理函数
	AddHandler(MAP_QUERY_CHECK_ACCOUNT, DBThreadHandler::DoCheckAccountQuery);
	AddHandler(DB_SDK_ACCOUNT_CHECK, DBThreadHandler::DoSDKAccountCheck);
	AddHandler(MAP_QUERY_CREATE_ACCOUNT, DBThreadHandler::DoCreateAccountQuery);
	AddHandler(DB_SDK_CREATE_ACCOUNT, DBThreadHandler::DoSDKCreateAccount);	
	AddHandler(MAP_QUERY_CHAR_LIST, DBThreadHandler::DoSelectZoneRoleQuery);
	AddHandler(MAP_QUERY_CREATE_CHAR, DBThreadHandler::DoCreateRoleQuery);
	AddHandler(MAP_QUERY_CHAR_INFO, DBThreadHandler::DoSelectRoleInfoQuery);
	AddHandler(MAP_QUERY_UPDATE_CHAR, DBThreadHandler::DoUpdateRoleInfoQuery);
	AddHandler(MAP_UPDATE_LAST_LOGIN_SERVER, DBThreadHandler::DoUpateLastLoginServer);
	AddHandler(MAP_QUERY_CHANGE_PASSWORD, DBThreadHandler::ProcessAccountPasswordModify);
	AddHandler(MAP_QUERY_CHANGE_MOBILE, DBThreadHandler::ProcessAccountMobileChange);
	
	


	AddHandler(MAP_SERVER_GOLBALDATA_SAVE,			DBThreadHandler::SaveGolbalData);
	AddHandler(MAP_SERVER_GOLBALDATA_LOAD,			DBThreadHandler::LoadGolbalData);

	AddHandler(DB_ROLE_BASE_DATA_LIST_LOAD,			DBThreadHandler::LoadRoleBaseDataList);

	AddHandler(DB_FRIEND_DATA_UPDATE,				DBThreadHandler::UpdateFriendData);

	AddHandler(DB_ARENA_DATA_SAVE,					DBThreadHandler::SaveArenaData);
	AddHandler(DB_ARENA_DATA_LOAD,					DBThreadHandler::LoadArenaData);

	AddHandler(DB_ARENA_BATTLE_RECORD_SAVE,			DBThreadHandler::SaveArenaBattleRecordData);
	AddHandler(DB_ARENA_BATTLE_RECORD_LOAD,			DBThreadHandler::LoadArenaBattleRecordData);

	AddHandler(DB_ROLE_RANK_DATA_SAVE,				DBThreadHandler::SaveRoleRankData);
	AddHandler(DB_ROLE_RANK_DATA_LOAD,				DBThreadHandler::LoadRoleRankData);

	AddHandler(MAP_QUERY_INSERT_EMAIL,				DBThreadHandler::SaveEmailData);

	AddHandler(DB_GM_EVENT_DATA_LOAD,				DBThreadHandler::LoadAndDeleteGMEvent);

	AddHandler(DB_SERVER_LIST_LOAD,					DBThreadHandler::LoadServerList);

	AddHandler(DB_ROLE_CURRENCY_LOGS_SAVE,			DBThreadHandler::SaveRoleCurencyLogs);

	AddHandler(DB_LAND_DATA_SAVE,					DBThreadHandler::SaveLandData);

	AddHandler(DB_LAND_DATA_LOAD,					DBThreadHandler::LoadLandData);

	AddHandler(DB_ARENA_RANK_VIEW_LIST_UPDATE,		DBThreadHandler::UpdateArenaRankViewList);

	AddHandler(DB_LEAVE_MESSAGE_SAVE,				DBThreadHandler::SaveLeaveMessage);

	AddHandler(DB_REWARD_CENTER_DATA_SAVE,			DBThreadHandler::SaveRewardCenterData);

	AddHandler(DB_ROLE_TEMP_DATA_UPDATE,			DBThreadHandler::UpdateRoleTempData);

	AddHandler(DB_ROLE_OFFLINE_RECORD_SAVE,			DBThreadHandler::SaveRoleOfflineRecord);
	AddHandler(DB_ROLE_LEVEL_RECORD_SAVE,			DBThreadHandler::SaveRoleLevelRecord);

	AddHandler(DB_ROLE_RECHARGE_RECORD_SAVE,		DBThreadHandler::SaveRoleRechargeRecord);

	AddHandler(DB_ROLE_MAP_LOSE_RECORD_SAVE,		DBThreadHandler::SaveRoleMapLoseRecord);

	AddHandler(DB_ROLE_FIRST_PASS_MAP_RECORD_SAVE, DBThreadHandler::SaveRoleFirstPassMapRecord);

	AddHandler(DB_SHOP_BUY_RECORD_SAVE,				DBThreadHandler::SaveShopBuyRecord);
	

	AddHandler(DB_GUILD_DATA_SAVE,					DBThreadHandler::SaveGuildData);
	AddHandler(DB_GUILD_DATA_LOAD,					DBThreadHandler::LoadGuildData);

	AddHandler(DB_CHAT_RECORD_SAVE,					DBThreadHandler::SaveChatRecord);
	AddHandler(DB_CHAT_RECORD_LOAD,					DBThreadHandler::LoadChatRecord);

	AddHandler(DB_LOG_SAVE,							DBThreadHandler::SaveLogs);

	AddHandler(DB_MAP_COMMENT_DATA_SAVE,			DBThreadHandler::SaveMapComment);
	AddHandler(DB_MAP_COMMENT_DATA_LOAD,			DBThreadHandler::LoadMapComment);

	AddHandler(DB_ROLE_ITEM_LOSE_RECORD_SAVE,		DBThreadHandler::SaveItemLoseRecord);

	AddHandler(DB_ROLE_ITEM_OPERATION_RECORD_SAVE, DBThreadHandler::SaveItemOperationRecord);

	AddHandler(DB_ROLE_ACCOUNT_LOCK_UPDATE,			DBThreadHandler::UpdateAccountLock);

	AddHandler(DB_KING_MATCH_DATA_SAVE,				DBThreadHandler::SaveKingMatchData);
	AddHandler(DB_KING_MATCH_DATA_LOAD,				DBThreadHandler::LoadKingMatchData);

	AddHandler(DB_SCORE_MATCH_DATA_SAVE,			DBThreadHandler::SaveScoreMatchData);
	AddHandler(DB_SCORE_MATCH_DATA_LOAD,			DBThreadHandler::LoadScoreMatchData);

	AddHandler(DB_TEAM_DUNGEON_RANK_DATA_SAVE,		DBThreadHandler::SaveTeamDungeonRankData);
	AddHandler(DB_TEAM_DUNGEON_RANK_DATA_LOAD,		DBThreadHandler::LoadTeamDungeonRankData);

	AddHandler(DB_OFFLINE_PLAYER_NUM_RECORD_SAVE,	DBThreadHandler::SaveOfflinePlayerNumRecord);
	
	AddHandler(DB_TURNTABLE_DATA_SAVE,				DBThreadHandler::SaveTurntableData);
	AddHandler(DB_TURNTABLE_DATA_LOAD,				DBThreadHandler::LoadTurntableData);

	AddHandler(DB_NEW_ROLE_GUIDE_UPDATE,			DBThreadHandler::UpdateNewRoleGuide);

	AddHandler(DB_RESOURCE_WAR_DATA_SAVE,			DBThreadHandler::SaveResourceWarData);
	AddHandler(DB_RESOURCE_WAR_DATA_LOAD,			DBThreadHandler::LoadResourceWarData);

	AddHandler(DB_GUILD_WAR_DATA_SAVE,				DBThreadHandler::SaveGuildWarData);
	AddHandler(DB_GUILD_WAR_DATA_LOAD,				DBThreadHandler::LoadGuildWarData);

	AddHandler(DB_ACTIVATION_CODE_SELECT,			DBThreadHandler::SelectActivityCode);
	AddHandler(DB_ACTIVATION_CODE_UPDATE,			DBThreadHandler::UpdateActivityCode);

	AddHandler(DB_GAME_STATISTICS_EVENT,			DBThreadHandler::GameStatisticsEvent);

	AddHandler(DB_GM_OPERATION_RECORD_SAVE,			DBThreadHandler::GMOperationRecordSave);

}

void DBThreadManager::AddHandler(DWORD dwIndex, fnProcessQuery fnHandler)
{
	m_ProcessQueryMap.insert(std::make_pair(dwIndex,fnHandler));
}

fnProcessQuery DBThreadManager::Find(DWORD dwIndex)
{
	std::map<DWORD,fnProcessQuery>::iterator it = m_ProcessQueryMap.find(dwIndex);
	if(it==m_ProcessQueryMap.end())
		return NULL;

	return it->second;
}

BOOL DBThreadManager::DBQuery( DWORD dwDBThreadIndex,QueryResult * pQuery )
{
	if(dwDBThreadIndex>=m_dwDBThreadNum)
		return FALSE;

	return m_pDBThread[dwDBThreadIndex].DBQuery(pQuery);
}

