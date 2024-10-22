/**\file
Ӣ�۹�����
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
struct HeroVipSummon_Config;

class CHeroManager
{
public:
	CHeroManager();
	~CHeroManager();

	void Init(Player *pPlayer);

public:
	// Ӣ������
	void OnHeroBattle(BYTE byPos, DWORD dwHeroUID);

	// Ԯ������
	void OnReinforcementBattle(BYTE byPos, DWORD dwHeroUID);

	// �ϳ�
	void OnCompose(WORD wHeroInfoId);

	// �ٻ�
	void OnSummon(BYTE byType);

	// VIP�ٻ�
	void OnNetVIPSummon(WORD wCamp);

	// ����(todo::�ɽӿ�Ŀǰ��ʱ��ʹ��)
	void OnLevelUp(DWORD dwHeroUID, DWORD *pCostHeroList, BYTE byCostHeroListNum);

	// ����
	void OnLevelUpEx(DWORD dwHeroUID, DWORD *pCostHeroList, WORD wCostHeroListNum);

	// ͻ��(����)
	void OnBreakEx(DWORD dwHeroUID);

	// ͻ��(����)
	void OnBreak(DWORD dwHeroUID, DWORD *pCostSameNameHeroList, WORD wCostSameNameHeroNum, 
		DWORD *pCostSameQualityHeroList, WORD wCostSameQualityHeroNum,
		DWORD *pCostSameProfessionHeroList, WORD wCostSameProfessionHeroNum);

	// ����(Ʒ��)
	void OnNetEvolution(DWORD dwHeroUID);

	// ����(todo::���հ汾,�ϵ����Ҷ�����)
	void OnNetTrain(DWORD dwHeroUID, BYTE byCostType, BYTE byTrainTimes);

	// �����滻(todo::���հ汾,�ϵ����Ҷ�����)
	void OnNetTrainReplace(DWORD dwHeroUID);

	// ��ȡ��������(todo::���հ汾,�ϵ����Ҷ�����)
	void OnNetGetTrainReward(DWORD dwHeroUID, BYTE byProType);

	// ����(Ǳ��)
	void OnNetDestiny(DWORD dwHeroUID);

	// ����(Ǳ��)
	void OnNetDestinyEx(DWORD dwHeroUID, WORD wDoTimes);

	// װ�����ѵ���
	void OnNetEquipAwakeningItem(DWORD dwHeroUID, WORD wItemId);

	// ����
	void OnNetAwaken(DWORD dwHeroUID, const DWORD *pCostHeroUIDList, WORD wCostHeroNum);

	// ����Ӣ�ۻ��������б�
	void OnNetHeroBaseList();

	// ����Ӣ����ϸ��Ϣ
	void OnNetHeroInfo(DWORD dwUID);

	// �����ռ�Ӣ���б�
	void OnNetCollectionList();

	// �����ٻ�����
	void OnNetSummonData();
public:
	// ���Ӣ��
	DWORD AddHero(DWORD dwHeroInfoId, bool bNotify = true, WORD wFromId = 0);

	// ���Ӣ��
	bool AddHero(const HeroInfo &rInfo, bool bNotify = true);

	// �Ƴ�Ӣ��
	bool RemoveHero(DWORD dwHeroUID, bool bNotify = true, WORD wFromId = 0);

	// �Ƴ�Ӣ��
	bool RemoveHero(DWORD *pHeroUIDList, WORD wHeroNum, WORD wFromId = 0);

	// ȡӢ��
	const HeroInfo* GetHero(DWORD dwHeroUID);

	// ȡ������������
	WORD GetBagEmptySize();

	// ȡӢ������
	WORD GetHeroNumByInfoId(DWORD dwHeroInfoId);

	// �Ƿ�����(ʹ��uid���)
	bool IsToBattle(DWORD dwUID);

	// �Ƿ�����(ʹ��infoid���)
	bool IsToBattleEx(DWORD dwInfoId, BYTE bySlot = INVALIDID8);

	// �Ƿ��������
	bool IsCanToBattle(DWORD dwInfoId, BYTE bySlot = INVALIDID8);

	// �Ƿ��������
	bool IsCanToBattleEx(DWORD dwInfoId, BYTE bySlot = INVALIDID8);

	// ����Ӣ���б�
	void SendHeroBaseList();

	// ���������б�
	void SendFormationList();

	// �����ٻ�����
	void SendSummonData();

	// ��������б�
	void FillFormationList(ClientFormation *pFormationList, BYTE &byFormationNum);

	// �Ƿ����ռ���Ӣ��
	bool IsAlreadyCollection(DWORD dwHeroInfoId);

	// ���������ȼ�
	void UpdateTrainLevel();

	// ȡ��ף����Ӣ������
	WORD GetBlessHeroNum();

	// �Ƿ�ף������
	bool IsBlessSprite(DWORD dwSpriteId);

	// ���þ���ף��
	void SetSpriteBless(WORD wFormactionPos, DWORD dwSpriteId);

	// ȡ��λ�ϵ�ף������
	DWORD GetBlessSpriteId(WORD wFormactionPos);

	// ȡף���������ڵ���λ
	short GetBlessSpriteFormactionPos(DWORD dwSpriteId);

public:

	// ȡ����Ӣ�۵�ս��
	//DWORD GetAllHeroBattleValue();

	// ����ռ���Ӣ��
	void AddCollectionHero(DWORD dwHeroInfoId);

	// ȡװ����������Ӣ��ID
	WORD GetEquipmentWearHeroId(DWORD dwEquipemntUID);

	// ȡ���ﱻ������Ӣ��ID
	WORD GetTreasureWearHeroId(DWORD dwTreasureUID);

	// ȡ���Ʊ�������Ӣ��ID
	DWORD GetCardWearHeroId(DWORD dwCardUID);

	// Ӣ�ۻ�����Ϣ֪ͨ
	void HeroBaseDataNotify(DWORD dwUID, BYTE byFlag = true, bool bNotice = true);

	// ͨ������ȡ��������
	const Formation* GetFormationByIndex(BYTE bySlot);

	// �Ƿ�װ��������
	bool IsEquipmentWear(BYTE bySlot, DWORD dwEquipmentInfoId);

	// �Ƿ��ﱻ����
	bool IsTreasureWear(BYTE bySlot, DWORD dwTreasureInfoId);

	// �Ƿ�װ���Ŀ�������
	bool IsEquipCardType(BYTE bySlot, WORD wCardType);

	// �ṩ������ģ��ʹ��
public:
	// ȡ����ָ��ͻ�Ƶȼ���Ӣ������
	WORD GetHeroNumByBreakLevel(WORD wBreakLevel);

	// ȡ����ָ��ͻ�Ƶȼ���Ӣ������
	WORD GetHeroNumByDestinyLevel(WORD wDestinyLevel);

	// ȡ����ָ�������ȼ���Ӣ������
	WORD GetHeroNumByTrainLevel(WORD wTrainLevel);

	// ȡ������Ӣ�۵���͵ȼ�
	WORD GetFormationHeroMinLevel();

	// ȡ������Ӣ�۵���ͽ��׵ȼ���ͻ�Ƶȼ���
	WORD GetFormationHeroMinBreakLevel();

	// ȡ������Ӣ�۵���������ȼ�
	WORD GetFormationHeroMinTrainLevel();

	// ȡ������Ӣ�۵���������ȼ�
	WORD GetFormationHeroMinDestinyLevel();

	// ȡ����������Ӣ�۵�����
	WORD GetFormationAllHeroAwakenStar();

	// ȡ����Ӣ������
	BYTE GetToBattleHeroNum();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ���ñ�ʯ��λ
	void SetGemHole(WORD wFormationSlot, WORD wHole, DWORD dwGemId);

	// ȡԮ������С�ȼ�
	WORD GetReinforcementMinLevel();

protected:
	// ����Ӣ��
	HeroInfo* FindHero(DWORD dwHeroUID);

	// ������ͨ�ٻ�
	bool DoNormalSummon(vector<DWORD> &OutHeroList, DWORD dwCostItemId = 0, int nFriendshipPoint = 0);

	// ����߼��ٻ�
	bool DoAdvanceSummon(vector<DWORD> &vOutHeroList, DWORD dwCostItemId = 0, int nCostDiamond = 0, int nSummonTimes = 1);

	// ���һ����ͨ�ٻ�Ӣ��
	DWORD RandNormalSummonHero();

	// ���һ���߼��ٻ�Ӣ��
	DWORD RandAdvancedSummonHero();

	// ���һ��VIP�ٻ�Ӣ��
	const HeroVipSummon_Config * RandVipSummonHero(WORD wCamp, bool isSpecial);

	// ȡһ���µ�ϵ��id
	DWORD GetNewSeriesId();

	// ȡӢ��������������
	int GetTrainProMax(BYTE byProType, WORD wHeroLevel);

	// ȡӢ��������������
	int GetTrainProMaxEx(BYTE byProType, WORD wProLevel);

	// ȡ�����ȼ�����
	BYTE GetTrainLevelMax(BYTE byProType);

	// ���ͻ�����ĵ�Ӣ��
	bool CheckBreakCostHero(HeroInfo *pBreakHero, DWORD *pForCostHeroList, WORD wForCostHeroNum, 
		WORD wNeedToCostHeroNum, vector<DWORD> *pForCostHeroInfoIdList, WORD wCostType);

	// ��¼Ӣ��ͻ�����ĵ�Ӣ��
	void RecordBreakCostHero(HeroInfo *pBreakHero, DWORD dwCostHeroInfoId);

private:
	DWORD m_dwSeriesId; // ϵ��ID
	Player *m_pPlayer;
	PlayerHeroInfo *m_pPlayerHeroInfo;

	// Ӣ��ͻ����������
	enum
	{
		SAME_NAME = 1,						// ͬ��Ӣ�� 
		SAME_QUALITY = 2,					// ͬƷ��Ӣ�� 
		SAME_QUALITY_AND_PROFESSION = 3,	// ͬƷ��ְͬҵӢ�� 
	};
};