#include "stdafx.h"
#include "WarcraftManager.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\PlayerBaseDataManager.h"
#include "CommonLogic.h"

CWarcraftManager::CWarcraftManager()
{

}

CWarcraftManager::~CWarcraftManager()
{
	
}

void CWarcraftManager::Init()
{

}

// 请求争霸赛数据
void CWarcraftManager::OnNetWarcraftData(Player *pPlayer)
{
	SendWarcraftData(pPlayer);
}

// 购买挑战次数
void CWarcraftManager::OnNetBuyBattleTimes(Player *pPlayer)
{
	WORD wFromMsgId = C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ;

	// 检测是否有资格购买
	WarcraftRankData *pRankData = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pRankData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]木有在排行榜里,不能购买挑战次数", __FUNCTION__);
		return;
	}

	// 检测购买次数
	int nBuyTimes = pPlayer->GetProperty(ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES);
	if (nBuyTimes >= pPlayer->GetVipValue(VIP_WARCRAFT_BUY_BATTLE_TIMES))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]木有在排行榜里,不能购买挑战次数", __FUNCTION__);
		return;
	}

	// 检测消耗
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(101);
	if (!pServerCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置[key:%d]", __FUNCTION__, 101);
		return;
	}
	int nCostDiamond = pServerCfg->GetListValue(nBuyTimes);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]购买争霸赛挑战次数消耗不足", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_WARCRAFT_BATTLE_TIMES_BUY);

	pPlayer->ModifyProperty(ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES, 1);

	MSG_S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 请求挑战目标
void CWarcraftManager::OnNetBattleTargetList(Player *pPlayer)
{
	SendTargetList(pPlayer);
}

// 请求排行榜列表
void CWarcraftManager::OnNetRankList(Player *pPlayer)
{
	MSG_S2C_WARCRAFT_RANK_LIST_RESP msg;
	msg.m_wRankNum = 0;

	for (vector<WarcraftRankData>::iterator iter = m_RankList.begin(); iter != m_RankList.end(); ++iter)
	{
		// 只取本服的玩家
		if (iter->dwServerId != pPlayer->GetServerId())
			continue;

		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter->dwServerId, iter->dwRoleId);
		if (pBaseData)
		{
			ClientWarcraftRankData &tRankData = msg.m_RankList[msg.m_wRankNum++];
			memcpy_s(&tRankData, sizeof(WarcraftRankData), &iter, sizeof(WarcraftRankData));
			tRankData.dwHeadImageId = pBaseData->wHeadIcon;
			strcpy_s(tRankData.szRoleName, sizeof(tRankData.szRoleName) - 1, pBaseData->szRoleName);
		}
	}

	pPlayer->SendPacket(&msg);
}

// 助威
void CWarcraftManager::OnNetCheer(Player *pPlayer, DWORD dwTargetId)
{
	WORD wFromMsgId = C2S_WARCRAFT_CHEER_REQ;

	// 检测目标数据
	WarcraftRankData *pRankData = GetPlayerRankData(dwTargetId);
	if (!pRankData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]助威的目标木有在排行榜[TargetId:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// 检测助威次数
	if (pPlayer->GetProperty(ROLE_PRO_WARCRAFT_CHEER_TIMES) >= 3)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]助威次数已达上限", __FUNCTION__);
		return;
	}
	
	pPlayer->ModifyProperty(ROLE_PRO_WARCRAFT_CHEER_TIMES);
	pPlayer->SetProperty(ROLE_PRO_WARCRAFT_CHEER_TIMES_RECOVER_TIME, time(NULL));

	// 更新被助威次数
	pRankData->wBeCheerTimes++;

	// 响应
	MSG_S2C_WARCRAFT_CHEER_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&msg);
}

// 开始战斗
bool CWarcraftManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// todo::检测挑战次数
	if (pPlayer->GetProperty(ROLE_PRO_WARCRAFT_BATTLE_TIMES) >= 10)
	{
		MAPLOG(ERROR_LEVEL, "[%s]争霸赛挑战次数限制", __FUNCTION__);
		return false;
	}

	WarcraftRankData *pRankData = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pRankData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有在排行榜里,木有挑战资格", __FUNCTION__);
		return false;
	}

	// 检测目标
	vector<WarcraftRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetRoleId(), &vTargetList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]取竞技场挑战目标列表失败", __FUNCTION__);
		return false;
	}

	bool bFound = false;
	for (int i = 0; i < vTargetList.size(); ++i)
	{
		if (vTargetList[i].dwRoleId == dwTargetId)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		SendTargetList(pPlayer);
		MAPLOG(ERROR_LEVEL, "[%s]挑战的目标不合法", __FUNCTION__);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);
	pFBMgr->SetBattleInfo(0, 0, 0);
	pFBMgr->SetTargetId(dwTargetId);

	// 如果是玩家,则需要把英雄发过去
	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, true);
	if (pTargetBaseData)
	{
		pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);
		pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
	}

	pPlayer->ModifyProperty(ROLE_PRO_WARCRAFT_BATTLE_TIMES, 1, false);

	return true;
}


// 结束战斗
bool CWarcraftManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	WarcraftRankData *pSrc = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pSrc)
	{
		MAPLOG(ERROR_LEVEL, "[%s]争霸赛挑战结束时,找不到挑战方数据", __FUNCTION__);
		return false;
	}
	WarcraftRankData *pTarget = GetPlayerRankData(pFBMgr->GetTargetId());
	if (!pTarget)
	{
		MAPLOG(ERROR_LEVEL, "[%s]争霸赛挑战结束时,找不到目标数据", __FUNCTION__);
		return false;
	}

	DWORD dwScore, dwScoreMatchHonor;

	// 积分 = 50+对手战力/5万
	dwScore = 50 + pTarget->dwFightingPower * 0.0005;

	// 胜利
	if (byIsWin)
	{
		// 如果目标的排名比我高
		if (pSrc->wRank > pTarget->wRank)
		{
			// 交换位置
			swap(*pSrc, *pTarget);	// 整个数据交换
			swap(pSrc->wRank, pTarget->wRank);	// 排名交换
			swap(pSrc, pTarget);	// 交换地址
		}

		// 荣誉勋章 = 积分 = 50+对手战力/5万
		dwScoreMatchHonor = dwScore;
	}
	// 失败
	else
	{
		// 失败=胜利积分/2  向上取整
		dwScoreMatchHonor = dwScore * 0.5 + ((dwScore % 2) ? 1 : 0);
	}

	pPlayer->ModifyProperty(ROLE_PRO_SCORE_MATCH_HONOR, dwScoreMatchHonor);

	return true;
}

// 发送争霸赛数据
void CWarcraftManager::SendWarcraftData(Player *pPlayer)
{
	MSG_S2C_WARCRAFT_DATA_RESP msg;
	msg.m_byWarcraftBattleTimes = pPlayer->GetProperty(ROLE_PRO_WARCRAFT_BATTLE_TIMES);
	msg.m_byBuyWarcraftBattleTimes = pPlayer->GetProperty(ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES);
	pPlayer->SendPacket(&msg);
}

// 发送目标列表
void CWarcraftManager::SendTargetList(Player *pPlayer)
{
	WarcraftRankData *pRankData = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pRankData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有在排行榜里,不能请求挑战目标", __FUNCTION__);
		return;
	}

	vector<WarcraftRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetRoleId(), &vTargetList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]获取目标列表失败!", __FUNCTION__);
		return;
	}

 	MSG_S2C_WARCRAFT_TARGET_LIST_RESP msg;
	msg.m_wRank = pRankData->wRank;
 	msg.m_byTargetNum = 0;
 	for (vector<WarcraftRankData>::iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter)
 	{
 		if (msg.m_byTargetNum >= MAX_ARENA_TARGET_NUM)
 			break;
 
		WarcraftTargetData &tTargetData = msg.m_TargetList[msg.m_byTargetNum++];
 		memset(&tTargetData, 0, sizeof(tTargetData));
		tTargetData.dwRoleId = iter->dwRoleId;
		tTargetData.wRank = iter->wRank;
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter->dwServerId, iter->dwRoleId);
 		if (pBaseData)
 		{
			strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
 			tTargetData.wHeadIcon = pBaseData->wHeadIcon;
 			tTargetData.dwFightingPower = pBaseData->dwFightingPower;
 		}
 	}
 
 	pPlayer->SendPacket(&msg);
}

// 取挑战目标列表
bool CWarcraftManager::GetTargetList(DWORD dwRoleId, vector<WarcraftRankData> *pOutTargetList)
{
	pOutTargetList->clear();

	WarcraftRankData *pRankData = GetPlayerRankData(dwRoleId);
	if (!pRankData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有在排行榜里,不能请求挑战目标", __FUNCTION__);
		return false;
	}

	WORD wShowRank = 10;

	// 取前10名
	for (vector<WarcraftRankData>::iterator iter = m_RankList.begin(); iter != m_RankList.end(); ++iter)
	{
		if (pOutTargetList->size() >= wShowRank)
			break;
		pOutTargetList->push_back(*iter);
	}

	WORD wRank = pRankData->wRank;

	const WarcraftRank_Config *pRankCfg = g_pCfgMgr->GetWarcraftRank(wRank);
 	if (!pRankCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到争霸赛排名配置[Rank:%d]", __FUNCTION__, wRank);
		return false;
 	}

	// 防止为0的情况
	WORD wTargetRankInterval = min(pRankCfg->wTargetRankInterval, 1);

	// 排名在10名之外
	vector<WarcraftRankData> vTmpRankList;
	if (wRank > wShowRank)
	{
		// 上面目标(只取9名)
		int nTargetRank = wRank - wTargetRankInterval * 9;
		nTargetRank = max(nTargetRank, 0);
		while (wTargetRankInterval && nTargetRank < wRank)
		{
			vTmpRankList.push_back(m_RankList[nTargetRank - 1]);
			nTargetRank += wTargetRankInterval;
		}

		for (size_t i = 0; i < vTmpRankList.size(); ++i)
		{
			bool bHave = false;
			for (size_t j = 0; j < pOutTargetList->size(); ++j)
			{
				if (pOutTargetList->at(j).dwRoleId == vTmpRankList[i].dwRoleId)
				{
					bHave = true;
					break;
				}
			}
			if (!bHave)
			{
				pOutTargetList->push_back(vTmpRankList[i]);
			}
		}

		// 下面目标(只取两名)
		nTargetRank = wRank;
		for (int i = 0; i < 2/*todo::暂时介么处理*/; ++i)
		{
			nTargetRank += wTargetRankInterval;
			if (m_RankList.size() > nTargetRank)
				pOutTargetList->push_back(m_RankList[nTargetRank - 1]);
		}
	}
	// 排名在10名之内
	else
	{
		pOutTargetList->push_back(m_RankList[wShowRank]);
		pOutTargetList->push_back(m_RankList[wShowRank + 1]);
	}

	return true;
}

// 添加竞技场排行榜数据
void CWarcraftManager::AddArenaRankData(DWORD dwRoleId, WORD wRank)
{
	m_ArenaRankList.insert(make_pair(dwRoleId, wRank));
}

// 取玩家排行数据
WarcraftRankData* CWarcraftManager::GetPlayerRankData(DWORD dwRoleId)
{
	for (size_t i = 0; i < m_RankList.size(); ++i)
	{
		if (m_RankList[i].dwRoleId == dwRoleId)
			return &m_RankList[i];
	}
	return NULL;
}