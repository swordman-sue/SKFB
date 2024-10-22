#ifndef _ZHAN_BU_DATA_DEFINE_H_
#define _ZHAN_BU_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// 卡牌数据
struct Card
{
	DWORD dwId;			// 唯一ID
	DWORD dwInfoId;		// 信息ID
	WORD wLevel;		// 等级
	DWORD dwExp;		// 经验
	WORD wBreakLevel;	// 突破等级
};

// 客户端卡牌数据
struct ClientCard : public Card
{
// 	DWORD dwId;			// 唯一ID
// 	DWORD dwInfoId;		// 信息ID
// 	WORD wLevel;		// 等级
// 	DWORD dwExp;		// 经验
// 	WORD wBreakLevel;	// 突破等级
	DWORD dwHeroId;		// 穿戴的英雄ID
};

// 仙灵图鉴
struct FairyPokedex
{
	WORD wId;		// 图鉴Id
	BYTE byStar;	// 星数
};

// 卡组数据
struct CardGroupData
{
	DWORD dwCardId; // 卡牌ID
	WORD wIsAlreadyOpen; // 是否已翻开
};

// 收藏组
struct CollectionGroup
{
	WORD wId;	// 收藏组ID
	WORD wStar; // 星数
};

// 收藏书
struct CollectionBook
{
	WORD wBookId;

	// 收藏组列表
	CollectionGroup arCollectionGroupList[MAX_COLLECTION_GROUP_NUM];
	WORD wCollectionGroupNum;
};

// 玩家占卜数据
struct PlayerZhanBuInfo
{
	Card arCardList[MAX_CARD_NUM];	// 卡牌列表
	WORD wCardNum;	// 仙灵数量

	DWORD dwHistoryCollectionValue; // 历史收藏值

	WORD wLowCardGroupMapIdx;	// 低级卡组关卡索引
	WORD wCurLowCardGroupId;	// 当前低级卡组ID

	DWORD dwLowCardGroupLastRecoverTime;	// 低级卡组最近恢复时间
	WORD wCurLowCardGroupHaveRefreshTimes;	// 当前低级卡组拥有的刷新次数

	WORD wTodayLowCardGroupOpenCardTimes;		// 今日低级卡组翻卡次数
	WORD wTodayLowCardGroupCostRefreshTimes;	// 今日低级卡组已经花费刷新的次数(需要消耗的刷新次数)
	WORD wTodayHighCardGroupCostRefreshTimes;	// 今日高级卡组已经花费刷新的次数(需要消耗的刷新次数)

	DWORD dwHighCardGroupLastRefreshTime;	// 高级卡组最近刷新时间

	// 低级卡组
	CardGroupData arLowCardGroup[MAX_CARD_GROUP_NUM];
	WORD wLowCardGroupNum;

	// 高级卡组
	CardGroupData arHighCardGroup[MAX_CARD_GROUP_NUM];
	WORD wHighCardGroupNum;

	// 收藏书列表
	CollectionBook arCollectionBookList[MAX_COLLECTION_BOOK_NUM];
	WORD wCollectionBookNum;

	WORD wAchievementLevel; // 成就等级
};

#pragma pack(pop)
#endif
