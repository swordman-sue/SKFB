/**\file
成就管理器
\author Kevin
\date 2016/10/17
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CAchievementManager
{
public:
	CAchievementManager();
	~CAchievementManager();

	void Init(Player *pPlayer);

public:

	// 请求成就列表
	void OnNetAchievementList();

	// 领取成就奖励
	void OnNetAchievementAward(WORD wAchievementId);

public:

	// 发送成就列表
	void SendAchievementList();

	// 发送成就数据
	void SendAchievementData(const AchievementData *pData);

	// 更新成就
	void OnUpdateAchievement(WORD wAchievementId, int nValue, bool bAdd);

	// 是否有红点
	bool IsHaveRedPoint();

protected:
	// 取成就数据
	AchievementData* GetAchievementData(WORD wAchievementId);

	// 添加成就数据
	void AddAchievementData(const AchievementData &tAchievement);

private:
	Player *m_pPlayer;
	PlayerAchievementData *m_pAchievementInfo;
};