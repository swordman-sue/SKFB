#ifndef _HERO_DATA_DEFINE_H_
#define _HERO_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// ��������
struct Formation
{
	DWORD dwHeroUID;	// �����ϵ�Ӣ��ID
	DWORD arEquipmentList[EQUIPMENT_TYPE_MAX];	// װ���б�
	DWORD arTreasureList[TREASURE_TYPE_MAX];	// �����б�	
	DWORD arGemList[MAX_GEM_HOLE_NUM];			// ��ʯ�б�
	DWORD arCardList[MAX_CARD_SLOT_NUM];		// �����б�
	DWORD dwBlessSpriteId;						// ף������ID(0=��ʾľ��)
};

// ����װ������
struct FormationEquipment
{
	WORD wInfoId;			// װ����ϢID
	WORD wStrengthenLevel;	// ǿ���ȼ�
	BYTE byRefineLevel;		// �����ȼ�
};

// ���ͱ�������
struct FormationTreasure
{
	WORD wInfoId;			// ��ϢID
	WORD wStrengthenLevel;	// ǿ���ȼ�
	BYTE byRefineLevel;		// �����ȼ�
};

// �ͻ�����������
struct ClientFormation
{
	BYTE bySlot;		// ��λ
	WORD wHeroInfoId;	// Ӣ����ϢID
	BYTE byAwakenStar;	// �����Ǽ�
	WORD wBreakLevel;	// ͻ�Ƶȼ�
	BYTE byQaulity;		// Ʒ��
	FormationEquipment arEquipmentList[EQUIPMENT_TYPE_MAX];
	BYTE byEquipmentNum;
	FormationTreasure arTreasureList[TREASURE_TYPE_MAX];
	BYTE byTreasureNum;
};

// �ͻ���Ӣ�ۻ�������
struct ClientHeroBaseData
{
	DWORD dwUID;		// ID	
	WORD wInfoId;		// ����ID
	BYTE byLevel;		// �ȼ�
	BYTE byQuality;		// Ʒ�ʵȼ�
	BYTE byQualityStage;// Ʒ�ʽ׶�
	BYTE byDestinyLevel;// �����ȼ�
	BYTE byAwakenStar;	// �����Ǽ�
	BYTE byAwakenLevel;	// ���ѵȼ�
	BYTE byBreakLevel;	// ͻ�Ƶȼ�
	BYTE byIsToBattle;	// �ͷ�������
	DWORD dwExp;		// ��ǰ����
// 	BYTE byTrainType;	// ��������
// 	BYTE byTrainLevel;	// �����ȼ�
	//int arTrainProValueList[HERO_TRAIN_PRO_NUM];	// �����ۻ�����ֵ�б�
	Property arTrainProList[HERO_TRAIN_PRO_NUM];
	BYTE arTrainProLvList[HERO_TRAIN_PRO_NUM];		// �������Եĵ�ǰ�ȼ��б�
	// ����װ���ĵ����б�
	WORD arAwakenItemList[MAX_AWAKEN_EQUIP_ITEM_NUM];
	WORD wAwakenItemNum;
};

// Ӣ����������
struct HeroTrainPro
{
	BYTE byProType;		// ��������
	BYTE byProLevel;	// ���Եȼ�
	int nProValue;		// ����ֵ
	int nTmpProValue;	// ������ʱֵ(���һ�������Ľ��,��0��ʾ��ľ���滻)
};


// Ӣ����Ϣ
struct HeroInfo
{
	DWORD dwUID;			// ΨһID
	DWORD dwIntoId;			// ����ID
	WORD wLevel;			// �ȼ�
	DWORD dwExp;			// ����
	WORD wQuality;			// Ʒ��
	WORD wQualityStage;		// Ʒ��
	WORD wBreakLevel;		// ͻ�Ƶȼ�
	WORD wDestinyLevel;		// �����ȼ�(Ǳ��)
	DWORD dwDestinyExp;		// ��������(Ǳ��)
	WORD wAwakenStar;		// �����Ǽ�
	WORD wAwakenLevel;		// ���ѵȼ�

	// ����װ���ĵ����б�
	WORD arAwakenItemList[MAX_AWAKEN_EQUIP_ITEM_NUM];
	WORD wAwakenItemNum;

	// �������
	BYTE arTrainProLvList[HERO_TRAIN_PRO_NUM];		// �������Եĵ�ǰ�ȼ��б�
	int arTrainProValueList[HERO_TRAIN_PRO_NUM];	// �����ۻ�����ֵ�б�
	int arLastTrainTmpProList[HERO_TRAIN_PRO_NUM];	// �ϴ���������������б�
	BYTE arTrainRewardFlag[HERO_TRAIN_PRO_NUM];		// ����������־�б�(1=��ʾ����ȡ,0=����ȡ��ð����ȡ)

	// ��Ϊ����ʹ��
	DWORD dwTrainCostItemNum;	// �������ĵ���Ʒ����
	DWORD dwDestinyCostItemNum;	// �������ĵ���Ʒ����
	int nTrainCostDiamond;		// ����������ʯ
	int nTrainCostGold;			// �������Ľ��
 	WORD arBreakCostHeroList[MAX_HERO_BREAK_COST_HERO_NUM];// toto::ͻ�����ĵ�Ӣ���б� ĿǰӢ�۵���ϢId���ᳬ�����ֽ�
 	WORD wBreakCostHeroNum;

	void Init()
	{
		memset(this, 0, sizeof(HeroInfo));
	}
};

// Ӣ��
struct ClientHero : public HeroInfo
{
	BYTE byTrainType;	// ��������
	BYTE byTrainLv;		// �����ȼ�
};

// ���Ӣ����Ϣ
struct PlayerHeroInfo
{
	WORD wNormalFreeSummonTimes;	// ��ͨ����ٻ�����
	DWORD dwNormalFreeSummonTime;	// ��ͨ����ٻ�ʱ��
	DWORD dwHighFreeSummonTime;	// �߼�����ٻ�ʱ��
	WORD wHighSummonTimes;		// �߼��ٻ�����
	WORD wTrainTimes;			// ��������

	HeroInfo arHeroList[MAX_HERO_BAG_NUM];	// Ӣ���б�
	WORD wHeroNum;							// Ӣ���б�����

	// ���������б�
	Formation arFormationList[MAX_BATTLE_HERO_NUM];

	// Ԯ���б�
	DWORD arReinforcementList[MAX_REINFORCEMENT_NUM];

	// Ӣ���ռ��б�
	DWORD arCollectionList[MAX_HERO_NUM];
	WORD wCollectionNum;
};



#pragma pack(pop)
#endif
