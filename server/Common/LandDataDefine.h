#pragma once

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

// 领地暴动事件
struct LandRiotEvent
{
	//WORD wEvenId;					// 事件ID
	DWORD dwHappenTime;				// 发生时间
	DWORD dwSuppressTime;			// 镇压时间
	char szFriendName[MAX_ROLE_LEN];// 好友名称
};

// 领地巡逻事件
struct LandPatrolEvent
{
	WORD wEventId;		// 事件ID
	DWORD dwHappenTime;	// 发生时间
	BYTE byIsDouble;	// 是否双倍奖励
};

// 领地城池数据
struct LandCityData
{
	WORD wId;						// 城池Id
	BYTE bySkillLevel;				// 技能等级
	WORD wPatrolHeroInfoId;			// 巡逻的英雄Id(领取奖励后会清空)
	DWORD dwStartPatrolTime;		// 开始巡逻时间(巡逻结束后会清空)
	BYTE byPatrolType;				// 巡逻方式
	BYTE byTimeType;				// 时间类型
	BYTE byIsRiot;					// 是否暴动(镇压或者巡逻结束后会清空)
	BYTE byIsGetAward;				// 是否已经领取奖励
	WORD wAwardItemNum;				// 奖励的碎片数量
	DWORD dwLastHappenEventTime;	// 上次发生事件时间
	WORD wIntervalTime;				// 收益时间间隔(秒)

	// 巡逻事件列表
	LandPatrolEvent arPatrolEventList[MAX_LAND_PATROL_EVENT];
	BYTE byPatrolEventNum;	

	// 暴动事件列表
	LandRiotEvent arRiotEventList[MAX_LAND_ROIT_EVENT];
	BYTE byRiotEventNum;

	// 添加巡逻事件
	void AddPatrolEvent(LandPatrolEvent &tEvent)
	{
		if (byPatrolEventNum >= MAX_LAND_PATROL_EVENT)
			return;

		arPatrolEventList[byPatrolEventNum++] = tEvent;
	}

	// 添加暴动事件
	void AddRoitEvent(const LandRiotEvent &tEvent)
	{
		if (byRiotEventNum >= MAX_LAND_ROIT_EVENT)
			return;

		arRiotEventList[byRiotEventNum++] = tEvent;
	}
};

// 玩家领地信息
struct PlayerLandInfo
{
	LandCityData arCityList[MAX_LAND_CITY_NUM];	// 城池列表
	WORD wCityNum;								// 城池数量
	//BYTE bySuppressRiotTimes;					// 镇压暴动次数
	DWORD dwPatrolTotalTime;					// 巡逻总时间(小时)

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

// 好友领地信息
struct FriendLandData
{
	DWORD dwRoleId;				// 好友ID
	char szName[MAX_ROLE_LEN];	// 好友名称
	WORD wLevel;				// 好友等级
	WORD wHeadIcon;				// 头像
	DWORD dwLastLogoutTime;		// 最后的离线时间

	BYTE byCityNum;				// 已有的城池数量
	BYTE byPatrolCityNum;		// 巡逻中的城池数量
	BYTE byRiotCityNum;			// 暴动中的城池数量
};

// 领地城池基本数据
struct LandCityBaseData
{
	WORD wCityId;				// 城池Id
	DWORD dwStartPatrolTime;	// 开始巡逻时间(0=表示木有在巡逻)
	BYTE byPatrolType;			// 巡逻方式
	BYTE byTimeType;			// 时间类型
	BYTE byIsRiot;				// 是否暴动
	BYTE bySkillLevel;			// 技能等级
	BYTE byIsCanGetReward;		// 是否可以领取奖励 
	WORD wPatrolHeroInfoId;		// 巡逻的英雄Id
};


#pragma pack(pop)
