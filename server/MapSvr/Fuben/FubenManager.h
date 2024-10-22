/**\file
副本管理器
\author Kevin
\date 2016/7/12
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CFubenManager;
struct Map_Config;

// 副本
class CFBBase
{
public:
	CFBBase();
	~CFBBase();

	void Init(Player *pPlayer, CFubenManager *pFBMgr);

protected:
	CFubenManager *m_pFBManager;
	Player *m_pPlayer;
};

// 主线副本
class CMainFB : public CFBBase
{
public:
	CMainFB();
	~CMainFB();

	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// 开始战斗
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx, DWORD dwHelpBattleTargetId);

	// 战斗结束
	bool OnBattleOver(BYTE byIsWin, BYTE byStar);

	// 重置副本挑战次数
	bool OnResetBattleTimes(WORD wChapter, BYTE byMapIdx);

	// 领取副本奖励
	bool OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx);

	// 请求副本数据
	void OnNetFBData();

	// 发送副本数据
	void SendFBData();

	// 是否有红点
	bool IsHaveRedPoint();

	// 扫荡
	void OnNetClean(WORD wChapter, BYTE byMapIdx);

	// 一键领取奖励(关卡+星数)
	bool OnNetOneKeyTakeReward(WORD wChapter);

	// 领取章节奖励
	void OnNetTakeChapterReward(WORD wRewardLevel);

public:
	// 取总星数
	WORD GetTotalStar();

protected:
	// 取关卡数据
	MapData* GetMapData(WORD wChapter, BYTE byMapIdx);

	// 取章节数据
	ChapterData* GetChapterData(WORD wChapter);

	// 取下一个挑战的章节
	WORD GetNextChapter();

	// 取下一个挑战的关卡索引
	BYTE GetNextMapIndex();



private:
	MainFBData *m_pFBData;	// 主线副本数据
};


// 精英副本
class CEliteFB : public CFBBase
{
public:
	CEliteFB();
	~CEliteFB();

	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// 开始战斗
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx);

	// 战斗结束
	bool OnBattleOver(BYTE byIsWin, BYTE byStar, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 重置副本挑战次数
	bool OnResetBattleTimes(WORD wChapter, BYTE byMapIdx);

	// 领取副本奖励
	bool OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx);

	// 请求副本数据
	void OnNetFBData();

	// 创建外敌
	WORD CreateEnemy();

	// 发送副本数据
	void SendFBData();

	// 登录
	void Login();

	// 是否有红点
	bool IsHaveRedPoint();

	// 一键领取奖励(关卡+星数)
	bool OnNetOneKeyTakeReward(WORD wChapter);

protected:

	// 创建外敌
	void CreateEnemy(WORD wChapter);

	// 取当前外敌数量
	BYTE GetEnemyNum();

	// 取关卡数据
	MapData* GetMapData(WORD wChapter, BYTE byMapIdx);

	// 取章节数据
	EliteFBChapter* GetChapterData(WORD wChapter);

	// 取下一个挑战的章节
	WORD GetNextChapter();

	// 取下一个挑战的关卡索引
	BYTE GetNextMapIndex();

	// 取总星数
	WORD GetTotalStar();

	// 是否外敌挂掉(todo::暂时已不使用)
	bool IsEnemyDead(WORD wChapter);

	// 更新怪物伤害(todo::暂时已不使用)
	void UpateMonsterDamage(WORD wChapter, MonsterData *pMonsterList, BYTE byMonsterNum);
private:
	EliteFBData *m_pFBData;	// 精英副本数据

	WORD m_wNewEnemyNum;
};

// 英雄副本
class CHeroFB : public CFBBase
{
public:
	CHeroFB();
	~CHeroFB();
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// 开始战斗
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx);

	// 战斗结束
	bool OnBattleOver(BYTE byIsWin, BYTE byStar);

	// 领取副本奖励
	bool OnTakeReward(BYTE byRewardType, WORD wChapter, BYTE byMapIdx);

	// 请求英雄副本数据
	void OnHeroFBData();

	// 取下一个挑战的章节
	WORD GetNextChapter();

	// 取下一个挑战的关卡索引
	BYTE GetNextMapIndex();

	// 是否有红点
	bool IsHaveRedPoint();

	// 发送副本数据
	void SendFBData();

protected:

	// 取章节数据
	HeroFubenChapterData* GetChapterData(WORD wChapter);

private:
	HeroFBData *m_pFBData;	// 英雄副本数据
};

// 日常副本
class CDailyFB : public CFBBase
{
public:
	CDailyFB();
	~CDailyFB();
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// 开始战斗
	bool OnBattleStart(WORD wChapter, BYTE byMapIdx);

	// 战斗结束
	bool OnBattleOver(BYTE byIsWin, WORD wContext, BYTE byIsPass);

	// 请求日常副本数据
	void OnDailyDataReq();


public:

	// 发送日常副本数据
	void SendDailyData();

protected:

	// 取章节数据
	DailyChapterData* GetChapterData(WORD wChapter);

	// 添加章节数据
	void AddChapterData(const DailyChapterData &tChapter);

private:
	DailyFBData *m_pFBData;
};

// 爬塔副本
class CTowerFB : public CFBBase
{
public:
	CTowerFB();
	~CTowerFB();
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// 普通战斗开始
	bool OnNormalBattleStart(BYTE byType);

	// 普通战斗结束
	bool OnNormalBattleOver(BYTE byIsWin, bool bIsClean = false);

	// 精英战斗开始
	bool OnEliteBattleStart(WORD wMapLevel);

	// 精英战斗开始
	bool OnEliteBattleOver(BYTE byIsWin);

	// 购买挑战次数
	void OnNetBattleTimesBuy();

	// 重置副本
	bool OnNetFBReset(bool bIsGM);

	// 选择属性
	void OnNetSelectPro(BYTE byStar, BYTE byProIndex);

	// 购买星数商品
	void OnNetBuyStarGoods();

	// 请求爬塔普通副本数据
	void OnTowerFBData();

	// 一键扫荡
	void OnNetOneKeyClean(bool bIsToHistoryMap);

public:

	// 发送爬塔数据
	void SendTowerData();

	// 是否有红点
	bool IsHaveRedPoint();

	// 取历史排行
	WORD GetMaxStarRecord();

protected:
	// 取爬塔总星数
	WORD GetTotalStar();

	// 随机属性
	BYTE RandProIdx(BYTE byStar);
private:
	TowerFBData *m_pFBData;
};

// 叛军副本
class CRebelFB : public CFBBase
{
public:
	CRebelFB(){};
	~CRebelFB(){};
	void Init(Player *pPlayer, CFubenManager *pFBMgr);

public:
	// 开始战斗
	bool OnBattleStart(DWORD dwRebelId, WORD wType);

	// 战斗结束
	bool OnBattleOver(BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum);

protected:

private:
	PlayerRebelData *m_pFBData;
};

class CFubenManager
{
public:
	CFubenManager();
	~CFubenManager();

	void Init(Player *pPlayer);

	void OnNewDay();
public:
	// 开始战斗
	void OnBattleStart(BYTE byFBType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, DWORD dwPower = 0, const char *pSign = NULL);

	// 战斗结束
	void OnBattleOver(BYTE byFBType, int nWinValue, BYTE byStar, DWORD dwContext, DWORD dwContext1, MonsterData *pMonsterList, BYTE byMonsterNum,
		DWORD dwDamage, DWORD dwPower = 0,  const char *pSign = NULL);

	// 领取副本奖励
	void OnNetTakeReward(BYTE byAwardType, BYTE byFBType, WORD wChapter, BYTE byIndex);

	// 副本扫荡
	///void OnNetFBClean(BYTE byFBType, BYTE byTimes, DWORD dwParam1, DWORD dwParam2);

	// 扫荡
	void OnNetSweep(BYTE byFBType, WORD wChapter, BYTE byMapIdx);

	// 副本重置
	void OnNetFBReset(BYTE byFBType, DWORD dwParam1, DWORD dwParam2);

	// 一键领取
	void OnNetOneKeyTakeReward(BYTE byFBType, WORD wChapter);
public:

	// 添加掉落对象
	bool AddDropObject(DWORD dwMonsterId, DWORD dwDropObjectId, WORD wDropObjectNum);

	// 添加掉落对象给玩家
	bool AddDropOjectToPlayer(WORD wMapType);

	// 处理怪物掉落物品
	bool MonsterDropItem(WORD wMapId, WORD wMapType);

	// 取掉落物品列表
	vector<MonsterDropData>& GetDropItemList() { return m_DropItemList; };

	// 处理限时掉落
	void DoLimitDrop(DWORD dwFBType, vector<Item> &vRewardItemList);

	// 添加怪物数据
	void AddMonsterData(const MonsterData &tMonster);

	// 设置挑战目标数据 
	void SetBattleTargetData(const vector<ToBattleHeroData> &vTargetHeroList, const vector<SpiritData> &vTargetSpriteList);

	// 检测是否在战斗中
	bool CheckIsInBattle();

	// 设置战斗信息
	void SetBattleInfo(WORD wChapter, BYTE byMapLv, DWORD wMapId = 0, DWORD dwHelpBattleTargetId = 0, DWORD dwHelpBattleHeroInfoId = 0);

	// 设置战斗类型
	void SetBattleType(BYTE byType) { m_byFBType = byType; };
	BYTE GetBattleType() { return m_byFBType; };

	// 取开始战斗时间
	UINT64 GetStartBattleTime() { return m_lStartBattleTime; };

	// 重置战斗信息
	void ResetBattleInfo();

	// 取当前挑战的章节
	WORD GetCurChapter(){ return m_wCurChapter; };

	// 取当前挑战的关卡索引
	BYTE GetMapIdx() { return m_byCurMapIdx; };

	// 取当前挑战的关卡ID
	DWORD GetMapId() { return m_dwMapId; };

	void SetMapId(DWORD dwMapId) { m_dwMapId = dwMapId; };

	// 取助战目标ID
	DWORD GetHelpBattleTargetId() { return m_dwHelpBattleTargetId; };
	DWORD GetHelpBattleHeroInfoId() { return m_dwHelpBattleHeroInfoId; };

	void SetTargetId(DWORD dwTargetId) { m_dwTargetId = dwTargetId; };
	DWORD GetTargetId() { return m_dwTargetId; };

	void SetTargetServerId(DWORD dwTargetServerId) { m_dwTargetServerId = dwTargetServerId; };
	DWORD GetTargetServerId() { return m_dwTargetServerId; };

	void SetTargetFightingPower(DWORD dwFightingPower) { m_dwTargetFightingPower = dwFightingPower; };
	DWORD GetTargetFightingPower() { return m_dwTargetFightingPower; };

	void SetResult(BYTE byResult) { m_byResult = byResult; };

	// 设置是否首次获胜
	void SetIsFirstWin(BYTE byIsFirstWin){ m_byIsFirstWin = byIsFirstWin; };

	// 设置是否已通关
	void SetIsPassed(BYTE byIsPassed) { m_byIsPassed = byIsPassed; };

	// 设置是否我击杀
	void SetIsMeKill(BYTE byIsMeKill) { m_byIsMeKill = byIsMeKill; };

	void SetParam(BYTE byIndex, DWORD dwParam);
	DWORD GetParam(BYTE byIndex);

	// 处理获胜奖励
	void DoWinReward(int nMapType, const Map_Config *pMapCfg);

	// 添加助阵英雄属性
	void AddHelpBattleHeroPro(const Property &tPro);
public:
	CTowerFB& GetTower() { return m_TowerFB; }; // 取爬塔副本逻辑类
	CDailyFB& GetDaily() { return m_DailyFB; }; // 取日常副本逻辑类
	CHeroFB& GetHeroFB() { return m_HeroFB; }; // 英雄副本逻辑类
	CEliteFB& GetEliteFB() { return m_EliteFB; }; // 精英副本逻辑类
	CMainFB& GetMainFB() { return m_MainFB; };

private:
	Player *m_pPlayer;
	PlayerFubenInfo *m_pFubenInfo;

	UINT64 m_lStartBattleTime;	// 开始战斗时间
	BYTE m_byFBType;			// 当前挑战的副本类型
	WORD m_wCurChapter;			// 当前挑战的章节
	BYTE m_byCurMapIdx;			// 当前挑战的关卡索引
	DWORD m_dwMapId;			// 当前关卡ID
	DWORD m_dwTargetId;			// 目标ID
	WORD m_dwTargetServerId;	// 目标所在的服务器ID
	DWORD m_arParamList[MAX_BATTLE_PARAM_NUM];	// 参数列表
	BYTE m_byResult;			// 结果
	BYTE m_byIsFirstWin;		// 是否第一次获胜(关卡)
	BYTE m_byIsPassed;			// 是否已通关(目前已无效)
	BYTE m_byIsMeKill;			// 是否我击杀
	DWORD m_dwTargetFightingPower;	// 目标战力
	DWORD m_dwHelpBattleTargetId;	// 助战目标ID
	DWORD m_dwHelpBattleHeroInfoId;	// 助战英雄信息ID

	vector<MonsterDropData> m_DropItemList; // 掉落列表
	vector<MonsterData> m_MonsterList;		// 怪物列表
	vector<ToBattleHeroData> m_TargetHeroList;	// 目标上阵英雄列表
	vector<SpiritData> m_vTargetSpriteList;		// 目标精灵列表
	vector<Property> m_vHelpBattleHeroProList; // 助阵英雄属性列表

	// 首胜奖励货币列表 m_vFirstWinMoneyList
	// 首胜奖励物品列表 m_vFirstWinItemList;
	// 普通奖励货币钱表 m_vNormalMoneyList;
	// 普通奖励物品列表 m_vNormalItemList;
	// 掉落奖励物品列表 m_vDropItemList;
	// 宝箱奖励物品列表 m_vBoxItemList;

	CMainFB		m_MainFB;
	CHeroFB		m_HeroFB;
	CDailyFB	m_DailyFB;
	CTowerFB	m_TowerFB;
	CRebelFB	m_RebelFB;
	CEliteFB	m_EliteFB;
};