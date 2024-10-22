#include "stdafx.h"
#include "EndlessLandManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\AccountInfo.h"
#include "..\Rank\RankManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Activity\ActivityManager.h"

CEndlessLandManager::CEndlessLandManager()
{
	m_bIsInBattle = false;
}

CEndlessLandManager::~CEndlessLandManager()
{
		
}

void CEndlessLandManager::Init(Player *pPlayer)
{
	m_bIsInBattle = false;
	m_pPlayer = pPlayer;
	m_pEndlessLandData = &pPlayer->GetRoleFullInfo().tSecondInfo.tEndlessLandData;
}

// 无心之地数据请求
void CEndlessLandManager::OnNetEndlessData()
{
	SendEndlessLandData();
}

// 进入战场
void CEndlessLandManager::OnNetEnter()
{
	// 当前挑战次数
	WORD wCurBattleTimes = m_pEndlessLandData->wTodayBattleTimes + 1;

	// 检测每日可挑战次数
	WORD wDailyBattleTimes = m_pPlayer->GetVipValue(VIP_ENDLESS_LAND_DAILY_BATTLE_TIMES);
	if (wCurBattleTimes > wDailyBattleTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]今日挑战次数已达上限[MaxBattleTimes:%d]", __FUNCTION__, wDailyBattleTimes);
		return;
	}

	// 取配置
	const EndlessLandBattleCost_Config *pCostCfg = g_pCfgMgr->GetEndlessLandBattleCost(wCurBattleTimes);
	if (!pCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到无尽之地挑战消耗配置[BattleTimes:%d]", __FUNCTION__, wCurBattleTimes);
		return;
	}

	// 检测消耗
	if (m_pPlayer->GetItemNum(pCostCfg->tCostItem.dwId) < pCostCfg->tCostItem.dwNum && 
		m_pPlayer->GetProperty(pCostCfg->tCostCurrency.wType) < pCostCfg->tCostCurrency.nValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地挑战消耗不足,不能挑战[BattleTimes:%d]", __FUNCTION__, wCurBattleTimes);
		return;
	}

	// 重置至可跳到的波数
	if (m_pEndlessLandData->wHistoryWaveRecord)
	{
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(m_pEndlessLandData->wHistoryWaveRecord);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_ENTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到波数配置[wave:%d]", __FUNCTION__, m_pEndlessLandData->wHistoryWaveRecord);
			return;
		}

		m_pEndlessLandData->wCurWave = pWaveCfg->wInitWave;
	}

	// 连杀数量重新开始计算
	m_pEndlessLandData->wCurContinueKillNum = 0;

	// 每日首次挑战有奖励 todo::已经调整为每次挑战都有奖励
	for (WORD i = 1; i < m_pEndlessLandData->wCurWave; ++i)
	{
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(i);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_ENTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到波数配置[wave:%d]", __FUNCTION__, i);
			return;
		}

		// 处理奖励
		m_pPlayer->ModifyProperty(ROLE_PRO_ENDLESS_STONE, pWaveCfg->nEndlessStone, TRUE, FROM_ENDLESS_ENTER_BATTLE);
		m_pPlayer->AddObjectList(pWaveCfg->vRewardItemList, TRUE, FROM_ENDLESS_ENTER_BATTLE);

		// 波掉落
		for (int k = 0; k < pWaveCfg->vDropDataList.size(); ++k)
		{
			const DropData &sDropData = pWaveCfg->vDropDataList[k];
			if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < sDropData.wRate)
			{
				m_pPlayer->AddObject(sDropData.dwDropObjectId, sDropData.wDropObjectNum, TRUE, FROM_ENDLESS_ENTER_BATTLE);
			}
		}

		// 记录连杀数量
		//m_pEndlessLandData->wCurContinueKillNum += pWaveCfg->wMonsterNum;
	}

	// 计算连杀数量
	for (WORD i = 1; i < m_pEndlessLandData->wCurWave; ++i)
	{
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(i);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_ENTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到波数配置[wave:%d]", __FUNCTION__, i);
			return;
		}
		// 记录连杀数量
		m_pEndlessLandData->wCurContinueKillNum += pWaveCfg->wMonsterNum;
	}

	// 无尽深渊击杀%s只怪物
	m_pPlayer->OnUpdateSevenDayTarget(24, m_pEndlessLandData->wCurContinueKillNum, true);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	// 节日活跃目标
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	// 处理消耗(物品)
	if (m_pPlayer->GetItemNum(pCostCfg->tCostItem.dwId) >= pCostCfg->tCostItem.dwNum)
	{
		m_pPlayer->RemoveItem(pCostCfg->tCostItem.dwId, pCostCfg->tCostItem.dwNum, FROM_ENDLESS_ENTER_BATTLE);
	}
	// 处理消耗(货币)
	else
	{
		m_pPlayer->ModifyProperty(pCostCfg->tCostCurrency.wType, -pCostCfg->tCostCurrency.nValue, TRUE, FROM_ENDLESS_ENTER_BATTLE);
	}

	//m_pEndlessLandData->wTodayWaveRecord = m_pEndlessLandData->wCurWave - 1;

	m_pEndlessLandData->wIsFirstBattle = false;
	m_pEndlessLandData->wTodayBattleTimes = wCurBattleTimes;

	m_bIsInBattle = true;

	// 同步数据
	SendEndlessLandData();

	// 响应
	MSG_S2C_ENDLESS_LAND_ENTER_RESP msg;
	m_pPlayer->SendPacket(&msg);
 
	// 日常任务(参与无尽深渊Ｘ次)
	m_pPlayer->OnUpdateDailyMission(23);

	// 节日活动(累计挑战无尽深渊次数)
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 17, 1, true);

	// 节日活跃目标(累计挑战无尽深渊次数)
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 17, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 17, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 17, 1, true);
}

// 开始战斗
bool CEndlessLandManager::OnNetBattleStart()
{
	if (!m_bIsInBattle)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_START_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地还没进入战斗状态[roleid:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return false;
	}

// 	MSG_S2C_ENDLESS_LAND_BATTLE_START_RESP msg;
// 	m_pPlayer->SendPacket(&msg);

	return true;
}

// 开始结束
bool CEndlessLandManager::OnNetBattleOver(BYTE byIsWin, WORD wWave)
{
	if (!m_bIsInBattle)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地还没进入战斗状态[roleid:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return false;
	}

	// 检测波数是否有问题
	if (wWave != m_pEndlessLandData->wCurWave)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地挑战波数出错[roleid:%d,wave:%d]", __FUNCTION__, m_pPlayer->GetRoleId(), wWave);
		return false;
	}

	int nMultiple = CActivityManager::GetLimitProductMultiple(BATTLE_ENDLESS_LAND);

	WORD wCurRewardWave = 0; // m_pEndlessLandData->wCurWave;

	if (byIsWin)
	{
		// 处理奖励
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(m_pEndlessLandData->wCurWave);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到波数配置[wave:%d]", __FUNCTION__, m_pEndlessLandData->wCurWave);
			return false;
		}

		// 检测击杀的怪是否足够
		if (m_pEndlessLandData->wCurWaveKillNum < pWaveCfg->wMonsterNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]击杀怪物的数量不够[wave:%d,cur_kill_num:%d,need_kill_num:%d]", __FUNCTION__,
				m_pEndlessLandData->wCurWave, m_pEndlessLandData->wCurWaveKillNum, pWaveCfg->wMonsterNum);
			return false;
		}

		// 限制每日只能拿一次奖励
		//if (m_pEndlessLandData->wCurWave > m_pEndlessLandData->wTodayWaveRecord)
		//{
			// 处理奖励
			m_pPlayer->ModifyProperty(ROLE_PRO_ENDLESS_STONE, pWaveCfg->nEndlessStone * nMultiple, TRUE, FROM_ENDLESS_LAND_BATTLE_OVER);
			m_pPlayer->AddObjectList(pWaveCfg->vRewardItemList, TRUE, FROM_ENDLESS_LAND_BATTLE_OVER);

			// 波掉落
			for (int k = 0; k < pWaveCfg->vDropDataList.size(); ++k)
			{
				const DropData &sDropData = pWaveCfg->vDropDataList[k];
				if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < sDropData.wRate)
				{
					m_pPlayer->AddObject(sDropData.dwDropObjectId, sDropData.wDropObjectNum, TRUE, FROM_ENDLESS_LAND_BATTLE_OVER);
				}
			}

			wCurRewardWave = m_pEndlessLandData->wCurWave;
		//}

		m_pEndlessLandData->wTodayWaveRecord = max(m_pEndlessLandData->wTodayWaveRecord, m_pEndlessLandData->wCurWave);
		m_pEndlessLandData->wHistoryWaveRecord = max(m_pEndlessLandData->wHistoryWaveRecord, m_pEndlessLandData->wCurWave);

		// 往下一波
		m_pEndlessLandData->wCurWave++;

		// 下一波的杀怪数量重置
		m_pEndlessLandData->wCurWaveKillNum = 0;

		// 成就事件(深渊最高杀怪X只)
		m_pPlayer->OnUpdateAchievement(25, m_pEndlessLandData->wHistoryWaveRecord, false);
	}

	// 响应
	MSG_S2C_ENDLESS_LAND_BATTLE_OVER_RESP msg;
	msg.m_byIsWin = byIsWin;
	msg.m_wRewardWave = wCurRewardWave;
	m_pPlayer->SendPacket(&msg);

	return true;
}

// 杀怪
void CEndlessLandManager::OnNetKillMonster(BYTE byIndex, DWORD dwMonsterId)
{
	if (!m_bIsInBattle)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地还没进入战斗状态[roleid:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_ENDLESS_LAND_KILL_MONSTER_RESP msg;
	msg.m_byIndex = byIndex;
	msg.m_dwMonsterId = dwMonsterId;
	msg.m_wDropItemNum = 0;

	// 处理今日挑战首次奖励
	if (m_pEndlessLandData->wCurWave > m_pEndlessLandData->wTodayWaveRecord)
	{
		// 取配置
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(m_pEndlessLandData->wCurWave);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到波数配置[wave:%d]", __FUNCTION__, m_pEndlessLandData->wCurWave);
			return;
		}

		// 检测下标的合法性
		if (byIndex >= pWaveCfg->vMonsterList.size())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]怪物下标越界[index:%d]", __FUNCTION__, byIndex);
			return;
		}

		DWORD dwMonsterId = pWaveCfg->vMonsterList[byIndex];
		const Monster_Config *pMonsterCfg = g_pCfgMgr->GetMonster(dwMonsterId);
		if (!pMonsterCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]找不到怪物配置数据[index:%d]", __FUNCTION__, byIndex);
			return;
		}

		// 处理怪物掉落
		for (int k = 0; k < pMonsterCfg->vDropList.size(); ++k)
		{
			const DropData &sDropData = pMonsterCfg->vDropList[k];
			if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < sDropData.wRate)
			{
				m_pPlayer->AddObject(sDropData.dwDropObjectId, sDropData.wDropObjectNum, TRUE, FROM_ENDLESS_LAND_KILL_MONSTER);
				
				if (msg.m_wDropItemNum < MAX_MAP_DROP_NUM)
				{
					MonsterDropData &tDropData = msg.m_DropItemList[msg.m_wDropItemNum++];
					tDropData.dwObjId = sDropData.dwDropObjectId;
					tDropData.wObjNum = sDropData.wDropObjectNum;
				}
			}
		}
	}

	// 击杀数量增加
	m_pEndlessLandData->wCurContinueKillNum++;
	m_pEndlessLandData->wCurWaveKillNum++;

	m_pEndlessLandData->wContinueKillNumRecord = max(m_pEndlessLandData->wContinueKillNumRecord, m_pEndlessLandData->wCurContinueKillNum);

	// 处理排行榜
	CRankManager::Instance()->AddRoleRankData(RANK_ENDLESS_KILL, m_pPlayer, m_pEndlessLandData->wContinueKillNumRecord);

	// 无尽深渊击杀%s只怪物
	m_pPlayer->OnUpdateSevenDayTarget(24, 1, true);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 9, 1, true);

	// 节日活跃目标
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 9, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 9, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 9, 1, true);

	// 响应
	m_pPlayer->SendPacket(&msg);

// 	MAPLOG(ERROR_LEVEL, "[%s]无尽之地杀怪上报[role_id:%d,cur_wave:%d,cur_kill_num:%d]",
// 		__FUNCTION__, m_pPlayer->GetRoleId(), m_pEndlessLandData->wCurWave, m_pEndlessLandData->wCurWaveKillNum);
}

// 挂机碰撞
void CEndlessLandManager::OnNetCollision()
{
	const EndlessLandCollision_Config *pCollisionCfg = NULL;

	// 检测今日次数是否已达上限
	if (m_pEndlessLandData->wTodayCollisionRewardTimes < g_GetConfigValue(176))
	{
		// 取配置列表
		const vector<EndlessLandCollision_Config> &vCollisionList = g_pCfgMgr->GetEndlessLandCollisionList();

		// 计算总概率
// 		vector<EndlessLandCollision_Config>::const_iterator iter = vCollisionList.begin();
// 		DWORD dwTotalRate = 0;
// 		for (; iter != vCollisionList.end(); ++iter)
// 		{
// 			dwTotalRate += iter->dwRate;
// 		}

		// 随机
		DWORD dwRandValue = g_Rand();

		// 计算那个命中
		vector<EndlessLandCollision_Config>::const_iterator iter = vCollisionList.begin();
		DWORD dwCurRate = 0;
		for (; iter != vCollisionList.end(); ++iter)
		{
			dwCurRate += iter->dwRate;
			if (dwRandValue < dwCurRate)
			{
				pCollisionCfg = &*iter;
				break;
			}
		}

		// 命中
		if (pCollisionCfg)
			m_pPlayer->AddObjectList(pCollisionCfg->vRewardItemList, TRUE, FROM_ENDLESS_LAND_COLLISION);

		++m_pEndlessLandData->wTodayCollisionRewardTimes;
	}

	// 响应
	MSG_S2C_ENDLESS_LAND_COLLISION_RESP msg;
	msg.m_wTodayCollisionRewardTimes = m_pEndlessLandData->wTodayCollisionRewardTimes;
	msg.m_wRewardId = pCollisionCfg ? pCollisionCfg->wRewardId : 0;
	m_pPlayer->SendPacket(&msg);
}

// 领取连杀奖励
void CEndlessLandManager::OnNetTakeKeepKillReward(WORD wKeepKillNum)
{
	// 取奖励配置
	const EndlessLandContinueKillReward_Config *pRewardCfg = g_pCfgMgr->GetEndlessLandKeepKillReward(wKeepKillNum);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到无尽之地连杀奖励配置[kill_num:%d]", __FUNCTION__, wKeepKillNum);
		return;
	}

	// 检测连杀的数量是否足够
	if (m_pEndlessLandData->wContinueKillNumRecord < wKeepKillNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地连杀数量不足,不能领取[kill_num:%d]", __FUNCTION__, wKeepKillNum);
		return;
	}


	// 检测是否已经领取过
	if (wKeepKillNum <= m_pEndlessLandData->wLastTakeContinueKillReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地连杀领取已领取过[kill_num:%d]", __FUNCTION__, wKeepKillNum);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRewardCfg->nDiamond, TRUE, FROM_ENDLESS_KEEP_KILL_REWARD);

	m_pEndlessLandData->wLastTakeContinueKillReward = wKeepKillNum;

	SendEndlessLandData();

	// 响应
	MSG_S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP msg;
	msg.m_wKeepKillNum = wKeepKillNum;
	m_pPlayer->SendPacket(&msg);

	// 玩家在深渊中达成目标获得钻石奖励时，在全服进行公告
	// 公告内容：【玩家名】在无尽深渊大杀四方，累计杀敌XXXX，获得了Y钻石！
	CSystemNotice sys_notice(17, m_pPlayer->GetServerId());
	sys_notice.AddParam(m_pPlayer->GetRoleName());
	sys_notice.AddParam(wKeepKillNum);
	sys_notice.AddParam(pRewardCfg->nDiamond);
	sys_notice.SendMsg();
}

// 发送无尽之地数据
void CEndlessLandManager::SendEndlessLandData()
{
	MSG_S2C_ENDLESS_LAND_DATA_RESP msg;
	msg.m_tEndlessLandData = *m_pEndlessLandData;
	m_pPlayer->SendPacket(&msg);
}

// 取最佳连杀记录
DWORD CEndlessLandManager::GetContinueKillNumRecord()
{
	return m_pEndlessLandData->wContinueKillNumRecord;
}

// 检测是否已领取过连杀奖励
// bool CEndlessLandManager::IsTakeKeepKillReward(WORD wKeepKillNum)
// {
// 	for (WORD i = 0; i < m_pEndlessLandData->wContinueKillRewardRecordNum; ++i)
// 	{
// 		if (m_pEndlessLandData->arContinueKillRewardRecord[i] == wKeepKillNum)
// 			return true;
// 	}
// 	return false;
// }