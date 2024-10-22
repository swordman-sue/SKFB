#include "stdafx.h"
#include "ZhanBuManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\Rank\RankManager.h"
#include "..\Fuben\FubenManager.h"


CZhanBuManager::CZhanBuManager()
{
	m_dwSeriesId = 1;
}

CZhanBuManager::~CZhanBuManager()
{

}

void CZhanBuManager::Init(Player *pPlayer)
{
	m_dwSeriesId = 1;
	m_pPlayer = pPlayer;
	m_pZhanBuInfo = &pPlayer->GetRoleFullInfo().tZhanBuInfo;
}

// 开始战斗
bool CZhanBuManager::OnBattleStart()
{
	WORD wMapIdx = m_pZhanBuInfo->wLowCardGroupMapIdx;
	const CardGroupMap_Config *pMapCfg = g_pCfgMgr->GetCardGroupMap(wMapIdx);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到卡组关卡配置[MapIdx:%d]!", __FUNCTION__, wMapIdx);
		return false;
	}

	// 记录战斗信息
	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
	pFBMgr->SetBattleInfo(0, 0, pMapCfg->dwMapId);

	return true;
}

// 战斗结束
bool CZhanBuManager::OnBattleOver(BYTE byIsWin)
{
	if (byIsWin)
	{
		CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
		DWORD dwMapId = pFBMgr->GetMapId();

		// 处理奖励
		const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
		if (!pMapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到关卡配置[MapId:%d]!", __FUNCTION__, dwMapId);
			return false;
		}

		// 首胜奖励
		pFBMgr->SetIsFirstWin(true);

		// 卡组升级
		m_pZhanBuInfo->wLowCardGroupMapIdx++;
	}

	return true;
}

// 请求玩家仙灵数据
void CZhanBuManager::OnNetPlayerZhanBuData()
{
	// 检测是否已初始化数据
	if (!IsAlreadyInitData())
	{
		InitData();
	}

	SendPlayerZhanBuData();
}

// 请求仙灵列表
void CZhanBuManager::OnNetCardList()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_CARD_LIST_RESP msg;
	msg.m_pZhanBuInfo = m_pZhanBuInfo;
	msg.m_wCardNum = 0;
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		ClientCard &tClientCard = msg.m_CardList[msg.m_wCardNum++];
		Card &tCard = m_pZhanBuInfo->arCardList[i];
		tClientCard.dwId = tCard.dwId;
		tClientCard.dwInfoId = tCard.dwInfoId;
		tClientCard.wLevel = tCard.wLevel;
		tClientCard.dwExp = tCard.dwExp;
		tClientCard.wBreakLevel = tCard.wBreakLevel;
		tClientCard.dwHeroId = pHeroMgr->GetCardWearHeroId(tCard.dwId);
	}
	m_pPlayer->SendPacket(&msg);
}

// 升级
void CZhanBuManager::OnNetLevelUp(DWORD dwUID, DWORD *pCostCardList, WORD wCostCardNum)
{
	// 取卡牌数据
	Card* pCard = GetCardInner(dwUID);
	if (!pCard)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家仙灵数据[uid:%d]!", __FUNCTION__, dwUID);
		return;
	}

	// 取卡牌配置
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵的配置数据[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// 1.检测消耗列表, 2.计算获得的总经验
	DWORD dwTotalExp = 0;
	for (WORD i = 0; i < wCostCardNum; ++i)
	{
		DWORD dwCostCardUId = pCostCardList[i];

		// 获得的经验
		dwTotalExp += GetCardCurTotalExp(dwCostCardUId);

		// 检测是否已在装备中
		if (m_pPlayer->GetHeroManager()->GetCardWearHeroId(dwCostCardUId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]卡牌正在装备中,不能作为消耗材料[InfoId:%d]!", __FUNCTION__, dwCostCardUId);
			return;
		}
	}

	pCard->dwExp += dwTotalExp;

	// 处理升级
	const CardLevelUp_Config *pLevelUpCfg;
	while (true)
	{
		// 取升级配置
		pLevelUpCfg = g_pCfgMgr->GetCardLevelUp(pCardCfg->byQuality, pCard->wLevel);
		if (!pLevelUpCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌的升级配置数据[Quality:%d,Level:%d]!",
				__FUNCTION__, pCardCfg->byQuality, pCard->wLevel);
			break;
		}

		// 角色等级限制
		if (m_pPlayer->GetLevel() < pLevelUpCfg->wNeedRoleLevel)
			break;

		// 已达等级上限
		if (!pLevelUpCfg->dwExp)
			break;

		// 不足够升级
		if (pCard->dwExp < pLevelUpCfg->dwExp)
			break;

		// 升级
		pCard->wLevel++;
		pCard->dwExp -= pLevelUpCfg->dwExp;
	}

	CardDataNotify(pCard);

	m_pPlayer->UpdateFightingPower();

	// 先记录,防止后面的删除操作会使数据的位置发生改变则造成数据变化
	WORD wLevel = pCard->wLevel;
	DWORD dwExp = pCard->dwExp;

	// 删除消耗列表
	RemoveCard(pCostCardList, wCostCardNum, FROM_CARD_LEVEL_UP);

	// 响应
	MSG_S2C_CARD_LEVEL_UP_RESP msg;
	msg.m_dwFairyUId = dwUID;
	msg.m_wFairyLevel = wLevel;
	msg.m_dwFairyExp = dwExp;
	msg.m_wCostFairyNum = wCostCardNum;
	memcpy_s(msg.m_arCostFairyList, wCostCardNum*sizeof(DWORD), pCostCardList, wCostCardNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);

	// 节日活动
	int nCurLevel = 0; // 目前是 10,20,30,40,50级处理
	for (int nLogicType = 21; nLogicType <= 25; ++nLogicType)
	{
		nCurLevel += 10;
		int nCardNum = GetCardNumByLevel(nCurLevel);
		if (nCardNum > 0)
		{
			m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, nLogicType, nCardNum, false);
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, nLogicType, nCardNum, false);
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, nLogicType, nCardNum, false);
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, nLogicType, nCardNum, false);
		}
	}
}

// 突破
void CZhanBuManager::OnNetBreak(DWORD dwUID)
{
	// 取仙灵数据
	Card* pCard = GetCardInner(dwUID);
	if (!pCard)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家仙灵数据[uid:%d]!", __FUNCTION__, dwUID);
		return;
	}

	// 取仙灵配置
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵的配置数据[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// 取仙灵突破配置
	const CardBreak_Config *pBreakCfg = g_pCfgMgr->GetCardBreak(pCardCfg->byQuality, pCard->wBreakLevel);
	if (!pBreakCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵的突破配置数据[Quality:%d,BreakLevel:%d]!", __FUNCTION__,
			pCardCfg->byQuality, pCard->wBreakLevel);
		return;
	}

	// 检测需要的卡牌等级
	if (pBreakCfg->wNeedCardLevel > pCard->wLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]卡牌突破受卡牌等级限制[CardUId:%d,CardInfoId:%d,NeedCardLv:%d,CurLevel:%d]!", 
			__FUNCTION__, pCard->dwId, pCard->dwInfoId, pBreakCfg->wNeedCardLevel, pCard->wLevel);
		return;
	}

	// 检测货币消耗
	if (!m_pPlayer->CheckMoneyCost(pBreakCfg->vCostMoneyList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]仙灵突破消耗货币不足[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// 检测物品消耗
	if (!m_pPlayer->CheckItemCost(pBreakCfg->vCostItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]仙灵突破消耗物品不足[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// 检测消耗的同名初始卡牌数量是否足够
	if (pBreakCfg->wCostSameCardNum && GetInitCardNum(pCard->dwInfoId) < pBreakCfg->wCostSameCardNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]需要消耗的同名卡牌数量不足[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// 处理货币消耗
	for (size_t i = 0; i < pBreakCfg->vCostMoneyList.size(); ++i)
	{
		m_pPlayer->ModifyProperty(pBreakCfg->vCostMoneyList[i].wType, pBreakCfg->vCostMoneyList[i].nValue, TRUE, FROM_FAIRY_BREAK);
	}

	// 处理物品消耗
	for (size_t i = 0; i < pBreakCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pBreakCfg->vCostItemList[i].dwId, pBreakCfg->vCostItemList[i].dwNum, FROM_FAIRY_BREAK);
	}

	// 处理同名卡牌消耗
	for (WORD i = 0; i < pBreakCfg->wCostSameCardNum; ++i)
	{
		RemoveInitCard(pCard->dwInfoId);
	}

	// 突破等级提升
	pCard->wBreakLevel++;
	
	CardDataNotify(pCard);

	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_CARD_BREAK_RESP msg;
	msg.m_dwFairyUId = dwUID;
	m_pPlayer->SendPacket(&msg);

	// 节日活动
	int nCurBreakLevel = 0; // 目前是 1,2,3,4,5级处理 
	for (int nLogicType = 26; nLogicType <= 30; ++nLogicType)
	{
		nCurBreakLevel += 1;
		int nCardNum = GetCardNumByBreakLevel(nCurBreakLevel);
		if (nCardNum > 0)
		{
			m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, nLogicType, nCardNum, false);
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, nLogicType, nCardNum, false);
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, nLogicType, nCardNum, false);
			m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, nLogicType, nCardNum, false);
		}
	}
}

// 卡牌刷新
void CZhanBuManager::OnNetCardRefresh(BYTE byRefreshType)
{
	// 低级卡组
	if (1 == byRefreshType)
	{
		if (!DoLowCardGroupRefresh())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]处理低级卡组刷新失败!", __FUNCTION__);
			return;
		}
	}
	// 高级卡组
	else
	{
		if (!DoHighCardGroupRefresh())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]处理高级卡组刷新失败!", __FUNCTION__);
			return;
		}
	}

	// 响应
	MSG_S2C_CARD_REFRESH_RESP resp;
	resp.m_byRefreshType = byRefreshType;
	m_pPlayer->SendPacket(&resp);
}

void CZhanBuManager::OnNetOpenCard(BYTE byType, WORD *pIdxList, WORD wIdxListNum)
{
	// 检测类型是否合法
	if (NULL == pIdxList || !wIdxListNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]翻牌参数出错[type:%d]!", __FUNCTION__, byType);
		return;
	}

	int nKey = 1 == byType ? 251 : 252;
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(nKey);
	if (!pServerCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置[key:%d]!", __FUNCTION__, nKey);
		return;
	}

	CardGroupData *pCardGroup = NULL;

	// 低级卡组翻牌
	if (1 == byType)
	{
		pCardGroup = m_pZhanBuInfo->arLowCardGroup;

		WORD wDailyFreeOpenCardTimes = g_GetConfigValue(247);

		// 翻卡免费次数不足
		if (wIdxListNum + m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes > wDailyFreeOpenCardTimes)
		{
			WORD wCostOpenCardTimes = wIdxListNum;

			// 还有免费次数,则扣除免费次数
			if (m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes < wDailyFreeOpenCardTimes)
			{
				WORD wFreeTimes = wDailyFreeOpenCardTimes - m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes;
				wCostOpenCardTimes -= wFreeTimes;
			}

			// 检测金币消耗
			int nCostGold = g_GetConfigValue(248) * wCostOpenCardTimes;
			if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]低级卡组翻牌消耗金币不足!", __FUNCTION__);
				return;
			}

			m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold);
		}

		m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes += wIdxListNum;
	}
	// 高级卡组翻牌
	else
	{
		pCardGroup = m_pZhanBuInfo->arHighCardGroup;

		// 检测消耗
		DWORD dwCostItemId = g_GetConfigValue(249);
		if (m_pPlayer->GetItemNum(dwCostItemId) < wIdxListNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]高级卡组翻牌消耗物品不足[ItemId:%d]!", __FUNCTION__, dwCostItemId);
			return;
		}

		m_pPlayer->RemoveItem(dwCostItemId, wIdxListNum, FROM_OPEN_CARD);
	}

	int nRandDestinyCoin = 0;
	
	DWORD arDestinyCoinList[MAX_CARD_GROUP_NUM] = { 0 };
	for (WORD i = 0; i < wIdxListNum; ++i)
	{
		WORD wIdx = pIdxList[i];

		// 检测索引下标是否越界(客户端从1开始)
		if (!wIdx || wIdx > MAX_CARD_GROUP_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]翻牌参数索引出错[type:%d,idx:%d]!", __FUNCTION__, byType, wIdx);
			return;
		}

		WORD wSvrIdx = wIdx - 1;

		// 检测卡牌是否已翻开
		if (IsCardAlreadyOpen(byType, wSvrIdx))
		{
			MAPLOG(ERROR_LEVEL, "[%s]卡牌已经被翻过[type:%d,idx:%d]!", __FUNCTION__, byType, wSvrIdx);
			continue;
		}

		CardGroupData &tGroupData = pCardGroup[wSvrIdx];

		// 获得卡牌
		AddCard(tGroupData.dwCardId, TRUE, FROM_OPEN_CARD);

		// 记录为已翻开
		tGroupData.wIsAlreadyOpen = true;


		// 随机命运币
		arDestinyCoinList[i] = g_Rand(pServerCfg->GetListValue(0), pServerCfg->GetListValue(1));
		nRandDestinyCoin += arDestinyCoinList[i];
	}

	// todo::重新同步数据


	// 节日活动(累计占卜翻牌次数)
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 19, wIdxListNum, true);

	// 节日活跃目标(累计占卜翻牌次数)
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 19, wIdxListNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 19, wIdxListNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 19, wIdxListNum, true);


	// 高级卡组翻牌
	if (byType != 1)
	{
		// 节日活动(累计高级占卜翻牌次数)
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 20, wIdxListNum, true);

		// 节日活跃目标(累计高级占卜翻牌次数)
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 20, wIdxListNum, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 20, wIdxListNum, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 20, wIdxListNum, true);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DESTINY_COIN, nRandDestinyCoin);

	// 日常任务(占卜翻牌10次)
	m_pPlayer->OnUpdateDailyMission(25, wIdxListNum);

	// 响应
	MSG_S2C_OPEN_CARD_RESP resp;
	resp.m_byType = byType;
	resp.m_dwDestinyCoin = nRandDestinyCoin;
	resp.m_wCardListNum = wIdxListNum;
	for (WORD i = 0; i < wIdxListNum; ++i)
	{
		WORD wCardIdx = pIdxList[i];
		resp.m_arCardIdxList[i] = wCardIdx;
		resp.m_arCardIdList[i] = pCardGroup[wCardIdx - 1].dwCardId;
		resp.m_arDestinyCoinList[i] = arDestinyCoinList[i];
	}
	m_pPlayer->SendPacket(&resp);
}

// 低级卡组刷新次数请求
void CZhanBuManager::OnNetLowCardGroupRefreshTimesUpdate()
{
	UpdateLowCardGroupRefreshTimes();

	// 响应
	MSG_S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP resp;
	resp.m_wCurLowCardGroupRefreshTimes = m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes;
	resp.m_dwLowCardGroupLastRecoverTime = m_pZhanBuInfo->dwLowCardGroupLastRecoverTime;
	m_pPlayer->SendPacket(&resp);
}

// 收藏组激活
void CZhanBuManager::OnNetCellectionGroupActivate(WORD wBookId, WORD wGroupId)
{
	// 检测是否已激活
	CollectionGroup *pGroupData = GetCollectionGroup(wBookId, wGroupId);
	if (pGroupData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]收藏组已激活[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// 取收藏组配置
	const CollectionGroup_Config *pGroupCfg = g_pCfgMgr->GetCollectionGroup(wGroupId);
	if (!pGroupCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏组配置[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}


	// 检测需要的收藏值
	const CollectionBook_Config *pBookCfg = g_pCfgMgr->GetCollectionBook(wBookId);
	if (pBookCfg && pBookCfg->nNeedCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]激活需要的收藏值不足[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// 检测消耗的卡牌
	vector<DWORD>::const_iterator iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		if (!GetInitCard(dwCostCardId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]激活收藏组需要消耗的卡牌不存在[BookId:%d,GroupId:%d,CostCardId:%d]!", 
				__FUNCTION__, wBookId, wGroupId, dwCostCardId);
			return;
		}
	}

	// 删除消耗卡牌
	iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		RemoveInitCard(dwCostCardId);
	}

	// 添加收藏组
	AddCollectGroup(wBookId, wGroupId);

	// 属性更新
	m_pPlayer->UpdateFightingPower();

	// 更新历史收藏值
	UpdateHistoryCollectionValue();

// 	DWORD dwCollectionValue = GetCurCollectionValue();
// 	if (dwCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
// 		m_pZhanBuInfo->dwHistoryCollectionValue = dwCollectionValue;

	// 响应
	MSG_S2C_CELLECTION_GROUP_ACTIVATE_RESP resp;
	resp.m_wBookId = wBookId;
	resp.m_wGroupId = wGroupId;
	resp.m_dwHistoryCollectionValue = m_pZhanBuInfo->dwHistoryCollectionValue;
	m_pPlayer->SendPacket(&resp);

// 	// 更新排行榜
// 	CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, m_pPlayer, dwCollectionValue);
}

// 收藏组升星
void CZhanBuManager::OnNetCellectionGroupStarUp(WORD wBookId, WORD wGroupId)
{
	// 取收藏组数据
	CollectionGroup *pGroupData = GetCollectionGroup(wBookId, wGroupId);
	if (!pGroupData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏组数据[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// 检测是否到达星数上限
	if (pGroupData->wStar >= 7)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]收藏组星数已达上限[BookId:%d,GroupId:%d,Star:%d]!", 
			__FUNCTION__, wBookId, wGroupId, pGroupData->wStar);
		return;
	}

	// 取收藏组配置
	const CollectionGroup_Config *pGroupCfg = g_pCfgMgr->GetCollectionGroup(wGroupId);
	if (!pGroupCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏组配置[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// 检测消耗的卡牌
	vector<DWORD>::const_iterator iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		if (!GetInitCard(dwCostCardId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]激活收藏组需要消耗的卡牌不存在[BookId:%d,GroupId:%d,CostCardId:%d]!",
				__FUNCTION__, wBookId, wGroupId, dwCostCardId);
			return;
		}
	}

	// 删除消耗卡牌
	iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		RemoveInitCard(dwCostCardId);
	}

	// 收藏组升星
	pGroupData->wStar++;

	// 属性更新
	m_pPlayer->UpdateFightingPower();

	// 更新历史收藏值
	UpdateHistoryCollectionValue();

// 	DWORD dwCollectionValue = GetCurCollectionValue();
// 	if (dwCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
// 		m_pZhanBuInfo->dwHistoryCollectionValue = dwCollectionValue;

	// 响应
	MSG_S2C_CELLECTION_GROUP_STAR_UP_RESP resp;
	resp.m_wBookId = wBookId;
	resp.m_wGroupId = wGroupId;
	resp.m_dwHistoryCollectionValue = m_pZhanBuInfo->dwHistoryCollectionValue;
	m_pPlayer->SendPacket(&resp);

	// 更新排行榜
	//CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, m_pPlayer, dwCollectionValue);
}

// 收藏组重置
void CZhanBuManager::OnNetCellectionGroupReset(WORD wBookId, WORD wGroupId)
{
	// 取收藏组数据
	CollectionGroup *pGroupData = GetCollectionGroup(wBookId, wGroupId);
	if (!pGroupData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏组数据[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// 取收藏组配置
	const CollectionGroup_Config *pGroupCfg = g_pCfgMgr->GetCollectionGroup(wGroupId);
	if (!pGroupCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏组配置[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// 归还消耗的卡牌
	for (WORD i = 0; i < pGroupData->wStar; ++i)
	{
		vector<DWORD>::const_iterator iter = pGroupCfg->vCostCardList.begin();
		for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
		{
			DWORD dwCostCardId = *iter;
			AddCard(dwCostCardId, TRUE, FROM_CELLECTION_GROUP_RESET);
		}
	}

	WORD wStar = pGroupData->wStar;

	// 删除重置的收藏组
	RemoveCollectGroup(wBookId, wGroupId);

	// 属性更新
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_CELLECTION_GROUP_RESET_RESP msg;
	msg.m_wBookId = wBookId;
	msg.m_wGroupId = wGroupId;
	msg.m_wStar = wStar;
	m_pPlayer->SendPacket(&msg);
}

// 请求收藏书列表
void CZhanBuManager::OnNetCollectionBookList()
{
	MSG_S2C_COLLECTION_BOOK_LIST_RESP msg;
	msg.m_wCollectionBookNum = m_pZhanBuInfo->wCollectionBookNum;
	for (WORD i = 0; i < m_pZhanBuInfo->wCollectionBookNum; ++i)
	{
		msg.m_arCollectionBookList[i] = m_pZhanBuInfo->arCollectionBookList[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// 装备卡牌
void CZhanBuManager::OnNetEquipCard(WORD wFormationPos, WORD wCardSlot, DWORD dwUID)
{
	// 检测参数 (阵型位置, 装备位置)
	if (!wFormationPos || wFormationPos > MAX_BATTLE_HERO_NUM || !wCardSlot || wCardSlot > MAX_CARD_SLOT_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]请求装备卡牌参数出错[formation_pos:%d, card_slot:%d]",
			__FUNCTION__, wFormationPos, wCardSlot);
		return;
	}

	// 卡槽等级限制检测
	const CardSlot_Config *pCardSlotCfg = g_pCfgMgr->GetCardSlot(wCardSlot);
	if (!pCardSlotCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到卡槽配置[card_slot:%d]",__FUNCTION__, wCardSlot);
		return;
	}
	if (m_pPlayer->GetLevel() < pCardSlotCfg->wNeedRoleLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]卡槽需要的角色等级不足[card_slot:%d]", __FUNCTION__, wCardSlot);
		return;
	}

	WORD wSvrFormationPos = wFormationPos - 1;
	WORD wSvrCardSlot = wCardSlot - 1;

	// 检测是否有此卡牌
	const Card *pCard = GetCard(dwUID);
	if (!pCard)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此卡牌[UID:%d]", __FUNCTION__, dwUID);
		return;
	}

	Formation &tFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[wSvrFormationPos];
	DWORD dwOldCardUID = tFormation.arCardList[wSvrCardSlot];
	tFormation.arCardList[wSvrCardSlot] = dwUID;

	// 同步卡牌当前装备状态
	if (dwOldCardUID)
		CardDataNotify(dwOldCardUID);
	CardDataNotify(dwUID);

	// 从新同步阵型列表
	m_pPlayer->GetHeroManager()->SendFormationList();

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_COLLECTION_EQUIP_CARD_RESP msg;
	msg.m_wFormationPos = wFormationPos;
	msg.m_wCardSlot = wCardSlot;
	msg.m_dwCardUID = dwUID;
	m_pPlayer->SendPacket(&msg);
}

// 卸下卡牌
void CZhanBuManager::OnNetUnloadCard(WORD wFormationPos, WORD wCardSlot)
{
	// 检测参数 (阵型位置, 装备位置)
	if (!wFormationPos || wFormationPos > MAX_BATTLE_HERO_NUM || !wCardSlot || wCardSlot > MAX_CARD_SLOT_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]请求装备卡牌参数出错[formation_pos:%d, card_slot:%d]",
			__FUNCTION__, wFormationPos, wCardSlot);
		return;
	}

	WORD wSvrFormationPos = wFormationPos - 1;
	WORD wSvrCardSlot = wCardSlot - 1;

	Formation &tFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[wSvrFormationPos];
	DWORD dwUnloadCardUID = tFormation.arCardList[wSvrCardSlot];
	tFormation.arCardList[wSvrCardSlot] = 0;

	// 同步卡牌当前装备状态
	if (dwUnloadCardUID)
		CardDataNotify(dwUnloadCardUID);

	// 从新同步阵型列表
	m_pPlayer->GetHeroManager()->SendFormationList();

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_COLLECTION_UNLOAD_CARD_RESP msg;
	msg.m_wFormationPos = wFormationPos;
	msg.m_wCardSlot = wCardSlot;
	m_pPlayer->SendPacket(&msg);
}

// 激活成就
void CZhanBuManager::OnNetAcivateAchievement()
{
	// 准备激活的等级
	WORD wReadyActivateLevel = m_pZhanBuInfo->wAchievementLevel + 1;

	const ZhanBuAchievement_Config *pAchievementCfg = g_pCfgMgr->GetZhanBuAchievement(wReadyActivateLevel);
	if (!pAchievementCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到占卜成就配置[level:%d]!", __FUNCTION__, wReadyActivateLevel);
		return;
	}

	// 检测需要的成就值是否足够
	if (pAchievementCfg->dwNeedCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]收藏值不足[level:%d,need_collection_value:%d]!", 
			__FUNCTION__, wReadyActivateLevel, pAchievementCfg->dwNeedCollectionValue);
		return;
	}

	m_pZhanBuInfo->wAchievementLevel = wReadyActivateLevel;

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP msg;
	msg.m_wCurAchievementLevel = wReadyActivateLevel;
	m_pPlayer->SendPacket(&msg);
}

// 取仙灵
const Card* CZhanBuManager::GetCard(DWORD dwUID)
{
	return GetCardInner(dwUID);
}

// 添加仙灵
void CZhanBuManager::AddCard(DWORD dwInfoId, bool bIsNotify, WORD wFromId)
{
	Card tFairy;
	memset(&tFairy, 0, sizeof(tFairy));
	tFairy.dwInfoId = dwInfoId;
	tFairy.dwId = GetNewSeriesId();
	tFairy.wLevel = 1;
	AddCard(&tFairy, bIsNotify, wFromId);
}

// 添加仙灵
void CZhanBuManager::AddCard(const Card *pCard, bool bIsNotify, WORD wFromId)
{
	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]卡牌背包已达上限[RoleId:%d,CardListNum:%d,CardInfoId:%d]", __FUNCTION__,
			m_pPlayer->GetRoleId(), m_pZhanBuInfo->wCardNum, pCard->dwInfoId);

		// 记录英雄丢失记录
		m_pPlayer->SaveItemLoseRecord(pCard->dwInfoId);

		return;
	}

	if (m_pZhanBuInfo->wCardNum >= MAX_CARD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]仙灵列表已满,添加失败!",__FUNCTION__);
		return;
	}

	// 检测配置
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌配置[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	m_pZhanBuInfo->arCardList[m_pZhanBuInfo->wCardNum++] = *pCard;

	// 通知客户端
	if (bIsNotify)
	{
		CardDataNotify(pCard);
	}

	// 记录获得
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), pCard->dwInfoId, 1, true, wFromId);
}

// 删除卡牌
void CZhanBuManager::RemoveCard(DWORD dwUID, bool bIsNotify, WORD wFromId)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		Card *pCard = &m_pZhanBuInfo->arCardList[i];
		if (pCard->dwId == dwUID)
		{
			if (bIsNotify)
				CardDataNotify(pCard,false);

			// 记录获得
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), pCard->dwInfoId, 1, false, wFromId);

			*pCard = m_pZhanBuInfo->arCardList[--m_pZhanBuInfo->wCardNum];
			return;
		}
	}
}

// 批量删除卡牌
bool CZhanBuManager::RemoveCard(DWORD *pCardUIDList, WORD wCardNum, WORD wFromId)
{
	for (WORD i = 0; i < wCardNum; ++i)
	{
		for (WORD k = 0; k < m_pZhanBuInfo->wCardNum; ++k)
		{
			DWORD dwCardUID = pCardUIDList[i];
			Card &tDeleteCard = m_pZhanBuInfo->arCardList[k];
			if (tDeleteCard.dwId == dwCardUID)
			{
				// 记录获得
				g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tDeleteCard.dwInfoId, 1, false, wFromId);

				CardDataNotify(&tDeleteCard, false);
				tDeleteCard = m_pZhanBuInfo->arCardList[--m_pZhanBuInfo->wCardNum];
				break;
			}
		}
	}

	return false;
}

// 取仙灵当前总经验
DWORD CZhanBuManager::GetCardCurTotalExp(DWORD dwUID)
{
	// 取仙灵数据
 	const Card *pCard = GetCard(dwUID);
	if (!pCard)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家卡牌数据[UId:%d]!",__FUNCTION__, dwUID);
 		return 0;
 	}
 
 	// 取卡牌配置
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵的配置数据[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
 		return 0;
 	}
 
 	// 取升级配置
	const CardLevelUp_Config *pLevelUpCfg = g_pCfgMgr->GetCardLevelUp(pCardCfg->byQuality, pCard->wLevel);
 	if (!pLevelUpCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵的升级配置数据[Quality:%d,Level:%d]!",
			__FUNCTION__, pCardCfg->byQuality, pCard->wLevel);
 		return 0;
 	}
 
	return pCard->dwExp + pCardCfg->dwToExp + pLevelUpCfg->dwTotalExp;
}

// 取初始卡牌
const Card* CZhanBuManager::GetInitCard(DWORD dwInfoId)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		Card &tCard = m_pZhanBuInfo->arCardList[i];
		if (tCard.dwInfoId == dwInfoId && IsInitCard(&tCard))
			return &tCard;
	}

	return NULL;
}

// 删除初始卡牌
void CZhanBuManager::RemoveInitCard(DWORD dwInfoId, bool bIsNotify)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		Card &tCard = m_pZhanBuInfo->arCardList[i];
		if (tCard.dwInfoId == dwInfoId && IsInitCard(&tCard))
		{
			CardDataNotify(&tCard, false);
			tCard = m_pZhanBuInfo->arCardList[--m_pZhanBuInfo->wCardNum];
			break;
		}
	}
}

// 是否初始卡牌
bool CZhanBuManager::IsInitCard(const Card *pCard)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	if (!pCard->dwExp && !pCard->wBreakLevel && pCard->wLevel <= 1 && !pHeroMgr->GetCardWearHeroId(pCard->dwId))
		return true;

	return false;
}

// 取初始卡牌数量
DWORD CZhanBuManager::GetInitCardNum(DWORD dwInfoId)
{
	DWORD dwNum = 0;
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		Card *pCard = &m_pZhanBuInfo->arCardList[i];
		if (pCard->dwInfoId == dwInfoId && IsInitCard(pCard))
			++dwNum;
	}

	return dwNum;
}

// 取收藏历史记录
DWORD CZhanBuManager::GetHistoryCollectionValue()
{
	return m_pZhanBuInfo->dwHistoryCollectionValue;
}

// 取指定等级的卡牌数量
int CZhanBuManager::GetCardNumByLevel(WORD wLevel)
{
	int nCardNum = 0;
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		if (m_pZhanBuInfo->arCardList[i].wLevel >= wLevel)
			nCardNum++;
	}
	return nCardNum;
}

// 取指定阶的卡牌数量
int CZhanBuManager::GetCardNumByBreakLevel(WORD wBreakLevel)
{
	int nCardNum = 0;
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		if (m_pZhanBuInfo->arCardList[i].wBreakLevel >= wBreakLevel)
			nCardNum++;
	}
	return nCardNum;
}

// 取仙灵数据
const Card* CZhanBuManager::GetCard(DWORD dwFairyInfoId, WORD wMaxLevel)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		Card *pFairy = &m_pZhanBuInfo->arCardList[i];
		if (pFairy->dwInfoId == dwFairyInfoId && pFairy->wLevel < wMaxLevel)
			return pFairy;
	}
	return NULL;
}


// 取仙灵(内部接口)
Card* CZhanBuManager::GetCardInner(DWORD dwUID)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		if (m_pZhanBuInfo->arCardList[i].dwId == dwUID)
			return &m_pZhanBuInfo->arCardList[i];
	}

	return NULL;
}

// 取一个新的系列id
DWORD CZhanBuManager::GetNewSeriesId()
{
	// 不应该出现这种情况
	if (m_dwSeriesId >= 9999)
	{
		m_dwSeriesId = 1;
		MAPLOG(ERROR_LEVEL, "[%s]仙灵的系列ID出现异常,使用超过9999次!!!", m_pPlayer->GetUTF8RoleName());
	}

	return 100000 * m_pPlayer->GetLoginTimes() + m_dwSeriesId++;
}

// 发送玩家仙灵数据
void CZhanBuManager::SendPlayerZhanBuData()
{
	MSG_S2C_PLAYER_ZHANBU_DATA_RESP msg;
	msg.m_pZhanBuInfo = m_pZhanBuInfo;
	m_pPlayer->SendPacket(&msg);
}

// 卡牌是否已翻开
bool CZhanBuManager::IsCardAlreadyOpen(BYTE byType, WORD wIdx)
{
	if (wIdx >= MAX_CARD_GROUP_NUM)
		return false;

	// 低级卡组
	if (1 == byType)
	{
		if (m_pZhanBuInfo->arLowCardGroup[wIdx].wIsAlreadyOpen)
			return true;
	}
	else
	{
		if (m_pZhanBuInfo->arHighCardGroup[wIdx].wIsAlreadyOpen)
			return true;
	}

	return false;
}

// 更新低级卡组刷新次数
void CZhanBuManager::UpdateLowCardGroupRefreshTimes()
{
	if (m_pZhanBuInfo->dwLowCardGroupLastRecoverTime)
	{
		int nRefreshTimes = DoRecoveryTimes(m_pZhanBuInfo->dwLowCardGroupLastRecoverTime, time(NULL), g_GetConfigValue(239)*60);
		if (nRefreshTimes > 0)
		{
			m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes += nRefreshTimes;

			// 恢复次数上限纠正
			m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes = min(m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes, g_GetConfigValue(240));
		}
	}
}


// 处理低级卡组刷新
bool CZhanBuManager::DoLowCardGroupRefresh(bool bIsNewRole)
{
	int nRandCardNum = 10;
	nRandCardNum = min(nRandCardNum, MAX_CARD_GROUP_NUM);

	DWORD dwCurTime = time(NULL);

	int nCostDiamond = 0;

	// 新建角色不消耗
	if (!bIsNewRole)
	{
		// 没有免费次数
		if (!m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes)
		{
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(241);
			if (!pServerCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置[key:%d]!", __FUNCTION__, 241);
				return false;
			}
			nCostDiamond = pServerCfg->GetListValue(m_pZhanBuInfo->wTodayLowCardGroupCostRefreshTimes);

			// 检测消耗
			if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
			{
				MAPLOG(ERROR_LEVEL, "[%s]低级卡组刷新消耗不足!", __FUNCTION__);
				return false;
			}
		}
	}

	// 取卡组关卡配置
	const CardGroupMap_Config *pMapCfg = g_pCfgMgr->GetCardGroupMap(m_pZhanBuInfo->wLowCardGroupMapIdx);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到低级卡组关卡配置[MapIdx:%d]!", __FUNCTION__, m_pZhanBuInfo->wLowCardGroupMapIdx);
		return false;
	}

	// 取卡组配置
	const vector<RateDataEx> *pCardGroupCfg = g_pCfgMgr->GetLowCardGroup(pMapCfg->wCardGroupId);
	if (!pCardGroupCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到低级卡组配置[CardGroupId:%d]!", __FUNCTION__, pMapCfg->wCardGroupId);
		return false;
	}

	// 检测卡组的数量是否足够
	if (pCardGroupCfg->size() < nRandCardNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]卡组卡牌数量不足[CardGroupId:%d,size:%d]!",
			__FUNCTION__, pMapCfg->wCardGroupId, pCardGroupCfg->size());
		return false;
	}


	// 计算总概率
	int nTotalRate = 0;
	vector<RateDataEx>::const_iterator iter = pCardGroupCfg->begin();
	for (; iter != pCardGroupCfg->end(); ++iter)
	{
		nTotalRate += iter->nRate;
	}

	// 随机指定数量的卡牌
	m_pZhanBuInfo->wLowCardGroupNum = 0;
	memset(m_pZhanBuInfo->arLowCardGroup, 0, sizeof(m_pZhanBuInfo->arLowCardGroup));
	for (int i = 0; i < nRandCardNum; ++i)
	{
		int nRandValue = g_Rand(0, nTotalRate);
		int nCurRate = 0;

		// 计算那个命中
		bool bIsHit = false;
		iter = pCardGroupCfg->begin();
		for (; iter != pCardGroupCfg->end(); ++iter)
		{
			nCurRate += iter->nRate;
			if (nRandValue <= nCurRate)
			{
				m_pZhanBuInfo->arLowCardGroup[m_pZhanBuInfo->wLowCardGroupNum++].dwCardId = iter->nValue;
				bIsHit = true;
				break;
			}
		}


		if (!bIsHit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]本次没有随机到卡牌[RoleId:%d,RandValue:%d,Idx:%d]!",
				__FUNCTION__, m_pPlayer->GetRoleId(), nRandValue,i);
		}
	}

	// 更新当前卡组ID
	m_pZhanBuInfo->wCurLowCardGroupId = pMapCfg->wCardGroupId;

	// 新创建的角色不需要处理
	if (!bIsNewRole)
	{
		// 使用消耗刷新
		if (nCostDiamond > 0)
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CARD_GROUP_REFRESH);
			m_pZhanBuInfo->wTodayLowCardGroupCostRefreshTimes++;
		}
		// 使用免费次数刷新
		else
		{
			m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes--;
		}
	}

	return true;
}


// 处理高级卡组刷新
bool CZhanBuManager::DoHighCardGroupRefresh(bool bIsNewRole)
{
	int nRandCardNum = 10;
	nRandCardNum = min(nRandCardNum, MAX_CARD_GROUP_NUM);

	DWORD dwCurTime = time(NULL);
	int nCostDiamond = 0;

	// 新建角色不消耗
	if (!bIsNewRole)
	{
		// 还没到达定时刷新时间
		if (m_pZhanBuInfo->dwHighCardGroupLastRefreshTime &&
			dwCurTime < m_pZhanBuInfo->dwHighCardGroupLastRefreshTime + g_GetConfigValue(242) * 60)
		{
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(243);
			if (!pServerCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置[key:%d]!", __FUNCTION__, 243);
				return false;
			}
			nCostDiamond = pServerCfg->GetListValue(m_pZhanBuInfo->wTodayHighCardGroupCostRefreshTimes);

			// 检测消耗
			if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
			{
				MAPLOG(ERROR_LEVEL, "[%s]高级卡组刷新消耗不足!", __FUNCTION__);
				return false;
			}
		}
	}

	// 取卡组配置
	const vector<HighCardGroup_Config> *pCardGroupCfg = g_pCfgMgr->GetHighCardGroup();
	if (!pCardGroupCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到高级卡组配置!", __FUNCTION__);
		return false;
	}

	// 检测卡组的数量是否足够
	if (pCardGroupCfg->size() < nRandCardNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]高级卡组卡牌数量不足[size:%d]!", __FUNCTION__, pCardGroupCfg->size());
		return false;
	}

	// 计算总概率
	int nTotalNormalWeight = 0;
	int nTotalMustWeight = 0;
	vector<HighCardGroup_Config>::const_iterator iter = pCardGroupCfg->begin();
	for (; iter != pCardGroupCfg->end(); ++iter)
	{
		nTotalNormalWeight += iter->nNormalWeight;
		nTotalMustWeight += iter->nMustWeight;
	}

	// 随机保底卡牌
	DWORD dwMustCardId = 0;
	{
		int nRandValue = g_Rand(0, nTotalMustWeight);
		int nCurWeight = 0;
		// 计算那个命中
		iter = pCardGroupCfg->begin();
		for (; iter != pCardGroupCfg->end(); ++iter)
		{
			nCurWeight += iter->nMustWeight;
			if (iter->nMustWeight > 0 && nRandValue <= nCurWeight)
			{
				dwMustCardId = iter->dwCardId;
				break;
			}
		}
	}

	// 随机普通卡牌
	m_pZhanBuInfo->wHighCardGroupNum = 0;
	memset(m_pZhanBuInfo->arHighCardGroup, 0, sizeof(m_pZhanBuInfo->arHighCardGroup));
	for (int i = 0; i < nRandCardNum; ++i)
	{
		int nRandValue = g_Rand(0, nTotalNormalWeight);
		int nCurWeight = 0;

		// 计算那个命中
		iter = pCardGroupCfg->begin();
		for (; iter != pCardGroupCfg->end(); ++iter)
		{
			nCurWeight += iter->nNormalWeight;
			if (iter->nNormalWeight > 0 && nRandValue < nCurWeight)
			{
				m_pZhanBuInfo->arHighCardGroup[m_pZhanBuInfo->wHighCardGroupNum++].dwCardId = iter->dwCardId;
				break;
			}
		}
	}

	// 随机个位置插入保底卡牌
	if (m_pZhanBuInfo->wHighCardGroupNum)
	{
		m_pZhanBuInfo->arHighCardGroup[g_Rand(0, m_pZhanBuInfo->wHighCardGroupNum - 1)].dwCardId = dwMustCardId;
	}

	// 新创建的角色不需要处理
	if (!bIsNewRole)
	{
		// 使用消耗刷新
		if (nCostDiamond > 0)
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CARD_GROUP_REFRESH);
			m_pZhanBuInfo->wTodayHighCardGroupCostRefreshTimes++;
		}
		// 使用定时刷新
		else
		{
			m_pZhanBuInfo->dwHighCardGroupLastRefreshTime = dwCurTime;
		}
	}

	return true;
}

// 卡牌数据通知
void CZhanBuManager::CardDataNotify(DWORD dwUID, BYTE byFlag)
{
	const Card *pCard = GetCard(dwUID);
	if (pCard)
		CardDataNotify(pCard, byFlag);
}

// 卡牌数据通知
void CZhanBuManager::CardDataNotify(const Card *pCard, BYTE byFlag)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_FAIRY_DATA_NOTICE msg;
	msg.m_byFlag = byFlag;
	memcpy_s(&msg.m_tCard, sizeof(Card), pCard, sizeof(Card));
	msg.m_tCard.dwHeroId = pHeroMgr->GetCardWearHeroId(pCard->dwId);
	m_pPlayer->SendPacket(&msg);
}

// 取收藏书数据
CollectionBook* CZhanBuManager::GetCollectionBook(WORD wBookId)
{
	// 找到则直接返回
	for (WORD i = 0; i < m_pZhanBuInfo->wCollectionBookNum; ++i)
	{
		if (m_pZhanBuInfo->arCollectionBookList[i].wBookId == wBookId)
			return &m_pZhanBuInfo->arCollectionBookList[i];
	}

	if (m_pZhanBuInfo->wCollectionBookNum >= MAX_COLLECTION_BOOK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]收藏书列表已满[size:%d]!", __FUNCTION__, m_pZhanBuInfo->wCollectionBookNum);
		return NULL;
	}

	// 找不到则添加
	CollectionBook tBook;
	memset(&tBook, 0, sizeof(tBook));
	tBook.wBookId = wBookId;
	m_pZhanBuInfo->arCollectionBookList[m_pZhanBuInfo->wCollectionBookNum++] = tBook;

	return &m_pZhanBuInfo->arCollectionBookList[m_pZhanBuInfo->wCollectionBookNum-1];
}

// 取收藏组数据
CollectionGroup* CZhanBuManager::GetCollectionGroup(WORD wBookId, WORD wGroupId)
{
	CollectionBook *pBook = GetCollectionBook(wBookId);
	if (pBook)
	{
		for (WORD i = 0; i < pBook->wCollectionGroupNum; ++i)
		{
			if (pBook->arCollectionGroupList[i].wId == wGroupId)
				return &pBook->arCollectionGroupList[i];
		}
	}

	return NULL;
}

// 添加收藏组数据
void CZhanBuManager::AddCollectGroup(WORD wBookId, WORD wGroupId)
{
	CollectionBook *pBook = GetCollectionBook(wBookId);
	if (!pBook)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏书数据[BookId:%d]!", __FUNCTION__, wBookId);
		return;
	}

	if (pBook->wCollectionGroupNum >= MAX_COLLECTION_GROUP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]收藏组列表已满[BookId:%d,GroupId:%d,Num:%d]!",
			__FUNCTION__, wBookId, wGroupId, pBook->wCollectionGroupNum);
		return;
	}

	CollectionGroup tGroup;
	tGroup.wId = wGroupId;
	tGroup.wStar = 1;
	pBook->arCollectionGroupList[pBook->wCollectionGroupNum++] = tGroup;
}

// 删除收藏组数据
void CZhanBuManager::RemoveCollectGroup(WORD wBookId, WORD wGroupId)
{
	CollectionBook *pBook = GetCollectionBook(wBookId);
	if (!pBook)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到收藏书数据[BookId:%d]!", __FUNCTION__, wBookId);
		return;
	}

	for (WORD i = 0; i < pBook->wCollectionGroupNum; ++i)
	{
		if (pBook->arCollectionGroupList[i].wId == wGroupId)
		{
			pBook->arCollectionGroupList[i] = pBook->arCollectionGroupList[--pBook->wCollectionGroupNum];
			break;
		}
	}
}

// 检测是否已初始化数据
bool CZhanBuManager::IsAlreadyInitData()
{
	return m_pZhanBuInfo->wLowCardGroupMapIdx ? true : false;
}

// 初始数据
void CZhanBuManager::InitData()
{
	m_pZhanBuInfo->wLowCardGroupMapIdx = 1;	// 关卡从1开始
	m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes = 1; // 初始化时给一次
	m_pZhanBuInfo->dwLowCardGroupLastRecoverTime = time(NULL); // 

	DoLowCardGroupRefresh(true);
	DoHighCardGroupRefresh(true);
}

// 取当前收藏值
DWORD CZhanBuManager::GetCurCollectionValue()
{
	DWORD dwCollectionValue = 0;
	const CollectionGroup_Config *pGroupCfg = NULL;
	for (WORD i = 0; i < m_pZhanBuInfo->wCollectionBookNum; ++i)
	{
		CollectionBook &tBook = m_pZhanBuInfo->arCollectionBookList[i];
		for (WORD k = 0; k < tBook.wCollectionGroupNum; ++k)
		{
			CollectionGroup &tGroup = tBook.arCollectionGroupList[k];
			pGroupCfg = g_pCfgMgr->GetCollectionGroup(tGroup.wId);
			if (pGroupCfg && tGroup.wStar)
			{
				// 激活收藏值
				dwCollectionValue += pGroupCfg->nActivateCollectionValue; 

				// 升级收藏值
				dwCollectionValue += (pGroupCfg->nUpgradeCollectionValue * (tGroup.wStar-1));
			}
		}
	}

	return dwCollectionValue;
}

// 取包裹空闲数量
WORD CZhanBuManager::GetBagEmptySize()
{
	WORD wBagSize = min(MAX_CARD_NUM, m_pPlayer->GetVipValue(32));
	return wBagSize > m_pZhanBuInfo->wCardNum ? wBagSize - m_pZhanBuInfo->wCardNum : 0;
}

// 更新历史收藏值
void CZhanBuManager::UpdateHistoryCollectionValue()
{
	DWORD dwCollectionValue = GetCurCollectionValue();
	if (dwCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
	{
		m_pZhanBuInfo->dwHistoryCollectionValue = dwCollectionValue;

		// 节日活动
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 31, dwCollectionValue, false);
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 31, dwCollectionValue, false);
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 31, dwCollectionValue, false);
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 31, dwCollectionValue, false);

		// 更新排行榜
		CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, m_pPlayer, dwCollectionValue);
	}
}