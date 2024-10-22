#include "stdafx.h"
#include "RecyclingManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "..\Hero\HeroManager.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\ZhanBu\ZhanBuManager.h"
#include "..\MapServer.h"


CRecyclingManager::CRecyclingManager()
{

}

CRecyclingManager::~CRecyclingManager()
{

}

// ��ʼ��
void CRecyclingManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
}

// ����ֽⷵ��
void CRecyclingManager::OnNetDecomposeReturn(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum)
{
	WORD wFromMsgId = C2S_RECYCLE_DECOMPOSE_RETURN_REQ;

	int nGold = 0;
	//int nDiamond = 0;
	int nTowerPrestige = 0;
	int nHeroSoul = 0;
	int nGodSoul = 0;
	vector<Item> vReturnObjectList;
	vector<MoneyData> vReturnMoneyList;
	//vector<CostData> vCostCurrencyList;


	// װ��
 	if (OBJECT_EQUIPMENT == byObjType)
 	{
		for (BYTE i = 0; i < byTargetNum; ++i)
		{
			if (!GetEquipmentRecycleReturn(pTargetUIdList[i], vReturnObjectList, nGold, nTowerPrestige))
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]����װ��������Ʒ����", __FUNCTION__);
				return;
			}
		}
 	}
 	// Ӣ��
 	else if (OBJECT_HERO == byObjType)
 	{
 		for (BYTE i = 0; i < byTargetNum; ++i)
 		{
			if (!GetHeroDecomposeReturn(pTargetUIdList[i], vReturnObjectList, vReturnMoneyList, nGold, nHeroSoul, nGodSoul))
 			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]����Ӣ�۷�����Ʒ����", __FUNCTION__);
 				return;
 			}
 		}
 	}
	// ����
	else if (OBJECT_CARD == byObjType)
	{
 		for (BYTE i = 0; i < byTargetNum; ++i)
 		{
 			if (!GetCardRecycleReturn(pTargetUIdList[i], vReturnObjectList, vReturnMoneyList))
 			{
 				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
 				MAPLOG(ERROR_LEVEL, "[%s]�����Ʒ�����Ʒ����", __FUNCTION__);
 				return;
 			}
 		}
	}
 	else
 	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
 		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ķֽⷵ������[ObjType:%d]", __FUNCTION__, byObjType);
 		return;
 	}

	MSG_S2C_RECYCLE_DECOMPOSE_RETURN_RESP msg;
	msg.m_byObjType = byObjType;
	msg.m_byObjectNum = 0;
	msg.m_byMoneyNum = 0;
	for (vector<Item>::iterator iter = vReturnObjectList.begin(); iter != vReturnObjectList.end(); ++iter)
	{
		if (msg.m_byObjectNum >= MAX_RECYCLE_RETURN_OBJ_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�ֽⷵ��������������ָ������[num:%d]", __FUNCTION__, msg.m_byObjectNum);
			break;
		}
		msg.m_ReturnObjectList[msg.m_byObjectNum++] = *iter;
	}

	// todo::��ʱ��������
	if (nGold)
	{
		msg.m_arMoneyList[msg.m_byMoneyNum++] = MoneyData(ROLE_PRO_GOLD, nGold);
	}
	if (nTowerPrestige)
	{
		msg.m_arMoneyList[msg.m_byMoneyNum++] = MoneyData(ROLE_PRO_TOWER_PRESTIGE, nTowerPrestige);
	}
	if (nHeroSoul)
	{
		msg.m_arMoneyList[msg.m_byMoneyNum++] = MoneyData(ROLE_PRO_HERO_SOUL, nHeroSoul);
	}
	if (nGodSoul)
	{
		msg.m_arMoneyList[msg.m_byMoneyNum++] = MoneyData(ROLE_PRO_GOD_SOUL, nGodSoul);
	}
	// ������ͬ�������͵���
	vector<MoneyData>::iterator iter = vReturnMoneyList.begin();
	for (; iter != vReturnMoneyList.end(); ++iter)
	{
		bool bIsNewData = true;
		for (BYTE i = 0; i < msg.m_byMoneyNum; i++)
		{
			if (msg.m_arMoneyList[i].byType == iter->byType)
			{
				msg.m_arMoneyList[i].nValue += iter->nValue;
				bIsNewData = false;
				break;
			}
		}

		if (bIsNewData && msg.m_byMoneyNum < MAX_MONEY_TYPE_NUM)
		{
			msg.m_arMoneyList[msg.m_byMoneyNum++] = *iter;
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// ������������
void CRecyclingManager::OnNetResetReturn(BYTE byObjType, DWORD dwTargetUId)
{
	WORD wFromMsgId = C2S_RECYCLE_RESET_RETURN_REQ;

	int nGold = 0;
	int nDiamond = 0;
	int nContext = 0;
	vector<Item> vReturnObjectList;		// ͨ�÷���
	//vector<Item> vComposeReturnList;	// �ϳɷ���
	vector<MoneyData> vMoneyReturnList;
	//vector<CostData> vCostCurrencyList;

	// װ��
	if (OBJECT_EQUIPMENT == byObjType)
	{
		if (!GetEquipmentRecycleReturn(dwTargetUId, vReturnObjectList, nGold, nContext, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]����װ��������Ʒ����", __FUNCTION__);
			return;
		}
	}
	// Ӣ��
	else if (OBJECT_HERO == byObjType)
	{
		if (!GetHeroResetReturn(dwTargetUId, vReturnObjectList, vMoneyReturnList, nGold))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]����Ӣ�۷�����Ʒ����", __FUNCTION__);
			return;
		}
	}
	// ����
	else if (OBJECT_TREASURE == byObjType)
	{
		if (!GetTreasureRecycleReturn(dwTargetUId, vReturnObjectList, nGold, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�����ﷵ����Ʒ����", __FUNCTION__);
			return;
		}
	}
	// ����
	else if(OBJECT_CARD == byObjType)
	{
		if (!GetCardRecycleReturn(dwTargetUId, vReturnObjectList, vMoneyReturnList, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]�����Ʒ�����Ʒ����", __FUNCTION__);
			return;
		}
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д����������������[ObjType:%d]", __FUNCTION__, byObjType);
		return;
	}

	MoneyData tMoneyData;

	MSG_S2C_RECYCLE_RESET_RETURN_RESP msg;
	msg.m_byObjType = byObjType;
	msg.m_tMoney.nValue = nGold;      // (todo::�Ѿ�����, ����Ϊ�˷�ֹ�ɰ汾����,��ʱ����)
	msg.m_tMoney.byType = ROLE_PRO_GOLD;  // (todo::�Ѿ�����, ����Ϊ�˷�ֹ�ɰ汾����,��ʱ����)
	msg.m_wMoneyNum = 0;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = nGold;
	msg.m_arMoneyList[msg.m_wMoneyNum++] = tMoneyData;
	// todo::Ӣ������������ʯĿǰ��û�����ͻ���,�ȿͻ��˵������ٴ���
	msg.m_byObjectNum = 0;
	for (vector<Item>::iterator iter = vReturnObjectList.begin(); iter != vReturnObjectList.end(); ++iter)
	{
		if (msg.m_byObjectNum >= MAX_RECYCLE_RETURN_OBJ_NUM)
			break;
		msg.m_ReturnObjectList[msg.m_byObjectNum++] = *iter;
	}
	for (int i = 0; i < vMoneyReturnList.size(); ++i)
	{
		if (msg.m_wMoneyNum >= MAX_MONEY_TYPE_NUM)
			break;

		tMoneyData = vMoneyReturnList[i];

		bool bIsNewData = true;
		for (WORD i = 0; i < msg.m_wMoneyNum; i++)
		{
			if (msg.m_arMoneyList[i].byType == tMoneyData.byType)
			{
				msg.m_arMoneyList[i].nValue += tMoneyData.nValue;
				bIsNewData = false;
				break;
			}
		}

		if (bIsNewData)
			msg.m_arMoneyList[msg.m_wMoneyNum++] = tMoneyData;
	}
	m_pPlayer->SendPacket(&msg);
}

// �ֽ����(�佫,װ��)
void CRecyclingManager::OnNetRecycleDecompose(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum)
{
	bool bRet = false;

	// װ��
	if (OBJECT_EQUIPMENT == byObjType)
	{
		bRet = EquipmentDecompose(pTargetUIdList, byTargetNum);
	}
	// Ӣ��
	else if (OBJECT_HERO == byObjType)
	{
		bRet = HeroDecompose(pTargetUIdList, byTargetNum);
	}
	// ����
	else if (OBJECT_CARD == byObjType)
	{
		bRet = CardDecompose(pTargetUIdList, byTargetNum);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_RECYCLE_DECOMPOSE_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ķֽ��������[ObjType:%d]", __FUNCTION__, byObjType);
		return;
	}

	if (bRet)
	{
		MSG_S2C_RECYCLE_DECOMPOSE_RESP msg;
		msg.m_byObjType = byObjType;
		msg.m_byTargetNum = byTargetNum;
		int nCopySize = byTargetNum * sizeof(DWORD);
		memcpy_s(msg.m_TargetList, nCopySize, pTargetUIdList, nCopySize);
		m_pPlayer->SendPacket(&msg);
	}
}

// ��������(�佫,װ��,����)
void CRecyclingManager::OnNetRecycleReset(BYTE byObjType, DWORD dwTargetUId)
{
	bool bRet = false;

	// װ��
	if (OBJECT_EQUIPMENT == byObjType)
	{
		bRet = EquipmentReset(dwTargetUId);
	}
	// Ӣ��
	else if (OBJECT_HERO == byObjType)
	{
		bRet = HeroReset(dwTargetUId);
	}
	// ����
	else if (OBJECT_TREASURE == byObjType)
	{
		bRet = TreasureReset(dwTargetUId);
	}
	// ����
	else if (OBJECT_CARD == byObjType)
	{
		bRet = CardReset(dwTargetUId);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_RECYCLE_RESET_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д����������������[ObjType:%d]", __FUNCTION__,byObjType);
		return;
	}

	if (bRet)
	{
		MSG_S2C_RECYCLE_RESET_RESP msg;
		msg.m_byObjType = byObjType;
		msg.m_dwTargetUId = dwTargetUId;
		m_pPlayer->SendPacket(&msg);
	}
}

// �佫�ֽ�
bool CRecyclingManager::HeroDecompose(DWORD *pHeroList, BYTE byHeroNum)
{
	if (!pHeroList || !byHeroNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ�۷ֽ�����Ƿ�!", __FUNCTION__);
		return false;
	}

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	vector<MoneyData> vCostCurrencyList;
	vector<Item> vReturnObjectList;
	int nGold = 0;
	int nHeroSoul = 0;
	int nGodSoul = 0;
	for (BYTE i = 0; i < byHeroNum; ++i)
	{
		DWORD dwHeroId = pHeroList[i];
		if (!GetHeroDecomposeReturn(dwHeroId, vReturnObjectList, vCostCurrencyList, nGold, nHeroSoul, nGodSoul))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡӢ�۷�������ʧ��![HeroId:%d]", __FUNCTION__, dwHeroId);
			return false;
		}
	}

	// todo::��ⱳ���Ƿ�����

	// ɾ���ֽ�Ӣ��
	pHeroMgr->RemoveHero(pHeroList, WORD(byHeroNum), FROM_HERO_DECOMPOSE);

	// ������Ʒ 
	if (nGold)
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, FROM_HERO_DECOMPOSE);
	if (nHeroSoul)
		m_pPlayer->ModifyProperty(ROLE_PRO_HERO_SOUL, nHeroSoul, TRUE, FROM_HERO_DECOMPOSE);
	if (nGodSoul)
		m_pPlayer->ModifyProperty(ROLE_PRO_GOD_SOUL, nGodSoul, TRUE, FROM_HERO_DECOMPOSE);
	for (int i = 0; i < vReturnObjectList.size(); ++i)
	{
		m_pPlayer->AddObject(vReturnObjectList[i].dwId, vReturnObjectList[i].dwNum, TRUE, FROM_HERO_DECOMPOSE);
	}
	for (int i = 0; i < vCostCurrencyList.size(); ++i)
	{
		m_pPlayer->ModifyProperty(vCostCurrencyList[i].byType, vCostCurrencyList[i].nValue, TRUE, FROM_HERO_DECOMPOSE);
	}

	return true;
}

// �佫����
bool CRecyclingManager::HeroReset(DWORD dwHeroId)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	const HeroInfo *pHeroData = pHeroMgr->GetHero(dwHeroId);
	if (!pHeroData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д�Ӣ��[HeroId:%d]",__FUNCTION__, dwHeroId);
		return false;
	}

	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}

	// ���������Ҫ�ĵ���ʯ
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pHeroCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ����,Ӣ������ʧ��[TreasureInfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}

	vector<MoneyData> vCostCurrencyList;
	vector<Item> vReturnObjectList;
	//vector<Item> vComposeReturnList;
	int nTotalGold = 0;
	//int nHeroSoul = 0;
	//int nTotalDiamond = 0;
	if (!GetHeroResetReturn(dwHeroId, vReturnObjectList, vCostCurrencyList, nTotalGold))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡӢ�۷�������ʧ��![HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}
	
	///// ����
	// �������
	if (nTotalGold)
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nTotalGold, TRUE, FROM_HERO_RESET);
	// ��������
	for (int i = 0; i < vReturnObjectList.size(); ++i)
	{
		m_pPlayer->AddObject(vReturnObjectList[i].dwId, vReturnObjectList[i].dwNum, TRUE, FROM_HERO_RESET);
	}
	// ��������
	for (int i = 0; i < vCostCurrencyList.size(); ++i)
	{
		m_pPlayer->ModifyProperty(vCostCurrencyList[i].byType, vCostCurrencyList[i].nValue, TRUE, FROM_HERO_RESET);
	}

	// ɾ��������Ӣ��
	pHeroMgr->RemoveHero(dwHeroId, TRUE, FROM_HERO_RESET);

	// ��������
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pHeroCfg->nResetCostDiamond, TRUE, FROM_HERO_RESET);

	return true;
}

// װ���ֽ�
bool CRecyclingManager::EquipmentDecompose(DWORD *pEquipmentList, BYTE byEquipmentNum)
{
	if (!pEquipmentList || !byEquipmentNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]װ���ֽ�����Ƿ�!", __FUNCTION__);
		return false;
	}

	CEquipmentManager* pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr, false);

	// ȡ��������
	vector<Item> vReturnObjectList;
	int nGold = 0;
	int nTowerPrestige = 0;
	for (BYTE i = 0; i < byEquipmentNum; ++i)
	{
		DWORD dwEquipmentId = pEquipmentList[i];
		if (!GetEquipmentRecycleReturn(dwEquipmentId, vReturnObjectList, nGold, nTowerPrestige))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ȡװ�����շ�����Ʒʧ��[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
			return false;
		}
	}

	// todo::��ⱳ���Ƿ�����

	// ɾ��
	for (BYTE i = 0; i < byEquipmentNum; ++i)
	{
		pEquipmentMgr->RemoveEquipment(pEquipmentList[i], true, FROM_EQUIPMENT_DECOMPOSE);
	}

	// ����
	// ��������(��������)
	m_pPlayer->ModifyProperty(ROLE_PRO_TOWER_PRESTIGE, nTowerPrestige, TRUE, FROM_EQUIPMENT_DECOMPOSE);
	// �������
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, FROM_EQUIPMENT_DECOMPOSE);
	// ��������
	if (vReturnObjectList.size())
		m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_EQUIPMENT_DECOMPOSE);

	return true;
}

// װ������
bool CRecyclingManager::EquipmentReset(DWORD dwEquipmentId)
{
	CEquipmentManager* pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr,false);

	const Equipment *pEquipmentData = pEquipmentMgr->GetEquipment(dwEquipmentId);
	if (!pEquipmentData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д�װ��[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
		return false;
	}

	const Equipment_Config *pEquimentCfg = CConfigManager::Instance()->GetEquipment(pEquipmentData->dwInfoId);
	if (!pEquimentCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ������[EquipmentInfoId:%d]", __FUNCTION__, pEquipmentData->dwInfoId);
		return false;
	}

	// ���������Ҫ�ĵ���ʯ
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pEquimentCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ����,װ������ʧ��[TreasureInfoId:%d]", __FUNCTION__, pEquipmentData->dwInfoId);
		return false;
	}

	// ȡ��������
	vector<Item> vReturnObjectList;
	int nGold = 0;
	int nContext = 0;
	if (!GetEquipmentRecycleReturn(dwEquipmentId, vReturnObjectList, nGold, nContext, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ȡװ�����շ�����Ʒʧ��[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
		return false;
	}

	// todo::��ⱳ���Ƿ�����

	// ɾ��װ��
	pEquipmentMgr->RemoveEquipment(dwEquipmentId, true, FROM_EQUIPMENT_RESET);
	
	// ����
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, FROM_EQUIPMENT_RESET);
	if (vReturnObjectList.size())
		m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_EQUIPMENT_RESET);

	// ��������
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pEquimentCfg->nResetCostDiamond, TRUE, FROM_EQUIPMENT_RESET);

	return true;
}

// �������� 
bool CRecyclingManager::TreasureReset(DWORD dwTreasureId)
{
	CTreasureManager* pTreasureMgr = m_pPlayer->GetTreasureManager();
	CHECK_POINTER_RET(pTreasureMgr,false);

	const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwTreasureId);
	if (!pTreasure)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˱���[TreasureId:%d]", __FUNCTION__, dwTreasureId);
		return false;
	}

	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TreasureInfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	// ���������Ҫ�ĵ���ʯ
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pTreasureCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ����,��������ʧ��[TreasureInfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	int nGold = 0;
	vector<Item> vReturnObjectList;
	if (!GetTreasureRecycleReturn(dwTreasureId, vReturnObjectList, nGold, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ���ﷵ����Ʒʧ��[InfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	// todo::��ⱳ���Ƿ�����

	// ɾ������
	pTreasureMgr->RemoveTreasure(dwTreasureId, FROM_TREASURE_RESET);

	// ������Ʒ
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, FROM_TREASURE_RESET);
	m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_TREASURE_RESET);
	
	// ��������
	//m_pPlayer->AddObject(pTreasureCfg->wInfoId);

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pTreasureCfg->nResetCostDiamond, TRUE, FROM_TREASURE_RESET);

	return true;
}

// ���Ʒֽ�
bool CRecyclingManager::CardDecompose(DWORD *pCardUIDList, WORD wCardNum)
{
	if (!pCardUIDList || !wCardNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ʒֽ�����Ƿ�!", __FUNCTION__);
		return false;
	}

	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();
	CHECK_POINTER_RET(pZhanBuMgr, false);

	vector<Item> vReturnObjectList;
	vector<MoneyData> vReturnMoneyList;
	for (WORD i = 0; i < wCardNum; ++i)
	{
		DWORD dwCardUID = pCardUIDList[i];
		if (!GetCardRecycleReturn(dwCardUID, vReturnObjectList, vReturnMoneyList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡ���Ʒ�������ʧ��![HeroId:%d]", __FUNCTION__, dwCardUID);
			return false;
		}
	}

	// todo::��ⱳ���Ƿ�����


	// ɾ���ֽ⿨��
	pZhanBuMgr->RemoveCard(pCardUIDList, wCardNum, FROM_CARD_DECOMPOSE);

	// ������Ʒ 
	m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_CARD_DECOMPOSE);
// 	for (vector<Item>::iterator iter = vReturnObjectList.begin(); iter != vReturnObjectList.end(); ++iter)
// 	{
// 		m_pPlayer->AddObject(iter->dwId, iter->dwNum);
// 	}

	// ��������
	for (vector<MoneyData>::iterator iter = vReturnMoneyList.begin(); iter != vReturnMoneyList.end(); ++iter)
	{
		m_pPlayer->ModifyProperty(iter->byType, iter->nValue);
	}

	return true;
}

// ��������
bool CRecyclingManager::CardReset(DWORD dwCardUID)
{
	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();
	CHECK_POINTER_RET(pZhanBuMgr, false);

	const Card *pCard = pZhanBuMgr->GetCard(dwCardUID);
	if (!pCard)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д�װ��[EquipmentId:%d]", __FUNCTION__, dwCardUID);
		return false;
	}

	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[CardInfoId:%d]", __FUNCTION__, pCard->dwInfoId);
		return false;
	}

	// ���������Ҫ�ĵ���ʯ
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pCardCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ����,��������ʧ��[CardInfoId:%d]", __FUNCTION__, pCard->dwInfoId);
		return false;
	}

	// todo::��ⱳ���Ƿ�����


	vector<Item> vReturnObjectList;
	vector<MoneyData> vReturnMoneyList;
	if (!GetCardRecycleReturn(dwCardUID, vReturnObjectList, vReturnMoneyList, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ���Ʒ�������ʧ��![HeroId:%d]", __FUNCTION__, dwCardUID);
		return false;
	}

	// ɾ���ֽ⿨��
	pZhanBuMgr->RemoveCard(dwCardUID, TRUE, FROM_CARD_RESET);

	// ������ʯ
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pCardCfg->nResetCostDiamond, TRUE, FROM_CARD_RESET);

	// ������Ʒ 
	m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_CARD_RESET);
// 	for (vector<Item>::iterator iter = vReturnObjectList.begin(); iter != vReturnObjectList.end(); ++iter)
// 	{
// 		m_pPlayer->AddObject(iter->dwId, iter->dwNum);
// 	}

	// ��������
	for (vector<MoneyData>::iterator iter = vReturnMoneyList.begin(); iter != vReturnMoneyList.end(); ++iter)
	{
		m_pPlayer->ModifyProperty(iter->byType, iter->nValue);
	}

	return true;
}

// ȡӢ�۷ֽⷵ��
bool CRecyclingManager::GetHeroDecomposeReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold, int &nHeroSoul, int &nGodSoul)
{
	// 996bt�汾 todo:: ���зֽⶼ�ֽ�Ϊ1��Һ�1�꾧
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
	{
		nGold = 1;
		nHeroSoul = 1;
		return true;
	}

	// ���һ���޸� 2017-02-28
	// �ֽ�����������:
	// 1.�ֽ��������һ���Ľ��
	// 2.�ֽⷵ���ۻ�, ����������Ƭ
	// 3.�ֽⷵ�����, �����������ѵ���

	// �ֽ�: ��ɫƷ������Ӣ�ۿɷֽ�
	// 1�����ۻ귽ʽ�������ֽ�Ӣ�۱���
	// 2������(���������������������ĵľ���ͽ�ң�����������5000�Ĳ���)(��ͬ)
	// 3.ͻ��(ͻ��ʯ, ͻ�ƹ��������ĵ�ͬ��Ӣ�����ۻ귽ʽ����)
	// 4.����(ֻ����������)(��ͬ)
	// 5.����(����ʯ��ͣ���ھ������ϵĲ�����)
	// 6.װ�����ѵ���(����귽ʽ�������ѹ��������ĵľ���װ��)
	// 7.����(��ң����ѵ�, ͬ��Ӣ�����ۻ귽ʽ����)
	// 8.��������Ǳ�ܺϼ�����������ǰͣ���ھ������ϣ������ĵ�Ǳ�ܺϼ���(��ͬ)

	// Ӣ�۷ֽ�
	// 1����ɫƷ������Ӣ�ۿɷֽ�
	// 2���Ի꾧��ʽ����Ӣ�۱����Լ�����ʱ���ĵ�ͬ��Ӣ�ۣ�����������ʱ���ĵĽ�ң�
	// 3�����������������������ĵľ���ͽ�ң���������������5000�Ĳ��֣�(��ͬ)
	// 4����������������������������ʱ���ĵ���ʯ�ͽ�ң�(��ͬ)
	// 5����������Ǳ�ܺϼ�����������ǰͣ���ھ������ϣ������ĵ�Ǳ�ܺϼ���(��ͬ)
	// 6�����������У������ĵľ��ѵ��ͽ�ң����ĵ�ͬ��Ӣ�ۣ��Ի꾧��ʽ����
	// 7���������ĵľ���װ����ȫ���������ʽ�������������ϳɾ���װ�������ĵĽ�ң�
	// 8����ɫӢ�۳������꾧waiting�����������ֺ�ɫ�佫����

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	const HeroInfo *pHeroData = pHeroMgr->GetHero(dwHeroId);
	if (!pHeroData)
	{
		m_pPlayer->SendErrorMsg(ERROR_DECOMPOSE_HERO_NOT_EXIST);
		MAPLOG(ERROR_LEVEL, "[%s]���û�д�Ӣ��[HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}

	//// �����ۻ�(���ۻ귽ʽ�������ֽ�Ӣ�۱���)
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}
	nHeroSoul += pHeroCfg->nHeroSoul;


	//// ͻ�Ʒ���(ͻ��ʯ, ͻ�ƹ��������ĵ�ͬ��Ӣ�����ۻ귽ʽ����)
	int nBreakCostGold = 0;
	WORD nCostSelfNum = 0;
	Item tCostItem;
	GetHeroBreakCost(pHeroData->wBreakLevel, nBreakCostGold, tCostItem, nCostSelfNum);
 	for (WORD i = 0; i < pHeroData->wBreakCostHeroNum; ++i){
 		vReturnObjectList.push_back(Item(pHeroData->arBreakCostHeroList[i],1));
 	}

	// ͻ��ʯ
	if (tCostItem.dwId && tCostItem.dwNum)
		vReturnObjectList.push_back(tCostItem);
	// ͬ��Ӣ�����ۻ귽ʽ����
	nHeroSoul += pHeroCfg->nHeroSoul * nCostSelfNum;

	// ��������(��ͬ)
	if (pHeroData->dwTrainCostItemNum)
		vReturnObjectList.push_back(Item(g_GetConfigValue(89), pHeroData->dwTrainCostItemNum));

	// ��������(��ͬ)
	DWORD dwDestinyCostItemId = g_GetConfigValue(86);
	if (dwDestinyCostItemId && pHeroData->dwDestinyCostItemNum)
		vReturnObjectList.push_back(Item(dwDestinyCostItemId, pHeroData->dwDestinyCostItemNum));

	// ��������(��ͬ)
	DWORD dwCostExp = 0;
	int nLevelUpCostGold = 0;
	GetHeroLevelUpCost(pHeroData->wQuality, pHeroData->wLevel, dwCostExp, nLevelUpCostGold);
	// ���(��������ת��� + ���ۼ۸� + ��ǰʣ�ྭ��) 
	nGold += (nLevelUpCostGold + pHeroCfg->nPrice + pHeroData->dwExp);

	// ���ϵ�ǰʣ�ྭ��
	dwCostExp += pHeroData->dwExp;

	///////// ������ݾ����ö�Ӧ�ľ������
	// �����ĸ߼�����Ӣ��
	GetItemByExp(OBJECT_HERO, dwCostExp, vReturnObjectList);
 
 	// ���ѷ���
	WORD wCostHeroNum = 0;
	vector<DWORD> vAwakenItemList;
	GetHeroAwakenCost(pHeroData, nGold, vReturnObjectList, wCostHeroNum, vAwakenItemList);
	for (size_t i = 0; i < vAwakenItemList.size(); ++i)
	{
		const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(vAwakenItemList[i]);
		if (!pHeroCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[InfoId:%d]", __FUNCTION__, vAwakenItemList[i]);
			continue;
		}
		nGodSoul += pItemCfg->nDecomposeGodSoul;
	}
	nHeroSoul += pHeroCfg->nHeroSoul * wCostHeroNum;

	// ��������
	//vector<CostData> vCostCurrencyList;
	GetHeroEvolutionCost(pHeroData, nGold, vReturnObjectList, vCostCurrencyList);

	// ������ͬ��Ʒ����
	if (vReturnObjectList.size())
	{
		map<DWORD, Item> tmp_map_list;
		map<DWORD, Item>::iterator iter;
		for (size_t i = 0; i < vReturnObjectList.size(); ++i)
		{
			Item &item = vReturnObjectList[i];
			// ֻ����Ʒ����Ҫ�������
			if (OBJECT_ITEM == GetObejctTypeById(item.dwId))
			{
				iter = tmp_map_list.find(item.dwId);
				if (iter != tmp_map_list.end())
					iter->second.dwNum += item.dwNum;
				else
					tmp_map_list.insert(make_pair(vReturnObjectList[i].dwId, item));
			}
			else
			{
				tmp_map_list.insert(make_pair(vReturnObjectList[i].dwId, item));
			}
		}

		vReturnObjectList.clear();
		iter = tmp_map_list.begin();
		for (; iter != tmp_map_list.end(); ++iter)
		{
			vReturnObjectList.push_back(iter->second);
		}
	}

	// ������ͬ���ҵ���
// 	if (vCostCurrencyList.size())
// 	{
// 		vector<MoneyData> tmp_list;
// 		for (vector<MoneyData>::iterator iter = vCostCurrencyList.begin(); iter != vCostCurrencyList.end(); ++iter)
// 		{
// 			// �������
// 			bool bIsNewData = true;
// 			for (int k = 0; k < tmp_list.size(); ++k)
// 			{
// 				if (tmp_list[k].byType == iter->byType)
// 				{
// 					tmp_list[k].nValue += iter->nValue;
// 					bIsNewData = false;
// 				}
// 			}
// 
// 			// �������
// 			if (bIsNewData)
// 				tmp_list.push_back(*iter);
// 		}
// 		vCostCurrencyList = tmp_list;
// 	}
	
	return true;
}

// ȡӢ����������
bool CRecyclingManager::GetHeroResetReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold)
{
	// ���һ���޸� 2017-02-28
	// �ֽ�����������:
	// 1.�ֽ��������һ���Ľ��
	// 2.�ֽⷵ���ۻ�, ����������Ƭ
	// 3.�ֽⷵ�����, �����������ѵ���

	// 	3�����������������������ĵĽ�Һ;��飨��������������5000�Ĳ��֣�
	//	1�������󣬷�����ʼ�佫��ȫ����Ƭ
	//	2��ͻ�ƣ�����Ƭ��ʽ����ͻ�ƹ��������ĵ�ͬ��Ӣ����Ƭ��
	//	4��������ֻ������������
	//	5����������Ǳ��ʯ��ͣ���ھ����ϵĲ�������
	//	6�����������У��������ĵľ��ѵ�����ҡ�ͬ��Ӣ����Ƭ
	//	7���������ĵľ���װ����ȫ�����������ѱ�����

	// 1����Ʒ������
	// 2��������ʼӢ�۵�ȫ����Ƭ��ͻ�ƹ��������ĵ�ͬ��Ӣ����Ƭ������������ʱ���ĵĽ�ң�
	// 3�����������������������ĵľ���ͽ�ң���������������5000�Ĳ��֣�(��ͬ)
	// 4����������������������������ʱ���ĵ���ʯ�ͽ�ң�(��ͬ)
	// 5����������Ǳ�ܺϼ�����������ǰͣ���ھ������ϵģ������ĵ�Ǳ�ܺϼ���(��ͬ)
	// 6�����������У������ĵľ��ѵ�����ҡ�ͬ��Ӣ����Ƭ
	// 7���������ĵľ���װ����ȫ�����������ѱ����У��������ϳɾ���װ�������ĵĽ�ң�
	// 8����

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	const HeroInfo *pHeroData = pHeroMgr->GetHero(dwHeroId);
	if (!pHeroData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д�Ӣ��[HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}

	//// �����ۻ�(����Ƭ��ʽ�������ֽ�Ӣ�۱���)
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}
	// ������ʼ�佫��ȫ����Ƭ
	Item tComposeCostItem;
	GetHeroComposeCost(pHeroData->dwIntoId, tComposeCostItem);
	if (tComposeCostItem.dwId && tComposeCostItem.dwNum)
		vReturnObjectList.push_back(tComposeCostItem);

	//// ͻ�ƣ�����Ƭ��ʽ����ͻ�ƹ��������ĵ�ͬ��Ӣ����Ƭ��
	int nBreakCostGold = 0;
	WORD wBreakCostHeroNum = 0;
	Item tBreakCostItem;
	GetHeroBreakCost(pHeroData->wBreakLevel, nBreakCostGold, tBreakCostItem, wBreakCostHeroNum);
	for (WORD i = 0; i < pHeroData->wBreakCostHeroNum; ++i){
		vReturnObjectList.push_back(Item(pHeroData->arBreakCostHeroList[i], 1));
	}
	// ͻ��ʯ
	if (tBreakCostItem.dwId && tBreakCostItem.dwNum)
		vReturnObjectList.push_back(tBreakCostItem);
	// ͬ��Ӣ����Ƭ 
// 	if (nCostSelfNum)
// 	{
// 		const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pHeroData->dwIntoId);
// 		if (pComposeCfg)
// 			vReturnObjectList.push_back(Item(pComposeCfg->dwItemId, pComposeCfg->wItemNum * nCostSelfNum));
// 	}

	//// ��������(��ͬ)
	DWORD dwTrainCostItemId = g_GetConfigValue(89);
	if (pHeroData->dwTrainCostItemNum && dwTrainCostItemId)
		vReturnObjectList.push_back(Item(dwTrainCostItemId, pHeroData->dwTrainCostItemNum));

	// ��������(��ͬ)
	DWORD dwDestinyCostItemId = g_GetConfigValue(86);
	if (dwDestinyCostItemId && pHeroData->dwDestinyCostItemNum)
	{
		vReturnObjectList.push_back(Item(dwDestinyCostItemId, pHeroData->dwDestinyCostItemNum));
	}

	// ��������(��ͬ)
	DWORD dwCostExp = 0;
	int nLevelUpCostGold = 0;
	GetHeroLevelUpCost(pHeroData->wQuality, pHeroData->wLevel, dwCostExp, nLevelUpCostGold);
	// ���(��������ת��� + ���ۼ۸� + ��ǰʣ�ྭ��) 
	nGold += (nLevelUpCostGold + pHeroCfg->nPrice + pHeroData->dwExp);

	// ���ϵ�ǰʣ�ྭ��
	dwCostExp += pHeroData->dwExp;

	///////// ������ݾ����ö�Ӧ�ľ������
	// �����ĸ߼�����Ӣ��
	GetItemByExp(OBJECT_HERO, dwCostExp, vReturnObjectList);

	// ���ѷ���
	WORD wAwakenCostHeroNum = 0;
	vector<DWORD> vAwakenItemList;
	GetHeroAwakenCost(pHeroData, nGold, vReturnObjectList, wAwakenCostHeroNum, vAwakenItemList);
	// �������ѵ���
	for (size_t i = 0; i < vAwakenItemList.size(); ++i)
	{
		vReturnObjectList.push_back(Item(vAwakenItemList[i], 1));
	}

	// ��������
	//vector<CostData> vCostCurrencyList;
	GetHeroEvolutionCost(pHeroData, nGold, vReturnObjectList, vCostCurrencyList);

	// ����ͬ��Ӣ����Ƭ(����+ͻ��)
	if (wBreakCostHeroNum + wAwakenCostHeroNum)
	{
		const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pHeroData->dwIntoId);
		if (pComposeCfg)
			vReturnObjectList.push_back(Item(pComposeCfg->dwItemId, pComposeCfg->wItemNum*(wAwakenCostHeroNum + wBreakCostHeroNum)));
	}

	// ������ͬ��Ʒ����
	if (vReturnObjectList.size())
	{
		map<DWORD, Item> tmp_map_list;
		map<DWORD, Item>::iterator iter;
		for (size_t i = 0; i < vReturnObjectList.size(); ++i)
		{
			Item &item = vReturnObjectList[i];
			BYTE byObjType = GetObejctTypeById(item.dwId);
			if (OBJECT_ITEM == byObjType || OBJECT_AWAKENING_ITEM == byObjType)
			{
				iter = tmp_map_list.find(item.dwId);
				if (iter != tmp_map_list.end())
					iter->second.dwNum += item.dwNum;
				else
					tmp_map_list.insert(make_pair(vReturnObjectList[i].dwId, item));
			}
			else
			{
				tmp_map_list.insert(make_pair(vReturnObjectList[i].dwId, item));
			}
		}

		vReturnObjectList.clear();
		iter = tmp_map_list.begin();
		for (; iter != tmp_map_list.end(); ++iter)
		{
			vReturnObjectList.push_back(iter->second);
		}
	}

	// ������ͬ���ҵ���
// 	if(vCostCurrencyList.size())
// 	{
// 		vector<MoneyData> tmp_list;
// 		for (vector<MoneyData>::iterator iter = vCostCurrencyList.begin(); iter != vCostCurrencyList.end(); ++iter)
// 		{
// 			// �������
// 			bool bIsNewData = true;
// 			for (int k = 0; k < tmp_list.size(); ++k)
// 			{
// 				if (tmp_list[k].byType == iter->byType)
// 				{
// 					tmp_list[k].nValue += iter->nValue;
// 					bIsNewData = false;
// 				}
// 			}
// 
// 			// �������
// 			if (bIsNewData)
// 				tmp_list.push_back(*iter);
// 		}
// 		vCostCurrencyList = tmp_list;
// 	}

	return true;
}

// ȡװ�����շ���
bool CRecyclingManager::GetEquipmentRecycleReturn(DWORD dwEquipmentId, vector<Item> &vReturnObjectList, int &nGold, int &nTowerPrestige, bool bReset)
{
	// 996bt�汾 todo:: �ĳɷֽ�Ϊ1���
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
	{
		nGold = 1;
		return true;
	}


	CEquipmentManager* pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr, false);

	const Equipment *pEquipmentData = pEquipmentMgr->GetEquipment(dwEquipmentId);
	if (!pEquipmentData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д�װ��[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
		return false;
	}

	const Equipment_Config *pEquimentCfg = CConfigManager::Instance()->GetEquipment(pEquipmentData->dwInfoId);
	if (!pEquimentCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ������[EquipmentInfoId:%d]", __FUNCTION__, pEquipmentData->dwInfoId);
		return false;
	}

	// ǿ������
	nGold += pEquipmentData->nEnhanceCostGold;

	// ���ۼ۸�
	// nGold += pEquimentCfg->dwPrice;

	// ��������
	for (WORD i = 0; i < pEquipmentData->wRefineCostItemNum; ++i)
	{
		vReturnObjectList.push_back(pEquipmentData->arRefineCostItem[i]);
	}

	// ����(������ʼװ��)
	if (bReset)
	{
		Item tCostItem;
		GetEquipmentComposeCost(pEquipmentData->dwInfoId, tCostItem);
		if (tCostItem.dwId && tCostItem.dwNum)
			vReturnObjectList.push_back(tCostItem);
	}
	// �ֽ�(������Ƭ)
	else
	{
		// ��������
		nTowerPrestige += pEquimentCfg->nTowerPrestige;
	}

	// ���Ƿ���
	Item tCostItem;
	DWORD dwCostFragmentNum = 0;
	if (GetEquipmentStarUPCost(pEquimentCfg->byType, pEquimentCfg->wQuality, pEquipmentData->wStar, 
		pEquipmentData->wStarLevel, tCostItem, dwCostFragmentNum, nGold))
	{
		if (tCostItem.dwId && tCostItem.dwNum)
			vReturnObjectList.push_back(tCostItem);

		if (dwCostFragmentNum)
			vReturnObjectList.push_back(Item(pEquimentCfg->dwStarUpCostItemId, dwCostFragmentNum));
	}

	return true;
}

// ȡ���ﷵ��
bool CRecyclingManager::GetTreasureRecycleReturn(DWORD dwTreasureId, vector<Item> &vReturnObjectList, int &nGold, bool bReset)
{
	CTreasureManager* pTreasureMgr = m_pPlayer->GetTreasureManager();
	CHECK_POINTER_RET(pTreasureMgr, false);

	const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwTreasureId);
	if (!pTreasure)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�д˱���[TreasureId:%d]", __FUNCTION__, dwTreasureId);
		return false;
	}

	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TreasureInfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	// ȡ��������
	Item tCostItem;
	WORD wCostSelfNum = 0;
	int nRefineCostGold = 0;
	GetTreasureRefineCost(pTreasureCfg->wQuality, pTreasure->wRefineLevel, nRefineCostGold, tCostItem, wCostSelfNum);
	if (tCostItem.dwId && tCostItem.dwNum)
		vReturnObjectList.push_back(tCostItem);
	if (wCostSelfNum)
		vReturnObjectList.push_back(Item(pTreasure->dwInfoId, wCostSelfNum));

	nGold += nRefineCostGold;

	// ���ۼ۸� todo::�õ�,��Ϊ�����᷵������
	//nGold += pTreasureCfg->nPrice;

	// ���������
	if (bReset)
		vReturnObjectList.push_back(Item(pTreasureCfg->wInfoId, 1));

	// ȡ�ϳɷ���(todo::�߻���ȷ��˵����Ҫ������Ƭ)
// 	vector<Item> vComposeCostItemList;
// 	GetTreasureComposeCost(pTreasure->dwInfoId, vComposeCostItemList);
// 	for (vector<Item>::iterator iter = vComposeCostItemList.begin(); iter != vComposeCostItemList.end(); ++iter)
// 	{
// 		vReturnObjectList.push_back(*iter);
// 	}

	// ȡǿ������
	DWORD dwCostExp = pTreasureMgr->GetStrengthLevelTotalExp(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);

	// ���ϵ�ǰʣ�ྭ��
	dwCostExp += pTreasure->dwEnhanceExp;

	// ��þ���������ĵĽ��
	nGold += dwCostExp;

	///////// ������ݾ����ö�Ӧ�ľ������
	// �����ĸ߼����鱦��
	const RecycleReturn_Config *pRecycleReturnCfg = CConfigManager::Instance()->GetRecycleReturn(OBJECT_TREASURE, 1);
	if (dwCostExp && pRecycleReturnCfg && pRecycleReturnCfg->dwNeedExp)
	{
		int nNum = dwCostExp / pRecycleReturnCfg->dwNeedExp;
		if (nNum > 0)
		{
			vReturnObjectList.push_back(Item(pRecycleReturnCfg->dwObjectId, nNum));
			dwCostExp -= (pRecycleReturnCfg->dwNeedExp * nNum);
		}
	}

	// �������м����鱦��
	pRecycleReturnCfg = CConfigManager::Instance()->GetRecycleReturn(OBJECT_TREASURE, 2);
	if (dwCostExp && pRecycleReturnCfg && pRecycleReturnCfg->dwNeedExp)
	{
		int nNum = dwCostExp / pRecycleReturnCfg->dwNeedExp;
		if (nNum > 0)
		{
			vReturnObjectList.push_back(Item(pRecycleReturnCfg->dwObjectId, nNum));
			dwCostExp -= (pRecycleReturnCfg->dwNeedExp * nNum);
		}
	}

	// �����ĳ������鱦��
	pRecycleReturnCfg = CConfigManager::Instance()->GetRecycleReturn(OBJECT_TREASURE, 3);
	if (dwCostExp && pRecycleReturnCfg && pRecycleReturnCfg->dwNeedExp)
	{
		int nNum = dwCostExp / pRecycleReturnCfg->dwNeedExp;
		if (nNum > 0)
		{
			vReturnObjectList.push_back(Item(pRecycleReturnCfg->dwObjectId, nNum));
			dwCostExp -= (pRecycleReturnCfg->dwNeedExp * nNum);
		}
	}

	return true;
}

// ȡ���ƻ��շ���
bool CRecyclingManager::GetCardRecycleReturn(DWORD dwCardUId, vector<Item> &vReturnObjectList, vector<MoneyData> &vReturnMoneyList, bool bReset)
{
 	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();
 	CHECK_POINTER_RET(pZhanBuMgr, false);
 
 	// ȡ��������
 	const Card *pCardData = pZhanBuMgr->GetCard(dwCardUId);
 	if (!pCardData)
 	{
 		//m_pPlayer->SendErrorMsg(ERROR_DECOMPOSE_HERO_NOT_EXIST);
 		MAPLOG(ERROR_LEVEL, "[%s]���û�д˿���[HeroId:%d]", __FUNCTION__, dwCardUId);
 		return false;
 	}
 
 	// ȡ��������
 	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCardData->dwInfoId);
 	if (!pCardCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ƶ���������[InfoId:%d]!", __FUNCTION__, pCardData->dwInfoId);
 		return false;
 	}
 
 	// ȡ��������
	const CardLevelUp_Config *pLevelUpCfg = g_pCfgMgr->GetCardLevelUp(pCardCfg->byQuality, pCardData->wLevel);
 	if (!pLevelUpCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������Ƶ�������������[Quality:%d,Level:%d]!",
 			__FUNCTION__, pCardCfg->byQuality, pCardData->wLevel);
 		return 0;
 	}
 
	// ������Ϊ�������Ļ�õľ��鲻�㵽����
 	DWORD dwTotalExp = pCardData->dwExp /*+ pCardCfg->dwToExp*/ + pLevelUpCfg->dwTotalExp;
 
 	// �ȼ�: ���ݵȼ��;���ת��Ϊ��Ӧ�����ľ��鿨��
 	GetItemByExp(OBJECT_CARD, dwTotalExp, vReturnObjectList);
 
 	// ����: ����֮ǰ�����ĵĽ���ʯ������ͬ����������
	WORD wSameNameCardNum = 0;
	GetCardBreakCost(pCardCfg->byQuality, pCardData->wBreakLevel, vReturnObjectList, vReturnMoneyList, wSameNameCardNum);
 
 	// ����
 	if (bReset)
 	{
 		// ����: ԭ�� 
		vReturnObjectList.push_back(Item(pCardData->dwInfoId, 1 + wSameNameCardNum));
 	}
 	// �ֽ�
 	else
 	{
 		// �ֽ�: �����ǻ�
 		MoneyData tMoneyData;
 		tMoneyData.byType = ROLE_PRO_STAR_SOUL;
		tMoneyData.nValue = pCardCfg->dwDecomposeReturnStarSoul * (1 + wSameNameCardNum);
 		vReturnMoneyList.push_back(tMoneyData);
 	}

	return true;
}

// ȡͻ������
bool CRecyclingManager::GetHeroBreakCost(WORD wBreakLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum)
{
	nCostGold = 0;
	tCostItem.dwId = 0;
	wCostSelfNum = 0;
	const HeroBreakLevelUp_Config *pBreakLvUpCfg;
	for (WORD i = 0; i < wBreakLevel; ++i)
	{
		pBreakLvUpCfg = CConfigManager::Instance()->GetHeroBreakLevelUp(i);
		if (!pBreakLvUpCfg)
			continue;

		// ���Ľ��
		nCostGold += pBreakLvUpCfg->nCostGold;

		// ������Ʒ
		if (pBreakLvUpCfg->dwCostItemId)
		{
			tCostItem.dwId = pBreakLvUpCfg->dwCostItemId;
			tCostItem.dwNum += pBreakLvUpCfg->wCostItemNum;
		}
		
		// ����ͬ��Ӣ��
		wCostSelfNum += pBreakLvUpCfg->wCostSameNameHeroNum;

		// todo::����ͬƷ��Ӣ��

		// todo::����ְͬҵӢ��

	}

	return true;
}

// ȡӢ����������
bool CRecyclingManager::GetHeroLevelUpCost(WORD wQuality, WORD wLevel, DWORD &dwCostExp, int &nCostGold)
{
	dwCostExp = 0;
	nCostGold = 0;
	const HeroLevel_Config* pHeroLvCfg = CConfigManager::Instance()->GetHeroLevel(wQuality, wLevel);
	if (!pHeroLvCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۵ȼ�����[quality:%d,level:%d]", __FUNCTION__, wQuality, wLevel);
		return false;
	}

	nCostGold = pHeroLvCfg->dwTotalExp;
	dwCostExp = pHeroLvCfg->dwTotalExp;

	return true;
}

// ȡӢ�ۺϳ�����
bool CRecyclingManager::GetHeroComposeCost(DWORD dwHeroInfoId, Item &tCostItem)
{
	tCostItem.dwId = 0;
	const HeroCompose_Config *pComposeCfg = CConfigManager::Instance()->GetHeroCompose(dwHeroInfoId);
	if (!pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�ۺϳ�����[HeroInfoId:%d]", __FUNCTION__, dwHeroInfoId);
		return false;
	}

	tCostItem.dwId = pComposeCfg->dwItemId;
	tCostItem.dwNum = pComposeCfg->wItemNum;
	return true;
}

// ȡװ���ϳ�����
bool CRecyclingManager::GetEquipmentComposeCost(DWORD dwEquipmentInfoId, Item &tCostItem)
{
	tCostItem.dwId = 0;
	const EquipmentCompose_Config *pComposeCfg = CConfigManager::Instance()->GetEquipmentCompose(dwEquipmentInfoId);
	if (!pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ���ϳ�����[TreasureInfoId:%d]", __FUNCTION__, dwEquipmentInfoId);
		return false;
	}

	tCostItem.dwId = pComposeCfg->dwFragmentId;
	tCostItem.dwNum = pComposeCfg->wFragmentNum;
	return true;
}

// ȡװ����������
bool CRecyclingManager::GetEquipmentStarUPCost(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel,Item &tCostItem, DWORD &dwCostFragmentNum, int &nCostGold)
{
	for (WORD i = 0; i <= wStar; ++i)
	{
		WORD wStarLevelMax = (i == wStar) ? wStarLevel : 20; // todo::��ʱ��20,Ŀǰû����ô��
		for (WORD k = 0; k < wStarLevelMax; ++k)
		{
			const EquipmentStarUp_Config *pStarUpCfg = g_pCfgMgr->GetEquipmentStartUp(wType, wQuality, i, k);
			// ����Ҳ���,���ʾ�Ѿ������һ���ȼ�
			if (!pStarUpCfg)
				break;

			// ���
			nCostGold += pStarUpCfg->nCostGold;

			// ����
			tCostItem.dwId = pStarUpCfg->tCostItem.dwId;
			tCostItem.dwNum += pStarUpCfg->tCostItem.dwNum;

			// ��Ƭ
			dwCostFragmentNum += pStarUpCfg->dwCostSelfNum;
		}
	}

	return true;
}

// ȡ����ľ�������
bool CRecyclingManager::GetTreasureRefineCost(WORD wQuality, WORD wLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum)
{
	nCostGold = 0;
	tCostItem.dwId = 0;
	for (WORD i = 0; i < wLevel; ++i)
	{
		// ȡ���ﾫ������
		const TreasureRefine_Config *pRefineCfg = CConfigManager::Instance()->GetTreasureRefine(wQuality, i);
		if (pRefineCfg)
		{
			// ���ĵĽ��
			nCostGold += pRefineCfg->nCostGold;

			// ���ĵı�ʯ
			if (pRefineCfg->dwCostItemId)
			{
				tCostItem.dwId = pRefineCfg->dwCostItemId;
				tCostItem.dwNum += pRefineCfg->wCostItemNum;
			}

			// ���ĵı���(�Լ�)
			wCostSelfNum += pRefineCfg->byCostTreasureNum;
		}
	}

	return true;
}

// ȡ����ϳɵ�����
bool CRecyclingManager::GetTreasureComposeCost(DWORD dwInfoId, vector<Item> &vCostList)
{
	vCostList.clear();

	// ȡ�ϳɱ�������
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(dwInfoId);
	if (!pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ϳ�����[TreasureInfoId:%d]", __FUNCTION__, dwInfoId);
		return false;
	}

	Item sCostItem;

	// �����Ҫ����Ʒ
	vector<DWORD>::const_iterator iter = pComposeCfg->cost_item_list.begin();
	for (; iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		sCostItem.dwId = *iter;
		sCostItem.dwNum = 1;
		vCostList.push_back(sCostItem);
	}

	return true;
}

// ���ݾ���ȡ����
bool CRecyclingManager::GetItemByExp(WORD wObjType, DWORD dwExp, vector<Item> &vItemList)
{
	const RecycleReturn_Config *pRecycleReturnCfg = NULL;
	pRecycleReturnCfg = CConfigManager::Instance()->GetRecycleReturn(wObjType, 1);
	if (dwExp && pRecycleReturnCfg && pRecycleReturnCfg->dwNeedExp)
	{
		int nNum = dwExp / pRecycleReturnCfg->dwNeedExp;
		if (nNum > 0)
		{
			vItemList.push_back(Item(pRecycleReturnCfg->dwObjectId, nNum));
			dwExp -= (pRecycleReturnCfg->dwNeedExp * nNum);
		}
	}

	// �������м�����Ӣ��
	pRecycleReturnCfg = CConfigManager::Instance()->GetRecycleReturn(wObjType, 2);
	if (dwExp && pRecycleReturnCfg && pRecycleReturnCfg->dwNeedExp)
	{
		int nNum = dwExp / pRecycleReturnCfg->dwNeedExp;
		if (nNum > 0)
		{
			vItemList.push_back(Item(pRecycleReturnCfg->dwObjectId, nNum));
			dwExp -= (pRecycleReturnCfg->dwNeedExp * nNum);
		}
	}

	// �����ĳ�������Ӣ��
	pRecycleReturnCfg = CConfigManager::Instance()->GetRecycleReturn(wObjType, 3);
	if (dwExp && pRecycleReturnCfg && pRecycleReturnCfg->dwNeedExp)
	{
		int nNum = dwExp / pRecycleReturnCfg->dwNeedExp;
		if (nNum > 0)
		{
			vItemList.push_back(Item(pRecycleReturnCfg->dwObjectId, nNum));
			dwExp -= (pRecycleReturnCfg->dwNeedExp * nNum);
		}
	}

	return true;
}

// ȡӢ�۾�����Ϣ
bool CRecyclingManager::GetHeroAwakenCost(const HeroInfo *pHeroData, int &nGold, vector<Item> &vCostItemList, WORD &wCostHeroNum, vector<DWORD> &vAwakenItemList)
{
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}

	for (BYTE i = 0; i <= pHeroData->wAwakenStar; ++i)
	{
		BYTE byAwakenLevel = HERO_AWAKEN_LEVEL;
		bool bCurStar = false;

		// ����ǵ�ǰ����,��ʹ�õ�ǰ�ȼ�
		if (i == pHeroData->wAwakenStar)
		{
			byAwakenLevel = pHeroData->wAwakenLevel;
			bCurStar = true;
		}

		for (BYTE k = 0; k <= byAwakenLevel; ++k)
		{
			const HeroAwaken_Config *pAwakenCfg = g_pCfgMgr->GetHeroAwaken(pHeroCfg->wInitQuality, pHeroCfg->byProfession, i, k);
			if (!pAwakenCfg)
			{
				MAPLOG(GUI_LEVEL, "[%s]�Ҳ���Ӣ�۾�����������[Quality:%d,Profession:%d,Star:%d,Level:%d]",
					__FUNCTION__, pHeroCfg->wInitQuality, pHeroCfg->byProfession, i, k);
				continue;
			}

			// ����ǵ�ǰ�Ǽ���ǰ�ȼ�
			if (bCurStar && k == byAwakenLevel)
			{
				for (BYTE m = 0; m < pHeroData->wAwakenItemNum; ++m)
				{
					vAwakenItemList.push_back(pHeroData->arAwakenItemList[m]);
				}
			}
			else
			{
				// ���ĵĻ���
				nGold += pAwakenCfg->nCostMoneyValue;

				// ���ĵ���Ʒ(���ѵ�) ����ʱ����
				vCostItemList.push_back(pAwakenCfg->tCostItem);
				//vReturnObjectList.push_back(pAwakenCfg->tCostItem);

				// ���ĵ�ͬ��Ӣ��
				wCostHeroNum += pAwakenCfg->wCostHeroNum;

				// ������Ʒ�б�(������Ʒ) װ������
				for (size_t m = 0; m < pAwakenCfg->vCostItemList.size(); ++m)
				{
					vAwakenItemList.push_back(pAwakenCfg->vCostItemList[m]);
				}
			}
		}
	}

	return true;
}

// ȡӢ�۽�������
bool CRecyclingManager::GetHeroEvolutionCost(const HeroInfo *pHeroData, int &nCostGold, vector<Item> &vCostItemList, vector<MoneyData> &vCostCurrencyList)
{
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}

	MoneyData tMoneyData;
	for (WORD i = pHeroCfg->wInitQuality; i <= pHeroData->wQuality; ++i)
	{
		// ȡ��ǰƷ������
		WORD wStageMax = (i == pHeroData->wQuality) ? pHeroData->wQualityStage : MAX_QUALITY_STAGE;
		for (WORD k = 0; k < wStageMax; ++k)
		{
			const HeroEvolution_Config *pEvolutionCfg = g_pCfgMgr->GetHeroEvolution(i, k);
			if (pEvolutionCfg)
			{
				// ���
				nCostGold += pEvolutionCfg->nCostGold;

				// ����
				for (int m = 0; m < pEvolutionCfg->vCostItemList.size(); ++m)
				{
					vCostItemList.push_back(pEvolutionCfg->vCostItemList[m]);
				}

				// ����
				for (int m = 0; m < pEvolutionCfg->vCostCurrencyList.size(); ++m)
				{
					tMoneyData.byType = pEvolutionCfg->vCostCurrencyList[m].wType;
					tMoneyData.nValue = pEvolutionCfg->vCostCurrencyList[m].nValue;
					vCostCurrencyList.push_back(tMoneyData);
				}
			}
		}
	}

	return true;
}

// ȡ����ͻ������
bool CRecyclingManager::GetCardBreakCost(BYTE byQuality, WORD wBreakLevel, vector<Item> &vCostItemList,vector<MoneyData> &vCostMoneyList, WORD &wSameNameCardNum)
{
	const CardBreak_Config *pBreakCfg;
	for (WORD i = 0; i < wBreakLevel; ++i)
	{
		pBreakCfg = g_pCfgMgr->GetCardBreak(byQuality, i);
		if (!pBreakCfg)
			continue;

		// ���Ļ��ҽ��
		vector<CostData>::const_iterator iter = pBreakCfg->vCostMoneyList.begin();
		for (; iter != pBreakCfg->vCostMoneyList.end(); ++iter)
		{
			MoneyData tMoneyData;
			tMoneyData.byType = iter->wType;
			tMoneyData.nValue = iter->nValue;
			vCostMoneyList.push_back(tMoneyData);
		}

		// ������Ʒ
		if (pBreakCfg->vCostItemList.size())
			vCostItemList.insert(vCostItemList.end(), pBreakCfg->vCostItemList.begin(), pBreakCfg->vCostItemList.end());

		// ����ͬ����
		wSameNameCardNum += pBreakCfg->wCostSameCardNum;
	}

	return true;
}
