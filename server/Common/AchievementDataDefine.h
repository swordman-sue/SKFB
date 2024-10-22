#ifndef _ACHIEVEMENT_DATA_DEFINE_H_
#define _ACHIEVEMENT_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 成就数据
struct AchievementData
{
	WORD wId;			// 成就ID
	BYTE byLv;			// 成就等级	
	DWORD dwValue;		// 成就值
	BYTE byCurRewardLevel;	// 当前奖励等级(已经领取的)
};

// 玩家成就数据
struct PlayerAchievementData
{
	AchievementData arAchievementList[MAX_ACHIEVEMENT_NUM];
	WORD wAchievementNum;
};

#pragma pack(pop)
#endif
