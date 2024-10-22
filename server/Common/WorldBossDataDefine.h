#ifndef _WORLD_BOSS_DATA_DEFINE_H_
#define _WORLD_BOSS_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 玩家世界BOSS数据
struct PlayerWorldBoseData
{
	BYTE byCamp;				// 选择的阵营(CAMP_INVALID=99表示还木有选择)
	BYTE byBattleTimes;			// 已经挑战的次数
	BYTE byBuyBattleTimes;		// 已经购买的挑战次数
	DWORD dwLastRecoveryTime;	// 上次恢复时间
	vector<WORD> vHonorRewardRecordList;	// 荣誉奖励记录列表
	vector<WORD> vDamageRewardRecordList;	// 伤害奖励记录列表
};

// 世界BOSS战报
struct WorldBossBattleRecord
{
	WORD wBossLevel;						// Boss等级	
	DWORD dwLuckAttackTime;					// 幸运攻击时间
	DWORD dwKillTime;						// 击杀时间
	char szLuckAttackName[MAX_ROLE_LEN];	// 幸运攻击者名称
	char szKillName[MAX_ROLE_LEN];			// 击杀者名称
};

// 世界BOSS数据
struct WorldBossData
{
	WORD wServerId; // 服务器ID
	WORD wBossLevel;	// 当前世界BOSS等级
	vector<MonsterData> vMonsterList; // 怪物列表

	DWORD dwReliveTime;	// 复活时间
	vector<WorldBossBattleRecord> vBattleRecordList; // 战报记录列表
	bool bHaveLuckReward;
};

#pragma pack(pop)
#endif
