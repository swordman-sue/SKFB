
/**\file
叛军管理器
\author Kevin
\date 2016/9/7
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "RebelDataDefine.h"

class Player;

typedef std::map<DWORD, RebelData> REBEL_MAP;
typedef std::map<DWORD, vector<DWORD>> REBEL_SHARE_MAP;
typedef std::map<DWORD, DWORD> REBEL_DISCOVER_MAP;

class CRebelManager : public Singleton<CRebelManager>
{
public:
	CRebelManager();
	~CRebelManager();

	void Init();

	void Update();

public:

	// 请求叛军数据 
	void OnRebelDataReq(Player *pPlayer);

	// 领取叛军功勋奖励
	void OnRebelAwardReq(Player *pPlayer, WORD *pAwardList, BYTE byAwardNum);

	// 请求功勋奖励记录
	void OnNetAwardRecordList(Player *pPlayer);

	// 分享叛军
	void OnNetRebelShare(Player *pPlayer);

	// 开始战斗
	bool OnBattleStart(Player *pPlayer, DWORD dwRebelId, WORD wAttackType);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, DWORD dwRebelId, WORD wAttackType, MonsterData *pMonsterList, BYTE byMonsterNum);

public:

	// 创建叛军
	DWORD CreateRebel(Player *pPlayer);

	// 删除叛军数据
	void RemoveRebelData(DWORD dwRebelId);

	// 添加叛军数据
	void AddRebelData(const RebelData &tRebelData);
	
	// 添加分享数据
	void AddShareData(DWORD dwShareTargetId, DWORD dwRebelId);

	// 删除分享数据
	void RemoveShareData(DWORD dwShareTargetId, DWORD dwRebelId);

	// 取自己发现的叛军ID
	DWORD GetSelfRebelId(DWORD dwRoleId);

	// 发送叛军数据
	void SendRebelData(Player *pPlayer);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);
protected:

	// 取叛军数据
	RebelData* GetRebelData(DWORD dwRebelId);

	// 取别人分享给自己的叛军列表
	vector<DWORD>* GetShareRebelList(DWORD dwRoleId);

	// 取叛军关卡怪物列表(内部使用的接口)
	//bool GetRebelMonsterList(WORD wRebelLevel, vector<MonsterData>* pMonsterList);

	// 是否消耗减半活动
	bool IsCostHalveActivity();

	// 是否奖励翻倍活动
	bool IsDoubleAwardActivity();

	// 更新叛军伤害
	void UpdateRebelDamage(DWORD dwRebelId, MonsterData *pMonsterList, BYTE byMonsterNum);

private:
	// 叛军列表
	REBEL_MAP m_RebelList;

	// 玩家收到的叛军分享列表
	REBEL_SHARE_MAP m_RebelShareList;

	// 玩家发现的叛军列表
	REBEL_DISCOVER_MAP m_RebelDiscoverList;

	// 接收叛军分享上限
	int m_nRebelShareMax;

	// 系列ID
	DWORD m_dwSeriesId;

	UINT64 m_lProcessTime;
};