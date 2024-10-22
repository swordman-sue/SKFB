/**\file
7天管理器
\author Kevin
\date 2016/11/19
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CSevenDayManager
{
public:
	CSevenDayManager();
	~CSevenDayManager();

	void Init(Player *pPlayer);

public:

	// 请求目标列表
	void OnNetTargetList();

	// 领取目标奖励
	void OnNetTargetAward(WORD wTargetId);

	// 领取完成度奖励 
	void OnNetCompletionReward(WORD wCompletionNum);

public:

	// 发送目标列表
	void SendTargetList();

	// 发送目标数据
	void SendTargetData(const SevenDayTargetData *pData);

	// 更新目标数据
	void OnUpdateTarget(WORD wLogicType, DWORD dwX, bool bAdd);

	// 更新目标数据
	void OnUpdateTarget(WORD wLogicType);

	// 是否有红点
	bool IsHaveRedPoint();

	// 是否有红点
	bool IsHaveRedPointEx();
protected:
	// 取成就数据
	SevenDayTargetData* GetTargetData(WORD wTargetId);

	// 添加成就数据
	void AddTargetData(const SevenDayTargetData &tTargetData);

	// 取活动天数
	WORD GetActivityDay();

	// 是否指定天数的目标
	bool IsTheDayTarget(WORD wTargetId,WORD wActivityDays);

	// 更新目标数据(目标值由外面计算)
	void DoUpdateTarget(WORD wTargetId, DWORD dwValue, bool bAdd);

	// 更新目标数据(目标值在里面计算)
	void DoUpdateTarget(WORD wTargetId);

	// 取当前完成度
	WORD GetComplationNum();

	// 是否完成度奖励记录
	bool IsComplationRewardRecord(WORD wComplationValue);

	// 是否当前活动目标
	bool IsCurActivityTarget(WORD wTargetId);
private:
	Player *m_pPlayer;
	//PlayerSevenDayData *m_pSevenDayInfo;
	PlayerActivityData *m_pActivityInfo;

};