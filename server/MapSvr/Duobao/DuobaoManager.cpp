#include "stdafx.h"
#include "DuobaoManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\PlayerManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Email\EmailManager.h"


CDuobaoManager::CDuobaoManager()
{

}

CDuobaoManager::~CDuobaoManager()
{

}

void CDuobaoManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerDuobaoInfo = &pPlayer->GetRoleFullInfo().tDuobaoInfo;
}

// 请求夺宝数据
void CDuobaoManager::OnNetDuobaoData()
{
	SendDuobaoData();
}

// 请求挑战目标列表
void CDuobaoManager::OnNetTargetList(WORD wTreasureInfoId, WORD wTreasureItemId)
{
	// 检测参数是否合法
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(wTreasureInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物的合成配置数据[InfoId:%d]", __FUNCTION__, wTreasureInfoId);
		return;
	}
	if (!pComposeCfg->IsCostItem(wTreasureItemId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]不是宝物的合成碎片[InfoId:%d,ItemId:%d]",
			__FUNCTION__, wTreasureInfoId, wTreasureItemId);
		return;
	}

	MSG_S2C_DUOBAO_TARGET_LIST_RESP msg;
	msg.m_wTreasureInfoId = wTreasureInfoId;
	msg.m_wTreasureItemId = wTreasureItemId;
	msg.m_byMonsterNum = 0;
	msg.m_byPlayerNum = 0;

	// 查找在线玩家
	{
 		WORD wOffsetLevel = 5;
 		Player* pPlayerList[100];
 		int nCount = 0;
 		PlayerManager::Instance()->GetAllPlayer(pPlayerList, nCount, 100, m_pPlayer->GetServerId());
 		WORD wMinLevel = m_pPlayer->GetLevel() >= wOffsetLevel ? m_pPlayer->GetLevel() - wOffsetLevel : 1;
 		WORD wMaxLevel = m_pPlayer->GetLevel() + wOffsetLevel;
 		for (int i = 0; i < nCount; ++i)
 		{
 			Player *pTarget = pPlayerList[i];
 			if (pTarget->GetCharID() != m_pPlayer->GetCharID() &&							// 排除自己
 				pTarget->GetLevel() >= wMinLevel && pTarget->GetLevel() <= wMaxLevel &&		// 指定等级范围内
 				pTarget->GetItemManager()->GetItem(wTreasureItemId) &&						// 拥有要抢夺的物品
				!pTarget->GetDuobaoManager()->IsInPeace() &&								// 非免战状态
				pTarget->GetLevel() >= 15)	// 15级以上才开放 todo::要调整为配置文件								
 			{
				DuobaoTargetPlayerData &tTargetData = msg.m_PlayerList[msg.m_byPlayerNum++];
				memset(&tTargetData, 0, sizeof(tTargetData));
				tTargetData.dwRoleId = pTarget->GetCharID();
				tTargetData.wLevel = pTarget->GetLevel();
				tTargetData.dwHeadImage = pTarget->GetProperty(ROLE_PRO_HEAD_IMAGE);
				tTargetData.dwFightingPower = pTarget->GetFightingPower();
				strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pTarget->GetRoleName());
				CHeroManager *pHeroMgr = pTarget->GetHeroManager();
				CHECK_POINTER(pHeroMgr);
				tTargetData.byToBattleHeroNum = MAX_BATTLE_HERO_NUM;
				for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
				{
					const Formation *pFormation = pHeroMgr->GetFormationByIndex(i);
					const HeroInfo *pHero = pFormation ? pHeroMgr->GetHero(pFormation->dwHeroUID) : NULL;
					if (pHero)
						tTargetData.arToBattleHeroList[i] = pHero->dwIntoId;
				}

				if (msg.m_byPlayerNum >= 2)
 					break;
 			}
 		}
	}

	// todo::查找离线玩家

	// 查找怪物
	const DuobaoMap_Config *pDuobaoMapCfg = CConfigManager::Instance()->GetDuobaoMap(m_pPlayer->GetLevel());
	if (!pDuobaoMapCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到夺宝关卡配置[InfoId:%d,ItemId:%d]",
			__FUNCTION__, wTreasureInfoId, wTreasureItemId);
		return;
	}
	if (!pDuobaoMapCfg->vMapList.size() || pDuobaoMapCfg->vMapList.size() < DUOBAO_TARGET_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]夺宝关卡配置列表出错(为空或者小于指定数量)![InfoId:%d,ItemId:%d,ListSize:%d]",
			__FUNCTION__, wTreasureInfoId, wTreasureItemId, pDuobaoMapCfg->vMapList.size());
		return;
	}
	while (msg.m_byPlayerNum + msg.m_byMonsterNum < DUOBAO_TARGET_NUM)
	{
		int nRandIndex = Rand::Instance()->irand(0, pDuobaoMapCfg->vMapList.size() - 1);
		msg.m_MonsterList[msg.m_byMonsterNum++].wMapId = pDuobaoMapCfg->vMapList[nRandIndex];
	}

	m_pPlayer->SendPacket(&msg);
}

// 请求5次夺宝(todo::5次夺宝的目标一定不会是玩家)
void CDuobaoManager::OnNetDuoBaoFiveTimes(WORD wTreasureInfoId, WORD wTreasureItemId, WORD wRequestTimes)
{
	// 暂时不开放
// 	m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
// 	return;

	// 扫荡的一定是碎片
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(wTreasureItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物碎片配置[InfoId:%d,TreasureItemId:%d]", 
			__FUNCTION__, wTreasureInfoId, wTreasureItemId);
		return;
	}

	// 检测VIP
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < m_pPlayer->GetVipValue(VIP_FIVE_TIMES_DB_FLAG))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]5次夺宝需要的VIP等级不足", __FUNCTION__);
		return;
	}

	// 取宝物配置
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(wTreasureInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物配置[InfoId:%d]", __FUNCTION__, wTreasureInfoId);
		return;
	}

	// 取夺宝关卡配置
	const DuobaoMap_Config *pMapCfg = CConfigManager::Instance()->GetDuobaoMap(m_pPlayer->GetLevel());
	if (!pMapCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到夺宝关卡配置[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	// 取掉落配置
	const DuobaoDrop_Config *pDropCfg = CConfigManager::Instance()->GetDuobaoDrop(pTreasureCfg->wQuality);
	if (!pDropCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到夺宝掉落配置[quality:%d]", __FUNCTION__, pTreasureCfg->wQuality);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	vector<Item> vItemList;
	WORD wDoTimes = 0;
	BYTE byFlag = false;
	for (int i = 0; i < wRequestTimes; ++i)
	{
		// 检测体力是否足够
		if (m_pPlayer->GetProperty(ROLE_PRO_ENERGY) < 2)
			break;

		++wDoTimes;
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, pMapCfg->nWinGold, TRUE, FROM_DOUBAO);
		m_pPlayer->AddExp(pMapCfg->dwExp);
		m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -2, TRUE, FROM_DOUBAO);

		// 翻牌	
		DWORD dwPFGitfId = 0;
		Item tPFItem;
		vector<Item> vDropRewardItemList;
		m_pPlayer->GetFubenManager()->DoLimitDrop(999, vDropRewardItemList);
		if (vDropRewardItemList.size())
		{
			// 正常不会有多个,如果出现多个,是只取第一个
			tPFItem = vDropRewardItemList[0];
		}
		else
		{
			// 如果没有限时掉落的,则取正常的
			tPFItem = Item(pMapCfg->wPFGitfId, 1);
		}

		if (!pItemMgr->UseRandItem(tPFItem.dwId, tPFItem.dwNum, vItemList))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]夺宝使用翻牌物品失败[ItemId:%d]", __FUNCTION__, pMapCfg->wPFGitfId);
			return;
		}
		// 一定是会只获得一个
		if (i + 1 != vItemList.size())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]夺宝翻牌产生的物品不匹配[ItemId:%d]", __FUNCTION__, pMapCfg->wPFGitfId);
			return;
		}

		WORD wRate = pDropCfg->wRobotRate;

		// todo::如果是第一次蓝色宝物,则必得(新手引导)
		if (!m_pPlayer->CheckOnceFlag(once_get_purple_treasure) && QUALITY_BLUE == pTreasureCfg->wQuality)
		{
			m_pPlayer->SetOnceFlag(once_get_purple_treasure);
			wRate = RAND_RANGE_MAX_10000;
		}

		// 夺宝到一定次数,则必定会获得
		WORD wDoubaoTimes = GetDoubaoTimes(pTreasureCfg->wQuality);
		if (wDoubaoTimes >= pDropCfg->wTimesOfSureDrop)
		{
			wRate = RAND_RANGE_MAX_10000;

			// 重置次数
			SetDoubaoTimes(pTreasureCfg->wQuality, 0);
		}

		// 命中(夺到想要的物品后就退出)
		if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) <= wRate)
		{
			// 挑战者获得物品
			m_pPlayer->AddObject(wTreasureItemId, 1, TRUE, FROM_FIVE_TIMES_DOUBAO);
			byFlag = true;
			break;
		}

		// 记录夺宝次数
		SetDoubaoTimes(pTreasureCfg->wQuality, wDoubaoTimes+1);
	}

	// 添加获得的物品
	m_pPlayer->AddObjectList(vItemList, TRUE, FROM_FIVE_TIMES_DOUBAO);

	// 8-14天活动事件(挑战夺宝（无论胜负）（包括战5次）)
	m_pPlayer->OnUpdateSevenDayTarget(21, wDoTimes, true);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 7, wDoTimes, true);

	// 节日活跃目标
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 7, wDoTimes, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 7, wDoTimes, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 7, wDoTimes, true);

	// todo::为了查找BUG
	for (size_t i = 0; i < vItemList.size(); ++i)
	{
		if (!vItemList[i].dwId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]夺宝翻牌产生的物品ID为空[ItemId:%d]", __FUNCTION__, pMapCfg->wPFGitfId);
		}
	}

	m_pPlayerDuobaoInfo->wDailyDoubaoTimes += wDoTimes;

	// 更新免战时间
	SetPeaceOverTime(0);
	SendDuobaoData();

	MSG_S2C_DUOBAO_FIVE_TIMES_RESP msg;
	msg.m_ItemList = vItemList;
	msg.m_byFlag = byFlag;
	//msg.m_byTimes = byTimes;
	m_pPlayer->SendPacket(&msg);

	// 日常任务事件
	m_pPlayer->OnUpdateDailyMission(11, 5);
}

// 领取进度奖励
void CDuobaoManager::OnNetTakePregrossReward(WORD *pPregrossIdList, WORD wPregrossIdNum)
{
	for (WORD i = 0; i < wPregrossIdNum; ++i)
	{
		WORD wPregrossId = pPregrossIdList[i];

		// 取配置
		const DuobaoPregrossReward_Config *pRewardCfg = g_pCfgMgr->GetDuobaoPregrossReward(wPregrossId);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到夺宝进度奖励配置[PregrossId:%d]", __FUNCTION__, wPregrossId);
			continue;
		}

		// 检测是否完成
		if (m_pPlayerDuobaoInfo->wDailyDoubaoTimes < pRewardCfg->wTimes)
		{
			MAPLOG(ERROR_LEVEL, "[%s]夺宝进度需要的次数不足[PregrossId:%d]", __FUNCTION__, wPregrossId);
			continue;
		}

		// 检测是否已领取过
		if (IsPregrossRewardRecord(wPregrossId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]夺宝进度奖励已领取过[PregrossId:%d]", __FUNCTION__, wPregrossId);
			continue;
		}

		// 添加奖励记录
		AddPregrossRewardRecord(wPregrossId);

		// 添加奖励
		m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_DOUBAO_PREGROSS_REWARD);
	}

	MSG_S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP msg;
	msg.m_wPreprossIdNum = wPregrossIdNum;
	memcpy_s(msg.m_PregrossIdList, sizeof(msg.m_PregrossIdList), pPregrossIdList, wPregrossIdNum * sizeof(WORD));
	m_pPlayer->SendPacket(&msg);
}

// 发送夺宝数据
void CDuobaoManager::SendDuobaoData()
{
	MSG_S2C_DUOBAO_DATA_RESP msg;
	msg.m_DuobaoInfo = *m_pPlayerDuobaoInfo;
	m_pPlayer->SendPacket(&msg);
}

// 挑战开始
bool CDuobaoManager::BattleStar(DWORD dwTargetId, bool bIsPlayer, WORD wTreasureInfoId, WORD wTreasureItemId)
{
	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// 检测体力
	if (m_pPlayer->GetProperty(ROLE_PRO_ENERGY) < 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]夺宝精力不足", __FUNCTION__);
		return false;
	}

	pFBMgr->SetBattleInfo(0, 0, 0);
	pFBMgr->SetTargetId(dwTargetId);
	pFBMgr->SetParam(0, wTreasureInfoId);
	pFBMgr->SetParam(1, wTreasureItemId);
	pFBMgr->SetParam(2, bIsPlayer);

	// 如果是玩家
	if (bIsPlayer)
	{
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId, true);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// 设置战力
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}
	return true;
}

// 挑战结束
bool CDuobaoManager::BattleOver(bool bIsWin)
{	
	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
	WORD wTreasureInfoId = pFBMgr->GetParam(0);
	WORD wTreasureItemId = pFBMgr->GetParam(1);
	BYTE bIsPlayer = pFBMgr->GetParam(2);
	BYTE byIsGetItem;
	return DoBattleOver(pFBMgr->GetTargetId(), wTreasureInfoId, wTreasureItemId, bIsPlayer, bIsWin, byIsGetItem);
}

// 设置免战结束时间 
void CDuobaoManager::SetPeaceOverTime(DWORD dwTime)
{
	m_pPlayerDuobaoInfo->dwPeaceOverTime = dwTime;
	//m_pPlayer->SetProperty(ROLE_PRO_DUOBAO_PEACE_OVER_TIME, dwTime);

	// 主动同步数据
	SendDuobaoData();
}

// 是否免战中
bool CDuobaoManager::IsInPeace()
{
	return time(NULL) < m_pPlayerDuobaoInfo->dwPeaceOverTime;
}

bool CDuobaoManager::DoBattleOver(DWORD dwTargetId, WORD wTreasureInfoId, WORD wTreasureItemId, BYTE byIsPlayer, bool bIsWin, BYTE &byIsGetItem)
{
	byIsGetItem = false;

	// 取宝物配置
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(wTreasureInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物配置[InfoId:%d]", __FUNCTION__, wTreasureInfoId);
		return false;
	}

	// 取夺宝关卡配置
	const DuobaoMap_Config *pMapCfg = CConfigManager::Instance()->GetDuobaoMap(m_pPlayer->GetLevel());
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到夺宝关卡配置[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return false;
	}

	// 取掉落配置
	const DuobaoDrop_Config *pDropCfg = CConfigManager::Instance()->GetDuobaoDrop(pTreasureCfg->wQuality);
	if (!pDropCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到夺宝掉落配置[quality:%d]", __FUNCTION__, pTreasureCfg->wQuality);
		return false;
	}

	// 获胜
	if (bIsWin)
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, pMapCfg->nWinGold, TRUE, FROM_DOUBAO);

		// 命中
		int nRate = byIsPlayer ? pDropCfg->wUserRate : pDropCfg->wRobotRate;

		// todo::如果是第一次紫色宝物,则必得
		if (!m_pPlayer->CheckOnceFlag(once_get_purple_treasure) && QUALITY_BLUE == pTreasureCfg->wQuality)
		{
			m_pPlayer->SetOnceFlag(once_get_purple_treasure);
			nRate = RAND_RANGE_MAX_10000;
		}

		// 夺宝到一定次数时,必定掉落
		WORD wDoubaoTimes = GetDoubaoTimes(pTreasureCfg->wQuality);
		if (wDoubaoTimes >= pDropCfg->wTimesOfSureDrop)
			nRate = RAND_RANGE_MAX_10000;

		if (nRate && Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) <= nRate)
		{
			// 被挑战者失去物品
			if (byIsPlayer)
			{
				Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
				CItemManager *pItemMgr = pTarget ? pTarget->GetItemManager() : NULL;
				if (pItemMgr && pItemMgr->GetItemNum(wTreasureItemId))
					pItemMgr->RemoveItem(wTreasureItemId, 1, TRUE, FROM_DOUBAO);

				// 发邮件通知(夺宝)
				EmailInfo tEailInfo(5);
				tEailInfo.AddContext(EMAIL_CONTEXT_ITEM, wTreasureItemId);
				tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
				EmailManager::AddEmail(dwTargetId, tEailInfo, true);
			}

			// 挑战者获得物品
			m_pPlayer->AddObject(wTreasureItemId, 1, TRUE, FROM_DOUBAO);
			byIsGetItem = true;
		}

		// 添加翻牌机会
		//m_pPlayer->GetItemManager()->SetFPItemId(pMapCfg->wPFGitfId);
		// 添加可翻牌
		vector<Item> vDropRewardItemList;
		m_pPlayer->GetFubenManager()->DoLimitDrop(999, vDropRewardItemList);
		if (vDropRewardItemList.size())
		{
			// 正常不会有多个,如果出现多个,是只取第一个
			m_pPlayer->GetItemManager()->SetFPItem(vDropRewardItemList[0]);
		}
		else
		{
			// 如果没有限时掉落的,则取正常的
			m_pPlayer->GetItemManager()->SetFPItem(Item(pMapCfg->wPFGitfId, 1));
		}

		// 设置夺宝次数
		SetDoubaoTimes(pTreasureCfg->wQuality, byIsGetItem ? 0 : (wDoubaoTimes + 1));
	}
	// 失败
	else
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, pMapCfg->nFailGold, TRUE, FROM_DOUBAO);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -2, TRUE, FROM_DOUBAO);

	m_pPlayer->AddExp(pMapCfg->dwExp);

	m_pPlayer->GetFubenManager()->SetParam(0, byIsGetItem); 

	// 更新夺宝次数
	m_pPlayerDuobaoInfo->wDailyDoubaoTimes++;

	// 更新免战时间
	SetPeaceOverTime(0);
	SendDuobaoData();

	// 日常任务事件
	m_pPlayer->OnUpdateDailyMission(11);

	// 8-14天活动事件(挑战夺宝（无论胜负）（包括战5次）)
	m_pPlayer->OnUpdateSevenDayTarget(21, 1, true);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 7, 1, true);

	// 节日活跃目标
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 7, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 7, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 7, 1, true);

	return true;
}

// 取夺宝次数
WORD CDuobaoManager::GetDoubaoTimes(WORD wQuality)
{
	if (wQuality >= QUALITY_NUM)
		return 0;

	return m_pPlayerDuobaoInfo->arDoubaoTimes[wQuality];
}

// 取夺宝次数
void CDuobaoManager::SetDoubaoTimes(WORD wQuality, WORD wTimes)
{
	if (wQuality >= QUALITY_NUM)
		return;

	m_pPlayerDuobaoInfo->arDoubaoTimes[wQuality] = wTimes;
}

// 是否进度奖励记录
bool CDuobaoManager::IsPregrossRewardRecord(WORD wPregrossId)
{
	for (WORD i = 0; i < m_pPlayerDuobaoInfo->wPregrossRewardRecordNum; ++i)
	{
		if (m_pPlayerDuobaoInfo->arPregrossRewardRecord[i] == wPregrossId)
			return true;
	}
	return false;
}

// 添加进度奖励记录
void CDuobaoManager::AddPregrossRewardRecord(WORD wPregrossId)
{
	if (m_pPlayerDuobaoInfo->wPregrossRewardRecordNum >= MAX_DOUBAO_PREGROSS_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]夺宝进度奖励记录列表已满[MaxNum:%d]", __FUNCTION__, MAX_DOUBAO_PREGROSS_NUM);
		return;
	}

	m_pPlayerDuobaoInfo->arPregrossRewardRecord[m_pPlayerDuobaoInfo->wPregrossRewardRecordNum++] = wPregrossId;
}

