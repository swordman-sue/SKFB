#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "AchievementManager.h"
#include "AchievementDataDefine.h"

CAchievementManager::CAchievementManager()
{

}

CAchievementManager::~CAchievementManager()
{

}

void CAchievementManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pAchievementInfo = &pPlayer->GetRoleFullInfo().tAchievementInfo;
}

// 请求成就列表
void CAchievementManager::OnNetAchievementList()
{
	SendAchievementList();
}

// 领取成就奖励
void CAchievementManager::OnNetAchievementAward(WORD wAchievementId)
{
	// 取成就配置
	const Achievement_Config *pAchievementCfg = CConfigManager::Instance()->GetAchievement(wAchievementId);
	if (!pAchievementCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到成就配置数据![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	//  玩家成就数据
	AchievementData *pAchievementData = GetAchievementData(wAchievementId);
	if (!pAchievementData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的成就数据![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	// 检测是否已领取
	if (pAchievementData->byCurRewardLevel >= pAchievementData->byLv)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]成就奖励已领取![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	const Achievement_Config::LvData *pLvDataCfg = pAchievementCfg->GetLvData(pAchievementData->byCurRewardLevel + 1);
	if (!pLvDataCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到成就等级配置数据[Id:%d,Lv:%d]", 
			__FUNCTION__, wAchievementId, pAchievementData->byCurRewardLevel + 1);
		return;
	}

	// 奖励
	m_pPlayer->AddObject(pLvDataCfg->tRewardItem.dwId, pLvDataCfg->tRewardItem.dwNum, true, FROM_ACHIEVEMENT_AWARD);

	// 记录奖励已领取
	pAchievementData->byCurRewardLevel++;

	MSG_S2C_ACHIEVEMENT_AWARD_RESP msg;
	msg.m_wAchievementId = wAchievementId;
	m_pPlayer->SendPacket(&msg);
}

// 发送成就列表
void CAchievementManager::SendAchievementList()
{
	MSG_S2C_ACHIEVEMENT_LIST_RESP msg;
	msg.m_wAchievementNum = m_pAchievementInfo->wAchievementNum;
	int nCopySize = sizeof(AchievementData)* m_pAchievementInfo->wAchievementNum;
	memcpy_s(msg.m_AhievementList, nCopySize, m_pAchievementInfo->arAchievementList, nCopySize);
	m_pPlayer->SendPacket(&msg);
}

// 发送成就数据
void CAchievementManager::SendAchievementData(const AchievementData *pData)
{
	MSG_S2C_ACHIEVEMENT_DATA_NOTIFY msg;
	memcpy_s(&msg.m_tAchievementData, sizeof(AchievementData), pData, sizeof(AchievementData));
	m_pPlayer->SendPacket(&msg);
}

// 更新成就
void CAchievementManager::OnUpdateAchievement(WORD wAchievementId, int nValue, bool bAdd)
{
	// 取成就配置
	const Achievement_Config *pAchievementCfg = CConfigManager::Instance()->GetAchievement(wAchievementId);
	if (!pAchievementCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到成就配置数据![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	//  玩家成就数据
	AchievementData *pAchievementData = GetAchievementData(wAchievementId);
	if (!pAchievementData)
	{
		AchievementData tData;
		memset(&tData, 0, sizeof(tData));
		tData.wId = wAchievementId;
		AddAchievementData(tData);
		pAchievementData = GetAchievementData(wAchievementId);
	}

	// 成就已完成
	if (pAchievementData->byLv >= pAchievementCfg->vLvDataList.size())
		return;

	// 添加值
	if (bAdd)
	{
		pAchievementData->dwValue += nValue;
	}
	// 设置值
	else
	{
		// 使用最佳记录
		if (nValue <= pAchievementData->dwValue)
			return;

		pAchievementData->dwValue = nValue;
	}

// todo::原来的逻辑
// 	const Achievement_Config::LvData *pLvDataCfg = pAchievementCfg->GetLvData(pAchievementData->byLv + 1);
// 	if (!pLvDataCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到成就等级配置![AchievementId:%d,Lv:%d]",
// 			__FUNCTION__, wAchievementId, pAchievementData->byLv + 1);
// 		break;
// 	}
// 
// 	if (pAchievementData->dwValue >= pLvDataCfg->wValue)
// 	{
// 		pAchievementData->byLv++;
// 	}

	while (true)
	{
		const Achievement_Config::LvData *pLvDataCfg = pAchievementCfg->GetLvData(pAchievementData->byLv + 1);
		if (pLvDataCfg && pAchievementData->dwValue >= pLvDataCfg->wValue)
		{
			pAchievementData->byLv++;
		}
		else
		{
			break;
		}
	}

	SendAchievementData(pAchievementData);

	// 记录角色信息更新
	m_pPlayer->RoleInfoUpdate();
}

// 是否有红点
bool CAchievementManager::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_pAchievementInfo->wAchievementNum; ++i)
	{
		AchievementData *pAchievement = &m_pAchievementInfo->arAchievementList[i];
		if (pAchievement->byLv > pAchievement->byCurRewardLevel)
			return true;
	}

	return false;
}

AchievementData* CAchievementManager::GetAchievementData(WORD wAchievementId)
{
	for (WORD i = 0; i < m_pAchievementInfo->wAchievementNum; ++i)
	{
		if (m_pAchievementInfo->arAchievementList[i].wId == wAchievementId)
		{
			return &m_pAchievementInfo->arAchievementList[i];
		}
	}
	return NULL;
}

// 添加成就数据
void CAchievementManager::AddAchievementData(const AchievementData &tAchievement)
{
	if (m_pAchievementInfo->wAchievementNum >= MAX_ACHIEVEMENT_NUM)
		return;

	m_pAchievementInfo->arAchievementList[m_pAchievementInfo->wAchievementNum++] = tAchievement;
}