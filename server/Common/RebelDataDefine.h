#ifndef _REBEL_DATA_DEFINE_H_
#define _REBEL_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// 叛军数据 
struct RebelData
{
	DWORD dwRebelId;										// 叛军ID
	WORD wLevel;											// 叛军等级
	BYTE byQuality;											// 叛军品质
	WORD wMapId;											// 关卡ID
// 	MonsterData arMonsterDamageList[MAX_BATTLE_MONSTER_NUM];// 怪物伤害列表
// 	BYTE byMonsterNum;										// 怪物数量
	vector<MonsterData> vMonsterList;						// 怪物列表
	DWORD dwRoleId;											// 发现者ID
	char szRoleName[MAX_ROLE_LEN];							// 发现者名称
	DWORD dwCreateTime;										// 创建时间
	DWORD dwLiveTime;										// 存活时间
	//DWORD dwTotalBlood;									// 总的血量
	//bool isDead;											// 是否已死亡
	BYTE byIsRemove;										// 是否已删除
	vector<DWORD> vSharePlayerList;							// 已经分享的玩家列表

	RebelData()
	{
		//isDead = false;
		byIsRemove = false;
		dwRebelId = 0;
		wLevel = 0;
		byQuality = 0;
		dwRoleId = 0;
		dwCreateTime = 0;
		dwLiveTime = 0;
		vMonsterList.clear();
		memset(szRoleName, 0, sizeof(szRoleName));
		vSharePlayerList.clear();
	}

	bool IsDead()
	{
		for (size_t i = 0; i < vMonsterList.size() /*MAX_BATTLE_MONSTER_NUM*/; ++i)
		{
			if (vMonsterList[i].dwDamage < vMonsterList[i].dwTotalHP)
				return false;
		}

		return true;
	}
};

struct ClientRebelData
{
	DWORD dwRebelId;
	DWORD dwDiscovererRoleId;				// 发现者ID
	char szDiscovererName[MAX_ROLE_LEN];	// 发现者名称
	DWORD dwTotalDamage;					// 当前总伤害
	WORD wRebelLevel;						// 叛军等级
	WORD wRebelMapId;						// 叛军关卡ID
	DWORD dwCreateTime;						// 创建时间
	BYTE byQuality;							// 叛军品质

	void Copy(const RebelData &tRebelData)
	{
		dwRebelId = tRebelData.dwRebelId;
		dwDiscovererRoleId = tRebelData.dwRoleId;
		wRebelLevel = tRebelData.wLevel;
		wRebelMapId = tRebelData.wMapId;
		dwCreateTime = tRebelData.dwCreateTime;
		byQuality = tRebelData.byQuality;
		strcpy_s(szDiscovererName, sizeof(szDiscovererName)-1, tRebelData.szRoleName);
		dwTotalDamage = 0;
		for (size_t i = 0; i < tRebelData.vMonsterList.size(); ++i)
		{
			dwTotalDamage += tRebelData.vMonsterList[i].dwDamage;
		}
	}
};

struct ClientPlayerRebelData
{
	DWORD dwTodayExploit;									// 今日累积功勋
	DWORD dwTodayDamage;									// 今日累积伤害
	WORD wExploitRank;										// 功勋排名
	WORD wDamageRank;										// 伤害排名
	BYTE byIsAlreadyShare;									// 是否已经分享
	WORD wInitRoleLevel;									// 角色每日初始等级
	ClientRebelData arRebelList[MAX_REBEL_SHARE_NUM];		// 叛军列表(叛军ID,发现者,当前HP)
	BYTE byRebelListNum;									// 叛军列表数量

	void AddRebelData(const ClientRebelData &tRebelData)
	{
		if (byRebelListNum >= MAX_REBEL_SHARE_NUM)
			return;

		arRebelList[byRebelListNum++] = tRebelData;
	}
};

// 玩家叛军数据
struct PlayerRebelData
{
	WORD wRebelLevel;								// 叛军等级
	DWORD dwTodayExploit;							// 今日累积功勋
	DWORD dwTodayDamage;							// 今日累积伤害
	WORD arAwardRecordList[MAX_EXPLOIT_AWARD_NUM];	// 奖励领取记录列表
	WORD wAwardRecordNum;							// 奖励记录数量
	WORD wIsAlreadyShare;							// 是否已经分享过

	// 是否在奖励记录列表里
	bool IsInAwardRecordList(WORD wAwardId) const
	{
		for (BYTE i = 0; i < wAwardRecordNum; ++i)
		{
			if (arAwardRecordList[i] == wAwardId)
				return true;
		}

		return false;
	}

	bool AddAwardRecord(WORD wAwardId)
	{
		if (wAwardRecordNum >= MAX_EXPLOIT_AWARD_NUM)
			return false;
		arAwardRecordList[wAwardRecordNum++] = wAwardId;
		return true;
	}
};


#pragma pack(pop)
#endif
