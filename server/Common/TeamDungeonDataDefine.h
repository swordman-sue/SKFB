#ifndef _TEAM_DUNGEON_DATA_DEFINE_H_
#define _TEAM_DUNGEON_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

class Player;

// �����Ա����
struct TeamMember
{
	Player *pPlayer;
	//DWORD dwRoleId;	// todo::��Ҫ����Ϊ��Ƶ�ԭ�����ô����
	DWORD dwContext; // ��ǰ���������ļ�(1.ʰȡ������Ʒ���� 2.ɱ������ 3.����ʱ��, 4.��Boss�����˺�)
	DWORD dwBattleTargetId; // ������ս��Ŀ��Id
};

// ��ӹؿ�����
struct TeamMapData
{
	DWORD dwUID; // UID
	DWORD dwMapId; // �ؿ�ID
	MapPos tMapPos;	// ��������
	DWORD dwStartBattleTime; // ��ʼս��ʱ��
	vector<MonsterData> vMonsterList; // �����б�
};

// ��������
struct SceneMonster
{
	DWORD dwUID;
	DWORD dwMapId;		// �ؿ�ID
	DWORD dwDamage;		// ��ǰ�˺�
	DWORD dwTotalHP;	// �ܵ�HP
	DWORD dwStartBattleTime; // ��ʼս��ʱ��
	MapPos tMapPos;		// ��������

	SceneMonster& operator=(const TeamMapData &tMap)
	{
		this->dwUID = tMap.dwUID;
		this->dwMapId = tMap.dwMapId;
		this->tMapPos = tMap.tMapPos;
		this->dwStartBattleTime = tMap.dwStartBattleTime;
		this->dwDamage = 0;
		this->dwTotalHP = 0;
		for (size_t i = 0; i < tMap.vMonsterList.size(); ++i)
		{
			const MonsterData &tMonster = tMap.vMonsterList[i];
			this->dwDamage += tMonster.dwDamage > tMonster.dwTotalHP ? tMonster.dwTotalHP : tMonster.dwDamage;
			this->dwTotalHP += tMonster.dwTotalHP;
		}
		return *this;
	}
};

// �����������
struct TaskTeamData
{
	// ��Ա�б�
	TeamMember arMemberList[MAX_TEAM_MEMBER_NUM];
	WORD wMemberNum;

	DWORD dwTeamId;
	WORD wDungeonId;		// ����ID
	DWORD dwTaskStartTime;	// ����ʼʱ��
	WORD wTaskLinkId;		// ������ID
	WORD wTaskFinishNum;	// �����������(����ɵ�)
	DWORD dwCurTaskId;		// ��ǰ����ID
	WORD wCurTaskType;		// ��ǰ��������
	WORD wCurTaskValue;		// ��ǰ����ֵ(1.ʰȡ������Ʒ���� 2.ɱ������ 3.������� 4.�Ի�NPC����)
	DWORD dwStageTaskStartTime; // �׶�����ʼʱ��
	BYTE byIsAllStageTaskFinish; // �Ƿ����еĽ׶��������
	BYTE byAnswerTimes;		// �������(����)
	BYTE byAnswerQustionMemberNum; // �Ѵ����Ա����

	// ��������
	DWORD dwEnergy;

	// ��ĿID
	WORD wQuestionId;

	// �ؿ��б�(����)
	vector<TeamMapData> vMapList;

	// ������Ʒ�б�
	vector<SceneItem> vSceneItemList;

	// BOSS����
	TeamMapData tBossData;

	TeamMapData* GetMapData(DWORD dwMapUId)
	{
		for (size_t i = 0; i < vMapList.size(); ++i)
		{
			if (vMapList[i].dwUID == dwMapUId)
				return &vMapList[i];
		}
		return NULL;
	}

	SceneItem* GetSceneItem(DWORD dwItemUId)
	{
		for (size_t i = 0; i < vSceneItemList.size(); ++i)
		{
			if (vSceneItemList[i].dwUID == dwItemUId)
				return &vSceneItemList[i];
		}
		return NULL;
	}

	TaskTeamData()
	{
		wMemberNum = 0;
		memset(arMemberList, 0, sizeof(arMemberList));
		dwTeamId = 0;
		wDungeonId = 0;
		dwTaskStartTime = 0;
		wTaskLinkId = 0;
		wTaskFinishNum = 0;
		dwCurTaskId = 0;
		wCurTaskValue = 0;
		dwStageTaskStartTime = 0;
		byIsAllStageTaskFinish = false;
		byAnswerTimes = 0;
		dwEnergy = 0;
		wQuestionId = 0;
		vMapList.clear();
		vSceneItemList.clear();
		tBossData.dwUID = 0;
		tBossData.dwMapId = 0;
		memset(&tBossData.tMapPos, 0, sizeof(tBossData.tMapPos));
		tBossData.dwStartBattleTime = 0;
		tBossData.vMonsterList.clear();
	}
};

// ƥ���������
struct MatchTeamData
{
	DWORD dwTeamId;
	Player* arMemberList[MAX_TEAM_MEMBER_NUM];
	WORD wMemberNum;
};

// ׼����������
struct ReadyTeamData
{
	WORD wDungeonId;
	Player* arMemberList[MAX_TEAM_MEMBER_NUM];
	WORD wMemberNum;
};

// ������а��Ա����
struct TeamDungeonMember
{
	WORD wServerId; // ������ID
	DWORD dwRoleId; // ��ɫID
	char szRoleName[MAX_ROLE_LEN]; // ��ɫ����
};

// ��Ӹ������а�
struct TeamDungeonRank
{
	WORD wRank; // ����
	DWORD dwUseTime; // ʹ��ʱ��(��)
	DWORD dwFinishTime; // ���ʱ��
	DWORD dwCaptainHeadImgId; // �ӳ�ͷ��ID
	TeamDungeonMember arMemberList[MAX_TEAM_MEMBER_NUM];// ��Ա�б�
};

// boss��������
struct TeamDungeonBossReward
{
	DWORD dwRoleId;
	DWORD dwAttackDamage;	// �����˺�(��ҹ����������˺�)
	vector<Item> vRewardList; // �����б�
};

// ��Ӹ�������
// struct TeamDungeonData
// {
// 	WORD wDungeonId; // ����ID
// 	WORD wDailyBattleTimes; // ������ս����
// 	WORD wIsPass; // �Ƿ���ͨ��
// };

// �����Ӹ�������
struct PlayerTeamDungeonData
{
	WORD wTodayBattleTimes; // ������ս����
	WORD wTodayKillRewardTimes; // ���ջ�ɱ��������
	WORD wTodayMVPRewardTimes;	// ����MVP��������

	WORD arPassDungeonList[MAX_TEAM_DUNGEON_NUM]; // ͨ�ظ����б�
	WORD wPassDungeonNum;	// ͨ�ظ�������

	void AddPassDungeon(WORD wDungeonId)
	{
		bool bIsAdd = true;
		for (WORD i = 0; i < wPassDungeonNum; ++i)
		{
			if (arPassDungeonList[i] == wDungeonId)
			{
				bIsAdd = false;
				break;
			}
		}

		if (bIsAdd && wPassDungeonNum < MAX_TEAM_DUNGEON_NUM)
		{
			arPassDungeonList[wPassDungeonNum++] = wDungeonId;
		}
	}
};

// ��Ӹ����齱��¼
struct TeamDungeonLotteryRecord
{
	WORD wServerId;					// ������ID	
	char szRoleName[MAX_ROLE_LEN];	// ��ɫ����
	DWORD dwItemId;					// ���е���ƷID
	DWORD dwItemNum;				// ���е���Ʒ����
	DWORD dwTime;					// �齱ʱ��
};

#pragma pack(pop)
#endif
