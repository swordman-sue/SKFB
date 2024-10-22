#include "stdafx.h"
#include "TreasureManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"


CTreasureManager::CTreasureManager()
{
	m_dwSeriesId = 1;
}

CTreasureManager::~CTreasureManager()
{
		
}

void CTreasureManager::Init(Player *pPlayer)
{
	m_dwSeriesId = 1;
	m_pPlayer = pPlayer;
	m_pTreasureInfo = &m_pPlayer->GetRoleFullInfo().tTreasureInfo;
}

// ����ǿ�� 
void CTreasureManager::OnEnhance(DWORD dwTreasureUID, DWORD *pCostTreasureList, WORD wCostTreasureNum)
{
	WORD wFromMsgId = C2S_TREASURE_STRENGTHEN_REQ;

	if (!wCostTreasureNum || !pCostTreasureList)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]����ǿ�������Ƿ�[UID:%d][CostTreasureNum:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID, wCostTreasureNum);
		return;
	}

	// ���������Ʒ
	for (WORD i = 0; i < wCostTreasureNum; ++i)
 	{
		for (WORD k = i+1; k < wCostTreasureNum; ++k)
		{
			if (pCostTreasureList[i] == pCostTreasureList[k])
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[name:%s]�����ظ���������Ʒ[UID:%d]",
					m_pPlayer->GetUTF8RoleName(), dwTreasureUID);
				return;
			}
		}
 	}

	// ����Ƿ��д�װ��
	Treasure *pTreasure = FindTreasure(dwTreasureUID);
	if (!pTreasure)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]�Ҳ�����ҵı�������[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID);
		return;
	}

	//MAPLOG(ERROR_LEVEL, "����ǿ���ı���[UID:%d, InfoId:%d]", dwTreasureUID, pTreasure->dwInfoId);

	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[InfoId:%d]",__FUNCTION__, dwTreasureUID);
		return;
	}

	// ����Ƿ�������
	if (pTreasure->wEnhanceLevel >= pTreasureCfg->wStrengthenLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]����ǿ���ȼ��Ѵ�����[InfoId:%d,MaxLevel:%d]",
			__FUNCTION__, dwTreasureUID, pTreasure->wEnhanceLevel);
		return;
	}

	WORD wOldLevel = pTreasure->wEnhanceLevel;

	// ���������Ʒ
	int nCostGold = 0;
	//WORD wRemoveTreasureNum = 0;
	for (WORD i = 0; i < wCostTreasureNum; ++i)
	{
		if (!DoStrengthen(pTreasure, pTreasureCfg, pCostTreasureList[i], nCostGold))
			break;

		//wRemoveTreasureNum++;
	}

	TreasureUpdateNotify(*pTreasure);

	WORD wNewLevel = pTreasure->wEnhanceLevel;

	// ��������
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold, TRUE, FROM_TREASURE_ENHANCE);
	for (WORD i = 0; i < wCostTreasureNum; ++i)
	{
		DWORD dwCostTreasureId = pCostTreasureList[i];
		Treasure *pCostTreasure = FindTreasure(dwCostTreasureId);
		if (pCostTreasure)
			MAPLOG(GUI_LEVEL, "����ǿ�����ĵı���[UID:%d, InfoId:%d]", pCostTreasure->dwId, pCostTreasure->dwInfoId);
		else
			MAPLOG(ERROR_LEVEL, "[%s]�������ĵ��ﲻ����[UID:%d]", __FUNCTION__, dwCostTreasureId);

		RemoveTreasure(dwCostTreasureId, FROM_TREASURE_ENHANCE);
	}

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_TREASURE_STRENGTHEN_RESP msg;
	msg.m_dwTreasureUID = dwTreasureUID;
	msg.m_wOldLevel = wOldLevel;
	msg.m_wNewLevel = wNewLevel;
	msg.m_dwExp = nCostGold;
	m_pPlayer->SendPacket(&msg);

	// �ճ������¼�(todo::����Ϊ����,���Ǹ���)
	m_pPlayer->OnUpdateDailyMission(2);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(19, GetMinEnhanceLevelInWear(), false);

	// ����ȼ�����
	//m_pPlayer->OnUpdateWorldTaskEvent(2);
}

// ���ﾫ��
void CTreasureManager::OnRefine(DWORD dwTreasureUID)
{
	WORD wFromMsgId = C2S_TREASURE_REFINE_REQ;

	// ����Ƿ��д�װ��
	Treasure *pTreasure = FindTreasure(dwTreasureUID);
	if (!pTreasure)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]�Ҳ�����ҵı�������[UID:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID);
		return;
	}

	// ȡ��������
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "�Ҳ����������������[InfoId:%d]", pTreasure->dwInfoId);
		return;
	}

	// ��⾫���ȼ��Ƿ񵽴�����
	if (pTreasure->wRefineLevel >= pTreasureCfg->wRefineLevelMax)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]���ﾫ���ȼ��Ѵ�����![UID:%d,RefineLv:%d]",
			m_pPlayer->GetUTF8RoleName(), dwTreasureUID, pTreasure->wRefineLevel);
		return;
	}

	// ȡ���ﾫ������
	const TreasureRefine_Config *pRefineCfg = 
		CConfigManager::Instance()->GetTreasureRefine(pTreasureCfg->wQuality, pTreasure->wRefineLevel);
	if (!pRefineCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "�Ҳ������ﾫ������������[Quality:%d,level:%d]", 
			pTreasureCfg->wQuality, pTreasure->wRefineLevel);
		return;
	}

	// �����Ʒ
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);
	if (pItemMgr->GetItemNum(pRefineCfg->dwCostItemId) < pRefineCfg->wCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[name:%s]���߲���,����ʧ��[CostItemId:%d, CostItemNum:%d]!", 
			m_pPlayer->GetUTF8RoleName(), pRefineCfg->dwCostItemId, pRefineCfg->wCostItemNum);
		return;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	// ��ⱦ��
	DWORD arCostTreasureList[MAX_TREASURE_NUM] = { 0 };
	BYTE byCostTreasureNum = 0;
	if (pRefineCfg->byCostTreasureNum)
	{
		for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
		{
			Treasure &tmp = m_pTreasureInfo->sTreasureList[i];
			// ǿ���������ȼ���û��������û�����, �����Ϊ����Ʒ
			if (dwTreasureUID != tmp.dwId && tmp.dwInfoId == pTreasure->dwInfoId && 
				tmp.wEnhanceLevel <= 1 && !tmp.wRefineLevel && !pHeroMgr->GetTreasureWearHeroId(tmp.dwId))
			{
				arCostTreasureList[byCostTreasureNum++] = tmp.dwId;
			}

			// �������㹻
			if (byCostTreasureNum >= pRefineCfg->byCostTreasureNum)
				break;
		}
	}

	// ������ĵı���
	if (byCostTreasureNum < pRefineCfg->byCostTreasureNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "���ﾫ�����ĵı�����������[InfoId:%d,CurNum:%d]", pTreasure->dwInfoId, byCostTreasureNum);
		return;
	}

	// �����
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < pRefineCfg->nCostGold)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "���ﾫ�����ĵĽ�Ҳ���[InfoId:%d]", pTreasure->dwInfoId);
		return;
	}

	// ����
	pTreasure->wRefineLevel++;

	TreasureUpdateNotify(*pTreasure);

	WORD wNewRefineLevel = pTreasure->wRefineLevel;

	// �۳���Ʒ,����,���
	pItemMgr->RemoveItem(pRefineCfg->dwCostItemId, pRefineCfg->wCostItemNum, TRUE, FROM_TREASURE_REFINE);
	for (BYTE i = 0; i < pRefineCfg->byCostTreasureNum; ++i)
	{
		RemoveTreasure(arCostTreasureList[i], FROM_TREASURE_REFINE);
	}
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -pRefineCfg->nCostGold, TRUE, FROM_TREASURE_REFINE);

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	MSG_S2C_TREASURE_REFINE_RESP msg;
	msg.m_dwTreasureUID = dwTreasureUID;
	msg.m_byRefineLevel = wNewRefineLevel;
	m_pPlayer->SendPacket(&msg);
}

// ����ϳ�
void CTreasureManager::OnComposeEx(DWORD dwTreasureInfoId)
{
	WORD wFromMsgId = C2S_TREASURE_COMPOSE_REQ;

	// ȡ�ϳɱ�������
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(dwTreasureInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ϳ�����[InfoId:%d]", __FUNCTION__, dwTreasureInfoId);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// �����Ҫ����Ʒ
	vector<DWORD>::const_iterator iter = pComposeCfg->cost_item_list.begin();
	for (; iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		// ���߲���
		if (!pItemMgr->GetItem(*iter))
		{
			m_pPlayer->SendErrorMsg(ERROR_DOUBAO_COMPOSE_ITEM_NOT_ENOUGH);
			MAPLOG(ERROR_LEVEL, "[%s]����ϳ���Ҫ�ĵ��߲���[InfoId:%d]", __FUNCTION__, dwTreasureInfoId);
			return;
		}
	}

	// �������ռ�
	if (!GetBagEmptySize())
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��������ռ䲻��", __FUNCTION__);
		return;
	}

	// ��ӱ���
	AddTreasure(dwTreasureInfoId, TRUE, FROM_TREASURE_COMPOSE);

	// ��������
	for (iter = pComposeCfg->cost_item_list.begin(); iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		pItemMgr->RemoveItem(*iter, 1, TRUE, FROM_TREASURE_COMPOSE);
	}

// 	MSG_S2C_TREASURE_COMPOSE_RESP msg;
// 	msg.m_wInfoId = dwTreasureInfoId;
// 	m_pPlayer->SendPacket(&msg);

	// �ճ�����(ʥ���ϳ�)
	m_pPlayer->OnUpdateDailyMission(22);
}

// ����ϳ�
void CTreasureManager::OnCompose(DWORD dwTreasureInfoId, WORD wTreasureNum)
{
	WORD wFromMsgId = C2S_TREASURE_COMPOSE_REQ;

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	// ȡ�ϳɱ�������
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(dwTreasureInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ϳ�����[InfoId:%d]", __FUNCTION__, dwTreasureInfoId);
		return;
	}

	// �����Ҫ����Ʒ
	vector<DWORD>::const_iterator iter = pComposeCfg->cost_item_list.begin();
	for (; iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		// ���߲���
		DWORD dwCostItemId = *iter;
		if (pItemMgr->GetItemNum(dwCostItemId) < wTreasureNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_DOUBAO_COMPOSE_ITEM_NOT_ENOUGH);
			MAPLOG(ERROR_LEVEL, "[%s]����ϳ���Ҫ�ĵ��߲���[InfoId:%d,CostItemId:%d]",
				__FUNCTION__, dwTreasureInfoId, dwCostItemId);
			return;
		}
	}

		// �������ռ�
	if (GetBagEmptySize() < wTreasureNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��������ռ䲻��", __FUNCTION__);
		return;
	}

	// ��ӱ���
	for (WORD i = 0; i < wTreasureNum; ++i)
	{
		AddTreasure(dwTreasureInfoId, TRUE, FROM_TREASURE_COMPOSE);
	}

	// ��������
	for (iter = pComposeCfg->cost_item_list.begin(); iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		pItemMgr->RemoveItem(*iter, wTreasureNum, TRUE, FROM_TREASURE_COMPOSE);
	}

	MSG_S2C_TREASURE_COMPOSE_RESP msg;
	msg.m_dwTreasureInfoId = dwTreasureInfoId;
	msg.m_wTreasureNum = wTreasureNum;
	m_pPlayer->SendPacket(&msg);

	// �ճ�����(ʥ���ϳ�)
	m_pPlayer->OnUpdateDailyMission(22);
}

// ���ﴩ��
void CTreasureManager::OnPutOn(BYTE byFormationPos, BYTE byTreasurePos, DWORD dwTreasureUID)
{
	WORD wFromMsgId = C2S_TREASURE_PUT_ON_REQ;

	// ������(���Ͳ�λ, �����λ)
	if (!byFormationPos || byFormationPos > MAX_BATTLE_HERO_NUM || byTreasurePos >= TREASURE_TYPE_MAX)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�����ﴩ����������[formation_pos:%d, treasure_pos:%d]",
			__FUNCTION__, byFormationPos, byTreasurePos);
		return;
	}

	BYTE bySvrFormationPos = byFormationPos - 1;
	BYTE bySvrTreasurePos = byTreasurePos;

	// ����Ƿ��д�װ��
	if (dwTreasureUID)
	{
		const Treasure *pTreasure = GetTreasure(dwTreasureUID);
		if (!pTreasure)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]���û�д˱���[UID:%d]", __FUNCTION__, dwTreasureUID);
			return;
		}
	}

	// todo::����װ���Ƿ���Դ�������λ��

	Formation &sFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[bySvrFormationPos];
	DWORD dwOldTreasureUID = sFormation.arTreasureList[bySvrTreasurePos];
	sFormation.arTreasureList[bySvrTreasurePos] = dwTreasureUID;

	if (dwOldTreasureUID)
		TreasureUpdateNotify(dwOldTreasureUID);
	TreasureUpdateNotify(dwTreasureUID);
 
 	// ����ͬ�������б�
 	m_pPlayer->GetHeroManager()->SendFormationList();

	// ����ս��
	m_pPlayer->UpdateFightingPower();
 
	MSG_S2C_TREASURE_PUT_ON_RESP msg;
	msg.m_byFormationPos = byFormationPos;
	msg.m_byTreasurePos = byTreasurePos;
	msg.m_dwUID = dwTreasureUID;
	m_pPlayer->SendPacket(&msg);

	// �ɾ�
	m_pPlayer->OnUpdateAchievement(20, GetLowQualityInWear(), false);
}

// ����
void CTreasureManager::OnNetSmelt(DWORD dwTreasureInfoId)
{
	WORD wFromMsgId = C2S_TREASURE_SMELT_REQ;

	// ȡ��������
	const TreasureSmelt_Config *pSmeltCfg = g_pCfgMgr->GetTreasureSmelt(dwTreasureInfoId);
	if (!pSmeltCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "�Ҳ�������������������[InfoId:%d]", dwTreasureInfoId);
		return;
	}

	// �����ʯ����
	int nCostDiamond = g_GetConfigValue(103);
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "��������������ʯ����[InfoId:%d]", dwTreasureInfoId);
		return;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	// ����Ƿ���Ҫ���ĵı���
	int nIndex = -1;
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure *pTreasure = &m_pTreasureInfo->sTreasureList[i];
		// 1.δǿ���� 2.δ������ 3.Ʒ��Ϊ��ɫ�����ñ�����߻�ֱ�����������ID���ɣ� 4.��û������
		if (pTreasure->dwInfoId == pSmeltCfg->dwCostTreasureInfoId && 
			1 == pTreasure->wEnhanceLevel && !pTreasure->wRefineLevel && !pHeroMgr->GetTreasureWearHeroId(pTreasure->dwId))
		{
			nIndex = i;
			break;
		}
	}
	if (-1 == nIndex)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "�����������ĵı��ﲻ��[CostTreasureId:%d]", pSmeltCfg->dwCostTreasureInfoId);
		return;
	}

	// ɾ������
	m_pTreasureInfo->sTreasureList[nIndex] = m_pTreasureInfo->sTreasureList[--m_pTreasureInfo->wTreasureNum];

	// ������ʯ
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TREASURE_SMELT);

	// ���������Ʒ
	m_pPlayer->AddObject(pSmeltCfg->dwItemId, 1, TRUE, FROM_TREASURE_SMELT);

	// ��Ӧ
	MSG_S2C_TREASURE_SMELT_RESP msg;
	msg.m_dwTreasureInfoId = dwTreasureInfoId;
	m_pPlayer->SendPacket(&msg);
}

// ��Ʒ
void CTreasureManager::OnNetQualityUP(DWORD dwTreasureUID)
{
	// ����Ƿ��д˱���
	Treasure *pTreasure = FindTreasure(dwTreasureUID);
	if (!pTreasure)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵı�������[UID:%d]",__FUNCTION__, dwTreasureUID);
		return;
	}

	// ȡ��������
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������������[InfoId:%d]",__FUNCTION__, pTreasure->dwInfoId);
		return;
	}

	// ����Ƿ������Ʒ
	if (!pTreasureCfg->dwNextQualityTreasureId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�˱��ﲻ����Ʒ[InfoId:%d,Quality:%d]", __FUNCTION__, pTreasure->dwInfoId, pTreasureCfg->wQuality);
		return;
	}

	// ȡ��Ʒ��������
	const TreasureQualityUPCost_Config *pQualityUPCostCfg = g_pCfgMgr->GetTreasureQualityUPCost(pTreasure->wRefineLevel);
	if (!pQualityUPCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ʒ��������[RefineLevel:%d]", __FUNCTION__, pTreasure->wRefineLevel);
		return;
	}

	// �����������(����)
	if (m_pPlayer->GetItemNum(pTreasureCfg->dwQualityUPCostItemId) < pQualityUPCostCfg->dwCostItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ���ĵ��߲���[RefineLevel:%d,CostItemId:%d,CostItemNum:%d]", 
			__FUNCTION__, pTreasure->wRefineLevel, pTreasureCfg->dwQualityUPCostItemId, pQualityUPCostCfg->dwCostItemNum);
		return;
	}

	// �����������(����)
	if (!m_pPlayer->CheckMoneyCost(pQualityUPCostCfg->vCostCurrencyList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ���Ļ��Ҳ���[RefineLevel:%d]",__FUNCTION__, pTreasure->wRefineLevel);
		return;
	}

	// ǿ������
	const TreasureEnhance_Config *pTreasureStrengthenCfg =
		g_pCfgMgr->GetTreasureStrengthen(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
	if (!pTreasureStrengthenCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǿ������[Quality:%d,EnhanceLevel:%d]",
			__FUNCTION__, pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		return;
	}

	// ��������(����)
	m_pPlayer->RemoveItem(pTreasureCfg->dwQualityUPCostItemId, pQualityUPCostCfg->dwCostItemNum, FROM_TREASURE_QUALITY_UP);

	// ��������(����)
	m_pPlayer->DoMoneyCost(pQualityUPCostCfg->vCostCurrencyList, 1, FROM_TREASURE_QUALITY_UP);

	// ��Ʒ����(1.�¾����ȼ�=�ɾ����ȼ� 2.ǿ���ȼ����´���)
	pTreasure->dwInfoId = pTreasureCfg->dwNextQualityTreasureId;
	pTreasure->wEnhanceLevel = 1;
	pTreasure->dwEnhanceExp += pTreasureStrengthenCfg->dwTotalExp;

	// ȡ�±�������
	const Treasure_Config *pNewTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������������[InfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return;
	}

	// ��������
	const TreasureEnhance_Config *pStrengthenCfg;
	while (true)
	{
		pStrengthenCfg = g_pCfgMgr->GetTreasureStrengthen(pNewTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		if (!pStrengthenCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǿ������[Quality:%d,StrengthenLevel:%d]",
				__FUNCTION__, pNewTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
			break;
		}

		// ������
		if (pStrengthenCfg->dwNeedExp && pTreasure->dwEnhanceExp >= pStrengthenCfg->dwNeedExp)
		{
			pTreasure->dwEnhanceExp -= pStrengthenCfg->dwNeedExp;
			pTreasure->wEnhanceLevel++;
		}
		else
		{
			break;
		}

		// ����Ƿ�������
		if (pTreasure->wEnhanceLevel >= pNewTreasureCfg->wStrengthenLevelMax)
			break;
	}

	// ����ͬ�����ݸ��ͻ���
	TreasureUpdateNotify(*pTreasure, true);

	// ��Ӧ
	MSG_S2C_TREASURE_QUALITY_UP_RESP msg;
	msg.m_tTreasure = *pTreasure;
	m_pPlayer->SendPacket(&msg);
}

// �������б�
void CTreasureManager::OnTreasureList()
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_TREASURE_LIST_RESP msg;
	msg.m_wTreasureNum = m_pTreasureInfo->wTreasureNum;
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		TreasureBaseData &sDest = msg.m_TreasureList[i];
		Treasure &sSrc = m_pTreasureInfo->sTreasureList[i];
		sDest.dwId = sSrc.dwId;
		sDest.wInfoId = sSrc.dwInfoId;
		sDest.byEnhanceLevel = sSrc.wEnhanceLevel;
		sDest.dwEnhanceExp = sSrc.dwEnhanceExp;
		sDest.byRefineLevel = sSrc.wRefineLevel;
		sDest.wWearHeroInfoId = pHeroMgr->GetTreasureWearHeroId(sSrc.dwId);
	}
	m_pPlayer->SendPacket(&msg);
}

// ���װ��
bool CTreasureManager::AddTreasure(DWORD dwTreasureInfoId, BOOL bNotice, WORD wFromId)
{
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(dwTreasureInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ���������������[InfoId:%d]", dwTreasureInfoId);
		return false;
	}

	// ��¼���
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwTreasureInfoId, 1, true, wFromId);

	// 7���¼�(ʥ���ռ�)
	m_pPlayer->OnUpdateSevenDayTarget(7);

	Treasure tTreasure;
	memset(&tTreasure, 0, sizeof(tTreasure));
	tTreasure.dwId = GetNewSeriesId();
	tTreasure.dwInfoId = dwTreasureInfoId;
	tTreasure.wEnhanceLevel = 1;
	return AddTreasure(tTreasure);
}

// ���װ��
bool CTreasureManager::AddTreasure(const Treasure &sTreasure, BOOL bNotice)
{
	if (!GetBagEmptySize())
	{
		//MAPLOG(ERROR_LEVEL, "[%s]���ﱳ������[num:%d]", __FUNCTION__, m_pTreasureInfo->wTreasureNum);

		MAPLOG(ERROR_LEVEL, "[%s]���ﱳ������[RoleId:%d,ListNum:%d,InfoId:%d]", __FUNCTION__,
			m_pPlayer->GetRoleId(), m_pTreasureInfo->wTreasureNum, sTreasure.dwInfoId);

		// ��¼װ����ʧ��¼
		m_pPlayer->SaveItemLoseRecord(sTreasure.dwInfoId);

		return false;
	}

	m_pTreasureInfo->sTreasureList[m_pTreasureInfo->wTreasureNum++] = sTreasure;

	if (bNotice)
		TreasureUpdateNotify(sTreasure);

	return true;
}

// ɾ������
bool CTreasureManager::RemoveTreasure(DWORD dwUID, WORD wFromId)
{
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure &sDel = m_pTreasureInfo->sTreasureList[i];
		if (sDel.dwId == dwUID)
		{
			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), sDel.dwInfoId, 1, false, wFromId);

			TreasureUpdateNotify(sDel, false);
			sDel = m_pTreasureInfo->sTreasureList[--m_pTreasureInfo->wTreasureNum];
			return true;
		}
	}
	return true;
}

// ɾ������(ֻɾ��û��ǿ����������)
bool CTreasureManager::RemoveTreasureByInfoId(DWORD dwInfoId, WORD wFromId)
{
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure *pDel = &m_pTreasureInfo->sTreasureList[i];
		if (pDel->dwInfoId == dwInfoId && !pDel->wEnhanceLevel && !pDel->wRefineLevel)
		{
			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), pDel->dwInfoId, 1, false, wFromId);

			*pDel = m_pTreasureInfo->sTreasureList[--m_pTreasureInfo->wTreasureNum];
			return true;
		}
	}

	return false;
}

// ȡװ��������������
WORD CTreasureManager::GetBagEmptySize()
{
	WORD wBagSize = min(MAX_TREASURE_NUM, m_pPlayer->GetVipValue(VIP_TREASURE_BAG_NUM));
	return wBagSize > m_pTreasureInfo->wTreasureNum ? wBagSize - m_pTreasureInfo->wTreasureNum : 0;
}

// ȡ����ǿ���ȼ���Ӧ���ܾ���
DWORD CTreasureManager::GetStrengthLevelTotalExp(WORD wQuality, WORD wLevel)
{
	const TreasureEnhance_Config *pStrengthenCf = CConfigManager::Instance()->GetTreasureStrengthen(wQuality, wLevel);
	if (!pStrengthenCf)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǿ���ȼ���Ӧ������[Quality:%d,Level:%d]", __FUNCTION__, wQuality, wLevel);
		return 0;
	}
	return pStrengthenCf->dwTotalExp;
}

// ȡ��������
const Treasure* CTreasureManager::GetTreasure(DWORD dwTreasureUID)
{
	return FindTreasure(dwTreasureUID);
}

// �������֪ͨ
void CTreasureManager::TreasureUpdateNotify(DWORD dwTreasureUID, BYTE byFlag)
{
	const Treasure *pTreasure = GetTreasure(dwTreasureUID);
	if (pTreasure)
	{
		TreasureUpdateNotify(*pTreasure);
	}
}

// �������֪ͨ
void CTreasureManager::TreasureUpdateNotify(const Treasure &sTreasure, BYTE byFlag)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER(pHeroMgr);

	MSG_S2C_TREASURE_INFO_NOTIFY msg;
	msg.m_byFlag = byFlag;
	msg.m_sBaseData.dwId = sTreasure.dwId;
	msg.m_sBaseData.wInfoId = sTreasure.dwInfoId;
	msg.m_sBaseData.byEnhanceLevel = sTreasure.wEnhanceLevel;
	msg.m_sBaseData.byRefineLevel = sTreasure.wRefineLevel;
	msg.m_sBaseData.dwEnhanceExp = sTreasure.dwEnhanceExp;
	msg.m_sBaseData.wWearHeroInfoId = pHeroMgr->GetTreasureWearHeroId(sTreasure.dwId);
	m_pPlayer->SendPacket(&msg);
}

// ����ǿ��
bool CTreasureManager::DoStrengthen(Treasure *pTreasure, const Treasure_Config *pTreasureCfg, DWORD dwCostTreasureUId, int &nCostGold)
{
	// ����Ƿ�������
	if (pTreasure->wEnhanceLevel >= pTreasureCfg->wStrengthenLevelMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ǿ���ȼ��Ѵ�����[uId:%d,MaxLevel:%d]",
			__FUNCTION__, pTreasure->dwId, pTreasure->wEnhanceLevel);
		return false;
	}

	Treasure *pCostTreasure = FindTreasure(dwCostTreasureUId);
	if (!pCostTreasure)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д��ڴ˱���[uid:%d]", __FUNCTION__, dwCostTreasureUId);
		return false;
	}

	// �Ѿ��������Ĳ�����Ϊ����
	if (pCostTreasure->wRefineLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ������ı��ﲻ����Ϊ����Ʒ[InfoId:%d]", __FUNCTION__, pCostTreasure->dwInfoId);
		return false;
	}

	const Treasure_Config *pCostTreasureCfg = g_pCfgMgr->GetTreasure(pCostTreasure->dwInfoId);
	if (!pCostTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[InfoId:%d]", __FUNCTION__, pCostTreasure->dwInfoId);
		return false;
	}

	const TreasureEnhance_Config *pCostTreasureStrengthenCfg = 
		g_pCfgMgr->GetTreasureStrengthen(pCostTreasureCfg->wQuality, pCostTreasure->wEnhanceLevel);
	if (!pCostTreasureStrengthenCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǿ������[Quality:%d,StrengthenLevel:%d]",
			__FUNCTION__, pCostTreasureCfg->wQuality, pCostTreasure->wEnhanceLevel);
		return false;
	}

	DWORD dwTotalExp = pCostTreasureStrengthenCfg->dwTotalExp + pCostTreasure->dwEnhanceExp + pCostTreasureCfg->dwAsExp;

	// Ӣ�������������ɵĿ��ƾ���ĳ�1, ʥ������Ҳ��Ӣ����������ʽһ��
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT && pCostTreasureCfg->byType == 2){
		dwTotalExp = 1;
	}

	// �����
	if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold + dwTotalExp)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ǿ����Ҳ���[InfoId:%d,NeedGold:%d]", __FUNCTION__, nCostGold + dwTotalExp);
		return false;
	}

	pTreasure->dwEnhanceExp += dwTotalExp;

	// ��������
	bool bMaxLevel = false;
	const TreasureEnhance_Config *pStrengthenCfg;
	while (true)
	{
		pStrengthenCfg = g_pCfgMgr->GetTreasureStrengthen(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		if (!pStrengthenCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ǿ������[Quality:%d,StrengthenLevel:%d]",
				__FUNCTION__, pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
			return false;
		}

		if (pStrengthenCfg->dwNeedExp && pTreasure->dwEnhanceExp >= pStrengthenCfg->dwNeedExp)
		{
			pTreasure->dwEnhanceExp -= pStrengthenCfg->dwNeedExp;
			pTreasure->wEnhanceLevel++;
		}
		else
		{
			break;
		}

		// ����Ƿ�������
		if (pTreasure->wEnhanceLevel >= pTreasureCfg->wStrengthenLevelMax)
			break;
	}

	nCostGold += dwTotalExp;
	return true;
}

// ȡ����ָ��Ʒ�ʵı�������
WORD CTreasureManager::GetTreasureNumByQuality(WORD wQuality)
{
	WORD wNum = 0;
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		Treasure *pTreasure = &m_pTreasureInfo->sTreasureList[i];
		const Treasure_Config *pTreasureCfg = g_pCfgMgr->GetTreasure(pTreasure->dwInfoId);

		if (pTreasureCfg && pTreasureCfg->wQuality >= wQuality)
			wNum++;
	}
	return wNum;
}

// ȡӢ�۱�����С�ȼ�
WORD CTreasureManager::GetMinEnhanceLevelInWear()
{
	WORD wMinLevel = 0;
	const Formation *pFormationList = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation &tFormation = pFormationList[i];
		if (!tFormation.dwHeroUID)
			return 0;

		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			DWORD dwTreasureId = tFormation.arTreasureList[k];
			const Treasure *pTreasure = dwTreasureId ? GetTreasure(dwTreasureId) : NULL;
			if (!pTreasure)
				return 0;

			if (!wMinLevel)
				wMinLevel = pTreasure->wEnhanceLevel;

			wMinLevel = min(wMinLevel, pTreasure->wEnhanceLevel);
		}
	}
	return wMinLevel;
}

// ȡӢ�۱������Ʒ��
WORD CTreasureManager::GetLowQualityInWear()
{
	WORD wLowQuality = 0;
	const Formation *pFormationList = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList;
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation &tFormation = pFormationList[i];
		if (!tFormation.dwHeroUID)
			return 0;

		for (BYTE k = 0; k < TREASURE_TYPE_MAX; ++k)
		{
			DWORD dwTreasureId = tFormation.arTreasureList[k];
			const Treasure *pTreasure = dwTreasureId ? GetTreasure(dwTreasureId) : NULL;
			if (!pTreasure)
				return 0;

			const Treasure_Config *pTreasureCfg = g_pCfgMgr->GetTreasure(pTreasure->dwInfoId);
			if (!pTreasureCfg)
				return 0;

			if (!wLowQuality)
				wLowQuality = pTreasureCfg->wQuality;

			wLowQuality = min(wLowQuality, pTreasureCfg->wQuality);
		}
	}

	return wLowQuality;
}

Treasure* CTreasureManager::FindTreasure(DWORD dwUID)
{
	for (WORD i = 0; i < m_pTreasureInfo->wTreasureNum; ++i)
	{
		if (m_pTreasureInfo->sTreasureList[i].dwId == dwUID)
		{
			return &m_pTreasureInfo->sTreasureList[i];
		}
	}

	return NULL;
}

// ȡһ���µ�ϵ��id
DWORD CTreasureManager::GetNewSeriesId()
{
	// ��Ӧ�ó����������
	if (m_dwSeriesId >= 9999)
	{
		m_dwSeriesId = 1;
		MAPLOG(ERROR_LEVEL, "[%s]�����ϵ��ID�����쳣,ʹ�ó���9999��!!!", m_pPlayer->GetUTF8RoleName());
	}

	return 100000 * m_pPlayer->GetLoginTimes() + m_dwSeriesId++;
}
