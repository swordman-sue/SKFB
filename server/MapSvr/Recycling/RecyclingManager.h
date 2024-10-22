/**\file
���չ�����
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

	// ��ʼ��
	void Init(Player *pPlayer);

public:
	// �ֽ�Ԥ��
	void OnNetDecomposeReturn(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum);

	// ����Ԥ��
	void OnNetResetReturn(BYTE byObjType, DWORD dwTargetUId);

	// �ֽ�
	void OnNetRecycleDecompose(BYTE byObjType, DWORD *pTargetUIdList, BYTE byTargetNum);

	// ����
	void OnNetRecycleReset(BYTE byObjType, DWORD dwTargetUId);

protected:
	// Ӣ�۷ֽ�
	bool HeroDecompose(DWORD *pHeroList, BYTE byHeroNum);

	// װ���ֽ�
	bool EquipmentDecompose(DWORD *pEquipmentList, BYTE byEquipmentNum);

	// Ӣ������
	bool HeroReset(DWORD dwHeroId);

	// װ������
	bool EquipmentReset(DWORD dwEquipmentId);

	// �������� 
	bool TreasureReset(DWORD dwTreasureId);

	// ���Ʒֽ�
	bool CardDecompose(DWORD *pCardUIDList, WORD wCardNum);

	// ��������
	bool CardReset(DWORD dwCardUID);

protected:

	// ȡӢ�۷ֽⷵ��
	bool GetHeroDecomposeReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold, int &nHeroSoul, int &nGodSoul);

	// ȡӢ����������
	bool GetHeroResetReturn(DWORD dwHeroId, vector<Item> &vReturnObjectList, vector<MoneyData> &vCostCurrencyList, int &nGold);



	// ȡװ�����շ���
	bool GetEquipmentRecycleReturn(DWORD dwEquipmentId, vector<Item> &vReturnObjectList, int &nGold, int &nTowerPrestige, bool bReset = false);

	// ȡ���ﷵ��
	bool GetTreasureRecycleReturn(DWORD dwTreasureId, vector<Item> &vReturnObjectList, int &nGold, bool bReset = false);

	// ȡ���ƻ��շ���
	bool GetCardRecycleReturn(DWORD dwCardUId, vector<Item> &vReturnObjectList, vector<MoneyData> &vReturnMoneyList, bool bReset = false);

public:

	// ȡӢ��ͻ������
	bool GetHeroBreakCost(WORD wBreakLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum);

	// ȡӢ����������
	bool GetHeroLevelUpCost(WORD wQuality, WORD wLevel, DWORD &dwCostExp, int &nCostGold);

	// ȡӢ�ۺϳ�����
	bool GetHeroComposeCost(DWORD dwHeroInfoId, Item &tCostItem);

	// ȡװ���ϳ�����
	bool GetEquipmentComposeCost(DWORD dwEquipmentInfoId, Item &tCostItem);

	// ȡװ����������
	bool GetEquipmentStarUPCost(WORD wType, WORD wQuality, WORD wStar, WORD wStarLevel,Item &tCostItem, DWORD &dwCostFragmentNum, int &nCostGold);

	// ȡ����ľ�������
	bool GetTreasureRefineCost(WORD wQuality, WORD wLevel, int &nCostGold, Item &tCostItem, WORD &wCostSelfNum);

	// ȡ����ϳɵ�����
	bool GetTreasureComposeCost(DWORD dwInfoId, vector<Item> &vCostList);

	// ���ݾ���ȡ����
	bool GetItemByExp(WORD wObjType, DWORD dwExp, vector<Item> &vItemList);

	// ȡӢ�۾�������
	bool GetHeroAwakenCost(const HeroInfo *pHeroData, int &nGold, vector<Item> &vCostItemList, WORD &wCostHeroNum, vector<DWORD> &vAwakenItemList);

	// ȡӢ�۽�������
	bool GetHeroEvolutionCost(const HeroInfo *pHeroData, int &nCostGold, vector<Item> &vCostItemList, vector<MoneyData> &vCostCurrencyList);

	// ȡ����ͻ������
	bool GetCardBreakCost(BYTE byQuality, WORD wBreakLevel, vector<Item> &vCostItemList, vector<MoneyData> &vCostMoneyList, WORD &wSameNameCardNum);
private:
	Player *m_pPlayer;
};