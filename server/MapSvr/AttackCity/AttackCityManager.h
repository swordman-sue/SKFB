/**\file
攻城管理器
\author Kevin
\date 2017/07/03
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;

// 服务器攻城数据
struct ServerAttackCityData
{
	DWORD dwServerId;				// 服务器ID
	DWORD dwRefreshMonsterTime;		// 最近刷怪时间
	WORD wFromChapter;				// 本次活动开始的章节
	DWORD dwNO1RoleId;				// 第一名玩家Id
	DWORD dwKillBossTime;			// 击杀BOSS时间

	map<DWORD, TreasureBox> mTreasureBoxList; // 宝箱列表
	map<WORD, AttackCityChapter> mWaveDataList; // 波数据列表
	map<DWORD, Player*> mPlayerList;

	map<DWORD, Player*> mBattlePlayerList; // 正在战斗的玩家列表(BOSS关)
};

class CAttackCityManager : public Singleton<CAttackCityManager>
{
public:
	CAttackCityManager();
	~CAttackCityManager();

	// 初始化
	void Init();

	// 开始
	void Start();

	void Update();

	// 登出
	void OnLogout(Player *pPlayer);

	// 战斗开始
	bool OnBattleStart(Player *pPlayer, DWORD dwTargetId);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum, DWORD dwHPRemainRate);

	// 击杀当前所有怪物(测试使用)
	void OnKillCurAllMonster(DWORD dwServerId);

////// 网络消息
public:
	// 攻城数据请求
	void OnNetAttackCityData(Player *pPlayer);

	// 进入攻城请求
	void OnNetAttackCityEnter(Player *pPlayer);

	// 离开攻城请求
	void OnNetAttackCityLeave(Player *pPlayer);

	// 行走请求
	void OnNetGoto(Player *pPlayer, MapPos tSrcPos, MapPos tDestPos);

	// 领取Boss奖励
	void OnNetTakeBossReward(Player *pPlayer, WORD wChapter);

	// 鼓舞
	void OnNetEncourage(Player *pPlayer);

	// 宝箱锁定
	void OnNetTreasureBoxLock(Player *pPlayer, DWORD dwTreasureBoxUId);

	// 宝箱拾取
	void OnNetTreasureBoxPickUp(Player *pPlayer, DWORD dwTreasureBoxUId);

	// 复活请求
	void OnNetRelive(Player *pPlayer, BYTE byIsFreeRelive);

public:
	// 是否在城里
	bool IsInCity(DWORD dwServerId, DWORD dwRoleId);

	// 广播消息
	void SendToAll(NetMsgBody *pMsg, DWORD dwServerId);

	// 随机地图坐标点
	bool RandMapPos(MapPos &tMapPos,WORD wKey);

	// 创建怪物(todo::暂时没使用到)
	void CreateMonster();

	// 创建怪物
	void CreateMonster(DWORD dwServerId);

	// 删除怪物
	void RemoveMonster(DWORD dwServerId, WORD wChapter, DWORD dwMonsterUId);

	// 创建宝箱
	void CreateTreasureBox();

	// 创建宝箱
	void CreateTreasureBox(DWORD dwServerId);

	// 创建Boss宝箱
	void CreateBossBox(DWORD dwServerId, WORD wChapter);

	// 当前怪物是否已全部死亡
	bool IsAllMonsterDead(DWORD dwServerId);

	// 是否怪物已死亡
	bool IsMonsterDead(const AttackCityMap &tMap);

	DWORD GetNewSeriesId() { return ++m_dwSeriesId; };

	// 发送怪物列表
	void SendMonsterList(Player *pPlayer,WORD wChapter = 0, bool bIsToAll = false);

	// 发送宝箱列表
	void SendTreasureBoxList(Player *pPlayer);

	// 发送玩家攻城数据
	void SendPlayerAttackCityData(Player *pPlayer);

	// 发送高排名玩家列表
	void SendTopRankList(Player *pPlayer, bool bIsToAll = false);

	// 给所有人都同步排名
	void SynRankToAll(DWORD dwServerId);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);
protected:
	// 取玩家列表
	map<DWORD, Player*>& GetPlayerList(DWORD dwServerId);

	// 取章节列表
	map<WORD, AttackCityChapter>& GetAttackCityChapterList(DWORD dwServerId);

	// 取攻城数据
	ServerAttackCityData& GetAttackCityData(DWORD dwServerId);

	// 取关卡数据
	AttackCityMap* GetAttackCityMap(DWORD dwServerId, DWORD dwMapUId);

	// 取宝箱
	TreasureBox* GetTreasureBox(DWORD dwServerId, DWORD dwUId);

	// 取玩家攻城数据
	PlayerAttackCityData& GetPlayerAttackCityData(DWORD dwRoleId);

	// 添加宝箱
	bool AddTreasureBox(DWORD dwServerId, const TreasureBox &tBox);

	// 删除宝箱
	void RemoveTreasureBox(DWORD dwServerId, DWORD dwUId);

	// 是否在锁住宝箱中
	bool IsLockBox(DWORD dwRoleId);

	// 添加锁住宝箱的玩家
	void AddLockBoxPlayer(DWORD dwRoleId);

	// 删除锁住宝箱的玩家
	void RemoveLockBoxPlayer(DWORD dwRoleId);

	// 更新怪物伤害
	void UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList,  MonsterData *pMonsterList, BYTE byMonsterNum);

	// 是否怪物死亡
	bool IsMonsterDead(vector<MonsterData> &vMonsterList);

	// 处理离开攻城
	void DoLeaveAttackCity(Player *pPlayer);

	// 处理怪物死亡
	bool OnAllMonsterDead(DWORD dwServerId, WORD wChapter, Player *pKiller);

	// 计算积分
	DWORD CalculateScore(DWORD dwBattleScore, DWORD dwKillScore, WORD wEncourageTimes, bool bIsKiller);

	// 删除正在挑战BOSS的玩家
	void RemoveBattleBossPlayer(DWORD dwServerId, DWORD dwRoleId);

	// 添加挑战BOSS的玩家
	void AddBattleBossPlayer(Player *pPlayer);
protected:
	// 活动开始
	void OnActivityStart();

	// 活动中
	void OnActivityIng();

	// 活动结束
	void OnActivityOver();

	// 发奖励
	void OnReward();
private:

	bool m_bStart;

	// 服务器攻城数据列表[server_id,data]
	map<DWORD, ServerAttackCityData> m_SvrAttackCityDataList;

	// 正在锁住箱子的玩家列表
	map<DWORD, DWORD> m_LockBoxPlayerList;

	// 玩家攻城数据列表
	map<DWORD, PlayerAttackCityData> m_PlayerAttackCityDataList;

	DWORD m_dwSeriesId; // 系列ID

	DWORD m_dwRefreshTreasureBoxCD; // 刷宝箱CD(秒)
	DWORD m_dwProcessRefreshBoxTimer; 

	DWORD m_dwRefreshMonsterCD; // 刷怪CD(秒)
	//DWORD m_dwProcessMonsterTimer;

	BYTE m_byActivityStatus; // 活动状态(空闲,活动中,等待奖励)
	DWORD m_dwActivityOverTime; // 活动结束时间
};