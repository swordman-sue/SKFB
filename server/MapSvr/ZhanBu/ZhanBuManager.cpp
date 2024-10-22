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

// ��ʼս��
bool CZhanBuManager::OnBattleStart()
{
	WORD wMapIdx = m_pZhanBuInfo->wLowCardGroupMapIdx;
	const CardGroupMap_Config *pMapCfg = g_pCfgMgr->GetCardGroupMap(wMapIdx);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ؿ�����[MapIdx:%d]!", __FUNCTION__, wMapIdx);
		return false;
	}

	// ��¼ս����Ϣ
	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
	pFBMgr->SetBattleInfo(0, 0, pMapCfg->dwMapId);

	return true;
}

// ս������
bool CZhanBuManager::OnBattleOver(BYTE byIsWin)
{
	if (byIsWin)
	{
		CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();
		DWORD dwMapId = pFBMgr->GetMapId();

		// ������
		const Map_Config *pMapCfg = g_pCfgMgr->GetMap(dwMapId);
		if (!pMapCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ؿ�����[MapId:%d]!", __FUNCTION__, dwMapId);
			return false;
		}

		// ��ʤ����
		pFBMgr->SetIsFirstWin(true);

		// ��������
		m_pZhanBuInfo->wLowCardGroupMapIdx++;
	}

	return true;
}

// ���������������
void CZhanBuManager::OnNetPlayerZhanBuData()
{
	// ����Ƿ��ѳ�ʼ������
	if (!IsAlreadyInitData())
	{
		InitData();
	}

	SendPlayerZhanBuData();
}

// ���������б�
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

// ����
void CZhanBuManager::OnNetLevelUp(DWORD dwUID, DWORD *pCostCardList, WORD wCostCardNum)
{
	// ȡ��������
	Card* pCard = GetCardInner(dwUID);
	if (!pCard)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������[uid:%d]!", __FUNCTION__, dwUID);
		return;
	}

	// ȡ��������
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������������[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// 1.��������б�, 2.�����õ��ܾ���
	DWORD dwTotalExp = 0;
	for (WORD i = 0; i < wCostCardNum; ++i)
	{
		DWORD dwCostCardUId = pCostCardList[i];

		// ��õľ���
		dwTotalExp += GetCardCurTotalExp(dwCostCardUId);

		// ����Ƿ�����װ����
		if (m_pPlayer->GetHeroManager()->GetCardWearHeroId(dwCostCardUId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]��������װ����,������Ϊ���Ĳ���[InfoId:%d]!", __FUNCTION__, dwCostCardUId);
			return;
		}
	}

	pCard->dwExp += dwTotalExp;

	// ��������
	const CardLevelUp_Config *pLevelUpCfg;
	while (true)
	{
		// ȡ��������
		pLevelUpCfg = g_pCfgMgr->GetCardLevelUp(pCardCfg->byQuality, pCard->wLevel);
		if (!pLevelUpCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ƶ�������������[Quality:%d,Level:%d]!",
				__FUNCTION__, pCardCfg->byQuality, pCard->wLevel);
			break;
		}

		// ��ɫ�ȼ�����
		if (m_pPlayer->GetLevel() < pLevelUpCfg->wNeedRoleLevel)
			break;

		// �Ѵ�ȼ�����
		if (!pLevelUpCfg->dwExp)
			break;

		// ���㹻����
		if (pCard->dwExp < pLevelUpCfg->dwExp)
			break;

		// ����
		pCard->wLevel++;
		pCard->dwExp -= pLevelUpCfg->dwExp;
	}

	CardDataNotify(pCard);

	m_pPlayer->UpdateFightingPower();

	// �ȼ�¼,��ֹ�����ɾ��������ʹ���ݵ�λ�÷����ı���������ݱ仯
	WORD wLevel = pCard->wLevel;
	DWORD dwExp = pCard->dwExp;

	// ɾ�������б�
	RemoveCard(pCostCardList, wCostCardNum, FROM_CARD_LEVEL_UP);

	// ��Ӧ
	MSG_S2C_CARD_LEVEL_UP_RESP msg;
	msg.m_dwFairyUId = dwUID;
	msg.m_wFairyLevel = wLevel;
	msg.m_dwFairyExp = dwExp;
	msg.m_wCostFairyNum = wCostCardNum;
	memcpy_s(msg.m_arCostFairyList, wCostCardNum*sizeof(DWORD), pCostCardList, wCostCardNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);

	// ���ջ
	int nCurLevel = 0; // Ŀǰ�� 10,20,30,40,50������
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

// ͻ��
void CZhanBuManager::OnNetBreak(DWORD dwUID)
{
	// ȡ��������
	Card* pCard = GetCardInner(dwUID);
	if (!pCard)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������[uid:%d]!", __FUNCTION__, dwUID);
		return;
	}

	// ȡ��������
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������������[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// ȡ����ͻ������
	const CardBreak_Config *pBreakCfg = g_pCfgMgr->GetCardBreak(pCardCfg->byQuality, pCard->wBreakLevel);
	if (!pBreakCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������ͻ����������[Quality:%d,BreakLevel:%d]!", __FUNCTION__,
			pCardCfg->byQuality, pCard->wBreakLevel);
		return;
	}

	// �����Ҫ�Ŀ��Ƶȼ�
	if (pBreakCfg->wNeedCardLevel > pCard->wLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����ͻ���ܿ��Ƶȼ�����[CardUId:%d,CardInfoId:%d,NeedCardLv:%d,CurLevel:%d]!", 
			__FUNCTION__, pCard->dwId, pCard->dwInfoId, pBreakCfg->wNeedCardLevel, pCard->wLevel);
		return;
	}

	// ����������
	if (!m_pPlayer->CheckMoneyCost(pBreakCfg->vCostMoneyList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����ͻ�����Ļ��Ҳ���[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// �����Ʒ����
	if (!m_pPlayer->CheckItemCost(pBreakCfg->vCostItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����ͻ��������Ʒ����[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// ������ĵ�ͬ����ʼ���������Ƿ��㹻
	if (pBreakCfg->wCostSameCardNum && GetInitCardNum(pCard->dwInfoId) < pBreakCfg->wCostSameCardNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Ҫ���ĵ�ͬ��������������[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	// �����������
	for (size_t i = 0; i < pBreakCfg->vCostMoneyList.size(); ++i)
	{
		m_pPlayer->ModifyProperty(pBreakCfg->vCostMoneyList[i].wType, pBreakCfg->vCostMoneyList[i].nValue, TRUE, FROM_FAIRY_BREAK);
	}

	// ������Ʒ����
	for (size_t i = 0; i < pBreakCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pBreakCfg->vCostItemList[i].dwId, pBreakCfg->vCostItemList[i].dwNum, FROM_FAIRY_BREAK);
	}

	// ����ͬ����������
	for (WORD i = 0; i < pBreakCfg->wCostSameCardNum; ++i)
	{
		RemoveInitCard(pCard->dwInfoId);
	}

	// ͻ�Ƶȼ�����
	pCard->wBreakLevel++;
	
	CardDataNotify(pCard);

	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_CARD_BREAK_RESP msg;
	msg.m_dwFairyUId = dwUID;
	m_pPlayer->SendPacket(&msg);

	// ���ջ
	int nCurBreakLevel = 0; // Ŀǰ�� 1,2,3,4,5������ 
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

// ����ˢ��
void CZhanBuManager::OnNetCardRefresh(BYTE byRefreshType)
{
	// �ͼ�����
	if (1 == byRefreshType)
	{
		if (!DoLowCardGroupRefresh())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]����ͼ�����ˢ��ʧ��!", __FUNCTION__);
			return;
		}
	}
	// �߼�����
	else
	{
		if (!DoHighCardGroupRefresh())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]����߼�����ˢ��ʧ��!", __FUNCTION__);
			return;
		}
	}

	// ��Ӧ
	MSG_S2C_CARD_REFRESH_RESP resp;
	resp.m_byRefreshType = byRefreshType;
	m_pPlayer->SendPacket(&resp);
}

void CZhanBuManager::OnNetOpenCard(BYTE byType, WORD *pIdxList, WORD wIdxListNum)
{
	// ��������Ƿ�Ϸ�
	if (NULL == pIdxList || !wIdxListNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���Ʋ�������[type:%d]!", __FUNCTION__, byType);
		return;
	}

	int nKey = 1 == byType ? 251 : 252;
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(nKey);
	if (!pServerCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ������[key:%d]!", __FUNCTION__, nKey);
		return;
	}

	CardGroupData *pCardGroup = NULL;

	// �ͼ����鷭��
	if (1 == byType)
	{
		pCardGroup = m_pZhanBuInfo->arLowCardGroup;

		WORD wDailyFreeOpenCardTimes = g_GetConfigValue(247);

		// ������Ѵ�������
		if (wIdxListNum + m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes > wDailyFreeOpenCardTimes)
		{
			WORD wCostOpenCardTimes = wIdxListNum;

			// ������Ѵ���,��۳���Ѵ���
			if (m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes < wDailyFreeOpenCardTimes)
			{
				WORD wFreeTimes = wDailyFreeOpenCardTimes - m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes;
				wCostOpenCardTimes -= wFreeTimes;
			}

			// ���������
			int nCostGold = g_GetConfigValue(248) * wCostOpenCardTimes;
			if (m_pPlayer->GetProperty(ROLE_PRO_GOLD) < nCostGold)
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]�ͼ����鷭�����Ľ�Ҳ���!", __FUNCTION__);
				return;
			}

			m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, -nCostGold);
		}

		m_pZhanBuInfo->wTodayLowCardGroupOpenCardTimes += wIdxListNum;
	}
	// �߼����鷭��
	else
	{
		pCardGroup = m_pZhanBuInfo->arHighCardGroup;

		// �������
		DWORD dwCostItemId = g_GetConfigValue(249);
		if (m_pPlayer->GetItemNum(dwCostItemId) < wIdxListNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�߼����鷭��������Ʒ����[ItemId:%d]!", __FUNCTION__, dwCostItemId);
			return;
		}

		m_pPlayer->RemoveItem(dwCostItemId, wIdxListNum, FROM_OPEN_CARD);
	}

	int nRandDestinyCoin = 0;
	
	DWORD arDestinyCoinList[MAX_CARD_GROUP_NUM] = { 0 };
	for (WORD i = 0; i < wIdxListNum; ++i)
	{
		WORD wIdx = pIdxList[i];

		// ��������±��Ƿ�Խ��(�ͻ��˴�1��ʼ)
		if (!wIdx || wIdx > MAX_CARD_GROUP_NUM)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���Ʋ�����������[type:%d,idx:%d]!", __FUNCTION__, byType, wIdx);
			return;
		}

		WORD wSvrIdx = wIdx - 1;

		// ��⿨���Ƿ��ѷ���
		if (IsCardAlreadyOpen(byType, wSvrIdx))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����Ѿ�������[type:%d,idx:%d]!", __FUNCTION__, byType, wSvrIdx);
			continue;
		}

		CardGroupData &tGroupData = pCardGroup[wSvrIdx];

		// ��ÿ���
		AddCard(tGroupData.dwCardId, TRUE, FROM_OPEN_CARD);

		// ��¼Ϊ�ѷ���
		tGroupData.wIsAlreadyOpen = true;


		// ������˱�
		arDestinyCoinList[i] = g_Rand(pServerCfg->GetListValue(0), pServerCfg->GetListValue(1));
		nRandDestinyCoin += arDestinyCoinList[i];
	}

	// todo::����ͬ������


	// ���ջ(�ۼ�ռ�����ƴ���)
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 19, wIdxListNum, true);

	// ���ջ�ԾĿ��(�ۼ�ռ�����ƴ���)
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 19, wIdxListNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 19, wIdxListNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 19, wIdxListNum, true);


	// �߼����鷭��
	if (byType != 1)
	{
		// ���ջ(�ۼƸ߼�ռ�����ƴ���)
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 20, wIdxListNum, true);

		// ���ջ�ԾĿ��(�ۼƸ߼�ռ�����ƴ���)
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 20, wIdxListNum, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 20, wIdxListNum, true);

		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 20, wIdxListNum, true);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DESTINY_COIN, nRandDestinyCoin);

	// �ճ�����(ռ������10��)
	m_pPlayer->OnUpdateDailyMission(25, wIdxListNum);

	// ��Ӧ
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

// �ͼ�����ˢ�´�������
void CZhanBuManager::OnNetLowCardGroupRefreshTimesUpdate()
{
	UpdateLowCardGroupRefreshTimes();

	// ��Ӧ
	MSG_S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP resp;
	resp.m_wCurLowCardGroupRefreshTimes = m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes;
	resp.m_dwLowCardGroupLastRecoverTime = m_pZhanBuInfo->dwLowCardGroupLastRecoverTime;
	m_pPlayer->SendPacket(&resp);
}

// �ղ��鼤��
void CZhanBuManager::OnNetCellectionGroupActivate(WORD wBookId, WORD wGroupId)
{
	// ����Ƿ��Ѽ���
	CollectionGroup *pGroupData = GetCollectionGroup(wBookId, wGroupId);
	if (pGroupData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ղ����Ѽ���[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// ȡ�ղ�������
	const CollectionGroup_Config *pGroupCfg = g_pCfgMgr->GetCollectionGroup(wGroupId);
	if (!pGroupCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}


	// �����Ҫ���ղ�ֵ
	const CollectionBook_Config *pBookCfg = g_pCfgMgr->GetCollectionBook(wBookId);
	if (pBookCfg && pBookCfg->nNeedCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Ҫ���ղ�ֵ����[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// ������ĵĿ���
	vector<DWORD>::const_iterator iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		if (!GetInitCard(dwCostCardId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�����ղ�����Ҫ���ĵĿ��Ʋ�����[BookId:%d,GroupId:%d,CostCardId:%d]!", 
				__FUNCTION__, wBookId, wGroupId, dwCostCardId);
			return;
		}
	}

	// ɾ�����Ŀ���
	iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		RemoveInitCard(dwCostCardId);
	}

	// ����ղ���
	AddCollectGroup(wBookId, wGroupId);

	// ���Ը���
	m_pPlayer->UpdateFightingPower();

	// ������ʷ�ղ�ֵ
	UpdateHistoryCollectionValue();

// 	DWORD dwCollectionValue = GetCurCollectionValue();
// 	if (dwCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
// 		m_pZhanBuInfo->dwHistoryCollectionValue = dwCollectionValue;

	// ��Ӧ
	MSG_S2C_CELLECTION_GROUP_ACTIVATE_RESP resp;
	resp.m_wBookId = wBookId;
	resp.m_wGroupId = wGroupId;
	resp.m_dwHistoryCollectionValue = m_pZhanBuInfo->dwHistoryCollectionValue;
	m_pPlayer->SendPacket(&resp);

// 	// �������а�
// 	CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, m_pPlayer, dwCollectionValue);
}

// �ղ�������
void CZhanBuManager::OnNetCellectionGroupStarUp(WORD wBookId, WORD wGroupId)
{
	// ȡ�ղ�������
	CollectionGroup *pGroupData = GetCollectionGroup(wBookId, wGroupId);
	if (!pGroupData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// ����Ƿ񵽴���������
	if (pGroupData->wStar >= 7)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ղ��������Ѵ�����[BookId:%d,GroupId:%d,Star:%d]!", 
			__FUNCTION__, wBookId, wGroupId, pGroupData->wStar);
		return;
	}

	// ȡ�ղ�������
	const CollectionGroup_Config *pGroupCfg = g_pCfgMgr->GetCollectionGroup(wGroupId);
	if (!pGroupCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// ������ĵĿ���
	vector<DWORD>::const_iterator iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		if (!GetInitCard(dwCostCardId))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�����ղ�����Ҫ���ĵĿ��Ʋ�����[BookId:%d,GroupId:%d,CostCardId:%d]!",
				__FUNCTION__, wBookId, wGroupId, dwCostCardId);
			return;
		}
	}

	// ɾ�����Ŀ���
	iter = pGroupCfg->vCostCardList.begin();
	for (; iter != pGroupCfg->vCostCardList.end(); ++iter)
	{
		DWORD dwCostCardId = *iter;
		RemoveInitCard(dwCostCardId);
	}

	// �ղ�������
	pGroupData->wStar++;

	// ���Ը���
	m_pPlayer->UpdateFightingPower();

	// ������ʷ�ղ�ֵ
	UpdateHistoryCollectionValue();

// 	DWORD dwCollectionValue = GetCurCollectionValue();
// 	if (dwCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
// 		m_pZhanBuInfo->dwHistoryCollectionValue = dwCollectionValue;

	// ��Ӧ
	MSG_S2C_CELLECTION_GROUP_STAR_UP_RESP resp;
	resp.m_wBookId = wBookId;
	resp.m_wGroupId = wGroupId;
	resp.m_dwHistoryCollectionValue = m_pZhanBuInfo->dwHistoryCollectionValue;
	m_pPlayer->SendPacket(&resp);

	// �������а�
	//CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, m_pPlayer, dwCollectionValue);
}

// �ղ�������
void CZhanBuManager::OnNetCellectionGroupReset(WORD wBookId, WORD wGroupId)
{
	// ȡ�ղ�������
	CollectionGroup *pGroupData = GetCollectionGroup(wBookId, wGroupId);
	if (!pGroupData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// ȡ�ղ�������
	const CollectionGroup_Config *pGroupCfg = g_pCfgMgr->GetCollectionGroup(wGroupId);
	if (!pGroupCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d,GroupId:%d]!", __FUNCTION__, wBookId, wGroupId);
		return;
	}

	// �黹���ĵĿ���
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

	// ɾ�����õ��ղ���
	RemoveCollectGroup(wBookId, wGroupId);

	// ���Ը���
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_CELLECTION_GROUP_RESET_RESP msg;
	msg.m_wBookId = wBookId;
	msg.m_wGroupId = wGroupId;
	msg.m_wStar = wStar;
	m_pPlayer->SendPacket(&msg);
}

// �����ղ����б�
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

// װ������
void CZhanBuManager::OnNetEquipCard(WORD wFormationPos, WORD wCardSlot, DWORD dwUID)
{
	// ������ (����λ��, װ��λ��)
	if (!wFormationPos || wFormationPos > MAX_BATTLE_HERO_NUM || !wCardSlot || wCardSlot > MAX_CARD_SLOT_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����װ�����Ʋ�������[formation_pos:%d, card_slot:%d]",
			__FUNCTION__, wFormationPos, wCardSlot);
		return;
	}

	// ���۵ȼ����Ƽ��
	const CardSlot_Config *pCardSlotCfg = g_pCfgMgr->GetCardSlot(wCardSlot);
	if (!pCardSlotCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[card_slot:%d]",__FUNCTION__, wCardSlot);
		return;
	}
	if (m_pPlayer->GetLevel() < pCardSlotCfg->wNeedRoleLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Ҫ�Ľ�ɫ�ȼ�����[card_slot:%d]", __FUNCTION__, wCardSlot);
		return;
	}

	WORD wSvrFormationPos = wFormationPos - 1;
	WORD wSvrCardSlot = wCardSlot - 1;

	// ����Ƿ��д˿���
	const Card *pCard = GetCard(dwUID);
	if (!pCard)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˿���[UID:%d]", __FUNCTION__, dwUID);
		return;
	}

	Formation &tFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[wSvrFormationPos];
	DWORD dwOldCardUID = tFormation.arCardList[wSvrCardSlot];
	tFormation.arCardList[wSvrCardSlot] = dwUID;

	// ͬ�����Ƶ�ǰװ��״̬
	if (dwOldCardUID)
		CardDataNotify(dwOldCardUID);
	CardDataNotify(dwUID);

	// ����ͬ�������б�
	m_pPlayer->GetHeroManager()->SendFormationList();

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_COLLECTION_EQUIP_CARD_RESP msg;
	msg.m_wFormationPos = wFormationPos;
	msg.m_wCardSlot = wCardSlot;
	msg.m_dwCardUID = dwUID;
	m_pPlayer->SendPacket(&msg);
}

// ж�¿���
void CZhanBuManager::OnNetUnloadCard(WORD wFormationPos, WORD wCardSlot)
{
	// ������ (����λ��, װ��λ��)
	if (!wFormationPos || wFormationPos > MAX_BATTLE_HERO_NUM || !wCardSlot || wCardSlot > MAX_CARD_SLOT_NUM)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����װ�����Ʋ�������[formation_pos:%d, card_slot:%d]",
			__FUNCTION__, wFormationPos, wCardSlot);
		return;
	}

	WORD wSvrFormationPos = wFormationPos - 1;
	WORD wSvrCardSlot = wCardSlot - 1;

	Formation &tFormation = m_pPlayer->GetRoleFullInfo().tHeroInfo.arFormationList[wSvrFormationPos];
	DWORD dwUnloadCardUID = tFormation.arCardList[wSvrCardSlot];
	tFormation.arCardList[wSvrCardSlot] = 0;

	// ͬ�����Ƶ�ǰװ��״̬
	if (dwUnloadCardUID)
		CardDataNotify(dwUnloadCardUID);

	// ����ͬ�������б�
	m_pPlayer->GetHeroManager()->SendFormationList();

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_COLLECTION_UNLOAD_CARD_RESP msg;
	msg.m_wFormationPos = wFormationPos;
	msg.m_wCardSlot = wCardSlot;
	m_pPlayer->SendPacket(&msg);
}

// ����ɾ�
void CZhanBuManager::OnNetAcivateAchievement()
{
	// ׼������ĵȼ�
	WORD wReadyActivateLevel = m_pZhanBuInfo->wAchievementLevel + 1;

	const ZhanBuAchievement_Config *pAchievementCfg = g_pCfgMgr->GetZhanBuAchievement(wReadyActivateLevel);
	if (!pAchievementCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ռ���ɾ�����[level:%d]!", __FUNCTION__, wReadyActivateLevel);
		return;
	}

	// �����Ҫ�ĳɾ�ֵ�Ƿ��㹻
	if (pAchievementCfg->dwNeedCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ղ�ֵ����[level:%d,need_collection_value:%d]!", 
			__FUNCTION__, wReadyActivateLevel, pAchievementCfg->dwNeedCollectionValue);
		return;
	}

	m_pZhanBuInfo->wAchievementLevel = wReadyActivateLevel;

	// ����ս��
	m_pPlayer->UpdateFightingPower();

	// ��Ӧ
	MSG_S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP msg;
	msg.m_wCurAchievementLevel = wReadyActivateLevel;
	m_pPlayer->SendPacket(&msg);
}

// ȡ����
const Card* CZhanBuManager::GetCard(DWORD dwUID)
{
	return GetCardInner(dwUID);
}

// �������
void CZhanBuManager::AddCard(DWORD dwInfoId, bool bIsNotify, WORD wFromId)
{
	Card tFairy;
	memset(&tFairy, 0, sizeof(tFairy));
	tFairy.dwInfoId = dwInfoId;
	tFairy.dwId = GetNewSeriesId();
	tFairy.wLevel = 1;
	AddCard(&tFairy, bIsNotify, wFromId);
}

// �������
void CZhanBuManager::AddCard(const Card *pCard, bool bIsNotify, WORD wFromId)
{
	if (!GetBagEmptySize())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ʊ����Ѵ�����[RoleId:%d,CardListNum:%d,CardInfoId:%d]", __FUNCTION__,
			m_pPlayer->GetRoleId(), m_pZhanBuInfo->wCardNum, pCard->dwInfoId);

		// ��¼Ӣ�۶�ʧ��¼
		m_pPlayer->SaveItemLoseRecord(pCard->dwInfoId);

		return;
	}

	if (m_pZhanBuInfo->wCardNum >= MAX_CARD_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����б�����,���ʧ��!",__FUNCTION__);
		return;
	}

	// �������
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
		return;
	}

	m_pZhanBuInfo->arCardList[m_pZhanBuInfo->wCardNum++] = *pCard;

	// ֪ͨ�ͻ���
	if (bIsNotify)
	{
		CardDataNotify(pCard);
	}

	// ��¼���
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), pCard->dwInfoId, 1, true, wFromId);
}

// ɾ������
void CZhanBuManager::RemoveCard(DWORD dwUID, bool bIsNotify, WORD wFromId)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		Card *pCard = &m_pZhanBuInfo->arCardList[i];
		if (pCard->dwId == dwUID)
		{
			if (bIsNotify)
				CardDataNotify(pCard,false);

			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), pCard->dwInfoId, 1, false, wFromId);

			*pCard = m_pZhanBuInfo->arCardList[--m_pZhanBuInfo->wCardNum];
			return;
		}
	}
}

// ����ɾ������
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
				// ��¼���
				g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), tDeleteCard.dwInfoId, 1, false, wFromId);

				CardDataNotify(&tDeleteCard, false);
				tDeleteCard = m_pZhanBuInfo->arCardList[--m_pZhanBuInfo->wCardNum];
				break;
			}
		}
	}

	return false;
}

// ȡ���鵱ǰ�ܾ���
DWORD CZhanBuManager::GetCardCurTotalExp(DWORD dwUID)
{
	// ȡ��������
 	const Card *pCard = GetCard(dwUID);
	if (!pCard)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҿ�������[UId:%d]!",__FUNCTION__, dwUID);
 		return 0;
 	}
 
 	// ȡ��������
	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������������[InfoId:%d]!", __FUNCTION__, pCard->dwInfoId);
 		return 0;
 	}
 
 	// ȡ��������
	const CardLevelUp_Config *pLevelUpCfg = g_pCfgMgr->GetCardLevelUp(pCardCfg->byQuality, pCard->wLevel);
 	if (!pLevelUpCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������������[Quality:%d,Level:%d]!",
			__FUNCTION__, pCardCfg->byQuality, pCard->wLevel);
 		return 0;
 	}
 
	return pCard->dwExp + pCardCfg->dwToExp + pLevelUpCfg->dwTotalExp;
}

// ȡ��ʼ����
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

// ɾ����ʼ����
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

// �Ƿ��ʼ����
bool CZhanBuManager::IsInitCard(const Card *pCard)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();

	if (!pCard->dwExp && !pCard->wBreakLevel && pCard->wLevel <= 1 && !pHeroMgr->GetCardWearHeroId(pCard->dwId))
		return true;

	return false;
}

// ȡ��ʼ��������
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

// ȡ�ղ���ʷ��¼
DWORD CZhanBuManager::GetHistoryCollectionValue()
{
	return m_pZhanBuInfo->dwHistoryCollectionValue;
}

// ȡָ���ȼ��Ŀ�������
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

// ȡָ���׵Ŀ�������
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

// ȡ��������
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


// ȡ����(�ڲ��ӿ�)
Card* CZhanBuManager::GetCardInner(DWORD dwUID)
{
	for (WORD i = 0; i < m_pZhanBuInfo->wCardNum; ++i)
	{
		if (m_pZhanBuInfo->arCardList[i].dwId == dwUID)
			return &m_pZhanBuInfo->arCardList[i];
	}

	return NULL;
}

// ȡһ���µ�ϵ��id
DWORD CZhanBuManager::GetNewSeriesId()
{
	// ��Ӧ�ó����������
	if (m_dwSeriesId >= 9999)
	{
		m_dwSeriesId = 1;
		MAPLOG(ERROR_LEVEL, "[%s]�����ϵ��ID�����쳣,ʹ�ó���9999��!!!", m_pPlayer->GetUTF8RoleName());
	}

	return 100000 * m_pPlayer->GetLoginTimes() + m_dwSeriesId++;
}

// ���������������
void CZhanBuManager::SendPlayerZhanBuData()
{
	MSG_S2C_PLAYER_ZHANBU_DATA_RESP msg;
	msg.m_pZhanBuInfo = m_pZhanBuInfo;
	m_pPlayer->SendPacket(&msg);
}

// �����Ƿ��ѷ���
bool CZhanBuManager::IsCardAlreadyOpen(BYTE byType, WORD wIdx)
{
	if (wIdx >= MAX_CARD_GROUP_NUM)
		return false;

	// �ͼ�����
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

// ���µͼ�����ˢ�´���
void CZhanBuManager::UpdateLowCardGroupRefreshTimes()
{
	if (m_pZhanBuInfo->dwLowCardGroupLastRecoverTime)
	{
		int nRefreshTimes = DoRecoveryTimes(m_pZhanBuInfo->dwLowCardGroupLastRecoverTime, time(NULL), g_GetConfigValue(239)*60);
		if (nRefreshTimes > 0)
		{
			m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes += nRefreshTimes;

			// �ָ��������޾���
			m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes = min(m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes, g_GetConfigValue(240));
		}
	}
}


// ����ͼ�����ˢ��
bool CZhanBuManager::DoLowCardGroupRefresh(bool bIsNewRole)
{
	int nRandCardNum = 10;
	nRandCardNum = min(nRandCardNum, MAX_CARD_GROUP_NUM);

	DWORD dwCurTime = time(NULL);

	int nCostDiamond = 0;

	// �½���ɫ������
	if (!bIsNewRole)
	{
		// û����Ѵ���
		if (!m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes)
		{
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(241);
			if (!pServerCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ������[key:%d]!", __FUNCTION__, 241);
				return false;
			}
			nCostDiamond = pServerCfg->GetListValue(m_pZhanBuInfo->wTodayLowCardGroupCostRefreshTimes);

			// �������
			if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�ͼ�����ˢ�����Ĳ���!", __FUNCTION__);
				return false;
			}
		}
	}

	// ȡ����ؿ�����
	const CardGroupMap_Config *pMapCfg = g_pCfgMgr->GetCardGroupMap(m_pZhanBuInfo->wLowCardGroupMapIdx);
	if (!pMapCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ͼ�����ؿ�����[MapIdx:%d]!", __FUNCTION__, m_pZhanBuInfo->wLowCardGroupMapIdx);
		return false;
	}

	// ȡ��������
	const vector<RateDataEx> *pCardGroupCfg = g_pCfgMgr->GetLowCardGroup(pMapCfg->wCardGroupId);
	if (!pCardGroupCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ͼ���������[CardGroupId:%d]!", __FUNCTION__, pMapCfg->wCardGroupId);
		return false;
	}

	// ��⿨��������Ƿ��㹻
	if (pCardGroupCfg->size() < nRandCardNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���鿨����������[CardGroupId:%d,size:%d]!",
			__FUNCTION__, pMapCfg->wCardGroupId, pCardGroupCfg->size());
		return false;
	}


	// �����ܸ���
	int nTotalRate = 0;
	vector<RateDataEx>::const_iterator iter = pCardGroupCfg->begin();
	for (; iter != pCardGroupCfg->end(); ++iter)
	{
		nTotalRate += iter->nRate;
	}

	// ���ָ�������Ŀ���
	m_pZhanBuInfo->wLowCardGroupNum = 0;
	memset(m_pZhanBuInfo->arLowCardGroup, 0, sizeof(m_pZhanBuInfo->arLowCardGroup));
	for (int i = 0; i < nRandCardNum; ++i)
	{
		int nRandValue = g_Rand(0, nTotalRate);
		int nCurRate = 0;

		// �����Ǹ�����
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
			MAPLOG(ERROR_LEVEL, "[%s]����û�����������[RoleId:%d,RandValue:%d,Idx:%d]!",
				__FUNCTION__, m_pPlayer->GetRoleId(), nRandValue,i);
		}
	}

	// ���µ�ǰ����ID
	m_pZhanBuInfo->wCurLowCardGroupId = pMapCfg->wCardGroupId;

	// �´����Ľ�ɫ����Ҫ����
	if (!bIsNewRole)
	{
		// ʹ������ˢ��
		if (nCostDiamond > 0)
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CARD_GROUP_REFRESH);
			m_pZhanBuInfo->wTodayLowCardGroupCostRefreshTimes++;
		}
		// ʹ����Ѵ���ˢ��
		else
		{
			m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes--;
		}
	}

	return true;
}


// ����߼�����ˢ��
bool CZhanBuManager::DoHighCardGroupRefresh(bool bIsNewRole)
{
	int nRandCardNum = 10;
	nRandCardNum = min(nRandCardNum, MAX_CARD_GROUP_NUM);

	DWORD dwCurTime = time(NULL);
	int nCostDiamond = 0;

	// �½���ɫ������
	if (!bIsNewRole)
	{
		// ��û���ﶨʱˢ��ʱ��
		if (m_pZhanBuInfo->dwHighCardGroupLastRefreshTime &&
			dwCurTime < m_pZhanBuInfo->dwHighCardGroupLastRefreshTime + g_GetConfigValue(242) * 60)
		{
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(243);
			if (!pServerCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ������[key:%d]!", __FUNCTION__, 243);
				return false;
			}
			nCostDiamond = pServerCfg->GetListValue(m_pZhanBuInfo->wTodayHighCardGroupCostRefreshTimes);

			// �������
			if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�߼�����ˢ�����Ĳ���!", __FUNCTION__);
				return false;
			}
		}
	}

	// ȡ��������
	const vector<HighCardGroup_Config> *pCardGroupCfg = g_pCfgMgr->GetHighCardGroup();
	if (!pCardGroupCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����߼���������!", __FUNCTION__);
		return false;
	}

	// ��⿨��������Ƿ��㹻
	if (pCardGroupCfg->size() < nRandCardNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�߼����鿨����������[size:%d]!", __FUNCTION__, pCardGroupCfg->size());
		return false;
	}

	// �����ܸ���
	int nTotalNormalWeight = 0;
	int nTotalMustWeight = 0;
	vector<HighCardGroup_Config>::const_iterator iter = pCardGroupCfg->begin();
	for (; iter != pCardGroupCfg->end(); ++iter)
	{
		nTotalNormalWeight += iter->nNormalWeight;
		nTotalMustWeight += iter->nMustWeight;
	}

	// ������׿���
	DWORD dwMustCardId = 0;
	{
		int nRandValue = g_Rand(0, nTotalMustWeight);
		int nCurWeight = 0;
		// �����Ǹ�����
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

	// �����ͨ����
	m_pZhanBuInfo->wHighCardGroupNum = 0;
	memset(m_pZhanBuInfo->arHighCardGroup, 0, sizeof(m_pZhanBuInfo->arHighCardGroup));
	for (int i = 0; i < nRandCardNum; ++i)
	{
		int nRandValue = g_Rand(0, nTotalNormalWeight);
		int nCurWeight = 0;

		// �����Ǹ�����
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

	// �����λ�ò��뱣�׿���
	if (m_pZhanBuInfo->wHighCardGroupNum)
	{
		m_pZhanBuInfo->arHighCardGroup[g_Rand(0, m_pZhanBuInfo->wHighCardGroupNum - 1)].dwCardId = dwMustCardId;
	}

	// �´����Ľ�ɫ����Ҫ����
	if (!bIsNewRole)
	{
		// ʹ������ˢ��
		if (nCostDiamond > 0)
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CARD_GROUP_REFRESH);
			m_pZhanBuInfo->wTodayHighCardGroupCostRefreshTimes++;
		}
		// ʹ�ö�ʱˢ��
		else
		{
			m_pZhanBuInfo->dwHighCardGroupLastRefreshTime = dwCurTime;
		}
	}

	return true;
}

// ��������֪ͨ
void CZhanBuManager::CardDataNotify(DWORD dwUID, BYTE byFlag)
{
	const Card *pCard = GetCard(dwUID);
	if (pCard)
		CardDataNotify(pCard, byFlag);
}

// ��������֪ͨ
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

// ȡ�ղ�������
CollectionBook* CZhanBuManager::GetCollectionBook(WORD wBookId)
{
	// �ҵ���ֱ�ӷ���
	for (WORD i = 0; i < m_pZhanBuInfo->wCollectionBookNum; ++i)
	{
		if (m_pZhanBuInfo->arCollectionBookList[i].wBookId == wBookId)
			return &m_pZhanBuInfo->arCollectionBookList[i];
	}

	if (m_pZhanBuInfo->wCollectionBookNum >= MAX_COLLECTION_BOOK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ղ����б�����[size:%d]!", __FUNCTION__, m_pZhanBuInfo->wCollectionBookNum);
		return NULL;
	}

	// �Ҳ��������
	CollectionBook tBook;
	memset(&tBook, 0, sizeof(tBook));
	tBook.wBookId = wBookId;
	m_pZhanBuInfo->arCollectionBookList[m_pZhanBuInfo->wCollectionBookNum++] = tBook;

	return &m_pZhanBuInfo->arCollectionBookList[m_pZhanBuInfo->wCollectionBookNum-1];
}

// ȡ�ղ�������
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

// ����ղ�������
void CZhanBuManager::AddCollectGroup(WORD wBookId, WORD wGroupId)
{
	CollectionBook *pBook = GetCollectionBook(wBookId);
	if (!pBook)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d]!", __FUNCTION__, wBookId);
		return;
	}

	if (pBook->wCollectionGroupNum >= MAX_COLLECTION_GROUP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ղ����б�����[BookId:%d,GroupId:%d,Num:%d]!",
			__FUNCTION__, wBookId, wGroupId, pBook->wCollectionGroupNum);
		return;
	}

	CollectionGroup tGroup;
	tGroup.wId = wGroupId;
	tGroup.wStar = 1;
	pBook->arCollectionGroupList[pBook->wCollectionGroupNum++] = tGroup;
}

// ɾ���ղ�������
void CZhanBuManager::RemoveCollectGroup(WORD wBookId, WORD wGroupId)
{
	CollectionBook *pBook = GetCollectionBook(wBookId);
	if (!pBook)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ղ�������[BookId:%d]!", __FUNCTION__, wBookId);
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

// ����Ƿ��ѳ�ʼ������
bool CZhanBuManager::IsAlreadyInitData()
{
	return m_pZhanBuInfo->wLowCardGroupMapIdx ? true : false;
}

// ��ʼ����
void CZhanBuManager::InitData()
{
	m_pZhanBuInfo->wLowCardGroupMapIdx = 1;	// �ؿ���1��ʼ
	m_pZhanBuInfo->wCurLowCardGroupHaveRefreshTimes = 1; // ��ʼ��ʱ��һ��
	m_pZhanBuInfo->dwLowCardGroupLastRecoverTime = time(NULL); // 

	DoLowCardGroupRefresh(true);
	DoHighCardGroupRefresh(true);
}

// ȡ��ǰ�ղ�ֵ
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
				// �����ղ�ֵ
				dwCollectionValue += pGroupCfg->nActivateCollectionValue; 

				// �����ղ�ֵ
				dwCollectionValue += (pGroupCfg->nUpgradeCollectionValue * (tGroup.wStar-1));
			}
		}
	}

	return dwCollectionValue;
}

// ȡ������������
WORD CZhanBuManager::GetBagEmptySize()
{
	WORD wBagSize = min(MAX_CARD_NUM, m_pPlayer->GetVipValue(32));
	return wBagSize > m_pZhanBuInfo->wCardNum ? wBagSize - m_pZhanBuInfo->wCardNum : 0;
}

// ������ʷ�ղ�ֵ
void CZhanBuManager::UpdateHistoryCollectionValue()
{
	DWORD dwCollectionValue = GetCurCollectionValue();
	if (dwCollectionValue > m_pZhanBuInfo->dwHistoryCollectionValue)
	{
		m_pZhanBuInfo->dwHistoryCollectionValue = dwCollectionValue;

		// ���ջ
		m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 31, dwCollectionValue, false);
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 31, dwCollectionValue, false);
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 31, dwCollectionValue, false);
		m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 31, dwCollectionValue, false);

		// �������а�
		CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, m_pPlayer, dwCollectionValue);
	}
}