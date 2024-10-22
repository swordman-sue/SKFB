#pragma once

#include "Const.h"
#include "DataDefine.h"

#pragma pack(push,1)

/*---------------------------------------------------------------------------------------------------*/
enum DBCFILE_KEY
{
	DBC_SERVER_CONFIG,					// ����������
	DBC_LV_EXP_ENTRY,
	DBC_MONSTER_ENTRY,
	DBC_EMAILSQL_ENTRY,
	DBC_EMAIL_ENTRY,
	DBC_TIMES_CONFIG,
	DBC_CHAT_ENTRY,
	DBC_STOP_MESSAGE,

	DBC_PLAYERLEVEL_ENTRY,				// ����������������
	DBC_ROLE_INIT,						// �½���ɫ��ʼ������
	DBC_BROADCAST,					// �㲥����	
	DBC_MONSTER,					// ��������
	DBC_HTTP_URL,
};

enum
{
	CONFIG_NORMAL_FREE_SUMMON_TIMES = 1,				// ÿ��ɽ�����ͨ����ٻ��Ĵ���
	CONFIG_NORMAL_SUMMON_CD,							// ��ͨ�ٻ�CD(��λ:��)
	CONFIG_ADVANCED_SUMMON_CD,							// �߼��ٻ�CD(��λ:��)
	CONFIG_NORMAL_SUMMON_COST_ITEM,						// ��ͨ�ٻ����ĵ���ƷID
	CONFIG_ADVANCED_SUMMON_COST_ITEM,					// �߼��ٻ����ĵ���ƷID
	CONFIG_ADVANCED_SUMMON_COST_YB,						// �߼��ٻ����ĵ�Ԫ��
	CONFIG_ADVANCED_ONEKEY_SUMMON_COST_YB,				// �߼������ٻ����ĵ�Ԫ��(һ���ٻ�)
	CONFIG_ADVANCED_ONEKEY_SUMMON_TIMES,				// �߼������ٻ��Ĵ���(һ���ٻ�)
	CONFIG_NORMAL_ONEKEY_SUMMON_TIMES,					// ��ͨ�����ٻ��Ĵ���(һ���ٻ�)
	CONFIG_ADVANCED_SUMMON_SPECIAL_TIMES,				// �߼��ٻ��س�ָ��Ӣ����Ҫ�Ĵ���
	CONFIG_HERO_BREAK_LEVEL_LIMIT,						// Ӣ��ͻ�Ƶȼ�����
	CONFIG_HEROSOUL_SHOP_REFRESH_FREE_TIMES = 49,		// �ۻ��̵�ÿ�����ˢ�´���
	CONFIG_HERO_SHOP_REFRESH_RECOVER_INTERVAL = 46,		// �ۻ��̵����ˢ�»ָ����ʱ��
	CONFIG_SHOP_REFRESH_ITEM_ID = 47,					// ˢ������ƷID
	CONFIG_HERO_SHOP_REFRESH_COST_HEROSOUL = 48,		// �ۻ��̵�ˢ�����ĵ��ۻ�ֵ		
	CONFIG_GUILD_CREATE_COST_GOLD = 114,				// ����������Ҫ�Ľ��
	CONFIG_GUILD_DISMISS_MEMBER_NUM_LIMIT = 42,			// �����ɢ��Ա��������
	CONFIG_AWAKEN_SHOP_REFRESH_COST_HEROSOUL = 60,		// �����̵�ˢ�����ĵ��ۻ�ֵ		
};

// ��������
struct CostData
{
	WORD wType;
	int nValue;
};

// ��������
struct Cost_Config
{
	vector<CostData> vCostList; // �����б�
};


//�ȼ��������ñ�
struct LvExpEntry
{
	DWORD		m_dwLv;								//�ȼ�
	DWORD		m_dwNextLvExp;						//��һ�����辭��
	DWORD		m_dwVocationTitle;					//ְҵ�ƺŵȼ�
	DWORD		m_dwHp;								//��ʼHP
};

 struct MonsterEntry
 {
 	DWORD		m_dwMonsterID;						//����ID
 	char*		m_szMonsterName;					//������
 	DWORD		m_dwMonsterRace;					//��������
 	DWORD		m_dwMissionMap;						//���������ͼ
 	DWORD		m_dwMonsterLv;						//����ȼ�
 	DWORD		m_dwHp;								//Hp
 	DWORD		m_dwGMoney;							//����ͭǮ
 	DWORD		m_dwRMoney;							//��������
 	DWORD		m_dwRMoneyOdds;						//����������
 	DWORD		m_dwExp;							//��������
 	DWORD		m_dwChaosDust;						//��������֮��
 	DWORD		m_dwCompoundToken;					//����������
 	DWORD		m_dwPrimordialLight;				//��������֮��
 	//RewardCard	m_tRewardCard[MAX_REWARD_CARD];		//��������
 	char*		m_szScript;							//�ű��ļ�
// 	DWORD		m_dwCardID[MAX_BATTLE_CARD_NUM];	//ս������ID
 };


//�ʼ��������ݿ����ñ�
struct SendEmailEntry
{
	DWORD		m_dwID;                             //ID
	char*		m_szSqlQuery;						//SQL���
	INT			m_nGender;							//��ɫ�Ա�
	INT			m_nVocation;						//��ɫְҵ
	INT			m_nRace;							//��ɫ����
	INT			m_nLv;								//��ɫ�ȼ�
	INT			m_nAddRMoney7Day;					//7�����
	INT			m_nAddRMoney30Day;					//30�����
};

//�ʼ��������ñ�
struct EmailEntry
{
	DWORD	dwID;         		// ID
	char*	pszTitle;			// ����
	char*	pszWord;			// ����
	char*	pszSender;			// ������
	DWORD	dwValidTime;		// �ʼ���Чʱ��(0=��ʾ��ʱ������) ��λ:��
	DWORD 	dwExp;				// ����
	DWORD 	dwGold;				// ͭǮ
	DWORD 	dwYB;				// Ԫ��
	DWORD 	dwPhysicalStrength;	// ����
	DWORD 	dwHonor;			// ����
};

//����ϵͳ��������ʱ������
struct SystemTimes
{
	INT			ID;
	INT			IsIng;
	INT			BeginHour;
	INT			BeginMinute;
	INT			EndHouse;
	INT			EndMinute;
	INT			IsUpdate;
	INT			UpdateHour;
	INT			UpdateMinute;
	CHAR*		Name;								//��Ϊ���� �߻������õ�
};

//�������ýṹ
struct ChatEntry
{
	INT			ID;									//ID
	INT			SystemInterval;						//ϵͳƵ��������ʱ��(��)
	INT			WorldInterval;						//����Ƶ��������ʱ��(��)
	INT			PersonalInterval;					//˽��Ƶ��������ʱ��(��)
};
struct StopMsg
{
	DWORD dwID;
	char* MsgInfo;							
};

//���������ñ�
struct RobotEntry
{
	DWORD		ID;
	char*		m_szAccID;							//AccID
	char*		m_szCharName;						//��ɫ��
	DWORD		m_dwLevel;							//�ȼ�
};
struct RobotLoginEntry
{
	DWORD		ID;
	DWORD		m_dwBattleType;						//ս������
	char*		m_szFileName;						//�������ļ���
};

struct ZoneEntry
{
	DWORD				m_dwZoneID;
	DWORD				m_dwServerID;
};


struct OpenServer_Config
{
	DWORD dwKey;
	DWORD dwValue;
	char *pszValue;
	char *pszDesc;
};

// �̵���Ʒ����
struct ShopGoodsData_Config
{
	WORD wGoodsId;			// ��ƷID
	BYTE byType;			// ����
	DWORD dwEntityId;		// ��Ʒʵ��ID
	DWORD dwEntityNum;		// ��Ʒʵ������
	WORD wBuyNumMax;		// ������������(0=��ʾ������)

	WORD wNeedRoleLevel;	// ��Ҫ�Ľ�ɫ�ȼ�
	int nNeedVipLevel;		// ��Ҫ��VIP�ȼ�
	WORD wNeedGuildLevel;	// ��Ҫ�Ĺ���ȼ�
	
// 	int nCostGold;			// ���Ľ�Ǯ
// 	int nCostDiamond;		// ���ĵ���ʯ

	// �����б�
	vector<CostData> vCostList;	// ��Ʒ�����б�(����)

	// �����б�
	vector<Item> vCostItemList;	// ��Ʒ�����б�(��Ʒ)
	
	int nContext;			// ������
	int nPriceListType;		// �۸��б�����
	vector<int> vPriceList;	// �۸��б�
	int nVipIdx;			// ��Ӧ�޹�vip����ID(0=��ʾľ������)

	// �����̵�����
	int nSpecialCostValue;		// �������ĵ�ֵ
	WORD wRefreshRate;			// ˢ������

	WORD wRefreshType;		// ˢ������(0=��ˢ��(0��) 1 = ����ˢ��(��һ0��) 2 = ��ˢ��(1��0��))

	void Init()
	{
		nSpecialCostValue = 0;
		nPriceListType = 0;
		wGoodsId = 0;
		byType = 0;
		dwEntityId = 0;
		wNeedRoleLevel = 0;
		wBuyNumMax = 0;
		nNeedVipLevel = 0;
		nContext = 0;
		nVipIdx = 0;
		wRefreshType = 0;
		vPriceList.clear();
		vCostList.clear();
		vCostItemList.clear();
	}
};

// ����������������
struct PlayerLevelExp
{
	DWORD m_dwLv;		// �ȼ�
	DWORD m_dwNeedExp;	// ��һ�����辭��
};


// ��ֵ��Ʒ���� 
// struct RechargeItem_Config
// {
// 	DWORD dwGoodsId;			//��ƷID
// 	DWORD dwCardType;			//��ø���������(0=��ʾľ��)
// 	DWORD dwGoodsPrice;			//��Ʒ�۸�
// 	DWORD dwGoods2YB;			//��Ʒ�һ�Ԫ��
// 	DWORD dwFirstExtraReward;	//�׳���⽱��Ԫ��
// };

// ����������
struct Server_Config
{
	DWORD dwKey;
	DWORD dwValue;
	string strValue;
	vector<DWORD> vValueList;

	void Init()
	{
		vValueList.clear();
	}

	DWORD GetListValue(int nIndex) const
	{
		if (!vValueList.size())
			return 0;
		return nIndex < vValueList.size() ? vValueList[nIndex] : vValueList[vValueList.size() - 1];
	}

	bool IsInList(DWORD dwValue) const
	{
		for (size_t i = 0; i < vValueList.size(); i++)
		{
			if (vValueList[i] == dwValue)
				return true;
		}
		return false;
	}
};

// ��������Ϣ����
struct ServerInfo_Config
{
	DWORD dwInitServerId; // ��ʼ������ID
	DWORD dwCurServerId; // ��ǰ������ID
	vector<DWORD> vServerRecordList; // �������Ϸ���¼�б� 
};

// �½���ɫ��ʼ������
struct RoleInit_Config
{
	DWORD dwDataId;				// ����ID
	int nHeadImage;				// ͷ��
	vector<DWORD> vHeroList;	// ��ʼӢ���б�(���ֱ������)
	vector<Item> vItemList;		// ��Ʒ�б�
	vector<Property> vProList;	// �����б�(��������)

// 	DWORD dwGold;		// ͭǮ
// 	DWORD dwYB;			// Ԫ��
// 	DWORD dwPower;		// ����	
// 	DWORD dwExp;		// ����
// 	DWORD dwHonor;		// ����
// 	char *pszHeroList;	// Ӣ���б�
};

// ʱ���¼�����
// struct TimeEvent_Config : public TimeEventData
// {
// 
// };

typedef TimeEventData TimeEvent_Config;

// ǩ������
struct SignIn_Config
{
	WORD wDay;					// ǩ������
	Item tRewardItem;			// ��������Ʒ
	BYTE byDoubleRewardVipLv;	// ˫��������Ҫ��VIP�ȼ�(0=��ʾľ��˫������)
};

// ӭ��������
struct YCS_Config
{
	WORD wRoleLevel;				// ��ɫ�ȼ�
	BYTE byMoneyType;				// �����Ļ�������
	vector<DWORD>	vMoneyValueList;// ��ȡ���������Ļ���ֵ�б�
};

// ͭȸ̨����
struct TQT_Config
{
	WORD wId;
	int nStartTime;	// ��ʼʱ��
	int nOverTime;	// ����ʱ��
};

// �㲥����
struct Broadcast_Config
{
	DWORD dwBroadcastId;	// �㲥ID
	char *pszText;			// �㲥����
};

// Http����url����
struct HttpUrl_Config
{
	DWORD dwUrlId;		// url��Ӧ��ID
	char *pszUrlDesc;	// URL����
	char *pszUrl;		// URL
};

// ��������Ʒ
struct GiftRandItem
{
	DWORD dwItemId;	// ��ƷID
	WORD wRate;		// ����
	WORD wNumMin;	// ��������
	WORD wNumMax;	// ��������
};

// ��Ʒ����
struct Item_Config
{
	DWORD dwId;				// ��ƷID
	BYTE byType;			// ��Ʒ����
	BYTE bySubType;			// ������
	DWORD dwPrice;			// ����۸�
	DWORD dwSellPrice;		// ���ۼ۸�
	BYTE byIsBroadcasting;	// �Ƿ�㲥
	BYTE byIsUseInGet;		// �Ƿ�������ʹ��
	WORD wMaxNum;			// �ѵ���������
	WORD wUseLevel;			// ʹ�õȼ�
	BYTE byIsCanUse;		// �ܷ�ֱ��ʹ��
	DWORD dwExpiryDate;		// ��Ч����(������ ���� ��������  ��:20171019 �� 9 )
	int nParam1;			// ����1
	int nParam2;			// ����2
	vector<GiftRandItem> vGiftItemList;	// �����Ʒ�б�

	// ���ѵ���ʹ��
	vector<Item> vComposeCostItemList;	// �ϳ����ĵ���Ʒ�б�
	int nDecomposeGodSoul;				// �ֽ��õ����
	vector<Property> vProList;			// Ӱ��������б�

	void Init()
	{
		vGiftItemList.clear();
		vProList.clear();
		vComposeCostItemList.clear();
	}

	DWORD GetTotalRate() const
	{
		DWORD dwTotalRate = 0;
		for (int i = 0; i < vGiftItemList.size(); ++i)
		{
			dwTotalRate += vGiftItemList[i].wRate;
		}
		return dwTotalRate;
	}

	const GiftRandItem* GetGiftItemByRate(DWORD dwRate) const
	{
		DWORD dwCurRate = 0;
		for (int i = 0; i < vGiftItemList.size(); ++i)
		{
			dwCurRate += vGiftItemList[i].wRate;
			if (dwRate <= dwCurRate)
			{
				return &vGiftItemList[i];
			}
		}
		return NULL;
	}

	const GiftRandItem* GetGiftItemId(DWORD dwGiftItemId) const
	{
		for (int i = 0; i < vGiftItemList.size(); ++i)
		{
			if (vGiftItemList[i].dwItemId == dwGiftItemId)
			{
				return &vGiftItemList[i];
			}
		}
		return NULL;
	}
};

// װ������
struct Equipment_Config
{
	DWORD dwId;					// װ��ID
	BYTE byType;				// װ������
	WORD wQuality;				// װ��Ʒ��
	DWORD dwPrice;				// ��Ǯ(���)
	int nTowerPrestige;			// ��������
	WORD wStrengthLevelMax;		// ǿ���ȼ����� 
	WORD wRefineLevelMax;		// �����ȼ�����
	int nResetCostDiamond;		// ����������ʯ
	WORD wTalentId;				// �츳ID(0=��ʾľ��)
	WORD wStarMax;				// ��������(0��ʾ��������)
	DWORD dwStarUpCostItemId;	// ����������ƬID
	WORD wNextQualityEquipmentId; // ��һƷ�ʵ�װ��ID(0��ʾ������)


	Property tStrengthenInitPro;	// ǿ����ʼ����
	Property tStrengthenLevelPro;	// ǿ���ȼ�����(ÿһ����)

	vector<Property> vRefineLevelProList;	// �����ȼ������б�
};

// װ��ǿ���ȼ�����
struct EquipmentEnhanceLevel_Config
{
	int nCostGold;			// ���Ľ��
	int nTotalCostGold;		// �����Ľ��
};

// װ�������ȼ����� 
struct EquipmentRefineLevel_Config
{
	BYTE byQuality;			// Ʒ��
	WORD wLevel;			// �����ȼ�
	DWORD dwExp;			// ����
	DWORD dwTotalExp;		// �ܾ���
};

// װ���ϳ�����
struct EquipmentCompose_Config
{
	DWORD dwFragmentId;		// ��ƬID
	WORD wFragmentNum;		// ��Ƭ����
};

// װ������
struct EquipmentForge_Config
{
	WORD wLevel;				// �ȼ�(ǿ��,����)
	vector<Property> vProList;	// Ӱ��������б�

	void AddProperty(const Property &sPro)
	{
		vector<Property>::iterator iter = vProList.begin();
		for (; iter != vProList.end(); ++iter)
		{
			if (iter->wType == sPro.wType)
			{
				iter->nValue += sPro.nValue;
				return;
			}
		}

		vProList.push_back(sPro);
	}
};

// ��װ����
struct EquipmentSuit_Config
{
	DWORD dwSuitId;	// ��װID
	vector<DWORD> vEquipmentList;		// װ���б�
// 	vector<Property> vProListForTwo;	// ��������
// 	vector<Property> vProListForThree;	// ��������
// 	vector<Property> vProListForFour;	// �ļ�����
	map<DWORD, vector<Property>> vProList;	// �����б�	
};

// װ���츳
struct EquipmentTalent_Config
{
	WORD wId;	// �츳Id
	WORD wLevel; // �ȼ�
	WORD wNeedEnhanceLevel; // ��Ҫ��ǿ���ȼ�
	vector<Property> vProList;	// ��õ������б�
};

// װ������
struct EquipmentStarUp_Config
{
	WORD wType;					// װ������
	WORD wQuality;				// װ��Ʒ��	
	WORD wStar;					// �Ǽ�	
	WORD wLevel;				// �ȼ�	
	bool bIsStarUp;				// �Ƿ�����	
	Item tCostItem;				// ���ĵ���
	DWORD dwCostSelfNum;		// ���Ķ�Ӧ��Ƭ����	
	int nCostGold;				// ���Ľ������	
	vector<Property> vProList;	// �����б�
};

// װ����Ʒ����
struct EquipmentQualityUp_Config
{
	WORD wQuality;				// װ��Ʒ��	
	Item tCostItem;				// ���ĵ���
	CostData tCostCurrency;		// ���Ļ���
};

// Ԯ����������
struct ReinforcementEncourage_Config
{
	WORD wReinforcementLevel;	// Ԯ���ȼ�
	WORD wEncourageLevel;		// �����ȼ�
	vector<Property> vProList;  // ��õ������б�
};


// �������
struct TreasureForge_Config
{
	WORD wLevel;				// �ȼ�(ǿ��,����)
	vector<Property> vProList;	// Ӱ��������б�

	void AddProperty(const Property &sPro)
	{
		vector<Property>::iterator iter = vProList.begin();
		for (; iter != vProList.end(); ++iter)
		{
			if (iter->wType == sPro.wType)
			{
				iter->nValue += sPro.nValue;
				return;
			}
		}

		vProList.push_back(sPro);
	}
};

// ����Ӣ������
struct RelationHeroProperty
{
	vector<DWORD> vHeroList;	// Ӣ���б�
	vector<Property> vProList;	// ��õ�����
};

// ������������
struct RelationCardProperty
{
	vector<DWORD> vCardTypeList;	// ���������б�
	vector<Property> vProList;		// ��õ�����
};

// ����װ������
struct RelationEquipmentProperty
{
	vector<DWORD> vEquipmentList;	// װ���б�(���߹�ϵ,ֻҪ����һ���Ϳ���)
	vector<Property> vProList;		// ��õ�����
};

// ������������
struct RelationTreasureProperty
{
	vector<DWORD> vTreasureList;	// �����б�(���߹�ϵ,ֻҪ����һ���Ϳ���)
	vector<Property> vProList;		// ��õ�����
};

// Ӣ������
struct Hero_Config
{
	BYTE byProfession;				// ְҵ
	BYTE bySex;						// �Ա�
	BYTE byHeroType;				// Ӣ������(1=��ͨӢ�� 2=����Ӣ��)
	WORD wInitLevel;				// ��ʼ�ȼ�
	WORD wLimitLevel;				// �ȼ�����(��ʱ��������)
	DWORD dwToExpAsCostItem;		// ��Ϊ��������Ʒ��õľ���
	DWORD dwNeedGoldAsCostItem;		// ��Ϊ��������Ʒʱ��Ҫ�Ľ��	
	vector<DWORD> SkillList;		// �����б�
	WORD wInitQuality;				// Ӣ�۳�ʼƷ��
	WORD wQualityMax;				// Ӣ��Ʒ������
	WORD wQualityStageMax;			// Ӣ��Ʒ������
	int nHeroSoul;					// ���ջ�õ��ۻ�
	int nResetCostDiamond;			// ����������ʯ
	int nPrice;						// ���ۼ۸�
	int nQualifications;			// Ӣ������

	vector<RelationHeroProperty> vRelationHeroList;				// ����Ӣ���б�
	vector<RelationEquipmentProperty> vRelationEquipmentList;	// ����װ���б�
	vector<RelationTreasureProperty> vRelationTreasureList;		// ���������б�
	vector<RelationCardProperty> vRelationCardList;				// ���������б�

	vector<RelationEquipmentProperty> vRelationEquipmentListEx;	// ����װ���б�(��ǰΪ��,��Ʒ�ʵ�Ӣ��)
	vector<RelationTreasureProperty> vRelationTreasureListEx;	// ���������б�(��ǰΪ��,��Ʒ�ʵ�Ӣ��)
};

// Ӣ����������
struct HeroPro_Config
{
	//DWORD dwHeroId;			// Ӣ��ID
	BYTE byProfession;		// ְҵ
	BYTE byQuality;			// Ʒ��
	WORD wLevel;			// Ӣ�۵ȼ�
	vector<Property> vProList;
// 	int nBloodMax;			// Ѫ������
// 	int nAttack;			// ����
// 	int nPhysicalDefence;	// ���
// 	int nMagicDefence;		// ����
};

// Ӣ�۵ȼ�����
struct HeroLevel_Config
{
	WORD wLevel;
	DWORD dwExp;		// ������Ҫ�ľ���
	DWORD dwTotalExp;	// �ܾ���
};

// Ӣ�ۺϳ�����
struct HeroCompose_Config
{
	DWORD dwItemId;		// ��Ҫ�ĵ���ID
	WORD wItemNum;		// ��Ҫ�ĵ�������
};

// ��ɫ�ȼ�����
struct RoleLevel_Config
{
	WORD wLevel;			// �ȼ�
	DWORD dwExp;			// ��Ҫ�ľ���
	DWORD dwStamina;		// �������ֵ
	DWORD dwEnergy;			// ��þ���ֵ
	DWORD dwStaminaMax;		// ����ֵ����
	DWORD dwEnergyMax;		// ����ֵ����
	DWORD dwWantedMax;		// ͨ��������
	DWORD dwFriendNumMax;	// ������������
	DWORD dwBlessSpriteNumMax;	// ף��������������
	DWORD dwDailyChatTimesMax;	// ÿ�������������
	vector<Item> vRewardItemList; // ������������Ʒ�б�
};

// ��ɫ���Իָ�
struct RoleProRecover_Config
{
	BYTE byProType;		// ��������
	int nIntervalTime;	// �ָ����ʱ��
	int nRecoverValue;	// ÿ�λָ���ֵ
};

// Ӣ����ͨ�ٻ�
struct HeroNormalSummon_Config
{
	DWORD dwHeroId;
	DWORD dwRate;
};

// Ӣ�۸߼��ٻ�
struct HeroAdvancedSummon_Config
{
	DWORD dwHeroId;
	DWORD dwRate;
	DWORD dwRate1;
};

// Ӣ��VIP�ٻ�
struct HeroVipSummon_Config
{
	DWORD dwHeroId;
	DWORD dwHeroNum;
	DWORD dwNormalRate;
	DWORD dwSpecialRate;
};

// Ӣ��ͻ����������
struct HeroBreakLevelUp_Config
{
	WORD wNeedHeroLevel;				// ��Ҫ��Ӣ�۵ȼ�
	int nCostGold;						// ��Ҫ���ĵĽ��
	DWORD dwCostItemId;					// ��Ҫ���ĵ���ƷID(ͻ��ʯ)
	WORD wCostItemNum;					// ��Ҫ���ĵ���Ʒ����(ͻ��ʯ)
	DWORD dwCostHeroId;					// ��Ҫ���ĵ�Ӣ��ID(todo::�Ѳ�ʹ��)
	WORD wNeedHeroQuality;				// ��Ҫ��Ӣ��Ʒ��
	WORD wCostSameNameHeroNum;			// ��Ҫ���ĵ�ͬ��Ӣ������(0��ʾ������)
	WORD wCostSameQualityHeroNum;		// ��Ҫ���ĵ�ͬƷ��Ӣ������(0��ʾ������)
	WORD wCostSameProfessionHeroNum;	// ��Ҫ���ĵ�ְͬҵӢ������(0��ʾ������)

	void Init()
	{
		memset(this, 0, sizeof(HeroBreakLevelUp_Config));
	}
};

// Ӣ��ͻ������
struct HeroBreak_Config
{
	DWORD dwHeroId;
	WORD wBreakLevel;
	vector<Property> vSelfProList;
	vector<Property> vTeamProList;	// ��������
};

// Ӣ�۽���
struct HeroEvolution_Config
{
	WORD wQuality;					// Ʒ��
	WORD wStage;					// Ʒ��
	bool isTheLastStage;			// �Ƿ����һ��
	vector<Item> vCostItemList;			// ������Ʒ�б�
	vector<CostData> vCostCurrencyList; // ���Ļ����б�
	int nCostGold;						// ���Ľ��
	vector<Property> vProList;			// �����б�

	WORD wAddQualifications;			// ��ӵ�����
	WORD wNeedBreakLevel;				// ��ҪӢ�۽��׵ȼ�
	WORD wNeedEnhanceLevel;				// ��ҪӢ��ǿ���ȼ�
};

// Ӣ��������������
struct HeroTrainLimit_Config
{
	vector<Property> list;
};

// Ӣ��ͼ��
struct HeroPokedex_Config
{
	WORD wCamp;		// ��Ӫ
	WORD wQuality;	// Ʒ��
	vector<DWORD> vHeroIdList; // Ӣ��Id�б�
};

// Ӣ��������������
// struct HeroTrainCost_Config
// {
// 	DWORD dwItemId;		// ������ƷID
// 	WORD wItemNum;		// ������Ʒ����
// 	DWORD dwGold;		// ���Ľ��
// 	DWORD dwYB;			// ����Ԫ��
// };

// Ӣ����������
struct HeroTrain_Config
{
	BYTE byProType;		// ��������
	BYTE byProLevel;	// ���Եȼ�
	WORD wNeedHeroLevel;// ��Ҫ��Ӣ�۵ȼ�
	int nProMax;		// ��������
	vector<Property> vLevelUpProList; // ���������б�(�ۻ�����)
};

// Ӣ����������
// struct HeroTrain_Config
// {
// 	BYTE byProfession;			// ְҵ
// 	BYTE byType;				// ��������
// 	BYTE byLevel;				// �����ȼ�
// 	WORD wNeedHeroLevel;		// ��Ҫ��Ӣ�۵ȼ�
// 	WORD wCostItemId;			// ���ĵ���ƷID
// 	WORD wCostItemNum1;			// ���ĵ���Ʒ����1
// 	WORD wCostItemNum2;			// ���ĵ���Ʒ����2
// 	int nCostDiamond;			// ������ʯ
// 	int nDefaultRate;			// Ĭ�ϳɹ���
// 	int nRateFromFail;			// ʧ�ܻ�õĳɹ�����
// 	int nRateMax;				// ��������
// 	vector<Property> vProList;	// Ӱ��������б�
// 
// 	void Init()
// 	{
// 		vProList.clear();
// 	}
// };

// Ӣ��������������
struct HeroTrainRateData
{
	WORD wMinRate;
	WORD wMaxRate;
	int nProp;
};

// Ӣ�����������������
struct HeroTrainRandPro_Config
{
	BYTE byProType;	// ��������
	int nMin;		// �������
	int nMax;		// �������
};

// Ӣ��������������
struct HeroTrainCost_Config
{
	BYTE byCostType;		// ��������
	Item tCostItem;			// ������Ʒ
	MoneyData tCostMoney;	// ���Ļ���
};

// Ӣ�����������������
struct HeroTrainRandProp_Config
{
	//map<DWORD, vector<HeroTrainRateData>> list;
	vector<HeroTrainRateData> list;
};

// Ӣ����������
struct HeroDestiny_Config
{
	DWORD dwNeedExp;					// �������辭��(0��ʾ��������)
	DWORD dwAddExp;						// ÿ��ע��ľ���
	DWORD dwCostItemId;					// ���ĵ���ƷID
	WORD wCostItemNum;					// ���ĵ���Ʒ����
	DWORD dwSkillId;					// �����ļ���ID
	vector<RateData> vLevelUpRateList; 	// ֱ�������ĸ�������
	vector<Property> vProList;			// Ӱ��������б�

	const RateData* GetRateData(DWORD dwValue) const
	{
		const RateData *pTemp = NULL;
		vector<RateData>::const_iterator iter = vLevelUpRateList.begin();
		for (; iter != vLevelUpRateList.end(); ++iter)
		{
			if (dwValue >= iter->dwMin && dwValue <= iter->dwMax)
			{
				pTemp = &*iter;
				break;
			}
		}
		return pTemp;
	}
};

// Ӣ�۾�������
struct HeroAwaken_Config
{
	BYTE byInitQuality;			// ��ʼ��Ʒ��
	BYTE byProfession;			// ְҵ
	BYTE byStar;				// ��
	BYTE byLevel;				// �ȼ�
	WORD wNeedHeroLevel;		// ��ҪӢ�۵ȼ�
	int nCostMoneyType;			// ���Ļ�������
	int nCostMoneyValue;		// ���Ļ���ֵ
	Item tCostItem;				// ���ĵ���Ʒ(���ѵ�) ����ʱ����
	WORD wCostHeroNum;			// ����ͬ��Ӣ������(0=��ʾ������)
	vector<DWORD> vCostItemList;// ������Ʒ�б�(������Ʒ) װ������
	vector<Property> vProList;	// Ӱ��������б�(����ǰ�׶ε�������,������������)
	vector<Property> vItemProList;	// ���ϼӵ������б�
};

// ��������
struct Treasure_Config
{
	WORD wInfoId;				// ��ϢID
	WORD wQuality;				// Ʒ��
	BYTE byType;				// ����
	WORD wStrengthenLevelMax;	// ǿ���ȼ�����
	WORD wRefineLevelMax;		// �����ȼ�����
	DWORD dwAsExp;				// ��Ϊ����
	int nPrice;					// �۸�
	int nResetCostDiamond;		// ����������ʯ

	DWORD dwNextQualityTreasureId;	// ��һ��Ʒ�ʱ���ID(0��ʾ��������)
	DWORD dwQualityUPCostItemId;	// ��Ʒ���ĵĵ���ID

	vector<Property> vStrengthenInitProList;	// ǿ����ʼ�����б�
	vector<Property> vStrengthenLevelProList;	// ǿ���ȼ������б�(ÿһ����)

	vector<Property> vRefineLevelProList;	// �����ȼ������б�
};

// ����ǿ������
struct TreasureEnhance_Config
{
	DWORD dwNeedExp;		// ��Ҫ�ľ���(0��ʾ����������)
	int wCostGold;		// ǿ�����Ľ��
	DWORD dwTotalExp;	// �ܵ��ۻ�����
};

// ���ﾫ������
struct TreasureRefine_Config
{
	DWORD dwCostItemId;			// ���ĵ���ƷID
	WORD wCostItemNum;			// ���ĵ���Ʒ����
	//DWORD dwCostTreasureId;		// ���ĵı���ID
	BYTE byCostTreasureNum;		// ���ĵı�������
	int nCostGold;			// ���ĵĽ��
};

// ����ϳ�����
struct TreasureCompose_Config
{
	vector<DWORD> cost_item_list;

	void Init()
	{
		cost_item_list.clear();
	}

	// �Ƿ�������Ʒ
	bool IsCostItem(WORD wItemId) const
	{
		for (int i = 0; i < cost_item_list.size(); ++i)
		{
			if (cost_item_list[i] == wItemId)
			{
				return true;
			}
		}
		return false;
	}
};

// ��������
struct TreasureSmelt_Config
{
	DWORD dwTreasureInfoId;
	DWORD dwCostTreasureInfoId;	// ���ĵı���ID
	DWORD dwItemId;				// ������õ���ƷID
};

// ������Ʒ��������
struct TreasureQualityUPCost_Config
{
	WORD wRefineLevel; // �����ȼ�
	DWORD dwCostItemNum; // ���ĵĵ�������
	vector<CostData> vCostCurrencyList; // ���Ļ��������б�
};

// ��������������
struct ArenaRank_Config
{
	WORD wRank;					// ����
	WORD wTargetRankInterval;	// ��սĿ���������
	int nAwardPrestige;			// ��������������
	int nAwardGold;				// �������
	int nFirstRankDiamond;		// �״����н���
};

// ��������������
struct ArenaAward_Config
{
	WORD wRoleLevel;
	DWORD dwExp;			// ���齱��
	int nWinGold;			// ��ʤ��ҽ���
	int nFailGold;			// ʧ�ܽ�ҽ���
	int nWinArenaPrestige;	// ��ʤ��������������
	int nFailArenaPrestige;	// ʧ�ܾ�������������
	WORD wFPItemId;			// ������ƷID
};

// ����������������
struct ArenaRobot_Config
{
	DWORD dwRobotId; // ������Id
	WORD wRank;	// ����
	string strRobotName; // ����������
};

// �ɾ�����
struct Achievement_Config
{
	// �ȼ�����
	struct LvData
	{
		WORD wLv;					// �ȼ�
		WORD wNeedRoleLv;			// ��Ҫ�Ľ�ɫ�ȼ�
		DWORD wValue;				// ��Ҫ��������ֵ
		Item tRewardItem;			// ������Ʒ
		//vector<Item> vAwardList;	// �����б�
	};

	WORD wAchievementId;			// �ɾ�ID
	vector<LvData> vLvDataList;		// �ɾ͵ȼ������б�

	const LvData* GetLvData(WORD wLv) const
	{
		for (size_t i = 0; i < vLvDataList.size(); ++i)
		{
			if (vLvDataList[i].wLv == wLv)
			{
				return &vLvDataList[i];
			}
		}
		return NULL;
	}
};

// 7��Ŀ����������
struct SevenDayTarget_Config
{
	WORD wTargetId;					// Ŀ��ID
	WORD wLogicType;				// �߼�����
	int nX;							// ����X
	int nY;							// ����Y
	BYTE byIsTheSameDayValid;		// �Ƿ�����Ч
	BYTE byIsFromTheSameDayValid;	// �Ƿ��쿪ʼ��Ч
	BYTE byIsCompletionTarget;		// �Ƿ���ɶ�Ŀ��
	BYTE byIsAsc;					// �Ƿ�����
	WORD wBelongActivityDay;		// ���������
	vector<Item> vRewardItemList;	// ������Ʒ�б�
};

// 7����ɶȽ���
struct SevenDayCompletionReward_Config
{
	WORD wCompletionNum;		
	vector<Item> vRewardItemList;	// ������Ʒ�б�
};

// �ճ���������
struct DailyMission_Config
{
	WORD wMissionId;	// ����Id
	WORD wNeedRoleLv;	// ��Ҫ�Ľ�ɫ�ȼ�
	DWORD dwValue;		// ����ֵ
	WORD wPoint;		// ��õĻ���
	DWORD dwBaseExp;	// ��õĻ�������ֵ
	//Item tAwardItem;	// ��������Ʒ
	vector<Item> vRewardItemList; // ������Ʒ�б�
};


// ��ɫ�ȼ��ճ���������
struct RoleLvDailyMission_Config
{
	// ��������
	struct PointData
	{
		WORD wPoint;		// ����
		WORD wRewardItemId;	// ��������ƷId
	};

	WORD wLevel;
	vector<DWORD> vMissionList;		// �����б�
	vector<PointData> vPointList;	// �����б�

	const PointData* GetPointData(WORD wPoint) const
	{
		for (size_t i = 0; i < vPointList.size(); ++i)
		{
			if (vPointList[i].wPoint == wPoint)
				return &vPointList[i];
		}
		return NULL;
	}

	// �Ƿ�����
	bool IsMission(WORD wMissionId) const
	{
		for (size_t i = 0; i < vMissionList.size(); ++i)
		{
			if (vMissionList[i] == wMissionId)
				return true;
		}
		return false;
	}
};

// ������������
struct StarAwardData
{
	BYTE byStar;
	//DWORD dwBoxId;
	vector<Item> vBoxList;
};

// �����½�����
struct FubenChapter_Config
{
	DWORD dwChapter;						// �½�
	vector<DWORD> vMapList;					// �ؿ��б�
	vector<DWORD> vLevelLimitList;			// �ؿ��ȼ������б�
	vector<StarAwardData> vStarAwardList;	// ���������б�
	vector<DWORD> vOpenWeekdayList;			// ���������б�
	WORD wOpenLevel;						// ���ŵȼ�
	WORD wExtraMapId;						// ����ؿ�
	DWORD dwNeedRoleLevel;					// ��Ҫ�Ľ�ɫ�ȼ�

	// �ճ�����ʹ��
	WORD wDailyBattleTimes;		// ÿ�տ���ս����(0��ʾ������)
	BYTE byIsDamageReward;		// �Ƿ��˺�����
	BYTE byWinCondition;		// ��ʤ���� 1=�ҷ�ʣ��Ѫ��������x % 2=�ҷ���������������x 3=x���ڻ�ʤ 4=ɱ���Է����е�λ
	BYTE byIsLoseAlsoHaveReward;// �Ƿ�ʧ��Ҳ�н���
	DWORD dwSweepRewardContext;	// ɨ������������


	// ���鸱��ʹ��
	vector<DWORD> vEnemyMapList;				// ��йؿ��б�
	vector<DWORD> vEnemyMapRateList;			// ��йؿ������б�
	WORD wEnemyRandNumMin;		// ��������������
	WORD wEnemyRandNumMax;		// ��������������
	WORD wEnemyRandChapterMin;	// �������½�����
	WORD wEnemyRandChapterMax;	// �������½�����


	DWORD GetMapId(BYTE byMapIndex) const
	{
		if (!byMapIndex || byMapIndex > vMapList.size())
			return 0;

		return vMapList[byMapIndex - 1];
	}

	// �Ƿ񿪷ŵ���
	bool IsOpenDay(BYTE byDay) const
	{
		for (int i = 0; i < vOpenWeekdayList.size(); ++i)
		{
			if (vOpenWeekdayList[i] == byDay)
				return true;
		}
		return false;
	}

	void Init()
	{
		vMapList.clear();
		vStarAwardList.clear();
		vOpenWeekdayList.clear();
		wDailyBattleTimes = 0;
		byIsDamageReward = false;
	}
};

// �ճ������˺�����
struct DailyFBDamageReward_Config
{
	WORD wChapter;		// �½�(��������)
	WORD wMapIdx;		// �ؿ�����(�Ѷ�����)
	//WORD wDamageRate;	// �˺�����(�ٷֱ�)
	WORD wContext;		// �����ļ�(�˺�����,����ʱ��)
	//BYTE byIsSweepReward;	// �Ƿ�ɨ������

	vector<Item> vItemList;	// ������Ʒ�б�
};

// ���߸����½ڽ���
struct MainFBChapterReward_Config
{
	WORD wLevel;				// �����ȼ�
	WORD wNeedChapter;		// ��Ҫ���½�
	DWORD dwItemId;			// ��ƷID
	//vector<Item> vItemList;	// ������Ʒ�б�
};

// ��������
struct DropData
{
	//BYTE byType;			// ����
	WORD wRate;				// �������
	DWORD dwDropObjectId;	// �������ID
	WORD wDropObjectNum;	// �����������
};

// ��ʱ��������
struct LimitDrop_Config
{
	DWORD dwDropItemId;	// ������ƷID
	DWORD dwStartDate;	// ��ʼ����
	DWORD dwOverDate;	// ��������
	DWORD dwDropRate;	// ���伸��
	DWORD dwDropNumMin;	// ������������
	DWORD dwDropNumMax;	// ������������ 
};

// �����ؿ�����
struct Map_Config
{
	DWORD dwMapId;
	BYTE byDailyBattleTimesLimit;	// ÿ�տ�����ս����
	vector<DWORD> vBattleList;		// ս���б�
	BYTE byIsCanComment;			// �Ƿ������
	DWORD dwNeedMinPower;			// ��ս��Ҫ��С��ս��

	// ����
	vector<CostData> vCostList;				// �����б�

	// ��ʤ����
	vector<Item> vFirstWinItemList;			// ��ʤ������Ʒ�б�
	vector<Property> vFirstWinCurrencyList;	// ��ʤ���������б�

	// ��ͨ����
	vector<Property> vNormalRewardList;		// ��ͨ���������б�
	vector<RewardItem> vRandRewardList;		// ��ͨ������Ʒ�б�(���ֹؿ���Ʒ���������)

	// ����,����
	vector<Item> vBoxList;					// ���佱����Ʒ�б�
	vector<DropData> vDropList;				// ���佱����Ʒ�б�
	
	int GetCostValue(WORD wCostType) const
	{
		for (size_t i = 0; i < vCostList.size(); ++i)
		{
			if (vCostList[i].wType == wCostType)
				return vCostList[i].nValue;
		}
		return 0;
	}
};


// ������������
struct TowerStarAward
{
	BYTE byStar;				// ����
	vector<Item> vAwardList;	// �����б�
};

// �����½�
struct TowerChapter_Config
{
	BYTE byChapter;								// �½�
	vector<WORD> vMapList;						// �����Ĺؿ��б�
	vector<TowerStarAward> vAwardList;			// �����б�

	const TowerStarAward* GetAward(BYTE byStar) const
	{
		for (size_t i = vAwardList.size() - 1; i >= 0; --i)
		{
			if (byStar >= vAwardList[i].byStar)
			{
				return &vAwardList[i];
			}
		}
		return NULL;
	}
};

// ������ͨ�ؿ�
struct TowerMap_Config
{
	struct MapData
	{
		WORD wMapId;	// �ؿ�ID
		BYTE byStar;		// ��������
	};

	WORD wMapLevel;					// �ؿ��ȼ�
	WORD wChapter;					// �����½�
	bool bIsLast;					// �Ƿ�����½�һ��
	vector<MapData> MapDataList;	// �ؿ������б�
};

// ������Ӣ�ؿ�
struct TowerEliteMap_Config
{
	WORD wMapLevel;				// �ؿ��ȼ�
	WORD wMapId;				// �ؿ�ID
	WORD wNeedNormalMapLevel;	// ��Ҫ��ͨ�����Ĺؿ��ȼ�
};

// ���������������
struct TowerRandPro_Config
{
	BYTE byStar;
	vector<Property> vProList;
};

// ����������Ʒ
struct TowerStarGoods_Config
{
	struct StarGoods
	{
		WORD nId;
		WORD wNum;
		int nPrice;
	};

	WORD wStarMin;				// ��������
	WORD wStarMax;				// ��������
	//vector<Item> vGoodsList;	// ��Ʒ�б�
	//vector<int> vPriceList;		// �۸��б�

	vector<StarGoods> vGoodsList; // ��Ʒ�б�
};

// ��ؼ���
struct LandSkill
{
	WORD wLevel;			// �ȼ�
	WORD wDoubleAwardRate;	// ˫����������
	WORD wNeedPatrolTime;	// ��Ҫ��Ѳ��ʱ��
	int nCostDiamond;		// ���ĵ���ʯ
};

// ��سǳ�����  
struct LandCity_Config
{
	WORD wCityId;					// �ǳ�ID
	WORD wPreCityId;				// ǰ�óǳ�ID
	WORD wMapId;					// �ؿ�ID
	int nSuppressEnergy;			// ��ѹ��������
	int nHelpSuppressDiamond;		// ������ѹ������ʯ
	vector<LandSkill> vSkillList;	// �����б�
};

// ���Ѳ���¼�����
struct LandPatrolEvent_Config
{
	// �¼�����
	struct EventData
	{
		WORD wEventId;		// �¼�ID
		WORD wEventRate;	// �¼�����
		DWORD dwObjId;		// ��������Id
		DWORD dwOjbNum;		// ������������
	};

	WORD wCityId;
	vector<EventData> vEventList; // �¼��б�

	const EventData* GetEventData(WORD wEventId) const
	{
		for (int i = 0; i < vEventList.size(); ++i)
		{
			if (vEventList[i].wEventId == wEventId)
			{
				return &vEventList[i];
			}
		}

		return NULL;
	}
};

// ���Ѳ������
struct LandPatrol_Config
{
	// ʱ������
	struct TimeData
	{
		BYTE byTimeType;		// ʱ������
		DWORD dwTotalTime;		// ��ʱ��(��)
		WORD wItemNumMin;		// ��Ƭ��������Fragment
		WORD wItemNumMax;		// ��Ƭ��������
		int nCostGold;			// ���Ľ��
		int nCostEnergy;		// ���ľ���
	};

	BYTE byType;					// Ѳ�߷�ʽ	
	WORD wIntervalTime;				// ����ʱ����(����)
	BYTE byVipIdx;					// ���޵�VIP����(0=��ʾľ������)
	vector<TimeData> vTimeDataList; // ʱ�������б�

	const TimeData* GetTimeData(BYTE byTimeType) const
	{
		for (int i = 0; i < vTimeDataList.size(); ++i)
		{
			if (vTimeDataList[i].byTimeType == byTimeType)
			{
				return &vTimeDataList[i];
			}
		}
		return NULL;
	}
};


// �ۻ��̵������Ʒ
 struct HeroSoulShopRandGoods
 {
 	WORD wGoodsId;		// ��ƷID
 	WORD wLevelLimit;	// �ȼ�����
 	WORD wRate;			// ����
 };

// �ۻ��̵���Ʒ����
struct HeroSoulShopGoods_Config
{
	WORD wGoodsId;		// ��ƷID
	BYTE byType;		// ��Ʒ����(1=��Ʒ,2=Ӣ��,3=��Ƭ)
	DWORD dwEntityId;	// ��Ʒʵ��ID
	WORD wEntityNum;	// ��Ʒʵ������
	WORD wLevelLimit;	// �ȼ�����
	WORD wRate;			// ����
	int nYB;			// ��Ʒ�ۼ�(Ԫ��)
	int nHeroSoul;		// ��Ʒ�ۼ�(�ۻ�)
	BYTE byIsRecommend;	// �Ƿ��Ƽ�
};

// �ۻ��̵�����
struct HeroSoulShop_Config
{
	BYTE byPos;										// λ��
	BYTE byIsRepeat;								// �Ƿ���ظ�
	vector<HeroSoulShopGoods_Config*> GoodsList;	// ��Ʒ����б�
};

// ����֮������
struct HeartOfWorld_Config
{
	struct LevelData
	{
		BYTE byLevel;			// �ȼ�
		WORD wRoleQuality;		// Ӱ�������Ʒ��
		//Property sAddPro;		// ���ӵ�����(��������|����ֵ)
		vector<Property> vAddProList; // ��ӵ������б�
		Item sAwardItem;		// ��������Ʒ
		Item sCostItem;			// ����������Ʒ(��ƷID|��Ʒ����)
	};

	BYTE byStage;
	vector<LevelData> vLeveDatalist;

	const LevelData* GetLevelData(BYTE byLevel) const
	{
		for (int i = 0; i < vLeveDatalist.size(); ++i)
		{
			if (vLeveDatalist[i].byLevel == byLevel)
				return &vLeveDatalist[i];
		}
		return NULL;
	}
};

// ��������
struct Spirit_Config
{
	DWORD dwId;			// ����ID
	WORD wPokedexId;	// ������ͼ��ID

	WORD wInitStar;			// ��ʼ����
	WORD wStarMax;			// ��������
	vector<DWORD> vQualificationsList;	// ��������
	DWORD dwTtarUpCostItemId; // �������ĵ���ƷID(��Ƭ)
	DWORD dwFragmentNum;	// ��Ƭ

	WORD GetQualifications(WORD wStar) const
	{
		if (!vQualificationsList.size())
			return 0;

		// �����ݼ���
		if (wStar < wInitStar)
			wStar = wInitStar;

		// ��ֹΪ0�����
		if (!wStar)
			return vQualificationsList[wStar];

		// ��ֹԽ��
		if (wStar >= vQualificationsList.size())
			return vQualificationsList[vQualificationsList.size()-1];

		return vQualificationsList[wStar];
	}
};

// ����ǿ������
struct SpiritEnhance_Config
{
	DWORD dwId;	// ����ID
	WORD wEnhanceLevel;	// ǿ���ȼ�
	vector<Item> vCostItemList;	// ���ĵ����б�
	vector<Property> vAddProList;// ��������б�	
};

// ��������
struct  SpriteTrain_Config
{
	DWORD dwId;				// ����ID
	WORD wStage;			// ��
	WORD wNeedEnhanceLevel;	// ��Ҫ��ǿ���ȼ�
	WORD wNeedTrainTimes;	// ������Ҫ����������(0=��ʾ�Ѵ�����)
	WORD wSkillLevel;		// ���ܵȼ�
	WORD wTalentLevel;		// �츳�ȼ�
	DWORD dwModulId;		// ����ģ��Id
	DWORD dwProConversionRate;		// ����ת����
	Item tStageUpCostItem;			// �������ĵ���Ʒ[id|num]
	vector<Item> vTrainCostItemList;// �������ĵ���Ʒ�б�
	vector<Property> vTrainProList; // ���������б�
	vector<Property> vTrainTotalProList; // �����������б�
};

// ����ͼ��
struct SpiritPokedex_Config
{
	DWORD dwPokedexId;	// ͼ��ID
	WORD wPokedexLevel; // ͼ���ȼ�
	vector<DWORD> vNeedSpiritList;	// ��Ҫ�ľ����б�
	WORD wNeedEnhanceLevel;		// ��Ҫ��ǿ���ȼ�
	Property vAddPro;			// ��õ�����
	
	//vector<WORD> vLevelUpConditionList; // ͼ�����������б�
	//int nLevelUpAddProValue;	// ������ӵ�����ֵ(ÿһ��)
};

// �����츳����
struct SpriteTalentSkill_Config
{
	DWORD dwSpriteId;							// ����ID	
	WORD wTalentSkillId;						// �츳���ܵȼ�	
	vector<Property> vHeroProList;				// ȫ���սӢ����������	ȫ���սӢ������ֵ	
	vector<Property> vSpriteProList;			// ȫ�徫����������	ȫ�徫������ֵ	
	vector<DWORD> m_vFBTypeList;				// ���������б�	
	vector<DWORD> vExtraRewardDropItemId;		// ��ö��⽱���ĵ�����ƷID	
	vector<DWORD> vExtraRewardDropItemRate;		// ��ö��⽱������Ʒ����	
	vector<DWORD> vExtraRewardCurrencyType;		// ��ö��⽱���Ļ�������	
	vector<DWORD> vExtraRewardCurrencyRate;		// ��ö��⽱���Ļ�����ֱ�

	// �Ƿ���⽱������Ʒ
	bool IsExtraRewardFB(WORD wFBType) const
	{
		for (size_t i = 0; i < m_vFBTypeList.size(); ++i)
		{
			if (m_vFBTypeList[i] == wFBType)
				return true;
		}
		return false;
	}
};

// ��������
struct SpriteStarUp_Config
{
	WORD wStar;					// ����
	DWORD dwStarUpCostItemNum;	// �����¼�����������Ƭ����
};

// ��������
struct SpriteSpecialProperty_Config
{
	DWORD dwSpriteId;	// ����ID
	WORD wStar;			// �Ǽ�
	vector<Property> vBlessProList;			// ף�������������б�
};

// ��������
struct SpriteQualifications_Config
{
	WORD wQualifications;	// ��������
	int nAddRate;			// �ӳ�ϵ��
};

// ����ģ������
struct SpiritModel_Config
{
	int nPrice;			// �۸�
	BYTE byNeedStage;	// ��Ҫ�Ľ׶�
	BYTE byNeedLevel;	// ��Ҫ�ĵȼ�
};

// ��������
struct Guild_Config
{
	WORD wLevel;		// ����ȼ�
	DWORD dwNeedExp;	// ������Ҫ�ľ���
	WORD wMaxMember;	// ��Ա��������
};


// ����Ȩ������
struct GuildPrivilege_Config
{
	bool bDismiss;		// ��ɢ
	bool bApplyCheck;	// ������
	bool bLeave;		// �˳�����
	bool bAppoint;		// �������᳤
						// ת�ƻ᳤
	bool bKickOut;		// ����
						// �����ڲ���Ϣ
						// ��������
};

// ����Ȫӿ����
struct GuildQY_Config : public Cost_Config
{
	WORD wType;						// Ȫӿ����
	WORD wCritRate;					// ��������
	DWORD dwExp;					// ��õľ���
	DWORD dwCritExp;				// ��������ֵ
	DWORD dwContribution;			// ��õĹ���ֵ
	DWORD dwCritContribution;		// ��������ֵ
	DWORD dwQuanShui;				// ��õ�Ȫˮ
	WORD wNeedVipLevel;				// ��Ҫ��VIP�ȼ�
};

// ����Ȫˮ��������
struct GuildQSAward_Config
{
	WORD wQS;			// Ȫˮ
	WORD wGuildLevel;	// ����ȼ�
	vector<Item> vItemList; // ��������Ʒ�б�
};

// ���Ἴ������
struct GuildSkill_Config
{
	WORD wSkillId;					// ����Id
	WORD wLevel;					// ���ܵȼ�
	WORD wNeedGuildLevel;			// ��Ҫ����ȼ�
	int nCostGuildExp;				// �����������ĵĹ��ᾭ��
	//CostData tCostData;				// ����ѧϰ����
	vector<CostData> vCostList;		// ����ѧϰ�����б�
	vector<Property> vAddProList;	// ��ӵ������б�

	int GetCostType(WORD wType) const
	{
		for (size_t i = 0; i < vCostList.size(); ++i)
		{
			if (vCostList[i].wType == wType)
				return vCostList[i].nValue;
		}
		return 0;
	}
};

// ���ḱ������
struct GuildFBChapter_Config
{
	WORD wChapter;						// �½�
	vector<DWORD> vMapList;				// ��ͼ�б�
	vector<Item> vMapRewardList;		// �ؿ������б�(���е���Ӫ��ʹ��ͬһ�������б�)
	vector<Item> vChapterRewardList;	// �½ڽ����б�
	int nKillExp;						// ��ɱ�������ᾭ��
	int nKillContribution;				// ��ɱ��������ֵ
	int nBattleContribution;			// ��ս��������ֵ
};

// ��������
struct Monster_Config
{
	DWORD dwMonsterId;
	DWORD dwBlood;				// Ѫ��	
	string strName;				// ��������
	vector<DropData> vDropList;	// �����б�
	void Init()
	{
		vDropList.clear();
	}
};

// ս������
struct Battle_Config
{
	DWORD dwBattleId;			// ս��ID
	//vector<DWORD> vMonsterList;	// �����б�
	DWORD arMonsterList[MAX_BATTLE_MONSTER_NUM];
	void Inti()
	{
		memset(arMonsterList, 0, sizeof(arMonsterList));
		//vMonsterList.clear();
	}

// 	void AddMonster(BYTE byIndex, WORD wMapId)
// 	{
// 		if (byIndex >= MAX_BATTLE_MONSTER_NUM)
// 			return;
// 
// 		arMonsterList[byIndex] = wMapId;
// 	}
};

// Ӣ����������
struct HeroToBattle_Config
{
	BYTE byType;
	BYTE byPos;
	WORD wNeedLevel;
};

// �ؿ�����
typedef Map_Config FubenMap_Config;



// ��������
// struct Box_Config
// {
// 	int nGold;		// ���
// 	int nExp;		// ����
// 	int nDiamond;	// ��ʯ
// 	vector<ObjectData> vOjbList; // �����б� 
// };

// �Ѿ���ѫ
struct RebelExploit_Config
{
	DWORD dwListId;		// �����б�ID
	DWORD dwAwardId;	// ����Id
	int nNeedExploit;	// ��Ҫ�Ĺ�ѫ
	WORD wAwardItemId;	// ��������ƷId
	WORD wAwardItemNum;	// ��������Ʒ����
	int nAwardGold;		// �����Ľ��
};

// �Ѿ�ս��
struct RebelBattle_Config
{
	WORD wLevel;	// �Ѿ��ȼ�	
	//WORD wMapId;	// �ؿ�ID
	vector<DWORD> vRandMapList;		// ����ؿ��б�
	vector<Property> vAddProList;	// ��ӵ������б�
};

// �Ѿ�Ʒ��
struct RebelQuality_Config
{
	BYTE byQuality;				// Ʒ��
	vector<Property> vProList;	// �����б�
	int nDiscoverAwardDiamond;	// �����߽�����ʯ
	int nKillerAwardDiamond;	// ��ɱ�߽�����ʯ	
	DWORD dwLiveTime;			// ���ʱ��

	void Init()
	{
		vProList.clear();
	}
};

// ��ɫ�Ѿ�
struct RoleRebel_Config
{
	WORD wRoleLevel;						// ��ɫ�ȼ�
	WORD wAwardListId;						// �������б�ID
	vector<RateDataEx> vRandList;			// Ʒ������б�

	void Init()
	{
		vRandList.clear();
	}

	int GetTotalRate() const
	{
		int nTotalRate = 0;
		for (int i = 0; i < vRandList.size(); ++i)
		{
			nTotalRate += vRandList[i].nRate;
		}
		return nTotalRate;
	}

	BYTE GetQuality(WORD wRate) const
	{
		int nMax = 0;
		for (int i = 0; i < vRandList.size(); ++i)
		{
			nMax += vRandList[i].nRate;
			if (nMax && wRate <= nMax)
			{
				return vRandList[i].nValue;
			}
		}
		return QUALITY_INVALID;
	}
};

// �Ѿ���������
struct RebelAttack_Config
{
	BYTE byAttackType;		// ��������
	int nStartTime;			// ��ʼʱ��
	int nOverTime;			// ����ʱ��
	int nActivityCostWanted;// ��ڼ�����ͨ����(������)
	int nActivityAwardRate;	// ���������
	int nAttackRate;		// ��������
	int nCostWanted;		// ����������ͨ����(������)

	// �Ƿ��ڻ��
	bool IsActivity(int nTime) const
	{
		return nTime >= nStartTime && nTime <= nOverTime ? true : false;
	}
};

// �Ѿ����а�������
struct RebelRankAward_Config
{
	WORD wRank;			// ����
	int nExploitAward;	// ��ѫ���н���
	int nDamageAward;	// �˺����н���
};

// ���շ�������
struct RecycleReturn_Config
{
	BYTE byObjectType;		// ���ն�������
	BYTE byObjectLevel;		// ���ն��󼶱�
	DWORD dwObjectId;		// ���շ����Ķ���Id
	DWORD dwNeedExp;		// ��Ҫ�ľ���
};

// �ᱦ�ؿ�����
struct DuobaoMap_Config
{
	WORD wRoleLevel;
	int nWinGold;			// ʤ�����
	int nFailGold;			// ʧ�ܽ��
	DWORD dwExp;			// ���齱��
	WORD wPFGitfId;			// ����������ID
	vector<DWORD> vMapList;	// �ؿ��б�

	void Init()
	{
		vMapList.clear();
	}
};

// �ᱦ��������
struct DuobaoDrop_Config
{
	BYTE byQuality;		// Ʒ��
	WORD wUserRate;		// �û�����
	WORD wRobotRate;	// �����˸���
	WORD wTimesOfSureDrop; // �ص�����(�ᱦ��һ������ʱ,һ�������)
};

// �ᱦ���Ƚ�������
struct DuobaoPregrossReward_Config
{
	WORD wId;	// ����ID	
	WORD wTimes; // �ᱦ����
	vector<Item> vRewardItemList; // ������Ʒ�б�
};


// Vip����
struct Vip_Config
{
	WORD wLevel;			// �ȼ�
	DWORD dwNeedExp;		// ������Ҫ�ľ���
	vector<int>	vVipList;	// Vip��Ȩֵ�б�
};

// VIPGM����
struct VipGM_Config
{
	WORD wGMLevel;	// VipGm�ȼ�
	DWORD dwNeedRechargeAmount;	// ��Ҫ�ĳ�ֵ���
};

// ˢ���̵��λ����
struct RefreshShopSlot_Config
{
	BYTE bySlot;								// ��λ
	//BYTE byIsCanRepeat;							// �Ƿ�����ظ�
	vector<ShopGoodsData_Config*> vGoodsList;	// ��Ʒ�б�
};

// ���Ὠ������
struct GuildBuilding_Config
{
	WORD wBuildingId; // ����ID
	WORD wNeedGuildLevel; // ��Ҫ�Ĺ���ȼ�
};

// ����BOSS
struct WorldBoss_Config
{
	WORD wBossLevel;		// BOSS�ȼ�
	WORD wMapId;			// ��Ӧ�ؿ�ID
	int nLuckRewardRate;	// ���˽�������
	Item tLuckRewardItem;	// ���˽�����Ʒ(��ʯ)
	Item tKillRewardItem;	// ���˽�����Ʒ(��ʯ)
	//WORD wKillRewardId;		// ����һ������(��Ʒ) 
};

// ����BOSS����ʱ��
struct WorldBossOpenTime_Config
{
	int nWeekDay;		// ���ڼ�
	int nStartTime;		// ��ʼʱ��[Сʱ:����]
	int nOverTime;		// ����ʱ��
	int nRewardTime;	// ����ʱ��
};

// ����BOSS���н�������
struct WorldBossRankReward_Config
{
	WORD wRank; // ����
	vector<Item> vHonorRewardList; // ���������б�
	vector<Item> vDamageRewardList; // �˺������б�
};

// ����BOSS������������
struct WorldBossHonorReward_Config
{
	WORD wRewardId;		// ����ID
	int nNeedHonor;		// ��Ҫ����
	Item tRewardItem;	// ������Ʒ
};

// ����BOSS��ɱ��������
struct WorldBossKillReward_Config
{
	WORD wRewardId;		// ����ID
	WORD nNeedBossLv;	// ��ҪBOSS�ȼ�
	Item tRewardItem;	// ������Ʒ
};

// ����BOSSˢ��ʱ������
struct WorldBossRefreshTime_Config
{
	
};

// ��������
struct Card_Config
{
	DWORD dwInfoId;	// ��ϢID
	BYTE byType;	// ����
	BYTE byQuality;	// Ʒ��
	DWORD dwToExp;	// ��Ϊ�������ϻ�õľ���
	DWORD dwDecomposeReturnStarSoul; // �ֽⷵ���ǻ�
	int nResetCostDiamond;	// ����������ʯ
	vector<Property> vBaseProList;	// ���������б�
	vector<Property> vLevelProList;	// �ȼ������б�
	vector<Property> vBreakProList;	// ͻ�������б�
};

// ����ؿ�����
struct CardGroupMap_Config
{
	WORD wMapIdx;		// �ؿ�����
	DWORD dwMapId;		// �ؿ�ID
	WORD wCardGroupId;	// ����ID
	BYTE byIsTheLastMap;// �Ƿ����Ĺؿ�
};

// �߼���������
struct HighCardGroup_Config
{
	DWORD dwCardId;		// ����ID
	int nNormalWeight;	// ����Ȩֵ
	int nMustWeight;	// ����Ȩֵ
};

// �ղ�������
struct CollectionGroup_Config
{
	WORD wGroupId; // ��ID
	vector<DWORD> vCostCardList;	// ���ĵĿ����б�
	vector<Property> vActivateProList;	// ���������б�
	vector<Property> vUpgradeProList;	// ���������б�
	int nActivateCollectionValue; // �����õ��ղ�ֵ
	int nUpgradeCollectionValue;	// ������õ��ղ�ֵ
};

// �ղ�������
struct CollectionBook_Config
{
	WORD wBookId; // �鱾ID
	int nNeedCollectionValue; // ��Ҫ���ղ�ֵ
	vector<DWORD> vCollectionGroupList; // �ղ����б�
};

// ռ���ɾ�
struct ZhanBuAchievement_Config
{
	WORD wLevel; // �ȼ�
	DWORD dwNeedCollectionValue; 
	vector<Property> vProList; // ��õ������б�
};

// ������������
struct CardLevelUp_Config
{
	BYTE byQuality;		// Ʒ��
	WORD wLevel;		// �ȼ�
	DWORD dwExp;		// ��������
	DWORD dwTotalExp;	// �����ܾ���
	WORD wNeedRoleLevel;// ��Ҫ��ɫ�ȼ�
};

// ����ͻ������
struct CardBreak_Config
{
	BYTE byQuality;		// Ʒ��
	WORD wBreakLevel;	// ͻ�Ƶȼ�
	WORD wNeedCardLevel;	// ��Ҫ���Ƶȼ�
	WORD wCostSameCardNum;	// ����ͬ��(ͬ��)��������
	vector<CostData> vCostMoneyList; // ���ĵĻ����б�
	vector<Item> vCostItemList;  // ���ĵ���Ʒ�б�
};

// ��������
struct CardSlot_Config
{
	WORD wCardSlot; // ����
	WORD wNeedRoleLevel; // ��Ҫ�Ľ�ɫ�ȼ�
};


// ������������
struct ScoreMatchRobot_Config
{
	DWORD dwRobotId;		// ������Id
	DWORD dwFightingPower;	// ս��
	WORD wServerId;			// ��װ���ڵķ�����
};

// ��������ս��������
struct ScoreMatchBattleTask_Config
{
	WORD wTaskId;			// ����ID
	WORD wBattleTimes;		// ��Ҫ����ս����
	vector<Item> vRewardItemList; // ������Ʒ�б�
};

// ���������н���
struct ScoreMatchRankReward_Config
{
	WORD wRank;	// ����
	vector<Item> vRewardItemList;	// ������Ʒ�б�
	int nTitleId;	// �����ĳƺ�ID
};

// �������ʱ��
struct ScoreMathActivityTime_Config
{
	WORD wIndex;		// �˳������
	DWORD dwStartDate;	// ��ʼ����
	DWORD dwStartTime;	// ��ʼʱ��
	DWORD dwOverDate;	// ��������
	DWORD dwOverTime;	// ����ʱ��
};

// ����������Ŀ������
struct ScoreMatchSearchTarget_Config
{
	WORD wType;			// ����
	WORD wTargetNum;	// ����Ŀ������
	vector<DWORD> vBattleValueRateMinList;	// ս���������ޱ����б�
	vector<DWORD> vBattleValueRateMaxList;	// ս���������ޱ����б�
};

// ������������������
struct ScoreMatchServerGroup_Config
{
	DWORD dwServerId;
	WORD wRankGroupId;			// ���а���ID(����)
	vector<DWORD> vGroupList;	// ���������б�
}; 

// ��������������
struct WarcraftRank_Config
{
	WORD wRank;					// ����
	WORD wTargetRankInterval;	// ��սĿ���������
	int nAwardPrestige;			// ��������������
	int nAwardGold;				// �������
};

// �ƺ�����
struct Title_Config
{
	int nTitleId;		// �ƺ�ID
	int nType;			// ����(1=�ƺ�,2=�ǳ�)
	int nValidTime;		// ��Чʱ��(����,0=������Ч)
	vector<Property> vProList; // ��õ������б�
};


// ��������
struct Reward_Config
{
	int nId;	// ������ϢID
	int nType;	// ��������(��Ϊ�����������߼�,����ͨ��)
	vector<Item> vItemList;	// ������Ʒ�б�
};

// ����û���
struct ActivityBase_Config
{
	int nTimeType;	// ʱ������
	int nDays;		// �������������Ч(��)

	vector<DWORD> vOpenServerList; // ���ŵķ������б�(0=��ʾ���з���������)

	int nStartDate;			// ��ʼ����(��/��/��)
	int nStartTime;			// ��ʼʱ��(Сʱ/����)
	int nOverDate;			// ��������(��/��/��)
	int nOverTime;			// ����ʱ��(Сʱ/����)

	int nRewardOverDate;	// ������������(��/��/��)
	int nRewardOverTime;	// ��������ʱ��(Сʱ/����)

	bool IsOpenServer(DWORD dwServerId) const
	{
		if (!vOpenServerList.size())
			return true;

		for (int i = 0; i < vOpenServerList.size(); ++i)
		{
			if (vOpenServerList[i] == dwServerId)
				return true;
		}
		return false;
	}
};

// �����
struct Activity_Config : public ActivityBase_Config
{
	int nSystemId; // ϵͳID
	WORD wDataId;	// ����Id
// 	int nTimeType;	// ʱ������
// 	int nDays;		// �������������Ч(��)
	WORD wNeedRoleLevel;	// ��Ҫ��ɫ�ȼ�
//	vector<DWORD> vOpenServerList; // ���ŵķ������б�(0=��ʾ���з���������)
// 	int nStartDate;			// ��ʼ����(��/��/��)
// 	int nStartTime;			// ��ʼʱ��(Сʱ/����)
// 	int nOverDate;			// ��������(��/��/��)
// 	int nOverTime;			// ����ʱ��(Сʱ/����)
// 	int nRewardOverDate;	// ������������(��/��/��)
// 	int nRewardOverTime;	// ��������ʱ��(Сʱ/����)
};

// ���ջ����
struct FestivalActivity_Config : public ActivityBase_Config
{
	int nActivityType;// �����
// 	int nTimeType;	// ʱ������
// 	int nDays;		// �������������Ч(��)
//	vector<DWORD> vOpenServerList; // ���ŵķ������б�(0=��ʾ���з���������)
// 	int nStartDate;			// ��ʼ����(��/��/��)
// 	int nStartTime;			// ��ʼʱ��(Сʱ/����)
// 	int nOverDate;			// ��������(��/��/��)
// 	int nOverTime;			// ����ʱ��(Сʱ/����)
// 	int nRewardOverDate;	// ������������(��/��/��)
// 	int nRewardOverTime;	// ��������ʱ��(Сʱ/����)	
	vector<DWORD> vParamList;// ������б�
};

// ��ʱ�����
struct LimitActivity_Config
{
	WORD wDataId;					// ����ID
	WORD wType;						// ��ʱ�����
	DWORD dwContext;				// ������
	WORD wRewardTimesMax;			// ������������
	vector<Item> vRewardItemList;	// ������Ʒ�б�
};

// ���ջĿ��
struct FestivalActivtiyTarget_Config
{
	WORD wTargetId;					// Ŀ��ID
	WORD wLogicType;				// �߼�����
	int nX;							// ����X
	int nY;							// ����Y
	BYTE byIsAsc;					// �Ƿ�����(��ʱ������)
	WORD wRewardTimes;				// ��������
	BYTE byIsDailyReset;			// �Ƿ�ÿ������
	BYTE byIsStageTarget;			// �Ƿ�׶�Ŀ��
	BYTE byIsNeedTake;				// �Ƿ���Ҫ��ȡ
	vector<Item> vRewardItemList;	// ������Ʒ�б�	

	// 	BYTE byIsTheSameDayValid;		// �Ƿ�����Ч
	// 	BYTE byIsFromTheSameDayValid;	// �Ƿ��쿪ʼ��Ч
	//	BYTE byIsCompletionTarget;		// �Ƿ���ɶ�Ŀ��
	//	WORD wBelongActivityDay;		// ���������
};

// VIPÿ����� 
struct VipDailyGift_Config
{
	WORD wVipLevel;		
	vector<Item> vItemList;	// ��������Ʒ�б�
};

// VIP�����
struct VipWeekGift_Config
{
	WORD wGiftId;
	WORD wBuyNumMax;			// ������������
	WORD wNeedVipLevel;			// ��ҪVIP�ȼ�
	vector<CostData> vCostList; // �����б�
	vector<Item> vItemList;		// ��Ʒ�б�
};

// VIP����������
struct VipWeekGiftRandList_Config
{
	WORD wGiftRandNum;					// ����������
	vector<RateDataEx> vGiftRandList;	// �������б�
};

// �����Ƽ���ֵ����
struct TodayRecommendRecharge_Config
{
	WORD wGiftId;	// ���ID
	WORD wVipLevel;	// VIP�ȼ�
	int nRate;	// �������
	WORD wRechargeId;	// ��ֵID
	vector<Item> vItemList;	// ��Ʒ�б�
};


// �ȼ��̵�����
struct LevelShopData_Config
{
	WORD wRoleLevel;
	int nValidTime;				// ��Чʱ��
	vector<DWORD> vGoodsList;	// ��Ʒ�б�
};

// ��������������
struct OpenServerFundReward_Config
{
	WORD wRewardId;			// ����ID
	WORD wNeedRoleLevel;	// ��Ҫ�Ľ�ɫ�ȼ�
	int nDiamond;			// ��������ʯ
};

// ȫ��������
struct AllPeopleWelfare_Config
{
	WORD wWelfareId;		// ����ID
	WORD wNeedBuyNum;		// ��Ҫ���������
	vector<Item> vItemList; // ��������Ʒ�б�
};

// ��ֵ
struct Recharge_Config
{
	WORD wId;			// ��ֵID
	//int nAmount;		// ��Ҫ�������(Ԫ)
	int nRMB;			// ��Ҫ�������(Ԫ)
	int nUSD;			// ��Ҫ����Ԫ(��)
	int nYCoin;			// Խ�ϰ汾ʹ�õĻ���
	string strName;	// ��Ʒ����
	//vector<Item> vRewardItemList;	// ��ֵ�����б�(�Ѳ�ʹ��)
	int nFirstRechargeDiamond;		// �׳佱����ʯ
	int nRechargeDiamond;		// ��õ���ʯ
	int nVipExp;		// ��õ�VIP����
	int nExtraDiamond;	// ���⽱������ʯ
	string strRechargeStrId; // ��ֵ�ִ�ID
};

// �¿�����
struct MonthCard_Config
{
	WORD wCardId;			// ��ID
	int nSellPrice;			// ���ۼ۸�(�����)
	int nSellPrice_USD;		// ���ۼ۸�(����)
	int nBuyRewardDiamond;	// ��������������ʯ
	int nDailyRewardDiamond;// ÿ��������ʯ
	int nValidTime;			// ��Чʱ��(����,0=��ʾ����)
	int nVipExp;			// ��õ�VIP����
	Item tRewardItem;		// ������Ʒ
	string strName;			// �¿�����
	string strRechargeStrId; // ��ֵ�ִ�ID
};

// ��ʱ�Ż��̵�
struct LimitPreferentialShop_Config
{
	WORD wRoleLevelMin;			// �ȼ�����
	WORD wRoleLevelMax;			// ��ɫ�ȼ�����
	WORD wRandGoodsNum;			// �����Ʒ����
	vector<DWORD> vGoodsList;	// ��Ʒ�б�
};

// ��ʱ�Żݳ�ֵ
struct LimitPreferentialRecharge_Config
{
	WORD wRechargeId;	// ��ֵID(��Ӧ��ֵ���ñ�ID)
	int nRate;			// �������
	int nRewardDiamond; // ������ʯ
	int nValidTime;		// ��Чʱ��(��)
};

// ��ʱ�Żݸ���
struct LimitPreferentialWelfare_Config
{
	WORD wWelfareId;		// ����ID
	WORD wNeedBuyTimes;		// ��Ҫ�������
	vector<Item> vItemList; // ��������Ʒ�б�
};

// �ȼ��������
struct LevelGift_Config
{
	WORD wLevel;
	vector<Item> vItemList; // ��������Ʒ�б�
};

// ���߽���ʱ����������
struct OnlineRewardTime_Config
{
	DWORD dwTime; // ʱ��(��)
	vector<Item> vRewardList; // �����б�
};

// ���߽�������
struct OnlineReward_Config
{
	WORD wDays; // ����
	vector<OnlineRewardTime_Config> vTimeDataList; // ʱ�������б�
};

// 7���¼����
struct SevenDayLogin_Config
{
	WORD wDays; // ����
	vector<Item> vRewardList; // �����б�
};


// ����������
struct ActivationCode_Config
{
	DWORD dwId;	// ID
	string strCode;	// ������
	DWORD dwRewardId; // ����ID
	//DWORD dwItemId; // �����������ƷID
};

// �����뽱������
struct ActivationCodeReward_Config
{
	WORD wRewardId;
	DWORD dwItemId;	// �����������ƷID
	int nValidDate;	// ��Ч����[��:��:��]
	int nValidTime; // ��Чʱ��[Сʱ:����]
};

// ����ͳ������
struct ReyunStatistic_Config
{
	WORD wType;	// ��Ϊ����
	string strPath;	// ·��
	string strContext; // ����
};

// PVP��ս����
struct PVPBattleReward_Config
{
	WORD wBattleTimes;	// ս������
	WORD wType;			// ����(1=ÿ��,2=ÿ��)
	vector<Item> vItemList;	// ������Ʒ�б�
};

// PVP��������
struct PVPRankReward_Config
{
	WORD wMinRank;	// ��������
	WORD wMaxRank;	// ��������
	vector<Item> vItemList;
	DWORD dwTitleId; // �ƺ�Id
};

// PVP�ʱ������
struct PVPActivityTime_Config
{
	DWORD dwWeekDay; // �ܼ�
	vector<TimeData> vTimeList; // ʱ���б�
// 	int nStartTime;	// ��ʼʱ��
// 	int nOverTime;	// ����ʱ��
};

// �����½�(��)����
struct AttackCityChapter_Config
{
	WORD wChapter; // �½�
	BYTE byIsBoss; // �Ƿ�boss
	WORD wResetInitChapter; // ����ʱ��ʼ����
	WORD wRefreshMonsterNum; // ˢ������
	vector<DWORD> vMapList;	// �ؿ��б�
	vector<DWORD> vMapRateList;	// �ؿ������б�
	
	vector<Property> vMonsterProList; // ���������б�
	WORD wRefreshItemNumMin;	// ˢ������������
	WORD wRefreshItemNumMax;	// ˢ������������
	DWORD dwRefreshItemId;	// ˢ���ı���Id(��ƷId)

	WORD wBossItemNumMin;	// Boss����������
	WORD wBossItemNumMax;	// Boss������������
	DWORD dwBossItemId;	// Boss�ı���Id(��ƷId)

	DWORD dwBattleScore;	// ��ս����
	DWORD dwKillScore;	// ��ɱ����
	vector<Property> vBattleMoneyList; // ��ս���������б�
	vector<Item> vKillRewardItemList;	// ��ɱ������Ʒ�б�
};

// ����boss��������
struct AttackCityBossReward_Config
{
	WORD wChapter;	// �½�
	vector<Item> vRewardItemList; // ������Ʒ�б�
};

// ������������
struct AttackCityWeak_Config
{
	DWORD dwHPRemainRateMin; // hpʣ���������
	DWORD dwHPRemainRateMax; // hpʣ���������
	WORD wWeakCD; // ����CD(��)
};

// ���ǹ�������
struct AttackCityEncourage_Config
{
	WORD wTimes; // �������
	int nScoreAddRate; // ���ּӳɱ���
	CostData tCostData; // ��������
	Property tAddPro;	// Ӱ���Ӣ������
};

// ���ǻʱ��
struct AttackCityActivityTime_Config
{
	WORD wWeekDay; // �ܼ�
	int nStartTime;	// ��ʼʱ��
	int nOverTime;	// ����ʱ��
};

// ����������������
struct AttackCityRankReward_Config
{
	WORD wRankMin;
	WORD wRankMax;
	vector<Item> vItemList;
};

// �޾�֮�ز�����
struct EndlessLandWave_Config
{
	WORD wWave; // ����
	WORD wInitWave; // ��ʼʱ��ʼ�Ĳ�
	WORD wMonsterNum; // ������������
	int nEndlessStone; // ͨ�ػ�õ��޾�ʯ
	vector<Item> vRewardItemList;  // ͨ�ؽ�����ƷID�б�
	vector<DWORD> vMonsterList; // �����б�
	vector<DropData> vDropDataList; // ���������б�
};

// �޾�֮����ײ����
struct EndlessLandCollision_Config
{
	WORD wRewardId; // ����ID
	DWORD dwRate; // ����
	vector<Item> vRewardItemList; // ��������Ʒ�б�
};

// �޾�֮����ɱ����
struct EndlessLandContinueKillReward_Config
{
	DWORD dwContinueKillNum; // ��ɱ����
	int nDiamond; // ��������ʯ
};

// �޾�֮�����н�������
struct EndlessLandRankReward_Config
{
	WORD wRankMin;	// ��������
	WORD wRankMax;	// ��������
	vector<Item> vItemList; // ������Ʒ�б�
	DWORD dwTitleId; // �����ĳƺ�ID
};

// �޾�֮����ս��������
struct EndlessLandBattleCost_Config
{
	WORD wBattleTimes;		// ��ս����
	CostData tCostCurrency;	// ���Ļ���
	Item tCostItem;			// ������Ʒ
};


// ��λ����
struct Nobility_Config
{
	WORD wLevel;
	DWORD dwNeedPower;	// ��Ҫ��ս��
	vector<Property> vProList; // ��õ������б�
};

// ��ʯ����
struct Gem_Config
{
	DWORD dwGemId;					// ��ʯID
	WORD wType;						// ��ʯ����(1����ͨ 2�����)
	WORD wComposeCostNum;			// �ϳ����ĵ�����
	DWORD dwNewGemFromCompose;		// �ϳɺ��õ��±�ʯ
	int nGemCoin;					// �ֽ��õĻ���ֵ(��ʯ��)
	WORD wSkillId;					// ����ID
	WORD wSkillLevel;				// ���ܵȼ�
	vector<Property> vInitProList;	// ��ʼ�����б�
};

// ��ʯ����
struct GemSkill_Config
{
	WORD wSkillId;			// ����ID
	WORD wSkillLevel;		// ���ܵȼ�
	vector<Property> vProList;	// �����б�
};

// ��ʯ��λ
struct GemHole_Config
{
	WORD wHole; // ��λ
	WORD wNeedRoleLevel;	// ��Ҫ�Ľ�ɫ�ȼ�		
	WORD wIsDefaultOpen; // �Ƿ�Ĭ�Ͽ���
	Item vCostItem; // ���ĵĵ���
};

// ��ʯ��Ƕ����
struct GemInlay_Config
{
	WORD wType; // ��ʯ����	
	WORD wInlayNumLimit; // ͬһ����λ��Ƕ��������
};

// ����ǩ������
struct LuxurySign_Config
{
	WORD wSignInId;		// ǩ��ID
	WORD wLevelMin;		// �ȼ�����
	WORD wLevelMax;		// �ȼ�����
	//int nAmount;		// ��ֵ���
	WORD wRechangeId;	// ��ֵId
	vector<RateDataEx> vRewardRandList; // ��������б�
};


// ����ǩ����������
struct LuxurySignInReward_Config
{
	DWORD dwRewardId; 
	vector<Item> vItemList;
};

// ��ֵ�ۿ۹���������
struct SuperDiscountBuyReward_Config
{
	WORD wDay;	// ����
	vector<Item> vItemList; // ������Ʒ�б�
};

// ��ֵ�ۿ��ۻ���������
struct SuperDiscountAccumulateReward_Config
{
	WORD wTimes;	// �ۻ�����
	WORD wNeedBuyTimes; // ��Ҫ�Ĺ������
	vector<Item> vItemList; // ������Ʒ�б�	
};

// ��ɫ���а���
struct RoleRankReward_Config
{
	WORD wRankType;	// ���а�����
	WORD wRankMin;	// ��������
	WORD wRankMax;	// ��������
	vector<Item> vItemList; // ������Ʒ�б�
	DWORD dwTitleId; // �����ĳƺ�ID
};

// ��׼�Ƽ���Ʒ����
struct AccurateRecommendGoods_Config
{
	WORD wGoodsId;		// ��ƷId
	DWORD dwItemId;		// ��ƷId
	DWORD dwItemNum;	// ��Ʒ����
	WORD wBuyNumMax;	// ������������(0=��ʾ������)
	WORD wRechargeId;	// ��ֵId
};

// ��������
struct Scene_Config
{
	DWORD dwSceneId;  // ����ID
	DWORD dwMapId; // ��ͼ
	BYTE byIsSyncMove;
	vector<DWORD> vMoveArea;		 // ����������
	vector<DWORD> vHeroBirthArea;	 // Ӣ�۳�������
	vector<DWORD> vHeroReliveArea;	 // Ӣ�۸�������
	vector<DWORD> vMonsterRefrshArea;// ����ˢ������
	vector<DWORD> vItemRefrshArea;	 // ������Ʒˢ������
};

// ��Ӹ�������
struct TeamDungeon_Config
{
	DWORD dwDungeonId;			// ����ID
	WORD wRecommendLevelMin;	// �Ƽ��ȼ�������
	WORD wRecommendLevelMax;	// �Ƽ��ȼ�������
	DWORD dwWorldMapId;			// �����ͼID
	vector<DWORD> vTaskList;	// ������ID�б�
};

// ��Ӹ����齱
struct TeamDungeonLottery_Config
{
	DWORD dwItemId; // ��ƷID
	DWORD dwItemNum;	// ��Ʒ����
	int nWeight;	// Ȩֵ
	BYTE byIsNeedRecord; // �Ƿ���Ҫ��¼
};

// ��Ӹ���������
struct TeamDungeonTaskLink_Config
{
	DWORD dwTaskListId;				// �����б�Id		
	DWORD dwBossMapId;				// BOSS�ؿ�
// 	DWORD dwBossKillRewardId;		// Boss��ɱ����
// 	DWORD dwMVPRewardId;			// MVP����

	vector<DWORD> vMVPRewardList;		// MVP�����б�
	vector<DWORD> vBossKillRewardList;	// Boss��ɱ�����б�
	
	MapPos tBossScenePos;			// BOSS�ڳ���������
	WORD wTaskNum;					// ��������
	DWORD dwTaskLimitTime;			// ������������ʱ��(��)
	vector<vector<DWORD>> vTaskList;// �����б�

	DWORD GetMVPReward(WORD wTimes) const
	{
		if (!vMVPRewardList.size())
			return 0;

		if (wTimes >= vMVPRewardList.size())
			return vMVPRewardList[vMVPRewardList.size()-1];

		return vMVPRewardList[wTimes];
	}

	DWORD GetKillReward(WORD wTimes) const
	{
		if (!vBossKillRewardList.size())
			return 0;

		if (wTimes >= vBossKillRewardList.size())
			return vBossKillRewardList[vBossKillRewardList.size() - 1];

		return vBossKillRewardList[wTimes];
	}
};

// �����������
struct TeamDungeonTask_Config
{
	DWORD dwTaskId; // ����ID
	WORD wTaskType;	// ��������
	DWORD dwSceneId; // ������ͼID(0=��ʾ����Ҫ����)
	WORD wTaskNpc; // ����NPC
	DWORD dwExtraRewardId; // ���⽱��ID
	vector<DWORD> vTaskParam; // ��������б�

	vector<DWORD> vScoreList;		// �����б�
	vector<DWORD> vEnergyList;		// ���������б�
	vector<DWORD> vRewardItemList;	// ��������Ʒ�б�

	DWORD GetTaskParam(int nIndex) const
	{
		if (nIndex >= vTaskParam.size())
			return 0;
		return vTaskParam[nIndex];
	}

	int GetScoreIndex(DWORD dwValue) const
	{
		for (size_t i = 0; i < vScoreList.size(); ++i)
		{
			if (dwValue <= vScoreList[i])
				return i;
		}
		return 0;
	}

	DWORD GetEnergy(int nIndex) const
	{
		if (nIndex >= vEnergyList.size())
			return 0;
		return vEnergyList[nIndex];
	}

	DWORD GetRewardItem(int nIndex) const
	{
		if (nIndex >= vRewardItemList.size())
			return 0;
		return vRewardItemList[nIndex];
	}
};

// ���BOSS����
struct TeamDungeonBossReward_Config
{
	DWORD dwDungeonId;		 // ����ID
	int nEnergyMin;			 // ��������
	int nEnergyMax;			 // ��������
	int nDamageRateMin;		 // Boss�˺���������
	int nDamageRateMax;		 // Boss�˺���������
	vector<Item> vRewardList;// �����б�
};

// ��¼��������
struct LoginReward_Config
{
	WORD wDays; // ��¼����
	vector<Item> vRewardItemList; // ������Ʒ�б�
};

// ��ⷵ�� 
struct ClosedTestRebate_Config
{
	WORD wId;
	DWORD dwChannel;
	string strAcount;
	int nDiamond;
};

// ��������λ����
struct KingMatchStage_Config
{
	WORD wStageLevel; // ��λ�ȼ�
	WORD wLevelUpNeedStar; // ������Ҫ������
	BYTE byIsDropStage;	// �Ƿ�ή��
	BYTE byIsDropStar;	// �Ƿ�����
	BYTE byIsLastStage;	// �Ƿ����Ķ�λ
	BYTE byIsBroadcastLeveUp;	// �Ƿ�Ҫ�㲥����
	vector<RateDataEx>	vMatchingStageList; // ��ƥ��Ķ�λ�б�
	vector<Item> vDailyFirstWinReward; // ÿ����ʤ����
	vector<Item> vLevelUpReward; // ��������
	vector<Item> vSeasonReward;	// ��������
};

// ����������ʱ��
struct KingMatchSeasonTime_Config
{
	WORD wSeason;		// ����
	DWORD dwStartDate;	// ��ʼ����
	DWORD dwStartTime;	// ��ʼʱ��
	DWORD dwOverDate;	// ��������
	DWORD dwOverTime;	// ����ʱ��
};

// ������������
struct KingMatchSeasonTask_Config
{
	WORD wWinTimes; // ��ʤ����
	vector<Item> vRewardList; // �����б�
};

// ���������н���
struct KingMatchRankReward_Config
{
	WORD wRankMin;	// ��������
	WORD wRankMax;	// ��������

	DWORD dwTitleId; // �ƺ�ID
	vector<Item> vRewardList; // �����б�
};

// ʥ����BOSS�ȼ�����
struct ChristmasBossLevel_Config
{
	WORD wType;	// boss����
	DWORD dwLevel; // boss�ȼ�
	DWORD dwBossHP;	// bossHP
	vector<Item> vKillRewardList; // ��ɱ�����б�
	vector<Item> vBossRewardList; // Boss�����б�
};

// ʥ����BOSS����
struct ChristmasBoss_Config
{
	WORD wType;							// boss����
	DWORD dwReviveTime;					// ����ʱ��
	vector<DWORD> vLimitSnowBallList;	// �ɶ�Boss������ѩ���б�
};

// ʥ����ѩ������
struct ChristmasSnowball_Config
{
	DWORD dwSnowballId; // ѩ��ID
	DWORD dwDamageMin;	// �˺�����
	DWORD dwDamageMax;	// �˺�����
	vector<DWORD> vRewardItemList;	// ������Ʒ�б�
	vector<DWORD> vRewardNumList;	// ���������б�
	vector<DWORD> vRewardRateList;	// ���������б�
};

// facebook������������
struct FacebookFriendInvite_Config
{
	DWORD dwInviteNum;			// ��������
	vector<Item> vRewardList;	// �����б�
};

// facebook�����ٻ�����
struct FacebookFriendSummon_Config
{
	DWORD dwSummonNum;			// �ٻ�����
	vector<Item> vRewardList;	// �����б�
};


// facebook�ʺŰ�����
struct FacebookAccountBinding_Config
{
	vector<Item> vRewardList;	// �����б�
};

// facebook��������
struct FacebookShare_Config
{
	vector<Item> vRewardList;	// �����б�	
};

// facebook��������(ǿ�м���fb�)
struct FacebookCommend_Config
{
	vector<Item> vRewardList;	// �����б�	
};

// ��ֵ��������
struct RechargeReward_Config
{
	WORD wDataId;
	WORD wRechargeId; // ��ֵID
	vector<Item> vRewardList;	// �����б�	
};

// ��ֵ���⽱��
struct RechargeExtraReward_Config
{
	WORD wDataId;			// �����ID
	WORD wFindId;				// ����ID
	WORD wNeedRechargeDay;		// ��ֵID
	vector<Item> vRewardList;	// �����б�	
};

// apple_iap ����
struct AppleIap_Config
{
	string strIapId;
	DWORD dwRechargeId;	// ��ֵID
};


// �۳佱������
struct TotalRechargeReward_Config
{
	WORD wDataId;	// ����ID
	WORD wRewardId;	// ����Id
	DWORD dwTotalRchargetAmount; // ��Ҫ���ܳ�ֵ���
	vector<Item> vRewardList; // �����б�
};

// �齱����
struct NDLottery_Config
{
	DWORD dwItemId;		// ��ƷID
	DWORD dwItemNum;	// ��Ʒ����
	int nWeight;		// Ȩֵ
	BYTE byIsNeedRecord;// �Ƿ���Ҫ��¼
};

// Ť���齱����
struct NDLotteryCost_Config
{
	DWORD dwDataId;			// ����ID
	DWORD dwCostItemId;		// ������ƷId

	int	nSingleCostDiamond;	// ��ͨ��������ʯ
	int nOenKeyCostDiamond;	// һ����������ʯ	
};

// ������Ʒ
struct HotSaleGoods_Config
{
	WORD wGoodsId;					// ��ƷId
	vector<Item> vItemList;			// ��Ʒ�б�
	vector<DWORD> vBuyNumMaxList;	// �������������б�(0 = ��ʾ������)
	vector<CostData> vCostList;		// �����б�
	vector<DWORD> vRechargeIdList;	// ��ֵID�б�
	vector<DWORD> vNeedLevelList;	// ��Ҫ�ĵȼ�

	vector<DWORD> vResetDayList;	// �������б������ʾ�����ã�
};

// ת������
struct Turntable_Config
{
	WORD wDataId;				// �����ID	
	int nInitDiamond;			// ���س�ʼ��ʯ	
	DWORD dwCostItemId;			// �齱���ĵ���Ʒid	
	DWORD dwCostItemNum;		// �齱���ĵ���Ʒ����
	int nNormalLotteryScore;	// ��ͨ�齱��û���
	int nOneKeyLotteryScore;	// һ���齱��û���
	int nNormalLotteryCostDiamond; // ��ͨ�齱���ĵ���ʯ
	int nOneKeyLotteryCostDiamond; // һ���齱���ĵ���ʯ
	int nRankNeedScore;			// �����Ҫ�Ļ���
	int nNormalLotteryDiamond;	// ��ͨ�齱��ӵĽ�����ʯ
	int nOneKeyLotteryDiamond;	// һ���齱��ӵĽ�����ʯ
	int nOneKeyLotteryTimes;	// һ���齱����
};

// ת�̳齱����
struct TurntableLottery_Config
{
	WORD wDataId;					// �����ID
	WORD wIndex;					// ��������
	DWORD dwRewardItemId;			// ������ƷID
	DWORD dwRewardItemNumMin;		// ������Ʒ��������
	DWORD dwRewardItemNumMax;		// ������Ʒ��������
	int nRewardDiamondRate;			// ������ʯ����
	int nWeight;					// Ȩֵ
	bool bIsNeedRecord;				// �Ƿ���Ҫ��¼
};

// ת�����а�����
struct TurntableRank_Config
{
	WORD wRank;
	vector<Item> vRewardItemList; // ������Ʒ�б�
};

// ��ʱ����
struct LimitProduct_Config
{
	WORD wMapType;	// ս����������
	int nStartDate; // ��ʼ����(ָ������ ��:20180323 ���������� �� 1 )
	int nOverDate;	// ��������
	vector<DWORD> vStartTimeList; // ��ʼʱ���б�
	vector<DWORD> vOverTimeList; // ��ʼʱ���б�
	//int nStartTime; // ��ʼʱ��(ָ��ʱ�� ��2230)
	//int nOverTime;	// ����ʱ��
	int nMultiple;	// ��ӵı���
};

// ��Ӫ��ļ����
struct CampRecruit_Config
{
	WORD wDataId;						// ����id
	WORD wCamp;							// ��Ӫ
	DWORD dwNormalRecruitScore;			// ��ͨ��ļ����
	DWORD dwOneKeyRecruitScore;			// һ����ļ����
	WORD wDailyFreeRecruitTimes;		// ÿ�������ļ����
	WORD wOneKeyRecruitTimes;			// һ����ļ����
	DWORD dwNormalRecruitCostDiamond;	// ��ͨ��ļ������ʯ
	DWORD dwOneKeyRecruitCostDiamond;	// һ����ļ������ʯ
	vector<DWORD> vScoreTaskList;		// ���������б�
	vector<Item> vScoreTaskRewardList;	// �����������б�

	// ȡ�������
	DWORD GetTaskScore(WORD wIndex) const
	{
		return wIndex >= vScoreTaskList.size() ? 9999999 : vScoreTaskList[wIndex];
	}

	// ȡ��������Ʒ
	const Item* GetTaskRewardItem(WORD wIndex) const
	{
		return wIndex >= vScoreTaskRewardList.size() ? NULL : &vScoreTaskRewardList[wIndex];
	}
};

// ��Ӫ��ļӢ������
struct CampRecruitHero_Config
{
	WORD wDataId;		// �����ID
	WORD wCamp;			// ��Ӫ
	DWORD dwWeight;		// Ȩֵ
	Item tRewardItem;	// ������Ʒ
	BYTE byIsNeedRecord;// �Ƿ���Ҫ��¼
};

// unlock�����
struct UnlockActivityReward_Config
{
	WORD wRewardId;		// ����ID
	vector<Item> vRewardItemList; // ������Ʒ�б�
};

// �ۿ���Ƶ����
struct UnlockWatchingVideoReward_Config
{
	WORD wWatchingTimes;		// �ۿ�����
	vector<Item> vRewardItemList; // ������Ʒ�б�	
};

// ��������ȼ�����
struct WorldTaskLevel_Config
{
	WORD wDays;							// ��������
	WORD wWorldLevel;					// ����ȼ�
	map<WORD, vector<Item>> vRewardList;// �����б�[RoleLevel,RewardData]
};

// ����ȼ�����ID�б�
struct WorldLevelTaskIdList_Config
{
	WORD wTaskLevel;			// ����ȼ�
	int nExp;					// ����
	vector<DWORD> vTaskIdList;	// ����ID�б�
};

// ����ȼ������¼�
struct WorldLevelTaskEvent_Config
{
	WORD wTaskId;				// ����ID
	WORD wTaskType;				// ��������
	int nX;						// �������X
	int nY;						// �������Y	
	vector<Item> vRewardList;	// �����б�
};

// ��Դս���⽱������
struct ResourceWarExtraReward
{
	int nCostDiamond;				// �ӳ�ռ��ʱ�����ĵ���ʯ
	WORD vRewardNeedTime;			// ������Ҫ��ʱ��
	vector<Item> tRewardItemList;	// ��������Ʒ�б�
};

// ��Դս�½�����
struct ResourceWarChapter_Config
{
	WORD wChapter; // �½�
	//vector<DWORD> vResourceList;	// ��Դ�б�(vector)
	map<DWORD, DWORD> resourceList;	// ��Դ�б�(map)
};

// ��Դս��Դ����
struct ResourceWarResource_Config
{
	WORD wResourceId;	// ��ԴID
	WORD wResourceType;	// ��Դ����
	DWORD dwMapId;		// �ؿ�id
	int nCostDiamond;	// ��ս���ĵ���ʯ
	int nBaseRewardResourceWarSocre;	// ������������Դս����
	WORD wBaseProfitIntervalTime;		// ����������ʱ��
	//vector<Property> vBaseRewardCurrencyList;	// �������������б�
	vector<ResourceWarExtraReward> vExtraRewardDataList;	// ���⽱�������б�
};

// ��Դս�Ƽ�����
struct ResourceWarTechnology_Config
{
	WORD wLevel;			// �ȼ�
	DWORD dwNeedAccupyTime;	// ��Ҫ��ռ��ʱ��(��)
	int nBaseRewardAddRate; // ���������ӳɱ���(��ֱ�)
};

// ��Դս����������Դ����
struct ResourceWarGuildProfit_Config
{
	WORD wMemberNum;		// ��Ҫռ���Ա������
	int nBaseRewardAddRate; // ���������ӳɱ���(��ֱ�)
};

// ����ս��������
struct GuildWarStar_Config
{
	DWORD dwStarId; // ����ID
	WORD wStarType;	// ��������(1=��Ʒ 2=��Ʒ 3=��Ʒ 4=��Ʒ)
	vector<DWORD> vTowerList; // �������б�
	DWORD dwMapId; // �ؿ���ͼID(����ģʽ)	
	vector<Item> vLocalProductList; // �ز��б�
	vector<Item> vDailyRewardList;	// ÿ�ս����б�(��սģʽ)
	vector<Item> vLoserRewardList;	// ��սʧ�ܽ����б�(����ģʽ)
	vector<Item> vWinnerRewardList;	// ��սʤ�������б�(����ģʽ)

	bool IsTower(DWORD dwTowerId) const
	{
		for (int i = 0; i < vTowerList.size(); ++i)
		{
			if (vTowerList[i] == dwTowerId)
				return true;
		}
		return false;
	}
};

// ����ս������
struct GuildWarTower_Config
{
	DWORD dwTowerId; // ��ID
	WORD wTowerType; // ������
	DWORD dwMapId; // �ؿ���ͼID
	WORD wMonsterDefenseTimes; // ������ش���
	WORD wDefenderNumMax;	// פ����������
	WORD wRoleDefenseTimes; // ��ɫ���ش���
	DWORD dwDurability; // �;ö�
	vector<Item> vBreakTowerRewardList; // �����������б�
	vector<Item> vLoserRewardList;	// ��սʧ�ܽ����б�
	vector<Item> vWinnerRewardList;	// ��սʤ�������б�
};

// ����ս���н���
struct GuildWarRankReward_Config
{
	WORD wRank; // ����
	vector<Item> vRewardList; // �����б�
};

// ����ս��ս����
struct GuildWarVSReward_Config
{
	WORD wVSType; // ��ս����
	WORD wVSResult; // ��ս���
	vector<DWORD> vBreakTowerNumList; // ���Ƶ�������
	vector<vector<Item>> vRewardList; // �����б�	
};

// ����ս��ս�ʱ��
struct GuildWarVSActivityTime_Config
{
	WORD wTimeType; // ʱ������ (1 = ����ʱ�� 2 = ��սʱ�� 3 = ����������ʱ��)
	DWORD dwDecideAttackerTime; // ����������ʱ��[����Сʱ����], �� 11030 ��ʾ��һ10��30��
	DWORD dwBeginTime; // ��ʼʱ��[����Сʱ����], �� 11030 ��ʾ��һ10��30��
	DWORD dwEndTime; // ��ʼ����[����Сʱ����], �� 32030 ��ʾ����20��30�� 7=��ʾ������
	vector<DWORD> vTakeRewardTime; // ��ȡ����ʱ��[���ڼ�|���ڼ�] ��: 1|3|5 ��ʾ����һ, ������, ������
};

// ����սͨ������
// struct GuildWarCommon_Config
// {
// 	DWORD dwKey;
// 	DWORD dwValue;
// 	vector<DWORD> vList;
// };


// ����ս����
struct GuildWarOrganizeDefense_Config
{
	WORD wTowerType;				// ������	
	vector<DWORD> vPowerIdxList;	// ս�������б�
};

// ����ս��������
struct GuildWarRegion_Config
{
	WORD wRegionId; // ����ID
	map<DWORD, DWORD> mServerList; // �������б�
};

// YGame֧������
struct YGamePayment_Config
{
	string strPaymentId;	// ֧��ID
	DWORD dwAssistId;		// ����Id
	DWORD dwRechargeId;		// ��ֵID(todo::Ŀǰֻ���¿��õ�)
	int nPaymentFromType;	// ֧����Դ����(1=�ڹ�, 2=�¿�, 3=��ҳ�ճ�, 4=��ҳ�׳� 5=��Ϸ�ڶһ�(ycoin�һ�gem))	
	int nYCoin;				// ycoin	
	int nVipExp;			// vip����	
	int nDiamond;			// ��õ���ʯ	
	int nBonus;				// ��������ʯ
	vector<Item> vItemList;	// ������Ʒ�б�
};

// ��ֵ���н��� 1.��ֵ�� 2.���Ѱ�
struct RechargeRankReward_Config
{
	WORD wRank; // ����
	vector<Item> vRewardList; // �����б�
};

// �һ���������
struct HangUpDrop_Config
{
	DWORD dwDropItemId;
	DWORD dwDropItemNum;
	int nDropRate;
};

/*---------------------------------------------------------------------------------------------------*/
#pragma pack(pop)