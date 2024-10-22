#include "stdafx.h"
#include "ScoreMatchManager.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\PlayerBaseDataManager.h"
#include "CommonLogic.h"
#include "..\Fuben\FubenManager.h"
#include "..\Rank\RankManager.h"
#include "..\MapServer.h"
#include "data_define.pb.h"
#include "..\MapSvrQuery.h"
#include "..\RewardCenter\RewardCenterManager.h"

CScoreMatchManager::CScoreMatchManager()
{
	m_byActivityStatus = 0;
	m_bIsLoadDBFinish = false;
	m_dwCurActivityIndex = 1;
}

CScoreMatchManager::~CScoreMatchManager()
{

}

void CScoreMatchManager::Init()
{
	m_byActivityStatus = 0;
	m_bIsLoadDBFinish = false;
	m_dwCurActivityIndex = 1;
}

void CScoreMatchManager::OnNewDay()
{
	std::map<DWORD, ScoreMatchData>::iterator iter = m_PlayerScoreMatchDataList.begin();
	for (; iter != m_PlayerScoreMatchDataList.end(); ++iter)
	{
		iter->second.wBattleTimes = 0;
		iter->second.wRefreshTimes = 0;
		iter->second.wBuyBattleTimes = 0;
		iter->second.wTaskRewardRecordNum = 0;
	}
}

// ����
void CScoreMatchManager::OnLogout(Player *pPlayer)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER(pFBMgr);

	// û���ڻ�������ս��,������
	if (pFBMgr->GetBattleType() != BATTLE_SCORE_MATCH)
		return;

	// ʧ�ܴ���
	DoBattleOver(pPlayer, false);
}

void CScoreMatchManager::Update()
{
	if (!m_bIsLoadDBFinish)
		return;

// 	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(m_dwCurActivityIndex);
// 	if (!pStageCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������λ����[season:%d]", __FUNCTION__, m_dwCurActivityIndex);
// 		return;
// 	}

	// ����
	if (0 == m_byActivityStatus)
	{
		if (IsActivityStart())
		{
			DoActivityStart();
			m_byActivityStatus = 1;
		}
	}
	// ���
	else if (1 == m_byActivityStatus)
	{
		if (IsActivityOver())
		{
			DoActivityOver();
			m_byActivityStatus = 2;
		}
	}
	// �ȴ�������
	if (2 == m_byActivityStatus)
	{
		if (IsActivityReward())
		{
			DoActivityReward();
			m_byActivityStatus = 0;
		}
	}

	// ��ʱ�洢����(1���Ӵ洢һ��)
	if (GetTickCount64() >= m_lProceesSaveTime + 60000)
	{
		SaveDataToDB();
		m_lProceesSaveTime = GetTickCount64();
	}
}

// �洢���ݵ����ݿ�
void CScoreMatchManager::SaveDataToDB()
{
	PB::ScoreMatchData PBScoreMatchData;

	// ��ɫ����������
	WORD wJoinRoleNum = 0;
	map<DWORD, ScoreMatchData>::iterator iter = m_PlayerScoreMatchDataList.begin();
	for (; iter != m_PlayerScoreMatchDataList.end(); ++iter)
	{
		::PB::RoleScoreMatchData *pPBRoleScoreMatchData = PBScoreMatchData.add_role_score_match_data_list();
		pPBRoleScoreMatchData->set_role_id(iter->second.dwRoleId);
		pPBRoleScoreMatchData->set_battle_times(iter->second.wBattleTimes);
		pPBRoleScoreMatchData->set_refresh_times(iter->second.wRefreshTimes);
		pPBRoleScoreMatchData->set_buy_battle_times(iter->second.wBuyBattleTimes);
		pPBRoleScoreMatchData->set_camp(iter->second.byCamp);
		pPBRoleScoreMatchData->set_win_times(iter->second.wWinTimes);
		// ��������¼
		for (WORD i = 0; i < iter->second.wTaskRewardRecordNum; ++i)
		{
			pPBRoleScoreMatchData->add_task_reward_record(iter->second.arTaskRewardRecord[i]);
		}
		// ��սĿ���б�
		for (WORD i = 0; i < iter->second.wBattleTargetNum; ++i)
		{
			ScoreMatchTarget &tTarget = iter->second.arBattleTargetList[i];
			::PB::ScoreMatchTarget *pPBTarget = pPBRoleScoreMatchData->add_battle_target_list();
			pPBTarget->set_target_id(tTarget.dwTargetId);
			pPBTarget->set_status(tTarget.wStatus);
		}

		wJoinRoleNum++;
	}

	string strSaveData("");
	PBScoreMatchData.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_SCORE_MATCH_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������洢���ݹ���[data_size:%d,list_num:%d]",
			__FUNCTION__, strSaveData.size(), PBScoreMatchData.role_score_match_data_list_size());
		return;
	}

	ScoreMatchData_Save* pQuery = ScoreMatchData_Save::ALLOC();
	pQuery->SetIndex(DB_SCORE_MATCH_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_SCORE_MATCH);
	pQuery->SetCurActivityIndex(m_dwCurActivityIndex);
	pQuery->SetActivityStatus(m_byActivityStatus);
	pQuery->SetActivityOverTime(m_dwActivityOverTime);
	pQuery->SetJoinRoleNum(wJoinRoleNum);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);
}



// ��������
void CScoreMatchManager::LoadDataFromDB(DWORD dwCurActivityIndex, BYTE byActivityStatus, DWORD dwActivityOverTime, const char *pData, int size)
{
	m_PlayerScoreMatchDataList.clear();

	if (size > 0)
	{
		PB::ScoreMatchData PBScoreMatchData;
		if (!PBScoreMatchData.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "�������������ݳ���(size:%d)", size);
			return;
		}

		// ��ɫ����������
		ScoreMatchData tScoreMatchData;
		for (int i = 0; i < PBScoreMatchData.role_score_match_data_list_size(); ++i)
		{
			const PB::RoleScoreMatchData &PBRoleScoreMatchData = PBScoreMatchData.role_score_match_data_list(i);
			tScoreMatchData.dwRoleId = PBRoleScoreMatchData.role_id();
			tScoreMatchData.wBattleTimes = PBRoleScoreMatchData.battle_times();
			tScoreMatchData.wRefreshTimes = PBRoleScoreMatchData.refresh_times();
			tScoreMatchData.wBuyBattleTimes = PBRoleScoreMatchData.buy_battle_times();
			tScoreMatchData.byCamp = PBRoleScoreMatchData.camp();
			tScoreMatchData.wWinTimes = PBRoleScoreMatchData.win_times();

			// ��������¼
			tScoreMatchData.wTaskRewardRecordNum = 0;	
			for (int k = 0; k < PBRoleScoreMatchData.task_reward_record_size() && k < MAX_SCORE_MATCH_TASK_NUM; ++k)
			{
				tScoreMatchData.arTaskRewardRecord[tScoreMatchData.wTaskRewardRecordNum++] = PBRoleScoreMatchData.task_reward_record(k);
			}
			// ��սĿ���б�
			tScoreMatchData.wBattleTargetNum = 0;
			for (int k = 0; k < PBRoleScoreMatchData.battle_target_list_size() && k < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++k)
			{
				const ::PB::ScoreMatchTarget &tPBTarget = PBRoleScoreMatchData.battle_target_list(k);
				ScoreMatchTarget &tTarget = tScoreMatchData.arBattleTargetList[tScoreMatchData.wBattleTargetNum++];
				tTarget.dwTargetId = tPBTarget.target_id();
				tTarget.wStatus = tPBTarget.status();
			}

			m_PlayerScoreMatchDataList.insert(make_pair(tScoreMatchData.dwRoleId, tScoreMatchData));
		}
	}

	m_bIsLoadDBFinish = true;
	m_dwCurActivityIndex = dwCurActivityIndex;
	m_byActivityStatus = byActivityStatus;
	m_dwActivityOverTime = dwActivityOverTime;
	m_lProceesSaveTime = GetTickCount64();
}

// ������������
void CScoreMatchManager::LoadDataRequest()
{
	// todo::��ʱ����
	//return;

	ScoreMatchData_Load* pQuery = ScoreMatchData_Load::ALLOC();
	pQuery->SetIndex(DB_SCORE_MATCH_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_SCORE_MATCH);
	g_MapServer.MapDBQuery(pQuery);
}

// �Ƿ���ʼ
bool CScoreMatchManager::IsActivityStart()
{
	const ScoreMathActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetScoreMatchActivityTime(m_dwCurActivityIndex);
	if (!pActivityTimeCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������ʱ������[season:%d]", __FUNCTION__, m_wCurSeason);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// ������ָ���ķ�Χ��
	if (nCurData > pActivityTimeCfg->dwStartDate && nCurData < pActivityTimeCfg->dwOverDate)
		return true;

	// ��ʼ�ĵ���
	if (nCurData == pActivityTimeCfg->dwStartDate && nCurTime >= pActivityTimeCfg->dwStartTime)
		return true;

	// ���һ��
	if (nCurData == pActivityTimeCfg->dwOverDate && nCurTime <= pActivityTimeCfg->dwOverTime)
		return true;

	return false;
}

// �Ƿ�����
bool CScoreMatchManager::IsActivityOver()
{
	const ScoreMathActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetScoreMatchActivityTime(m_dwCurActivityIndex);
	if (!pActivityTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������ʱ������[ActivityIndex:%d]", __FUNCTION__, m_dwCurActivityIndex);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// ������������һ����
	if (nCurData > pActivityTimeCfg->dwOverDate)
		return true;

	// ���һ��
	if (nCurData == pActivityTimeCfg->dwOverDate && nCurTime > pActivityTimeCfg->dwOverTime)
		return true;

	return false;
}

// �Ƿ����Ž���
bool CScoreMatchManager::IsActivityReward()
{
	// �Ӻ�3���ӷ���
	if (time(NULL) >= m_dwActivityOverTime + 180)
		return true;

	return false;
}

// ���ʼ
bool CScoreMatchManager::DoActivityStart()
{
	// ��������
	m_PlayerScoreMatchDataList.clear();

	// �������а�
	CRankManager::Instance()->ResetRoleRank(RANK_SCORE_MATCH_SUPER);
	CRankManager::Instance()->ResetRoleRank(RANK_SCORE_MATCH_LEGEND);
	CRankManager::Instance()->ResetRoleRank(RANK_SCORE_MATCH_WARCRAFT);

	MAPLOG(ERROR_LEVEL, "[%s]��������ѿ�ʼ[ActivityIndex:%d]",__FUNCTION__, m_dwCurActivityIndex);

	return true;
}

// �����
bool CScoreMatchManager::DoActivityOver()
{
	m_dwActivityOverTime = time(NULL);

	MAPLOG(ERROR_LEVEL, "[%s]��������ѽ���[ActivityIndex:%d]", __FUNCTION__, m_dwCurActivityIndex);

	return true;
}

// ��������
void CScoreMatchManager::DoActivityReward()
{
	// �������а�
	DoActivityReward(RANK_SCORE_MATCH_SUPER);

	// ����(��˵)���а�
	DoActivityReward(RANK_SCORE_MATCH_LEGEND);

	// ħ��(����)���а�
	DoActivityReward(RANK_SCORE_MATCH_WARCRAFT);

	// �����´λ
	m_dwCurActivityIndex++;

	MAPLOG(ERROR_LEVEL, "[%s]��������ѷ��Ž���[ActivityIndex:%d]", __FUNCTION__, m_dwCurActivityIndex);
}

// ��������
void CScoreMatchManager::DoActivityReward(WORD wRankType)
{
	// �������а�
	const AllRoleRankList *pAllRankList = CRankManager::Instance()->GetRankListEx(wRankType);
	if (!pAllRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������а�����[RankType:%d]", __FUNCTION__, wRankType);
		return;
	}

	AllRoleRankList::const_iterator iter1 = pAllRankList->begin();
	for (; iter1 != pAllRankList->end(); ++iter1)
	{
		RoleRankList::const_iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			const ScoreMatchRankReward_Config *pRewardCfg = g_pCfgMgr->GetScoreMatchRankReward(iter2->wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 28;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = iter2->wRank;
				CRewardCenterManager::AddReward(iter2->dwServerId, iter2->dwRoleId, tRewardData);

				MAPLOG(GUI_LEVEL, "����ҷ��Ż��������н���[role_id:%d,rank:%d,rank_type:%d]", iter2->dwRoleId, iter2->wRank, wRankType);
			}
		}
	}
}

// �������������
void CScoreMatchManager::OnNetScoreMatchData(Player *pPlayer)
{
	SendScoreMatchData(pPlayer);
}

// ѡ����Ӫ
void CScoreMatchManager::OnNetSelectCamp(Player *pPlayer, BYTE byCamp)
{
	// ����Ƿ��ڻ��
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������û��ʼ�����ѽ���", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return;
	}

	// ����������(���������ж�,���Ʋ��ᳬ��,��һ�������ٵ���)
	if (m_PlayerScoreMatchDataList.size() >= MAX_SCORE_MATCH_JOIN_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������������Ѵ�����![size:%d]", __FUNCTION__, m_PlayerScoreMatchDataList.size());
		return;
	}

	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӫ�������Ϸ�![camp:%d]",__FUNCTION__, byCamp);
		return;
	}

	// ���������������Ѿ�ѡ����Ӫ
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ѿ�ѡ�����Ӫ![camp:%d]", __FUNCTION__, byCamp);
		return;
	}

	ScoreMatchData tData;
	tData.byCamp = byCamp;
	tData.wBattleTimes = 0;
	tData.wBuyBattleTimes = 0;
	tData.wRefreshTimes = 0;
	tData.wWinTimes = 0;
	tData.wTaskRewardRecordNum = 0;
	tData.dwRoleId = pPlayer->GetRoleId();
	m_PlayerScoreMatchDataList.insert(make_pair(pPlayer->GetRoleId(), tData));
	pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����һ���������![roleid:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ��ȡ��սĿ��
	vector<ScoreMatchTarget> vTargetList;
	if (!GetTargetList(pPlayer, vTargetList) || vTargetList.size() != SCORE_MATCH_TARGET_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ��������սĿ�����[num:%d]!", __FUNCTION__, vTargetList.size());
		return;
	}

	// ��¼��սĿ��
	pScoreMatchData->wBattleTargetNum = 0;
	for (int i = 0; i < vTargetList.size() && i < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++i)
	{
		pScoreMatchData->arBattleTargetList[pScoreMatchData->wBattleTargetNum++] = vTargetList[i];
	}

	SendScoreMatchData(pPlayer);

	// ��Ӧ
	MSG_S2C_SCORE_MATCH_SELECT_CAMP_RESP msg;
	msg.m_byCamp = byCamp;
	pPlayer->SendPacket(&msg);
}

// ˢ��Ŀ��
void CScoreMatchManager::OnNetRefreshTarget(Player *pPlayer)
{
	if (!DoRefreshTarget(pPlayer, true))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ˢ����սĿ��ʧ��[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	SendScoreMatchData(pPlayer);

	// ��Ӧ
	MSG_S2C_SCORE_MATCH_REFRESH_TARGET_RESP msg;
	pPlayer->SendPacket(&msg);
}

// ������ս����
void CScoreMatchManager::OnNetBuyBattleTimes(Player *pPlayer)
{
	// ����Ƿ��ڻ��
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������û��ʼ�����ѽ���", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return;
	}

	// ȡ��һ���������
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ľ��ѡ����Ӫ!", __FUNCTION__);
		return;
	}

	// ��⹺�����
	if (pScoreMatchData->wBuyBattleTimes >= pPlayer->GetVipValue(VIP_SCORE_MATCH_BATTLE_TIMES_BUY))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս���������Ѵ�����!", __FUNCTION__);
		return;
	}

	int nCostDiamond = g_pCfgMgr->GetScoreMatchTimesCost(1, pScoreMatchData->wBuyBattleTimes + 1);

	// �������
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����������ս�������ĵ���ʯ����[Cost:%d]!", __FUNCTION__, nCostDiamond);
		return;
	}
	
	// ��ӹ������
	pScoreMatchData->wBuyBattleTimes++;

	// ������ʯ
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_SCORE_MATCH_BATTLE_TIMES_BUY);

	SendScoreMatchData(pPlayer);

	// ��Ӧ
	MSG_S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP msg;
	pPlayer->SendPacket(&msg);
}

// ��ȡ��ս������
void CScoreMatchManager::OnNetTakeBattleTaskReward(Player *pPlayer, WORD *pTaskList, WORD wTaskNum)
{
	// ������
	if (!pTaskList || !wTaskNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ������������Ϊ��!", __FUNCTION__);
		return;
	}

	// ȡ��һ���������
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ľ��ѡ����Ӫ[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ������
	for (WORD i = 0; i < wTaskNum; ++i)
	{
		// ȡ��������
		WORD wTaskId = pTaskList[i];

		// ����Ƿ�����ȡ��
		if (IsTaskRewardRecord(pPlayer->GetRoleId(), wTaskId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������ս��������ȡ��[RoleId:%d,TaskId:%d]!", __FUNCTION__, pPlayer->GetRoleId(), wTaskId);
			continue;
		}

		// ��ȡ������������
		const ScoreMatchBattleTask_Config *pRewardCfg = g_pCfgMgr->GetScoreMatchBattleTask(wTaskId);
		if (!pRewardCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������ս������������[TaskId:%d]!", __FUNCTION__, wTaskId);
			return;
		}
		
		// ����ʤ����
		if (pRewardCfg->wBattleTimes > pScoreMatchData->wBattleTimes)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������������Ҫ����ս��������[RewardId:%d]!", __FUNCTION__, wTaskId);
			return;
		}

		// ��ӽ���
		pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_SCORE_MATCH_BATTLE_TIMES_REWRAD);

		// ��������¼
		AddTaskRewardRecord(pPlayer->GetRoleId(), wTaskId);
	}

	// ��Ӧ
	MSG_S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP msg;
	msg.m_wTaskNum = wTaskNum;
	memcpy_s(msg.m_TaskList, wTaskNum*sizeof(WORD), pTaskList, wTaskNum*sizeof(WORD));
	pPlayer->SendPacket(&msg);
}

// ��ʼս��
bool CScoreMatchManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// ����Ƿ��ڻ��
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������û��ʼ�����ѽ���", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// ȡ��һ���������
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ľ��ѡ����Ӫ!", __FUNCTION__);
		return false;
	}

	// �����ս����
	if (pScoreMatchData->wBattleTimes >= g_GetConfigValue(95) + pScoreMatchData->wBuyBattleTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս��������!", __FUNCTION__);
		return false;
	}

	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
	if (!pTargetBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������Ҳ�����սĿ����Ϣ[TargetId:%d]!", __FUNCTION__, dwTargetId);
		return false;
	}

	pScoreMatchData->wBattleTimes++;

	pFBMgr->SetTargetId(dwTargetId);

	// ����ս��
	pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);

	pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

	return true;
}

// ����ս��
bool CScoreMatchManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	return DoBattleOver(pPlayer, byIsWin);
}

void CScoreMatchManager::SendScoreMatchData(Player *pPlayer)
{
	MSG_S2C_SCORE_MATCH_DATA_RESP msg;
	msg.m_wBattleTargetNum = 0;
	msg.m_wCurActivityIdx = m_dwCurActivityIndex;

	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (pScoreMatchData)
	{
		msg.m_MatchData = *pScoreMatchData;
		for (WORD i = 0; i < pScoreMatchData->wBattleTargetNum && i < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++i)
		{
			DWORD dwTargetId = pScoreMatchData->arBattleTargetList[i].dwTargetId;
			const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
			if (pTargetBaseData)
				msg.m_BattleTargetList[msg.m_wBattleTargetNum++] = *pTargetBaseData;

			// todo:: ����BUG��־
			//MAPLOG(ERROR_LEVEL, "��սĿ������![TargetId:%d,IsDead:%d]", dwTargetId, pScoreMatchData->arBattleTargetList[i].wStatus);
		}
	}
	else
	{
		msg.m_MatchData.wBattleTimes = 0;
		msg.m_MatchData.wRefreshTimes = 0;
		msg.m_MatchData.wBuyBattleTimes = 0;
		msg.m_MatchData.byCamp = CAMP_INVALID;
		msg.m_MatchData.wWinTimes = 0;
		msg.m_MatchData.wTaskRewardRecordNum = 0;
	}

	//MAPLOG(ERROR_LEVEL, "��ǰ�����![m_dwCurActivityIndex:%d]", msg.m_wCurActivityIdx);

	pPlayer->SendPacket(&msg);
}

// ȡ��սĿ���б�
bool CScoreMatchManager::GetTargetList(Player *pPlayer, vector<ScoreMatchTarget> &vTargetList)
{
	// ȡ��սĿ��������б�
	const ScoreMatchServerGroup_Config *pGroupServerCfg = g_pCfgMgr->GetScoreMatchTargetServerList(pPlayer->GetServerId());
	if (!pGroupServerCfg || !pGroupServerCfg->vGroupList.size())
	{
	 	MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������սĿ��������б�����б�Ϊ��![ServerId:%d]",__FUNCTION__, pPlayer->GetServerId());
	 	return false;
	}

	// �������ԭ�������б�
	map<DWORD,DWORD> vFindList;
	vector<DWORD> vTargetServerList;
	for (int i = 0; i < pGroupServerCfg->vGroupList.size(); ++i)
	{
		DWORD dwServerId = g_MapServer.GetCurServerId(pGroupServerCfg->vGroupList.at(i));

		// ��ֹ�Ϸ������������һ������Ӷ��
		if (vFindList.find(dwServerId) == vFindList.end())
		{
			vTargetServerList.push_back(dwServerId);
			vFindList.insert(make_pair(dwServerId, dwServerId));
		}
	}
	int nTargetServerListSize = vTargetServerList.size();
	for (int i = 0; i < nTargetServerListSize; ++i)
	{
		int nIndex = g_Rand(0, nTargetServerListSize-1);
		swap(vTargetServerList[nIndex], vTargetServerList[nTargetServerListSize - 1]);
	}

	// ȡ�������Ŀ������
	const ScoreMatchSearchTarget_Config *pRandTargetCfg = g_pCfgMgr->GetScoreMatchSearchTarget(2);
	if (!pRandTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�����������Ŀ������!", __FUNCTION__);
		return false;
	}
	// ս�������б����ó���
	if (!pRandTargetCfg->vBattleValueRateMinList.size() ||
		pRandTargetCfg->vBattleValueRateMinList.size() != pRandTargetCfg->vBattleValueRateMaxList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ŀ��ս�������б����ó���[rand]!", __FUNCTION__);
		return false;
	}
	// ȡ��սĿ������
	const ScoreMatchSearchTarget_Config *pLowTargetCfg = g_pCfgMgr->GetScoreMatchSearchTarget(1);
	if (!pLowTargetCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�������ҵ�ս��Ŀ������!", __FUNCTION__);
		return false;
	}
	// ս�������б����ó���
	if (!pLowTargetCfg->vBattleValueRateMinList.size() ||
		pLowTargetCfg->vBattleValueRateMinList.size() != pLowTargetCfg->vBattleValueRateMaxList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ŀ��ս�������б����ó���[low]!", __FUNCTION__);
		return false;
	}

	DWORD dwBattleValue = pPlayer->GetFightingPower();

	vector<ScoreMatchTarget> vRandTargetList;
	vector<ScoreMatchTarget> vLowTargetList;

	// ȡ���ս�����
	for (int i = 0; i < pRandTargetCfg->vBattleValueRateMinList.size(); ++i)
	{
		// Ŀ��ս��������
		DWORD dwTargetBattleValueMin = dwBattleValue - double(pRandTargetCfg->vBattleValueRateMinList[i] * 0.0001) * dwBattleValue;
		DWORD dwTargetBattleValueMax = dwBattleValue + double(pRandTargetCfg->vBattleValueRateMaxList[i] * 0.0001) * dwBattleValue;

		// ����������־
		//MAPLOG(ERROR_LEVEL, "���ս��Ŀ��[PowerMin:%d,PowerMax:%d,idx:%d]!", dwTargetBattleValueMin, dwTargetBattleValueMax, i);

		// �Ѳ��ҵ�ָ��������
		if (SearchTargetList(&vTargetServerList, pPlayer, vRandTargetList, dwTargetBattleValueMin, 
			dwTargetBattleValueMax, 20, pRandTargetCfg->wTargetNum, vTargetList))
			break;
	}

	// ����ѡĿ���б������Ƿ��㹻
	if (pRandTargetCfg->wTargetNum > vRandTargetList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ս��Ŀ���б���������[size:%d,role_id:%d,power:%d]!", 
			__FUNCTION__, vRandTargetList.size(), pPlayer->GetRoleId(), dwBattleValue);
		return false;
	}

// 	MAPLOG(ERROR_LEVEL, "���ս��Ŀ���б�����[size:%d", vRandTargetList.size());
// 	for (int i = 0; i < vRandTargetList.size(); ++i)
// 	{
// 		MAPLOG(ERROR_LEVEL, "Ŀ��ID[%d]", vRandTargetList[i].dwTargetId);
// 	}

	// ���Ŀ��
	while (vRandTargetList.size())
	{
		// Ŀ���Ѵ��
		if (vTargetList.size() >= pRandTargetCfg->wTargetNum)
			break;

		int nMax = vRandTargetList.size() - 1;
		int nIndex = g_Rand(0, nMax);
		//MAPLOG(ERROR_LEVEL, "size:%d,index:%d", vRandTargetList.size(), nIndex);
		vTargetList.push_back(vRandTargetList[nIndex]);	

		// ɾ���Ѿ�ѡ�е�
		vRandTargetList[nIndex] = vRandTargetList[nMax];
		vRandTargetList.pop_back();
	}

	// ȡ��ս�����
	for (int i = 0; i < pLowTargetCfg->vBattleValueRateMinList.size(); ++i)
	{
		// Ŀ��ս��������
		DWORD dwTargetBattleValueMin = dwBattleValue - double(pLowTargetCfg->vBattleValueRateMinList[i] * 0.0001) * dwBattleValue;
		DWORD dwTargetBattleValueMax = dwBattleValue + double(pLowTargetCfg->vBattleValueRateMaxList[i] * 0.0001) * dwBattleValue;

		// ����������־
		MAPLOG(ERROR_LEVEL, "��սս��Ŀ��[PowerMin:%d,PowerMax:%d,idx:%d]!", dwTargetBattleValueMin, dwTargetBattleValueMax, i);

		// �Ѳ��ҵ�ָ��������
		if (SearchTargetList(&vTargetServerList, pPlayer, vLowTargetList, dwTargetBattleValueMin, 
			dwTargetBattleValueMax, 10, pLowTargetCfg->wTargetNum, vTargetList))
			break;
	}

	// ����ѡĿ���б������Ƿ��㹻
	if (pLowTargetCfg->wTargetNum > vLowTargetList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ս��Ŀ���б���������[size:%d,role_id:%d,power:%d]!", 
			__FUNCTION__, vLowTargetList.size(), pPlayer->GetRoleId(), dwBattleValue);
		return false; 
	}

	//MAPLOG(ERROR_LEVEL, "��ս��Ŀ���б�����[size:%d", vRandTargetList.size());

	// ���Ŀ��
	while (vLowTargetList.size())
	{
		// Ŀ���Ѵ��
		if (vTargetList.size() >= pRandTargetCfg->wTargetNum + pLowTargetCfg->wTargetNum)
			break;

		int nMax = vLowTargetList.size() - 1;
		int nIndex = g_Rand(0, nMax);
		vTargetList.push_back(vLowTargetList[nIndex]);

		// ɾ���Ѿ�ѡ�е�
		vLowTargetList[nIndex] = vLowTargetList[nMax];
		vLowTargetList.pop_back();
	}

	return true;
}

// ȡ��սĿ���б�
bool CScoreMatchManager::SearchTargetList(const vector<DWORD> *pServerList, Player *pPlayer, vector<ScoreMatchTarget> &vTargetList,
	DWORD dwBattleValueMin, DWORD dwBattleValueMax, int nMaxTargetNum, int nMinTargetNum, const vector<ScoreMatchTarget> &vExceptList)
{
	// ���Ŀ���б�
	vTargetList.clear();

	ScoreMatchTarget tTarget;
	for (vector<DWORD>::const_iterator iter = pServerList->begin(); iter != pServerList->end(); ++iter)
	{
		DWORD dwServerId = *iter;
		const PlayerBaseDataMap *pPlayerList = PlayerBaseDataManager::Instance()->GetPlayerList(dwServerId);
		if (!pPlayerList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������б�![ServerId:%d]", __FUNCTION__, dwServerId);
			continue;
		}

		PlayerBaseDataMap::const_iterator iter1 = pPlayerList->begin();
		for (; iter1 != pPlayerList->end(); ++iter1)
		{
			PlayerBaseData *pBaseData = iter1->second;

			// ȥ���Լ�
			if (pPlayer->GetRoleId() == pBaseData->dwRoleId)
				continue;

			bool bIsExist = false;
			for (int i = 0; i < vExceptList.size(); ++i)
			{
				if (vExceptList[i].dwTargetId == pBaseData->dwRoleId)
				{
					bIsExist = true;
					break;
				}
			}

			// ����Ƿ����
			if (bIsExist)
				continue;

			// ս����ָ���ķ�Χ��
			if (pBaseData->dwFightingPower >= dwBattleValueMin && pBaseData->dwFightingPower <= dwBattleValueMax)
			{
				tTarget.wStatus = 0;
				tTarget.dwTargetId = pBaseData->dwRoleId;
				vTargetList.push_back(tTarget);

				//MAPLOG(ERROR_LEVEL, "Ŀ������[ServerId:%d,TargetId:%d]", dwServerId, pBaseData->dwRoleId);
			}

			// Ŀ���б��Ѵ��
			if (vTargetList.size() >= nMaxTargetNum)
				return true;
		}
	}

	// �Ѵﵽ��СĿ������Ҳ��ɹ�
	if (vTargetList.size() >= nMinTargetNum)
		return true;
	
	return false;
}


// ȡ��һ���������
ScoreMatchData* CScoreMatchManager::GetPlayerScoreMatchData(DWORD dwRoleId)
{
	std::map<DWORD, ScoreMatchData>::iterator iter = m_PlayerScoreMatchDataList.find(dwRoleId);
	return iter != m_PlayerScoreMatchDataList.end() ? &iter->second : NULL;
}

// ͨ����Ӫȡ���а�����
WORD CScoreMatchManager::GetRankTypeByCamp(WORD wCamp)
{
	WORD wRankType;

	// ����
	if (CAMP_SUPER == wCamp)
	{
		wRankType = RANK_SCORE_MATCH_SUPER;
	}
	// ����(��˵)
	else if (CAMP_LEGEND == wCamp)
	{
		wRankType = RANK_SCORE_MATCH_LEGEND;
	}
	// ħ��(����)
	else
	{
		wRankType = RANK_SCORE_MATCH_WARCRAFT;
	}

	return wRankType;
}

// �Ƿ�������������¼
bool CScoreMatchManager::IsTaskRewardRecord(DWORD dwRoleId, WORD wTaskId)
{
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(dwRoleId);
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����һ���������![dwRoleId:%d]", __FUNCTION__, dwRoleId);
		return false;
	}

	
	for (WORD i = 0; i < pScoreMatchData->wTaskRewardRecordNum; ++i)
	{
		if (pScoreMatchData->arTaskRewardRecord[i] == wTaskId)
			return true;
	}
	
	return false;
}

// �����������¼
void CScoreMatchManager::AddTaskRewardRecord(DWORD dwRoleId, WORD wTaskId)
{
	// ������
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(dwRoleId);
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����һ���������![dwRoleId:%d]", __FUNCTION__, dwRoleId);
		return;
	}

	// ����Ƿ�����
	if (pScoreMatchData->wTaskRewardRecordNum >= MAX_SCORE_MATCH_TASK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��һ������������б�����[num:%d]!", 
			__FUNCTION__, pScoreMatchData->wTaskRewardRecordNum);
		return;
	}

	pScoreMatchData->arTaskRewardRecord[pScoreMatchData->wTaskRewardRecordNum++] = wTaskId;
}

// ����ս�������߼�
bool CScoreMatchManager::DoBattleOver(Player *pPlayer, BYTE byIsWin)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// ȡ��һ���������
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ľ��ѡ����Ӫ!", __FUNCTION__);
		return false;
	}

	DWORD dwTargetId = pFBMgr->GetTargetId();


	// �����սĿ��
	ScoreMatchTarget *pTarget = NULL;
	for (WORD i = 0; i < pScoreMatchData->wBattleTargetNum; ++i)
	{
		if (pScoreMatchData->arBattleTargetList[i].dwTargetId == dwTargetId)
		{
			pTarget = &pScoreMatchData->arBattleTargetList[i];
			break;
		}
	}
	if (!pTarget)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����սĿ������[TargetId:%d]!", __FUNCTION__, dwTargetId);
		return false;
	}

	// ���Ŀ���Ƿ��ѹҵ�
	if (pTarget->wStatus)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��սĿ������ս��[TargetId:%d,Status:%d]!", __FUNCTION__, dwTargetId, pTarget->wStatus);
		return false;
	}

	// ȡ��սĿ���������
	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
	if (!pTargetBaseData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����սĿ���������[TargetId:%d]!", __FUNCTION__, dwTargetId);
		return false;
	}

	double fRate = g_GetConfigValue(290) * 0.0001;

	DWORD dwScore, dwScoreMatchHonor;


	// ���� = 50+����ս��/5��
	//dwScore = 50 + pTargetBaseData->dwFightingPower * 0.0005;

	// ʤ��
	if (byIsWin)
	{

		// սʤ����=math.floor(math.pow(����ս��,0.3354)+50)
		dwScore = floor(pow(pTargetBaseData->dwFightingPower, fRate) + 50);

		// սʤ����=math.floor(math.pow(����ս��,0.3354)+50)
		dwScoreMatchHonor = dwScore;

		// ���»�ʤ����
		pScoreMatchData->wWinTimes++;

		pTarget->wStatus = 1;
	}
	// ʧ��
	else
	{
		// ս�ܻ���=math.floor(math.pow(����ս��,0.3354)/2+25)
		dwScore = floor(pow(pTargetBaseData->dwFightingPower, fRate) * 0.5 + 25);

		// ս������=math.floor(math.pow(����ս��,0.3354)/2+25)
		dwScoreMatchHonor = dwScore;

		pTarget->wStatus = 2;
	}

	// ����Ƿ���Ҫ����ˢ��
	bool bIsNeedRefresh = true;
	for (WORD i = 0; i < pScoreMatchData->wBattleTargetNum; ++i)
	{
		if (!pScoreMatchData->arBattleTargetList[i].wStatus)
		{
			bIsNeedRefresh = false;
			break;
		}
	}
	if (bIsNeedRefresh)
		DoRefreshTarget(pPlayer, false);

	pPlayer->ModifyProperty(ROLE_PRO_SCORE_MATCH_HONOR, dwScoreMatchHonor, TRUE);

	WORD wRankType = GetRankTypeByCamp(pScoreMatchData->byCamp);

	// ֪ͨ�ͻ��˻�õĻ��ּ�����
	pFBMgr->SetParam(0, dwScore);
	pFBMgr->SetParam(1, dwScoreMatchHonor);


	// �������а�
	const ScoreMatchServerGroup_Config *pGroupServerCfg = g_pCfgMgr->GetScoreMatchTargetServerList(pPlayer->GetServerId());
	if (pGroupServerCfg)
	{
		DWORD dwTotalScore = CRankManager::Instance()->GetRoleRankValue(pPlayer->GetRoleId(), pGroupServerCfg->wRankGroupId, wRankType) + dwScore;
		CRankManager::Instance()->AddRoleRankData(wRankType, pPlayer, dwTotalScore, pScoreMatchData->wWinTimes, 0, 200);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������а�����[serverid:%d, type:%d]", __FUNCTION__, pPlayer->GetServerId(), wRankType);
	}

	return true;
}

// ����ˢ��Ŀ���߼�
bool CScoreMatchManager::DoRefreshTarget(Player *pPlayer, bool bIsNeedCost)
{
	// ����Ƿ��ڻ��
	if (!IsInActivity())
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������û��ʼ�����ѽ���", __FUNCTION__);
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		return false;
	}

	// ȡ��ҵĻ���������
	ScoreMatchData *pScoreMatchData = GetPlayerScoreMatchData(pPlayer->GetRoleId());
	if (!pScoreMatchData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ľ��ѡ����Ӫ!", __FUNCTION__);
		return false;
	}

	int nCostDiamond = g_pCfgMgr->GetScoreMatchTimesCost(2, pScoreMatchData->wRefreshTimes + 1);

	// ������ʯ����(todo::ǰ�漸�ο��ܻ�����Ϊ���)
	if (bIsNeedCost && pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������ˢ��Ŀ��������ʯ����[Cost:%d]!", __FUNCTION__, nCostDiamond);
		return false;
	}

	pScoreMatchData->wBattleTargetNum = 0;

	// ��ȡ��սĿ��
	vector<ScoreMatchTarget> vTargetList;
	if (!GetTargetList(pPlayer, vTargetList) || vTargetList.size() != SCORE_MATCH_TARGET_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ��������սĿ�����[num:%d]!", __FUNCTION__, vTargetList.size());
		return false;
	}

	// ��Ҫ����
	if (bIsNeedCost)
	{
		// ��������
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_SCORE_MATCH_REFRESH_TARGET);

		// ���ˢ�´���
		pScoreMatchData->wRefreshTimes++;
	}

	// ��¼��սĿ��
	// ��¼��սĿ��
	//pScoreMatchData->wBattleTargetNum = 0;
	for (int i = 0; i < vTargetList.size() && i < MAX_SCORE_MATCH_BATTLE_TARGET_NUM; ++i)
	{
		pScoreMatchData->arBattleTargetList[pScoreMatchData->wBattleTargetNum++] = vTargetList[i];
	}

	return true;
}

