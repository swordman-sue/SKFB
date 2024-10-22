#include "stdafx.h"
#include "SevenDayManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\PlayerManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\PlayerBaseDataManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\RedPoint\RedPointManager.h"
#include "..\MapServer.h"
#include "..\Shop\ShopManager.h"
#include "..\Activity\ActivityManager.h"


CSevenDayManager::CSevenDayManager()
{
	
}

CSevenDayManager::~CSevenDayManager()
{
	
}

void CSevenDayManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pActivityInfo = &pPlayer->GetRoleFullInfo().tActivityInfo;
}

// ����Ŀ���б�
void CSevenDayManager::OnNetTargetList()
{
	SendTargetList();
}

// ��ȡĿ�꽱��
void CSevenDayManager::OnNetTargetAward(WORD wTargetId)
{
	WORD wFromMsgId = C2S_SEVEN_DAY_GET_REWARD_REQ;

	// ȡĿ������
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	SevenDayTargetData *pTargetData = GetTargetData(wTargetId);
	if (!pTargetData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��ľ�д�Ŀ��,������ȡ����![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// ����Ŀ��
	if (pTargetCfg->byIsAsc)
	{
		if (pTargetData->dwValue < pTargetCfg->nX)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]��ľ����ɴ�Ŀ��,������ȡ����![TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}
	// ����Ŀ��
	else
	{
		if (pTargetData->dwValue > pTargetCfg->nX)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]��ľ����ɴ�Ŀ��,������ȡ����![TargetId:%d]", __FUNCTION__, wTargetId);
			return;
		}
	}

	if (pTargetData->byIsGetReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��Ŀ�꽱������ȡ��![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	m_pPlayer->AddObjectList(pTargetCfg->vRewardItemList, TRUE, FROM_SEVEN_DAY_TARGET_REWARD);

	pTargetData->byIsGetReward = true;

	// ��Ӧ
	MSG_S2C_SEVEN_DAY_GET_REWARD_RESP msg;
	msg.m_wTargetId = wTargetId;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ��ɶȽ��� 
void CSevenDayManager::OnNetCompletionReward(WORD wCompletionNum)
{
	WORD wFromMsgId = C2S_SEVEN_DAY_COMPLETION_REWARD_REQ;

	// ����Ƿ�Ϸ�����ɶȽ���
	const SevenDayCompletionReward_Config *pCompletionRewardCfg = g_pCfgMgr->GetSevenDayCompletionReward(wCompletionNum);
	if (!pCompletionRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7����ɶȽ�������![CompletionNum:%d]", __FUNCTION__, wCompletionNum);
		return;
	}

	// ����Ƿ����ȡ
	WORD wAlreadyNum = GetComplationNum();
	if (wAlreadyNum < wCompletionNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��ɶȻ�û�д��,������ȡ![CompletionNum:%d]", __FUNCTION__, wCompletionNum);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsComplationRewardRecord(wCompletionNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]7����ɶȽ�������ȡ��![CompletionNum:%d]", __FUNCTION__, wCompletionNum);
		return;
	}

	// �����ȡ��¼
	if (m_pActivityInfo->wCompletionRewardRecordNum < MAX_SEVEN_DAY_COMPLETION_REWARD)
		m_pActivityInfo->arCompletionRewardRecord[m_pActivityInfo->wCompletionRewardRecordNum++] = wCompletionNum;

	// ������
	m_pPlayer->AddObjectList(pCompletionRewardCfg->vRewardItemList, TRUE, FROM_SEVEN_DAY_COMPLETIO_REWARD);

	// ��Ӧ
	MSG_S2C_SEVEN_DAY_COMPLETION_REWARD_RESP msg;
	msg.m_wComplecton_num = wCompletionNum;
	m_pPlayer->SendPacket(&msg);
}

// ����Ŀ���б�
void CSevenDayManager::SendTargetList()
{
	MSG_S2C_SEVEN_DAY_TARGET_LIST_RESP msg;
	msg.m_byTargetNum = 0;
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		msg.m_arTargetList[msg.m_byTargetNum++] = m_pActivityInfo->arSevenDayTargetList[i];
	}
	msg.m_byRewardRecordNum = 0;
	for (WORD i = 0; i < m_pActivityInfo->wCompletionRewardRecordNum; ++i)
	{
		msg.m_arRewardRecordList[msg.m_byRewardRecordNum++] = m_pActivityInfo->arCompletionRewardRecord[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// ����Ŀ������
void CSevenDayManager::SendTargetData(const SevenDayTargetData *pData)
{
	MSG_S2C_SEVEN_DAY_TARGET_DATA_NOTICE msg;
	msg.m_tTargetData = *pData;
	m_pPlayer->SendPacket(&msg);
}

// ����Ŀ������
void CSevenDayManager::OnUpdateTarget(WORD wLogicType, DWORD dwX, bool bAdd)
{
	const vector<WORD> *pTargetList = g_pCfgMgr->GetTheSameLogicTargetList(wLogicType);
	if (!pTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������![LogicType:%d]", __FUNCTION__, wLogicType);
		return;
	}

	for (size_t i = 0; i < pTargetList->size(); ++i)
	{
		DoUpdateTarget(pTargetList->at(i), dwX, bAdd);
	}
}

// ����Ŀ������
void CSevenDayManager::OnUpdateTarget(WORD wLogicType)
{
	const vector<WORD> *pTargetList = g_pCfgMgr->GetTheSameLogicTargetList(wLogicType);
	if (!pTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������![LogicType:%d]", __FUNCTION__, wLogicType);
		return;
	}

	for (size_t i = 0; i < pTargetList->size(); ++i)
	{
		DoUpdateTarget(pTargetList->at(i));
	}
}

// �Ƿ��к��
bool CSevenDayManager::IsHaveRedPoint()
{
	WORD wActivityDays = GetActivityDay();

	// todo::��ʱ�⴦��д
	wActivityDays = min(wActivityDays, 7);

	// Ŀ�꽱��
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		const SevenDayTargetData *pTargetData = &m_pActivityInfo->arSevenDayTargetList[i];
		// ����ȡ
		if (pTargetData->byIsGetReward)
			continue;

		// ȡ�ɾ�����
		const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(pTargetData->wId);
		if (!pTargetCfg)
			continue;

		if ((pTargetCfg->byIsAsc && pTargetData->dwValue >= pTargetCfg->nX) ||	// ���������,���ֵ��Ч
			(!pTargetCfg->byIsAsc && pTargetData->dwValue <= pTargetCfg->nX))	// ����ǽ���,��Сֵ��Ч
		{
			// ����Ƿ�ǰ��֮ǰ������Ŀ��(����ǰ�Ƿ����ȥ��ȡ��Ŀ��)
			for (WORD k = 1; k <= wActivityDays; ++k)
			{
				if (IsTheDayTarget(pTargetData->wId, k))
					return true;
			}
		}
	}

	// ��ɶȽ���
//  	WORD wComplationNum = GetComplationNum();
// 	map<WORD, SevenDayCompletionReward_Config>::iterator iter = g_pCfgMgr->m_SevenDayCompletionRewardList.begin();
// 	for (; iter != g_pCfgMgr->m_SevenDayCompletionRewardList.end(); ++iter)
// 	{
// 		// �Ѿ����,��ľ����ȡ
// 		if (wComplationNum >= iter->second.wCompletionNum && !IsComplationRewardRecord(iter->first))
// 			return true;
// 	}


	// 7���̵�
	CShopManager *pShopMgr = m_pPlayer->GetShopManager();
	for (size_t i = 0; i < CConfigManager::Instance()->m_vSevenDayShopGoodsList.size(); ++i)
	{
		DWORD dwGoodsId = CConfigManager::Instance()->m_vSevenDayShopGoodsList[i];
		// �Ѿ�������
		if (pShopMgr->GetGoodsBuyRecordNum(SHOP_SEVEN_DAY, dwGoodsId))
			continue;

		// ����Ƿ�ǰ��֮ǰ������Ŀ��(����ǰ�Ƿ����ȥ��ȡ��Ŀ��)
		for (WORD k = 1; k <= wActivityDays; ++k)
		{
			const vector<DWORD> *pActivityList = g_pCfgMgr->GetServerDayActivity(k);
			if (!pActivityList)
				continue;

			vector<DWORD>::const_iterator iter = pActivityList->begin();
			for (; iter != pActivityList->end(); ++iter)
			{
				const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(*iter);
				if (pTargetCfg && pTargetCfg->wLogicType == 19 && pTargetCfg->nX == dwGoodsId)
					return true;
			}
		}
	}

	return false;
}

// �Ƿ��к��
bool CSevenDayManager::IsHaveRedPointEx()
{
	WORD wActivityDays = GetActivityDay();

	if (wActivityDays <= 7)
		return false;

	// todo::��ʱ�⴦��д
	wActivityDays = min(wActivityDays, 14);

	// Ŀ�꽱��
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		const SevenDayTargetData *pTargetData = &m_pActivityInfo->arSevenDayTargetList[i];
		// ����ȡ
		if (pTargetData->byIsGetReward)
			continue;

		// ȡ�ɾ�����
		const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(pTargetData->wId);
		if (!pTargetCfg)
			continue;

		if ((pTargetCfg->byIsAsc && pTargetData->dwValue >= pTargetCfg->nX) ||	// ���������,���ֵ��Ч
			(!pTargetCfg->byIsAsc && pTargetData->dwValue <= pTargetCfg->nX))	// ����ǽ���,��Сֵ��Ч
		{
			// ����Ƿ�ǰ��֮ǰ������Ŀ��(����ǰ�Ƿ����ȥ��ȡ��Ŀ��)
			for (WORD k = 8; k <= wActivityDays; ++k)
			{
				if (IsTheDayTarget(pTargetData->wId, k))
					return true;
			}
		}
	}

	// 7���̵�
	CShopManager *pShopMgr = m_pPlayer->GetShopManager();
	for (size_t i = 0; i < CConfigManager::Instance()->m_vSevenDayShopGoodsList.size(); ++i)
	{
		DWORD dwGoodsId = CConfigManager::Instance()->m_vSevenDayShopGoodsList[i];
		// �Ѿ�������
		if (pShopMgr->GetGoodsBuyRecordNum(SHOP_SEVEN_DAY, dwGoodsId))
			continue;

		// ����Ƿ�ǰ��֮ǰ������Ŀ��(����ǰ�Ƿ����ȥ��ȡ��Ŀ��)
		for (WORD k = 8; k <= wActivityDays; ++k)
		{
			const vector<DWORD> *pActivityList = g_pCfgMgr->GetServerDayActivity(k);
			if (!pActivityList)
				continue;

			vector<DWORD>::const_iterator iter = pActivityList->begin();
			for (; iter != pActivityList->end(); ++iter)
			{
				const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(*iter);
				if (pTargetCfg && pTargetCfg->wLogicType == 19 && pTargetCfg->nX == dwGoodsId)
					return true;
			}
		}
	}

	return false;
}

// ����Ŀ������
void CSevenDayManager::DoUpdateTarget(WORD wTargetId, DWORD dwValue, bool bAdd)
{
	// ȡĿ������
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������![TargetId:%d]", __FUNCTION__, wTargetId);
 		return;
 	}

	// ����Ƿ�ǰ�Ŀ��
	if (!IsCurActivityTarget(wTargetId))
		return;
 
 	//  ��ҳɾ�����
	SevenDayTargetData *pTargetData = GetTargetData(wTargetId);
	if (!pTargetData)
 	{
		SevenDayTargetData tData;
 		memset(&tData, 0, sizeof(tData));
		tData.wId = wTargetId;
 		AddTargetData(tData);
		pTargetData = GetTargetData(wTargetId);
 	}

	if (!pTargetData)
		return;

	// ����Ŀ��
	if (pTargetCfg->byIsAsc)
	{
		// �ɾ������
		if (pTargetData->dwValue >= pTargetCfg->nX)
			return;
	}
	// ����Ŀ��
	else
	{
		// �ɾ������
		if (pTargetData->dwValue && pTargetData->dwValue <= pTargetCfg->nX)
			return;
	}

	// ���ֵ
	if (bAdd)
	{
		pTargetData->dwValue += dwValue;
	}
	// ����ֵ
	else
	{		
		// ����Ŀ��
		if (pTargetCfg->byIsAsc)
		{
			// ʹ����Ѽ�¼
			if (dwValue <= pTargetData->dwValue)
				return;
		}
		// ����Ŀ��
		else
		{
			// ʹ����Ѽ�¼
			if (pTargetData->dwValue && dwValue >= pTargetData->dwValue)
				return;
		}

		pTargetData->dwValue = dwValue;

		// ����Ŀ��
		if (pTargetCfg->byIsAsc)
		{
			// ����Ŀ��ֵ
			pTargetData->dwValue = min(pTargetData->dwValue, pTargetCfg->nX);
		}
		// ����Ŀ��
		else
		{
			// ����Ŀ��ֵ
			pTargetData->dwValue = max(pTargetData->dwValue, pTargetCfg->nX);
		}
	}

	SendTargetData(pTargetData);
}

// ����Ŀ������
void CSevenDayManager::DoUpdateTarget(WORD wTargetId)
{
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������![TargetId:%d]", __FUNCTION__, wTargetId);
		return;
	}

	// ����Ƿ�ǰ�Ŀ��
	if (!IsCurActivityTarget(wTargetId))
		return;

	SevenDayTargetData *pTargetData = GetTargetData(wTargetId);

	// �����
	if (pTargetData && pTargetData->dwValue >= pTargetCfg->nX)
		return;

	WORD wNum = 0;

	switch (pTargetCfg->wLogicType)
	{
	// װ��ǿ��
	case 5:
	{
		CEquipmentManager *pEquipMgr = m_pPlayer->GetEquipmentManager();
		CHECK_POINTER(pEquipMgr);
		wNum = pEquipMgr->GetEquipmentNumByStrengthenLevel(pTargetCfg->nY);
	}
	break;
	// ʥ���ռ�
	case 7:
	{
		CTreasureManager *pTreasureMgr = m_pPlayer->GetTreasureManager();
		CHECK_POINTER(pTreasureMgr);
		wNum = pTreasureMgr->GetTreasureNumByQuality(pTargetCfg->nY);
	}
	break;
	// Ӣ��ͻ��
	case 9:
	{
		CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
		CHECK_POINTER(pHeroMgr);
		wNum = pHeroMgr->GetHeroNumByBreakLevel(pTargetCfg->nY);
	}
	break;
	// Ӣ��Ǳ�������ȼ�(����)
	case 11:
	{
		CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
		CHECK_POINTER(pHeroMgr);
		wNum = pHeroMgr->GetHeroNumByDestinyLevel(pTargetCfg->nY);
	}
	break;
	// Ӣ������
	case 15:
	{
		CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
		CHECK_POINTER(pHeroMgr);
		wNum = pHeroMgr->GetHeroNumByTrainLevel(pTargetCfg->nY);
	}
	break;
	// װ��Ʒ��
	case 13:
	{
		CEquipmentManager *pEquipMgr = m_pPlayer->GetEquipmentManager();
		CHECK_POINTER(pEquipMgr);
		wNum = pEquipMgr->GetEquipmentNumByQuality(pTargetCfg->nY);
	}
	break;
	// װ������
	case 14:
	{
		CEquipmentManager *pEquipMgr = m_pPlayer->GetEquipmentManager();
		CHECK_POINTER(pEquipMgr);
		wNum = pEquipMgr->GetEquipmentNumByRefineLevel(pTargetCfg->nY);
	}
	break;
	default:
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д����7��Ŀ���߼�����![TargetI:%d,LogicType:%d]",
			__FUNCTION__, wTargetId, pTargetCfg->wLogicType);
		return;
	}

	// һ����ľ�У����ҽ�������������
	if (!wNum)
		return;

	if (pTargetData)
	{
		pTargetData->dwValue = wNum;

		SendTargetData(pTargetData);
	}
	else
	{
		SevenDayTargetData tTargetData;
		memset(&tTargetData, 0, sizeof(tTargetData));
		tTargetData.wId = wTargetId;
		tTargetData.dwValue = wNum;
		AddTargetData(tTargetData);

		SendTargetData(&tTargetData);
	}
}

// ȡ�ɾ�����
SevenDayTargetData* CSevenDayManager::GetTargetData(WORD wTargetId)
{
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		if (m_pActivityInfo->arSevenDayTargetList[i].wId == wTargetId)
		{
			return &m_pActivityInfo->arSevenDayTargetList[i];
		}
	}
	return NULL;
}

// ��ӳɾ�����
void CSevenDayManager::AddTargetData(const SevenDayTargetData &tTargetData)
{
	if (m_pActivityInfo->wSevenDayTargetNum >= MAX_SEVEN_DAY_TARGET_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]7��Ŀ���б�����![ListNum:%d]", __FUNCTION__, m_pActivityInfo->wSevenDayTargetNum);
		return;
	}

	m_pActivityInfo->arSevenDayTargetList[m_pActivityInfo->wSevenDayTargetNum++] = tTargetData;
}

// ȡ�����
WORD CSevenDayManager::GetActivityDay()
{
	return g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());
}

// �Ƿ���յ�Ŀ��
bool CSevenDayManager::IsTheDayTarget(WORD wTargetId, WORD wActivityDays)
{
	const vector<DWORD> *pDayTargetList = g_pCfgMgr->GetServerDayActivity(wActivityDays);
	if (!pDayTargetList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7������������![Day:%d]", __FUNCTION__, wActivityDays);
		return false;
	}

	for (size_t i = 0; i < pDayTargetList->size(); ++i)
	{
		if (pDayTargetList->at(i) == wTargetId)
			return true;
	}

	return false;
}

// ȡ��ǰ��ɶ�
WORD CSevenDayManager::GetComplationNum()
{
	WORD wComplationNum = 0;
	for (WORD i = 0; i < m_pActivityInfo->wSevenDayTargetNum; ++i)
	{
		SevenDayTargetData *pTargetData = &m_pActivityInfo->arSevenDayTargetList[i];
		const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(pTargetData->wId);
		if (pTargetCfg && pTargetCfg->byIsCompletionTarget && pTargetData->byIsGetReward)
			wComplationNum++;
	}
	return wComplationNum;
}

// �Ƿ���ɶȽ�����¼
bool CSevenDayManager::IsComplationRewardRecord(WORD wComplationValue)
{
	for (WORD i = 0; i < m_pActivityInfo->wCompletionRewardRecordNum; ++i)
	{
		if (m_pActivityInfo->arCompletionRewardRecord[i] == wComplationValue)
			return true;
	}
	return false;
}

// �Ƿ�ǰ�Ŀ��
bool CSevenDayManager::IsCurActivityTarget(WORD wTargetId)
{
	// ȡĿ������
	const SevenDayTarget_Config *pTargetCfg = g_pCfgMgr->GetSevenDayTarget(wTargetId);
	if (!pTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���7��Ŀ����������![TargetId:%d]", __FUNCTION__, wTargetId);
		return false;
	}

	WORD wActivityDay = GetActivityDay();

	// ����Ƿ��ѽ��� todo::��ʱ�������� 
	if (pTargetCfg->wBelongActivityDay > 7)
	{ // 8-14��
		// ����Ƿ���
		if (!CActivityManager::IsActivityOpen(SYSTEM_SEVEN_DAY_2, m_pPlayer))
			return false;
	}
	else
	{ // 7��
		// ����Ƿ���
		if (!CActivityManager::IsActivityOpen(SYSTEM_SEVEN_DAY_1, m_pPlayer))
			return false;
	}

	// ���ǽ���Ŀ������
	if (pTargetCfg->byIsTheSameDayValid && !IsTheDayTarget(wTargetId, wActivityDay))
		return false;

	// �Ƿ�ӵ��쿪ʼ��Ч, �����,��������һ�쿪ʼ�Ż��Ч
	if (pTargetCfg->byIsFromTheSameDayValid && wActivityDay < pTargetCfg->wBelongActivityDay)
		return false;

	return true;
}