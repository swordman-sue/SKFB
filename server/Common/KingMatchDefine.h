#ifndef _KING_MATCH_DEFINE_H_
#define _KING_MATCH_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 玩家王者赛数据
struct PlayerKingMatchData
{
	WORD wLastSeason;		// 最近一个赛季(参加的)
	WORD wTotalBattleTimes;	// 总的挑战次数
	WORD wTotalWinTimes;	// 总的获胜次数
	WORD wTodayWinTimes;	// 今日获胜次数
	WORD wStageLevel;		// 段位等级
	DWORD dwContext;		// 上下文(星数,积分)
	WORD wHistoryStageLevel;// 历史段位等级

	// 赛季任务奖励记录
	WORD arSeasonTaskRewardRecord[MAX_SEASON_TASK_REWARD_RECORD];
	WORD wSeasonRewardRecordNum;
};

// 王者赛挑战目标
struct KingMatchBattleTarget
{
	DWORD dwRoleId;		// 角色ID
	WORD wServerId;		// 服务器ID
	WORD wStageLevel;	// 段位等级
	WORD wStar;			// 星数
	WORD wBattleTimes;	// 挑战次数
};

// 王者赛排行记录
struct KingMatchRankRecord
{
	WORD wSeason; // 赛季
	WORD wRoleRankNum; // 角色排行数量
	PlayerCommonData arRoleRankList[MAX_KING_MATCH_TOP_RANK_NUM];	// 角色排行列表
};

// 角色王者赛季排行榜数据
struct RoleKingMatchSeasonRank
{
	DWORD dwRoleId;		// 角色ID
	WORD wServerId;		// 服务器ID	
};

// 奖励类型
/*
1.段位奖励
2.王者排行奖励
3.赛季胜场奖励
4.每场战斗奖励
5.每日首胜奖励
6.升段奖励
*/


#pragma pack(pop)
#endif
