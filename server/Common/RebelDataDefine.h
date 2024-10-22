#ifndef _REBEL_DATA_DEFINE_H_
#define _REBEL_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// �Ѿ����� 
struct RebelData
{
	DWORD dwRebelId;										// �Ѿ�ID
	WORD wLevel;											// �Ѿ��ȼ�
	BYTE byQuality;											// �Ѿ�Ʒ��
	WORD wMapId;											// �ؿ�ID
// 	MonsterData arMonsterDamageList[MAX_BATTLE_MONSTER_NUM];// �����˺��б�
// 	BYTE byMonsterNum;										// ��������
	vector<MonsterData> vMonsterList;						// �����б�
	DWORD dwRoleId;											// ������ID
	char szRoleName[MAX_ROLE_LEN];							// ����������
	DWORD dwCreateTime;										// ����ʱ��
	DWORD dwLiveTime;										// ���ʱ��
	//DWORD dwTotalBlood;									// �ܵ�Ѫ��
	//bool isDead;											// �Ƿ�������
	BYTE byIsRemove;										// �Ƿ���ɾ��
	vector<DWORD> vSharePlayerList;							// �Ѿ����������б�

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
	DWORD dwDiscovererRoleId;				// ������ID
	char szDiscovererName[MAX_ROLE_LEN];	// ����������
	DWORD dwTotalDamage;					// ��ǰ���˺�
	WORD wRebelLevel;						// �Ѿ��ȼ�
	WORD wRebelMapId;						// �Ѿ��ؿ�ID
	DWORD dwCreateTime;						// ����ʱ��
	BYTE byQuality;							// �Ѿ�Ʒ��

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
	DWORD dwTodayExploit;									// �����ۻ���ѫ
	DWORD dwTodayDamage;									// �����ۻ��˺�
	WORD wExploitRank;										// ��ѫ����
	WORD wDamageRank;										// �˺�����
	BYTE byIsAlreadyShare;									// �Ƿ��Ѿ�����
	WORD wInitRoleLevel;									// ��ɫÿ�ճ�ʼ�ȼ�
	ClientRebelData arRebelList[MAX_REBEL_SHARE_NUM];		// �Ѿ��б�(�Ѿ�ID,������,��ǰHP)
	BYTE byRebelListNum;									// �Ѿ��б�����

	void AddRebelData(const ClientRebelData &tRebelData)
	{
		if (byRebelListNum >= MAX_REBEL_SHARE_NUM)
			return;

		arRebelList[byRebelListNum++] = tRebelData;
	}
};

// ����Ѿ�����
struct PlayerRebelData
{
	WORD wRebelLevel;								// �Ѿ��ȼ�
	DWORD dwTodayExploit;							// �����ۻ���ѫ
	DWORD dwTodayDamage;							// �����ۻ��˺�
	WORD arAwardRecordList[MAX_EXPLOIT_AWARD_NUM];	// ������ȡ��¼�б�
	WORD wAwardRecordNum;							// ������¼����
	WORD wIsAlreadyShare;							// �Ƿ��Ѿ������

	// �Ƿ��ڽ�����¼�б���
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
