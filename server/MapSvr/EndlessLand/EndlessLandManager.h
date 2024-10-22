/**\file
无尽之地管理器
\author Kevin
\date 2017/07/14
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CEndlessLandManager
{
public:
	CEndlessLandManager();
	~CEndlessLandManager();

	void Init(Player *pPlayer);

	////// 网络消息
public:
	// 无心之地数据请求
	void OnNetEndlessData();

	// 进入战场
	void OnNetEnter();

	// 开始战斗
	bool OnNetBattleStart();

	// 开始结束
	bool OnNetBattleOver(BYTE byIsWin, WORD wWave);

	// 杀怪
	void OnNetKillMonster(BYTE byIndex, DWORD dwMonsterId);

	// 挂机碰撞
	void OnNetCollision();

	// 领取连杀奖励
	void OnNetTakeKeepKillReward(WORD wKeepKillNum);

public:

	// 发送无尽之地数据
	void SendEndlessLandData();

	// 取最佳连杀记录
	DWORD GetContinueKillNumRecord();

	// 检测是否已领取过连杀奖励
	//bool IsTakeKeepKillReward(WORD wKeepKillNum);

private:
	Player *m_pPlayer;
	bool m_bIsInBattle; // 是否在战斗中

	PlayerEndlessLandData *m_pEndlessLandData;
};