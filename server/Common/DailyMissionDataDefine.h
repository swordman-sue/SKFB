#ifndef _DAILY_MISSION_DATA_DEFINE_H_
#define _DAILY_MISSION_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 日常任务数据
 struct DailyMissionData
 {
 	WORD wMissionId;	// 任务ID
 	BYTE byIsGetAward;	// 是否已领取奖励	
 	DWORD dwValue;		// 任务值
 };
 
 // 玩家日常任务数据
 struct PlayerDailyMissionData
 {
	 // 当前积分
	 WORD wPoint;

	 // 积分奖励记录列表
	 WORD arPointAwardRecord[MAX_DAILY_MISSION_POINT_AWARD];
	 WORD wPointAwardRecordNum;

	 // 日常任务列表
	 DailyMissionData arDailyMissionList[MAX_DAILY_MISSION_NUM];
	 WORD wDailyMissionNum;	
 };

#pragma pack(pop)
#endif
