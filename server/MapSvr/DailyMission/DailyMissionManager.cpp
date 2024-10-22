#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "DailyMissionManager.h"


CDailyMissionManager::CDailyMissionManager()
{

}

CDailyMissionManager::~CDailyMissionManager()
{

}

void CDailyMissionManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pDailyMissionInfo = &pPlayer->GetRoleFullInfo().tDailyMissionInfo;
}

// 重置任务数据
void CDailyMissionManager::OnNewDay()
{
	memset(m_pDailyMissionInfo, 0, sizeof(PlayerDailyMissionData));
}

// 请求任务列表
void CDailyMissionManager::OnNetMissionList()
{
	SendMissionList();
}

// 领取任务奖励
void CDailyMissionManager::OnNetMissionAward(WORD wMissionId)
{
	// 取成就配置
	const DailyMission_Config *pMissionCfg = CConfigManager::Instance()->GetDailyMission(wMissionId);
	if (!pMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到日常任务配置数据![MissionId:%d]", wMissionId);
		return;
	}

	//  玩家成就数据
	DailyMissionData *pMissionData = GetMissionData(wMissionId);
	if (!pMissionData)
	{
		MAPLOG(ERROR_LEVEL, "找不到玩家的日常任务数据![MissionId:%d]", wMissionId);
		return;
	}

	// 检测是否已领取
	if (pMissionData->byIsGetAward)
	{
		MAPLOG(ERROR_LEVEL, "日常任务奖励已领取![MissionId:%d]", wMissionId);
		return;
	}

	// 检测是否可领取
	if (pMissionData->dwValue < pMissionCfg->dwValue)
	{
		MAPLOG(ERROR_LEVEL, "日常任务还没完成不可领取![MissionId:%d]", wMissionId);
		return;
	}

	// 奖励
	m_pPlayer->AddObjectList(pMissionCfg->vRewardItemList, TRUE, FROM_DAILY_MISSION_REWARD);

	// 获得积分
	m_pDailyMissionInfo->wPoint += pMissionCfg->wPoint;

	// 获得经验值
	m_pPlayer->AddExp(pMissionCfg->dwBaseExp * m_pPlayer->GetDailyInitLevel());

	// 记录已领取奖励
	pMissionData->byIsGetAward = true;

	SendMissionData(pMissionData);

 	MSG_S2C_DAILY_MISSION_AWARD_RESP msg;
	msg.m_wMissionId = wMissionId;
 	m_pPlayer->SendPacket(&msg);
}

// 积分奖励
void CDailyMissionManager::OnNetPointAward(WORD wPoint)
{
	// 检测是否已领取过
	if (CheckPointAward(wPoint))
	{
		MAPLOG(ERROR_LEVEL, "日常任务积分奖励已领取过![Point:%d]", wPoint);
		return;
	}

	const RoleLvDailyMission_Config *pMissonLvCfg = CConfigManager::Instance()->GetRoleLvDailyMission(m_pPlayer->GetLevel());
	if (!pMissonLvCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到等级日常任务配置![Level:%d]", m_pPlayer->GetLevel());
		return;
	}

	const RoleLvDailyMission_Config::PointData *pPointCfg = pMissonLvCfg->GetPointData(wPoint);
	if (!pPointCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到日常任务积分配置![score:%d]", wPoint);
		return;
	}

	m_pPlayer->AddObject(pPointCfg->wRewardItemId, TRUE, FROM_DAILY_MISSION_SCORE_REWARD);

	AddPointAwardRecord(wPoint);

	MSG_S2C_DAILY_MISSION_POINT_AWARD_RESP msg;
	msg.m_wPoint = wPoint;
	m_pPlayer->SendPacket(&msg);
}

// 发送任务列表
void CDailyMissionManager::SendMissionList()
{
	MSG_S2C_DAILY_MISSION_LIST_RESP msg;
	msg.m_DailyMission = *m_pDailyMissionInfo;
	m_pPlayer->SendPacket(&msg);
}

// 发送任务数据
void CDailyMissionManager::SendMissionData(const DailyMissionData *pData)
{
	MSG_S2C_DAILY_MISSION_DATA_NOTIFY msg;
	msg.m_DailyMissionData = *pData;
	m_pPlayer->SendPacket(&msg);
}

// 更新任务
void CDailyMissionManager::DoUpdateMission(WORD wMissionId, int nValue, bool bAdd)
{
	const DailyMission_Config *pMissionCfg = CConfigManager::Instance()->GetDailyMission(wMissionId);
	if (!pMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到日常任务配置数据![MissionId:%d]", wMissionId);
		return;
	}

	//  玩家成就数据
	DailyMissionData *pMissionData = GetMissionData(wMissionId);
	if (!pMissionData)
	{
		DailyMissionData tData;
		tData.wMissionId = wMissionId;
		tData.dwValue = 0;
		tData.byIsGetAward = false;
		AddMissionData(tData);
		pMissionData = GetMissionData(wMissionId);
		if (!pMissionData)
			return;
	}

	// 任务已完成
	if (pMissionData->dwValue >= pMissionCfg->dwValue)
		return;

	// 添加值
	if (bAdd)
	{
		pMissionData->dwValue += nValue;

		// 防止超过上限
		pMissionData->dwValue = min(pMissionData->dwValue, pMissionCfg->dwValue);
	}
	// 设置值
	else
	{		// 使用最佳记录
		if (nValue <= pMissionData->dwValue)
			return;

		pMissionData->dwValue = nValue;
	}

	SendMissionData(pMissionData);

	// 角色信息更新
	m_pPlayer->RoleInfoUpdate();
}

// 更新任务
void CDailyMissionManager::OnUpdateMission(WORD wType, int nValue, bool bAdd)
{
	// 取等级对应的任务列表
	WORD wInitTeamLv = m_pPlayer->GetProperty(ROLE_PRO_DAILY_INIT_LEVEL);
	const RoleLvDailyMission_Config *pRoleLvMissionCfg = CConfigManager::Instance()->GetRoleLvDailyMission(wInitTeamLv);
	if (!pRoleLvMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "取不到战队等级对应的日常任务配置![TeamLevel:%d]", wInitTeamLv);
		return;
	}

	const vector<WORD> *pMissionList = CConfigManager::Instance()->GetTypeDailyMissionList(wType);
	if (!pMissionList)
	{
		MAPLOG(ERROR_LEVEL, "取不到日常类型的任务列表![type:%d]", wType);
		return;
	}

	for (size_t i = 0; i < pMissionList->size(); ++i)
	{
		WORD wMissionId = pMissionList->at(i);
		if (pRoleLvMissionCfg->IsMission(wMissionId))
		{
			DoUpdateMission(wMissionId, nValue, bAdd);
			break;
		}
	}
}

// 是否有红点
bool CDailyMissionManager::IsHaveRedPoint()
{
	// 检测开放等级
	if (m_pPlayer->GetLevel() < g_pCfgMgr->GetSystemOpenLevel(SYSTEM_DAILY_MISSION))
		return false;

	// 取等级对应的任务列表
	WORD wInitRoleLevel = m_pPlayer->GetProperty(ROLE_PRO_DAILY_INIT_LEVEL);
	const RoleLvDailyMission_Config *pRoleLvMissionCfg = CConfigManager::Instance()->GetRoleLvDailyMission(wInitRoleLevel);
	if (!pRoleLvMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到战队等级对应的日常任务配置![TeamLevel:%d]", __FUNCTION__, wInitRoleLevel);
		return false;
	}

	// 当日常任务中有已完成的任务，可领取任务奖励时
	for (WORD i = 0; i < m_pDailyMissionInfo->wDailyMissionNum; ++i)
	{
		DailyMissionData *pMssionData = &m_pDailyMissionInfo->arDailyMissionList[i];
		// 已经领取过
		if (pMssionData->byIsGetAward)
			continue;

		const DailyMission_Config *pMissionCfg = g_pCfgMgr->GetDailyMission(pMssionData->wMissionId);
		if (!pMissionCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到日常任务配置数据![MissionId:%d]", __FUNCTION__, pMssionData->wMissionId);
			return false;
		}

		// 已达可领取条件(目标值,角色等级)
		if (pMssionData->dwValue >= pMissionCfg->dwValue && m_pPlayer->GetLevel() >= pMissionCfg->wNeedRoleLv)
			return true;
	}

	// 当日常任务中,有可领取的积分宝箱奖励时
	for(size_t i = 0; i < pRoleLvMissionCfg->vPointList.size(); ++i)
	{
		const RoleLvDailyMission_Config::PointData *pPointDataCfg = &pRoleLvMissionCfg->vPointList[i];
		// 积分已满足,还木有领取过
		if (m_pDailyMissionInfo->wPoint >= pPointDataCfg->wPoint && !CheckPointAward(pPointDataCfg->wPoint))
			return true;
	}

	return false;
}

// 取任务数据
DailyMissionData* CDailyMissionManager::GetMissionData(WORD wMissionId)
{
	for (WORD i = 0; i < m_pDailyMissionInfo->wDailyMissionNum; ++i)
	{
		if (m_pDailyMissionInfo->arDailyMissionList[i].wMissionId == wMissionId)
		{
			return &m_pDailyMissionInfo->arDailyMissionList[i];
		}
	}
	return NULL;
}

// 添加任务数据
void CDailyMissionManager::AddMissionData(const DailyMissionData &tMission)
{
	if (m_pDailyMissionInfo->wDailyMissionNum >= MAX_DAILY_MISSION_NUM)
	{
		MAPLOG(ERROR_LEVEL, "日常任务列表已满!");
		return;
	}

	m_pDailyMissionInfo->arDailyMissionList[m_pDailyMissionInfo->wDailyMissionNum++] = tMission;
}

// 检测是否已领取过
bool CDailyMissionManager::CheckPointAward(WORD wPoint)
{
	for (WORD i = 0; i < m_pDailyMissionInfo->wPointAwardRecordNum; ++i)
	{
		if (m_pDailyMissionInfo->arPointAwardRecord[i] == wPoint)
		{
			return true;
		}
	}
	return false;
}

// 添加积分奖励数据
void CDailyMissionManager::AddPointAwardRecord(WORD wPoint)
{
	if (m_pDailyMissionInfo->wPointAwardRecordNum >= MAX_DAILY_MISSION_POINT_AWARD)
	{
		MAPLOG(ERROR_LEVEL, "日常任务积分奖励列表已满!");
		return;
	}

	m_pDailyMissionInfo->arPointAwardRecord[m_pDailyMissionInfo->wPointAwardRecordNum++] = wPoint;
}