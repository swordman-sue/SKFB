#include "stdafx.h"
#include "HeroManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\Property\PropertyManager.h"
#include "..\PlayerManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Activity\ActivityManager.h"
#include "..\ZhanBu\ZhanBuManager.h"
#include "DBCDefine.h"


CHeroManager::CHeroManager()
{
	m_dwSeriesId = 0;
}

CHeroManager::~CHeroManager()
{

}

void CHeroManager::Init(Player *pPlayer)
{
	m_dwSeriesId = 0;
	m_pPlayer = pPlayer;
	m_pPlayerHeroInfo = &m_pPlayer->GetRoleFullInfo().tHeroInfo;
}

// 上阵英雄
void CHeroManager::OnHeroBattle(BYTE byPos, DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_HERO_TO_BATTLE_REQ;

	// 检测是否拥有此英雄
	const HeroInfo *pHero = GetHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]还没获得此英雄,不能上阵![HeroUID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	if (!byPos || byPos > MAX_BATTLE_HERO_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]请求英雄上阵的位置不合法![Pos:%d]", __FUNCTION__, byPos);
		return;
	}

	BYTE byIndexPos = byPos - 1;

	// 取位置配置数据
 	const HeroToBattle_Config *pToBattleCfg = CConfigManager::Instance()->GetHeroToBattle(1,byPos);
 	if (!pToBattleCfg)
 	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
 		MAPLOG(ERROR_LEVEL, "[%s]找不到上阵英雄位置配置数据![Pos:%d]", __FUNCTION__, byPos);
 		return;
 	}
 
 	// 检测开放等级
 	if (pToBattleCfg->wNeedLevel > m_pPlayer->GetLevel())
 	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
 		MAPLOG(ERROR_LEVEL, "[%s]等级限制还不能上阵![Pos:%d,NeedLevel:%d]", __FUNCTION__, byPos, pToBattleCfg->wNeedLevel);
 		return;
 	}

	// 同一种英雄不能同时上阵
	if (!IsCanToBattle(pHero->dwIntoId, byPos))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]同一种英雄不能同时上阵![HeroInfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	DWORD dwOldHeroUID = m_pPlayerHeroInfo->arFormationList[byIndexPos].dwHeroUID;
	// 记录上阵位置
	m_pPlayerHeroInfo->arFormationList[byIndexPos].dwHeroUID = dwHeroUID;

	// 上一个上阵英雄状态去掉
	if (dwOldHeroUID)
		HeroBaseDataNotify(dwOldHeroUID);
	HeroBaseDataNotify(dwHeroUID);

	SendFormationList();

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_TO_BATTLE_RESP msg;
	msg.m_dwUId = dwHeroUID;
	msg.m_byPos = byPos;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(21, GetToBattleHeroNum(), false);

	// 成就
	m_pPlayer->OnUpdateAchievement(9, GetFormationHeroMinLevel(), false);
}

// 援军上阵
void CHeroManager::OnReinforcementBattle(BYTE byPos, DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_REINFORCEMENT_TO_BATTLE_REQ;

	if (!byPos || byPos > MAX_REINFORCEMENT_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]请求援军上阵的位置不合法![Pos:%d]", __FUNCTION__,byPos);
		return;
	}

	BYTE bySlotIndex = byPos - 1;

	// 上阵
	if (dwHeroUID)
	{
		// 检测是否拥有此英雄
		const HeroInfo *pHero = GetHero(dwHeroUID);
		if (!pHero)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]还没获得此英雄,不能上阵![HeroUID:%d]", __FUNCTION__, dwHeroUID);
			return;
		}

		// 取位置配置数据
		const HeroToBattle_Config *pToBattleCfg = CConfigManager::Instance()->GetHeroToBattle(2, byPos);
		if (!pToBattleCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]找不到上阵英雄位置配置数据![Pos:%d]", __FUNCTION__, byPos);
			return;
		}

		// 检测开放等级
		if (pToBattleCfg->wNeedLevel > m_pPlayer->GetLevel())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]等级限制还不能上阵![Pos:%d,NeedLevel:%d]", 
				__FUNCTION__, byPos, pToBattleCfg->wNeedLevel);
			return;
		}

		// 同一种英雄不能同时上阵
		if (!IsCanToBattleEx(pHero->dwIntoId, byPos))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]同一种英雄不能同时上阵![InfoId:%d]",__FUNCTION__, pHero->dwIntoId);
			return;
		}
	}
	// 下阵
	else
	{ 
		// todo::暂时不需要处理逻辑
	}

	DWORD dwOldHeroUID = m_pPlayerHeroInfo->arReinforcementList[bySlotIndex];
	m_pPlayerHeroInfo->arReinforcementList[bySlotIndex] = dwHeroUID;

	// 被替换的英雄
	if (dwOldHeroUID)
		HeroBaseDataNotify(dwOldHeroUID);

	// 新上阵的英雄
	if (dwHeroUID)
		HeroBaseDataNotify(dwHeroUID);

	SendFormationList();

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_REINFORCEMENT_TO_BATTLE_RESP msg;
	msg.m_byPos = byPos;
	msg.m_dwUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);
}

// 合成
void CHeroManager::OnCompose(WORD wHeroInfoId)
{
	WORD wFromMsgId = C2S_HERO_COMPOSE_REQ;

	const HeroCompose_Config *pComposeCfg = CConfigManager::Instance()->GetHeroCompose(wHeroInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "找不到英雄配置[InfoId:%d]", wHeroInfoId);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 检测是否有足够的道具
	if (pItemMgr->GetItemNum(pComposeCfg->dwItemId) < pComposeCfg->wItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		// 提示物品数量不足
		MAPLOG(ERROR_LEVEL, "[%s]英雄合成需要消耗的物品不足[HeroInfoId:%d,CostItemId:%d,CostItemNum:%d]", 
			__FUNCTION__, wHeroInfoId, pComposeCfg->dwItemId, pComposeCfg->wItemNum);
		return;
	}

	// 添加英雄
	if (!AddHero(wHeroInfoId, TRUE, FROM_HERO_COMPOSE))
	{
		// 提示添加英雄失败
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]玩家添加英雄失败[InfoId:%d]", __FUNCTION__, wHeroInfoId);
		return;
	}

	// 处理消耗
	pItemMgr->RemoveItem(pComposeCfg->dwItemId, pComposeCfg->wItemNum, TRUE, FROM_HERO_COMPOSE);

	MSG_S2C_HERO_COMPOSE_RESP msg;
	msg.m_wHeroInfoId = wHeroInfoId;
	m_pPlayer->SendPacket(&msg);
}

// 召唤
void CHeroManager::OnSummon(BYTE byType)
{
	WORD wFromMsgId = C2S_HERO_SUMMON_REQ;

	int nSummonTimes = 1;

	vector<DWORD> vOutHeroList;
	bool bRet = false;
	switch (byType)
	{
	case NORMAL_FREE_SUMMON:
		bRet = DoNormalSummon(vOutHeroList);
		break;
	case NORMAL_ITEM_SUMMON:
		bRet = DoNormalSummon(vOutHeroList,g_GetConfigValue(CONFIG_NORMAL_SUMMON_COST_ITEM));
		break;
	case NORMAL_ITEM_ONEKEY_SUMMON:
		{
			DWORD dwCostItemId = g_GetConfigValue(CONFIG_NORMAL_SUMMON_COST_ITEM);
			BYTE byOneKeyTimes = g_GetConfigValue(CONFIG_NORMAL_ONEKEY_SUMMON_TIMES);
			for (BYTE i = 0; i < byOneKeyTimes; ++i)
			{
				bRet = DoNormalSummon(vOutHeroList,dwCostItemId);
			}
			nSummonTimes = byOneKeyTimes;
		}
		break;
	case FRIENDSHIP_POINT_SUMMON:
		bRet = DoNormalSummon(vOutHeroList, 0, g_GetConfigValue(129,1));
		break;
	case ADVANCED_FREE_SUMMON:
		bRet = DoAdvanceSummon(vOutHeroList);
		break;
	case ADVANCED_ITEM_SUMMON:
		bRet = DoAdvanceSummon(vOutHeroList,g_GetConfigValue(CONFIG_ADVANCED_SUMMON_COST_ITEM));
		break;
	case ADVANCED_GOLD_SUMMON:
		bRet = DoAdvanceSummon(vOutHeroList,0,g_GetConfigValue(CONFIG_ADVANCED_SUMMON_COST_YB));
		break;
	case ADVANCED_GOLD_ONEKEY_SUMMON:
		{
			int nCostDiamond = g_GetConfigValue(CONFIG_ADVANCED_ONEKEY_SUMMON_COST_YB);
			int nOneKeySummonTimes = g_GetConfigValue(CONFIG_ADVANCED_ONEKEY_SUMMON_TIMES);
			bRet = DoAdvanceSummon(vOutHeroList, 0, nCostDiamond, nOneKeySummonTimes);
			nSummonTimes = nOneKeySummonTimes;
		}
		break;
	case ADVANCED_ITEM_ONEKEY_SUMMON:
		{
			DWORD dwCostItemId = g_GetConfigValue(CONFIG_ADVANCED_SUMMON_COST_ITEM);
			int nOneKeySummonTimes = g_GetConfigValue(CONFIG_ADVANCED_ONEKEY_SUMMON_TIMES);
			bRet = DoAdvanceSummon(vOutHeroList, dwCostItemId, 0, nOneKeySummonTimes);
			nSummonTimes = nOneKeySummonTimes;
		}
		break;
	case FRIENDSHIP_POINT_ONE_KEY_SUMMON:
		{
			for (int i = 0; i < 10; ++i)
			{
				bRet = DoNormalSummon(vOutHeroList, 0, g_GetConfigValue(129, 1));
				if (!bRet)
					break;

				nSummonTimes = i + 1;
			}
		}
		break;
	default:
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]没处理的召唤类型[type:%d]", m_pPlayer->GetUTF8RoleName(),byType);
		break;
	}

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄召唤失败[type:%d]", __FUNCTION__, byType);
		return;
	}

	SendSummonData();

	MSG_S2C_HERO_SUMMON_RESP msg;
	msg.m_byType = byType;
	msg.m_byHeroNum = 0;
	for (int i = 0; i < vOutHeroList.size(); ++i)
	{
		if (msg.m_byHeroNum >= HERO_ONE_KEY_SUMMON_TIMES)
			break;
		msg.m_HeroList[msg.m_byHeroNum++] = vOutHeroList[i];
	}
	m_pPlayer->SendPacket(&msg);

	// 日常任务(低级召唤)
	m_pPlayer->OnUpdateDailyMission(8, nSummonTimes);

	if (ADVANCED_FREE_SUMMON == byType || ADVANCED_ITEM_SUMMON == byType || 
		ADVANCED_GOLD_SUMMON == byType || ADVANCED_GOLD_ONEKEY_SUMMON == byType)
	{
		// 日常任务(高级召唤)
		m_pPlayer->OnUpdateDailyMission(9, nSummonTimes);
	}

	// 处理广播
	for (size_t i = 0; i < vOutHeroList.size(); ++i)
	{
		const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(vOutHeroList[i]);
		if (pHeroCfg && pHeroCfg->wInitQuality >= QUALITY_ORANGE)
		{
			CSystemNotice sys_notice(2, m_pPlayer->GetServerId());
			sys_notice.AddParam(m_pPlayer->GetRoleName());
			sys_notice.AddParam(vOutHeroList[i]);
			sys_notice.SendMsg();
		}
	}

}

// VIP召唤
void CHeroManager::OnNetVIPSummon(WORD wCamp)
{
	// 检测VIP等级
	int nNeedVipLevel = g_GetConfigValue(302);
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < nNeedVipLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]VIP召唤需要的VIP等级不足[NeedVipLevel:%d]", __FUNCTION__, nNeedVipLevel);
		return;
	}

	// 检测需要的钻石
	int nCostDiamond = g_GetConfigValue(303);
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]VIP召唤需要消耗的钻石不足[CostDiamond:%d]", __FUNCTION__, nCostDiamond);
		return;
	}

	// 一键抽次数
	int nSummonTimes = g_GetConfigValue(304, 10);
	
	// 检测包裹空间
	if (GetBagEmptySize() < nSummonTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]英雄背包已满,VIP召唤失败!", __FUNCTION__);
		return;
	}
	
	vector<Item> vHeroList;

	// 随机一个英雄
	for (int i = 1; i <= nSummonTimes; ++i)
	{
		const HeroVipSummon_Config *pSummonCfg = RandVipSummonHero(wCamp, i == nSummonTimes ? true : false);
		if (!pSummonCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄VIP召唤随机英雄失败!", __FUNCTION__);
			return;
		}

		vHeroList.push_back(Item(pSummonCfg->dwHeroId, pSummonCfg->dwHeroNum));
	}

	// 扣除钻石
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_VIP_SUMMON);

	// 添加英雄
	m_pPlayer->AddObjectList(vHeroList, TRUE, FROM_VIP_SUMMON);

	// 响应
	MSG_S2C_HERO_VIP_SUMMON_RESP msg;
	msg.m_dwCamp = wCamp;
	msg.m_vHeroList = vHeroList;
	m_pPlayer->SendPacket(&msg);
}

// 升级
void CHeroManager::OnLevelUp(DWORD dwHeroUID, DWORD *pCostHeroList, BYTE byCostHeroListNum)
{
	// 检测消耗列表是否有重复ID
	for (BYTE i = 0; i < byCostHeroListNum; ++i)
	{
		for (BYTE k = i + 1; k < byCostHeroListNum; ++k)
		{
			if (pCostHeroList[i] == pCostHeroList[k])
			{
				MAPLOG(ERROR_LEVEL, "[%s]升级消耗列表出现ID重复情况[UID:%d]", __FUNCTION__, dwHeroUID);
				return;
			}
		}
	}

	// 升级英雄数据
	HeroInfo *pLevelUpHero = FindHero(dwHeroUID);
	if (!pLevelUpHero)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的升级英雄数据[UID:%d]",__FUNCTION__, dwHeroUID);
		return;
	}

	// 升级英雄配置
	const Hero_Config *pLevelUpHeroCfg = CConfigManager::Instance()->GetHero(pLevelUpHero->dwIntoId);
	if (!pLevelUpHeroCfg)
	{
		// 要升级的英雄配置不存在
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]",__FUNCTION__,pLevelUpHero->dwIntoId);
		return;
	}

	// 检测是否还能升级
	if (pLevelUpHero->wLevel >= pLevelUpHeroCfg->wLimitLevel)
	{
		// 已到达等级上限
		MAPLOG(ERROR_LEVEL, "[%s]英雄已到达等级上限[HeroInfoId:%d]",__FUNCTION__, pLevelUpHero->dwIntoId);
		return;
	}

	DWORD arCostHeroInfoId[5] = { 0 };
	BYTE byRealCostHeroNum = 0;

	bool bLimitLevel = false;
	int nCostGold = 0;
	for (BYTE i = 0; i < byCostHeroListNum; ++i)
	{
		// 消耗英雄数据
		const HeroInfo *pCostHero = GetHero(pCostHeroList[i]);
		if (!pCostHero)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此英雄[HeroUId:%d]",__FUNCTION__, pCostHeroList[i]);
			return;
		}

		const HeroLevel_Config *pCostHeroLvCfg = g_pCfgMgr->GetHeroLevel(pCostHero->wQuality, pCostHero->wLevel);
		if (!pCostHeroLvCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄升级配置数据[Quality:%d,Lv:%d]",
				__FUNCTION__, pCostHero->wQuality, pCostHero->wLevel);
			return;
		}

		// 消耗英雄配置
		const Hero_Config *pCostHeroCfg = g_pCfgMgr->GetHero(pCostHero->dwIntoId);
		if (!pCostHeroCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]",__FUNCTION__, pCostHero->dwIntoId);
			return;
		}

		DWORD dwAddExp = 0;
		dwAddExp += pCostHeroCfg->dwToExpAsCostItem;	// 作为升级消耗品获得的经验
		dwAddExp += pCostHero->dwExp;					// 当前剩余的经验
		dwAddExp += pCostHeroLvCfg->dwTotalExp;			// 当前等级的总经验

		// 计算消耗的金币(消耗的金币 = 获得的经验)
		nCostGold += dwAddExp;

		// 使用此英雄升级需要的金币
		if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold)
		{
			// 金币不足
			MAPLOG(ERROR_LEVEL, "[%s]升级英雄需要的金币不足[HeroInfoId:%d]",__FUNCTION__, pCostHero->dwIntoId);
			break;
		}

		pLevelUpHero->dwExp += dwAddExp;

		const HeroLevel_Config *pHeroLvCfg;
		while (true)
		{
			// 当前升级英雄等级配置
			pHeroLvCfg = g_pCfgMgr->GetHeroLevel(pLevelUpHero->wQuality, pLevelUpHero->wLevel);
			if (!pHeroLvCfg)
			{
				// 找不到英雄等级配置文件
				MAPLOG(ERROR_LEVEL, "[%s]找不到英雄等级配置文件[QualityLevel:%d,Level:%d]",
					__FUNCTION__, pLevelUpHero->wQuality, pLevelUpHero->wLevel);
				return;
			}

			// 如果经验不足够升级,则退出
			if (!pHeroLvCfg->dwExp || pLevelUpHero->dwExp < pHeroLvCfg->dwExp)
				break;

			// 队伍等级限制
			if (pLevelUpHero->wLevel >= m_pPlayer->GetLevel())
			{
				// 需求调整为保留溢出的验
				//pLevelUpHero->dwExp = pHeroLvCfg->dwExp - 1;
				bLimitLevel = true;
				break;
			}
			else
			{
				pLevelUpHero->dwExp -= pHeroLvCfg->dwExp;
				++pLevelUpHero->wLevel;
				//++wAddLevel;
			}
		}

		// 移除英雄及扣除金币
		//RemoveHero(pCostHero->dwUID, false);

		arCostHeroInfoId[byRealCostHeroNum++] = pCostHero->dwIntoId;
		
		// 等级已满则跳出
		if (bLimitLevel)
			break;
	}

	RemoveHero(pCostHeroList, byRealCostHeroNum, FROM_HERO_LEVEL_UP);

	// 消耗金币
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold, TRUE, FROM_HERO_LEVEL_UP);

	// 检测培养等级是否需要提升
	UpdateTrainLevel();

	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_LEVEL_UP_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(9, GetFormationHeroMinLevel(), false);
}

// 升级
void CHeroManager::OnLevelUpEx(DWORD dwHeroUID, DWORD *pCostHeroList, WORD wCostHeroListNum)
{
	WORD wFromMsgId = C2S_HERO_LEVEL_UP_REQ;


	// 升级英雄数据
	HeroInfo *pLevelUpHero = FindHero(dwHeroUID);
	if (!pLevelUpHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家的升级英雄数据[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// 升级英雄配置
	const Hero_Config *pLevelUpHeroCfg = g_pCfgMgr->GetHero(pLevelUpHero->dwIntoId);
	if (!pLevelUpHeroCfg)
	{
		// 要升级的英雄配置不存在
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]", __FUNCTION__, pLevelUpHero->dwIntoId);
		return;
	}

	// 检测是否还能升级
	if (pLevelUpHero->wLevel >= pLevelUpHeroCfg->wLimitLevel)
	{
		// 已到达等级上限
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄已到达等级上限[HeroInfoId:%d]", __FUNCTION__, pLevelUpHero->dwIntoId);
		return;
	}

	// 检测消耗列表是否有重复ID
	for (WORD i = 0; i < wCostHeroListNum; ++i)
	{
		DWORD dwCheckHeroId = pCostHeroList[i];
		for (WORD k = i + 1; k < wCostHeroListNum; ++k)
		{
			if (dwCheckHeroId == pCostHeroList[k])
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]升级消耗列表出现ID重复情况[UID:%d]", __FUNCTION__, dwHeroUID);
				return;
			}
		}
	}

	// 计算获得的总经验
	DWORD dwAddExp = 0;
	for (WORD i = 0; i < wCostHeroListNum; ++i)
	{
		// 消耗英雄数据
		const HeroInfo *pCostHero = GetHero(pCostHeroList[i]);
		if (!pCostHero)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此英雄[HeroUId:%d]", __FUNCTION__, pCostHeroList[i]);
			return;
		}

		const HeroLevel_Config *pCostHeroLvCfg = g_pCfgMgr->GetHeroLevel(pCostHero->wQuality, pCostHero->wLevel);
		if (!pCostHeroLvCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄升级配置数据[Quality:%d,Lv:%d]",__FUNCTION__, pCostHero->wQuality, pCostHero->wLevel);
			return;
		}

		// 消耗英雄配置
		const Hero_Config *pCostHeroCfg = CConfigManager::Instance()->GetHero(pCostHero->dwIntoId);
		if (!pCostHeroCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]", __FUNCTION__, pCostHero->dwIntoId);
			return;
		}


		// 英雄升级，把吞噬的卡牌经验改成1, 传承核心不变
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT && pCostHeroCfg->byHeroType == 1)
		{
			dwAddExp += 1;
		}
		else
		{
			dwAddExp += pCostHeroCfg->dwToExpAsCostItem;	// 作为升级消耗品获得的经验
			dwAddExp += pCostHero->dwExp;					// 当前剩余的经验
			dwAddExp += pCostHeroLvCfg->dwTotalExp;			// 当前等级的总经验
		}

		// todo::输出日志

	}

	// 计算消耗的金币(消耗的金币 = 获得的经验)
	int nCostGold = dwAddExp;

	// 使用此英雄升级需要的金币
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]升级英雄需要的金币不足[HeroUID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// 处理升级
	pLevelUpHero->dwExp += dwAddExp;
	const HeroLevel_Config *pHeroLvCfg;
	while (true)
	{
		// 当前升级英雄等级配置
		pHeroLvCfg = g_pCfgMgr->GetHeroLevel(pLevelUpHero->wQuality, pLevelUpHero->wLevel);
		if (!pHeroLvCfg)
		{
			// 找不到英雄等级配置文件
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄等级配置文件[QualityLevel:%d,Level:%d]",
				__FUNCTION__, pLevelUpHero->wQuality, pLevelUpHero->wLevel);
			break;
		}

		// 如果经验不足够升级,退出
		if (!pHeroLvCfg->dwExp || pLevelUpHero->dwExp < pHeroLvCfg->dwExp)
			break;

		// 队伍等级限制,退出
		if (pLevelUpHero->wLevel >= m_pPlayer->GetLevel())
			break;
	
		pLevelUpHero->dwExp -= pHeroLvCfg->dwExp;
		++pLevelUpHero->wLevel;
	}

	// 删除作为升级材料的英雄
	RemoveHero(pCostHeroList, wCostHeroListNum, FROM_HERO_LEVEL_UP);

	// 消耗金币
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold, TRUE, FROM_HERO_LEVEL_UP);

	// 检测培养等级是否需要提升
	UpdateTrainLevel();

	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_LEVEL_UP_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(9, GetFormationHeroMinLevel(), false);

	MAPLOG(ERROR_LEVEL, "[%s]本次升级获得经验[dwAddExp:%d]", __FUNCTION__, dwAddExp);
}

// 突破(进阶)
void CHeroManager::OnBreakEx(DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_HERO_BREAK_REQ;

	// 检测是否有此英雄
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]找不到角色英雄数据[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwHeroUID);
		return;
	}

	const HeroBreakLevelUp_Config *pBreakLvUpCfg = g_pCfgMgr->GetHeroBreakLevelUp(pHero->wBreakLevel);
	if (!pBreakLvUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄突破等级对应的配置[InfoId:%d,BreakLv:%d]", 
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// 检测突破等级需要的品质
	if (pHero->wQuality < pBreakLvUpCfg->wNeedHeroQuality)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄品质限制不能突破[InfoId:%d,Quality:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wQuality);
		return;
	}

	// 检测突破条件(英雄等级)
	if (pHero->wLevel < pBreakLvUpCfg->wNeedHeroLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破等级受到当前英雄等级限制[InfoId:%d,BreakLv:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// 检测突破消耗(金币)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < int(pBreakLvUpCfg->nCostGold))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的金币不足[InfoId:%d,CostGold:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->nCostGold);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr)
	// 检测突破消耗(突破石)
	if (pItemMgr->GetItemNum(pBreakLvUpCfg->dwCostItemId) < pBreakLvUpCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的突破石不足[InfoId:%d,ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum);
		return;
	}

	// 检测突破消耗(同名英雄)
	DWORD arCanCostHeroList[MAX_HERO_BAG_NUM];
	WORD wCanCostHeroNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		const HeroInfo &sCostHero = m_pPlayerHeroInfo->arHeroList[i];
		// 突破及天命等级没有升过,可作为消耗品, 没有上阵
		if (sCostHero.dwIntoId == pHero->dwIntoId && !sCostHero.wBreakLevel && !sCostHero.wDestinyLevel && !IsToBattle(sCostHero.dwUID)){
			arCanCostHeroList[wCanCostHeroNum++] = sCostHero.dwUID;
		}

		// 获得足够的数量,则跳出
		if (wCanCostHeroNum >= pBreakLvUpCfg->wCostSameNameHeroNum)
			break;
	}
	if (wCanCostHeroNum < pBreakLvUpCfg->wCostSameNameHeroNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的英雄不足[InfoId:%d,CostHeroInfoId:%d,CostHeroNum:%d,HaveHeroNum:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->dwIntoId, pBreakLvUpCfg->wCostSameNameHeroNum, wCanCostHeroNum);
		return;
	}

	// 突破等级提升
	pHero->wBreakLevel++;

	WORD wBreakLevel = pHero->wBreakLevel;
	DWORD dwHeroInfoId = pHero->dwIntoId;

	// 处理消耗(金币,突破石,英雄)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pBreakLvUpCfg->nCostGold, TRUE, FROM_HERO_BREAK);
	pItemMgr->RemoveItem(pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum, TRUE, FROM_HERO_BREAK);
	RemoveHero(arCanCostHeroList, pBreakLvUpCfg->wCostSameNameHeroNum, FROM_HERO_BREAK);

	// 重新广播数据
	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_BREAK_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(10, GetFormationHeroMinBreakLevel(), false);

	// 7天活动事件(英雄突破)
	m_pPlayer->OnUpdateSevenDayTarget(9);

	// 处理广播
	if (wBreakLevel >= g_GetConfigValue(139))
	{
		CSystemNotice sys_notice(6, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(dwHeroInfoId);
		sys_notice.AddParam(wBreakLevel);
		sys_notice.SendMsg();
	}
}




// 突破(进阶)
void CHeroManager::OnBreak(DWORD dwHeroUID, DWORD *pCostSameNameHeroList, WORD wCostSameNameHeroNum,
	DWORD *pCostSameQualityHeroList, WORD wCostSameQualityHeroNum,
	DWORD *pCostSameProfessionHeroList, WORD wCostSameProfessionHeroNum)
{
	WORD wFromMsgId = C2S_HERO_BREAK_REQ;

	// 检测是否有此英雄
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]找不到角色英雄数据[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwHeroUID);
		return;
	}

	const HeroBreakLevelUp_Config *pBreakLvUpCfg = g_pCfgMgr->GetHeroBreakLevelUp(pHero->wBreakLevel);
	if (!pBreakLvUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄突破等级对应的配置[InfoId:%d,BreakLv:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// 检测突破等级需要的品质
	if (pHero->wQuality < pBreakLvUpCfg->wNeedHeroQuality)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄品质限制不能突破[InfoId:%d,Quality:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wQuality);
		return;
	}

	// 检测突破条件(英雄等级)
	if (pHero->wLevel < pBreakLvUpCfg->wNeedHeroLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破等级受到当前英雄等级限制[InfoId:%d,BreakLv:%d]",
			__FUNCTION__, pHero->dwIntoId, pHero->wBreakLevel);
		return;
	}

	// 检测突破消耗(金币)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < int(pBreakLvUpCfg->nCostGold))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的金币不足[InfoId:%d,CostGold:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->nCostGold);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr)
	// 检测突破消耗(突破石)
	if (pItemMgr->GetItemNum(pBreakLvUpCfg->dwCostItemId) < pBreakLvUpCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的突破石不足[InfoId:%d,ItemId:%d,ItemNum:%d]",
			__FUNCTION__, pHero->dwIntoId, pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum);
		return;
	}

	vector<DWORD> vCostHeroInfoIdList;
		
	// 检测消耗的英雄(同名英雄)
	if (!CheckBreakCostHero(pHero, pCostSameNameHeroList, wCostSameNameHeroNum, 
		pBreakLvUpCfg->wCostSameNameHeroNum, NULL, SAME_NAME))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL,"[%s]检测英雄突破消耗同名英雄失败[UId:%d,InfoId:%d]",__FUNCTION__,pHero->dwUID,pHero->dwIntoId);
		return;
	}

	// 检测消耗的英雄(同品质英雄)
	if (!CheckBreakCostHero(pHero, pCostSameQualityHeroList, wCostSameQualityHeroNum, 
		pBreakLvUpCfg->wCostSameQualityHeroNum, &vCostHeroInfoIdList, SAME_QUALITY))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]检测英雄突破消耗同品质英雄失败[UId:%d,InfoId:%d]", __FUNCTION__, pHero->dwUID, pHero->dwIntoId);
		return;
	}

	// 检测消耗的英雄(同职业英雄)
	if (!CheckBreakCostHero(pHero, pCostSameProfessionHeroList, wCostSameProfessionHeroNum, 
		pBreakLvUpCfg->wCostSameProfessionHeroNum, &vCostHeroInfoIdList, SAME_QUALITY_AND_PROFESSION))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]检测英雄突破消耗同职业英雄失败[UId:%d,InfoId:%d]", __FUNCTION__, pHero->dwUID, pHero->dwIntoId);
		return;
	}

	// 突破等级提升
	pHero->wBreakLevel++;

	WORD wBreakLevel = pHero->wBreakLevel;
	DWORD dwHeroInfoId = pHero->dwIntoId;

	// 处理消耗(金币,突破石,英雄)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pBreakLvUpCfg->nCostGold, TRUE, FROM_HERO_BREAK);
	pItemMgr->RemoveItem(pBreakLvUpCfg->dwCostItemId, pBreakLvUpCfg->wCostItemNum, TRUE, FROM_HERO_BREAK);
	RemoveHero(pCostSameNameHeroList, wCostSameNameHeroNum, FROM_HERO_BREAK);
	RemoveHero(pCostSameQualityHeroList, wCostSameQualityHeroNum, FROM_HERO_BREAK);
	RemoveHero(pCostSameProfessionHeroList, wCostSameProfessionHeroNum, FROM_HERO_BREAK);

	// 记录消耗英雄
	for (int i = 0; i < vCostHeroInfoIdList.size(); ++i){
		RecordBreakCostHero(pHero, vCostHeroInfoIdList[i]);
	}

	// 重新广播数据
	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_BREAK_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(10, GetFormationHeroMinBreakLevel(), false);

	// 7天活动事件(英雄突破)
	m_pPlayer->OnUpdateSevenDayTarget(9);

	// 处理广播
	if (wBreakLevel >= g_GetConfigValue(139))
	{
		CSystemNotice sys_notice(6, m_pPlayer->GetServerId());
		sys_notice.AddParam(m_pPlayer->GetRoleName());
		sys_notice.AddParam(dwHeroInfoId);
		sys_notice.AddParam(wBreakLevel);
		sys_notice.SendMsg();
	}
}

// 进化(品质)
void CHeroManager::OnNetEvolution(DWORD dwHeroUID)
{
	// 检测是否有此英雄
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色英雄数据[UID:%d]",__FUNCTION__, dwHeroUID);
		return;
	}

	// 取英雄配置
	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置数据[InfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	// 检测是否已达进化上限
	if (pHero->wQuality == pHeroCfg->wQualityMax && pHero->wQualityStage >= pHeroCfg->wQualityStageMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄已经不能再进化[InfoId:%d,Quality:%d,Stage:%d]", 
			__FUNCTION__, pHero->dwIntoId, pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// 取进化配置
	const HeroEvolution_Config *pEvolutionCfg = g_pCfgMgr->GetHeroEvolution(pHero->wQuality, pHero->wQualityStage);
	if (!pEvolutionCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄进化配置[Quality:%d,Stage:%d]",__FUNCTION__,pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// 检测进化需要消耗(金币)
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < pEvolutionCfg->nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄进化消耗的金币不足[Quality:%d,Stage:%d,CostGold:%d]",
			__FUNCTION__, pHero->wQuality, pHero->wQualityStage, pEvolutionCfg->nCostGold);
		return;
	}

	// 检测进化需要消耗(货币)
	if (!m_pPlayer->CheckMoneyCost(pEvolutionCfg->vCostCurrencyList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄进化消耗的货币不足[Quality:%d,Stage:%d]",
			__FUNCTION__, pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// 检测进化需要消耗(道具)
	if (!m_pPlayer->CheckItemCost(pEvolutionCfg->vCostItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄进化消耗的道具不足[Quality:%d,Stage:%d]",
			__FUNCTION__, pHero->wQuality, pHero->wQualityStage);
		return;
	}

	// 处理进化需要消耗(金币)
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pEvolutionCfg->nCostGold, TRUE, FROM_HERO_EVOLUTION);

	// 处理进化需要消耗(货币)
	m_pPlayer->DoMoneyCost(pEvolutionCfg->vCostCurrencyList, 1, FROM_HERO_EVOLUTION);

	// 处理进化需要消耗(道具)
	for (int i = 0; i < pEvolutionCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pEvolutionCfg->vCostItemList[i], FROM_HERO_EVOLUTION);
	}

	// 升品
	if (pEvolutionCfg->isTheLastStage)
	{
		pHero->wQuality++;
		pHero->wQualityStage = 0;
	}
	// 升阶
	else
	{
		pHero->wQualityStage++;
	}

	// 同步数据
	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_HERO_EVOLUTION_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	msg.m_wQuality = pHero->wQuality;
	msg.m_wStage = pHero->wQualityStage;
	m_pPlayer->SendPacket(&msg);
}

// 培养
void CHeroManager::OnNetTrain(DWORD dwHeroUID, BYTE byCostType, BYTE byTrainTimes)
{
	WORD wFromMsgId = C2S_HERO_TRAIN_REQ;

	// 检测次数
	if (!byTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄培养次数为0", __FUNCTION__);
		return;
	}

	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到要培养的英雄[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// 取消耗配置
	const HeroTrainCost_Config *pTrainCostCfg = g_pCfgMgr->GetHeroTrainCost(byCostType);
	if (!pTrainCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄培养消耗配置[CostType:%d]", __FUNCTION__, byCostType);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 检测消耗物品
	if (pItemMgr->GetItemNum(pTrainCostCfg->tCostItem.dwId) < pTrainCostCfg->tCostItem.dwNum * byTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄培养消耗物品不足[CostType:%d]", __FUNCTION__, byCostType);
		return;
	}

	// 检测消耗货币
	if (pTrainCostCfg->tCostMoney.nValue && 
		m_pPlayer->GetProperty(pTrainCostCfg->tCostMoney.byType) < pTrainCostCfg->tCostMoney.nValue * byTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄培养消耗货币不足[CostType:%d]", __FUNCTION__, byCostType);
		return;
	}

	// 取属性随机列表
	const vector<HeroTrainRandPro_Config> *pRandProlListConfig = g_pCfgMgr->GetHeroTrainRandProList();
	if (!pRandProlListConfig)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄培养随机属性隐表为空", __FUNCTION__);
		return;
	}

	int arRandProList[HERO_TRAIN_PRO_NUM] = {0};

	// 随机属性
	for (BYTE i = 0; i < byTrainTimes; ++i)
	{
		for (size_t k = 0; k < pRandProlListConfig->size(); ++k)
		{
			const HeroTrainRandPro_Config *pProCfg = &pRandProlListConfig->at(k);
			BYTE byProIdx = GetTrainProIdx(pProCfg->byProType);
			if (byProIdx >= HERO_TRAIN_PRO_NUM)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]英雄培养属性索引越界[ProType:%d]", __FUNCTION__, pProCfg->byProType);
				return;
			}

			// todo::如果是前5次培养,则必是提升(新手引导功能)
			if (m_pPlayerHeroInfo->wTrainTimes <= 5)
			{
				arRandProList[byProIdx] += g_Rand(1, pProCfg->nMax);
			}
			else
			{
				arRandProList[byProIdx] += g_Rand(pProCfg->nMin, pProCfg->nMax);
			}
		}

		if (m_pPlayerHeroInfo->wTrainTimes <= 5)
			m_pPlayerHeroInfo->wTrainTimes++;
	}

	// 纠正随机值
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		int &nRandProValue = arRandProList[i];
		BYTE byProType = GetTrainProType(i);
		BYTE byProLevel = pHero->arTrainProLvList[i];

		const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, byProLevel);
		if (!pTrainCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄培养配置[ProType:%d,ProLevel:%d]",__FUNCTION__, byProType, byProLevel);
			return;
		}

		// 下限纠正(下限不能为负数)
		if (nRandProValue < 0)
		{
			// 如果累积属性值已达当前级上限,则不会变动
			if (pHero->arTrainProValueList[i] >= pTrainCfg->nProMax)
			{
				nRandProValue = 0;
			}
			else
			{
				int nCurProValue = 0;
				// 已升过级
				if (byProLevel)
				{
					// 保证不能掉当前等级	
					const HeroTrain_Config *pTrainCfgEx = g_pCfgMgr->GetHeroTrain(byProType, byProLevel - 1);
					if (pTrainCfgEx)
						nCurProValue = pHero->arTrainProValueList[i] - pTrainCfgEx->nProMax;
				}
				// 还没升过级
				else
				{
					nCurProValue = pHero->arTrainProValueList[i];
				}

				if (abs(nRandProValue) > nCurProValue)
				{
					nRandProValue = -nCurProValue;
				}
			}
		}
		
		// 上限纠正(不能超过当前可升级的上限)
		if (nRandProValue > 0)
		{
			// 取当前阶段的属性上限
			int nProMax = pTrainCfg->nProMax; // min(pTrainCfg->nProMax, GetTrainProMax(byProType, pHero->wLevel));
			if (nProMax)
			{
				if (nRandProValue + pHero->arTrainProValueList[i] > nProMax)
				{
					nRandProValue = nProMax - pHero->arTrainProValueList[i];
				}
			}
			else
			{
				nRandProValue = 0;
			}
		}
	}

	// 缓存最终值
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		pHero->arLastTrainTmpProList[i] = arRandProList[i];
	}

	// 处理消耗
	pItemMgr->RemoveItem(pTrainCostCfg->tCostItem.dwId, pTrainCostCfg->tCostItem.dwNum * byTrainTimes, TRUE, FROM_HERO_TRAIN);
	if (pTrainCostCfg->tCostMoney.nValue)
		m_pPlayer->ModifyProperty(pTrainCostCfg->tCostMoney.byType, -pTrainCostCfg->tCostMoney.nValue * byTrainTimes, TRUE, FROM_HERO_TRAIN);

	// 记录消耗的材料
	pHero->dwTrainCostItemNum += pTrainCostCfg->tCostItem.dwNum * byTrainTimes;
	if (ROLE_PRO_GOLD == pTrainCostCfg->tCostMoney.byType)
	{
		pHero->nTrainCostGold += pTrainCostCfg->tCostMoney.nValue * byTrainTimes;
	}
	else if (ROLE_PRO_DIAMOND == pTrainCostCfg->tCostMoney.byType)
	{
		pHero->nTrainCostDiamond += pTrainCostCfg->tCostMoney.nValue * byTrainTimes;
	}
	else if (pTrainCostCfg->tCostMoney.byType)
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的培养消耗货币类型[ProType:%d]", __FUNCTION__, pTrainCostCfg->tCostMoney.byType);
	}

	// 响应
	MSG_S2C_HERO_TRAIN_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	msg.m_byProNum = HERO_TRAIN_PRO_NUM;       
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		msg.m_ProList[i].nValue = arRandProList[i];
		msg.m_ProList[i].wType = GetTrainProType(i);
	}
	m_pPlayer->SendPacket(&msg);

	// 日常任务事件
	m_pPlayer->OnUpdateDailyMission(3, byTrainTimes);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 33, byTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 33, byTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 33, byTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 33, byTrainTimes, true);

	// 世界等级任务
	//m_pPlayer->OnUpdateWorldTaskEvent(3, byTrainTimes);

	// 成就
	//m_pPlayer->OnUpdateAchievement(11, GetFormationHeroMinTrainLevel(), false);

	// todo::测试代码
// 	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
// 	{
// 		MAPLOG(ERROR_LEVEL, "培养生成属性[ProType:%d,ProValue:%d]", GetTrainProType(i), arRandProList[i]);
// 	}
}

// 培养替换
void CHeroManager::OnNetTrainReplace(DWORD dwHeroUID)
{
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_HERO_TRAIN_REPLACE_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到要培养的英雄[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	// 更新培养属性
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		pHero->arTrainProValueList[i] += pHero->arLastTrainTmpProList[i];
	}

	//bool bHaveLevelUp = false;

	// 处理升级
	for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
	{
		int nProValue = pHero->arTrainProValueList[i];
		BYTE byProType = GetTrainProType(i);
		const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, pHero->arTrainProLvList[i]);
		// 属性满足,本次替换属性有变动(todo::投机取巧逻辑)
		if (pTrainCfg && nProValue >= pTrainCfg->nProMax && pHero->arLastTrainTmpProList[i])
		{
			pHero->arTrainRewardFlag[i] = true;

			// 检测培养等级上限(最终等级上限,当前英雄等级限制)
// 			if (pHero->wLevel >= pTrainCfg->wNeedHeroLevel && pHero->arTrainProLvList[i] < GetTrainLevelMax(byProType))
// 			{
// 				pHero->arTrainProLvList[i]++;
// 
// 				bHaveLevelUp = true;
// 			}
		}
	}

	// 清空缓存结果
	memset(pHero->arLastTrainTmpProList, 0, sizeof(pHero->arLastTrainTmpProList));

	HeroBaseDataNotify(dwHeroUID);

	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_HERO_TRAIN_REPLACE_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	m_pPlayer->SendPacket(&msg);

// 	if (bHaveLevelUp)
// 	{
// 		// 7天活动事件(英雄培养)
// 		m_pPlayer->OnUpdateSevenDayTarget(15);
// 	}
}

// 领取培养奖励
void CHeroManager::OnNetGetTrainReward(DWORD dwHeroUID, BYTE byProType)
{
	WORD wFromMsgId = C2S_HERO_TRAIN_REWARD_GET_REQ;

	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到要培养的英雄[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	BYTE byProIdx = GetTrainProIdx(byProType);

	// 检测培养类型参数
	if (byProIdx >= HERO_TRAIN_PRO_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]培养类型参数越界[ProType:%d]", __FUNCTION__, byProType);
		return;
	}

	// 检测是否已领取过
	if (!pHero->arTrainRewardFlag[byProIdx])
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]培养奖励已领取过[ProType:%d]", __FUNCTION__, byProType);
		return;
	}

	// 记录已领取过
	pHero->arTrainRewardFlag[byProIdx] = false;

	// 处理升级
	const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, pHero->arTrainProLvList[byProIdx]);
	// 检测培养等级上限(最终等级上限,当前英雄等级限制)
	if (pTrainCfg && pHero->wLevel >= pTrainCfg->wNeedHeroLevel && pHero->arTrainProLvList[byProIdx] < GetTrainLevelMax(byProType))
	{
		pHero->arTrainProLvList[byProIdx]++;

		// 7天活动事件(英雄培养)
		m_pPlayer->OnUpdateSevenDayTarget(15);
	}

	HeroBaseDataNotify(dwHeroUID);
	
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_TRAIN_REWARD_GET_RESP msg;
	msg.m_dwHeroUId = dwHeroUID;
	msg.m_byProType = byProType;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(11, GetFormationHeroMinTrainLevel(), false);
}

// 添加天命值
void CHeroManager::OnNetDestiny(DWORD dwHeroUID)
{
	WORD wFromMsgId = C2S_HERO_DESTINY_REQ;

	// 检测英雄是否存在
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色英雄数据[UID:%d]",__FUNCTION__, dwHeroUID);
		return;
	}

	// 查找天命配置
	const HeroDestiny_Config *pDestinyCfg = CConfigManager::Instance()->GetHeroDestiny(pHero->wDestinyLevel);
	if (!pDestinyCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到天命配置[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
		return;
	}

	// 检测等级上限
	if (!pDestinyCfg->dwNeedExp)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]天命等级已达上限[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	// 检测消耗
	if (pDestinyCfg->dwCostItemId && pItemMgr->GetItemNum(pDestinyCfg->dwCostItemId) < pDestinyCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]天命消耗物品不足[ItemId:%d]", __FUNCTION__, pDestinyCfg->dwCostItemId);
		return;
	}

	// 添加经验
	pHero->dwDestinyExp += pDestinyCfg->dwAddExp;
	
	// 经验升级
	if (pHero->dwDestinyExp >= pDestinyCfg->dwNeedExp)
	{
		pHero->wDestinyLevel++;
		pHero->dwDestinyExp = 0;
	}
	else
	{
		// 几率升级
		const RateData *pRateData = pDestinyCfg->GetRateData(pHero->dwDestinyExp);
		if (!pRateData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到天命经验对应的升级几率配置[Exp:%d]",
				__FUNCTION__, pHero->dwDestinyExp);
			return;
		}

		// 命中
		if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < pRateData->nRate)
		{
			pHero->wDestinyLevel++;
			pHero->dwDestinyExp = 0;
		}
	}

	// 处理消耗
	if (pDestinyCfg->dwCostItemId)
	{
		pItemMgr->RemoveItem(pDestinyCfg->dwCostItemId, pDestinyCfg->wCostItemNum, TRUE, FROM_HERO_DESTINY);
	}

	// 记录总的天命消耗
	pHero->dwDestinyCostItemNum += pDestinyCfg->wCostItemNum;

	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_DESTINY_RESP msg;
	msg.m_dwHeroUID = dwHeroUID;
	msg.m_byDestinyLevel = pHero->wDestinyLevel;
	msg.m_dwDestinyExp = pHero->dwDestinyExp;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(12, GetFormationHeroMinDestinyLevel(), false);

	// 7天活动事件(潜能提升等级)
	m_pPlayer->OnUpdateSevenDayTarget(11);
}

// 天命(潜能)
void CHeroManager::OnNetDestinyEx(DWORD dwHeroUID, WORD wDoTimes)
{
	WORD wFromMsgId = C2S_HERO_DESTINY_REQ;

	// 检测英雄是否存在
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色英雄数据[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	for (WORD i = 0; i < wDoTimes; ++i)
	{
		// 查找天命配置
		const HeroDestiny_Config *pDestinyCfg = CConfigManager::Instance()->GetHeroDestiny(pHero->wDestinyLevel);
		if (!pDestinyCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到天命配置[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
			return;
		}

		// 检测等级上限
		if (!pDestinyCfg->dwNeedExp)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]天命等级已达上限[DestinyLv:%d]", __FUNCTION__, pHero->wDestinyLevel);
			return;
		}

		CItemManager *pItemMgr = m_pPlayer->GetItemManager();
		CHECK_POINTER(pItemMgr);
		// 检测消耗
		if (pDestinyCfg->dwCostItemId && pItemMgr->GetItemNum(pDestinyCfg->dwCostItemId) < pDestinyCfg->wCostItemNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]天命消耗物品不足[ItemId:%d]", __FUNCTION__, pDestinyCfg->dwCostItemId);
			return;
		}

		// 添加经验
		pHero->dwDestinyExp += pDestinyCfg->dwAddExp;

		// 经验升级
		if (pHero->dwDestinyExp >= pDestinyCfg->dwNeedExp)
		{
			pHero->wDestinyLevel++;
			pHero->dwDestinyExp = 0;
		}
		else
		{
			// 几率升级
			const RateData *pRateData = pDestinyCfg->GetRateData(pHero->dwDestinyExp);
			if (!pRateData)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]找不到天命经验对应的升级几率配置[Exp:%d]",
					__FUNCTION__, pHero->dwDestinyExp);
				return;
			}

			// 命中
			if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < pRateData->nRate)
			{
				pHero->wDestinyLevel++;
				pHero->dwDestinyExp = 0;
			}
		}

		// 处理消耗
		if (pDestinyCfg->dwCostItemId)
		{
			pItemMgr->RemoveItem(pDestinyCfg->dwCostItemId, pDestinyCfg->wCostItemNum, TRUE, FROM_HERO_DESTINY);
		}

		// 记录总的天命消耗
		pHero->dwDestinyCostItemNum += pDestinyCfg->wCostItemNum;
	}

	HeroBaseDataNotify(dwHeroUID);

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_DESTINY_RESP msg;
	msg.m_dwHeroUID = dwHeroUID;
	msg.m_byDestinyLevel = pHero->wDestinyLevel;
	msg.m_dwDestinyExp = pHero->dwDestinyExp;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(12, GetFormationHeroMinDestinyLevel(), false);

	// 7天活动事件(潜能提升等级)
	m_pPlayer->OnUpdateSevenDayTarget(11);
}

// 装备觉醒道具
void CHeroManager::OnNetEquipAwakeningItem(DWORD dwHeroUID, WORD wItemId)
{
	WORD wFromMsgId = C2S_HERO_EQUIP_AWAKEN_ITEM_REQ;

	// 检测英雄是否存在
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色英雄数据[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[InfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	const HeroAwaken_Config *pAwakenCfg = g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality,
		pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
	if (!pAwakenCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄觉醒配置[Qaulity:%d,Profession:%d,Star:%d,Level:%d]",
			__FUNCTION__, pHeroCfg->wInitQuality, pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
		return;
	}

	// 检测是否觉醒道具
	bool bAwakenItem = false;
	for (size_t i = 0; i < pAwakenCfg->vCostItemList.size(); ++i)
	{
		if (pAwakenCfg->vCostItemList[i] == wItemId)
		{
			bAwakenItem = true;
			break;
		}
	}
	if (!bAwakenItem)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]装备的道具不是该英雄觉醒需要的道具[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	// 检测是否重复装备
	for (BYTE i = 0; i < pHero->wAwakenItemNum; ++i)
	{
		if (pHero->arAwakenItemList[i] == wItemId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]重复装备同样道具[ItemId:%d]", __FUNCTION__, wItemId);
			return;
		}
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 检测道具是否存在
	if (!pItemMgr->GetItemNum(wItemId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]装备的觉醒道具不存在[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	pItemMgr->RemoveItem(wItemId, 1, TRUE, FROM_EQUIP_AWAKENING_ITEM);

	if (pHero->wAwakenItemNum < MAX_AWAKEN_EQUIP_ITEM_NUM)
		pHero->arAwakenItemList[pHero->wAwakenItemNum++] = wItemId;

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_EQUIP_AWAKEN_ITEM_RESP msg;
	msg.m_dwUId = dwHeroUID;
	msg.m_wAwakenItemId = wItemId;
	m_pPlayer->SendPacket(&msg);
}

// 觉醒
void CHeroManager::OnNetAwaken(DWORD dwHeroUID, const DWORD *pCostHeroUIDList, WORD wCostHeroNum)
{
	//MAPLOG(GUI_LEVEL, "[%s]请求觉醒[UID:%d,CostHeroUID:%d]", __FUNCTION__, dwHeroUID, dwCostHeroUID);

	WORD wFromMsgId = C2S_HERO_AWAKEN_REQ;

	// 检测英雄是否存在
	HeroInfo *pHero = FindHero(dwHeroUID);
	if (!pHero)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色英雄数据[UID:%d]", __FUNCTION__, dwHeroUID);
		return;
	}

	const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
	if (!pHeroCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[InfoId:%d]", __FUNCTION__, pHero->dwIntoId);
		return;
	}

	const HeroAwaken_Config *pAwakenCfg = g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality,
		pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
	if (!pAwakenCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄觉醒配置[Qaulity:%d,Profession:%d,Star:%d,Level:%d]", 
			__FUNCTION__, pHeroCfg->wInitQuality, pHeroCfg->byProfession, pHero->wAwakenStar, pHero->wAwakenLevel);
		return;
	}

	// 检测英雄等级限制
	if (pAwakenCfg->wNeedHeroLevel > pHero->wLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒受英雄等级限制!", __FUNCTION__);
		return;
	}

	// 检测消耗货币
	if (m_pPlayer->GetProperty(pAwakenCfg->nCostMoneyType) < pAwakenCfg->nCostMoneyValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗物货币不足[MoneyType:%d,MoneyValue:%d]",
			__FUNCTION__, pAwakenCfg->nCostMoneyType, pAwakenCfg->nCostMoneyValue);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// 检测消耗材料物品
	if (pItemMgr->GetItemNum(pAwakenCfg->tCostItem.dwId) < pAwakenCfg->tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗材料物品不足[ItemId:%d]",__FUNCTION__, pAwakenCfg->tCostItem.dwId);
		return;
	}

	// 检测是否已装备上觉醒所需物品
	for (size_t i = 0; i < pAwakenCfg->vCostItemList.size(); ++i)
	{
		DWORD dwCostItemId = pAwakenCfg->vCostItemList[i];
		bool bHave = false;
		for (BYTE k = 0; k < pHero->wAwakenItemNum; ++k)
		{
			if (pHero->arAwakenItemList[k] == dwCostItemId)
			{
				bHave = true;
				break;
			}
		}
		if (!bHave)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗装备物品不足[ItemId:%d]", __FUNCTION__, dwCostItemId);
			return;
		}
	}

	// 检测消耗同名英雄
	if (pAwakenCfg->wCostHeroNum != wCostHeroNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗的同名英雄数量不足[NeedNum:%d,HaveNum]", 
			__FUNCTION__, pAwakenCfg->wCostHeroNum, wCostHeroNum);
		return;
	}
	map<DWORD, DWORD> findList;
	for (WORD i = 0; i < wCostHeroNum; ++i)
	{
		DWORD dwCostHeroUID = pCostHeroUIDList[i];
		HeroInfo *pCostHero = FindHero(dwCostHeroUID);
		if (!pCostHero)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗的同名英雄不存在[UID:%d]", __FUNCTION__, dwCostHeroUID);
			return;
		}

		// 是否同名英雄
		if (pCostHero->dwIntoId != pHero->dwIntoId)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]我读书少,你可别骗我,这根本不是觉醒同名英雄![UID:%d]", __FUNCTION__, dwCostHeroUID);
			return;
		}

		// 检测是否会出现重复英雄
		if (findList.find(dwCostHeroUID) != findList.end())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]英雄觉醒消耗出现重复英雄[UID:%d]", __FUNCTION__, dwCostHeroUID);
			return;
		}
		findList.insert(make_pair(dwCostHeroUID, dwCostHeroUID));
	}

	// 删除消耗
	m_pPlayer->ModifyProperty(pAwakenCfg->nCostMoneyType, -pAwakenCfg->nCostMoneyValue, TRUE, FROM_HERO_AWAKEN);
	pItemMgr->RemoveItem(pAwakenCfg->tCostItem.dwId, pAwakenCfg->tCostItem.dwNum, TRUE, FROM_HERO_AWAKEN);

	// 升星
	if (pHero->wAwakenLevel >= HERO_AWAKEN_LEVEL)
	{
		pHero->wAwakenStar++;
		pHero->wAwakenLevel = 0;
	}
	// 升级
	else
	{
		pHero->wAwakenLevel++;
	}
	pHero->wAwakenItemNum = 0;

	HeroBaseDataNotify(dwHeroUID);

	// 先记录,防止后面的删除操作导致数据位置更改而数据错乱
	WORD wAwakenStar = pHero->wAwakenStar;
	WORD wAwakenLevel = pHero->wAwakenLevel;

	// 删除消耗的英雄
	for (WORD i = 0; i < wCostHeroNum; ++i){
		RemoveHero(pCostHeroUIDList[i], TRUE, FROM_HERO_AWAKEN);
	}

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HERO_AWAKEN_RESP msg;
	msg.m_dwUId = dwHeroUID;
	msg.m_byAwakenStar = wAwakenStar;
	msg.m_byAwakenLevel = wAwakenLevel;
	m_pPlayer->SendPacket(&msg);

	// 成就
	m_pPlayer->OnUpdateAchievement(23, GetFormationAllHeroAwakenStar(), false);
}

// 请求英雄基本数据列表
void CHeroManager::OnNetHeroBaseList()
{
	SendHeroBaseList();
}

// 请求英雄详细信息 
void CHeroManager::OnNetHeroInfo(DWORD dwUID)
{
	HeroInfo *pHeroInfo = FindHero(dwUID);
	if (!pHeroInfo)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_HERO_INFO_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]请求的英雄不存在[UID:%d]",__FUNCTION__, dwUID);
		return;
	}

	MSG_S2C_HERO_INFO_RESP msg;
	memcpy_s(&msg.m_Info, sizeof(HeroInfo), pHeroInfo, sizeof(HeroInfo));
	m_pPlayer->SendPacket(&msg);
}

// 请求收集英雄列表
void CHeroManager::OnNetCollectionList()
{
	MSG_S2C_HERO_COLLECTION_LIST_RESP msg;
	msg.m_byCollectionNum = m_pPlayerHeroInfo->wCollectionNum;
	for (BYTE i = 0; i < msg.m_byCollectionNum; ++i)
	{
		msg.m_CollectionList[i] = m_pPlayerHeroInfo->arCollectionList[i];
	}

	m_pPlayer->SendPacket(&msg);
}

// 请求召唤数据
void CHeroManager::OnNetSummonData()
{
	SendSummonData();
}

// 
DWORD CHeroManager::AddHero(DWORD dwHeroInfoId, bool bNotify, WORD wFromId)
{
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(dwHeroInfoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "找不到英雄的配置数据[HeroInfoId:%d]", dwHeroInfoId);
		return 0;
	}

	HeroInfo sHero;
	sHero.Init();
	sHero.dwUID = GetNewSeriesId();
	sHero.dwIntoId = dwHeroInfoId;
	sHero.wLevel = pHeroCfg->wInitLevel;
	sHero.wQuality = pHeroCfg->wInitQuality;
	//sHero.wTrainDataIndex = 1;

	// 记录获得
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwHeroInfoId, 1, true, wFromId);

	return AddHero(sHero, bNotify) ? sHero.dwUID : 0;
}

// 添加英雄
bool CHeroManager::AddHero(const HeroInfo &rInfo, bool bNotify)
{
	if (GetHero(rInfo.dwUID))
	{
		MAPLOG(ERROR_LEVEL, "添加重复英雄[HeroUId:%d]", rInfo.dwUID);
		return false;
	}

	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄背包已达上限[RoleId:%d,HeroListNum:%d,HeroInfoId:%d]", __FUNCTION__, 
			m_pPlayer->GetRoleId(), m_pPlayerHeroInfo->wHeroNum, rInfo.dwIntoId);

		// 记录英雄丢失记录
		m_pPlayer->SaveItemLoseRecord(rInfo.dwIntoId);

		return false;
	}

	m_pPlayerHeroInfo->arHeroList[m_pPlayerHeroInfo->wHeroNum] = rInfo;
	++m_pPlayerHeroInfo->wHeroNum;

	// 收集英雄
	AddCollectionHero(rInfo.dwIntoId);

	if (bNotify)
		HeroBaseDataNotify(rInfo.dwUID);

	return true;
}

// 移除英雄
bool CHeroManager::RemoveHero(DWORD dwHeroUID, bool bNotify, WORD wFromId)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		HeroInfo &tFind = m_pPlayerHeroInfo->arHeroList[i];
		if (tFind.dwUID == dwHeroUID)
		{
			// 记录获得
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tFind.dwIntoId, 1, false, wFromId);

			HeroBaseDataNotify(dwHeroUID, false, bNotify);
			tFind = m_pPlayerHeroInfo->arHeroList[--m_pPlayerHeroInfo->wHeroNum];
			return true;
		}
	}

	return false;
}

// 移除英雄
bool CHeroManager::RemoveHero(DWORD *pHeroUIDList, WORD wHeroNum, WORD wFromId)
{
	for (WORD i = 0; i < wHeroNum; ++i)
	{
		for (WORD k = 0; k < m_pPlayerHeroInfo->wHeroNum; ++k)
		{
			DWORD dwHeroUID = pHeroUIDList[i];
			HeroInfo &tDeleteHero = m_pPlayerHeroInfo->arHeroList[k];
			if (tDeleteHero.dwUID == dwHeroUID)
			{
				// 记录获得
				g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tDeleteHero.dwIntoId, 1, false, wFromId);

				HeroBaseDataNotify(dwHeroUID, false, false);
				tDeleteHero = m_pPlayerHeroInfo->arHeroList[--m_pPlayerHeroInfo->wHeroNum];
				break;
			}
		}
	}

	MSG_S2C_HERO_BASE_INFO_NOTIFY msg;
	msg.m_byFlag = false;
	msg.m_wHeroNum = 0;
	for (WORD i = 0; i < wHeroNum; ++i)
	{
		ClientHeroBaseData &tBaseData = msg.m_HeroBaseDataList[msg.m_wHeroNum++];
		memset(&tBaseData, 0, sizeof(tBaseData));
		tBaseData.dwUID = pHeroUIDList[i];
	}
	m_pPlayer->SendPacket(&msg);

	return false;
}


// 取英雄
const HeroInfo* CHeroManager::GetHero(DWORD dwHeroUID)
{
	return FindHero(dwHeroUID);
}


// 查找英雄
HeroInfo* CHeroManager::FindHero(DWORD dwHeroUID)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].dwUID == dwHeroUID)
		{
			return &m_pPlayerHeroInfo->arHeroList[i];
		}
	}

	return NULL;
}

// 处理普通召唤
bool CHeroManager::DoNormalSummon(vector<DWORD> &OutHeroList, DWORD dwCostItemId, int nFriendshipPoint)
{
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER_RET(pItemMgr, false);

	// 消耗物品
	if (dwCostItemId)
	{
		if (!pItemMgr->GetItemNum(dwCostItemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄召唤消耗物品数量不足[CostItemId:%d]", __FUNCTION__, dwCostItemId);
			return false;
		}
	}
	// 消息友情点
	else if (nFriendshipPoint > 0)
	{
		if (m_pPlayer->GetProperty(ROLE_PRO_FRIENDSHIP_PIONT) < nFriendshipPoint)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄召唤消耗友情点不足[cost_point:%d]", __FUNCTION__, nFriendshipPoint);
			return false;
		}
	}
	// 消耗免费次数
	else
	{
		// 检测免费次数
		if (m_pPlayerHeroInfo->wNormalFreeSummonTimes >= g_GetConfigValue(CONFIG_NORMAL_FREE_SUMMON_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄召唤免费次数不足[used_times:%d]", 
				__FUNCTION__, m_pPlayerHeroInfo->wNormalFreeSummonTimes);
			return false;
		}

		// 检测CD(允许10秒的误差)
		if (time(NULL) + 10000 < m_pPlayerHeroInfo->dwNormalFreeSummonTime + g_GetConfigValue(CONFIG_NORMAL_SUMMON_CD) * SECOND_OF_MIN)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄召唤免费CD中...",__FUNCTION__);
			return false;
		}
	}

	// 检测英雄包裹是否已满
	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄背包已满,召唤失败!", __FUNCTION__);
		return false;
	}

	// 随机英雄
	DWORD dwRandHeroId = RandNormalSummonHero();
	if (!dwRandHeroId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄召唤随机英雄失败!", __FUNCTION__);
		return false;
	}

	// 添加英雄
	AddHero(dwRandHeroId, true, FROM_HERO_SUMMON);

	// 处理消耗
	if (dwCostItemId)
	{
		pItemMgr->RemoveItem(dwCostItemId, 1, TRUE, FROM_HERO_SUMMON);
	}
	else if (nFriendshipPoint > 0 )
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_FRIENDSHIP_PIONT, -nFriendshipPoint);
	}
	else
	{
		// 减掉次数,设置CD
		++m_pPlayerHeroInfo->wNormalFreeSummonTimes;
		m_pPlayerHeroInfo->dwNormalFreeSummonTime = (DWORD)time(NULL);
	}

	OutHeroList.push_back(dwRandHeroId);

	return true;
}

// 处理高级召唤
bool CHeroManager::DoAdvanceSummon(vector<DWORD> &vOutHeroList, DWORD dwCostItemId, int nCostDiamond, int nSummonTimes)
{
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER_RET(pItemMgr, false);

	// 消耗物品
	if (dwCostItemId)
	{
		if (pItemMgr->GetItemNum(dwCostItemId) < nSummonTimes)
		{
			MAPLOG(ERROR_LEVEL, "[%s]高级英雄召唤消耗物品数量不足[CostItemId:%d,CostItemNum:%d]",__FUNCTION__, dwCostItemId, nSummonTimes);
			return false;
		}
	}
	// 消耗钻石
	else if (nCostDiamond > 0)
	{
		if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			MAPLOG(ERROR_LEVEL, "[%s]高级英雄召唤消耗钻石不足[CostDiamond:%d]", __FUNCTION__, nCostDiamond);
			return false;
		}
	}
	// 消耗免费次数
	else
	{
		// 检测CD(允许10秒的误差)
		if (time(NULL) + 10000 < m_pPlayerHeroInfo->dwHighFreeSummonTime + g_GetConfigValue(CONFIG_ADVANCED_SUMMON_CD) * SECOND_OF_MIN)
		{
			MAPLOG(ERROR_LEVEL, "[%s]高级英雄召唤免费CD中...", __FUNCTION__);
			return false;
		}
	}

	// 检测包裹空间
	if (GetBagEmptySize() < nSummonTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄背包已满,召唤失败!", __FUNCTION__);
		return false;
	}

	// 随机一个英雄
	for (int i = 0; i < nSummonTimes; ++i)
	{
		DWORD dwRandHeroId = RandAdvancedSummonHero();
		if (!dwRandHeroId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]高级英雄召唤随机英雄失败!", __FUNCTION__);
			return false;
		}

		// 记录召唤次数
		m_pPlayerHeroInfo->wHighSummonTimes++;

		// 召唤到一定的次数时重置次数
		if (m_pPlayerHeroInfo->wHighSummonTimes >= g_GetConfigValue(CONFIG_ADVANCED_SUMMON_SPECIAL_TIMES))
		{
			m_pPlayerHeroInfo->wHighSummonTimes = 0;
		}

		vOutHeroList.push_back(dwRandHeroId);

		// 限时招募活动
		//m_pPlayer->GetActivityManager()->GetLimitActivity().AddRecruitTimes();
	}

	// 消耗物品
	if (dwCostItemId)
	{
		pItemMgr->RemoveItem(dwCostItemId, nSummonTimes, TRUE, FROM_HERO_SUMMON);
	}
	// 消耗钻石
	else if (nCostDiamond > 0)
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_HERO_HIGH_SUMMON);
	}
	// 消耗免费次数
	else
	{
		// 设置CD
		m_pPlayerHeroInfo->dwHighFreeSummonTime = time(NULL);
	}

	// 添加英雄
	for (size_t i = 0; i < vOutHeroList.size(); ++i)
	{
		AddHero(vOutHeroList[i], TRUE, FROM_HERO_SUMMON);
	}

	//vOutHeroList.push_back(dwRandHeroId);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(LIMIT_RECRUIT, 10, nSummonTimes, true);

	return true;
}

// 随机一个普通召唤英雄
DWORD CHeroManager::RandNormalSummonHero()
{
	// todo::如果是第一次召唤,则必出指定英雄
	if (!m_pPlayer->CheckOnceFlag(once_normal_summon))
	{
		m_pPlayer->SetOnceFlag(once_normal_summon);
		return g_GetConfigValue(210, 32341);
	}

	// 随机英雄
	DWORD dwTotalRate = CConfigManager::Instance()->GetHeroNormalSummonTotalRate();
	if (!dwTotalRate)
	{
		MAPLOG(ERROR_LEVEL, "[%s]普通召唤总的概率为0!", __FUNCTION__);
		return 0;
	}
	DWORD dwRandRate = Rand::Instance()->urand(0, dwTotalRate - 1);
	const HeroNormalSummon_Config *pNormalSummonCfg = CConfigManager::Instance()->GetHeroNormalSummon(dwRandRate);
	if (!pNormalSummonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到普通召唤概率对应的配置[rate:%d]!", __FUNCTION__, dwRandRate);
		return 0;
	}

	return pNormalSummonCfg->dwHeroId;
}

// 随机一个高级召唤英雄
DWORD CHeroManager::RandAdvancedSummonHero()
{
	DWORD dwRandHeroId = 0;
	if (m_pPlayerHeroInfo->wHighSummonTimes + 1 >= g_GetConfigValue(CONFIG_ADVANCED_SUMMON_SPECIAL_TIMES))
	{
		// 随机英雄
		DWORD dwTotalRate = CConfigManager::Instance()->GetHeroAdvancedSummonTotalRateEx();
		if (!dwTotalRate)
		{
			MAPLOG(ERROR_LEVEL, "[%s]高级召唤总的几率为0!", __FUNCTION__);
			return dwRandHeroId;
		}
		DWORD dwRandRate = Rand::Instance()->urand(0, dwTotalRate - 1);
		const HeroAdvancedSummon_Config *pAdvancedSummonCfg = CConfigManager::Instance()->GetHeroAdvancedSummonEx(dwRandRate);
		if (!pAdvancedSummonCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到高级召唤概率对应的配置[rate:%d]!", __FUNCTION__, dwRandRate);
			return dwRandHeroId;
		}

		dwRandHeroId = pAdvancedSummonCfg->dwHeroId;
	}
	else
	{
		// 随机英雄
		DWORD dwTotalRate = CConfigManager::Instance()->GetHeroAdvancedSummonTotalRate();
		if (!dwTotalRate)
		{
			MAPLOG(ERROR_LEVEL, "[%s]高级召唤总的几率为0!", __FUNCTION__);
			return dwRandHeroId;
		}
		DWORD dwRandRate = Rand::Instance()->urand(0, dwTotalRate - 1);
		const HeroAdvancedSummon_Config *pAdvancedSummonCfg = CConfigManager::Instance()->GetHeroAdvancedSummon(dwRandRate);
		if (!pAdvancedSummonCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到高级召唤概率对应的配置[rate:%d]!", __FUNCTION__, dwRandRate);
			return dwRandHeroId;
		}

		dwRandHeroId = pAdvancedSummonCfg->dwHeroId;
	}

	return dwRandHeroId;
}

// 随机一个VIP召唤英雄
const HeroVipSummon_Config * CHeroManager::RandVipSummonHero(WORD wCamp, bool isSpecial)
{
	// 随机英雄
	DWORD dwTotalRate = g_pCfgMgr->GetHeroVIPSummonTotalRate(wCamp, isSpecial);
	if (!dwTotalRate)
	{
		MAPLOG(ERROR_LEVEL, "[%s]VIP召唤总的几率为0!", __FUNCTION__);
		return NULL;
	}

	DWORD dwRandRate = g_Rand(1, dwTotalRate);
	const HeroVipSummon_Config *pVipSummonCfg = g_pCfgMgr->GetHeroVIPSummon(wCamp, isSpecial, dwRandRate);
	if (!pVipSummonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到VIP召唤概率对应的配置[rate:%d]!", __FUNCTION__, dwRandRate);
		return NULL;
	}

	return pVipSummonCfg;
	//return ObjectData(pVipSummonCfg->dwHeroId, pVipSummonCfg->dwHeroNum);
}

// 取包裹空闲数量
WORD CHeroManager::GetBagEmptySize()
{
	WORD wBagSize = min(MAX_HERO_BAG_NUM, m_pPlayer->GetVipValue(VIP_HERO_BAG_NUM));
	return wBagSize > m_pPlayerHeroInfo->wHeroNum ? wBagSize - m_pPlayerHeroInfo->wHeroNum : 0;
}

// 取英雄数量 
WORD CHeroManager::GetHeroNumByInfoId(DWORD dwHeroInfoId)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].dwIntoId == dwHeroInfoId)
		{
			++wNum;
		}
	}
	return wNum;
}

// 是否上阵
bool CHeroManager::IsToBattle(DWORD dwUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arFormationList[i].dwHeroUID == dwUID)
			return true;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arReinforcementList[i] == dwUID)
			return true;
	}

	return false;
}

// 是否上阵(使用infoid检测)
bool CHeroManager::IsToBattleEx(DWORD dwInfoId, BYTE bySlot)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId && bySlot != i)
			return true;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId)
			return true;
	}

	return false;
}

// 是否可以上阵
bool CHeroManager::IsCanToBattle(DWORD dwInfoId, BYTE bySlot)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		// 同一个英雄不能同时上阵,但可以替换
		if (pHero && pHero->dwIntoId == dwInfoId && i + 1 != bySlot)
			return false;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId)
			return false;
	}

	return true;
}

// 是否可以做援军
bool CHeroManager::IsCanToBattleEx(DWORD dwInfoId, BYTE bySlot)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId)
			return false;
	}

	for (BYTE i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUId = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUId ? GetHero(dwUId) : NULL;
		if (pHero && pHero->dwIntoId == dwInfoId && i + 1 != bySlot)
			return false;
	}

	return true;
}

// 发送英雄列表
void CHeroManager::SendHeroBaseList()
{
	static MSG_S2C_HERO_BASE_LIST_RESP msg;
	msg.m_wHeroBaseNum = m_pPlayerHeroInfo->wHeroNum;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		ClientHeroBaseData &sDest = msg.m_HeroBaseList[i];
		HeroInfo &sSrc = m_pPlayerHeroInfo->arHeroList[i];
		sDest.dwUID = sSrc.dwUID;
		sDest.wInfoId = sSrc.dwIntoId;
		sDest.byLevel = sSrc.wLevel;
		sDest.byBreakLevel = sSrc.wBreakLevel;
		sDest.byQuality = sSrc.wQuality;
		sDest.byQualityStage = sSrc.wQualityStage;
		sDest.dwExp = sSrc.dwExp;
		sDest.byDestinyLevel = sSrc.wDestinyLevel;
		sDest.byIsToBattle = IsToBattle(sSrc.dwUID);
		sDest.byAwakenStar = sSrc.wAwakenStar;
		sDest.byAwakenLevel = sSrc.wAwakenLevel;
		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			sDest.arTrainProList[k].wType = GetTrainProType(k);
			sDest.arTrainProList[k].nValue = sSrc.arTrainProValueList[k];
			sDest.arTrainProLvList[k] = sSrc.arTrainProLvList[k];
		}
		sDest.wAwakenItemNum = sSrc.wAwakenItemNum;
		for (WORD k = 0; k < sSrc.wAwakenItemNum; ++k)
		{
			sDest.arAwakenItemList[k] = sSrc.arAwakenItemList[k];
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// 发送阵型列表
void CHeroManager::SendFormationList()
{
	MSG_S2C_FORMATION_LIST_NOTIFY msg;	
	msg.m_byFormationNum = MAX_BATTLE_HERO_NUM;
	int nCopySize = sizeof(m_pPlayerHeroInfo->arFormationList);
	memcpy_s(msg.m_FormactionList, nCopySize, m_pPlayerHeroInfo->arFormationList, nCopySize);
	msg.m_byReinforcementNum = MAX_REINFORCEMENT_NUM;
	nCopySize = sizeof(msg.m_arReinforcementList);
	memcpy_s(msg.m_arReinforcementList, nCopySize, m_pPlayerHeroInfo->arReinforcementList, nCopySize);
	m_pPlayer->SendPacket(&msg);
}

// 发送召唤数据
void CHeroManager::SendSummonData()
{
	MSG_S2C_HERO_SUMMON_DATA_RESP msg;
	msg.m_byNormalFreeTimes = m_pPlayerHeroInfo->wNormalFreeSummonTimes;
	msg.m_dwNormalFreeTime = m_pPlayerHeroInfo->dwNormalFreeSummonTime;
	msg.m_dwAdvancedFreeTime = m_pPlayerHeroInfo->dwHighFreeSummonTime;
	msg.m_byAdvancedTimes = m_pPlayerHeroInfo->wHighSummonTimes;
	m_pPlayer->SendPacket(&msg);
}

// 填充阵型列表 
void CHeroManager::FillFormationList(ClientFormation *pFormationList, BYTE &byFormationNum)
{
	byFormationNum = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			continue;

		ClientFormation *pClientFormation = &pFormationList[byFormationNum++];

		// 填充英雄信息
		pClientFormation->bySlot = i;
		pClientFormation->wHeroInfoId = pHero->dwIntoId;
		pClientFormation->byAwakenStar = pHero->wAwakenStar;
		pClientFormation->wBreakLevel = pHero->wBreakLevel;
		pClientFormation->byQaulity = pHero->wQuality;

		// 填充装备信息
		pClientFormation->byEquipmentNum = 0;
		for (BYTE k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			DWORD dwEquipmentId = pFormation->arEquipmentList[k];
			const Equipment *pEquipment = dwEquipmentId ? m_pPlayer->GetEquipmentManager()->GetEquipment(dwEquipmentId) : NULL;
			if (pEquipment)
			{
				FormationEquipment *pTmp = &pClientFormation->arEquipmentList[pClientFormation->byEquipmentNum++];
				pTmp->wInfoId = pEquipment->dwInfoId;
				pTmp->wStrengthenLevel = pEquipment->wEnhanceLevel;
				pTmp->byRefineLevel = pEquipment->wRefineLevel;
			}
		}

		// 填充宝物信息
		pClientFormation->byTreasureNum = 0;
		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			DWORD dwTreasureId = pFormation->arTreasureList[k];
			const Treasure *pTreasure = dwTreasureId ? m_pPlayer->GetTreasureManager()->GetTreasure(dwTreasureId) : NULL;
			if (pTreasure)
			{
				FormationTreasure *pTmp = &pClientFormation->arTreasureList[pClientFormation->byTreasureNum++];
				pTmp->wInfoId = pTreasure->dwInfoId;
				pTmp->wStrengthenLevel = pTreasure->wEnhanceLevel;
				pTmp->byRefineLevel = pTreasure->wRefineLevel;
			}
		}
	}
}

// 是否已收集此英雄
bool CHeroManager::IsAlreadyCollection(DWORD dwHeroInfoId)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wCollectionNum; ++i)
	{
		if (m_pPlayerHeroInfo->arCollectionList[i] == dwHeroInfoId)
			return true;
	}
	return false;
}

// 更新培养等级
void CHeroManager::UpdateTrainLevel()
{
	bool bHaveLevelUp = false;

	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		DWORD dwHeroUID = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
		if (!dwHeroUID)
			continue;

		HeroInfo *pHero = FindHero(dwHeroUID);
		if (!pHero)
			continue;

		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			BYTE byProType = GetTrainProType(k);
			int nProValue = pHero->arTrainProValueList[k];
			BYTE byProLevel = pHero->arTrainProLvList[k];
			BYTE byIsTake = pHero->arTrainRewardFlag[k];
			const HeroTrain_Config *pTrainCfg = g_pCfgMgr->GetHeroTrain(byProType, byProLevel);
			// 如果英雄升级后,等级满足,是提升培养等级
			if (pTrainCfg && nProValue >= pTrainCfg->nProMax && pHero->wLevel >= pTrainCfg->wNeedHeroLevel && !byIsTake)
			{
				pHero->arTrainProLvList[k]++;
				bHaveLevelUp = true;
			}
		}
	}

	if (bHaveLevelUp)
	{
	 	// 7天活动事件(英雄培养)
	 	m_pPlayer->OnUpdateSevenDayTarget(15);
	}		
}

// 取被祝福的英雄数量
WORD CHeroManager::GetBlessHeroNum()
{
	WORD wNum = 0;
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arFormationList[i].dwBlessSpriteId)
			wNum++;
	}
	return wNum;
}

// 是否祝福精灵
bool CHeroManager::IsBlessSprite(DWORD dwSpriteId)
{
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerHeroInfo->arFormationList[i].dwBlessSpriteId == dwSpriteId)
			return true;
	}
	return false;
}

// 设置精灵祝福
void CHeroManager::SetSpriteBless(WORD wFormactionPos, DWORD dwSpriteId)
{
	if (wFormactionPos >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]设置精灵祝福阵营越界[pos:%d]", __FUNCTION__, wFormactionPos);
		return;
	}

	m_pPlayerHeroInfo->arFormationList[wFormactionPos].dwBlessSpriteId = dwSpriteId;
}

// 取阵位上的祝福精灵
DWORD CHeroManager::GetBlessSpriteId(WORD wFormactionPos)
{
	if (wFormactionPos >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵营越界[pos:%d]", __FUNCTION__, wFormactionPos);
		return 0;
	}
	return m_pPlayerHeroInfo->arFormationList[wFormactionPos].dwBlessSpriteId;
}

// 取祝福精灵所在的阵位
short CHeroManager::GetBlessSpriteFormactionPos(DWORD dwSpriteId)
{
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormaction = &m_pPlayerHeroInfo->arFormationList[i];
		if (pFormaction->dwBlessSpriteId && pFormaction->dwBlessSpriteId == dwSpriteId)
			return i;
	}
	return -1;
}

// 取所有英雄的战力
// DWORD CHeroManager::GetAllHeroBattleValue()
// {
// 	DWORD dwTotalBattleValue = 0;
// 	for (int i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
// 	{
// 		DWORD dwHeroId = m_pPlayerHeroInfo->arFormationList[i].dwHeroUID;
// 		if (!dwHeroId)
// 			continue;
// 
// 		const HeroInfo *pHeroInfo = GetHero(dwHeroId);
// 		if (pHeroInfo)
// 		{
// 			dwTotalBattleValue += CPropertyManager::CalculateHeroBattleValue(pHeroInfo);
// 		}	
// 	}
// 
// 	return dwTotalBattleValue;
// }

// 添加收集的英雄
void CHeroManager::AddCollectionHero(DWORD dwHeroInfoId)
{
	for (WORD i = 0; i < m_pPlayerHeroInfo->wCollectionNum; ++i)
	{
		if (m_pPlayerHeroInfo->arCollectionList[i] == dwHeroInfoId)
			return;
	}

	if (m_pPlayerHeroInfo->wCollectionNum >= MAX_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "英雄收集列表已满,添加失败[%d]", m_pPlayerHeroInfo->wCollectionNum);
		return;
	}

	m_pPlayerHeroInfo->arCollectionList[m_pPlayerHeroInfo->wCollectionNum++] = dwHeroInfoId;
}

// 取装备被穿戴的英雄ID
WORD CHeroManager::GetEquipmentWearHeroId(DWORD dwEquipemntUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &sFormation = m_pPlayerHeroInfo->arFormationList[i];
		for (BYTE k = 0; k < EQUIPMENT_TYPE_MAX; ++k)
		{
			if (sFormation.arEquipmentList[k] == dwEquipemntUID)
			{				
				const HeroInfo *pHero = GetHero(sFormation.dwHeroUID);
				return pHero ? pHero->dwIntoId : 0;
			}
		}
	}
	return 0;
}

// 取宝物被穿戴的英雄ID 
WORD CHeroManager::GetTreasureWearHeroId(DWORD dwTreasureUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &sFormation = m_pPlayerHeroInfo->arFormationList[i];
		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			if (sFormation.arTreasureList[k] == dwTreasureUID)
			{
				const HeroInfo *pHero = GetHero(sFormation.dwHeroUID);
				return pHero ? pHero->dwIntoId : 0;
			}
		}
	}
	return 0;
}

// 取卡牌被穿戴的英雄ID
DWORD CHeroManager::GetCardWearHeroId(DWORD dwCardUID)
{
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation &sFormation = m_pPlayerHeroInfo->arFormationList[i];
		for (BYTE k = 0; k < MAX_CARD_SLOT_NUM; ++k)
		{
			if (sFormation.arCardList[k] == dwCardUID)
			{
				const HeroInfo *pHero = GetHero(sFormation.dwHeroUID);
				return pHero ? pHero->dwIntoId : 0;
			}
		}
	}
	return 0;
}

// 英雄更新通知
void CHeroManager::HeroBaseDataNotify(DWORD dwUID, BYTE byFlag, bool bNotice)
{
	const HeroInfo *pHero = GetHero(dwUID);
	if (pHero && bNotice)
	{
		MSG_S2C_HERO_BASE_INFO_NOTIFY msg;
		msg.m_byFlag = byFlag;
		msg.m_wHeroNum = 0;
		ClientHeroBaseData &tBaseData = msg.m_HeroBaseDataList[msg.m_wHeroNum++];
		tBaseData.dwUID = pHero->dwUID;
		tBaseData.wInfoId = pHero->dwIntoId;
		tBaseData.byLevel = pHero->wLevel;
		tBaseData.byBreakLevel = pHero->wBreakLevel;
		tBaseData.byIsToBattle = IsToBattle(pHero->dwUID);
		tBaseData.dwExp = pHero->dwExp;
		tBaseData.byQuality = pHero->wQuality;
		tBaseData.byQualityStage = pHero->wQualityStage;
		tBaseData.byAwakenStar = pHero->wAwakenStar;
		tBaseData.byAwakenLevel = pHero->wAwakenLevel;
		tBaseData.byDestinyLevel = pHero->wDestinyLevel;
		for (BYTE i = 0; i < HERO_TRAIN_PRO_NUM; ++i)
		{
			tBaseData.arTrainProList[i].wType = GetTrainProType(i);
			tBaseData.arTrainProList[i].nValue = pHero->arTrainProValueList[i];
			tBaseData.arTrainProLvList[i] = pHero->arTrainProLvList[i];
		}
		tBaseData.wAwakenItemNum = pHero->wAwakenItemNum;
		memcpy_s(tBaseData.arAwakenItemList, sizeof(tBaseData.arAwakenItemList), pHero->arAwakenItemList, pHero->wAwakenItemNum*sizeof(WORD));

		m_pPlayer->SendPacket(&msg);
	}
}

// 通过索引取阵型数据
const Formation* CHeroManager::GetFormationByIndex(BYTE bySlot)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return NULL;

	return &m_pPlayerHeroInfo->arFormationList[bySlot];
}

// 是否穿戴
bool CHeroManager::IsEquipmentWear(BYTE bySlot, DWORD dwEquipmentInfoId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return false;

	Formation &tFormation = m_pPlayerHeroInfo->arFormationList[bySlot];
	for (BYTE i = 0; i < EQUIPMENT_TYPE_MAX; ++i)
	{
		DWORD dwUID = tFormation.arEquipmentList[i];

		if (!dwUID)
			continue;

		const Equipment *pEquipment = m_pPlayer->GetEquipmentManager()->GetEquipment(dwUID);
		if (pEquipment && pEquipment->dwInfoId == dwEquipmentInfoId)
			return true;
	}

	return false;
}

// 是否宝物被穿戴
bool CHeroManager::IsTreasureWear(BYTE bySlot, DWORD dwTreasureInfoId)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return false;

	Formation &tFormation = m_pPlayerHeroInfo->arFormationList[bySlot];
	for (BYTE i = 0; i < TREASURE_TYPE_MAX; ++i)
	{
		DWORD dwUID = tFormation.arTreasureList[i];

		if (!dwUID)
			continue;

		const Treasure *pTreasure = m_pPlayer->GetTreasureManager()->GetTreasure(dwUID);
		if (pTreasure && pTreasure->dwInfoId == dwTreasureInfoId)
			return true;
	}

	return false;
}

// 是否装备的卡牌类型
bool CHeroManager::IsEquipCardType(BYTE bySlot, WORD wCardType)
{
	if (bySlot >= MAX_BATTLE_HERO_NUM)
		return false;

	Formation &tFormation = m_pPlayerHeroInfo->arFormationList[bySlot];
	for (BYTE i = 0; i < MAX_CARD_SLOT_NUM; ++i)
	{
		DWORD dwUID = tFormation.arCardList[i];

		if (!dwUID)
			continue;

		const Card *pCard = m_pPlayer->GetZhanBuManager()->GetCard(dwUID);
		if (!pCard)
			continue;

		const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);

		if (pCardCfg && pCardCfg->byType == wCardType)
			return true;
	}

	return false;
}

// 取到达指定突破等级的英雄数量
WORD CHeroManager::GetHeroNumByBreakLevel(WORD wBreakLevel)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].wBreakLevel >= wBreakLevel)
			wNum++;
	}
	return wNum;
}

// 取到达指定突破等级的英雄数量
WORD CHeroManager::GetHeroNumByDestinyLevel(WORD wDestinyLevel)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
	{
		if (m_pPlayerHeroInfo->arHeroList[i].wDestinyLevel >= wDestinyLevel)
			wNum++;
	}
	return wNum;
}

// 取到达指定培养等级的英雄数量
WORD CHeroManager::GetHeroNumByTrainLevel(WORD wTrainLevel)
{
// todo::此处逻辑为整个背包
 	WORD wNum = 0;
 	for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
 	{
 		HeroInfo *pHero = &m_pPlayerHeroInfo->arHeroList[i];
 		BYTE byMinLevel = pHero->arTrainProLvList[0];
 		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
 		{
 			byMinLevel = min(byMinLevel, pHero->arTrainProLvList[k]);
 		}
 		if (byMinLevel >= wTrainLevel)
 			++wNum;
 	}
 	return wNum;

// 此处逻辑限制为上阵英雄
// 	WORD wNum = 0;
// 	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
// 	{
// 		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
// 		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
// 		if (pHero)
// 		{
// 			BYTE byMinLevel = pHero->arTrainProLvList[0];
// 			for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
// 			{
// 				byMinLevel = min(byMinLevel, pHero->arTrainProLvList[k]);
// 			}
// 			if (byMinLevel >= wTrainLevel)
// 				++wNum;
// 		}
// 	}
// 
// 	return wNum;
}

// 取阵型上英雄的最低等级
WORD CHeroManager::GetFormationHeroMinLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		if (!wMinLevel)
			wMinLevel = pHero->wLevel;
		
		wMinLevel = min(wMinLevel, pHero->wLevel);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// 取阵型上英雄的最低进阶等级（突破等级）
WORD CHeroManager::GetFormationHeroMinBreakLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		if (!wMinLevel)
			wMinLevel = pHero->wBreakLevel;

		wMinLevel = min(wMinLevel, pHero->wBreakLevel);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// 取阵型上英雄的最低培养等级
WORD CHeroManager::GetFormationHeroMinTrainLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		WORD wProLv = pHero->arTrainProLvList[0];
		for (BYTE k = 0; k < HERO_TRAIN_PRO_NUM; ++k)
		{
			wProLv = min(wProLv, pHero->arTrainProLvList[k]);
		}

		if (!wMinLevel)
			wMinLevel = wProLv;

		wMinLevel = min(wMinLevel, wProLv);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// 取阵型上英雄的最低天命等级
WORD CHeroManager::GetFormationHeroMinDestinyLevel()
{
	WORD wMinLevel = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		if (!wMinLevel)
			wMinLevel = pHero->wDestinyLevel;

		wMinLevel = min(wMinLevel, pHero->wDestinyLevel);

		if (!wMinLevel)
			break;
	}

	return wMinLevel;
}

// 取阵型上所有英雄的星数
WORD CHeroManager::GetFormationAllHeroAwakenStar()
{
	WORD wStar = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		if (!pHero)
			return 0;

		wStar += pHero->wAwakenStar;
	}

	return wStar;
}

// 取上阵英雄数量
BYTE CHeroManager::GetToBattleHeroNum()
{
	BYTE byNum = 0;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_pPlayerHeroInfo->arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? GetHero(pFormation->dwHeroUID) : NULL;
		pHero ? byNum++ : NULL;
	}
	return byNum;
}

// 是否有红点
bool CHeroManager::IsHaveRedPoint()
{
	// 当初级招募的免费招募次数立即可使用时
	if (m_pPlayerHeroInfo->wNormalFreeSummonTimes < g_GetConfigValue(CONFIG_NORMAL_FREE_SUMMON_TIMES) &&
		time(NULL) >= m_pPlayerHeroInfo->dwNormalFreeSummonTime + g_GetConfigValue(CONFIG_NORMAL_SUMMON_CD) * SECOND_OF_MIN)
		return true;

	// 当高级招募的免费招募次数立即可使用时
	if (time(NULL) > m_pPlayerHeroInfo->dwHighFreeSummonTime + g_GetConfigValue(CONFIG_ADVANCED_SUMMON_CD) * SECOND_OF_MIN)
		return true;

	// 初级招募有招募道具可用时
	if (m_pPlayer->GetItemManager()->GetItem(g_GetConfigValue(CONFIG_NORMAL_SUMMON_COST_ITEM)))
		return true;

	// 友情点招募
	if (m_pPlayer->GetProperty(ROLE_PRO_FRIENDSHIP_PIONT) >= g_GetConfigValue(129))
		return true;

	return false;
}

// 设置宝石孔位
void CHeroManager::SetGemHole(WORD wFormationSlot, WORD wHole, DWORD dwGemId)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]阵型槽位越界[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return;
	}

	if (wHole >= MAX_GEM_HOLE_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]宝石孔位越界[Hole:%d]", __FUNCTION__, wHole);
		return;
	}

	m_pPlayerHeroInfo->arFormationList[wFormationSlot].arGemList[wHole] = dwGemId;
}

// 取援军的最小等级
WORD CHeroManager::GetReinforcementMinLevel()
{
	WORD wLevel = INVALIDID16;
	for (WORD i = 0; i < MAX_REINFORCEMENT_NUM; ++i)
	{
		DWORD dwUID = m_pPlayerHeroInfo->arReinforcementList[i];
		const HeroInfo *pHero = dwUID ? GetHero(dwUID) : NULL;
		// 还有空阵位,则算0级
		if (!pHero)
		{
			wLevel = 0;
			break;
		}
		wLevel = min(wLevel, pHero->wLevel);
	}
	return wLevel;
}

// 取一个新的系列id
DWORD CHeroManager::GetNewSeriesId()
{
	if (!m_dwSeriesId)
	{
		// 取目前最大的ID
		for (WORD i = 0; i < m_pPlayerHeroInfo->wHeroNum; ++i)
		{
			if (m_pPlayerHeroInfo->arHeroList[i].dwUID > m_dwSeriesId)
				m_dwSeriesId = m_pPlayerHeroInfo->arHeroList[i].dwUID;
		}
		++m_dwSeriesId;
	}

	return m_dwSeriesId++;
}

// 取英雄培养属性上限
int CHeroManager::GetTrainProMax(BYTE byProType, WORD wHeroLevel)
{
	int nProMax = 0;
	const vector<HeroTrain_Config> *pTrainList = g_pCfgMgr->GetHeroTrainList(byProType);
	if (pTrainList)
	{
		for (size_t i = 0; i < pTrainList->size(); ++i)
		{
			if (pTrainList->at(i).wNeedHeroLevel && wHeroLevel >= pTrainList->at(i).wNeedHeroLevel)
				nProMax = pTrainList->at(i).nProMax;
		}
	}
	return nProMax;
}

// 取英雄培养属性上限
int CHeroManager::GetTrainProMaxEx(BYTE byProType, WORD wProLevel)
{
	int nProMax = 0;
	const vector<HeroTrain_Config> *pTrainList = g_pCfgMgr->GetHeroTrainList(byProType);
	if (pTrainList)
	{
		for (size_t i = 0; i < pTrainList->size(); ++i)
		{
			if (wProLevel >= pTrainList->at(i).byProLevel)
				nProMax = max(nProMax, pTrainList->at(i).nProMax);
		}
	}
	return nProMax;
}

// 取培养等级上限
BYTE CHeroManager::GetTrainLevelMax(BYTE byProType)
{
	const vector<HeroTrain_Config> *pTrainList = g_pCfgMgr->GetHeroTrainList(byProType);
	return pTrainList ? pTrainList->size() : 0;
}

// 检测突破消耗的英雄
bool CHeroManager::CheckBreakCostHero(HeroInfo *pBreakHero, DWORD *pForCostHeroList, WORD wForCostHeroNum,
	WORD wNeedToCostHeroNum, vector<DWORD> *pForCostHeroInfoIdList, WORD wCostType)
{
	// 0表示不需要消耗
	if (!wNeedToCostHeroNum)
		return true;

	// 检测消耗的数量
	if (wForCostHeroNum != wNeedToCostHeroNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的英雄数量异常[UId:%d, InfoId:%d, ForCostHeroNum:%d, NeedToCostHeroNum:%d]",
			__FUNCTION__, pBreakHero->dwUID, pBreakHero->dwIntoId, wForCostHeroNum, wNeedToCostHeroNum);
		return false;
	}

	map<DWORD, DWORD> tmp_find_list;

	// 检测消耗英雄是否存在
	for (WORD i = 0; i < wForCostHeroNum; ++i)
	{
		DWORD dwCostHeroUId = pForCostHeroList[i];
		const HeroInfo *pCostHero = GetHero(dwCostHeroUId);
		if (!pCostHero)
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的英雄不存在[InfoId:%d,CostHeroUId:%d]",
				__FUNCTION__, pBreakHero->dwIntoId, dwCostHeroUId);
			return false;
		}

		// 检测同名英雄
		if (wCostType == SAME_NAME)
		{
			if (pBreakHero->dwIntoId != pCostHero->dwIntoId)
			{
				MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的英雄不是同名英雄[BreakHeroInfoId:%d,CostHeroInfoId:%d]",
					__FUNCTION__, pBreakHero->dwIntoId, pCostHero->dwIntoId);
				return false;
			}
		}
		// 同品质英雄
		else if (wCostType == SAME_QUALITY)
		{
			if (pBreakHero->wQuality != pCostHero->wQuality)
			{
				MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的英雄不是同品质英雄[BreakHeroInfoId:%d,CostHeroInfoId:%d,BreakHeroQuality:%d,CostHeroQuality:%d]",
					__FUNCTION__, pBreakHero->dwIntoId, pCostHero->dwIntoId,pBreakHero->wQuality, pCostHero->wQuality);
				return false;
			}
		}
		// 同品质同职业英雄
		else
		{
			const Hero_Config *pBreakHeroCfg = g_pCfgMgr->GetHero(pBreakHero->dwIntoId);
			if (!pBreakHeroCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]",__FUNCTION__, pBreakHero->dwIntoId);
				return false;
			}
			const Hero_Config *pCostHeroCfg = g_pCfgMgr->GetHero(pCostHero->dwIntoId);
			if (!pBreakHeroCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]", __FUNCTION__, pCostHero->dwIntoId);
				return false;
			}

			if ((pBreakHero->wQuality != pCostHero->wQuality) || (pBreakHeroCfg->byProfession != pCostHeroCfg->byProfession))
			{
				MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗同品质同职业英雄出错[BreakHeroInfoId:%d,Quality:%d,Profession:%d][CostHeroInfoId:%d,Quality:%d,Profession:%d]",
					__FUNCTION__, pBreakHero->dwIntoId, pBreakHero->wQuality, pBreakHeroCfg->byProfession,
					pCostHero->dwIntoId, pCostHero->wQuality, pCostHeroCfg->byProfession);
				return false;
			}
		}

		// 是否有重复英雄
		if (tmp_find_list.find(dwCostHeroUId) != tmp_find_list.end())
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄突破消耗的重复英雄[InfoId:%d,CostHeroUId:%d]",
				__FUNCTION__, pBreakHero->dwIntoId, dwCostHeroUId);
			return false;
		}

		tmp_find_list.insert(make_pair(dwCostHeroUId, dwCostHeroUId));

		if (pForCostHeroInfoIdList)
			pForCostHeroInfoIdList->push_back(dwCostHeroUId);
	}

	return true;
}

// 记录英雄突破消耗的英雄
void CHeroManager::RecordBreakCostHero(HeroInfo *pBreakHero, DWORD dwCostHeroInfoId)
{
 	if (pBreakHero->wBreakCostHeroNum >= MAX_HERO_BREAK_COST_HERO_NUM)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]记录英雄突破消耗的英雄失败[RoleId:%d,BreakInfoId:%d,CostHeroUId:%d]",
 			__FUNCTION__, m_pPlayer->GetRoleId(), pBreakHero->dwIntoId, dwCostHeroInfoId);
 		return;
 	}

	pBreakHero->arBreakCostHeroList[pBreakHero->wBreakCostHeroNum++] = dwCostHeroInfoId;
}