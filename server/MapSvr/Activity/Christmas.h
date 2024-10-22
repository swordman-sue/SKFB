/**\file
圣诞节逻辑类
\author Kevin
\date 2017/12/13
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

// 圣诞节BOSS数据
struct ChristmasBossData
{
	WORD wBossLevel;	// Boss等级
	DWORD dwCurHP;		// 当前HP
	DWORD dwDeadTime;	// 死亡时间(0=表示还活着)
};

// 圣诞节数据
struct ChristmasData
{
	// 低级BOSS数据
	ChristmasBossData tLowerBossData;

	// 高级BOSS数据
	ChristmasBossData tHighBossData;
};

// 圣诞节boss类型
enum
{
	CHRISTMAS_BOSS_LOWER = 1,	// 低级的
	CHRISTMAS_BOSS_HIGH,		// 高级的
};

class CChristmas : public Singleton<CChristmas>
{
public:
	CChristmas();
	~CChristmas();

	void Init();

	// 登出
	void OnLogout(Player *pPlayer);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// 重置数据(活动重开)
	void ResetData();

public:

	// 请求圣诞节数据
	void OnNetChristmasData(Player *pPlayer);

	// 进入战斗
	void OnNetEnterBattle(Player *pPlayer);

	// 离开战斗
	void OnNetLeaveBattle(Player *pPlayer);

	// 攻击Boss
	void OnNetAttackBoss(Player *pPlayer, WORD wBossType, DWORD dwSnowBallId);

	// 领取奖励
	void OnNetTakeBossReward(Player *pPlayer, WORD wBossType);

public:

	// 发送圣诞数据
	void SendChristmasData(Player *pPlayer);

	// 初始化BOSS
	//void InitBoss(WORD wBossType, WORD wBossLevel = 1);

protected:

	// 删除正在挑战的玩家
	void RemoveBattlePlayer(DWORD dwServerId, DWORD dwRoleId);

	// 添加正在挑战的玩家
	void AddBattlePlayer(Player *pPlayer);

	// 是否正在挑战中
	bool IsInBattle(DWORD dwServerId, DWORD dwRoleId);

	// 取BOSS数据
	ChristmasBossData& GetBossData(DWORD dwServerId, WORD wBossType);

	// 发送消息给战斗中的玩家
	void SendMsgToBattlePlayer(DWORD dwServerId, NetMsgBody *pMsg);

	// 圣诞节数据
	ChristmasData& GetChristmasData(DWORD dwServerId);

private:

	map<DWORD, ChristmasData> m_ChristmasDataList; // 圣诞节数据列表[server_id, data]

	// 正在挑战的玩家列表
	map<DWORD, map<DWORD, Player*>> m_BattlePlayerList;
};