#include "stdafx.h"
#include "TeamDungeonManager.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Fuben\FubenManager.h"
#include "..\Item\ItemManager.h"
#include "ToolKit\LogFile.h"
#include "..\PlayerManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "data_define.pb.h"

TeamMember*	GetMemberData(TaskTeamData *pTeamData, DWORD dwMemberId)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (pTeamData->arMemberList[i].pPlayer->GetRoleId() == dwMemberId)
			return &pTeamData->arMemberList[i];
	}
	return NULL;
}

CTeamDungeonManager::CTeamDungeonManager()
{

}

CTeamDungeonManager::~CTeamDungeonManager()
{

}

void CTeamDungeonManager::Init()
{
	LoadDataRequest();
}

void CTeamDungeonManager::Release()
{
	SaveDataToDB();
}

void CTeamDungeonManager::Update()
{
	DWORD dwBossBattleLimitTime = g_GetConfigValue(230, 300);
	DWORD dwCurTime = time(NULL);
	map<DWORD, DWORD>::iterator iter = m_BattleBossTeamList.begin();
	for (; iter != m_BattleBossTeamList.end(); ++iter)
	{
		// 暂时定5分钟,延后20秒处理(如果成员还没上报完,强行结束)
		if (dwCurTime >= iter->second + dwBossBattleLimitTime)
		{
			TaskTeamData *pTeamData = GetTaskTeamData(iter->first);
			if (pTeamData)
				DoBossReward(pTeamData);
			m_BattleBossTeamList.erase(iter);
			break;
		}
	}

	// 处理答题卡住的情况
	DWORD dwAnserQuestionLimitTime = g_GetConfigValue(214);
 	map<DWORD, TaskTeamData>::iterator team_iter = m_TaskTeamList.begin();
	for (; team_iter != m_TaskTeamList.end(); ++team_iter)
 	{
		TaskTeamData *pTeamData = &team_iter->second;

		// 处理3秒钟的延误时间
		if (TASK_ANSWER == pTeamData->wCurTaskType && pTeamData->byAnswerQustionMemberNum && !pTeamData->byIsAllStageTaskFinish &&
			dwCurTime >= pTeamData->dwStageTaskStartTime + dwAnserQuestionLimitTime)
 		{
			MAPLOG(ERROR_LEVEL, "[%s]答题超时,系统帮答题[TeamId:%d]", __FUNCTION__, pTeamData->dwTeamId);
		
			TimeOutAnswerQuestion(pTeamData);

			break; // 一次只处理一个(防止接口里面有删除动作)
 		}
 	}
}

// 登录
void CTeamDungeonManager::OnLogin(Player *pPlayer)
{

}

// 登出
void CTeamDungeonManager::OnLogout(Player *pPlayer)
{
	//MAPLOG(ERROR_LEVEL, "[%s]玩家下线[role_id:%d,TeamId:%d]",__FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

	// 在大厅
	if (IsInHall(pPlayer->GetRoleId()))
	{
		//MAPLOG(ERROR_LEVEL, "[%s]玩家在大厅中直接下线[role_id:%d,TeamId:%d]", __FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());
		DoLeaveHall(pPlayer);
	}

	// 在任务队伍,或者准备队伍中
	if (pPlayer->GetTeamId())
	{
		//MAPLOG(ERROR_LEVEL, "[%s]玩家在组队中直接下线[role_id:%d,TeamId:%d]", __FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

		// 准备状态的队伍列表
		DoLeaveReadyTeam(pPlayer);

		// 处理离开任务中的队伍
		DoLeaveTaskTeam(pPlayer);
	}
	
	// 匹配池
	if (pPlayer->GetTeamDungeonId())
	{
// 		MAPLOG(ERROR_LEVEL, "[%s]玩家在匹配中直接下线(个人)[role_id:%d,TeamId:%d]",
// 			__FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

		 // 个人匹配池
		 map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(pPlayer->GetTeamDungeonId());
		 if (iter != m_PersonMatchList.end())
		 {
			 iter->second.erase(pPlayer->GetRoleId());
		 }

// 		 MAPLOG(ERROR_LEVEL, "[%s]玩家在匹配中直接下线(队伍)[role_id:%d,TeamId:%d]", 
// 			 __FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

		 // 队伍匹配池
		 if (pPlayer->GetTeamId())
		 {
			 DoLeaveTeamMatchList(pPlayer);
		 }
	}

	pPlayer->SetTeamDungeonId(0);
	pPlayer->SetTeamId(0);
}

void CTeamDungeonManager::SaveDataToDB()
{ 
	PB::TeamDungeonData PBTeamDungeonData;

	// 赛季排行榜记录
	map<WORD, vector<TeamDungeonRank>>::iterator iter = m_TeamDungeonRankList.begin();
	for (; iter != m_TeamDungeonRankList.end(); ++iter)
	{
		::PB::TeamDungeonRank *pDungeonRankList = PBTeamDungeonData.add_dungeon_rank_list();
		pDungeonRankList->set_dungeon_id(iter->first);
		vector<TeamDungeonRank>::iterator iter1 = iter->second.begin();
		for (; iter1 != iter->second.end(); ++iter1)
		{
			::PB::TeamDungeonRoleRank *pPBRoleRank = pDungeonRankList->add_role_rank_list();
			pPBRoleRank->set_rank(iter1->wRank);
			pPBRoleRank->set_finish_time(iter1->dwFinishTime);
			pPBRoleRank->set_use_time(iter1->dwUseTime);
			pPBRoleRank->set_captain_head_img_id(iter1->dwCaptainHeadImgId);
			for (WORD i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				TeamDungeonMember &tMember = iter1->arMemberList[i];
				::PB::TeamDungeonMember *pPBMember = pPBRoleRank->add_member_list();
				pPBMember->set_server_id(tMember.wServerId);
				pPBMember->set_role_name(tMember.szRoleName);
				pPBMember->set_role_id(tMember.dwRoleId);
			}
		}
	}

	// 抽奖记录
	deque<TeamDungeonLotteryRecord>::iterator lotter_record_iter = m_LotteryRecordList.begin();
	for (; lotter_record_iter != m_LotteryRecordList.end(); ++lotter_record_iter)
	{
		::PB::TeamDungeonLotteryRecord *pPBLotteryRecord = PBTeamDungeonData.add_lottery_record();
		if (pPBLotteryRecord)
		{
			pPBLotteryRecord->set_server_id(lotter_record_iter->wServerId);
			pPBLotteryRecord->set_role_name(lotter_record_iter->szRoleName);
			pPBLotteryRecord->set_item_id(lotter_record_iter->dwItemId);
			pPBLotteryRecord->set_item_num(lotter_record_iter->dwItemNum);
			pPBLotteryRecord->set_record_time(lotter_record_iter->dwTime);
		}
	}

	string strSaveData("");
	PBTeamDungeonData.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_TEAM_DUNGEON_RANK_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]组队副本存储数据过大[data_size:%d,list_num:%d]",
			__FUNCTION__, strSaveData.size(), PBTeamDungeonData.dungeon_rank_list_size());
		return;
	}

	TeamDungeonRankData_Save* pQuery = TeamDungeonRankData_Save::ALLOC();
	pQuery->SetIndex(DB_TEAM_DUNGEON_RANK_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);
}

void CTeamDungeonManager::LoadDataFromDB(const char *pData, int size)
{
	m_TeamDungeonRankList.clear();
	m_LotteryRecordList.clear();

	if (size > 0)
	{
		PB::TeamDungeonData PBTeamDungeonData;
		if (!PBTeamDungeonData.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "解析组队副本排行数据出错(size:%d)", size);
			return;
		}

		// 排行榜数据
		vector<TeamDungeonRank> vDungeonRank;
		TeamDungeonRank tRankData;
		for (int i = 0; i < PBTeamDungeonData.dungeon_rank_list_size(); ++i)
		{
			const ::PB::TeamDungeonRank &tPBDungeonRank = PBTeamDungeonData.dungeon_rank_list(i);
			vDungeonRank.clear();
			for (int k = 0; k < tPBDungeonRank.role_rank_list_size(); ++k)
			{
				const ::PB::TeamDungeonRoleRank &tPBRoleRank = tPBDungeonRank.role_rank_list(k);
				memset(&tRankData, 0, sizeof(tRankData));
				tRankData.wRank = tPBRoleRank.rank();
				tRankData.dwUseTime = tPBRoleRank.use_time();
				tRankData.dwCaptainHeadImgId = tPBRoleRank.captain_head_img_id();
				for (int m = 0; m < tPBRoleRank.member_list_size(); ++m)
				{
					const ::PB::TeamDungeonMember &tPBMemberData = tPBRoleRank.member_list(m);
					TeamDungeonMember &tMemberData = tRankData.arMemberList[m];
					tMemberData.wServerId = tPBMemberData.server_id();
					tMemberData.dwRoleId = tPBMemberData.role_id();
					strcpy_s(tMemberData.szRoleName, sizeof(tMemberData.szRoleName), tPBMemberData.role_name().c_str());
				}
				vDungeonRank.push_back(tRankData);
			}

			m_TeamDungeonRankList.insert(make_pair(tPBDungeonRank.dungeon_id(), vDungeonRank));
		}

		// 抽奖购买记录
		TeamDungeonLotteryRecord tLotteryRecord;
		for (int i = 0; i < PBTeamDungeonData.lottery_record_size(); ++i)
		{
			const ::PB::TeamDungeonLotteryRecord &tPBLotteryRecord = PBTeamDungeonData.lottery_record(i);
			tLotteryRecord.wServerId = tPBLotteryRecord.server_id();
			strcpy_s(tLotteryRecord.szRoleName, sizeof(tLotteryRecord.szRoleName), tPBLotteryRecord.role_name().c_str());
			tLotteryRecord.dwItemId = tPBLotteryRecord.item_id();
			tLotteryRecord.dwItemNum = tPBLotteryRecord.item_num();
			tLotteryRecord.dwTime = tPBLotteryRecord.record_time();
			AddLotteryRecord(tLotteryRecord);
		}
	}
}

// 加载数据请求
void CTeamDungeonManager::LoadDataRequest()
{
	TeamDungeonRankData_Load* pQuery = TeamDungeonRankData_Load::ALLOC();
	pQuery->SetIndex(DB_TEAM_DUNGEON_RANK_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	g_MapServer.MapDBQuery(pQuery);
}

// 进入场景
void CTeamDungeonManager::OnNetEnterScene(WORD wSceneType, DWORD dwContext, Player *pPlayer)
{
// 	MAPLOG(ERROR_LEVEL, "[%s]玩家进入场景[SceneType:%d,RoleId:%d]!",
// 		__FUNCTION__, wSceneType, pPlayer->GetRoleId());

	// 大厅
	if (wSceneType == TEAM_HALL)
	{
		// 检测是否在场景里
		if (IsInHall(pPlayer->GetRoleId()))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]玩家已在场景里[SceneType:%d,RoleId:%d]!", __FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		// 取场景配置
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(1);
		if (!pSceneCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]取不到场景配置[SceneId:%d]!", __FUNCTION__, 1);
			return;
		}

		// 随机坐标
		MapPos tMapPos;
		RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
		pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

		// todo::暂时处理
		//pPlayer->SetMapPos(100, 320);

		// 广播给其他玩家,有人进入场景
		MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
		pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
		enter_notice.m_wScene = wSceneType;
		enter_notice.m_tMapPos = pPlayer->GetMapPos();
		SendToHallPlayer(&enter_notice);

		// 响应
		MSG_S2C_ENTER_SCENE_RESP resp;
		resp.m_dwContext = dwContext;
		resp.m_wScene = wSceneType;
		resp.m_tMapPos = pPlayer->GetMapPos();
		resp.m_wPlayerNum = 0;
		map<DWORD, Player*>::iterator iter = m_HallPlayerList.begin();
		for (; iter != m_HallPlayerList.end(); ++iter)
		{
			if (resp.m_wPlayerNum < MAX_SCENE_PLAYER_NUM)
			{
				PlayerCommonData &tPlayerData = resp.m_PlayerList[resp.m_wPlayerNum];
				resp.m_PlayerMapPos[resp.m_wPlayerNum] = iter->second->GetMapPos();
				iter->second->FillCommonData(&tPlayerData);
				resp.m_wPlayerNum++;
			}
		}
		pPlayer->SendPacket(&resp);

		AddHallPlayer(pPlayer);
	}
	// 1.采集, 2.打怪  3.Boss
	else if (wSceneType == TEAM_GATHER || wSceneType == TEAM_NORMAL_MONSTER || wSceneType == TEAM_BOSS)
	{
		TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
		if (!pTeamData)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]找不到玩家队伍数据[SceneType:%d,RoleId:%d]!",__FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
		if (!pTaskLinkCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]取不到组队副本任务链配置[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
			return;
		}

		// 取场景配置 
		//const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTeamData->wDungeonId);
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(dwContext);
		if (!pSceneCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]取不到场景配置[SceneId:%d]!", __FUNCTION__, dwContext);
			return;
		}

		// 如果是BOSS
		if (wSceneType == TEAM_BOSS)
		{	
			// 检测是否可以挑战BOSS(条件1:任务数量已完成 2:任务时间已到达)
			if (pTeamData->wTaskFinishNum < pTaskLinkCfg->wTaskNum &&
				time(NULL) < pTeamData->dwTaskStartTime + pTaskLinkCfg->dwTaskLimitTime)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]任务还没完成,不能挑战BOSS[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
				return;
			}

			// 是否已创建BOSS
			if (!pTeamData->tBossData.dwStartBattleTime)
				CreateBoss(pTeamData);
		}
			
		// 随机坐标
		MapPos tMapPos;
		RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
		pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

		// 广播给其他玩家,有人进入场景
		MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
		pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
		enter_notice.m_wScene = wSceneType;
		enter_notice.m_tMapPos = pPlayer->GetMapPos();
		SendMsgToTeamMember(pTeamData, &enter_notice);

		// 物品列表
		if (pTeamData->vSceneItemList.size())
		{
			MSG_S2C_SCENE_ITEM_LIST_NOTICE item_list_msg;
			item_list_msg.m_wSceneItemNum = 0;
			for (size_t i = 0; i < pTeamData->vSceneItemList.size(); ++i)
			{
				item_list_msg.m_SceneItemList[item_list_msg.m_wSceneItemNum++] = pTeamData->vSceneItemList[i];
			}
			pPlayer->SendPacket(&item_list_msg);
		}

		MSG_S2C_SCENE_MONSTER_LIST_NOTICE monster_list_msg;
		monster_list_msg.m_wMonsterNum = 0;
		
		// 如果是boss
		if (wSceneType == TEAM_BOSS)
		{
			monster_list_msg.m_arMonsterList[monster_list_msg.m_wMonsterNum++] = pTeamData->tBossData;
		}
		
		// 普通怪物列表
		if (pTeamData->vMapList.size())
		{
			for (size_t i = 0; i < pTeamData->vMapList.size(); ++i)
			{
				if (monster_list_msg.m_wMonsterNum >= MAX_ATTACK_CITY_MONSTER_NUM)
					break;

				monster_list_msg.m_arMonsterList[monster_list_msg.m_wMonsterNum++] = pTeamData->vMapList[i];
			}
		}

		pPlayer->SendPacket(&monster_list_msg);

		// 进入响应
		MSG_S2C_ENTER_SCENE_RESP enter_resp;
		enter_resp.m_wPlayerNum = pTeamData->wMemberNum;
		enter_resp.m_wScene = wSceneType;
		enter_resp.m_dwContext = dwContext;
		enter_resp.m_tMapPos = pPlayer->GetMapPos();
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			Player *pMember = pTeamData->arMemberList[i].pPlayer;
			pMember->FillCommonData(&enter_resp.m_PlayerList[i]);
			enter_resp.m_PlayerMapPos[i] = pMember->GetMapPos();
		}
		pPlayer->SendPacket(&enter_resp);
	}
}

// 离开场景
void CTeamDungeonManager::OnNetLeaveScene(WORD wSceneType, Player *pPlayer)
{
// 	MAPLOG(ERROR_LEVEL, "[%s]玩家离开场景[SceneType:%d,RoleId:%d]!",
// 		__FUNCTION__, wSceneType, pPlayer->GetRoleId());

	// 大厅
	if (wSceneType == TEAM_HALL)
	{
		// 检测是否在场景里
		if (!IsInHall(pPlayer->GetRoleId()))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]玩家木有在场景里[SceneType:%d,RoleId:%d]!", __FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		// 处理离开逻辑
		DoLeaveHall(pPlayer);

		pPlayer->SetTeamDungeonId(0);
		pPlayer->SetTeamId(0);
	}
	// 1.采集, 2.打怪  3.Boss
	else
	{
		TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
		if (pTeamData)
		{
			// 广播给其他人,有人离开场景
			MSG_S2C_LEAVE_SCENE_NOTICE leave_notify;
			leave_notify.m_dwRoleId = pPlayer->GetRoleId();
			SendMsgToTeamMember(pTeamData, &leave_notify);
		}
	}

	// 响应给离开者
	MSG_S2C_LEAVE_SCENE_RESP resp_msg;
	resp_msg.m_wScene = wSceneType;
	pPlayer->SendPacket(&resp_msg);
}

// 行走请求
void CTeamDungeonManager::OnNetGoto(WORD wSceneType, Player *pPlayer, MapPos tSrcPos, MapPos tDestPos)
{
	// 大厅
	if (wSceneType == TEAM_HALL)
	{
		// 检测是否在场景里
		if (!IsInHall(pPlayer->GetRoleId()))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]玩家没在场景里[SceneType:%d,RoleId:%d]!", __FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		pPlayer->SetMapPos(tDestPos.wMapX, tDestPos.wMapY);

		// 响应
		MSG_S2C_HERO_GOTO_RESP resp;
		resp.m_wScene = wSceneType;
		resp.m_tSrcPos = tSrcPos;
		resp.m_tDestPos = tDestPos;
		pPlayer->SendPacket(&resp);

		// 广播给其他成员
		MSG_S2C_HERO_GOTO_NOTIFY notice;
		notice.m_wScene = wSceneType;
		notice.m_dwRoleId = pPlayer->GetRoleId();
		notice.m_tSrcPos = tSrcPos;
		notice.m_tDestPos = tDestPos;
		SendToHallPlayer(&notice);
	}
	// 1.采集, 2.打怪  3.Boss
	else
	{
		// 响应
		MSG_S2C_HERO_GOTO_RESP resp;
		resp.m_wScene = wSceneType;
		resp.m_tSrcPos = tSrcPos;
		resp.m_tDestPos = tDestPos;
		pPlayer->SendPacket(&resp);

		// 取队伍数据
		TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
		if (pTeamData)
		{
			// 广播给其他成员
			MSG_S2C_HERO_GOTO_NOTIFY notify;
			notify.m_wScene = wSceneType;
			notify.m_dwRoleId = pPlayer->GetRoleId();
			notify.m_tSrcPos = tSrcPos;
			notify.m_tDestPos = tDestPos;
			SendMsgToTeamMember(pTeamData, &notify);
		}
	}
}

// 聊天
void CTeamDungeonManager::Chat(NetMsgBody *pMsg)
{
	// 大厅玩家
	map<DWORD, Player*>::iterator hall_iter = m_HallPlayerList.begin();
	for (; hall_iter != m_HallPlayerList.end(); ++hall_iter)
	{
		hall_iter->second->SendPacket(pMsg);
	}

	// 任务中的玩家
	map<DWORD, TaskTeamData>::iterator task_iter = m_TaskTeamList.begin();
	for (; task_iter != m_TaskTeamList.end(); ++task_iter)
	{
		for (int i = 0; i < task_iter->second.wMemberNum; i++)
		{
			if (task_iter->second.arMemberList[i].pPlayer)
				task_iter->second.arMemberList[i].pPlayer->SendPacket(pMsg);
		}
	}
}

// 开始战斗(怪物)
bool CTeamDungeonManager::OnMonsterBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return false;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId()); // pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到成员数据[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return false;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return false;
	}

	// 检测是否杀怪任务
	if (pTaskCfg->wTaskType != TASK_MONSTER)
	{
		MAPLOG(ERROR_LEVEL, "[%s]当前不是杀怪任务[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return false;
	}

	// 取怪物数据
	TeamMapData *pMap = pTeamData->GetMapData(dwTargetId);
	if (!pMap)
	{
		MAPLOG(ERROR_LEVEL, "[%s]怪物不存在或者已死亡[target_id:%d]", __FUNCTION__,dwTargetId);
		return false;
	}

	// 怪物是否正在战斗中
	if (pMap->dwStartBattleTime && time(NULL) < pMap->dwStartBattleTime + g_GetConfigValue(209))
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]怪物正在战斗中...[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	for (size_t i = 0; i < pMap->vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pMap->vMonsterList[i]);
	}

	pFBMgr->SetBattleInfo(0, 0, pMap->dwMapId);
	pFBMgr->SetTargetId(dwTargetId);

	pMap->dwStartBattleTime = time(NULL);

	// 记录正在挑战的怪物
	pMemberData->dwBattleTargetId = dwTargetId;

	// 广播怪物数据变动
 	MSG_S2C_SCENE_MONSTER_DATA_NOTICE msg;
	msg.m_tMonster = *pMap;
	SendMsgToTeamMember(pTeamData, &msg);

	return true;
}

// 战斗结束(怪物)
bool CTeamDungeonManager::OnMonsterBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	DWORD dwTargetId = pFBMgr->GetTargetId();

	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return false;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return false;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId()); //pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到成员数据[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return false;
	}

	// 取怪物数据
	TeamMapData *pMap = pTeamData->GetMapData(dwTargetId);
	if (!pMap)
	{
		MAPLOG(ERROR_LEVEL, "[%s]怪物不存在或者已死亡[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	// 怪物是否正在战斗中
	if (!pMap->dwStartBattleTime)
	{
		MAPLOG(ERROR_LEVEL, "[%s]怪物没在战斗中...[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	pMap->dwStartBattleTime = 0;

	// 更新怪物伤害
	UpdateMonsterDamage(pMap->vMonsterList, pMonsterList, byMonsterNum);

	// 广播怪物数据变动
	MSG_S2C_SCENE_MONSTER_DATA_NOTICE msg;
	msg.m_tMonster = *pMap;
	SendMsgToTeamMember(pTeamData, &msg);

	// 检测是否已死亡
	if (IsMonsterDead(pMap->vMonsterList))
	{
		pMemberData->dwContext++;
		pTeamData->wCurTaskValue++;
		
		// 阶段任务完成
		if (pTeamData->wCurTaskValue >= pTaskCfg->GetTaskParam(1))
		{
			DoFinishStageTask(*pTeamData, pTaskCfg);
		}
	}

	// 死亡则回到复活点
	if (!byIsWin)
	{
		// 取任务数据
		const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
		if (!pTaskCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到任务数据[TaskId:%d]!", __FUNCTION__, pTeamData->dwCurTaskId);
			return false;
		}

		// 取场景配置
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
		if (pSceneCfg)
		{
			// 随机坐标
			MapPos tMapPos;
			RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
			pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

			// 广播给其他玩家,有人进入场景
			MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
			pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
			enter_notice.m_wScene = 0;
			enter_notice.m_tMapPos = pPlayer->GetMapPos();
			SendMsgToTeamMember(pTeamData, &enter_notice);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到场景配置[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
		}
	}

	return true;
}

// 开始战斗(Boss)
bool CTeamDungeonManager::OnBossBattleStart(Player *pPlayer)
{
	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, pPlayer->GetTeamId());
		return false;
	}

	// 检测是否已可以挑战BOSS了
	if (!pTeamData->tBossData.vMonsterList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]还没创建BOSS,不能挑战[TeamId:%d]!", __FUNCTION__, pPlayer->GetTeamId());
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	for (size_t i = 0; i < pTeamData->tBossData.vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pTeamData->tBossData.vMonsterList[i]);
	}

	return true;
}

// 战斗结束(Boss)
bool CTeamDungeonManager::OnBossBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
	if (pTeamData)
	{
		TeamMember *pMember = GetMemberData(pTeamData, pPlayer->GetRoleId()); //pTeamData->GetMemberData(pPlayer->GetRoleId());
		if (!pMember)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到队伍成员数据[TeamId:%d,RoleId:%d]!", __FUNCTION__,
				pPlayer->GetTeamId(), pPlayer->GetRoleId());
			return false;
		}

		TeamMapData &tBossData = pTeamData->tBossData;

		// 还没死亡
		if (!IsMonsterDead(tBossData.vMonsterList))
		{
			DWORD dwTotalDamage = 0;
			for (BYTE i = 0; i < byMonsterNum; ++i)
			{
				dwTotalDamage += pMonsterList[i].dwDamage;
			}
			pMember->dwContext += dwTotalDamage;

			MAPLOG(GUI_LEVEL, "[%s]total damage[TeamId:%d,RoleId:%d,Damage:%d]!", __FUNCTION__,
				pPlayer->GetTeamId(), pPlayer->GetRoleId(), dwTotalDamage);

			// 更新怪物伤害
			UpdateMonsterDamage(tBossData.vMonsterList, pMonsterList, byMonsterNum);

			// 广播怪物数据变动
			MSG_S2C_SCENE_MONSTER_DATA_NOTICE msg;
			msg.m_tMonster = tBossData;
			SendMsgToTeamMember(pTeamData, &msg);

			// 如果被击杀
			if (IsMonsterDead(tBossData.vMonsterList))
			{
				DWORD dwTeamId = pTeamData->dwTeamId;

				// 处理奖励
				DoBossReward(pTeamData, pPlayer->GetRoleId());

				m_BattleBossTeamList.erase(dwTeamId);
			}
		}

		// 死亡则回到复活点
		if (!byIsWin)
		{
			// 取任务数据
			const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
			if (!pTaskCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到任务数据[TaskId:%d]!", __FUNCTION__, pTeamData->dwCurTaskId);
				return false;
			}

			// 取场景配置
			const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
			if (pSceneCfg)
			{
				// 随机坐标
				MapPos tMapPos;
				RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
				pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

				// 广播给其他玩家,有人进入场景
				MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
				pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
				enter_notice.m_wScene = 0;
				enter_notice.m_tMapPos = pPlayer->GetMapPos();
				SendMsgToTeamMember(pTeamData, &enter_notice);
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到场景配置[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
			}
		}
	}

	return true;
}

// 物品锁定
void CTeamDungeonManager::OnNetLockItem(Player *pPlayer, DWORD dwSceneItemUId)
{
	WORD wFromMsgId = C2S_SCENE_ITEM_LOCK_REQ;

	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// 检测是否采集任务
	if (pTaskCfg->wTaskType != TASK_GATHER)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]当前不是采集任务[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	// 检测当前是否已在锁住其他宝箱
// 	if (IsLockBox(pPlayer->GetRoleId()))
// 	{
// 		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SCENE_ITEM_LOCK_REQ);
// 		MAPLOG(ERROR_LEVEL, "[%s]玩家已经锁住了其他宝箱[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
// 		return;
// 	}

	MSG_S2C_SCENE_ITEM_LOCK_RESP msg;
	msg.m_wScene = 0;

	// 宝箱是否存在
	SceneItem *pSceneItem = pTeamData->GetSceneItem(dwSceneItemUId);
	if (!pSceneItem)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]场景物品不存在或者已被拾取[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	DWORD dwCurTime = time(NULL);

	// 检测宝想是否已被锁住(延后1秒)
	if (pSceneItem->dwLockPlayerId && dwCurTime <= pSceneItem->dwLockTime + g_GetConfigValue(166) + 1)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]场景物已被锁住[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	pSceneItem->dwLockPlayerId = pPlayer->GetRoleId();
	pSceneItem->dwLockTime = dwCurTime;

	msg.m_dwUID = dwSceneItemUId;
	pPlayer->SendPacket(&msg);


	MSG_S2C_SCENE_ITEM_LOCK_NOTIFY lock_notify;
	lock_notify.m_dwUID = dwSceneItemUId;
	lock_notify.m_wScene = 0;
	lock_notify.m_dwRoleId = pPlayer->GetRoleId();
	SendMsgToTeamMember(pTeamData, &lock_notify, pPlayer->GetRoleId());
}

// 物品拾取
void CTeamDungeonManager::OnNetPickUpItem(Player *pPlayer, DWORD dwSceneItemUId)
{
	WORD wFromMsgId = C2S_SCENE_ITEM_PICK_UP_REQ;

	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId());// pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到成员数据[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// 宝箱是否存在
	SceneItem *pSceneItem = pTeamData->GetSceneItem(dwSceneItemUId);
	if (!pSceneItem)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]场景物品不存在或者已被拾取[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	// 检测锁住宝箱跟拾取的是否同一个玩家
	if (pSceneItem->dwLockPlayerId != pPlayer->GetRoleId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]锁住宝箱跟拾取宝箱不是同一个玩家[box_uid:%d,lock_id:%d,pick_up_id:%d]",
			__FUNCTION__, dwSceneItemUId, pSceneItem->dwLockPlayerId, pPlayer->GetRoleId());
		return;
	}

	// 检测CD时间
	if (time(NULL) < pSceneItem->dwLockTime + g_GetConfigValue(166))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]不能拾取,宝箱CD中...[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	DWORD dwInfoId = pSceneItem->dwInfoId;

	// 添加拾取数量
	pMemberData->dwContext++;
	pTeamData->wCurTaskValue++;

	// 删除锁宝箱状态
	//RemoveLockBoxPlayer(pPlayer->GetRoleId());	

	// 删除已被拾取的宝箱
	vector<SceneItem>::iterator iter = pTeamData->vSceneItemList.begin();
	for (; iter != pTeamData->vSceneItemList.end(); ++iter)
	{
		if (iter->dwUID == dwSceneItemUId)
		{
			pTeamData->vSceneItemList.erase(iter);
			break;
		}
	}

	// 检测任务是否已完成
	if (pTeamData->wCurTaskValue >= pTaskCfg->GetTaskParam(1))
	{
		DoFinishStageTask(*pTeamData, pTaskCfg);
	}

	// 响应(todo::为了配合客户端的表现,调整顺序)
	MSG_S2C_SCENE_ITEM_PICK_UP_RESP resp;
	resp.m_dwUID = dwSceneItemUId;
	resp.m_dwInfoId = dwInfoId;
	resp.m_vItemList.clear();
	pPlayer->SendPacket(&resp);

	// 通知其他人
	MSG_S2C_SCENE_ITEM_PICK_UP_NOTIFY pick_up_notify;
	pick_up_notify.m_dwUID = dwSceneItemUId;
	pick_up_notify.m_dwInfoId = dwInfoId;
	pick_up_notify.m_dwRoleId = pPlayer->GetRoleId();
	SendMsgToTeamMember(pTeamData, &pick_up_notify, pPlayer->GetRoleId());

	// 广播
 	MSG_S2C_SCENE_ITEM_DISAPPEAR_NOTICE notice;
 	notice.m_dwUID = dwSceneItemUId;
 	SendMsgToTeamMember(pTeamData, &notice);
}

// 快捷聊天
void CTeamDungeonManager::OnNetShortcutChat(Player *pPlayer, WORD wChatId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP msg;
	msg.m_dwRoleId = pPlayer->GetRoleId();
	msg.m_wChatId = wChatId;
	SendMsgToTeamMember(pTeamData, &msg);
}

// 请求排行榜列表
void CTeamDungeonManager::OnNetRankList(Player *pPlayer, WORD wDungeonId)
{
	MSG_S2C_TEAM_DUNGEON_RANK_RESP msg;
	msg.m_wRankdNum = 0;
	msg.m_wDungeonId = wDungeonId;
	map<WORD, vector<TeamDungeonRank>>::iterator dungeon_rank_iter = m_TeamDungeonRankList.find(wDungeonId);
	if (dungeon_rank_iter != m_TeamDungeonRankList.end())
	{
		vector<TeamDungeonRank>::iterator role_rank_iter = dungeon_rank_iter->second.begin();
		for (; role_rank_iter != dungeon_rank_iter->second.end(); ++role_rank_iter)
		{
			if (msg.m_wRankdNum >= MAX_TEAM_DUNGEON_NUM)
				break;

			msg.m_arRankList[msg.m_wRankdNum++] = *role_rank_iter;
		}
	}
	pPlayer->SendPacket(&msg);
}

// 请求抽奖记录
void CTeamDungeonManager::OnNetLotteryRecord(Player *pPlayer)
{
	MSG_S2C_TEAM_DUNGEON_LOTTERY_RECORD_RESP msg;
	msg.m_LotteryRecordList.clear();
	deque<TeamDungeonLotteryRecord>::iterator iter = m_LotteryRecordList.begin();
	for (; iter != m_LotteryRecordList.end(); ++iter)
	{
		msg.m_LotteryRecordList.push_back(*iter);
	}
	pPlayer->SendPacket(&msg);
}

// 回答问题
void CTeamDungeonManager::OnNetAnswerQuestion(Player *pPlayer, BYTE byIsRight, BYTE byAnswer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// 检测是否答题任务
	if (pTaskCfg->wTaskType != TASK_ANSWER)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当前不是答题任务[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId()); //  pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到成员数据[TeamId:%d,RoleId:%d]!",__FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// 检测是否已答过
	if (pMemberData->dwContext)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]成员已经答过题目[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	if (byIsRight)
		pTeamData->wCurTaskValue++;

	// 标记已答过题
	pMemberData->dwContext = time(NULL);

	// 检测是否所有人都答题了
	BYTE byIsEveryoneAnswer = true;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (!pTeamData->arMemberList[i].dwContext)
		{
			byIsEveryoneAnswer = false;
			break;
		}
	}

	// 记录答题成员数量
	pTeamData->byAnswerQustionMemberNum++;

	// 第一个人答题就开始计时
	if (1 == pTeamData->byAnswerQustionMemberNum)
		pTeamData->dwStageTaskStartTime = time(NULL);	

	// 广播有人答题
	MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY notify;
	notify.m_dwRoleId = pPlayer->GetRoleId();
	notify.m_byAnswer = byAnswer;
	SendMsgToTeamMember(pTeamData, &notify);

	// 所有人都已答题
	if (byIsEveryoneAnswer)
	{
		// 达到指定的人数答对,则任务完成
		if (pTeamData->wCurTaskValue >= MAX_TEAM_MEMBER_NUM-1)
		{
			DoFinishStageTask(*pTeamData, pTaskCfg);
		}
		// 如果答对人数不足,则重新出题目
		else
		{
			pTeamData->byAnswerTimes++;

			// 答题次数已达上限,进入下一任务
			if (pTeamData->byAnswerTimes >= 5)
			{
				DoFinishStageTask(*pTeamData, pTaskCfg);
			}
			else
			{
				OnNewTask(*pTeamData, false);
			}
		}
	}

	MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP resp;
	resp.m_byAnswer = byAnswer;
	resp.m_byIsRight = byIsRight;
	SendMsgToTeamMember(pTeamData, &resp);
}

// 对话结束
void CTeamDungeonManager::OnNetDialogOver(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// 检测是否对话在任务
	if (pTaskCfg->wTaskType != TASK_DIALOG)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当前不是对话任务[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	// 记录已经对话的NPC数量
	pTeamData->wCurTaskValue++; 

	// 检测任务是否完成
	if (pTeamData->wCurTaskValue >= pTaskCfg->vTaskParam.size())
	{
		DoFinishStageTask(*pTeamData, pTaskCfg);
	}

	MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY notity;
	SendMsgToTeamMember(pTeamData, &notity);

	// 响应
	MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 下一个NPC
void CTeamDungeonManager::OnNetNextNPC(Player *pPlayer, WORD wNpcId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY notify;
	notify.m_wNpcId = wNpcId;
	SendMsgToTeamMember(pTeamData, &notify);

	MSG_S2C_TEAM_DUNGEON_NEXT_NPC_RESP resp;
	resp.m_wNpcId = wNpcId;
	pPlayer->SendPacket(&resp);
}

// 组队邀请
void CTeamDungeonManager::OnNetInvite(Player *pPlayer, DWORD dwTargetId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到准备状态的队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 取目标数据
	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	// 没在队伍里,没在匹配池里,才会收到邀请消息
	if (pTarget && !pTarget->GetTeamId() && !IsInMatchList(pTarget))
	{
		// 通知目标
		MSG_S2C_TEAM_DUNGEON_INVITE_NOTIFY notify;
		notify.m_dwTeamId = dwTeamId;
		notify.m_wServerId = pPlayer->GetServerId();
		notify.m_wDungeonId = pTeamData->wDungeonId;
		strcpy_s(notify.m_szRoleName, sizeof(notify.m_szRoleName), pPlayer->GetRoleName());
		pTarget->SendPacket(&notify);
	}

	// 响应
	MSG_S2C_TEAM_DUNGEON_INVITE_RESP resp;
	resp.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&resp);
}

// 接受组队邀请
void CTeamDungeonManager::OnNetAcceptInvite(Player *pPlayer, DWORD dwTeamId)
{
	// 取被邀请的目标队伍
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_TEAM_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]找不到准备状态的队伍数据[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 检测是否已在队伍里
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (pTeamData->arMemberList[i]->GetRoleId() == pPlayer->GetRoleId())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]玩家已经是队伍成员[TeamId:%d,RoleId:%d]!", 
				__FUNCTION__, dwTeamId, pPlayer->GetRoleId());
			return;
		}
	}

	// 检测队伍成员是否已满
	if (pTeamData->wMemberNum >= MAX_TEAM_MEMBER_NUM)
	{
		// 需要告诉客户端
		pPlayer->SendErrorMsg(ERROR_TEAM_MEMBER_IS_FULL);
		MAPLOG(GUI_LEVEL, "[%s]队伍成员数量已满[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 检测是否在在队伍里
	if (pPlayer->GetTeamId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经在队伍里[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return; 
	}

	// 检测玩家自己是否已在匹配池里
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经在匹配池里[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 通知其他成员有人加入
	MSG_S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY notify;
	pPlayer->FillCommonData(&notify.m_NewMember);
	SendMsgToReadyTeamMember(pTeamData, &notify);

	// 添加新成员
	pTeamData->arMemberList[pTeamData->wMemberNum++] = pPlayer;

	pPlayer->SetTeamDungeonId(pTeamData->wDungeonId);
	pPlayer->SetTeamId(dwTeamId);

	MSG_S2C_TEAM_DUNGEON_ACCEPT_INVITE_RESP resp;
	resp.m_dwTeamId = dwTeamId;
	resp.m_wDungeonId = pTeamData->wDungeonId;
	resp.m_wMemberNum = pTeamData->wMemberNum;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i]->FillCommonData(&resp.m_MemberList[i]);
	}
	pPlayer->SendPacket(&resp);

	// 从大厅列表里去掉
	//RemoveHallPlayer(pPlayer);
}

// 加入准备队伍
void CTeamDungeonManager::OnNetJoinInReadyTeam(Player *pPlayer, DWORD dwTeamId, WORD wDungeonId)
{
	// 1、该队伍还在等待中，并且没满人——直接加入队伍，提示“成功加入队伍”
	// 2、该队伍还在等待中，但是已满人——提示“队伍已满人”；
	// 3、该队伍已进入副本——提示“找不到该队伍”；
	// 4、该队伍已解散——提示“找不到该队伍”；
	// 5、其它情况——提示“找不到该队伍”。

	// 检测是否在在队伍里
	if (pPlayer->GetTeamId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经在队伍里[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 检测玩家自己是否已在匹配池里
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经在匹配池里[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 检测队伍是否存在
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		// todo:提示队伍已不存在
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(GUI_LEVEL, "[%s]找不到该队伍[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 检测副本是否匹配
	if (wDungeonId != pTeamData->wDungeonId)
	{
		// todo:提示队伍已不存在
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(GUI_LEVEL, "[%s]找不到该队伍[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 取副本配置
	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(wDungeonId);
	if (!pDungeonCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到组队副本配置[DungeonId:%d]!", __FUNCTION__, wDungeonId);
		return;
	}

	// 检测等级是否可以挑战此副本
	if (pPlayer->GetLevel() < pDungeonCfg->wRecommendLevelMin)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]等级不足,不能挑战此副本[DungeonId:%d,NeedLevel:%d]!",
			__FUNCTION__, wDungeonId, pDungeonCfg->wRecommendLevelMin);
		return;
	}

	// 检测是否已在队伍里
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (pTeamData->arMemberList[i]->GetRoleId() == pPlayer->GetRoleId())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]玩家已经是队伍成员[TeamId:%d,RoleId:%d]!",
				__FUNCTION__, dwTeamId, pPlayer->GetRoleId());
			return;
		}
	}

	// 检测队伍成员是否已满
	if (pTeamData->wMemberNum >= MAX_TEAM_MEMBER_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_TEAM_MEMBER_IS_FULL);
		MAPLOG(GUI_LEVEL, "[%s]队伍成员数量已满[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 通知其他成员有人加入
	MSG_S2C_TEAM_DUNGEON_JOIN_IN_NOTIFY notify;
	pPlayer->FillCommonData(&notify.m_NewMember);
	SendMsgToReadyTeamMember(pTeamData, &notify);

	// 添加新成员
	pTeamData->arMemberList[pTeamData->wMemberNum++] = pPlayer;

	pPlayer->SetTeamDungeonId(pTeamData->wDungeonId);
	pPlayer->SetTeamId(dwTeamId);

	MSG_S2C_TEAM_DUNGEON_JOIN_IN_RESP resp;
	resp.m_dwTeamId = dwTeamId;
	resp.m_wDungeonId = pTeamData->wDungeonId;
	resp.m_wMemberNum = pTeamData->wMemberNum;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i]->FillCommonData(&resp.m_MemberList[i]);
	}
	pPlayer->SendPacket(&resp);
}

// 离开准备状态的队伍
void CTeamDungeonManager::OnNetLeaveReadyTeam(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 取队伍数据
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在队伍里[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 如果是队长,或者只有一个人, 则解散队伍
	if (pPlayer->GetRoleId() == dwTeamId || 1 == pTeamData->wMemberNum)
	{
		MSG_S2C_TEAM_DUNGEON_DISMISS_NOTIFY dismiss;
		dismiss.m_byIsCaptainLeave = pPlayer->GetRoleId() == dwTeamId ? true : false;
		SendMsgToReadyTeamMember(pTeamData, &dismiss);

		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			pTeamData->arMemberList[i]->SetTeamDungeonId(0);
			pTeamData->arMemberList[i]->SetTeamId(0);
		}

		RemoveReadyTeam(dwTeamId);
	}
	// 如果是成员 
	else
	{ 
		pPlayer->SetTeamDungeonId(0);
		pPlayer->SetTeamId(0);

		MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY leave_msg;
		leave_msg.m_dwRoleId = pPlayer->GetRoleId();
		leave_msg.m_byIsTaskTeam = false;
		SendMsgToReadyTeamMember(pTeamData, &leave_msg);

		RemoveReadyTeamMember(pTeamData, pPlayer->GetRoleId());
	}

	MSG_S2C_TEAM_DUNGEON_LEAVE_RESP resp;
	pPlayer->SendPacket(&resp);
}

// 请求玩家推荐列表
void CTeamDungeonManager::OnNetPlayerRecommendList(Player *pPlayer)
{
	ReadyTeamData *pTeamData = GetReadyTeamData(pPlayer->GetTeamId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到准备状态的队伍数据[TeamId:%d]!", __FUNCTION__, pPlayer->GetTeamId());
		return;
	}

	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(pPlayer->GetTeamDungeonId());
	if (!pDungeonCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到组队副本配置[TeamId:%d,DungeonId:%d]!", __FUNCTION__,
			pPlayer->GetTeamId(), pPlayer->GetTeamDungeonId());
		return;
	}

	WORD wMinLevel = pDungeonCfg->wRecommendLevelMin;

	// 推荐玩家数量
	int nRecommentPlayerNum = g_GetConfigValue(228, 20);
	nRecommentPlayerNum = min(nRecommentPlayerNum, MAX_TEAM_DUNGEON_RECOMMEND_LIST_NUM);

	MSG_S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP msg;
	msg.m_wPlayerNum = 0;
	
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.begin();
	for (; iter != m_HallPlayerList.end(); ++iter)
	{
		// 推荐人数已达上限
		if (msg.m_wPlayerNum >= nRecommentPlayerNum)
			break;

		Player *pTarget = iter->second;

		// 等级不足的忽略
		if (pTarget->GetLevel() < wMinLevel)
			continue;

		// 不要自己
		if (pTarget->GetRoleId() == pPlayer->GetRoleId())
			continue;

		// 已在准备队伍的不要,已经任务队伍的不要,已经进入匹配池的不要
		if (pTarget->GetTeamDungeonId() || pTarget->GetTeamId())
			continue;

		// 已在准备队伍的不要
// 		if (IsInReadyTeam(iter->second))
// 			continue;
// 
// 		// 已经任务队伍的不要
// 		if (IsInTaskTeam(iter->second))
// 			continue;

		iter->second->FillCommonData(&msg.m_PlayerList[msg.m_wPlayerNum++]);
	}

	pPlayer->SendPacket(&msg);
}


// 抽奖
void CTeamDungeonManager::OnNetLottery(Player *pPlayer, WORD wTimes, BYTE byIsCostItem)
{
	// 检测抽奖次数的合法性
	if (!wTimes || wTimes > MAX_TEAM_DUNGEON_LOTTERY_TIMES)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]抽奖次数不合法[times:%d]!", __FUNCTION__, wTimes);
		return;
	}

	DWORD dwCostItemId = g_GetConfigValue(211);
	int nCostDiamond = wTimes > 1 ? g_GetConfigValue(285) : g_GetConfigValue(284);	// 1.扭蛋一次消耗钻石 2.扭蛋十次消耗钻石

	// 是否消耗道具
	if (byIsCostItem)
	{
		// 检测消耗的物品
		if (pPlayer->GetItemNum(dwCostItemId) < wTimes)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]抽奖消耗的物品不足[ItemId:%d]!", __FUNCTION__, dwCostItemId);
			return;
		}
	}
	else
	{
		// 检测消耗的钻石
		if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]抽奖消耗的钻石不足[CostDiamond:%d]!", __FUNCTION__, nCostDiamond);
			return;
		}
	}

	// 取抽奖列表
	const vector<TeamDungeonLottery_Config> &lotteryList = g_pCfgMgr->GetTeamDungeonLotteryList();

	// 计算总的权值
	int nTotalWeight = 0;
	vector<TeamDungeonLottery_Config>::const_iterator iter = lotteryList.begin();
	for (; iter != lotteryList.end(); ++iter)
	{
		nTotalWeight += iter->nWeight;
	}

	// 处理抽中的逻辑
	vector<Item> vItemList;
	for (WORD i = 0; i < wTimes; ++i)
	{
		int nRandValue = g_Rand(1, nTotalWeight);
		int nCurWeight = 0;
		for (iter = lotteryList.begin(); iter != lotteryList.end(); ++iter)
		{
			nCurWeight += iter->nWeight;
			if (nCurWeight >= nRandValue)
			{
				vItemList.push_back(Item(iter->dwItemId, iter->dwItemNum));

				// 是否需要记录
				if (iter->byIsNeedRecord)
				{
					TeamDungeonLotteryRecord tLotteryRecord;
					memset(&tLotteryRecord, 0, sizeof(tLotteryRecord));
					tLotteryRecord.wServerId = pPlayer->GetServerId();
					tLotteryRecord.dwItemId = iter->dwItemId;
					tLotteryRecord.dwItemNum = iter->dwItemNum;
					strcpy_s(tLotteryRecord.szRoleName, sizeof(tLotteryRecord.szRoleName), pPlayer->GetRoleName());
					AddLotteryRecord(tLotteryRecord);
				}
				break;
			}
		}
	}

	if (byIsCostItem)
	{
		// 扣除消耗的物品
		pPlayer->RemoveItem(dwCostItemId, wTimes, FROM_TEAM_DUNGEON_ND_LOTTERY);
	}
	else
	{
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TEAM_DUNGEON_ND_LOTTERY);
	}

	// 添加抽中的物品
	pPlayer->AddObjectList(vItemList, TRUE, FROM_TEAM_DUNGEON_ND_LOTTERY);

	// 响应
	MSG_S2C_TEAM_DUNGEON_LOTTERY_RESP msg;
	msg.m_wTimes = wTimes;
	msg.m_wItemNum = 0;
	for (size_t i = 0; i < vItemList.size(); ++i)
	{
		if (msg.m_wItemNum >= MAX_TEAM_DUNGEON_LOTTERY_TIMES)
			break;

		msg.m_ItemList[msg.m_wItemNum++] = vItemList[i];
	}
	pPlayer->SendPacket(&msg);

	// 节日活动
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 36, wTimes, true);
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 36, wTimes, true);
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 36, wTimes, true);
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 36, wTimes, true);
}

// Boss任务开始
void CTeamDungeonManager::OnNetBossTaskStart(Player *pPlayer)
{
	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetRoleId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家不是队长不能进行下一个任务操作[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 取任务配置
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!", 
			__FUNCTION__, pPlayer->GetRoleId(), pTeamData->dwCurTaskId);
		return;
	}

	// 取任务链配置
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务链数据[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
		return;
	}

	// 检测是否可以挑战BOSS(条件1:任务数量已完成 2:任务时间已到达)
	if (pTeamData->wTaskFinishNum < pTaskLinkCfg->wTaskNum &&
		time(NULL) < pTeamData->dwTaskStartTime + pTaskLinkCfg->dwTaskLimitTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		DWORD dwPassTime = time(NULL) - pTeamData->dwTaskStartTime;
		MAPLOG(ERROR_LEVEL, "[%s]条件还没满足,不能挑战BOSS[TaskLinkId:%d,CurFinishNum:%d,NeedNum:%d,PassTime:%d,LimitTime:%d]!", 
			__FUNCTION__, pTeamData->wTaskLinkId, pTeamData->wTaskFinishNum, pTaskLinkCfg->wTaskNum, dwPassTime, pTaskLinkCfg->dwTaskLimitTime);
		return;
	}

	// 战斗开始
	if (!CreateBoss(pTeamData))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]创建组队BOSS失败[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY notify;
	SendMsgToTeamMember(pTeamData, &notify);

	MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP resp;
	pPlayer->SendPacket(&resp);
}

// 接受任务
void CTeamDungeonManager::OnNetAcceptTask(Player *pPlayer)
{
	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetRoleId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到玩家队伍数据[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY notity;
	SendMsgToTeamMember(pTeamData, &notity);

	MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP resp;
	pPlayer->SendPacket(&resp);
}

// 任务开始(主要针对自己组队成功的情况)
void CTeamDungeonManager::OnNetTaskStart(Player *pPlayer)
{
	// 取被邀请的目标队伍
	ReadyTeamData *pReadyTeamData = GetReadyTeamData(pPlayer->GetRoleId());
	if (!pReadyTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到准备状态的队伍数据[TeamId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 填充成员列表
	vector<Player*> vPlayerList;
	for (WORD i = 0; i < pReadyTeamData->wMemberNum; ++i)
	{
		vPlayerList.push_back(pReadyTeamData->arMemberList[i]);
	}

	// 创建队伍
	if (!CreateTaskTeam(pReadyTeamData->wDungeonId, vPlayerList))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]创建队伍失败[TeamId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 取队伍数据
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetRoleId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到队伍数据[TeamId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

 	// 任务开始通知
	MSG_S2C_TEAM_DUNGEON_TASK_START_NOTIFY task_start_notify;
	task_start_notify.m_dwTaskLinkId = pTeamData->wTaskLinkId;
	task_start_notify.m_dwTastStartTime = time(NULL);
	SendMsgToTeamMember(pTeamData, &task_start_notify);

	MSG_S2C_TEAM_DUNGEON_TASK_START_RESP resp;
	pPlayer->SendPacket(&resp);

	// 从准备队伍中删掉
	RemoveReadyTeam(pPlayer->GetRoleId());
}

// 请求组队副本数据
void CTeamDungeonManager::OnNetTeamDungeonData(Player *pPlayer)
{
	PlayerTeamDungeonData &tTeamDungeonData = pPlayer->GetRoleFullInfo().tSecondInfo.tTeamDungeonData;

	MSG_S2C_TEAM_DUNGEON_DATA_RESP msg;
	msg.m_tTeamDungeonData = tTeamDungeonData;
	pPlayer->SendPacket(&msg);
}

// 创建队伍
void CTeamDungeonManager::OnNetCreate(Player *pPlayer, WORD wDungeonId)
{
	// 取副本配置
	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(wDungeonId);
	if (!pDungeonCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到组队副本配置[DungeonId:%d]!", __FUNCTION__, wDungeonId);
		return;
	}

	// 检测等级是否可以挑战此副本
	if (pPlayer->GetLevel() < pDungeonCfg->wRecommendLevelMin)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]等级不足,不能挑战此副本[DungeonId:%d,NeedLevel:%d]!", 
			__FUNCTION__, wDungeonId, pDungeonCfg->wRecommendLevelMin);
		return;
	}

	DWORD dwTeamId = pPlayer->GetTeamId();

	// 检测是否已有队伍
	if (dwTeamId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已有队伍[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// 检测是否已在查找列表里
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已在查找列表里[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	dwTeamId = pPlayer->GetRoleId();

	pPlayer->SetTeamId(dwTeamId);
	pPlayer->SetTeamDungeonId(wDungeonId);

	ReadyTeamData tTeamData;
	memset(&tTeamData, 0, sizeof(tTeamData));
	tTeamData.wDungeonId = wDungeonId;
	tTeamData.arMemberList[tTeamData.wMemberNum++] = pPlayer;
	AddReadyTeamData(dwTeamId, tTeamData);

	// 响应
	MSG_S2C_TEAM_DUNGEON_CREATE_TEAM_RESP msg;
	msg.m_wDungeonId = wDungeonId;
	msg.m_dwTeamId = dwTeamId;
	pPlayer->SendPacket(&msg);	

	// 离开大厅
	// DoLeaveHall(pPlayer);
}

// 查找队友
void CTeamDungeonManager::OnNetMatch(Player *pPlayer, WORD wDungeonId, BYTE byIsTeam)
{
	// 注意规则:
	// 1.队伍进入匹配状态以后,不能再邀请人
	// 2.取消匹配后回到组队状态

	DWORD dwTeamId = pPlayer->GetTeamId();

	// 检测是否已在任务中
	if (!byIsTeam && IsInReadyTeam(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已在准备队伍中[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// 检测是否已在任务中
	if (IsInTaskTeam(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已在任务队伍中[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// 检测是否已在匹配池中
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已在查找列表里[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	vector<Player*> vPlayerList;
	vector<MatchTeamData*> vTeamList;

	// 队伍查找队友
	if (byIsTeam)
	{
		// 检测是否在队伍里
		if (!dwTeamId)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]没有在队伍里,不能进行队伍匹配[DungeonId:%d,RoleId:%d]!",
				__FUNCTION__, wDungeonId, pPlayer->GetRoleId());
			return;
		}

		// 检测是否队长
		if (dwTeamId != pPlayer->GetRoleId())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]不是队长,不能进行队伍匹配[DungeonId:%d,RoleId:%d]!",
				__FUNCTION__, wDungeonId, pPlayer->GetRoleId());
			return;
		}

		// 自己的队伍成员数量
		ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
		if (!pTeamData)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]找不到准备中的队伍数据[TeamId:%d]!",__FUNCTION__, dwTeamId);
			return; 
		}

		// 成员已满
		if (pTeamData->wMemberNum >= MAX_TEAM_MEMBER_NUM)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]准备中的队伍成员已满[TeamId:%d]!", __FUNCTION__, dwTeamId);
			return;
		}

		WORD wNeedMemberNum = MAX_TEAM_MEMBER_NUM - pTeamData->wMemberNum;

		// 匹配到足人数
		vector<Player*> vOnePersonList;
		if (FindMatchTarget(wDungeonId, wNeedMemberNum, vTeamList, vOnePersonList))
		{
			// 目标队伍(先进的队伍成为队长)
			for (size_t i = 0; i < vTeamList.size(); ++i)
			{
				MatchTeamData *pTeamData = vTeamList[i];
				for (WORD k = 0; k < pTeamData->wMemberNum; ++k)
				{
					vPlayerList.push_back(pTeamData->arMemberList[k]);
				}
			}

			// 自己队伍
			for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
			{
				vPlayerList.push_back(pTeamData->arMemberList[i]);
			}

			// 散人
			vector<Player*>::iterator iter = vOnePersonList.begin();
			for (; iter != vOnePersonList.end(); ++iter)
			{
				vPlayerList.push_back(*iter);
			}
		}
		// 匹配不到人,则进入待匹配列表
		else
		{
			MatchTeamData tTeamData;
			memset(&tTeamData, 0, sizeof(tTeamData));
			tTeamData.dwTeamId = dwTeamId;
			for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
			{
				tTeamData.arMemberList[tTeamData.wMemberNum++] = pTeamData->arMemberList[i];
			}

			map<DWORD, MatchTeamData> *pTeamList = GetTeamSearchList(wDungeonId);
			if (pTeamList)
			{
				pTeamList->insert(make_pair(tTeamData.dwTeamId, tTeamData));
			}
			else
			{
				map<DWORD, MatchTeamData> teamList;
				teamList.insert(make_pair(tTeamData.dwTeamId, tTeamData));
				m_TeamMatchList.insert(make_pair(wDungeonId, teamList));
			}

			// 匹配通知
			MSG_S2C_TEAM_DUNGEON_MATCH_NOTIFY match_notify;
			SendMsgToReadyTeamMember(pTeamData, &match_notify);
		}

		// 删除准备中的队伍
		RemoveReadyTeam(dwTeamId);
	}
	// 散人查找队友
	else
	{
		// 匹配到足够人数
		if (FindMatchTarget(wDungeonId, MAX_TEAM_MEMBER_NUM-1, vTeamList, vPlayerList))
		{
			for (size_t i = 0; i < vTeamList.size(); ++i)
			{
				MatchTeamData *pTeamData = vTeamList[i];
				for (WORD k = 0; k < pTeamData->wMemberNum; ++k)
				{
					vPlayerList.push_back(pTeamData->arMemberList[k]);
				}
			}

			vPlayerList.push_back(pPlayer);
		}
		// 匹配不到人,则进入待匹配列表
		else
		{
			pPlayer->SetTeamDungeonId(wDungeonId);
			map<DWORD, Player*> *pMatchList = GetPersonSearchList(wDungeonId);
			if (pMatchList)
			{
				pMatchList->insert(make_pair(pPlayer->GetRoleId(), pPlayer));
			}
			else
			{
				map<DWORD, Player*> playerList;
				playerList.insert(make_pair(pPlayer->GetRoleId(), pPlayer));
				m_PersonMatchList.insert(make_pair(wDungeonId, playerList));
			}
		}
	}

	// 检测匹配到人数是否合法
	if (vPlayerList.size() > MAX_TEAM_MEMBER_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]匹配到的人数超过指定数量[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// 人数已满
	if (vPlayerList.size() == MAX_TEAM_MEMBER_NUM)
	{
		vector<Player*> vTeamMemberList;

		// 散人匹配并匹配到的全都是散人
		if (!byIsTeam && !vTeamList.size())
		{
			// 随机队长
			int nRandIdx = g_Rand(0, vPlayerList.size() - 1);
			vTeamMemberList.push_back(vPlayerList[nRandIdx]);
			for (int i = 0; i < vPlayerList.size(); ++i)
			{
				// 不用添加已经随机成为队长的玩家
				if (nRandIdx != i)
					vTeamMemberList.push_back(vPlayerList[i]);
			}
		}
		else
		{
			vTeamMemberList = vPlayerList;
		}

		if (!CreateTaskTeam(wDungeonId, vTeamMemberList))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]创建任务队伍失败[TeamId:%d]!", __FUNCTION__, dwTeamId);
			return;
		}

		// 删除已经被匹配到的(队伍)
		map<DWORD, MatchTeamData> *pTeamSearchList = GetTeamSearchList(wDungeonId);
		if (pTeamSearchList)
		{
			vector<MatchTeamData*>::iterator iter = vTeamList.begin();
			for (; iter != vTeamList.end(); ++iter)
			{
				pTeamSearchList->erase((*iter)->dwTeamId);
			}
		}

		// 删除已经被匹配到的(个人)
		map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
		if (pPersonSearchList)
		{
			vector<Player*>::iterator iter = vPlayerList.begin();
			for (; iter != vPlayerList.end(); ++iter)
			{
				pPersonSearchList->erase((*iter)->GetRoleId());
			}
		}
	}

	MSG_S2C_TEAM_DUNGEON_MATCH_RESP msg;
	msg.m_wDungeonId = wDungeonId;
	msg.m_byIsTeam = byIsTeam;
	pPlayer->SendPacket(&msg);

	// 广播匹配目标数量
	BroadcastMatchTargetNum(wDungeonId);
}

// 取消匹配队友
void CTeamDungeonManager::OnNetCancelMatch(Player *pPlayer)
{
	WORD wDungeonId = pPlayer->GetTeamDungeonId();
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 队伍匹配池(只要有人取消,整个队伍都会返回组队状态)
	if (dwTeamId)
	{
		DoTeamCancelMatch(wDungeonId, dwTeamId);
	}
	// 个人匹配池
	else
	{
		map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(wDungeonId);
		if (iter != m_PersonMatchList.end())
		{
			iter->second.erase(pPlayer->GetRoleId());
		}

		pPlayer->SetTeamDungeonId(0);
		pPlayer->SetTeamId(0);
	}

	MSG_S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP msg;
	pPlayer->SendPacket(&msg);

	// 广播匹配目标数量
	BroadcastMatchTargetNum(wDungeonId);
}

// 离开场景
void CTeamDungeonManager::DoLeaveHall(Player *pPlayer)
{
	// 广播给其他人,有人退出
	MSG_S2C_LEAVE_SCENE_NOTICE notice;
	notice.m_dwRoleId = pPlayer->GetRoleId();
	SendToHallPlayer(&notice);

	RemoveHallPlayer(pPlayer);
}

// 是否在场景
bool CTeamDungeonManager::IsInHall(DWORD dwRoleId)
{
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.find(dwRoleId);
	return iter != m_HallPlayerList.end() ? true : false;
}

// 取个人查找数量
DWORD CTeamDungeonManager::GetPersonSearchListNum(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(dwDungeonId);
	return iter != m_PersonMatchList.end() ? iter->second.size() : 0;
}

// 取队伍查找列表数量
DWORD CTeamDungeonManager::GetTeamSearchListNum(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
	return iter != m_TeamMatchList.end() ? iter->second.size() : 0;
}

// 取队伍查找列表数量
DWORD CTeamDungeonManager::GetTeamSearchListNum(DWORD dwDungeonId, WORD wTeamMemberNum)
{
	DWORD dwListNum = 0;
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
	if (iter != m_TeamMatchList.end())
	{
		map<DWORD, MatchTeamData>::iterator iter2 = iter->second.begin();
		for (; iter2 != iter->second.end(); ++iter2)
		{
			if (iter2->second.wMemberNum == wTeamMemberNum)
				dwListNum++;
		}
	}
	return dwListNum;
}

// 新任务
bool CTeamDungeonManager::OnNewTask(TaskTeamData &tTeamData, BYTE byIsNewTask)
{
	if (CreateTask(tTeamData, byIsNewTask))
	{
// 		if (byIsNewTask)
// 			tTeamData.wTaskFinishNum++;
		// 通知新的任务开始
		MSG_S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY new_task;
		new_task.m_dwTaskId = tTeamData.dwCurTaskId;
		new_task.m_wQuestionId = tTeamData.wQuestionId;
		new_task.m_byIsNewTask = byIsNewTask;
		SendMsgToTeamMember(&tTeamData, &new_task);

		return true;
	}

	return false;
}

// 发送消息匹配队伍成员
void CTeamDungeonManager::SendMsgToMatchTeamMember(MatchTeamData *pTeamData, NetMsgBody *pMsg)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i]->SendPacket(pMsg);
	}
}

// 添加场景玩家
void CTeamDungeonManager::AddHallPlayer(Player *pPlayer)
{
	m_HallPlayerList.insert(std::make_pair(pPlayer->GetRoleId(), pPlayer));
}

// 删除场景玩家
void CTeamDungeonManager::RemoveHallPlayer(Player *pPlayer)
{
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.find(pPlayer->GetRoleId());
	if (iter != m_HallPlayerList.end())
	{
		m_HallPlayerList.erase(iter);
	}
}

// 广播消息
void CTeamDungeonManager::SendToHallPlayer(NetMsgBody *pMsg)
{
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.begin();
	for (; iter != m_HallPlayerList.end(); ++iter)
	{
		iter->second->SendPacket(pMsg);
	}
}

// 取队伍查找列表
map<DWORD, MatchTeamData>* CTeamDungeonManager::GetTeamSearchList(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
	return iter != m_TeamMatchList.end() ? &iter->second : NULL;
}

// 取散人查找列表
map<DWORD, Player*>* CTeamDungeonManager::GetPersonSearchList(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(dwDungeonId);
	return iter != m_PersonMatchList.end() ? &iter->second : NULL;
}

// 取匹配中的队伍数据
vector<MatchTeamData*> CTeamDungeonManager::GetTeamSearchData(DWORD dwDungeonId, WORD wMemberNum)
{
	vector<MatchTeamData*> vResultList;
	map<DWORD, MatchTeamData> *pSearchTeamList = GetTeamSearchList(dwDungeonId);
	if (pSearchTeamList)
	{
		map<DWORD, MatchTeamData>::iterator iter = pSearchTeamList->begin();
		for (; iter != pSearchTeamList->end(); ++iter)
		{
			if (iter->second.wMemberNum == wMemberNum)
				vResultList.push_back(&iter->second);
		}
	}
	return vResultList;
}

// 删除匹配中的队伍数据
void CTeamDungeonManager::RemoveTeamSearchData(DWORD dwDungeonId, DWORD dwTeamId)
{
	map<DWORD, MatchTeamData> *pSearchTeamList = GetTeamSearchList(dwDungeonId);
	if (pSearchTeamList)
	{
		map<DWORD, MatchTeamData>::iterator iter = pSearchTeamList->find(dwTeamId);
		if (iter != pSearchTeamList->end())
		{
			pSearchTeamList->erase(iter);
		}
	}
}

// 添加准备队伍数据
void CTeamDungeonManager::AddReadyTeamData(DWORD dwTeamId, const ReadyTeamData &tTeamData)
{
	m_ReadyTeamList.insert(make_pair(dwTeamId, tTeamData));
}

// 取队伍数据
ReadyTeamData* CTeamDungeonManager::GetReadyTeamData(DWORD dwTeamId)
{
	map<DWORD, ReadyTeamData>::iterator iter = m_ReadyTeamList.find(dwTeamId);
	return iter != m_ReadyTeamList.end() ? &iter->second : NULL;
}

// 添加任务队伍数据
void CTeamDungeonManager::AddTaskTeamData(DWORD dwTeamId, const TaskTeamData &tTeamData)
{
	m_TaskTeamList.insert(make_pair(dwTeamId, tTeamData));
}

// 取任务队伍数据
TaskTeamData* CTeamDungeonManager::GetTaskTeamData(DWORD dwTeamId)
{
	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(dwTeamId);
	return iter != m_TaskTeamList.end() ? &iter->second : NULL;
}

// 创建任务
bool CTeamDungeonManager::CreateTask(TaskTeamData &tTeamData, BYTE byIsNewTask)
{
	// 取任务链数据
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(tTeamData.wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务链数据[TaskLinkId:%d]!", __FUNCTION__, tTeamData.wTaskLinkId);
		return false;
	}

	// 检测是否所有的任务已完成
	if (tTeamData.wTaskFinishNum >= pTaskLinkCfg->wTaskNum || tTeamData.wTaskFinishNum >= pTaskLinkCfg->vTaskList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]整个任务链都已完成[TeamId:%d,TaskLinkId:%d]!",
			__FUNCTION__, tTeamData.dwTeamId, tTeamData.wTaskLinkId);
		return false;
	}

	// 如果是新任务
	if (byIsNewTask)
	{
		const vector<DWORD> &vTaskRankList = pTaskLinkCfg->vTaskList[tTeamData.wTaskFinishNum];
		if (!vTaskRankList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]任务随机列表为空[TeamId:%d,TaskLinkId:%d]!",
				__FUNCTION__, tTeamData.dwTeamId, tTeamData.wTaskLinkId);
			return false;
		}
		//tTeamData.wTaskFinishNum++;
		int nRandIdx = g_Rand(0, vTaskRankList.size() - 1);
		tTeamData.dwCurTaskId = vTaskRankList[nRandIdx];

		MAPLOG(ERROR_LEVEL, "[%s]随机任务信息[TeamId:%d,TaskLinkId:%d,CurTaskId:%d]!",
			__FUNCTION__, tTeamData.dwTeamId, tTeamData.wTaskLinkId, tTeamData.dwCurTaskId);
	}

	tTeamData.vMapList.clear();
	tTeamData.vSceneItemList.clear();
	tTeamData.wCurTaskValue = 0;
	tTeamData.dwStageTaskStartTime = time(NULL);
	tTeamData.byIsAllStageTaskFinish = false;
	tTeamData.byAnswerQustionMemberNum = 0;

	for (WORD i = 0; i < tTeamData.wMemberNum; ++i)
	{
		tTeamData.arMemberList[i].dwContext = 0;
		tTeamData.arMemberList[i].dwBattleTargetId = 0;
	}

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(tTeamData.dwCurTaskId);
	if (!pTaskCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务数据[TaskId:%d]!", __FUNCTION__, tTeamData.dwCurTaskId);
		return false;
	}

	// 打怪
	if (TASK_MONSTER == pTaskCfg->wTaskType)
	{
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
		if (!pSceneCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到场景配置[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
			return false;
		}

		TeamMapData tMapData;
		DWORD dwMapId = pTaskCfg->GetTaskParam(0);
		DWORD dwMapNum = pTaskCfg->GetTaskParam(1);
		for (DWORD i = 0; i < dwMapNum; ++i)
		{
			tMapData.dwUID = i + 1;
			tMapData.dwMapId = dwMapId;
			tMapData.dwStartBattleTime = 0;
			RandMapPos(tMapData.tMapPos, pSceneCfg->vMonsterRefrshArea);
			tMapData.vMonsterList.clear();
			if (!g_pCfgMgr->GetMapMonsterList(tMapData.dwMapId, &tMapData.vMonsterList))
			{
				MAPLOG(ERROR_LEVEL, "[%s]取组队副本怪物列表失败[MapId:%d]!", __FUNCTION__, tMapData.dwMapId);
				return false;
			}
			tTeamData.vMapList.push_back(tMapData);

			MAPLOG(ERROR_LEVEL, "[%s]创建场景怪物[SceneId:%d,Monster:%d]!",
				__FUNCTION__, pTaskCfg->dwSceneId, dwMapId);
		}
	}
	// 采集
	else if (TASK_GATHER == pTaskCfg->wTaskType)
	{
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
		if (!pSceneCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到场景配置[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
			return false;
		}

		SceneItem tSceneItem;
		DWORD dwItemId = pTaskCfg->GetTaskParam(0);
		DWORD dwItemNum = pTaskCfg->GetTaskParam(1);
		for (DWORD i = 0; i < dwItemNum; ++i)
		{
			tSceneItem.dwUID = i + 1;
			tSceneItem.dwInfoId = dwItemId;
			tSceneItem.dwCreateTime = time(NULL);
			tSceneItem.dwLockPlayerId = 0;
			tSceneItem.dwLockTime = 0;
			RandMapPos(tSceneItem.tMapPos, pSceneCfg->vItemRefrshArea);
			tTeamData.vSceneItemList.push_back(tSceneItem);

			MAPLOG(ERROR_LEVEL, "[%s]创建场景物品[SceneId:%d,ItemId:%d]!", 
				__FUNCTION__, pTaskCfg->dwSceneId, dwItemId);
		}
	}
	// 答题
	else if (TASK_ANSWER == pTaskCfg->wTaskType)
	{
		if (!pTaskCfg->vTaskParam.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]任务参数列表为空[TaskId:%d]!", __FUNCTION__, tTeamData.dwCurTaskId);
			return false;
		}

		int nRandIdx = g_Rand(0, pTaskCfg->vTaskParam.size() - 1);
		tTeamData.wQuestionId = pTaskCfg->vTaskParam[nRandIdx];

		if (byIsNewTask)
			tTeamData.byAnswerTimes = 0;
	}
	// 对话
	else if (TASK_DIALOG == pTaskCfg->wTaskType)
	{
		// todo::暂时不需要处理
	}

	tTeamData.wCurTaskType = pTaskCfg->wTaskType;

	return true;
}

// 是否怪物死亡
bool CTeamDungeonManager::IsMonsterDead(vector<MonsterData> &vMonsterList)
{
	for (size_t i = 0; i < vMonsterList.size(); ++i)
	{
		if (vMonsterList[i].dwDamage < vMonsterList[i].dwTotalHP)
			return false;
	}
	return true;
}

// 更新怪物伤害
void CTeamDungeonManager::UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		vector<MonsterData>::iterator iter = vSrcMonsterList.begin();
		for (; iter != vSrcMonsterList.end(); ++iter)
		{
			MonsterData &tMonster = pMonsterList[i];
			if (tMonster.byIndex == iter->byIndex)
			{
				iter->dwDamage += tMonster.dwDamage;

				// 伤害纠正
				if (iter->dwTotalHP && iter->dwDamage > iter->dwTotalHP)
					iter->dwDamage = iter->dwTotalHP;
			}
		}
	}
}


// 处理完成阶段任务完成
void CTeamDungeonManager::DoFinishStageTask(TaskTeamData &tTeamData, const TeamDungeonTask_Config *pTaskCfg)
{
	DWORD dwUseTime = time(NULL) - tTeamData.dwStageTaskStartTime;
	int nIndex = pTaskCfg->GetScoreIndex(dwUseTime);
	DWORD dwItemId = pTaskCfg->GetRewardItem(nIndex);
	DWORD dwEnergy = pTaskCfg->GetEnergy(nIndex);

	tTeamData.dwEnergy += dwEnergy;

	vector<DWORD> vExtraRewardMemberList;
	GetExtraRewardMemberList(pTaskCfg->wTaskType, &tTeamData, vExtraRewardMemberList);

	MSG_S2S_TEAM_DUNDEON_TASK_REWARD_NOTIFY reward_notify;
	reward_notify.m_dwEnergy = tTeamData.dwEnergy;

	// 处理任务奖励
	for (WORD i = 0; i < tTeamData.wMemberNum; ++i)
	{
		TeamMember &tMember = tTeamData.arMemberList[i];
		if (tMember.pPlayer)
		{
			// 普通奖励
			reward_notify.m_vItemList.clear();
			reward_notify.m_vExtraRewardList.clear();
			CItemManager::UseRandItem(dwItemId, 1, reward_notify.m_vItemList);
			tMember.pPlayer->AddObjectList(reward_notify.m_vItemList, TRUE, FROM_TEAM_DUNGEON_FINISH_STAGE_TASK);
			

			// 检测是否获得额外奖励
			vector<DWORD>::iterator iter = vExtraRewardMemberList.begin();
			for (; iter != vExtraRewardMemberList.end(); ++iter)
			{
				if (tMember.pPlayer->GetRoleId() == *iter)
				{
					CItemManager::UseRandItem(pTaskCfg->dwExtraRewardId, 1, reward_notify.m_vExtraRewardList);
					tMember.pPlayer->AddObjectList(reward_notify.m_vExtraRewardList, TRUE, FROM_TEAM_DUNGEON_FINISH_STAGE_TASK);
// 					MAPLOG(ERROR_LEVEL, "[%s]组队副本额外奖励[RoleId:%d,ExtraRewardId:%d, ExtraRewardList:%d]!",
// 						__FUNCTION__, tMember.pPlayer->GetRoleId(), pTaskCfg->dwExtraRewardId, reward_notify.m_vExtraRewardList.size());
					break;
				}
			}

			tMember.pPlayer->SendPacket(&reward_notify);
		}
	}

	//增加完成数量
	tTeamData.wTaskFinishNum++;

	// 取任务链数据
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(tTeamData.wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务链数据[TaskLinkId:%d]!", __FUNCTION__, tTeamData.wTaskLinkId);
		return;
	}

	// 整个任务链已完成
	if (tTeamData.wTaskFinishNum >= pTaskLinkCfg->wTaskNum || tTeamData.wTaskFinishNum >= pTaskLinkCfg->vTaskList.size())
	{
		tTeamData.byIsAllStageTaskFinish = true;

		MSG_S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY link_finish_notify;
		SendMsgToTeamMember(&tTeamData, &link_finish_notify);
	}
	// 单个任务完成
	else
	{
		// 创建新的任务
		OnNewTask(tTeamData, true);
	}

	MAPLOG(ERROR_LEVEL, "[%s]阶段任务结束[LinkId:%d,TaskId:%d,TaskFinishNum:%d]!",
		__FUNCTION__, tTeamData.wTaskLinkId, tTeamData.dwCurTaskId, tTeamData.wTaskFinishNum);
}

// 删除任务中的队伍
void CTeamDungeonManager::RemoveTeamTask(DWORD dwTeamId)
{
	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(dwTeamId);
	if (iter != m_TaskTeamList.end())
	{
		m_TaskTeamList.erase(iter);
	}
}

// 随机地图坐标点
bool CTeamDungeonManager::RandMapPos(MapPos &tMapPos, vector<DWORD> vRankPosList)
{
	if (4 == vRankPosList.size())
	{
		tMapPos.wMapX = g_Rand(vRankPosList[0], vRankPosList[1]);
		tMapPos.wMapY = g_Rand(vRankPosList[2], vRankPosList[3]);
	}
	else if (2 == vRankPosList.size())
	{
		tMapPos.wMapX = vRankPosList[0];
		tMapPos.wMapY = vRankPosList[1];
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]组队随机坐标配置出错", __FUNCTION__);
		return false;
	}

	return true;
}

// 发送消息队伍成员
void CTeamDungeonManager::SendMsgToTeamMember(TaskTeamData *pTeamData, NetMsgBody *pMsg, DWORD dwMemberId)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		Player *pPlayer = pTeamData->arMemberList[i].pPlayer;

		if (!dwMemberId || (dwMemberId && pPlayer->GetRoleId() != dwMemberId))
			pPlayer->SendPacket(pMsg);
	}
}

// 创建任务队伍
bool CTeamDungeonManager::CreateTaskTeam(WORD wDungeonId, vector<Player*> &vPlayerList)
{
	// 取副本配置
	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(wDungeonId);
	if (!pDungeonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到组队副本配置[DungeonId:%d]", __FUNCTION__, wDungeonId);
		return false;
	}

	// 检测任务链
	if (!pDungeonCfg->vTaskList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]组队副本任务链列表配置为空[DungeonId:%d]", __FUNCTION__, wDungeonId);
		return false;
	}

	// 检测成员数量
	if (vPlayerList.size() != MAX_TEAM_MEMBER_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]队伍成员数量不足,不能创建![DungeonId:%d,PlayerNum:%d]",
			__FUNCTION__, wDungeonId, vPlayerList.size());
		return false;
	}

	// 列表第一个玩家成为队长
	DWORD dwTeamId = vPlayerList[0]->GetRoleId();

	TaskTeamData tTeamData;
	tTeamData.dwTeamId = dwTeamId;
	tTeamData.wDungeonId = wDungeonId;
	tTeamData.dwTaskStartTime = time(NULL);
	for (size_t i = 0; i < vPlayerList.size(); ++i)
	{
		vPlayerList[i]->SetTeamId(dwTeamId);
		vPlayerList[i]->SetTeamDungeonId(wDungeonId);
		tTeamData.arMemberList[tTeamData.wMemberNum++].pPlayer = vPlayerList[i];
	}

	AddTaskTeamData(tTeamData.dwTeamId, tTeamData);

	TaskTeamData *pTeamData = GetTaskTeamData(tTeamData.dwTeamId);
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务队伍数据[TeamId:%d]", __FUNCTION__, tTeamData.dwTeamId);
		return false;
	}

	// 随机任务链Id
	int nRandIdx = g_Rand(0, pDungeonCfg->vTaskList.size()-1);
	pTeamData->wTaskLinkId = pDungeonCfg->vTaskList[nRandIdx];

	// 广播队伍任务数据
	MSG_S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY team_data;
	team_data.m_dwTastStartTime = pTeamData->dwTaskStartTime;
	team_data.m_wTaskLinkId = pTeamData->wTaskLinkId;
	team_data.m_wMemberNum = pTeamData->wMemberNum;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i].pPlayer->FillCommonData(&team_data.m_MemberList[i]);
	}
	SendMsgToTeamMember(pTeamData, &team_data);

	// 创建任务
 	CreateTask(*pTeamData);
 
 	// 通知新的任务开始
 	MSG_S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY new_task;
	new_task.m_dwTaskId = pTeamData->dwCurTaskId;
 	new_task.m_wQuestionId = pTeamData->wQuestionId;
 	new_task.m_byIsNewTask = true;
 	SendMsgToTeamMember(pTeamData, &new_task);

	// 离开大厅
	for (size_t i = 0; i < vPlayerList.size(); ++i)
	{
		DoLeaveHall(vPlayerList[i]);
	}

	return true;
}

// 处理离开准备中队伍
void CTeamDungeonManager::DoLeaveReadyTeam(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	map<DWORD, ReadyTeamData>::iterator iter = m_ReadyTeamList.find(dwTeamId);

	// 没在队伍中
	if (iter == m_ReadyTeamList.end())
		return;

	ReadyTeamData *pTeamData = &iter->second;

	// 如果是队长,或者只有一个人, 则解散队伍
	if (pPlayer->GetRoleId() == dwTeamId || 1 == pTeamData->wMemberNum)
	{
		MSG_S2C_TEAM_DUNGEON_DISMISS_NOTIFY dismiss;
		dismiss.m_byIsCaptainLeave = pPlayer->GetRoleId() == dwTeamId ? true : false;
		SendMsgToReadyTeamMember(pTeamData, &dismiss);

		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			pTeamData->arMemberList[i]->SetTeamDungeonId(0);
			pTeamData->arMemberList[i]->SetTeamId(0);
		}

		RemoveReadyTeam(dwTeamId);
	}
	// 如果是成员 
	else
	{
		// 广播有人离开
		MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY leave_msg;
		leave_msg.m_dwRoleId = pPlayer->GetRoleId();
		leave_msg.m_byIsTaskTeam = false;
		SendMsgToReadyTeamMember(pTeamData, &leave_msg);

		RemoveReadyTeamMember(pTeamData, pPlayer->GetRoleId());

		pPlayer->SetTeamDungeonId(0);
		pPlayer->SetTeamId(0);
	}


	// 如果有两个人以上,则需要通知其他人
// 	if (pTeamData->wMemberNum > 1)
// 	{
// 		// 删除离开的成员
// 		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
// 		{
// 			Player *pMember = pTeamData->arMemberList[i];
// 			if (pMember->GetRoleId() == pPlayer->GetRoleId())
// 			{
// 				pTeamData->arMemberList[i] = pTeamData->arMemberList[--pTeamData->wMemberNum];
// 				break;
// 			}
// 		}
// 
// 		// 广播有人离开
// 		MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY msg;
// 		msg.m_dwRoleId = pPlayer->GetRoleId();
// 		msg.m_byIsTaskTeam = false;
// 		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
// 		{
// 			pTeamData->arMemberList[i]->SendPacket(&msg);
// 		}
// 	}
// 	else
// 	{
// 		// 只剩下一个人,则从列表里拿掉
// 		RemoveReadyTeam(pPlayer->GetTeamId());
// 	}
}

// 处理离开任务中的队伍
void CTeamDungeonManager::DoLeaveTaskTeam(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// 任务中的队伍列表
	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(dwTeamId);

	// 没在队伍中
	if (iter == m_TaskTeamList.end())
		return;

	TaskTeamData *pTeamData = &iter->second;

	// 如果是队长 或者人数少于3,则解散队伍
	if (dwTeamId == pPlayer->GetRoleId() || pTeamData->wMemberNum <= MAX_TEAM_MEMBER_NUM-1)
	{
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			pTeamData->arMemberList[i].pPlayer->SetTeamDungeonId(0);
			pTeamData->arMemberList[i].pPlayer->SetTeamId(0);
		}

		// 通知队伍解散
		MSG_S2C_TEAM_DUNGEON_DISMISS_NOTIFY dismiss;
		dismiss.m_byIsCaptainLeave = dwTeamId == pPlayer->GetRoleId() ? true : false;
		SendMsgToTeamMember(pTeamData, &dismiss);

		// 广播给其他人,有人离开场景
		MSG_S2C_LEAVE_SCENE_NOTICE leave_notify;
		leave_notify.m_dwRoleId = pPlayer->GetRoleId();
		SendMsgToTeamMember(pTeamData, &leave_notify);

		RemoveTeamTask(dwTeamId);
	}
	else
	{
		// 删除离开的成员
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			if (pTeamData->arMemberList[i].pPlayer->GetRoleId() == pPlayer->GetRoleId())
			{
				pTeamData->arMemberList[i] = pTeamData->arMemberList[--pTeamData->wMemberNum];
				break;
			}
		}

		// todo::如果正在答题,且是最后一个没答题的玩家

		// 广播有人离开
		MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY msg;
		msg.m_dwRoleId = pPlayer->GetRoleId();
		msg.m_byIsTaskTeam = true;
		SendMsgToTeamMember(pTeamData, &msg);

		// 广播给其他人,有人离开场景
		MSG_S2C_LEAVE_SCENE_NOTICE leave_notify;
		leave_notify.m_dwRoleId = pPlayer->GetRoleId();
		SendMsgToTeamMember(pTeamData, &leave_notify);
	}

	pPlayer->SetTeamDungeonId(0);
	pPlayer->SetTeamId(0);
}

// 创建BOSS
bool CTeamDungeonManager::CreateBoss(TaskTeamData *pTeamData)
{
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务链配置[TaskLinkId:%d]", __FUNCTION__, pTeamData->wTaskLinkId);
		return false;
	}

	// 初始成员数据
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i].dwContext = 0;
		pTeamData->arMemberList[i].dwBattleTargetId = 0;
	}

	pTeamData->vSceneItemList.clear();
	pTeamData->vMapList.clear();

	pTeamData->tBossData.dwUID = 1;
	pTeamData->tBossData.dwMapId = pTaskLinkCfg->dwBossMapId;
	pTeamData->tBossData.tMapPos = pTaskLinkCfg->tBossScenePos;
	pTeamData->tBossData.dwStartBattleTime = time(NULL);
	pTeamData->tBossData.vMonsterList.clear();
	if (!g_pCfgMgr->GetMapMonsterList(pTaskLinkCfg->dwBossMapId, &pTeamData->tBossData.vMonsterList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]取组队副本Boss怪物列表失败[MapId:%d]!", __FUNCTION__, pTaskLinkCfg->dwBossMapId);
		return false;
	}

	// 处理随机坐标
	const Scene_Config *pSeceneCfg = g_pCfgMgr->GetScene(pTeamData->tBossData.dwMapId);
	if (pSeceneCfg)
	{
		RandMapPos(pTeamData->tBossData.tMapPos, pSeceneCfg->vMonsterRefrshArea);
	}

	m_BattleBossTeamList.insert(make_pair(pTeamData->dwTeamId, DWORD(time(NULL))));

	MAPLOG(ERROR_LEVEL, "[%s]创建组队副本Boss成功[BossMapId:%d,MonsterNum:%d]!", __FUNCTION__, 
		pTaskLinkCfg->dwBossMapId, pTeamData->tBossData.vMonsterList.size());

	return true;
}

// 处理BOSS奖励
void CTeamDungeonManager::DoBossReward(TaskTeamData *pTeamData, DWORD dwKillerId)
{
	// 提前清空任务状态
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i].pPlayer->SetTeamId(0);
		pTeamData->arMemberList[i].pPlayer->SetTeamDungeonId(0);
	}

	DWORD dwTotalDamage = 0;
	DWORD dwTotalHP = 0;

	TeamMapData &tBossData = pTeamData->tBossData;
	for (size_t i = 0; i < tBossData.vMonsterList.size(); ++i)
	{
		dwTotalDamage += tBossData.vMonsterList[i].dwDamage;
		dwTotalHP += tBossData.vMonsterList[i].dwTotalHP;
	}

	int nDamageRate = 0;
	if (dwTotalDamage && dwTotalHP)
	{
		nDamageRate = ceil(double(dwTotalDamage) * 100 / dwTotalHP);
	}

	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(pTeamData->wDungeonId);
	if (!pDungeonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到组队副本配置[DungeonId:%d]!",__FUNCTION__, pTeamData->wDungeonId);
		return;
	}
		
	const TeamDungeonBossReward_Config *pBossRewardCfg = 
		g_pCfgMgr->GetTeamDungeonBossReward(pTeamData->wDungeonId, pTeamData->dwEnergy, nDamageRate);
	if (!pBossRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到组队副本Boss奖励配置[DungeonId:%d,DamageRate:%d, Energy:%d]!", 
			__FUNCTION__, pTeamData->wDungeonId, nDamageRate, pTeamData->dwEnergy);
		return;
	}

	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到任务链配置[TaskLinkId:%d]!",__FUNCTION__, pTeamData->wTaskLinkId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_BOSS_REWARD_NOTIFY reward_notify;
	reward_notify.m_dwKillerId = dwKillerId;
	reward_notify.m_wMemberNum = 0;

	DWORD dwMaxDamage = 0;
	DWORD dwBossDamage = 0;
	DWORD dwMVPId = 0;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		// 计算MVP
		TeamMember &tMember = pTeamData->arMemberList[i];
		if (tMember.dwContext > dwMaxDamage)
		{
			dwMVPId = tMember.pPlayer->GetRoleId();
			dwMaxDamage = tMember.dwContext;
		}

		dwBossDamage += tMember.dwContext;

		TeamDungeonBossReward &tMemberReward = reward_notify.m_MemberRewardList[reward_notify.m_wMemberNum++];
		tMemberReward.dwRoleId = tMember.pPlayer->GetRoleId();
		tMemberReward.dwAttackDamage = tMember.dwContext;
		tMemberReward.vRewardList.clear();

		PlayerTeamDungeonData &tPlayerDungeonData = tMember.pPlayer->GetRoleFullInfo().tSecondInfo.tTeamDungeonData;

		// 常规部分奖励(每日有次数限制)
		if (tPlayerDungeonData.wTodayBattleTimes < tMember.pPlayer->GetVipValue(VIP_TEAM_DUNGEON_DAILY_REWARD_TIMES))
		{
			for (size_t k = 0; k < pBossRewardCfg->vRewardList.size(); ++k)
			{
				const Item &tRewardItem = pBossRewardCfg->vRewardList[k];
				CItemManager::UseRandItem(tRewardItem.dwId, tRewardItem.dwNum, tMemberReward.vRewardList);
			}
			tMember.pPlayer->AddObjectList(tMemberReward.vRewardList, TRUE, FROM_TEAM_DUNGEON_BOSS_REWARD);
		}
			
		// 记录次数
		tPlayerDungeonData.wTodayBattleTimes++;

		// 记录副本通关记录
		tPlayerDungeonData.AddPassDungeon(pTeamData->wDungeonId);

		// 活跃目标
		tMember.pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 12, 1, true);

		// 节日活跃目标
		tMember.pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 12, 1, true);

		tMember.pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 12, 1, true);

		tMember.pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 12, 1, true);
	}

	// 处理击杀奖励
	TeamMember *pKiller = GetMemberData(pTeamData, dwKillerId); 
	if (pKiller)
	{
		PlayerTeamDungeonData &tDungeonData = pKiller->pPlayer->GetRoleFullInfo().tSecondInfo.tTeamDungeonData;
		DWORD dwKillRewardId = pTaskLinkCfg->GetKillReward(tDungeonData.wTodayKillRewardTimes);

		reward_notify.m_vKillRewardList.clear();
		CItemManager::UseRandItem(dwKillRewardId, 1, reward_notify.m_vKillRewardList);
		pKiller->pPlayer->AddObjectList(reward_notify.m_vKillRewardList, TRUE, FROM_TEAM_DUNGEON_BOSS_REWARD);

		tDungeonData.wTodayKillRewardTimes++;
	}

	// 处理MVP奖励
	TeamMember *pMVP = GetMemberData(pTeamData, dwMVPId);
	if (pMVP)
	{
		PlayerTeamDungeonData &tDungeonData = pMVP->pPlayer->GetRoleFullInfo().tSecondInfo.tTeamDungeonData;
		DWORD dwMVPRewardId = pTaskLinkCfg->GetMVPReward(tDungeonData.wTodayMVPRewardTimes);

		reward_notify.m_vMVPRewardList.clear();
		CItemManager::UseRandItem(dwMVPRewardId, 1, reward_notify.m_vMVPRewardList);
		pMVP->pPlayer->AddObjectList(reward_notify.m_vMVPRewardList, TRUE, FROM_TEAM_DUNGEON_BOSS_REWARD);

		tDungeonData.wTodayMVPRewardTimes++;
	}

	// 广播奖励
	reward_notify.m_dwMVPId = dwMVPId;
	reward_notify.m_dwBossDamage = dwBossDamage;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i].pPlayer->SendPacket(&reward_notify);
	}

	// 检测是否可以记入排行榜(只要有人超过等级上限,则不记入)
	bool bAddRank = true;
	for (WORD i = 0; i < pTeamData->wMemberNum; i++)
	{
		if (pTeamData->arMemberList[i].pPlayer->GetLevel() > pDungeonCfg->wRecommendLevelMax)
		{
			bAddRank = false;
			break;
		}
	}
	if (bAddRank)
	{
		TeamDungeonRank dungeonRank;
		memset(&dungeonRank, 0, sizeof(dungeonRank));
		dungeonRank.wRank = 0;
		dungeonRank.dwFinishTime = time(NULL);
		dungeonRank.dwUseTime = time(NULL) - pTeamData->dwTaskStartTime;
		if (pTeamData->wMemberNum)
			dungeonRank.dwCaptainHeadImgId = pTeamData->arMemberList[0].pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE);
		for (WORD k = 0; k < pTeamData->wMemberNum; ++k)
		{
			Player *pPlayer = pTeamData->arMemberList[k].pPlayer;
			TeamDungeonMember &tMember = dungeonRank.arMemberList[k];
			tMember.wServerId = pPlayer->GetServerId();
			tMember.dwRoleId = pPlayer->GetRoleId();
			strcpy_s(tMember.szRoleName, sizeof(tMember.szRoleName), pPlayer->GetRoleName());
		}
		AddRankData(pTeamData->wDungeonId, dungeonRank);
	}

	// 解散角色队伍
	DWORD dwTeamId = pTeamData->dwTeamId;
	
	MAPLOG(ERROR_LEVEL, "[%s]组队副本挑战boss结算[DungeonId:%d,TeamId:%d, MemberNum:%d, Killer:%d,MVP:%d]!",
		__FUNCTION__, pTeamData->wDungeonId, pTeamData->dwTeamId, pTeamData->wMemberNum, dwKillerId, dwMVPId);

	m_TaskTeamList.erase(dwTeamId);
}

// 是否在匹配池
bool CTeamDungeonManager::IsInMatchList(Player *pPlayer)
{
	DWORD dwDungeonId = pPlayer->GetTeamDungeonId();
	if (!dwDungeonId)
		return false;

	// 在个人匹配池
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(dwDungeonId);
	if (iter != m_PersonMatchList.end())
	{
		if (iter->second.find(pPlayer->GetRoleId()) != iter->second.end())
			return true;
	}

	// 在队伍匹配池
	DWORD dwTeamId = pPlayer->GetTeamId();
	if (dwTeamId)
	{
		map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
		if (iter != m_TeamMatchList.end() && iter->second.find(dwTeamId) != iter->second.end())
			return true;
	}

	return false;
}

// 是否在准备队伍中
bool CTeamDungeonManager::IsInReadyTeam(Player *pPlayer)
{
	if (!pPlayer->GetTeamId())
		return false;

	map<DWORD, ReadyTeamData>::iterator iter = m_ReadyTeamList.find(pPlayer->GetTeamId());
	if (iter != m_ReadyTeamList.end())
		return true;

	return false;
}

// 是否在任务队伍中
bool CTeamDungeonManager::IsInTaskTeam(Player *pPlayer)
{
	if (!pPlayer->GetTeamId())
		return false;

	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(pPlayer->GetTeamId());
	if (iter != m_TaskTeamList.end())
		return true;

	return false;
}

// 取额外奖励列表
void CTeamDungeonManager::GetExtraRewardMemberList(WORD wTaskType, TaskTeamData *pTeamData, vector<DWORD>& vExtraRewardMemberList)
{
	// 答题 第一个答对的玩家
	if (TASK_ANSWER == wTaskType)
	{
		DWORD dwMemberId = 0;

		// 查找第一个答对的成员
		DWORD dwFirstRightTarget = time(NULL);
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			TeamMember *pMember = &pTeamData->arMemberList[i];
			if (pMember->dwContext < dwFirstRightTarget/*todo::需要判断是否答对*/)
			{
				dwFirstRightTarget = pMember->dwContext;
				dwMemberId = pMember->pPlayer->GetRoleId();
			}
		}

		if (dwMemberId)
			vExtraRewardMemberList.push_back(dwMemberId);
	}
	// 对话 只有队长获得
	else if (TASK_DIALOG == wTaskType)
	{
		if (pTeamData->wMemberNum)
			vExtraRewardMemberList.push_back(pTeamData->arMemberList[0].pPlayer->GetRoleId());
	}
	// 采集,打怪 采集最多的,杀怪最多的玩家(可能有多个玩家一起获得)
	else if (TASK_GATHER == wTaskType || TASK_MONSTER == wTaskType)
	{
		DWORD dwFinishMax = 0; // 完成数量最大的
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			TeamMember *pMember = &pTeamData->arMemberList[i];
			if (pMember->dwContext > dwFinishMax)
				dwFinishMax = pMember->dwContext;
		}

		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			TeamMember *pMember = &pTeamData->arMemberList[i];
			if (pMember->dwContext >= dwFinishMax)
			{
				vExtraRewardMemberList.push_back(pMember->pPlayer->GetRoleId());
			}
		}
	}
}

// 超时答题
void CTeamDungeonManager::TimeOutAnswerQuestion(TaskTeamData *pTeamData)
{
	DWORD dwTeamId = pTeamData->dwTeamId;

	// 取任务数据
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到任务配置数据[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// 检测是否答题任务
	if (pTaskCfg->wTaskType != TASK_ANSWER)
	{
		MAPLOG(ERROR_LEVEL, "[%s]当前不是答题任务[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	// 达到指定的人数答对,则任务完成
	if (pTeamData->wCurTaskValue >= MAX_TEAM_MEMBER_NUM - 1)
	{
		DoFinishStageTask(*pTeamData, pTaskCfg);
	}
	// 如果答对人数不足,则重新出题目
	else
	{
		pTeamData->byAnswerTimes++;

		// 答题次数已达上限,进入下一任务
		if (pTeamData->byAnswerTimes >= 5)
		{
			DoFinishStageTask(*pTeamData, pTaskCfg);
		}
		else
		{
			OnNewTask(*pTeamData, false);
		}
	}
}

// 发送消息准备状态队伍成员
void CTeamDungeonManager::SendMsgToReadyTeamMember(ReadyTeamData *pTeamData, NetMsgBody *pMsg)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		Player *pPlayer = pTeamData->arMemberList[i];
		if (!pPlayer)
		{
			MAPLOG(ERROR_LEVEL, "[%s]成员指定为空[idx:%d]!", __FUNCTION__, i);
			continue;
		}

		pPlayer->SendPacket(pMsg);
	}
}

// 删除准备队伍成员
void CTeamDungeonManager::RemoveReadyTeamMember(ReadyTeamData *pTeamData, DWORD dwRoleId)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (!pTeamData->arMemberList[i])
		{
			MAPLOG(ERROR_LEVEL, "[%s]成员指定为空[idx:%d]!", __FUNCTION__, i);
			continue;
		}

		if (pTeamData->arMemberList[i]->GetRoleId() == dwRoleId)
		{
			pTeamData->arMemberList[i] = pTeamData->arMemberList[--pTeamData->wMemberNum];
			break;
		}
	}
}

// 删除准备中的队伍数据
void CTeamDungeonManager::RemoveReadyTeam(DWORD dwTeamId)
{
	m_ReadyTeamList.erase(dwTeamId);
}

// 处理队伍取消匹配
void CTeamDungeonManager::DoTeamCancelMatch(WORD wDungeonId, DWORD dwTeamId)
{
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(wDungeonId);
	if (iter == m_TeamMatchList.end())
		return;

	map<DWORD, MatchTeamData>::iterator iter_team = iter->second.find(dwTeamId);
	if (iter_team == iter->second.end())
		return;

	MatchTeamData *pMatchTeamData = &iter_team->second;

	// 通知取消匹配
	MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY cannel_match_noifty;
	for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
	{
		pMatchTeamData->arMemberList[i]->SendPacket(&cannel_match_noifty);
	}

	// 重新创建队伍
	ReadyTeamData tReadyTeam;
	tReadyTeam.wDungeonId = wDungeonId;
	tReadyTeam.wMemberNum = 0;
	for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
	{
		tReadyTeam.arMemberList[tReadyTeam.wMemberNum++] = pMatchTeamData->arMemberList[i];
	}
	AddReadyTeamData(dwTeamId, tReadyTeam);

	// 同步数据队伍数据
	MSG_S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY notity;
	notity.m_wMemberNum = 0;
	notity.m_dwTeamId = dwTeamId;
	for (WORD i = 0; i < tReadyTeam.wMemberNum; ++i)
	{
		tReadyTeam.arMemberList[i]->FillCommonData(&notity.m_MemberList[notity.m_wMemberNum++]);
	}
	SendMsgToReadyTeamMember(&tReadyTeam, &notity);

	// 从匹配池里去掉
	iter->second.erase(iter_team);
}

// 处理离开队伍匹配池
void CTeamDungeonManager::DoLeaveTeamMatchList(Player *pPlayer)
{
	WORD wDungeonId = pPlayer->GetTeamDungeonId();
	DWORD dwTeamId = pPlayer->GetTeamId();
	DWORD dwRoleId = pPlayer->GetRoleId();


	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter_list = m_TeamMatchList.find(wDungeonId);
	if (iter_list == m_TeamMatchList.end())
		return;

	map<DWORD, MatchTeamData>::iterator iter = iter_list->second.find(pPlayer->GetTeamId());
	if (iter == iter_list->second.end())
		return;

	MatchTeamData *pMatchTeamData = &iter->second;

	// 如果是队伍或者只有一个人,则直接解散
	if (pMatchTeamData->dwTeamId == dwRoleId || pMatchTeamData->wMemberNum <= 1)
	{
		// 通知取消匹配
		MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY cannel_match_noifty;
		SendMsgToMatchTeamMember(pMatchTeamData, &cannel_match_noifty);

		// 清空队伍状态
		for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
		{
			pMatchTeamData->arMemberList[i]->SetTeamDungeonId(0);
			pMatchTeamData->arMemberList[i]->SetTeamId(0);
		}

		iter_list->second.erase(iter);
	}
	else
	{
		// 删除离开的成员
		for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
		{
			if (pMatchTeamData->arMemberList[i]->GetRoleId() == dwRoleId)
			{
				pMatchTeamData->arMemberList[i] = pMatchTeamData->arMemberList[--pMatchTeamData->wMemberNum];
				break;
			}
		}

		// 通知取消匹配
		MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY cannel_match_noifty;
		SendMsgToMatchTeamMember(pMatchTeamData, &cannel_match_noifty);

		// 重新创建队伍
		ReadyTeamData tReadyTeam;
		tReadyTeam.wDungeonId = wDungeonId;
		tReadyTeam.wMemberNum = 0;
		for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
		{
			tReadyTeam.arMemberList[tReadyTeam.wMemberNum++] = pMatchTeamData->arMemberList[i];
		}
		AddReadyTeamData(dwTeamId, tReadyTeam);

		// 同步数据队伍数据
		MSG_S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY notity;
		notity.m_wMemberNum = 0;
		notity.m_dwTeamId = dwTeamId;
		for (WORD i = 0; i < tReadyTeam.wMemberNum; ++i)
		{
			tReadyTeam.arMemberList[i]->FillCommonData(&notity.m_MemberList[notity.m_wMemberNum++]);
		}
		SendMsgToReadyTeamMember(&tReadyTeam, &notity);

		// 从匹配池里去掉
		iter_list->second.erase(iter);
	}
}

bool FillOnePersonList(const map<DWORD, Player*> *pPersonSearchList, const vector<MatchTeamData*> vOneSearchList, 
	vector<MatchTeamData*> &vTeamList, vector<Player*> &vPersonList, WORD &wCurTargetNum, WORD wNeedTargetNum)
{
	if (pPersonSearchList && pPersonSearchList->size())
	{
		map<DWORD, Player*>::const_iterator iter = pPersonSearchList->begin();
		for (; iter != pPersonSearchList->end(); ++iter)
		{
			vPersonList.push_back(iter->second);

			// 人数已足够
			if (++wCurTargetNum >= wNeedTargetNum)
				return true;
		}
	}

	if (vOneSearchList.size())
	{
		vector<MatchTeamData*>::const_iterator iter = vOneSearchList.begin();
		for (; iter != vOneSearchList.end(); ++iter)
		{
			vTeamList.push_back(*iter);

			// 人数已足够
			if (++wCurTargetNum >= wNeedTargetNum)
				return true;
		}
	}

	return false;
}

// 查找匹配目标
bool CTeamDungeonManager::FindMatchTarget(WORD wDungeonId, WORD wNeedTargetNum, vector<MatchTeamData*> &vTeamList, vector<Player*> &vPersonList)
{
	WORD wThreePersonTeamNum = GetTeamSearchListNum(wDungeonId, 3);	// 3人队伍数量
	WORD wTwoPersonTeamNum = GetTeamSearchListNum(wDungeonId, 2);	// 2人队伍数量
	WORD wOnePersonTeamNum = GetTeamSearchListNum(wDungeonId, 1);	// 1人队伍数量
	WORD wOnePersonNum = GetPersonSearchListNum(wDungeonId);		// 散人数量

	WORD wCurTargetNum = 0;

	// 需要1个目标
	if (1 == wNeedTargetNum)
	{
		// 人数足够
		if (wOnePersonTeamNum + wOnePersonNum >= wNeedTargetNum)
		{
			// 优先散人
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
	}
	// 需要2个目标
	else if (2 == wNeedTargetNum)
	{
		// 2人队
		if (wTwoPersonTeamNum)
		{
			vector<MatchTeamData*> vTwoSearchList = GetTeamSearchData(wDungeonId, 2);
			if (vTwoSearchList.size())
			{
				vector<MatchTeamData*>::iterator iter = vTwoSearchList.begin();
				if (iter != vTwoSearchList.end())
				{
					vTeamList.push_back(*iter);
					wCurTargetNum += 2;
				}

				// 人数已足够
				if (wCurTargetNum >= wNeedTargetNum)
					return true;
			}
		}
		// 1人队+散人
		else if (wOnePersonTeamNum + wOnePersonNum >= wNeedTargetNum)
		{
			// 优先散人
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
	}
	// 需要3个目标
	else
	{
		// 3人队
		if (wThreePersonTeamNum)
		{
			vector<MatchTeamData*> vThreeSearchList = GetTeamSearchData(wDungeonId, 3);
			if (vThreeSearchList.size())
			{
				vector<MatchTeamData*>::iterator iter = vThreeSearchList.begin();
				if (iter != vThreeSearchList.end())
				{
					vTeamList.push_back(*iter);
					wCurTargetNum += 3;
				}

				// 人数已足够
				if (wCurTargetNum >= wNeedTargetNum)
					return true;
			}
		}
		// 一个2人队 + (一个1人队 或者 一个散人)
		else if (wTwoPersonTeamNum && (wOnePersonTeamNum + wOnePersonNum))
		{
			// 2人队
			vector<MatchTeamData*> vTwoSearchList = GetTeamSearchData(wDungeonId, 2);
			if (vTwoSearchList.size())
			{
				vector<MatchTeamData*>::iterator iter = vTwoSearchList.begin();
				if (iter != vTwoSearchList.end())
				{
					vTeamList.push_back(*iter);
					wCurTargetNum += 2;
				}
				wCurTargetNum += 2;
			}

			// 优先散人
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
		// 1人队 + 散人
		else if (wOnePersonTeamNum + wOnePersonNum >= 3)
		{
			// 优先散人
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
	}
	
	return false;
}

// 
bool TeamDungeonRankPred(TeamDungeonRank &data1, TeamDungeonRank &data2)
{
	// 第一排序值不相等
	if (data1.dwUseTime < data2.dwUseTime)
		return true;

	// 第一排序值相等
	if (data1.dwUseTime == data2.dwUseTime && data1.dwFinishTime < data2.dwFinishTime)
		return true;

	return false;
}

// 更新排行榜
void UpdateTeamDungeonRan(vector<TeamDungeonRank> &vRankList)
{
	std::sort(vRankList.begin(), vRankList.end(), TeamDungeonRankPred);

	// 更新排名
	for (size_t i = 0; i < vRankList.size(); ++i)
	{
		vRankList[i].wRank = i + 1;
	}
}

// 添加排行榜数据
void CTeamDungeonManager::AddRankData(WORD wDungeonId, const TeamDungeonRank &tRankData)
{
	map<WORD, vector<TeamDungeonRank>>::iterator iter = m_TeamDungeonRankList.find(wDungeonId);
	if (iter == m_TeamDungeonRankList.end())
	{
		m_TeamDungeonRankList.insert(make_pair(wDungeonId, vector<TeamDungeonRank>()));
		iter = m_TeamDungeonRankList.find(wDungeonId);
	}

	if (iter != m_TeamDungeonRankList.end())
	{
		// 如果排行榜低于最后一名则不处理(任务消耗的时间)
		if (iter->second.size() >= MAX_TEAM_DUNGEON_RANK_NUM && tRankData.dwUseTime >= iter->second[iter->second.size() - 1].dwUseTime)
			return;

		iter->second.push_back(tRankData);

		// 重新排序
		UpdateTeamDungeonRan(iter->second);

		// 只取前3名
		if (iter->second.size() > MAX_TEAM_DUNGEON_RANK_NUM)
			iter->second.pop_back();

		// 数据存储
		SaveDataToDB();
	}
}

// 添加抽奖记录
void CTeamDungeonManager::AddLotteryRecord(const TeamDungeonLotteryRecord &tLotteryRecord)
{
	m_LotteryRecordList.push_back(tLotteryRecord);

	// 只取最近20条记录
	if (m_LotteryRecordList.size() > 20)
	{
		m_LotteryRecordList.pop_front();
	}
}

// 广播匹配目标数量
void CTeamDungeonManager::BroadcastMatchTargetNum(WORD wDungeonId)
{
	WORD wTargetNum = 0;

	// 散人数量
	map<DWORD, Player*> *pPersonMatchList = GetPersonSearchList(wDungeonId);
	if (pPersonMatchList)
	{
		wTargetNum += pPersonMatchList->size();
	}

	// 队伍数量
	map<DWORD, MatchTeamData> *pTeamMatchList = GetTeamSearchList(wDungeonId);
	if (pTeamMatchList)
	{
		map<DWORD, MatchTeamData>::iterator iter = pTeamMatchList->begin();
		for (; iter != pTeamMatchList->end(); ++iter)
		{
			wTargetNum += iter->second.wMemberNum;
		}
	}

	// 广播散人
	MSG_S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY target_num_notify;
	target_num_notify.m_wDungeonId = wDungeonId;
	target_num_notify.m_wTargetNum = wTargetNum;
	if (pPersonMatchList)
	{
		map<DWORD, Player*>::iterator iter = pPersonMatchList->begin();
		for (; iter != pPersonMatchList->end(); ++iter)
		{
			iter->second->SendPacket(&target_num_notify);
		}
	}

	// 广播队伍
	if (pTeamMatchList)
	{
		map<DWORD, MatchTeamData>::iterator iter = pTeamMatchList->begin();
		for (; iter != pTeamMatchList->end(); ++iter)
		{
			for (WORD i = 0; i < iter->second.wMemberNum; ++i)
			{
				iter->second.arMemberList[i]->SendPacket(&target_num_notify);
			}
		}
	}
}
 