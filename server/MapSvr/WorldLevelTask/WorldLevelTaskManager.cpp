#include "stdafx.h"
#include "WorldLevelTaskManager.h"
#include "..\Player.h"
#include "..\MapServer.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"

CWorldLevelTaskMananger::CWorldLevelTaskMananger()
{

}

CWorldLevelTaskMananger::~CWorldLevelTaskMananger()
{

}

void CWorldLevelTaskMananger::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_PlayerWorldLevelTaskData = &pPlayer->GetRoleFullInfo().tSecondInfo.tRoleWorldLevelTaskData;
}

void CWorldLevelTaskMananger::OnNewDay()
{
	UpdateTask();
}

// 请求世界等级任务数据
void CWorldLevelTaskMananger::OnNetWorldLevelTaskData()
{
	// 还没初始化则更新
	if (!m_PlayerWorldLevelTaskData->wTaskEventNum)
		UpdateTask();

	SendWorldLevelTaskData();
}

// 领取任务奖励
void CWorldLevelTaskMananger::OnNetTakeTaskReward(WORD wTaskId)
{
	// 取任务数据
	WorldLevelTaskEvent *pTaskEvent = GetTaskEvent(wTaskId);
	if (!pTaskEvent)
	{
		MAPLOG(ERROR_LEVEL, "玩家没有此世界等级任务[TaskId:%d]", wTaskId);
		return;
	}

	// 检测是否已领取
	if (pTaskEvent->wIsAlreadyTake)
	{
		MAPLOG(ERROR_LEVEL, "玩家已领取过奖励[TaskId:%d]", wTaskId);
		return;
	}

	// 取任务配置
	const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(wTaskId);
	if (!pTaskEventCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到世界等级任务事件配置[TaskId:%d]", wTaskId);
		return;
	}

	// 检测是否任务已完成
	if (pTaskEvent->nContext < pTaskEventCfg->nX)
	{
		MAPLOG(ERROR_LEVEL, "世界等级任务还没完成[TaskId:%d]", wTaskId);
		return;
	}

	WORD wOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());
	const WorldTaskLevel_Config *pTaskLevelCfg = g_pCfgMgr->GetWorldTaskLevelData(wOpenServerDays);
	if (!pTaskLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到世界等级任务配置数据[TaskId:%d]", wTaskId);
		return;
	}

	// 取配置
	const WorldLevelTaskIdList_Config *pTasIdkListCfg = g_pCfgMgr->GetWorldLevelTaskIdListByTaskId(wTaskId);
	if (!pTasIdkListCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到世界等级任务ID列表配置[TaskId:%d]", wTaskId);
		return;
	}

	int nRate = g_GetConfigValue(279);

	// 因为要成为分子,要保证不能为0的情况
	nRate = max(nRate, 1);

	// 发奖励
	// 领取经验 = math.min(math.max(世界等级 - 任务等级, 0), 系数) / 系数 * 任务经验
	// 注：初始等级越高，经验奖励越少
	double fTemp = min(max(int(pTaskLevelCfg->wWorldLevel - pTasIdkListCfg->wTaskLevel), 0), nRate);
	int nExp = fTemp / nRate * pTasIdkListCfg->nExp;
	m_pPlayer->AddExp(nExp);

	// 道具奖励数量 = math.floor((1 + math.min(math.max(任务等级 - 世界等级 + 系数, 0), 系数) / 系数) * 任务道具数量)
	// 注：初始等级越高，道具奖励越多	
	for (int i = 0; i < pTaskEventCfg->vRewardList.size(); ++i)
	{
		Item item = pTaskEventCfg->vRewardList[i];
		fTemp = min(max(pTasIdkListCfg->wTaskLevel - pTaskLevelCfg->wWorldLevel + nRate, 0), nRate);
		int nItemNum = (1 + fTemp / nRate) * item.dwNum;
		item.dwNum = nItemNum;
		m_pPlayer->AddObjectData(&item, TRUE, FROM_WORLD_LEVEL_TASK_REWARD);
	}

	// 记录已领取过奖励
	pTaskEvent->wIsAlreadyTake = true;

	SendWorldLevelTaskData();

	// 响应
	MSG_S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP msg;
	msg.m_wTaskId = wTaskId;
	m_pPlayer->SendPacket(&msg);
}

// 领取最终奖励
void CWorldLevelTaskMananger::OnNetTakeFinalReward()
{
	// 检测是否已领取奖励
	if (m_PlayerWorldLevelTaskData->wIsAlreadyTakeFinalReward)
	{
		MAPLOG(ERROR_LEVEL, "玩家已领取过世界等级任务最终奖励");
		return;
	}

	// 检测是否所有的任务都已完成
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		WorldLevelTaskEvent &tTaskEvent = m_PlayerWorldLevelTaskData->arTaskEventList[i];
		const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(tTaskEvent.wTaskId);
		if (!pTaskEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "找不到世界等级任务事件配置[TaskId:%d]", tTaskEvent.wTaskId);
			return;
		}

		if (tTaskEvent.nContext < pTaskEventCfg->nX)
		{
			MAPLOG(ERROR_LEVEL, "世界等级任务事件还没完成[TaskId:%d, Cur:%d, Need:%d]", 
				tTaskEvent.wTaskId, tTaskEvent.nContext, pTaskEventCfg->nX);
			return;
		}
	}

	WORD wOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	const WorldTaskLevel_Config *pTaskLevelCfg = g_pCfgMgr->GetWorldTaskLevelData(wOpenServerDays);
	if (!pTaskLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到世界任务等级数据[OpenServerDays:%d]", wOpenServerDays);
		return;
	}

	WORD wInitLevel = m_pPlayer->GetDailyInitLevel();
	map<WORD, vector<Item>>::const_iterator iter = pTaskLevelCfg->vRewardList.find(wInitLevel);
	if (iter == pTaskLevelCfg->vRewardList.end())
	{
		MAPLOG(ERROR_LEVEL, "找不到世界任务最终奖励配置[OpenServerDays:%d,RoleInitLevel:%d]", 
			wOpenServerDays, wInitLevel);
		return;
	}

	// 发奖励
	m_pPlayer->AddObjectList(iter->second, TRUE, FROM_WORLD_TASK_FINAL_REWARD);

	// 记录已领取
	m_PlayerWorldLevelTaskData->wIsAlreadyTakeFinalReward = true;

	// 响应
	MSG_S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 更新任务事件
void CWorldLevelTaskMananger::UpdateTaskEvent(WORD wTaskType, int nValue, bool isAdd)
{
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		WorldLevelTaskEvent &tTaskEvent = m_PlayerWorldLevelTaskData->arTaskEventList[i];
		if (tTaskEvent.wTaskType == wTaskType)
		{ 
 			// 已领取
			if (tTaskEvent.wIsAlreadyTake)
 				continue;
 
 			if (isAdd)
				tTaskEvent.nContext += nValue;
 			else
				tTaskEvent.nContext = nValue;
 
			const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(tTaskEvent.wTaskId);
 			if (!pTaskEventCfg)
 			{
				MAPLOG(ERROR_LEVEL, "找不到世界等级任务事件配置[TaskId:%d]", tTaskEvent.wTaskId);
 				continue;
 			}
 
 			// 数据纠正
			tTaskEvent.nContext = min(tTaskEvent.nContext, pTaskEventCfg->nX);
		}
	}
}

// 发送等级任务数据
void CWorldLevelTaskMananger::SendWorldLevelTaskData()
{
	MSG_S2C_WORLD_LEVEL_TASK_DATA_RESP msg;
	msg.m_Data = *m_PlayerWorldLevelTaskData;

	// 特殊逻辑,为了方便客户端操作把任务的等级也发过去
// 	for (WORD i = 0; i < msg.m_Data.wTaskEventNum; ++i)
// 	{
// 		WorldLevelTaskEvent &tTaskEvent = msg.m_Data.arTaskEventList[i];
// 		const WorldLevelTaskIdList_Config *pCfg = g_pCfgMgr->GetWorldLevelTaskIdListByTaskId(tTaskEvent.wTaskId);
// 		tTaskEvent.nContext = pCfg ? pCfg->wTaskLevel : 0;
// 	}

	m_pPlayer->SendPacket(&msg);
}

// 更新任务
void CWorldLevelTaskMananger::UpdateTask()
{
	// 重置任务
	m_PlayerWorldLevelTaskData->wIsAlreadyTakeFinalReward = false;
	m_PlayerWorldLevelTaskData->wTaskEventNum = 0;

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	WORD wOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	const WorldTaskLevel_Config *pTaskLevelCfg = g_pCfgMgr->GetWorldTaskLevelData(wOpenServerDays);
	if (!pTaskLevelCfg)
		return;

	// 计算任务等级
	// 1.等级段下限 = 初始等级
	// 2.等级段上限 = 初始等级 + math.min(math.max(世界等级 - 初始等级, 5), 9)
	WORD wTaskLevelMin = tBaseInfo.wDailyInitLevel;
	WORD wTaskLevelMax = wTaskLevelMin + min(max(int(pTaskLevelCfg->wWorldLevel - wTaskLevelMin), 5), 9);

	for (WORD i = wTaskLevelMin; i <= wTaskLevelMax; ++i)
	{
		// 取配置
		const WorldLevelTaskIdList_Config *pTasIdkListCfg = g_pCfgMgr->GetWorldLevelTaskIdList(i);
		if (!pTasIdkListCfg)
		{
			MAPLOG(ERROR_LEVEL, "找不到世界等级任务ID列表配置[TaskLevel:%d]", i);
			return;
		}
		// 检测列表是否为空
		if (!pTasIdkListCfg->vTaskIdList.size())
		{
			MAPLOG(ERROR_LEVEL, "世界等级任务ID配置列表为空[TaskLevel:%d]", i);
			return;
		}

		// 随机任务ID
		WORD wTaskId = pTasIdkListCfg->vTaskIdList[g_Rand(0, pTasIdkListCfg->vTaskIdList.size()-1)];

		const WorldLevelTaskEvent_Config *pTastEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(wTaskId);
		if (!pTastEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "找不到世界等级任务事件配置[TaskId:%d]", wTaskId);
			return;
		}

		WorldLevelTaskEvent tTaskEvent;
		tTaskEvent.wTaskId = wTaskId;
		tTaskEvent.nContext = 0;
		tTaskEvent.wIsAlreadyTake = false;
		tTaskEvent.wTaskType = pTastEventCfg->wTaskType;
		tTaskEvent.wTaskLevel = pTasIdkListCfg->wTaskLevel;
		AddTaskEvent(tTaskEvent);
	}
}

// 是否有红点
bool CWorldLevelTaskMananger::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		WorldLevelTaskEvent &tTaskEvent = m_PlayerWorldLevelTaskData->arTaskEventList[i];

		// 已领取
		if (tTaskEvent.wIsAlreadyTake)
			continue;

		// 有奖励可领取
		const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(tTaskEvent.wTaskId);
		if (pTaskEventCfg && tTaskEvent.nContext >= pTaskEventCfg->nX)
			return true;
	}

	return false;
}

// 添加任务事件
void CWorldLevelTaskMananger::AddTaskEvent(WorldLevelTaskEvent &tTaskEvent)
{
	if (m_PlayerWorldLevelTaskData->wTaskEventNum >= MAX_DAILY_WORLD_LEVEL_TASK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "任务事件列表已满[MaxNum:%d]", MAX_DAILY_WORLD_LEVEL_TASK_NUM);
		return;
	}

// 	WorldLevelTaskEvent tTaskEvent;
// 	tTaskEvent.wTaskId = wTaskId;
// 	tTaskEvent.nContext = 0;
// 	tTaskEvent.wIsAlreadyTake = false;
	m_PlayerWorldLevelTaskData->arTaskEventList[m_PlayerWorldLevelTaskData->wTaskEventNum++] = tTaskEvent;
}

// 取任务事件
WorldLevelTaskEvent* CWorldLevelTaskMananger::GetTaskEvent(WORD wTaskId)
{
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		if (m_PlayerWorldLevelTaskData->arTaskEventList[i].wTaskId == wTaskId)
			return &m_PlayerWorldLevelTaskData->arTaskEventList[i];
	}
	return NULL;
}
