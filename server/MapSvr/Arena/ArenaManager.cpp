#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Hero\HeroManager.h"
#include "DBCDefine.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Singleton.h"
#include "ArenaManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\MapServer.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Email\EmailManager.h"
#include "..\MapSvrQuery.h"
#include "data_define.pb.h"
#include "..\SevenDayActivity\SevenDayManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Shop\ShopManager.h"
#include "..\Activity\ActivityManager.h"


CArenaManager::CArenaManager()
{
	m_lProccessTime = GetTickCount64();
	m_dwDoRewardTime = time(NULL);
	m_bRankUpdate = false;
	m_bAlreadyLoadData = false;
	m_lProceesSaveTime = GetTickCount64();
	m_RankRewardTimer.AddTriggerTime(g_GetConfigValue(193));
}

CArenaManager::~CArenaManager()
{

}

void CArenaManager::Init()
{
	m_lProccessTime = GetTickCount64();
	m_dwDoRewardTime = time(NULL);
	m_bRankUpdate = false;
	m_bAlreadyLoadData = false;
	m_lProceesSaveTime = GetTickCount64();
	m_RankListForSendReward.clear();

	// �������ݿ��������
	ArenaBattleRecord_Load* pQuery = ArenaBattleRecord_Load::ALLOC();
	pQuery->SetIndex(DB_ARENA_BATTLE_RECORD_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_ARENA_BATTLE_RECORD);
	g_MapServer.MapDBQuery(pQuery);
}

void CArenaManager::Update()
{
	// todo::������ʱ����
	//return;

	// todo::��ʱ��������...
	if (m_RankRewardTimer.IsTrigger())
	{
		DoRankAward();
		m_RankRewardTimer.SetCDOverTime();
	}

	// todo::���Խ׶�1���Ӵ���
	if (GetTickCount64() >= m_lProceesSaveTime + 60000)
	{
		ArenaAllRankMap::iterator iter = m_RankList.begin();
		for (; iter != m_RankList.end(); ++iter)
		{
			m_SaveDataServerList.push_back(iter->first);
		}
		m_lProceesSaveTime = GetTickCount64();
	}

	if (m_SaveDataServerList.size())
	{	 
		SaveDataToDB(m_SaveDataServerList[m_SaveDataServerList.size() - 1]);
		m_SaveDataServerList.pop_back();
	}

	if (m_RankListForSendReward.size())
	{
		DWORD dwLastTime = GetTickCount64();
		int nPlayerNum = 0;
		ArenaAllRankMap::iterator iter_list = m_RankListForSendReward.begin();
		ArenaRankMap::iterator iter_rank_data = iter_list->second.begin();
		for (; iter_rank_data != iter_list->second.end(); ++iter_rank_data)
		{
			// ����ǻ������򲻴���
			if (IsRobotId(iter_rank_data->dwTargetId))
				continue;

			RewardData tRewardData;
			tRewardData.wInfoId = 1;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = iter_rank_data->wCurRank;
			CRewardCenterManager::AddReward(iter_list->first, iter_rank_data->dwTargetId, tRewardData);

			nPlayerNum++;
		}
		
		DWORD dwCostTime = GetTickCount64() - dwLastTime;
		MAPLOG(ERROR_LEVEL, "[%s]���������Ž���[server_id:%d,list_num:%d,player_num:%d,cost_time:%d]", 
			__FUNCTION__, iter_list->first, iter_list->second.size(), nPlayerNum, dwCostTime);

		m_RankListForSendReward.erase(iter_list);
	}
}

// �洢����
void CArenaManager::SaveDataToDB(DWORD dwServerId)
{
	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������а��б�[ServerId:%d]", __FUNCTION__, dwServerId);
		return;
	}

	PB::ArenaRankDataList PBRankDataList;
	for (size_t i = 0; i < pRankList->size(); ++i)
	{
		const ArenaRankData &tRankData = pRankList->at(i);
		PB::ArenaRankData *pPBRankData = PBRankDataList.add_rank_list();
		pPBRankData->set_role_id(tRankData.dwTargetId);
		pPBRankData->set_cur_rank(tRankData.wCurRank);
		pPBRankData->set_history_rank(tRankData.wHistoryBestRank);
	}

	string strSaveData("");
	PBRankDataList.AppendToString(&strSaveData);
	if (strSaveData.size() > MAX_ARENA_LIST_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������洢���ݹ���[ListSize:%d, DataSize:%d]", 
			__FUNCTION__, pRankList->size(), strSaveData.size());
		return;
	}

	ArenaDataList_Save* pQuery = ArenaDataList_Save::ALLOC();
	pQuery->SetIndex(DB_ARENA_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_ARENA);
	pQuery->SetServerId(dwServerId);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);

	MAPLOG(GUI_LEVEL, "[%s]���������ݴ洢����[ListSize:%d, DataSize:%d]", 
		__FUNCTION__, pRankList->size(), strSaveData.size());
}

// �洢��������
void CArenaManager::SaveAllData()
{
	ArenaAllRankMap::iterator iter = m_RankList.begin();
	for (; iter != m_RankList.end(); ++iter)
	{
		SaveDataToDB(iter->first);
	}
}

// ��������
void CArenaManager::LoadDataFromDB(DWORD dwServerId, const char *pData, int size)
{
	ArenaRankMap vRankList;
	if (size)
	{
		PB::ArenaRankDataList PBRankDataList;
		if (!PBRankDataList.ParseFromArray(pData, size))
		{
			MAPLOG(ERROR_LEVEL, "�������������ݳ���(%d)", size);
			return;
		}

		// ���������
		m_RankList.erase(dwServerId);

		ArenaRankData tRankData;
		for (int i = 0; i < PBRankDataList.rank_list_size(); ++i)
		{
			const PB::ArenaRankData &tPBRankData = PBRankDataList.rank_list(i);
			tRankData.dwTargetId = tPBRankData.role_id();
			tRankData.wCurRank = tPBRankData.cur_rank();
			tRankData.wHistoryBestRank = tPBRankData.history_rank();
			vRankList.push_back(tRankData);
		}

		m_RankList.insert(make_pair(dwServerId, vRankList));
	}

	MAPLOG(GUI_LEVEL, "�ɹ����ؾ���������[data_size:%d,server_id:%d,list_num:%d]!", size, dwServerId, vRankList.size());
}

// ������������
void CArenaManager::LoadDataRequest()
{
	if (m_bAlreadyLoadData)
		return;

	const map<DWORD, ServerInfo> &mapServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = mapServerList.begin();
	for (; iter != mapServerList.end(); ++iter)
	{
		ArenaDataList_Load* pQuery = ArenaDataList_Load::ALLOC();
		pQuery->SetIndex(DB_ARENA_DATA_LOAD);
		pQuery->SetThreadIndex(DB_THREAD_ARENA);
		pQuery->SetServerId(iter->second.dwServerId);
		g_MapServer.MapDBQuery(pQuery);
	}

	m_bAlreadyLoadData = true;
}

// �洢ս����¼����
void CArenaManager::SaveBattleRecord(DWORD dwRoleId)
{
	ArenaBattleRecordMap::iterator map_iter = m_vBattleRecordList.find(dwRoleId);
	if (map_iter != m_vBattleRecordList.end())
	{
		ArenaBattleRecord_Save* pQuery = ArenaBattleRecord_Save::ALLOC();
		pQuery->SetIndex(DB_ARENA_BATTLE_RECORD_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_ARENA_BATTLE_RECORD);
		pQuery->SetRoleId(dwRoleId);

		list<ArenaBattleRecord>::iterator list_iter = map_iter->second.begin();
		for (; list_iter != map_iter->second.end(); ++list_iter)
		{
			pQuery->AddBattleRecord(&*list_iter);
		}

		g_MapServer.MapDBQuery(pQuery);
	}
}

// ������սĿ���б� 
void CArenaManager::OnNetTargetList(Player *pPlayer)
{
	SendTargetList(pPlayer);
}

// ����ս��
void CArenaManager::OnNetBattleRecordList(Player *pPlayer)
{
	MSG_S2C_ARENA_BATTLE_RECORD_RESP msg;
	msg.m_byRecordNum = 0;

	ArenaBattleRecordMap::iterator map_iter = m_vBattleRecordList.find(pPlayer->GetCharID());
	if (map_iter != m_vBattleRecordList.end())
	{
		list<ArenaBattleRecord>::iterator list_iter = map_iter->second.begin();
		for (; list_iter != map_iter->second.end(); ++list_iter)
		{
			if (msg.m_byRecordNum >= MAX_ARENA_BATTLE_RECORD)
				break;
			msg.m_RecordList[msg.m_byRecordNum++] = *list_iter;
		}
	}

	pPlayer->SendPacket(&msg);
}

// ս����ʼ
bool CArenaManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId, DWORD dwIsSweep)
{
	// ��⾫��
	if (pPlayer->GetProperty(ROLE_PRO_ENERGY) < 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������,��������սʧ��!", __FUNCTION__);
		return false;
	}


// 	// ����ǵ�һ����ս���������һ��
// 	if (!GetPlayerRankData(pPlayer->GetServerId(),pPlayer->GetCharID()))
// 	{
// 		ArenaRankMap *pRankList = GetServerRankList(pPlayer->GetServerId());
// 		if (!pRankList)
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������а��б�[ServerId:%d]", __FUNCTION__, pPlayer->GetServerId());
// 			return false;
// 		}
// 		ArenaRankData tRankData;
// 		tRankData.dwTargetId = pPlayer->GetRoleId();
// 		tRankData.wCurRank = pRankList->size()+1;
// 		tRankData.wHistoryBestRank = 0;
// 		pRankList->push_back(tRankData);
// 	}

	// ���Ŀ��
	vector<ArenaRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetServerId(), pPlayer->GetCharID(), &vTargetList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ��������սĿ���б�ʧ��", __FUNCTION__);
		return false;
	}

	bool bFound = false;
	for (int i = 0; i < vTargetList.size(); ++i)
	{
		if (vTargetList[i].dwTargetId == dwTargetId)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		SendTargetList(pPlayer);
		MAPLOG(ERROR_LEVEL, "[%s]��ս��Ŀ�겻�Ϸ�", __FUNCTION__);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);
	//pFBMgr->SetParam(0, dwTargetId);
	pFBMgr->SetBattleInfo(0, 0, 0);
	pFBMgr->SetTargetId(dwTargetId);
	pFBMgr->SetParam(1, dwIsSweep);

	// ��������,����Ҫ��Ӣ�۷���ȥ
	if (!IsRobotId(dwTargetId))
	{
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId, true);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// ����ս��
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}

	pPlayer->ModifyProperty(ROLE_PRO_ENERGY, -2, TRUE, FROM_ARENA_BATTLE);

	return true;
}

// ս������
bool CArenaManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	// ����ǵ�һ����ս���������һ��
	if (!GetPlayerRankData(pPlayer->GetServerId(), pPlayer->GetCharID()))
	{
		// ����ǵ�һ����ս����
		if (!byIsWin)
			return true;

		ArenaRankMap *pRankList = GetServerRankList(pPlayer->GetServerId());
		if (!pRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������а��б�[ServerId:%d]", __FUNCTION__, pPlayer->GetServerId());
			return false;
		}
		ArenaRankData tRankData;
		tRankData.dwTargetId = pPlayer->GetRoleId();
		tRankData.wCurRank = pRankList->size() + 1;
		tRankData.wHistoryBestRank = 0;
		pRankList->push_back(tRankData);
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	DWORD dwTargetId = pFBMgr->GetTargetId();
	BYTE byIsSweep = pFBMgr->GetParam(1);

	ArenaRankData *pSrc = GetPlayerRankData(pPlayer->GetServerId(), pPlayer->GetRoleId());
	if (!pSrc)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս����ʱ,�Ҳ�����ս������", __FUNCTION__);
		return false;
	}
	ArenaRankData *pTarget = GetPlayerRankData(pPlayer->GetServerId(), dwTargetId);
	if (!pTarget)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս����ʱ,�Ҳ���Ŀ������", __FUNCTION__);
		return false;
	}

	const ArenaAward_Config* pAwardCfg = CConfigManager::Instance()->GetArenaAward(pPlayer->GetLevel());
	if (!pAwardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������������������[RoleLevel:%d]", __FUNCTION__, pPlayer->GetLevel());
		return false;
	}

	// �����ս��¼
	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId);
	if (pTargetBaseData)
	{
		AddBattleRecord(pPlayer->GetRoleId(), dwTargetId, pSrc->wCurRank, pTarget->wCurRank, pPlayer->GetRoleName(),
			pTargetBaseData->szRoleName, pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE), pTargetBaseData->wHeadIcon,
			pPlayer->GetLevel(), pTargetBaseData->wLevel, byIsWin);
	}
	else
	{
		AddBattleRecord(pPlayer->GetRoleId(), dwTargetId, pSrc->wCurRank, pTarget->wCurRank, pPlayer->GetRoleName(),
			"", pPlayer->GetProperty(ROLE_PRO_HEAD_IMAGE), 0, pPlayer->GetLevel(), 0, byIsWin);
	}

	int nMultiple = CActivityManager::GetLimitProductMultiple(BATTLE_ARENA_MAP);

	if (byIsWin)
	{
		// ���Ŀ����������Ҹ�
		if (pSrc->wCurRank > pTarget->wCurRank)
		{
			// ����㲥(ֻ�㲥����ǰ3����)
			if (pTarget->wCurRank <= 3)
			{
				CSystemNotice sys_notice(4, pPlayer->GetServerId());
				sys_notice.AddParam(pPlayer->GetRoleName());
				const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), dwTargetId, false);
				if (pTargetBaseData)
					sys_notice.AddParam(pTargetBaseData->szRoleName);
				else
				{
					const ArenaRobot_Config *pRobotCfg = g_pCfgMgr->GetArenaRobot(dwTargetId);
					sys_notice.AddParam(pRobotCfg ? pRobotCfg->strRobotName.c_str() : "Robot");
				}
						
				sys_notice.AddParam(pTarget->wCurRank);
				sys_notice.SendMsg();
			}


			// �״����н���
			if (!pSrc->wHistoryBestRank || pTarget->wCurRank < pSrc->wHistoryBestRank)
			{
				int nTotalDiamond = 0;
				WORD wMax = pSrc->wHistoryBestRank ? pSrc->wHistoryBestRank : CConfigManager::Instance()->m_ArenaFirstRankRewardList.size() + 1;
				for (WORD i = pTarget->wCurRank; i < wMax; ++i)
				{
					nTotalDiamond += CConfigManager::Instance()->GetArenaFirstRankReward(i);
				}
				
				if (nTotalDiamond)
					pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, nTotalDiamond, TRUE, FROM_ARENA_BATTLE);

				MAPLOG(GUI_LEVEL, "���ξ�������ս�����ʯ����[%d][TargetRank:%d, SrcRank:%d]", 
					nTotalDiamond, pTarget->wCurRank, pSrc->wCurRank);
			}

			// ����λ��
			swap(*pSrc, *pTarget);	// �������ݽ���
			swap(pSrc->wCurRank, pTarget->wCurRank);	// ��������
			swap(pSrc, pTarget);	// ������ַ

			// �������������¼
			if (pSrc->wHistoryBestRank)
				pSrc->wHistoryBestRank = min(pSrc->wHistoryBestRank, pSrc->wCurRank);
			else
				pSrc->wHistoryBestRank = pSrc->wCurRank;

			//SendTargetList(pPlayer);

			// ����������ʧ��(��������)
			if (!byIsSweep && !IsRobotId(dwTargetId))
			{
				// ���ʼ�֪ͨ
				EmailInfo tEailInfo(2);
				tEailInfo.AddContext(EMAIL_CONTEXT_STRING, pPlayer->GetRoleName());
				tEailInfo.AddContext(EMAIL_CONTEXT_INT, pTarget->wCurRank);
				EmailManager::AddEmail(dwTargetId, tEailInfo, true);
			}

			m_bRankUpdate = true;

			// 7���¼�(����������)
			pPlayer->OnUpdateSevenDayTarget(6, pSrc->wHistoryBestRank, false);
		}
		else
		{
			// ����������ʧ��(��������)
			if (!byIsSweep && !IsRobotId(dwTargetId))
			{
				// ���ʼ�֪ͨ
				EmailInfo tEailInfo(4);
				tEailInfo.AddContext(EMAIL_CONTEXT_STRING, pPlayer->GetRoleName());
				EmailManager::AddEmail(dwTargetId, tEailInfo, true);
			}
		}

		// ��ӿɷ���
		vector<Item> vDropRewardItemList;
		pPlayer->GetFubenManager()->DoLimitDrop(9999, vDropRewardItemList);
		if (vDropRewardItemList.size())
		{
			// ���������ж��,������ֶ��,��ֻȡ��һ��
			pPlayer->GetItemManager()->SetFPItem(vDropRewardItemList[0]);
		}
		else
		{
			// ���û����ʱ�����,��ȡ������
			pPlayer->GetItemManager()->SetFPItem(Item(pAwardCfg->wFPItemId,1));
		}

		pPlayer->ModifyProperty(ROLE_PRO_GOLD, pAwardCfg->nWinGold * nMultiple, TRUE, FROM_ARENA_BATTLE);
		pPlayer->ModifyProperty(ROLE_PRO_ARENA_PRESTIGE, pAwardCfg->nWinArenaPrestige * nMultiple, TRUE, FROM_ARENA_BATTLE);
	}
	else
	{
		pPlayer->ModifyProperty(ROLE_PRO_GOLD, pAwardCfg->nFailGold * nMultiple, TRUE, FROM_ARENA_BATTLE);
		pPlayer->ModifyProperty(ROLE_PRO_ARENA_PRESTIGE, pAwardCfg->nFailArenaPrestige * nMultiple, TRUE, FROM_ARENA_BATTLE);

		// ���ʼ�֪ͨĿ��(���سɹ�)
		if (!byIsSweep && !IsRobotId(dwTargetId))
		{
			// ���ʼ�֪ͨ
			EmailInfo tEailInfo(1);
			tEailInfo.AddContext(EMAIL_CONTEXT_STRING, pPlayer->GetRoleName());
			EmailManager::AddEmail(dwTargetId, tEailInfo, true);
		}
	}

	// ���齱��
	pPlayer->AddExp(pAwardCfg->dwExp);

	// �ճ�����
	pPlayer->OnUpdateDailyMission(10);

	// 8-14���¼�(��ս������������ʤ����)
	pPlayer->OnUpdateSevenDayTarget(23, 1, true);

	// ���ջ
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 8, 1, true);

	// ���ջ�ԾĿ��
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 8, 1, true);	

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 8, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 8, 1, true);

	

	return true;
}

// ȡ��ҵ�����
WORD CArenaManager::GetPlayerRank(DWORD dwServerId, DWORD dwRoleId)
{
	ArenaRankData *pRankData = GetPlayerRankData(dwServerId, dwRoleId);
	return pRankData ? pRankData->wCurRank : 0;
}

// ȡ��ҵ���ʷ����
WORD CArenaManager::GetPlayerHistoryRank(DWORD dwServerId, DWORD dwRoleId)
{
	ArenaRankData *pRankData = GetPlayerRankData(dwServerId, dwRoleId);
	return pRankData ? pRankData->wHistoryBestRank : 0;
}

// ȡ��սĿ���б�
bool CArenaManager::GetTargetList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutTargetList)
{
	pOutTargetList->clear();

	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������а��б�[ServerId:%d]", __FUNCTION__, dwServerId);
		return false;
	}

	// ������а����� todo::�պ����
	if (pRankList->size() < 5000)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������а���������ָ������[ServerId:%d,size:%d]", 
			__FUNCTION__, dwServerId, pRankList->size());
		return false;
	}

	WORD wShowRank = 10;

	// ȡǰ10��
	for (ArenaRankMap::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		if (pOutTargetList->size() >= wShowRank)
			break;
		pOutTargetList->push_back(*iter);
	}

	WORD wRank = GetPlayerRank(dwServerId, dwRoleId);

	bool bFirstTimes = false;

	// �����û����������һ��
	if (!wRank)
	{
		wRank = pRankList->size() + 1;
		bFirstTimes = true;
	}

	const ArenaRank_Config *pRankCfg = g_pCfgMgr->GetArenaRank(wRank);
 	if (!pRankCfg)
 	{
		// ����Ҳ�����ʹ����һ������������
		pRankCfg = g_pCfgMgr->GetArenaRank(g_pCfgMgr->GetAreanRankNum());
		if (!pRankCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������������[Rank:%d]", __FUNCTION__, g_pCfgMgr->GetAreanRankNum());
			return false;
		}
 	}

	// ��ֹΪ0�����
	WORD wTargetRankInterval = bFirstTimes ? 1 : max(pRankCfg->wTargetRankInterval, 1);

	// ������10��֮��
	vector<ArenaRankData> vTmpRankList;
	if (wRank > wShowRank)
	{
		// ����Ŀ��(ֻȡ9��)
		int nTargetRank = wRank - wTargetRankInterval * 9;
		nTargetRank = max(nTargetRank, 0);
		while (wTargetRankInterval && nTargetRank < wRank)
		{
			//pOutTargetList->push_back((*pRankList)[nTargetRank-1]);
			vTmpRankList.push_back((*pRankList)[nTargetRank - 1]);
			nTargetRank += wTargetRankInterval;
		}

		for (size_t i = 0; i < vTmpRankList.size(); ++i)
		{
			bool bHave = false;
			for (size_t k = 0; k < pOutTargetList->size(); ++k)
			{
				if (pOutTargetList->at(k).dwTargetId == vTmpRankList[i].dwTargetId)
				{
					bHave = true;
					break;
				}
			}
			if (!bHave)
			{
				pOutTargetList->push_back(vTmpRankList[i]);
			}
		}

		// ����Ŀ��(ֻȡ����)
		nTargetRank = wRank;
		for (int i = 0; i < 2/*todo::��ʱ��ô����*/; ++i)
		{
			nTargetRank += wTargetRankInterval;
			if (pRankList->size() > nTargetRank)
				pOutTargetList->push_back((*pRankList)[nTargetRank-1]);
		}
	}
	// ������10��֮��
	else
	{
		pOutTargetList->push_back((*pRankList)[wShowRank]);
		pOutTargetList->push_back((*pRankList)[wShowRank+1]);
	}	

	return true;
}

// ����Ŀ���б�
void CArenaManager::SendTargetList(Player *pPlayer)
{

	vector<ArenaRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetServerId(), pPlayer->GetCharID(), &vTargetList))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ARENA_TARGET_LIST_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ��������սĿ���б�ʧ��", __FUNCTION__);
		return;
	}

	MSG_S2C_ARENA_TARGET_LIST_RESP msg;
	msg.m_wRank = 0; // GetPlayerRank(pPlayer->GetServerId(), pPlayer->GetCharID());
	msg.m_wHistoryRank = 0;
	msg.m_byTargetNum = 0;
	ArenaRankData *pRankData = GetPlayerRankData(pPlayer->GetServerId(), pPlayer->GetCharID());
	if (pRankData)
	{
		msg.m_wRank = pRankData->wCurRank;
		msg.m_wHistoryRank = pRankData->wHistoryBestRank;
	}
	for (vector<ArenaRankData>::iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter)
	{
		if (msg.m_byTargetNum >= MAX_ARENA_TARGET_NUM)
			break;

		ArenaTargetData &tTargetData = msg.m_TargetList[msg.m_byTargetNum++];
		memset(&tTargetData, 0, sizeof(tTargetData));
		tTargetData.dwTargetId = iter->dwTargetId;
		tTargetData.wRank = iter->wCurRank;
		if (!IsRobotId(iter->dwTargetId))
		{
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), iter->dwTargetId);
			if (pBaseData)
			{
				strcpy_s(tTargetData.szTargetName, sizeof(tTargetData.szTargetName) - 1, pBaseData->szRoleName);
				tTargetData.wHeadIcon = pBaseData->wHeadIcon;
				tTargetData.dwFightingPower = pBaseData->dwFightingPower;
				tTargetData.dwTitleId = pBaseData->dwTitleId;
				tTargetData.dwNicknameId = pBaseData->dwNicknameId;
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// ȡ���а��б�
void CArenaManager::GetRankList(DWORD dwServerId, DWORD dwRoleId, vector<ArenaRankData> *pOutRankList, WORD wMaxNum)
{
	pOutRankList->clear();
	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������а��б�[ServerId:%d]", __FUNCTION__, dwServerId);
		return;
	}

	for (vector<ArenaRankData>::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		if (pOutRankList->size() >= wMaxNum)
			break;
		pOutRankList->push_back(*iter);
	}
}

// �������н���
void CArenaManager::DoRankAward()
{	// todo::������������Ƚ϶�Ļ�������߼���Ƚ���ѹ������Ҫ����
	
	//WORD wRankArenaNum = g_pCfgMgr->GetAreanRankNum();
// 	ArenaAllRankMap::iterator map_iter = m_RankList.begin();
// 	for (; map_iter != m_RankList.end(); ++map_iter)
// 	{
// 		ArenaRankMap::iterator vector_iter = map_iter->second.begin();
// 		for (; vector_iter != map_iter->second.end(); ++vector_iter)
// 		{
// 			// ������,ǰx�������н���
// 			//if (vector_iter->wCurRank <= wRankArenaNum)
// 			//{
// 			RewardData tRewardData;
// 			tRewardData.wInfoId = 1;
// 			tRewardData.dwTime = time(NULL);
// 			tRewardData.dwValue1 = vector_iter->wCurRank;
// 			CRewardCenterManager::AddReward(map_iter->first, vector_iter->dwTargetId, tRewardData);
// 
// 			//MAPLOG(ERROR_LEVEL, "[%s]���������Ž���[role_id:%d,Rank:%d]", __FUNCTION__, vector_iter->dwTargetId, vector_iter->wCurRank);
// 			//}
// 		}
// 
// 		MAPLOG(ERROR_LEVEL, "[%s]���������Ž���[server_id:%d]", __FUNCTION__, map_iter->first);
// 	}

	m_RankListForSendReward = m_RankList;

	MAPLOG(ERROR_LEVEL, "[%s]���������а��ѷ��Ž���[server_num:%d]", __FUNCTION__, m_RankListForSendReward.size());
}

// ����7�����н���
void CArenaManager::DoSevenDayRankAward(DWORD dwServerId)
{
	ArenaRankMap *pRankList = GetServerRankList(dwServerId);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������а��б�[ServerId:%d]", __FUNCTION__, dwServerId);
		return;
	}

	MAPLOG(ERROR_LEVEL, "[%s]��ʼ����7�����а���(��������)[serverid:%d]", __FUNCTION__, dwServerId);
	const RoleRankReward_Config *pRewardCfg;
	ArenaRankMap::iterator iter = pRankList->begin();
	for (; iter != pRankList->end(); ++iter)
	{
		// ����ǻ������򲻴���
		if (IsRobotId(iter->dwTargetId))
			continue;

		pRewardCfg = g_pCfgMgr->GetSevenDayRankReward(RANK_ARENA, iter->wCurRank);
		if (pRewardCfg)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 18;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = iter->wCurRank;
			tRewardData.dwValue2 = RANK_ARENA; 
			CRewardCenterManager::AddReward(dwServerId, iter->dwTargetId, tRewardData);

			MAPLOG(ERROR_LEVEL, "����7��ս������(server_id:%d,role_id:%d,rank:%d)",
				dwServerId, iter->dwTargetId, iter->wCurRank);
		}
	}
}

// ������ս����¼
void CArenaManager::AddPlayerBattleRecord(const PlayerArenaBattleRecord *pBattleRecord)
{
	ArenaBattleRecordMap::iterator map_iter = m_vBattleRecordList.find(pBattleRecord->dwRoleId);
	if (map_iter != m_vBattleRecordList.end())
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ظ������Ҿ�����ս����¼[roleid:%d]", __FUNCTION__,pBattleRecord->dwRoleId);
		return;
	}

	list<ArenaBattleRecord> lBattleRecordList;
	for (WORD i = 0; i < pBattleRecord->wBattleRecordNum; ++i)
	{
		lBattleRecordList.push_back(pBattleRecord->arBattleRecordList[i]);
	}
	m_vBattleRecordList.insert(make_pair(pBattleRecord->dwRoleId, lBattleRecordList));
}

// �Ƿ��к��
bool CArenaManager::IsHaveRedPoint(Player *pPlayer)
{
	WORD wRank = GetPlayerRank(pPlayer->GetServerId(), pPlayer->GetRoleId());
// 	if (!wRank)
// 		return false;

	const vector<DWORD> &vGoodsList = g_pCfgMgr->GetArenaShopGoodsList();
	for (size_t i = 0; i < vGoodsList.size(); ++i)
	{
		const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_ARENA, vGoodsList[i]);
		if (!pGoodsCfg)
			continue;

		// ����ָ������(0��ʾľ������Ҫ��)
		if (!pGoodsCfg->nContext || (wRank && wRank <= pGoodsCfg->nContext))
		{
			// �����Թ���(�����������ĵ�����)
			if (!pPlayer->GetShopManager()->GetGoodsBuyRecordNum(SHOP_ARENA, vGoodsList[i]) && 
				pPlayer->CheckMoneyCost(pGoodsCfg->vCostList))
				return true;
		}
	}

	return false;
}

// �������а�����
void CArenaManager::ResetRankData(WORD wServerId)
{
	ArenaAllRankMap::iterator rank_list_iter = m_RankList.find(wServerId);
	if (rank_list_iter != m_RankList.end())
	{
		// ��ս������
		ArenaRankMap::iterator rank_data_iter = rank_list_iter->second.begin();
		for (; rank_data_iter != rank_list_iter->second.end(); ++rank_data_iter)
		{
			m_vBattleRecordList.erase(rank_data_iter->dwTargetId);
		}

		// �����а�����
		rank_list_iter->second.clear();
	}
}

// ȡ���а��б�
ArenaRankMap* CArenaManager::GetServerRankList(DWORD dwServerId)
{
	ArenaAllRankMap::iterator iter = m_RankList.find(dwServerId);
	if (iter == m_RankList.end() || !iter->second.size())
	{
		ArenaRankMap vRankList;
		const vector<ArenaRobot_Config> *pRobotListCfg = CConfigManager::Instance()->GetArenaRobotList();
		if (!pRobotListCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������������������б�", __FUNCTION__);
			return NULL;
		}

		ArenaRankData tRankData;
		vector<ArenaRobot_Config>::const_iterator robot_iter = pRobotListCfg->begin();
		for (; robot_iter != pRobotListCfg->end(); ++robot_iter)
		{
			tRankData.dwTargetId = robot_iter->dwRobotId;
			tRankData.wCurRank = robot_iter->wRank;
			tRankData.wHistoryBestRank = 0;
			vRankList.push_back(tRankData);
		}

		m_RankList.insert(make_pair(dwServerId, vRankList));
		iter = m_RankList.find(dwServerId);
	}
	return &iter->second;
}

// ȡ�����������
ArenaRankData* CArenaManager::GetPlayerRankData(DWORD dwServerId, DWORD dwRoleId)
{
	ArenaRankMap* pRankList = GetServerRankList(dwServerId);
	if (pRankList)
	{
		ArenaRankMap::iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			if (iter->dwTargetId == dwRoleId)
			{
				return &*iter;
			}
		}
	}
	return NULL;
}

// ���ս��
void CArenaManager::AddBattleRecord(DWORD dwRoleId, const ArenaBattleRecord &tRecord)
{
	ArenaBattleRecordMap::iterator iter = m_vBattleRecordList.find(dwRoleId);
	if (iter == m_vBattleRecordList.end())
	{
		list<ArenaBattleRecord> vRecordList;
		vRecordList.push_back(tRecord);
		m_vBattleRecordList.insert(make_pair(dwRoleId, vRecordList));
	}
	else
	{
		iter->second.push_back(tRecord);
		if (iter->second.size() > MAX_ARENA_BATTLE_RECORD)
		{
			iter->second.pop_front();
		}
	}

	// �洢�����ݿ� todo::������Ҫ����Ϊ������������ͬʱ�����洢
	SaveBattleRecord(dwRoleId);
}

// ���ս��
void CArenaManager::AddBattleRecord(DWORD dwSrcRoleId, DWORD dwTargetId, WORD wSrcRank, WORD wTargetRank, 
	const char *pszSrcName, const char *pszTargetName, WORD wScrHeadIcon, WORD wTargetHeadIcon, WORD wScrLevel, WORD wTargetLevel, BYTE byIsWin)
{
	ArenaBattleRecord tSrcBattleRecord;
	ArenaBattleRecord tTargetBattleRecord;

	// Id
	tSrcBattleRecord.dwTargetId = dwTargetId;
	tTargetBattleRecord.dwTargetId = dwSrcRoleId;

	// ����
	strcpy_s(tSrcBattleRecord.szTargetName, sizeof(tSrcBattleRecord.szTargetName) - 1, pszTargetName);
	strcpy_s(tTargetBattleRecord.szTargetName, sizeof(tTargetBattleRecord.szTargetName) - 1, pszSrcName);

	// սǰ����
	tSrcBattleRecord.wMyOldRank = wSrcRank;
	tTargetBattleRecord.wMyOldRank = wTargetRank;

	// ս������
	if (byIsWin && wSrcRank > wTargetRank)
	{
		tSrcBattleRecord.wMyNewRank = wTargetRank;
		tTargetBattleRecord.wMyNewRank = wSrcRank;
	}
	else
	{
		tSrcBattleRecord.wMyNewRank = wSrcRank;
		tTargetBattleRecord.wMyNewRank = wTargetRank;
	}

	// �Ƿ��ʤ
	tSrcBattleRecord.byIsWin = byIsWin ? true : false;
	tTargetBattleRecord.byIsWin = byIsWin ? false : true;

	// �Ƿ񹥻���
	tSrcBattleRecord.byIsAttack = true;
	tTargetBattleRecord.byIsAttack = false;

	// ��սʱ��
	tSrcBattleRecord.dwTime = time(NULL);
	tTargetBattleRecord.dwTime = time(NULL);

	// ͷ��
	tSrcBattleRecord.wTargetHeadIcon = wTargetHeadIcon;
	tTargetBattleRecord.wTargetHeadIcon = wScrHeadIcon;

	// �ȼ�
	tSrcBattleRecord.wTargetLevel = wTargetLevel;
	tTargetBattleRecord.wTargetLevel = wScrLevel;

	// ������¼
	AddBattleRecord(dwSrcRoleId, tSrcBattleRecord);

	// Ŀ�귽��¼(�����˵Ļ�����Ҫ��¼)
	if (!IsRobotId(dwTargetId))
		AddBattleRecord(dwTargetId, tTargetBattleRecord);
}

// �������а�
void CArenaManager::UpdateRank(DWORD dwServerId)
{
	
}