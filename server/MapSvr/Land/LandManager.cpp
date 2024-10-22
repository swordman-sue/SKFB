/**\file
��ع�����
\author Kevin
\date 2016/9/24
\note ���ʱ��ȫ��������һ��Ҫ����Ѳ����ʱ��
*/

#include "stdafx.h"
#include "LandManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Hero\HeroManager.h"
#include "DBCDefine.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "..\Fuben\FubenManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"

CLandManager::CLandManager()
{
	m_lProccessTime = GetTickCount64();
	m_PlayerLandList.clear();
}

CLandManager::~CLandManager()
{

}

void CLandManager::Init()
{
	m_PlayerLandList.clear();
}

// �洢��������
void CLandManager::SaveAllDataToDB()
{
	LandListMap::iterator iter = m_PlayerLandList.begin();
	for (; iter != m_PlayerLandList.end(); ++iter)
	{
		LandData_Save* pQuery = LandData_Save::ALLOC();
		pQuery->SetIndex(DB_LAND_DATA_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_LAND);
		pQuery->SetLandInfo(iter->second);
		pQuery->SetRoleId(iter->first);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// �洢����
void CLandManager::SaveDataToDB(DWORD dwRoleId)
{
	LandListMap::iterator iter = m_PlayerLandList.find(dwRoleId);
	if (iter != m_PlayerLandList.end())
	{
		LandData_Save* pQuery = LandData_Save::ALLOC();
		pQuery->SetIndex(DB_LAND_DATA_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_LAND);
		pQuery->SetLandInfo(iter->second);
		pQuery->SetRoleId(dwRoleId);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// ��������
void CLandManager::LoadDataFromDB(DWORD dwRoleId, const PlayerLandInfo &tLandInfo)
{
	std::pair<std::map<DWORD, PlayerLandInfo>::iterator, bool> result;
	result = m_PlayerLandList.insert(make_pair(dwRoleId, tLandInfo));
	if (!result.second)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������ʧ��[RoleId:%d]", __FUNCTION__, dwRoleId);
	}
}

// ������������
void CLandManager::LoadDataRequest()
{
	LandData_Load* pQuery = LandData_Load::ALLOC();
	pQuery->SetIndex(DB_LAND_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_LAND);
	g_MapServer.MapDBQuery(pQuery);
}

// ��ӡѲ�߽���
void CLandManager::PrintPatrolReward(Player *pPlayer, WORD wCityId)
{
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[RoleId:%d]", __FUNCTION__, pPlayer->GetCharID());
		return;
	}

	// ���ҵ����
	LandCityData *pLand = GetCityInfo(pPlayer->GetCharID(), wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��������[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// ȡ�������
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// Ѳ���¼�����
	for (BYTE i = 0; i < pLand->byPatrolEventNum; ++i)
	{
		LandPatrolEvent &tPatrolEvent = pLand->arPatrolEventList[i];
		const LandPatrolEvent_Config *pPatrolEventCfg = CConfigManager::Instance()->GetLandPatrolEvent(wCityId);
		if (!pPatrolEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ѳ���¼�����[RoleId:%d,CityId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId);
			return;
		}
		const LandPatrolEvent_Config::EventData *pEventCfg = pPatrolEventCfg->GetEventData(tPatrolEvent.wEventId);
		if (!pEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ѳ���¼�����[RoleId:%d,CityId:%d, EventId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId, tPatrolEvent.wEventId);
			return;
		}

		if (pEventCfg->dwObjId && pEventCfg->dwOjbNum)
		{
			DWORD dwOjbNum = tPatrolEvent.byIsDouble ? pEventCfg->dwOjbNum * 2 : pEventCfg->dwOjbNum;
			MAPLOG(ERROR_LEVEL, "[%s]Ѳ���¼�����[CityId:%d,RewardId:%d,RewardNum:%d]",
				__FUNCTION__, wCityId, pEventCfg->dwObjId, dwOjbNum);
		}
	}

	// ��������
	for (BYTE i = 0; i < pLand->byRiotEventNum; ++i)
	{
		LandRiotEvent &tRiotEvent = pLand->arRiotEventList[i];
		if (tRiotEvent.dwSuppressTime)
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ѳ�߱�����������[HappenTime:%d,SuppressTime:%d,RewardId:%s]",
				__FUNCTION__, tRiotEvent.dwHappenTime, tRiotEvent.dwSuppressTime, tRiotEvent.szFriendName);
		}
	}

	// ��Ƭ����
	const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pLand->wPatrolHeroInfoId);
	if (pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ѳ����Ƭ����[ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pComposeCfg->dwItemId, pLand->wAwardItemNum);
	}
}

void CLandManager::Update()
{
	UINT64 lCurTime = GetTickCount64();

	// 1�봦��һ��
	if (lCurTime < m_lProccessTime + 1000)
		return;

	DWORD dwCurTime = time(NULL);
	LandListMap::iterator iter = m_PlayerLandList.begin();
	for (; iter != m_PlayerLandList.end(); ++iter)
	{
		PlayerLandInfo *pPlayerLandInfo = &iter->second;
		for (WORD i = 0; i < pPlayerLandInfo->wCityNum; ++i)
		{
			LandCityData &tCityData = pPlayerLandInfo->arCityList[i];
			if (tCityData.dwStartPatrolTime && dwCurTime >= tCityData.dwLastHappenEventTime + tCityData.wIntervalTime)
			{
// 				if (!PatrolEventEx(iter->first, tCityData.wId))
// 					tCityData.dwLastHappenEventTime = dwCurTime;
				PatrolEventEx(iter->first, tCityData.wId);
 				tCityData.dwLastHappenEventTime = dwCurTime;
			}
		}
	}
	m_lProccessTime = lCurTime;
}

// ������ҵ��������
void CLandManager::OnNetPlayerLandData(Player *pPlayer, DWORD dwTargetId)
{
	SendPlayerLandData(pPlayer, dwTargetId);
}

// ��ʼѲ��
void CLandManager::OnNetPatrol(Player *pPlayer, DWORD dwHeroUID, WORD wCityId, BYTE byPatrolType, BYTE byTimeType)
{
	WORD wFromMsgId = C2S_LAND_PATROL_REQ;

	PlayerLandInfo *pPlayrLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayrLandInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ������Ϣ[RoleId:%d,LandId:%d]", 
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// ����Ƿ��д�Ӣ��
	const HeroInfo *pHero = pPlayer->GetHeroManager()->GetHero(dwHeroUID);
	if (!pHero)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��һ�û�л��Ѳ�ߵ�Ӣ��[RoleId:%d,HeroUID:%d]",
			__FUNCTION__, pPlayer->GetCharID(), dwHeroUID);
		return;
	}

	// ��Ӣ���Ƿ��Ѿ���Ѳ����
	if (pPlayrLandInfo->IsInPatrol(pHero->dwIntoId))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���Ѿ���Ѳ����...[RoleId:%d,HeroInfoId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), pHero->dwIntoId);
		return;
	}

	// ����ҪѲ�ߵ����
	LandCityData *pLand = pPlayrLandInfo->GetLandData(wCityId);
	if (!pLand)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]Ѳ�ߵ���ز�����[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(byPatrolType);
	if (!pPatrolCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(byTimeType);
	if (!pTimeData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[TimeType:%d]", __FUNCTION__, byTimeType);
		return;
	}

	// ���������ʯ
	if (pTimeData->nCostGold && pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pTimeData->nCostGold)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Ѳ�����ĵ���ʯ����[cost_diamond:%d]", __FUNCTION__, pTimeData->nCostGold);
		return;
	}

	// ������ľ���
	if (pTimeData->nCostEnergy && pPlayer->GetProperty(ROLE_PRO_ENERGY) < pTimeData->nCostEnergy)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Ѳ�����ĵ���ʯ����[cost_energy:%d]", __FUNCTION__, pTimeData->nCostEnergy);
		return;
	}

	// ���VIP����
	if (pPatrolCfg->byVipIdx && pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pPlayer->GetVipValue(pPatrolCfg->byVipIdx-1))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���Ѳ����VIP����[PatrolType:%d, VipIdx:%d]", 
			__FUNCTION__, byPatrolType, pPatrolCfg->byVipIdx);
		return;
	}

	// ����Ƿ���Ѳ����
	if (pLand->dwStartPatrolTime)
	{
		if (time(NULL) < pLand->dwStartPatrolTime + pTimeData->dwTotalTime)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�������Ѳ����...[LandId:%d]",__FUNCTION__, wCityId);
			return;
		}
	}

	// ��������
	if (pTimeData->nCostGold)
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pTimeData->nCostGold, TRUE, FROM_LAND_PATROL);
	if (pTimeData->nCostEnergy)
		pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -pTimeData->nCostEnergy);

	pLand->dwStartPatrolTime = time(NULL);
	pLand->byPatrolType = byPatrolType;
	pLand->byTimeType = byTimeType;
	pLand->dwLastHappenEventTime = 0;
	pLand->wIntervalTime = pPatrolCfg->wIntervalTime;
	pLand->wPatrolHeroInfoId = pHero->dwIntoId;
	pLand->byPatrolEventNum = 0;
	pLand->byRiotEventNum = 0;
	pLand->byIsGetAward = false;
	pLand->wAwardItemNum = 0;
	pLand->byIsRiot = false;

	// ����Ѳ���¼�
	PatrolEventEx(pPlayer->GetCharID(), wCityId);

	SendPlayerLandData(pPlayer);

	MSG_S2C_LAND_PATROL_RESP msg;
	msg.m_dwPatrolHeroUID = dwHeroUID;
	msg.m_byPatrolType = byPatrolType;
	msg.m_byTimeType = byTimeType;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// ���ݴ洢
	SaveDataToDB(pPlayer->GetRoleId());
}

// ��ȡѲ�߽���
void CLandManager::OnNetPatrolAward(Player *pPlayer, WORD wCityId)
{
	WORD wFromMsgId = C2S_LAND_GET_PATROL_AWARD_REQ;

	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[RoleId:%d]", __FUNCTION__, pPlayer->GetCharID());
		return;
	}

	WORD arCityList[MAX_LAND_CITY_NUM];
	BYTE byCityNum = 0;
	DWORD dwPatrolTotalTime = 0;

	// ������ȡ
	if (wCityId)
	{
		arCityList[byCityNum++] = wCityId;
	}
	// һ����ȡ
	else
	{
		// ȡ������ȡ����ȡ�б�
		for (WORD i = 0; i < pPlayerLandInfo->wCityNum; i++)
		{
			LandCityData &tCityData = pPlayerLandInfo->arCityList[i];
		}
	}

	BYTE byTakeCityNum = 0;
	for (BYTE i = 0; i < byCityNum; ++i)
	{
		wCityId = arCityList[i];

		// ���ҵ����
		LandCityData *pLand = GetCityInfo(pPlayer->GetCharID(), wCityId);
		if (!pLand)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��������[RoleId:%d,LandId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId);
			return;
		}

		// ����Ƿ������ȡ(ľ��Ѳ��Ӣ�ۻ���Ѳ�߻�ľ�н���) todo::�ݶ����ÿ���ȡ�ж�
// 		if (!pLand->wPatrolHeroInfoId || pLand->dwStartPatrolTime)
// 			continue;

		// ȡ�������
		const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
		if (!pLandCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
			return;
		}

		// ����Ѳ����ʱ��
		dwPatrolTotalTime += GetPatrolTotalTime(pLand->byPatrolType, pLand->byTimeType);

		// Ѳ���¼�����
		for (BYTE i = 0; i < pLand->byPatrolEventNum; ++i)
		{
			LandPatrolEvent &tPatrolEvent = pLand->arPatrolEventList[i];
			const LandPatrolEvent_Config *pPatrolEventCfg = CConfigManager::Instance()->GetLandPatrolEvent(wCityId);
			if (!pPatrolEventCfg)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ѳ���¼�����[RoleId:%d,CityId:%d]",
					__FUNCTION__, pPlayer->GetCharID(), wCityId);
				return;
			}
			const LandPatrolEvent_Config::EventData *pEventCfg = pPatrolEventCfg->GetEventData(tPatrolEvent.wEventId);
			if (!pEventCfg)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ѳ���¼�����[RoleId:%d,CityId:%d, EventId:%d]",
					__FUNCTION__, pPlayer->GetCharID(), wCityId, tPatrolEvent.wEventId);
				return;
			}

			if (pEventCfg->dwObjId && pEventCfg->dwOjbNum)
			{
				DWORD dwOjbNum = tPatrolEvent.byIsDouble ? pEventCfg->dwOjbNum * 2 : pEventCfg->dwOjbNum;
				pPlayer->AddObject(pEventCfg->dwObjId, dwOjbNum, TRUE, FROM_LAND_PATROL_AWARD);
			}
		}

		// ��������
		for (BYTE i = 0; i < pLand->byRiotEventNum; ++i)
		{
			LandRiotEvent &tRiotEvent = pLand->arRiotEventList[i];
			if (tRiotEvent.dwSuppressTime)
			{
				pPlayer->ModifyProperty(ROLE_PRO_ENERGY, pLandCfg->nSuppressEnergy);
			}
		}

		// ��Ƭ����
		const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pLand->wPatrolHeroInfoId);
		if (pComposeCfg)
		{
			pPlayer->AddObject(pComposeCfg->dwItemId, pLand->wAwardItemNum, TRUE, FROM_LAND_PATROL_AWARD);

			// ����㲥(�����ֻӢ��)
			if (pLand->wAwardItemNum >= pComposeCfg->wItemNum)
			{
				CSystemNotice sys_notice(10, pPlayer->GetServerId());
				sys_notice.AddParam(pPlayer->GetRoleName());
				sys_notice.AddParam(pLand->wPatrolHeroInfoId);
				sys_notice.SendMsg();
			}
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�ۺϳ���������[HeroId:%d]",__FUNCTION__, pLand->wPatrolHeroInfoId);
		}

		// ��¼�Ѿ���ȡ������
		pLand->byIsGetAward = true;
		pLand->wPatrolHeroInfoId = 0;
		pLand->dwStartPatrolTime = 0;
		pLand->byPatrolType = 0;
		pLand->byTimeType = 0;
		pLand->byPatrolEventNum = 0;
		pLand->byRiotEventNum = 0;

		++byTakeCityNum;
	}

	if (!byTakeCityNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰľ�п���ȡ���������", __FUNCTION__);
		return;
	}

	// ��¼�ۻ�Ѳ��ʱ��
	if (dwPatrolTotalTime)
		dwPatrolTotalTime = dwPatrolTotalTime / 3600;
	pPlayerLandInfo->dwPatrolTotalTime += dwPatrolTotalTime;

	MSG_S2C_LAND_GET_PATROL_AWARD_RESP msg;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// �ճ�����
	pPlayer->OnUpdateDailyMission(19, dwPatrolTotalTime);

	// �ɾ��¼�
	pPlayer->OnUpdateAchievement(14, dwPatrolTotalTime);

	// ���ݴ洢
	SaveDataToDB(pPlayer->GetRoleId());
}

// �������(����,����)
void CLandManager::OnNetLandLevelUp(Player *pPlayer, WORD wCityId)
{
	WORD wFromMsgId = C2S_LAND_SKILL_LEVELUP_REQ;

	// ȡ�������
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// ���ҵ����
	LandCityData *pLand = GetCityInfo(pPlayer->GetCharID(), wCityId);
	if (!pLand)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��������[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// �Ƿ��Ѵ�ȼ�����
	if (pLand->bySkillLevel >= pLandCfg->vSkillList.size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ؼ��ܵȼ��Ѵ�����[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// ȡ��ҵ������Ϣ
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ������Ϣ[RoleId:%d]",
			__FUNCTION__, pPlayer->GetCharID());
		return;
	}

	const LandSkill &tLandSkill = pLandCfg->vSkillList[pLand->bySkillLevel];

	// ��⼼����Ҫ��Ѳ��ʱ��
	if (tLandSkill.wNeedPatrolTime > pPlayerLandInfo->dwPatrolTotalTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ؼ�����Ҫ��Ѳ��ʱ�䲻��[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}
	
	// ������ĵ���ʯ
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < tLandSkill.nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ؼ�����Ҫ����ʯ����[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -tLandSkill.nCostDiamond, TRUE, FROM_LAND_LEVEL_UP);

	pLand->bySkillLevel++;

	SendPlayerLandData(pPlayer);

	MSG_S2C_LAND_SKILL_LEVELUP_RESP msg;
	msg.m_wCityId = wCityId;
	msg.m_bySkillLevel = pLand->bySkillLevel;
	pPlayer->SendPacket(&msg);

	// ���ݴ洢
	SaveDataToDB(pPlayer->GetRoleId());
}

// �������к�������б�
void CLandManager::OnNetFriendLandList(Player *pPlayer)
{
	WORD wFromMsgId = C2S_FRIEND_LAND_LIST_REQ;

	CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
	if (!pFriendMgr)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ѹ�����", __FUNCTION__);
		return;
	}

	MSG_S2C_FRIEND_LAND_LIST_RESP msg;
	msg.m_byLandNum = 0;
	const FriendData *pFriendList = pFriendMgr->GetFrindList();
	BYTE byFriendNum = pFriendMgr->GetFriendNum();
	for (BYTE i = 0; i < byFriendNum; ++i)
	{
		const FriendData &tFiendData = pFriendList[i];
		const PlayerBaseData *pFriendBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), tFiendData.dwFriendId);
		if (!pFriendBaseData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[FriendId:%d]", __FUNCTION__, tFiendData.dwFriendId);
			continue;
		}

		FriendLandData &tLandData = msg.m_LandList[msg.m_byLandNum++];
		memset(&tLandData, 0, sizeof(tLandData));

		// �����ѵĽ�ɫ��Ϣ
		tLandData.dwRoleId = pFriendBaseData->dwRoleId;
		tLandData.wLevel = pFriendBaseData->wLevel;
		tLandData.wHeadIcon = pFriendBaseData->wHeadIcon;
		strcpy_s(tLandData.szName, sizeof(tLandData.szName) - 1, pFriendBaseData->szRoleName);
		tLandData.dwLastLogoutTime = pFriendBaseData->dwOfflineTime;
	
		// �����ҵ������Ϣ
		PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(tFiendData.dwFriendId);
		if (pPlayerLandInfo)
		{
			tLandData.byCityNum = pPlayerLandInfo->wCityNum;
			for (BYTE k = 0; k < pPlayerLandInfo->wCityNum; ++k)
			{
				if (pPlayerLandInfo->arCityList[k].dwStartPatrolTime)
					tLandData.byPatrolCityNum++;

				if (pPlayerLandInfo->arCityList[k].byIsRiot)
					tLandData.byRiotCityNum++;
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// �����������б�
void CLandManager::OnNetCityDetails(Player *pPlayer, DWORD dwTargetId, WORD wCityId)
{
	LandCityData* pCityData = GetCityInfo(dwTargetId, wCityId);
	if (!pCityData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LAND_CITY_DETAILS_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĳǳ�����[RoleId:%d, CityId:%d]", 
			__FUNCTION__, dwTargetId, wCityId);
		return;
	}

	MSG_S2C_LAND_CITY_DETAILS_RESP msg;
	msg.m_dwPlayerId = dwTargetId;
	memcpy_s(&msg.m_tCityData, sizeof(LandCityData), pCityData, sizeof(LandCityData));
	pPlayer->SendPacket(&msg);
}

// ��ѹ����
void CLandManager::OnSuppressRiot(Player *pPlayer, DWORD dwFriendId, WORD wCityId)
{
	WORD wFromMsgId = C2S_LAND_SUPPRESS_RIOT_REQ;

	// �����ѹ��������
	if (pPlayer->GetProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES) >= pPlayer->GetVipValue(VIP_LAND_SUPPRESS_TIMES))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������ѹ�����Ѵ�����", __FUNCTION__);
		return;
	}

	// ȡ�������
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[CityId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// ���Һ��ѵ��������
	LandCityData *pLand = GetCityInfo(dwFriendId, wCityId);
	if (!pLand)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ѵ��������[FriendId:%d,CityId:%d]",
			__FUNCTION__, dwFriendId, wCityId);
		return;
	}

	// �Ƿ��ڱ�����
	if (!pLand->byIsRiot)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���ѵ����ľ���ڱ�����[FriendId:%d,CityId:%d]",
			__FUNCTION__, dwFriendId, wCityId);
		return;
	}

	// �����ѵ�ǰ�Ƿ�����ѹ�¼�
	if (!pLand->byRiotEventNum)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰû����ѹ�¼���¼[CityId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// ��¼������ѹ�¼�(���º��ѵı����¼�����) todo::�˴��߼���Щ�������,��ȡ�պ����������
	LandRiotEvent &tRoitEvent = pLand->arRiotEventList[pLand->byRiotEventNum - 1];
	tRoitEvent.dwSuppressTime = time(NULL);
	strcpy_s(tRoitEvent.szFriendName, sizeof(tRoitEvent.szFriendName) - 1, pPlayer->GetRoleName());
	pLand->byIsRiot = false;

	pPlayer->ModifyProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES, 1, false);

	SendPlayerLandData(pPlayer, dwFriendId);

	// �������ѹ��ý���
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pLandCfg->nHelpSuppressDiamond, TRUE, FROM_LAND_SUPPRESS_RIOT);

	MSG_S2C_LAND_SUPPRESS_RIOT_RESP msg;
	msg.m_dwFriendId = dwFriendId;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// �ճ�����
	pPlayer->OnUpdateDailyMission(16);

	// �ɾ��¼�
	pPlayer->OnUpdateAchievement(15);

	// ���ݴ洢
	SaveDataToDB(dwFriendId);
}

// ս����ʼ
bool CLandManager::OnBattleStar(Player *pPlayer, WORD wCityId)
{
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	// ����Ƿ��Ѵ����
	if (GetCityInfo(pPlayer->GetCharID(), wCityId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ѵ��[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	// ��Ҫǰ������
	if (pLandCfg->wPreCityId)
	{
		// ���ǰ�������Ƿ�������
		if (!GetCityInfo(pPlayer->GetCharID(), pLandCfg->wPreCityId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ǰ����ػ�ľ��ͨ��[RoleId:%d,LandId:%d,PreLandId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId, pLandCfg->wPreCityId);
			return false;
		}
	}

	pPlayer->GetFubenManager()->SetBattleInfo(wCityId, 0, pLandCfg->wMapId);

	return true;
}

// ս������
bool CLandManager::OnBattleOver(Player *pPlayer, WORD wCityId, BYTE byIsWin)
{
	// ֻ��ʤ���Ŵ���
	if (byIsWin)
	{
		// ����Ƿ��Ѵ��(todo::�������������ʾ�߼����Ѿ����ִ���)
		if (GetCityInfo(pPlayer->GetCharID(), wCityId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ش���!!!������Ѵ��[LandId:%d]", __FUNCTION__, wCityId);
			return false;
		}

		LandCityData tCityData;
		memset(&tCityData, 0, sizeof(tCityData));
		tCityData.wId = wCityId;
		AddCityInfo(pPlayer->GetCharID(), tCityData);
		SendPlayerLandData(pPlayer);

		// ���ݴ洢
		SaveDataToDB(pPlayer->GetRoleId());
	}

	return true;
}
// ��������������
void CLandManager::SendPlayerLandData(Player *pPlayer, DWORD dwTargetId)
{
	MSG_S2C_PLAYER_LAND_DATA_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	msg.m_byCityNum = 0;
	msg.m_byRiotSuppressTimes = pPlayer->GetProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES);
	msg.m_dwPatrolTotalTime = 0;
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(dwTargetId ? dwTargetId : pPlayer->GetCharID());
	if (pPlayerLandInfo)
	{
		msg.m_dwPatrolTotalTime = pPlayerLandInfo->dwPatrolTotalTime;
		for (WORD i = 0; i < pPlayerLandInfo->wCityNum; ++i)
		{
			LandCityData &tCityData = pPlayerLandInfo->arCityList[i];
			msg.m_CityList[i].wCityId = tCityData.wId;
			msg.m_CityList[i].dwStartPatrolTime = tCityData.dwStartPatrolTime;
			msg.m_CityList[i].byPatrolType = tCityData.byPatrolType;
			msg.m_CityList[i].byTimeType = tCityData.byTimeType;
			msg.m_CityList[i].byIsRiot = tCityData.byIsRiot;
			msg.m_CityList[i].bySkillLevel = tCityData.bySkillLevel;
			msg.m_CityList[i].wPatrolHeroInfoId = tCityData.wPatrolHeroInfoId;
			msg.m_CityList[i].byIsCanGetReward = false;
			// Ѳ�߽��� && ��ľ����ȡ && �н���
			if (!tCityData.dwStartPatrolTime && !tCityData.byIsGetAward && tCityData.byPatrolEventNum)
			{
				msg.m_CityList[i].byIsCanGetReward = true;
			}
		}
		msg.m_byCityNum = pPlayerLandInfo->wCityNum;
	}

	pPlayer->SendPacket(&msg);
}

// ȡ���״̬
bool CLandManager::IsHaveRedPoint(DWORD dwRoleId)
{
	// 1.���пɽ���Ѱ�õ����ʱ
	// 2.����Ѱ����ɣ��п���ȡ��Ѱ�ý���δ��ȡʱ�����·���ʾ���
	PlayerLandInfo *pLandInfo = GetPlayerLandInfo(dwRoleId);
	if (!pLandInfo)
		return false;

	for (WORD i = 0; i < pLandInfo->wCityNum; ++i)
	{
		LandCityData &tCity = pLandInfo->arCityList[i];
		// ��Ѳ��
		if (!tCity.wPatrolHeroInfoId)
			return true;

		// ����ȡ
		if (!tCity.dwStartPatrolTime && !tCity.byIsGetAward)
			return true;
	}

	return false;
}

// ȡ����������
WORD CLandManager::GetPlayerLandNum(DWORD dwRoleId)
{
	PlayerLandInfo *pLandInfo = GetPlayerLandInfo(dwRoleId);
	return pLandInfo ? pLandInfo->wCityNum : 0;
}

// ����ʱ�ӿ�(������)
PlayerLandInfo* CLandManager::GetPlayerLandInfoEx(DWORD dwRoleId)
{
	return GetPlayerLandInfo(dwRoleId);
}

void CLandManager::PatrolEvent_Test(DWORD dwRoleId, WORD wCityId)
{
	PatrolEventEx(dwRoleId, wCityId);
}

// ȡ��������Ϣ 
PlayerLandInfo* CLandManager::GetPlayerLandInfo(DWORD dwRoleId)
{
	LandListMap::iterator iter = m_PlayerLandList.find(dwRoleId);
	return iter != m_PlayerLandList.end() ? &iter->second : NULL;
}

// ȡ�ǳ���Ϣ
LandCityData* CLandManager::GetCityInfo(DWORD dwRoleId, WORD wCityId)
{
	PlayerLandInfo* pPlayerLandInfo = GetPlayerLandInfo(dwRoleId);
	if (!pPlayerLandInfo)
	{
		return NULL;
	}

	for (WORD i = 0; i < pPlayerLandInfo->wCityNum; ++i)
	{
		if (pPlayerLandInfo->arCityList[i].wId == wCityId)
		{
			return &pPlayerLandInfo->arCityList[i];
		}
	}
	return NULL;
}

// ��������Ϣ
void CLandManager::AddCityInfo(DWORD dwRoleId, const LandCityData &tCityData)
{
	PlayerLandInfo* pPlayerLandInfo = GetPlayerLandInfo(dwRoleId);
	if (pPlayerLandInfo)
	{
		if (pPlayerLandInfo->wCityNum >= MAX_LAND_CITY_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������Ϣʱ�б�����!!!!!", __FUNCTION__);
			return;
		}

		pPlayerLandInfo->arCityList[pPlayerLandInfo->wCityNum++] = tCityData;
	}
	else
	{
		PlayerLandInfo tPlayerLandInfo;
		memset(&tPlayerLandInfo, 0, sizeof(tPlayerLandInfo));
		tPlayerLandInfo.arCityList[tPlayerLandInfo.wCityNum++] = tCityData;
		m_PlayerLandList.insert(make_pair(dwRoleId, tPlayerLandInfo));
	}
}

// Ѳ���¼�
// void CLandManager::PatrolEvent(DWORD dwRoleId, WORD wCityId)
// {
// 	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
// 	if (!pLandCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
// 		return;
// 	}
// 
// 	LandCityData *pLand = GetCityInfo(dwRoleId, wCityId);
// 	if (!pLand)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��������[RoleId:%d,LandId:%d]",
// 			__FUNCTION__, dwRoleId, wCityId);
// 		return;
// 	}
// 
// 	// ����¼�ID
// 	WORD wEventId = RandEventId(wCityId);
// 	if (!wEventId)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�����سǳ��¼�IDΪ��[LandId:%d]",__FUNCTION__, wCityId);
// 		return;
// 	}
// 
// 	DWORD dwCurTime = time(NULL);
// 
// 	LandPatrolEvent tPatrolEvent;
// 	tPatrolEvent.wEventId = wEventId;
// 	tPatrolEvent.dwHappenTime = dwCurTime;
// 	tPatrolEvent.byIsDouble = false;
// 
// 	// ���˫������
// 	if (pLand->bySkillLevel)
// 	{
// 		if (g_Rand(1,100) <= pLandCfg->vSkillList[pLand->bySkillLevel - 1].wDoubleAwardRate)
// 		{
// 			tPatrolEvent.byIsDouble = true;
// 		}
// 	}
// 
// 	pLand->AddPatrolEvent(tPatrolEvent);
// 
// 	// ��������¼�
// 	if (!pLand->byIsRiot && g_Rand() <= g_GetConfigValue(100))
// 	{
// 		LandRiotEvent tRiotEvent;
// 		memset(&tRiotEvent, 0, sizeof(tRiotEvent));
// 		tRiotEvent.dwHappenTime = dwCurTime;
// 		pLand->AddRoitEvent(tRiotEvent);
// 		pLand->byIsRiot = true;
// 	}
// 
// 	// ���һ�δ��������Ƭ
// 	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(pLand->byPatrolType);
// 	if (!pPatrolCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d]", __FUNCTION__, pLand->byPatrolType);
// 		return;
// 	}
// 	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(pLand->byTimeType);
// 	if (!pTimeData)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d,TimeType:%d]",
// 			__FUNCTION__, pLand->byPatrolType, pLand->byTimeType);
// 		return;
// 	}
// 	// todo::�����Ƿ���Ҫ�����ʱ��
// 	if (dwCurTime >= (pLand->dwStartPatrolTime + pTimeData->dwTotalTime))
// 	{
// 		pLand->wAwardItemNum = Rand::Instance()->irand(pTimeData->wItemNumMin, pTimeData->wItemNumMax);
// 		
// 		pLand->dwStartPatrolTime = 0; // Ѳ�߽���
// 		pLand->byIsRiot = false;
// 		MAPLOG(GUI_LEVEL, "[%s]Ѳ�߽���[RoleId:%d,CityId:%d]", __FUNCTION__, dwRoleId,wCityId);
// 	}
// 
// 	pLand->dwLastHappenEventTime = dwCurTime;
// 	MAPLOG(GUI_LEVEL, "[%s]�������Ѳ���¼�[CityId:%d]", __FUNCTION__, wCityId);
// }

bool CLandManager::PatrolEventEx(DWORD dwRoleId, WORD wCityId)
{
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	LandCityData *pLand = GetCityInfo(dwRoleId, wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��������[RoleId:%d,LandId:%d]",
			__FUNCTION__, dwRoleId, wCityId);
		return false;
	}

	// ����¼�ID
	WORD wEventId = RandEventId(wCityId);
	if (!wEventId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����سǳ��¼�IDΪ��[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	DWORD dwCurTime = time(NULL);

	// ���·����¼�ʱ��
	if (pLand->dwLastHappenEventTime)
		pLand->dwLastHappenEventTime += pLand->wIntervalTime;
	else
		pLand->dwLastHappenEventTime = dwCurTime;

	// �¼�����
	LandPatrolEvent tPatrolEvent;
	tPatrolEvent.wEventId = wEventId;
	tPatrolEvent.dwHappenTime = pLand->dwLastHappenEventTime;
	tPatrolEvent.byIsDouble = false;

	// ���˫������
	if (pLand->bySkillLevel && g_Rand(1, 100) <= pLandCfg->vSkillList[pLand->bySkillLevel - 1].wDoubleAwardRate)
		tPatrolEvent.byIsDouble = true;

	pLand->AddPatrolEvent(tPatrolEvent);

	// ��������¼�
	if (!pLand->byIsRiot && g_Rand() <= g_GetConfigValue(100))
	{
		LandRiotEvent tRiotEvent;
		memset(&tRiotEvent, 0, sizeof(tRiotEvent));
		tRiotEvent.dwHappenTime = dwCurTime;
		pLand->AddRoitEvent(tRiotEvent);
		pLand->byIsRiot = true;
	}


	// ���һ�δ��������Ƭ
	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(pLand->byPatrolType);
	if (!pPatrolCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d]", __FUNCTION__, pLand->byPatrolType);
		return false;
	}
	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(pLand->byTimeType);
	if (!pTimeData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d,TimeType:%d]",
			__FUNCTION__, pLand->byPatrolType, pLand->byTimeType);
		return false;
	}
	// ��Ϊһ��ʼѲ�߾���һ�� todo::�����Ƿ���Ҫ�����ʱ�� 
	if (pLand->wIntervalTime * (pLand->byPatrolEventNum - 1) >= pTimeData->dwTotalTime)
	//if (dwCurTime >= (pLand->dwStartPatrolTime + pTimeData->dwTotalTime))
	{
		pLand->wAwardItemNum = Rand::Instance()->irand(pTimeData->wItemNumMin, pTimeData->wItemNumMax);

		pLand->dwStartPatrolTime = 0; // Ѳ�߽���
		pLand->byIsRiot = false;
		MAPLOG(GUI_LEVEL, "[%s]Ѳ�߽���[RoleId:%d,CityId:%d]", __FUNCTION__, dwRoleId, wCityId);
	}

	//pLand->dwLastHappenEventTime = dwCurTime;
	MAPLOG(GUI_LEVEL, "[%s]�������Ѳ���¼�[CityId:%d]", __FUNCTION__, wCityId);

	// ���ݴ洢
	SaveDataToDB(dwRoleId);

	return true;
}

// ����¼�ID
WORD CLandManager::RandEventId(WORD wCityId)
{
	// ȡ��ؽ�������
	const LandPatrolEvent_Config *pPatrolEventCfg = CConfigManager::Instance()->GetLandPatrolEvent(wCityId);
	if (!pPatrolEventCfg || !pPatrolEventCfg->vEventList.size())
	{
	 	MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ؽ�������[LandId:%d]", __FUNCTION__, wCityId);
	 	return 0;
	}

	// �����ܸ���
	int nTotalRate = 0;
	for (size_t i = 0; i < pPatrolEventCfg->vEventList.size(); ++i)
	{
	 	nTotalRate += pPatrolEventCfg->vEventList[i].wEventRate;
	}

	// ���һ��ֵ
	int nRandValue = Rand::Instance()->irand(1, nTotalRate);

	// �����Ǹ��¼�������
	int nRandEventIdx = 0;
	int nCurRate = 0;
	for (size_t i = 0; i < pPatrolEventCfg->vEventList.size(); ++i)
	{
	 	nCurRate += pPatrolEventCfg->vEventList[i].wEventRate;
	 	if (nRandValue <= nCurRate)
	 	{
	 		nRandEventIdx = i;
	 		break;
	 	}
	}

	return pPatrolEventCfg->vEventList[nRandEventIdx].wEventId;
}

// ȡѲ����ʱ��
DWORD CLandManager::GetPatrolTotalTime(BYTE byPatrolType, BYTE byTimeType)
{
	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(byPatrolType);
	if (!pPatrolCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d]", __FUNCTION__, byPatrolType);
		return 0;
	}
	
	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(byTimeType);
	if (!pTimeData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d,TimeType:%d]",__FUNCTION__, byPatrolType, byTimeType);
		return 0;
	}

	return pTimeData->dwTotalTime;
}