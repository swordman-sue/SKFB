/**\file
���Թ�����
\author Kevin
\date 2016/11/14
\note
*/

#pragma once
#include "DataDefine.h"
class Player;


// ������ԴID(�����߼�ʹ�ã�ֻ����Ϊ���㶨λ����)
enum PRO_FORM_ID
{
	PRO_FROM_TITLE = 1,					// �ƺ�����
	PRO_FROM_SPRITE,					// ��������
	PRO_FROM_NOBILITY,					// ��λ����
	PRO_FROM_TEAM_BREAK,				// Ӣ��ͻ�ƶ�������
	PRO_FROM_ZHANBU,					// ռ������
	PRO_FROM_HERO_BASE,					// Ӣ�ۻ�������
	PRO_FROM_HERO_BREAK,				// Ӣ��ͻ������
	PRO_FROM_HERO_TRAIN,				// ��������
	PRO_FROM_HERO_DESTINY,				// ��������
	PRO_FROM_HERO_AWAKEN,				// ��������
	PRO_FROM_HERO_EVOLUTION,			// ��������	
	PRO_FROM_HERO_RELATION,				// ����Ӣ������
	PRO_FROM_EQUIM_RELATION,			// ����װ������
	PRO_FROM_TREASURE_RELATION,			// ������������
	PRO_FROM_CARD_RELATION,				// ������������
	PRO_FROM_WORLD_HEART,				// ����֮��
	PRO_FROM_SPRITE_POKEDEX,			// ����ͼ��
	PRO_FROM_GUILD_SKILL,				// ���Ἴ��
	PRO_FROM_EQUIP_ENHANCE,				// װ��ǿ������
	PRO_FROM_EQUIP_REFINE,				// װ����������	
	PRO_FROM_EQUIP_TALENT,				// װ���츳����
	PRO_FROM_EQUIP_ENHANCE_FORGE,		// װ��ǿ����������
	PRO_FROM_EQUIP_REFINE_FORGE,		// װ��������������
	PRO_FROM_EQUIP_SUIT,				// װ����װ����
	PRO_FROM_EQUIP_STARUP,				// װ����������
	PRO_FROM_TREASURE_ENHANCE,			// ����ǿ������	
	PRO_FROM_TREASURE_REFINE,			// ���ﾫ������
	PRO_FROM_TREASURE_ENHANCE_FORGE,	// ����ǿ����������	
	PRO_FROM_TREASURE_REFINE_FORGE,		// ���ﾫ����������
	PRO_FROM_GEM_INIT,					// ��ʯ��ʼ����
	PRO_FROM_GEM_SKILL,					// ��ʯ��������
	PRO_FROM_CARD_BASE,					// ���ƻ�������
	PRO_FROM_CARD_LEVEL,				// ���Ƶȼ�����
	PRO_FROM_CARD_BREAK,				// ����ͻ������
	PRO_FROM_SPRITE_BLESS_BASE,			// ����ף����������
	PRO_FROM_SPRITE_BLESS_CONVERSION,	// ����ף��ת������	
	PRO_FROM_TALENT_SKILL_CONVERSION,	// �츳�������Ծ���ף��ת��	
	PRO_FROM_REINFORCEMENT_ENCOURAGE,	// Ԯ����������
};

class CPropertyManager
{
public:
	CPropertyManager();
	~CPropertyManager();

	void Init(Player *pPlayer);

	// ȡ��ս��
	DWORD GetAllFightingPower();

	// ��������
	void  CalculateProperty();

	// ����ս��
	DWORD CalculateFightingPower();

	// ������Ͳ�λ�����б�
	bool FillSlotProList(BYTE bySlot, Property *pProList, BYTE &byProNum);

	// ȡ���Ͳ�λ�����б�
	const int* GetSlotProList(BYTE bySlot);

	// Ӱ��Ӣ��ս��: Ӣ������ + װ�� + ��װ + Ӣ����� + ����

public:
	// Ѫ��ս��=Ѫ������*Ѫ��ս��ϵ��*����ս��ϵ��*��ս��ϵ��*����ս��ϵ��*����ս��ϵ��
	static float CalculateBloodBattleValue(int nBlood, int nDodgeRate, int nBlockRate, int nBlockRateCorrect,
		int nCritDefenceRate, int nCritDefenceRateCorrect, int nDamageDecreaseRate);

	// ����ս��=����*����ս��ϵ��*����ս��ϵ��*����ս��ϵ��*�˺��ӳ�ս��ϵ��*�ٷֱȴ�͸ս��ϵ��+
	// �˺�����*�˺�����ս��ϵ��+�̶�ֵ��͸*�̶�ֵ��͸ս��ϵ��
	static float CalculateAttackBattleValue(int nAttack, int nHitRate, int nCritRate, int nCritRateCorrect,
		int nDamageIncreaseRate, int nPenetrationRate, int nDamageIncreaseValue, int nPenetrationValue);

	// ����ս��=���*���ս��ϵ��+����*����ս��ϵ��+�̶�ֵ����*�̶�ֵ����ս��ϵ��
	static float CalculateDefenceBattleValue(int nPhysicalDefence, int nMagicDefence, int nDamageDecreaseValue);

	// Ӣ��ս��=Ѫ��ս��+����ս��+����ս��
	//static float CalculateHeroBattleValue(const HeroInfo *pHeroInfo);

	// ����ս��=�����ɫս���ܺ�

	// for test 
	void PrintHeroPro();
	DWORD GetTotalHP();
protected:
	// �������
	void AddProperty(BYTE bySlot, const vector<Property> *pProList, WORD wFromId);

	// �������
	void AddProperty(BYTE bySlot, const Property &tProperty, WORD wFromId);

	// ����Ӣ������
	void CalculateHeroProperty(BYTE bySlot);

	// ����װ������
	void CalculateEquipmentProperty(BYTE bySlot);

	// ���㱦������
	void CalculateTreasureProperty(BYTE bySlot);

	// ���㱦ʯ����
	void CalculateGemProperty(BYTE bySlot);

	// ���㿨������
	void CalculateCardProperty(BYTE bySlot);

	// ����ӳ�����
	void CalculateIncProperty(BYTE bySlot);

	// ����ƺ�����
	void CalculateTitleProperty();

	// �����λ����
	void CalculateNobilityProperty();

	// Ӣ��ͻ�ƶ�������
	void CalculateHeroBreakTeamProperty();

	// ����ռ������
	void CalculateZhanBuProperty();

	// ����Ԯ����������
	void CalculateReinforcementEncourageProperty();

protected:

	// ���㾫������(�����������Ӣ����Ӱ��)
	void CalculateSpriteProperty();

	// ���㾫���������(ǿ��,����)
	void CalculateSpriteBaseProperty(vector<Property> &vProList, const SpiritData *pSpriteData);

	// ���㾫���츳��������
	void CalculateSpriteTalentSkillProperty();

	// ���㾫��ף������(ֻ�Ա�ף����Ӣ����Ӱ��)
	void CalculateSpriteBlessProperty(BYTE bySlot);


private:
	Player *m_pPlayer;
	PlayerHeroInfo *m_pHeroInfo;

	int	m_arProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX];			// ���������б�
	
	// ��sӢ����Ч��
	vector<Property> m_vTitleProList;					// �ƺ�����
	vector<Property> m_vSpriteProList;					// ��������
	vector<Property> m_vNobilityProList;				// ��λ����
	vector<Property> m_vHeroBreakTeamProList;			// Ӣ��ͻ�ƶ�������
	vector<Property> m_vZhanBuProList;					// ռ������
	vector<Property> m_vReinforcementEncourageProList;	// Ԯ����������

	map<WORD, WORD> m_GemSkillList;	// ��ʯ�����б�


	vector<Property> m_vSpriteTalentSkillProList;	// �����츳��������

	DWORD m_dwTotalHP;
};