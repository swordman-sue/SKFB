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
		// ��ʱ��5����,�Ӻ�20�봦��(�����Ա��û�ϱ���,ǿ�н���)
		if (dwCurTime >= iter->second + dwBossBattleLimitTime)
		{
			TaskTeamData *pTeamData = GetTaskTeamData(iter->first);
			if (pTeamData)
				DoBossReward(pTeamData);
			m_BattleBossTeamList.erase(iter);
			break;
		}
	}

	// ������⿨ס�����
	DWORD dwAnserQuestionLimitTime = g_GetConfigValue(214);
 	map<DWORD, TaskTeamData>::iterator team_iter = m_TaskTeamList.begin();
	for (; team_iter != m_TaskTeamList.end(); ++team_iter)
 	{
		TaskTeamData *pTeamData = &team_iter->second;

		// ����3���ӵ�����ʱ��
		if (TASK_ANSWER == pTeamData->wCurTaskType && pTeamData->byAnswerQustionMemberNum && !pTeamData->byIsAllStageTaskFinish &&
			dwCurTime >= pTeamData->dwStageTaskStartTime + dwAnserQuestionLimitTime)
 		{
			MAPLOG(ERROR_LEVEL, "[%s]���ⳬʱ,ϵͳ�����[TeamId:%d]", __FUNCTION__, pTeamData->dwTeamId);
		
			TimeOutAnswerQuestion(pTeamData);

			break; // һ��ֻ����һ��(��ֹ�ӿ�������ɾ������)
 		}
 	}
}

// ��¼
void CTeamDungeonManager::OnLogin(Player *pPlayer)
{

}

// �ǳ�
void CTeamDungeonManager::OnLogout(Player *pPlayer)
{
	//MAPLOG(ERROR_LEVEL, "[%s]�������[role_id:%d,TeamId:%d]",__FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

	// �ڴ���
	if (IsInHall(pPlayer->GetRoleId()))
	{
		//MAPLOG(ERROR_LEVEL, "[%s]����ڴ�����ֱ������[role_id:%d,TeamId:%d]", __FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());
		DoLeaveHall(pPlayer);
	}

	// ���������,����׼��������
	if (pPlayer->GetTeamId())
	{
		//MAPLOG(ERROR_LEVEL, "[%s]����������ֱ������[role_id:%d,TeamId:%d]", __FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

		// ׼��״̬�Ķ����б�
		DoLeaveReadyTeam(pPlayer);

		// �����뿪�����еĶ���
		DoLeaveTaskTeam(pPlayer);
	}
	
	// ƥ���
	if (pPlayer->GetTeamDungeonId())
	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�����ƥ����ֱ������(����)[role_id:%d,TeamId:%d]",
// 			__FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

		 // ����ƥ���
		 map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(pPlayer->GetTeamDungeonId());
		 if (iter != m_PersonMatchList.end())
		 {
			 iter->second.erase(pPlayer->GetRoleId());
		 }

// 		 MAPLOG(ERROR_LEVEL, "[%s]�����ƥ����ֱ������(����)[role_id:%d,TeamId:%d]", 
// 			 __FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetTeamId());

		 // ����ƥ���
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

	// �������а��¼
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

	// �齱��¼
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
		MAPLOG(ERROR_LEVEL, "[%s]��Ӹ����洢���ݹ���[data_size:%d,list_num:%d]",
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
			MAPLOG(ERROR_LEVEL, "������Ӹ����������ݳ���(size:%d)", size);
			return;
		}

		// ���а�����
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

		// �齱�����¼
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

// ������������
void CTeamDungeonManager::LoadDataRequest()
{
	TeamDungeonRankData_Load* pQuery = TeamDungeonRankData_Load::ALLOC();
	pQuery->SetIndex(DB_TEAM_DUNGEON_RANK_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	g_MapServer.MapDBQuery(pQuery);
}

// ���볡��
void CTeamDungeonManager::OnNetEnterScene(WORD wSceneType, DWORD dwContext, Player *pPlayer)
{
// 	MAPLOG(ERROR_LEVEL, "[%s]��ҽ��볡��[SceneType:%d,RoleId:%d]!",
// 		__FUNCTION__, wSceneType, pPlayer->GetRoleId());

	// ����
	if (wSceneType == TEAM_HALL)
	{
		// ����Ƿ��ڳ�����
		if (IsInHall(pPlayer->GetRoleId()))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]������ڳ�����[SceneType:%d,RoleId:%d]!", __FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		// ȡ��������
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(1);
		if (!pSceneCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]ȡ������������[SceneId:%d]!", __FUNCTION__, 1);
			return;
		}

		// �������
		MapPos tMapPos;
		RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
		pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

		// todo::��ʱ����
		//pPlayer->SetMapPos(100, 320);

		// �㲥���������,���˽��볡��
		MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
		pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
		enter_notice.m_wScene = wSceneType;
		enter_notice.m_tMapPos = pPlayer->GetMapPos();
		SendToHallPlayer(&enter_notice);

		// ��Ӧ
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
	// 1.�ɼ�, 2.���  3.Boss
	else if (wSceneType == TEAM_GATHER || wSceneType == TEAM_NORMAL_MONSTER || wSceneType == TEAM_BOSS)
	{
		TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
		if (!pTeamData)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ҷ�������[SceneType:%d,RoleId:%d]!",__FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
		if (!pTaskLinkCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӹ�������������[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
			return;
		}

		// ȡ�������� 
		//const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTeamData->wDungeonId);
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(dwContext);
		if (!pSceneCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]ȡ������������[SceneId:%d]!", __FUNCTION__, dwContext);
			return;
		}

		// �����BOSS
		if (wSceneType == TEAM_BOSS)
		{	
			// ����Ƿ������սBOSS(����1:������������� 2:����ʱ���ѵ���)
			if (pTeamData->wTaskFinishNum < pTaskLinkCfg->wTaskNum &&
				time(NULL) < pTeamData->dwTaskStartTime + pTaskLinkCfg->dwTaskLimitTime)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]����û���,������սBOSS[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
				return;
			}

			// �Ƿ��Ѵ���BOSS
			if (!pTeamData->tBossData.dwStartBattleTime)
				CreateBoss(pTeamData);
		}
			
		// �������
		MapPos tMapPos;
		RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
		pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

		// �㲥���������,���˽��볡��
		MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
		pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
		enter_notice.m_wScene = wSceneType;
		enter_notice.m_tMapPos = pPlayer->GetMapPos();
		SendMsgToTeamMember(pTeamData, &enter_notice);

		// ��Ʒ�б�
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
		
		// �����boss
		if (wSceneType == TEAM_BOSS)
		{
			monster_list_msg.m_arMonsterList[monster_list_msg.m_wMonsterNum++] = pTeamData->tBossData;
		}
		
		// ��ͨ�����б�
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

		// ������Ӧ
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

// �뿪����
void CTeamDungeonManager::OnNetLeaveScene(WORD wSceneType, Player *pPlayer)
{
// 	MAPLOG(ERROR_LEVEL, "[%s]����뿪����[SceneType:%d,RoleId:%d]!",
// 		__FUNCTION__, wSceneType, pPlayer->GetRoleId());

	// ����
	if (wSceneType == TEAM_HALL)
	{
		// ����Ƿ��ڳ�����
		if (!IsInHall(pPlayer->GetRoleId()))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���ľ���ڳ�����[SceneType:%d,RoleId:%d]!", __FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		// �����뿪�߼�
		DoLeaveHall(pPlayer);

		pPlayer->SetTeamDungeonId(0);
		pPlayer->SetTeamId(0);
	}
	// 1.�ɼ�, 2.���  3.Boss
	else
	{
		TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
		if (pTeamData)
		{
			// �㲥��������,�����뿪����
			MSG_S2C_LEAVE_SCENE_NOTICE leave_notify;
			leave_notify.m_dwRoleId = pPlayer->GetRoleId();
			SendMsgToTeamMember(pTeamData, &leave_notify);
		}
	}

	// ��Ӧ���뿪��
	MSG_S2C_LEAVE_SCENE_RESP resp_msg;
	resp_msg.m_wScene = wSceneType;
	pPlayer->SendPacket(&resp_msg);
}

// ��������
void CTeamDungeonManager::OnNetGoto(WORD wSceneType, Player *pPlayer, MapPos tSrcPos, MapPos tDestPos)
{
	// ����
	if (wSceneType == TEAM_HALL)
	{
		// ����Ƿ��ڳ�����
		if (!IsInHall(pPlayer->GetRoleId()))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���û�ڳ�����[SceneType:%d,RoleId:%d]!", __FUNCTION__, wSceneType, pPlayer->GetRoleId());
			return;
		}

		pPlayer->SetMapPos(tDestPos.wMapX, tDestPos.wMapY);

		// ��Ӧ
		MSG_S2C_HERO_GOTO_RESP resp;
		resp.m_wScene = wSceneType;
		resp.m_tSrcPos = tSrcPos;
		resp.m_tDestPos = tDestPos;
		pPlayer->SendPacket(&resp);

		// �㲥��������Ա
		MSG_S2C_HERO_GOTO_NOTIFY notice;
		notice.m_wScene = wSceneType;
		notice.m_dwRoleId = pPlayer->GetRoleId();
		notice.m_tSrcPos = tSrcPos;
		notice.m_tDestPos = tDestPos;
		SendToHallPlayer(&notice);
	}
	// 1.�ɼ�, 2.���  3.Boss
	else
	{
		// ��Ӧ
		MSG_S2C_HERO_GOTO_RESP resp;
		resp.m_wScene = wSceneType;
		resp.m_tSrcPos = tSrcPos;
		resp.m_tDestPos = tDestPos;
		pPlayer->SendPacket(&resp);

		// ȡ��������
		TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
		if (pTeamData)
		{
			// �㲥��������Ա
			MSG_S2C_HERO_GOTO_NOTIFY notify;
			notify.m_wScene = wSceneType;
			notify.m_dwRoleId = pPlayer->GetRoleId();
			notify.m_tSrcPos = tSrcPos;
			notify.m_tDestPos = tDestPos;
			SendMsgToTeamMember(pTeamData, &notify);
		}
	}
}

// ����
void CTeamDungeonManager::Chat(NetMsgBody *pMsg)
{
	// �������
	map<DWORD, Player*>::iterator hall_iter = m_HallPlayerList.begin();
	for (; hall_iter != m_HallPlayerList.end(); ++hall_iter)
	{
		hall_iter->second->SendPacket(pMsg);
	}

	// �����е����
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

// ��ʼս��(����)
bool CTeamDungeonManager::OnMonsterBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return false;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId()); // pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ա����[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return false;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return false;
	}

	// ����Ƿ�ɱ������
	if (pTaskCfg->wTaskType != TASK_MONSTER)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ����ɱ������[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return false;
	}

	// ȡ��������
	TeamMapData *pMap = pTeamData->GetMapData(dwTargetId);
	if (!pMap)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ﲻ���ڻ���������[target_id:%d]", __FUNCTION__,dwTargetId);
		return false;
	}

	// �����Ƿ�����ս����
	if (pMap->dwStartBattleTime && time(NULL) < pMap->dwStartBattleTime + g_GetConfigValue(209))
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_INVALID);
		MAPLOG(ERROR_LEVEL, "[%s]��������ս����...[target_id:%d]", __FUNCTION__, dwTargetId);
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

	// ��¼������ս�Ĺ���
	pMemberData->dwBattleTargetId = dwTargetId;

	// �㲥�������ݱ䶯
 	MSG_S2C_SCENE_MONSTER_DATA_NOTICE msg;
	msg.m_tMonster = *pMap;
	SendMsgToTeamMember(pTeamData, &msg);

	return true;
}

// ս������(����)
bool CTeamDungeonManager::OnMonsterBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	DWORD dwTargetId = pFBMgr->GetTargetId();

	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return false;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return false;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId()); //pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ա����[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return false;
	}

	// ȡ��������
	TeamMapData *pMap = pTeamData->GetMapData(dwTargetId);
	if (!pMap)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ﲻ���ڻ���������[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	// �����Ƿ�����ս����
	if (!pMap->dwStartBattleTime)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����û��ս����...[target_id:%d]", __FUNCTION__, dwTargetId);
		return false;
	}

	pMap->dwStartBattleTime = 0;

	// ���¹����˺�
	UpdateMonsterDamage(pMap->vMonsterList, pMonsterList, byMonsterNum);

	// �㲥�������ݱ䶯
	MSG_S2C_SCENE_MONSTER_DATA_NOTICE msg;
	msg.m_tMonster = *pMap;
	SendMsgToTeamMember(pTeamData, &msg);

	// ����Ƿ�������
	if (IsMonsterDead(pMap->vMonsterList))
	{
		pMemberData->dwContext++;
		pTeamData->wCurTaskValue++;
		
		// �׶��������
		if (pTeamData->wCurTaskValue >= pTaskCfg->GetTaskParam(1))
		{
			DoFinishStageTask(*pTeamData, pTaskCfg);
		}
	}

	// ������ص������
	if (!byIsWin)
	{
		// ȡ��������
		const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
		if (!pTaskCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TaskId:%d]!", __FUNCTION__, pTeamData->dwCurTaskId);
			return false;
		}

		// ȡ��������
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
		if (pSceneCfg)
		{
			// �������
			MapPos tMapPos;
			RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
			pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

			// �㲥���������,���˽��볡��
			MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
			pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
			enter_notice.m_wScene = 0;
			enter_notice.m_tMapPos = pPlayer->GetMapPos();
			SendMsgToTeamMember(pTeamData, &enter_notice);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
		}
	}

	return true;
}

// ��ʼս��(Boss)
bool CTeamDungeonManager::OnBossBattleStart(Player *pPlayer)
{
	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
	if (!pTeamData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, pPlayer->GetTeamId());
		return false;
	}

	// ����Ƿ��ѿ�����սBOSS��
	if (!pTeamData->tBossData.vMonsterList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��û����BOSS,������ս[TeamId:%d]!", __FUNCTION__, pPlayer->GetTeamId());
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	for (size_t i = 0; i < pTeamData->tBossData.vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pTeamData->tBossData.vMonsterList[i]);
	}

	return true;
}

// ս������(Boss)
bool CTeamDungeonManager::OnBossBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetTeamId());
	if (pTeamData)
	{
		TeamMember *pMember = GetMemberData(pTeamData, pPlayer->GetRoleId()); //pTeamData->GetMemberData(pPlayer->GetRoleId());
		if (!pMember)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������Ա����[TeamId:%d,RoleId:%d]!", __FUNCTION__,
				pPlayer->GetTeamId(), pPlayer->GetRoleId());
			return false;
		}

		TeamMapData &tBossData = pTeamData->tBossData;

		// ��û����
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

			// ���¹����˺�
			UpdateMonsterDamage(tBossData.vMonsterList, pMonsterList, byMonsterNum);

			// �㲥�������ݱ䶯
			MSG_S2C_SCENE_MONSTER_DATA_NOTICE msg;
			msg.m_tMonster = tBossData;
			SendMsgToTeamMember(pTeamData, &msg);

			// �������ɱ
			if (IsMonsterDead(tBossData.vMonsterList))
			{
				DWORD dwTeamId = pTeamData->dwTeamId;

				// ������
				DoBossReward(pTeamData, pPlayer->GetRoleId());

				m_BattleBossTeamList.erase(dwTeamId);
			}
		}

		// ������ص������
		if (!byIsWin)
		{
			// ȡ��������
			const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
			if (!pTaskCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TaskId:%d]!", __FUNCTION__, pTeamData->dwCurTaskId);
				return false;
			}

			// ȡ��������
			const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
			if (pSceneCfg)
			{
				// �������
				MapPos tMapPos;
				RandMapPos(tMapPos, pSceneCfg->vHeroBirthArea);
				pPlayer->SetMapPos(tMapPos.wMapX, tMapPos.wMapY);

				// �㲥���������,���˽��볡��
				MSG_S2C_ENTER_SCENE_NOTICE enter_notice;
				pPlayer->FillCommonData(&enter_notice.m_tPlayerData);
				enter_notice.m_wScene = 0;
				enter_notice.m_tMapPos = pPlayer->GetMapPos();
				SendMsgToTeamMember(pTeamData, &enter_notice);
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
			}
		}
	}

	return true;
}

// ��Ʒ����
void CTeamDungeonManager::OnNetLockItem(Player *pPlayer, DWORD dwSceneItemUId)
{
	WORD wFromMsgId = C2S_SCENE_ITEM_LOCK_REQ;

	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// ����Ƿ�ɼ�����
	if (pTaskCfg->wTaskType != TASK_GATHER)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ���ǲɼ�����[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	// ��⵱ǰ�Ƿ�������ס��������
// 	if (IsLockBox(pPlayer->GetRoleId()))
// 	{
// 		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_SCENE_ITEM_LOCK_REQ);
// 		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ���ס����������[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
// 		return;
// 	}

	MSG_S2C_SCENE_ITEM_LOCK_RESP msg;
	msg.m_wScene = 0;

	// �����Ƿ����
	SceneItem *pSceneItem = pTeamData->GetSceneItem(dwSceneItemUId);
	if (!pSceneItem)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]������Ʒ�����ڻ����ѱ�ʰȡ[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	DWORD dwCurTime = time(NULL);

	// ��ⱦ���Ƿ��ѱ���ס(�Ӻ�1��)
	if (pSceneItem->dwLockPlayerId && dwCurTime <= pSceneItem->dwLockTime + g_GetConfigValue(166) + 1)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]�������ѱ���ס[uid:%d]", __FUNCTION__, dwSceneItemUId);
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

// ��Ʒʰȡ
void CTeamDungeonManager::OnNetPickUpItem(Player *pPlayer, DWORD dwSceneItemUId)
{
	WORD wFromMsgId = C2S_SCENE_ITEM_PICK_UP_REQ;

	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId());// pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ա����[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// �����Ƿ����
	SceneItem *pSceneItem = pTeamData->GetSceneItem(dwSceneItemUId);
	if (!pSceneItem)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]������Ʒ�����ڻ����ѱ�ʰȡ[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	// �����ס�����ʰȡ���Ƿ�ͬһ�����
	if (pSceneItem->dwLockPlayerId != pPlayer->GetRoleId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]��ס�����ʰȡ���䲻��ͬһ�����[box_uid:%d,lock_id:%d,pick_up_id:%d]",
			__FUNCTION__, dwSceneItemUId, pSceneItem->dwLockPlayerId, pPlayer->GetRoleId());
		return;
	}

	// ���CDʱ��
	if (time(NULL) < pSceneItem->dwLockTime + g_GetConfigValue(166))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]����ʰȡ,����CD��...[uid:%d]", __FUNCTION__, dwSceneItemUId);
		return;
	}

	DWORD dwInfoId = pSceneItem->dwInfoId;

	// ���ʰȡ����
	pMemberData->dwContext++;
	pTeamData->wCurTaskValue++;

	// ɾ��������״̬
	//RemoveLockBoxPlayer(pPlayer->GetRoleId());	

	// ɾ���ѱ�ʰȡ�ı���
	vector<SceneItem>::iterator iter = pTeamData->vSceneItemList.begin();
	for (; iter != pTeamData->vSceneItemList.end(); ++iter)
	{
		if (iter->dwUID == dwSceneItemUId)
		{
			pTeamData->vSceneItemList.erase(iter);
			break;
		}
	}

	// ��������Ƿ������
	if (pTeamData->wCurTaskValue >= pTaskCfg->GetTaskParam(1))
	{
		DoFinishStageTask(*pTeamData, pTaskCfg);
	}

	// ��Ӧ(todo::Ϊ����Ͽͻ��˵ı���,����˳��)
	MSG_S2C_SCENE_ITEM_PICK_UP_RESP resp;
	resp.m_dwUID = dwSceneItemUId;
	resp.m_dwInfoId = dwInfoId;
	resp.m_vItemList.clear();
	pPlayer->SendPacket(&resp);

	// ֪ͨ������
	MSG_S2C_SCENE_ITEM_PICK_UP_NOTIFY pick_up_notify;
	pick_up_notify.m_dwUID = dwSceneItemUId;
	pick_up_notify.m_dwInfoId = dwInfoId;
	pick_up_notify.m_dwRoleId = pPlayer->GetRoleId();
	SendMsgToTeamMember(pTeamData, &pick_up_notify, pPlayer->GetRoleId());

	// �㲥
 	MSG_S2C_SCENE_ITEM_DISAPPEAR_NOTICE notice;
 	notice.m_dwUID = dwSceneItemUId;
 	SendMsgToTeamMember(pTeamData, &notice);
}

// �������
void CTeamDungeonManager::OnNetShortcutChat(Player *pPlayer, WORD wChatId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP msg;
	msg.m_dwRoleId = pPlayer->GetRoleId();
	msg.m_wChatId = wChatId;
	SendMsgToTeamMember(pTeamData, &msg);
}

// �������а��б�
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

// ����齱��¼
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

// �ش�����
void CTeamDungeonManager::OnNetAnswerQuestion(Player *pPlayer, BYTE byIsRight, BYTE byAnswer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// ����Ƿ��������
	if (pTaskCfg->wTaskType != TASK_ANSWER)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ���Ǵ�������[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	TeamMember *pMemberData = GetMemberData(pTeamData, pPlayer->GetRoleId()); //  pTeamData->GetMemberData(pPlayer->GetRoleId());
	if (!pMemberData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ա����[TeamId:%d,RoleId:%d]!",__FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ��Ѵ��
	if (pMemberData->dwContext)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Ա�Ѿ������Ŀ[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	if (byIsRight)
		pTeamData->wCurTaskValue++;

	// ����Ѵ����
	pMemberData->dwContext = time(NULL);

	// ����Ƿ������˶�������
	BYTE byIsEveryoneAnswer = true;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (!pTeamData->arMemberList[i].dwContext)
		{
			byIsEveryoneAnswer = false;
			break;
		}
	}

	// ��¼�����Ա����
	pTeamData->byAnswerQustionMemberNum++;

	// ��һ���˴���Ϳ�ʼ��ʱ
	if (1 == pTeamData->byAnswerQustionMemberNum)
		pTeamData->dwStageTaskStartTime = time(NULL);	

	// �㲥���˴���
	MSG_S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY notify;
	notify.m_dwRoleId = pPlayer->GetRoleId();
	notify.m_byAnswer = byAnswer;
	SendMsgToTeamMember(pTeamData, &notify);

	// �����˶��Ѵ���
	if (byIsEveryoneAnswer)
	{
		// �ﵽָ�����������,���������
		if (pTeamData->wCurTaskValue >= MAX_TEAM_MEMBER_NUM-1)
		{
			DoFinishStageTask(*pTeamData, pTaskCfg);
		}
		// ��������������,�����³���Ŀ
		else
		{
			pTeamData->byAnswerTimes++;

			// ��������Ѵ�����,������һ����
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

// �Ի�����
void CTeamDungeonManager::OnNetDialogOver(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// ����Ƿ�Ի�������
	if (pTaskCfg->wTaskType != TASK_DIALOG)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ���ǶԻ�����[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	// ��¼�Ѿ��Ի���NPC����
	pTeamData->wCurTaskValue++; 

	// ��������Ƿ����
	if (pTeamData->wCurTaskValue >= pTaskCfg->vTaskParam.size())
	{
		DoFinishStageTask(*pTeamData, pTaskCfg);
	}

	MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY notity;
	SendMsgToTeamMember(pTeamData, &notity);

	// ��Ӧ
	MSG_S2C_TEAM_DUNGEON_DIALOG_OVER_RESP msg;
	pPlayer->SendPacket(&msg);
}

// ��һ��NPC
void CTeamDungeonManager::OnNetNextNPC(Player *pPlayer, WORD wNpcId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY notify;
	notify.m_wNpcId = wNpcId;
	SendMsgToTeamMember(pTeamData, &notify);

	MSG_S2C_TEAM_DUNGEON_NEXT_NPC_RESP resp;
	resp.m_wNpcId = wNpcId;
	pPlayer->SendPacket(&resp);
}

// �������
void CTeamDungeonManager::OnNetInvite(Player *pPlayer, DWORD dwTargetId)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���׼��״̬�Ķ�������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ȡĿ������
	Player *pTarget = PlayerManager::Instance()->FindPlayerByCharID(dwTargetId);
	// û�ڶ�����,û��ƥ�����,�Ż��յ�������Ϣ
	if (pTarget && !pTarget->GetTeamId() && !IsInMatchList(pTarget))
	{
		// ֪ͨĿ��
		MSG_S2C_TEAM_DUNGEON_INVITE_NOTIFY notify;
		notify.m_dwTeamId = dwTeamId;
		notify.m_wServerId = pPlayer->GetServerId();
		notify.m_wDungeonId = pTeamData->wDungeonId;
		strcpy_s(notify.m_szRoleName, sizeof(notify.m_szRoleName), pPlayer->GetRoleName());
		pTarget->SendPacket(&notify);
	}

	// ��Ӧ
	MSG_S2C_TEAM_DUNGEON_INVITE_RESP resp;
	resp.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&resp);
}

// �����������
void CTeamDungeonManager::OnNetAcceptInvite(Player *pPlayer, DWORD dwTeamId)
{
	// ȡ�������Ŀ�����
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_TEAM_INVALID);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ���׼��״̬�Ķ�������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ����Ƿ����ڶ�����
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (pTeamData->arMemberList[i]->GetRoleId() == pPlayer->GetRoleId())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]����Ѿ��Ƕ����Ա[TeamId:%d,RoleId:%d]!", 
				__FUNCTION__, dwTeamId, pPlayer->GetRoleId());
			return;
		}
	}

	// �������Ա�Ƿ�����
	if (pTeamData->wMemberNum >= MAX_TEAM_MEMBER_NUM)
	{
		// ��Ҫ���߿ͻ���
		pPlayer->SendErrorMsg(ERROR_TEAM_MEMBER_IS_FULL);
		MAPLOG(GUI_LEVEL, "[%s]�����Ա��������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ����Ƿ����ڶ�����
	if (pPlayer->GetTeamId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ��ڶ�����[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return; 
	}

	// �������Լ��Ƿ�����ƥ�����
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ���ƥ�����[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ֪ͨ������Ա���˼���
	MSG_S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY notify;
	pPlayer->FillCommonData(&notify.m_NewMember);
	SendMsgToReadyTeamMember(pTeamData, &notify);

	// ����³�Ա
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

	// �Ӵ����б���ȥ��
	//RemoveHallPlayer(pPlayer);
}

// ����׼������
void CTeamDungeonManager::OnNetJoinInReadyTeam(Player *pPlayer, DWORD dwTeamId, WORD wDungeonId)
{
	// 1���ö��黹�ڵȴ��У�����û���ˡ���ֱ�Ӽ�����飬��ʾ���ɹ�������顱
	// 2���ö��黹�ڵȴ��У����������ˡ�����ʾ�����������ˡ���
	// 3���ö����ѽ��븱��������ʾ���Ҳ����ö��顱��
	// 4���ö����ѽ�ɢ������ʾ���Ҳ����ö��顱��
	// 5���������������ʾ���Ҳ����ö��顱��

	// ����Ƿ����ڶ�����
	if (pPlayer->GetTeamId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ��ڶ�����[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// �������Լ��Ƿ�����ƥ�����
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ���ƥ�����[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// �������Ƿ����
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		// todo:��ʾ�����Ѳ�����
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ����ö���[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ��⸱���Ƿ�ƥ��
	if (wDungeonId != pTeamData->wDungeonId)
	{
		// todo:��ʾ�����Ѳ�����
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ����ö���[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ȡ��������
	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(wDungeonId);
	if (!pDungeonCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӹ�������[DungeonId:%d]!", __FUNCTION__, wDungeonId);
		return;
	}

	// ���ȼ��Ƿ������ս�˸���
	if (pPlayer->GetLevel() < pDungeonCfg->wRecommendLevelMin)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ȼ�����,������ս�˸���[DungeonId:%d,NeedLevel:%d]!",
			__FUNCTION__, wDungeonId, pDungeonCfg->wRecommendLevelMin);
		return;
	}

	// ����Ƿ����ڶ�����
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (pTeamData->arMemberList[i]->GetRoleId() == pPlayer->GetRoleId())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]����Ѿ��Ƕ����Ա[TeamId:%d,RoleId:%d]!",
				__FUNCTION__, dwTeamId, pPlayer->GetRoleId());
			return;
		}
	}

	// �������Ա�Ƿ�����
	if (pTeamData->wMemberNum >= MAX_TEAM_MEMBER_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_TEAM_MEMBER_IS_FULL);
		MAPLOG(GUI_LEVEL, "[%s]�����Ա��������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ֪ͨ������Ա���˼���
	MSG_S2C_TEAM_DUNGEON_JOIN_IN_NOTIFY notify;
	pPlayer->FillCommonData(&notify.m_NewMember);
	SendMsgToReadyTeamMember(pTeamData, &notify);

	// ����³�Ա
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

// �뿪׼��״̬�Ķ���
void CTeamDungeonManager::OnNetLeaveReadyTeam(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ȡ��������
	ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڶ�����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƕӳ�,����ֻ��һ����, ���ɢ����
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
	// ����ǳ�Ա 
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

// ��������Ƽ��б�
void CTeamDungeonManager::OnNetPlayerRecommendList(Player *pPlayer)
{
	ReadyTeamData *pTeamData = GetReadyTeamData(pPlayer->GetTeamId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���׼��״̬�Ķ�������[TeamId:%d]!", __FUNCTION__, pPlayer->GetTeamId());
		return;
	}

	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(pPlayer->GetTeamDungeonId());
	if (!pDungeonCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӹ�������[TeamId:%d,DungeonId:%d]!", __FUNCTION__,
			pPlayer->GetTeamId(), pPlayer->GetTeamDungeonId());
		return;
	}

	WORD wMinLevel = pDungeonCfg->wRecommendLevelMin;

	// �Ƽ��������
	int nRecommentPlayerNum = g_GetConfigValue(228, 20);
	nRecommentPlayerNum = min(nRecommentPlayerNum, MAX_TEAM_DUNGEON_RECOMMEND_LIST_NUM);

	MSG_S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP msg;
	msg.m_wPlayerNum = 0;
	
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.begin();
	for (; iter != m_HallPlayerList.end(); ++iter)
	{
		// �Ƽ������Ѵ�����
		if (msg.m_wPlayerNum >= nRecommentPlayerNum)
			break;

		Player *pTarget = iter->second;

		// �ȼ�����ĺ���
		if (pTarget->GetLevel() < wMinLevel)
			continue;

		// ��Ҫ�Լ�
		if (pTarget->GetRoleId() == pPlayer->GetRoleId())
			continue;

		// ����׼������Ĳ�Ҫ,�Ѿ��������Ĳ�Ҫ,�Ѿ�����ƥ��صĲ�Ҫ
		if (pTarget->GetTeamDungeonId() || pTarget->GetTeamId())
			continue;

		// ����׼������Ĳ�Ҫ
// 		if (IsInReadyTeam(iter->second))
// 			continue;
// 
// 		// �Ѿ��������Ĳ�Ҫ
// 		if (IsInTaskTeam(iter->second))
// 			continue;

		iter->second->FillCommonData(&msg.m_PlayerList[msg.m_wPlayerNum++]);
	}

	pPlayer->SendPacket(&msg);
}


// �齱
void CTeamDungeonManager::OnNetLottery(Player *pPlayer, WORD wTimes, BYTE byIsCostItem)
{
	// ���齱�����ĺϷ���
	if (!wTimes || wTimes > MAX_TEAM_DUNGEON_LOTTERY_TIMES)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�齱�������Ϸ�[times:%d]!", __FUNCTION__, wTimes);
		return;
	}

	DWORD dwCostItemId = g_GetConfigValue(211);
	int nCostDiamond = wTimes > 1 ? g_GetConfigValue(285) : g_GetConfigValue(284);	// 1.Ť��һ��������ʯ 2.Ť��ʮ��������ʯ

	// �Ƿ����ĵ���
	if (byIsCostItem)
	{
		// ������ĵ���Ʒ
		if (pPlayer->GetItemNum(dwCostItemId) < wTimes)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�齱���ĵ���Ʒ����[ItemId:%d]!", __FUNCTION__, dwCostItemId);
			return;
		}
	}
	else
	{
		// ������ĵ���ʯ
		if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�齱���ĵ���ʯ����[CostDiamond:%d]!", __FUNCTION__, nCostDiamond);
			return;
		}
	}

	// ȡ�齱�б�
	const vector<TeamDungeonLottery_Config> &lotteryList = g_pCfgMgr->GetTeamDungeonLotteryList();

	// �����ܵ�Ȩֵ
	int nTotalWeight = 0;
	vector<TeamDungeonLottery_Config>::const_iterator iter = lotteryList.begin();
	for (; iter != lotteryList.end(); ++iter)
	{
		nTotalWeight += iter->nWeight;
	}

	// ������е��߼�
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

				// �Ƿ���Ҫ��¼
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
		// �۳����ĵ���Ʒ
		pPlayer->RemoveItem(dwCostItemId, wTimes, FROM_TEAM_DUNGEON_ND_LOTTERY);
	}
	else
	{
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_TEAM_DUNGEON_ND_LOTTERY);
	}

	// ��ӳ��е���Ʒ
	pPlayer->AddObjectList(vItemList, TRUE, FROM_TEAM_DUNGEON_ND_LOTTERY);

	// ��Ӧ
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

	// ���ջ
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 36, wTimes, true);
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 36, wTimes, true);
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 36, wTimes, true);
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 36, wTimes, true);
}

// Boss����ʼ
void CTeamDungeonManager::OnNetBossTaskStart(Player *pPlayer)
{
	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetRoleId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Ҳ��Ƕӳ����ܽ�����һ���������[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!", 
			__FUNCTION__, pPlayer->GetRoleId(), pTeamData->dwCurTaskId);
		return;
	}

	// ȡ����������
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
		return;
	}

	// ����Ƿ������սBOSS(����1:������������� 2:����ʱ���ѵ���)
	if (pTeamData->wTaskFinishNum < pTaskLinkCfg->wTaskNum &&
		time(NULL) < pTeamData->dwTaskStartTime + pTaskLinkCfg->dwTaskLimitTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		DWORD dwPassTime = time(NULL) - pTeamData->dwTaskStartTime;
		MAPLOG(ERROR_LEVEL, "[%s]������û����,������սBOSS[TaskLinkId:%d,CurFinishNum:%d,NeedNum:%d,PassTime:%d,LimitTime:%d]!", 
			__FUNCTION__, pTeamData->wTaskLinkId, pTeamData->wTaskFinishNum, pTaskLinkCfg->wTaskNum, dwPassTime, pTaskLinkCfg->dwTaskLimitTime);
		return;
	}

	// ս����ʼ
	if (!CreateBoss(pTeamData))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�������BOSSʧ��[TaskLinkId:%d]!", __FUNCTION__, pTeamData->wTaskLinkId);
		return;
	}

	MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY notify;
	SendMsgToTeamMember(pTeamData, &notify);

	MSG_S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP resp;
	pPlayer->SendPacket(&resp);
}

// ��������
void CTeamDungeonManager::OnNetAcceptTask(Player *pPlayer)
{
	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetRoleId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ҷ�������[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY notity;
	SendMsgToTeamMember(pTeamData, &notity);

	MSG_S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP resp;
	pPlayer->SendPacket(&resp);
}

// ����ʼ(��Ҫ����Լ���ӳɹ������)
void CTeamDungeonManager::OnNetTaskStart(Player *pPlayer)
{
	// ȡ�������Ŀ�����
	ReadyTeamData *pReadyTeamData = GetReadyTeamData(pPlayer->GetRoleId());
	if (!pReadyTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���׼��״̬�Ķ�������[TeamId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ա�б�
	vector<Player*> vPlayerList;
	for (WORD i = 0; i < pReadyTeamData->wMemberNum; ++i)
	{
		vPlayerList.push_back(pReadyTeamData->arMemberList[i]);
	}

	// ��������
	if (!CreateTaskTeam(pReadyTeamData->wDungeonId, vPlayerList))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��������ʧ��[TeamId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ȡ��������
	TaskTeamData *pTeamData = GetTaskTeamData(pPlayer->GetRoleId());
	if (!pTeamData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TeamId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

 	// ����ʼ֪ͨ
	MSG_S2C_TEAM_DUNGEON_TASK_START_NOTIFY task_start_notify;
	task_start_notify.m_dwTaskLinkId = pTeamData->wTaskLinkId;
	task_start_notify.m_dwTastStartTime = time(NULL);
	SendMsgToTeamMember(pTeamData, &task_start_notify);

	MSG_S2C_TEAM_DUNGEON_TASK_START_RESP resp;
	pPlayer->SendPacket(&resp);

	// ��׼��������ɾ��
	RemoveReadyTeam(pPlayer->GetRoleId());
}

// ������Ӹ�������
void CTeamDungeonManager::OnNetTeamDungeonData(Player *pPlayer)
{
	PlayerTeamDungeonData &tTeamDungeonData = pPlayer->GetRoleFullInfo().tSecondInfo.tTeamDungeonData;

	MSG_S2C_TEAM_DUNGEON_DATA_RESP msg;
	msg.m_tTeamDungeonData = tTeamDungeonData;
	pPlayer->SendPacket(&msg);
}

// ��������
void CTeamDungeonManager::OnNetCreate(Player *pPlayer, WORD wDungeonId)
{
	// ȡ��������
	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(wDungeonId);
	if (!pDungeonCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӹ�������[DungeonId:%d]!", __FUNCTION__, wDungeonId);
		return;
	}

	// ���ȼ��Ƿ������ս�˸���
	if (pPlayer->GetLevel() < pDungeonCfg->wRecommendLevelMin)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ȼ�����,������ս�˸���[DungeonId:%d,NeedLevel:%d]!", 
			__FUNCTION__, wDungeonId, pDungeonCfg->wRecommendLevelMin);
		return;
	}

	DWORD dwTeamId = pPlayer->GetTeamId();

	// ����Ƿ����ж���
	if (dwTeamId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������ж���[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ����ڲ����б���
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������ڲ����б���[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
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

	// ��Ӧ
	MSG_S2C_TEAM_DUNGEON_CREATE_TEAM_RESP msg;
	msg.m_wDungeonId = wDungeonId;
	msg.m_dwTeamId = dwTeamId;
	pPlayer->SendPacket(&msg);	

	// �뿪����
	// DoLeaveHall(pPlayer);
}

// ���Ҷ���
void CTeamDungeonManager::OnNetMatch(Player *pPlayer, WORD wDungeonId, BYTE byIsTeam)
{
	// ע�����:
	// 1.�������ƥ��״̬�Ժ�,������������
	// 2.ȡ��ƥ���ص����״̬

	DWORD dwTeamId = pPlayer->GetTeamId();

	// ����Ƿ�����������
	if (!byIsTeam && IsInReadyTeam(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�������׼��������[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ�����������
	if (IsInTaskTeam(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����������������[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ�����ƥ�����
	if (IsInMatchList(pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������ڲ����б���[TeamId:%d,RoleId:%d]!", __FUNCTION__, dwTeamId, pPlayer->GetRoleId());
		return;
	}

	vector<Player*> vPlayerList;
	vector<MatchTeamData*> vTeamList;

	// ������Ҷ���
	if (byIsTeam)
	{
		// ����Ƿ��ڶ�����
		if (!dwTeamId)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]û���ڶ�����,���ܽ��ж���ƥ��[DungeonId:%d,RoleId:%d]!",
				__FUNCTION__, wDungeonId, pPlayer->GetRoleId());
			return;
		}

		// ����Ƿ�ӳ�
		if (dwTeamId != pPlayer->GetRoleId())
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]���Ƕӳ�,���ܽ��ж���ƥ��[DungeonId:%d,RoleId:%d]!",
				__FUNCTION__, wDungeonId, pPlayer->GetRoleId());
			return;
		}

		// �Լ��Ķ����Ա����
		ReadyTeamData *pTeamData = GetReadyTeamData(dwTeamId);
		if (!pTeamData)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���׼���еĶ�������[TeamId:%d]!",__FUNCTION__, dwTeamId);
			return; 
		}

		// ��Ա����
		if (pTeamData->wMemberNum >= MAX_TEAM_MEMBER_NUM)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]׼���еĶ����Ա����[TeamId:%d]!", __FUNCTION__, dwTeamId);
			return;
		}

		WORD wNeedMemberNum = MAX_TEAM_MEMBER_NUM - pTeamData->wMemberNum;

		// ƥ�䵽������
		vector<Player*> vOnePersonList;
		if (FindMatchTarget(wDungeonId, wNeedMemberNum, vTeamList, vOnePersonList))
		{
			// Ŀ�����(�Ƚ��Ķ����Ϊ�ӳ�)
			for (size_t i = 0; i < vTeamList.size(); ++i)
			{
				MatchTeamData *pTeamData = vTeamList[i];
				for (WORD k = 0; k < pTeamData->wMemberNum; ++k)
				{
					vPlayerList.push_back(pTeamData->arMemberList[k]);
				}
			}

			// �Լ�����
			for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
			{
				vPlayerList.push_back(pTeamData->arMemberList[i]);
			}

			// ɢ��
			vector<Player*>::iterator iter = vOnePersonList.begin();
			for (; iter != vOnePersonList.end(); ++iter)
			{
				vPlayerList.push_back(*iter);
			}
		}
		// ƥ�䲻����,������ƥ���б�
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

			// ƥ��֪ͨ
			MSG_S2C_TEAM_DUNGEON_MATCH_NOTIFY match_notify;
			SendMsgToReadyTeamMember(pTeamData, &match_notify);
		}

		// ɾ��׼���еĶ���
		RemoveReadyTeam(dwTeamId);
	}
	// ɢ�˲��Ҷ���
	else
	{
		// ƥ�䵽�㹻����
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
		// ƥ�䲻����,������ƥ���б�
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

	// ���ƥ�䵽�����Ƿ�Ϸ�
	if (vPlayerList.size() > MAX_TEAM_MEMBER_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ƥ�䵽����������ָ������[TeamId:%d]!", __FUNCTION__, dwTeamId);
		return;
	}

	// ��������
	if (vPlayerList.size() == MAX_TEAM_MEMBER_NUM)
	{
		vector<Player*> vTeamMemberList;

		// ɢ��ƥ�䲢ƥ�䵽��ȫ����ɢ��
		if (!byIsTeam && !vTeamList.size())
		{
			// ����ӳ�
			int nRandIdx = g_Rand(0, vPlayerList.size() - 1);
			vTeamMemberList.push_back(vPlayerList[nRandIdx]);
			for (int i = 0; i < vPlayerList.size(); ++i)
			{
				// ��������Ѿ������Ϊ�ӳ������
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
			MAPLOG(ERROR_LEVEL, "[%s]�����������ʧ��[TeamId:%d]!", __FUNCTION__, dwTeamId);
			return;
		}

		// ɾ���Ѿ���ƥ�䵽��(����)
		map<DWORD, MatchTeamData> *pTeamSearchList = GetTeamSearchList(wDungeonId);
		if (pTeamSearchList)
		{
			vector<MatchTeamData*>::iterator iter = vTeamList.begin();
			for (; iter != vTeamList.end(); ++iter)
			{
				pTeamSearchList->erase((*iter)->dwTeamId);
			}
		}

		// ɾ���Ѿ���ƥ�䵽��(����)
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

	// �㲥ƥ��Ŀ������
	BroadcastMatchTargetNum(wDungeonId);
}

// ȡ��ƥ�����
void CTeamDungeonManager::OnNetCancelMatch(Player *pPlayer)
{
	WORD wDungeonId = pPlayer->GetTeamDungeonId();
	DWORD dwTeamId = pPlayer->GetTeamId();

	// ����ƥ���(ֻҪ����ȡ��,�������鶼�᷵�����״̬)
	if (dwTeamId)
	{
		DoTeamCancelMatch(wDungeonId, dwTeamId);
	}
	// ����ƥ���
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

	// �㲥ƥ��Ŀ������
	BroadcastMatchTargetNum(wDungeonId);
}

// �뿪����
void CTeamDungeonManager::DoLeaveHall(Player *pPlayer)
{
	// �㲥��������,�����˳�
	MSG_S2C_LEAVE_SCENE_NOTICE notice;
	notice.m_dwRoleId = pPlayer->GetRoleId();
	SendToHallPlayer(&notice);

	RemoveHallPlayer(pPlayer);
}

// �Ƿ��ڳ���
bool CTeamDungeonManager::IsInHall(DWORD dwRoleId)
{
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.find(dwRoleId);
	return iter != m_HallPlayerList.end() ? true : false;
}

// ȡ���˲�������
DWORD CTeamDungeonManager::GetPersonSearchListNum(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(dwDungeonId);
	return iter != m_PersonMatchList.end() ? iter->second.size() : 0;
}

// ȡ��������б�����
DWORD CTeamDungeonManager::GetTeamSearchListNum(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
	return iter != m_TeamMatchList.end() ? iter->second.size() : 0;
}

// ȡ��������б�����
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

// ������
bool CTeamDungeonManager::OnNewTask(TaskTeamData &tTeamData, BYTE byIsNewTask)
{
	if (CreateTask(tTeamData, byIsNewTask))
	{
// 		if (byIsNewTask)
// 			tTeamData.wTaskFinishNum++;
		// ֪ͨ�µ�����ʼ
		MSG_S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY new_task;
		new_task.m_dwTaskId = tTeamData.dwCurTaskId;
		new_task.m_wQuestionId = tTeamData.wQuestionId;
		new_task.m_byIsNewTask = byIsNewTask;
		SendMsgToTeamMember(&tTeamData, &new_task);

		return true;
	}

	return false;
}

// ������Ϣƥ������Ա
void CTeamDungeonManager::SendMsgToMatchTeamMember(MatchTeamData *pTeamData, NetMsgBody *pMsg)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i]->SendPacket(pMsg);
	}
}

// ��ӳ������
void CTeamDungeonManager::AddHallPlayer(Player *pPlayer)
{
	m_HallPlayerList.insert(std::make_pair(pPlayer->GetRoleId(), pPlayer));
}

// ɾ���������
void CTeamDungeonManager::RemoveHallPlayer(Player *pPlayer)
{
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.find(pPlayer->GetRoleId());
	if (iter != m_HallPlayerList.end())
	{
		m_HallPlayerList.erase(iter);
	}
}

// �㲥��Ϣ
void CTeamDungeonManager::SendToHallPlayer(NetMsgBody *pMsg)
{
	map<DWORD, Player*>::iterator iter = m_HallPlayerList.begin();
	for (; iter != m_HallPlayerList.end(); ++iter)
	{
		iter->second->SendPacket(pMsg);
	}
}

// ȡ��������б�
map<DWORD, MatchTeamData>* CTeamDungeonManager::GetTeamSearchList(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
	return iter != m_TeamMatchList.end() ? &iter->second : NULL;
}

// ȡɢ�˲����б�
map<DWORD, Player*>* CTeamDungeonManager::GetPersonSearchList(DWORD dwDungeonId)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(dwDungeonId);
	return iter != m_PersonMatchList.end() ? &iter->second : NULL;
}

// ȡƥ���еĶ�������
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

// ɾ��ƥ���еĶ�������
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

// ���׼����������
void CTeamDungeonManager::AddReadyTeamData(DWORD dwTeamId, const ReadyTeamData &tTeamData)
{
	m_ReadyTeamList.insert(make_pair(dwTeamId, tTeamData));
}

// ȡ��������
ReadyTeamData* CTeamDungeonManager::GetReadyTeamData(DWORD dwTeamId)
{
	map<DWORD, ReadyTeamData>::iterator iter = m_ReadyTeamList.find(dwTeamId);
	return iter != m_ReadyTeamList.end() ? &iter->second : NULL;
}

// ��������������
void CTeamDungeonManager::AddTaskTeamData(DWORD dwTeamId, const TaskTeamData &tTeamData)
{
	m_TaskTeamList.insert(make_pair(dwTeamId, tTeamData));
}

// ȡ�����������
TaskTeamData* CTeamDungeonManager::GetTaskTeamData(DWORD dwTeamId)
{
	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(dwTeamId);
	return iter != m_TaskTeamList.end() ? &iter->second : NULL;
}

// ��������
bool CTeamDungeonManager::CreateTask(TaskTeamData &tTeamData, BYTE byIsNewTask)
{
	// ȡ����������
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(tTeamData.wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[TaskLinkId:%d]!", __FUNCTION__, tTeamData.wTaskLinkId);
		return false;
	}

	// ����Ƿ����е����������
	if (tTeamData.wTaskFinishNum >= pTaskLinkCfg->wTaskNum || tTeamData.wTaskFinishNum >= pTaskLinkCfg->vTaskList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����������������[TeamId:%d,TaskLinkId:%d]!",
			__FUNCTION__, tTeamData.dwTeamId, tTeamData.wTaskLinkId);
		return false;
	}

	// �����������
	if (byIsNewTask)
	{
		const vector<DWORD> &vTaskRankList = pTaskLinkCfg->vTaskList[tTeamData.wTaskFinishNum];
		if (!vTaskRankList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������б�Ϊ��[TeamId:%d,TaskLinkId:%d]!",
				__FUNCTION__, tTeamData.dwTeamId, tTeamData.wTaskLinkId);
			return false;
		}
		//tTeamData.wTaskFinishNum++;
		int nRandIdx = g_Rand(0, vTaskRankList.size() - 1);
		tTeamData.dwCurTaskId = vTaskRankList[nRandIdx];

		MAPLOG(ERROR_LEVEL, "[%s]���������Ϣ[TeamId:%d,TaskLinkId:%d,CurTaskId:%d]!",
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

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(tTeamData.dwCurTaskId);
	if (!pTaskCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[TaskId:%d]!", __FUNCTION__, tTeamData.dwCurTaskId);
		return false;
	}

	// ���
	if (TASK_MONSTER == pTaskCfg->wTaskType)
	{
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
		if (!pSceneCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
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
				MAPLOG(ERROR_LEVEL, "[%s]ȡ��Ӹ��������б�ʧ��[MapId:%d]!", __FUNCTION__, tMapData.dwMapId);
				return false;
			}
			tTeamData.vMapList.push_back(tMapData);

			MAPLOG(ERROR_LEVEL, "[%s]������������[SceneId:%d,Monster:%d]!",
				__FUNCTION__, pTaskCfg->dwSceneId, dwMapId);
		}
	}
	// �ɼ�
	else if (TASK_GATHER == pTaskCfg->wTaskType)
	{
		const Scene_Config *pSceneCfg = g_pCfgMgr->GetScene(pTaskCfg->dwSceneId);
		if (!pSceneCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[SceneId:%d]!", __FUNCTION__, pTaskCfg->dwSceneId);
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

			MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ[SceneId:%d,ItemId:%d]!", 
				__FUNCTION__, pTaskCfg->dwSceneId, dwItemId);
		}
	}
	// ����
	else if (TASK_ANSWER == pTaskCfg->wTaskType)
	{
		if (!pTaskCfg->vTaskParam.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������б�Ϊ��[TaskId:%d]!", __FUNCTION__, tTeamData.dwCurTaskId);
			return false;
		}

		int nRandIdx = g_Rand(0, pTaskCfg->vTaskParam.size() - 1);
		tTeamData.wQuestionId = pTaskCfg->vTaskParam[nRandIdx];

		if (byIsNewTask)
			tTeamData.byAnswerTimes = 0;
	}
	// �Ի�
	else if (TASK_DIALOG == pTaskCfg->wTaskType)
	{
		// todo::��ʱ����Ҫ����
	}

	tTeamData.wCurTaskType = pTaskCfg->wTaskType;

	return true;
}

// �Ƿ��������
bool CTeamDungeonManager::IsMonsterDead(vector<MonsterData> &vMonsterList)
{
	for (size_t i = 0; i < vMonsterList.size(); ++i)
	{
		if (vMonsterList[i].dwDamage < vMonsterList[i].dwTotalHP)
			return false;
	}
	return true;
}

// ���¹����˺�
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

				// �˺�����
				if (iter->dwTotalHP && iter->dwDamage > iter->dwTotalHP)
					iter->dwDamage = iter->dwTotalHP;
			}
		}
	}
}


// ������ɽ׶��������
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

	// ����������
	for (WORD i = 0; i < tTeamData.wMemberNum; ++i)
	{
		TeamMember &tMember = tTeamData.arMemberList[i];
		if (tMember.pPlayer)
		{
			// ��ͨ����
			reward_notify.m_vItemList.clear();
			reward_notify.m_vExtraRewardList.clear();
			CItemManager::UseRandItem(dwItemId, 1, reward_notify.m_vItemList);
			tMember.pPlayer->AddObjectList(reward_notify.m_vItemList, TRUE, FROM_TEAM_DUNGEON_FINISH_STAGE_TASK);
			

			// ����Ƿ��ö��⽱��
			vector<DWORD>::iterator iter = vExtraRewardMemberList.begin();
			for (; iter != vExtraRewardMemberList.end(); ++iter)
			{
				if (tMember.pPlayer->GetRoleId() == *iter)
				{
					CItemManager::UseRandItem(pTaskCfg->dwExtraRewardId, 1, reward_notify.m_vExtraRewardList);
					tMember.pPlayer->AddObjectList(reward_notify.m_vExtraRewardList, TRUE, FROM_TEAM_DUNGEON_FINISH_STAGE_TASK);
// 					MAPLOG(ERROR_LEVEL, "[%s]��Ӹ������⽱��[RoleId:%d,ExtraRewardId:%d, ExtraRewardList:%d]!",
// 						__FUNCTION__, tMember.pPlayer->GetRoleId(), pTaskCfg->dwExtraRewardId, reward_notify.m_vExtraRewardList.size());
					break;
				}
			}

			tMember.pPlayer->SendPacket(&reward_notify);
		}
	}

	//�����������
	tTeamData.wTaskFinishNum++;

	// ȡ����������
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(tTeamData.wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[TaskLinkId:%d]!", __FUNCTION__, tTeamData.wTaskLinkId);
		return;
	}

	// ���������������
	if (tTeamData.wTaskFinishNum >= pTaskLinkCfg->wTaskNum || tTeamData.wTaskFinishNum >= pTaskLinkCfg->vTaskList.size())
	{
		tTeamData.byIsAllStageTaskFinish = true;

		MSG_S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY link_finish_notify;
		SendMsgToTeamMember(&tTeamData, &link_finish_notify);
	}
	// �����������
	else
	{
		// �����µ�����
		OnNewTask(tTeamData, true);
	}

	MAPLOG(ERROR_LEVEL, "[%s]�׶��������[LinkId:%d,TaskId:%d,TaskFinishNum:%d]!",
		__FUNCTION__, tTeamData.wTaskLinkId, tTeamData.dwCurTaskId, tTeamData.wTaskFinishNum);
}

// ɾ�������еĶ���
void CTeamDungeonManager::RemoveTeamTask(DWORD dwTeamId)
{
	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(dwTeamId);
	if (iter != m_TaskTeamList.end())
	{
		m_TaskTeamList.erase(iter);
	}
}

// �����ͼ�����
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
		MAPLOG(ERROR_LEVEL, "[%s]�������������ó���", __FUNCTION__);
		return false;
	}

	return true;
}

// ������Ϣ�����Ա
void CTeamDungeonManager::SendMsgToTeamMember(TaskTeamData *pTeamData, NetMsgBody *pMsg, DWORD dwMemberId)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		Player *pPlayer = pTeamData->arMemberList[i].pPlayer;

		if (!dwMemberId || (dwMemberId && pPlayer->GetRoleId() != dwMemberId))
			pPlayer->SendPacket(pMsg);
	}
}

// �����������
bool CTeamDungeonManager::CreateTaskTeam(WORD wDungeonId, vector<Player*> &vPlayerList)
{
	// ȡ��������
	const TeamDungeon_Config *pDungeonCfg = g_pCfgMgr->GetTeamDungeon(wDungeonId);
	if (!pDungeonCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ӹ�������[DungeonId:%d]", __FUNCTION__, wDungeonId);
		return false;
	}

	// ���������
	if (!pDungeonCfg->vTaskList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӹ����������б�����Ϊ��[DungeonId:%d]", __FUNCTION__, wDungeonId);
		return false;
	}

	// ����Ա����
	if (vPlayerList.size() != MAX_TEAM_MEMBER_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����Ա��������,���ܴ���![DungeonId:%d,PlayerNum:%d]",
			__FUNCTION__, wDungeonId, vPlayerList.size());
		return false;
	}

	// �б��һ����ҳ�Ϊ�ӳ�
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
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������[TeamId:%d]", __FUNCTION__, tTeamData.dwTeamId);
		return false;
	}

	// ���������Id
	int nRandIdx = g_Rand(0, pDungeonCfg->vTaskList.size()-1);
	pTeamData->wTaskLinkId = pDungeonCfg->vTaskList[nRandIdx];

	// �㲥������������
	MSG_S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY team_data;
	team_data.m_dwTastStartTime = pTeamData->dwTaskStartTime;
	team_data.m_wTaskLinkId = pTeamData->wTaskLinkId;
	team_data.m_wMemberNum = pTeamData->wMemberNum;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i].pPlayer->FillCommonData(&team_data.m_MemberList[i]);
	}
	SendMsgToTeamMember(pTeamData, &team_data);

	// ��������
 	CreateTask(*pTeamData);
 
 	// ֪ͨ�µ�����ʼ
 	MSG_S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY new_task;
	new_task.m_dwTaskId = pTeamData->dwCurTaskId;
 	new_task.m_wQuestionId = pTeamData->wQuestionId;
 	new_task.m_byIsNewTask = true;
 	SendMsgToTeamMember(pTeamData, &new_task);

	// �뿪����
	for (size_t i = 0; i < vPlayerList.size(); ++i)
	{
		DoLeaveHall(vPlayerList[i]);
	}

	return true;
}

// �����뿪׼���ж���
void CTeamDungeonManager::DoLeaveReadyTeam(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	map<DWORD, ReadyTeamData>::iterator iter = m_ReadyTeamList.find(dwTeamId);

	// û�ڶ�����
	if (iter == m_ReadyTeamList.end())
		return;

	ReadyTeamData *pTeamData = &iter->second;

	// ����Ƕӳ�,����ֻ��һ����, ���ɢ����
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
	// ����ǳ�Ա 
	else
	{
		// �㲥�����뿪
		MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY leave_msg;
		leave_msg.m_dwRoleId = pPlayer->GetRoleId();
		leave_msg.m_byIsTaskTeam = false;
		SendMsgToReadyTeamMember(pTeamData, &leave_msg);

		RemoveReadyTeamMember(pTeamData, pPlayer->GetRoleId());

		pPlayer->SetTeamDungeonId(0);
		pPlayer->SetTeamId(0);
	}


	// ���������������,����Ҫ֪ͨ������
// 	if (pTeamData->wMemberNum > 1)
// 	{
// 		// ɾ���뿪�ĳ�Ա
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
// 		// �㲥�����뿪
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
// 		// ֻʣ��һ����,����б����õ�
// 		RemoveReadyTeam(pPlayer->GetTeamId());
// 	}
}

// �����뿪�����еĶ���
void CTeamDungeonManager::DoLeaveTaskTeam(Player *pPlayer)
{
	DWORD dwTeamId = pPlayer->GetTeamId();

	// �����еĶ����б�
	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(dwTeamId);

	// û�ڶ�����
	if (iter == m_TaskTeamList.end())
		return;

	TaskTeamData *pTeamData = &iter->second;

	// ����Ƕӳ� ������������3,���ɢ����
	if (dwTeamId == pPlayer->GetRoleId() || pTeamData->wMemberNum <= MAX_TEAM_MEMBER_NUM-1)
	{
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			pTeamData->arMemberList[i].pPlayer->SetTeamDungeonId(0);
			pTeamData->arMemberList[i].pPlayer->SetTeamId(0);
		}

		// ֪ͨ�����ɢ
		MSG_S2C_TEAM_DUNGEON_DISMISS_NOTIFY dismiss;
		dismiss.m_byIsCaptainLeave = dwTeamId == pPlayer->GetRoleId() ? true : false;
		SendMsgToTeamMember(pTeamData, &dismiss);

		// �㲥��������,�����뿪����
		MSG_S2C_LEAVE_SCENE_NOTICE leave_notify;
		leave_notify.m_dwRoleId = pPlayer->GetRoleId();
		SendMsgToTeamMember(pTeamData, &leave_notify);

		RemoveTeamTask(dwTeamId);
	}
	else
	{
		// ɾ���뿪�ĳ�Ա
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			if (pTeamData->arMemberList[i].pPlayer->GetRoleId() == pPlayer->GetRoleId())
			{
				pTeamData->arMemberList[i] = pTeamData->arMemberList[--pTeamData->wMemberNum];
				break;
			}
		}

		// todo::������ڴ���,�������һ��û��������

		// �㲥�����뿪
		MSG_S2C_TEAM_DUNGEON_LEAVE_NOTIFY msg;
		msg.m_dwRoleId = pPlayer->GetRoleId();
		msg.m_byIsTaskTeam = true;
		SendMsgToTeamMember(pTeamData, &msg);

		// �㲥��������,�����뿪����
		MSG_S2C_LEAVE_SCENE_NOTICE leave_notify;
		leave_notify.m_dwRoleId = pPlayer->GetRoleId();
		SendMsgToTeamMember(pTeamData, &leave_notify);
	}

	pPlayer->SetTeamDungeonId(0);
	pPlayer->SetTeamId(0);
}

// ����BOSS
bool CTeamDungeonManager::CreateBoss(TaskTeamData *pTeamData)
{
	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[TaskLinkId:%d]", __FUNCTION__, pTeamData->wTaskLinkId);
		return false;
	}

	// ��ʼ��Ա����
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
		MAPLOG(ERROR_LEVEL, "[%s]ȡ��Ӹ���Boss�����б�ʧ��[MapId:%d]!", __FUNCTION__, pTaskLinkCfg->dwBossMapId);
		return false;
	}

	// �����������
	const Scene_Config *pSeceneCfg = g_pCfgMgr->GetScene(pTeamData->tBossData.dwMapId);
	if (pSeceneCfg)
	{
		RandMapPos(pTeamData->tBossData.tMapPos, pSeceneCfg->vMonsterRefrshArea);
	}

	m_BattleBossTeamList.insert(make_pair(pTeamData->dwTeamId, DWORD(time(NULL))));

	MAPLOG(ERROR_LEVEL, "[%s]������Ӹ���Boss�ɹ�[BossMapId:%d,MonsterNum:%d]!", __FUNCTION__, 
		pTaskLinkCfg->dwBossMapId, pTeamData->tBossData.vMonsterList.size());

	return true;
}

// ����BOSS����
void CTeamDungeonManager::DoBossReward(TaskTeamData *pTeamData, DWORD dwKillerId)
{
	// ��ǰ�������״̬
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
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӹ�������[DungeonId:%d]!",__FUNCTION__, pTeamData->wDungeonId);
		return;
	}
		
	const TeamDungeonBossReward_Config *pBossRewardCfg = 
		g_pCfgMgr->GetTeamDungeonBossReward(pTeamData->wDungeonId, pTeamData->dwEnergy, nDamageRate);
	if (!pBossRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӹ���Boss��������[DungeonId:%d,DamageRate:%d, Energy:%d]!", 
			__FUNCTION__, pTeamData->wDungeonId, nDamageRate, pTeamData->dwEnergy);
		return;
	}

	const TeamDungeonTaskLink_Config *pTaskLinkCfg = g_pCfgMgr->GetTeamDungeonTaskLink(pTeamData->wTaskLinkId);
	if (!pTaskLinkCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ��������������[TaskLinkId:%d]!",__FUNCTION__, pTeamData->wTaskLinkId);
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
		// ����MVP
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

		// ���沿�ֽ���(ÿ���д�������)
		if (tPlayerDungeonData.wTodayBattleTimes < tMember.pPlayer->GetVipValue(VIP_TEAM_DUNGEON_DAILY_REWARD_TIMES))
		{
			for (size_t k = 0; k < pBossRewardCfg->vRewardList.size(); ++k)
			{
				const Item &tRewardItem = pBossRewardCfg->vRewardList[k];
				CItemManager::UseRandItem(tRewardItem.dwId, tRewardItem.dwNum, tMemberReward.vRewardList);
			}
			tMember.pPlayer->AddObjectList(tMemberReward.vRewardList, TRUE, FROM_TEAM_DUNGEON_BOSS_REWARD);
		}
			
		// ��¼����
		tPlayerDungeonData.wTodayBattleTimes++;

		// ��¼����ͨ�ؼ�¼
		tPlayerDungeonData.AddPassDungeon(pTeamData->wDungeonId);

		// ��ԾĿ��
		tMember.pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 12, 1, true);

		// ���ջ�ԾĿ��
		tMember.pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 12, 1, true);

		tMember.pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 12, 1, true);

		tMember.pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 12, 1, true);
	}

	// �����ɱ����
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

	// ����MVP����
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

	// �㲥����
	reward_notify.m_dwMVPId = dwMVPId;
	reward_notify.m_dwBossDamage = dwBossDamage;
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		pTeamData->arMemberList[i].pPlayer->SendPacket(&reward_notify);
	}

	// ����Ƿ���Լ������а�(ֻҪ���˳����ȼ�����,�򲻼���)
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

	// ��ɢ��ɫ����
	DWORD dwTeamId = pTeamData->dwTeamId;
	
	MAPLOG(ERROR_LEVEL, "[%s]��Ӹ�����սboss����[DungeonId:%d,TeamId:%d, MemberNum:%d, Killer:%d,MVP:%d]!",
		__FUNCTION__, pTeamData->wDungeonId, pTeamData->dwTeamId, pTeamData->wMemberNum, dwKillerId, dwMVPId);

	m_TaskTeamList.erase(dwTeamId);
}

// �Ƿ���ƥ���
bool CTeamDungeonManager::IsInMatchList(Player *pPlayer)
{
	DWORD dwDungeonId = pPlayer->GetTeamDungeonId();
	if (!dwDungeonId)
		return false;

	// �ڸ���ƥ���
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_PersonMatchList.find(dwDungeonId);
	if (iter != m_PersonMatchList.end())
	{
		if (iter->second.find(pPlayer->GetRoleId()) != iter->second.end())
			return true;
	}

	// �ڶ���ƥ���
	DWORD dwTeamId = pPlayer->GetTeamId();
	if (dwTeamId)
	{
		map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(dwDungeonId);
		if (iter != m_TeamMatchList.end() && iter->second.find(dwTeamId) != iter->second.end())
			return true;
	}

	return false;
}

// �Ƿ���׼��������
bool CTeamDungeonManager::IsInReadyTeam(Player *pPlayer)
{
	if (!pPlayer->GetTeamId())
		return false;

	map<DWORD, ReadyTeamData>::iterator iter = m_ReadyTeamList.find(pPlayer->GetTeamId());
	if (iter != m_ReadyTeamList.end())
		return true;

	return false;
}

// �Ƿ������������
bool CTeamDungeonManager::IsInTaskTeam(Player *pPlayer)
{
	if (!pPlayer->GetTeamId())
		return false;

	map<DWORD, TaskTeamData>::iterator iter = m_TaskTeamList.find(pPlayer->GetTeamId());
	if (iter != m_TaskTeamList.end())
		return true;

	return false;
}

// ȡ���⽱���б�
void CTeamDungeonManager::GetExtraRewardMemberList(WORD wTaskType, TaskTeamData *pTeamData, vector<DWORD>& vExtraRewardMemberList)
{
	// ���� ��һ����Ե����
	if (TASK_ANSWER == wTaskType)
	{
		DWORD dwMemberId = 0;

		// ���ҵ�һ����Եĳ�Ա
		DWORD dwFirstRightTarget = time(NULL);
		for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
		{
			TeamMember *pMember = &pTeamData->arMemberList[i];
			if (pMember->dwContext < dwFirstRightTarget/*todo::��Ҫ�ж��Ƿ���*/)
			{
				dwFirstRightTarget = pMember->dwContext;
				dwMemberId = pMember->pPlayer->GetRoleId();
			}
		}

		if (dwMemberId)
			vExtraRewardMemberList.push_back(dwMemberId);
	}
	// �Ի� ֻ�жӳ����
	else if (TASK_DIALOG == wTaskType)
	{
		if (pTeamData->wMemberNum)
			vExtraRewardMemberList.push_back(pTeamData->arMemberList[0].pPlayer->GetRoleId());
	}
	// �ɼ�,��� �ɼ�����,ɱ���������(�����ж�����һ����)
	else if (TASK_GATHER == wTaskType || TASK_MONSTER == wTaskType)
	{
		DWORD dwFinishMax = 0; // �����������
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

// ��ʱ����
void CTeamDungeonManager::TimeOutAnswerQuestion(TaskTeamData *pTeamData)
{
	DWORD dwTeamId = pTeamData->dwTeamId;

	// ȡ��������
	const TeamDungeonTask_Config *pTaskCfg = g_pCfgMgr->GetTeamDungeonTask(pTeamData->dwCurTaskId);
	if (!pTaskCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TeamId:%d,TaskId:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId);
		return;
	}

	// ����Ƿ��������
	if (pTaskCfg->wTaskType != TASK_ANSWER)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ���Ǵ�������[TeamId:%d,TaskId:%d,TaskType:%d]!",
			__FUNCTION__, dwTeamId, pTeamData->dwCurTaskId, pTaskCfg->wTaskType);
		return;
	}

	// �ﵽָ�����������,���������
	if (pTeamData->wCurTaskValue >= MAX_TEAM_MEMBER_NUM - 1)
	{
		DoFinishStageTask(*pTeamData, pTaskCfg);
	}
	// ��������������,�����³���Ŀ
	else
	{
		pTeamData->byAnswerTimes++;

		// ��������Ѵ�����,������һ����
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

// ������Ϣ׼��״̬�����Ա
void CTeamDungeonManager::SendMsgToReadyTeamMember(ReadyTeamData *pTeamData, NetMsgBody *pMsg)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		Player *pPlayer = pTeamData->arMemberList[i];
		if (!pPlayer)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Աָ��Ϊ��[idx:%d]!", __FUNCTION__, i);
			continue;
		}

		pPlayer->SendPacket(pMsg);
	}
}

// ɾ��׼�������Ա
void CTeamDungeonManager::RemoveReadyTeamMember(ReadyTeamData *pTeamData, DWORD dwRoleId)
{
	for (WORD i = 0; i < pTeamData->wMemberNum; ++i)
	{
		if (!pTeamData->arMemberList[i])
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Աָ��Ϊ��[idx:%d]!", __FUNCTION__, i);
			continue;
		}

		if (pTeamData->arMemberList[i]->GetRoleId() == dwRoleId)
		{
			pTeamData->arMemberList[i] = pTeamData->arMemberList[--pTeamData->wMemberNum];
			break;
		}
	}
}

// ɾ��׼���еĶ�������
void CTeamDungeonManager::RemoveReadyTeam(DWORD dwTeamId)
{
	m_ReadyTeamList.erase(dwTeamId);
}

// �������ȡ��ƥ��
void CTeamDungeonManager::DoTeamCancelMatch(WORD wDungeonId, DWORD dwTeamId)
{
	map<DWORD, map<DWORD, MatchTeamData>>::iterator iter = m_TeamMatchList.find(wDungeonId);
	if (iter == m_TeamMatchList.end())
		return;

	map<DWORD, MatchTeamData>::iterator iter_team = iter->second.find(dwTeamId);
	if (iter_team == iter->second.end())
		return;

	MatchTeamData *pMatchTeamData = &iter_team->second;

	// ֪ͨȡ��ƥ��
	MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY cannel_match_noifty;
	for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
	{
		pMatchTeamData->arMemberList[i]->SendPacket(&cannel_match_noifty);
	}

	// ���´�������
	ReadyTeamData tReadyTeam;
	tReadyTeam.wDungeonId = wDungeonId;
	tReadyTeam.wMemberNum = 0;
	for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
	{
		tReadyTeam.arMemberList[tReadyTeam.wMemberNum++] = pMatchTeamData->arMemberList[i];
	}
	AddReadyTeamData(dwTeamId, tReadyTeam);

	// ͬ�����ݶ�������
	MSG_S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY notity;
	notity.m_wMemberNum = 0;
	notity.m_dwTeamId = dwTeamId;
	for (WORD i = 0; i < tReadyTeam.wMemberNum; ++i)
	{
		tReadyTeam.arMemberList[i]->FillCommonData(&notity.m_MemberList[notity.m_wMemberNum++]);
	}
	SendMsgToReadyTeamMember(&tReadyTeam, &notity);

	// ��ƥ�����ȥ��
	iter->second.erase(iter_team);
}

// �����뿪����ƥ���
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

	// ����Ƕ������ֻ��һ����,��ֱ�ӽ�ɢ
	if (pMatchTeamData->dwTeamId == dwRoleId || pMatchTeamData->wMemberNum <= 1)
	{
		// ֪ͨȡ��ƥ��
		MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY cannel_match_noifty;
		SendMsgToMatchTeamMember(pMatchTeamData, &cannel_match_noifty);

		// ��ն���״̬
		for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
		{
			pMatchTeamData->arMemberList[i]->SetTeamDungeonId(0);
			pMatchTeamData->arMemberList[i]->SetTeamId(0);
		}

		iter_list->second.erase(iter);
	}
	else
	{
		// ɾ���뿪�ĳ�Ա
		for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
		{
			if (pMatchTeamData->arMemberList[i]->GetRoleId() == dwRoleId)
			{
				pMatchTeamData->arMemberList[i] = pMatchTeamData->arMemberList[--pMatchTeamData->wMemberNum];
				break;
			}
		}

		// ֪ͨȡ��ƥ��
		MSG_S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY cannel_match_noifty;
		SendMsgToMatchTeamMember(pMatchTeamData, &cannel_match_noifty);

		// ���´�������
		ReadyTeamData tReadyTeam;
		tReadyTeam.wDungeonId = wDungeonId;
		tReadyTeam.wMemberNum = 0;
		for (WORD i = 0; i < pMatchTeamData->wMemberNum; ++i)
		{
			tReadyTeam.arMemberList[tReadyTeam.wMemberNum++] = pMatchTeamData->arMemberList[i];
		}
		AddReadyTeamData(dwTeamId, tReadyTeam);

		// ͬ�����ݶ�������
		MSG_S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY notity;
		notity.m_wMemberNum = 0;
		notity.m_dwTeamId = dwTeamId;
		for (WORD i = 0; i < tReadyTeam.wMemberNum; ++i)
		{
			tReadyTeam.arMemberList[i]->FillCommonData(&notity.m_MemberList[notity.m_wMemberNum++]);
		}
		SendMsgToReadyTeamMember(&tReadyTeam, &notity);

		// ��ƥ�����ȥ��
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

			// �������㹻
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

			// �������㹻
			if (++wCurTargetNum >= wNeedTargetNum)
				return true;
		}
	}

	return false;
}

// ����ƥ��Ŀ��
bool CTeamDungeonManager::FindMatchTarget(WORD wDungeonId, WORD wNeedTargetNum, vector<MatchTeamData*> &vTeamList, vector<Player*> &vPersonList)
{
	WORD wThreePersonTeamNum = GetTeamSearchListNum(wDungeonId, 3);	// 3�˶�������
	WORD wTwoPersonTeamNum = GetTeamSearchListNum(wDungeonId, 2);	// 2�˶�������
	WORD wOnePersonTeamNum = GetTeamSearchListNum(wDungeonId, 1);	// 1�˶�������
	WORD wOnePersonNum = GetPersonSearchListNum(wDungeonId);		// ɢ������

	WORD wCurTargetNum = 0;

	// ��Ҫ1��Ŀ��
	if (1 == wNeedTargetNum)
	{
		// �����㹻
		if (wOnePersonTeamNum + wOnePersonNum >= wNeedTargetNum)
		{
			// ����ɢ��
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
	}
	// ��Ҫ2��Ŀ��
	else if (2 == wNeedTargetNum)
	{
		// 2�˶�
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

				// �������㹻
				if (wCurTargetNum >= wNeedTargetNum)
					return true;
			}
		}
		// 1�˶�+ɢ��
		else if (wOnePersonTeamNum + wOnePersonNum >= wNeedTargetNum)
		{
			// ����ɢ��
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
	}
	// ��Ҫ3��Ŀ��
	else
	{
		// 3�˶�
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

				// �������㹻
				if (wCurTargetNum >= wNeedTargetNum)
					return true;
			}
		}
		// һ��2�˶� + (һ��1�˶� ���� һ��ɢ��)
		else if (wTwoPersonTeamNum && (wOnePersonTeamNum + wOnePersonNum))
		{
			// 2�˶�
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

			// ����ɢ��
			map<DWORD, Player*> *pPersonSearchList = GetPersonSearchList(wDungeonId);
			vector<MatchTeamData*> vOneSearchList = GetTeamSearchData(wDungeonId, 1);
			if (FillOnePersonList(pPersonSearchList, vOneSearchList, vTeamList, vPersonList, wCurTargetNum, wNeedTargetNum))
				return true;
		}
		// 1�˶� + ɢ��
		else if (wOnePersonTeamNum + wOnePersonNum >= 3)
		{
			// ����ɢ��
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
	// ��һ����ֵ�����
	if (data1.dwUseTime < data2.dwUseTime)
		return true;

	// ��һ����ֵ���
	if (data1.dwUseTime == data2.dwUseTime && data1.dwFinishTime < data2.dwFinishTime)
		return true;

	return false;
}

// �������а�
void UpdateTeamDungeonRan(vector<TeamDungeonRank> &vRankList)
{
	std::sort(vRankList.begin(), vRankList.end(), TeamDungeonRankPred);

	// ��������
	for (size_t i = 0; i < vRankList.size(); ++i)
	{
		vRankList[i].wRank = i + 1;
	}
}

// ������а�����
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
		// ������а�������һ���򲻴���(�������ĵ�ʱ��)
		if (iter->second.size() >= MAX_TEAM_DUNGEON_RANK_NUM && tRankData.dwUseTime >= iter->second[iter->second.size() - 1].dwUseTime)
			return;

		iter->second.push_back(tRankData);

		// ��������
		UpdateTeamDungeonRan(iter->second);

		// ֻȡǰ3��
		if (iter->second.size() > MAX_TEAM_DUNGEON_RANK_NUM)
			iter->second.pop_back();

		// ���ݴ洢
		SaveDataToDB();
	}
}

// ��ӳ齱��¼
void CTeamDungeonManager::AddLotteryRecord(const TeamDungeonLotteryRecord &tLotteryRecord)
{
	m_LotteryRecordList.push_back(tLotteryRecord);

	// ֻȡ���20����¼
	if (m_LotteryRecordList.size() > 20)
	{
		m_LotteryRecordList.pop_front();
	}
}

// �㲥ƥ��Ŀ������
void CTeamDungeonManager::BroadcastMatchTargetNum(WORD wDungeonId)
{
	WORD wTargetNum = 0;

	// ɢ������
	map<DWORD, Player*> *pPersonMatchList = GetPersonSearchList(wDungeonId);
	if (pPersonMatchList)
	{
		wTargetNum += pPersonMatchList->size();
	}

	// ��������
	map<DWORD, MatchTeamData> *pTeamMatchList = GetTeamSearchList(wDungeonId);
	if (pTeamMatchList)
	{
		map<DWORD, MatchTeamData>::iterator iter = pTeamMatchList->begin();
		for (; iter != pTeamMatchList->end(); ++iter)
		{
			wTargetNum += iter->second.wMemberNum;
		}
	}

	// �㲥ɢ��
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

	// �㲥����
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
 