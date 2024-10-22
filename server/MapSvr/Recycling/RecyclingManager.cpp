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

// 初始化
void CRecyclingManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
}

// 请求分解返还
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


	// 装备
 	if (OBJECT_EQUIPMENT == byObjType)
 	{
		for (BYTE i = 0; i < byTargetNum; ++i)
		{
			if (!GetEquipmentRecycleReturn(pTargetUIdList[i], vReturnObjectList, nGold, nTowerPrestige))
			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]处理装备返还物品错误", __FUNCTION__);
				return;
			}
		}
 	}
 	// 英雄
 	else if (OBJECT_HERO == byObjType)
 	{
 		for (BYTE i = 0; i < byTargetNum; ++i)
 		{
			if (!GetHeroDecomposeReturn(pTargetUIdList[i], vReturnObjectList, vReturnMoneyList, nGold, nHeroSoul, nGodSoul))
 			{
				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
				MAPLOG(ERROR_LEVEL, "[%s]处理英雄返还物品错误", __FUNCTION__);
 				return;
 			}
 		}
 	}
	// 卡牌
	else if (OBJECT_CARD == byObjType)
	{
 		for (BYTE i = 0; i < byTargetNum; ++i)
 		{
 			if (!GetCardRecycleReturn(pTargetUIdList[i], vReturnObjectList, vReturnMoneyList))
 			{
 				m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
 				MAPLOG(ERROR_LEVEL, "[%s]处理卡牌返还物品错误", __FUNCTION__);
 				return;
 			}
 		}
	}
 	else
 	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
 		MAPLOG(ERROR_LEVEL, "[%s]木有处理的分解返还请求[ObjType:%d]", __FUNCTION__, byObjType);
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
			MAPLOG(ERROR_LEVEL, "[%s]分解返还道具数量超过指定数量[num:%d]", __FUNCTION__, msg.m_byObjectNum);
			break;
		}
		msg.m_ReturnObjectList[msg.m_byObjectNum++] = *iter;
	}

	// todo::暂时介样处理
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
	// 处理相同货币类型叠加
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

// 请求重生返还
void CRecyclingManager::OnNetResetReturn(BYTE byObjType, DWORD dwTargetUId)
{
	WORD wFromMsgId = C2S_RECYCLE_RESET_RETURN_REQ;

	int nGold = 0;
	int nDiamond = 0;
	int nContext = 0;
	vector<Item> vReturnObjectList;		// 通用返还
	//vector<Item> vComposeReturnList;	// 合成返还
	vector<MoneyData> vMoneyReturnList;
	//vector<CostData> vCostCurrencyList;

	// 装备
	if (OBJECT_EQUIPMENT == byObjType)
	{
		if (!GetEquipmentRecycleReturn(dwTargetUId, vReturnObjectList, nGold, nContext, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]处理装备返还物品错误", __FUNCTION__);
			return;
		}
	}
	// 英雄
	else if (OBJECT_HERO == byObjType)
	{
		if (!GetHeroResetReturn(dwTargetUId, vReturnObjectList, vMoneyReturnList, nGold))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]处理英雄返还物品错误", __FUNCTION__);
			return;
		}
	}
	// 宝物
	else if (OBJECT_TREASURE == byObjType)
	{
		if (!GetTreasureRecycleReturn(dwTargetUId, vReturnObjectList, nGold, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]处理宝物返还物品错误", __FUNCTION__);
			return;
		}
	}
	// 卡牌
	else if(OBJECT_CARD == byObjType)
	{
		if (!GetCardRecycleReturn(dwTargetUId, vReturnObjectList, vMoneyReturnList, true))
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
			MAPLOG(ERROR_LEVEL, "[%s]处理卡牌返还物品错误", __FUNCTION__);
			return;
		}
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的重生回收请求[ObjType:%d]", __FUNCTION__, byObjType);
		return;
	}

	MoneyData tMoneyData;

	MSG_S2C_RECYCLE_RESET_RETURN_RESP msg;
	msg.m_byObjType = byObjType;
	msg.m_tMoney.nValue = nGold;      // (todo::已经作废, 但是为了防止旧版本报错,暂时保留)
	msg.m_tMoney.byType = ROLE_PRO_GOLD;  // (todo::已经作废, 但是为了防止旧版本报错,暂时保留)
	msg.m_wMoneyNum = 0;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = nGold;
	msg.m_arMoneyList[msg.m_wMoneyNum++] = tMoneyData;
	// todo::英雄重生返还钻石目前还没发给客户端,等客户端得闲了再处理
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

// 分解回收(武将,装备)
void CRecyclingManager::OnNetRecycleDecompose(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum)
{
	bool bRet = false;

	// 装备
	if (OBJECT_EQUIPMENT == byObjType)
	{
		bRet = EquipmentDecompose(pTargetUIdList, byTargetNum);
	}
	// 英雄
	else if (OBJECT_HERO == byObjType)
	{
		bRet = HeroDecompose(pTargetUIdList, byTargetNum);
	}
	// 卡牌
	else if (OBJECT_CARD == byObjType)
	{
		bRet = CardDecompose(pTargetUIdList, byTargetNum);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_RECYCLE_DECOMPOSE_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的分解回收请求[ObjType:%d]", __FUNCTION__, byObjType);
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

// 重生回收(武将,装备,宝物)
void CRecyclingManager::OnNetRecycleReset(BYTE byObjType, DWORD dwTargetUId)
{
	bool bRet = false;

	// 装备
	if (OBJECT_EQUIPMENT == byObjType)
	{
		bRet = EquipmentReset(dwTargetUId);
	}
	// 英雄
	else if (OBJECT_HERO == byObjType)
	{
		bRet = HeroReset(dwTargetUId);
	}
	// 宝物
	else if (OBJECT_TREASURE == byObjType)
	{
		bRet = TreasureReset(dwTargetUId);
	}
	// 卡牌
	else if (OBJECT_CARD == byObjType)
	{
		bRet = CardReset(dwTargetUId);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_RECYCLE_RESET_REQ);;
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的重生回收请求[ObjType:%d]", __FUNCTION__,byObjType);
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

// 武将分解
bool CRecyclingManager::HeroDecompose(DWORD *pHeroList, BYTE byHeroNum)
{
	if (!pHeroList || !byHeroNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄分解参数非法!", __FUNCTION__);
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
			MAPLOG(ERROR_LEVEL, "[%s]取英雄返还数据失败![HeroId:%d]", __FUNCTION__, dwHeroId);
			return false;
		}
	}

	// todo::检测背包是否已满

	// 删除分解英雄
	pHeroMgr->RemoveHero(pHeroList, WORD(byHeroNum), FROM_HERO_DECOMPOSE);

	// 返还物品 
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

// 武将重生
bool CRecyclingManager::HeroReset(DWORD dwHeroId)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	const HeroInfo *pHeroData = pHeroMgr->GetHero(dwHeroId);
	if (!pHeroData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此英雄[HeroId:%d]",__FUNCTION__, dwHeroId);
		return false;
	}

	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}

	// 检测重生需要的的钻石
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pHeroCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]钻石不足,英雄重生失败[TreasureInfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
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
		MAPLOG(ERROR_LEVEL, "[%s]取英雄返还数据失败![HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}
	
	///// 返还
	// 返还金币
	if (nTotalGold)
		m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nTotalGold, TRUE, FROM_HERO_RESET);
	// 返还对象
	for (int i = 0; i < vReturnObjectList.size(); ++i)
	{
		m_pPlayer->AddObject(vReturnObjectList[i].dwId, vReturnObjectList[i].dwNum, TRUE, FROM_HERO_RESET);
	}
	// 返还货币
	for (int i = 0; i < vCostCurrencyList.size(); ++i)
	{
		m_pPlayer->ModifyProperty(vCostCurrencyList[i].byType, vCostCurrencyList[i].nValue, TRUE, FROM_HERO_RESET);
	}

	// 删除重生的英雄
	pHeroMgr->RemoveHero(dwHeroId, TRUE, FROM_HERO_RESET);

	// 重生消耗
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pHeroCfg->nResetCostDiamond, TRUE, FROM_HERO_RESET);

	return true;
}

// 装备分解
bool CRecyclingManager::EquipmentDecompose(DWORD *pEquipmentList, BYTE byEquipmentNum)
{
	if (!pEquipmentList || !byEquipmentNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]装备分解参数非法!", __FUNCTION__);
		return false;
	}

	CEquipmentManager* pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr, false);

	// 取返还数据
	vector<Item> vReturnObjectList;
	int nGold = 0;
	int nTowerPrestige = 0;
	for (BYTE i = 0; i < byEquipmentNum; ++i)
	{
		DWORD dwEquipmentId = pEquipmentList[i];
		if (!GetEquipmentRecycleReturn(dwEquipmentId, vReturnObjectList, nGold, nTowerPrestige))
		{
			MAPLOG(ERROR_LEVEL, "[%s]获取装备回收返还物品失败[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
			return false;
		}
	}

	// todo::检测背包是否已满

	// 删除
	for (BYTE i = 0; i < byEquipmentNum; ++i)
	{
		pEquipmentMgr->RemoveEquipment(pEquipmentList[i], true, FROM_EQUIPMENT_DECOMPOSE);
	}

	// 返还
	// 返还威名(爬塔声望)
	m_pPlayer->ModifyProperty(ROLE_PRO_TOWER_PRESTIGE, nTowerPrestige, TRUE, FROM_EQUIPMENT_DECOMPOSE);
	// 返还金币
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, TRUE, FROM_EQUIPMENT_DECOMPOSE);
	// 返还对象
	if (vReturnObjectList.size())
		m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_EQUIPMENT_DECOMPOSE);

	return true;
}

// 装备重生
bool CRecyclingManager::EquipmentReset(DWORD dwEquipmentId)
{
	CEquipmentManager* pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr,false);

	const Equipment *pEquipmentData = pEquipmentMgr->GetEquipment(dwEquipmentId);
	if (!pEquipmentData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此装备[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
		return false;
	}

	const Equipment_Config *pEquimentCfg = CConfigManager::Instance()->GetEquipment(pEquipmentData->dwInfoId);
	if (!pEquimentCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备配置[EquipmentInfoId:%d]", __FUNCTION__, pEquipmentData->dwInfoId);
		return false;
	}

	// 检测重生需要的的钻石
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pEquimentCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]钻石不足,装备重生失败[TreasureInfoId:%d]", __FUNCTION__, pEquipmentData->dwInfoId);
		return false;
	}

	// 取返还数据
	vector<Item> vReturnObjectList;
	int nGold = 0;
	int nContext = 0;
	if (!GetEquipmentRecycleReturn(dwEquipmentId, vReturnObjectList, nGold, nContext, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]获取装备回收返还物品失败[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
		return false;
	}

	// todo::检测背包是否已满

	// 删除装备
	pEquipmentMgr->RemoveEquipment(dwEquipmentId, true, FROM_EQUIPMENT_RESET);
	
	// 返还
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, FROM_EQUIPMENT_RESET);
	if (vReturnObjectList.size())
		m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_EQUIPMENT_RESET);

	// 重生消耗
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pEquimentCfg->nResetCostDiamond, TRUE, FROM_EQUIPMENT_RESET);

	return true;
}

// 宝物重生 
bool CRecyclingManager::TreasureReset(DWORD dwTreasureId)
{
	CTreasureManager* pTreasureMgr = m_pPlayer->GetTreasureManager();
	CHECK_POINTER_RET(pTreasureMgr,false);

	const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwTreasureId);
	if (!pTreasure)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此宝物[TreasureId:%d]", __FUNCTION__, dwTreasureId);
		return false;
	}

	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物配置[TreasureInfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	// 检测重生需要的的钻石
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pTreasureCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]钻石不足,宝物重生失败[TreasureInfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	int nGold = 0;
	vector<Item> vReturnObjectList;
	if (!GetTreasureRecycleReturn(dwTreasureId, vReturnObjectList, nGold, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]取宝物返还物品失败[InfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	// todo::检测背包是否已满

	// 删除宝物
	pTreasureMgr->RemoveTreasure(dwTreasureId, FROM_TREASURE_RESET);

	// 返还物品
	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, nGold, FROM_TREASURE_RESET);
	m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_TREASURE_RESET);
	
	// 返还宝物
	//m_pPlayer->AddObject(pTreasureCfg->wInfoId);

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pTreasureCfg->nResetCostDiamond, TRUE, FROM_TREASURE_RESET);

	return true;
}

// 卡牌分解
bool CRecyclingManager::CardDecompose(DWORD *pCardUIDList, WORD wCardNum)
{
	if (!pCardUIDList || !wCardNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]卡牌分解参数非法!", __FUNCTION__);
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
			MAPLOG(ERROR_LEVEL, "[%s]取卡牌返还数据失败![HeroId:%d]", __FUNCTION__, dwCardUID);
			return false;
		}
	}

	// todo::检测背包是否已满


	// 删除分解卡牌
	pZhanBuMgr->RemoveCard(pCardUIDList, wCardNum, FROM_CARD_DECOMPOSE);

	// 返还物品 
	m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_CARD_DECOMPOSE);
// 	for (vector<Item>::iterator iter = vReturnObjectList.begin(); iter != vReturnObjectList.end(); ++iter)
// 	{
// 		m_pPlayer->AddObject(iter->dwId, iter->dwNum);
// 	}

	// 返还货币
	for (vector<MoneyData>::iterator iter = vReturnMoneyList.begin(); iter != vReturnMoneyList.end(); ++iter)
	{
		m_pPlayer->ModifyProperty(iter->byType, iter->nValue);
	}

	return true;
}

// 卡牌重生
bool CRecyclingManager::CardReset(DWORD dwCardUID)
{
	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();
	CHECK_POINTER_RET(pZhanBuMgr, false);

	const Card *pCard = pZhanBuMgr->GetCard(dwCardUID);
	if (!pCard)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此装备[EquipmentId:%d]", __FUNCTION__, dwCardUID);
		return false;
	}

	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCard->dwInfoId);
	if (!pCardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌配置[CardInfoId:%d]", __FUNCTION__, pCard->dwInfoId);
		return false;
	}

	// 检测重生需要的的钻石
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pCardCfg->nResetCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]钻石不足,卡牌重生失败[CardInfoId:%d]", __FUNCTION__, pCard->dwInfoId);
		return false;
	}

	// todo::检测背包是否已满


	vector<Item> vReturnObjectList;
	vector<MoneyData> vReturnMoneyList;
	if (!GetCardRecycleReturn(dwCardUID, vReturnObjectList, vReturnMoneyList, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]取卡牌返还数据失败![HeroId:%d]", __FUNCTION__, dwCardUID);
		return false;
	}

	// 删除分解卡牌
	pZhanBuMgr->RemoveCard(dwCardUID, TRUE, FROM_CARD_RESET);

	// 消耗钻石
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pCardCfg->nResetCostDiamond, TRUE, FROM_CARD_RESET);

	// 返还物品 
	m_pPlayer->AddObjectList(vReturnObjectList, TRUE, FROM_CARD_RESET);
// 	for (vector<Item>::iterator iter = vReturnObjectList.begin(); iter != vReturnObjectList.end(); ++iter)
// 	{
// 		m_pPlayer->AddObject(iter->dwId, iter->dwNum);
// 	}

	// 返还货币
	for (vector<MoneyData>::iterator iter = vReturnMoneyList.begin(); iter != vReturnMoneyList.end(); ++iter)
	{
		m_pPlayer->ModifyProperty(iter->byType, iter->nValue);
	}

	return true;
}

// 取英雄分解返还
bool CRecyclingManager::GetHeroDecomposeReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold, int &nHeroSoul, int &nGodSoul)
{
	// 996bt版本 todo:: 所有分解都分解为1金币和1魂晶
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
	{
		nGold = 1;
		nHeroSoul = 1;
		return true;
	}

	// 最近一次修改 2017-02-28
	// 分解与重生区别:
	// 1.分解多重生多一级的金币
	// 2.分解返还雄魂, 重生返还碎片
	// 3.分解返还神魂, 重生返还觉醒道具

	// 分解: 绿色品质以上英雄可分解
	// 1、以雄魂方式返还被分解英雄本身
	// 2、升级(返还所有升级过程中消耗的经验和金币，不返还少于5000的部分)(相同)
	// 3.突破(突破石, 突破过程中消耗的同名英雄以雄魂方式返还)
	// 4.培养(只返还培养丹)(相同)
	// 5.天命(天命石，停留在经验条上的不返还)
	// 6.装备觉醒道具(以神魂方式返还觉醒过程中消耗的觉醒装备)
	// 7.觉醒(金币，觉醒丹, 同名英雄以雄魂方式返还)
	// 8.返还所有潜能合剂（不返还当前停留在经验条上，所消耗的潜能合剂）(相同)

	// 英雄分解
	// 1、绿色品质以上英雄可分解
	// 2、以魂晶形式返还英雄本身以及进阶时消耗的同名英雄（不返还进阶时消耗的金币）
	// 3、返还所有升级过程中消耗的经验和金币（不返还经验少于5000的部分）(相同)
	// 4、返还所有培养丹（不返还培养时消耗的钻石和金币）(相同)
	// 5、返还所有潜能合剂（不返还当前停留在经验条上，所消耗的潜能合剂）(相同)
	// 6、返还觉醒中，所消耗的觉醒丹和金币，消耗的同名英雄，以魂晶方式返还
	// 7、所有消耗的觉醒装备，全部以神魂形式返还（不返还合成觉醒装备所消耗的金币）
	// 8、红色英雄除返还魂晶waiting，还返还部分红色武将精华

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	const HeroInfo *pHeroData = pHeroMgr->GetHero(dwHeroId);
	if (!pHeroData)
	{
		m_pPlayer->SendErrorMsg(ERROR_DECOMPOSE_HERO_NOT_EXIST);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此英雄[HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}

	//// 返还雄魂(以雄魂方式返还被分解英雄本身)
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}
	nHeroSoul += pHeroCfg->nHeroSoul;


	//// 突破返还(突破石, 突破过程中消耗的同名英雄以雄魂方式返还)
	int nBreakCostGold = 0;
	WORD nCostSelfNum = 0;
	Item tCostItem;
	GetHeroBreakCost(pHeroData->wBreakLevel, nBreakCostGold, tCostItem, nCostSelfNum);
 	for (WORD i = 0; i < pHeroData->wBreakCostHeroNum; ++i){
 		vReturnObjectList.push_back(Item(pHeroData->arBreakCostHeroList[i],1));
 	}

	// 突破石
	if (tCostItem.dwId && tCostItem.dwNum)
		vReturnObjectList.push_back(tCostItem);
	// 同名英雄以雄魂方式返还
	nHeroSoul += pHeroCfg->nHeroSoul * nCostSelfNum;

	// 培养返还(相同)
	if (pHeroData->dwTrainCostItemNum)
		vReturnObjectList.push_back(Item(g_GetConfigValue(89), pHeroData->dwTrainCostItemNum));

	// 天命返还(相同)
	DWORD dwDestinyCostItemId = g_GetConfigValue(86);
	if (dwDestinyCostItemId && pHeroData->dwDestinyCostItemNum)
		vReturnObjectList.push_back(Item(dwDestinyCostItemId, pHeroData->dwDestinyCostItemNum));

	// 升级返还(相同)
	DWORD dwCostExp = 0;
	int nLevelUpCostGold = 0;
	GetHeroLevelUpCost(pHeroData->wQuality, pHeroData->wLevel, dwCostExp, nLevelUpCostGold);
	// 金币(升级经验转金币 + 出售价格 + 当前剩余经验) 
	nGold += (nLevelUpCostGold + pHeroCfg->nPrice + pHeroData->dwExp);

	// 加上当前剩余经验
	dwCostExp += pHeroData->dwExp;

	///////// 处理根据经验获得对应的经验道具
	// 返还的高级经验英雄
	GetItemByExp(OBJECT_HERO, dwCostExp, vReturnObjectList);
 
 	// 觉醒返还
	WORD wCostHeroNum = 0;
	vector<DWORD> vAwakenItemList;
	GetHeroAwakenCost(pHeroData, nGold, vReturnObjectList, wCostHeroNum, vAwakenItemList);
	for (size_t i = 0; i < vAwakenItemList.size(); ++i)
	{
		const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(vAwakenItemList[i]);
		if (!pHeroCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到觉醒配置[InfoId:%d]", __FUNCTION__, vAwakenItemList[i]);
			continue;
		}
		nGodSoul += pItemCfg->nDecomposeGodSoul;
	}
	nHeroSoul += pHeroCfg->nHeroSoul * wCostHeroNum;

	// 进化返还
	//vector<CostData> vCostCurrencyList;
	GetHeroEvolutionCost(pHeroData, nGold, vReturnObjectList, vCostCurrencyList);

	// 处理相同物品叠加
	if (vReturnObjectList.size())
	{
		map<DWORD, Item> tmp_map_list;
		map<DWORD, Item>::iterator iter;
		for (size_t i = 0; i < vReturnObjectList.size(); ++i)
		{
			Item &item = vReturnObjectList[i];
			// 只有物品才需要处理叠加
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

	// 处理相同货币叠加
// 	if (vCostCurrencyList.size())
// 	{
// 		vector<MoneyData> tmp_list;
// 		for (vector<MoneyData>::iterator iter = vCostCurrencyList.begin(); iter != vCostCurrencyList.end(); ++iter)
// 		{
// 			// 有则叠加
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
// 			// 无则插入
// 			if (bIsNewData)
// 				tmp_list.push_back(*iter);
// 		}
// 		vCostCurrencyList = tmp_list;
// 	}
	
	return true;
}

// 取英雄重生返还
bool CRecyclingManager::GetHeroResetReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold)
{
	// 最近一次修改 2017-02-28
	// 分解与重生区别:
	// 1.分解多重生多一级的金币
	// 2.分解返还雄魂, 重生返还碎片
	// 3.分解返还神魂, 重生返还觉醒道具

	// 	3、返还所有升级过程中消耗的金币和经验（不返还经验少于5000的部分）
	//	1、重生后，返还初始武将的全部碎片
	//	2、突破（以碎片形式返还突破过程中消耗的同名英雄碎片）
	//	4、培养（只返还培养丹）
	//	5、返还所有潜能石（停留在经验上的不返还）
	//	6、返还觉醒中，所有消耗的觉醒丹，金币、同名英雄碎片
	//	7、所有消耗的觉醒装备，全部返还到觉醒背包中

	// 1、无品质限制
	// 2、返还初始英雄的全部碎片和突破过程中消耗的同名英雄碎片（不返还进阶时消耗的金币）
	// 3、返还所有升级过程中消耗的经验和金币（不返还经验少于5000的部分）(相同)
	// 4、返还所有培养丹（不返还培养时消耗的钻石和金币）(相同)
	// 5、返还所有潜能合剂（不返还当前停留在经验条上的，所消耗的潜能合剂）(相同)
	// 6、返还觉醒中，锁消耗的觉醒丹，金币、同名英雄碎片
	// 7、所有消耗的觉醒装备，全部返还到觉醒背包中（不返还合成觉醒装备所消耗的金币）
	// 8、无

	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	const HeroInfo *pHeroData = pHeroMgr->GetHero(dwHeroId);
	if (!pHeroData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此英雄[HeroId:%d]", __FUNCTION__, dwHeroId);
		return false;
	}

	//// 返还雄魂(以碎片方式返还被分解英雄本身)
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}
	// 返还初始武将的全部碎片
	Item tComposeCostItem;
	GetHeroComposeCost(pHeroData->dwIntoId, tComposeCostItem);
	if (tComposeCostItem.dwId && tComposeCostItem.dwNum)
		vReturnObjectList.push_back(tComposeCostItem);

	//// 突破（以碎片形式返还突破过程中消耗的同名英雄碎片）
	int nBreakCostGold = 0;
	WORD wBreakCostHeroNum = 0;
	Item tBreakCostItem;
	GetHeroBreakCost(pHeroData->wBreakLevel, nBreakCostGold, tBreakCostItem, wBreakCostHeroNum);
	for (WORD i = 0; i < pHeroData->wBreakCostHeroNum; ++i){
		vReturnObjectList.push_back(Item(pHeroData->arBreakCostHeroList[i], 1));
	}
	// 突破石
	if (tBreakCostItem.dwId && tBreakCostItem.dwNum)
		vReturnObjectList.push_back(tBreakCostItem);
	// 同名英雄碎片 
// 	if (nCostSelfNum)
// 	{
// 		const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pHeroData->dwIntoId);
// 		if (pComposeCfg)
// 			vReturnObjectList.push_back(Item(pComposeCfg->dwItemId, pComposeCfg->wItemNum * nCostSelfNum));
// 	}

	//// 培养返还(相同)
	DWORD dwTrainCostItemId = g_GetConfigValue(89);
	if (pHeroData->dwTrainCostItemNum && dwTrainCostItemId)
		vReturnObjectList.push_back(Item(dwTrainCostItemId, pHeroData->dwTrainCostItemNum));

	// 天命返还(相同)
	DWORD dwDestinyCostItemId = g_GetConfigValue(86);
	if (dwDestinyCostItemId && pHeroData->dwDestinyCostItemNum)
	{
		vReturnObjectList.push_back(Item(dwDestinyCostItemId, pHeroData->dwDestinyCostItemNum));
	}

	// 升级返还(相同)
	DWORD dwCostExp = 0;
	int nLevelUpCostGold = 0;
	GetHeroLevelUpCost(pHeroData->wQuality, pHeroData->wLevel, dwCostExp, nLevelUpCostGold);
	// 金币(升级经验转金币 + 出售价格 + 当前剩余经验) 
	nGold += (nLevelUpCostGold + pHeroCfg->nPrice + pHeroData->dwExp);

	// 加上当前剩余经验
	dwCostExp += pHeroData->dwExp;

	///////// 处理根据经验获得对应的经验道具
	// 返还的高级经验英雄
	GetItemByExp(OBJECT_HERO, dwCostExp, vReturnObjectList);

	// 觉醒返还
	WORD wAwakenCostHeroNum = 0;
	vector<DWORD> vAwakenItemList;
	GetHeroAwakenCost(pHeroData, nGold, vReturnObjectList, wAwakenCostHeroNum, vAwakenItemList);
	// 返还觉醒道具
	for (size_t i = 0; i < vAwakenItemList.size(); ++i)
	{
		vReturnObjectList.push_back(Item(vAwakenItemList[i], 1));
	}

	// 进化返还
	//vector<CostData> vCostCurrencyList;
	GetHeroEvolutionCost(pHeroData, nGold, vReturnObjectList, vCostCurrencyList);

	// 返还同名英雄碎片(觉醒+突破)
	if (wBreakCostHeroNum + wAwakenCostHeroNum)
	{
		const HeroCompose_Config *pComposeCfg = g_pCfgMgr->GetHeroCompose(pHeroData->dwIntoId);
		if (pComposeCfg)
			vReturnObjectList.push_back(Item(pComposeCfg->dwItemId, pComposeCfg->wItemNum*(wAwakenCostHeroNum + wBreakCostHeroNum)));
	}

	// 处理相同物品叠加
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

	// 处理相同货币叠加
// 	if(vCostCurrencyList.size())
// 	{
// 		vector<MoneyData> tmp_list;
// 		for (vector<MoneyData>::iterator iter = vCostCurrencyList.begin(); iter != vCostCurrencyList.end(); ++iter)
// 		{
// 			// 有则叠加
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
// 			// 无则插入
// 			if (bIsNewData)
// 				tmp_list.push_back(*iter);
// 		}
// 		vCostCurrencyList = tmp_list;
// 	}

	return true;
}

// 取装备回收返还
bool CRecyclingManager::GetEquipmentRecycleReturn(DWORD dwEquipmentId, vector<Item> &vReturnObjectList, int &nGold, int &nTowerPrestige, bool bReset)
{
	// 996bt版本 todo:: 改成分解为1金币
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
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此装备[EquipmentId:%d]", __FUNCTION__, dwEquipmentId);
		return false;
	}

	const Equipment_Config *pEquimentCfg = CConfigManager::Instance()->GetEquipment(pEquipmentData->dwInfoId);
	if (!pEquimentCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备配置[EquipmentInfoId:%d]", __FUNCTION__, pEquipmentData->dwInfoId);
		return false;
	}

	// 强化返还
	nGold += pEquipmentData->nEnhanceCostGold;

	// 出售价格
	// nGold += pEquimentCfg->dwPrice;

	// 精炼返还
	for (WORD i = 0; i < pEquipmentData->wRefineCostItemNum; ++i)
	{
		vReturnObjectList.push_back(pEquipmentData->arRefineCostItem[i]);
	}

	// 重生(返还初始装备)
	if (bReset)
	{
		Item tCostItem;
		GetEquipmentComposeCost(pEquipmentData->dwInfoId, tCostItem);
		if (tCostItem.dwId && tCostItem.dwNum)
			vReturnObjectList.push_back(tCostItem);
	}
	// 分解(返还碎片)
	else
	{
		// 返还威名
		nTowerPrestige += pEquimentCfg->nTowerPrestige;
	}

	// 升星返还
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

// 取宝物返还
bool CRecyclingManager::GetTreasureRecycleReturn(DWORD dwTreasureId, vector<Item> &vReturnObjectList, int &nGold, bool bReset)
{
	CTreasureManager* pTreasureMgr = m_pPlayer->GetTreasureManager();
	CHECK_POINTER_RET(pTreasureMgr, false);

	const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwTreasureId);
	if (!pTreasure)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此宝物[TreasureId:%d]", __FUNCTION__, dwTreasureId);
		return false;
	}

	const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
	if (!pTreasureCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物配置[TreasureInfoId:%d]", __FUNCTION__, pTreasure->dwInfoId);
		return false;
	}

	// 取精炼返还
	Item tCostItem;
	WORD wCostSelfNum = 0;
	int nRefineCostGold = 0;
	GetTreasureRefineCost(pTreasureCfg->wQuality, pTreasure->wRefineLevel, nRefineCostGold, tCostItem, wCostSelfNum);
	if (tCostItem.dwId && tCostItem.dwNum)
		vReturnObjectList.push_back(tCostItem);
	if (wCostSelfNum)
		vReturnObjectList.push_back(Item(pTreasure->dwInfoId, wCostSelfNum));

	nGold += nRefineCostGold;

	// 出售价格 todo::拿掉,因为重生会返还本身
	//nGold += pTreasureCfg->nPrice;

	// 如果是重生
	if (bReset)
		vReturnObjectList.push_back(Item(pTreasureCfg->wInfoId, 1));

	// 取合成返还(todo::策划已确定说不需要返换碎片)
// 	vector<Item> vComposeCostItemList;
// 	GetTreasureComposeCost(pTreasure->dwInfoId, vComposeCostItemList);
// 	for (vector<Item>::iterator iter = vComposeCostItemList.begin(); iter != vComposeCostItemList.end(); ++iter)
// 	{
// 		vReturnObjectList.push_back(*iter);
// 	}

	// 取强化返还
	DWORD dwCostExp = pTreasureMgr->GetStrengthLevelTotalExp(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);

	// 加上当前剩余经验
	dwCostExp += pTreasure->dwEnhanceExp;

	// 获得经验等于消耗的金币
	nGold += dwCostExp;

	///////// 处理根据经验获得对应的经验道具
	// 返还的高级经验宝物
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

	// 返还的中级经验宝物
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

	// 返还的初级经验宝物
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

// 取卡牌回收返还
bool CRecyclingManager::GetCardRecycleReturn(DWORD dwCardUId, vector<Item> &vReturnObjectList, vector<MoneyData> &vReturnMoneyList, bool bReset)
{
 	CZhanBuManager *pZhanBuMgr = m_pPlayer->GetZhanBuManager();
 	CHECK_POINTER_RET(pZhanBuMgr, false);
 
 	// 取卡牌数据
 	const Card *pCardData = pZhanBuMgr->GetCard(dwCardUId);
 	if (!pCardData)
 	{
 		//m_pPlayer->SendErrorMsg(ERROR_DECOMPOSE_HERO_NOT_EXIST);
 		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此卡牌[HeroId:%d]", __FUNCTION__, dwCardUId);
 		return false;
 	}
 
 	// 取卡牌配置
 	const Card_Config *pCardCfg = g_pCfgMgr->GetCard(pCardData->dwInfoId);
 	if (!pCardCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌的配置数据[InfoId:%d]!", __FUNCTION__, pCardData->dwInfoId);
 		return false;
 	}
 
 	// 取升级配置
	const CardLevelUp_Config *pLevelUpCfg = g_pCfgMgr->GetCardLevelUp(pCardCfg->byQuality, pCardData->wLevel);
 	if (!pLevelUpCfg)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]找不到卡牌的升级配置数据[Quality:%d,Level:%d]!",
 			__FUNCTION__, pCardCfg->byQuality, pCardData->wLevel);
 		return 0;
 	}
 
	// 本身作为升级消耗获得的经验不算到里面
 	DWORD dwTotalExp = pCardData->dwExp /*+ pCardCfg->dwToExp*/ + pLevelUpCfg->dwTotalExp;
 
 	// 等级: 根据等级和经验转化为对应数量的经验卡牌
 	GetItemByExp(OBJECT_CARD, dwTotalExp, vReturnObjectList);
 
 	// 进阶: 返还之前所消耗的进阶石总数和同名卡总数据
	WORD wSameNameCardNum = 0;
	GetCardBreakCost(pCardCfg->byQuality, pCardData->wBreakLevel, vReturnObjectList, vReturnMoneyList, wSameNameCardNum);
 
 	// 重生
 	if (bReset)
 	{
 		// 重生: 原卡 
		vReturnObjectList.push_back(Item(pCardData->dwInfoId, 1 + wSameNameCardNum));
 	}
 	// 分解
 	else
 	{
 		// 分解: 配置星魂
 		MoneyData tMoneyData;
 		tMoneyData.byType = ROLE_PRO_STAR_SOUL;
		tMoneyData.nValue = pCardCfg->dwDecomposeReturnStarSoul * (1 + wSameNameCardNum);
 		vReturnMoneyList.push_back(tMoneyData);
 	}

	return true;
}

// 取突破消耗
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

		// 消耗金币
		nCostGold += pBreakLvUpCfg->nCostGold;

		// 消耗物品
		if (pBreakLvUpCfg->dwCostItemId)
		{
			tCostItem.dwId = pBreakLvUpCfg->dwCostItemId;
			tCostItem.dwNum += pBreakLvUpCfg->wCostItemNum;
		}
		
		// 消耗同名英雄
		wCostSelfNum += pBreakLvUpCfg->wCostSameNameHeroNum;

		// todo::消耗同品质英雄

		// todo::消耗同职业英雄

	}

	return true;
}

// 取英雄升级消耗
bool CRecyclingManager::GetHeroLevelUpCost(WORD wQuality, WORD wLevel, DWORD &dwCostExp, int &nCostGold)
{
	dwCostExp = 0;
	nCostGold = 0;
	const HeroLevel_Config* pHeroLvCfg = CConfigManager::Instance()->GetHeroLevel(wQuality, wLevel);
	if (!pHeroLvCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄等级配置[quality:%d,level:%d]", __FUNCTION__, wQuality, wLevel);
		return false;
	}

	nCostGold = pHeroLvCfg->dwTotalExp;
	dwCostExp = pHeroLvCfg->dwTotalExp;

	return true;
}

// 取英雄合成消耗
bool CRecyclingManager::GetHeroComposeCost(DWORD dwHeroInfoId, Item &tCostItem)
{
	tCostItem.dwId = 0;
	const HeroCompose_Config *pComposeCfg = CConfigManager::Instance()->GetHeroCompose(dwHeroInfoId);
	if (!pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄合成配置[HeroInfoId:%d]", __FUNCTION__, dwHeroInfoId);
		return false;
	}

	tCostItem.dwId = pComposeCfg->dwItemId;
	tCostItem.dwNum = pComposeCfg->wItemNum;
	return true;
}

// 取装备合成消耗
bool CRecyclingManager::GetEquipmentComposeCost(DWORD dwEquipmentInfoId, Item &tCostItem)
{
	tCostItem.dwId = 0;
	const EquipmentCompose_Config *pComposeCfg = CConfigManager::Instance()->GetEquipmentCompose(dwEquipmentInfoId);
	if (!pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到装备合成配置[TreasureInfoId:%d]", __FUNCTION__, dwEquipmentInfoId);
		return false;
	}

	tCostItem.dwId = pComposeCfg->dwFragmentId;
	tCostItem.dwNum = pComposeCfg->wFragmentNum;
	return true;
}

// 取装备升星消耗
bool CRecyclingManager::GetEquipmentStarUPCost(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel,Item &tCostItem, DWORD &dwCostFragmentNum, int &nCostGold)
{
	for (WORD i = 0; i <= wStar; ++i)
	{
		WORD wStarLevelMax = (i == wStar) ? wStarLevel : 20; // todo::暂时定20,目前没有这么大
		for (WORD k = 0; k < wStarLevelMax; ++k)
		{
			const EquipmentStarUp_Config *pStarUpCfg = g_pCfgMgr->GetEquipmentStartUp(wType, wQuality, i, k);
			// 如果找不到,则表示已经是最后一个等级
			if (!pStarUpCfg)
				break;

			// 金币
			nCostGold += pStarUpCfg->nCostGold;

			// 道具
			tCostItem.dwId = pStarUpCfg->tCostItem.dwId;
			tCostItem.dwNum += pStarUpCfg->tCostItem.dwNum;

			// 碎片
			dwCostFragmentNum += pStarUpCfg->dwCostSelfNum;
		}
	}

	return true;
}

// 取宝物的精炼消耗
bool CRecyclingManager::GetTreasureRefineCost(WORD wQuality, WORD wLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum)
{
	nCostGold = 0;
	tCostItem.dwId = 0;
	for (WORD i = 0; i < wLevel; ++i)
	{
		// 取宝物精练配置
		const TreasureRefine_Config *pRefineCfg = CConfigManager::Instance()->GetTreasureRefine(wQuality, i);
		if (pRefineCfg)
		{
			// 消耗的金币
			nCostGold += pRefineCfg->nCostGold;

			// 消耗的宝石
			if (pRefineCfg->dwCostItemId)
			{
				tCostItem.dwId = pRefineCfg->dwCostItemId;
				tCostItem.dwNum += pRefineCfg->wCostItemNum;
			}

			// 消耗的宝物(自己)
			wCostSelfNum += pRefineCfg->byCostTreasureNum;
		}
	}

	return true;
}

// 取宝物合成的消耗
bool CRecyclingManager::GetTreasureComposeCost(DWORD dwInfoId, vector<Item> &vCostList)
{
	vCostList.clear();

	// 取合成宝物配置
	const TreasureCompose_Config *pComposeCfg = CConfigManager::Instance()->GetTreasureCompose(dwInfoId);
	if (!pComposeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到宝物合成配置[TreasureInfoId:%d]", __FUNCTION__, dwInfoId);
		return false;
	}

	Item sCostItem;

	// 检测需要的物品
	vector<DWORD>::const_iterator iter = pComposeCfg->cost_item_list.begin();
	for (; iter != pComposeCfg->cost_item_list.end(); ++iter)
	{
		sCostItem.dwId = *iter;
		sCostItem.dwNum = 1;
		vCostList.push_back(sCostItem);
	}

	return true;
}

// 根据经验取道具
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

	// 返还的中级经验英雄
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

	// 返还的初级经验英雄
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

// 取英雄觉醒消息
bool CRecyclingManager::GetHeroAwakenCost(const HeroInfo *pHeroData, int &nGold, vector<Item> &vCostItemList, WORD &wCostHeroNum, vector<DWORD> &vAwakenItemList)
{
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}

	for (BYTE i = 0; i <= pHeroData->wAwakenStar; ++i)
	{
		BYTE byAwakenLevel = HERO_AWAKEN_LEVEL;
		bool bCurStar = false;

		// 如果是当前星数,则使用当前等级
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
				MAPLOG(GUI_LEVEL, "[%s]找不到英雄觉醒配置数据[Quality:%d,Profession:%d,Star:%d,Level:%d]",
					__FUNCTION__, pHeroCfg->wInitQuality, pHeroCfg->byProfession, i, k);
				continue;
			}

			// 如果是当前星及当前等级
			if (bCurStar && k == byAwakenLevel)
			{
				for (BYTE m = 0; m < pHeroData->wAwakenItemNum; ++m)
				{
					vAwakenItemList.push_back(pHeroData->arAwakenItemList[m]);
				}
			}
			else
			{
				// 消耗的货币
				nGold += pAwakenCfg->nCostMoneyValue;

				// 消耗的物品(觉醒丹) 觉醒时消耗
				vCostItemList.push_back(pAwakenCfg->tCostItem);
				//vReturnObjectList.push_back(pAwakenCfg->tCostItem);

				// 消耗的同名英雄
				wCostHeroNum += pAwakenCfg->wCostHeroNum;

				// 消耗物品列表(觉醒物品) 装备消耗
				for (size_t m = 0; m < pAwakenCfg->vCostItemList.size(); ++m)
				{
					vAwakenItemList.push_back(pAwakenCfg->vCostItemList[m]);
				}
			}
		}
	}

	return true;
}

// 取英雄进化消耗
bool CRecyclingManager::GetHeroEvolutionCost(const HeroInfo *pHeroData, int &nCostGold, vector<Item> &vCostItemList, vector<MoneyData> &vCostCurrencyList)
{
	const Hero_Config *pHeroCfg = CConfigManager::Instance()->GetHero(pHeroData->dwIntoId);
	if (!pHeroCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[InfoId:%d]", __FUNCTION__, pHeroData->dwIntoId);
		return false;
	}

	MoneyData tMoneyData;
	for (WORD i = pHeroCfg->wInitQuality; i <= pHeroData->wQuality; ++i)
	{
		// 取当前品阶上限
		WORD wStageMax = (i == pHeroData->wQuality) ? pHeroData->wQualityStage : MAX_QUALITY_STAGE;
		for (WORD k = 0; k < wStageMax; ++k)
		{
			const HeroEvolution_Config *pEvolutionCfg = g_pCfgMgr->GetHeroEvolution(i, k);
			if (pEvolutionCfg)
			{
				// 金币
				nCostGold += pEvolutionCfg->nCostGold;

				// 道具
				for (int m = 0; m < pEvolutionCfg->vCostItemList.size(); ++m)
				{
					vCostItemList.push_back(pEvolutionCfg->vCostItemList[m]);
				}

				// 货币
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

// 取卡牌突破消耗
bool CRecyclingManager::GetCardBreakCost(BYTE byQuality, WORD wBreakLevel, vector<Item> &vCostItemList,vector<MoneyData> &vCostMoneyList, WORD &wSameNameCardNum)
{
	const CardBreak_Config *pBreakCfg;
	for (WORD i = 0; i < wBreakLevel; ++i)
	{
		pBreakCfg = g_pCfgMgr->GetCardBreak(byQuality, i);
		if (!pBreakCfg)
			continue;

		// 消耗货币金币
		vector<CostData>::const_iterator iter = pBreakCfg->vCostMoneyList.begin();
		for (; iter != pBreakCfg->vCostMoneyList.end(); ++iter)
		{
			MoneyData tMoneyData;
			tMoneyData.byType = iter->wType;
			tMoneyData.nValue = iter->nValue;
			vCostMoneyList.push_back(tMoneyData);
		}

		// 消耗物品
		if (pBreakCfg->vCostItemList.size())
			vCostItemList.insert(vCostItemList.end(), pBreakCfg->vCostItemList.begin(), pBreakCfg->vCostItemList.end());

		// 消耗同名卡
		wSameNameCardNum += pBreakCfg->wCostSameCardNum;
	}

	return true;
}
