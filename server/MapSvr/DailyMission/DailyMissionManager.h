/**\file
日常任务管理器
\author Kevin
\date 2016/10/18
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CDailyMissionManager
{
public:
	CDailyMissionManager();
	~CDailyMissionManager();

	void Init(Player *pPlayer);

	// 重置任务数据
	void OnNewDay();

public:
	// 请求任务列表
	void OnNetMissionList();

	// 领取任务奖励
	void OnNetMissionAward(WORD wMissionId);

	// 领取积分奖励
	void OnNetPointAward(WORD wPoint);

public:
	// 发送任务列表
	void SendMissionList();

	// 发送任务数据
	void SendMissionData(const DailyMissionData *pData);

	// 更新任务
	void OnUpdateMission(WORD wType, int nValue, bool bAdd);

	// 是否有红点
	bool IsHaveRedPoint();

protected:
	// 取任务数据
	DailyMissionData* GetMissionData(WORD wMissionId);

	// 添加任务数据
	void AddMissionData(const DailyMissionData &tMission);

	// 检测积分奖励(是否已领取)
	bool CheckPointAward(WORD wPoint);

	// 添加积分奖励记录
	void AddPointAwardRecord(WORD wPoint);

	// 更新任务
	void DoUpdateMission(WORD wMissionId, int nValue, bool bAdd);

private:
	Player *m_pPlayer;
	PlayerDailyMissionData *m_pDailyMissionInfo;
};