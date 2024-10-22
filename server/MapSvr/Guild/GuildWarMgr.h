/**\file
工会战管理器
\author Kevin
\date 2018/9/4
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "Guild.h"
#include "ToolKit\Timer.h"
#include "DBCDefine.h"
class Player;



class CGuildWarMgr : public Singleton<CGuildWarMgr>
{
public:
	CGuildWarMgr();
	~CGuildWarMgr();

	void Init();
	void Release();

	void Update();

	void OnNewDay();

	// 请求加载数据
	void LoadDataRequest();

	// 存储数据到数据库
	void SaveDataToDB();

	// 加载数据
	void LoadDataFromDB(const char *pszData, int nSize);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// 占领星球
	void OnOccupyStarForGM(Player *pPlayer, WORD wStarId);

public:

	// 战斗开始(低级星球)
	bool OnNetLowStarBattleStart(Player *pPlayer, DWORD dwStarId);

	// 战斗结束(低级星球) 
	bool OnNetLowStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate);

	// 战斗开始(高级星球) 
	bool OnNetHighStarBattleStart(Player *pPlayer, DWORD dwStarId, WORD wTowerId, DWORD dwTargetId);

	// 战斗结束(高级星球)
	bool OnNetHighStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate);

public:

	// 请求工会战数据
	void OnNetGuildWarData(Player *pPlayer);

	// 请求本军团已占领的星球
	void OnNetGuildOccupyStarList(Player *pPlayer);

	// 请求本军团已宣战的星球列表
	void OnNetGuildDeclareWarStarList(Player *pPlayer);

	// 请求星球列表
	void OnNetStarList(Player *pPlayer);

	// 宣战
	void OnNetDeclareWar(Player *pPlayer, DWORD dwStarId);

	// 布防
	void OnNetOrganizeDefence(Player *pPlayer, WORD wStarId, WORD wTowerType, DWORD dwMemberId, BYTE byIsToBattle);

	// 撤离
	void OnNetEvacuate(Player *pPlayer, WORD wStarId);

	// 购买对高级星球挑战次数
	void OnNetBuyHighStarBattleTimes(Player *pPlayer);

	// 领取星球奖励(开采Exploit)
	void OnNetTakeStarReward(Player *pPlayer, WORD wTakeType, WORD *pStarList, WORD wStarNum);

	// 请求星球详细
	void OnNetStarDetail(Player *pPlayer, WORD wStarId);

	// 请求塔列表
	void OnNetTowerList(Player *pPlayer, DWORD dwStarId);

public:
	// 解散工会事件
	void OnDismissGuild(WORD wServerId, DWORD dwGuildId);

	// 退出工会事件
	void OnLeave(CGuild *pGuild, DWORD dwRoleId);

public:

	// 取工会已经宣战的数量
	WORD GetAlreadyDeclareWarNum(WORD wRegionId, DWORD dwGuildId);

	// 取占领高级星球的数量
	WORD GetOccupyHighStarNum(WORD wServerId, DWORD dwGuildId);

	// 取区域ID
	WORD GetRegionId(WORD wServerId);

	// 检测是否工会的星球
	bool IsGuildStar(DWORD dwGuildId, DWORD dwStarId);

	// 是否低级星球挑战时间
	bool IsLowStarBattleTime();

	// 是否低级星球奖励时间
	bool IsLowStarRewardTime();

	// 检测高级星球活动状态
	bool CheckHighStarActivityStatus(int nActivityStatus);

	// 检测塔是否已破
	bool IsTowerBreak(DWORD dwBelongGuildId, const StarTowerInfo *pTowerInfo);

	// 取破塔数量
	WORD GetBreakTowerNum(const StarInfo *pStarInfo);

	// 是否可攻击基地
	bool IsCanAttackBoss(const StarInfo *pStarInfo);

	// 是否低级星球
	bool IsLowStar(WORD wStarType);

	// 是否高级星球
	bool IsHighStar(WORD wStarType);

	// 是否正在防守中
	bool IsInDefense(const StarInfo *pStarInfo, DWORD dwRoleId);

	// 取塔类型
	WORD GetTowerType(WORD wTowerId);

	// 取星球塔当前防守人数
	WORD GetCurTowerDefenseNum(const StarInfo *pStarInfo, WORD wTowerType);

protected:

	// 添加工会积分
	void AddGuildScore(StarInfo &tStarInfo, DWORD dwGuildId, DWORD dwScore);

	// 取星球信息(如果目前没有则会创建)
	StarInfo& GetStarInfo(WORD wServerId, DWORD dwStarId);

	// 取星球信息(如果目前没有则会创建)
	StarInfo& GetStarInfoByRegionId(WORD wRegionId, DWORD dwStarId);

	// 查找星球信息
	StarInfo* GetStarInfoEx(WORD wServerId, DWORD dwStarId);

	// 取塔信息
	StarTowerInfo* GetTowerInfo(StarInfo *pStarInfo, WORD wTowerId);

	// 通过类型取塔信息
	StarTowerInfo* GetTowerInfoByType(StarInfo *pStarInfo, WORD wTowerType);

	// 取防守者信息
	StarDefenderInfo* GetDefenderInfo(StarTowerInfo *pTowerInfo, DWORD dwRoleId);

	// 取服务器星球列表
	map<DWORD, StarInfo>* GetStarList(WORD wAreaId);

	// 取工会占领的的星球列表
	const map<DWORD, DWORD>* GetGuildStarList(DWORD dwGuildId);

	// 重置星球信息
	void ResetStarInfo(StarInfo &tStarInfo, DWORD dwBelongGuildId, bool bIsNew);

	// 取玩家工会战数据
	RoleGuildWarData& GetRoleGuildWarData(DWORD dwRoleId);

	// 添加占领的星球
	void AddOccupyStar(DWORD dwGuildId, WORD wStarId);

	// 删除占领的星球
	void RemoveOccupyStar(DWORD dwGuildId, WORD wStarId);

	// 删除宣战的工会
	void RemoveDeclareWarGuild(WORD wRegionId, DWORD dwGuildId);

	// 添加星球数据
	void AddStarInfo(WORD wRegionId, const StarInfo &tStarInfo);

	// 移除防守者
	void RemoveDefender(StarInfo &tStarInfo, DWORD dwRoleId);

	// 星球数据更新通知
	//void StarUpdateNotify(DWORD dwGuildId, WORD wStarId, WORD wTowerId);

protected:
	// 破塔奖励
	void BreakTowerReward(WORD wStarId, StarTowerInfo *pTowerInfo, DWORD dwAttackGuildId);

	// 高级星球活动结束奖励
	void HighStarActivtiyOverReward(const StarInfo *pStarInfo, BYTE byIsAttackerWin, WORD wBreakTowerNum = 0);

	// 高级星球宣战时间结束(决定谁获得进攻权)
	void HighStarDeclareWarTimeOver();

	// 高级星球活动结束(决定谁获得星球权)
	void HighStarActivityOver();

	// 低级星球活动开始(重置星球数据)
	void LowStarActivityStart();

	// 低级星球活动结束(决定谁获得星球权)
	void LowStarActivityOver();


protected:
	// 发送工会军角色数据
	void SendGuildWarRoleData(Player *pPlayer);

	// 发送星球列表
	void SendStarList(Player *pPlayer);

	// 发送工会占领的星球列表
	void SendOccupyStarList(Player *pPlayer);

	// 发送工会宣战的星球列表
	void SendDeclareWarStarList(Player *pPlayer);

	// 配置
public:
	// 取通用配置
	const Server_Config* GetCommonCfg(DWORD dwKey, bool bOutLog = false);

	DWORD GetConfigValue(DWORD dwKey, DWORD dwDefault = 0);

private:

	bool m_bIsLoadDataFromDB;
	DWORD m_dwSaveTimer;

	// 高级星球宣战结束时间计时器(决定进攻者时间)
	vector<CDateTimers> m_vHighStarDeclareWarOverTimer;

	// 高级星球活动结束时间计时器(对战时间结束时间,决定获胜者时间)
	vector<CDateTimers> m_vHighStarActivityOverTimer;

	// 低级星球活动结束时间计时器(决定获胜者时间)
	CDateTimers m_LowStarActivityOverTimer;

	///// 需要存储数据库
	
	// 星球信息列表
	map<DWORD, map<DWORD, StarInfo>> m_StarInfoList; // [area_id, star_id, data]

	// 玩家工会战数据列表
	map<DWORD, RoleGuildWarData> m_RoleGuildWarDataList; //[RoleId, Data]

	// 玩家排行榜


	///// 不需要存储数据库

	// 工会星球列表
	map<DWORD, map<DWORD,DWORD>> m_GuildStarList;// [guild_id, star_list[star_id, star_id]]
};