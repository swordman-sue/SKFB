#ifndef _ARENA_DATA_DEFINE_H_
#define _ARENA_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// ��������������
struct ArenaRankData
{
	DWORD dwTargetId;		// Ŀ��Id
	WORD wCurRank;			// ��ǰ����
	WORD wHistoryBestRank;	// ��ʷ�������
};

// ������Ŀ������
struct ArenaTargetData
{
	DWORD dwTargetId;					// Ŀ��Id
	WORD wHeadIcon;						// ͷ��ͼ��
	char szTargetName[MAX_ROLE_LEN];	// Ŀ������
	WORD wRank;							// ����
	DWORD dwFightingPower;				// ս��
	DWORD dwTitleId;					// �ƺ�ID
	DWORD dwNicknameId;					// �ǳ�ID
};

// ����ս��
struct ArenaBattleRecord
{
	DWORD dwTime;						// ս��ʱ��
	DWORD dwTargetId;					// Ŀ��ID
	char szTargetName[MAX_ROLE_LEN];	// Ŀ������
	WORD wTargetHeadIcon;				// Ŀ��ͷ��ͼ��
	BYTE byIsWin;						// �Ƿ��ʤ
	BYTE byIsAttack;					// �Ƿ񹥻�
	WORD wMyOldRank;					// �ҵ�սǰ����
	WORD wMyNewRank;					// �ҵ�ս������
	WORD wTargetLevel;					// Ŀ��ȼ�

	ArenaBattleRecord()
	{
		memset(this, 0, sizeof(ArenaBattleRecord));
	}
};

// ��Ҿ�����ս����¼
struct PlayerArenaBattleRecord
{
	DWORD dwRoleId;
	ArenaBattleRecord arBattleRecordList[MAX_ARENA_BATTLE_RECORD];
	WORD wBattleRecordNum;
};

#pragma pack(pop)
#endif
