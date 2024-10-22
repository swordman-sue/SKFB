#ifndef _WORLD_BOSS_DATA_DEFINE_H_
#define _WORLD_BOSS_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// �������BOSS����
struct PlayerWorldBoseData
{
	BYTE byCamp;				// ѡ�����Ӫ(CAMP_INVALID=99��ʾ��ľ��ѡ��)
	BYTE byBattleTimes;			// �Ѿ���ս�Ĵ���
	BYTE byBuyBattleTimes;		// �Ѿ��������ս����
	DWORD dwLastRecoveryTime;	// �ϴλָ�ʱ��
	vector<WORD> vHonorRewardRecordList;	// ����������¼�б�
	vector<WORD> vDamageRewardRecordList;	// �˺�������¼�б�
};

// ����BOSSս��
struct WorldBossBattleRecord
{
	WORD wBossLevel;						// Boss�ȼ�	
	DWORD dwLuckAttackTime;					// ���˹���ʱ��
	DWORD dwKillTime;						// ��ɱʱ��
	char szLuckAttackName[MAX_ROLE_LEN];	// ���˹���������
	char szKillName[MAX_ROLE_LEN];			// ��ɱ������
};

// ����BOSS����
struct WorldBossData
{
	WORD wServerId; // ������ID
	WORD wBossLevel;	// ��ǰ����BOSS�ȼ�
	vector<MonsterData> vMonsterList; // �����б�

	DWORD dwReliveTime;	// ����ʱ��
	vector<WorldBossBattleRecord> vBattleRecordList; // ս����¼�б�
	bool bHaveLuckReward;
};

#pragma pack(pop)
#endif
