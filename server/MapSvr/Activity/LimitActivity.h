/**\file
限时活动逻辑类
\author Kevin
\date 2017/6/27
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;
struct LimitActivity_Config;

class CLimitActivity
{
public:
	CLimitActivity(){};
	~CLimitActivity(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// 首充数据请求
	void OnNetFirstRechargeData();

	// 单充数据请求
// 	void OnNetSingleRechargeData();
// 
// 	// 累充数据请求
// 	void OnNetAccumulateRechargeData();
// 
// 	// 限时招募数据请求
// 	void OnNetRecruitData();

	// 领取活动奖励
	void OnNetTakeActivityReward(WORD wDataId);

public:
	// 处理领取首充奖励
	bool DoTakeFirstRechargeReward(const LimitActivity_Config *pActivityCfg);

// 	// 处理领取单充奖励
// 	bool DoTakeSingleRechargeReward(const LimitActivity_Config *pActivityCfg);
// 
// 	// 处理领取累充奖励
// 	bool DoTakeAccumulateRechargeReward(const LimitActivity_Config *pActivityCfg);
// 
// 	// 处理限时招募奖励
// 	bool DoTakeLimitRecruitReward(const LimitActivity_Config *pActivityCfg);

	// 添加充值值
	void AddRechargeValue(DWORD dwValue);

// 	// 添加充值数据
// 	void AddRechargeData(WORD wRechargeId);
// 
// 	// 添加招募次数
// 	void AddRecruitTimes(WORD wRecruitTimes = 1);
// 

	// 是否有红点
 	bool IsHaveRedPoint(WORD wSystemId);

	// 是否已领取首充奖励
	bool IsTakeFirstRechargeReward(WORD wDataId);

	// 是否已领取限时招募奖励
// 	bool IsTakeLimitRecruitReward(WORD wDataId);
 
 	// 是否已领取了累充奖励
// 	bool IsTakeAccumulateRechargeReward(WORD wDataId);
// 
// 	// 是否已领取了奖励
// 	bool IsTakeReward(WORD wDataId);

	// 添加奖励记录
	bool AddRewardRecord(WORD wType, WORD wDataId);

	// 更新活动数据
	void UpdateActivityData(int nSystemId);

	// 取充值次数
	//WORD GetRechargeTimes(WORD wRechargeId);
protected:
	// 发送首充数据
	void SendFirstRechargeData();

// 	// 数据单充数据
// 	void SendSingleRechargeData();
// 
// 	// 发送累充数据
// 	void SendAccumulateRechargeData();
// 
// 	// 发送限时招募数据
// 	void SendRecruitData();

	// 取单充奖励记录
	//RecordData* FindSingleRechargeRewardRecord(WORD wDataId);

	// 查找普通充值数据
	//CommonRecharge* FindCommonRecharge(WORD wRechargeId);
private:
	Player *m_pPlayer;
	LimitActivityData *m_pLimitActivityData;

	// 限时活动
	CActivityManager *m_pActivityMgr;   
};