/**\file
PVP管理器
\author Kevin
\date 2017/6/19
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include <deque>
class Player;


class CPVPManager : public Singleton<CPVPManager>
{
public:
	CPVPManager();
	~CPVPManager();

	void Init();
	void Release();

	void Update();

	// 登出
	void OnLogout(Player *pPlayer);

public:
	// 请求玩家PVP数据
	void OnNetPlayerPVPData(Player *pPlayer);

	// 查找目标
	void OnNetSearchTarget(Player *pPlayer, DWORD dwSpiritSkillId);

	// 取消战斗
	void OnNetCancelSearchTarget(Player *pPlayer);

	// 准备完成
	void OnNetReadyFinish(Player *pPlayer);

	// 使用技能
	void OnNetUseSkill(Player *pPlayer, DWORD dwObjId, DWORD dwSkillId, WORD wX, WORD wY);

	// 血量变化
	void OnNetHPChange(Player *pPlayer, DWORD dwSrcObjId, DWORD dwTgtObjId, int nChangeHP, int nDamageType, DWORD dwPower, const char *pSign);

	// 创建buff
	void OnNetCreateBuff(Player *pPlayer, DWORD dwBuffId, DWORD dwSrcObjId, DWORD dwTgtObjId, BYTE byIsFromSpriteBless);
	
	// 战斗结果(只有打机器人才会发生) 时间到的平局由服务器来判定
	void OnNetBattleResult(Player *pPlayer, BYTE byResult);

	// 领取挑战奖励
	void OnNetTakeBattleReward(Player *pPlayer, BYTE byType, BYTE *pTimesList, WORD wListNum);

public:
	// 准备进入战斗(玩家)
	bool ReadyToBattleWithPlayer(DWORD dwRoleId, DWORD dwTargetId, DWORD dwSpiritSkillId);

	// 准备进入战斗(机器人)
	bool ReadyToBattleWithRobot(DWORD dwRoleId);

	// 取战场里扮演的类型
	BYTE GetBattlefieldActType(DWORD dwRoleId);

	// 取战场里扮演的类型
	BYTE GetBattlefieldActType(const PVPBattlefieldData *pBattleData, DWORD dwRoleId);

	// 英雄是否已死亡
	bool IsHeroDead(const PVPBattlefieldData *pBattleData, DWORD dwRoleId, DWORD dwHeroInfoId);

	// 是否所有的英雄都挂掉
	bool IsAllHeroDead(const PVPBattlefieldData *pBattleData, BYTE byActType);

	// 通过战力筛选目标
	DWORD SelectTargetByFightingPower(DWORD dwFightingPower);

	// 删除等待挑战的角色数据
	void RemoveWaitBattleRoleData(DWORD dwRoleId);

	// 更新英雄HP
	int UpateHeroHP(PVPBattlefieldData *pBattleData, BYTE byActType, DWORD dwHeroInfoId, int nChangeHP);

	// 取战斗目标ID
	DWORD GetTargetId(const PVPBattlefieldData *pBattleData, DWORD dwRoleId);

	// 删除战场数据
	void RemoveBattlefieldData(DWORD dwRoleId);

	// 是否活动开放
	bool IsActivityOpen();

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// 处理排行奖励
	void DoRankAward();

	// 处理排行称号
	void DoRankTitle();

	// 修改积分(GM使用)
	void ModifyScoreForGM(Player *pPlayer, DWORD dwScore, WORD wWeekWinTimes);

protected:
	// 查找等待挑战的角色数据
	PVPWaitBattleRoleData* FindWaitBattleRoleData(DWORD dwRoleId);

	// 查找战场数据
	PVPBattlefieldData* FindBattlefieldData(DWORD dwRoleId);

	// 取角色战斗数据
	PVPRoleBattleData* GetRoleBattleData(PVPBattlefieldData *pBattleData, DWORD dwRoleId);

	// 取角色战斗数据
	PVPRoleBattleData* GetRoleBattleData(DWORD dwRoleId);

	// 取角色战斗数据
	PVPRoleBattleData* GetRoleBattleData(PVPBattlefieldData *pBattleData, BYTE byActType);

	// 添加战场数据
	void AddBattlefieldData(const PVPBattlefieldData &tBattleData);

	// 计算获胜积分
	DWORD CalculateWinScore(DWORD dwTargetFightingPower);

	// 计算失败积分
	DWORD CalculateLoseScore(DWORD dwWinScore);

	// 取平局积分
	DWORD CalculateDrawScore(DWORD dwWinScore);

	// 计算积分
	DWORD CalculateScore(BYTE byResult, DWORD dwTargetFightingPower);

	// 发送玩家PVP数据
	void SendPlayerPVPData(Player *pPlayer);

	// 处理战斗开始
	void DoBattleStart(Player *pPlayer, Player *pTarget);

	void DoBattleOverWithPlayer(Player *pSrc, Player *pTgt, DWORD dwWinnerId);

	// 处理战斗结束(机器人)
	void DoBattleOverWithRobot(Player *pPlayer, DWORD dwTargetFightinPower, BYTE byResult);

	// 添加积分
	void AddScore(Player *pPlayer, DWORD dwScore, BYTE byWinner = false);

	// 取排名
	WORD GetRank(DWORD dwRoleId);
private:

	// PVP等待挑战的角色数据列表
	deque<PVPWaitBattleRoleData> m_WaitBattleRoleDataList;

	// 战场列表
	vector<PVPBattlefieldData> m_BattlefieldList;

	// 等级挑战时间上限
	DWORD m_dwWaitBattleTimeMax;

	// 一场战斗的限制时间
	DWORD m_dwBattleLimitTime;

	// 排名奖励计时器
	CDateTimers m_RankRewardTimer;

	// 排名称号计时器
	CDateTimers m_RankTitleTimer;
};