#pragma once

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// ��ر����¼�
struct LandRiotEvent
{
	//WORD wEvenId;					// �¼�ID
	DWORD dwHappenTime;				// ����ʱ��
	DWORD dwSuppressTime;			// ��ѹʱ��
	char szFriendName[MAX_ROLE_LEN];// ��������
};

// ���Ѳ���¼�
struct LandPatrolEvent
{
	WORD wEventId;		// �¼�ID
	DWORD dwHappenTime;	// ����ʱ��
	BYTE byIsDouble;	// �Ƿ�˫������
};

// ��سǳ�����
struct LandCityData
{
	WORD wId;						// �ǳ�Id
	BYTE bySkillLevel;				// ���ܵȼ�
	WORD wPatrolHeroInfoId;			// Ѳ�ߵ�Ӣ��Id(��ȡ����������)
	DWORD dwStartPatrolTime;		// ��ʼѲ��ʱ��(Ѳ�߽���������)
	BYTE byPatrolType;				// Ѳ�߷�ʽ
	BYTE byTimeType;				// ʱ������
	BYTE byIsRiot;					// �Ƿ񱩶�(��ѹ����Ѳ�߽���������)
	BYTE byIsGetAward;				// �Ƿ��Ѿ���ȡ����
	WORD wAwardItemNum;				// ��������Ƭ����
	DWORD dwLastHappenEventTime;	// �ϴη����¼�ʱ��
	WORD wIntervalTime;				// ����ʱ����(��)

	// Ѳ���¼��б�
	LandPatrolEvent arPatrolEventList[MAX_LAND_PATROL_EVENT];
	BYTE byPatrolEventNum;	

	// �����¼��б�
	LandRiotEvent arRiotEventList[MAX_LAND_ROIT_EVENT];
	BYTE byRiotEventNum;

	// ���Ѳ���¼�
	void AddPatrolEvent(LandPatrolEvent &tEvent)
	{
		if (byPatrolEventNum >= MAX_LAND_PATROL_EVENT)
			return;

		arPatrolEventList[byPatrolEventNum++] = tEvent;
	}

	// ��ӱ����¼�
	void AddRoitEvent(const LandRiotEvent &tEvent)
	{
		if (byRiotEventNum >= MAX_LAND_ROIT_EVENT)
			return;

		arRiotEventList[byRiotEventNum++] = tEvent;
	}
};

// ��������Ϣ
struct PlayerLandInfo
{
	LandCityData arCityList[MAX_LAND_CITY_NUM];	// �ǳ��б�
	WORD wCityNum;								// �ǳ�����
	//BYTE bySuppressRiotTimes;					// ��ѹ��������
	DWORD dwPatrolTotalTime;					// Ѳ����ʱ��(Сʱ)

	LandCityData* GetLandData(WORD wCityId)
	{
		for (WORD i = 0; i < wCityNum; ++i)
		{
			if (arCityList[i].wId == wCityId)
			{
				return &arCityList[i];
			}
		}
		return NULL;
	}

	bool IsInPatrol(WORD wHeroInfoId) const
	{
		for (WORD i = 0; i < wCityNum; ++i)
		{
			if (arCityList[i].wPatrolHeroInfoId == wHeroInfoId)
			{
				return true;
			}
		}
		return false;
	}
};

// ���������Ϣ
struct FriendLandData
{
	DWORD dwRoleId;				// ����ID
	char szName[MAX_ROLE_LEN];	// ��������
	WORD wLevel;				// ���ѵȼ�
	WORD wHeadIcon;				// ͷ��
	DWORD dwLastLogoutTime;		// ��������ʱ��

	BYTE byCityNum;				// ���еĳǳ�����
	BYTE byPatrolCityNum;		// Ѳ���еĳǳ�����
	BYTE byRiotCityNum;			// �����еĳǳ�����
};

// ��سǳػ�������
struct LandCityBaseData
{
	WORD wCityId;				// �ǳ�Id
	DWORD dwStartPatrolTime;	// ��ʼѲ��ʱ��(0=��ʾľ����Ѳ��)
	BYTE byPatrolType;			// Ѳ�߷�ʽ
	BYTE byTimeType;			// ʱ������
	BYTE byIsRiot;				// �Ƿ񱩶�
	BYTE bySkillLevel;			// ���ܵȼ�
	BYTE byIsCanGetReward;		// �Ƿ������ȡ���� 
	WORD wPatrolHeroInfoId;		// Ѳ�ߵ�Ӣ��Id
};


#pragma pack(pop)
