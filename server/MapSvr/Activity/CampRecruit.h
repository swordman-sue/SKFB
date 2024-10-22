/**\file
阵营招募
\author Kevin
\date 2018/3/27
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CCampRecruit
{
public:
	CCampRecruit(){};
	~CCampRecruit(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// 请求招募数据
	void OnNetRecruitData();

	// 招募请求
	void OnNetRecruit(WORD wRecuitType, WORD wCamp);

	// 积分任务奖励请求
	void OnNetScoreTaskReward(WORD wCamp, WORD wRewardIdx);

	// 招募记录请求
	void OnNetRecruitRecord(WORD wCamp);

public:
	// 发送招募数据
	void SendRecruitData();

	// 更新招募数据
	void UpdateRecruitData();

	// 取当前活动数据ID
	WORD GetCurDataId();

	// 添加阵营招募积分
	void AddCampRecruitScore(WORD wCamp, DWORD dwScore);

	// 取阵营招募积分
	DWORD GetCampRecruitScore(WORD wCamp);

	// 是否积分任务奖励记录
	bool IsScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx);

	// 添加积分任务奖励记录
	void AddScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx);

	// 添加招募记录
	void AddRecruitRecord(WORD wCamp, const Item &tItem);

private:
	Player *m_pPlayer;
	PlayerCampRecruitData *m_pCampRecruitData;
	CActivityManager *m_pActivityMgr;
};