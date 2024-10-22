/**\file
领地管理器
\author Kevin
\date 2016/9/24
\note
*/

#pragma once
#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "LandDataDefine.h"
class Player;

typedef map<DWORD, PlayerLandInfo> LandListMap;

class CLandManager : public Singleton<CLandManager>
{
public:
	CLandManager();
	~CLandManager();

	void Init();

	void Update();

	// 存储所有数据
	void SaveAllDataToDB();

	// 存储数据
	void SaveDataToDB(DWORD dwRoleId);

	// 加载数据
	void LoadDataFromDB(DWORD dwRoleId, const PlayerLandInfo &tLandInfo);

	// 加载数据请求
	void LoadDataRequest();

	// 打印巡逻奖励(测试使用)
	void PrintPatrolReward(Player *pPlayer, WORD wCityId);
public:
	// 请求玩家的领地数据
	void OnNetPlayerLandData(Player *pPlayer, DWORD dwTargetId);

	// 巡逻请求
	void OnNetPatrol(Player *pPlayer, DWORD dwHeroUID, WORD wCityId, BYTE byPatrolType, BYTE byTimeType);

	// 领取巡逻奖励
	void OnNetPatrolAward(Player *pPlayer, WORD wCityId);

	// 领地升级(开启,提升)
	void OnNetLandLevelUp(Player *pPlayer, WORD wCityId);

	// 请求好友领地列表
	void OnNetFriendLandList(Player *pPlayer);

	// 请求城池详情
	void OnNetCityDetails(Player *pPlayer, DWORD dwTargetId, WORD wCityId);

	// 镇压暴动
	void OnSuppressRiot(Player *pPlayer, DWORD dwFriendId, WORD wCityId);

public:
	// 战斗开始
	bool OnBattleStar(Player *pPlayer, WORD wCityId);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, WORD wCityId, BYTE byIsWin);

	// 发送玩家领地数据
	void SendPlayerLandData(Player *pPlayer, DWORD dwTargetId = 0);

	// 是否有红点
	bool IsHaveRedPoint(DWORD dwRoleId);

	// 取领地列表数量
	int GetLandListSize() { return m_PlayerLandList.size(); };

	// 取玩家领地数量
	WORD GetPlayerLandNum(DWORD dwRoleId);

	// 星临时接口(测试用)
	PlayerLandInfo* GetPlayerLandInfoEx(DWORD dwRoleId);
	void PatrolEvent_Test(DWORD dwRoleId, WORD wCityId);
protected:
	// 取玩家领地信息
	PlayerLandInfo* GetPlayerLandInfo(DWORD dwRoleId);

	// 取城池信息
	LandCityData* GetCityInfo(DWORD dwRoleId, WORD wCityId);

	// 添加领地信息
	void AddCityInfo(DWORD dwRoleId, const LandCityData &tCityData);

	// 巡逻事件(暂时不使用)
	//void PatrolEvent(DWORD dwRoleId, WORD wCityId);

	// 巡逻事件
	bool PatrolEventEx(DWORD dwRoleId, WORD wCityId);

	// 随机事件ID
	WORD RandEventId(WORD wCityId);

	// 取巡逻总时间
	DWORD GetPatrolTotalTime(BYTE byPatrolType, BYTE byTimeType);
private:
	LandListMap m_PlayerLandList;	// 玩家领地列表[roleid, data]

	UINT64 m_lProccessTime;
};
