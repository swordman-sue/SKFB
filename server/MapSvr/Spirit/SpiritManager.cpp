#include "stdafx.h"
#include "SpiritManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\PlayerManager.h"
#include "..\Hero\HeroManager.h"

CSpriteManager::CSpriteManager()
{

}

CSpriteManager::~CSpriteManager()
{

}

void CSpriteManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pSpriteInfo = &m_pPlayer->GetRoleFullInfo().tSpiritInfo;
}

// 请求精灵数据
void CSpriteManager::OnNetSpiritDataReq()
{
	SendSpiritData();
}

// 世界之心数据请求
void CSpriteManager::OnNetHeartOfWorldData()
{
	SendHeartOfWorldData();
}

// 世界之心升级
void CSpriteManager::OnNetHeartOfWorldLevelUp()
{
	WORD wFromMsgId = C2S_HEART_OF_WORLD_LEVEL_UP_REQ;

	WORD wStage = m_pSpriteInfo->tHeartOfWorld.wStage;
	WORD wLevel = m_pSpriteInfo->tHeartOfWorld.wLevel;

	// 取当前阶配置数据
	const HeartOfWorld_Config *pSpiritCfg = CConfigManager::Instance()->GetHeartOfWorld(wStage);
	if (!pSpiritCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵配置[stage:%d]", __FUNCTION__, wStage);
		return;
	}

	// 计算将要升级的阶与等级
	if (wLevel < pSpiritCfg->vLeveDatalist.size())
	{ // 升等级
		wLevel++;
	}
	else
	{ // 升阶
		// 已达阶段上限
		if (wStage >= CConfigManager::Instance()->GetSpiritStageMax())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]精灵阶段已达上限[stage:%d,level:%d]", __FUNCTION__, wStage, wLevel);
			return;
		}

		wStage++;
		wLevel = 1;

		// 取将要升级的阶配置
		pSpiritCfg = CConfigManager::Instance()->GetHeartOfWorld(wStage);
		if (!pSpiritCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵配置[stage:%d]", __FUNCTION__, wStage);
			return;
		}
	}

	const HeartOfWorld_Config::LevelData *pSpiritLvCfg = pSpiritCfg->GetLevelData(wLevel);
	if (!pSpiritLvCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵阶段等级配置[stage:%d,level:%d]", __FUNCTION__, wStage, wLevel);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	const Item &tCostItem = pSpiritLvCfg->sCostItem;
	if (tCostItem.dwId && pItemMgr->GetItemNum(tCostItem.dwId) < tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]精灵升级消耗不足![ItemId:%d,ItemNum:%d]", __FUNCTION__, tCostItem.dwId, tCostItem.dwNum);
		return;
	}

	// 更新升级以后的数据
	m_pSpriteInfo->tHeartOfWorld.wStage = wStage;
	m_pSpriteInfo->tHeartOfWorld.wLevel = wLevel;

	// 消耗物品
	pItemMgr->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_WORLD_HEART_UPGRADE);

	// 奖励物品
	if (pSpiritLvCfg->sAwardItem.dwId && pSpiritLvCfg->sAwardItem.dwNum)
		m_pPlayer->AddObject(pSpiritLvCfg->sAwardItem.dwId, pSpiritLvCfg->sAwardItem.dwNum, TRUE, FROM_WORLD_HEART_UPGRADE);

	SendHeartOfWorldData();

	// 更新战力
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HEART_OF_WORLD_LEVEL_UP_RESP msg;
	msg.m_byStage = m_pSpriteInfo->tHeartOfWorld.wStage;
	msg.m_byLevel = m_pSpriteInfo->tHeartOfWorld.wLevel;
	m_pPlayer->SendPacket(&msg);
}

// 精灵强化
void CSpriteManager::OnNetSpiritEnhance(DWORD dwSpiritId)
{
	WORD wFromMsgId = C2S_SPIRIT_ENHANCE_REQ;

	// 取精灵数据
	SpiritData *pSpirit = GetSpirit(dwSpiritId);
	if (!pSpirit)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]还木有获胜此精灵,不能强化[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	const Spirit_Config *pSpiritCfg = g_pCfgMgr->GetSpirit(dwSpiritId);
	if (!pSpiritCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵配置[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// 取精灵强化配置
	const SpiritEnhance_Config *pEnhaceCfg = g_pCfgMgr->GetSpiritEnhance(dwSpiritId, pSpirit->wEnhanceLevel);
	if (!pEnhaceCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵强化配置[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// 检测是否到达等级上限
	if (!g_pCfgMgr->GetSpiritEnhance(dwSpiritId, pSpirit->wEnhanceLevel+1))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]精灵强化等级已达上限[spirit_id:%d,enhance_level:%d]", 
			__FUNCTION__, dwSpiritId, pSpirit->wEnhanceLevel);
		return;
	}

	// 检测消耗
	if (!m_pPlayer->CheckItemCost(pEnhaceCfg->vCostItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]精灵强化道具不足,不能强化[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// 处理消耗
	for (size_t i = 0; i < pEnhaceCfg->vCostItemList.size(); ++i)
	{
		const Item &tItem = pEnhaceCfg->vCostItemList[i];
		m_pPlayer->GetItemManager()->RemoveItem(tItem.dwId, tItem.dwNum, TRUE, FROM_SPRITE_ENHANCE);
	}

	// 升级
	pSpirit->wEnhanceLevel++;

	// 处理图鉴升级
	SpiritPokedex *pPokedex = GetPokedex(pSpiritCfg->wPokedexId);
	if (pPokedex)
	{
		const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(pPokedex->wPokedexId, pPokedex->wLevel + 1);
		if (pPokedexCfg && GetEnhanceLevel(pSpiritCfg->wPokedexId) >= pPokedexCfg->wNeedEnhanceLevel)
		{
			pPokedex->wLevel++;
			SendSpiritData();
		}
	}

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_SPIRIT_ENHANCE_RESP msg;
	msg.m_dwSpiritId = dwSpiritId;
	m_pPlayer->SendPacket(&msg);

	// 25：精灵强化等级（只算最高的那一只）
	m_pPlayer->OnUpdateSevenDayTarget(25, pSpirit->wEnhanceLevel, false);

	// 成就事件(精灵最高强化到X级)
	m_pPlayer->OnUpdateAchievement(26, pSpirit->wEnhanceLevel, false);
}

// 精灵合成
void CSpriteManager::OnNetCompose(DWORD dwFragmentId)
{
	// 取碎片配置
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(dwFragmentId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵碎片配置[dwFragmentId:%d]", __FUNCTION__, dwFragmentId);
		return;
	}

	DWORD dwSpriteId = pItemCfg->nParam1;
	DWORD dwCostFragmentNum = pItemCfg->nParam2;

	// 是否已拥有此精灵
	if (HaveSpirit(dwSpriteId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]已经拥有此精灵[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}
	
	// 检测合成数量
	if (m_pPlayer->GetItemNum(dwFragmentId) < dwCostFragmentNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]精灵碎片不足,不能合成[FragmentId:%d,CostFragmentNum:%d]", 
			__FUNCTION__, dwFragmentId, dwCostFragmentNum);
		return;
	}

	m_pPlayer->RemoveItem(dwFragmentId, dwCostFragmentNum, FROM_SPRITE_COMPOSE);

	AddSpirit(dwSpriteId, FROM_SPRITE_COMPOSE);

	// 响应
	MSG_S2C_SPRITE_COMPOSE_RESP msg;
	msg.m_dwFragmentId = dwFragmentId;
	m_pPlayer->SendPacket(&msg);
}

// 精灵培养
void CSpriteManager::OnNetTrainEx(DWORD dwSpriteId)
{
	WORD wFromMsgId = C2S_SPIRIT_TRAIN_REQ;

	// 检测是否有此精灵
	SpiritData *pSpriteData = GetSpirit(dwSpriteId);
	if (!pSpriteData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此精灵[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// 取培养配置
	const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(dwSpriteId, pSpriteData->wTrainStage);
	if (!pTrainCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// 检测是否最后一阶
	if (!pTrainCfg->wNeedTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]精灵已到达最后一阶[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// 升阶
	if (pSpriteData->wTrainTimes >= pTrainCfg->wNeedTrainTimes)
	{
		// 检测需要的强化等级
		if (pSpriteData->wEnhanceLevel < pTrainCfg->wNeedEnhanceLevel)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]需要的强化等级不足[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		// 检测消耗的物品
		const Item &tCostItem = pTrainCfg->tStageUpCostItem;
		if (m_pPlayer->GetItemNum(tCostItem.dwId) < tCostItem.dwNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]精灵升级消耗的物品不足[spirit_id:%d,item_id:%d]", __FUNCTION__, dwSpriteId, tCostItem.dwId);
			return;
		}

		pSpriteData->wTrainTimes = 0;
		pSpriteData->wTrainStage++;

		// 删除物品
		m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);

		// 26：精灵升华阶段（只算最高的那一只）
		m_pPlayer->OnUpdateSevenDayTarget(26, pSpriteData->wTrainStage, false);

		// 成就事件(精灵最高升华到X级)
		m_pPlayer->OnUpdateAchievement(27, pSpriteData->wTrainStage, false);
	}
	// 培养
	else
	{
		// 检测消耗的物品
		if (!m_pPlayer->CheckItemCost(pTrainCfg->vTrainCostItemList))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]精灵培养消耗的物品不足[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		pSpriteData->wTrainTimes++;

		// 删除消耗的物品
		for (size_t i = 0; i < pTrainCfg->vTrainCostItemList.size(); ++i)
		{
			const Item &tCostItem = pTrainCfg->vTrainCostItemList[i];
			m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);
		}
	}

	SendSpiritData();

	// 更新属性
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_SPIRIT_TRAIN_RESP msg;
	msg.m_dwSpriteId = dwSpriteId;
	m_pPlayer->SendPacket(&msg);
}

// 精灵培养
void CSpriteManager::OnNetTrain(DWORD dwSpriteId, WORD wTrainTimes)
{
	WORD wFromMsgId = C2S_SPIRIT_TRAIN_REQ;

	// 检测是否有此精灵
	SpiritData *pSpriteData = GetSpirit(dwSpriteId);
	if (!pSpriteData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此精灵[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	for (WORD i = 0; i < wTrainTimes; ++i)
	{
		// 取培养配置
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(dwSpriteId, pSpriteData->wTrainStage);
		if (!pTrainCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		// 检测是否最后一阶
		if (!pTrainCfg->wNeedTrainTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]精灵已到达最后一阶[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		// 升阶(升华)
		if (pSpriteData->wTrainTimes >= pTrainCfg->wNeedTrainTimes)
		{
			// 检测需要的强化等级
			if (pSpriteData->wEnhanceLevel < pTrainCfg->wNeedEnhanceLevel)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]需要的强化等级不足[spirit_id:%d]", __FUNCTION__, dwSpriteId);
				return;
			}

			// 检测消耗的物品
			const Item &tCostItem = pTrainCfg->tStageUpCostItem;
			if (m_pPlayer->GetItemNum(tCostItem.dwId) < tCostItem.dwNum)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]精灵升级消耗的物品不足[spirit_id:%d,item_id:%d]", __FUNCTION__, dwSpriteId, tCostItem.dwId);
				return;
			}

			pSpriteData->wTrainTimes = 0;
			pSpriteData->wTrainStage++;

			// 删除物品
			m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);

			// 26：精灵升华阶段（只算最高的那一只）
			m_pPlayer->OnUpdateSevenDayTarget(26, pSpriteData->wTrainStage, false);

			// 成就事件(精灵最高升华到X级)
			m_pPlayer->OnUpdateAchievement(27, pSpriteData->wTrainStage, false);
		}
		// 培养
		else
		{
			// 检测消耗的物品
			if (!m_pPlayer->CheckItemCost(pTrainCfg->vTrainCostItemList))
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]精灵培养消耗的物品不足[spirit_id:%d]", __FUNCTION__, dwSpriteId);
				return;
			}

			pSpriteData->wTrainTimes++;

			// 删除消耗的物品
			for (size_t k = 0; k < pTrainCfg->vTrainCostItemList.size(); ++k)
			{
				const Item &tCostItem = pTrainCfg->vTrainCostItemList[k];
				m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);
			}
		}
	}

	SendSpiritData();

	// 更新属性
	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_SPIRIT_TRAIN_RESP msg;
	msg.m_dwSpriteId = dwSpriteId;
	msg.m_wTrainTimes = wTrainTimes;
	m_pPlayer->SendPacket(&msg);

	// 节日活动
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 34, wTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 34, wTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 34, wTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 34, wTrainTimes, true);
}

// 精灵升星
void CSpriteManager::OnNetStarUp(DWORD dwSpriteId)
{
	// 检测是否有此精灵
	SpiritData *pSpriteData = GetSpirit(dwSpriteId);
	if (!pSpriteData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此精灵[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// 取精灵配置
	const Spirit_Config *pSpriteCfg = g_pCfgMgr->GetSpirit(dwSpriteId);
	if (!pSpriteCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// 检测是否到达星级上限
	if (pSpriteData->wStarLevel >= pSpriteCfg->wStarMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]精灵星级已达上限[SpriteId:%d,StarMax:%d]", __FUNCTION__, dwSpriteId, pSpriteCfg->wStarMax);
		return;
	}

	// 取升星配置
	const SpriteStarUp_Config *pStarUpCfg = g_pCfgMgr->GetSpriteStarUp(pSpriteData->wStarLevel);
	if (!pStarUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵升星配置[Star:%d]", __FUNCTION__, pSpriteData->wStarLevel);
		return;
	}

	// 检测消耗
	if (m_pPlayer->GetItemNum(pSpriteCfg->dwTtarUpCostItemId) < pStarUpCfg->dwStarUpCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]精灵升星消耗的物品不足[SpriteId:%d,Star:%d]", __FUNCTION__, dwSpriteId, pSpriteData->wStarLevel);
		return;
	}

	// 删除消耗物品
	m_pPlayer->RemoveItem(pSpriteCfg->dwTtarUpCostItemId, pStarUpCfg->dwStarUpCostItemNum, FROM_SPRITE_STAR_UP);

	// 升星
	pSpriteData->wStarLevel++;

	m_pPlayer->UpdateFightingPower();

	// 响应
	MSG_S2C_SPRITE_STAR_UP_RESP msg;
	msg.m_dwSpriteId = dwSpriteId;
	m_pPlayer->SendPacket(&msg);
}

// 精灵祝福
void CSpriteManager::OnNetBless(WORD wBlessType, WORD wFormationPos, DWORD dwSpiritId)
{
	MAPLOG(GUI_LEVEL, "[%s]精灵祝福[BlessType:%d,FormationPos:%d,SpriteId:%d]", 
		__FUNCTION__, wBlessType, wFormationPos, dwSpiritId);

	// 已上阵的精灵不能祝福
	if (dwSpiritId && GetToBattleSpiritId() == dwSpiritId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]已上阵的精灵不能祝福[SpriteId:%d]", __FUNCTION__,dwSpiritId);
		return;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	DWORD dwOldSpriteId = pHeroMgr->GetBlessSpriteId(wFormationPos);

	int nNewSpriteFormationPos = pHeroMgr->GetBlessSpriteFormactionPos(dwSpiritId);

	//WORD wBlessType = GetBlessType(dwOldSpriteId, dwSpiritId);

	// 祝福
// 	if (wBlessType == 1)
// 	{
// 		// 检测此位置是否已有祝福
// 		if (dwOldSpriteId)
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
// 			MAPLOG(ERROR_LEVEL, "[%s]此位置上已有祝福[RoleId:%d,Level:%d,CurNum:%d]", __FUNCTION__,
// 				m_pPlayer->GetRoleId(), m_pPlayer->GetLevel(), GetBlessSpriteNum());
// 			return;
// 		}
// 
// 		// 检测可祝福数量
// 		if (GetBlessSpriteNum() >= GetCurMaxBlessSpriteNum())
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
// 			MAPLOG(ERROR_LEVEL, "[%s]当前精灵祝福数量已达上限[RoleId:%d,Level:%d,CurNum:%d]", __FUNCTION__, 
// 				m_pPlayer->GetRoleId(), m_pPlayer->GetLevel(), GetBlessSpriteNum());
// 			return;
// 		}
// 	}
// 	// 替换
// 	else if (wBlessType == 2)
// 	{
// 		// 检测此位置是否已有祝福
//  		if (!dwOldSpriteId)
//  		{
//  			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
//  			MAPLOG(ERROR_LEVEL, "[%s]此位置上没有祝福,不能替换[wFormationPos:%d]", __FUNCTION__,wFormationPos);
//  			return;
//  		}
// 	}
// 	// 取消祝福
// 	else
// 	{
// 		// 检测此位置是否已有祝福
// 		if (dwSpiritId)
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
// 			MAPLOG(ERROR_LEVEL, "[%s]取消祝福的目标要为非0[OldSpriteId:%d]", __FUNCTION__, dwSpiritId);
// 			return;
// 		}
// 	}

	// 检测新的精灵是否之前已祝福别的英雄, 有则置空
	if (nNewSpriteFormationPos >= 0)
		pHeroMgr->SetSpriteBless(nNewSpriteFormationPos, 0);

	// 设置精灵
	pHeroMgr->SetSpriteBless(wFormationPos, dwSpiritId);

	// 检测可祝福数量
	if (GetBlessSpriteNum() > GetCurMaxBlessSpriteNum())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当前精灵祝福数量已达上限[RoleId:%d, Level:%d, CurBlessNum:%d, BlessNumMax:%d]", __FUNCTION__,
			m_pPlayer->GetRoleId(), m_pPlayer->GetLevel(), GetBlessSpriteNum(), GetCurMaxBlessSpriteNum());

		// 纠正操作
		pHeroMgr->SetSpriteBless(wFormationPos, 0);
		m_pPlayer->GetHeroManager()->SendFormationList();
		return;
	}

	m_pPlayer->GetHeroManager()->SendFormationList();

	// 更新属性
	m_pPlayer->UpdateFightingPower();
	
	// 响应
	MSG_S2C_SPRITE_BLESS_RESP msg;
	msg.m_wBlessType = wBlessType;
	msg.m_wFormationPos = wFormationPos;
	msg.m_dwSpriteId = dwSpiritId;
	m_pPlayer->SendPacket(&msg);
}

// 使用精灵
void CSpriteManager::OnNetSpiritUse(DWORD dwSpiritId)
{
	WORD wFromMsgId = C2S_SPIRIT_USE_REQ;

	// 是否已获胜此精灵
	if (!HaveSpirit(dwSpiritId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]还木有获胜此精灵,不能使用[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// 已祝福的精灵不能上阵
	if (m_pPlayer->GetHeroManager()->IsBlessSprite(dwSpiritId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]已祝福的精灵不能上阵[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	m_pSpriteInfo->dwCurUseSpiritId = dwSpiritId;

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_SPIRIT_USE_RESP msg;
	msg.m_dwSpiritId = dwSpiritId;
	m_pPlayer->SendPacket(&msg);
}

// 是否有此精灵
bool CSpriteManager::HaveSpirit(DWORD dwSpiritId)
{
	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		if (m_pSpriteInfo->arSpiritList[i].dwId == dwSpiritId)
			return true;
	}
	return false;
}

// 添加精灵
bool CSpriteManager::AddSpirit(DWORD dwSpiritId, WORD wFromId)
{
	const Spirit_Config *pSpiritCfg = g_pCfgMgr->GetSpirit(dwSpiritId);
	if (!pSpiritCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵配置[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return false;
	}

	// 如果已有精灵，则变成碎片
	if (HaveSpirit(dwSpiritId))
	{
		m_pPlayer->AddObject(pSpiritCfg->dwTtarUpCostItemId, pSpiritCfg->dwFragmentNum, TRUE, wFromId);
		//MAPLOG(ERROR_LEVEL, "[%s]重复添加精灵[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return true;
	}

	SpiritPokedex *pPokedex = GetPokedex(pSpiritCfg->wPokedexId);

	SpiritData tSprite;
	tSprite.dwId = dwSpiritId;
	tSprite.wEnhanceLevel = 0;
	tSprite.wTrainStage = 1;
	tSprite.wTrainTimes = 0;
	tSprite.wStarLevel = pSpiritCfg->wInitStar;

	if (!AddSpirit(tSprite))
	{
		MAPLOG(ERROR_LEVEL, "[%s]重复精灵失败[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return false;
	}

	// 记录获得
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwSpiritId, 1, true, wFromId);

	// 是否激活图鉴
	if (!pPokedex && IsPokedexActivate(pSpiritCfg->wPokedexId))
	{
		const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(pSpiritCfg->wPokedexId, 1);
		if (pPokedexCfg && GetEnhanceLevel(pSpiritCfg->wPokedexId) >= pPokedexCfg->wNeedEnhanceLevel)
		{
			AddPokedex(pSpiritCfg->wPokedexId);
			m_pPlayer->UpdateFightingPower();
		}
	}

	SendSpiritData();

	return true;
}

// 添加精灵
bool CSpriteManager::AddSpirit(const SpiritData &tSpriteData)
{
	if (m_pSpriteInfo->wSpiritNum >= MAX_SPIRIT_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精灵列表已满,添加失败!", __FUNCTION__);
		return false;
	}

	m_pSpriteInfo->arSpiritList[m_pSpriteInfo->wSpiritNum++] = tSpriteData;
	return true;
}

// 发送精灵数据
void CSpriteManager::SendSpiritData()
{
	// todo::为了兼容旧数据
	for (int i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		SpiritData &tSpriteData = m_pSpriteInfo->arSpiritList[i];

		if (!tSpriteData.wTrainStage)
			tSpriteData.wTrainStage = 1;

		if (!tSpriteData.wStarLevel)
		{
			const Spirit_Config *pSpriteCfg = g_pCfgMgr->GetSpirit(tSpriteData.dwId);
			if (pSpriteCfg)
				tSpriteData.wStarLevel = pSpriteCfg->wInitStar;
		}
	}

	MSG_S2C_SPIRIT_DATA_RESP msg;
	msg.m_tSpriteInfo = *m_pSpriteInfo;
	m_pPlayer->SendPacket(&msg);
}

// 取图鉴强化等级
WORD CSpriteManager::GetEnhanceLevel(WORD wPokedexId)
{
	const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(wPokedexId, 1);
 	if (!pPokedexCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵图鉴[pokedex_id:%d]", __FUNCTION__, wPokedexId);
 		return 0;
 	}
 
 	// 取强化等级
	WORD wEnhanceLevel = 0;
 	for (size_t i = 0; i < pPokedexCfg->vNeedSpiritList.size(); ++i)
 	{
 		SpiritData *pSpirit = GetSpirit(pPokedexCfg->vNeedSpiritList[i]);
 		if (!pSpirit)
 		{
 			wEnhanceLevel = 0;
 			break;
 		}

		// 默认取第一个的值
		if (i == 0)
			wEnhanceLevel = pSpirit->wEnhanceLevel;
		else
			wEnhanceLevel = min(pSpirit->wEnhanceLevel, wEnhanceLevel);	
 	} 
	return wEnhanceLevel;
}

// 是否图鉴已激活
bool CSpriteManager::IsPokedexActivate(WORD wPokedexId)
{
	const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(wPokedexId, 1);
	if (!pPokedexCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到精灵图鉴[pokedex_id:%d]", __FUNCTION__, wPokedexId);
		return false;
	}
	for (size_t i = 0; i < pPokedexCfg->vNeedSpiritList.size(); ++i)
	{
		SpiritData *pSpirit = GetSpirit(pPokedexCfg->vNeedSpiritList[i]);
		if (!pSpirit)
			return false;
	}
	return true;
}

// 添加图鉴
void CSpriteManager::AddPokedex(WORD wPokedexId)
{
	if (m_pSpriteInfo->wPokedexNum >= MAX_SPIRIT_POKEDEX_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]精灵图鉴列表已满,添加失败[pokedex_id:%d]", __FUNCTION__, wPokedexId);
		return;
	}

	SpiritPokedex pokedex;
	pokedex.wPokedexId = wPokedexId;
	pokedex.wLevel = 1;
	m_pSpriteInfo->arPokedexList[m_pSpriteInfo->wPokedexNum++] = pokedex;
}

// 发送世界之心数据
void CSpriteManager::SendHeartOfWorldData()
{
	MSG_S2C_HEART_OF_WORLD_DATA_RESP msg;
	msg.m_byStage = m_pSpriteInfo->tHeartOfWorld.wStage;
	msg.m_byLevel = m_pSpriteInfo->tHeartOfWorld.wLevel;
	m_pPlayer->SendPacket(&msg);
}

// 取出战精灵ID
DWORD CSpriteManager::GetToBattleSpiritId()
{
	return m_pSpriteInfo->dwCurUseSpiritId;
}

// 取精灵强化等级
WORD CSpriteManager::GetSpiritEnhanceLevel(DWORD dwSpiritId)
{
	SpiritData *pSpirit = GetSpirit(dwSpiritId);
	return pSpirit ? pSpirit->wEnhanceLevel : 0;
}

// 取精灵培养次数
WORD CSpriteManager::GetSpiritTrainTimes(DWORD dwSpiritId)
{
	SpiritData *pSpirit = GetSpirit(dwSpiritId);
	return pSpirit ? pSpirit->wTrainTimes : 0;
}

// 取精灵数据
const SpiritData* CSpriteManager::GetSpiritEx(DWORD dwSpiritId)
{
	return GetSpirit(dwSpiritId);
}

// 取祝福精灵数量
WORD CSpriteManager::GetBlessSpriteNum()
{
	return m_pPlayer->GetHeroManager()->GetBlessHeroNum();
}

// 取当前祝福精灵数量上限
WORD CSpriteManager::GetCurMaxBlessSpriteNum()
{
	const RoleLevel_Config *pRoleLevelCfg = g_pCfgMgr->GetRoleLevel(m_pPlayer->GetLevel());
	if (!pRoleLevelCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]找不到角色等级配置[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return 0;
	}

	return pRoleLevelCfg->dwBlessSpriteNumMax;
}

// 取祝福英雄列表
void CSpriteManager::GetBlessSpriteList(vector<SpiritData> &vSpriteList)
{
	vSpriteList.clear();
	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		vSpriteList.push_back(m_pSpriteInfo->arSpiritList[i]);
	}
}

// 取祝福类型
WORD CSpriteManager::GetBlessType(DWORD dwOldSpriteId, DWORD dwSpiritId)
{
	WORD wType = 0;

	// 祝福, 替换
	if (dwSpiritId)
	{
		// 替换
		if (dwOldSpriteId)
		{
			wType = 2;
		}
		// 祝福
		else
		{
			wType = 1;
		}
	}
	// 取消祝福
	else
	{
		wType = 3;
	}

	return wType;
}

// 取额外奖励的掉落物品信息
void CSpriteManager::GetExtraRewardDropItemInfo(WORD wFBType, map<DWORD, int> &mItemInfoList)
{
	mItemInfoList.clear();

	// 木有精灵
	if (!m_pSpriteInfo->wSpiritNum)
		return;

	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		SpiritData &tSpriteData = m_pSpriteInfo->arSpiritList[i];

		// 取培养配置
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(tSpriteData.dwId, tSpriteData.wTrainStage);
		if (!pTrainCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置数据[SpriteId:%d, TrainStage:%d]", 
				__FUNCTION__, tSpriteData.dwId, tSpriteData.wTrainStage);
			continue;
		}

		// 取天赋技能配置
		const SpriteTalentSkill_Config *pTalentSkillCfg = g_pCfgMgr->GetSpriteTalentSkill(tSpriteData.dwId, pTrainCfg->wTalentLevel);
		if (!pTalentSkillCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵天赋技能配置数据[SpriteId:%d,TalentLevel:%d]", 
				__FUNCTION__, tSpriteData.dwId, pTrainCfg->wTalentLevel);
			continue;
		}

		// 不是额外奖励的副本
		if (!pTalentSkillCfg->IsExtraRewardFB(wFBType))
			continue;

		for (size_t k = 0; k < pTalentSkillCfg->vExtraRewardDropItemId.size(); ++k)
		{
			int nItemId = pTalentSkillCfg->vExtraRewardDropItemId[k];
			int nRate = k >= pTalentSkillCfg->vExtraRewardDropItemRate.size() ? 0 : pTalentSkillCfg->vExtraRewardDropItemRate[k];

			map<DWORD, int>::iterator iter = mItemInfoList.find(nItemId);
			if (iter != mItemInfoList.end())
			{
				iter->second += nRate;
			}
			else
			{
				mItemInfoList.insert(make_pair(nItemId, nRate));
			}
		}
	}
}

// 取额外奖励的货币信息
void CSpriteManager::GetExtraRewardCurrencyInfo(WORD wFBType, map<int, int> &mCurrencyInfoList)
{
	mCurrencyInfoList.clear();

	// 木有精灵
	if (!m_pSpriteInfo->wSpiritNum)
		return;

	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		SpiritData &tSpriteData = m_pSpriteInfo->arSpiritList[i];

		// 取培养配置
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(tSpriteData.dwId, tSpriteData.wTrainStage);
		if (!pTrainCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵培养配置数据[SpriteId:%d, TrainStage:%d]",
				__FUNCTION__, tSpriteData.dwId, tSpriteData.wTrainStage);
			continue;
		}

		// 取天赋技能配置
		const SpriteTalentSkill_Config *pTalentSkillCfg = g_pCfgMgr->GetSpriteTalentSkill(tSpriteData.dwId, pTrainCfg->wTalentLevel);
		if (!pTalentSkillCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到精灵天赋技能配置数据[SpriteId:%d,TalentLevel:%d]",
				__FUNCTION__, tSpriteData.dwId, pTrainCfg->wTalentLevel);
			continue;
		}

		// 不是额外奖励的副本
		if (!pTalentSkillCfg->IsExtraRewardFB(wFBType))
			continue;

		for (size_t k = 0; k < pTalentSkillCfg->vExtraRewardCurrencyType.size(); ++k)
		{
			int nCurrencyType = pTalentSkillCfg->vExtraRewardCurrencyType[k];
			int nRate = k >= pTalentSkillCfg->vExtraRewardCurrencyRate.size() ? 0 : pTalentSkillCfg->vExtraRewardCurrencyRate[k];

			map<int, int>::iterator iter = mCurrencyInfoList.find(nCurrencyType);
			if (iter != mCurrencyInfoList.end())
			{
				iter->second += nRate;
			}
			else
			{
				mCurrencyInfoList.insert(make_pair(nCurrencyType, nRate));
			}
		}
	}
}

// 取额外奖励的货币系数
int CSpriteManager::GetExtraRewardCurrencyValue(const map<int, int> &mCurrencyInfoList, int nCurrencyType, int nCurrencyValue)
{
	int nExtraRewardValue = 0;
	map<int, int>::const_iterator iter = mCurrencyInfoList.find(nCurrencyType);
	if (iter != mCurrencyInfoList.end())
	{
		nExtraRewardValue = (nCurrencyValue * iter->second * PROPERTY_DECREASE_MULTIPLE);
	}
	return nExtraRewardValue;
}


// 取精灵数据
SpiritData* CSpriteManager::GetSpirit(DWORD dwSpiritId)
{
	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		if (m_pSpriteInfo->arSpiritList[i].dwId == dwSpiritId)
			return &m_pSpriteInfo->arSpiritList[i];
	}
	return NULL;
}

// 取精灵图鉴
SpiritPokedex* CSpriteManager::GetPokedex(WORD wPokedexId)
{
	for (WORD i = 0; i < m_pSpriteInfo->wPokedexNum; ++i)
	{
		if (m_pSpriteInfo->arPokedexList[i].wPokedexId == wPokedexId)
			return &m_pSpriteInfo->arPokedexList[i];
	}
	return NULL;
}