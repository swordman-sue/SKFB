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

// ����������
void CSpriteManager::OnNetSpiritDataReq()
{
	SendSpiritData();
}

// ����֮����������
void CSpriteManager::OnNetHeartOfWorldData()
{
	SendHeartOfWorldData();
}

// ����֮������
void CSpriteManager::OnNetHeartOfWorldLevelUp()
{
	WORD wFromMsgId = C2S_HEART_OF_WORLD_LEVEL_UP_REQ;

	WORD wStage = m_pSpriteInfo->tHeartOfWorld.wStage;
	WORD wLevel = m_pSpriteInfo->tHeartOfWorld.wLevel;

	// ȡ��ǰ����������
	const HeartOfWorld_Config *pSpiritCfg = CConfigManager::Instance()->GetHeartOfWorld(wStage);
	if (!pSpiritCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[stage:%d]", __FUNCTION__, wStage);
		return;
	}

	// ���㽫Ҫ�����Ľ���ȼ�
	if (wLevel < pSpiritCfg->vLeveDatalist.size())
	{ // ���ȼ�
		wLevel++;
	}
	else
	{ // ����
		// �Ѵ�׶�����
		if (wStage >= CConfigManager::Instance()->GetSpiritStageMax())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]����׶��Ѵ�����[stage:%d,level:%d]", __FUNCTION__, wStage, wLevel);
			return;
		}

		wStage++;
		wLevel = 1;

		// ȡ��Ҫ�����Ľ�����
		pSpiritCfg = CConfigManager::Instance()->GetHeartOfWorld(wStage);
		if (!pSpiritCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[stage:%d]", __FUNCTION__, wStage);
			return;
		}
	}

	const HeartOfWorld_Config::LevelData *pSpiritLvCfg = pSpiritCfg->GetLevelData(wLevel);
	if (!pSpiritLvCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������׶εȼ�����[stage:%d,level:%d]", __FUNCTION__, wStage, wLevel);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	const Item &tCostItem = pSpiritLvCfg->sCostItem;
	if (tCostItem.dwId && pItemMgr->GetItemNum(tCostItem.dwId) < tCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����������Ĳ���![ItemId:%d,ItemNum:%d]", __FUNCTION__, tCostItem.dwId, tCostItem.dwNum);
		return;
	}

	// ���������Ժ������
	m_pSpriteInfo->tHeartOfWorld.wStage = wStage;
	m_pSpriteInfo->tHeartOfWorld.wLevel = wLevel;

	// ������Ʒ
	pItemMgr->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_WORLD_HEART_UPGRADE);

	// ������Ʒ
	if (pSpiritLvCfg->sAwardItem.dwId && pSpiritLvCfg->sAwardItem.dwNum)
		m_pPlayer->AddObject(pSpiritLvCfg->sAwardItem.dwId, pSpiritLvCfg->sAwardItem.dwNum, TRUE, FROM_WORLD_HEART_UPGRADE);

	SendHeartOfWorldData();

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_HEART_OF_WORLD_LEVEL_UP_RESP msg;
	msg.m_byStage = m_pSpriteInfo->tHeartOfWorld.wStage;
	msg.m_byLevel = m_pSpriteInfo->tHeartOfWorld.wLevel;
	m_pPlayer->SendPacket(&msg);
}

// ����ǿ��
void CSpriteManager::OnNetSpiritEnhance(DWORD dwSpiritId)
{
	WORD wFromMsgId = C2S_SPIRIT_ENHANCE_REQ;

	// ȡ��������
	SpiritData *pSpirit = GetSpirit(dwSpiritId);
	if (!pSpirit)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ľ�л�ʤ�˾���,����ǿ��[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	const Spirit_Config *pSpiritCfg = g_pCfgMgr->GetSpirit(dwSpiritId);
	if (!pSpiritCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// ȡ����ǿ������
	const SpiritEnhance_Config *pEnhaceCfg = g_pCfgMgr->GetSpiritEnhance(dwSpiritId, pSpirit->wEnhanceLevel);
	if (!pEnhaceCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǿ������[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// ����Ƿ񵽴�ȼ�����
	if (!g_pCfgMgr->GetSpiritEnhance(dwSpiritId, pSpirit->wEnhanceLevel+1))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����ǿ���ȼ��Ѵ�����[spirit_id:%d,enhance_level:%d]", 
			__FUNCTION__, dwSpiritId, pSpirit->wEnhanceLevel);
		return;
	}

	// �������
	if (!m_pPlayer->CheckItemCost(pEnhaceCfg->vCostItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]����ǿ�����߲���,����ǿ��[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// ��������
	for (size_t i = 0; i < pEnhaceCfg->vCostItemList.size(); ++i)
	{
		const Item &tItem = pEnhaceCfg->vCostItemList[i];
		m_pPlayer->GetItemManager()->RemoveItem(tItem.dwId, tItem.dwNum, TRUE, FROM_SPRITE_ENHANCE);
	}

	// ����
	pSpirit->wEnhanceLevel++;

	// ����ͼ������
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

	// 25������ǿ���ȼ���ֻ����ߵ���һֻ��
	m_pPlayer->OnUpdateSevenDayTarget(25, pSpirit->wEnhanceLevel, false);

	// �ɾ��¼�(�������ǿ����X��)
	m_pPlayer->OnUpdateAchievement(26, pSpirit->wEnhanceLevel, false);
}

// ����ϳ�
void CSpriteManager::OnNetCompose(DWORD dwFragmentId)
{
	// ȡ��Ƭ����
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(dwFragmentId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ƭ����[dwFragmentId:%d]", __FUNCTION__, dwFragmentId);
		return;
	}

	DWORD dwSpriteId = pItemCfg->nParam1;
	DWORD dwCostFragmentNum = pItemCfg->nParam2;

	// �Ƿ���ӵ�д˾���
	if (HaveSpirit(dwSpriteId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ�ӵ�д˾���[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}
	
	// ���ϳ�����
	if (m_pPlayer->GetItemNum(dwFragmentId) < dwCostFragmentNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Ƭ����,���ܺϳ�[FragmentId:%d,CostFragmentNum:%d]", 
			__FUNCTION__, dwFragmentId, dwCostFragmentNum);
		return;
	}

	m_pPlayer->RemoveItem(dwFragmentId, dwCostFragmentNum, FROM_SPRITE_COMPOSE);

	AddSpirit(dwSpriteId, FROM_SPRITE_COMPOSE);

	// ��Ӧ
	MSG_S2C_SPRITE_COMPOSE_RESP msg;
	msg.m_dwFragmentId = dwFragmentId;
	m_pPlayer->SendPacket(&msg);
}

// ��������
void CSpriteManager::OnNetTrainEx(DWORD dwSpriteId)
{
	WORD wFromMsgId = C2S_SPIRIT_TRAIN_REQ;

	// ����Ƿ��д˾���
	SpiritData *pSpriteData = GetSpirit(dwSpriteId);
	if (!pSpriteData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˾���[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// ȡ��������
	const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(dwSpriteId, pSpriteData->wTrainStage);
	if (!pTrainCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// ����Ƿ����һ��
	if (!pTrainCfg->wNeedTrainTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�����ѵ������һ��[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// ����
	if (pSpriteData->wTrainTimes >= pTrainCfg->wNeedTrainTimes)
	{
		// �����Ҫ��ǿ���ȼ�
		if (pSpriteData->wEnhanceLevel < pTrainCfg->wNeedEnhanceLevel)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]��Ҫ��ǿ���ȼ�����[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		// ������ĵ���Ʒ
		const Item &tCostItem = pTrainCfg->tStageUpCostItem;
		if (m_pPlayer->GetItemNum(tCostItem.dwId) < tCostItem.dwNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�����������ĵ���Ʒ����[spirit_id:%d,item_id:%d]", __FUNCTION__, dwSpriteId, tCostItem.dwId);
			return;
		}

		pSpriteData->wTrainTimes = 0;
		pSpriteData->wTrainStage++;

		// ɾ����Ʒ
		m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);

		// 26�����������׶Σ�ֻ����ߵ���һֻ��
		m_pPlayer->OnUpdateSevenDayTarget(26, pSpriteData->wTrainStage, false);

		// �ɾ��¼�(�������������X��)
		m_pPlayer->OnUpdateAchievement(27, pSpriteData->wTrainStage, false);
	}
	// ����
	else
	{
		// ������ĵ���Ʒ
		if (!m_pPlayer->CheckItemCost(pTrainCfg->vTrainCostItemList))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�����������ĵ���Ʒ����[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		pSpriteData->wTrainTimes++;

		// ɾ�����ĵ���Ʒ
		for (size_t i = 0; i < pTrainCfg->vTrainCostItemList.size(); ++i)
		{
			const Item &tCostItem = pTrainCfg->vTrainCostItemList[i];
			m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);
		}
	}

	SendSpiritData();

	// ��������
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_SPIRIT_TRAIN_RESP msg;
	msg.m_dwSpriteId = dwSpriteId;
	m_pPlayer->SendPacket(&msg);
}

// ��������
void CSpriteManager::OnNetTrain(DWORD dwSpriteId, WORD wTrainTimes)
{
	WORD wFromMsgId = C2S_SPIRIT_TRAIN_REQ;

	// ����Ƿ��д˾���
	SpiritData *pSpriteData = GetSpirit(dwSpriteId);
	if (!pSpriteData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˾���[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	for (WORD i = 0; i < wTrainTimes; ++i)
	{
		// ȡ��������
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(dwSpriteId, pSpriteData->wTrainStage);
		if (!pTrainCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		// ����Ƿ����һ��
		if (!pTrainCfg->wNeedTrainTimes)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�����ѵ������һ��[spirit_id:%d]", __FUNCTION__, dwSpriteId);
			return;
		}

		// ����(����)
		if (pSpriteData->wTrainTimes >= pTrainCfg->wNeedTrainTimes)
		{
			// �����Ҫ��ǿ���ȼ�
			if (pSpriteData->wEnhanceLevel < pTrainCfg->wNeedEnhanceLevel)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]��Ҫ��ǿ���ȼ�����[spirit_id:%d]", __FUNCTION__, dwSpriteId);
				return;
			}

			// ������ĵ���Ʒ
			const Item &tCostItem = pTrainCfg->tStageUpCostItem;
			if (m_pPlayer->GetItemNum(tCostItem.dwId) < tCostItem.dwNum)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]�����������ĵ���Ʒ����[spirit_id:%d,item_id:%d]", __FUNCTION__, dwSpriteId, tCostItem.dwId);
				return;
			}

			pSpriteData->wTrainTimes = 0;
			pSpriteData->wTrainStage++;

			// ɾ����Ʒ
			m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);

			// 26�����������׶Σ�ֻ����ߵ���һֻ��
			m_pPlayer->OnUpdateSevenDayTarget(26, pSpriteData->wTrainStage, false);

			// �ɾ��¼�(�������������X��)
			m_pPlayer->OnUpdateAchievement(27, pSpriteData->wTrainStage, false);
		}
		// ����
		else
		{
			// ������ĵ���Ʒ
			if (!m_pPlayer->CheckItemCost(pTrainCfg->vTrainCostItemList))
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]�����������ĵ���Ʒ����[spirit_id:%d]", __FUNCTION__, dwSpriteId);
				return;
			}

			pSpriteData->wTrainTimes++;

			// ɾ�����ĵ���Ʒ
			for (size_t k = 0; k < pTrainCfg->vTrainCostItemList.size(); ++k)
			{
				const Item &tCostItem = pTrainCfg->vTrainCostItemList[k];
				m_pPlayer->GetItemManager()->RemoveItem(tCostItem.dwId, tCostItem.dwNum, TRUE, FROM_SPRITE_TRAIN);
			}
		}
	}

	SendSpiritData();

	// ��������
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_SPIRIT_TRAIN_RESP msg;
	msg.m_dwSpriteId = dwSpriteId;
	msg.m_wTrainTimes = wTrainTimes;
	m_pPlayer->SendPacket(&msg);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 34, wTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 34, wTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 34, wTrainTimes, true);
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 34, wTrainTimes, true);
}

// ��������
void CSpriteManager::OnNetStarUp(DWORD dwSpriteId)
{
	// ����Ƿ��д˾���
	SpiritData *pSpriteData = GetSpirit(dwSpriteId);
	if (!pSpriteData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˾���[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// ȡ��������
	const Spirit_Config *pSpriteCfg = g_pCfgMgr->GetSpirit(dwSpriteId);
	if (!pSpriteCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[spirit_id:%d]", __FUNCTION__, dwSpriteId);
		return;
	}

	// ����Ƿ񵽴��Ǽ�����
	if (pSpriteData->wStarLevel >= pSpriteCfg->wStarMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�����Ǽ��Ѵ�����[SpriteId:%d,StarMax:%d]", __FUNCTION__, dwSpriteId, pSpriteCfg->wStarMax);
		return;
	}

	// ȡ��������
	const SpriteStarUp_Config *pStarUpCfg = g_pCfgMgr->GetSpriteStarUp(pSpriteData->wStarLevel);
	if (!pStarUpCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[Star:%d]", __FUNCTION__, pSpriteData->wStarLevel);
		return;
	}

	// �������
	if (m_pPlayer->GetItemNum(pSpriteCfg->dwTtarUpCostItemId) < pStarUpCfg->dwStarUpCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�����������ĵ���Ʒ����[SpriteId:%d,Star:%d]", __FUNCTION__, dwSpriteId, pSpriteData->wStarLevel);
		return;
	}

	// ɾ��������Ʒ
	m_pPlayer->RemoveItem(pSpriteCfg->dwTtarUpCostItemId, pStarUpCfg->dwStarUpCostItemNum, FROM_SPRITE_STAR_UP);

	// ����
	pSpriteData->wStarLevel++;

	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_SPRITE_STAR_UP_RESP msg;
	msg.m_dwSpriteId = dwSpriteId;
	m_pPlayer->SendPacket(&msg);
}

// ����ף��
void CSpriteManager::OnNetBless(WORD wBlessType, WORD wFormationPos, DWORD dwSpiritId)
{
	MAPLOG(GUI_LEVEL, "[%s]����ף��[BlessType:%d,FormationPos:%d,SpriteId:%d]", 
		__FUNCTION__, wBlessType, wFormationPos, dwSpiritId);

	// ������ľ��鲻��ף��
	if (dwSpiritId && GetToBattleSpiritId() == dwSpiritId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������ľ��鲻��ף��[SpriteId:%d]", __FUNCTION__,dwSpiritId);
		return;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	DWORD dwOldSpriteId = pHeroMgr->GetBlessSpriteId(wFormationPos);

	int nNewSpriteFormationPos = pHeroMgr->GetBlessSpriteFormactionPos(dwSpiritId);

	//WORD wBlessType = GetBlessType(dwOldSpriteId, dwSpiritId);

	// ף��
// 	if (wBlessType == 1)
// 	{
// 		// ����λ���Ƿ�����ף��
// 		if (dwOldSpriteId)
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
// 			MAPLOG(ERROR_LEVEL, "[%s]��λ��������ף��[RoleId:%d,Level:%d,CurNum:%d]", __FUNCTION__,
// 				m_pPlayer->GetRoleId(), m_pPlayer->GetLevel(), GetBlessSpriteNum());
// 			return;
// 		}
// 
// 		// ����ף������
// 		if (GetBlessSpriteNum() >= GetCurMaxBlessSpriteNum())
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
// 			MAPLOG(ERROR_LEVEL, "[%s]��ǰ����ף�������Ѵ�����[RoleId:%d,Level:%d,CurNum:%d]", __FUNCTION__, 
// 				m_pPlayer->GetRoleId(), m_pPlayer->GetLevel(), GetBlessSpriteNum());
// 			return;
// 		}
// 	}
// 	// �滻
// 	else if (wBlessType == 2)
// 	{
// 		// ����λ���Ƿ�����ף��
//  		if (!dwOldSpriteId)
//  		{
//  			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
//  			MAPLOG(ERROR_LEVEL, "[%s]��λ����û��ף��,�����滻[wFormationPos:%d]", __FUNCTION__,wFormationPos);
//  			return;
//  		}
// 	}
// 	// ȡ��ף��
// 	else
// 	{
// 		// ����λ���Ƿ�����ף��
// 		if (dwSpiritId)
// 		{
// 			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
// 			MAPLOG(ERROR_LEVEL, "[%s]ȡ��ף����Ŀ��ҪΪ��0[OldSpriteId:%d]", __FUNCTION__, dwSpiritId);
// 			return;
// 		}
// 	}

	// ����µľ����Ƿ�֮ǰ��ף�����Ӣ��, �����ÿ�
	if (nNewSpriteFormationPos >= 0)
		pHeroMgr->SetSpriteBless(nNewSpriteFormationPos, 0);

	// ���þ���
	pHeroMgr->SetSpriteBless(wFormationPos, dwSpiritId);

	// ����ף������
	if (GetBlessSpriteNum() > GetCurMaxBlessSpriteNum())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ����ף�������Ѵ�����[RoleId:%d, Level:%d, CurBlessNum:%d, BlessNumMax:%d]", __FUNCTION__,
			m_pPlayer->GetRoleId(), m_pPlayer->GetLevel(), GetBlessSpriteNum(), GetCurMaxBlessSpriteNum());

		// ��������
		pHeroMgr->SetSpriteBless(wFormationPos, 0);
		m_pPlayer->GetHeroManager()->SendFormationList();
		return;
	}

	m_pPlayer->GetHeroManager()->SendFormationList();

	// ��������
	m_pPlayer->UpdateFightingPower();
	
	// ��Ӧ
	MSG_S2C_SPRITE_BLESS_RESP msg;
	msg.m_wBlessType = wBlessType;
	msg.m_wFormationPos = wFormationPos;
	msg.m_dwSpriteId = dwSpiritId;
	m_pPlayer->SendPacket(&msg);
}

// ʹ�þ���
void CSpriteManager::OnNetSpiritUse(DWORD dwSpiritId)
{
	WORD wFromMsgId = C2S_SPIRIT_USE_REQ;

	// �Ƿ��ѻ�ʤ�˾���
	if (!HaveSpirit(dwSpiritId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ľ�л�ʤ�˾���,����ʹ��[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	// ��ף���ľ��鲻������
	if (m_pPlayer->GetHeroManager()->IsBlessSprite(dwSpiritId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ף���ľ��鲻������[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return;
	}

	m_pSpriteInfo->dwCurUseSpiritId = dwSpiritId;

	m_pPlayer->UpdateFightingPower();

	MSG_S2C_SPIRIT_USE_RESP msg;
	msg.m_dwSpiritId = dwSpiritId;
	m_pPlayer->SendPacket(&msg);
}

// �Ƿ��д˾���
bool CSpriteManager::HaveSpirit(DWORD dwSpiritId)
{
	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		if (m_pSpriteInfo->arSpiritList[i].dwId == dwSpiritId)
			return true;
	}
	return false;
}

// ��Ӿ���
bool CSpriteManager::AddSpirit(DWORD dwSpiritId, WORD wFromId)
{
	const Spirit_Config *pSpiritCfg = g_pCfgMgr->GetSpirit(dwSpiritId);
	if (!pSpiritCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return false;
	}

	// ������о��飬������Ƭ
	if (HaveSpirit(dwSpiritId))
	{
		m_pPlayer->AddObject(pSpiritCfg->dwTtarUpCostItemId, pSpiritCfg->dwFragmentNum, TRUE, wFromId);
		//MAPLOG(ERROR_LEVEL, "[%s]�ظ���Ӿ���[spirit_id:%d]", __FUNCTION__, dwSpiritId);
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
		MAPLOG(ERROR_LEVEL, "[%s]�ظ�����ʧ��[spirit_id:%d]", __FUNCTION__, dwSpiritId);
		return false;
	}

	// ��¼���
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwSpiritId, 1, true, wFromId);

	// �Ƿ񼤻�ͼ��
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

// ��Ӿ���
bool CSpriteManager::AddSpirit(const SpiritData &tSpriteData)
{
	if (m_pSpriteInfo->wSpiritNum >= MAX_SPIRIT_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����б�����,���ʧ��!", __FUNCTION__);
		return false;
	}

	m_pSpriteInfo->arSpiritList[m_pSpriteInfo->wSpiritNum++] = tSpriteData;
	return true;
}

// ���;�������
void CSpriteManager::SendSpiritData()
{
	// todo::Ϊ�˼��ݾ�����
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

// ȡͼ��ǿ���ȼ�
WORD CSpriteManager::GetEnhanceLevel(WORD wPokedexId)
{
	const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(wPokedexId, 1);
 	if (!pPokedexCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ͼ��[pokedex_id:%d]", __FUNCTION__, wPokedexId);
 		return 0;
 	}
 
 	// ȡǿ���ȼ�
	WORD wEnhanceLevel = 0;
 	for (size_t i = 0; i < pPokedexCfg->vNeedSpiritList.size(); ++i)
 	{
 		SpiritData *pSpirit = GetSpirit(pPokedexCfg->vNeedSpiritList[i]);
 		if (!pSpirit)
 		{
 			wEnhanceLevel = 0;
 			break;
 		}

		// Ĭ��ȡ��һ����ֵ
		if (i == 0)
			wEnhanceLevel = pSpirit->wEnhanceLevel;
		else
			wEnhanceLevel = min(pSpirit->wEnhanceLevel, wEnhanceLevel);	
 	} 
	return wEnhanceLevel;
}

// �Ƿ�ͼ���Ѽ���
bool CSpriteManager::IsPokedexActivate(WORD wPokedexId)
{
	const SpiritPokedex_Config *pPokedexCfg = g_pCfgMgr->GetSpiritPokedex(wPokedexId, 1);
	if (!pPokedexCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ͼ��[pokedex_id:%d]", __FUNCTION__, wPokedexId);
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

// ���ͼ��
void CSpriteManager::AddPokedex(WORD wPokedexId)
{
	if (m_pSpriteInfo->wPokedexNum >= MAX_SPIRIT_POKEDEX_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ͼ���б�����,���ʧ��[pokedex_id:%d]", __FUNCTION__, wPokedexId);
		return;
	}

	SpiritPokedex pokedex;
	pokedex.wPokedexId = wPokedexId;
	pokedex.wLevel = 1;
	m_pSpriteInfo->arPokedexList[m_pSpriteInfo->wPokedexNum++] = pokedex;
}

// ��������֮������
void CSpriteManager::SendHeartOfWorldData()
{
	MSG_S2C_HEART_OF_WORLD_DATA_RESP msg;
	msg.m_byStage = m_pSpriteInfo->tHeartOfWorld.wStage;
	msg.m_byLevel = m_pSpriteInfo->tHeartOfWorld.wLevel;
	m_pPlayer->SendPacket(&msg);
}

// ȡ��ս����ID
DWORD CSpriteManager::GetToBattleSpiritId()
{
	return m_pSpriteInfo->dwCurUseSpiritId;
}

// ȡ����ǿ���ȼ�
WORD CSpriteManager::GetSpiritEnhanceLevel(DWORD dwSpiritId)
{
	SpiritData *pSpirit = GetSpirit(dwSpiritId);
	return pSpirit ? pSpirit->wEnhanceLevel : 0;
}

// ȡ������������
WORD CSpriteManager::GetSpiritTrainTimes(DWORD dwSpiritId)
{
	SpiritData *pSpirit = GetSpirit(dwSpiritId);
	return pSpirit ? pSpirit->wTrainTimes : 0;
}

// ȡ��������
const SpiritData* CSpriteManager::GetSpiritEx(DWORD dwSpiritId)
{
	return GetSpirit(dwSpiritId);
}

// ȡף����������
WORD CSpriteManager::GetBlessSpriteNum()
{
	return m_pPlayer->GetHeroManager()->GetBlessHeroNum();
}

// ȡ��ǰף��������������
WORD CSpriteManager::GetCurMaxBlessSpriteNum()
{
	const RoleLevel_Config *pRoleLevelCfg = g_pCfgMgr->GetRoleLevel(m_pPlayer->GetLevel());
	if (!pRoleLevelCfg)
	{
		MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ�����ɫ�ȼ�����[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return 0;
	}

	return pRoleLevelCfg->dwBlessSpriteNumMax;
}

// ȡף��Ӣ���б�
void CSpriteManager::GetBlessSpriteList(vector<SpiritData> &vSpriteList)
{
	vSpriteList.clear();
	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		vSpriteList.push_back(m_pSpriteInfo->arSpiritList[i]);
	}
}

// ȡף������
WORD CSpriteManager::GetBlessType(DWORD dwOldSpriteId, DWORD dwSpiritId)
{
	WORD wType = 0;

	// ף��, �滻
	if (dwSpiritId)
	{
		// �滻
		if (dwOldSpriteId)
		{
			wType = 2;
		}
		// ף��
		else
		{
			wType = 1;
		}
	}
	// ȡ��ף��
	else
	{
		wType = 3;
	}

	return wType;
}

// ȡ���⽱���ĵ�����Ʒ��Ϣ
void CSpriteManager::GetExtraRewardDropItemInfo(WORD wFBType, map<DWORD, int> &mItemInfoList)
{
	mItemInfoList.clear();

	// ľ�о���
	if (!m_pSpriteInfo->wSpiritNum)
		return;

	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		SpiritData &tSpriteData = m_pSpriteInfo->arSpiritList[i];

		// ȡ��������
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(tSpriteData.dwId, tSpriteData.wTrainStage);
		if (!pTrainCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������������[SpriteId:%d, TrainStage:%d]", 
				__FUNCTION__, tSpriteData.dwId, tSpriteData.wTrainStage);
			continue;
		}

		// ȡ�츳��������
		const SpriteTalentSkill_Config *pTalentSkillCfg = g_pCfgMgr->GetSpriteTalentSkill(tSpriteData.dwId, pTrainCfg->wTalentLevel);
		if (!pTalentSkillCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������츳������������[SpriteId:%d,TalentLevel:%d]", 
				__FUNCTION__, tSpriteData.dwId, pTrainCfg->wTalentLevel);
			continue;
		}

		// ���Ƕ��⽱���ĸ���
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

// ȡ���⽱���Ļ�����Ϣ
void CSpriteManager::GetExtraRewardCurrencyInfo(WORD wFBType, map<int, int> &mCurrencyInfoList)
{
	mCurrencyInfoList.clear();

	// ľ�о���
	if (!m_pSpriteInfo->wSpiritNum)
		return;

	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		SpiritData &tSpriteData = m_pSpriteInfo->arSpiritList[i];

		// ȡ��������
		const SpriteTrain_Config *pTrainCfg = g_pCfgMgr->GetSpriteTrain(tSpriteData.dwId, tSpriteData.wTrainStage);
		if (!pTrainCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������������[SpriteId:%d, TrainStage:%d]",
				__FUNCTION__, tSpriteData.dwId, tSpriteData.wTrainStage);
			continue;
		}

		// ȡ�츳��������
		const SpriteTalentSkill_Config *pTalentSkillCfg = g_pCfgMgr->GetSpriteTalentSkill(tSpriteData.dwId, pTrainCfg->wTalentLevel);
		if (!pTalentSkillCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������츳������������[SpriteId:%d,TalentLevel:%d]",
				__FUNCTION__, tSpriteData.dwId, pTrainCfg->wTalentLevel);
			continue;
		}

		// ���Ƕ��⽱���ĸ���
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

// ȡ���⽱���Ļ���ϵ��
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


// ȡ��������
SpiritData* CSpriteManager::GetSpirit(DWORD dwSpiritId)
{
	for (WORD i = 0; i < m_pSpriteInfo->wSpiritNum; ++i)
	{
		if (m_pSpriteInfo->arSpiritList[i].dwId == dwSpiritId)
			return &m_pSpriteInfo->arSpiritList[i];
	}
	return NULL;
}

// ȡ����ͼ��
SpiritPokedex* CSpriteManager::GetPokedex(WORD wPokedexId)
{
	for (WORD i = 0; i < m_pSpriteInfo->wPokedexNum; ++i)
	{
		if (m_pSpriteInfo->arPokedexList[i].wPokedexId == wPokedexId)
			return &m_pSpriteInfo->arPokedexList[i];
	}
	return NULL;
}