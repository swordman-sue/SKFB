#pragma once

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 角色排名数据
struct RoleRankData
{
	DWORD dwRoleId;							// ID
	DWORD dwServerId;						// 服务器ID
	WORD wLevel;							// 等级
	WORD wRank;								// 排名
	DWORD dwValue;							// 排名的值
	//DWORD dwValueEx;						// 扩展排名值
	DWORD dwParam1;							// 参数1	
	DWORD dwParam2;							// 参数2	
	DWORD dwUpdateTime;						// 更新时间
};

// 工会排名数据
struct GuildRankData
{
	DWORD dwGuildId;	// 工会ID
	WORD wRank;			// 排名
	DWORD dwValue;		// 排名值
	DWORD dwUpdateTime;	// 更新时间
};

typedef vector<RoleRankData> RoleRankList;
typedef map<WORD, RoleRankList> AllRoleRankList; // serverid, ranklist

typedef vector<GuildRankData> GuildRankList;
typedef map<WORD, GuildRankList> AllGuildRankList; // serverid, ranklist

#pragma pack(pop)
