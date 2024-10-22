#pragma once

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// 关卡地图数据
struct MapData
{
	BYTE byStar;			// 获得的星数
	BYTE byBattleTimes;		// 当前挑战次数
	BYTE byResetTimes;		// 今日重置次数
	BYTE byIsGetAward;		// 是否已经领取奖励
};

// 章节数据
struct ChapterData
{
	MapData arMapList[MAX_CHAPTER_MAP_NUM];			// 关卡列表
	BYTE byMapIdx;									// 当前关卡索引(从1开始算)
	BYTE arStarAwardList[CHAPTER_STAR_AWARD_MAX];	// 星数奖励领取标志列表(1=已领取,0=木有领取)

	WORD GetStar()
	{
		WORD wStar = 0;
		for (BYTE i = 0; i < byMapIdx; ++i)
		{
			wStar += arMapList[i].byStar;
		}
		return wStar;
	}
};

// 主线副本数据
struct MainFBData
{
	ChapterData arChapterList[MAX_FUBEN_CHAPER_NUM];	// 章节数据列表
	WORD wChapter;	// 当前章节
	WORD wTakeChapterRewardRecord; // 领取章节奖励记录
};

// 精英关卡敌军数据
struct EliteMapEnemyData
{
	WORD wMapId;
	WORD wIsDead;	// 是否已死亡

	MonsterData arMonsterList[MAX_BATTLE_MONSTER_NUM];	// 怪物列表(todo::已无效)
	BYTE byMonsterNum;	// 怪物数量(todo::已无效)
};

// 精英副本章节数据
struct EliteFBChapter
{
	MapData arMapList[MAX_CHAPTER_MAP_NUM];			// 关卡列表
	BYTE byMapIdx;									// 当前关卡索引
	BYTE arStarAwardList[CHAPTER_STAR_AWARD_MAX];	// 星数奖励领取标志列表	
	EliteMapEnemyData tEnemyData;					// 外敌数据

	WORD GetStar()
	{
		WORD wStar = 0;
		for (BYTE i = 0; i < byMapIdx; ++i)
		{
			wStar += arMapList[i].byStar;
		}
		return wStar;
	}
};

// 精英副本数据
struct EliteFBData
{
	EliteFBChapter arChapterList[MAX_FUBEN_CHAPER_NUM];
	WORD wChapter;	// 当前章节
	DWORD dwLastRefreshEnemyTime;	// 上次刷新外敌时间
};

// 英雄关卡数据
struct HeroMapData
{
	BYTE byBattleTimes;		// 挑战次数
	BYTE byIsGetAward;		// 是否已领取奖励
	//BYTE byIsBattle;		// 今日是否已经挑战过
};

// 英雄副本章节数据
struct HeroFubenChapterData
{
	HeroMapData arMapList[MAX_CHAPTER_MAP_NUM];// 关卡列表
	BYTE byMapIdx;		// 当前关卡等级
	BYTE byIsExtraMapPass;	// 额外关卡是否通关
};

// 英雄副本数据
struct HeroFBData
{
	HeroFubenChapterData arChapterList[MAX_FUBEN_CHAPER_NUM]; // 章节数据列表 
	WORD wChapter;		// 当前章节
	BYTE byBattleTimes;	// 今日已挑战次数
};

// 日常副本章节数据
struct DailyChapterData
{
	WORD wChapter;			// 章节(关卡类型)
	short nMapIdxRecord;	// 关卡通关记录
	BYTE byBattleTimes;		// 今日已挑战次数
};

// 日常副本数据
struct DailyFBData
{
	DailyChapterData arChapterList[MAX_DAILY_MAP_NUM]; // 今日已挑战过的章节
	WORD wChapterNum; // 今日已挑战的章节数量
};


// 日常副本数据
// struct DailyFBDataEx
// {
// 	WORD arChapterList[MAX_DAILY_MAP_NUM];	// 今日已挑战过的章节
// 	BYTE byChapterNum;						// 今日已挑战的章节数量
// };

// 爬塔副本章节数据
struct TowerFubenChapterData
{
	BYTE arMapList[MAX_CHAPTER_MAP_NUM];// 关卡列表
	WORD wMapLevel;						// 当前关卡等级
	WORD wPropertyIndex;				// 选择加属性的索引
};

// 爬塔副本数据
struct TowerFBData
{
	BYTE arMapStarList[MAX_TOWER_MAP_NUM];	// 关卡星数列表
	WORD wCurMapLevel;						// 当前关卡等级(已经打过的)
	WORD wMapLevelRecord;					// 关卡等级记录
	BYTE byKeepThreeStarPassedRecord;		// 连续三星通关记录(章节)

	Property arSelectProList[HERO_PRO_MAX];	// 选择的属性列表
	BYTE bySelectProNum;					// 选择的属性数量

	BYTE byIsBuyStarGoods;				// 是否已经购买星数商品
	BYTE byStarGoodsIndex;				// 星数商品索引

	BYTE byIsNeedReset;				// 是否需要重置
	BYTE byTodayResetTimes;			// 今日已重置次数

	WORD wCurStar;					// 当前星数(选择属性扣掉剩下的)
	WORD wStarMaxRecord;			// 历史最佳星数记录

	WORD wEliteMapLevel;			// 精英关卡等级(已经打过的)
	BYTE byEliteMapBattleTimes;		// 精英关卡已经挑战的次数
	BYTE byBuyEliteMapBattleTimes;	// 已经购买的精英关卡挑战次数

	BYTE arRandProIdxList[3];		// 随机属性索引列表
	BYTE  byRandProNum;				// 随机属性数量
};

// 叛军数据
// 当前关卡等级

// 好友分享的关卡列表

// 玩家副本数据 
struct PlayerFubenInfo
{
	MainFBData tMainFubenData;		// 主线副本数据
	HeroFBData tHeroFubenData;		// 名将副本数据
	DailyFBData tDailyFubenData;	// 日常副本数据
	TowerFBData tTowerFubenData;	// 爬塔副本数据
	EliteFBData tEliteFBData;		// 精英副本数据
};

#pragma pack(pop)
