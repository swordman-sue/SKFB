#include "stdafx.h"
#include "KingMatchManager.h"
#include "..\Player.h"
#include "..\Rank\RankManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Fuben\FubenManager.h"
#include "ToolKit\LogFile.h"
#include "..\stdafx.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Title\TitleManager.h"
#include "data_define.pb.h"
#include "..\MapSvrQuery.h"

CKingMatchManager::CKingMatchManager()
{
	m_wCurSeason = 1;
	m_wDataBelongSeason = 0;
	m_bIsLoadDBFinish = false;
	//m_bIsInActivity = false;
	m_byActivityStatus = 0;
	m_dwActivityOverTime = 0;
	m_lProceesSaveTime = 0;
}

CKingMatchManager::~CKingMatchManager()
{

}

void CKingMatchManager::Init()
{
	m_wCurSeason = 1;
	m_wDataBelongSeason = 0;
	m_bIsLoadDBFinish = false;
	m_byActivityStatus = 0;
	m_dwActivityOverTime = 0;
	m_lProceesSaveTime = 0;
}

void CKingMatchManager::Release()
{

}

void CKingMatchManager::Update()
{
	if (!m_bIsLoadDBFinish)
		return;

	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(m_wCurSeason);
	if (!pStageCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������λ����[season:%d]", __FUNCTION__, m_wCurSeason);
		return;
	}

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
			m_dwActivityOverTime = time(NULL);
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

// �ǳ�
void CKingMatchManager::OnLogout(Player *pPlayer)
{	
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER(pFBMgr);
	BYTE byBattleType = pFBMgr->GetBattleType();
	if (BATTLE_KING_MATCH == byBattleType && GetPlayerStageLevel(pPlayer->GetRoleId()))
	{
		if (!DoBattleOver(pPlayer, false))
		{
			MAPLOG(ERROR_LEVEL, "[%s]�������ʱ��������������ʧ��[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		}
	}
}

// ��¼
void CKingMatchManager::OnLogin(Player *pPlayer)
{

}

// ���ʼ
bool CKingMatchManager::DoActivityStart()
{
	int nBattleTargetNum = 0;

	WORD wOpenLevel = g_pCfgMgr->GetSystemOpenLevel(108);

	// ��������뵱ǰ������һ�� ���� ��λ����Ϊ��
	if (m_wCurSeason != m_wDataBelongSeason || !m_StageDataList.size())
	{
		// ������а�����
		CRankManager::Instance()->ResetRoleRank(ALL_SERVER_ID, RANK_KINGMATCH);

		// �������
		m_StageDataList.clear();

		// �����Ҷ�λ����
		//m_PlayerStageLevelList.clear();

		DWORD dwPowerMin = 0;
		DWORD dwPowerMax = 0;
		const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(238);
		if (pSvrCfg)
		{
			dwPowerMin = pSvrCfg->GetListValue(0);
			dwPowerMax = pSvrCfg->GetListValue(1);
		}

		// ����һ����������Ϊ��սĿ��,��ֹ�Ƚ������û��Ҳ���Ŀ��

		const map<DWORD, ServerInfo> &serverList = g_MapServer.GetServerList();
		map<DWORD, ServerInfo>::const_iterator svr_iter = serverList.begin();
		for (; svr_iter != serverList.end(); ++svr_iter)
		{
			const PlayerBaseDataMap *pBaseDataList = PlayerBaseDataManager::Instance()->GetPlayerList(svr_iter->second.dwServerId);
			PlayerBaseDataMap::const_iterator base_data_iter = pBaseDataList->begin();
			for (; base_data_iter != pBaseDataList->end(); ++base_data_iter)
			{
				PlayerBaseData *pBaseData = base_data_iter->second;

				// ����ָ���ȼ�,ս��,��û������ս�б�
				if (pBaseData->wLevel >= wOpenLevel
					&& pBaseData->dwFightingPower >= dwPowerMin
					&& pBaseData->dwFightingPower <= dwPowerMax
					&& !GetPlayerStageLevel(pBaseData->dwRoleId))
				{
					AddStageData(pBaseData->wServerId, pBaseData->dwRoleId);
					nBattleTargetNum++;
				}

				if (nBattleTargetNum >= 100)
					break;
			}

			if (nBattleTargetNum >= 100)
				break;
		}

		m_wDataBelongSeason = m_wCurSeason;
	}

	MAPLOG(ERROR_LEVEL, "[%s]��������ѿ�ʼ[season:%d,init_target_num:%d, total_target_num:%d]", 
		__FUNCTION__, m_wCurSeason, nBattleTargetNum, GetTotalBattleTargetNum());

	return true;
}

// �����
bool CKingMatchManager::DoActivityOver()
{
	MAPLOG(ERROR_LEVEL, "[%s]��������ѽ���[season:%d]", __FUNCTION__, m_wCurSeason);

	return true;
}

// ��������
void CKingMatchManager::DoActivityReward()
{
	DoSeasonRankReward();
	DoSeasonStageReward();

	vector<RoleKingMatchSeasonRank> vNewRankList;
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_KINGMATCH);
	if (pRankList && pRankList->size())
	{
		RoleRankList::const_iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			RoleKingMatchSeasonRank rankData;
			rankData.dwRoleId = iter->dwRoleId;
			rankData.wServerId = iter->dwServerId;
			vNewRankList.push_back(rankData);

			// ֻȡǰ3��
			if (vNewRankList.size() >= MAX_KING_MATCH_TOP_RANK_NUM)
				break;
		}
	}

	AddSeasonRank(m_wCurSeason, vNewRankList);

	// ������һ����
	m_wCurSeason++;

	MAPLOG(ERROR_LEVEL, "[%s]��������ѷ��Ž���[season:%d]", __FUNCTION__, m_wCurSeason);
}

// ����������λ����
void CKingMatchManager::DoSeasonStageReward()
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.begin();
	for (; stage_iter != m_StageDataList.end(); ++stage_iter)
	{
		const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(stage_iter->first);
		if (!pStageCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������������[season:%d]", __FUNCTION__, stage_iter->first);
			continue;
		}

		if (pStageCfg->vSeasonReward.size())
		{
			map<DWORD, KingMatchBattleTarget>::iterator target_iter = stage_iter->second.begin();
			for (; target_iter != stage_iter->second.end(); ++target_iter)
			{
				// ���Ž���, ֻҪ��ս���Ķ��ᷢ
				if (target_iter->second.wBattleTimes)
				{
					// ��ʱ���������Ľ���
					RewardData tRewardData;
					tRewardData.wInfoId = 22;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = stage_iter->first;
					CRewardCenterManager::AddReward(target_iter->second.wServerId, target_iter->second.dwRoleId, tRewardData);
				}
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]���������Ŷ�λ����", __FUNCTION__);
}

// �����������н���
void CKingMatchManager::DoSeasonRankReward()
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(ALL_SERVER_ID, RANK_KINGMATCH);
	if (pRankList && pRankList->size())
	{
		RoleRankList::const_iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			// �㲥��һ��:{1}�����������ѽ�����{2}���������������������ٻ�������ǿ���ߣ�
			if (1 == iter->wRank)
			{
				const PlayerBaseData *pNo1 = g_GetPlayerBaseData(iter->dwServerId, iter->dwRoleId, false);
				if (pNo1)
				{
					CSystemNotice sys_notice(21);
					sys_notice.AddParam(m_wCurSeason);
					sys_notice.AddParam(pNo1->szRoleName);
					sys_notice.SendMsg();
				}
			}

			const KingMatchRankReward_Config *pRewardCfg = g_pCfgMgr->GetKingMatchRankReward(iter->wRank);
			if (pRewardCfg)
			{
				// ��Ʒ����
				if (pRewardCfg->vRewardList.size())
				{
					// todo::��ʱ���������Ľ���
					RewardData tRewardData;
					tRewardData.wInfoId = 21;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = iter->wRank;
					CRewardCenterManager::AddReward(iter->dwServerId, iter->dwRoleId, tRewardData);
				}


				// �ƺŽ���
				if (pRewardCfg->dwTitleId)
				{
					CTitleManager::AddTitleEx(iter->dwServerId, iter->dwRoleId, pRewardCfg->dwTitleId);

					MAPLOG(ERROR_LEVEL, "[%s]���������з��ųƺŽ���[role_id:%d,rank:%d,title_id:%d]",
						__FUNCTION__, iter->dwRoleId, iter->wRank, pRewardCfg->dwTitleId);
				}
			}
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]�������������н���", __FUNCTION__);
}



// �洢���ݵ����ݿ�
void CKingMatchManager::SaveDataToDB()
{
	PB::KingMatchData PBKingMatchData;

	// ��λ����
	WORD wBattleTargetNum = 0;
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.begin();
	for (; stage_iter != m_StageDataList.end(); ++stage_iter)
	{
		::PB::KingMatchStage *pPBStageList = PBKingMatchData.add_stage_list();
		pPBStageList->set_stage(stage_iter->first);
		map<DWORD, KingMatchBattleTarget>::iterator iter = stage_iter->second.begin();
		for (; iter != stage_iter->second.end(); ++iter)
		{
			KingMatchBattleTarget &tBattleTarget = iter->second;
			::PB::KingMatchBattleTarget *pPBBattleTarget = pPBStageList->add_battle_target_list();
			pPBBattleTarget->set_role_id(tBattleTarget.dwRoleId);
			pPBBattleTarget->set_server_id(tBattleTarget.wServerId);
			pPBBattleTarget->set_stage_level(tBattleTarget.wStageLevel);
			pPBBattleTarget->set_star(tBattleTarget.wStar);
			pPBBattleTarget->set_battle_times(tBattleTarget.wBattleTimes);

			wBattleTargetNum++;
		}
	}

	// �������а��¼
	LastSeasonRankList::iterator season_iter = m_LastSeasonRankList.begin();
	for (; season_iter != m_LastSeasonRankList.end(); ++season_iter)
	{
		::PB::SeasonRankData *pLastSeasonRankList = PBKingMatchData.add_last_season_rank_list();
		pLastSeasonRankList->set_season(season_iter->first);
		vector<RoleKingMatchSeasonRank>::iterator rank_iter = season_iter->second.begin();
		for (; rank_iter != season_iter->second.end(); ++rank_iter)
		{
			::PB::RoleSeasonRank *pRank = pLastSeasonRankList->add_rank_list();
			pRank->set_role_id(rank_iter->dwRoleId);
			pRank->set_server_id(rank_iter->wServerId);
		}
	}

	string strSaveData("");
	PBKingMatchData.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_KING_MATCH_STAGE_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������洢���ݹ���[data_size:%d,list_num:%d]",
			__FUNCTION__, strSaveData.size(), PBKingMatchData.stage_list_size());
		return;
	}

	KingMatchData_Save* pQuery = KingMatchData_Save::ALLOC();
	pQuery->SetIndex(DB_KING_MATCH_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_KING_MATCH);
	pQuery->SetDataBelongSeason(m_wDataBelongSeason);
	pQuery->SetCurSeason(m_wCurSeason);
	pQuery->SetBattleTargetNum(wBattleTargetNum);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);
}

// �����ݿ��������
// void CKingMatchManager::LoadDataFromDB(const KingMatchBattleTarget *pTargetList, WORD wTargetListNum)
// {
// 	for (WORD i = 0; i < wTargetListNum; ++i)
// 	{
// 		const KingMatchBattleTarget &tTarget = pTargetList[i];
// 		AddStageData(tTarget.wServerId, tTarget.dwRoleId, tTarget.wStageLevel, tTarget.wStar);
// 	}
// }

// ��������
void CKingMatchManager::LoadDataFromDB(WORD wCurSeason, WORD wDataBelongSeason, const char *pData, int size)
{
	m_StageDataList.clear();
	m_LastSeasonRankList.clear();

	if (size > 0)
	{
		PB::KingMatchData PBKingMatchData;
		if (!PBKingMatchData.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "�������������ݳ���(size:%d)", size);
			return;
		}

		// ��λ����
		KingMatchBattleTarget tBattleTarget;
		for (int i = 0; i < PBKingMatchData.stage_list_size(); ++i)
		{
			map<DWORD, KingMatchBattleTarget> Battle_Target_List;
			const PB::KingMatchStage &PBStageData = PBKingMatchData.stage_list(i);
			for (int k = 0; k < PBStageData.battle_target_list_size(); ++k)
			{
				const PB::KingMatchBattleTarget &PBBattleTarget = PBStageData.battle_target_list(k);
				tBattleTarget.dwRoleId = PBBattleTarget.role_id();
				tBattleTarget.wServerId = PBBattleTarget.server_id();
				tBattleTarget.wStageLevel = PBBattleTarget.stage_level();
				tBattleTarget.wStar = PBBattleTarget.star();
				tBattleTarget.wBattleTimes = PBBattleTarget.battle_times();
				Battle_Target_List.insert(make_pair(tBattleTarget.dwRoleId, tBattleTarget));
			}
			m_StageDataList.insert(make_pair(PBStageData.stage(), Battle_Target_List));
		}

		// ������������
		for (int i = 0; i < PBKingMatchData.last_season_rank_list_size(); ++i)
		{
			const PB::SeasonRankData &PBLastSeasonRankList = PBKingMatchData.last_season_rank_list(i);
			vector<RoleKingMatchSeasonRank> vRankList;
			for (int k = 0; k < PBLastSeasonRankList.rank_list_size(); ++k)
			{
				const ::PB::RoleSeasonRank &PBRankData = PBLastSeasonRankList.rank_list(k);
				RoleKingMatchSeasonRank tRankData;
				tRankData.dwRoleId = PBRankData.role_id();
				tRankData.wServerId = PBRankData.server_id();
				vRankList.push_back(tRankData);
			}
			AddSeasonRank(PBLastSeasonRankList.season(), vRankList);
		}
	}

	m_wCurSeason = wCurSeason;
	m_wDataBelongSeason = wDataBelongSeason;
	m_bIsLoadDBFinish = true;
	m_lProceesSaveTime = GetTickCount64();
}

// ������������
void CKingMatchManager::LoadDataRequest()
{
	KingMatchData_Load* pQuery = KingMatchData_Load::ALLOC();
	pQuery->SetIndex(DB_KING_MATCH_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_KING_MATCH);
	pQuery->SetCurSeason(m_wCurSeason);
	pQuery->SetDataBelongSeason(m_wDataBelongSeason);
	g_MapServer.MapDBQuery(pQuery);
}

// �Ƿ��к��
bool CKingMatchManager::IsHaveRedPoint(Player *pPlayer)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;
	map<WORD, KingMatchSeasonTask_Config>::iterator iter = g_pCfgMgr->m_KingMatchSeasonTaskList.begin();
	for (; iter != g_pCfgMgr->m_KingMatchSeasonTaskList.end(); ++iter)
	{
		// ��ʤ��������
		if (tKingMatchData.wTotalWinTimes >= iter->second.wWinTimes)
		{
			// ��û��ȡ
			if (!IsTakeSeasonTaskReward(tKingMatchData, iter->second.wWinTimes))
				return true;
		}
	}

	return false;
}

// fot gm
void CKingMatchManager::SetStageForTest(Player *pPlayer, WORD wStage)
{
	// ������õ������Ϸ���
	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(wStage);
	if (!pStageCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������λ����[Stage:%d]", __FUNCTION__, wStage);
		return;
	}

	OnNetPlayerKingMatchData(pPlayer);

	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	RemoveStageData(tKingMatchData.wStageLevel, pPlayer->GetRoleId());
	AddStageData(pPlayer->GetServerId(), pPlayer->GetRoleId(), wStage, tKingMatchData.dwContext, tKingMatchData.wTotalBattleTimes);
	tKingMatchData.wStageLevel = wStage;
	if (tKingMatchData.wStageLevel > tKingMatchData.wHistoryStageLevel)
	{
		tKingMatchData.wHistoryStageLevel = tKingMatchData.wStageLevel;
	}
}

// ��ʼս��
bool CKingMatchManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// ����Ƿ���
	if (m_byActivityStatus != 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������û��ʼ[RoleId:%d]!",__FUNCTION__,pPlayer->GetRoleId());
		return false;
	}

	// ������ڵķ������ܷ�μ�
// 	if (!IsHaveActivity(pPlayer->GetRoleId()))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]������ڵķ��������ܲμ�[RoleId:%d,ServerId:%d]!",
// 			__FUNCTION__, pPlayer->GetRoleId(), pPlayer->GetServerId());
// 		return false;
// 	}

	// ���Ŀ���Ƿ�Ϸ�
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	if (pFBMgr->GetTargetId() != dwTargetId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��սĿ������쳣[ClientTargetId:%d, ServerTargetId:%d]!",
			__FUNCTION__, dwTargetId, pFBMgr->GetTargetId());
		return false;
	}

	WORD wServerId = pFBMgr->GetTargetServerId();

	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(wServerId, dwTargetId, true);
	if (pTargetBaseData)
	{
		pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

		// ����ս��
		pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
	}

	return true;
}

// ����ս��
bool CKingMatchManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	return DoBattleOver(pPlayer, byIsWin);
}

// ������ҵ�����������
void CKingMatchManager::OnNetPlayerKingMatchData(Player *pPlayer)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	KingMatchBattleTarget *pBattleTarget = GetStageData(pPlayer->GetRoleId());
	if (pBattleTarget)
	{
		// ������Ҫ����
		if (!pBattleTarget->wBattleTimes)
		{
			memset(&tKingMatchData, 0, sizeof(tKingMatchData));
			tKingMatchData.wLastSeason = m_wCurSeason;
			tKingMatchData.wStageLevel = 1;
		}

		// �����ݾ���(��ֹ���ݴ洢������)
		if (pBattleTarget->wStageLevel != tKingMatchData.wStageLevel)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ҷ�λ�����쳣,Ҫ���о���[RoleId:%d,RoleStage:%d,ListStage:%d]!",
				__FUNCTION__, pPlayer->GetRoleId(), tKingMatchData.wStageLevel, pBattleTarget->wStageLevel);

			tKingMatchData.wStageLevel = pBattleTarget->wStageLevel;
		}
	}
	else
	{
		// ��û�����λ
		memset(&tKingMatchData, 0, sizeof(tKingMatchData));
		tKingMatchData.wLastSeason = m_wCurSeason;
		tKingMatchData.wStageLevel = 1;
		AddStageData(pPlayer->GetServerId(), pPlayer->GetRoleId(), tKingMatchData.wStageLevel);
	}

	SendKingMatchData(pPlayer);
}

// ƥ��Ŀ��
void CKingMatchManager::OnNetMatchTarget(Player *pPlayer)
{
	WORD wFromMsgId = C2S_KING_MATCH_SEARCH_TARGET_REQ;

	// ����Ƿ���
	if (m_byActivityStatus != 1)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������û��ʼ[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	// ȡ��λ����
	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
	if (!pStageCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������λ����[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
		return;
	}

	// �������λ
	int nTotalRate = 0;
	for (size_t i = 0; i < pStageCfg->vMatchingStageList.size(); ++i)
	{
		nTotalRate += pStageCfg->vMatchingStageList[i].nRate;
	}
	int nRandValue = g_Rand(0, nTotalRate);
	WORD wRandStageLevel = 0;
	int nCurRate = 0;
	for (size_t i = 0; i < pStageCfg->vMatchingStageList.size(); ++i)
	{
		nCurRate += pStageCfg->vMatchingStageList[i].nRate;
		if (nRandValue <= nCurRate)
		{
			wRandStageLevel = pStageCfg->vMatchingStageList[i].nValue;
			break;
		}
	}

	// �����Ŀ��
	const map<DWORD, KingMatchBattleTarget> *pTargetList = NULL;
	while (wRandStageLevel)
	{
		// ȡ������Ķ�λĿ���б�
		pTargetList = GetStageLevelList(wRandStageLevel);
		if (!pTargetList)
			break;

		// Ŀ���б������ұ�֤����������(��Ϊ���ֻ��һ���˶��Ǹ��˸غ����Լ��Ǿͱ�����...)
		if (pTargetList->size() > 1)
			break;

		wRandStageLevel--;
	}
	if (!pTargetList || !pTargetList->size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������ƥ�䲻��Ŀ��[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
		return;
	}
	KingMatchBattleTarget tTarget;
	memset(&tTarget, 0, sizeof(tTarget));

	WORD wDoTimes = 0;
	while (true)
	{
		int nRandIdx = g_Rand(0, pTargetList->size() - 1);
		int nCurIdx = 0;
		for (map<DWORD, KingMatchBattleTarget>::const_iterator iter = pTargetList->begin(); iter != pTargetList->end(); ++iter)
		{
			// ��ʱ��������
			if (wDoTimes++ >= 1000)
				break;

			// ȥ���Լ�
			if (iter->second.dwRoleId == pPlayer->GetRoleId())
				continue;

			// ʹ�� ">=" ��ԭ����Ҫ�ǵ�ʹ�õ��Լ�ʱ,��ȡ��һ��Ŀ��
			if (nCurIdx++ >= nRandIdx)
			{
				tTarget = iter->second;
				break;
			}
		}

		// �����Ŀ����
		if (tTarget.dwRoleId)
			break;

		// ��ʱ��������
		if (wDoTimes++ >= 1000)
			break;
	}

	MSG_S2C_KING_MATCH_SEARCH_TARGET_RESP msg;
	msg.m_wStageLevel = wRandStageLevel;
	msg.m_byStar = tTarget.wStar;
	msg.m_wRank = 0;
	msg.m_dwRankValue = 0;
	msg.m_TargetData.dwRoleId = tTarget.dwRoleId;
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(g_MapServer.GetCurServerId(tTarget.wServerId), tTarget.dwRoleId);
	if (pBaseData)
		memcpy_s(&msg.m_TargetData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(ALL_SERVER_ID, RANK_KINGMATCH, tTarget.dwRoleId);
	if (pRankData)
	{
		msg.m_wRank = pRankData->wRank;
		msg.m_dwRankValue = pRankData->dwValue;
	}
	pPlayer->SendPacket(&msg);

	// ��¼ƥ�䵽����սĿ��
	pPlayer->GetFubenManager()->SetTargetId(tTarget.dwRoleId);
	pPlayer->GetFubenManager()->SetTargetServerId(tTarget.wServerId);

	MAPLOG(ERROR_LEVEL, "[%s]������ƥ��Ŀ����Ϣ[wRandStageLevel:%d, TargetId:%d]!", __FUNCTION__, wRandStageLevel, tTarget.dwRoleId);
}

// ��ȡ����������
void CKingMatchManager::OnNetTakeSesionTaskReward(Player *pPlayer, WORD wWinTimes)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	if (tKingMatchData.wTotalWinTimes < wWinTimes)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʤ��������,������ȡ[CurWinTimes:%d,ReqWinTimes:%d]!",
			__FUNCTION__, tKingMatchData.wTotalWinTimes, wWinTimes);
		return;
	}

	if (tKingMatchData.wSeasonRewardRecordNum >= MAX_SEASON_TASK_REWARD_RECORD)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������������¼�б�����[num:%d]!", __FUNCTION__, tKingMatchData.wSeasonRewardRecordNum);
		return;
	}

	const KingMatchSeasonTask_Config *pTaskCfg = g_pCfgMgr->GetKingMatchSeasonTask(wWinTimes);
	if (!pTaskCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[WinTimes:%d]!", __FUNCTION__, wWinTimes);
		return;
	}

	// ������
	pPlayer->AddObjectList(pTaskCfg->vRewardList, TRUE, FROM_KING_MATCH_SEASON_TASK_REWARD);

	tKingMatchData.arSeasonTaskRewardRecord[tKingMatchData.wSeasonRewardRecordNum++] = wWinTimes;

	MSG_S2C_TAKE_KING_MATCH_TASK_REWARD_RESP msg;
	msg.m_wWinTimes = wWinTimes;
	pPlayer->SendPacket(&msg);
}

// ��������������а�
void CKingMatchManager::OnNetRecentlySesionRank(Player *pPlayer)
{
	MSG_S2C_KING_MATCH_SEASON_RANK_RESP msg;
	msg.m_wSeasonRankNum = 0;

	LastSeasonRankList::iterator season_iter = m_LastSeasonRankList.begin();
	for (; season_iter != m_LastSeasonRankList.end(); ++season_iter)
	{
		if (msg.m_wSeasonRankNum >= RECENTLY_KING_MATCH_SEASON_RANK_NUM)
			break;

		KingMatchRankRecord &season_record = msg.m_SeasonRankList[msg.m_wSeasonRankNum++];
		season_record.wRoleRankNum = 0;
		season_record.wSeason = season_iter->first;
		vector<RoleKingMatchSeasonRank>::iterator role_iter = season_iter->second.begin();
		for (; role_iter != season_iter->second.end(); ++role_iter)
		{
			if (season_record.wRoleRankNum >= MAX_KING_MATCH_TOP_RANK_NUM)
				break;

			DWORD dwCurServerId = g_MapServer.GetCurServerId(role_iter->wServerId, role_iter->dwRoleId);
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwCurServerId, role_iter->dwRoleId, true);
			if (pBaseData)
			{
				PlayerCommonData &role_record = season_record.arRoleRankList[season_record.wRoleRankNum++];
				memcpy_s(&role_record, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
			}
			else
			{
				MAPLOG(IMPORTANT_LEVEL, "[%s]�Ҳ����������[ServerId:%d,RoleId:%d]!",
					__FUNCTION__, role_iter->wServerId, role_iter->dwRoleId);
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// ���õ�ǰ����
void CKingMatchManager::SetCurSeason(WORD wSeason)
{
	// ����ǿ���״̬��������,��ֹ����ڽ�����,�����
	if (m_byActivityStatus)
		return;

	// ������õ������Ϸ���
	const KingMatchSeasonTime_Config *pSeasonTimeCfg = g_pCfgMgr->GetKingMatchSeasonTime(wSeason);
	if (!pSeasonTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������ʱ������[season:%d]", __FUNCTION__, wSeason);
		return;
	}

	m_wCurSeason = wSeason;
}

// �Ƿ��д˻
bool CKingMatchManager::IsHaveActivity(WORD wServerId)
{
	DWORD dwOpenDay = g_MapServer.GetAlreadyOpenServerDay(wServerId);

	// ��������ָ���������ſ���
	if (dwOpenDay >= g_GetConfigValue(225))
		return true;

	return false;
}

// �����������
bool CKingMatchManager::DoBattleOver(Player *pPlayer, BYTE byIsWin)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	// ȡ��λ����
	const KingMatchStage_Config *pStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
	if (!pStageCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������λ����[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
		return false;
	}

	WORD wOldStageLevel = tKingMatchData.wStageLevel;

	if (byIsWin)
	{
		// ��ʤ����
		if (!tKingMatchData.wTodayWinTimes)
		{
			// ��Ʒ����
			pPlayer->AddObjectList(pStageCfg->vDailyFirstWinReward, TRUE, FROM_KING_MATCH_BATTLE_REWARD);

			// ����,��������
			tKingMatchData.dwContext++;

			// ����Ϊ��ʤ
			pFBMgr->SetIsFirstWin(true);
		}

		// ����,��������
		tKingMatchData.dwContext++;

		// ���ܻ�ʤ���������ջ�ʤ����
		tKingMatchData.wTodayWinTimes++;
		tKingMatchData.wTotalWinTimes++;

		// ����������Ķ�λ,�����Ƿ������
		if (!pStageCfg->byIsLastStage && tKingMatchData.dwContext >= pStageCfg->wLevelUpNeedStar)
		{
			// ����
			tKingMatchData.wStageLevel++;

			// ��������
			tKingMatchData.dwContext = tKingMatchData.dwContext - pStageCfg->wLevelUpNeedStar;

			// ��������
			if (tKingMatchData.wStageLevel > tKingMatchData.wHistoryStageLevel)
			{
				pPlayer->AddObjectList(pStageCfg->vLevelUpReward, TRUE, FROM_KING_MATCH_BATTLE_REWARD);
				tKingMatchData.wHistoryStageLevel = tKingMatchData.wStageLevel;

				// �㲥:{1}����������������նɱ����{1}����λ����Ϊ{2}��
				if (pStageCfg->byIsBroadcastLeveUp)
				{
					const PlayerBaseData *pTargetData = g_GetPlayerBaseData(pFBMgr->GetTargetServerId(), pFBMgr->GetTargetId(), false);
					if (pTargetData)
					{
						CSystemNotice sys_notice(19, pPlayer->GetServerId());
						sys_notice.AddParam(pPlayer->GetRoleName());
						sys_notice.AddParam(pTargetData->szRoleName);
						sys_notice.AddParam(tKingMatchData.wStageLevel);
						sys_notice.SendMsg();
					}
				}
			}
		}
	}
	else
	{
		// �������
		if (pStageCfg->byIsDropStage)
		{
			// ������,����
			if (tKingMatchData.dwContext)
			{
				tKingMatchData.dwContext--;
			}
			// ����
			else
			{
				tKingMatchData.wStageLevel--;
				const KingMatchStage_Config *pPreStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
				if (!pPreStageCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������λ����[StageLevel:%d]!", __FUNCTION__, tKingMatchData.wStageLevel);
					return false;
				}
				tKingMatchData.dwContext = pPreStageCfg->wLevelUpNeedStar - 1;
			}
		}
		// ���ή��
		else
		{
			// ������,����
			if (pStageCfg->byIsDropStar && tKingMatchData.dwContext)
				tKingMatchData.dwContext--;
		}
	}

	// �����ܵ���ս����
	tKingMatchData.wTotalBattleTimes++;

	// ��������һ��λ,��������
	const KingMatchStage_Config *pCurStageCfg = g_pCfgMgr->GetKingMatchStage(tKingMatchData.wStageLevel);
	if (pCurStageCfg && pCurStageCfg->byIsLastStage)
	{
		int nWinRate = 0;
		if (tKingMatchData.wTotalBattleTimes && tKingMatchData.wTotalWinTimes)
			nWinRate = ceil((double)tKingMatchData.wTotalWinTimes * 100 / tKingMatchData.wTotalBattleTimes);
		CRankManager::Instance()->AddRoleRankData(RANK_KINGMATCH, pPlayer, tKingMatchData.dwContext, nWinRate, tKingMatchData.wTotalBattleTimes, 200);

		WORD wRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), ALL_SERVER_ID, RANK_KINGMATCH);

		pFBMgr->SetParam(0, wRank);
	}


	// �����λ�б仯
	if (wOldStageLevel != tKingMatchData.wStageLevel)
	{
		RemoveStageData(wOldStageLevel, pPlayer->GetRoleId());
		AddStageData(pPlayer->GetServerId(), pPlayer->GetRoleId(), tKingMatchData.wStageLevel,
			tKingMatchData.dwContext, tKingMatchData.wTotalBattleTimes);
	}
	else
	{
		// ������ս����,����
		KingMatchBattleTarget *pStageData = GetStageData(tKingMatchData.wStageLevel, pPlayer->GetRoleId());
		if (pStageData)
		{
			pStageData->wBattleTimes++;

			// ����������һ����λ,����Ҫ��������
			if (pCurStageCfg && pCurStageCfg->byIsLastStage)
				pStageData->wStar = tKingMatchData.dwContext;
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ҷ�λ����[StageLevel:%d,RoleId:%d]!",
				__FUNCTION__, tKingMatchData.wStageLevel, pPlayer->GetRoleId());
		}
	}

	// ���ջ(�ۼ���ս����������)
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 18, 1, true);

	// ���ջ�ԾĿ��(�ۼ���ս����������)
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 18, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 18, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 18, 1, true);

	return true;
}

// ���������������
void CKingMatchManager::SendKingMatchData(Player *pPlayer)
{
	PlayerKingMatchData &tKingMatchData = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData;

	MSG_S2C_KING_MATCH_DATA_RESP msg;
	msg.m_wCurSeason = m_wCurSeason;
	msg.m_KingMatchData = tKingMatchData;
	msg.m_wMyRank = 0;
	msg.m_dwMyRankValue = 0;
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(ALL_SERVER_ID, RANK_KINGMATCH, pPlayer->GetRoleId());
	if (pRankData)
	{
		msg.m_wMyRank = pRankData->wRank;
		msg.m_dwMyRankValue = pRankData->dwValue;
	}

	pPlayer->SendPacket(&msg);
}

// �Ƿ���
bool CKingMatchManager::IsActivityIng()
{
	if (!IsActivityStart())
		return false;

	// 223 ��ʼʱ��
	DWORD dwStartTime = g_GetConfigValue(223);

	// 224 ����ʱ��
	DWORD dwOverTime = g_GetConfigValue(224);

	tm tmDetailTime;
	GetTMTime(&tmDetailTime, time(NULL));

	DWORD dwCurTime = tmDetailTime.tm_hour * 100 + tmDetailTime.tm_min;

	if (dwCurTime >= dwStartTime && dwCurTime <= dwOverTime)
		return true;

	return false;
}

// �Ƿ���ʼ
bool CKingMatchManager::IsActivityStart()
{
	const KingMatchSeasonTime_Config *pSeasonTimeCfg = g_pCfgMgr->GetKingMatchSeasonTime(m_wCurSeason);
	if (!pSeasonTimeCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������ʱ������[season:%d]", __FUNCTION__, m_wCurSeason);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// ������ָ���ķ�Χ��
	if (nCurData > pSeasonTimeCfg->dwStartDate && nCurData < pSeasonTimeCfg->dwOverDate)
		return true;

	// ��ʼ�ĵ���
	if (nCurData == pSeasonTimeCfg->dwStartDate && nCurTime >= pSeasonTimeCfg->dwStartTime)
		return true;

	// ���һ��
	if (nCurData == pSeasonTimeCfg->dwOverDate && nCurTime <= pSeasonTimeCfg->dwOverTime)
		return true;

	return false;
}


// �Ƿ�����
bool CKingMatchManager::IsActivityOver()
{
	const KingMatchSeasonTime_Config *pSeasonTimeCfg = g_pCfgMgr->GetKingMatchSeasonTime(m_wCurSeason);
	if (!pSeasonTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������ʱ������[season:%d]", __FUNCTION__, m_wCurSeason);
		return false;
	}

	tm tmDetails;
	GetTMTime(&tmDetails, time(NULL));

	int nCurData = g_Time2Date(time(NULL));
	int nCurTime = tmDetails.tm_hour * 100 + tmDetails.tm_min;

	// ������������һ����
	if (nCurData > pSeasonTimeCfg->dwOverDate)
		return true;

	// ���һ��
	if (nCurData == pSeasonTimeCfg->dwOverDate && nCurTime > pSeasonTimeCfg->dwOverTime)
		return true;

	return false;
}

// �Ƿ����Ž���
bool CKingMatchManager::IsActivityReward()
{
	// �Ӻ�3���ӷ���
	if (time(NULL) >= m_dwActivityOverTime + 180)
		return true;

	return false;
}

// �Ƿ�����ս�б���
bool CKingMatchManager::IsInStageList(Player *pPlayer)
{
	WORD wStageLevel = pPlayer->GetRoleFullInfo().tSecondInfo.tKingMatchData.wStageLevel;
	DWORD dwRoleId = pPlayer->GetRoleId();

	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.find(wStageLevel);
	// �ҵ���λ����
	if (iter != m_StageDataList.end())
	{
		// �ҵ��������
		if (iter->second.find(dwRoleId) != iter->second.end())
			return true;
	}
	return false;
}

// �����սĿ������
void CKingMatchManager::AddStageData(WORD wServerId, DWORD dwRoleId, WORD wStageLevel, WORD wStar, WORD wBattleTimes)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.find(wStageLevel);
	// �ҵ���λ����
	if (iter == m_StageDataList.end())
	{
		map<DWORD, KingMatchBattleTarget> list;
		m_StageDataList.insert(make_pair(wStageLevel, list));
		iter = m_StageDataList.find(wStageLevel);
	}

	KingMatchBattleTarget tTarget;
	tTarget.wServerId = wServerId;
	tTarget.dwRoleId = dwRoleId;
	tTarget.wStageLevel = wStageLevel;
	tTarget.wStar = wStar;
	tTarget.wBattleTimes = wBattleTimes;
	iter->second.insert(make_pair(dwRoleId, tTarget));

	//AddPlayerStageLevelData(dwRoleId, wStageLevel);
}

// ɾ����λ����
void CKingMatchManager::RemoveStageData(WORD wStageLevel, DWORD dwRoleId)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.find(wStageLevel);
	// �ҵ���λ����
	if (stage_iter != m_StageDataList.end())
	{
		stage_iter->second.erase(dwRoleId);
	}
}

// ȡ��λ����
KingMatchBattleTarget* CKingMatchManager::GetStageData(WORD wStageLevel, DWORD dwRoleId)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.find(wStageLevel);
	// �ҵ���λ����
	if (stage_iter != m_StageDataList.end())
	{
		map<DWORD, KingMatchBattleTarget>::iterator target_iter = stage_iter->second.find(dwRoleId);
		if (target_iter != stage_iter->second.end())
			return &target_iter->second;
	}
	return NULL;
}

// ȡ��λ����
KingMatchBattleTarget* CKingMatchManager::GetStageData(DWORD dwRoleId)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator stage_iter = m_StageDataList.begin();
	for (; stage_iter != m_StageDataList.end(); ++stage_iter)
	{
		map<DWORD, KingMatchBattleTarget>::iterator battle_target_iter = stage_iter->second.find(dwRoleId);
		if (battle_target_iter != stage_iter->second.end())
			return &battle_target_iter->second;
	}
	return NULL;
}

// ȡ��λ�ȼ��б�
const map<DWORD, KingMatchBattleTarget>* CKingMatchManager::GetStageLevelList(WORD wStageLevel)
{
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.find(wStageLevel);
	if (iter == m_StageDataList.end())
	{
		m_StageDataList.insert(make_pair(wStageLevel, map<DWORD, KingMatchBattleTarget>()));
		iter = m_StageDataList.find(wStageLevel);
	}
	return iter != m_StageDataList.end() ? &iter->second : NULL;
}

// ȡ��Ҷ�λ�ȼ�
WORD CKingMatchManager::GetPlayerStageLevel(DWORD dwRoleId)
{
	KingMatchBattleTarget *pBattleTarget = GetStageData(dwRoleId);
	return pBattleTarget ? pBattleTarget->wStageLevel : 0;
}

// ������Ҷ�λ�ȼ�
// void CKingMatchManager::UpdatePlayerStageLevel(DWORD dwRoleId, WORD wStageLevel)
// {
// 	map<DWORD, WORD>::iterator iter = m_PlayerStageLevelList.find(dwRoleId);
// 	if (iter != m_PlayerStageLevelList.end())
// 	{
// 		iter->second = wStageLevel;
// 	}
// }

// �����Ҷ�λ�ȼ�����
// void CKingMatchManager::AddPlayerStageLevelData(DWORD dwRoleId, WORD wStageLevel)
// {
// 	m_PlayerStageLevelList.insert(make_pair(dwRoleId, wStageLevel));
// }

// ����������а�
void CKingMatchManager::AddSeasonRank(WORD wSeason, const vector<RoleKingMatchSeasonRank> &vRankList)
{
	m_LastSeasonRankList.insert(make_pair(wSeason, vRankList));

	// ֻȡָ���ļ�������
	if (m_LastSeasonRankList.size() > RECENTLY_KING_MATCH_SEASON_RANK_NUM)
	{
		// ������С������(Ҳ����õ�����)
		WORD wRemoveSeason = 0;
		LastSeasonRankList::iterator iter = m_LastSeasonRankList.begin();
		for (; iter != m_LastSeasonRankList.end(); ++iter)
		{
			if (!wRemoveSeason || iter->first < wRemoveSeason)
			{
				wRemoveSeason = iter->first;
			}
		}

		if (wRemoveSeason)
			m_LastSeasonRankList.erase(wRemoveSeason);
	}

	// todo::�洢�����ݿ�
}

// ȡ�ܵ���սĿ������
DWORD CKingMatchManager::GetTotalBattleTargetNum()
{
	DWORD dwTotalBattleTargetNum = 0;
	map<WORD, map<DWORD, KingMatchBattleTarget>>::iterator iter = m_StageDataList.begin();
	for (; iter != m_StageDataList.end(); ++iter)
	{
		dwTotalBattleTargetNum += iter->second.size();
	}
	return dwTotalBattleTargetNum;
}

// ����Ƿ��Ѿ�������ȡ��
bool CKingMatchManager::IsTakeSeasonTaskReward(const PlayerKingMatchData &tKingMatchData, WORD wWinTimes)
{
	for (WORD i = 0; i < tKingMatchData.wSeasonRewardRecordNum; i++)
	{
		if (tKingMatchData.arSeasonTaskRewardRecord[i] == wWinTimes)
			return true;
	}

	return false;
}