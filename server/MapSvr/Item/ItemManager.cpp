#include "stdafx.h"
#include "ItemManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Duobao\DuobaoManager.h"
#include "..\MapServer.h"


CItemManager::CItemManager()
{

}

CItemManager::~CItemManager()
{

}

void CItemManager::Init(Player *pPlayer)
{
	memset(&m_tFPItem, 0, sizeof(m_tFPItem));
	m_pPlayer = pPlayer;
	m_pPlayerItemInfo = &m_pPlayer->GetRoleFullInfo().tItemInfo;
}

// ������Ʒ�б�
void CItemManager::OnNetItemList()
{
	MSG_S2C_ITEM_LIST_RESP msg;
	msg.m_wItemNum = m_pPlayerItemInfo->wListNum;
	if (msg.m_wItemNum){
		memcpy_s(msg.m_ItemList, MAX_ITEM_NUM*sizeof(Item), m_pPlayerItemInfo->arItemList, m_pPlayerItemInfo->wListNum*sizeof(Item));
	}

	m_pPlayer->SendPacket(&msg);
}

// ����ʹ����Ʒ
void CItemManager::OnNetUseItem(DWORD dwItemId, WORD wUseNum, DWORD dwGiftItemId)
{
	WORD wFromMsgId = C2S_ITEM_USE_REQ;

	// ����Ƿ����㹻�����Ʒ
	if (GetItemNum(dwItemId) < wUseNum)
	{
		// ��Ʒ��������
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "ʹ�õ���Ʒ��������[ItemId:%d,ItemNum:%d]", dwItemId,wUseNum);
		return;
	}

	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(dwItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ItemId:%d]", __FUNCTION__, dwItemId);
		return;
	}

	// todo::���ʹ����Ʒ���������������Ƿ����㹻�Ŀռ�

	MSG_S2C_ITEM_USE_RESP msg;
	msg.m_dwItemId = dwItemId;
	msg.m_wUseNum = wUseNum;
	msg.m_byItemNum = 0;
	msg.m_byMoneyNum = 0;

	m_vCreateItemList.clear();
	m_vCreateMoneyList.clear();
	//vector<Item> vItemList;
	for (WORD i = 0; i < wUseNum; ++i)
	{
		if (!UseItem(dwItemId, 1, NULL, dwGiftItemId, true, FROM_USE_ITEM))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "ʹ����Ʒʧ��[ItemId:%d]", dwItemId);
			return;
		}
	}

	for (int i = 0; i < m_vCreateItemList.size() && msg.m_byItemNum < MAX_ITEM_NUM; ++i)
	{
		msg.m_ItemList[msg.m_byItemNum++] = m_vCreateItemList[i];
	}

	for (int i = 0; i < m_vCreateMoneyList.size() && msg.m_byMoneyNum < MAX_MONEY_TYPE_NUM; ++i)
	{
		msg.m_MoneyList[msg.m_byMoneyNum++] = m_vCreateMoneyList[i];
	}

	RemoveItem(dwItemId, wUseNum, TRUE, FROM_USE_ITEM);

	m_pPlayer->SendPacket(&msg);

	// ��ɫ��Ϣ����
	m_pPlayer->RoleInfoUpdate();
}

// ����ʹ�����
void CItemManager::OnNetFP(BYTE byFBType)
{
	WORD wFromMsgId = C2S_ITEM_FP_REQ;

	if (!m_tFPItem.dwId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��ǰû�пɷ��Ƶ���Ʒ[type:%d]", __FUNCTION__, byFBType);
		return;
	}

	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(m_tFPItem.dwId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ItemId:%d]", __FUNCTION__, m_tFPItem.dwId);
		return;
	}

	// �Ƿ�����Ʒ
// 	if (ITEM_TYPE_GIFT != pItemCfg->byType || GIFT_TYPE_RAND != pItemCfg->bySubType)
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]���Ƿ�����Ʒ[ItemId:%d,Type:%d,SubType:%d]", __FUNCTION__,
// 			m_tFPItem.dwId, pItemCfg->byType, pItemCfg->bySubType);
// 		return;
// 	}

//	m_vCreateItemList.clear();
//	m_vCreateMoneyList.clear();
	//vector<Item> vItemList;
// 	if (!UseItem(m_dwFPItemId,1, NULL, 0, true))
// 	{
// 		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
// 		MAPLOG(ERROR_LEVEL, "[%s]ʹ�÷�����Ʒʧ��[ItemId:%d]", __FUNCTION__, m_dwFPItemId);
// 		return;
// 	}

	vector<Item> vItemList;
	if (!UseRandItem(m_tFPItem.dwId, m_tFPItem.dwNum, vItemList))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ʹ�÷�����Ʒʧ��[ItemId:%d,ItemNum:%d]",
			__FUNCTION__, m_tFPItem.dwId, m_tFPItem.dwNum);
		return;
	}

	// �����Ʒ
	m_pPlayer->AddObjectList(vItemList, TRUE, FROM_FP_REWARD);

	// һ���ǻ�ֻ���һ��
// 	if (1 != m_vCreateItemList.size())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]ʹ�÷�����Ʒ��������Ʒ����һ��[ItemId:%d]", __FUNCTION__, m_dwFPItemId);
// 		for (size_t i = 0; i < m_vCreateItemList.size(); ++i)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s][ItemId:%d]", __FUNCTION__, m_vCreateItemList[i]);
// 		}
// 		return;
// 	}

	// ��Ϊ�淨�ϱ�֤һ����ֻ��һ����Ʒ���߻���,���Կ�����ô��
	Item tItem;
	memset(&tItem, 0, sizeof(tItem));
	for (size_t i = 0; i < vItemList.size(); i++)
	{
		Item &tmp = vItemList[i];
		if (tItem.dwId && tItem.dwId != tmp.dwId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���Ʋ�������Ʒ��ֻһ������[FPItem:%d]", __FUNCTION__, m_tFPItem.dwId);
			continue;
		}

		tItem.dwId = tmp.dwId;
		tItem.dwNum += tmp.dwNum;
	}

	MSG_S2C_ITEM_FP_RESP msg;
	msg.m_byType = byFBType;
	msg.m_dwFPItemId = m_tFPItem.dwId;
	memset(&msg.m_tMoneyData, 0, sizeof(msg.m_tMoneyData));
	msg.m_tCreateItem = tItem;
	m_pPlayer->SendPacket(&msg);

	// �����ѷ��Ƶ���Ʒ
	//m_dwFPItemId = 0;

	m_tFPItem.dwId = 0;
	m_tFPItem.dwNum = 0;

	// ��ɫ��Ϣ����
	m_pPlayer->RoleInfoUpdate();
}

// ��Ʒ�ϳ�
void CItemManager::OnNetCompose(WORD wItemId)
{
	WORD wFromMsgId = C2S_ITEM_COMPOSE_REQ;

	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(wItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ѵ�����������[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	// ������ĵ���Ʒ
	for (size_t i = 0; i < pItemCfg->vComposeCostItemList.size(); ++i)
	{
		const Item *pItem = &pItemCfg->vComposeCostItemList[i];
		if (GetItemNum(pItem->dwId) < pItem->dwNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]���ѵ��ߺϳ����ĵ���Ʒ����[ItemId:%d]", __FUNCTION__, pItem->dwId);
			return;
		}
	}

	// ɾ��������Ʒ
	for (size_t i = 0; i < pItemCfg->vComposeCostItemList.size(); ++i)
	{
		const Item *pItem = &pItemCfg->vComposeCostItemList[i];
		RemoveItem(pItem->dwId, pItem->dwNum, TRUE, FROM_ITEM_COMPOSE);
	}

	// ��Ӻϳ���Ʒ
	AddItem(wItemId, 1, TRUE, FROM_ITEM_COMPOSE);

	MSG_S2C_ITEM_COMPOSE_RESP msg;
	msg.m_wItemId = wItemId;
	m_pPlayer->SendPacket(&msg);

	// ��ɫ��Ϣ����
	m_pPlayer->RoleInfoUpdate();
}

// ��Ʒ�ֽ�
void CItemManager::OnNetDecompose(WORD wItemId, WORD wItemNum)
{
	WORD wFromMsgId = C2S_ITEM_DECOMPOSE_REQ;

	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(wItemId);
	if (!pItemCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ѵ�����������[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	if (GetItemNum(wItemId) < wItemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�ֽ���ѵ�����������[ItemId:%d]", __FUNCTION__, wItemId);
		return;
	}

	RemoveItem(wItemId, wItemNum, TRUE, FROM_ITEM_DECOMPOSE);

	m_pPlayer->ModifyProperty(ROLE_PRO_GOD_SOUL, pItemCfg->nDecomposeGodSoul * wItemNum, TRUE, FROM_ITEM_DECOMPOSE);

	MSG_S2C_ITEM_DECOMPOSE_RESP msg;
	msg.m_wItemId = wItemId;
	msg.m_wItemNum = wItemNum;
	m_pPlayer->SendPacket(&msg);

	// ��ɫ��Ϣ����
	m_pPlayer->RoleInfoUpdate();
}

DWORD CItemManager::GetItemNum(DWORD dwItemId)
{
	Item *pItem = FindItem(dwItemId);
	return pItem ? pItem->dwNum : 0;
}

const Item* CItemManager::GetItem(DWORD dwItemId)
{
	return FindItem(dwItemId);
}

// �����Ʒ
bool CItemManager::AddItem(DWORD dwItemId, DWORD dwNum, BOOL bNotify, WORD wFromId)
{
	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(dwItemId);
	if (!pItemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[%d]", __FUNCTION__,dwItemId);
		return false;
	}

	if (pItemCfg->byIsUseInGet)
	{
		//vector<Item> vAddItemList;
		if (!UseItem(dwItemId, dwNum, NULL, 0, false, wFromId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����Ʒʱֱ��ʹ�ó���![ItemId:%d]", __FUNCTION__, dwItemId);
			return false;
		}
	}
	else
	{
		Item *pItem = FindItem(dwItemId);
		if (pItem)
		{
			pItem->dwNum += dwNum;
		}
		else
		{
			// �б�����
			if (m_pPlayerItemInfo->wListNum >= MAX_ITEM_NUM)
			{
				MAPLOG(ERROR_LEVEL, "�����Ʒ,�б�����[%d]", m_pPlayerItemInfo->wListNum);
				return false;
			}

			pItem = &m_pPlayerItemInfo->arItemList[m_pPlayerItemInfo->wListNum++];
			pItem->dwId = dwItemId;
			pItem->dwNum = dwNum;
		}

		if (bNotify)
		{
			MSG_S2C_ITEM_DATA_NOTIFY msg;
			msg.m_dwItemId = dwItemId;
			msg.m_dwUseNum = dwNum;
			msg.m_byFlag = true;
			m_pPlayer->SendPacket(&msg);
		}

		// ��¼���
		g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwItemId, dwNum, true, wFromId);
	}

	return true;
}

// �Ƴ���Ʒ
DWORD CItemManager::RemoveItem(DWORD dwItemId, DWORD dwNum, BOOL bNotify, WORD wFromId)
{
	Item *pItem;
	DWORD dwRemoveNum = 0;
	for (WORD i = 0; i < m_pPlayerItemInfo->wListNum; ++i)
	{
		pItem = &m_pPlayerItemInfo->arItemList[i];
		if (pItem->dwId == dwItemId)
		{
			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwItemId, dwNum, false, wFromId);

			if (pItem->dwNum > dwNum)
			{
				pItem->dwNum -= dwNum;
				dwRemoveNum = dwNum;
			}
			else
			{
				dwRemoveNum = pItem->dwNum;
				*pItem = m_pPlayerItemInfo->arItemList[--m_pPlayerItemInfo->wListNum];
			}
			break;
		}
	}

	if (bNotify && dwRemoveNum)
	{
		MSG_S2C_ITEM_DATA_NOTIFY msg;
		msg.m_dwItemId = dwItemId;
		msg.m_dwUseNum = dwRemoveNum;
		msg.m_byFlag = false;
		m_pPlayer->SendPacket(&msg);
	}

	return dwRemoveNum;
}

// ʹ����Ʒ
bool CItemManager::UseItem(DWORD dwItemId, DWORD dwItemNum, vector<Item> *pItemList, DWORD dwGiftItemId, bool bAddToCreateList, WORD wFromId)
{
	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(dwItemId);
	if (!pItemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ItemId:%d]", __FUNCTION__, dwItemId);
		return false;
	}

	if (pItemCfg->byType == ITEM_TYPE_NORMAL)
	{
		switch (pItemCfg->bySubType)
		{
		// ����ҩ��
		case ITEM_TYPE_1:
		{
			int nValue = pItemCfg->nParam1 * dwItemNum;
			m_pPlayer->ModifyProperty(ROLE_PRO_STAMINA, nValue);
			AddCreateMoney(ROLE_PRO_STAMINA, nValue);
		}
		break;
		// ����ҩ��
		case ITEM_TYPE_2: 
		{
			int nValue = pItemCfg->nParam1 * dwItemNum;	
			m_pPlayer->ModifyProperty(ROLE_PRO_ENERGY, nValue);
			AddCreateMoney(ROLE_PRO_ENERGY, nValue);
		}
		break;
		// �ᱦ��ս��Ʒ
		case ITEM_TYPE_14:
		{
			m_pPlayer->GetDuobaoManager()->SetPeaceOverTime(time(NULL) + pItemCfg->nParam1 * 60 * 60);
		}
		break;
		// ������
		case ITEM_TYPE_15:
		{
			m_pPlayer->ModifyProperty(ROLE_PRO_WANTED, pItemCfg->nParam1);
			AddCreateMoney(ROLE_PRO_WANTED, pItemCfg->nParam1);
		}
		break;
		// ��������
		case ITEM_TYPE_20:
		{
			m_pPlayer->ModifyProperty(pItemCfg->nParam1, pItemCfg->nParam2 * dwItemNum, TRUE, FROM_USE_ITEM);
			AddCreateMoney(pItemCfg->nParam1, pItemCfg->nParam2 * dwItemNum);
		}
		break;
		default:
		{
			MAPLOG(ERROR_LEVEL, "[%s]�޷�ʶ�����Ʒ������[ItemId:%d,type:%d,subtype:%d]", 
				__FUNCTION__, dwItemId, pItemCfg->byType, pItemCfg->bySubType);
			return false;
		}
		break;
		}
	}
	else if (pItemCfg->byType == ITEM_TYPE_GIFT)
	{
		// ������
		if (GIFT_TYPE_RAND == pItemCfg->bySubType)
		{
			for (DWORD i = 0; i < dwItemNum; ++i)
			{
				DWORD dwTotalRate = pItemCfg->GetTotalRate();
				DWORD dwRandValue = Rand::Instance()->irand(0, dwTotalRate);
				const GiftRandItem *pGiftItem = pItemCfg->GetGiftItemByRate(dwRandValue);
				if (!pGiftItem)
				{
					MAPLOG(ERROR_LEVEL, "[%s]���ó���!!!�Ҳ������ʶ�Ӧ�������Ʒ����[ItemId:%d,Rate:%d]",
						__FUNCTION__, dwItemId, dwRandValue);
					return false;
				}

				WORD wRandItemNum = Rand::Instance()->irand(pGiftItem->wNumMin, pGiftItem->wNumMax);

				m_pPlayer->AddObject(pGiftItem->dwItemId, wRandItemNum, TRUE, wFromId);

				//if (bAddToCreateList)
				AddCreateItem(pGiftItem->dwItemId, wRandItemNum);

				if (pItemList)
					pItemList->push_back(Item(pGiftItem->dwItemId, wRandItemNum));

				MAPLOG(GUI_LEVEL, "�����������Ʒ[ItemId:%d,ItemNum:%d]", pGiftItem->dwItemId, wRandItemNum);
			}
		}
		// ��ѡһ
		else if (GIFT_TYPE_MULTIPLE == pItemCfg->bySubType)
		{
			const GiftRandItem *pGiftItem = pItemCfg->GetGiftItemId(dwGiftItemId);
			if (!pGiftItem)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������Ʒ����[ItemId:%d,GiftItemId:%d]",__FUNCTION__, dwItemId, dwGiftItemId);
				return false;
			}
			
			DWORD dwAddNum = pGiftItem->wNumMin * dwItemNum;

			m_pPlayer->AddObject(pGiftItem->dwItemId, dwAddNum, TRUE, wFromId);

			//if (bAddToCreateList)
			AddCreateItem(pGiftItem->dwItemId, dwAddNum);

			if (pItemList)
				pItemList->push_back(Item(pGiftItem->dwItemId, dwAddNum));
		}
		// �̶���Ʒ���
		else if (GIFT_TYPE_STATIC == pItemCfg->bySubType)
		{
			for (int i = 0; i < pItemCfg->vGiftItemList.size(); ++i)
			{
				const GiftRandItem &sItem = pItemCfg->vGiftItemList[i];
				m_pPlayer->AddObject(sItem.dwItemId, sItem.wNumMin, TRUE, wFromId);

				//if (bAddToCreateList)
				AddCreateItem(sItem.dwItemId, sItem.wNumMin);

				if (pItemList)
					pItemList->push_back(Item(sItem.dwItemId, sItem.wNumMin));
			}
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]�޷�ʶ������������[ItemId:%d,type:%d,subtype:%d]",
				__FUNCTION__, dwItemId, pItemCfg->byType, pItemCfg->bySubType);
			return false;
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]�޷�ʶ�����Ʒ����[ItemId:%d,type:%d]", 
			__FUNCTION__, dwItemId, pItemCfg->byType);
		return false;
	}

	return true;
}

// ʹ�������Ʒ
bool CItemManager::UseRandItem(DWORD dwItemId, DWORD dwItemNum, vector<Item> &vItemList)
{
	const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(dwItemId);
	if (!pItemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ItemId:%d]", __FUNCTION__, dwItemId);
		return false;
	}

	// ���ǻ��ʹ�õ���Ʒ
	if (!pItemCfg->byIsUseInGet)
	{
		vItemList.push_back(Item(dwItemId, dwItemNum));
		return true;
	}

	// ������Ʒ
 	if (ITEM_TYPE_NORMAL == pItemCfg->byType)
 	{
 		vItemList.push_back(Item(dwItemId, dwItemNum));
 		return true;
 	}
 
 	// ��ѡһ���
 	if (ITEM_TYPE_GIFT == pItemCfg->byType && GIFT_TYPE_MULTIPLE == pItemCfg->bySubType)
 	{
 		vItemList.push_back(Item(dwItemId, dwItemNum));
 		return true;
 	}

	for (DWORD i = 0; i < dwItemNum; ++i)
	{
		DWORD dwTotalRate = pItemCfg->GetTotalRate();
		DWORD dwRandValue = Rand::Instance()->irand(0, dwTotalRate);
		const GiftRandItem *pGiftItem = pItemCfg->GetGiftItemByRate(dwRandValue);
		if (!pGiftItem)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ó���!!!�Ҳ������ʶ�Ӧ�������Ʒ����[ItemId:%d,Rate:%d]",
				__FUNCTION__, dwItemId, dwRandValue);
			continue;
		}
		WORD wRandItemNum = Rand::Instance()->irand(pGiftItem->wNumMin, pGiftItem->wNumMax);

// 		const Item_Config *pRandItemCfg = CConfigManager::Instance()->GetItem(pGiftItem->dwItemId);
// 		if (!pRandItemCfg)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ItemId:%d]", __FUNCTION__, pGiftItem->dwItemId);
// 			continue;
// 		}

		// ֻȡ�ڶ�����Ʒ
		vItemList.push_back(Item(pGiftItem->dwItemId, wRandItemNum));
	}

	return true;
}

// ɾ��������Ʒ
void CItemManager::DeleteTimeOutItem()
{
	Item arItemList[MAX_ITEM_NUM];
	WORD wListNum = 0;		
	DWORD dwCurDate = g_Time2Date(time(NULL));
	DWORD dwOpenSvrDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());
	vector<Item> vExpiryDateItemList;
	
	// ��������Щ��Ʒ�ѹ���
	for (WORD i = 0; i < m_pPlayerItemInfo->wListNum; ++i)
	{
		Item &tItem = m_pPlayerItemInfo->arItemList[i];
		const Item_Config *pItemCfg = g_pCfgMgr->GetItem(tItem.dwId);
		if (!pItemCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ�������ļ�[ItemId:%d]", __FUNCTION__, tItem.dwId);
			continue;
		}

		// ������Ч����
		if (pItemCfg->dwExpiryDate > MIN_DATE && dwCurDate > pItemCfg->dwExpiryDate)
		{
			vExpiryDateItemList.push_back(tItem);
		}
		// ����������Ч����
		else if (pItemCfg->dwExpiryDate && pItemCfg->dwExpiryDate < MIN_DATE && dwOpenSvrDays > pItemCfg->dwExpiryDate)
		{
			vExpiryDateItemList.push_back(tItem);
		}
	}

	// ɾ���ѹ��ڵ���Ʒ
	for (vector<Item>::iterator iter = vExpiryDateItemList.begin(); iter != vExpiryDateItemList.end(); ++iter)
	{
		RemoveItem(iter->dwId, iter->dwNum, TRUE, FROM_DELETE_TIMEOUT_ITEM);
	}
}

Item* CItemManager::FindItem(DWORD dwItemId)
{
	for (WORD i = 0; i < m_pPlayerItemInfo->wListNum; ++i)
	{
		if (m_pPlayerItemInfo->arItemList[i].dwId == dwItemId)
		{
			return &m_pPlayerItemInfo->arItemList[i];
		}
	}
	return NULL;
}

// ���ʹ����Ʒ��������Ʒ
void CItemManager::AddCreateItem(DWORD dwItemId, DWORD dwItemNum)
{
	// �������Ʒ
	if (OBJECT_ITEM == GetObejctTypeById(dwItemId))
	{
		const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(dwItemId);
		if (!pItemCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ItemId:%d]", __FUNCTION__, dwItemId);
			return;
		}

		// ����ǻ��ʹ�õ���Ʒ
		if (pItemCfg->byIsUseInGet)
			return;
	}

	// �������
	for (size_t i = 0; i < m_vCreateItemList.size(); ++i)
	{
		if (m_vCreateItemList[i].dwId == dwItemId)
		{
			m_vCreateItemList[i].dwNum += dwItemNum;
			return;
		}
	}

	// �������
	m_vCreateItemList.push_back(Item(dwItemId, dwItemNum));
}

// ���ʹ�ò����Ļ���
void CItemManager::AddCreateMoney(BYTE byType, int nValue)
{
	// �������
	for (size_t i = 0; i < m_vCreateMoneyList.size(); ++i)
	{
		if (m_vCreateMoneyList[i].byType == byType)
		{
			m_vCreateMoneyList[i].nValue += nValue;
			return;
		}
	}

	// �������
	MoneyData tData;
	tData.byType = byType;
	tData.nValue = nValue;
	m_vCreateMoneyList.push_back(tData);
}