#ifndef _GUILD_DATA_DEFINE_H_
#define _GUILD_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 申请加入工会数据
struct GuildApplyData
{
	DWORD dwRoleId;					// 申请角色ID
};

// 工会成员数据
struct GuildMember
{
	DWORD dwRoleId;						// 成员ID
	BYTE byPosition;					// 职位
	DWORD dwTotalContribution;			// 累积贡献值 
	//DWORD dwCurContribution;			// 当前贡献值(todo::此处字段已不使用) 
	DWORD dwTodayContribution;			// 今日贡献值
	DWORD dwLastLogoutTime;				// 最近的离线时间
	BYTE byQYType;						// 泉涌类型(0表示还木有泉涌)
	BYTE byBattleTimes;					// 挑战副本次数
	BYTE byTodayBattleTimes;			// 今日挑战次数
	BYTE byBuyBattleTimes;				// 已经购买的副本挑战次数
	DWORD dwLastBattleTimesRecoverTime;	// 挑战次数最近恢复时间

	// 领取泉水奖励记录
	WORD arQSRewardRecord[MAX_GUILD_QS_REWARD_RECORD];
	BYTE byQSRewardRecordNum;

	// 领取副本章节奖励记录(已迁移到人物身上)
// 	WORD arTakeFBChapterRewardRecord[MAX_GUILD_FB_CHAPTER_NUM];
// 	WORD wTakeFBChapterRewardRecordNum;

	// 不需要存储的字段
	bool bHaveNewLeaveMessage;  // 是否有新的留言
	bool bIsLimitShopRefresh;	// 是否限时商店刷新


	void Init()
	{
		memset(this, 0, sizeof(GuildMember));
	}
};

// 客户端工会成员数据
struct ClientGuildMember : public GuildMember
{
	char szRoleName[MAX_ROLE_LEN];	// 成员名称
	WORD wLevel;					// 成员等级
	DWORD dwFightingPower;			// 战力
	WORD wHeadIconId;				// 头像ID
	void Init()
	{
		memset(this, 0, sizeof(GuildMember));
	}
};

// 工会数据
struct GuildData
{
	DWORD dwId;										// 工会ID
	char szName[MAX_GUILD_NAME_LEN];				// 工会名称
	WORD wIconId;									// 工会图标
	DWORD dwCreateTime;								// 创建时间
	WORD wLevel;									// 公会等级
	DWORD dwExp;									// 公会经验
	DWORD dwQuanShui;								// 泉水
	BYTE byQYTimes;									// 泉涌次数
	char szNotice[MAX_GUILD_NOTICE_LEN];			// 公告
	char szDeclaration[MAX_GUILD_DECLARATION_LEN];	// 宣言
	WORD wTodayStartChapter;						// 今日开始章节
	WORD wCurPassChapter;							// 当前通关的章节
	WORD wHistoryChapterRecord;						// 历史章节记录
	WORD wIsFromHistoryChapter;						// 是否从历史章节开始
	DWORD dwActive;									// 工会活跃度

	GuildMember arMemberList[MAX_GUILD_MEMBER_NUM];		// 成员列表
	BYTE byMemberNum;									// 成员数量

	void Init()
	{
		memset(this, 0, sizeof(GuildData));
	}
};

// 工会留言数据
struct GuildLeaveMessage
{
	DWORD dwTime;								// 留言时间
	char szName[MAX_ROLE_LEN];					// 留言角色名称
	BYTE byPos;									// 留言角色职位
	char szText[MAX_GUILD_LEAVEMESSAGE_LEN];	// 留言内容
};

// 工会基本数据
struct GuildBaseData
{
	DWORD dwId;										// 工会ID
	char szGuildName[MAX_GUILD_NAME_LEN];			// 工会名称
	WORD wIconId;									// 工会图标
	WORD wLevel;									// 公会等级
	BYTE byMemberNum;								// 成员数量
	char szCaptainName[MAX_ROLE_LEN];				// 会长名称
	BYTE byIsFull;									// 是否已满员
	BYTE byIsApply;									// 是否已申请
	char szDeclaration[MAX_GUILD_DECLARATION_LEN];	// 宣言
};

// 工会事件
struct GuildEvent
{
	WORD wEventId;						// 事件ID
	char szTargetName[MAX_ROLE_LEN];	// 事件目标名称
	DWORD dwValue;						// 上下文值
	DWORD dwTime;						// 发生时间
};

// 军团副本数据
struct GuildFBData
{
	WORD wCurPassChapter;	// 当前通关的章节(0=表示还没有通关)
	BYTE byBattleTimes;		// 已挑战购买次数
	BYTE byBuyTimes;		// 已购买次数
	DWORD dwLastBattleTimesRecoverTime;	// 挑战次数最近恢复时间
	WORD wTodayStartChapter;						// 今日开始章节
	//WORD wHistoryChapterRecord;						// 历史章节记录
	WORD wIsFromHistoryChapter;						// 是否从历史章节开始

	vector<WORD> vTakeChapterRewardRecord;	// 领取章节奖励记录
};

// 军团副本宝箱
struct GuildFBBox
{
	WORD wBoxIdx;		// 宝箱索引
	WORD wRewardIdx;	// 宝箱奖励索引
	char szRoleName[MAX_ROLE_LEN]; // 领取者的名称
};

// 军团副本关卡奖励数据
struct GuildFBMapReward
{
	vector<GuildFBBox> vTakeBoxList[CAMP_NUM];		// 已领取的宝箱记录列表
	set<DWORD> arAlreadyGetPlayerList[CAMP_NUM];	// 已经领取的玩家列表
};

// 军团副本伤害数据
struct GuildFBDamageData : public PlayerCommonData
{
	DWORD dwDamage;		// 伤害
	BYTE byBattleTimes;	// 战斗次数
};

// 工会副本怪物数据
// struct GuildMonsterData : public MonsterData
// {
// 	char szKillerName[MAX_ROLE_LEN];	// 击杀者名称
// };

// 我的工会章节关卡奖励记录
struct MyGuildMapRewardRecord
{
	WORD wChapter;	// 章节
	BYTE arRecordList[CAMP_NUM]; // 记录列表 
};


#pragma pack(pop)
#endif
