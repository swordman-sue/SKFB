#pragma once
#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 7天目标数据
struct SevenDayTargetData
{
	WORD wId;				// 目标ID
	DWORD dwValue;			// 目标值
	BYTE byIsGetReward;		// 是否已领取奖励
};

// 玩家7天数据
// struct PlayerSevenDayData
// {
// 	SevenDayTargetData arTargetDataList[MAX_SEVEN_DAY_TARGET_NUM];
// 	BYTE byTargetDataNum;
// };

#pragma pack(pop)
//#endif
