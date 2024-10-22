/**\file
资源战管理器
\author Kevin
\date 2018/08/02
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
class Player;


class CResourceWarManager : public Singleton<CResourceWarManager>
{
public:
	CResourceWarManager();
	~CResourceWarManager();

	void Init();

	void Update();

	void OnLogin(Player *pPlayer);

	void OnLogout(Player *pPlayer);

	// 存储数据到数据库
	void SaveDataToDB();

	// 加载数据
	void LoadDataFromDB(const char *pData, int nSize);

	// 加载数据请求
	void LoadDataRequest();

public:

	// 活动开始
	bool DoActivityStart();

public:

	// 请求资源战数据
	void OnNetResourceWardData(Player *pPlayer);

	// 请求战报
	void OnNetBattleRecord(Player *pPlayer);

	// 请求敌人列表
	void OnNetEnemyList(Player *pPlayer);

	// 请求章节数据
	void OnNetChapterData(Player *pPlayer, WORD wChapter);

	// 请求同类资源数据
	void OnNetSameResourceList(Player *pPlayer, WORD wResourceType);

	// 请求军团资源
	void OnNetGuildResourceList(Player *pPlayer);
	
	// 升级科技
	void OnNetUpgradeTechnology(Player *pPlayer);
	
	// 延长占领
	void OnNetKeepOccupy(Player *pPlayer);

	// 放弃占领
	void OnNetGiveUpOccupy(Player *pPlayer);

public:

	// 战斗开始
	bool OnNetBattleStart(Player *pPlayer, WORD wChapter, WORD wResourceId);

	// 战斗结束
	bool OnNetBattleOver(Player *pPlayer, BYTE byIsWin);

public:
	// 处理占领时间结束
	void DoOccupyTimeOver(const ResourceWarResouceData *pResourceWarData);

	// 处理基础奖励
	void DoBaseReward(ResourceWarResouceData *pResourceWarData);

	// 处理额外奖励
	void DoExtraReward(ResourceWarResouceData *pResourceWarData);

	// 取额外奖励列表
	const vector<Item>* GetExtraRewardList(DWORD dwResourceUId, DWORD dwRewardIdx);

	// 添加占领时间(GM使用)
	void AddOccupyTimeForGM(Player *pPlayer, DWORD dwAddTime);

protected:

	// 更新占领资源时间
	void UpdateAccupyResourceTime(DWORD dwRoleId, DWORD dwAddTime);

	// 发放占领基础奖励
	void SendBaseReward(DWORD dwRoleId, WORD wChapter, WORD wResourceId);

	// 发送资源战数据
	void SendResourceWardData(Player *pPlayer);

	// 发送章节数据
	void SendChapterData(Player *pPlayer, WORD wChapter);

protected:

	// 是否在活动中
	bool IsActivity();

	// 添加战报
	void AddBattleRecord(const ResourceWardBattleRecord &tRecord);

	// 添加资源战数据
	void AddResourceWarData(const ResourceWarResouceData &tResourceWarData);

	// 移除资源战数据
	void RemoveResourceWarData(WORD wChapter, WORD wResourceId);

	// 添加敌人数据
	void AddEnemyData(DWORD dwRoleId, DWORD dwEnemyId);

	// 添加敌人数据
	void AddEnemyData(DWORD dwRoleId, const ResourceWarEnemy &tEnemy);

	// 取资源战数据
	ResourceWarResouceData* FindResourceWarData(WORD wChapter, WORD wResourceId);

	// 取资源战数据
	ResourceWarResouceData* FindResourceWarData(DWORD dwRoleId);

	// 取玩家敌人列表
	map<DWORD, ResourceWarEnemy>* GetEnemyList(DWORD dwRoleId);

	// 检测资源是否已被占领
	void IsResourceBeOccupy(WORD wChapter, WORD wResourceId);

	// 更新并检测资源是否被挑战
	bool UpdateAndCheckIsBeBattle(WORD wChapter, WORD wResourceId);

	// 添加被挑战的资源
	void AddBeBattleResource(WORD wChapter, WORD wResourceId);

	// 移除被挑战的资源
	void RemoveBeBattleResource(WORD wChapter, WORD wResourceId);

	// 添加在挑战中的玩家
	void AddInBattlePlayer(DWORD dwRoleId, WORD wChapter, WORD wResourceId);

	// 移除在挑战中的玩家
	void RemoveInBattlePlayer(DWORD dwRoleId);

	// 取正在被挑战的资源uid
	DWORD GetBeBattleResourceUId(DWORD dwRoleId);

	// 取玩家已经占领的资源uid
	DWORD GetRoleAccupyResourceUId(DWORD dwRoleId);

	// 添加查找玩家资源数据
	void AddFindRoleResourceData(DWORD dwRoleId, WORD wChapter, WORD wResourceId);

	// 移除查找玩家资源数据
	void RemoveFindRoleResourceData(DWORD dwRoleId);

	// 取玩家的基础奖励加成比例
	int GetBaseRewardAddRate(DWORD dwRoleId);

	// 取同一章节工会成员占领人数
	WORD GetGuildAccupyMemberNum(DWORD dwRoleId);

	// 取玩家资源战数据
	RoleResourceWarData& GetRoleResourceWarData(DWORD dwRoleId);

	// 添加玩家资源战数据
	void AddRoleResourceWarData(DWORD dwRoleId, const RoleResourceWarData &tData);

private:

	bool m_bLoadDataFromDB;
	DWORD m_dwLastProccessTimer;

	////////////////////////
	// 需要存储的数据

	// 资源列表
	map<DWORD, map<DWORD, ResourceWarResouceData>> m_ResourceList; // [chapter_id, resource_id, data]

	// 敌人列表
	map<DWORD, map<DWORD, ResourceWarEnemy>> m_EnemyList;	// [role_id, enemy_role_id, data]

	// 战报列表
	deque<ResourceWardBattleRecord> m_BattleRecordList;

	// 玩家资源战数据列表
	map<DWORD, RoleResourceWarData> m_RoleResourceWarDataList;
	

	////////////////////////
	// 临时数据

	// 玩家资源查找列表
	map<DWORD, DWORD> m_RoleResourceFindList;	// [role_id, chapter_id + resource_id]

	// 正在被挑战的资源列表
	map<DWORD, DWORD> m_BeBattleResourceList;	// [chapter_id + resource_id, star_battle_time]

	// 正在挑战中的玩家列表
	map<DWORD, DWORD> m_InBattlePlayerList;		// [role_id, chapter_id + resource_id]
};