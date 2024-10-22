#ifndef _ARENA_DATA_DEFINE_H_
#define _ARENA_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 竞技场排行数据
struct ArenaRankData
{
	DWORD dwTargetId;		// 目标Id
	WORD wCurRank;			// 当前排名
	WORD wHistoryBestRank;	// 历史最佳排名
};

// 竞技场目标数据
struct ArenaTargetData
{
	DWORD dwTargetId;					// 目标Id
	WORD wHeadIcon;						// 头像图标
	char szTargetName[MAX_ROLE_LEN];	// 目标名称
	WORD wRank;							// 排名
	DWORD dwFightingPower;				// 战力
	DWORD dwTitleId;					// 称号ID
	DWORD dwNicknameId;					// 昵称ID
};

// 竞技战报
struct ArenaBattleRecord
{
	DWORD dwTime;						// 战斗时间
	DWORD dwTargetId;					// 目标ID
	char szTargetName[MAX_ROLE_LEN];	// 目标名称
	WORD wTargetHeadIcon;				// 目标头像图标
	BYTE byIsWin;						// 是否获胜
	BYTE byIsAttack;					// 是否攻击
	WORD wMyOldRank;					// 我的战前排名
	WORD wMyNewRank;					// 我的战后排名
	WORD wTargetLevel;					// 目标等级

	ArenaBattleRecord()
	{
		memset(this, 0, sizeof(ArenaBattleRecord));
	}
};

// 玩家竞技场战斗记录
struct PlayerArenaBattleRecord
{
	DWORD dwRoleId;
	ArenaBattleRecord arBattleRecordList[MAX_ARENA_BATTLE_RECORD];
	WORD wBattleRecordNum;
};

#pragma pack(pop)
#endif
