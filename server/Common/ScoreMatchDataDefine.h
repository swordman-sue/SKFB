#ifndef _SCORE_MATCH_DATA_DEFINE_H_
#define _SCORE_MATCH_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// ��������սĿ������
struct ScoreMatchTarget
{
	DWORD dwTargetId;		// Ŀ��ID
	//DWORD dwHeadIcondId;	// ͷ��ID
	//DWORD dwTitleId;		// ��ǰʹ�õĳƺ�(0=��ʾľ��)
	//char szTargetName[MAX_ROLE_LEN];
	//WORD wServerId;			// Ŀ�����ڷ�����
	//DWORD dwFightingPower;	// Ŀ��ս��
	WORD wStatus;			// ״̬(0=��û��ս,1=սʤ,2=ս��)
};

// ��ɫ����������
struct ScoreMatchData
{
	WORD wBattleTimes;		// �Ѿ�ʹ�õ���ս����
	WORD wRefreshTimes;		// �Ѿ�ʹ�õ�ˢ�´���
	WORD wBuyBattleTimes;	// �Ѿ��������ս����

	DWORD dwRoleId;		// ��ɫID
	BYTE byCamp;		// ��Ӫ
	WORD wWinTimes;		// ��ʤ����

	// ��������¼�б�(����ȡ��)
	WORD arTaskRewardRecord[MAX_SCORE_MATCH_TASK_NUM];
	WORD wTaskRewardRecordNum;

	// ��սĿ���б�
	ScoreMatchTarget arBattleTargetList[MAX_SCORE_MATCH_BATTLE_TARGET_NUM];
	WORD wBattleTargetNum;
};



// struct ScoreMatchData : public ScoreMatchBaseData
// {
// 	vector<ScoreMatchTarget> vTargetList;	// ��սĿ���б�
// };

#pragma pack(pop)
#endif
