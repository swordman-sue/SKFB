#include "stdafx.h"
#include "PlayerManager.h"
#include "Player.h"
#include "ObjectFactory.h"
#include "AccountInfo.h"
#include "EmailSystem.h"
#include "MapSvrQuery.h"
#include "TimeoutManager.h"
#include "../Common/DBCDefine.h"
#include "../Common/DataDefine.h"
#include "../ToolKit/Rand.h"
#include "../Common/CommonLogic.h"
#include "Friend/FriendManager.h"

PlayerManager::PlayerManager()
{

}

PlayerManager::~PlayerManager()
{

}

BOOL PlayerManager::Init()
{
	m_PlayerCharIDMap.clear();
	m_PlayerGuidMap.clear();
	m_PlayerNameMap.clear();

	return TRUE;
}

VOID PlayerManager::Release()
{
	Player* pPlayer = NULL;
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if(pPlayer)
		{
			ObjectFactory::Instance()->FreePlayer(pPlayer);
		}
	}

	m_PlayerCharIDMap.clear();
	m_PlayerGuidMap.clear();
	m_PlayerNameMap.clear();
}

VOID PlayerManager::Update(DWORD dwTick)
{
	Player* pPlayer = NULL;
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if(pPlayer)
		{
			pPlayer->Update(dwTick);
		}
	}
}

//加入一个玩家到管理器
void PlayerManager::AddPlayer(Player *pPlayer)
{
	if ( pPlayer == NULL )
	{
		return;
	}

	PlayerMapIter iter = m_PlayerCharIDMap.find(pPlayer->GetCharID());
	if (iter != m_PlayerCharIDMap.end())
	{
		//写日志 玩家数据异常
		MAPLOG(ERROR_LEVEL,"PlayerManager的CharIDMap中存在相同账号的玩家CharID=%d", pPlayer->GetCharID());
		return;
	}

	m_PlayerCharIDMap.insert(std::make_pair(pPlayer->GetCharID(), pPlayer));
	m_PlayerGuidMap.insert(std::make_pair(pPlayer->GetGuid(), pPlayer));

	// 更新在线人数
	map<DWORD, DWORD>::iterator iter1 = m_ServerRoleNumList.find(pPlayer->GetServerId());
	if (iter1 != m_ServerRoleNumList.end())
		iter1->second++;
	else
		m_ServerRoleNumList.insert(make_pair(pPlayer->GetServerId(), 1));
}

//加入一个玩家到管理器
VOID PlayerManager::AddPlayerName(Player* pPlayer)
{
	if ( pPlayer == NULL )
	{
		return;
	}

	PlayerNameMapIter iter = m_PlayerNameMap.find(pPlayer->GetRoleName());
	if (iter != m_PlayerNameMap.end())
	{
		//写日志 玩家数据异常
		MAPLOG(ERROR_LEVEL,"PlayerManager的NameMap中存在相同账号的玩家CharID=%d", pPlayer->GetCharID());
		return;
	}

	m_PlayerNameMap.insert(std::make_pair(pPlayer->GetRoleName(), pPlayer));
}

//从管理器中删除一个玩家
VOID PlayerManager::RemovePlayer(Player* pPlayer)
{
	if (pPlayer == NULL)
	{
		return;
	}

	m_PlayerCharIDMap.erase(pPlayer->GetObjectKey());
	m_PlayerGuidMap.erase(pPlayer->GetGuid());
	m_PlayerNameMap.erase(pPlayer->GetRoleName());

	// 更新在线人数
	map<DWORD, DWORD>::iterator iter1 = m_ServerRoleNumList.find(pPlayer->GetServerId());
	if (iter1 != m_ServerRoleNumList.end() && iter1->second > 0)
		iter1->second--;

	ObjectFactory::Instance()->FreePlayer(pPlayer);
}


VOID PlayerManager::RoleNameModify(Player* pPlayer, const char* pszOldName, const char* pszNewName)
{
	PlayerNameMapIter iter = m_PlayerNameMap.find(pszOldName);
	if (iter != m_PlayerNameMap.end())
	{
		m_PlayerNameMap.erase(iter);
	}

	pPlayer->SetRoleName(pszNewName);
	
	m_PlayerNameMap.insert(std::make_pair(pszNewName, pPlayer));
}

Player*	PlayerManager::FindPlayerByRoleId(DWORD dwRoleId) const
{
	PlayerMapIter iter = m_PlayerCharIDMap.find(dwRoleId);
	return iter != m_PlayerCharIDMap.end() ? iter->second : NULL;
}

//通过CharID进行查找
Player* PlayerManager::FindPlayerByCharID(DWORD dwCharID) const
{
	PlayerMapIter iter = m_PlayerCharIDMap.find(dwCharID);
	if (iter == m_PlayerCharIDMap.end())
	{
		return NULL;
	}

	return iter->second;
}

void PlayerManager::SendEmilByCondition(DWORD dwCondition,DWORD dwID)
{
// 	if(TRUE)
// 	{
// 		// zhangy TODO 都是GM在周期性调用，并且服务器也会定期调用，都是关于7/30天理财计划
// 		return;
// 	}
// 
// 	bool b7DaysFlag = false;
// 	bool b30DaysFlag = false;
// 
// 	SendEmailEntry * pSendEntry   = EmailSystem::Instance()->GetSendEmailEntry(dwCondition);
// 	EmailEntry*      pEmailEntry = EmailSystem::Instance()->GetEmailEntry(dwID);
// 	if (!pSendEntry || !pEmailEntry )
// 	{
// 		return ;
// 	}
// 	EmailInfo Info;
// 	Info.m_byAttachmentCount = 0;
// 	memset(&Info.m_Attachment,0,sizeof(Info.m_Attachment));
// 	EmailSystem::Instance()->GetEmailAttachmentInfo(pEmailEntry,Info);
// 	Info.m_bIsRead = 0;
// 	Info.m_dwInsertTime =  (DWORD)TimeoutManager::Instance()->GetCurrentTime();
// 	Info.m_dwID = Info.m_dwInsertTime +(DWORD)Rand::Instance()->irand(2,1000000);
// 	strncpy(Info.m_szTitle,pEmailEntry->m_szTitle,MAX_EMAIL_TITLE_LEN);
// 	strncpy(Info.m_Word,pEmailEntry->m_szWord,MAX_EMAIL_WORD);
// 	
// 	Info.m_IsReword=0;
// 	if (Info.m_byAttachmentCount)
// 	{
// 		Info.m_dwDeleteTime = Info.m_dwInsertTime + 15*SECOND_OF_DAY; 
// 	}
// 	else
// 	{
// 		Info.m_dwDeleteTime = Info.m_dwInsertTime + 3*SECOND_OF_DAY;
// 	}
// 	if (-1 != pSendEntry->m_nAddRMoney7Day)
// 	{
// 		b7DaysFlag = true;
// 	}
// 	if (-1 != pSendEntry->m_nAddRMoney30Day)
// 	{
// 		b30DaysFlag = true;
// 	}
// 
// 
// 	PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 	for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 	{
// 		if (-1 != pSendEntry->m_nGender )
// 		{
// 			if (iter->second->GetGender() != pSendEntry->m_nGender )
// 			{
// 				continue;
// 			}
// 		}
// 		if (-1 != pSendEntry->m_nRace )
// 		{
// 			if (iter->second->GetRace() != pSendEntry->m_nRace )
// 			{
// 				continue;
// 			}
// 		}
// 		if (-1 != pSendEntry->m_nVocation )
// 		{
// 			if (iter->second->GetVocation() != pSendEntry->m_nVocation )
// 			{
// 				continue;
// 			}
// 		}
// 		if (-1 != pSendEntry->m_nLv)
// 		{
// 			if (iter->second->GetLevel()<pSendEntry->m_nLv)
// 			{
// 				continue;
// 			}
// 		}
// // 		if (-1 != pSendEntry->m_nAddRMoney7Day)
// // 		{
// // 			if (iter->second->Get7DaysAddRmoney()<=pSendEntry->m_nAddRMoney7Day)
// // 			{
// // 				continue;
// // 			}
// // 			else
// // 			{
// // 				iter->second->Dec7DaysAddRmoney();
// // 				
// // 			}
// // 		}
// // 		if (-1 != pSendEntry->m_nAddRMoney30Day)
// // 		{
// // 			if (iter->second->Get30DaysAddRmoney()<=pSendEntry->m_nAddRMoney30Day)
// // 			{
// // 				continue;
// // 			}
// // 			else
// // 			{
// // 				iter->second->Dec30DaysAddRmoney();
// // 			}
// // 		}
// 		iter->second->GetEmailManager().AddEmail(Info,true);
// 	}
// 	//向数据库发送请求
// 	EmailSystem::Instance()->InsertEmailSyn(MAP_QUERY_INSERT_EMAIL,Info,pSendEntry->m_szSqlQuery,b7DaysFlag,b30DaysFlag);
// 	return ;
}

//通过Guid进行查找
Player* PlayerManager::FindPlayerByGuid(DWORD dwGuid) const
{
	PlayerMapIter iter = m_PlayerGuidMap.find(dwGuid);
	if (iter == m_PlayerGuidMap.end())
	{
		return NULL;
	}

	return iter->second;
}

//通过Name进行查找
Player* PlayerManager::FindPlayerByCharName(const char* pCharName) const
{
	PlayerNameMapIter iter = m_PlayerNameMap.find(pCharName);
	if (iter == m_PlayerNameMap.end())
	{
		return NULL;
	}

	return iter->second;
}

void PlayerManager::GetAllPlayer(Player** pPlayer, int& count, int nMaxcount, DWORD dwSeverId)
{
	PlayerMap::iterator it;

	count=0;
	for (it = m_PlayerCharIDMap.begin();it != m_PlayerCharIDMap.end();++ it)
	{
		if (it->second != NULL && count < nMaxcount && (!dwSeverId || it->second->GetServerId() == dwSeverId))
		{
			pPlayer[count]=it->second;
			count++;
		}
	}
}

void PlayerManager::GetAllPlayer(vector<Player*> &vPlayerList, DWORD dwSeverId)
{
	PlayerMap::iterator it;
	for (it = m_PlayerCharIDMap.begin(); it != m_PlayerCharIDMap.end(); ++it)
	{
		if (it->second != NULL && (!dwSeverId || it->second->GetServerId() == dwSeverId))
		{
			vPlayerList.push_back(it->second);
		}
	}
}

//获得第一个玩家
Player* PlayerManager::GetFirstPlayer() const
{
	PlayerMapIter iter = m_PlayerCharIDMap.begin();
	if (iter == m_PlayerCharIDMap.end())
	{
		return NULL;
	}

	return iter->second;
}

void PlayerManager::UpdateSys(SystemIDs id,ActivityOpe Ope)
{
	switch(Ope)
	{
	case ActivityOpe_Begin:
		{

		}
		break;
	case ActivityOpe_End:
		{

		}
		break;
	case ActivityOpe_Update:
		{
			switch(id)
			{
			case SystemIDs_DeleteEmail:
				{
// 					DWORD dwCurrTime = (DWORD)TimeoutManager::Instance()->GetCurrentTime();
// 	
// 					//首先删除数据库的数据
// 					EmailSystem::Instance()->DeleteEmailByConditionSyn(dwCurrTime);
// 					//删除在线人员的过期邮件
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->GetEmailManager()->DeleteTimeOutEmail();
// 					}
				}
				break;
			case SystemIDs_ClearMission:
				{
// 					ClearMissionQuery* pQuery = ClearMissionQuery::ALLOC();
// 					pQuery->SetIndex(MAP_QUERY_CLEAR_MISSION);
// 					pQuery->SetThreadIndex(0);
// 					g_MapServer.MapDBQuery(pQuery);
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->ClearMission();
// 					}
				}
				break;
			case SystemIDs_ClearLuckyTreeNum:
				{
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->SetLuckyTreeNum(0);
// 					}
				}
				break;
			case SystemIDs_RandAcceptMission:
				{
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->RandAcceptMission();
// 					}
				}
				break;
			case SystemIDs_Add7DaysRMoney:
				{
					SendEmilByCondition(7,10);
				}
				break;
			case SystemIDs_Add30DaysRMoney:
				{
					SendEmilByCondition(8,11);
				}
				break;
			case SystemIDs_LadderAward:
				{
// 					LadderAwardQuery* pQuery = LadderAwardQuery::ALLOC();
// 					pQuery->SetIndex(MAP_QUERY_LADDER_AWARD);
// 					pQuery->SetThreadIndex(0);
// 					g_MapServer.MapDBQuery(pQuery);
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->SetLadderAward(0);
// 						
// 						g_MapServer.MapDBQuery(pQuery);
// 					}

				}
				break;
			case SystemIDs_QQVip:
				{
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->SendBlueDiamonData();
// 						iter->second->SendBlueDiamonAwardData();
// 						iter->second->SendYellowDiamonData();
// 						iter->second->SendYellowDiamonAwardData();
// 					}
				}
				break;
			case SystemIDs_OnlineTimeReset:
				{
// 					OnlineTimeResetQuery* pQuery = OnlineTimeResetQuery::ALLOC();
// 					pQuery->SetIndex(MAP_QUERY_ONLINETIME_RESET);
// 					pQuery->SetThreadIndex(0);
// 					g_MapServer.MapDBQuery(pQuery);
// 					PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 					for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 					{
// 						iter->second->SetOnlineTime(0);
// 						iter->second->SetOnlineAwardLev(0);
// 						iter->second->InitOnlineTime();
// 						iter->second->SendBaseOnlineInfo(false,false);
// 					}

				}
				break;
			}

		}
		break;
	}
}

void PlayerManager::OnNewDay()
{
	Player* pPlayer = NULL;
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if(pPlayer)
		{
			pPlayer->OnNewDay();
		}
	}
}

void PlayerManager::OnNewWeek()
{
	Player* pPlayer = NULL;
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if (pPlayer)
		{
			pPlayer->OnNewWeek();
		}
	}
}

void PlayerManager::OnNewMonth()
{
	Player* pPlayer = NULL;
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if (pPlayer)
		{
			pPlayer->OnNewMonth();
		}
	}
}

void PlayerManager::SendPacketAround(NetMsgBody * pMsg, DWORD dwServerId, DWORD dwRoleId)
{
	Player* pPlayer;
	std::map<DWORD, Player*>::const_iterator iter;
	for (iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if (!pPlayer)
			continue;

		// 指定用户除外
		if (pPlayer->GetRoleId() == dwRoleId)
			continue;

		// 不是同一个服的
		if (dwServerId && pPlayer->GetServerId() != dwServerId)
			continue;

		// 在黑名单
// 		if (dwRoleId && pPlayer->GetFriendManager()->IsBlack(dwRoleId))
// 			continue;

		pPlayer->SendPacket(pMsg);
	}
}

DWORD PlayerManager::GetPlayerNum(DWORD dwServerId)
{
	map<DWORD, DWORD>::iterator iter = m_ServerRoleNumList.find(dwServerId);
	return iter != m_ServerRoleNumList.end() ? iter->second : 0;
}

void PlayerManager::SendWorldChat(NetMsgBody * pMsg, DWORD dwServerId, DWORD dwRoleId)
{
	Player* pPlayer;
	std::map<DWORD, Player*>::const_iterator iter;
	for (iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if (!pPlayer)
			continue;

		// 指定用户除外
// 		if (pPlayer->GetRoleId() == dwRoleId)
// 			continue;

		// 不是同一个服的
		if (dwServerId && pPlayer->GetServerId() != dwServerId)
			continue;

		// 在黑名单
		if (dwRoleId && pPlayer->GetFriendManager()->IsBlack(dwRoleId))
			continue;

		pPlayer->SendPacket(pMsg);
	}
}

void PlayerManager::CheckTimeOutRestrictions()
{
// 	PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 	for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 	{
// 		if (NULL != iter->second)
// 		{
// 			iter->second->CheckTimeOutRestrictions();
// 		}
// 	}

}

void PlayerManager::SendAllRestrictions()
{
// 	PlayerMapIter iter = m_PlayerCharIDMap.begin();
// 	for (iter;iter!=m_PlayerCharIDMap.end();iter++)
// 	{
// 		if (NULL != iter->second)
// 		{
// 			iter->second->SendAllRestrictions();
// 		}
// 	}

}
VOID PlayerManager::ProcessReset()
{
	Player* pPlayer;
	PlayerMapIter iter;
	for (iter = m_PlayerGuidMap.begin(); iter != m_PlayerGuidMap.end(); ++iter)
	{
		pPlayer = iter->second;
		if (!pPlayer)
			continue;

		AccountInfo* pAccInfo = pPlayer->GetAccountInfo();
		if(!pAccInfo)
			continue;

		if(pAccInfo->GetUserState() == USER_AT_GAME_STATE )
		{
// 			//重置体力
// 			pPlayer->ResetStamina();
// 
// 			//重置付费产出次数
// 			pPlayer->ResetPayOutputNum();
		}
	}
}

VOID PlayerManager::ProcessEventBroadcast( EventBroadcastType enBroadcastType, BroadcastInfo& tBroadcastInfo )
{

// 	if (enBroadcastType <= EBT_NONE || enBroadcastType >= EBT_MAX)
// 		return;
// 
// 	MSG_M2C_CHAR_EVENT_BROADCAST_CMD msg;
// 	msg.m_byBroadcastType = enBroadcastType;
// 	switch(enBroadcastType)
// 	{
// 		case EBT_OPENCARDPACKET://			= 1,	//抽卡
// 		case EBT_7DAY_OPENCARD://			= 3,	//开服7天抽卡系统
// 		case EBT_7DAY_EXCHANGE://			= 4,	//开服7天兑换
// 		{
// 			strncpy(msg.m_tBroadcastPara.m_szParaText[0], tBroadcastInfo.m_CardInfo.szPlayerName, MAX_ROLE_LEN);
// 			sprintf(msg.m_tBroadcastPara.m_szParaText[1], "%u", tBroadcastInfo.m_CardInfo.dwCardID);
// 			msg.m_tBroadcastPara.m_byParaNum = 2;
// 
// 		}
// 		break;
// 		case EBT_FINANCIAL_PLAN://			= 2,	//理财计划
// 		case EBT_BUY_GROUPCARD:
// 		{
// 			strncpy(msg.m_tBroadcastPara.m_szParaText[0], tBroadcastInfo.m_Financial_Plan.szPlayerName, MAX_ROLE_LEN);
// 			sprintf(msg.m_tBroadcastPara.m_szParaText[1], "%u", tBroadcastInfo.m_Financial_Plan.byBuyType);
// 			msg.m_tBroadcastPara.m_byParaNum = 2;
// 
// 		}
// 		break;
// 		default:
// 		{
// 			return;
// 		}
// 
// 	}
// 	SendPacketAround(&msg);
	
}

VOID PlayerManager::ShowPlayerList()
{
	char szCharName[MAX_ROLE_LEN+1];
	Player *pPlayer;
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); iter++)
	{
		pPlayer = iter->second;
		ChangeLocalChar(pPlayer->GetRoleName(), szCharName, sizeof(szCharName));
		MAPLOG(GUI_LEVEL,"[RoleId:%d][RoleName:%s]",pPlayer->GetCharID(), szCharName);
	}
}

void PlayerManager::SaveAllDataToDB()
{
	for (PlayerMapIter iter = m_PlayerCharIDMap.begin(); iter != m_PlayerCharIDMap.end(); iter++)
	{
		Player *pPlayer = iter->second;
		if (pPlayer)
			pPlayer->OnSave(SAVE_DB_TYPE_TIMING);
	}
}
