#ifndef _GUILD_WAR_DATA_DEFINE_H_
#define _GUILD_WAR_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// 对抗星球活动状态
enum VSStarActivityStatus
{
	VSSTAR_ACTIVITY_CLOSE = 0,		// 关闭
	VSSTAR_ACTIVITY_DECLARE_WAR,	// 宣战
	VSSTAR_ACTIVITY_BATTLE,			// 战斗
	VS_STAR_DECIDE_ATTACKER,		// 决定攻击者时间
	VSSTAR_ACTIVITY_EXPLOIT = 4,	// 开采

	VSSTAR_ACTIVITY_BEGIN = VSSTAR_ACTIVITY_DECLARE_WAR,
	VSSTAR_ACTIVITY_END = VSSTAR_ACTIVITY_BATTLE,
};


// 塔类型
enum
{
	STAR_TOWER_1 = 1, // 1 = 基地
	STAR_TOWER_2 = 2, // 2 = 上路
	STAR_TOWER_3 = 3, // 3 = 中路
	STAR_TOWER_4 = 4, // 4 = 下路
};

// 星球类型
enum
{
	// 低级星球
	GUILD_WAR_STAR_GREEN = 1,	// 1 = 绿品
	GUILD_WAR_STAR_BLUE = 2,	// 2 = 蓝品

	// 高级星球
	GUILD_WAR_STAR_PURPLE = 3,	// 3 = 紫品 
	GUILD_WAR_STAR_ORANGE = 4,	// 4 = 橙品	
};

// 星球防守者信息
struct StarDefenderInfo
{
	DWORD dwRoleId;			// 角色ID
	WORD wBeAttackTimes;	// 被攻击次数
};

struct StarTowerBaseInfo
{
	WORD wTowerId;				// 塔信息ID
	DWORD dwDurability;			// 耐久度
	//WORD wType;					// 类型
	WORD wMonsterBeAttackTimes;	// 怪物被攻击次数
};

// 星球塔信息
struct StarTowerInfo : public StarTowerBaseInfo
{
	vector<StarDefenderInfo> vDefenderList; // 防守者列表(人守是才有效)
};

struct StarDefenderForClient
{
	DWORD dwRoleId;			// 角色ID
	WORD wBeAttackTimes;	// 被攻击次数
	WORD wHeadIcon;			// 头像(为形象英雄InfoId)
	DWORD dwFightingPower;	// 战力
	char szRoleName[MAX_ROLE_LEN];	// 角色名称
};

// 工会战塔信息(客户端)
struct GuildWarTowerForClient
{
	WORD wTowerId;				// 塔信息ID
	DWORD dwDurability;			// 当前耐久度
	WORD wMonsterBeAttackTimes;	// 怪物被攻击次数
	vector<StarDefenderForClient> vDefenderList; // 防守者列表(人守是才有效)
};

struct StarBaseInfo
{
	DWORD dwInfoId;			// 信息ID
	DWORD dwDelongGuildId;	// 所属工会ID
	DWORD dwAttackGuildId;	// 进攻工会的ID
};

// 星球信息
struct StarInfo : public StarBaseInfo
{
	map<DWORD, DWORD> mRewardRecordList;	// 奖励领取记录列表[role_id,role_id]

	//// 积分模式专用
	map<DWORD, DWORD> mScoreList;			// 积分列表(积分模式使用)[GuildId,Score]

	//// 对战模块专用
	map<DWORD, DWORD> mDeclareWarList;		// 宣战列表[GuildId, GuildId]
	vector<StarTowerInfo> vTowerList;		// 塔列表

	// 上一次所属的工会ID,只是本次活动期间有效，活动结束后会清空
	DWORD dwLastDelongGuildId;	// (todo::因为是后面加的功能,为了不影响之前功能,使用的逻辑有点别扭)
};


// 角色工会战数据
struct RoleGuildWarData
{
	WORD wBuyHighStarBattleTimes;		// 1.购买高级星球挑战次数
	WORD wUsedLowStarBattleTimes;		// 2.低级星球已挑战次数 
	WORD wUsedHighStarBattleTimes;		// 3.高级星球已挑战次数
};

// 工会战宣战信息
struct GuildWarDeclareWarData
{
	WORD wStarId; // 宣战的星球ID
	char szGuildName[MAX_GUILD_NAME_LEN];
	DWORD dwActive;	
	WORD wServerId;
	WORD wIconId;	// 工会图标
};

// 占领的星球信息(客户端使用)
struct GuildWarOccupyStarDataForClient
{
	WORD wStarId; // 星球ID
	BYTE byIsHaveReward;	// 是否有奖励
};

// 星球基本信息(客户端使用)
struct GuildWarStarBaseDataForClient
{
	char szGuildName[MAX_GUILD_NAME_LEN];	// 占领的工会名称
	WORD wStarId;							// 星球ID
 	WORD wIconId;							// 工会图标
	BYTE byIsInBattle;						// 是否在战斗中(跟我关联的)	
// 	DWORD dwActive;							// 工会活跃度
};

// 星球所属工会信息
struct StarBelongGuildData
{
	DWORD dwGuildId;
	char szGuildName[MAX_GUILD_NAME_LEN];	// 工会名称
	DWORD dwActive;
	WORD wServerId;
	WORD wIconId;							// 工会图标
};

// 星球攻击工会信息
struct StarAttackGuildData : public StarBelongGuildData
{
		
};


// 工会战星球基本信息
struct GuildWarStarBaseData
{
	WORD wStarId; // 宣战的星球ID
	char szGuildName[MAX_GUILD_NAME_LEN];
	DWORD dwContext;	// 上下本(活跃度,积分)
	WORD wServerId;
	WORD wIconId;		// 工会图标
};

#pragma pack(pop)
#endif
