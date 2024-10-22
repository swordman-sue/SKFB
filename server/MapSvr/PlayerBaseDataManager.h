/**\file
玩家基本数据管理器
\author Kevin
\date 2016/8/29
\note
*/

#pragma once

#include "DataDefine.h"
#include <map>
#include <hash_map>
#include "ToolKit\Singleton.h"

class Player;

typedef std::hash_map<DWORD, PlayerBaseData*>	PlayerBaseDataMap;
typedef std::hash_map<DWORD, PlayerBaseDataMap>		AllPlayerBaseDataMap;

typedef std::hash_map<string, PlayerBaseData*>	PlayerBaseDataNameMap;
typedef std::hash_map<DWORD, PlayerBaseDataNameMap>	AllPlayerBaseDataNameMap;

class PlayerBaseDataManager : public Singleton<PlayerBaseDataManager>
{
public:
	PlayerBaseDataManager();
	~PlayerBaseDataManager();

	// 初始化
	bool				Init();

	// 释放
	void				Release();

	// 玩家登录
	void				OnPlayerLogin(Player *pPlayer);

	// 玩家下线
	void				OnPlayerLogout(Player *pPlayer);

	// 添加服务器
	void				AddServer(DWORD dwServerId);

public:
	// 添加玩家基本数据
	void AddPlayerBaseData(DWORD dwInitServerId, const PlayerBaseData &tBaseData);

	// 移除离线数据(目前已不使用)
	void RemovePlayerBaseData(DWORD dwServerId, DWORD dwRoleId);

	// 取玩家列表
	const PlayerBaseDataMap* GetPlayerList(DWORD dwServerId);

	// 取玩基本数据(ID)
	const PlayerBaseData* GetPlayerBaseDataById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);

	// 取玩基本数据(名称)
	const PlayerBaseData* GetPlayerBaseDataByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate = true);

	// 取玩基本数据(名称)
	const PlayerBaseData* GetPlayerBaseDataByName(const char *pszRoleName);

	// 取玩基本数据(Id)
	const PlayerBaseData* GetPlayerBaseDataById(DWORD dwRoleId);

	// 取玩基本数据(account)
	const PlayerBaseData* GetPlayerBaseDataByAccount(DWORD dwInitServerId, DWORD dwChannelId, const char *pszAccount);

	// 取用户所在服务器
	DWORD GetServerId(DWORD dwRoleId);

	// 取角色所在的服务器ID
	DWORD GetServerIdByRoleId(DWORD dwRoleId);

	// 取角色初始所在的服务器ID
	DWORD GetInitServerIdByRoleId(DWORD dwRoleId);

	// 模块逻辑接口
public:
	// 添加好友赠送的体力数据
	void AddGiveEnergyData(DWORD dwServerId, DWORD dwRoleId, DWORD dwFriendId);

	// 添加申请好友数据
	void AddFriendApplyData(DWORD dwServerId, DWORD dwRoleId, DWORD dwTargetId);

	// 添加奖励
	void AddReward(DWORD dwServerId, DWORD dwRoleId, const RewardData &tReward);

	// 添加奖励
	//void AddReward(DWORD dwServerId, WORD wMinLevel, WORD wMaxLevel, const RewardData &tReward);

	// 删除奖励
	void RemoveReward(DWORD dwServerId, DWORD dwRoleId, DWORD dwRewardUId);

	// 添加好友数量
	void AddFriendNum(DWORD dwServerId, DWORD dwRoleId, BYTE byFriendNum = 1);

	// 修改角色名称
	VOID RoleNameModify(DWORD dwServerId, const char* pszOldName, const char* pszNewName);

	// 取用户总数量
	DWORD GetTotalPlayerNum();

	// 取玩家ID列表
	const vector<DWORD>* GetPlayerIdList(DWORD dwServerId);

	// 特殊通道接口(todo::不要滥用)
	PlayerBaseData* GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);

	// 设置战力
	void SetFightingPower(DWORD dwServerId, DWORD dwRoleId, DWORD dwFightingPower);

	// 是否有效帐号
	bool IsInvalidAccount(DWORD dwChannelId, const char *pszAccount);

	// 取某个帐号的角色ID列表
	const vector<DWORD>* GetRoleIdList(DWORD dwChannelId, const char *pszAccount);

protected:
	PlayerBaseData* GetPlayerBaseDataInnerById(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);
	PlayerBaseData* GetPlayerBaseDataInnerByName(DWORD dwServerId, const char *pszRoleName, bool bUpdate = true);

private:
	AllPlayerBaseDataMap m_AllPlayerBaseDataList;				// 所有玩家基本数据列表(RoleId, Data)
	AllPlayerBaseDataNameMap m_AllPlayerBaseDataNameList;		// 所有玩家基本数据列表(RoleName,Data)

	map<DWORD, vector<DWORD>> m_vPlayerIdList;	// 玩家ID列表(主要作为遍历使用)[server_id, role_id_list] 

	map<string, vector<DWORD>> m_mapPlayerAccountList;	// 帐号列表(主要作为遍历使用)[account, role_id_list]

	map<DWORD, DWORD> m_mServerIdList;		//  服务器ID列表[role_id, server_id]

	map<DWORD, DWORD> m_mInitServerIdList;	//  初始服务器ID列表[role_id, server_id] 创建角色时所在服务器

	DWORD m_dwSeriesId; // 系列ID
};

const PlayerBaseData* g_GetPlayerBaseData(DWORD dwServerId, DWORD dwRoleId, bool bUpdate = true);

const PlayerBaseData* g_GetPlayerBaseData(DWORD dwRoleId, bool bUpdate = true);