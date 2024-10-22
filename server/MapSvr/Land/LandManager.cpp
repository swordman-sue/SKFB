/**\file
领地管理器
\author Kevin
\date 2016/9/24
\note 间隔时间全部加起来一定要等于巡逻总时间
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

// 存储所有数据
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

// 存储数据
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

// 加载数据
void CLandManager::LoadDataFromDB(DWORD dwRoleId, const PlayerLandInfo &tLandInfo)
{
	std::pair<std::map<DWORD, PlayerLandInfo>::iterator, bool> result;
	result = m_PlayerLandList.insert(make_pair(dwRoleId, tLandInfo));
	if (!result.second)
	{
		MAPLOG(ERROR_LEVEL, "[%s]插入领地失败[RoleId:%d]", __FUNCTION__, dwRoleId);
	}
}

// 加载数据请求
void CLandManager::LoadDataRequest()
{
	LandData_Load* pQuery = LandData_Load::ALLOC();
	pQuery->SetIndex(DB_LAND_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_LAND);
	g_MapServer.MapDBQuery(pQuery);
}

// 打印巡逻奖励
void CLandManager::PrintPatrolReward(Player *pPlayer, WORD wCityId)
{
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家领地配置[RoleId:%d]", __FUNCTION__, pPlayer->GetCharID());
		return;
	}

	// 查找的领地
	LandCityData *pLand = GetCityInfo(pPlayer->GetCharID(), wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地数据[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// 取领地配置
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// 巡逻事件奖励
	for (BYTE i = 0; i < pLand->byPatrolEventNum; ++i)
	{
		LandPatrolEvent &tPatrolEvent = pLand->arPatrolEventList[i];
		const LandPatrolEvent_Config *pPatrolEventCfg = CConfigManager::Instance()->GetLandPatrolEvent(wCityId);
		if (!pPatrolEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到领地巡逻事件配置[RoleId:%d,CityId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId);
			return;
		}
		const LandPatrolEvent_Config::EventData *pEventCfg = pPatrolEventCfg->GetEventData(tPatrolEvent.wEventId);
		if (!pEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到领地巡逻事件配置[RoleId:%d,CityId:%d, EventId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId, tPatrolEvent.wEventId);
			return;
		}

		if (pEventCfg->dwObjId && pEventCfg->dwOjbNum)
		{
			DWORD dwOjbNum = tPatrolEvent.byIsDouble ? pEventCfg->dwOjbNum * 2 : pEventCfg->dwOjbNum;
			MAPLOG(ERROR_LEVEL, "[%s]巡逻事件奖励[CityId:%d,RewardId:%d,RewardNum:%d]",
				__FUNCTION__, wCityId, pEventCfg->dwObjId, dwOjbNum);
		}
	}

	// 暴动奖励
	for (BYTE i = 0; i < pLand->byRiotEventNum; ++i)
	{
		LandRiotEvent &tRiotEvent = pLand->arRiotEventList[i];
		if (tRiotEvent.dwSuppressTime)
		{
			MAPLOG(ERROR_LEVEL, "[%s]巡逻暴动奖励精力[HappenTime:%d,SuppressTime:%d,RewardId:%s]",
				__FUNCTION__, tRiotEvent.dwHappenTime, tRiotEvent.dwSuppressTime, tRiotEvent.szFriendName);
		}
	}

	// 碎片奖励
	const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pLand->wPatrolHeroInfoId);
	if (pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]巡逻碎片奖励[ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pComposeCfg->dwItemId, pLand->wAwardItemNum);
	}
}

void CLandManager::Update()
{
	UINT64 lCurTime = GetTickCount64();

	// 1秒处理一次
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

// 请求玩家的领地数据
void CLandManager::OnNetPlayerLandData(Player *pPlayer, DWORD dwTargetId)
{
	SendPlayerLandData(pPlayer, dwTargetId);
}

// 开始巡逻
void CLandManager::OnNetPatrol(Player *pPlayer, DWORD dwHeroUID, WORD wCityId, BYTE byPatrolType, BYTE byTimeType)
{
	WORD wFromMsgId = C2S_LAND_PATROL_REQ;

	PlayerLandInfo *pPlayrLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayrLandInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地信息[RoleId:%d,LandId:%d]", 
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// 检测是否有此英雄
	const HeroInfo *pHero = pPlayer->GetHeroManager()->GetHero(dwHeroUID);
	if (!pHero)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家还没有获得巡逻的英雄[RoleId:%d,HeroUID:%d]",
			__FUNCTION__, pPlayer->GetCharID(), dwHeroUID);
		return;
	}

	// 此英雄是否已经在巡逻中
	if (pPlayrLandInfo->IsInPatrol(pHero->dwIntoId))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]英雄已经在巡逻中...[RoleId:%d,HeroInfoId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), pHero->dwIntoId);
		return;
	}

	// 查找要巡逻的领地
	LandCityData *pLand = pPlayrLandInfo->GetLandData(wCityId);
	if (!pLand)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]巡逻的领地不存在[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(byPatrolType);
	if (!pPatrolCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻类型配置[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(byTimeType);
	if (!pTimeData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[TimeType:%d]", __FUNCTION__, byTimeType);
		return;
	}

	// 检测消耗钻石
	if (pTimeData->nCostGold && pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pTimeData->nCostGold)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领地巡逻消耗的钻石不足[cost_diamond:%d]", __FUNCTION__, pTimeData->nCostGold);
		return;
	}

	// 检测消耗精力
	if (pTimeData->nCostEnergy && pPlayer->GetProperty(ROLE_PRO_ENERGY) < pTimeData->nCostEnergy)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领地巡逻消耗的钻石不足[cost_energy:%d]", __FUNCTION__, pTimeData->nCostEnergy);
		return;
	}

	// 检测VIP限制
	if (pPatrolCfg->byVipIdx && pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pPlayer->GetVipValue(pPatrolCfg->byVipIdx-1))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领地巡逻受VIP限制[PatrolType:%d, VipIdx:%d]", 
			__FUNCTION__, byPatrolType, pPatrolCfg->byVipIdx);
		return;
	}

	// 检测是否在巡逻中
	if (pLand->dwStartPatrolTime)
	{
		if (time(NULL) < pLand->dwStartPatrolTime + pTimeData->dwTotalTime)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]领地正在巡逻中...[LandId:%d]",__FUNCTION__, wCityId);
			return;
		}
	}

	// 处理消耗
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

	// 触发巡逻事件
	PatrolEventEx(pPlayer->GetCharID(), wCityId);

	SendPlayerLandData(pPlayer);

	MSG_S2C_LAND_PATROL_RESP msg;
	msg.m_dwPatrolHeroUID = dwHeroUID;
	msg.m_byPatrolType = byPatrolType;
	msg.m_byTimeType = byTimeType;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// 数据存储
	SaveDataToDB(pPlayer->GetRoleId());
}

// 领取巡逻奖励
void CLandManager::OnNetPatrolAward(Player *pPlayer, WORD wCityId)
{
	WORD wFromMsgId = C2S_LAND_GET_PATROL_AWARD_REQ;

	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家领地配置[RoleId:%d]", __FUNCTION__, pPlayer->GetCharID());
		return;
	}

	WORD arCityList[MAX_LAND_CITY_NUM];
	BYTE byCityNum = 0;
	DWORD dwPatrolTotalTime = 0;

	// 单个领取
	if (wCityId)
	{
		arCityList[byCityNum++] = wCityId;
	}
	// 一键领取
	else
	{
		// 取可以领取的领取列表
		for (WORD i = 0; i < pPlayerLandInfo->wCityNum; i++)
		{
			LandCityData &tCityData = pPlayerLandInfo->arCityList[i];
		}
	}

	BYTE byTakeCityNum = 0;
	for (BYTE i = 0; i < byCityNum; ++i)
	{
		wCityId = arCityList[i];

		// 查找的领地
		LandCityData *pLand = GetCityInfo(pPlayer->GetCharID(), wCityId);
		if (!pLand)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地数据[RoleId:%d,LandId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId);
			return;
		}

		// 检测是否可以领取(木有巡逻英雄或者巡逻还木有结束) todo::暂定不用可领取判断
// 		if (!pLand->wPatrolHeroInfoId || pLand->dwStartPatrolTime)
// 			continue;

		// 取领地配置
		const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
		if (!pLandCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[LandId:%d]", __FUNCTION__, wCityId);
			return;
		}

		// 本次巡逻总时间
		dwPatrolTotalTime += GetPatrolTotalTime(pLand->byPatrolType, pLand->byTimeType);

		// 巡逻事件奖励
		for (BYTE i = 0; i < pLand->byPatrolEventNum; ++i)
		{
			LandPatrolEvent &tPatrolEvent = pLand->arPatrolEventList[i];
			const LandPatrolEvent_Config *pPatrolEventCfg = CConfigManager::Instance()->GetLandPatrolEvent(wCityId);
			if (!pPatrolEventCfg)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]找不到领地巡逻事件配置[RoleId:%d,CityId:%d]",
					__FUNCTION__, pPlayer->GetCharID(), wCityId);
				return;
			}
			const LandPatrolEvent_Config::EventData *pEventCfg = pPatrolEventCfg->GetEventData(tPatrolEvent.wEventId);
			if (!pEventCfg)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(ERROR_LEVEL, "[%s]找不到领地巡逻事件配置[RoleId:%d,CityId:%d, EventId:%d]",
					__FUNCTION__, pPlayer->GetCharID(), wCityId, tPatrolEvent.wEventId);
				return;
			}

			if (pEventCfg->dwObjId && pEventCfg->dwOjbNum)
			{
				DWORD dwOjbNum = tPatrolEvent.byIsDouble ? pEventCfg->dwOjbNum * 2 : pEventCfg->dwOjbNum;
				pPlayer->AddObject(pEventCfg->dwObjId, dwOjbNum, TRUE, FROM_LAND_PATROL_AWARD);
			}
		}

		// 暴动奖励
		for (BYTE i = 0; i < pLand->byRiotEventNum; ++i)
		{
			LandRiotEvent &tRiotEvent = pLand->arRiotEventList[i];
			if (tRiotEvent.dwSuppressTime)
			{
				pPlayer->ModifyProperty(ROLE_PRO_ENERGY, pLandCfg->nSuppressEnergy);
			}
		}

		// 碎片奖励
		const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pLand->wPatrolHeroInfoId);
		if (pComposeCfg)
		{
			pPlayer->AddObject(pComposeCfg->dwItemId, pLand->wAwardItemNum, TRUE, FROM_LAND_PATROL_AWARD);

			// 处理广播(获得整只英雄)
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
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄合成配置数据[HeroId:%d]",__FUNCTION__, pLand->wPatrolHeroInfoId);
		}

		// 记录已经领取过奖励
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
		MAPLOG(ERROR_LEVEL, "[%s]当前木有可领取奖励的领地", __FUNCTION__);
		return;
	}

	// 记录累积巡逻时间
	if (dwPatrolTotalTime)
		dwPatrolTotalTime = dwPatrolTotalTime / 3600;
	pPlayerLandInfo->dwPatrolTotalTime += dwPatrolTotalTime;

	MSG_S2C_LAND_GET_PATROL_AWARD_RESP msg;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// 日常任务
	pPlayer->OnUpdateDailyMission(19, dwPatrolTotalTime);

	// 成就事件
	pPlayer->OnUpdateAchievement(14, dwPatrolTotalTime);

	// 数据存储
	SaveDataToDB(pPlayer->GetRoleId());
}

// 领地升级(开启,提升)
void CLandManager::OnNetLandLevelUp(Player *pPlayer, WORD wCityId)
{
	WORD wFromMsgId = C2S_LAND_SKILL_LEVELUP_REQ;

	// 取领地配置
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// 查找的领地
	LandCityData *pLand = GetCityInfo(pPlayer->GetCharID(), wCityId);
	if (!pLand)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地数据[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// 是否已达等级上限
	if (pLand->bySkillLevel >= pLandCfg->vSkillList.size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领地技能等级已达上限[RoleId:%d,LandId:%d]",
			__FUNCTION__, pPlayer->GetCharID(), wCityId);
		return;
	}

	// 取玩家的领地信息
	PlayerLandInfo *pPlayerLandInfo = GetPlayerLandInfo(pPlayer->GetCharID());
	if (!pPlayerLandInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地信息[RoleId:%d]",
			__FUNCTION__, pPlayer->GetCharID());
		return;
	}

	const LandSkill &tLandSkill = pLandCfg->vSkillList[pLand->bySkillLevel];

	// 检测技能需要的巡逻时间
	if (tLandSkill.wNeedPatrolTime > pPlayerLandInfo->dwPatrolTotalTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领地技能需要的巡逻时间不足[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}
	
	// 检测消耗的钻石
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < tLandSkill.nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领地技能需要的钻石不足[LandId:%d]", __FUNCTION__, wCityId);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -tLandSkill.nCostDiamond, TRUE, FROM_LAND_LEVEL_UP);

	pLand->bySkillLevel++;

	SendPlayerLandData(pPlayer);

	MSG_S2C_LAND_SKILL_LEVELUP_RESP msg;
	msg.m_wCityId = wCityId;
	msg.m_bySkillLevel = pLand->bySkillLevel;
	pPlayer->SendPacket(&msg);

	// 数据存储
	SaveDataToDB(pPlayer->GetRoleId());
}

// 请求所有好友领地列表
void CLandManager::OnNetFriendLandList(Player *pPlayer)
{
	WORD wFromMsgId = C2S_FRIEND_LAND_LIST_REQ;

	CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
	if (!pFriendMgr)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到好友管理器", __FUNCTION__);
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
			MAPLOG(ERROR_LEVEL, "[%s]找不到好友数据[FriendId:%d]", __FUNCTION__, tFiendData.dwFriendId);
			continue;
		}

		FriendLandData &tLandData = msg.m_LandList[msg.m_byLandNum++];
		memset(&tLandData, 0, sizeof(tLandData));

		// 填充好友的角色信息
		tLandData.dwRoleId = pFriendBaseData->dwRoleId;
		tLandData.wLevel = pFriendBaseData->wLevel;
		tLandData.wHeadIcon = pFriendBaseData->wHeadIcon;
		strcpy_s(tLandData.szName, sizeof(tLandData.szName) - 1, pFriendBaseData->szRoleName);
		tLandData.dwLastLogoutTime = pFriendBaseData->dwOfflineTime;
	
		// 填充玩家的领地信息
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

// 请求玩家领地列表
void CLandManager::OnNetCityDetails(Player *pPlayer, DWORD dwTargetId, WORD wCityId)
{
	LandCityData* pCityData = GetCityInfo(dwTargetId, wCityId);
	if (!pCityData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_LAND_CITY_DETAILS_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的城池数据[RoleId:%d, CityId:%d]", 
			__FUNCTION__, dwTargetId, wCityId);
		return;
	}

	MSG_S2C_LAND_CITY_DETAILS_RESP msg;
	msg.m_dwPlayerId = dwTargetId;
	memcpy_s(&msg.m_tCityData, sizeof(LandCityData), pCityData, sizeof(LandCityData));
	pPlayer->SendPacket(&msg);
}

// 镇压暴动
void CLandManager::OnSuppressRiot(Player *pPlayer, DWORD dwFriendId, WORD wCityId)
{
	WORD wFromMsgId = C2S_LAND_SUPPRESS_RIOT_REQ;

	// 检测镇压次数上限
	if (pPlayer->GetProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES) >= pPlayer->GetVipValue(VIP_LAND_SUPPRESS_TIMES))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]今日镇压次数已达上限", __FUNCTION__);
		return;
	}

	// 取领地配置
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[CityId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// 查找好友的领地数据
	LandCityData *pLand = GetCityInfo(dwFriendId, wCityId);
	if (!pLand)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到好友的领地数据[FriendId:%d,CityId:%d]",
			__FUNCTION__, dwFriendId, wCityId);
		return;
	}

	// 是否在暴动中
	if (!pLand->byIsRiot)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]好友的领地木有在暴动中[FriendId:%d,CityId:%d]",
			__FUNCTION__, dwFriendId, wCityId);
		return;
	}

	// 检测好友当前是否有镇压事件
	if (!pLand->byRiotEventNum)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]当前没有镇压事件记录[CityId:%d]", __FUNCTION__, wCityId);
		return;
	}

	// 记录暴动镇压事件(更新好友的暴动事件数据) todo::此处逻辑有些不好理解,争取日后调整。。。
	LandRiotEvent &tRoitEvent = pLand->arRiotEventList[pLand->byRiotEventNum - 1];
	tRoitEvent.dwSuppressTime = time(NULL);
	strcpy_s(tRoitEvent.szFriendName, sizeof(tRoitEvent.szFriendName) - 1, pPlayer->GetRoleName());
	pLand->byIsRiot = false;

	pPlayer->ModifyProperty(ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES, 1, false);

	SendPlayerLandData(pPlayer, dwFriendId);

	// 帮好友镇压获得奖励
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pLandCfg->nHelpSuppressDiamond, TRUE, FROM_LAND_SUPPRESS_RIOT);

	MSG_S2C_LAND_SUPPRESS_RIOT_RESP msg;
	msg.m_dwFriendId = dwFriendId;
	msg.m_wCityId = wCityId;
	pPlayer->SendPacket(&msg);

	// 日常任务
	pPlayer->OnUpdateDailyMission(16);

	// 成就事件
	pPlayer->OnUpdateAchievement(15);

	// 数据存储
	SaveDataToDB(dwFriendId);
}

// 战斗开始
bool CLandManager::OnBattleStar(Player *pPlayer, WORD wCityId)
{
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	// 检测是否已打过了
	if (GetCityInfo(pPlayer->GetCharID(), wCityId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]此领地已打过[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	// 需要前置条件
	if (pLandCfg->wPreCityId)
	{
		// 检测前置条件是否已满足
		if (!GetCityInfo(pPlayer->GetCharID(), pLandCfg->wPreCityId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]前置领地还木有通过[RoleId:%d,LandId:%d,PreLandId:%d]",
				__FUNCTION__, pPlayer->GetCharID(), wCityId, pLandCfg->wPreCityId);
			return false;
		}
	}

	pPlayer->GetFubenManager()->SetBattleInfo(wCityId, 0, pLandCfg->wMapId);

	return true;
}

// 战斗结束
bool CLandManager::OnBattleOver(Player *pPlayer, WORD wCityId, BYTE byIsWin)
{
	// 只有胜利才处理
	if (byIsWin)
	{
		// 检测是否已打过(todo::出现这种情况表示逻辑上已经出现错误)
		if (GetCityInfo(pPlayer->GetCharID(), wCityId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]严重错误!!!此领地已打过[LandId:%d]", __FUNCTION__, wCityId);
			return false;
		}

		LandCityData tCityData;
		memset(&tCityData, 0, sizeof(tCityData));
		tCityData.wId = wCityId;
		AddCityInfo(pPlayer->GetCharID(), tCityData);
		SendPlayerLandData(pPlayer);

		// 数据存储
		SaveDataToDB(pPlayer->GetRoleId());
	}

	return true;
}
// 发送玩家领地数据
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
			// 巡逻结束 && 还木有领取 && 有奖励
			if (!tCityData.dwStartPatrolTime && !tCityData.byIsGetAward && tCityData.byPatrolEventNum)
			{
				msg.m_CityList[i].byIsCanGetReward = true;
			}
		}
		msg.m_byCityNum = pPlayerLandInfo->wCityNum;
	}

	pPlayer->SendPacket(&msg);
}

// 取红点状态
bool CLandManager::IsHaveRedPoint(DWORD dwRoleId)
{
	// 1.当有可进行寻访的领地时
	// 2.当有寻访完成，有可领取的寻访奖励未领取时，在下方显示红点
	PlayerLandInfo *pLandInfo = GetPlayerLandInfo(dwRoleId);
	if (!pLandInfo)
		return false;

	for (WORD i = 0; i < pLandInfo->wCityNum; ++i)
	{
		LandCityData &tCity = pLandInfo->arCityList[i];
		// 可巡逻
		if (!tCity.wPatrolHeroInfoId)
			return true;

		// 可领取
		if (!tCity.dwStartPatrolTime && !tCity.byIsGetAward)
			return true;
	}

	return false;
}

// 取玩家领地数量
WORD CLandManager::GetPlayerLandNum(DWORD dwRoleId)
{
	PlayerLandInfo *pLandInfo = GetPlayerLandInfo(dwRoleId);
	return pLandInfo ? pLandInfo->wCityNum : 0;
}

// 星临时接口(测试用)
PlayerLandInfo* CLandManager::GetPlayerLandInfoEx(DWORD dwRoleId)
{
	return GetPlayerLandInfo(dwRoleId);
}

void CLandManager::PatrolEvent_Test(DWORD dwRoleId, WORD wCityId)
{
	PatrolEventEx(dwRoleId, wCityId);
}

// 取玩家领地信息 
PlayerLandInfo* CLandManager::GetPlayerLandInfo(DWORD dwRoleId)
{
	LandListMap::iterator iter = m_PlayerLandList.find(dwRoleId);
	return iter != m_PlayerLandList.end() ? &iter->second : NULL;
}

// 取城池信息
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

// 添加领地信息
void CLandManager::AddCityInfo(DWORD dwRoleId, const LandCityData &tCityData)
{
	PlayerLandInfo* pPlayerLandInfo = GetPlayerLandInfo(dwRoleId);
	if (pPlayerLandInfo)
	{
		if (pPlayerLandInfo->wCityNum >= MAX_LAND_CITY_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]添加领地信息时列表已满!!!!!", __FUNCTION__);
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

// 巡逻事件
// void CLandManager::PatrolEvent(DWORD dwRoleId, WORD wCityId)
// {
// 	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
// 	if (!pLandCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[LandId:%d]", __FUNCTION__, wCityId);
// 		return;
// 	}
// 
// 	LandCityData *pLand = GetCityInfo(dwRoleId, wCityId);
// 	if (!pLand)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地数据[RoleId:%d,LandId:%d]",
// 			__FUNCTION__, dwRoleId, wCityId);
// 		return;
// 	}
// 
// 	// 随机事件ID
// 	WORD wEventId = RandEventId(wCityId);
// 	if (!wEventId)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]随机领地城池事件ID为空[LandId:%d]",__FUNCTION__, wCityId);
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
// 	// 随机双倍奖励
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
// 	// 随机暴乱事件
// 	if (!pLand->byIsRiot && g_Rand() <= g_GetConfigValue(100))
// 	{
// 		LandRiotEvent tRiotEvent;
// 		memset(&tRiotEvent, 0, sizeof(tRiotEvent));
// 		tRiotEvent.dwHappenTime = dwCurTime;
// 		pLand->AddRoitEvent(tRiotEvent);
// 		pLand->byIsRiot = true;
// 	}
// 
// 	// 最后一次处理随机碎片
// 	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(pLand->byPatrolType);
// 	if (!pPatrolCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[PatrolType:%d]", __FUNCTION__, pLand->byPatrolType);
// 		return;
// 	}
// 	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(pLand->byTimeType);
// 	if (!pTimeData)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[PatrolType:%d,TimeType:%d]",
// 			__FUNCTION__, pLand->byPatrolType, pLand->byTimeType);
// 		return;
// 	}
// 	// todo::看看是否需要加误差时间
// 	if (dwCurTime >= (pLand->dwStartPatrolTime + pTimeData->dwTotalTime))
// 	{
// 		pLand->wAwardItemNum = Rand::Instance()->irand(pTimeData->wItemNumMin, pTimeData->wItemNumMax);
// 		
// 		pLand->dwStartPatrolTime = 0; // 巡逻结束
// 		pLand->byIsRiot = false;
// 		MAPLOG(GUI_LEVEL, "[%s]巡逻结束[RoleId:%d,CityId:%d]", __FUNCTION__, dwRoleId,wCityId);
// 	}
// 
// 	pLand->dwLastHappenEventTime = dwCurTime;
// 	MAPLOG(GUI_LEVEL, "[%s]发生领地巡逻事件[CityId:%d]", __FUNCTION__, wCityId);
// }

bool CLandManager::PatrolEventEx(DWORD dwRoleId, WORD wCityId)
{
	const LandCity_Config *pLandCfg = CConfigManager::Instance()->GetLand(wCityId);
	if (!pLandCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到领地配置[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	LandCityData *pLand = GetCityInfo(dwRoleId, wCityId);
	if (!pLand)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的领地数据[RoleId:%d,LandId:%d]",
			__FUNCTION__, dwRoleId, wCityId);
		return false;
	}

	// 随机事件ID
	WORD wEventId = RandEventId(wCityId);
	if (!wEventId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]随机领地城池事件ID为空[LandId:%d]", __FUNCTION__, wCityId);
		return false;
	}

	DWORD dwCurTime = time(NULL);

	// 更新发生事件时间
	if (pLand->dwLastHappenEventTime)
		pLand->dwLastHappenEventTime += pLand->wIntervalTime;
	else
		pLand->dwLastHappenEventTime = dwCurTime;

	// 事件数据
	LandPatrolEvent tPatrolEvent;
	tPatrolEvent.wEventId = wEventId;
	tPatrolEvent.dwHappenTime = pLand->dwLastHappenEventTime;
	tPatrolEvent.byIsDouble = false;

	// 随机双倍奖励
	if (pLand->bySkillLevel && g_Rand(1, 100) <= pLandCfg->vSkillList[pLand->bySkillLevel - 1].wDoubleAwardRate)
		tPatrolEvent.byIsDouble = true;

	pLand->AddPatrolEvent(tPatrolEvent);

	// 随机暴乱事件
	if (!pLand->byIsRiot && g_Rand() <= g_GetConfigValue(100))
	{
		LandRiotEvent tRiotEvent;
		memset(&tRiotEvent, 0, sizeof(tRiotEvent));
		tRiotEvent.dwHappenTime = dwCurTime;
		pLand->AddRoitEvent(tRiotEvent);
		pLand->byIsRiot = true;
	}


	// 最后一次处理随机碎片
	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(pLand->byPatrolType);
	if (!pPatrolCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[PatrolType:%d]", __FUNCTION__, pLand->byPatrolType);
		return false;
	}
	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(pLand->byTimeType);
	if (!pTimeData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[PatrolType:%d,TimeType:%d]",
			__FUNCTION__, pLand->byPatrolType, pLand->byTimeType);
		return false;
	}
	// 因为一开始巡逻就算一次 todo::看看是否需要加误差时间 
	if (pLand->wIntervalTime * (pLand->byPatrolEventNum - 1) >= pTimeData->dwTotalTime)
	//if (dwCurTime >= (pLand->dwStartPatrolTime + pTimeData->dwTotalTime))
	{
		pLand->wAwardItemNum = Rand::Instance()->irand(pTimeData->wItemNumMin, pTimeData->wItemNumMax);

		pLand->dwStartPatrolTime = 0; // 巡逻结束
		pLand->byIsRiot = false;
		MAPLOG(GUI_LEVEL, "[%s]巡逻结束[RoleId:%d,CityId:%d]", __FUNCTION__, dwRoleId, wCityId);
	}

	//pLand->dwLastHappenEventTime = dwCurTime;
	MAPLOG(GUI_LEVEL, "[%s]发生领地巡逻事件[CityId:%d]", __FUNCTION__, wCityId);

	// 数据存储
	SaveDataToDB(dwRoleId);

	return true;
}

// 随机事件ID
WORD CLandManager::RandEventId(WORD wCityId)
{
	// 取领地奖励配置
	const LandPatrolEvent_Config *pPatrolEventCfg = CConfigManager::Instance()->GetLandPatrolEvent(wCityId);
	if (!pPatrolEventCfg || !pPatrolEventCfg->vEventList.size())
	{
	 	MAPLOG(ERROR_LEVEL, "[%s]找不到领地奖励配置[LandId:%d]", __FUNCTION__, wCityId);
	 	return 0;
	}

	// 计算总概率
	int nTotalRate = 0;
	for (size_t i = 0; i < pPatrolEventCfg->vEventList.size(); ++i)
	{
	 	nTotalRate += pPatrolEventCfg->vEventList[i].wEventRate;
	}

	// 随机一个值
	int nRandValue = Rand::Instance()->irand(1, nTotalRate);

	// 计算那个事件被命中
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

// 取巡逻总时间
DWORD CLandManager::GetPatrolTotalTime(BYTE byPatrolType, BYTE byTimeType)
{
	const LandPatrol_Config *pPatrolCfg = CConfigManager::Instance()->GetLandPatrol(byPatrolType);
	if (!pPatrolCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[PatrolType:%d]", __FUNCTION__, byPatrolType);
		return 0;
	}
	
	const LandPatrol_Config::TimeData *pTimeData = pPatrolCfg->GetTimeData(byTimeType);
	if (!pTimeData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到巡逻配置数据[PatrolType:%d,TimeType:%d]",__FUNCTION__, byPatrolType, byTimeType);
		return 0;
	}

	return pTimeData->dwTotalTime;
}