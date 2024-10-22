/**\file
世界等级任务管理器
\author Kevin
\date 2018/5/25
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CWorldLevelTaskMananger
{
public:
	CWorldLevelTaskMananger();
	~CWorldLevelTaskMananger();

	void Init(Player *pPlayer);

	void OnNewDay();

public:
	// 请求世界等级任务数据
	void OnNetWorldLevelTaskData();

	// 领取任务奖励
	void OnNetTakeTaskReward(WORD wTaskId);

	// 领取最终奖励
	void OnNetTakeFinalReward();

public:
	// 更新任务事件
	void UpdateTaskEvent(WORD wTaskType, int nValue, bool isAdd = true);

	// 更新任务
	void UpdateTask();

	// 是否有红点
	bool IsHaveRedPoint();

protected:

	// 发送等级任务数据
	void SendWorldLevelTaskData();

	// 添加任务事件
	void AddTaskEvent(WorldLevelTaskEvent &tTaskEvent);

	// 取任务事件
	WorldLevelTaskEvent* GetTaskEvent(WORD wTaskId);

private:
	Player *m_pPlayer;

	// 世界等级任务数据
	PlayerWorldLevelTaskData *m_PlayerWorldLevelTaskData;
};