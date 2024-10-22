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


// 邮件唯一ID
struct EmailGUID
{
	DWORD dwInserTime;		// 产生邮件时间戳
	DWORD dwSerialId;		// 序列ID
};

// 邮件上下文数据
struct EmailContext
{
	WORD wType;			// 数据类型(1=数字,2=字符串)
	char szText[BUFF_LEN];	// 文本
};

//邮件
struct EmailInfo
{
	DWORD	dwID;					// 邮件ID = yearday + 0-1000000
	WORD	wInfoId;				// 邮件信息ID
	DWORD	dwReceiveTime;			// 接收时间
	EmailContext arContextList[MAX_EMAIL_CONTEXT_NUM];	// 上下文列表
	WORD  wContextNum; // 上下文数量	

	char szTitle[MAX_EMAIL_TITLE_LEN]; // 标题
	char szText[MAX_EMAIL_TEXT]; // 内容

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
	DWORD dwItemId;	// 物品ID
	WORD wItemNum;	// 物品数量
};

// 奖励物品
struct RewardItem
{
	DWORD dwItemId;
	WORD wNumMin;
	WORD wNumMax;
};

// 角色邮件信息
struct PlayerEmailInfo
{
	WORD wIsHaveNewEmail; // 是否有新邮件

	EmailInfo arEmailList[MAX_EMAIL_COUNT];	// 邮件列表
	WORD wEmailCount;						// 邮件数量
};

// 怪物掉落数据
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

// 聊天语音
struct ChatVoice
{
	char szVoice[MAX_CHAT_VOICE_LEN];
	//WORD wVoiceLen;
};

//聊天请求数据 
struct ChatReqData
{
	BYTE	byChatType;								//聊天类型
	BYTE	byIsVoice;								//是否语音
	DWORD	dwRoleId;								//目标角色ID
	char	szTargetName[MAX_ROLE_LEN+1];		//聊天目标名
	char	szChatText[MAX_CHAT_TEXT_LEN+1];			//聊天文本
	char	szVoice[MAX_CHAT_VOICE_LEN+1];			//语音内容
};

// 聊天角色数据
struct ChatRoleData
{
	DWORD	dwRoleID;							// 角色ID
	//WORD	wLevel;								// 角色等级
	DWORD	dwHeadIcon;							// 角色头像
	WORD    wServerId;							// 服务器ID
	BYTE	byVipLevel;							// VIP等级
	DWORD	dwCurUseNicknameId;					// 当前使用的昵称ID
	char	szRoleName[MAX_ROLE_LEN];		// 角色名称
};

//聊天通知数据
struct ChatNotifyData
{
	BYTE byChatType;						// 聊天类型
	ChatRoleData tSenderData;				// 发送者数据
	DWORD dwVoiceId;						// 语音ID
	char szChatText[MAX_CHAT_TEXT_LEN];		// 聊天文本
	//char szVoice[MAX_CHAT_VOICE_LEN];		// 语音
	void CleanUp()
	{
		memset(this, 0, sizeof(ChatNotifyData));
	}
};

struct ChatRecord : public ChatNotifyData
{
	DWORD dwTime;							// 时间戳
	BYTE byGMLevel;							// todo::临时功能
};

// 上阵英雄数据
struct ToBattleHeroData
{
	WORD wHeroInfoId;					// 英雄信息ID
	BYTE byPos;							// 位置
	WORD wLevel;						// 英雄等级
	BYTE byQuality;						// 品质等级
	BYTE byProNum;						// 属性数量
	Property arPorList[HERO_PRO_MAX];	// 属性列表
	BlessSpriteData tBlessSpriteData;	// 祝福精灵数据
};

struct DBPlayerBaseData : public PlayerCommonData
{
	ToBattleHeroData arToBattleHeroList[MAX_BATTLE_HERO_NUM];	// 上阵英雄列表
	BYTE byToBattleHeroNum;
	//WORD wServerId;
};



// VIP数据
struct VipData
{
	WORD wLevel;						// vip等级
	DWORD dwExp;						// vip经验
};

// 月卡数据
struct MonthCard
{
	WORD wCardId;				// 月卡ID
	DWORD dwBuyTime;			// 购买时间
	DWORD dwLastTakeRewardTime;	// 最近领取奖励时间
};

// 普通充值数据
struct CommonRecharge
{
	WORD wRechargeId;		// 充值ID
	WORD wRechargeTimes;	// 充值次数
	//WORD wTakeRewardTimes;	// 领取奖励次数
};

// 限时活动数据
struct LimitActivityData
{
	// 高级招募次数
//	WORD wRecruitTimes;

	// 总的充值
	DWORD dwTotleRecharge;

	// 招募奖励记录
// 	WORD arRecruitRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
// 	WORD wRecruitRewardRecordNum;
// 
// 	// 累积充值奖励记录
// 	WORD arAccumulateRechargeRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
// 	WORD wAccumulateRechargeRewardRecordNum;
// 	WORD wLastAccumulateRechargeDataId; // 上次累充活动数据Id
// 	DWORD dwAccumulateRecharge; // 累积充值(会重置)
// 
// 	// 单次充奖励记录
// 	RecordData arSingleRechargeRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
// 	WORD wSingleRechargeRewardRecordNum;
// 	WORD wLastSingleRechargeDataId; // 上次活动单充数据Id
// 	// 充值列表(作为活动条件数据,会重置)
// 	CommonRecharge arRechargeList[MAX_RECHARGE_NUM];
// 	WORD wRechargeNum;

	// 首充奖励记录
	WORD arFirstRechargeRewardRecord[MAX_LIMIT_ACTIVITY_REWARD_NUM];
	WORD wFirstRechargeRewardRecordNum;
};

// 活动目标数据
struct ActivityTarget
{
	WORD wId;				// 目标ID
	DWORD dwContext;		// 上下文(阶段值,完成次数)
	WORD wTakeRewardTimes;	// 领取奖励次数
};

// 节日活动数据
struct FestivalActivity
{
	// 活动目标数据
	ActivityTarget arTargetList[MAX_ACTIVITY_TARGET_NUM];
	WORD wTargetNum;

	// 数据重置时间
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

// 角色节日活动信息
struct PlayerFestivalActivityInfo
{
	// 1、循环单充
	FestivalActivity tLoopOnceRecharge;

	// 2、循环累充
	FestivalActivity tLoopAccumulateRecharge;

	// 3、开服限时召唤
	FestivalActivity tLimitRecruit;

	// 4、活动单充
	FestivalActivity tActivityOnceRecharge;

	// 5、活动累充
	FestivalActivity tActivityAccumulateRecharge;

	// 6、打折兑换（物品打折卖钻石,钻石换物品）
	FestivalActivity tDiamondExchange;

	// 7、活跃目标
	FestivalActivity tActiveTarget;

	// 8、材料兑换（物品换物品）
	FestivalActivity tItemExchange;

	// 9、节日登陆
	FestivalActivity tFestivalLogin;

	// 10、开服登陆
	FestivalActivity tOpenServerLogin;

	// 11 开服累充
	FestivalActivity tOpenSvrAccumulateRecharge;

	// 12 节日材料兑换
	FestivalActivity tFestivalItemExchange;

	// 13 节日打折贩售，逻辑同原类型6（打折贩售）
	FestivalActivity tFestivalDiscountSell;

	// 14 节日活跃目标，逻辑同原类型7（活跃目标）
	FestivalActivity tFestivalActiveTarget;

	// 15.开服活跃2(同7)
	FestivalActivity tOpenServerActiveTarget;

	// 16.节日活跃2(同14)
	FestivalActivity tFestivalActiveTarget2;

	// 17、活动单充
	FestivalActivity tActivityOnceRecharge2;
};

// 角色充值数据
struct RoleRechargeData
{
	// 充值列表
	CommonRecharge arRechargeList[MAX_RECHARGE_NUM];
	WORD wRechargeNum;

	// 月卡列表
	MonthCard arMonthCardList[MAX_MONTH_CAR_NUM];
	WORD wMontchCardNum;

	// 订单记录列表
	DWORD arCPOrderRecordList[MAX_ORDER_RECORD_NUM];
	WORD wCPOrderRecordNum;

	// 今日充值次数
	WORD wTodayRechargetTimes; 
};

// 商品
struct Goods
{
	WORD wId;	// 商品ID
	WORD wNum;	// 商品数量
};

// 礼包
struct Gift
{
	WORD wId;	// 礼包Id
	WORD wNum;	// 礼包数量
};

// 玩家物品信息
struct PlayerItemInfo
{
	Item arItemList[MAX_ITEM_NUM];	// 物品列表
	WORD wListNum;				// 物品列表数量
};

// 宝石
struct Gem
{
	DWORD dwId;	// 宝石ID
	DWORD dwNum;// 数量	
};

// 玩家宝石信息
struct PlayerGemInfo
{
	Gem arGemList[MAX_GEM_NUM]; // 宝石列表
	WORD wGemNum;	// 宝石数量
	WORD arHoleOpenList[MAX_BATTLE_HERO_NUM]; // 孔位开启列表
	DWORD dwHoleInitTime; // 孔位初始时间
};

// 装备
struct Equipment
{
	DWORD dwId;				// 唯一ID
	DWORD dwInfoId;			// 配置ID
	WORD wEnhanceLevel;		// 强化等级
	WORD wRefineLevel;		// 精炼等级
	DWORD dwRefineExp;		// 精炼经验
	WORD wStar;				// 星数
	WORD wStarLevel;		// 星级

	int nEnhanceCostGold; // 强化消耗的金币
	Item arRefineCostItem[MAX_EQUIPMENT_REFINE_COST_ITEM];	// 精练消耗的物品列表
	WORD wRefineCostItemNum;
};

// 玩家装备信息
struct PlayerEquipmentInfo
{
	Equipment sEquipmentList[MAX_EQUIPMENT_NUM];
	WORD wEquipmentNum;
};

// 宝物基本数据
struct TreasureBaseData
{
	DWORD dwId;				// 唯一ID
	WORD wInfoId;			// 配置ID
	BYTE byEnhanceLevel;	// 强化等级
	DWORD dwEnhanceExp;		// 强化经验
	BYTE byRefineLevel;		// 精炼等级
	WORD wWearHeroInfoId;	// 穿戴英雄ID
};

// 宝物
struct Treasure
{
	DWORD dwId;				// 唯一ID
	DWORD dwInfoId;			// 配置ID
	WORD wEnhanceLevel;		// 强化等级
	DWORD dwEnhanceExp;		// 强化经验
	WORD wRefineLevel;		// 精炼等级
	//DWORD dwRefineExp;		// 精炼经验
};

// 玩家宝物数据
struct PlayerTreasureInfo
{
	Treasure sTreasureList[MAX_TREASURE_NUM];
	WORD wTreasureNum;
};

// 刷新商店
struct RefreshShopData
{
	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // 商品列表
	WORD wGoodsNum;	// 商品数量

	DWORD dwLastRecoverTime;	// 上一次恢复时间(免费刷新次数恢复时间)
	WORD wFreeRefreshTimes;	// 已使用的免费刷新次数
	WORD wCostRefreshTimes;	// 已使用的消耗刷新次数		
};

// 星魂商店
struct StarSoulShopData
{
	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // 商品列表
	WORD wGoodsNum; // 商品数量

	WORD wUsedRefreshTimes;		// 已经使用的刷新次数
	DWORD dwLastAutoRefreshTime;	// 上次自动刷新时间
};

// 积分赛商品
struct ScoreMatchShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;		
};

// 7天商品
struct SevenDayShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// 等级礼包商品
struct LevelGiftShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};


// 玩家工会商品数据
struct PlayerGuildShopData
{
	ShopGoodsBuyRecord arAwardGoodsRecord[MAX_GUILD_AWARD_SHOP_GOODS_NUM];	// 奖励商品购买记录
	WORD wAwardGoodsRecordNum;												// 奖励商品购买数量

	ShopGoodsBuyRecord arItemGoodsRecord[MAX_GUILD_AWARD_SHOP_GOODS_NUM];	// 道具商品购买记录
	WORD wItemGoodsRecordNum;												// 道具商品购买数量

	// 这部分不需要记录,因为是工会共享的
	ShopGoodsBuyRecord arLimitGoodsRecord[MAX_GUILD_AWARD_SHOP_GOODS_NUM];	// 限时商品购买记录
	WORD wLimitGoodsRecordNum;												// 限时商品购买数量
};

// 玩家商城数据
struct PlayerMallData
{
	ShopGoodsBuyRecord arItemBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wItemBuyRecordNum;

	ShopGoodsBuyRecord arGiftBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wGiftBuyRecordNum;
};

// 玩家爬塔商店数据
struct PlayerTowerShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// 玩家竞技场商店数据
struct PlayerArenaShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// 无尽之地商店
struct EndlessLandShopData
{
	// 道具商品
	ShopGoodsBuyRecord arItemGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wItemGoodsBuyRecordNum;

	// 奖励商品
	ShopGoodsBuyRecord arRewardGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wRewardGoodsBuyRecordNum;
};

// 直购商店
struct DirectBuyShopData
{
	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // 商品列表
	WORD wGoodsNum;	// 商品数量

	WORD wLastRefreshYearDay;	// 上次刷新年日			
	WORD wLastRefreshTime;	// 最后一次刷新的时间[小时分钟] 如: 2330 23点30分 year_day + our + min
	WORD wIsNewGoods;	// 是否新商品

	// 可领取的商店列表
	WORD arCanTakeGoodsList[MAX_DIRECT_BUY_GOODS_NUM];
	WORD wCanTakeGoodsNum;

	void AddCanTakeGoods(WORD wGoodsId)
	{
		// 检测是否已存在
		for (WORD i = 0; i < wCanTakeGoodsNum; i++)
		{
			if (arCanTakeGoodsList[i] == wGoodsId)
				return;
		}

		// 添加
		if (wCanTakeGoodsNum < MAX_DIRECT_BUY_GOODS_NUM)
			arCanTakeGoodsList[wCanTakeGoodsNum++] = wGoodsId;
	}
};

// 兑换商店数据
struct ExchangeShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// 推荐商店数据
struct RecommendShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// 组队商店数据
struct TeamDungeonShopData
{
	// 普通商品购买记录
	ShopGoodsBuyRecord arNormalGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wNormalGoodsBuyRecordNum;

	// 奖励商品购买记录
	ShopGoodsBuyRecord arRewardGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wRewardGoodsBuyRecordNum;
};

// 圣诞节商店数据
struct ChristmasShopData
{
	// 积分兑换商品购买记录
	ShopGoodsBuyRecord arScoreExchangeGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wScoreExchangeGoodsBuyRecordNum;

	// 物品兑换商品购买记录
	ShopGoodsBuyRecord arItemExchangeGoodsBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wItemExchangeGoodsBuyRecordNum;
};


// 水晶商店
struct CrystalShopData
{
	// 今日刷新次数
	WORD wTodayRefreshTimes;

	ShopGoodsBuyRecord arGoodsList[REFRESH_SHOP_GOODS_NUM]; // 商品列表
	WORD wGoodsNum;	// 商品数量
};

// 资源战商店数据
struct ResourceWarShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	WORD wBuyRecordNum;
};

// GM商店
struct GMShopData
{
	ShopGoodsBuyRecord arBuyRecord[MAX_GM_SHOP_GOODS_NUM];
	WORD wBuyRecordNum;
};



// 玩家商店数据
struct PlayerShopData
{
	RefreshShopData tHeroShop;				// 英雄商店数据
	PlayerGuildShopData tGuildShop;			// 工会商品购买记录
	PlayerMallData tMallShop;				// 商城商店购买记录
	PlayerTowerShopData tTowerShop;			// 爬塔商店数据
	PlayerArenaShopData tArenaShop;			// 竞技场商店数据
	RefreshShopData tAwakenShop;			// 觉醒商店数据
	StarSoulShopData tStarSoulShop;			// 星魂商店数据
	ScoreMatchShopData tScoreMatchShop;		// 积分赛商品购买记录
	SevenDayShopData tSevenShop;			// 7天商店
	LevelGiftShopData tLevelGiftShop;		// 等级限时商店	
	EndlessLandShopData tEndlessLandShop;	// 无尽之地商店	
	DirectBuyShopData tDirectBuyShop;		// 直购商店
	ExchangeShopData tExchangeShop;			// 兑换商店
	RecommendShopData tRecommendShop;		// 推荐商店
	TeamDungeonShopData tTeamDungeonShop;	// 组队商店
	ChristmasShopData tChristmasShop;		// 圣诞节商店
	CrystalShopData tCrystalShopData;		// 水晶商店
	ResourceWarShopData tResourceWarShop;	// 资源战商店
	GMShopData tGMShop;						// GM商店(todo::目前不需要记录到数据库, 因为目前购买次数不作限制)
};

// 世界之心
struct HeartOfWorld
{
	WORD wStage;	// 
	WORD wLevel;	// 等级	
};

// 精灵数据
struct SpiritData
{
	DWORD dwId;	// 精灵ID
	WORD wEnhanceLevel;	// 强化等级
	WORD wTrainStage;	// 培养阶(默认是1级)
	WORD wTrainTimes;	// 培养次数

	WORD wStarLevel;	// 星级
};

// 精灵图鉴
struct SpiritPokedex
{
	WORD wPokedexId; // 图鉴ID
	WORD wLevel;	 // 等级
};

// 玩家精灵信息
struct PlayerSpriteInfo
{
	HeartOfWorld tHeartOfWorld;	// 世界之心
	DWORD dwCurUseSpiritId;	// 当前使用的精灵ID
	SpiritData arSpiritList[MAX_SPIRIT_NUM]; // 精灵列表
	WORD wSpiritNum;	// 精灵数量
	SpiritPokedex arPokedexList[MAX_SPIRIT_POKEDEX_NUM]; // 图鉴列表
	WORD wPokedexNum; // 图鉴数量
};


// 客户端角色详细
struct ClientRoleDetails : public PlayerCommonData
{
	BYTE byIsFriend;	// 是否好友
	BYTE byIsBlack;		// 是否黑名单
	ClientFormation arFormationList[MAX_BATTLE_HERO_NUM];
	BYTE byFormationNum;

	ClientRoleDetails()
	{
		memset(this, 0, sizeof(ClientRoleDetails));
	}
};

// 签到数据
struct SignInData
{
	WORD wCurSignInDay;		// 当前签到天数
	DWORD dwLastSignInTime;		// 最近签到时间
};

// 豪华签到数据
struct LuxurySignData
{
	WORD wSignId;	// 签到Id
	WORD wStatus;	// 奖励状态(0=未达到领取条件,1=可以领取,2=已领取)
	WORD wRewardId;	// 奖励Id
};

// 玩家豪华签到数据
struct PlayerLuxurySignData
{
	LuxurySignData arSignDataList[MAX_LUXURY_SIGN_NUM];
	WORD wSignDataNum;
};

// 迎财神数据
struct YCSData
{
	BYTE byTodayGetTimes;	// 今日领取次数
	BYTE byTotalGetTimes;	// 总的领取次数
	int nTotalRewardValue;	// 总的奖励累积值
	DWORD dwLastGetTime;	// 最近一次领取的时间
};

// 铜雀台数据
struct TQTData
{
	DWORD dwLastGetTime;							// 最近一次领取的时间
	WORD arTakeRecordList[MAX_TQT_TAKE_RECORD_NUM];	// 领取记录列表
	WORD wTakeRecordNum;							// 领取记录数量
};

// 登录奖励数据
struct LoginRewardData
{
	WORD wLoginDays;	// 登录天数
	WORD arRewardRecordList[MAX_LOGIN_REWARD_RECORD_NUM];	// 奖励记录列表
	WORD wRewardRecordNum;	// 奖励记录数量	
};

// 玩家争霸赛数据
// struct WarcraftData
// {
// 	BYTE byBattleTimes;		// 挑战次数
// 	BYTE byBuyBattleTimes;	// 购买的挑战次数	
// };

// 引导数据
struct GuideData
{
	int nKey;
	int nValue;
};

// 玩家PVP数据
struct PlayerPVPData
{
	WORD wTodayBattleTimes; // 今日挑战次数
	WORD wWeekBattleTimes;	// 本周挑战次数
	WORD wLastWeekRank;		// 上周排名(0=表示木有排上)

	DWORD dwScore;			// 积分
	WORD wWeekWinTimes;		// 本周胜利次数
	DWORD dwTotalBattleTimes; // 总的挑战次数

	// 每日奖励领取记录
	WORD arDailyRewardTakeRecord[MAX_PVP_BATTLE_REWARD_NUM];
	WORD wDailyRewardTakeRecordNum;

	// 每周奖励领取记录
	WORD arWeeklyRewardTakeRecord[MAX_PVP_BATTLE_REWARD_NUM];
	WORD wWeeklyRewardTakeRecordNum;
};

// 角色基本信息 
struct RoleBaseInfo
{
	DWORD dwRoleId;						// 玩家ID
	char szRoleName[MAX_ROLE_LEN];		// 玩家名称
	char szAccount[MAX_ACCOUNT_LEN];	// 帐号
	DWORD dwStamina;					// 体力
	DWORD dwEnergy;						// 精力 vigor
	WORD wSex;							// 性别
	WORD wLevel;						// 等级
	WORD wDailyInitLevel;				// 每日初始等级
	DWORD dwExp;						// 等级经验
	DWORD dwGold;						// 金币
	DWORD dwDiamond;					// 钻石
	DWORD dwHeadImage;					// 头像Id(英雄信息ID)
	VipData sVipData;					// VIP数据
	WORD wClickLikeTimes;				// 点赞次数
	WORD wRiotSuppressTimes;			// 领地暴动镇压次数
	int nOnceFlagList;					// 一次性标志列表
	int nGuideX;						// 引导X
	int nGuideY;						// 引导Y
	DWORD dwRoleCreateTime;				// 角色创建时间
	DWORD dwUpdateTimes;				// 更新次数
	DWORD dwTotalRechargeAmount;		// 总的充值额度
										
	int nHeroSoul;						// 雄魂(魂晶)
	int nArenaPrestige;					// 竞技场声望(声望)
	int nGodSoul;						// 神魂
	int nWanted;						// 通缉令(征讨令)
	int nTowerPrestige;					// 爬塔声望(威名)								
	DWORD dwFightingPower;				// 战力	
	DWORD dwRebelValue;					// 战功(叛军值)
	DWORD dwCurContribution;			// 当前工会贡献值
	int nHonor;							// 荣誉
	WORD wWorldBossBattleTimes;			// 世界BOSS挑战次数
	WORD wGuildFBRewardChapter;			// 已经领取的军团副本章节奖励
	int nFairySoul;						// 仙魂
	int nScoreMatchHonor;				// 积分赛荣誉
	WORD wFriendshipPoint;				// 友情点
	WORD wTodayFriendshipPoint;			// 今日友情点
	int nEndlessStore;					// 无尽之地值(无尽石)
	int nGemCoin;						// 宝石币
	int nDestinyCoin;					// 命运硬币
	int nStarSoul;						// 星魂
	int nCrystal;						// 水晶
	int nResouceWarScore;				// 资源战积分
	int nYCoin;							// 越南币

	CHAR szLastLoginTime[MAX_TIME_LEN];		// 上一次登录时间
	CHAR szLastLogoutTime[MAX_TIME_LEN];	// 上一次退出时间
	DWORD dwOnlineTime;						// 总的在线时间
	DWORD dwLoginTimes;						// 登录次数
	WORD wLoginDays;						// 登录天数
	DWORD dwLastLoginTime;					// 最近登录时间
	DWORD dwLastLogoutTime;					// 最近下线时间

	DWORD dwLastStaminaRecoverTime;		// 体力上次恢复时间
	DWORD dwLastEnergyRecoverTime;		// 精力上次恢复时间
	DWORD dwLastWantedRecoverTime;		// 通缉令(征讨令)上次恢复时间

// 	BYTE byScoreMatchBattleTimes;		// 积分赛挑战次数
// 	BYTE byScoreMatchRefreshTimes;		// 积分赛刷新次数

	WORD wWarcraftBattleTimes;			// 争霸赛挑战次数
	WORD wBuyWarcraftBattleTimes;		// 购买的争霸赛挑战次数

	WORD wCheerTimes;					// 助威次数
	int nCheerTimesRecoverTime;			// 上次助威次数恢复时间

	DWORD dwDBPeaceOverTime;			// 夺宝免战结束时间(0=表示木有免战) (todo::目前已不使用)

	DWORD dwLeaveGuildTime;				// 离开公会时间
	WORD wLastActivityNoticeId;		// 上次活动公告的ID

	int	arFormationProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX]; // 阵型属性列表

	GuideData arGuideList[MAX_GUIDE_NUM];
	WORD wGuideNum;

	// 激活码领取记录(只记录通用的)
	DWORD arActivationTakeRecord[MAX_ACTIVATION_RECORD_NUM];
	WORD wActivationTakeRecordNum;

	//PlayerPVPData tPVPData;	// 玩家PVP数据

	DWORD dwForbidToTalkOverTime; // 禁言结束时间

	WORD wNobilityLevel;	// 爵位等级

	// 临时数据
	WORD wArenaHistroyRank;	// 竞技场历史排名
	WORD wLandNum;			// 领地数量

	DWORD dwTurntableScore;			// 转盘积分
	WORD wTurntableActivityDataId;	// 转盘活动数据ID

	WORD wTodayUsedHelpBattleTimes;	// 今日已使用助战的次数

	WORD wIsAlreadyTakeClosedTestRebate; // 是否已领取封测返利

	WORD wTodayChatTimes;			// 今日聊天次数

	//DWORD dwResourceWarAccupyTime;		// 资源战总占领资源时间
	//WORD wResourceWarTechnologyLevel;	// 资源战科技等级
};

// 玩家无尽之地数据
struct PlayerEndlessLandData
{
	WORD wIsTodayFirstBattle; // 是否今日首次挑战(todo::目前暂时不使用)
	WORD wTodayBattleTimes;	// 今日挑战次数
	WORD wCurWave; // 当前挑战的波数(正在打的)
	WORD wTodayWaveRecord;	// 今日波数记录(已通关的)
	WORD wCurWaveKillNum; // 当前波击杀怪的数量
	WORD wHistoryWaveRecord; // 历史波数记录(已通关的)
	WORD wCurContinueKillNum;	 // 当前连杀数量
	WORD wContinueKillNumRecord; // 连杀数量记录
	WORD wTodayCollisionRewardTimes; // 碰撞奖励次数(今日已经获得的次数)

	WORD wLastTakeContinueKillReward; // 最近领取的连杀奖励(连杀的数量,0表示还没领取)

	WORD wIsFirstBattle; // 是否首次挑战(1=是,0=否) 新手引导使用

	// 连杀奖励记录
// 	WORD arContinueKillRewardRecord[MAX_ENDLESS_LAND_CONTINUE_KILL_REWARD];
// 	WORD wContinueKillRewardRecordNum;
};

// 玩家工会数据
struct PlayerGuildData
{
	// 领取的章节奖励记录
	WORD arTakeChapterRewardRecord[MAX_GUILD_FB_CHAPTER_NUM];
	WORD wTakeChapterRewardRecordNum;
};

// 精准推荐商品
struct AccurateRecommendGoods
{
	WORD wGoodsId; // 商品ID
	WORD wBuyTimes;	// 购买次数
	WORD wCanTakeTimes;	// 可领取次数
};

// 玩家精准推荐数据
struct PlayerAccurateRecommendData
{
	AccurateRecommendGoods arGoodsList[MAX_ACCURATE_RECOMMEND_GOODS_NUM];
	WORD wGoodsNum;
};

// 世界等级任务事件
struct WorldLevelTaskEvent
{
	WORD wTaskId;			// 任务ID
	int nContext;			// 任务上下文
	WORD wIsAlreadyTake;	// 是否已领取

	// 属于配置字段
	WORD wTaskType;			// 任务类型(为了方便操作)
	WORD wTaskLevel;		// 任务等级(临时数据,不作存储)		
};

// 玩家世界等级任务数据
struct PlayerWorldLevelTaskData
{
	// 是否已领取最终奖励
	WORD wIsAlreadyTakeFinalReward;

	// 任务事件列表
	WorldLevelTaskEvent arTaskEventList[MAX_DAILY_WORLD_LEVEL_TASK_NUM]; 
	WORD wTaskEventNum;
};

// 挂机掉落数据
struct PlayerHangUpDropData
{
	DWORD dwLastObtainRewardTime;	// 最近一次领取奖励时间

	// 掉落奖励物品
	//Item arRewardRecordList[MAX_ONLINE_DROP_REWARD_ITEM_TYPE]; 
	//WORD wRewardRecordNum;
};

// 充值与消耗钻石排行榜数据
struct RechargeAndCostDiamondRankData
{
	WORD wLastActivityDataId;		// 最近一次活动数据ID
	DWORD dwCostDiamond;			// 消耗的钻石
	DWORD dwRechargeAmount;			// 充值额度
};

// 角色二级信息
struct RoleSecondInfo
{
	PlayerPVPData tPVPData;
	PlayerTitleInfo	tTitleInfo;	// 称号信息	ok
	PlayerEndlessLandData tEndlessLandData; // 无尽之地数据
	PlayerGuildData tGuildData; // 工会数据(挂在人身上的数据)
	PlayerLuxurySignData tLuxurySignData; // 豪华签到数据
	PlayerGemInfo tGemInfo;		// 宝石信息
	LoginRewardData tLoginRewardData; // 登录奖励数据
	PlayerKingMatchData tKingMatchData; // 王者赛数据
	PlayerTeamDungeonData tTeamDungeonData; // 组队副本数据
	PlayerWorldLevelTaskData tRoleWorldLevelTaskData; // 角色世界等级任务数据
	PlayerHangUpDropData tHangUpDropData;	// 挂机掉落数据
};

// 夺宝挑战目标玩家数据
struct DuobaoTargetPlayerData
{
	DWORD dwRoleId;									// 目标角色ID
	WORD wLevel;									// 角色等级
	DWORD dwHeadImage;								// 头像
	DWORD dwFightingPower;							// 战力	
	char szRoleName[MAX_ROLE_LEN];					// 目标角色名称
	BYTE byToBattleHeroNum;							// 上阵英雄数量
	WORD arToBattleHeroList[MAX_BATTLE_HERO_NUM];	// 上阵英雄列表
};

// 夺宝挑战目标怪物数据
struct DuobaoTargetMonsterData
{
	WORD wMapId;	// 目标怪物所在的关卡ID
};

// 夺宝次数
struct DoubaoTimes
{
	WORD wQuality;	// 品质
	WORD wTimes;	// 次数	 
};

// 玩家夺宝数据
struct PlayerDuobaoInfo
{
	DWORD dwPeaceOverTime;	// 免战结束时间(0=表示木有免战)
	WORD arDoubaoTimes[QUALITY_NUM];

	// 每日夺宝次数
	WORD wDailyDoubaoTimes;

	// 夺宝进度奖励记录
	WORD arPregrossRewardRecord[MAX_DOUBAO_PREGROSS_NUM];
	WORD wPregrossRewardRecordNum;
};

// 玩家工会技能数据
struct PlayerGuildSkillData
{
	SkillData arSkillList[MAX_GUILD_SKILL_NUM];
	WORD wSkillNum;
};

// 奖励数据
struct RewardData
{
	DWORD dwUId;				// 奖励UId
	WORD wInfoId;				// 奖励配置Id
	DWORD dwValue1;				// 奖励值1
	DWORD dwValue2;				// 奖励值2
	DWORD dwTime;				// 发放时间
	char szText[BUFF_LEN_64];	// 文本

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

// 玩家奖励数据
struct PlayerRewardData
{
	RewardData arRewardList[MAX_REWARD_CENTER_REWARD_NUM];
	WORD wRewardNum;
};

// VIP礼包数据
struct VipGiftData
{
	WORD wInitVipLevel;	// 每日初始VIP等级
	short nVipLevelRewardRecord; // VIP等级奖励记录(已经领取的,-1=表示还没有领取)

	// 每周礼包列表
	Gift arWeekGiftList[MAX_VIP_WEEK_GIFT_NUM];
	WORD wWeekGiftNum;

	WORD wTodayRecommendRechargeRewardId;	// 今日推荐充值奖励Id
	WORD wTodayRecommendRechargeRewardStatus; // 今日推荐充值奖励状态(0=不能领取,1=可领取,2=已领取)
};

// 等级商店信息
// struct LevelShopInfo
// {
// 	WORD wLevel;			// 等级
// 	DWORD dwActivateTime;	// 激活时间
// };

// 等级商店活动数据
struct LevelShopActivityData
{
	WORD wLevel;			// 等级
	DWORD dwActivateTime;	// 激活时间
};


// 开服基金数据
struct OpenServerFundData
{
	WORD wIsAlreadyBuyFund;	// 是否已购买基金
	WORD arFundRewardTakeRecord[MAX_OPEN_SERVER_FUND_REWARD];	// 基金奖励领取记录	
	WORD wFundRewardTakeRecordNum;
};

// 全民福利数据
 struct AllPeopleWelfareData
 {
 	// 领取福利记录
	 WORD arTakeWelfareRecordList[MAX_ALL_THE_PEOPLE_WELFARE_NUM];
	 WORD wTakeWelfareRecordNum;
 };

 // 限时优惠数据
 struct LimitPreferentialData
 {
	 WORD wBuySchedule;				// 购买进度
	 WORD wRechargeId;				// 有优惠的充值Id(0=表示木有)
	 DWORD dwStartPreferentialTime;	// 开始优惠时间

	 WORD arTakeWelfareRecordList[MAX_LIMIT_PREFERENTIAL_WELFARE_NUM];	// 领取福利记录列表
	 WORD wTakeWelfareRecordNum; // 领取福利记录数量	
 };

 // 超值折扣数据
 struct SuperDiscountData
 {
	 WORD wStatus;						// 购买状态(0=还没购买, 1=已购买(可领取), 2=已领取)
	 WORD wTakeAccumulateRewardTimes;	// 已领取累积奖励的次数(新超值折扣使用)
	 WORD wCurBuyTimes;					// 当前购买次数(领取奖励时会重置)
	 WORD wLastActivityDataId;			// 最近一次活动数据Id

	 // 已领取累积奖励记录(旧超值折扣使用)
	 WORD arTookAccumulateRewardList[MAX_SUPER_DISCOUNT_ACCUMULATE_REWARD];
	 WORD wTookAccumulateRewardNum;
 };



 // 热卖商品数据
 struct HotSaleGoods
 {
	 WORD wGoodsId;		// 商品ID
	 WORD wCurDataIdx;	// 当前数据索引
	 WORD wBuyTimes;	// 购买次数
	 WORD wTakeTimes;	// 领取次数
 };

 // 玩家热卖商品信息
 struct PlayerHotSaleGoodsInfo
 {
	 HotSaleGoods arGoodsList[MAX_HOT_SALE_GOODS_NUM];
	 WORD wGoodsNum;
	 WORD wLastActivityDataId;
 };


 // 玩家圣诞节数据
 struct PlayerChristmasData
 {
	 // 总积分
	 DWORD dwTotalScore;

	 // 当前积分
	 DWORD dwCurScore;

	 // 已领取的低级BOSS奖励等级
	 WORD wAlreadyTakeLowerBossRewardLevel;

	 // 已领取的高级BOSS奖励等级
	 WORD wAlreadyTakeHighBossRewardLevel;

	 // 最后重置数据时间戳
	 DWORD dwLastResetDataTime;
 };

 // facebook好友信息
 struct FacebookFriendInfo
 {
	 WORD wFriendNum;
	 char arFriendList[MAX_FACEBOOK_FRIEND_NUM][MAX_FACEBOOK_ACCOUNT_LEN];
 };


 // 召回好友数据
 struct RecallFriendData
 {
	 DWORD dwTime; // 召回时间
	 DWORD dwFriendId; // 好友ID
 };

 // Facebook活动数据
 struct FacebookActivityData
 {
	 WORD wIsAlreadyTakeAccountBindingReward; // 是否已领取帐号绑定奖励

	 DWORD dwReadyToShareTime;		// 准备分享时间(0=表示木有,1=表示已分享过)

	 DWORD dwReadyToCommendTime;	// 准备评论时间(0=表示木有,1=表示已评论过, 大于1表示准备评论时间)

	 // 召回成功的好友数量
	 WORD wSummonSuccessFriendNum;

	 // 邀请成功的好友数量
	 WORD wInviteSuccessFriendNum;

	 // 召回奖励记录(已领取的)
	 WORD arSummonRewardRecord[MAX_FACEBOOK_SUMMON_REWARD_RECORD];
	 WORD wSummonRewardRecordNum;

	 // 邀请奖励记录(已领取的)
	 WORD arInviteRewardRecord[MAX_FACEBOOK_INVITE_REWARD_RECORD];
	 WORD wInviteRewardRecordNum;

	 // 已发出召回的好友记录(游戏内好友)
	 RecallFriendData arSummonFriendRecord[MAX_FRIEND_NUM];
	 WORD wSummonFriendRecordNum;

	 // 已发出邀请的好友记录(facebook好友)
	 FacebookFriendInfo tSendInviteFriendInfo;
 };

 // unlock活动数据
 struct UnlockActivityData
 {
	 WORD wIsScoreForShop;			// 是否已对商城评分
	 WORD wWatchingVideosTimes;		// 观看视频次数
 };

 // 在线奖励数据
 struct OnlineRewardData
 {
	 DWORD dwOnlineTime;// 在线时间(秒)
	 DWORD dwStartTime;	// 开始时间(本次登录在线时间的开始记录时间点)(不需要记录数据库)
	 WORD arRewardRewardList[MAX_ONLINE_REWARD_DAY][MAX_ONLINE_REWARD_TIME_LIST]; // 奖励记录列表[days][flag] (0=表示还没领取,1=表示已领取)
 };

 // 7天登录数据
 struct SevenDayLoginData
 {
	 WORD arDailyDataList[MAX_SEVEN_DAY_LOGIN_DAYS];	// 每日数据列表[1=表示已登录,2=表示已领取,index=天数]
 };

 // 充值奖励数据
 struct RecahrgeRewardData
 {
	 WORD wLastActivityDataId;			// 活动数据ID	
	 WORD wIsTookExtraReward; // 是否已领取额外奖励
	 WORD arDailyDataList[MAX_RECHARGE_REWARD_DAY_NUM]; // 每日数据列表(0=还没充值,1=已充值,2=已领取) 
 };

 // 累积奖励数据
 struct TotalRechargeRewardData
 {
	 WORD wLastActivityDataId;	// 活动数据ID
	 DWORD  dwTotalAmount;	// 总的充值额度
	 WORD arRewardRecordList[MAX_TOTAL_RECHARGE_REWARD_NUM]; // 奖励记录列表
	 WORD wRewardRecordNum;
 };

 // 玩家阵营招募数据
 struct PlayerCampRecruitData
 {
	 WORD wCurDataId;					// 当前活动数据ID
	 WORD wTodayAlreadyRecruitTimes;	// 今日已招募次数
	 DWORD arCampScoreList[CAMP_NUM];	// 阵营积分列表

	 // 积分任务奖励记录列表(已领取的记录)
	 WORD arScoreTaskRewardRecordList[MAX_CAMP_RECRUIT_SCORE_TASK_NUM];
	 WORD wScoreTaskRewardRecordNum;
 };

// 玩家活动数据
struct PlayerActivityData
{
	// 迎财神数据
	YCSData	tYCSData;				

	// 铜雀台数据
	TQTData	tTQTData;
									
	// 7天活动数据
	SevenDayTargetData arSevenDayTargetList[MAX_SEVEN_DAY_TARGET_NUM];
	WORD wSevenDayTargetNum;
	// 7天完成度奖励记录
	WORD arCompletionRewardRecord[MAX_SEVEN_DAY_COMPLETION_REWARD];
	WORD wCompletionRewardRecordNum;

	// VIP礼包数据
	VipGiftData tVipGiftData;	

	// 等级商店活动数据
	LevelShopActivityData arLevelShopActivityDataList[MAX_LEVEL_SHOP_ACTIVITY_NUM];
	WORD wLevelShopActivityDataNum;

	// 开服基金
	OpenServerFundData tOpenServerFundData;

	// 全民福利
	AllPeopleWelfareData tAllPoepleWelfareData;

	// 限时优惠数据
	LimitPreferentialData tLimitPreferentialData;

	// 等级礼包领取记录
	WORD arLevelGiftTakeRecord[MAX_LEVEL_GIFT_TAKE_RECORD_NUM];
	WORD wLevelGiftTakeRecordNum;

	// 充值数据
	RoleRechargeData tRechargeData;

	// 限时活动
	LimitActivityData tLimitActivityData;

	// 是否领取了次日奖励
	WORD wIsTakeNextDayReward;

	// 超值折扣数据
	SuperDiscountData tSuperDiscountData;

	// 节日活动
	PlayerFestivalActivityInfo tFestivalActivityInfo;

	// 角色精准推荐数据
	PlayerAccurateRecommendData tAccurateRecommendData; 

	// 角色圣诞节数据
	PlayerChristmasData tChristmasData;

	// facebook活动数据
	FacebookActivityData tFacebookActivityData;

	// 在线奖励数据
	OnlineRewardData tOnlineRewardData;

	// 7天登录数据
	SevenDayLoginData tSevenDayLoginData;

	// 充值奖励数据
	RecahrgeRewardData tRechargeRewardData;

	// 累积奖励数据
	TotalRechargeRewardData tTotalRechargeRewardData;

	// 新超值折扣数据
	SuperDiscountData tNewSuperDiscountData;

	// 热卖商品数据
	PlayerHotSaleGoodsInfo tPlayerHotSaleGoodsInfo;

	// 玩家阵营招募数据
	PlayerCampRecruitData tPlayerCampRecruitData;

	// unlock活动数据
	UnlockActivityData tUnlockActivityData;

	// 充值与消费排行榜数据
	RechargeAndCostDiamondRankData tRechargeAndCostDiamondRankData;
};

// 角色临时数据
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

//角色完整信息
struct RoleFullInfo
{
	RoleBaseInfo			tRoleBaseInfo;			// 角色基本信息		max_role_base_info_size ok
	PlayerEquipmentInfo		tEquipmentInfo;			// 装备信息			max_role_equipment_info_size ok
	PlayerHeroInfo			tHeroInfo;				// 英雄信息			max_role_hero_info_size ok
	PlayerTreasureInfo		tTreasureInfo;			// 宝物信息			max_role_treasure_info_size	ok		
	PlayerItemInfo			tItemInfo;				// 物品信息			max_role_item_info_size ok
	PlayerEmailInfo			tEmailInfo;				// 邮件信息			max_role_email_info_size ok	
	PlayerFubenInfo			tFubenInfo;				// 副本信息			max_role_fb_info_size ok
	PlayerShopData			tShopInfo;				// 商店信息			max_role_shop_info_size ok
	PlayerSpriteInfo		tSpiritInfo;			// 精灵信息			max_role_spirit_info_size ok
	PlayerFriendData		tFriendInfo;			// 好友信息			max_role_friend_info_size ok
	PlayerRebelData			tRebelInfo;				// 叛军信息			max_role_rebel_info_size ok
	PlayerAchievementData	tAchievementInfo;		// 成就信息			max_role_achievement_info_size ok
	PlayerDailyMissionData  tDailyMissionInfo;		// 日常任务信息		max_role_daily_mission_info_size ok
	PlayerGuildSkillData	tGuildSkillData;		// 个人工会技能数据	max_role_guild_skill_data_size ok
	SignInData				tSignInData;			// 签到数据			max_role_signin_data_size ok
	PlayerActivityData		tActivityInfo;			// 活动数据 ok			
	PlayerZhanBuInfo		tZhanBuInfo;			// 占卜信息			max_role_fairy_info_size (木)
	// PlayerTitleInfo			tTitleInfo;				// 称号信息	ok
	PlayerRewardData		tRewardInfo;			// 奖励信息	ok
	//RechargeData			tRechargeData;			// 充值数据 ok
	PlayerDuobaoInfo        tDuobaoInfo;			// 夺宝信息
	RoleTempDatas			tTempDatas;				// 临时数据

	RoleSecondInfo			tSecondInfo;				// 角色二级信息
};

union BroadcastInfo
{
	struct OpenCardPacket
	{
		char	szPlayerName[MAX_ROLE_LEN];		//角色名
		DWORD   dwCardID;
	} m_CardInfo;

	struct Financial_Plan
	{
		char	szPlayerName[MAX_ROLE_LEN];		//角色名
		BYTE   byBuyType;
	} m_Financial_Plan;

};


// 服务器全局数据
struct ServerGolbalData
{
// 	WORD wWorldBossLevel;			// 世界BOSS等级
// 	DWORD dwOpenServerFundBuyTimes;	// 开服基金购买次数
	WORD wServerId;
	int nKey;
	DWORD dwValue;
};

// 概率数
struct RateData
{
	DWORD dwMin;
	DWORD dwMax;
	int nRate;
};

// 概率数
struct RateDataEx
{
	int nValue;
	int nRate;
};

// 开始战斗数据
struct BeginBattleData
{
	BYTE byType;			// 战斗类型
	WORD wChapterLevel;		// 章节
	WORD MapLevel;			// 关卡等级
};

// 装备信息(客户端)
struct EquipmentClientData
{
	DWORD dwUID;
	WORD wInfoId;			// 配置ID
	WORD wEnhanceLevel;	// 强化等级
	BYTE byRefineLevel;		// 精炼等级
	DWORD dwRefineExp;		// 精练经验
	WORD wWearHeroId;		// 穿戴的英雄ID 
	int nEnhanceCostGold;	// 强化消耗的金币
	BYTE byStar;				// 星数
	BYTE byStarLevel;		// 星级
};

// 最近登录的服务器信息
struct LastLoginServerData
{
	DWORD dwServerId;	// 服务器ID
	DWORD dwHeadIcon;	// 头像图标
	WORD wLevel;		// 角色等级
	char szRoleName[MAX_ROLE_LEN + 1]; // 角色名称
};

// 留言数据
struct LeaveMessageData
{
	DWORD dwRoleId;
	char szMessage[MAX_RANK_LEAVE_MESSAGE_LEN];
};

// 副本扫荡掉落数据
struct SweepDropData
{
	BYTE byTimes; // 第几次数扫荡
	vector<Item> vDropItemList;	// 掉落物品列表
};

// 客户端奖励数据
struct ClientRewardData : public RewardData
{
	vector<Item> vItemList;				// 物品奖励列表(todo::目前只是发送消息时使用)
	vector<MoneyData> vCurrencyList;	// 货币奖励列表(todo::目前只是发送消息时使用)
};

// 玩家基本数据
struct PlayerBaseData : public PlayerCommonData
{
	vector<ToBattleHeroData> vToBattleHeroList; // 上阵英雄列表
	vector<SpiritData> vSpriteList;				// 精灵列表

	set<DWORD> setFriendGiveEnergyList;		// 赠送精力玩家列表
	set<DWORD> setFriendApplyList;			// 申请好友列表
	BYTE byFriendNum;						// 好友数量
	vector<RewardData> vRewardList;			// 奖励列表
	int nTodayHelpBattleFriendShipPoint;	// 今日助战获得的友情点
	DWORD dwLastTakeFriendShipPointTime;	// 最近一次获得友情点的时间

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


// 客户端角色排名数据
struct ClientRoleRankData
{
	DWORD dwRoleId;							// ID
	DWORD dwServerId;						// 服务器ID
	char szRoleName[MAX_ROLE_LEN];			// 名称
	WORD wHeadIcon;							// 头像
	WORD wLevel;							// 等级
	WORD wRank;								// 排名
	DWORD dwFightingPower;					// 战力
	DWORD dwValue;							// 排名的值
	DWORD dwParam1;							// 参数1
	DWORD dwParam2;							// 参数2
	DWORD dwUpdateTime;						// 更新时间
	//DWORD dwLikeTimes;						// 点赞次数
	char szGuildName[MAX_GUILD_NAME_LEN];	// 工会名称
	BYTE byVipLevel;							// VIP等级
	DWORD dwTitleId;						// 称号ID	

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

// 客户端工会排名数据
struct ClientGuildRankData
{
	DWORD dwGuildId;						// ID
	char szGuildName[MAX_GUILD_NAME_LEN];	// 名称
	char szCaptainName[MAX_ROLE_LEN];		// 会长名称
	WORD wIconId;							// 工会图标ID
	WORD wLevel;							// 等级
	BYTE byMemberNum;						// 成员数量
	DWORD dwValue;							// 排名值

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

// GM事件数据
struct GMEventData
{
	DWORD dwIdx;	// 数据索引
	WORD wEventId;	// 事件ID
	DWORD arParamList[10];
	char szText[MAX_SYSTEM_NOTICE_LEN + 1];
	char szText1[MAX_SYSTEM_NOTICE_LEN + 1];
};

// 时间事件数据
struct TimeEventData
{
	WORD wEventId;				// 事件ID
	BYTE byTimeType;			// 时间类型 1.间隔时间 2.每日 3.周日 4.月日 5.年日
	int nCD;					// CD时间
	int nIntervalTime;			// 间隔时间(单位秒)
	int nStarValidTime;			// 开始有效时间[月:日:小时:分钟]
	int nTriggerDate;			// 触发日期
	int nTriggerTime;			// 触发时间
	DWORD dwLastTriggerTime;	// 上次触发时间

	int nBeginTime;				// 开始时间
	int nEndTime;				// 结束时间
};

// 争霸赛排行数据
struct WarcraftRankData
{
	DWORD dwRoleId;			// 角色ID
	DWORD dwServerId;		// 服务器ID
	WORD wRank;				// 排行
	DWORD dwFightingPower;	// 战力
	WORD wBeCheerTimes;		// 被助威次数
};

// 争霸赛目标数据
struct WarcraftTargetData
{
	DWORD dwRoleId;						// 目标Id
	WORD wHeadIcon;						// 头像图标
	char szRoleName[MAX_ROLE_LEN];		// 目标名称
	WORD wRank;							// 排名
	DWORD dwFightingPower;				// 战力
};

// 客户端争霸赛排行数据
struct ClientWarcraftRankData : public WarcraftRankData
{
	DWORD dwHeadImageId;
	char szRoleName[MAX_ROLE_LEN + 1];
};

// 红点数据
struct RedPointData
{
	WORD wModuleId;	// 模块ID
	BYTE byState;	// 状态(1=有,0=无)
};

// 系统公告参数
struct SystemNoticeParam
{
	char szText[MAX_SYSTEM_NOTICE_PARAM_LEN+1];
};

// 服务器全局数据
struct ServerGlobalData
{
	DWORD dwOpenServerFundBuyTimes;			// 开服基金购买次数
	WORD wChapterPassRecord;				// 章节通关记录
	DWORD dwLimitActivityGoodsBuyTimes;		// 限时活动商品购买次数
	DWORD dwFightingPowerRecordIdx;			// 战力记录索引
	WORD wAttackCityChapter;				// 攻城怪章节(准备要去创建的)
	DWORD dwAttackCityRankNO1RoleId;		// 攻城排名第一的玩家ID(0=表示木有)
	WORD wChristmasLowerBossLevel;			// 圣诞节低级boss等级
	WORD wChristmasHighBossLevel;			// 圣诞节高级boss等级
	DWORD dwChristmasLastResetTime;			// 圣诞节数据重置时间
};

// 手机号认证数据
struct MobileAuthData
{
	string strMobile;	// 手机号
	DWORD dwTime;		// 认证时间戳
	WORD wCode;			// 认证码
};

// 助战目标数据
struct HelpBattleTargetData
{
	DWORD dwRoleId;					// 角色Id
	char szRoleName[MAX_ROLE_LEN];	// 角色名称
	WORD wLevel;					// 等级
	BYTE byVipLevel;				// VIP等级
	DWORD dwFightingPower;			// 战力
	DWORD dwHeroInfoId;				// 助战英雄信息ID
	BYTE byTargetType;				// 目标类型(0=陌生人, 1=好友, 2=军团成员)
	BYTE byHelpBattleTimes;			// 助战次数
	DWORD dwTitleId;				// 称号ID
};

// 关卡评论
struct MapComment
{
	DWORD dwRoleId;		// 评论者ID
	WORD wCommentId;	// 评论ID
	char szText[BUFF_LEN_128];	// 评论内容
	DWORD dwTime;		// 评论时间
	WORD wLikeTimes;	// 点赞次数
	//set<DWORD> vLikePlayerList; // 点赞玩家列表
	DWORD arLikePlayerList[MAX_MAP_COMMENT_LIKE_NUM];
	WORD wLikePlayerNum;
};

// 关卡评论
struct MapComments
{
// 	DWORD dwServerId;
// 	DWORD dwMapId;
	DWORD dwId; // (server_id + map_id)
	MapComment arCommentList[MAX_MAP_COMMENT_NUM];
	WORD wCommentNum;
};

// 关卡评论
struct MapCommentClient : public MapComment
{
	char szRoleName[MAX_ROLE_LEN];	// 评论者名称
	BYTE byVipLevel;		// VIP等级		
};

// PVP等待挑战的角色数据
struct PVPWaitBattleRoleData
{
	DWORD dwRoleId;
	DWORD dwFightingPower;
	//DWORD dwToBattleSpiritId; // 出战精灵
	DWORD dwSpiritSkillId; // 精灵技能ID
	DWORD dwTime;			// 请求时间
};

// PVP英雄数据
struct PVPHeroData : public ToBattleHeroData
{
	//DWORD dwHeroInfoId;
	DWORD dwTotalHP;
	INT nCurHP;			// 当前HP

	BlessSpriteData tBlessSpriteData; // 祝福精灵数据

	//int arProList[HERO_PRO_MAX];	// 属性列表
};

// PVP角色战斗数据 
struct PVPRoleBattleData
{
	DWORD dwRoleId;
	char szRoleName[MAX_ROLE_LEN];
	WORD wHeadImageId;
	DWORD dwServerId;
	bool bIsReady;	// 是否已准备好
	BYTE byType;	// 1=红方,2=蓝方
	DWORD dwFightingPower;
	DWORD dwToBattleSpiritId; // 出战精灵ID
	DWORD dwSpiritSkillId;	// 精灵技能
	WORD wSpriteEnhanceLevel; // 精灵强化等级
	WORD wSpriteTrainStage;	// 精灵培养阶
	WORD wSpriteTrainTimes;	// 精灵培养次数
	WORD wSpriteStar;		// 精灵星数
	PVPHeroData arHeroList[MAX_BATTLE_HERO_NUM]; // 英雄列表
	WORD wHeroNum;
	SpiritData arSpriteList[MAX_SPIRIT_NUM];
	WORD wSpriteNum;
};

// PVP战场数据
struct PVPBattlefieldData
{
	DWORD dwBattleId; // 战斗ID
	bool bIsInBattle;	// 是否在战斗中
	bool bIsRobot;		// 是否挑战机器人
	DWORD dwStartTime;	// 开始时间(开始准备时间,开始战斗时间)
	PVPRoleBattleData arRoleBattleDataList[PVP_BATTLEFIELD_ROLE_NUM];  // 角色战斗数据列表
};

// PVP战斗结算数据
struct PVPBattleOverData
{
	WORD wOldRank;	// 旧排名(0=未上榜)
	WORD wNewRank;	// 新排名(0=未上榜)
	WORD wLastWeekRank; // 上周排名(0=未上榜)
	WORD wScore;	// 本次获得积分
};

typedef SceneItem TreasureBox;

// 宝箱
// struct TreasureBox
// {
// 	DWORD dwUID;
// 	DWORD dwInfoId;
// 	DWORD dwCreateTime;	// 创建时间
// 	DWORD dwLockPlayerId; // 锁住宝箱的玩家Id
// 	DWORD dwLockTime; // 锁住时间
// 	MapPos tMapPos;	// 地图坐标
// };

// 攻城关卡
struct AttackCityMap
{
	DWORD dwUID;
	DWORD dwMapId;
	MapPos tMapPos;	// 怪物坐标
	bool bIsDead;	// 是否已死亡
	//bool bIsInBattle;	// 是否正在挑战中
	DWORD dwStartBattleTime; // 开始战斗时间
	WORD wChapter;		// 所属章节
	vector<MonsterData> vMonsterList;	// 怪物列表
};


// 攻城章节数据
struct AttackCityChapter
{
	WORD wChapter; // 章节		
	vector<AttackCityMap> vMapList;	// 关卡列表
};

// 攻城的怪物
struct AttackCityMonster
{
	DWORD dwUID;
	DWORD dwMapId;		// 关卡ID
	DWORD dwDamage;		// 当前伤害
	DWORD dwTotalHP;	// 总的HP
	//BYTE byIsInBattle;	// 是否在战斗中
	DWORD dwStartBattleTime; // 开始战斗时间
	MapPos tMapPos;		// 怪物坐标

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

// 玩家攻城数据
struct PlayerAttackCityData
{
	DWORD dwBattleOverTime;	// 战斗结束时间
	WORD wReliveCD;			// 复活CD(秒)
	WORD wWeakCD;			// 虚弱CD(秒)
	BYTE byEncourageTimes;	// 鼓舞次数
	DWORD dwScore;			// 积分
	DWORD dwBattleTargetId;	// 挑战的目标Id
	vector<WORD> vTakeBossRewardRecordList; // 领取boss奖励记录列表
};


// 时间数据
struct TimeData
{
	int nStart;	// 开始[小时分钟] 如: 2230 22点30分
	int nOver;	// 结束[小时分钟]
};

// 角色等级记录
struct RoleLevelRecord
{
	DWORD dwRoleId;			// 角色ID
	WORD wLevel;			// 角色等级
	WORD wVipLevel;			// VIP等级
	DWORD dwDiamond;		// 钻石
	DWORD dwGold;			// 黄金
	WORD wServerId;			// 服务器ID
	WORD wMainFBChapter;	// 主线章节
	WORD wMainFBMap;		// 主线关卡
	int nGuideX;			// 引导X
	int nGuideY;			// 引导Y
	DWORD dwFightingPower;	// 战力
	DWORD dwRecordTime;		// 记录时间
	WORD wHeroFBChapter;	// 英雄副本章节
	WORD wEliteFBChapter;	// 精英雄副本章节
	WORD wTowerMapLevel;	// 爬塔层数
	WORD wLandNum;			// 领地占领数量
	WORD wWorldHeartStage;	// 世界之心阶段
	WORD wArenaHistoryRank;	// 竞技场历史排名
	WORD wItemNum;			// 物品数量
	WORD wEquipmentNum;		// 装备数量
	WORD wHeroNum;			// 英雄数量
	WORD wTreasureNum;		// 宝物数量
};

// 角色下线记录 
struct RoleOfflineRecord
{
	DWORD dwRoleId;			// 角色ID
	WORD wLevel;			// 角色等级
	WORD wVipLevel;			// VIP等级
	DWORD dwDiamond;		// 钻石
	DWORD dwGold;			// 黄金
	WORD wServerId;			// 服务器ID
	WORD wMainFBChapter;	// 主线章节
	WORD wMainFBMap;		// 主线关卡
	int nGuideX;			// 引导X
	int nGuideY;			// 引导Y
	DWORD dwOfflineTime;	// 下线时间
	DWORD dwOnlineTime;		// 在线时间(秒)
};

// 充值记录
struct RechargeRecord
{
	DWORD dwRoleId;
	WORD wRoleLevel;	// 角色等级
	WORD wServerId;		// 服务器ID
	WORD wRechargeId;	// 充值ID
	WORD wRechargeType;	// 充值类型
	//WORD wCardId;		// 月卡ID
	WORD wFrom;			// 充值来源
	DWORD dwAmount;		// 充值额度
	DWORD dwDiamond;	// 充值获得的钻石
	DWORD dwRecordTime;	// 充值时间戳
	char szPayTime[32];	// 支付时间
	char szSDKOrderId[MAX_SDK_ORDER_ID_LEN]; // 渠道订单号
};

// 关卡失败记录
struct MapLoseRecord
{
	DWORD dwRoleId; // 角色ID
	DWORD dwMapId;	// 关卡ID
	WORD wType;		// 关卡类型
	WORD wChapter;	// 章节
	WORD wMapIdx;	// 关卡索引
	DWORD dwFightingPower; // 战力
};

// 首次通关记录
struct FirstPassMapRecord
{
	DWORD dwRoleId; // 角色ID
	WORD wLevel;	// 角色等级
	DWORD dwMapId;	// 关卡ID
	WORD wType;		// 关卡类型
	WORD wChapter;	// 章节
	WORD wMapIdx;	// 关卡索引
	DWORD dwFightingPower; // 战力
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
// 充值回调数据
struct RechargeCallbackData
{
	BYTE byIsTest;					// 必有 是否为测试订单 1为测试 0为线上正式订单，游戏应根据情况确定上线后是否向测试订单发放道具。
	DWORD dwChannel;				// 必有 渠道标示ID 注意 : 游戏可根据实情, 确定发货时是否校验充值来源渠道是否与该角色注册渠道相符
	//DWORD dwChannelUid;			// 必有 渠道用户唯一标示, 该值从客户端GetUserId()中可获取
	DWORD dwCPOrderId;				// 必有 游戏在调用QucikSDK发起支付时传递的游戏方订单, 这里会原样传回
	char szSDKOrderId[BUFF_LEN_64];	// 必有 QuickSDK唯一订单号
	char szPayTime[BUFF_LEN_64];	// 必有 支付时间 2015 - 01 - 01   23:00 : 00
	char szChannelUid[BUFF_LEN_64];	// 必有 渠道用户唯一标示, 该值从客户端GetUserId()中可获取
	int nAmount;					// 必有 成交金额，单位元，游戏最终发放道具金额应以此为准
	BYTE  byStatus;					// 必有 充值状态 0   成功 1失败(为1时 应返回FAILED失败)
	DWORD dwRoleId;					// 可为空 游戏客户端调用SDK发起支付时填写的透传参数.没有则为空 (role_id)
	char szFailedDesc[BUFF_LEN_128];// 可为空 失败描述
	WORD wRechargeId;				// 充值ID
	WORD wRechargeType;				// 充值类型
	WORD wGoodsId;					// 商品ID
	char szIP[BUFF_LEN_32];			// IP地址
	DWORD dwYDChannelId;			// 云顶渠道ID
	WORD wCurrencyType;				// 货币类型(0=人民币, 非0其他货币(目前只有美元))
	BYTE byIsMyCard;				// 是否mycard充值
	BYTE byIsCheckRechargeStrId;	// 是否检测充值字串Id
	char szRechargeStrId[BUFF_LEN_64];	// 充值字串Id
	BYTE byIsNotCheck;				// 是否免检检测(1=是,0=否)
	WORD wFrom;
};


// 充值成功上报数据
struct RechargeSuccessReportData
{
	DWORD dwChannel;					// 	channel  是  int  渠道代码(要求客户端用YundingKeyData.getChannel()获取最终的渠道并传给服务端)
	DWORD dwServerId;					// 	server_id  是  string  区服 ID
	char szAccountId[MAX_ACCOUNT_LEN];	// 	account_id  是  string  账号 ID
	DWORD dwRoleId;						// 	role_id  是  string  角色 ID
	char szRoleName[MAX_ROLE_LEN];		// 	role_name  是  string  角色名称
	WORD wRoleLevel;					// 	role_level  是  int  角色等级[如无法获取数据默认传 0]
	WORD wVipLevel;						// 	vip_level  是  int  VIP 等级[如无法获取数据默认传 0]
	DWORD dwVipExp;						// 	vip_exp  是  int  VIP 经验[如无法获取数据默认传 0]
	char szOrderId[64];					// 	order_id  是  string  订单号（支付平台提供的订单号）	
	DWORD dwIapId;						// 	ap_id  是  string  商品编码
	char szIapDes[32];					// 	iap_des  是  string  商品名称
	DWORD dwCurrencyAmount;				// 	currency_amount  是  double  现金金额（精确到元） 深V靓仔
	DWORD dwVirtualCurrencyAmount;		// 	virtual_currency_amount 是  double  充值获得代币数量
	DWORD dwVirtualCurrencyAmountEx;	// 	virtual_currency_amount_ex 是  double  额外获得代币数量
	char szIP[32];						// 	ip  是  string  玩家充值时的 IP 地址

	// 	sign  是  string  md5_32 位小写(app_key + timestamp)timestamp  是  int  上报时间的时间戳，精确到秒
	// 	app_id  是  string  产品 ID
	// 	currency_type  是  int  货币类型【币种】 1 - RMB
	// 	platform_id  是  int  支付渠道 ID 支付宝：2
	// 	access_type  是  int  获取方式 1 - 充值 2 - 赠与
};

// MyCard充值成功上报数据
struct MyCardRechargeSuccessReportData
{
	DWORD dwVid;					//	是	int	万家用户唯一id，登陆后才有
	char szIP[IP_LEN];				//	是	string	用户的IP信息
	char szIMSI[BUFF_LEN_64];		//  否	string	imsi
	char szIMEI[BUFF_LEN_64];		//  否	string	手机串号
	char szVIMEI[BUFF_LEN_64];		//	否	string	vimei
	DWORD dwRoleId;					//  否	string	角色id
	char szRoleName[MAX_ROLE_LEN];	//	否	string	角色名
	DWORD dwServerId;				//	否	string	服务器id
	char szServerName[BUFF_LEN];	//  否	string	服务器名
	char szOrderTime[BUFF_LEN_32];	//  是	string	订单时间，格式：2018 - 05 - 30 10:05 : 23
	char szOrderNo[BUFF_LEN_64];	//  是  order_no	是	string	订单号
	int nPay;						//	是	int	订单成功金额，单位：美分

	// 2.sign签名算法
	// 将上行的所有字段按照字段名的ASCII 码从小到大排序（字典序）后，使用URL键值对的格式（即key1 = value1&key2 = value2…）
	// 拼接成字符串string1。这里需要注意的是所有参数名均为小写字符，
	// 不包括sign字段。对string1作sha1加密，字段名和字段值都采用原始值，不进行URL 转义。
};



// 宝石镶嵌
struct GemInlay
{
	DWORD dwGemId; // 宝石Id
	WORD wHole;	// 孔位
};


// 物品丢失记录
struct ItemLoseRecord
{
	DWORD dwRoleId;
	DWORD dwItemId;
	DWORD dwItemNum;
};

// 物品操作记录
struct ItemOperationRecord
{
	WORD wFromId;
	DWORD dwRoleId;
	DWORD dwItemId;
	DWORD dwItemNum;
	DWORD dwTime;
	bool bIsAdd;
};

// 精炼消耗数据
struct EquipmentRefineCostData
{
	DWORD dwItemId;
	WORD wItemNum;
};

// 扭蛋抽奖记录
struct NDLotteryRecord
{
	WORD wServerId;					// 服务器ID	
	char szRoleName[MAX_ROLE_LEN];	// 角色名称
	DWORD dwItemId;					// 抽中的物品ID
	DWORD dwItemNum;				// 抽中的物品数量
	DWORD dwTime;					// 抽奖时间
};

// 客户端信息
struct ClientInfo
{
	char szDeviceId[MAX_DEVICE_ID_LEN]; // 客户端设备ID
	char szIP[IP_LEN];					// IP长度
	BYTE byTerminalType;				// 终端类型
};

// 转盘基本数据
struct TurntableBaseData
{
	DWORD dwServerId;			// 服务器ID
	WORD wCurActivityDataId;	// 当前活动数据ID
	DWORD dwDiamond;			// 奖池钻石数量
};

// 转盘抽奖记录
struct TurntableLotteryRecord
{
	DWORD dwRoleId; // 抽奖的角色ID
	DWORD dwTime;	// 抽奖时间戳
	WORD wIndex;	// 抽中的数据索引
	DWORD dwContext;// 上下文(物品数量,钻石)
};

// 转盘抽奖记录
struct ClientTurntableLotteryRecord : public TurntableLotteryRecord
{
	char szRoleName[MAX_ROLE_LEN];	// 抽奖的角色名称
};

// 转盘抽奖结果数据
struct TurntableLotteryResultData
{
	WORD wIndex;		// 数据索引
	DWORD dwContext;	// 上下文(物品数量,钻石)
};

// 阵营招募记录
struct CampRecruitRecord
{
	char szRoleName[MAX_ROLE_LEN];	// 招募的角色名称
	Item tRewardData;				// 招募到的奖励数据
	DWORD dwRecruitTime;			// 招募时间
	WORD wCamp;						// 所属阵营
};

// 资源战基础数据
struct ResourceWarBaseData
{
	WORD wChapter;					// 章节
	WORD wResourceId;				// 资源ID
};

// 资源战资源数据 
struct ResourceWarResouceData
{
	WORD wChapter;						// 章节
	WORD wResourceId;					// 资源ID
	WORD wResourceType;					// 资源类型
	DWORD dwOccupyStartTime;			// 占领开始时间戳
	DWORD dwOccupyOverTime;				// 占领结束时间戳
	DWORD dwOwnerId;					// 拥有者ID
	WORD wKeepOccupyTimes;				// 延长占领次数(默认就相当于延长了一次,但是次数从0开始)
	//WORD wCurOccupyRewardIdx;			// 当前占领奖励索引
	DWORD dwLastBaseRewardTime;			// 最近获得基础奖励的时间戳
	DWORD dwLastExtraRewardTime;		// 最近获得额外奖励的时间戳

	WORD wBaseRewardIntervalTime;		// 获得基础奖励间隔时间(分钟)
	WORD wCurExtraRewardNeedTime;		// 当前获得额外奖励需要的时间(分钟)
	WORD wAlreadySendBaseRewardTimes;	// 已经发放的基础奖励次数
	WORD wAlreadySendExtraRewardTimes;	// 已经发放的额外奖励次数
};

// 资源战数据(客户端) 
struct ResourceWarDataClient
{
	PlayerCommonData tOwnerInfo;	// 拥有者信息
	//WORD wChapter;					// 章节
	WORD wResourceId;				// 资源ID
};

// 资源战挑战记录 【挑战玩家名】占领了【被挑战玩家名】在第X章的【超级资源】
struct ResourceWardBattleRecord
{
	DWORD dwAttackerId;	// 攻击者ID
	DWORD dwDefenderId;	// 守防者ID
	DWORD dwBattleTime;	// 挑战时间
	WORD wChapter;		// 章节
	WORD wResourceId;	// 资源ID
};

// 资源战记录(客户端)
struct ResourceWardRecordClient
{
	char szAttackerName[MAX_ROLE_LEN];
	char szDefenderName[MAX_ROLE_LEN];
	DWORD dwBattleTime;	// 挑战时间
	WORD wChapter;		// 章节
	WORD wResourceId;	// 资源ID
};

// 资源战敌人信息
struct ResourceWarEnemy
{
	DWORD dwRoleId;
	WORD wAttackTimes;	// 攻击次数
};

// 资源战敌人信息
struct ResourceWarEnemyClient
{
	PlayerCommonData tCommonData;
	//DWORD dwRoleId;
	WORD wAttackTimes;	// 攻击次数
	WORD wChapter;		
	WORD wResourceId;	// 0=表示木有占领
};


// 角色资源战数据
struct RoleResourceWarData
{
	WORD wTechnologyLevel;			// 科技等级
	DWORD dwTotalAccupyTime;		// 总占领时间(秒)
	int nRewardResourceWarScore;	// 奖励资源战积分
};

// 服务器在线数据
struct ServerOnlineData
{
	WORD wServerId;			// 服务器ID
	WORD wOnlinePlayerNum;	// 在线玩家数量
};

// 角色战力
struct RolePower
{
	DWORD dwRoleId;
	DWORD dwPower;
};

#pragma pack(pop)
#endif
