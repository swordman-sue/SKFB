#ifndef _SCORE_MATCH_DATA_DEFINE_H_
#define _SCORE_MATCH_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 积分赛挑战目标数据
struct ScoreMatchTarget
{
	DWORD dwTargetId;		// 目标ID
	//DWORD dwHeadIcondId;	// 头像ID
	//DWORD dwTitleId;		// 当前使用的称号(0=表示木有)
	//char szTargetName[MAX_ROLE_LEN];
	//WORD wServerId;			// 目标所在服务器
	//DWORD dwFightingPower;	// 目标战力
	WORD wStatus;			// 状态(0=还没挑战,1=战胜,2=战败)
};

// 角色积分赛数据
struct ScoreMatchData
{
	WORD wBattleTimes;		// 已经使用的挑战次数
	WORD wRefreshTimes;		// 已经使用的刷新次数
	WORD wBuyBattleTimes;	// 已经购买的挑战次数

	DWORD dwRoleId;		// 角色ID
	BYTE byCamp;		// 阵营
	WORD wWinTimes;		// 获胜次数

	// 任务奖励记录列表(已领取的)
	WORD arTaskRewardRecord[MAX_SCORE_MATCH_TASK_NUM];
	WORD wTaskRewardRecordNum;

	// 挑战目标列表
	ScoreMatchTarget arBattleTargetList[MAX_SCORE_MATCH_BATTLE_TARGET_NUM];
	WORD wBattleTargetNum;
};



// struct ScoreMatchData : public ScoreMatchBaseData
// {
// 	vector<ScoreMatchTarget> vTargetList;	// 挑战目标列表
// };

#pragma pack(pop)
#endif
