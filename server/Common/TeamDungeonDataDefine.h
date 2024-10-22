#ifndef _TEAM_DUNGEON_DATA_DEFINE_H_
#define _TEAM_DUNGEON_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)

class Player;

// 队伍成员数据
struct TeamMember
{
	Player *pPlayer;
	//DWORD dwRoleId;	// todo::主要是因为设计的原因才这么处理
	DWORD dwContext; // 当前任务上下文件(1.拾取掉落物品数量 2.杀怪数量 3.答题时间, 4.对Boss的总伤害)
	DWORD dwBattleTargetId; // 正在挑战的目标Id
};

// 组队关卡数据
struct TeamMapData
{
	DWORD dwUID; // UID
	DWORD dwMapId; // 关卡ID
	MapPos tMapPos;	// 怪物坐标
	DWORD dwStartBattleTime; // 开始战斗时间
	vector<MonsterData> vMonsterList; // 怪物列表
};

// 场景怪物
struct SceneMonster
{
	DWORD dwUID;
	DWORD dwMapId;		// 关卡ID
	DWORD dwDamage;		// 当前伤害
	DWORD dwTotalHP;	// 总的HP
	DWORD dwStartBattleTime; // 开始战斗时间
	MapPos tMapPos;		// 怪物坐标

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

// 任务队伍数据
struct TaskTeamData
{
	// 成员列表
	TeamMember arMemberList[MAX_TEAM_MEMBER_NUM];
	WORD wMemberNum;

	DWORD dwTeamId;
	WORD wDungeonId;		// 副本ID
	DWORD dwTaskStartTime;	// 任务开始时间
	WORD wTaskLinkId;		// 任务链ID
	WORD wTaskFinishNum;	// 任务完成数量(已完成的)
	DWORD dwCurTaskId;		// 当前任务ID
	WORD wCurTaskType;		// 当前任务类型
	WORD wCurTaskValue;		// 当前任务值(1.拾取掉落物品数量 2.杀怪数量 3.答对数量 4.对话NPC数量)
	DWORD dwStageTaskStartTime; // 阶段任务开始时间
	BYTE byIsAllStageTaskFinish; // 是否所有的阶段任务完成
	BYTE byAnswerTimes;		// 答题次数(错误)
	BYTE byAnswerQustionMemberNum; // 已答题成员数量

	// 法阵能量
	DWORD dwEnergy;

	// 题目ID
	WORD wQuestionId;

	// 关卡列表(怪物)
	vector<TeamMapData> vMapList;

	// 场景物品列表
	vector<SceneItem> vSceneItemList;

	// BOSS数据
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

// 匹配队伍数据
struct MatchTeamData
{
	DWORD dwTeamId;
	Player* arMemberList[MAX_TEAM_MEMBER_NUM];
	WORD wMemberNum;
};

// 准备队伍数据
struct ReadyTeamData
{
	WORD wDungeonId;
	Player* arMemberList[MAX_TEAM_MEMBER_NUM];
	WORD wMemberNum;
};

// 组队排行榜成员数据
struct TeamDungeonMember
{
	WORD wServerId; // 服务器ID
	DWORD dwRoleId; // 角色ID
	char szRoleName[MAX_ROLE_LEN]; // 角色名称
};

// 组队副本排行榜
struct TeamDungeonRank
{
	WORD wRank; // 排行
	DWORD dwUseTime; // 使用时间(秒)
	DWORD dwFinishTime; // 完成时间
	DWORD dwCaptainHeadImgId; // 队长头像ID
	TeamDungeonMember arMemberList[MAX_TEAM_MEMBER_NUM];// 成员列表
};

// boss奖励数据
struct TeamDungeonBossReward
{
	DWORD dwRoleId;
	DWORD dwAttackDamage;	// 攻击伤害(玩家攻击产生的伤害)
	vector<Item> vRewardList; // 奖励列表
};

// 组队副本数据
// struct TeamDungeonData
// {
// 	WORD wDungeonId; // 副本ID
// 	WORD wDailyBattleTimes; // 今日挑战次数
// 	WORD wIsPass; // 是否已通关
// };

// 玩家组队副本数据
struct PlayerTeamDungeonData
{
	WORD wTodayBattleTimes; // 今日挑战次数
	WORD wTodayKillRewardTimes; // 今日击杀奖励次数
	WORD wTodayMVPRewardTimes;	// 今日MVP奖励次数

	WORD arPassDungeonList[MAX_TEAM_DUNGEON_NUM]; // 通关副本列表
	WORD wPassDungeonNum;	// 通关副本数量

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

// 组队副本抽奖记录
struct TeamDungeonLotteryRecord
{
	WORD wServerId;					// 服务器ID	
	char szRoleName[MAX_ROLE_LEN];	// 角色名称
	DWORD dwItemId;					// 抽中的物品ID
	DWORD dwItemNum;				// 抽中的物品数量
	DWORD dwTime;					// 抽奖时间
};

#pragma pack(pop)
#endif
