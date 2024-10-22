/**\file
回收管理器
\author Kevin
\date 2016/7/19
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;

class CRecyclingManager
{
public:
	CRecyclingManager();
	~CRecyclingManager();

	// 初始化
	void Init(Player *pPlayer);

public:
	// 分解预览
	void OnNetDecomposeReturn(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum);

	// 重生预览
	void OnNetResetReturn(BYTE byObjType, DWORD dwTargetUId);

	// 分解
	void OnNetRecycleDecompose(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum);

	// 重生
	void OnNetRecycleReset(BYTE byObjType, DWORD dwTargetUId);

protected:
	// 英雄分解
	bool HeroDecompose(DWORD *pHeroList, BYTE byHeroNum);

	// 装备分解
	bool EquipmentDecompose(DWORD *pEquipmentList, BYTE byEquipmentNum);

	// 英雄重生
	bool HeroReset(DWORD dwHeroId);

	// 装备重生
	bool EquipmentReset(DWORD dwEquipmentId);

	// 宝物重生 
	bool TreasureReset(DWORD dwTreasureId);

	// 卡牌分解
	bool CardDecompose(DWORD *pCardUIDList, WORD wCardNum);

	// 卡牌重生
	bool CardReset(DWORD dwCardUID);

protected:

	// 取英雄分解返还
	bool GetHeroDecomposeReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold, int &nHeroSoul, int &nGodSoul);

	// 取英雄重生返还
	bool GetHeroResetReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold);



	// 取装备回收返还
	bool GetEquipmentRecycleReturn(DWORD dwEquipmentId, vector<Item> &vReturnObjectList, int &nGold, int &nTowerPrestige, bool bReset = false);

	// 取宝物返还
	bool GetTreasureRecycleReturn(DWORD dwTreasureId, vector<Item> &vReturnObjectList, int &nGold, bool bReset = false);

	// 取卡牌回收返还
	bool GetCardRecycleReturn(DWORD dwCardUId, vector<Item> &vReturnObjectList, vector<MoneyData> &vReturnMoneyList, bool bReset = false);

public:

	// 取英雄突破消耗
	bool GetHeroBreakCost(WORD wBreakLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum);

	// 取英雄升级消耗
	bool GetHeroLevelUpCost(WORD wQuality, WORD wLevel, DWORD &dwCostExp, int &nCostGold);

	// 取英雄合成消耗
	bool GetHeroComposeCost(DWORD dwHeroInfoId, Item &tCostItem);

	// 取装备合成消耗
	bool GetEquipmentComposeCost(DWORD dwEquipmentInfoId, Item &tCostItem);

	// 取装备升星消耗
	bool GetEquipmentStarUPCost(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel,Item &tCostItem, DWORD &dwCostFragmentNum, int &nCostGold);

	// 取宝物的精炼消耗
	bool GetTreasureRefineCost(WORD wQuality, WORD wLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum);

	// 取宝物合成的消耗
	bool GetTreasureComposeCost(DWORD dwInfoId, vector<Item> &vCostList);

	// 根据经验取道具
	bool GetItemByExp(WORD wObjType, DWORD dwExp, vector<Item> &vItemList);

	// 取英雄觉醒消耗
	bool GetHeroAwakenCost(const HeroInfo *pHeroData, int &nGold, vector<Item> &vCostItemList, WORD &wCostHeroNum, vector<DWORD> &vAwakenItemList);

	// 取英雄进化消耗
	bool GetHeroEvolutionCost(const HeroInfo *pHeroData, int &nCostGold, vector<Item> &vCostItemList, vector<MoneyData> &vCostCurrencyList);

	// 取卡牌突破消耗
	bool GetCardBreakCost(BYTE byQuality, WORD wBreakLevel, vector<Item> &vCostItemList, vector<MoneyData> &vCostMoneyList, WORD &wSameNameCardNum);
private:
	Player *m_pPlayer;
};