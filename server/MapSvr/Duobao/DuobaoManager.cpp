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

// ����ᱦ����
void CDuobaoManager::OnNetDuobaoData()
{
	SendDuobaoData();
}

// ������սĿ���б�
void CDuobaoManager::OnNetTargetList(WORD wTreasureInfoId, WORD wTreasureItemId)
{
	// �������Ƿ�Ϸ�
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(wTreasureInfoId);
	if (!pComposeCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ĺϳ���������[InfoId:%d]", __FUNCTION__, wTreasureInfoId);
		return;
	}
	if (!pComposeCfg->IsCostItem(wTreasureItemId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���Ǳ���ĺϳ���Ƭ[InfoId:%d,ItemId:%d]",
			__FUNCTION__, wTreasureInfoId, wTreasureItemId);
		return;
	}

	MSG_S2C_DUOBAO_TARGET_LIST_RESP msg;
	msg.m_wTreasureInfoId = wTreasureInfoId;
	msg.m_wTreasureItemId = wTreasureItemId;
	msg.m_byMonsterNum = 0;
	msg.m_byPlayerNum = 0;

	// �����������
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
 			if (pTarget->GetCharID() != m_pPlayer->GetCharID() &&							// �ų��Լ�
 				pTarget->GetLevel() >= wMinLevel && pTarget->GetLevel() <= wMaxLevel &&		// ָ���ȼ���Χ��
 				pTarget->GetItemManager()->GetItem(wTreasureItemId) &&						// ӵ��Ҫ�������Ʒ
				!pTarget->GetDuobaoManager()->IsInPeace() &&								// ����ս״̬
				pTarget->GetLevel() >= 15)	// 15�����ϲſ��� todo::Ҫ����Ϊ�����ļ�								
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

	// todo::�����������

	// ���ҹ���
	const DuobaoMap_Config *pDuobaoMapCfg = CConfigManager::Instance()->GetDuobaoMap(m_pPlayer->GetLevel());
	if (!pDuobaoMapCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ᱦ�ؿ�����[InfoId:%d,ItemId:%d]",
			__FUNCTION__, wTreasureInfoId, wTreasureItemId);
		return;
	}
	if (!pDuobaoMapCfg->vMapList.size() || pDuobaoMapCfg->vMapList.size() < DUOBAO_TARGET_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�ᱦ�ؿ������б����(Ϊ�ջ���С��ָ������)![InfoId:%d,ItemId:%d,ListSize:%d]",
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

// ����5�ζᱦ(todo::5�ζᱦ��Ŀ��һ�����������)
void CDuobaoManager::OnNetDuoBaoFiveTimes(WORD wTreasureInfoId, WORD wTreasureItemId, WORD wRequestTimes)
{
	// ��ʱ������
// 	m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
// 	return;

	// ɨ����һ������Ƭ
	const Item_Config *pItemCfg = g_pCfgMgr->GetItem(wTreasureItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ƭ����[InfoId:%d,TreasureItemId:%d]", 
			__FUNCTION__, wTreasureInfoId, wTreasureItemId);
		return;
	}

	// ���VIP
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < m_pPlayer->GetVipValue(VIP_FIVE_TIMES_DB_FLAG))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]5�ζᱦ��Ҫ��VIP�ȼ�����", __FUNCTION__);
		return;
	}

	// ȡ��������
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(wTreasureInfoId);
	if (!pTreasureCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[InfoId:%d]", __FUNCTION__, wTreasureInfoId);
		return;
	}

	// ȡ�ᱦ�ؿ�����
	const DuobaoMap_Config *pMapCfg = CConfigManager::Instance()->GetDuobaoMap(m_pPlayer->GetLevel());
	if (!pMapCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ᱦ�ؿ�����[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return;
	}

	// ȡ��������
	const DuobaoDrop_Config *pDropCfg = CConfigManager::Instance()->GetDuobaoDrop(pTreasureCfg->wQuality);
	if (!pDropCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ᱦ��������[quality:%d]", __FUNCTION__, pTreasureCfg->wQuality);
		return;
	}

	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER(pItemMgr);

	vector<Item> vItemList;
	WORD wDoTimes = 0;
	BYTE byFlag = false;
	for (int i = 0; i < wRequestTimes; ++i)
	{
		// ��������Ƿ��㹻
		if (m_pPlayer->GetProperty(ROLE_PRO_ENERGY) < 2)
			break;

		++wDoTimes;
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, pMapCfg->nWinGold, TRUE, FROM_DOUBAO);
		m_pPlayer->AddExp(pMapCfg->dwExp);
		m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -2, TRUE, FROM_DOUBAO);

		// ����	
		DWORD dwPFGitfId = 0;
		Item tPFItem;
		vector<Item> vDropRewardItemList;
		m_pPlayer->GetFubenManager()->DoLimitDrop(999, vDropRewardItemList);
		if (vDropRewardItemList.size())
		{
			// ���������ж��,������ֶ��,��ֻȡ��һ��
			tPFItem = vDropRewardItemList[0];
		}
		else
		{
			// ���û����ʱ�����,��ȡ������
			tPFItem = Item(pMapCfg->wPFGitfId, 1);
		}

		if (!pItemMgr->UseRandItem(tPFItem.dwId, tPFItem.dwNum, vItemList))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�ᱦʹ�÷�����Ʒʧ��[ItemId:%d]", __FUNCTION__, pMapCfg->wPFGitfId);
			return;
		}
		// һ���ǻ�ֻ���һ��
		if (i + 1 != vItemList.size())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_DUOBAO_FIVE_TIMES_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�ᱦ���Ʋ�������Ʒ��ƥ��[ItemId:%d]", __FUNCTION__, pMapCfg->wPFGitfId);
			return;
		}

		WORD wRate = pDropCfg->wRobotRate;

		// todo::����ǵ�һ����ɫ����,��ص�(��������)
		if (!m_pPlayer->CheckOnceFlag(once_get_purple_treasure) && QUALITY_BLUE == pTreasureCfg->wQuality)
		{
			m_pPlayer->SetOnceFlag(once_get_purple_treasure);
			wRate = RAND_RANGE_MAX_10000;
		}

		// �ᱦ��һ������,��ض�����
		WORD wDoubaoTimes = GetDoubaoTimes(pTreasureCfg->wQuality);
		if (wDoubaoTimes >= pDropCfg->wTimesOfSureDrop)
		{
			wRate = RAND_RANGE_MAX_10000;

			// ���ô���
			SetDoubaoTimes(pTreasureCfg->wQuality, 0);
		}

		// ����(�ᵽ��Ҫ����Ʒ����˳�)
		if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) <= wRate)
		{
			// ��ս�߻����Ʒ
			m_pPlayer->AddObject(wTreasureItemId, 1, TRUE, FROM_FIVE_TIMES_DOUBAO);
			byFlag = true;
			break;
		}

		// ��¼�ᱦ����
		SetDoubaoTimes(pTreasureCfg->wQuality, wDoubaoTimes+1);
	}

	// ��ӻ�õ���Ʒ
	m_pPlayer->AddObjectList(vItemList, TRUE, FROM_FIVE_TIMES_DOUBAO);

	// 8-14���¼�(��ս�ᱦ������ʤ����������ս5�Σ�)
	m_pPlayer->OnUpdateSevenDayTarget(21, wDoTimes, true);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 7, wDoTimes, true);

	// ���ջ�ԾĿ��
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 7, wDoTimes, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 7, wDoTimes, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 7, wDoTimes, true);

	// todo::Ϊ�˲���BUG
	for (size_t i = 0; i < vItemList.size(); ++i)
	{
		if (!vItemList[i].dwId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ᱦ���Ʋ�������ƷIDΪ��[ItemId:%d]", __FUNCTION__, pMapCfg->wPFGitfId);
		}
	}

	m_pPlayerDuobaoInfo->wDailyDoubaoTimes += wDoTimes;

	// ������սʱ��
	SetPeaceOverTime(0);
	SendDuobaoData();

	MSG_S2C_DUOBAO_FIVE_TIMES_RESP msg;
	msg.m_ItemList = vItemList;
	msg.m_byFlag = byFlag;
	//msg.m_byTimes = byTimes;
	m_pPlayer->SendPacket(&msg);

	// �ճ������¼�
	m_pPlayer->OnUpdateDailyMission(11, 5);
}

// ��ȡ���Ƚ���
void CDuobaoManager::OnNetTakePregrossReward(WORD *pPregrossIdList, WORD wPregrossIdNum)
{
	for (WORD i = 0; i < wPregrossIdNum; ++i)
	{
		WORD wPregrossId = pPregrossIdList[i];

		// ȡ����
		const DuobaoPregrossReward_Config *pRewardCfg = g_pCfgMgr->GetDuobaoPregrossReward(wPregrossId);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ᱦ���Ƚ�������[PregrossId:%d]", __FUNCTION__, wPregrossId);
			continue;
		}

		// ����Ƿ����
		if (m_pPlayerDuobaoInfo->wDailyDoubaoTimes < pRewardCfg->wTimes)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ᱦ������Ҫ�Ĵ�������[PregrossId:%d]", __FUNCTION__, wPregrossId);
			continue;
		}

		// ����Ƿ�����ȡ��
		if (IsPregrossRewardRecord(wPregrossId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ᱦ���Ƚ�������ȡ��[PregrossId:%d]", __FUNCTION__, wPregrossId);
			continue;
		}

		// ��ӽ�����¼
		AddPregrossRewardRecord(wPregrossId);

		// ��ӽ���
		m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_DOUBAO_PREGROSS_REWARD);
	}

	MSG_S2C_TAKE_DUOBAO_PROGRESS_REWARD_RESP msg;
	msg.m_wPreprossIdNum = wPregrossIdNum;
	memcpy_s(msg.m_PregrossIdList, sizeof(msg.m_PregrossIdList), pPregrossIdList, wPregrossIdNum * sizeof(WORD));
	m_pPlayer->SendPacket(&msg);
}

// ���Ͷᱦ����
void CDuobaoManager::SendDuobaoData()
{
	MSG_S2C_DUOBAO_DATA_RESP msg;
	msg.m_DuobaoInfo = *m_pPlayerDuobaoInfo;
	m_pPlayer->SendPacket(&msg);
}

// ��ս��ʼ
bool CDuobaoManager::BattleStar(DWORD dwTargetId, bool bIsPlayer, WORD wTreasureInfoId, WORD wTreasureItemId)
{
	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// �������
	if (m_pPlayer->GetProperty(ROLE_PRO_ENERGY) < 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ᱦ��������", __FUNCTION__);
		return false;
	}

	pFBMgr->SetBattleInfo(0, 0, 0);
	pFBMgr->SetTargetId(dwTargetId);
	pFBMgr->SetParam(0, wTreasureInfoId);
	pFBMgr->SetParam(1, wTreasureItemId);
	pFBMgr->SetParam(2, bIsPlayer);

	// ��������
	if (bIsPlayer)
	{
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), dwTargetId, true);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// ����ս��
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}
	return true;
}

// ��ս����
bool CDuobaoManager::BattleOver(bool bIsWin)
{	
	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
	WORD wTreasureInfoId = pFBMgr->GetParam(0);
	WORD wTreasureItemId = pFBMgr->GetParam(1);
	BYTE bIsPlayer = pFBMgr->GetParam(2);
	BYTE byIsGetItem;
	return DoBattleOver(pFBMgr->GetTargetId(), wTreasureInfoId, wTreasureItemId, bIsPlayer, bIsWin, byIsGetItem);
}

// ������ս����ʱ�� 
void CDuobaoManager::SetPeaceOverTime(DWORD dwTime)
{
	m_pPlayerDuobaoInfo->dwPeaceOverTime = dwTime;
	//m_pPlayer->SetProperty(ROLE_PRO_DUOBAO_PEACE_OVER_TIME, dwTime);

	// ����ͬ������
	SendDuobaoData();
}

// �Ƿ���ս��
bool CDuobaoManager::IsInPeace()
{
	return time(NULL) < m_pPlayerDuobaoInfo->dwPeaceOverTime;
}

bool CDuobaoManager::DoBattleOver(DWORD dwTargetId, WORD wTreasureInfoId, WORD wTreasureItemId, BYTE byIsPlayer, bool bIsWin, BYTE &byIsGetItem)
{
	byIsGetItem = false;

	// ȡ��������
	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(wTreasureInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[InfoId:%d]", __FUNCTION__, wTreasureInfoId);
		return false;
	}

	// ȡ�ᱦ�ؿ�����
	const DuobaoMap_Config *pMapCfg = CConfigManager::Instance()->GetDuobaoMap(m_pPlayer->GetLevel());
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ᱦ�ؿ�����[RoleLevel:%d]", __FUNCTION__, m_pPlayer->GetLevel());
		return false;
	}

	// ȡ��������
	const DuobaoDrop_Config *pDropCfg = CConfigManager::Instance()->GetDuobaoDrop(pTreasureCfg->wQuality);
	if (!pDropCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ᱦ��������[quality:%d]", __FUNCTION__, pTreasureCfg->wQuality);
		return false;
	}

	// ��ʤ
	if (bIsWin)
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, pMapCfg->nWinGold, TRUE, FROM_DOUBAO);

		// ����
		int nRate = byIsPlayer ? pDropCfg->wUserRate : pDropCfg->wRobotRate;

		// todo::����ǵ�һ����ɫ����,��ص�
		if (!m_pPlayer->CheckOnceFlag(once_get_purple_treasure) && QUALITY_BLUE == pTreasureCfg->wQuality)
		{
			m_pPlayer->SetOnceFlag(once_get_purple_treasure);
			nRate = RAND_RANGE_MAX_10000;
		}

		// �ᱦ��һ������ʱ,�ض�����
		WORD wDoubaoTimes = GetDoubaoTimes(pTreasureCfg->wQuality);
		if (wDoubaoTimes >= pDropCfg->wTimesOfSureDrop)
			nRate = RAND_RANGE_MAX_10000;

		if (nRate && Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) <= nRate)
		{
			// ����ս��ʧȥ��Ʒ
			if (byIsPlayer)
			{
				Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
				CItemManager *pItemMgr = pTarget ? pTarget->GetItemManager() : NULL;
				if (pItemMgr && pItemMgr->GetItemNum(wTreasureItemId))
					pItemMgr->RemoveItem(wTreasureItemId, 1, TRUE, FROM_DOUBAO);

				// ���ʼ�֪ͨ(�ᱦ)
				EmailInfo tEailInfo(5);
				tEailInfo.AddContext(EMAIL_CONTEXT_ITEM, wTreasureItemId);
				tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
				EmailManager::AddEmail(dwTargetId, tEailInfo, true);
			}

			// ��ս�߻����Ʒ
			m_pPlayer->AddObject(wTreasureItemId, 1, TRUE, FROM_DOUBAO);
			byIsGetItem = true;
		}

		// ��ӷ��ƻ���
		//m_pPlayer->GetItemManager()->SetFPItemId(pMapCfg->wPFGitfId);
		// ��ӿɷ���
		vector<Item> vDropRewardItemList;
		m_pPlayer->GetFubenManager()->DoLimitDrop(999, vDropRewardItemList);
		if (vDropRewardItemList.size())
		{
			// ���������ж��,������ֶ��,��ֻȡ��һ��
			m_pPlayer->GetItemManager()->SetFPItem(vDropRewardItemList[0]);
		}
		else
		{
			// ���û����ʱ�����,��ȡ������
			m_pPlayer->GetItemManager()->SetFPItem(Item(pMapCfg->wPFGitfId, 1));
		}

		// ���öᱦ����
		SetDoubaoTimes(pTreasureCfg->wQuality, byIsGetItem ? 0 : (wDoubaoTimes + 1));
	}
	// ʧ��
	else
	{
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, pMapCfg->nFailGold, TRUE, FROM_DOUBAO);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -2, TRUE, FROM_DOUBAO);

	m_pPlayer->AddExp(pMapCfg->dwExp);

	m_pPlayer->GetFubenManager()->SetParam(0, byIsGetItem); 

	// ���¶ᱦ����
	m_pPlayerDuobaoInfo->wDailyDoubaoTimes++;

	// ������սʱ��
	SetPeaceOverTime(0);
	SendDuobaoData();

	// �ճ������¼�
	m_pPlayer->OnUpdateDailyMission(11);

	// 8-14���¼�(��ս�ᱦ������ʤ����������ս5�Σ�)
	m_pPlayer->OnUpdateSevenDayTarget(21, 1, true);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 7, 1, true);

	// ���ջ�ԾĿ��
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 7, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 7, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 7, 1, true);

	return true;
}

// ȡ�ᱦ����
WORD CDuobaoManager::GetDoubaoTimes(WORD wQuality)
{
	if (wQuality >= QUALITY_NUM)
		return 0;

	return m_pPlayerDuobaoInfo->arDoubaoTimes[wQuality];
}

// ȡ�ᱦ����
void CDuobaoManager::SetDoubaoTimes(WORD wQuality, WORD wTimes)
{
	if (wQuality >= QUALITY_NUM)
		return;

	m_pPlayerDuobaoInfo->arDoubaoTimes[wQuality] = wTimes;
}

// �Ƿ���Ƚ�����¼
bool CDuobaoManager::IsPregrossRewardRecord(WORD wPregrossId)
{
	for (WORD i = 0; i < m_pPlayerDuobaoInfo->wPregrossRewardRecordNum; ++i)
	{
		if (m_pPlayerDuobaoInfo->arPregrossRewardRecord[i] == wPregrossId)
			return true;
	}
	return false;
}

// ��ӽ��Ƚ�����¼
void CDuobaoManager::AddPregrossRewardRecord(WORD wPregrossId)
{
	if (m_pPlayerDuobaoInfo->wPregrossRewardRecordNum >= MAX_DOUBAO_PREGROSS_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ᱦ���Ƚ�����¼�б�����[MaxNum:%d]", __FUNCTION__, MAX_DOUBAO_PREGROSS_NUM);
		return;
	}

	m_pPlayerDuobaoInfo->arPregrossRewardRecord[m_pPlayerDuobaoInfo->wPregrossRewardRecordNum++] = wPregrossId;
}

