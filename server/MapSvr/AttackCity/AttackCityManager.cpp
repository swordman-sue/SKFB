#include "stdafx.h"
#include "AttackCityManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Guild\GuildManager.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\AccountInfo.h"
#include "..\Friend\FriendManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\Rank\RankManager.h"
#include "..\Item\ItemManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Spirit\SpiritManager.h"
#include "..\Title\TitleManager.h"
#include "..\SystemNotice\SystemNotice.h"

CAttackCityManager::CAttackCityManager()
{
	m_bStart = false;
	m_byActivityStatus = 0;
	m_dwSeriesId = 0;
	DWORD dwCurTime = time(NULL);
	m_dwProcessRefreshBoxTimer = dwCurTime;
	//m_dwProcessMonsterTimer = dwCurTime;
}

CAttackCityManager::~CAttackCityManager()
{

}

void CAttackCityManager::Init()
{
	//m_wCurWave = 1;
	m_dwRefreshTreasureBoxCD = g_GetConfigValue(169);
	m_dwRefreshMonsterCD = g_GetConfigValue(170);
}

// ��ʼ
void CAttackCityManager::Start()
{
	m_bStart = true;
}

void CAttackCityManager::Update()
{
	if (!m_bStart)
		return;

	tm tmDetailTime;
	GetTMTime(&tmDetailTime, time(NULL));
	const AttackCityActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetAttackCityActivityTime(tmDetailTime.tm_wday);
	// ����ľ�л
	if (!pActivityTimeCfg)
		return;

	int nCurTime = tmDetailTime.tm_hour * 100 + tmDetailTime.tm_min;

	// ����
	if (0 == m_byActivityStatus)
	{
		// ����Ƿ�ʼ
		if (nCurTime >= pActivityTimeCfg->nStartTime && nCurTime <= pActivityTimeCfg->nOverTime)
			OnActivityStart();
	}
	// ���
	else if (1 == m_byActivityStatus)
	{
		OnActivityIng();

		// ����Ƿ����
		if (nCurTime >= pActivityTimeCfg->nOverTime)
			OnActivityOver();
	}
	// �ȴ�������
	else if (2 == m_byActivityStatus)
	{
		OnReward();
	}
}

// �ǳ�
void CAttackCityManager::OnLogout(Player *pPlayer)
{
	DoLeaveAttackCity(pPlayer);
}

// ս����ʼ
bool CAttackCityManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// ȡ��������
	AttackCityMap *pMap = GetAttackCityMap(pPlayer->GetServerId(), dwTargetId);
	if (!pMap)
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_DIE);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]���ﲻ���ڻ���������[target_id:%d]", dwTargetId);
		return false;
	}

	// ����Ƿ�������
	if (/*IsMonsterDead(*pMap)*/pMap->bIsDead)
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_DIE);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]����������[target_id:%d]", dwTargetId);
		return false;
	}

	const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(pMap->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]�Ҳ��������½�����[chapter:%d]", pMap->wChapter);
		return false;
	}

	// �����Ƿ�����ս����
	//if (pMap->bIsInBattle)
	if (pMap->dwStartBattleTime && time(NULL) < pMap->dwStartBattleTime + g_GetConfigValue(209))
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_INVALID);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]��������ս����...[target_id:%d]", dwTargetId);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	for (size_t i = 0; i < pMap->vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pMap->vMonsterList[i]);
	}

	pFBMgr->SetBattleInfo(pMap->wChapter, 0, pMap->dwMapId);
	pFBMgr->SetTargetId(dwTargetId);

	// �����BOSS��,������ս�б�
	if (pChapterCfg->byIsBoss)
		AddBattleBossPlayer(pPlayer);

	// boss����Ҫ��ʾս��״̬
	if (!pChapterCfg->byIsBoss)
		pMap->dwStartBattleTime = time(NULL);
		//pMap->bIsInBattle = true;

	// ��¼������ս�Ĺ���
	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	tPlayerAttackCityData.dwBattleTargetId = dwTargetId;

	// �㲥�������ݱ䶯
	MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE msg;
	msg.m_tMonster = *pMap;
	SendToAll(&msg, pPlayer->GetServerId());

	return true;
}

// ս������
bool CAttackCityManager::OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum, DWORD dwHPRemainRate)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	DWORD dwTargetId = pFBMgr->GetTargetId();

	// ȡ��������
 	AttackCityMap *pMap = GetAttackCityMap(pPlayer->GetServerId(), dwTargetId);
 	if (!pMap)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]���ﲻ���ڻ���������[target_id:%d]",__FUNCTION__, dwTargetId);
 		return false;
 	}

	const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(pMap->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ǲ�����[wave:%d]", __FUNCTION__, pMap->wChapter);
		return false;
	}

	// for test
// 	if (pChapterCfg->byIsBoss)
// 		MAPLOG(ERROR_LEVEL, "[%s]��������BOSS�����ϱ�[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());

 	// �����Ƿ�����ս����(ֻ��С����Ҫ�Ż�����) 
	if (!pChapterCfg->byIsBoss && !pMap->dwStartBattleTime)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]����û��ս����...[target_id:%d]",__FUNCTION__, dwTargetId);
 		return false;
 	}

	//pMap->bIsInBattle = false;
	pMap->dwStartBattleTime = 0;

	// Ϊ����Ͽͻ��˽������
	pFBMgr->SetParam(2, pMap->wChapter);

	pFBMgr->SetMapId(pMap->dwMapId);

	// �����û�ҵ��ٽ�ȥ
	if (!IsMonsterDead(pMap->vMonsterList))
	{
		// �����˺�
		UpdateMonsterDamage(pMap->vMonsterList, pMonsterList, byMonsterNum);

		PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
		tPlayerAttackCityData.dwBattleOverTime = time(NULL);
		tPlayerAttackCityData.wReliveCD = 0;
		tPlayerAttackCityData.wWeakCD = 0;
		tPlayerAttackCityData.dwBattleTargetId = 0;

		// ������ս����
		pPlayer->ModifyPropertyList(pChapterCfg->vBattleMoneyList, TRUE, FROM_ATTACK_CITY_KILL_REWARD);

		// �㲥�������ݱ䶯
		MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE msg;
		msg.m_tMonster = *pMap;
		SendToAll(&msg, pPlayer->GetServerId());

		ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());

		bool bIsKiller = false;

		// ����Ƿ�������
		if (IsMonsterDead(pMap->vMonsterList))
		{
			bIsKiller = true;

			// ���Ż�ɱ����
			pPlayer->AddObjectList(pChapterCfg->vKillRewardItemList, TRUE, FROM_ATTACK_CITY_KILL_REWARD);

			// �Ƿ�ǰ���еĹֶ�������
			if (IsAllMonsterDead(pPlayer->GetServerId()))
				OnAllMonsterDead(pPlayer->GetServerId(), pMap->wChapter, pPlayer);

			// �����С�����������״̬
			if (!pChapterCfg->byIsBoss)
				tPlayerAttackCityData.wWeakCD = g_pCfgMgr->GetAttackCityWeakCD(dwHPRemainRate);

			// ���������
			pMap->bIsDead = true;

			pFBMgr->SetIsMeKill(true);

			MAPLOG(ERROR_LEVEL, "[%s]Boss��ɱ��[role_name:%s][monster_id:%d]", 
				__FUNCTION__, pPlayer->GetUTF8RoleName(), pMap->dwUID);
		}
		else
		{
			tPlayerAttackCityData.wReliveCD = g_GetConfigValue(167);
		}

		SendPlayerAttackCityData(pPlayer);

		// �����õĻ���
		DWORD dwScore = CalculateScore(pChapterCfg->dwBattleScore, pChapterCfg->dwKillScore, tPlayerAttackCityData.byEncourageTimes, bIsKiller);
		tPlayerAttackCityData.dwScore += dwScore;

		// �������а�
		WORD wOldRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_ATTACK_CITY);
		CRankManager::Instance()->AddRoleRankData(RANK_ATTACK_CITY, pPlayer, tPlayerAttackCityData.dwScore);
		WORD wNewRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_ATTACK_CITY);

		// ǰ3�������б仯
		if (wOldRank != wNewRank && wNewRank <= 3)
			SendTopRankList(pPlayer, true);

		// ��������������б仯 ���� ��ɱ����BOSS, ����Ҫ����ͬ��ÿ���˵�����
		if (wOldRank != wNewRank || (bIsKiller && pChapterCfg->byIsBoss))
		{
			// ����ͬ��ÿ��������
			SynRankToAll(pPlayer->GetServerId());
		}
		// ����ֻͬ�������ߵĻ���
		else
		{
			// ͬ������
			const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_ATTACK_CITY, pPlayer->GetRoleId());
			if (pRankData)
			{
				MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE msg;
				msg.m_wRank = pRankData->wRank;
				msg.m_dwScore = pRankData->dwValue;
				pPlayer->SendPacket(&msg);
			}
		}

		// ���BOSS�ؿ�
		if (pChapterCfg->byIsBoss)
			RemoveBattleBossPlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());
	}

	return true;
}

// ��ɱ��ǰ���й���(����ʹ��)
void CAttackCityManager::OnKillCurAllMonster(DWORD dwServerId)
{
	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tChapterList.begin();
	for (; iter != tChapterList.end(); ++iter)
	{
		for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
		{
			AttackCityMap &tMap = iter->second.vMapList[i];
			// ֻ����û��ս���е�,��û�ҵ���
			if (!tMap.dwStartBattleTime && !tMap.bIsDead)
			{
				for (size_t k = 0; k < tMap.vMonsterList.size(); ++k)
				{
					MonsterData &tMonster = tMap.vMonsterList[k];
					tMonster.dwDamage = tMonster.dwTotalHP;
				}
				tMap.bIsDead = true;
			}
		}
	}
}

// ���ʼ
void CAttackCityManager::OnActivityStart()
{
	// ��¼��ǰ��������
	vector<Player*> vPlayerList;
	map<DWORD, ServerAttackCityData>::iterator svr_iter = m_SvrAttackCityDataList.begin();
	for (; svr_iter != m_SvrAttackCityDataList.end(); ++svr_iter)
	{
		map<DWORD, Player*>::iterator player_iter = svr_iter->second.mPlayerList.begin();
		for (; player_iter != svr_iter->second.mPlayerList.end(); ++player_iter)
		{
			vPlayerList.push_back(player_iter->second);
		}
	}

	m_byActivityStatus = 1;
	m_PlayerAttackCityDataList.clear();
	m_LockBoxPlayerList.clear();
	m_dwSeriesId = 1;
	m_dwProcessRefreshBoxTimer = 0;
	m_dwRefreshTreasureBoxCD = g_GetConfigValue(169);
	m_dwRefreshMonsterCD = g_GetConfigValue(170);
	m_SvrAttackCityDataList.clear();

 	const map<DWORD, ServerInfo> &tServerList = g_MapServer.GetServerList();
 	map<DWORD, ServerInfo>::const_iterator iter = tServerList.begin();
 	for (; iter != tServerList.end(); ++iter)
 	{
		DWORD dwServerId = iter->second.dwServerId;
		ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);

		// �����½�(����) todo::�˴��淨��ȷ��һ��
		const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter);
		if (!pChapterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������½���������[chapter:%d]", __FUNCTION__, tGlobalData.wAttackCityChapter);
			return;
		}
		tGlobalData.wAttackCityChapter = pChapterCfg->wResetInitChapter;
		
		ServerAttackCityData tAttackCityData;
		tAttackCityData.dwServerId = dwServerId;
		tAttackCityData.dwRefreshMonsterTime = 0;
		tAttackCityData.dwNO1RoleId = tGlobalData.dwAttackCityRankNO1RoleId;
		tAttackCityData.wFromChapter = tGlobalData.wAttackCityChapter;
		m_SvrAttackCityDataList.insert(make_pair(dwServerId, tAttackCityData));

		// �������а�����
		CRankManager::Instance()->ResetRoleRank(dwServerId, RANK_ATTACK_CITY);

		// ��㲥
		MSG_S2C_ATTACK_CITY_ACTIVITY_START_NOTICE msg;
		SendToAll(&msg, dwServerId);
 	}

	// ��ǰ�����������½���
	for (size_t i = 0; i < vPlayerList.size(); ++i)
	{
		OnNetAttackCityEnter(vPlayerList[i]);
	}

	MAPLOG(ERROR_LEVEL, "[%s]���﹥�ǻ�ѿ�ʼ...", __FUNCTION__);
}

// ���
void CAttackCityManager::OnActivityIng()
{
	DWORD dwCurTime = time(NULL);
	if (dwCurTime >= m_dwProcessRefreshBoxTimer + m_dwRefreshTreasureBoxCD)
	{
		CreateTreasureBox();
		m_dwProcessRefreshBoxTimer = dwCurTime;
	}

	// ����BOSS������ˢ��ʱ��(��)
	DWORD dwAfterBossDeadRefreshMonsterCD = g_GetConfigValue(171);

	map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.begin();
	for (; iter != m_SvrAttackCityDataList.end(); ++iter)
	{
		ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(iter->second.dwServerId);
		const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter - 1);
		DWORD dwRefreshMonsterCD = pChapterCfg && pChapterCfg->byIsBoss ? dwAfterBossDeadRefreshMonsterCD : m_dwRefreshMonsterCD;
		if (dwCurTime >= iter->second.dwRefreshMonsterTime + dwRefreshMonsterCD)
			CreateMonster(iter->second.dwServerId);
	}
}

// �����
void CAttackCityManager::OnActivityOver()
{
	m_byActivityStatus = 2;
	m_dwActivityOverTime = time(NULL);

	// ��㲥
	map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.begin();
	for (; iter != m_SvrAttackCityDataList.end(); ++iter)
	{
		MSG_S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE msg;
		SendToAll(&msg, iter->second.dwServerId);
	}

	MAPLOG(ERROR_LEVEL, "[%s]���﹥�ǻ�ѽ���...", __FUNCTION__);
}

// ������
void CAttackCityManager::OnReward()
{
	// ������
	if (m_dwActivityOverTime && time(NULL) >= m_dwActivityOverTime + g_GetConfigValue(192))
	{
		m_byActivityStatus = 0;
		m_dwActivityOverTime = 0;

		const AttackCityRankReward_Config *pRewardCfg = NULL;

		map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.begin();
		for (; iter != m_SvrAttackCityDataList.end(); ++iter)
		{
			const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(iter->second.dwServerId, RANK_ATTACK_CITY);
			if (!pRankList)
				continue;

			for (size_t i = 0; i < pRankList->size(); ++i)
			{
				const RoleRankData &tRankData = pRankList->at(i);
				pRewardCfg = g_pCfgMgr->GetAttackCityRankReward(tRankData.wRank);
				if (pRewardCfg)
				{
					// ������
					RewardData tRewardData;
					tRewardData.wInfoId = 13;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = tRankData.wRank;
					CRewardCenterManager::AddReward(iter->second.dwServerId, tRankData.dwRoleId, tRewardData);
					
					MAPLOG(ERROR_LEVEL, "[%s]���﹥�����з��Ž���[role_id:%d,rank:%d]",
						__FUNCTION__, tRankData.dwRoleId, tRankData.wRank);
				}

				// ��¼������һ�����
				if (0 == i)
				{
					ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(iter->second.dwServerId);
					tGlobalData.dwAttackCityRankNO1RoleId = tRankData.dwRoleId;
					g_MapServer.SaveGolbalData(iter->second.dwServerId, GOLBAL_ATTCK_CITY_RANK_NO1_ROLE_ID, tRankData.dwRoleId);
				}
			}

			// �������а�����
			CRankManager::Instance()->ResetRoleRank(iter->second.dwServerId, RANK_ATTACK_CITY);

			// �����
			iter->second.mWaveDataList.clear();

			// �屦��
			iter->second.mTreasureBoxList.clear();
		}
		MAPLOG(ERROR_LEVEL, "[%s]���﹥�ǻ�ѷ�����...", __FUNCTION__);
	}
}

// ������������
void CAttackCityManager::OnNetAttackCityData(Player *pPlayer)
{
	SendPlayerAttackCityData(pPlayer);
}

// ���빥������
void CAttackCityManager::OnNetAttackCityEnter(Player *pPlayer)
{
	// ����Ƿ����ڳ���
	if (IsInCity(pPlayer->GetServerId(),pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENTER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]������ڳ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// todo::����Ƿ��ڼ�,���ڷ������Ƿ��д˻

	MapPos tMapPost;
	if (!RandMapPos(tMapPost, 158))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENTER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�����ͼ�����ʧ��[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	pPlayer->SetMapPos(tMapPost.wMapX, tMapPost.wMapY);

	//MAPLOG(ERROR_LEVEL, "�������[role_id:%d,x:%d,y:%d]", pPlayer->GetRoleId(), tMapPost.nX, tMapPost.nY);

	// �㲥���������,���˼���
	MSG_S2C_ATTACK_CITY_ENTER_NOTICE notice;
	pPlayer->FillCommonData(&notice.m_tPlayerData);
	notice.m_tMapPos = pPlayer->GetMapPos();
	SendToAll(&notice, pPlayer->GetServerId());

	map<DWORD, Player*> &tPlayerList = GetPlayerList(pPlayer->GetServerId());

	// ��ӵ�����б�
	tPlayerList.insert(make_pair(pPlayer->GetRoleId(), pPlayer));

	// �����ƹ�������
	SendPlayerAttackCityData(pPlayer);

	MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE rank_notice;
	rank_notice.m_wRank = 0;
	rank_notice.m_dwScore = 0;
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_ATTACK_CITY, pPlayer->GetRoleId());
	if (pRankData)
	{
		rank_notice.m_wRank = pRankData->wRank;
		rank_notice.m_dwScore = pRankData->dwValue;
	}
	else
	{
		PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
		rank_notice.m_dwScore = tPlayerAttackCityData.dwScore;
	}
	pPlayer->SendPacket(&rank_notice);

	// ��Ӧ
	MSG_S2C_ATTACK_CITY_ENTER_RESP resp;
	resp.m_wPlayerNum = 0;
	map<DWORD, Player*>::iterator iter = tPlayerList.begin();
	for (; iter != tPlayerList.end(); ++iter)
	{
		if (resp.m_wPlayerNum < MAX_ATTACK_CITY_PLAYER_NUM)
		{
			PlayerCommonData &tPlayerData = resp.m_PlayerList[resp.m_wPlayerNum];
			resp.m_PlayerMapPos[resp.m_wPlayerNum] = iter->second->GetMapPos();
			iter->second->FillCommonData(&tPlayerData);
			resp.m_wPlayerNum++;
		}

	}
	pPlayer->SendPacket(&resp);

	// ���͹����б�
	SendMonsterList(pPlayer);

	// ���ͱ����б�
	SendTreasureBoxList(pPlayer);

	// ���͸������б�
	SendTopRankList(pPlayer);
}

// �뿪��������
void CAttackCityManager::OnNetAttackCityLeave(Player *pPlayer)
{
	DoLeaveAttackCity(pPlayer);

	MSG_S2C_ATTACK_CITY_LEAVE_RESP resp;
	pPlayer->SendPacket(&resp);
}

// ��������
void CAttackCityManager::OnNetGoto(Player *pPlayer, MapPos tSrcPos, MapPos tDestPos)
{
	// ����Ƿ����ڳ���
	if (!IsInCity(pPlayer->GetServerId(),pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_GOTO_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڳ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	pPlayer->SetMapPos(tDestPos.wMapX, tDestPos.wMapY);

	// ��Ӧ
	MSG_S2C_ATTACK_CITY_GOTO_RESP resp;
	resp.m_tSrcPos = tSrcPos;
	resp.m_tDestPos = tDestPos;
	pPlayer->SendPacket(&resp);

	// todo::����Ƿ���Ҫͬ��(����������һ������ʱ)
	map<DWORD, Player*> &playerList = GetPlayerList(pPlayer->GetServerId());
	if (playerList.size() >= 8)
	{
		int nRandValue = g_Rand(1, 100);
		if (nRandValue >= 20)
			return;
	}

	MSG_S2C_ATTACK_CITY_GOTO_NOTICE notice;
	notice.m_dwRoleId = pPlayer->GetRoleId();
	notice.m_tSrcPos = tSrcPos;
	notice.m_tDestPos = tDestPos;
	SendToAll(&notice, pPlayer->GetServerId());

// 	MAPLOG(ERROR_LEVEL, "��������[role_id:%d,src_x:%d, src_y:%d, dest_x:%d, dest_y:%d]", 
// 		pPlayer->GetRoleId(), tSrcPos.nX, tSrcPos.nY, tDestPos.nX, tDestPos.nY);
}

// ��ȡBoss����
void CAttackCityManager::OnNetTakeBossReward(Player *pPlayer, WORD wChapter)
{
	const AttackCityBossReward_Config *RewardCfg = g_pCfgMgr->GetAttackCityBossReward(wChapter);
	if (!RewardCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS��������[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}

	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());

	// ����Ƿ�����ȡ��
	for (size_t i = 0; i < tPlayerAttackCityData.vTakeBossRewardRecordList.size(); ++i)
	{
		if (tPlayerAttackCityData.vTakeBossRewardRecordList[i] == wChapter)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]����Boss��������ȡ��[chapter:%d]", __FUNCTION__, wChapter);
			return;
		}
	}

	// ��ӽ���
	pPlayer->AddObjectList(RewardCfg->vRewardItemList, TRUE, FROM_ATTACK_CITY_BOSS_REWARD);

	// ��ȡ��¼
	tPlayerAttackCityData.vTakeBossRewardRecordList.push_back(wChapter);

	SendPlayerAttackCityData(pPlayer);

	MSG_S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP msg;
	msg.m_wChapter = wChapter;
	pPlayer->SendPacket(&msg);
}

// ����
void CAttackCityManager::OnNetEncourage(Player *pPlayer)
{
	// �Ƿ��ڳ���
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENCOURAGE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڳ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	PlayerAttackCityData &tAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	
	WORD wEncourageTimes = tAttackCityData.byEncourageTimes + 1;
	const AttackCityEncourage_Config *pEncourageCfg = g_pCfgMgr->GetAttackCityEncourage(wEncourageTimes);
	if (!pEncourageCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENCOURAGE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ǹ�������[times:%d]", __FUNCTION__, wEncourageTimes);
		return;
	}

	// �������
	if (pPlayer->GetProperty(pEncourageCfg->tCostData.wType) < pEncourageCfg->tCostData.nValue)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENCOURAGE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���ǹ������Ĳ���[times:%d]", __FUNCTION__, wEncourageTimes);
		return;
	}

	tAttackCityData.byEncourageTimes++;
	SendPlayerAttackCityData(pPlayer);

	// ��������
	pPlayer->ModifyProperty(pEncourageCfg->tCostData.wType, -pEncourageCfg->tCostData.nValue, TRUE, FROM_ATTACK_CITY_ENCOURAGE);

	MSG_S2C_ATTACK_CITY_ENCOURAGE_RESP msg;
	pPlayer->SendPacket(&msg);
}

// ��������
void CAttackCityManager::OnNetTreasureBoxLock(Player *pPlayer, DWORD dwTreasureBoxUId)
{
	// �Ƿ��ڳ���
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TREASURE_BOX_LOCK_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڳ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ��⵱ǰ�Ƿ�������ס��������
	if (IsLockBox(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TREASURE_BOX_LOCK_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]����Ѿ���ס����������[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_TREASURE_BOX_LOCK_RESP msg;
	msg.m_dwUID = dwTreasureBoxUId;

	// �����Ƿ����
	TreasureBox *pTreasureBox = GetTreasureBox(pPlayer->GetServerId(), dwTreasureBoxUId);
	if (!pTreasureBox)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]���䲻���ڻ����ѱ�ʰȡ[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	DWORD dwCurTime = time(NULL);

	// ��ⱦ���Ƿ��ѱ���ס(�Ӻ�1��)
	if (pTreasureBox->dwLockPlayerId && dwCurTime <= pTreasureBox->dwLockTime + g_GetConfigValue(166) + 1)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]�����ѱ���ס[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	pTreasureBox->dwLockPlayerId = pPlayer->GetRoleId();
	pTreasureBox->dwLockTime = dwCurTime;

	msg.m_dwUID = dwTreasureBoxUId;
	pPlayer->SendPacket(&msg);
}

// ����ʰȡ
void CAttackCityManager::OnNetTreasureBoxPickUp(Player *pPlayer, DWORD dwTreasureBoxUId)
{
	WORD wFromMsgId = C2S_TREASURE_BOX_PICK_UP_REQ;

	// �Ƿ��ڳ���
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڳ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// �����Ƿ����
	TreasureBox *pTreasureBox = GetTreasureBox(pPlayer->GetServerId(), dwTreasureBoxUId);
	if (!pTreasureBox)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]���䲻����[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	// �����ס�����ʰȡ���Ƿ�ͬһ�����
	if (pTreasureBox->dwLockPlayerId != pPlayer->GetRoleId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]��ס�����ʰȡ���䲻��ͬһ�����[box_uid:%d,lock_id:%d,pick_up_id:%d]",
			__FUNCTION__, dwTreasureBoxUId, pTreasureBox->dwLockPlayerId, pPlayer->GetRoleId());
		return;
	}

	// ���CDʱ��
	if (time(NULL) < pTreasureBox->dwLockTime + g_GetConfigValue(166))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]����ʰȡ,����CD��...[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	// һ���ǻ��ʹ�õ�������
	vector<Item> vItemList;
	if (!pPlayer->GetItemManager()->UseItem(pTreasureBox->dwInfoId, 1, &vItemList))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ʹ�ù���ʰȡ��Ʒʧ��[ItemId:%d]", __FUNCTION__, pTreasureBox->dwInfoId);
		return;
	}

	// ɾ��������״̬
	RemoveLockBoxPlayer(pPlayer->GetRoleId());

	DWORD dwInfoId = pTreasureBox->dwInfoId;

	// ɾ���ѱ�ʰȡ�ı���
	RemoveTreasureBox(pPlayer->GetServerId(), dwTreasureBoxUId);

	// �㲥
	MSG_S2C_TREASURE_BOX_DISAPPEAR_NOTICE notice;
	notice.m_dwUID = dwTreasureBoxUId;
	SendToAll(&notice, pPlayer->GetServerId());

	// ��Ӧ
	MSG_S2C_TREASURE_BOX_PICK_UP_RESP resp;
	resp.m_dwUID = dwTreasureBoxUId;
	resp.m_dwInfoId = dwInfoId;
	resp.m_vItemList = vItemList;
	pPlayer->SendPacket(&resp);
}

// ��������
void CAttackCityManager::OnNetRelive(Player *pPlayer, BYTE byIsFreeRelive)
{
	WORD wFromMsgId = C2S_ATTACK_CITY_RELIVE_REQ;

	// �Ƿ��ڳ���
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڳ���[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	PlayerAttackCityData &tAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());

	// ��Ѹ���
	if (byIsFreeRelive)
	{
		// ���ʱ��
		if (time(NULL) < tAttackCityData.wReliveCD + g_GetConfigValue(167))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�������������CD��...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
			return;
		}

		// �㲥λ��
		MSG_S2C_ATTACK_CITY_ENTER_NOTICE notice;
		pPlayer->FillCommonData(&notice.m_tPlayerData);
		if (!RandMapPos(notice.m_tMapPos, 158))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]�����ͼ�����ʧ��[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
			return;
		}
		SendToAll(&notice, pPlayer->GetServerId());
	}
	// ���ĸ���
	else
	{
		// �������
		int nCostDiamond = g_GetConfigValue(168);
		if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]���������������ĵ���ʯ����[cost:%d]", __FUNCTION__, nCostDiamond);
			return;
		}

		// ��������
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_ATTACK_CITY_RELIVE);
	}

	// ���ø�������
	tAttackCityData.wReliveCD = 0;
	tAttackCityData.wWeakCD = 0;

	// ����ͬ������
	SendPlayerAttackCityData(pPlayer);

	// ��Ӧ
	MSG_S2C_ATTACK_CITY_RELIVE_RESP msg;
	msg.m_byIsFreeRelive = byIsFreeRelive;
	pPlayer->SendPacket(&msg);
}

// �Ƿ��ڳ���
bool CAttackCityManager::IsInCity(DWORD dwServerId, DWORD dwRoleId)
{
	map<DWORD, Player*> &tPlayerList = GetPlayerList(dwServerId);
	map<DWORD, Player*>::iterator iter = tPlayerList.find(dwRoleId);
	return iter != tPlayerList.end() ? true : false;
}

// �㲥��Ϣ
void CAttackCityManager::SendToAll(NetMsgBody *pMsg, DWORD dwServerId)
{
	map<DWORD, Player*> &tPlayerList = GetPlayerList(dwServerId);
	map<DWORD, Player*>::iterator iter = tPlayerList.begin();
	for (; iter != tPlayerList.end(); ++iter)
	{
		iter->second->SendPacket(pMsg);
	}
}

// �����ͼ�����
bool CAttackCityManager::RandMapPos(MapPos &tMapPos, WORD wKey)
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(wKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������������[key:%d]", __FUNCTION__, wKey);
		return false;
	}

	if (4 == pServerCfg->vValueList.size())
	{
		tMapPos.wMapX = g_Rand(pServerCfg->GetListValue(0), pServerCfg->GetListValue(1));
		tMapPos.wMapY = g_Rand(pServerCfg->GetListValue(2), pServerCfg->GetListValue(3));
	}
	else if (2 == pServerCfg->vValueList.size())
	{
		tMapPos.wMapX = pServerCfg->GetListValue(0);
		tMapPos.wMapY = pServerCfg->GetListValue(1);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������������ó���[key:%d]", __FUNCTION__, wKey);
	}

	return true;
}

// ��������
void CAttackCityManager::CreateMonster()
{
	const map<DWORD, ServerInfo> &tServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = tServerList.begin();
	for (; iter != tServerList.end(); ++iter)
	{
		CreateMonster(iter->second.dwServerId);
	}
}

// ��������
void CAttackCityManager::CreateMonster(DWORD dwServerId)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(dwServerId);
	tServerAttackCityData.dwRefreshMonsterTime = time(NULL);

	// todo::������Ҫͬ��ˢ��ʱ��

	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
	const AttackCityChapter_Config *pWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter);
	if (!pWaveMonsterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ǲ�������[wave:%d]", __FUNCTION__, tGlobalData.wAttackCityChapter);
		return;
	}

	// �����ǰҪ��������boss�ؿ�,��ǰ���С�ֱ��趼Ҫ����,�Ż����
	if (pWaveMonsterCfg->byIsBoss && !IsAllMonsterDead(dwServerId))
		return;

	// ���ǵ�ǰ��ս����BOSS,�����Ҫ�������Ż�ˢ��һ��
	if (tGlobalData.wAttackCityChapter > 1)
	{
		const AttackCityChapter_Config *pPreWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter - 1);
		if (pPreWaveMonsterCfg->byIsBoss && !IsAllMonsterDead(dwServerId))
			return;
	}

	MSG_S2C_ATTACK_CITY_MONSTER_LIST_NOTICE monster_list_notice;
	monster_list_notice.m_wCurChapter = g_MapServer.GetServerGlobalData(dwServerId).wAttackCityChapter;
	monster_list_notice.m_wMonsterNum = 0;

	//MAPLOG(ERROR_LEVEL, "[%s]��ǰ�����½�[chapter:%d]", __FUNCTION__, monster_list_notice.m_wCurChapter);

	double fBloodRate = 0;
	for (size_t i = 0; i < pWaveMonsterCfg->vMonsterProList.size(); ++i)
	{
		const Property &tPro = pWaveMonsterCfg->vMonsterProList[i];
		if (tPro.wType == HERO_PRO_BLOOD_RATE)
		{
			fBloodRate = tPro.nValue;
			break;
		}
	}

	// ��������
	AttackCityChapter tChapterData;
	tChapterData.wChapter = tGlobalData.wAttackCityChapter;
	AttackCityMap tMap;
	tMap.bIsDead = false;
	tMap.dwStartBattleTime = 0;
	tMap.wChapter = tGlobalData.wAttackCityChapter;
	WORD wRandPosKey = pWaveMonsterCfg->byIsBoss ? 163 : 165;
	for (WORD i = 0; i < pWaveMonsterCfg->wRefreshMonsterNum; ++i)
	{
// 		int nIndex = g_Rand(0, pWaveMonsterCfg->vMapList.size() - 1);
// 		tMap.dwMapId = pWaveMonsterCfg->vMapList[nIndex];

		// ����ؿ�
		DWORD dwTotalRate = 0;
		for (size_t k = 0; k < pWaveMonsterCfg->vMapRateList.size();++k)
		{
			dwTotalRate += pWaveMonsterCfg->vMapRateList[k];
		}
		tMap.dwMapId = 0;
		DWORD dwRandValue = g_Rand(0, dwTotalRate);
		DWORD dwCurRate = 0;
		for (size_t k = 0; k < pWaveMonsterCfg->vMapRateList.size(); ++k)
		{
			dwCurRate += pWaveMonsterCfg->vMapRateList[k];
			if (dwRandValue <= dwCurRate && k < pWaveMonsterCfg->vMapList.size())
			{
				tMap.dwMapId = pWaveMonsterCfg->vMapList[k];
				break;
			}
		}

		if (!RandMapPos(tMap.tMapPos, wRandPosKey))
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ǹ�����ʧ��[wave:%d,mapid:%d]", __FUNCTION__, tChapterData.wChapter, tMap.dwMapId);
			return;
		}
		tMap.vMonsterList.clear();
		if (!g_pCfgMgr->GetMapMonsterList(tMap.dwMapId, &tMap.vMonsterList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡ���ǹ����б�ʧ��[MapId:%d]!", __FUNCTION__, tMap.dwMapId);
			return;
		}
		// Ӱ������
		for (size_t k = 0; k < tMap.vMonsterList.size(); k++)
		{
			//tMap.vMonsterList[k].dwTotalHP += nAddHPValue * PROPERTY_DECREASE_MULTIPLE;
			tMap.vMonsterList[k].dwTotalHP *= (1 + fBloodRate * PROPERTY_DECREASE_MULTIPLE);
		}

		tMap.dwUID = GetNewSeriesId();
		tChapterData.vMapList.push_back(tMap);

		monster_list_notice.m_arMonsterList[monster_list_notice.m_wMonsterNum++] = tMap;
	}


	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	tChapterList.insert(make_pair(tChapterData.wChapter, tChapterData));

	SendToAll(&monster_list_notice, dwServerId);

	// ָ����һ��(׼��ˢ����)
	tGlobalData.wAttackCityChapter++;
	g_MapServer.SaveGolbalData(dwServerId, GOLBAL_ATTCK_CITY_CHAPTER, tGlobalData.wAttackCityChapter);
}

// ɾ������
void CAttackCityManager::RemoveMonster(DWORD dwServerId, WORD wChapter, DWORD dwMonsterUId)
{
	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tChapterList.find(wChapter);
	if (iter != tChapterList.end())
	{
		vector<AttackCityMap>::iterator monster_iter = iter->second.vMapList.begin();
		for (; monster_iter != iter->second.vMapList.end(); ++monster_iter)
		{
			if (monster_iter->dwUID == dwMonsterUId)
			{
				iter->second.vMapList.erase(monster_iter);
				break;
			}
		}
	}
}

// ��������
void CAttackCityManager::CreateTreasureBox()
{
	const map<DWORD, ServerInfo> &tServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = tServerList.begin();
	for (; iter != tServerList.end(); ++iter)
	{
		CreateTreasureBox(iter->second.dwServerId);
	}
}

// ��������
void CAttackCityManager::CreateTreasureBox(DWORD dwServerId)
{
	// todo::���˷������Ƿ��д˻

	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
	const AttackCityChapter_Config *pWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter);
	if (!pWaveMonsterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ǲ�������[wave:%d]", __FUNCTION__, tGlobalData.wAttackCityChapter);
		return;
	}

	MSG_S2C_TREASURE_BOX_LIST_NOTICE msg;
	msg.m_wTreasureBoxNum = 0;

	TreasureBox tBox;
	tBox.dwCreateTime = time(NULL);
	tBox.dwInfoId = pWaveMonsterCfg->dwRefreshItemId;
	tBox.dwLockPlayerId = 0;
	tBox.dwLockTime = 0;

	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);

	WORD wCreateNum = g_Rand(pWaveMonsterCfg->wRefreshItemNumMin, pWaveMonsterCfg->wRefreshItemNumMax);
	for (WORD i = 0; i < wCreateNum; ++i)
	{
		// �������ָ�����������ٴ���
		if (tAttackCityData.mTreasureBoxList.size() >= g_GetConfigValue(175))
			break;

		tBox.dwUID = GetNewSeriesId();
		RandMapPos(tBox.tMapPos, 165);
		if (AddTreasureBox(dwServerId, tBox))
		{
			if (msg.m_wTreasureBoxNum < MAX_ATTACK_CITY_TREASURE_BOX_NUM)
				msg.m_TreasureBoxList[msg.m_wTreasureBoxNum++] = tBox;
		}
	}

	if (msg.m_wTreasureBoxNum)
		SendToAll(&msg, dwServerId);
}

// ����Boss����
void CAttackCityManager::CreateBossBox(DWORD dwServerId, WORD wChapter)
{
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
	const AttackCityChapter_Config *pWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(wChapter);
	if (!pWaveMonsterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ǲ�������[wave:%d]", __FUNCTION__, wChapter);
		return;
	}

	MSG_S2C_TREASURE_BOX_LIST_NOTICE msg;
	msg.m_wTreasureBoxNum = 0;

	TreasureBox tBox;
	tBox.dwCreateTime = time(NULL);
	tBox.dwInfoId = pWaveMonsterCfg->dwBossItemId;
	tBox.dwLockPlayerId = 0;
	tBox.dwLockTime = 0;

	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);

	WORD wCreateNum = g_Rand(pWaveMonsterCfg->wBossItemNumMin, pWaveMonsterCfg->wBossItemNumMax);
	for (WORD i = 0; i < wCreateNum; ++i)
	{
		// �������ָ�����������ٴ��� todo::bossˢ�ı��䲻����������
// 		if (tAttackCityData.mTreasureBoxList.size() >= g_GetConfigValue(175))
// 			break;

		tBox.dwUID = GetNewSeriesId();
		RandMapPos(tBox.tMapPos, 164);
		if (AddTreasureBox(dwServerId, tBox))
		{
			if (msg.m_wTreasureBoxNum < MAX_ATTACK_CITY_TREASURE_BOX_NUM)
				msg.m_TreasureBoxList[msg.m_wTreasureBoxNum++] = tBox;
		}
	}

	if (msg.m_wTreasureBoxNum)
		SendToAll(&msg, dwServerId);
}

// ��ǰ�����Ƿ���ȫ������
bool CAttackCityManager::IsAllMonsterDead(DWORD dwServerId)
{
	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tChapterList.begin();

	// ������
	for (; iter != tChapterList.end(); ++iter)
	{
		// �����ؿ�
		for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
		{
			// ��������
			AttackCityMap &tMap = iter->second.vMapList[i];
			if (!IsMonsterDead(tMap))
				return false;
		}
	}

	return true;
}

// �Ƿ����������
bool CAttackCityManager::IsMonsterDead(const AttackCityMap &tMap)
{
	for (size_t i = 0; i < tMap.vMonsterList.size(); ++i)
	{
		const MonsterData &tMonster = tMap.vMonsterList[i];
		if (tMonster.dwDamage < tMonster.dwTotalHP)
			return false;
	}
	return true;
}

// ���͹����б�
void CAttackCityManager::SendMonsterList(Player *pPlayer, WORD wChapter, bool bIsToAll)
{
	MSG_S2C_ATTACK_CITY_MONSTER_LIST_NOTICE msg;
	msg.m_wCurChapter = g_MapServer.GetServerGlobalData(pPlayer->GetServerId()).wAttackCityChapter - 1;
	msg.m_wCurChapter = max(msg.m_wCurChapter, 1);

	//MAPLOG(ERROR_LEVEL, "[%s]��ǰ�����½�[chapter:%d]", __FUNCTION__, msg.m_wCurChapter);


	msg.m_wMonsterNum = 0;

	// ������ڻ�����͹��Ｐ�����б�
	if (1 == m_byActivityStatus)
	{
		map<WORD, AttackCityChapter> &tWaveDataList = GetAttackCityChapterList(pPlayer->GetServerId());
		if (wChapter)
		{
			map<WORD, AttackCityChapter>::iterator iter = tWaveDataList.find(wChapter);
			if (iter != tWaveDataList.end())
			{
				for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
				{
					if (msg.m_wMonsterNum < MAX_ATTACK_CITY_MONSTER_NUM)
					{
						AttackCityMap &tMonster = iter->second.vMapList[i];
						msg.m_arMonsterList[msg.m_wMonsterNum++] = tMonster;
					}
				}
			}
		}
		else
		{
			map<WORD, AttackCityChapter>::iterator iter = tWaveDataList.begin();
			for (; iter != tWaveDataList.end(); ++iter)
			{
				for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
				{
					AttackCityMap &tMonster = iter->second.vMapList[i];
					// ֻ����û��������,��ΪĿ����߼��������Ĺ���û��ɾ��
					if (/*!IsMonsterDead(tMonster)*/!tMonster.bIsDead)
					{
						if (msg.m_wMonsterNum < MAX_ATTACK_CITY_MONSTER_NUM)
							msg.m_arMonsterList[msg.m_wMonsterNum++] = tMonster;
					}
				}
			}
		}
	}

	if (bIsToAll)
		SendToAll(&msg, pPlayer->GetServerId());
	else
		pPlayer->SendPacket(&msg);
}

// ���ͱ����б�
void CAttackCityManager::SendTreasureBoxList(Player *pPlayer)
{
	// ������ڻ�����͹��Ｐ�����б�
	if (1 == m_byActivityStatus)
	{
		MSG_S2C_TREASURE_BOX_LIST_NOTICE msg;
		msg.m_wTreasureBoxNum = 0;

		ServerAttackCityData &tAttackCityData = GetAttackCityData(pPlayer->GetServerId());
		map<DWORD, TreasureBox>::iterator iter = tAttackCityData.mTreasureBoxList.begin();
		for (; iter != tAttackCityData.mTreasureBoxList.end(); ++iter)
		{
			if (msg.m_wTreasureBoxNum >= MAX_ATTACK_CITY_TREASURE_BOX_NUM)
				break;

			msg.m_TreasureBoxList[msg.m_wTreasureBoxNum++] = iter->second;
		}

		pPlayer->SendPacket(&msg);
	}
}

// ������ҹ�������
void CAttackCityManager::SendPlayerAttackCityData(Player *pPlayer)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	MSG_S2C_ATTACK_CITY_DATA_RESP msg;
	msg.m_AttackCityData = tPlayerAttackCityData;
	msg.m_wFromChapter = tServerAttackCityData.wFromChapter;
	msg.m_dwKillBossTime = 0;
	msg.m_dwNo1RoleHeadImgId = 0;
	memset(msg.m_szNo1RoleName, 0, sizeof(msg.m_szNo1RoleName));
	
	
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(pPlayer->GetServerId());
	if (tGlobalData.wAttackCityChapter)
	{
		const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter - 1);
		if (pChapterCfg && pChapterCfg->byIsBoss && IsAllMonsterDead(pPlayer->GetServerId()))
			msg.m_dwKillBossTime = tServerAttackCityData.dwRefreshMonsterTime;
	}

	if (tGlobalData.dwAttackCityRankNO1RoleId)
	{
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), tGlobalData.dwAttackCityRankNO1RoleId);
		if (pBaseData)
		{
			msg.m_dwNo1RoleHeadImgId = pBaseData->wHeadIcon;
			strcpy_s(msg.m_szNo1RoleName, sizeof(msg.m_szNo1RoleName), pBaseData->szRoleName);
		}
	}


	pPlayer->SendPacket(&msg);
}

// ���͸���������б�
void CAttackCityManager::SendTopRankList(Player *pPlayer, bool bIsToAll)
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(pPlayer->GetServerId(), RANK_ATTACK_CITY);
	if (pRankList)
	{
		MSG_S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE msg;
		msg.m_wListNum = 0;
		for (size_t i = 0; i < pRankList->size(); ++i)
		{
			const RoleRankData &tRankData =	pRankList->at(i);
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), tRankData.dwRoleId, false);

			if (msg.m_wListNum < MAX_DEFAULT_RANK_MUM)
				msg.m_arRankList[msg.m_wListNum++] = *pBaseData;
		}

		if (bIsToAll)
			SendToAll(&msg, pPlayer->GetServerId());
		else
			pPlayer->SendPacket(&msg);
	}
}

// �������˶�ͬ������
void CAttackCityManager::SynRankToAll(DWORD dwServerId)
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(dwServerId, RANK_ATTACK_CITY);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����������а�����[serverid:%d]", __FUNCTION__, dwServerId);
		return;
	}

	ServerAttackCityData &tAttackCity = GetAttackCityData(dwServerId);
	RoleRankList::const_iterator rank_iter = pRankList->begin();
	for (; rank_iter != pRankList->end(); ++rank_iter)
	{
		map<DWORD, Player*>::iterator player_iter = tAttackCity.mPlayerList.find(rank_iter->dwRoleId);
		if (player_iter != tAttackCity.mPlayerList.end())
		{
			MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE msg;
			msg.m_wRank = rank_iter->wRank;
			msg.m_dwScore = rank_iter->dwValue;
			player_iter->second->SendPacket(&msg);
		}
	}
}

// �Ƿ��к��
bool CAttackCityManager::IsHaveRedPoint(Player *pPlayer)
{
	// ����Ƿ��ڻ��
	if (1 != m_byActivityStatus)
		return false;


	// ȡ��ҹ�������
	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(pPlayer->GetServerId());
	for (WORD wChapter = tServerAttackCityData.wFromChapter; wChapter < tGlobalData.wAttackCityChapter; ++wChapter)
	{
		// �����Ƿ���BOSS����
		if (!g_pCfgMgr->GetAttackCityBossReward(wChapter))
			continue;


		bool bExist = false;
		vector<WORD>::iterator iter = tPlayerAttackCityData.vTakeBossRewardRecordList.begin();
		for (; iter != tPlayerAttackCityData.vTakeBossRewardRecordList.end(); ++iter)
		{
			if (wChapter == *iter)
			{
				bExist = true;
				break;
			}
		}

		// ��û��ȡ
		if (!bExist)
			return true;
	}

	return false;
}

// ȡ����б�
map<DWORD, Player*>& CAttackCityManager::GetPlayerList(DWORD dwServerId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	return tAttackCityData.mPlayerList;
}

// ȡ������
map<WORD, AttackCityChapter>& CAttackCityManager::GetAttackCityChapterList(DWORD dwServerId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	return tAttackCityData.mWaveDataList;
}

// ȡ��������
ServerAttackCityData& CAttackCityManager::GetAttackCityData(DWORD dwServerId)
{
	map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.find(dwServerId);
	if (iter == m_SvrAttackCityDataList.end())
	{
		ServerAttackCityData tAttackCityData;
		tAttackCityData.dwServerId = dwServerId;
		tAttackCityData.dwRefreshMonsterTime = 0;
		tAttackCityData.dwKillBossTime = 0;
		tAttackCityData.dwNO1RoleId = 0;
		m_SvrAttackCityDataList.insert(make_pair(dwServerId, tAttackCityData));
		iter = m_SvrAttackCityDataList.find(dwServerId);
	}
	return iter->second;
}

// ȡ����
TreasureBox* CAttackCityManager::GetTreasureBox(DWORD dwServerId, DWORD dwUId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	map<DWORD, TreasureBox>::iterator iter = tAttackCityData.mTreasureBoxList.find(dwUId);
	return iter != tAttackCityData.mTreasureBoxList.end() ? &iter->second : NULL;
}

// ȡ�ؿ�����
AttackCityMap* CAttackCityManager::GetAttackCityMap(DWORD dwServerId, DWORD dwMapUId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tAttackCityData.mWaveDataList.begin();
	for (; iter != tAttackCityData.mWaveDataList.end(); ++iter)
	{
		for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
		{
			if (iter->second.vMapList[i].dwUID == dwMapUId)
				return &iter->second.vMapList[i];
		}
	}
	return NULL;
}

// ȡ��ҹ�������
PlayerAttackCityData& CAttackCityManager::GetPlayerAttackCityData(DWORD dwRoleId)
{
	map<DWORD, PlayerAttackCityData>::iterator iter = m_PlayerAttackCityDataList.find(dwRoleId);
	if (iter == m_PlayerAttackCityDataList.end())
	{
		PlayerAttackCityData tAttackCityData;
		memset(&tAttackCityData, 0, sizeof(tAttackCityData));
		m_PlayerAttackCityDataList.insert(make_pair(dwRoleId, tAttackCityData));
		iter = m_PlayerAttackCityDataList.find(dwRoleId);
	}
	return iter->second;
}

// ��ӱ���
bool CAttackCityManager::AddTreasureBox(DWORD dwServerId, const TreasureBox &tBox)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	// ���������������ʱ,��������ڵ�
 	if (tAttackCityData.mTreasureBoxList.size() >= MAX_ATTACK_CITY_TREASURE_BOX_NUM)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]���﹥�Ǳ���������������[serverid:%d]", __FUNCTION__, dwServerId);
 		return false;
 	}
	tAttackCityData.mTreasureBoxList.insert(make_pair(tBox.dwUID, tBox));
	return true;
}

// ɾ������
void CAttackCityManager::RemoveTreasureBox(DWORD dwServerId, DWORD dwUId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	map<DWORD, TreasureBox>::iterator iter = tAttackCityData.mTreasureBoxList.find(dwUId);
	if (iter != tAttackCityData.mTreasureBoxList.end())
	{
		tAttackCityData.mTreasureBoxList.erase(iter);
	}
}

// �Ƿ�����ס������
bool CAttackCityManager::IsLockBox(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_LockBoxPlayerList.find(dwRoleId);
	return iter != m_LockBoxPlayerList.end() ? true : false;
}

// �����ס��������
void CAttackCityManager::AddLockBoxPlayer(DWORD dwRoleId)
{
	m_LockBoxPlayerList.insert(make_pair(dwRoleId, dwRoleId));
}

// ɾ����ס��������
void CAttackCityManager::RemoveLockBoxPlayer(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_LockBoxPlayerList.find(dwRoleId);
	if (iter != m_LockBoxPlayerList.end())
	{
		m_LockBoxPlayerList.erase(iter);
	}
}

// ���¹����˺�
void CAttackCityManager::UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList, MonsterData *pMonsterList, BYTE byMonsterNum)
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

// �Ƿ��������
bool CAttackCityManager::IsMonsterDead(vector<MonsterData> &vMonsterList)
{
	for (size_t i = 0; i < vMonsterList.size(); ++i)
	{
		if (vMonsterList[i].dwDamage < vMonsterList[i].dwTotalHP)
			return false;
	}
	return true;
}

// �����뿪����
void CAttackCityManager::DoLeaveAttackCity(Player *pPlayer)
{
	// ����Ƿ����ڳ���
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
		return;

	// ɾ���뿪�����
	ServerAttackCityData &tAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	// ��ս����б�
	map<DWORD, Player*>::iterator iter1 = tAttackCityData.mPlayerList.find(pPlayer->GetRoleId());
	if (iter1 != tAttackCityData.mPlayerList.end())
	{
		tAttackCityData.mPlayerList.erase(iter1);
	}

	// ����ս��������б�(boss��)
	RemoveBattleBossPlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());

	// �㲥��������,�����˳�
	MSG_S2C_ATTACK_CITY_LEAVE_NOTICE notice;
	notice.m_dwRoleId = pPlayer->GetRoleId();
	SendToAll(&notice, pPlayer->GetServerId());

	// ɾ�������ӵ�״̬
	RemoveLockBoxPlayer(pPlayer->GetRoleId());


	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	if (tPlayerAttackCityData.dwBattleTargetId)
	{
		// ȥ��������ս״̬
		AttackCityMap *pMap = GetAttackCityMap(pPlayer->GetServerId(), tPlayerAttackCityData.dwBattleTargetId);
		if (pMap && pMap->dwStartBattleTime)
		{
			pMap->dwStartBattleTime = 0;
			MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE msg;
			msg.m_tMonster = *pMap;
			SendToAll(&msg, pPlayer->GetServerId());
		}

		// ��յ�ǰ��ս��Ŀ��ID
		tPlayerAttackCityData.dwBattleTargetId = 0;
	}
}

// �����������
bool CAttackCityManager::OnAllMonsterDead(DWORD dwServerId, WORD wChapter, Player *pKiller)
{
	const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ǲ�����[wave:%d]", __FUNCTION__, wChapter);
		return false;
	}

	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(dwServerId);

	// �����BOSS
	if (pChapterCfg->byIsBoss)
	{
		// ����������ս�е���ҽ���
		map<DWORD, Player*>::iterator iter = tServerAttackCityData.mBattlePlayerList.begin();
		for (; iter != tServerAttackCityData.mBattlePlayerList.end(); ++iter)
		{
			// for test
			//MAPLOG(ERROR_LEVEL, "[%s]����BOSS����ɱ��ʼ������[role_id:%d]", __FUNCTION__, iter->second->GetRoleId());

			// ��ɱ�߲���Ҫ����
			if (iter->second->GetRoleId() == pKiller->GetRoleId())
				continue;

			DWORD dwScore = pChapterCfg->dwBattleScore;

			PlayerAttackCityData &tOtherAttackCityData = GetPlayerAttackCityData(iter->second->GetRoleId());
			// ��������õĶ������
			if (tOtherAttackCityData.byEncourageTimes)
			{
				const AttackCityEncourage_Config *pEncourageCfg = g_pCfgMgr->GetAttackCityEncourage(tOtherAttackCityData.byEncourageTimes);
				if (!pEncourageCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[times:%d]", __FUNCTION__, tOtherAttackCityData.byEncourageTimes);
					return false;
				}
				dwScore *= (1 + pEncourageCfg->nScoreAddRate * PROPERTY_DECREASE_MULTIPLE);
			}
			tOtherAttackCityData.dwScore += dwScore;
			CRankManager::Instance()->AddRoleRankData(RANK_ATTACK_CITY, iter->second, tOtherAttackCityData.dwScore);
		}

		// Boss����
		CreateBossBox(dwServerId, wChapter);

		// �������б����
		tServerAttackCityData.mBattlePlayerList.clear();

		// ����㲥
		CSystemNotice sys_notice(16, dwServerId);
		sys_notice.AddParam(wChapter);
		sys_notice.AddParam(pKiller->GetRoleName());
		sys_notice.SendMsg();

	}

	WORD wAttackCityChapter = g_MapServer.GetServerGlobalData(dwServerId).wAttackCityChapter;
	const AttackCityChapter_Config *pPreChapterCfg = g_pCfgMgr->GetAttackCityChapter(wAttackCityChapter - 1);
	if (pPreChapterCfg)
	{
		// �����BOSS,���һ��ʱ����ˢ��
		if (pPreChapterCfg->byIsBoss)
		{
			tServerAttackCityData.dwRefreshMonsterTime = time(NULL);
			tServerAttackCityData.dwKillBossTime = time(NULL);
		}
		// �����С������ˢ����һ��
		else
		{
			CreateMonster(dwServerId);
			tServerAttackCityData.dwKillBossTime = 0;
		}
	}

	return true;
}

// �������
DWORD CAttackCityManager::CalculateScore(DWORD dwBattleScore, DWORD dwKillScore, WORD wEncourageTimes, bool bIsKiller)
{
	// ��ս����
	DWORD dwScore = dwBattleScore;

	// ��ɱ����
	if (bIsKiller)
		dwScore += dwKillScore;

	// ��������õĶ������
	if (wEncourageTimes)
	{
		const AttackCityEncourage_Config *pEncourageCfg = g_pCfgMgr->GetAttackCityEncourage(wEncourageTimes);
		if (!pEncourageCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[times:%d]", __FUNCTION__, wEncourageTimes);
			return dwScore;
		}
		dwScore *= (1 + float(pEncourageCfg->nScoreAddRate) * PROPERTY_DECREASE_MULTIPLE);
	}

	return dwScore;
}

// ɾ��������սBOSS�����
void CAttackCityManager::RemoveBattleBossPlayer(DWORD dwServerId, DWORD dwRoleId)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(dwServerId);
	map<DWORD, Player*>::iterator iter = tServerAttackCityData.mBattlePlayerList.find(dwRoleId);
	if (iter != tServerAttackCityData.mBattlePlayerList.end())
		tServerAttackCityData.mBattlePlayerList.erase(iter);
}

// �����սBOSS�����
void CAttackCityManager::AddBattleBossPlayer(Player *pPlayer)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	tServerAttackCityData.mBattlePlayerList.insert(make_pair(pPlayer->GetRoleId(), pPlayer));
}
