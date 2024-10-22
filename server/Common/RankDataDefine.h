#pragma once

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// ��ɫ��������
struct RoleRankData
{
	DWORD dwRoleId;							// ID
	DWORD dwServerId;						// ������ID
	WORD wLevel;							// �ȼ�
	WORD wRank;								// ����
	DWORD dwValue;							// ������ֵ
	//DWORD dwValueEx;						// ��չ����ֵ
	DWORD dwParam1;							// ����1	
	DWORD dwParam2;							// ����2	
	DWORD dwUpdateTime;						// ����ʱ��
};

// ������������
struct GuildRankData
{
	DWORD dwGuildId;	// ����ID
	WORD wRank;			// ����
	DWORD dwValue;		// ����ֵ
	DWORD dwUpdateTime;	// ����ʱ��
};

typedef vector<RoleRankData> RoleRankList;
typedef map<WORD, RoleRankList> AllRoleRankList; // serverid, ranklist

typedef vector<GuildRankData> GuildRankList;
typedef map<WORD, GuildRankList> AllGuildRankList; // serverid, ranklist

#pragma pack(pop)
