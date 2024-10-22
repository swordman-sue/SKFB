#include "stdafx.h"
#include "CampRecruit.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"
#include "..\MapServer.h"
#include "CommonLogic.h"
#include "..\GameLogic.h"


void CCampRecruit::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pCampRecruitData = &pPlayer->GetRoleFullInfo().tActivityInfo.tPlayerCampRecruitData;
}

// 请求招募数据
void CCampRecruit::OnNetRecruitData()
{
	UpdateRecruitData();
	SendRecruitData();
}

// 招募请求
void CCampRecruit::OnNetRecruit(WORD wRecuitType, WORD wCamp)
{
	UpdateRecruitData();

	// 检测活动是否开启
	WORD wDataId = GetCurDataId();
	if (!wDataId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营招募活动没有开启", __FUNCTION__);
		return;
	}

	// 取招募配置
	const CampRecruit_Config *pRecruitCfg = g_pCfgMgr->GetCampRecruit(wDataId, wCamp);
	if (!pRecruitCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到阵营招募配置[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
		return;
	}

	// 取招募英雄列表
	const vector<CampRecruitHero_Config> *pRecruitHeroList =  g_pCfgMgr->GetCampRecruitHeroList(wDataId, wCamp);
	if (!pRecruitHeroList || !pRecruitHeroList->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到阵营招募英雄列表配置[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
		return;
	}

	int nCostDiamond = 0;
	DWORD dwRecruitScore = 0;
	WORD wDailyFreeRecruitTimes = 0;
	WORD wRecruitTimes = 1;

	// 普通招募
	if (1 == wRecuitType)
	{
		nCostDiamond = pRecruitCfg->dwNormalRecruitCostDiamond;
		dwRecruitScore = pRecruitCfg->dwNormalRecruitScore;
		wDailyFreeRecruitTimes = pRecruitCfg->wDailyFreeRecruitTimes;

		// 如果免费招募次数已使用完
		if (m_pCampRecruitData->wTodayAlreadyRecruitTimes >= wDailyFreeRecruitTimes)
		{
			// 检测消耗的钻石
			if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
			{
				MAPLOG(ERROR_LEVEL, "[%s]阵营招募消耗的钻石不足[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
				return;
			}

			// 扣除消耗
			m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CAMP_RECRUIT);
		}

		m_pCampRecruitData->wTodayAlreadyRecruitTimes++;
	}
	// 一键招募
	else
	{
		nCostDiamond = pRecruitCfg->dwOneKeyRecruitCostDiamond;
		dwRecruitScore = pRecruitCfg->dwOneKeyRecruitScore;
		wRecruitTimes = pRecruitCfg->wOneKeyRecruitTimes;

		// 检测消耗的钻石
		if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			MAPLOG(ERROR_LEVEL, "[%s]阵营招募消耗的钻石不足[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
			return;
		}

		// 扣除消耗
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CAMP_RECRUIT);
	}

	// 添加获得的积分
	AddCampRecruitScore(wCamp, dwRecruitScore);

	// 计算总概率
	int nTotalWeight = 0;
	vector<CampRecruitHero_Config>::const_iterator iter = pRecruitHeroList->begin();
	for (; iter != pRecruitHeroList->end(); ++iter)
	{
		nTotalWeight += iter->dwWeight;
	}

	// 处理招募随机
	vector<Item> vRewardItemList;
	for (WORD i = 0; i < wRecruitTimes; ++i)
	{
		int nRandValue = g_Rand(0, nTotalWeight);
		int nCurValue = 0;
		iter = pRecruitHeroList->begin();
		for (; iter != pRecruitHeroList->end(); ++iter)
		{
			nCurValue += iter->dwWeight;
			if (nRandValue <= nCurValue)
			{
				vRewardItemList.push_back(iter->tRewardItem);

				// 是否需要记录
				if (iter->byIsNeedRecord)
					AddRecruitRecord(wCamp, iter->tRewardItem);
				break;
			}
		}
	}

	// 添加招募到的英雄或者碎片
	m_pPlayer->AddObjectList(vRewardItemList, TRUE, FROM_CAMP_RECRUIT);

	// 响应
	MSG_S2C_CAMP_RECRUIT_RESP msg;
	msg.m_wCamp = wCamp;
	msg.m_wRecuitType = wRecuitType;
	msg.m_vHeroList = vRewardItemList;
	m_pPlayer->SendPacket(&msg);
}

// 领取积分任务奖励
void CCampRecruit::OnNetScoreTaskReward(WORD wCamp, WORD wRewardIdx)
{
	// 更新数据
	UpdateRecruitData();

	// 检测活动是否开启
	WORD wDataId = GetCurDataId();
	if (!wDataId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营招募活动没有开启", __FUNCTION__);
		return;
	}

	// 取招募配置
	const CampRecruit_Config *pRecruitCfg = g_pCfgMgr->GetCampRecruit(wDataId, wCamp);
	if (!pRecruitCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到阵营招募配置[DataId:%d,Camp:%d]", __FUNCTION__,wDataId, wCamp);
		return;
	}

	// 检测需要的积分是否足够
	DWORD dwCurScore = GetCampRecruitScore(wCamp);
	DWORD dwNeedScore = pRecruitCfg->GetTaskScore(wRewardIdx);
	if (dwCurScore < dwNeedScore)
	{
		MAPLOG(ERROR_LEVEL, "[%s]领取阵营招募奖励需要的积分不足[DataId:%d, Camp:%d, RewardIdx:%d,CurScore:%d, NeedScore:%d]", 
			__FUNCTION__, wDataId, wCamp, wRewardIdx, dwCurScore, dwNeedScore);
		return;
	}

	// 检测是否已领取过
	if (IsScoreTaskRewardRecord(wCamp, wRewardIdx))
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营招募积分任务奖励已领取过[DataId:%d,Camp:%d,RewardIdx:%d]", __FUNCTION__, wDataId, wCamp, wRewardIdx);
		return;
	}

	// 发奖励
	const Item *pRewardCfg = pRecruitCfg->GetTaskRewardItem(wRewardIdx);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到阵营招募积分任务奖励配置[DataId:%d,Camp:%d,RewardIdx:%d]", __FUNCTION__, wDataId, wCamp, wRewardIdx);
		return;
	}
	m_pPlayer->AddObjectData(pRewardCfg, TRUE, FROM_GET_CAMP_RECRUIT_SCORE_TASK_REWARD);

	// 添加记录
	AddScoreTaskRewardRecord(wCamp, wRewardIdx);

	// 响应
	MSG_S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP msg;
	msg.m_wCamp = wCamp;
	msg.m_wRewardIdx = wRewardIdx;
	m_pPlayer->SendPacket(&msg);
}

// 请求招募记录
void CCampRecruit::OnNetRecruitRecord(WORD wCamp)
{
	MSG_S2C_CAMP_RECRUIT_RECORD_RESP msg;
	msg.m_wCamp = wCamp;
	const deque<CampRecruitRecord> *pRecordList = CGameLogic::Instance()->GetCampRecruitRecordList(m_pPlayer->GetServerId(), wCamp);
	if (pRecordList)
	{
		deque<CampRecruitRecord>::const_iterator iter = pRecordList->begin();
		for (; iter != pRecordList->end(); ++iter)
		{
			//if (iter->wCamp == wCamp)
			msg.m_vRecruitRecordList.push_back(*iter);
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// 发送阵营招募数据
void CCampRecruit::SendRecruitData()
{
	MSG_S2C_CAMP_RECRUIT_DATA_RESP msg;
	msg.m_wCurDataId = m_pCampRecruitData->wCurDataId;
	msg.m_wTodayAlreadyRecruitTimes = m_pCampRecruitData->wTodayAlreadyRecruitTimes;
	msg.m_vCampScoreList.clear();
	msg.m_RewardRecordList.clear();
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		msg.m_vCampScoreList.push_back(m_pCampRecruitData->arCampScoreList[i]);
	}
	vector<WORD> tCampRecordList;
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		tCampRecordList.clear();
		//vector<WORD> &tCampRecordList = msg.m_RewardRecordList[i];
		for (int k = 0; k < m_pCampRecruitData->wScoreTaskRewardRecordNum; ++k)
		{
			WORD wRecordId = m_pCampRecruitData->arScoreTaskRewardRecordList[k];
			WORD wCamp = wRecordId * 0.01;
			if (wCamp == i)
			{
				WORD wRewardIdx = wRecordId ? wRecordId % 100 : 0;
				tCampRecordList.push_back(wRewardIdx);
			}
		}
		msg.m_RewardRecordList.push_back(tCampRecordList);
	}

	m_pPlayer->SendPacket(&msg);
}

// 更新阵营招募数据
void CCampRecruit::UpdateRecruitData()
{
	WORD wCurDataId = m_pActivityMgr->GetActivityDataId(146);
	if (wCurDataId != m_pCampRecruitData->wCurDataId)
	{
		m_pCampRecruitData->wCurDataId = wCurDataId;
		m_pCampRecruitData->wScoreTaskRewardRecordNum = 0;
		m_pCampRecruitData->wTodayAlreadyRecruitTimes = 0;
		memset(m_pCampRecruitData->arCampScoreList, 0, sizeof(m_pCampRecruitData->arCampScoreList));
	}
}

// 取当前活动数据ID
WORD CCampRecruit::GetCurDataId()
{
	return m_pCampRecruitData->wCurDataId;
}

// 添加阵营招募积分
void CCampRecruit::AddCampRecruitScore(WORD wCamp, DWORD dwScore)
{
	if (wCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营越界[Camp:%d]", __FUNCTION__, wCamp);
		return;
	}
	m_pCampRecruitData->arCampScoreList[wCamp] += dwScore;
}

// 取阵营招募积分
DWORD CCampRecruit::GetCampRecruitScore(WORD wCamp)
{
	if (wCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营越界[Camp:%d]", __FUNCTION__, wCamp);
		return 0;
	}
	return m_pCampRecruitData->arCampScoreList[wCamp];
}

WORD MakeRecordId(WORD wCamp, WORD wRewardIdx)
{
	return 100 * wCamp + wRewardIdx;
}

// 是否已领取积分任务奖励
bool CCampRecruit::IsScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx)
{
	WORD wRecordId = MakeRecordId(wCamp, wRewardIdx);
	for (WORD i = 0; i < m_pCampRecruitData->wScoreTaskRewardRecordNum; ++i)
	{
		if (m_pCampRecruitData->arScoreTaskRewardRecordList[i] == wRecordId)
			return true;
	}
	return false;
}

// 添加积分任务奖励记录
void CCampRecruit::AddScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx)
{
	if (m_pCampRecruitData->wScoreTaskRewardRecordNum >= MAX_CAMP_RECRUIT_SCORE_TASK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营招募积分任务奖励记录列表已满[Camp:%d]", __FUNCTION__, wCamp);
		return;
	}

	WORD wRecordId = MakeRecordId(wCamp, wRewardIdx);
	m_pCampRecruitData->arScoreTaskRewardRecordList[m_pCampRecruitData->wScoreTaskRewardRecordNum++] = wRecordId;
}

// 添加招募记录
void CCampRecruit::AddRecruitRecord(WORD wCamp, const Item &tItem)
{
	CampRecruitRecord tRecord;
	tRecord.dwRecruitTime = time(NULL);
	tRecord.tRewardData = tItem;
	strcpy_s(tRecord.szRoleName, sizeof(tRecord.szRoleName), m_pPlayer->GetRoleName());
	tRecord.wCamp = wCamp;
	CGameLogic::Instance()->AddCampRecruitRecord(m_pPlayer->GetServerId(), wCamp, tRecord);
}