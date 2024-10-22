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

CLandManager::CLandManager()
{
	m_dwProccessTime = GetTickCount();
}

CLandManager::~CLandManager()
{

}

void CLandManager::Init()
{

}

void CLandManager::Update()
{
	// 20�봦��һ��
// 	if (GetTickCount() < m_dwProccessTime + 20000)
// 		return;

	DWORD dwCurTime = time(NULL);
	LandListMap::iterator iter = m_PlayerLandList.begin();
	for (; iter != m_PlayerLandList.end(); ++iter)
	{
		PlayerLandInfo *pPlayerLandInfo = &iter->second;
		for (BYTE i = 0; i < pPlayerLandInfo->byCityNum; ++i)
		{
			LandCityData &tCityData = pPlayerLandInfo->arCityList[i];
			if (tCityData.dwStartPatrolTime && dwCurTime >= tCityData.dwLastHappenEventTime + tCityData.wIntervalTime)
			{
				PatrolEvent(iter->first, tCityData.wId);
				tCityData.dwLastHappenEventTime = dwCurTime;
			}
		}
	}
}

// ������ҵ��������
void CLandManager::OnNetPlayerLandData(Player *pPlayer, DWORD dwTargetId)
{
	SendPlayerLandData(pPlayer, dwTargetId);
}

// ��ʼѲ��
void CLandManager::OnNetPatrol(Player *pPlayer, DWORD dwHeroUID, WORD wCityId, BYTE byPatrolType, BYTE byTimeType)
{
	PlayerLandInfo *pPlayrLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayrLandInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ������Ϣ[RoleId:%d,LandId:%d]", 
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// ����Ƿ��д�Ӣ��
	const HeroInfo *pHero = pPlayer->GetHeroManager()->GetHero(dwHeroUID);
	if (!pHero)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��һ�û�л��Ѳ�ߵ�Ӣ��[RoleId:%d,HeroUID:%d]",
			__FUNCTION__, pPlayer->GetCharID(), dwHeroUID);
		return;
	}

	// ��Ӣ���Ƿ��Ѿ���Ѳ����
	if (pPlayrLandInfo->IsInPatrol(pHero->dwIntoId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���Ѿ���Ѳ����...[RoleId:%d,HeroInfoId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), pHero->dwIntoId);
		return;
	}

	// ����ҪѲ�ߵ����
	LandCityData *pLand = pPlayrLandInfo->GetLandData(wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ѳ�ߵ���ز�����[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(byPatrolType);
	if (!pPatrolCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// ���VIP����
	if (pPatrolCfg->byVipIdx && pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pPlayer->GetVipValue(pPatrolCfg->byVipIdx-1))
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ѳ����VIP����[PatrolType:%d, VipIdx:%d]", 
			__FUNCTION__, byPatrolType, pPatrolCfg->byVipIdx);
		return;
	}

	// ����Ƿ���Ѳ����
	if (pLand->dwStartPatrolTime)
	{
		const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(byTimeType);
		if (!pTimeData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[TimeType:%d]",__FUNCTION__, byTimeType);
			return;
		}
		if (time(NULL) < pLand->dwStartPatrolTime + pTimeData->dwTotalTime)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�������Ѳ����...[LandId:%d]",__FUNCTION__, wCityId);
			return;
		}
	}

	pLand->dwStartPatrolTime = time(NULL);
	pLand->byPatrolType = byPatrolType;
	pLand->byTimeType = byTimeType;
	pLand->dwLastHappenEventTime = 0;
	pLand->wIntervalTime = pPatrolCfg->wIntervalTime;
	pLand->wPatrolHeroInfoId = pHero->dwIntoId;
	pLand->byPatrolEventNum = 0;
	pLand->byRiotEventNum = 0;
	pLand->byIsGetAward = false;

	// ����Ѳ���¼�
	PatrolEvent(pPlayer->GetCharID(), wCityId);

	SendPlayerLandData(pPlayer);

	MSG_S2C_LAND_PATROL_RESP msg;
	msg.m_dwPatrolHeroUID = dwHeroUID;
	msg.m_byPatrolType = byPatrolType;
	msg.m_byTimeType = byTimeType;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);
}

// ��ȡѲ�߽���
void CLandManager::OnNetPatrolAward(Player *pPlayer, WORD wCityId)
{
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[RoleId:%d]", __FUNCTION__, pPlayer->GetCharID());
		return;
	}

	WORD arCityList[MAX_LAND_CITY_NUM];
	BYTE byCityNum = 0;
	bool bCheckTime = true;
	DWORD dwPatrolTotalTime = 0;

	if (wCityId)
	{
		arCityList[byCityNum++] = wCityId;
	}
	else
	{
		for (BYTE i = 0; i < pPlayerLandInfo->byCityNum; i++)
		{
			LandCityData &tCityData = pPlayerLandInfo->arCityList[i];
			if (!tCityData.dwStartPatrolTime || tCityData.byIsGetAward)
				continue;

			const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(tCityData.byPatrolType);
			if (!pPatrolCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d]", __FUNCTION__, tCityData.byPatrolType);
				return;
			}
			const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(tCityData.byTimeType);
			if (!pTimeData)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d,TimeType:%d]", 
					__FUNCTION__, tCityData.byPatrolType, tCityData.byTimeType);
				return;
			}

			if (time(NULL) >= tCityData.dwStartPatrolTime + pTimeData->dwTotalTime)
			{
				arCityList[byCityNum++] = tCityData.wId;

				dwPatrolTotalTime += pTimeData->dwTotalTime;
			}
		}

		bCheckTime = false;
	}

	for (BYTE i = 0; i < byCityNum; ++i)
	{
		wCityId = arCityList[i];

		// ȡ�������
		const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
		if (!pLandCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
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

		// ����Ƿ��Ѿ���ȡ������
		if (pLand->byIsGetAward)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��سǳؽ����Ѿ���ȡ��[RoleId:%d,LandId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId);
			return;
		}

		// �Ƿ���Ҫ���ʱ��
		if (bCheckTime)
		{
			const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(pLand->byPatrolType);
			if (!pPatrolCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d]", __FUNCTION__, pLand->byPatrolType);
				return;
			}
			const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(pLand->byTimeType);
			if (!pTimeData)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d,TimeType:%d]",
					__FUNCTION__, pLand->byPatrolType, pLand->byTimeType);
				return;
			}

			// ����Ƿ������ȡ��
			if (time(NULL) < pLand->dwStartPatrolTime + pTimeData->dwTotalTime)
			{
				MAPLOG(ERROR_LEVEL, "[%s]��û����ȡѲ�߽���ʱ��[RoleId:%d,LandId:%d]",
					__FUNCTION__, pPlayer->GetCharID(), wCityId);
				return;
			}

			dwPatrolTotalTime += pTimeData->dwTotalTime;
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

			if (pEventCfg->wObjId && pEventCfg->wOjbNum)
				pPlayer->AddObject(pEventCfg->wObjId, pEventCfg->wOjbNum);
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
			pPlayer->AddObject(pComposeCfg->dwItemId, pLand->byAwardItemNum);

			// ����㲥(�����ֻӢ��)
			if (pLand->byAwardItemNum >= pComposeCfg->wItemNum)
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
}

// �������(����,����)
void CLandManager::OnNetLandLevelUp(Player *pPlayer, WORD wCityId)
{
	// ȡ�������
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
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

	// �Ƿ��Ѵ�ȼ�����
	if (pLand->bySkillLevel >= pLandCfg->vSkillList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ؼ��ܵȼ��Ѵ�����[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// ȡ��ҵ������Ϣ
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ������Ϣ[RoleId:%d]",
			__FUNCTION__, pPlayer->GetCharID());
		return;
	}

	const LandSkill &tLandSkill = pLandCfg->vSkillList[pLand->bySkillLevel];

	// ��⼼����Ҫ��Ѳ��ʱ��
	if (tLandSkill.wNeedPatrolTime > pPlayerLandInfo->dwPatrolTotalTime)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ؼ�����Ҫ��Ѳ��ʱ�䲻��[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}
	
	// ������ĵ���ʯ
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < tLandSkill.nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ؼ�����Ҫ����ʯ����[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -tLandSkill.nCostDiamond, TRUE, CURRENCY_FROM_LAND_LEVEL_UP);

	pLand->bySkillLevel++;

	SendPlayerLandData(pPlayer);

	MSG_S2C_LAND_SKILL_LEVELUP_RESP msg;
	msg.m_wCityId = wCityId;
	msg.m_bySkillLevel = pLand->bySkillLevel;
	pPlayer->SendPacket(&msg);
}

// �������к�������б�
void CLandManager::OnNetFriendLandList(Player *pPlayer)
{
	CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
	if (!pFriendMgr)
	{
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
			tLandData.byCityNum = pPlayerLandInfo->byCityNum;
			for (BYTE j = 0; j < pPlayerLandInfo->byCityNum; ++j)
			{
				if (pPlayerLandInfo->arCityList[j].dwStartPatrolTime)
					tLandData.byPatrolCityNum++;

				if (pPlayerLandInfo->arCityList[j].byIsRiot)
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
	// �����ѹ��������
	if (pPlayer->GetProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES) >= pPlayer->GetVipValue(VIP_LAND_SUPPRESS_TIMES))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ѹ�����Ѵ�����", __FUNCTION__);
		return;
	}

	// ȡ�������
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[CityId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// ���Һ��ѵ��������
	LandCityData *pLand = GetCityInfo(dwFriendId, wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ѵ��������[FriendId:%d,CityId:%d]",
			__FUNCTION__, dwFriendId, wCityId);
		return;
	}

	// �Ƿ��ڱ�����
	if (!pLand->byIsRiot)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ѵ����ľ���ڱ�����[FriendId:%d,CityId:%d]",
			__FUNCTION__, dwFriendId, wCityId);
		return;
	}

	// �����ѵ�ǰ�Ƿ�����ѹ�¼�
	if (!pLand->byRiotEventNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰû����ѹ�¼���¼[CityId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// ��¼������ѹ�¼�(���º��ѵı����¼�����) todo::�˴��߼���Щ�������,��ȡ�պ����������
	LandRiotEvent &tRoitEvent = pLand->arRiotEventList[pLand->byRiotEventNum - 1];
	tRoitEvent.dwSuppressTime = time(NULL);
	strcpy_s(tRoitEvent.szFriendName, sizeof(tRoitEvent.szFriendName) - 1, pPlayer->GetCharName());
	pLand->byIsRiot = false;

	pPlayer->ModifyProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES, 1, false);

	SendPlayerLandData(pPlayer, dwFriendId);

	// �������ѹ��ý���
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pLandCfg->nHelpSuppressDiamond, TRUE, CURRENCY_FROM_LAND_SUPPRESS_RIOT);

	MSG_S2C_LAND_SUPPRESS_RIOT_RESP msg;
	msg.m_dwFriendId = dwFriendId;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// �ճ�����
	pPlayer->OnUpdateDailyMission(16);

	// �ɾ��¼�
	pPlayer->OnUpdateAchievement(15);
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
		for (BYTE i = 0; i < pPlayerLandInfo->byCityNum; ++i)
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
		msg.m_byCityNum = pPlayerLandInfo->byCityNum;
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

	for (BYTE i = 0; i < pLandInfo->byCityNum; ++i)
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

// ����ʱ�ӿ�(������)
PlayerLandInfo* CLandManager::GetPlayerLandInfoEx(DWORD dwRoleId)
{
	return GetPlayerLandInfo(dwRoleId);
}

void CLandManager::PatrolEvent_Test(DWORD dwRoleId, WORD wCityId)
{
	PatrolEvent(dwRoleId, wCityId);
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

	for (BYTE i = 0; i < pPlayerLandInfo->byCityNum; ++i)
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
		if (pPlayerLandInfo->byCityNum >= MAX_LAND_CITY_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������Ϣʱ�б�����!!!!!", __FUNCTION__);
			return;
		}

		pPlayerLandInfo->arCityList[pPlayerLandInfo->byCityNum++] = tCityData;
	}
	else
	{
		PlayerLandInfo tPlayerLandInfo;
		memset(&tPlayerLandInfo, 0, sizeof(tPlayerLandInfo));
		tPlayerLandInfo.arCityList[tPlayerLandInfo.byCityNum++] = tCityData;
		m_PlayerLandList.insert(make_pair(dwRoleId, tPlayerLandInfo));
	}
}

// Ѳ���¼�
void CLandManager::PatrolEvent(DWORD dwRoleId, WORD wCityId)
{
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	LandCityData *pLand = GetCityInfo(dwRoleId, wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ��������[RoleId:%d,LandId:%d]",
			__FUNCTION__, dwRoleId, wCityId);
		return;
	}

	DWORD dwCurTime = time(NULL);

	// ��һ�η����¼�ʱ,��Ҫ��ʼʱ��
// 	if (!pLand->dwLastHappenEventTime)
// 		pLand->dwLastHappenEventTime = dwCurTime;
// 
// 	do 
// 	{
// 		// todo::��ֹ������ѭ��(�Ǳ����߼�)
// 		if (!pLand->wIntervalTime)
// 			break;
// 
// 		// ����¼�ID
// 		WORD wEventId = RandEventId(wCityId);
// 		if (!wEventId)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]�����سǳ��¼�IDΪ��[LandId:%d]", __FUNCTION__, wCityId);
// 			return;
// 		}
// 
// 		// �����¼�����ʱ��
// 		pLand->dwLastHappenEventTime += pLand->wIntervalTime;
// 
// 		LandPatrolEvent tPatrolEvent;
// 		tPatrolEvent.wEventId = wEventId;
// 		tPatrolEvent.dwHappenTime = pLand->dwLastHappenEventTime;
// 		tPatrolEvent.byIsDouble = false;
// 
// 		// ���˫������
// 		if (pLand->bySkillLevel && (g_Rand(1, 100) <= pLandCfg->vSkillList[pLand->bySkillLevel - 1].wDoubleAwardRate))
// 			tPatrolEvent.byIsDouble = true;
// 
// 		pLand->AddPatrolEvent(tPatrolEvent);
// 
// 		// ��������¼�
// 		if (!pLand->byIsRiot && g_Rand() <= g_GetConfigValue(100))
// 		{
// 			LandRiotEvent tRiotEvent;
// 			memset(&tRiotEvent, 0, sizeof(tRiotEvent));
// 			tRiotEvent.dwHappenTime = dwCurTime;
// 			pLand->AddRoitEvent(tRiotEvent);
// 			pLand->byIsRiot = true;
// 		}
// 	} while (dwCurTime >= (pLand->dwLastHappenEventTime + pLand->wIntervalTime));

	// ����¼�ID
 	WORD wEventId = RandEventId(wCityId);
 	if (!wEventId)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�����سǳ��¼�IDΪ��[LandId:%d]",__FUNCTION__, wCityId);
 		return;
 	}

 	//DWORD dwCurTime = time(NULL);
 
 	LandPatrolEvent tPatrolEvent;
 	tPatrolEvent.wEventId = wEventId;
 	tPatrolEvent.dwHappenTime = dwCurTime;
 	tPatrolEvent.byIsDouble = false;
 
 	// ���˫������
 	if (pLand->bySkillLevel)
 	{
 		if (g_Rand(1,100) <= pLandCfg->vSkillList[pLand->bySkillLevel - 1].wDoubleAwardRate)
 		{
 			tPatrolEvent.byIsDouble = true;
 		}
 	}
 
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
		return;
	}
	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(pLand->byTimeType);
	if (!pTimeData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ѳ����������[PatrolType:%d,TimeType:%d]",
			__FUNCTION__, pLand->byPatrolType, pLand->byTimeType);
		return;
	}
	// todo::�����Ƿ���Ҫ�����ʱ��
	if (dwCurTime >= (pLand->dwStartPatrolTime + pTimeData->dwTotalTime))
	{
		pLand->byAwardItemNum = Rand::Instance()->irand(pTimeData->wItemNumMin, pTimeData->wItemNumMax);
		
		pLand->dwStartPatrolTime = 0; // Ѳ�߽���
		pLand->byIsRiot = false;
		//pLand->dwLastHappenEventTime = 0;
		MAPLOG(GUI_LEVEL, "[%s]Ѳ�߽���[RoleId:%d,CityId:%d]", __FUNCTION__, dwRoleId,wCityId);
	}

	pLand->dwLastHappenEventTime = dwCurTime;
	MAPLOG(GUI_LEVEL, "[%s]�������Ѳ���¼�[CityId:%d]", __FUNCTION__, wCityId);
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
