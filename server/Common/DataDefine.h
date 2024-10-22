#ifndef _SERVER_STRUCT_H_
#define _SERVER_STRUCT_H_

#include "Const.h"
#include <time.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <map>
#include "BaseDataDefine.h"
#include "HeroDataDefine.h"
#include "FBDataDefine.h"
#include "GuildDataDefine.h"
#include "FriendDataDefine.h"
#include "RebelDataDefine.h"
#include "LandDataDefine.h"
#include "AchievementDataDefine.h"
#include "ArenaDataDefine.h"
#include "DailyMissionDataDefine.h"
#include "WorldBossDataDefine.h"
#include "RankDataDefine.h"
#include "SevenDayDataDefine.h"
#include "ScoreMatchDataDefine.h"
#include "TitleDataDefine.h"
#include "TeamDungeonDataDefine.h"
#include "KingMatchDefine.h"
#include "ZhanBuDataDefine.h"
#include "GuildWarDataDefine.h"

using namespace std;


/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// �ʼ�ΨһID
struct EmailGUID
{
	DWORD dwInserTime;		// �����ʼ�ʱ���
	DWORD dwSerialId;		// ����ID
};

// �ʼ�����������
struct EmailContext
{
	WORD wType;			// ��������(1=����,2=�ַ���)
	char szText[BUFF_LEN];	// �ı�
};

//�ʼ�
struct EmailInfo
{
	DWORD	dwID;					// �ʼ�ID = yearday + 0-1000000
	WORD	wInfoId;				// �ʼ���ϢID
	DWORD	dwReceiveTime;			// ����ʱ��
	EmailContext arContextList[MAX_EMAIL_CONTEXT_NUM];	// �������б�
	WORD  wContextNum; // ����������	

	char szTitle[MAX_EMAIL_TITLE_LEN]; // ����
	char szText[MAX_EMAIL_TEXT]; // ����

	void CleanUp()
	{
		memset(this,0,sizeof(EmailInfo));
		dwReceiveTime = time(NULL);
	}

	EmailInfo()
	{
		CleanUp();
	}

	EmailInfo(WORD wInfoId)
	{
		CleanUp();
		this->wInfoId = wInfoId;
	}

	void AddContext(BYTE byType, const char *pszText)
	{
		if (wContextNum >= MAX_EMAIL_CONTEXT_NUM)
			return;

		EmailContext *pContext = &arContextList[wContextNum++];
		pContext->wType = byType;
		strcpy_s(pContext->szText, sizeof(pContext->szText) - 1, pszText);
	}

	void AddContext(BYTE byType, int nValue)
	{
		if (wContextNum >= MAX_EMAIL_CONTEXT_NUM)
			return;

		EmailContext *pContext = &arContextList[wContextNum++];
		pContext->wType = byType;
		sprintf_s(pContext->szText, sizeof(pContext->szText) - 1, "%d", nValue);
	}
};

struct CostItemData
{
	DWORD dwItemId;	// ��ƷID
	WORD wItemNum;	// ��Ʒ����
};

// ������Ʒ
struct RewardItem
{
	DWORD dwItemId;
	WORD wNumMin;
	WORD wNumMax;
};

// ��ɫ�ʼ���Ϣ
struct PlayerEmailInfo
{
	WORD wIsHaveNewEmail; // �Ƿ������ʼ�

	EmailInfo arEmailList[MAX_EMAIL_COUNT];	// �ʼ��б�
	WORD wEmailCount;						// �ʼ�����
};

// �����������
struct MonsterDropData
{
	DWORD dwMonsterId;	
	BYTE byType;		
	DWORD dwObjId;
	WORD wObjNum;
};

struct BeginEndTime
{
	DWORD				dwBeginTime;
	DWORD				dwEndTime;
	bool				bBeginFlag;
	bool                bIsDisposable;
};

// ��������
struct ChatVoice
{
	char szVoice[MAX_CHAT_VOICE_LEN];
	//WORD wVoiceLen;
};

//������������ 
struct ChatReqData
{
	BYTE	byChatType;								//��������
	BYTE	byIsVoice;								//�Ƿ�����
	DWORD	dwRoleId;								//Ŀ���ɫID
	char	szTargetName[MAX_ROLE_LEN+1];		//����Ŀ����
	char	szChatText[MAX_CHAT_TEXT_LEN+1];			//�����ı�
	char	szVoice[MAX_CHAT_VOICE_LEN+1];			//��������
};

// �����ɫ����
struct ChatRoleData
{
	DWORD	dwRoleID;							// ��ɫID
	//WORD	wLevel;								// ��ɫ�ȼ�
	DWORD	dwHeadIcon;							// ��ɫͷ��
	WORD    wServerId;							// ������ID
	BYTE	byVipLevel;							// VIP�ȼ�
	DWORD	dwCurUseNicknameId;					// ��ǰʹ�õ��ǳ�ID
	char	szRoleName[MAX_ROLE_LEN];		// ��ɫ����
};

//����֪ͨ����
struct ChatNotifyData
{
	BYTE byChatType;						// ��������
	ChatRoleData tSenderData;				// ����������
	DWORD dwVoiceId;						// ����ID
	char szChatText[MAX_CHAT_TEXT_LEN];		// �����ı�
	//char szVoice[MAX_CHAT_VOICE_LEN];		// ����
	void CleanUp()
	{
		memset(this, 0, sizeof(ChatNotifyData));
	}
};

struct ChatRecord : public ChatNotifyData
{
	DWORD dwTime;							// ʱ���
	BYTE byGMLevel;							// todo::��ʱ����
};

// ����Ӣ������
struct ToBattleHeroData
{
	WORD wHeroInfoId;					// Ӣ����ϢID
	BYTE byPos;							// λ��
	WORD wLevel;						// Ӣ�۵ȼ�
	BYTE byQuality;						// Ʒ�ʵȼ�
	BYTE byProNum;						// ��������
	Property arPorList[HERO_PRO_MAX];	// �����б�
	BlessSpriteData tBlessSpriteData;	// ף����������
};

struct DBPlayerBaseData : public PlayerCommonData
{
	ToBattleHeroData arToBattleHeroList[MAX_BATTLE_HERO_NUM];	// ����Ӣ���б�
	BYTE byToBattleHeroNum;
	//WORD wServerId;
};



// VIP����
struct VipData
{
	WORD wLevel;						// vip�ȼ�
	DWORD dwExp;						// vip����
};

// �¿�����
struct MonthCard
{
	WORD wCardId;				// �¿�ID
	DWORD dwBuyTime;			// ����ʱ��
	DWORD dwLastTakeRewardTime;	// �����ȡ����ʱ��
};

// ��ͨ��ֵ����
struct CommonRecharge
{
	WORD wRechargeId;		// ��ֵID
	WORD wRechargeTimes;	// ��ֵ����
	//WORD wTakeRewardTimes;	// ��ȡ��������
};

// ��ʱ�����
struct LimitActivityData
{
	// �߼���ļ����
//	WORD wRecruitTimes;

	// �ܵĳ�ֵ
	DWORD dwTotleRecharge;

	// ��ļ������¼
// 	WORD arRecruitRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
// 	WORD wRecruitRewardRecordNum;
// 
// 	// �ۻ���ֵ������¼
// 	WORD arAccumulateRechargeRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
// 	WORD wAccumulateRechargeRewardRecordNum;
// 	WORD wLastAccumulateRechargeDataId; // �ϴ��۳�����Id
// 	DWORD dwAccumulateRecharge; // �ۻ���ֵ(������)
// 
// 	// ���γ佱����¼
// 	RecordData arSingleRechargeRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
// 	WORD wSingleRechargeRewardRecordNum;
// 	WORD wLastSingleRechargeDataId; // �ϴλ��������Id
// 	// ��ֵ�б�(��Ϊ���������,������)
// 	CommonRecharge arRechargeList[MAX_RECHARGE_NUM];
// 	WORD wRechargeNum;

	// �׳佱����¼
	WORD arFirstRechargeRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
	WORD wFirstRechargeRewardRecordNum;
};

// �Ŀ������
struct ActivityTarget
{
	WORD wId;				// Ŀ��ID
	DWORD dwContext;		// ������(�׶�ֵ,��ɴ���)
	WORD wTakeRewardTimes;	// ��ȡ��������
};

// ���ջ����
struct FestivalActivity
{
	// �Ŀ������
	ActivityTarget arTargetList[MAX_ACTIVITY_TARGET_NUM];
	WORD wTargetNum;

	// ��������ʱ��
	DWORD dwDataResetTime;	

	ActivityTarget* GetTarget(WORD wTargetId)
	{
		for (WORD i = 0; i < wTargetNum; ++i)
		{
			if (arTargetList[i].wId == wTargetId)
				return &arTargetList[i];
		}
		return NULL;
	}
};

// ��ɫ���ջ��Ϣ
struct PlayerFestivalActivityInfo
{
	// 1��ѭ������
	FestivalActivity tLoopOnceRecharge;

	// 2��ѭ���۳�
	FestivalActivity tLoopAccumulateRecharge;

	// 3��������ʱ�ٻ�
	FestivalActivity tLimitRecruit;

	// 4�������
	FestivalActivity tActivityOnceRecharge;

	// 5����۳�
	FestivalActivity tActivityAccumulateRecharge;

	// 6�����۶һ�����Ʒ��������ʯ,��ʯ����Ʒ��
	FestivalActivity tDiamondExchange;

	// 7����ԾĿ��
	FestivalActivity tActiveTarget;

	// 8�����϶һ�����Ʒ����Ʒ��
	FestivalActivity tItemExchange;

	// 9�����յ�½
	FestivalActivity tFestivalLogin;

	// 10��������½
	FestivalActivity tOpenServerLogin;

	// 11 �����۳�
	FestivalActivity tOpenSvrAccumulateRecharge;

	// 12 ���ղ��϶һ�
	FestivalActivity tFestivalItemExchange;

	// 13 ���մ��۷��ۣ��߼�ͬԭ����6�����۷��ۣ�
	FestivalActivity tFestivalDiscountSell;

	// 14 ���ջ�ԾĿ�꣬�߼�ͬԭ����7����ԾĿ�꣩
	FestivalActivity tFestivalActiveTarget;

	// 15.������Ծ2(ͬ7)
	FestivalActivity tOpenServerActiveTarget;

	// 16.���ջ�Ծ2(ͬ14)
	FestivalActivity tFestivalActiveTarget2;

	// 17�������
	FestivalActivity tActivityOnceRecharge2;
};

// ��ɫ��ֵ����
struct RoleRechargeData
{
	// ��ֵ�б�
	CommonRecharge arRechargeList[MAX_RECHARGE_NUM];
	WORD wRechargeNum;

	// �¿��б�
	MonthCard arMonthCardList[MAX_MONTH_CAR_NUM];
	WORD wMontchCardNum;

	// ������¼�б�
	DWORD arCPOrderRecordList[MAX_ORDER_RECORD_NUM];
	WORD wCPOrderRecordNum;

	// ���ճ�ֵ����
	WORD wTodayRechargetTimes; 
};

// ��Ʒ
struct Goods
{
	WORD wId;	// ��ƷID
	WORD wNum;	// ��Ʒ����
};

// ���
struct Gift
{
	WORD wId;	// ���Id
	WORD wNum;	// �������
};

// �����Ʒ��Ϣ
struct PlayerItemInfo
{
	Item arItemList[MAX_ITEM_NUM];	// ��Ʒ�б�
	WORD wListNum;				// ��Ʒ�б�����
};

// ��ʯ
struct Gem
{
	DWORD dwId;	// ��ʯID
	DWORD dwNum;// ����	
};

// ��ұ�ʯ��Ϣ
struct PlayerGemInfo
{
	Gem arGemList[MAX_GEM_NUM]; // ��ʯ�б�
	WORD wGemNum;	// ��ʯ����
	WORD arHoleOpenList[MAX_BATTLE_HERO_NUM]; // ��λ�����б�
	DWORD dwHoleInitTime; // ��λ��ʼʱ��
};

// װ��
struct Equipment
{
	DWORD dwId;				// ΨһID
	DWORD dwInfoId;			// ����ID
	WORD wEnhanceLevel;		// ǿ���ȼ�
	WORD wRefineLevel;		// �����ȼ�
	DWORD dwRefineExp;		// ��������
	WORD wStar;				// ����
	WORD wStarLevel;		// �Ǽ�

	int nEnhanceCostGold; // ǿ�����ĵĽ��
	Item arRefineCostItem[MAX_EQUIPMENT_REFINE_COST_ITEM];	// �������ĵ���Ʒ�б�
	WORD wRefineCostItemNum;
};

// ���װ����Ϣ
struct PlayerEquipmentInfo
{
	Equipment sEquipmentList[MAX_EQUIPMENT_NUM];
	WORD wEquipmentNum;
};

// �����������
struct TreasureBaseData
{
	DWORD dwId;				// ΨһID
	WORD wInfoId;			// ����ID
	BYTE byEnhanceLevel;	// ǿ���ȼ�
	DWORD dwEnhanceExp;		// ǿ������
	BYTE byRefineLevel;		// �����ȼ�
	WORD wWearHeroInfoId;	// ����Ӣ��ID
};

// ����
struct Treasure
{
	DWORD dwId;				// ΨһID
	DWORD dwInfoId;			// ����ID
	WORD wEnhanceLevel;		// ǿ���ȼ�
	DWORD dwEnhanceExp;		// ǿ������
	WORD wRefineLevel;		// �����ȼ�
	//DWORD dwRefineExp;		// ��������
};

// ��ұ�������
struct PlayerTreasureInfo
{
	Treasure sTreasureList[MAX_TREASURE_NUM];
	WORD wTreasureNum;
};

// ˢ���̵�
struct RefreshShopData
{
	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // ��Ʒ�б�
	WORD wGoodsNum;	// ��Ʒ����

	DWORD dwLastRecoverTime;	// ��һ�λָ�ʱ��(���ˢ�´����ָ�ʱ��)
	WORD wFreeRefreshTimes;	// ��ʹ�õ����ˢ�´���
	WORD wCostRefreshTimes;	// ��ʹ�õ�����ˢ�´���		
};

// �ǻ��̵�
struct StarSoulShopData
{
	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // ��Ʒ�б�
	WORD wGoodsNum; // ��Ʒ����

	WORD wUsedRefreshTimes;		// �Ѿ�ʹ�õ�ˢ�´���
	DWORD dwLastAutoRefreshTime;	// �ϴ��Զ�ˢ��ʱ��
};

// ��������Ʒ
struct ScoreMatchShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;		
};

// 7����Ʒ
struct SevenDayShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// �ȼ������Ʒ
struct LevelGiftShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};


// ��ҹ�����Ʒ����
struct PlayerGuildShopData
{
	ShopGoodsBuyRecord arAwardGoodsRecord[MAX_GUILD_AWARD_SHOP_GOODS_NUM];	// ������Ʒ�����¼
	WORD wAwardGoodsRecordNum;												// ������Ʒ��������

	ShopGoodsBuyRecord arItemGoodsRecord[MAX_GUILD_AWARD_SHOP_GOODS_NUM];	// ������Ʒ�����¼
	WORD wItemGoodsRecordNum;												// ������Ʒ��������

	// �ⲿ�ֲ���Ҫ��¼,��Ϊ�ǹ��Ṳ���
	ShopGoodsBuyRecord arLimitGoodsRecord[MAX_GUILD_AWARD_SHOP_GOODS_NUM];	// ��ʱ��Ʒ�����¼
	WORD wLimitGoodsRecordNum;												// ��ʱ��Ʒ��������
};

// ����̳�����
struct PlayerMallData
{
	ShopGoodsBuyRecord arItemBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wItemBuyRecordNum;

	ShopGoodsBuyRecord arGiftBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wGiftBuyRecordNum;
};

// ��������̵�����
struct PlayerTowerShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// ��Ҿ������̵�����
struct PlayerArenaShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// �޾�֮���̵�
struct EndlessLandShopData
{
	// ������Ʒ
	ShopGoodsBuyRecord arItemGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wItemGoodsBuyRecordNum;

	// ������Ʒ
	ShopGoodsBuyRecord arRewardGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wRewardGoodsBuyRecordNum;
};

// ֱ���̵�
struct DirectBuyShopData
{
	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // ��Ʒ�б�
	WORD wGoodsNum;	// ��Ʒ����

	WORD wLastRefreshYearDay;	// �ϴ�ˢ������			
	WORD wLastRefreshTime;	// ���һ��ˢ�µ�ʱ��[Сʱ����] ��: 2330 23��30�� year_day + our + min
	WORD wIsNewGoods;	// �Ƿ�����Ʒ

	// ����ȡ���̵��б�
	WORD arCanTakeGoodsList[MAX_DIRECT_BUY_GOODS_NUM];
	WORD wCanTakeGoodsNum;

	void AddCanTakeGoods(WORD wGoodsId)
	{
		// ����Ƿ��Ѵ���
		for (WORD i = 0; i < wCanTakeGoodsNum; i++)
		{
			if (arCanTakeGoodsList[i] == wGoodsId)
				return;
		}

		// ���
		if (wCanTakeGoodsNum < MAX_DIRECT_BUY_GOODS_NUM)
			arCanTakeGoodsList[wCanTakeGoodsNum++] = wGoodsId;
	}
};

// �һ��̵�����
struct ExchangeShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// �Ƽ��̵�����
struct RecommendShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// ����̵�����
struct TeamDungeonShopData
{
	// ��ͨ��Ʒ�����¼
	ShopGoodsBuyRecord arNormalGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wNormalGoodsBuyRecordNum;

	// ������Ʒ�����¼
	ShopGoodsBuyRecord arRewardGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wRewardGoodsBuyRecordNum;
};

// ʥ�����̵�����
struct ChristmasShopData
{
	// ���ֶһ���Ʒ�����¼
	ShopGoodsBuyRecord arScoreExchangeGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wScoreExchangeGoodsBuyRecordNum;

	// ��Ʒ�һ���Ʒ�����¼
	ShopGoodsBuyRecord arItemExchangeGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wItemExchangeGoodsBuyRecordNum;
};


// ˮ���̵�
struct CrystalShopData
{
	// ����ˢ�´���
	WORD wTodayRefreshTimes;

	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // ��Ʒ�б�
	WORD wGoodsNum;	// ��Ʒ����
};

// ��Դս�̵�����
struct ResourceWarShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// GM�̵�
struct GMShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GM_SHOP_GOODS_NUM];
	WORD wBuyRecordNum;
};



// ����̵�����
struct PlayerShopData
{
	RefreshShopData tHeroShop;				// Ӣ���̵�����
	PlayerGuildShopData tGuildShop;			// ������Ʒ�����¼
	PlayerMallData tMallShop;				// �̳��̵깺���¼
	PlayerTowerShopData tTowerShop;			// �����̵�����
	PlayerArenaShopData tArenaShop;			// �������̵�����
	RefreshShopData tAwakenShop;			// �����̵�����
	StarSoulShopData tStarSoulShop;			// �ǻ��̵�����
	ScoreMatchShopData tScoreMatchShop;		// ��������Ʒ�����¼
	SevenDayShopData tSevenShop;			// 7���̵�
	LevelGiftShopData tLevelGiftShop;		// �ȼ���ʱ�̵�	
	EndlessLandShopData tEndlessLandShop;	// �޾�֮���̵�	
	DirectBuyShopData tDirectBuyShop;		// ֱ���̵�
	ExchangeShopData tExchangeShop;			// �һ��̵�
	RecommendShopData tRecommendShop;		// �Ƽ��̵�
	TeamDungeonShopData tTeamDungeonShop;	// ����̵�
	ChristmasShopData tChristmasShop;		// ʥ�����̵�
	CrystalShopData tCrystalShopData;		// ˮ���̵�
	ResourceWarShopData tResourceWarShop;	// ��Դս�̵�
	GMShopData tGMShop;						// GM�̵�(todo::Ŀǰ����Ҫ��¼�����ݿ�, ��ΪĿǰ���������������)
};

// ����֮��
struct HeartOfWorld
{
	WORD wStage;	// 
	WORD wLevel;	// �ȼ�	
};

// ��������
struct SpiritData
{
	DWORD dwId;	// ����ID
	WORD wEnhanceLevel;	// ǿ���ȼ�
	WORD wTrainStage;	// ������(Ĭ����1��)
	WORD wTrainTimes;	// ��������

	WORD wStarLevel;	// �Ǽ�
};

// ����ͼ��
struct SpiritPokedex
{
	WORD wPokedexId; // ͼ��ID
	WORD wLevel;	 // �ȼ�
};

// ��Ҿ�����Ϣ
struct PlayerSpriteInfo
{
	HeartOfWorld tHeartOfWorld;	// ����֮��
	DWORD dwCurUseSpiritId;	// ��ǰʹ�õľ���ID
	SpiritData arSpiritList[MAX_SPIRIT_NUM]; // �����б�
	WORD wSpiritNum;	// ��������
	SpiritPokedex arPokedexList[MAX_SPIRIT_POKEDEX_NUM]; // ͼ���б�
	WORD wPokedexNum; // ͼ������
};


// �ͻ��˽�ɫ��ϸ
struct ClientRoleDetails : public PlayerCommonData
{
	BYTE byIsFriend;	// �Ƿ����
	BYTE byIsBlack;		// �Ƿ������
	ClientFormation arFormationList[MAX_BATTLE_HERO_NUM];
	BYTE byFormationNum;

	ClientRoleDetails()
	{
		memset(this, 0, sizeof(ClientRoleDetails));
	}
};

// ǩ������
struct SignInData
{
	WORD wCurSignInDay;		// ��ǰǩ������
	DWORD dwLastSignInTime;		// ���ǩ��ʱ��
};

// ����ǩ������
struct LuxurySignData
{
	WORD wSignId;	// ǩ��Id
	WORD wStatus;	// ����״̬(0=δ�ﵽ��ȡ����,1=������ȡ,2=����ȡ)
	WORD wRewardId;	// ����Id
};

// ��Һ���ǩ������
struct PlayerLuxurySignData
{
	LuxurySignData arSignDataList[MAX_LUXURY_SIGN_NUM];
	WORD wSignDataNum;
};

// ӭ��������
struct YCSData
{
	BYTE byTodayGetTimes;	// ������ȡ����
	BYTE byTotalGetTimes;	// �ܵ���ȡ����
	int nTotalRewardValue;	// �ܵĽ����ۻ�ֵ
	DWORD dwLastGetTime;	// ���һ����ȡ��ʱ��
};

// ͭȸ̨����
struct TQTData
{
	DWORD dwLastGetTime;							// ���һ����ȡ��ʱ��
	WORD arTakeRecordList[MAX_TQT_TAKE_RECORD_NUM];	// ��ȡ��¼�б�
	WORD wTakeRecordNum;							// ��ȡ��¼����
};

// ��¼��������
struct LoginRewardData
{
	WORD wLoginDays;	// ��¼����
	WORD arRewardRecordList[MAX_LOGIN_REWARD_RECORD_NUM];	// ������¼�б�
	WORD wRewardRecordNum;	// ������¼����	
};

// �������������
// struct WarcraftData
// {
// 	BYTE byBattleTimes;		// ��ս����
// 	BYTE byBuyBattleTimes;	// �������ս����	
// };

// ��������
struct GuideData
{
	int nKey;
	int nValue;
};

// ���PVP����
struct PlayerPVPData
{
	WORD wTodayBattleTimes; // ������ս����
	WORD wWeekBattleTimes;	// ������ս����
	WORD wLastWeekRank;		// ��������(0=��ʾľ������)

	DWORD dwScore;			// ����
	WORD wWeekWinTimes;		// ����ʤ������
	DWORD dwTotalBattleTimes; // �ܵ���ս����

	// ÿ�ս�����ȡ��¼
	WORD arDailyRewardTakeRecord[MAX_PVP_BATTLE_REWARD_NUM];
	WORD wDailyRewardTakeRecordNum;

	// ÿ�ܽ�����ȡ��¼
	WORD arWeeklyRewardTakeRecord[MAX_PVP_BATTLE_REWARD_NUM];
	WORD wWeeklyRewardTakeRecordNum;
};

// ��ɫ������Ϣ 
struct RoleBaseInfo
{
	DWORD dwRoleId;						// ���ID
	char szRoleName[MAX_ROLE_LEN];		// �������
	char szAccount[MAX_ACCOUNT_LEN];	// �ʺ�
	DWORD dwStamina;					// ����
	DWORD dwEnergy;						// ���� vigor
	WORD wSex;							// �Ա�
	WORD wLevel;						// �ȼ�
	WORD wDailyInitLevel;				// ÿ�ճ�ʼ�ȼ�
	DWORD dwExp;						// �ȼ�����
	DWORD dwGold;						// ���
	DWORD dwDiamond;					// ��ʯ
	DWORD dwHeadImage;					// ͷ��Id(Ӣ����ϢID)
	VipData sVipData;					// VIP����
	WORD wClickLikeTimes;				// ���޴���
	WORD wRiotSuppressTimes;			// ��ر�����ѹ����
	int nOnceFlagList;					// һ���Ա�־�б�
	int nGuideX;						// ����X
	int nGuideY;						// ����Y
	DWORD dwRoleCreateTime;				// ��ɫ����ʱ��
	DWORD dwUpdateTimes;				// ���´���
	DWORD dwTotalRechargeAmount;		// �ܵĳ�ֵ���
										
	int nHeroSoul;						// �ۻ�(�꾧)
	int nArenaPrestige;					// ����������(����)
	int nGodSoul;						// ���
	int nWanted;						// ͨ����(������)
	int nTowerPrestige;					// ��������(����)								
	DWORD dwFightingPower;				// ս��	
	DWORD dwRebelValue;					// ս��(�Ѿ�ֵ)
	DWORD dwCurContribution;			// ��ǰ���ṱ��ֵ
	int nHonor;							// ����
	WORD wWorldBossBattleTimes;			// ����BOSS��ս����
	WORD wGuildFBRewardChapter;			// �Ѿ���ȡ�ľ��Ÿ����½ڽ���
	int nFairySoul;						// �ɻ�
	int nScoreMatchHonor;				// ����������
	WORD wFriendshipPoint;				// �����
	WORD wTodayFriendshipPoint;			// ���������
	int nEndlessStore;					// �޾�֮��ֵ(�޾�ʯ)
	int nGemCoin;						// ��ʯ��
	int nDestinyCoin;					// ����Ӳ��
	int nStarSoul;						// �ǻ�
	int nCrystal;						// ˮ��
	int nResouceWarScore;				// ��Դս����
	int nYCoin;							// Խ�ϱ�

	CHAR szLastLoginTime[MAX_TIME_LEN];		// ��һ�ε�¼ʱ��
	CHAR szLastLogoutTime[MAX_TIME_LEN];	// ��һ���˳�ʱ��
	DWORD dwOnlineTime;						// �ܵ�����ʱ��
	DWORD dwLoginTimes;						// ��¼����
	WORD wLoginDays;						// ��¼����
	DWORD dwLastLoginTime;					// �����¼ʱ��
	DWORD dwLastLogoutTime;					// �������ʱ��

	DWORD dwLastStaminaRecoverTime;		// �����ϴλָ�ʱ��
	DWORD dwLastEnergyRecoverTime;		// �����ϴλָ�ʱ��
	DWORD dwLastWantedRecoverTime;		// ͨ����(������)�ϴλָ�ʱ��

// 	BYTE byScoreMatchBattleTimes;		// ��������ս����
// 	BYTE byScoreMatchRefreshTimes;		// ������ˢ�´���

	WORD wWarcraftBattleTimes;			// ��������ս����
	WORD wBuyWarcraftBattleTimes;		// �������������ս����

	WORD wCheerTimes;					// ��������
	int nCheerTimesRecoverTime;			// �ϴ����������ָ�ʱ��

	DWORD dwDBPeaceOverTime;			// �ᱦ��ս����ʱ��(0=��ʾľ����ս) (todo::Ŀǰ�Ѳ�ʹ��)

	DWORD dwLeaveGuildTime;				// �뿪����ʱ��
	WORD wLastActivityNoticeId;		// �ϴλ�����ID

	int	arFormationProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX]; // ���������б�

	GuideData arGuideList[MAX_GUIDE_NUM];
	WORD wGuideNum;

	// ��������ȡ��¼(ֻ��¼ͨ�õ�)
	DWORD arActivationTakeRecord[MAX_ACTIVATION_RECORD_NUM];
	WORD wActivationTakeRecordNum;

	//PlayerPVPData tPVPData;	// ���PVP����

	DWORD dwForbidToTalkOverTime; // ���Խ���ʱ��

	WORD wNobilityLevel;	// ��λ�ȼ�

	// ��ʱ����
	WORD wArenaHistroyRank;	// ��������ʷ����
	WORD wLandNum;			// �������

	DWORD dwTurntableScore;			// ת�̻���
	WORD wTurntableActivityDataId;	// ת�̻����ID

	WORD wTodayUsedHelpBattleTimes;	// ������ʹ����ս�Ĵ���

	WORD wIsAlreadyTakeClosedTestRebate; // �Ƿ�����ȡ��ⷵ��

	WORD wTodayChatTimes;			// �����������

	//DWORD dwResourceWarAccupyTime;		// ��Դս��ռ����Դʱ��
	//WORD wResourceWarTechnologyLevel;	// ��Դս�Ƽ��ȼ�
};

// ����޾�֮������
struct PlayerEndlessLandData
{
	WORD wIsTodayFirstBattle; // �Ƿ�����״���ս(todo::Ŀǰ��ʱ��ʹ��)
	WORD wTodayBattleTimes;	// ������ս����
	WORD wCurWave; // ��ǰ��ս�Ĳ���(���ڴ��)
	WORD wTodayWaveRecord;	// ���ղ�����¼(��ͨ�ص�)
	WORD wCurWaveKillNum; // ��ǰ����ɱ�ֵ�����
	WORD wHistoryWaveRecord; // ��ʷ������¼(��ͨ�ص�)
	WORD wCurContinueKillNum;	 // ��ǰ��ɱ����
	WORD wContinueKillNumRecord; // ��ɱ������¼
	WORD wTodayCollisionRewardTimes; // ��ײ��������(�����Ѿ���õĴ���)

	WORD wLastTakeContinueKillReward; // �����ȡ����ɱ����(��ɱ������,0��ʾ��û��ȡ)

	WORD wIsFirstBattle; // �Ƿ��״���ս(1=��,0=��) ��������ʹ��

	// ��ɱ������¼
// 	WORD arContinueKillRewardRecord[MAX_ENDLESS_LAND_CONTINUE_KILL_REWARD];
// 	WORD wContinueKillRewardRecordNum;
};

// ��ҹ�������
struct PlayerGuildData
{
	// ��ȡ���½ڽ�����¼
	WORD arTakeChapterRewardRecord[MAX_GUILD_FB_CHAPTER_NUM];
	WORD wTakeChapterRewardRecordNum;
};

// ��׼�Ƽ���Ʒ
struct AccurateRecommendGoods
{
	WORD wGoodsId; // ��ƷID
	WORD wBuyTimes;	// �������
	WORD wCanTakeTimes;	// ����ȡ����
};

// ��Ҿ�׼�Ƽ�����
struct PlayerAccurateRecommendData
{
	AccurateRecommendGoods arGoodsList[MAX_ACCURATE_RECOMMEND_GOODS_NUM];
	WORD wGoodsNum;
};

// ����ȼ������¼�
struct WorldLevelTaskEvent
{
	WORD wTaskId;			// ����ID
	int nContext;			// ����������
	WORD wIsAlreadyTake;	// �Ƿ�����ȡ

	// ���������ֶ�
	WORD wTaskType;			// ��������(Ϊ�˷������)
	WORD wTaskLevel;		// ����ȼ�(��ʱ����,�����洢)		
};

// �������ȼ���������
struct PlayerWorldLevelTaskData
{
	// �Ƿ�����ȡ���ս���
	WORD wIsAlreadyTakeFinalReward;

	// �����¼��б�
	WorldLevelTaskEvent arTaskEventList[MAX_DAILY_WORLD_LEVEL_TASK_NUM]; 
	WORD wTaskEventNum;
};

// �һ���������
struct PlayerHangUpDropData
{
	DWORD dwLastObtainRewardTime;	// ���һ����ȡ����ʱ��

	// ���佱����Ʒ
	//Item arRewardRecordList[MAX_ONLINE_DROP_REWARD_ITEM_TYPE]; 
	//WORD wRewardRecordNum;
};

// ��ֵ��������ʯ���а�����
struct RechargeAndCostDiamondRankData
{
	WORD wLastActivityDataId;		// ���һ�λ����ID
	DWORD dwCostDiamond;			// ���ĵ���ʯ
	DWORD dwRechargeAmount;			// ��ֵ���
};

// ��ɫ������Ϣ
struct RoleSecondInfo
{
	PlayerPVPData tPVPData;
	PlayerTitleInfo	tTitleInfo;	// �ƺ���Ϣ	ok
	PlayerEndlessLandData tEndlessLandData; // �޾�֮������
	PlayerGuildData tGuildData; // ��������(���������ϵ�����)
	PlayerLuxurySignData tLuxurySignData; // ����ǩ������
	PlayerGemInfo tGemInfo;		// ��ʯ��Ϣ
	LoginRewardData tLoginRewardData; // ��¼��������
	PlayerKingMatchData tKingMatchData; // ����������
	PlayerTeamDungeonData tTeamDungeonData; // ��Ӹ�������
	PlayerWorldLevelTaskData tRoleWorldLevelTaskData; // ��ɫ����ȼ���������
	PlayerHangUpDropData tHangUpDropData;	// �һ���������
};

// �ᱦ��սĿ���������
struct DuobaoTargetPlayerData
{
	DWORD dwRoleId;									// Ŀ���ɫID
	WORD wLevel;									// ��ɫ�ȼ�
	DWORD dwHeadImage;								// ͷ��
	DWORD dwFightingPower;							// ս��	
	char szRoleName[MAX_ROLE_LEN];					// Ŀ���ɫ����
	BYTE byToBattleHeroNum;							// ����Ӣ������
	WORD arToBattleHeroList[MAX_BATTLE_HERO_NUM];	// ����Ӣ���б�
};

// �ᱦ��սĿ���������
struct DuobaoTargetMonsterData
{
	WORD wMapId;	// Ŀ��������ڵĹؿ�ID
};

// �ᱦ����
struct DoubaoTimes
{
	WORD wQuality;	// Ʒ��
	WORD wTimes;	// ����	 
};

// ��Ҷᱦ����
struct PlayerDuobaoInfo
{
	DWORD dwPeaceOverTime;	// ��ս����ʱ��(0=��ʾľ����ս)
	WORD arDoubaoTimes[QUALITY_NUM];

	// ÿ�նᱦ����
	WORD wDailyDoubaoTimes;

	// �ᱦ���Ƚ�����¼
	WORD arPregrossRewardRecord[MAX_DOUBAO_PREGROSS_NUM];
	WORD wPregrossRewardRecordNum;
};

// ��ҹ��Ἴ������
struct PlayerGuildSkillData
{
	SkillData arSkillList[MAX_GUILD_SKILL_NUM];
	WORD wSkillNum;
};

// ��������
struct RewardData
{
	DWORD dwUId;				// ����UId
	WORD wInfoId;				// ��������Id
	DWORD dwValue1;				// ����ֵ1
	DWORD dwValue2;				// ����ֵ2
	DWORD dwTime;				// ����ʱ��
	char szText[BUFF_LEN_64];	// �ı�

	RewardData()
	{
		dwUId = 0;
		wInfoId = 0;
		dwValue1 = 0;
		dwValue2 = 0;
		dwTime = 0;
		memset(szText, 0, sizeof(szText));
	}
};

// ��ҽ�������
struct PlayerRewardData
{
	RewardData arRewardList[MAX_REWARD_CENTER_REWARD_NUM];
	WORD wRewardNum;
};

// VIP�������
struct VipGiftData
{
	WORD wInitVipLevel;	// ÿ�ճ�ʼVIP�ȼ�
	short nVipLevelRewardRecord; // VIP�ȼ�������¼(�Ѿ���ȡ��,-1=��ʾ��û����ȡ)

	// ÿ������б�
	Gift arWeekGiftList[MAX_VIP_WEEK_GIFT_NUM];
	WORD wWeekGiftNum;

	WORD wTodayRecommendRechargeRewardId;	// �����Ƽ���ֵ����Id
	WORD wTodayRecommendRechargeRewardStatus; // �����Ƽ���ֵ����״̬(0=������ȡ,1=����ȡ,2=����ȡ)
};

// �ȼ��̵���Ϣ
// struct LevelShopInfo
// {
// 	WORD wLevel;			// �ȼ�
// 	DWORD dwActivateTime;	// ����ʱ��
// };

// �ȼ��̵�����
struct LevelShopActivityData
{
	WORD wLevel;			// �ȼ�
	DWORD dwActivateTime;	// ����ʱ��
};


// ������������
struct OpenServerFundData
{
	WORD wIsAlreadyBuyFund;	// �Ƿ��ѹ������
	WORD arFundRewardTakeRecord[MAX_OPEN_SERVER_FUND_REWARD];	// ��������ȡ��¼	
	WORD wFundRewardTakeRecordNum;
};

// ȫ��������
 struct AllPeopleWelfareData
 {
 	// ��ȡ������¼
	 WORD arTakeWelfareRecordList[MAX_ALL_THE_PEOPLE_WELFARE_NUM];
	 WORD wTakeWelfareRecordNum;
 };

 // ��ʱ�Ż�����
 struct LimitPreferentialData
 {
	 WORD wBuySchedule;				// �������
	 WORD wRechargeId;				// ���Żݵĳ�ֵId(0=��ʾľ��)
	 DWORD dwStartPreferentialTime;	// ��ʼ�Ż�ʱ��

	 WORD arTakeWelfareRecordList[MAX_LIMIT_PREFERENTIAL_WELFARE_NUM];	// ��ȡ������¼�б�
	 WORD wTakeWelfareRecordNum; // ��ȡ������¼����	
 };

 // ��ֵ�ۿ�����
 struct SuperDiscountData
 {
	 WORD wStatus;						// ����״̬(0=��û����, 1=�ѹ���(����ȡ), 2=����ȡ)
	 WORD wTakeAccumulateRewardTimes;	// ����ȡ�ۻ������Ĵ���(�³�ֵ�ۿ�ʹ��)
	 WORD wCurBuyTimes;					// ��ǰ�������(��ȡ����ʱ������)
	 WORD wLastActivityDataId;			// ���һ�λ����Id

	 // ����ȡ�ۻ�������¼(�ɳ�ֵ�ۿ�ʹ��)
	 WORD arTookAccumulateRewardList[MAX_SUPER_DISCOUNT_ACCUMULATE_REWARD];
	 WORD wTookAccumulateRewardNum;
 };



 // ������Ʒ����
 struct HotSaleGoods
 {
	 WORD wGoodsId;		// ��ƷID
	 WORD wCurDataIdx;	// ��ǰ��������
	 WORD wBuyTimes;	// �������
	 WORD wTakeTimes;	// ��ȡ����
 };

 // ���������Ʒ��Ϣ
 struct PlayerHotSaleGoodsInfo
 {
	 HotSaleGoods arGoodsList[MAX_HOT_SALE_GOODS_NUM];
	 WORD wGoodsNum;
	 WORD wLastActivityDataId;
 };


 // ���ʥ��������
 struct PlayerChristmasData
 {
	 // �ܻ���
	 DWORD dwTotalScore;

	 // ��ǰ����
	 DWORD dwCurScore;

	 // ����ȡ�ĵͼ�BOSS�����ȼ�
	 WORD wAlreadyTakeLowerBossRewardLevel;

	 // ����ȡ�ĸ߼�BOSS�����ȼ�
	 WORD wAlreadyTakeHighBossRewardLevel;

	 // �����������ʱ���
	 DWORD dwLastResetDataTime;
 };

 // facebook������Ϣ
 struct FacebookFriendInfo
 {
	 WORD wFriendNum;
	 char arFriendList[MAX_FACEBOOK_FRIEND_NUM][MAX_FACEBOOK_ACCOUNT_LEN];
 };


 // �ٻغ�������
 struct RecallFriendData
 {
	 DWORD dwTime; // �ٻ�ʱ��
	 DWORD dwFriendId; // ����ID
 };

 // Facebook�����
 struct FacebookActivityData
 {
	 WORD wIsAlreadyTakeAccountBindingReward; // �Ƿ�����ȡ�ʺŰ󶨽���

	 DWORD dwReadyToShareTime;		// ׼������ʱ��(0=��ʾľ��,1=��ʾ�ѷ����)

	 DWORD dwReadyToCommendTime;	// ׼������ʱ��(0=��ʾľ��,1=��ʾ�����۹�, ����1��ʾ׼������ʱ��)

	 // �ٻسɹ��ĺ�������
	 WORD wSummonSuccessFriendNum;

	 // ����ɹ��ĺ�������
	 WORD wInviteSuccessFriendNum;

	 // �ٻؽ�����¼(����ȡ��)
	 WORD arSummonRewardRecord[MAX_FACEBOOK_SUMMON_REWARD_RECORD];
	 WORD wSummonRewardRecordNum;

	 // ���뽱����¼(����ȡ��)
	 WORD arInviteRewardRecord[MAX_FACEBOOK_INVITE_REWARD_RECORD];
	 WORD wInviteRewardRecordNum;

	 // �ѷ����ٻصĺ��Ѽ�¼(��Ϸ�ں���)
	 RecallFriendData arSummonFriendRecord[MAX_FRIEND_NUM];
	 WORD wSummonFriendRecordNum;

	 // �ѷ�������ĺ��Ѽ�¼(facebook����)
	 FacebookFriendInfo tSendInviteFriendInfo;
 };

 // unlock�����
 struct UnlockActivityData
 {
	 WORD wIsScoreForShop;			// �Ƿ��Ѷ��̳�����
	 WORD wWatchingVideosTimes;		// �ۿ���Ƶ����
 };

 // ���߽�������
 struct OnlineRewardData
 {
	 DWORD dwOnlineTime;// ����ʱ��(��)
	 DWORD dwStartTime;	// ��ʼʱ��(���ε�¼����ʱ��Ŀ�ʼ��¼ʱ���)(����Ҫ��¼���ݿ�)
	 WORD arRewardRewardList[MAX_ONLINE_REWARD_DAY][MAX_ONLINE_REWARD_TIME_LIST]; // ������¼�б�[days][flag] (0=��ʾ��û��ȡ,1=��ʾ����ȡ)
 };

 // 7���¼����
 struct SevenDayLoginData
 {
	 WORD arDailyDataList[MAX_SEVEN_DAY_LOGIN_DAYS];	// ÿ�������б�[1=��ʾ�ѵ�¼,2=��ʾ����ȡ,index=����]
 };

 // ��ֵ��������
 struct RecahrgeRewardData
 {
	 WORD wLastActivityDataId;			// �����ID	
	 WORD wIsTookExtraReward; // �Ƿ�����ȡ���⽱��
	 WORD arDailyDataList[MAX_RECHARGE_REWARD_DAY_NUM]; // ÿ�������б�(0=��û��ֵ,1=�ѳ�ֵ,2=����ȡ) 
 };

 // �ۻ���������
 struct TotalRechargeRewardData
 {
	 WORD wLastActivityDataId;	// �����ID
	 DWORD  dwTotalAmount;	// �ܵĳ�ֵ���
	 WORD arRewardRecordList[MAX_TOTAL_RECHARGE_REWARD_NUM]; // ������¼�б�
	 WORD wRewardRecordNum;
 };

 // �����Ӫ��ļ����
 struct PlayerCampRecruitData
 {
	 WORD wCurDataId;					// ��ǰ�����ID
	 WORD wTodayAlreadyRecruitTimes;	// ��������ļ����
	 DWORD arCampScoreList[CAMP_NUM];	// ��Ӫ�����б�

	 // ������������¼�б�(����ȡ�ļ�¼)
	 WORD arScoreTaskRewardRecordList[MAX_CAMP_RECRUIT_SCORE_TASK_NUM];
	 WORD wScoreTaskRewardRecordNum;
 };

// ��һ����
struct PlayerActivityData
{
	// ӭ��������
	YCSData	tYCSData;				

	// ͭȸ̨����
	TQTData	tTQTData;
									
	// 7������
	SevenDayTargetData arSevenDayTargetList[MAX_SEVEN_DAY_TARGET_NUM];
	WORD wSevenDayTargetNum;
	// 7����ɶȽ�����¼
	WORD arCompletionRewardRecord[MAX_SEVEN_DAY_COMPLETION_REWARD];
	WORD wCompletionRewardRecordNum;

	// VIP�������
	VipGiftData tVipGiftData;	

	// �ȼ��̵�����
	LevelShopActivityData arLevelShopActivityDataList[MAX_LEVEL_SHOP_ACTIVITY_NUM];
	WORD wLevelShopActivityDataNum;

	// ��������
	OpenServerFundData tOpenServerFundData;

	// ȫ����
	AllPeopleWelfareData tAllPoepleWelfareData;

	// ��ʱ�Ż�����
	LimitPreferentialData tLimitPreferentialData;

	// �ȼ������ȡ��¼
	WORD arLevelGiftTakeRecord[MAX_LEVEL_GIFT_TAKE_RECORD_NUM];
	WORD wLevelGiftTakeRecordNum;

	// ��ֵ����
	RoleRechargeData tRechargeData;

	// ��ʱ�
	LimitActivityData tLimitActivityData;

	// �Ƿ���ȡ�˴��ս���
	WORD wIsTakeNextDayReward;

	// ��ֵ�ۿ�����
	SuperDiscountData tSuperDiscountData;

	// ���ջ
	PlayerFestivalActivityInfo tFestivalActivityInfo;

	// ��ɫ��׼�Ƽ�����
	PlayerAccurateRecommendData tAccurateRecommendData; 

	// ��ɫʥ��������
	PlayerChristmasData tChristmasData;

	// facebook�����
	FacebookActivityData tFacebookActivityData;

	// ���߽�������
	OnlineRewardData tOnlineRewardData;

	// 7���¼����
	SevenDayLoginData tSevenDayLoginData;

	// ��ֵ��������
	RecahrgeRewardData tRechargeRewardData;

	// �ۻ���������
	TotalRechargeRewardData tTotalRechargeRewardData;

	// �³�ֵ�ۿ�����
	SuperDiscountData tNewSuperDiscountData;

	// ������Ʒ����
	PlayerHotSaleGoodsInfo tPlayerHotSaleGoodsInfo;

	// �����Ӫ��ļ����
	PlayerCampRecruitData tPlayerCampRecruitData;

	// unlock�����
	UnlockActivityData tUnlockActivityData;

	// ��ֵ���������а�����
	RechargeAndCostDiamondRankData tRechargeAndCostDiamondRankData;
};

// ��ɫ��ʱ����
struct RoleTempData
{
	WORD wType;
	DWORD arParamList[MAX_ROLE_TEMP_DATA_PARAM_NUM];
	WORD wParamNum;
	char szText[MAX_ROLE_TEMP_DATA_TEXT_LEN];
};

struct RoleTempDatas
{
	RoleTempData arTempDataList[MAX_ROLE_TEMP_DATA_NUM];
	WORD wTempDataNum;
};

//��ɫ������Ϣ
struct RoleFullInfo
{
	RoleBaseInfo			tRoleBaseInfo;			// ��ɫ������Ϣ		max_role_base_info_size ok
	PlayerEquipmentInfo		tEquipmentInfo;			// װ����Ϣ			max_role_equipment_info_size ok
	PlayerHeroInfo			tHeroInfo;				// Ӣ����Ϣ			max_role_hero_info_size ok
	PlayerTreasureInfo		tTreasureInfo;			// ������Ϣ			max_role_treasure_info_size	ok		
	PlayerItemInfo			tItemInfo;				// ��Ʒ��Ϣ			max_role_item_info_size ok
	PlayerEmailInfo			tEmailInfo;				// �ʼ���Ϣ			max_role_email_info_size ok	
	PlayerFubenInfo			tFubenInfo;				// ������Ϣ			max_role_fb_info_size ok
	PlayerShopData			tShopInfo;				// �̵���Ϣ			max_role_shop_info_size ok
	PlayerSpriteInfo		tSpiritInfo;			// ������Ϣ			max_role_spirit_info_size ok
	PlayerFriendData		tFriendInfo;			// ������Ϣ			max_role_friend_info_size ok
	PlayerRebelData			tRebelInfo;				// �Ѿ���Ϣ			max_role_rebel_info_size ok
	PlayerAchievementData	tAchievementInfo;		// �ɾ���Ϣ			max_role_achievement_info_size ok
	PlayerDailyMissionData  tDailyMissionInfo;		// �ճ�������Ϣ		max_role_daily_mission_info_size ok
	PlayerGuildSkillData	tGuildSkillData;		// ���˹��Ἴ������	max_role_guild_skill_data_size ok
	SignInData				tSignInData;			// ǩ������			max_role_signin_data_size ok
	PlayerActivityData		tActivityInfo;			// ����� ok			
	PlayerZhanBuInfo		tZhanBuInfo;			// ռ����Ϣ			max_role_fairy_info_size (ľ)
	// PlayerTitleInfo			tTitleInfo;				// �ƺ���Ϣ	ok
	PlayerRewardData		tRewardInfo;			// ������Ϣ	ok
	//RechargeData			tRechargeData;			// ��ֵ���� ok
	PlayerDuobaoInfo        tDuobaoInfo;			// �ᱦ��Ϣ
	RoleTempDatas			tTempDatas;				// ��ʱ����

	RoleSecondInfo			tSecondInfo;				// ��ɫ������Ϣ
};

union BroadcastInfo
{
	struct OpenCardPacket
	{
		char	szPlayerName[MAX_ROLE_LEN];		//��ɫ��
		DWORD   dwCardID;
	} m_CardInfo;

	struct Financial_Plan
	{
		char	szPlayerName[MAX_ROLE_LEN];		//��ɫ��
		BYTE   byBuyType;
	} m_Financial_Plan;

};


// ������ȫ������
struct ServerGolbalData
{
// 	WORD wWorldBossLevel;			// ����BOSS�ȼ�
// 	DWORD dwOpenServerFundBuyTimes;	// �������������
	WORD wServerId;
	int nKey;
	DWORD dwValue;
};

// ������
struct RateData
{
	DWORD dwMin;
	DWORD dwMax;
	int nRate;
};

// ������
struct RateDataEx
{
	int nValue;
	int nRate;
};

// ��ʼս������
struct BeginBattleData
{
	BYTE byType;			// ս������
	WORD wChapterLevel;		// �½�
	WORD MapLevel;			// �ؿ��ȼ�
};

// װ����Ϣ(�ͻ���)
struct EquipmentClientData
{
	DWORD dwUID;
	WORD wInfoId;			// ����ID
	WORD wEnhanceLevel;	// ǿ���ȼ�
	BYTE byRefineLevel;		// �����ȼ�
	DWORD dwRefineExp;		// ��������
	WORD wWearHeroId;		// ������Ӣ��ID 
	int nEnhanceCostGold;	// ǿ�����ĵĽ��
	BYTE byStar;				// ����
	BYTE byStarLevel;		// �Ǽ�
};

// �����¼�ķ�������Ϣ
struct LastLoginServerData
{
	DWORD dwServerId;	// ������ID
	DWORD dwHeadIcon;	// ͷ��ͼ��
	WORD wLevel;		// ��ɫ�ȼ�
	char szRoleName[MAX_ROLE_LEN + 1]; // ��ɫ����
};

// ��������
struct LeaveMessageData
{
	DWORD dwRoleId;
	char szMessage[MAX_RANK_LEAVE_MESSAGE_LEN];
};

// ����ɨ����������
struct SweepDropData
{
	BYTE byTimes; // �ڼ�����ɨ��
	vector<Item> vDropItemList;	// ������Ʒ�б�
};

// �ͻ��˽�������
struct ClientRewardData : public RewardData
{
	vector<Item> vItemList;				// ��Ʒ�����б�(todo::Ŀǰֻ�Ƿ�����Ϣʱʹ��)
	vector<MoneyData> vCurrencyList;	// ���ҽ����б�(todo::Ŀǰֻ�Ƿ�����Ϣʱʹ��)
};

// ��һ�������
struct PlayerBaseData : public PlayerCommonData
{
	vector<ToBattleHeroData> vToBattleHeroList; // ����Ӣ���б�
	vector<SpiritData> vSpriteList;				// �����б�

	set<DWORD> setFriendGiveEnergyList;		// ���;�������б�
	set<DWORD> setFriendApplyList;			// ��������б�
	BYTE byFriendNum;						// ��������
	vector<RewardData> vRewardList;			// �����б�
	int nTodayHelpBattleFriendShipPoint;	// ������ս��õ������
	DWORD dwLastTakeFriendShipPointTime;	// ���һ�λ��������ʱ��

	PlayerBaseData()
	{
		Init();
	}

	void Init()
	{
		dwRoleId = 0;
		wLevel = 0;
		wHeadIcon = 0;
		dwFightingPower = 0;
		memset(szRoleName, 0, sizeof(szRoleName));
		memset(szGuildName, 0, sizeof(szGuildName));
		dwOfflineTime = 0;
		vToBattleHeroList.clear();
		setFriendGiveEnergyList.clear();
		setFriendApplyList.clear();
		byFriendNum = 0;
		nTodayHelpBattleFriendShipPoint = 0;
		dwLastTakeFriendShipPointTime = 0;
	}
};


// �ͻ��˽�ɫ��������
struct ClientRoleRankData
{
	DWORD dwRoleId;							// ID
	DWORD dwServerId;						// ������ID
	char szRoleName[MAX_ROLE_LEN];			// ����
	WORD wHeadIcon;							// ͷ��
	WORD wLevel;							// �ȼ�
	WORD wRank;								// ����
	DWORD dwFightingPower;					// ս��
	DWORD dwValue;							// ������ֵ
	DWORD dwParam1;							// ����1
	DWORD dwParam2;							// ����2
	DWORD dwUpdateTime;						// ����ʱ��
	//DWORD dwLikeTimes;						// ���޴���
	char szGuildName[MAX_GUILD_NAME_LEN];	// ��������
	BYTE byVipLevel;							// VIP�ȼ�
	DWORD dwTitleId;						// �ƺ�ID	

	ClientRoleRankData& operator=(const PlayerBaseData &tBaseData)
	{
		//this->dwLikeTimes = 0;
		this->dwRoleId = tBaseData.dwRoleId;
		this->dwServerId = 0;
		this->wHeadIcon = tBaseData.wHeadIcon;
		this->wLevel = tBaseData.wLevel;
		this->wRank = 0;
		this->dwValue = 0;
		this->dwParam1 = 0;
		this->dwParam2 = 0;
		this->dwFightingPower = tBaseData.dwFightingPower;
		this->byVipLevel = tBaseData.byVipLevel;
		strcpy_s(this->szRoleName, sizeof(this->szRoleName) - 1, tBaseData.szRoleName);
		strcpy_s(this->szGuildName, sizeof(this->szGuildName) - 1, tBaseData.szGuildName);
		this->dwTitleId = tBaseData.dwTitleId;
		return *this;
	}
};

// �ͻ��˹�����������
struct ClientGuildRankData
{
	DWORD dwGuildId;						// ID
	char szGuildName[MAX_GUILD_NAME_LEN];	// ����
	char szCaptainName[MAX_ROLE_LEN];		// �᳤����
	WORD wIconId;							// ����ͼ��ID
	WORD wLevel;							// �ȼ�
	BYTE byMemberNum;						// ��Ա����
	DWORD dwValue;							// ����ֵ

// 	ClientGuildRankData& operator=(const GuildData &tGuildData)
// 	{
// 		this->dwGuildId = tGuildData.dwId;
// 		this->wIconId = tGuildData.wIconId;
// 		this->wLevel = tGuildData.wLevel;
// 		this->byMemberNum = tGuildData.byMemberNum;
// 		strcpy_s(this->szGuildName, sizeof(this->szGuildName) - 1, tGuildData.szName);
// 		return *this;
// 	}
};

// GM�¼�����
struct GMEventData
{
	DWORD dwIdx;	// ��������
	WORD wEventId;	// �¼�ID
	DWORD arParamList[10];
	char szText[MAX_SYSTEM_NOTICE_LEN + 1];
	char szText1[MAX_SYSTEM_NOTICE_LEN + 1];
};

// ʱ���¼�����
struct TimeEventData
{
	WORD wEventId;				// �¼�ID
	BYTE byTimeType;			// ʱ������ 1.���ʱ�� 2.ÿ�� 3.���� 4.���� 5.����
	int nCD;					// CDʱ��
	int nIntervalTime;			// ���ʱ��(��λ��)
	int nStarValidTime;			// ��ʼ��Чʱ��[��:��:Сʱ:����]
	int nTriggerDate;			// ��������
	int nTriggerTime;			// ����ʱ��
	DWORD dwLastTriggerTime;	// �ϴδ���ʱ��

	int nBeginTime;				// ��ʼʱ��
	int nEndTime;				// ����ʱ��
};

// ��������������
struct WarcraftRankData
{
	DWORD dwRoleId;			// ��ɫID
	DWORD dwServerId;		// ������ID
	WORD wRank;				// ����
	DWORD dwFightingPower;	// ս��
	WORD wBeCheerTimes;		// ����������
};

// ������Ŀ������
struct WarcraftTargetData
{
	DWORD dwRoleId;						// Ŀ��Id
	WORD wHeadIcon;						// ͷ��ͼ��
	char szRoleName[MAX_ROLE_LEN];		// Ŀ������
	WORD wRank;							// ����
	DWORD dwFightingPower;				// ս��
};

// �ͻ�����������������
struct ClientWarcraftRankData : public WarcraftRankData
{
	DWORD dwHeadImageId;
	char szRoleName[MAX_ROLE_LEN + 1];
};

// �������
struct RedPointData
{
	WORD wModuleId;	// ģ��ID
	BYTE byState;	// ״̬(1=��,0=��)
};

// ϵͳ�������
struct SystemNoticeParam
{
	char szText[MAX_SYSTEM_NOTICE_PARAM_LEN+1];
};

// ������ȫ������
struct ServerGlobalData
{
	DWORD dwOpenServerFundBuyTimes;			// �������������
	WORD wChapterPassRecord;				// �½�ͨ�ؼ�¼
	DWORD dwLimitActivityGoodsBuyTimes;		// ��ʱ���Ʒ�������
	DWORD dwFightingPowerRecordIdx;			// ս����¼����
	WORD wAttackCityChapter;				// ���ǹ��½�(׼��Ҫȥ������)
	DWORD dwAttackCityRankNO1RoleId;		// ����������һ�����ID(0=��ʾľ��)
	WORD wChristmasLowerBossLevel;			// ʥ���ڵͼ�boss�ȼ�
	WORD wChristmasHighBossLevel;			// ʥ���ڸ߼�boss�ȼ�
	DWORD dwChristmasLastResetTime;			// ʥ������������ʱ��
};

// �ֻ�����֤����
struct MobileAuthData
{
	string strMobile;	// �ֻ���
	DWORD dwTime;		// ��֤ʱ���
	WORD wCode;			// ��֤��
};

// ��սĿ������
struct HelpBattleTargetData
{
	DWORD dwRoleId;					// ��ɫId
	char szRoleName[MAX_ROLE_LEN];	// ��ɫ����
	WORD wLevel;					// �ȼ�
	BYTE byVipLevel;				// VIP�ȼ�
	DWORD dwFightingPower;			// ս��
	DWORD dwHeroInfoId;				// ��սӢ����ϢID
	BYTE byTargetType;				// Ŀ������(0=İ����, 1=����, 2=���ų�Ա)
	BYTE byHelpBattleTimes;			// ��ս����
	DWORD dwTitleId;				// �ƺ�ID
};

// �ؿ�����
struct MapComment
{
	DWORD dwRoleId;		// ������ID
	WORD wCommentId;	// ����ID
	char szText[BUFF_LEN_128];	// ��������
	DWORD dwTime;		// ����ʱ��
	WORD wLikeTimes;	// ���޴���
	//set<DWORD> vLikePlayerList; // ��������б�
	DWORD arLikePlayerList[MAX_MAP_COMMENT_LIKE_NUM];
	WORD wLikePlayerNum;
};

// �ؿ�����
struct MapComments
{
// 	DWORD dwServerId;
// 	DWORD dwMapId;
	DWORD dwId; // (server_id + map_id)
	MapComment arCommentList[MAX_MAP_COMMENT_NUM];
	WORD wCommentNum;
};

// �ؿ�����
struct MapCommentClient : public MapComment
{
	char szRoleName[MAX_ROLE_LEN];	// ����������
	BYTE byVipLevel;		// VIP�ȼ�		
};

// PVP�ȴ���ս�Ľ�ɫ����
struct PVPWaitBattleRoleData
{
	DWORD dwRoleId;
	DWORD dwFightingPower;
	//DWORD dwToBattleSpiritId; // ��ս����
	DWORD dwSpiritSkillId; // ���鼼��ID
	DWORD dwTime;			// ����ʱ��
};

// PVPӢ������
struct PVPHeroData : public ToBattleHeroData
{
	//DWORD dwHeroInfoId;
	DWORD dwTotalHP;
	INT nCurHP;			// ��ǰHP

	BlessSpriteData tBlessSpriteData; // ף����������

	//int arProList[HERO_PRO_MAX];	// �����б�
};

// PVP��ɫս������ 
struct PVPRoleBattleData
{
	DWORD dwRoleId;
	char szRoleName[MAX_ROLE_LEN];
	WORD wHeadImageId;
	DWORD dwServerId;
	bool bIsReady;	// �Ƿ���׼����
	BYTE byType;	// 1=�췽,2=����
	DWORD dwFightingPower;
	DWORD dwToBattleSpiritId; // ��ս����ID
	DWORD dwSpiritSkillId;	// ���鼼��
	WORD wSpriteEnhanceLevel; // ����ǿ���ȼ�
	WORD wSpriteTrainStage;	// ����������
	WORD wSpriteTrainTimes;	// ������������
	WORD wSpriteStar;		// ��������
	PVPHeroData arHeroList[MAX_BATTLE_HERO_NUM]; // Ӣ���б�
	WORD wHeroNum;
	SpiritData arSpriteList[MAX_SPIRIT_NUM];
	WORD wSpriteNum;
};

// PVPս������
struct PVPBattlefieldData
{
	DWORD dwBattleId; // ս��ID
	bool bIsInBattle;	// �Ƿ���ս����
	bool bIsRobot;		// �Ƿ���ս������
	DWORD dwStartTime;	// ��ʼʱ��(��ʼ׼��ʱ��,��ʼս��ʱ��)
	PVPRoleBattleData arRoleBattleDataList[PVP_BATTLEFIELD_ROLE_NUM];  // ��ɫս�������б�
};

// PVPս����������
struct PVPBattleOverData
{
	WORD wOldRank;	// ������(0=δ�ϰ�)
	WORD wNewRank;	// ������(0=δ�ϰ�)
	WORD wLastWeekRank; // ��������(0=δ�ϰ�)
	WORD wScore;	// ���λ�û���
};

typedef SceneItem TreasureBox;

// ����
// struct TreasureBox
// {
// 	DWORD dwUID;
// 	DWORD dwInfoId;
// 	DWORD dwCreateTime;	// ����ʱ��
// 	DWORD dwLockPlayerId; // ��ס��������Id
// 	DWORD dwLockTime; // ��סʱ��
// 	MapPos tMapPos;	// ��ͼ����
// };

// ���ǹؿ�
struct AttackCityMap
{
	DWORD dwUID;
	DWORD dwMapId;
	MapPos tMapPos;	// ��������
	bool bIsDead;	// �Ƿ�������
	//bool bIsInBattle;	// �Ƿ�������ս��
	DWORD dwStartBattleTime; // ��ʼս��ʱ��
	WORD wChapter;		// �����½�
	vector<MonsterData> vMonsterList;	// �����б�
};


// �����½�����
struct AttackCityChapter
{
	WORD wChapter; // �½�		
	vector<AttackCityMap> vMapList;	// �ؿ��б�
};

// ���ǵĹ���
struct AttackCityMonster
{
	DWORD dwUID;
	DWORD dwMapId;		// �ؿ�ID
	DWORD dwDamage;		// ��ǰ�˺�
	DWORD dwTotalHP;	// �ܵ�HP
	//BYTE byIsInBattle;	// �Ƿ���ս����
	DWORD dwStartBattleTime; // ��ʼս��ʱ��
	MapPos tMapPos;		// ��������

	AttackCityMonster& operator=(const AttackCityMap &tMap)
	{
		this->dwUID = tMap.dwUID;
		this->dwMapId = tMap.dwMapId;
		this->tMapPos = tMap.tMapPos;
		this->dwStartBattleTime = tMap.dwStartBattleTime;
		this->dwDamage = 0;
		this->dwTotalHP = 0;
		for (size_t i = 0; i < tMap.vMonsterList.size(); ++i)
		{
			const MonsterData &tMonster = tMap.vMonsterList[i];
			this->dwDamage += tMonster.dwDamage > tMonster.dwTotalHP ? tMonster.dwTotalHP : tMonster.dwDamage;
			this->dwTotalHP += tMonster.dwTotalHP;
		}
		return *this;
	}
};

// ��ҹ�������
struct PlayerAttackCityData
{
	DWORD dwBattleOverTime;	// ս������ʱ��
	WORD wReliveCD;			// ����CD(��)
	WORD wWeakCD;			// ����CD(��)
	BYTE byEncourageTimes;	// �������
	DWORD dwScore;			// ����
	DWORD dwBattleTargetId;	// ��ս��Ŀ��Id
	vector<WORD> vTakeBossRewardRecordList; // ��ȡboss������¼�б�
};


// ʱ������
struct TimeData
{
	int nStart;	// ��ʼ[Сʱ����] ��: 2230 22��30��
	int nOver;	// ����[Сʱ����]
};

// ��ɫ�ȼ���¼
struct RoleLevelRecord
{
	DWORD dwRoleId;			// ��ɫID
	WORD wLevel;			// ��ɫ�ȼ�
	WORD wVipLevel;			// VIP�ȼ�
	DWORD dwDiamond;		// ��ʯ
	DWORD dwGold;			// �ƽ�
	WORD wServerId;			// ������ID
	WORD wMainFBChapter;	// �����½�
	WORD wMainFBMap;		// ���߹ؿ�
	int nGuideX;			// ����X
	int nGuideY;			// ����Y
	DWORD dwFightingPower;	// ս��
	DWORD dwRecordTime;		// ��¼ʱ��
	WORD wHeroFBChapter;	// Ӣ�۸����½�
	WORD wEliteFBChapter;	// ��Ӣ�۸����½�
	WORD wTowerMapLevel;	// ��������
	WORD wLandNum;			// ���ռ������
	WORD wWorldHeartStage;	// ����֮�Ľ׶�
	WORD wArenaHistoryRank;	// ��������ʷ����
	WORD wItemNum;			// ��Ʒ����
	WORD wEquipmentNum;		// װ������
	WORD wHeroNum;			// Ӣ������
	WORD wTreasureNum;		// ��������
};

// ��ɫ���߼�¼ 
struct RoleOfflineRecord
{
	DWORD dwRoleId;			// ��ɫID
	WORD wLevel;			// ��ɫ�ȼ�
	WORD wVipLevel;			// VIP�ȼ�
	DWORD dwDiamond;		// ��ʯ
	DWORD dwGold;			// �ƽ�
	WORD wServerId;			// ������ID
	WORD wMainFBChapter;	// �����½�
	WORD wMainFBMap;		// ���߹ؿ�
	int nGuideX;			// ����X
	int nGuideY;			// ����Y
	DWORD dwOfflineTime;	// ����ʱ��
	DWORD dwOnlineTime;		// ����ʱ��(��)
};

// ��ֵ��¼
struct RechargeRecord
{
	DWORD dwRoleId;
	WORD wRoleLevel;	// ��ɫ�ȼ�
	WORD wServerId;		// ������ID
	WORD wRechargeId;	// ��ֵID
	WORD wRechargeType;	// ��ֵ����
	//WORD wCardId;		// �¿�ID
	WORD wFrom;			// ��ֵ��Դ
	DWORD dwAmount;		// ��ֵ���
	DWORD dwDiamond;	// ��ֵ��õ���ʯ
	DWORD dwRecordTime;	// ��ֵʱ���
	char szPayTime[32];	// ֧��ʱ��
	char szSDKOrderId[MAX_SDK_ORDER_ID_LEN]; // ����������
};

// �ؿ�ʧ�ܼ�¼
struct MapLoseRecord
{
	DWORD dwRoleId; // ��ɫID
	DWORD dwMapId;	// �ؿ�ID
	WORD wType;		// �ؿ�����
	WORD wChapter;	// �½�
	WORD wMapIdx;	// �ؿ�����
	DWORD dwFightingPower; // ս��
};

// �״�ͨ�ؼ�¼
struct FirstPassMapRecord
{
	DWORD dwRoleId; // ��ɫID
	WORD wLevel;	// ��ɫ�ȼ�
	DWORD dwMapId;	// �ؿ�ID
	WORD wType;		// �ؿ�����
	WORD wChapter;	// �½�
	WORD wMapIdx;	// �ؿ�����
	DWORD dwFightingPower; // ս��
};

/*
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<quicksdk_message>
<message>
<is_test>0</is_test>
<channel>8888</channel>
<channel_uid>231845</channel_uid>
<game_order>123456789</game_order>
<order_no>12520160612114220441168433</order_no>
<pay_time>2016-06-12 11:42:20</pay_time>
<amount>1.00</amount>
<status>0</status>
<extras_params>{1}_{2}</extras_params>
</message>
</quicksdk_message>
*/
// ��ֵ�ص�����
struct RechargeCallbackData
{
	BYTE byIsTest;					// ���� �Ƿ�Ϊ���Զ��� 1Ϊ���� 0Ϊ������ʽ��������ϷӦ�������ȷ�����ߺ��Ƿ�����Զ������ŵ��ߡ�
	DWORD dwChannel;				// ���� ������ʾID ע�� : ��Ϸ�ɸ���ʵ��, ȷ������ʱ�Ƿ�У���ֵ��Դ�����Ƿ���ý�ɫע���������
	//DWORD dwChannelUid;			// ���� �����û�Ψһ��ʾ, ��ֵ�ӿͻ���GetUserId()�пɻ�ȡ
	DWORD dwCPOrderId;				// ���� ��Ϸ�ڵ���QucikSDK����֧��ʱ���ݵ���Ϸ������, �����ԭ������
	char szSDKOrderId[BUFF_LEN_64];	// ���� QuickSDKΨһ������
	char szPayTime[BUFF_LEN_64];	// ���� ֧��ʱ�� 2015 - 01 - 01   23:00 : 00
	char szChannelUid[BUFF_LEN_64];	// ���� �����û�Ψһ��ʾ, ��ֵ�ӿͻ���GetUserId()�пɻ�ȡ
	int nAmount;					// ���� �ɽ�����λԪ����Ϸ���շ��ŵ��߽��Ӧ�Դ�Ϊ׼
	BYTE  byStatus;					// ���� ��ֵ״̬ 0   �ɹ� 1ʧ��(Ϊ1ʱ Ӧ����FAILEDʧ��)
	DWORD dwRoleId;					// ��Ϊ�� ��Ϸ�ͻ��˵���SDK����֧��ʱ��д��͸������.û����Ϊ�� (role_id)
	char szFailedDesc[BUFF_LEN_128];// ��Ϊ�� ʧ������
	WORD wRechargeId;				// ��ֵID
	WORD wRechargeType;				// ��ֵ����
	WORD wGoodsId;					// ��ƷID
	char szIP[BUFF_LEN_32];			// IP��ַ
	DWORD dwYDChannelId;			// �ƶ�����ID
	WORD wCurrencyType;				// ��������(0=�����, ��0��������(Ŀǰֻ����Ԫ))
	BYTE byIsMyCard;				// �Ƿ�mycard��ֵ
	BYTE byIsCheckRechargeStrId;	// �Ƿ����ֵ�ִ�Id
	char szRechargeStrId[BUFF_LEN_64];	// ��ֵ�ִ�Id
	BYTE byIsNotCheck;				// �Ƿ������(1=��,0=��)
	WORD wFrom;
};


// ��ֵ�ɹ��ϱ�����
struct RechargeSuccessReportData
{
	DWORD dwChannel;					// 	channel  ��  int  ��������(Ҫ��ͻ�����YundingKeyData.getChannel()��ȡ���յ����������������)
	DWORD dwServerId;					// 	server_id  ��  string  ���� ID
	char szAccountId[MAX_ACCOUNT_LEN];	// 	account_id  ��  string  �˺� ID
	DWORD dwRoleId;						// 	role_id  ��  string  ��ɫ ID
	char szRoleName[MAX_ROLE_LEN];		// 	role_name  ��  string  ��ɫ����
	WORD wRoleLevel;					// 	role_level  ��  int  ��ɫ�ȼ�[���޷���ȡ����Ĭ�ϴ� 0]
	WORD wVipLevel;						// 	vip_level  ��  int  VIP �ȼ�[���޷���ȡ����Ĭ�ϴ� 0]
	DWORD dwVipExp;						// 	vip_exp  ��  int  VIP ����[���޷���ȡ����Ĭ�ϴ� 0]
	char szOrderId[64];					// 	order_id  ��  string  �����ţ�֧��ƽ̨�ṩ�Ķ����ţ�	
	DWORD dwIapId;						// 	ap_id  ��  string  ��Ʒ����
	char szIapDes[32];					// 	iap_des  ��  string  ��Ʒ����
	DWORD dwCurrencyAmount;				// 	currency_amount  ��  double  �ֽ����ȷ��Ԫ�� ��V����
	DWORD dwVirtualCurrencyAmount;		// 	virtual_currency_amount ��  double  ��ֵ��ô�������
	DWORD dwVirtualCurrencyAmountEx;	// 	virtual_currency_amount_ex ��  double  �����ô�������
	char szIP[32];						// 	ip  ��  string  ��ҳ�ֵʱ�� IP ��ַ

	// 	sign  ��  string  md5_32 λСд(app_key + timestamp)timestamp  ��  int  �ϱ�ʱ���ʱ�������ȷ����
	// 	app_id  ��  string  ��Ʒ ID
	// 	currency_type  ��  int  �������͡����֡� 1 - RMB
	// 	platform_id  ��  int  ֧������ ID ֧������2
	// 	access_type  ��  int  ��ȡ��ʽ 1 - ��ֵ 2 - ����
};

// MyCard��ֵ�ɹ��ϱ�����
struct MyCardRechargeSuccessReportData
{
	DWORD dwVid;					//	��	int	����û�Ψһid����½�����
	char szIP[IP_LEN];				//	��	string	�û���IP��Ϣ
	char szIMSI[BUFF_LEN_64];		//  ��	string	imsi
	char szIMEI[BUFF_LEN_64];		//  ��	string	�ֻ�����
	char szVIMEI[BUFF_LEN_64];		//	��	string	vimei
	DWORD dwRoleId;					//  ��	string	��ɫid
	char szRoleName[MAX_ROLE_LEN];	//	��	string	��ɫ��
	DWORD dwServerId;				//	��	string	������id
	char szServerName[BUFF_LEN];	//  ��	string	��������
	char szOrderTime[BUFF_LEN_32];	//  ��	string	����ʱ�䣬��ʽ��2018 - 05 - 30 10:05 : 23
	char szOrderNo[BUFF_LEN_64];	//  ��  order_no	��	string	������
	int nPay;						//	��	int	�����ɹ�����λ������

	// 2.signǩ���㷨
	// �����е������ֶΰ����ֶ�����ASCII ���С���������ֵ��򣩺�ʹ��URL��ֵ�Եĸ�ʽ����key1 = value1&key2 = value2����
	// ƴ�ӳ��ַ���string1��������Ҫע��������в�������ΪСд�ַ���
	// ������sign�ֶΡ���string1��sha1���ܣ��ֶ������ֶ�ֵ������ԭʼֵ��������URL ת�塣
};



// ��ʯ��Ƕ
struct GemInlay
{
	DWORD dwGemId; // ��ʯId
	WORD wHole;	// ��λ
};


// ��Ʒ��ʧ��¼
struct ItemLoseRecord
{
	DWORD dwRoleId;
	DWORD dwItemId;
	DWORD dwItemNum;
};

// ��Ʒ������¼
struct ItemOperationRecord
{
	WORD wFromId;
	DWORD dwRoleId;
	DWORD dwItemId;
	DWORD dwItemNum;
	DWORD dwTime;
	bool bIsAdd;
};

// ������������
struct EquipmentRefineCostData
{
	DWORD dwItemId;
	WORD wItemNum;
};

// Ť���齱��¼
struct NDLotteryRecord
{
	WORD wServerId;					// ������ID	
	char szRoleName[MAX_ROLE_LEN];	// ��ɫ����
	DWORD dwItemId;					// ���е���ƷID
	DWORD dwItemNum;				// ���е���Ʒ����
	DWORD dwTime;					// �齱ʱ��
};

// �ͻ�����Ϣ
struct ClientInfo
{
	char szDeviceId[MAX_DEVICE_ID_LEN]; // �ͻ����豸ID
	char szIP[IP_LEN];					// IP����
	BYTE byTerminalType;				// �ն�����
};

// ת�̻�������
struct TurntableBaseData
{
	DWORD dwServerId;			// ������ID
	WORD wCurActivityDataId;	// ��ǰ�����ID
	DWORD dwDiamond;			// ������ʯ����
};

// ת�̳齱��¼
struct TurntableLotteryRecord
{
	DWORD dwRoleId; // �齱�Ľ�ɫID
	DWORD dwTime;	// �齱ʱ���
	WORD wIndex;	// ���е���������
	DWORD dwContext;// ������(��Ʒ����,��ʯ)
};

// ת�̳齱��¼
struct ClientTurntableLotteryRecord : public TurntableLotteryRecord
{
	char szRoleName[MAX_ROLE_LEN];	// �齱�Ľ�ɫ����
};

// ת�̳齱�������
struct TurntableLotteryResultData
{
	WORD wIndex;		// ��������
	DWORD dwContext;	// ������(��Ʒ����,��ʯ)
};

// ��Ӫ��ļ��¼
struct CampRecruitRecord
{
	char szRoleName[MAX_ROLE_LEN];	// ��ļ�Ľ�ɫ����
	Item tRewardData;				// ��ļ���Ľ�������
	DWORD dwRecruitTime;			// ��ļʱ��
	WORD wCamp;						// ������Ӫ
};

// ��Դս��������
struct ResourceWarBaseData
{
	WORD wChapter;					// �½�
	WORD wResourceId;				// ��ԴID
};

// ��Դս��Դ���� 
struct ResourceWarResouceData
{
	WORD wChapter;						// �½�
	WORD wResourceId;					// ��ԴID
	WORD wResourceType;					// ��Դ����
	DWORD dwOccupyStartTime;			// ռ�쿪ʼʱ���
	DWORD dwOccupyOverTime;				// ռ�����ʱ���
	DWORD dwOwnerId;					// ӵ����ID
	WORD wKeepOccupyTimes;				// �ӳ�ռ�����(Ĭ�Ͼ��൱���ӳ���һ��,���Ǵ�����0��ʼ)
	//WORD wCurOccupyRewardIdx;			// ��ǰռ�콱������
	DWORD dwLastBaseRewardTime;			// �����û���������ʱ���
	DWORD dwLastExtraRewardTime;		// �����ö��⽱����ʱ���

	WORD wBaseRewardIntervalTime;		// ��û����������ʱ��(����)
	WORD wCurExtraRewardNeedTime;		// ��ǰ��ö��⽱����Ҫ��ʱ��(����)
	WORD wAlreadySendBaseRewardTimes;	// �Ѿ����ŵĻ�����������
	WORD wAlreadySendExtraRewardTimes;	// �Ѿ����ŵĶ��⽱������
};

// ��Դս����(�ͻ���) 
struct ResourceWarDataClient
{
	PlayerCommonData tOwnerInfo;	// ӵ������Ϣ
	//WORD wChapter;					// �½�
	WORD wResourceId;				// ��ԴID
};

// ��Դս��ս��¼ ����ս�������ռ���ˡ�����ս��������ڵ�X�µġ�������Դ��
struct ResourceWardBattleRecord
{
	DWORD dwAttackerId;	// ������ID
	DWORD dwDefenderId;	// �ط���ID
	DWORD dwBattleTime;	// ��սʱ��
	WORD wChapter;		// �½�
	WORD wResourceId;	// ��ԴID
};

// ��Դս��¼(�ͻ���)
struct ResourceWardRecordClient
{
	char szAttackerName[MAX_ROLE_LEN];
	char szDefenderName[MAX_ROLE_LEN];
	DWORD dwBattleTime;	// ��սʱ��
	WORD wChapter;		// �½�
	WORD wResourceId;	// ��ԴID
};

// ��Դս������Ϣ
struct ResourceWarEnemy
{
	DWORD dwRoleId;
	WORD wAttackTimes;	// ��������
};

// ��Դս������Ϣ
struct ResourceWarEnemyClient
{
	PlayerCommonData tCommonData;
	//DWORD dwRoleId;
	WORD wAttackTimes;	// ��������
	WORD wChapter;		
	WORD wResourceId;	// 0=��ʾľ��ռ��
};


// ��ɫ��Դս����
struct RoleResourceWarData
{
	WORD wTechnologyLevel;			// �Ƽ��ȼ�
	DWORD dwTotalAccupyTime;		// ��ռ��ʱ��(��)
	int nRewardResourceWarScore;	// ������Դս����
};

// ��������������
struct ServerOnlineData
{
	WORD wServerId;			// ������ID
	WORD wOnlinePlayerNum;	// �����������
};

// ��ɫս��
struct RolePower
{
	DWORD dwRoleId;
	DWORD dwPower;
};

#pragma pack(pop)
#endif
