#ifndef _KING_MATCH_DEFINE_H_
#define _KING_MATCH_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// �������������
struct PlayerKingMatchData
{
	WORD wLastSeason;		// ���һ������(�μӵ�)
	WORD wTotalBattleTimes;	// �ܵ���ս����
	WORD wTotalWinTimes;	// �ܵĻ�ʤ����
	WORD wTodayWinTimes;	// ���ջ�ʤ����
	WORD wStageLevel;		// ��λ�ȼ�
	DWORD dwContext;		// ������(����,����)
	WORD wHistoryStageLevel;// ��ʷ��λ�ȼ�

	// ������������¼
	WORD arSeasonTaskRewardRecord[MAX_SEASON_TASK_REWARD_RECORD];
	WORD wSeasonRewardRecordNum;
};

// ��������սĿ��
struct KingMatchBattleTarget
{
	DWORD dwRoleId;		// ��ɫID
	WORD wServerId;		// ������ID
	WORD wStageLevel;	// ��λ�ȼ�
	WORD wStar;			// ����
	WORD wBattleTimes;	// ��ս����
};

// ���������м�¼
struct KingMatchRankRecord
{
	WORD wSeason; // ����
	WORD wRoleRankNum; // ��ɫ��������
	PlayerCommonData arRoleRankList[MAX_KING_MATCH_TOP_RANK_NUM];	// ��ɫ�����б�
};

// ��ɫ�����������а�����
struct RoleKingMatchSeasonRank
{
	DWORD dwRoleId;		// ��ɫID
	WORD wServerId;		// ������ID	
};

// ��������
/*
1.��λ����
2.�������н���
3.����ʤ������
4.ÿ��ս������
5.ÿ����ʤ����
6.���ν���
*/


#pragma pack(pop)
#endif
