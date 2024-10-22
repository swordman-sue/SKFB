/**\file
节日活动逻辑类
\author Kevin
\date 2017/6/27
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;
struct FestivalActivity_Config;

class CFestivalActivity
{
public:
	CFestivalActivity(){};
	~CFestivalActivity(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	void OnNewDay();

public:

	// 活动活动请求
	void OnNetActivityData(WORD wActivityType);

	// 领取活动奖励
	void OnNetTakeActivityReward(WORD wActivityType, WORD wTargetId);

	// 商品兑换
	void OnNetGoodsExchange(WORD wActivityType, WORD wTargetId, WORD wExchangeNum);

	// 请求红点
	void OnNetRedPoint();

public:
	// 单充回调
	void OnOnceRechargeCallback(WORD wRechargeId);

	// 循环累充回调
	void OnAccumulateRechargeCallback(DWORD dwAmount);

	// 限时招募
	void OnLimitRecruit(WORD wTimes);

	// 打折兑换（钻石换物品）
	void OnDiamondExchange(WORD wTargetId, DWORD dwExchangeNum);

	// 活跃目标 
	void OnActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd);

	// 活跃目标 
	void OnActiveTarget(WORD wActivityType, WORD wTargetId, DWORD dwContext);

	// 材料兑换（物品换物品）
	void OnItemExchange(WORD wTargetId, DWORD dwExchangeNum);

	// 节日登陆
	void OnNetFestivalLogin();

public:
	// 处理单充事件
	void DoOnceRechargeEvent(WORD wActivityType,WORD wRechargeId);

	// 处理累充事件
	void DoAccumulateRechargeEvent(WORD wActivityType, DWORD dwAmount);

	// 处理登录事件
	void DoLoginEvent(WORD wActivityType);

	// 处理登录事件
	void DoLoginEventEx(WORD wActivityType);

	// 处理活跃目标事件
	void DoActiveTargetEvent(WORD wActivityType, WORD wTargetId, DWORD dwContext);

	// 处理兑换事件
	void DoExchangeEvent(WORD wActivityType, WORD wTargetId, DWORD dwExchangeNum);

public:
	// 是否节日活动开放
	static bool IsFestivalActivityOpen(int nActivityType, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 是否节日活动开放
	static bool IsFestivalActivityOpen(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 取活动的数据重置时间(取正在活动中的)
	static DWORD GetActivityDataResetTime(WORD wActivityType, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 取活动的数据重置时间(取正在活动中的)
	static DWORD GetActivityDataResetTime(const FestivalActivity_Config *pActivityCfg, DWORD dwServerId, DWORD dwCreateRoleTime);

	// 是否有红点
	bool IsHaveRedPoint();

	// 是否有红点
	bool IsHaveRedPoint(WORD wActivityType);
protected:
	// 添加目标数据
	void AddActivityTarget(WORD wActivityId, WORD wTargetId, DWORD dwValue = 1);

	// 取节日活动数据
	FestivalActivity* GetFestivalActivity(WORD wActivityId);

	// 更新活动数据
	void UpdateActivityData(WORD wActivityType);

	// 发送活动数据
	void SendActivityData(WORD wActivityType);



private:
	Player *m_pPlayer;
	PlayerFestivalActivityInfo *m_pFestivalActivityInfo;

	// 限时活动
	CActivityManager *m_pActivityMgr;
};