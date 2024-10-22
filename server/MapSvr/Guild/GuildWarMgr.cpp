#include "stdafx.h"
#include "GuildWarMgr.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Rank\RankManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "data_define.pb.h"
#include "GuildManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "DBCDefine.h"



CGuildWarMgr::CGuildWarMgr()
{
	m_bIsLoadDataFromDB = false;
	m_dwSaveTimer = 0;
}

CGuildWarMgr::~CGuildWarMgr()
{

}

void CGuildWarMgr::Init()
{
	m_bIsLoadDataFromDB = false;

	// �߼�������ս����ʱ���ʱ��(����������ʱ��)
	const vector<GuildWarVSActivityTime_Config> *pDeclareWarOverTimeCfg = g_pCfgMgr->GetGuildWarVSActivityTime(3);
	if (pDeclareWarOverTimeCfg)
	{
		CDateTimers timer;
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pDeclareWarOverTimeCfg->begin();
		for (; iter != pDeclareWarOverTimeCfg->end(); ++iter)
		{
			int nWeekDay = iter->dwDecideAttackerTime * 0.0001;
			int nTime = iter->dwDecideAttackerTime % 10000;
			timer.AddTriggerTime(nTime);
			timer.SetWeekDay(nWeekDay);
			m_vHighStarDeclareWarOverTimer.push_back(timer);
		}
	}

	// ��սʱ�����ʱ��,������ʤ��ʱ��
	const vector<GuildWarVSActivityTime_Config> *pVSOverTimeCfg = g_pCfgMgr->GetGuildWarVSActivityTime(2);
	if (pVSOverTimeCfg)
	{
		CDateTimers timer;
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pVSOverTimeCfg->begin();
		for (; iter != pVSOverTimeCfg->end(); ++iter)
		{
			int nWeekDay = iter->dwEndTime * 0.0001;
			int nTime = iter->dwEndTime % 10000;
			timer.AddTriggerTime(nTime);
			timer.SetWeekDay(nWeekDay);
			m_vHighStarActivityOverTimer.push_back(timer);
		}
	}

	// ������������ʱ��
	const Server_Config *pSvrCfg = GetCommonCfg(4);
	if (pSvrCfg){
		m_LowStarActivityOverTimer.AddTriggerTime(pSvrCfg->dwValue);
	}
}

void CGuildWarMgr::Release()
{

}

void CGuildWarMgr::Update()
{
	if (!m_bIsLoadDataFromDB)
		return;

	// ������������ʱ��
	if (m_LowStarActivityOverTimer.IsTrigger())
	{
		LowStarActivityOver();
		m_LowStarActivityOverTimer.SetCDOverTime();
	}

	// �߼�������ս����ʱ��(����������ʱ��)
	for (int i = 0; i < m_vHighStarDeclareWarOverTimer.size(); ++i)
	{
		CDateTimers &tTimer = m_vHighStarDeclareWarOverTimer[i];
		if (tTimer.IsTrigger())
		{
			HighStarDeclareWarTimeOver();
			tTimer.SetCDOverTime();
		}
	}

	// �߼���������ʱ��
	for (int i = 0; i < m_vHighStarActivityOverTimer.size(); ++i)
	{
		CDateTimers &tTimer = m_vHighStarActivityOverTimer[i];
		if (tTimer.IsTrigger())
		{
			HighStarActivityOver();
			tTimer.SetCDOverTime();
		}
	}

	// ��ʱ�洢
	DWORD dwCurTime = time(NULL);
	if (dwCurTime >= m_dwSaveTimer + 60)
	{
		SaveDataToDB();
		m_dwSaveTimer = dwCurTime;
	}
}

void CGuildWarMgr::OnNewDay()
{
	// ����ÿ����ս����
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.begin();
	for (; iter != m_RoleGuildWarDataList.end(); ++iter){
		iter->second.wUsedLowStarBattleTimes = 0;
	}

	map<DWORD, map<DWORD, StarInfo>>::iterator iterRegion = m_StarInfoList.begin();
	for (; iterRegion != m_StarInfoList.end(); ++iterRegion)
	{
		map<DWORD, StarInfo>::iterator iterStarInfo = iterRegion->second.begin();
		for (; iterStarInfo != iterRegion->second.end(); ++iterStarInfo)
		{
			iterStarInfo->second.mRewardRecordList.clear();
		}
	}

	LowStarActivityStart();
}

// �����������
void CGuildWarMgr::LoadDataRequest()
{
	GuildWarData_Load* pQuery = GuildWarData_Load::ALLOC();
	pQuery->SetIndex(DB_GUILD_WAR_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	g_MapServer.MapDBQuery(pQuery);
}

// �洢���ݵ����ݿ�
void CGuildWarMgr::SaveDataToDB()
{
	UINT64 lCurTime = g_GetCurrentTime();

	PB::GuildWarData PBGuildWarData;

	DWORD dwStarNum = 0;
	map<DWORD, map<DWORD, StarInfo>>::iterator iterStarList = m_StarInfoList.begin();
	for (; iterStarList != m_StarInfoList.end(); ++iterStarList)
	{
		dwStarNum += iterStarList->second.size();
		map<DWORD, StarInfo>::iterator iterStar = iterStarList->second.begin();
		for (; iterStar != iterStarList->second.end(); ++iterStar)
		{
			StarInfo *pStarInfo = &iterStar->second;

			PB::StarInfo *pPBStar = PBGuildWarData.add_star_list();
			pPBStar->set_region_id(iterStarList->first);
			pPBStar->set_info_id(pStarInfo->dwInfoId);
			pPBStar->set_belong_guild_id(pStarInfo->dwDelongGuildId);
			pPBStar->set_attack_guild_id(pStarInfo->dwAttackGuildId);
			pPBStar->set_last_belong_guild_id(pStarInfo->dwLastDelongGuildId);

			// ������ȡ��¼�б�
			map<DWORD, DWORD>::iterator iterRewardRecord = pStarInfo->mRewardRecordList.begin();
			for (; iterRewardRecord != pStarInfo->mRewardRecordList.end(); ++iterRewardRecord){
				pPBStar->add_took_reward_record_list(iterRewardRecord->first);
			}

			// ��ս�б�
			map<DWORD, DWORD>::iterator iterDeclareWar = pStarInfo->mDeclareWarList.begin();
			for (; iterDeclareWar != pStarInfo->mDeclareWarList.end(); ++iterDeclareWar){
				pPBStar->add_declare_war_list(iterDeclareWar->first);
			}

			// �����б�
			map<DWORD, DWORD>::iterator iterScore = pStarInfo->mScoreList.begin();
			for (; iterScore != pStarInfo->mScoreList.end(); ++iterScore){
				PB::GuildWarScore *pPBScore = pPBStar->add_score_list();
				pPBScore->set_guild_id(iterScore->first);
				pPBScore->set_score(iterScore->second);
			}

			// ���б�
			vector<StarTowerInfo>::iterator iterTower = pStarInfo->vTowerList.begin();
			for (; iterTower != pStarInfo->vTowerList.end(); ++iterTower){
				PB::StarTowerInfo *pPBTower = pPBStar->add_tower_list();
				pPBTower->set_tower_id(iterTower->wTowerId);
				pPBTower->set_durability(iterTower->dwDurability);
				pPBTower->set_monster_be_attack_times(iterTower->wMonsterBeAttackTimes);

				// �������б�
				vector<StarDefenderInfo>::iterator iterDefender = iterTower->vDefenderList.begin();
				for (; iterDefender != iterTower->vDefenderList.end(); ++iterDefender){
					PB::StarDefenderInfo *pPBDefender = pPBTower->add_defender_list();
					pPBDefender->set_role_id(iterDefender->dwRoleId);
					pPBDefender->set_be_attack_times(iterDefender->wBeAttackTimes);
				}
			}
		}
	}

	// ��ɫ����ս����
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.begin();
	for (; iter != m_RoleGuildWarDataList.end(); ++iter)
	{
		PB::RoleGuildWarData *pRoleGuildWarData = PBGuildWarData.add_role_guild_war_data_list();
		pRoleGuildWarData->set_role_id(iter->first);
		pRoleGuildWarData->set_buy_high_star_battle_times(iter->second.wBuyHighStarBattleTimes);
		pRoleGuildWarData->set_used_high_star_battle_times(iter->second.wUsedLowStarBattleTimes);
		pRoleGuildWarData->set_used_low_star_battle_times(iter->second.wUsedHighStarBattleTimes);
	}

	string strSaveData("");
	if (!PBGuildWarData.AppendToString(&strSaveData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ϵ�л�����ս����ʧ��", __FUNCTION__);
		return;
	}
	if (strSaveData.size() > MAX_GUILD_WAR_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ս�洢���ݹ���[size:%d]", __FUNCTION__, strSaveData.size());
		return;
	}

	GuildWarData_Save* pQuery = GuildWarData_Save::ALLOC();
	pQuery->SetIndex(DB_GUILD_WAR_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	pQuery->SetJoinRoleNum(m_RoleGuildWarDataList.size());
	pQuery->SetStarNum(dwStarNum);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);

	DWORD dwCostTime = g_GetCurrentTime() - lCurTime;
	if (dwCostTime){
		MAPLOG(ERROR_LEVEL, "[%s]ϵ�л�����ս��������(%d)����!", __FUNCTION__, dwCostTime);
	}
}

// ��������
void CGuildWarMgr::LoadDataFromDB(const char *pszData, int nSize)
{
	m_RoleGuildWarDataList.clear();
	m_StarInfoList.clear();

	PB::GuildWarData PBGuildWarData;
	if (!PBGuildWarData.ParseFromArray(pszData, nSize))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ϵ�л�����ս���ݳ���!", __FUNCTION__);
		return;
	}
	
	// ��ɫ����ս����
	//RoleGuildWarData tRoleGuildWarData;
	for (int i = 0; i < PBGuildWarData.role_guild_war_data_list_size(); ++i)
	{
		const ::PB::RoleGuildWarData &tPBRoleGuildWarData = PBGuildWarData.role_guild_war_data_list(i);
		RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(tPBRoleGuildWarData.role_id());
		tRoleGuildWarData.wBuyHighStarBattleTimes = tPBRoleGuildWarData.buy_high_star_battle_times();
		tRoleGuildWarData.wUsedLowStarBattleTimes = tPBRoleGuildWarData.used_low_star_battle_times();
		tRoleGuildWarData.wUsedHighStarBattleTimes = tPBRoleGuildWarData.used_high_star_battle_times();
	}

	// �����б�
	for (int i = 0; i < PBGuildWarData.star_list_size(); ++i)
	{
		const ::PB::StarInfo &tPBStarInfo = PBGuildWarData.star_list(i);
		StarInfo tStarInfo;
		DWORD dwRegionId = tPBStarInfo.region_id();
		tStarInfo.dwInfoId = tPBStarInfo.info_id();
		tStarInfo.dwDelongGuildId = tPBStarInfo.belong_guild_id();
		tStarInfo.dwAttackGuildId = tPBStarInfo.attack_guild_id();
		tStarInfo.dwLastDelongGuildId = tPBStarInfo.last_belong_guild_id();

		// ������ȡ��¼
		for (int k = 0; k < tPBStarInfo.took_reward_record_list_size(); ++k){
			DWORD dwRoleId = tPBStarInfo.took_reward_record_list(k);
			tStarInfo.mRewardRecordList.insert(make_pair(dwRoleId, dwRoleId));
		}

		// �����б�
		for (int k = 0; k < tPBStarInfo.score_list_size(); ++k){
			const PB::GuildWarScore &tPBScoreData = tPBStarInfo.score_list(k);
			tStarInfo.mScoreList.insert(make_pair(tPBScoreData.guild_id(), tPBScoreData.score()));
		}

		// ��ս�б�
		for (int k = 0; k < tPBStarInfo.declare_war_list_size(); ++k){
			DWORD dwGuildId = tPBStarInfo.declare_war_list(k);
			tStarInfo.mDeclareWarList.insert(make_pair(dwGuildId, dwGuildId));
		}

		// ���б�
		for (int k = 0; k < tPBStarInfo.tower_list_size(); ++k){
			const PB::StarTowerInfo &tPBStarTower = tPBStarInfo.tower_list(k);
			StarTowerInfo tTower;
			tTower.wTowerId = tPBStarTower.tower_id();
			tTower.dwDurability = tPBStarTower.durability();
			tTower.wMonsterBeAttackTimes = tPBStarTower.monster_be_attack_times();
			for (int m = 0; m < tPBStarTower.defender_list_size(); ++m){
				const PB::StarDefenderInfo &tPBDefender = tPBStarTower.defender_list(m);
				StarDefenderInfo tDefender;
				tDefender.dwRoleId = tPBDefender.role_id();
				tDefender.wBeAttackTimes = tPBDefender.be_attack_times();
				tTower.vDefenderList.push_back(tDefender);
			}
			tStarInfo.vTowerList.push_back(tTower);
		}

		AddStarInfo(dwRegionId, tStarInfo);

		if (tStarInfo.dwDelongGuildId){
			AddOccupyStar(tStarInfo.dwDelongGuildId, tStarInfo.dwInfoId);
		}
	}


	m_bIsLoadDataFromDB = true;
	m_dwSaveTimer = time(NULL);
}

// �Ƿ��к��
bool CGuildWarMgr::IsHaveRedPoint(Player *pPlayer)
{
	return true;
}

// ռ������
void CGuildWarMgr::OnOccupyStarForGM(Player *pPlayer, WORD wStarId)
{
	// ����Ƿ��й���
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ�����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// ����Ƿ��д�����
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
	if (!pStarCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�鵰,����ɶID��[wStarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	WORD wRegionId = GetRegionId(pPlayer->GetServerId());

	StarInfo &tStarInfo = GetStarInfoByRegionId(wRegionId, wStarId);

	// ��ǰ����ռ��,���˽���,������ս,���ܻ��
	if (tStarInfo.dwAttackGuildId || tStarInfo.dwAttackGuildId || tStarInfo.mScoreList.size() || tStarInfo.mDeclareWarList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ����ռ������˽�������������ս[wStarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	AddOccupyStar(dwGuildId, tStarInfo.dwInfoId);

	// ����������Ϣ
	ResetStarInfo(tStarInfo, dwGuildId, true);
}

// ս����ʼ(����ģʽ)
bool CGuildWarMgr::OnNetLowStarBattleStart(Player *pPlayer, DWORD dwStarId)
{
	// ����Ƿ�ʱ��
	if (!IsLowStarBattleTime())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���ǻ���������սʱ��[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// todo::�����Ҫ�ĵȼ�

	// ����Ƿ��й���
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ�����,������ս����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// ȡ��������
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(dwStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// ȡ��������
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	// �����ս����
	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	if (tRoleGuildWarData.wUsedLowStarBattleTimes >= GetConfigValue(1))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ͼ�������ս�����Ѵ�����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	pPlayer->GetFubenManager()->SetTargetId(dwStarId);

	return true;
}

// ս������(����ģʽ)
bool CGuildWarMgr::OnNetLowStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate)
{
	// ����Ƿ��й���
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ�����,������ս����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	DWORD dwStarId = pPlayer->GetFubenManager()->GetTargetId();
	if (!dwStarId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������ս��Ŀ������IDΪ��[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// ȡ��������
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(dwStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// ȡ��������
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	DWORD dwScore = 0;

	// �������
	if (byIsWin)
	{
		// սʤ��û���=20+math.floor(ʣ��Ѫ���ٷֱ�*20)
		dwScore = 20 + floor(double(nRemainHPRate * 0.01) * 20);

		pPlayer->AddObjectList(pStarCfg->vWinnerRewardList, TRUE, FROM_GUILD_WAR_SCORE_STAR_BATTLE);
	}
	else
	{
		// ս�ܻ�û���=20-math.floor(ʣ��Ѫ���ٷֱ�*10)
		dwScore = 20 - floor(double(nRemainHPRate * 0.01) * 10);

		pPlayer->AddObjectList(pStarCfg->vLoserRewardList, TRUE, FROM_GUILD_WAR_SCORE_STAR_BATTLE);
	}

	AddGuildScore(tStarInfo, pGuild->GetGuildId(), dwScore);

	pPlayer->GetFubenManager()->SetParam(4, dwScore);

	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	tRoleGuildWarData.wUsedLowStarBattleTimes++;

	return true;
}

// ս����ʼ(����սģʽ)
bool CGuildWarMgr::OnNetHighStarBattleStart(Player *pPlayer, DWORD dwStarId, WORD wTowerId, DWORD dwTargetId)
{
	// ��⵱ǰ�Ƿ�ս��ʱ��
	if (!CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ����ս��ʱ��!", __FUNCTION__);
		return false;
	}

	// ����Ƿ��й���
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ�����,������ս����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// �����ҿ���ս����
	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	if (tRoleGuildWarData.wUsedHighStarBattleTimes >= GetConfigValue(1) + tRoleGuildWarData.wBuyHighStarBattleTimes)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�߼�������ս�����Ѵ�����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// ȡ��������
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	// ����Ƿ��ý���Ȩ
	if (tStarInfo.dwAttackGuildId != dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]û�л�ý���Ȩ[StarId:%d,AttackGuildId:%d]!", __FUNCTION__, dwStarId, tStarInfo.dwAttackGuildId);
		return false;
	}

	// ȡ��������
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(dwStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// ȡ����Ϣ
	StarTowerInfo *pTowerInfo = GetTowerInfo(&tStarInfo, wTowerId);
	if (!pTowerInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���Ǳ��������[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// ȡ������
	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(wTowerId);
	if (!pTowerCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// ���Ƿ�����
	if (IsTowerBreak(tStarInfo.dwDelongGuildId, pTowerInfo))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�������ѵ���[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// ��⵱ǰ�ܷ񹥻�����
	if (pTowerCfg->wTowerType == STAR_TOWER_1 && !IsCanAttackBoss(&tStarInfo))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ���ܹ�������[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// �Ѿ�����ռ��
	if (tStarInfo.dwDelongGuildId)
	{ ///// ��ս��ɫ
		
		StarDefenderInfo *pDefender = GetDefenderInfo(pTowerInfo, dwTargetId);
		if (!pDefender)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������Ϣ[StarId:%d,TowerId:%d,Defender:%d]!", 
				__FUNCTION__, dwStarId, wTowerId, dwTargetId);
			return false;
		}

		// ���Ŀ��ķ��ش���
		if (pDefender->wBeAttackTimes >= pTowerCfg->wRoleDefenseTimes)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]��������ſ��,Ī��ʬ������[StarId:%d,TowerId:%d,Defender:%d]!",
				__FUNCTION__, dwStarId, wTowerId, dwTargetId);
			return false;
		}

		// ���ͷ�������Ϣ���ͻ���
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// ����ս��
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}
	else
	{ //// ��ս����
		
		// todo::��ʱ����Ҫ���� pass
	}

	// ��¼��ʱ����
	pFBMgr->SetParam(0, dwStarId);
	pFBMgr->SetParam(1, wTowerId);
	pFBMgr->SetParam(2, dwTargetId);

	return true;
}

// ս������(����սģʽ)
bool CGuildWarMgr::OnNetHighStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate)
{
	// ���ݺϷ��Լ��
	if (nRemainHPRate > 100)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ʣ��Ѫ���ٷֱȷǷ�[RoleId:%d, RemainHPRate:%d]!", 
			__FUNCTION__, pPlayer->GetRoleId(), nRemainHPRate);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	DWORD dwStarId = pFBMgr->GetParam(0);
	WORD wTowerId = pFBMgr->GetParam(1);
	DWORD dwTargetId = pFBMgr->GetParam(2);

	// ����Ƿ��й���
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ�����,������ս����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	DWORD dwGuildId = pGuild->GetGuildId();

	// ȡ��������
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	// ����Ƿ��ý���Ȩ
	if (tStarInfo.dwAttackGuildId != dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]û�л�ý���Ȩ[StarId:%d,AttackGuildId:%d]!", __FUNCTION__, dwStarId, tStarInfo.dwAttackGuildId);
		return false;
	}

	// ȡ����Ϣ
	StarTowerInfo *pTowerInfo = GetTowerInfo(&tStarInfo, wTowerId);
	if (!pTowerInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���Ǳ��������[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// ȡ������
	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(wTowerId);
	if (!pTowerCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	bool bIsBreak = IsTowerBreak(tStarInfo.dwDelongGuildId, pTowerInfo);

	DWORD dwDurability = 0;
	if (byIsWin)
	{
		// ��ֹ������ϱ�֮ǰ�ѱ������˸ɵ�
		if (!bIsBreak)
		{
			// �Ѿ�����ռ��
			if (tStarInfo.dwDelongGuildId)
			{ ///// ��ս��ɫ
				StarDefenderInfo *pDefender = GetDefenderInfo(pTowerInfo, dwTargetId);
				if (pDefender)
					pDefender->wBeAttackTimes++;
			}
			else
			{ //// ��ս����
				pTowerInfo->wMonsterBeAttackTimes++;
			}
		}

		// սʤ�;öȿ۳���ʽ��50+math.floor(ʣ��Ѫ���ٷֱ�*50) floor:����ȡ��
		dwDurability = 50 + floor(double(nRemainHPRate * 0.01) * 50);

		// ������ս��ʤ����
		pPlayer->AddObjectList(pTowerCfg->vWinnerRewardList, TRUE, FROM_GUILD_WAR_VS_STAR_BATTLE);
	}
	else
	{
		// ս���;öȿ۳���ʽ��50-math.floor(ʣ��Ѫ���ٷֱ�*40)	
		dwDurability = 50 - floor(double(nRemainHPRate * 0.01) * 40);

		// ������սʧ�ܽ���
		pPlayer->AddObjectList(pTowerCfg->vLoserRewardList, TRUE, FROM_GUILD_WAR_VS_STAR_BATTLE);
	}

	// �����;ö�
	pTowerInfo->dwDurability >= dwDurability ? pTowerInfo->dwDurability -= dwDurability : pTowerInfo->dwDurability = 0;

	MAPLOG(DEBUG_LEVEL, "[%s]�;ö�[CurDurability:%d,ChangeDurability:%d]!", __FUNCTION__, pTowerInfo->dwDurability, dwDurability);

	// �����û�Ծ��
	//pGuild->AddActive(dwDurability);

	// ������Ƿ񱻸ɵ�
	if (!bIsBreak && IsTowerBreak(tStarInfo.dwDelongGuildId, pTowerInfo))
	{
		// ÿ����һ·���ߺ󹥻���ȫ����Ա�ɻ�ö��⽱��(ͨ���������ķ���)
		BreakTowerReward(dwStarId, pTowerInfo, tStarInfo.dwAttackGuildId);

		// ����ǻ�����ռ������, ��ս����
		if (pTowerCfg->wTowerType == STAR_TOWER_1)
		{
			DWORD dwOldDelongGuildId = tStarInfo.dwDelongGuildId;

			// ���������
			HighStarActivtiyOverReward(&tStarInfo, true);

			RemoveOccupyStar(tStarInfo.dwDelongGuildId, tStarInfo.dwInfoId);
			AddOccupyStar(dwGuildId, tStarInfo.dwInfoId);

			// ����������Ϣ
			ResetStarInfo(tStarInfo, dwGuildId, true);

			// ��¼�ϴ�ռ����(��������˾ͼ�¼����ID,��������־ͼ�¼����ID)
			tStarInfo.dwLastDelongGuildId = dwOldDelongGuildId ? dwOldDelongGuildId : 999;

			MAPLOG(ERROR_LEVEL, "[%s]��ս���������[StarId:%d, OldDelongGuildId:%d, NewDelongGuildId:%d, RoleId:%d]!", 
				__FUNCTION__, tStarInfo.dwInfoId, dwOldDelongGuildId, dwGuildId, dwGuildId);
		}
	}

	pFBMgr->SetParam(4,dwDurability);

	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	tRoleGuildWarData.wUsedHighStarBattleTimes++;

	// ֪ͨ���ų�Ա�������ݸ���
	MSG_S2C_GUILD_WAR_STAR_UPDATE_NOTIFY msg;
	msg.m_wStarId = dwStarId;
	msg.m_wTowerId = wTowerId;
	pGuild->SendMsgToAllMember(&msg);

	// ������˻������а� todo::��ʱ��֪����ʽ
	// CRankManager::Instance()->AddRoleRankData(RANK_GUILD_WAR, pPlayer, 1);

	// todo::��Ӧ���������ų�Ա

	return true;
}

// ���󹤻�ս����
void CGuildWarMgr::OnNetGuildWarData(Player *pPlayer)
{
	SendStarList(pPlayer);

	SendOccupyStarList(pPlayer);

	SendGuildWarRoleData(pPlayer);
}

// ���󱾾�����ռ�������
void CGuildWarMgr::OnNetGuildOccupyStarList(Player *pPlayer)
{
	SendOccupyStarList(pPlayer);
}

// ���󱾾�������ս�������б�
void CGuildWarMgr::OnNetGuildDeclareWarStarList(Player *pPlayer)
{
	SendDeclareWarStarList(pPlayer);
}

// ���������б�
void CGuildWarMgr::OnNetStarList(Player *pPlayer)
{
	SendStarList(pPlayer);
}

// ��ս
void CGuildWarMgr::OnNetDeclareWar(Player *pPlayer, DWORD dwStarId)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// ����Ƿ���սʱ��
	if (!CheckHighStarActivityStatus(VSSTAR_ACTIVITY_DECLARE_WAR))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ǰ������սʱ��!", __FUNCTION__);
		return;
	}

	// �Ƿ��й���
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,������ս[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// �Ƿ�᳤�����߸��᳤
	BYTE byMemberPos = pGuild->GetMemberPos(dwRoleId);
	if (byMemberPos != GUILD_POS_CAPTAIN && byMemberPos != GUILD_POS_ASSIST)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û��Ȩ����ս[RoleId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	WORD wRegionId = GetRegionId(pPlayer->GetServerId());

	// ������ս��������
	WORD wDeclareWarNum = GetAlreadyDeclareWarNum(wRegionId, pGuild->GetGuildId());
	if (wDeclareWarNum >= GetConfigValue(2))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ս���������Ѵ�����[DeclareWarNum:%d]!", __FUNCTION__, wDeclareWarNum);
		return;
	}

	// ���Ŀ��ɽ�����ս��������
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);
	if (tStarInfo.mDeclareWarList.size() >= GetConfigValue(3))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]Ŀ������ɽ�����ս�����Ѵ�����[StarId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	// ���Ŀǰ��ռ�������
	WORD wOccupyStarNum = GetOccupyHighStarNum(pGuild->GetServerId(), pGuild->GetGuildId());
	if (wOccupyStarNum >= GetConfigValue(13))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����ռ��������Ѵ�����,��������ս��[StarId:%d,OccupyStarNum:%d]!", __FUNCTION__, dwRoleId, wOccupyStarNum);
		return;
	}

	// ����Ƿ��Լ�������
	if (pGuild->GetGuildId() == tStarInfo.dwDelongGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���ܶ��Լ�ռ���������ս[StarId:%d]!", __FUNCTION__, tStarInfo.dwInfoId);
		return;
	}

	// ����Ƿ��ظ���ս
	if (tStarInfo.mDeclareWarList.find(dwStarId) != tStarInfo.mDeclareWarList.end())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ظ���ͬһ������ս[StarId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	tStarInfo.mDeclareWarList.insert(make_pair(pGuild->GetGuildId(), pGuild->GetGuildId()));

	// ��Ӧ
	MSG_S2C_GUILD_WAR_DECLARE_WAR_RESP msg;
	msg.m_wStarId = dwStarId;
	pPlayer->SendPacket(&msg);
}

// ����
void CGuildWarMgr::OnNetOrganizeDefence(Player *pPlayer, WORD wStarId, WORD wTowerType, DWORD dwMemberId, BYTE byIsToBattle)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// ս���в��ܳ���
	if (CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ս���в��ܲ���[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// �Ƿ��й���
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,���ܳ���[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// �Ƿ�᳤�����߸��᳤
	BYTE byMemberPos = pGuild->GetMemberPos(dwRoleId);
	if (byMemberPos != GUILD_POS_CAPTAIN && byMemberPos != GUILD_POS_ASSIST)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û��Ȩ�޳���[RoleId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	// ȡ��������
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	// ֻ�и߼�������ܲ���
	if (!IsHighStar(pStarCfg->wStarType))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ͼ������ܲ���[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);

	// ����Ƿ������Ĺ���
	if (tStarInfo.dwDelongGuildId != pGuild->GetGuildId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���Ǳ���������������[GuildId:%d,StarId:%d]!", __FUNCTION__, pGuild->GetGuildId(), wStarId);
		return;
	}

	// ����
	if (byIsToBattle)
	{
		// ����Ա�Ƿ��ѷ�������������
		if (IsInDefense(&tStarInfo, dwMemberId))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]��������ڷ�����[StarId:%d,RoleId:%d]!", __FUNCTION__, wStarId, dwMemberId);
			return;
		}

		// ȡҪ���ص�������
		StarTowerInfo *pTowerInfo = GetTowerInfoByType(&tStarInfo, wTowerType);
		if (!pTowerInfo)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[StarId:%d,TowerType:%d]!", __FUNCTION__, wStarId, wTowerType);
			return;
		}

		// ȡ������
		const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pTowerInfo->wTowerId);
		if (!pTowerCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������������[StarId:%d,TowerId:%d]!", __FUNCTION__, wStarId, pTowerInfo->wTowerId);
			return;
		}

		// ��������������
		if (pTowerInfo->vDefenderList.size() >= pTowerCfg->wDefenderNumMax)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]�����������Ѵ�����[StarId:%d,TowerId:%d,Persons:%d]!", 
				__FUNCTION__, wStarId, pTowerInfo->wTowerId, pTowerInfo->vDefenderList.size());
			return;
		}

		// �����Ӧ��������
		StarDefenderInfo tDefender;
		tDefender.dwRoleId = dwMemberId;
		tDefender.wBeAttackTimes = 0;
		pTowerInfo->vDefenderList.push_back(tDefender);

// 		vector<StarTowerInfo>::iterator iter = tStarInfo.vTowerList.begin();
// 		for (; iter != tStarInfo.vTowerList.end(); ++iter)
// 		{
// 			if (GetTowerType(iter->wTowerId) == wTowerType)
// 			{
// 				StarDefenderInfo tDefender;
// 				tDefender.dwRoleId = dwMemberId;
// 				tDefender.wBeAttackTimes = 0;
// 				iter->vDefenderList.push_back(tDefender);
// 				break;
// 			}
// 		}
	}
	// ����
	else
	{
		// ����ǻ���,������Ҫ����һ����
		if (STAR_TOWER_1 == wTowerType)
		{
			WORD wCurDefenseNum = GetCurTowerDefenseNum(&tStarInfo, wTowerType);
			if (wCurDefenseNum <= 1)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]���ط�������Ҫ����һ��[StarId:%d,TowerType:%d,CurDefenseNum:%d]!",
					__FUNCTION__, wStarId, wTowerType, wCurDefenseNum);
				return;
			}
		}

		vector<StarTowerInfo>::iterator iter = tStarInfo.vTowerList.begin();
		for (; iter != tStarInfo.vTowerList.end(); ++iter)
		{
			if (GetTowerType(iter->wTowerId) == wTowerType)
			{
				for (size_t i = 0; i < iter->vDefenderList.size(); i++)
				{
					if (iter->vDefenderList[i].dwRoleId == dwMemberId)
					{
						// ��ǰλ�ñ����λ�õĸ���,���õ����λ�õ�����
						iter->vDefenderList[i] = iter->vDefenderList[iter->vDefenderList.size() - 1];
						iter->vDefenderList.pop_back();
						break;
					}
				}
				break;
			}
		}
	}

	MSG_S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP msg;
	msg.m_wStarId = wStarId;
	msg.m_wTowerType = wTowerType;
	msg.m_dwRoleId = dwMemberId;
	msg.m_byIsToBattle = byIsToBattle;
	pPlayer->SendPacket(&msg);
}

// ����
void CGuildWarMgr::OnNetEvacuate(Player *pPlayer, WORD wStarId)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// ս���в��ܳ���
	if (CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ս���в��ܳ�������[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// �Ƿ��й���
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,���ܳ���[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// �Ƿ�᳤�����߸��᳤
	BYTE byMemberPos = pGuild->GetMemberPos(dwRoleId);
	if (byMemberPos != GUILD_POS_CAPTAIN && byMemberPos != GUILD_POS_ASSIST)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û��Ȩ�޳���[RoleId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	// ȡ��������
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	// ���������ܳ���
	if (pStarCfg->wStarType <= GUILD_WAR_STAR_BLUE)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������ܳ���[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);

	// ����Ƿ������Ĺ���
	if (tStarInfo.dwDelongGuildId != pGuild->GetGuildId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���Ǳ���������������[GuildId:%d,StarId:%d]!", __FUNCTION__, pGuild->GetGuildId(), wStarId);
		return;
	}

	RemoveOccupyStar(tStarInfo.dwDelongGuildId, tStarInfo.dwInfoId);

	// �黹����
	ResetStarInfo(tStarInfo, 0, false);

	// ��Ӧ
	MSG_S2C_GUILD_WAR_EVACUATE_RESP msg;
	msg.m_wStarId = wStarId;
	pPlayer->SendPacket(&msg);
}

// ����Կ�ģʽ��ս����
void CGuildWarMgr::OnNetBuyHighStarBattleTimes(Player *pPlayer)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// �Ƿ��й���
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(dwRoleId);
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,���ܹ���������ս����[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());

	// ��⹺���������
	if (tRoleGuildWarData.wBuyHighStarBattleTimes >= GetConfigValue(12))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����߼�������ս�����Ѵ�����[RoleId:%d,AlreadyBuyTimes:%d]!", 
			__FUNCTION__, dwRoleId, tRoleGuildWarData.wBuyHighStarBattleTimes);
		return;
	}

	// �������
	int nCostDiamond = GetConfigValue(10);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����߼�������ս����������ʯ����[CostDiamond:%d]!", __FUNCTION__, nCostDiamond);
		return;
	}

	// ��������
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_BUY_GUILD_WAR_BATTLE_TIMES);

	// �����ս����
	tRoleGuildWarData.wBuyHighStarBattleTimes++;

	// ��Ӧ
	MSG_S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP msg;
	msg.m_wCurBuyTimes = tRoleGuildWarData.wBuyHighStarBattleTimes;
	pPlayer->SendPacket(&msg);
}

// ��ȡ������
void CGuildWarMgr::OnNetTakeStarReward(Player *pPlayer, WORD wTakeType, WORD *pStarList, WORD wStarNum)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// �Ƿ��й���
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(dwRoleId);
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ���,������ȡ����������[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	for (WORD i = 0; i < wStarNum; ++i)
	{
		WORD wStarId = pStarList[i];

		// ȡ��������
		const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
		if (!pStarCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, wStarId);
			continue;
		}

		StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);

		// ����Ƿ�����ȡ��
		if (tStarInfo.mRewardRecordList.find(dwRoleId) != tStarInfo.mRewardRecordList.end())
		{
			MAPLOG(ERROR_LEVEL, "[%s]����Ľ�������ȡ��[RoleId:%d,GuildId:%d,StarId:%d]!",
				__FUNCTION__, dwRoleId, dwGuildId, wStarId);
			continue;
		}
		
		// ����Ƿ����ڱ����������
		if (tStarInfo.dwDelongGuildId != dwGuildId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��������������ڹ����[RoleId:%d,RoleGuildId:%d,StarId:%d,StarBelongGuildId;%d]!",
				__FUNCTION__, dwRoleId, dwGuildId, wStarId, tStarInfo.dwDelongGuildId);
			continue;
		}

		// �ͼ�����
		if (IsLowStar(pStarCfg->wStarType))
		{
			// ����Ƿ���ȡ����ʱ��
			if (!IsLowStarRewardTime())
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]��ǰ������ȡ����������ʱ��[RoleId:%d,StarId:%d]!", __FUNCTION__, dwRoleId, wStarId);
				continue;
			}

			//  ��ӽ���
			pPlayer->AddObjectList(pStarCfg->vLocalProductList, TRUE, FROM_GUILD_WAR_TAKE_STAR_REWARD);
		}
		// �߼�����
		else
		{
			// ����Ƿ���ȡ����ʱ��
			if (!CheckHighStarActivityStatus(VSSTAR_ACTIVITY_EXPLOIT))
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]��ǰ������ȡ�Կ�������ʱ��[RoleId:%d,StarId:%d]!", __FUNCTION__, dwRoleId, wStarId);
				continue;
			}

			//  ��ӽ���
			pPlayer->AddObjectList(pStarCfg->vDailyRewardList, TRUE, FROM_GUILD_WAR_TAKE_STAR_REWARD);
		}

		// ��¼����ȡ
		tStarInfo.mRewardRecordList.insert(make_pair(dwRoleId, dwRoleId));
	}

	// ��Ӧ
	MSG_S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP msg;
	msg.m_wTakeType = wTakeType;
	msg.m_wStarNum = wStarNum;
	memcpy_s(msg.m_arStarList, sizeof(msg.m_arStarList), pStarList, sizeof(WORD)*wStarNum);
	pPlayer->SendPacket(&msg);
}

// ����������ϸ
void CGuildWarMgr::OnNetStarDetail(Player *pPlayer, WORD wStarId)
{
	MSG_S2C_GUILD_WAR_STAR_DETAIL_RESP msg;
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);
	msg.m_wStarId = tStarInfo.dwInfoId;
	BYTE byIsBattleOver = tStarInfo.dwLastDelongGuildId ? true : false;

	MAPLOG(SPECAIL_LEVEL, "[%s]��������Ϣ[StarId:%d,LastDelongGuildId:%d,RoleId:%d,IsBattleOver:%d]!",
		__FUNCTION__, tStarInfo.dwInfoId, tStarInfo.dwLastDelongGuildId, pPlayer->GetRoleId(), byIsBattleOver);


	msg.m_byIsBattleOver = byIsBattleOver;
	memset(&msg.m_tBelongGuildData, 0, sizeof(msg.m_tBelongGuildData));
	memset(&msg.m_tAttackGuildData, 0, sizeof(msg.m_tAttackGuildData));
	{
		CGuild *pBelongGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwDelongGuildId);
		if (pBelongGuild)
		{
			StarBelongGuildData &tBelongGuild = msg.m_tBelongGuildData;
			tBelongGuild.dwGuildId = pBelongGuild->GetGuildId();
			tBelongGuild.dwActive = pBelongGuild->GetActive();
			tBelongGuild.wServerId = pBelongGuild->GetServerId();
			tBelongGuild.wIconId = pBelongGuild->GetGuildIconId();
			strcpy_s(tBelongGuild.szGuildName, sizeof(tBelongGuild.szGuildName), pBelongGuild->GetGuildName());
		}
	}
	{
		// ս�����������һ��û����
		CGuild *pAttackGuild = NULL;
		if (byIsBattleOver && tStarInfo.dwLastDelongGuildId != 999) // 999=��ʾ�ӹ���������������
		{
			pAttackGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwLastDelongGuildId);
		}
		else if (tStarInfo.dwAttackGuildId)
		{
			pAttackGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwAttackGuildId);
		}

		//CGuild *pAttackGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwAttackGuildId);
		if (pAttackGuild)
		{
			StarAttackGuildData &tAttackGuild = msg.m_tAttackGuildData;
			tAttackGuild.dwGuildId = pAttackGuild->GetGuildId();
			tAttackGuild.dwActive = pAttackGuild->GetActive();
			tAttackGuild.wServerId = pAttackGuild->GetServerId();
			tAttackGuild.wIconId = pAttackGuild->GetGuildIconId();
			strcpy_s(tAttackGuild.szGuildName, sizeof(tAttackGuild.szGuildName), pAttackGuild->GetGuildName());
		}
	}

	map<DWORD, DWORD> *pGuildList = NULL;
	bool bIsScore = false;

	// ��ս�����б�
	if (tStarInfo.mDeclareWarList.size()){
		pGuildList = &tStarInfo.mDeclareWarList;
	}
	// ��ս�����б�
	else if (tStarInfo.mScoreList.size()){
		pGuildList = &tStarInfo.mScoreList;
		bIsScore = true;
	}

	if (pGuildList)
	{
		CGuild *pGuild;
		GuildWarStarBaseData tGuildData;
		map<DWORD, DWORD>::iterator iter = pGuildList->begin();
		for (; iter != pGuildList->end(); ++iter)
		{
			pGuild = CGuildManager::Instance()->GetGuildById(iter->first);
			if (pGuild)
			{
				tGuildData.dwContext = bIsScore ? iter->second : pGuild->GetActive();
				tGuildData.wServerId = pGuild->GetServerId();
				tGuildData.wIconId = pGuild->GetGuildIconId();
				strcpy_s(tGuildData.szGuildName, sizeof(tGuildData.szGuildName), pGuild->GetGuildName());
				msg.m_vDeclareWarGuildList.push_back(tGuildData);
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// �������б�
void CGuildWarMgr::OnNetTowerList(Player *pPlayer, DWORD dwStarId)
{
	WORD wServerId = pPlayer->GetServerId();
	StarInfo *pStarInfo = GetStarInfoEx(wServerId, dwStarId);
	if (!pStarInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[server_id:%d,StarId:%d]!", __FUNCTION__, wServerId, dwStarId);
		return;
	}

	MSG_S2C_GUILD_WAR_TOWER_LIST_RESP msg;
	msg.m_wServerId = wServerId;
	msg.m_wStarId = dwStarId;

	GuildWarTowerForClient tTowerData;
	StarDefenderForClient tDefender;
	vector<StarTowerInfo>::iterator iter1 = pStarInfo->vTowerList.begin();
	for (; iter1 != pStarInfo->vTowerList.end(); ++iter1)
	{
		tTowerData.wTowerId = iter1->wTowerId;
		tTowerData.dwDurability = iter1->dwDurability;
		tTowerData.wMonsterBeAttackTimes = iter1->wMonsterBeAttackTimes;
		tTowerData.vDefenderList.clear();
		vector<StarDefenderInfo>::iterator iter2 = iter1->vDefenderList.begin();
		for (; iter2 != iter1->vDefenderList.end(); ++iter2)
		{
			tDefender.dwRoleId = iter2->dwRoleId;
			tDefender.wBeAttackTimes = iter2->wBeAttackTimes;

			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter2->dwRoleId, false);
			if (pBaseData){
				tDefender.wHeadIcon = pBaseData->wHeadIcon;
				tDefender.dwFightingPower = pBaseData->dwFightingPower;
				strcpy_s(tDefender.szRoleName, sizeof(tDefender.szRoleName), pBaseData->szRoleName);
			}

			tTowerData.vDefenderList.push_back(tDefender);
		}
		msg.m_vTowerList.push_back(tTowerData);
	}


	pPlayer->SendPacket(&msg);
}

// ��ɢ�����¼�
void CGuildWarMgr::OnDismissGuild(WORD wServerId, DWORD dwGuildId)
{
	WORD wRegionId = GetRegionId(wServerId);

	// �Լ�ռ���(�黹)
	const map<DWORD, DWORD> *pGuildStarList = GetGuildStarList(dwGuildId);
	if (pGuildStarList)
	{
		map<DWORD, DWORD>::const_iterator iter = pGuildStarList->begin();
		for (; iter != pGuildStarList->end(); ++iter)
		{
			StarInfo &tStarInfo = GetStarInfoByRegionId(wRegionId, dwGuildId);

			// �黹����
			ResetStarInfo(tStarInfo, 0, false);
		}
	}
	m_GuildStarList.erase(dwGuildId);


	// ����ռ���(��״̬)
	map<DWORD, StarInfo> *pStarlist = GetStarList(wRegionId);
	if (pStarlist)
	{
		map<DWORD, StarInfo>::iterator iter = pStarlist->begin();
		for (; iter != pStarlist->end(); ++iter){

			// �������
			iter->second.mDeclareWarList.erase(dwGuildId);

			// ��ջ���
			iter->second.mScoreList.erase(dwGuildId);
		}
	}
}

// �˳������¼�
void CGuildWarMgr::OnLeave(CGuild *pGuild, DWORD dwRoleId)
{
	DWORD dwGuildId = pGuild->GetGuildId();
	const map<DWORD, DWORD> *pGuildStarList = GetGuildStarList(dwGuildId);
	if (!pGuildStarList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ռ�������[GuildId:%d]!", __FUNCTION__, dwGuildId);
		return;
	}

	map<DWORD, DWORD>::const_iterator iter = pGuildStarList->begin();
	for (; iter != pGuildStarList->end(); ++iter)
	{
		StarInfo &tStarInfo = GetStarInfo(pGuild->GetServerId(), iter->first);
			
		// ����Ƿ���������, ���Ƴ�
		bool bIsDefense = false;
		for (int i = 0; i < tStarInfo.vTowerList.size(); ++i)
		{
			StarTowerInfo &tTower = tStarInfo.vTowerList[i];
			for (int k = 0; k < tTower.vDefenderList.size(); ++k)
			{
				if (tTower.vDefenderList[k].dwRoleId == dwRoleId)
				{
					// �Ƴ��˳���
					tTower.vDefenderList[k] = tTower.vDefenderList[tTower.vDefenderList.size() - 1];
					tTower.vDefenderList.pop_back();				
						
					// ����ǻ���,�ҵ�ǰ�����˷���,�Զ�����ǰ������ս����ߵ�������õ����صĲ����б��У�������֮ǰ���л���������������������
					if (!tTower.vDefenderList.size() && STAR_TOWER_1 == GetTowerType(tTower.wTowerId))
					{
						vector<RolePower> vPlayerList;
						pGuild->GetMemeberPowerRankList(vPlayerList);
						if (vPlayerList.size())
						{
							RemoveDefender(tStarInfo, vPlayerList[0].dwRoleId);

							StarDefenderInfo tDefender;
							tDefender.dwRoleId = vPlayerList[0].dwRoleId;
							tDefender.wBeAttackTimes = 0;
							tTower.vDefenderList.push_back(tDefender);
						}

						MAPLOG(SPECAIL_LEVEL, "�������һ������˳�����[GuildId:%d,StarId:%d,RoleId:%d,MemberNum:%d,TopPowerRoleId:%d]!",
							dwGuildId, tStarInfo.dwInfoId, dwRoleId, vPlayerList.size(), vPlayerList[0].dwRoleId);
					}

					MAPLOG(SPECAIL_LEVEL, "����˳�����[GuildId:%d,StarId:%d]!", dwGuildId, tStarInfo.dwInfoId);
						
					bIsDefense = true;
					break;
				}
			}

			if (bIsDefense)
				break;
		}
	}
}

// ȡ�����Ѿ���ս������
WORD CGuildWarMgr::GetAlreadyDeclareWarNum(WORD wRegionId, DWORD dwGuildId)
{
	WORD wNum = 0;
	map<DWORD, StarInfo> *pStarlist = GetStarList(wRegionId);
	if (pStarlist)
	{
		map<DWORD, StarInfo>::iterator iter = pStarlist->begin();
		for (; iter != pStarlist->end(); ++iter)
		{
			if (iter->second.mDeclareWarList.end() != iter->second.mDeclareWarList.find(dwGuildId))
				wNum++;
		}
	}
	return wNum;
}

// ȡռ��߼����������
WORD CGuildWarMgr::GetOccupyHighStarNum(WORD wServerId, DWORD dwGuildId)
{
	WORD wNum = 0;
	const map<DWORD, DWORD> *pGuildStarList = GetGuildStarList(dwGuildId);
	if (pGuildStarList)
	{
		map<DWORD, DWORD>::const_iterator iter = pGuildStarList->begin();
		for (; iter != pGuildStarList->end(); ++iter)
		{
			StarInfo *pStarInfo = GetStarInfoEx(wServerId, dwGuildId);
			if (!pStarInfo)
				continue;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
				continue;

			if (IsHighStar(pStarCfg->wStarType) && pStarInfo->dwDelongGuildId == dwGuildId)
				++wNum;
		}
	}
	return wNum;
}

// ȡ����ID
WORD CGuildWarMgr::GetRegionId(WORD wServerId)
{
	const GuildWarRegion_Config *pRegionCfg = g_pCfgMgr->GetGuildWarRegionByServerId(wServerId);
	return pRegionCfg ? pRegionCfg->wRegionId : 0;
}

// ����Ƿ񹤻������
bool CGuildWarMgr::IsGuildStar(DWORD dwGuildId, DWORD dwStarId)
{
	const map<DWORD, DWORD> *pStarList = GetGuildStarList(dwGuildId);
	if (pStarList && pStarList->find(dwStarId) != pStarList->end())
		return true;

	return false;
}

// �Ƿ����������սʱ��
bool CGuildWarMgr::IsLowStarBattleTime()
{
	const Server_Config *pStarCfg = GetCommonCfg(5);
	const Server_Config *pEndCfg = GetCommonCfg(6);
	if (!pStarCfg || !pEndCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ͼ�����ÿ����սʱ������!", __FUNCTION__);
		return false;
	}

	tm tmTime;
	time_t t = time(NULL);
	localtime_s(&tmTime, &t);

	DWORD dwCurTime = tmTime.tm_hour * 100 + tmTime.tm_min;

	return dwCurTime >= pStarCfg->dwValue && dwCurTime <= pEndCfg->dwValue;
}

// �Ƿ����������ʱ��
bool CGuildWarMgr::IsLowStarRewardTime()
{
	const Server_Config *pStarCfg = GetCommonCfg(7);
	const Server_Config *pEndCfg = GetCommonCfg(8);
	if (!pStarCfg || !pEndCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ͼ�����ÿ�ս���ʱ������!", __FUNCTION__);
		return false;
	}

	tm tmTime;
	time_t t = time(NULL);
	localtime_s(&tmTime, &t);

	DWORD dwCurTime = tmTime.tm_hour * 100 + tmTime.tm_min;

	return dwCurTime >= pStarCfg->dwValue && dwCurTime <= pEndCfg->dwValue;
}




// ���߼�����״̬
bool CGuildWarMgr::CheckHighStarActivityStatus(int nActivityStatus)
{
	const vector<GuildWarVSActivityTime_Config> *pTimeCfg = g_pCfgMgr->GetGuildWarVSActivityTime(nActivityStatus);
	if (!pTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����߼�����ʱ������[TimeType:%d]!", __FUNCTION__, nActivityStatus);
		return VSSTAR_ACTIVITY_CLOSE;
	}

	tm tmTime;
	time_t t = time(NULL);
	localtime_s(&tmTime, &t);
	DWORD dwCurTime = g_CorrectWeekDay(tmTime.tm_wday) * 10000 + tmTime.tm_hour * 100 + tmTime.tm_min;

	// ��ս, ս��
	if (VSSTAR_ACTIVITY_DECLARE_WAR == nActivityStatus || VSSTAR_ACTIVITY_BATTLE == nActivityStatus)
	{
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pTimeCfg->begin();
		for (; iter != pTimeCfg->end(); ++iter)
		{
			if (dwCurTime >= iter->dwBeginTime && dwCurTime <= iter->dwEndTime)
				return true;
		}
	}
	// ����
	else if (VSSTAR_ACTIVITY_EXPLOIT == nActivityStatus)
	{
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pTimeCfg->begin();
		for (; iter != pTimeCfg->end(); ++iter)
		{
			for (int i = 0; i < iter->vTakeRewardTime.size(); ++i)
			{
				if (iter->vTakeRewardTime[i] == g_CorrectWeekDay(tmTime.tm_wday))
					return true;
			}
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]δ����ĸ߼�����ʱ��[TimeType:%d]!", __FUNCTION__, nActivityStatus);
	}

	return false;
}

// ������Ƿ�����
bool CGuildWarMgr::IsTowerBreak(DWORD dwBelongGuildId, const StarTowerInfo *pTowerInfo)
{
	// �;���Ϊ0
	if (!pTowerInfo->dwDurability)
		return true;

	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pTowerInfo->wTowerId);
	if (!pTowerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TowerId:%d]!", __FUNCTION__, pTowerInfo->wTowerId);
		return false;
	}

	// ����
	if (dwBelongGuildId)
	{
		// ���˷���
		if (!pTowerInfo->vDefenderList.size())
			return true;

		for (int i = 0; i < pTowerInfo->vDefenderList.size(); ++i)
		{
			// ���з��ش���
			if (pTowerInfo->vDefenderList[i].wBeAttackTimes < pTowerCfg->wRoleDefenseTimes)
				return false;
		}
	}
	// ����
	else
	{
		// ���з��ش���
		if (pTowerInfo->wMonsterBeAttackTimes < pTowerCfg->wMonsterDefenseTimes)
			return false;
	}

	return true;
}

// ȡ��������
WORD CGuildWarMgr::GetBreakTowerNum(const StarInfo *pStarInfo)
{
	WORD wBreakTowerNum = 0;
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		if (IsTowerBreak(pStarInfo->dwDelongGuildId, &pStarInfo->vTowerList[i]))
			wBreakTowerNum++;
	}
	return wBreakTowerNum;
}

// �Ƿ�ɹ�������
bool CGuildWarMgr::IsCanAttackBoss(const StarInfo *pStarInfo)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		const StarTowerInfo *pTower = &pStarInfo->vTowerList[i];
		const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pTower->wTowerId);
		if (!pTowerCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TowerId:%d]!", __FUNCTION__, pTower->wTowerId);
			return false;
		}

		if (pTowerCfg->wTowerType != STAR_TOWER_1)
		{
			// ֻҪ������һ��������,�Ϳ��Թ���
			if (IsTowerBreak(pStarInfo->dwDelongGuildId, pTower))
				return true;
		}
	}

	return false;
}

// �Ƿ�ͼ�����
bool CGuildWarMgr::IsLowStar(WORD wStarType)
{
	return wStarType <= GUILD_WAR_STAR_BLUE ? true : false;
}

// �Ƿ�߼�����
bool CGuildWarMgr::IsHighStar(WORD wStarType)
{
	return wStarType >= GUILD_WAR_STAR_PURPLE ? true : false;
}

// �Ƿ����ڷ�����
bool CGuildWarMgr::IsInDefense(const StarInfo *pStarInfo, DWORD dwRoleId)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		const StarTowerInfo *pTower = &pStarInfo->vTowerList[i];
		for (int k = 0;  k < pTower->vDefenderList.size(); ++k)
		{
			if (pTower->vDefenderList[k].dwRoleId == dwRoleId)
				return true;
		}
	}
	return false;
}

// ȡ������
WORD CGuildWarMgr::GetTowerType(WORD wTowerId)
{
	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(wTowerId);
	if (!pTowerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[TowerId:%d]!", __FUNCTION__, wTowerId);
		return 0;
	}

	return pTowerCfg->wTowerType;
}

// ȡ��������ǰ��������
WORD CGuildWarMgr::GetCurTowerDefenseNum(const StarInfo *pStarInfo, WORD wTowerType)
{
	vector<StarTowerInfo>::const_iterator iter = pStarInfo->vTowerList.begin();
	for (; iter != pStarInfo->vTowerList.end(); ++iter)
	{
		if (GetTowerType(iter->wTowerId) == wTowerType)
			return iter->vDefenderList.size();
	}

	return 0;
}

// ��ӹ������
void CGuildWarMgr::AddGuildScore(StarInfo &tStarInfo, DWORD dwGuildId, DWORD dwScore)
{
	map<DWORD, DWORD>::iterator iter = tStarInfo.mScoreList.find(dwGuildId);
	if (iter != tStarInfo.mScoreList.end()){
		iter->second += dwScore;
	}
	else{
		tStarInfo.mScoreList.insert(make_pair(dwGuildId, dwScore));
	}
}

// ȡ������Ϣ
StarInfo& CGuildWarMgr::GetStarInfo(WORD wServerId, DWORD dwStarId)
{
	WORD wRegionId = GetRegionId(wServerId);

	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 == m_StarInfoList.end())
	{
		m_StarInfoList.insert(make_pair(wRegionId, map<DWORD, StarInfo>()));
		iter1 = m_StarInfoList.find(wRegionId);
	}

	map<DWORD, StarInfo>::iterator iter2 = iter1->second.find(dwStarId);
	if (iter2 == iter1->second.end())
	{
		StarInfo tStar;
		tStar.dwInfoId = dwStarId;
		ResetStarInfo(tStar, 0, true);
		iter1->second.insert(make_pair(dwStarId, tStar));
		iter2 = iter1->second.find(dwStarId);
	}

	return iter2->second;
}

// ȡ������Ϣ(���Ŀǰû����ᴴ��)
StarInfo& CGuildWarMgr::GetStarInfoByRegionId(WORD wRegionId, DWORD dwStarId)
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 == m_StarInfoList.end())
	{
		m_StarInfoList.insert(make_pair(wRegionId, map<DWORD, StarInfo>()));
		iter1 = m_StarInfoList.find(wRegionId);
	}

	map<DWORD, StarInfo>::iterator iter2 = iter1->second.find(dwStarId);
	if (iter2 == iter1->second.end())
	{
		StarInfo tStar;
		tStar.dwInfoId = dwStarId;
		ResetStarInfo(tStar, 0, true);
		iter1->second.insert(make_pair(dwStarId, tStar));
		iter2 = iter1->second.find(dwStarId);
	}

	return iter2->second;
}

// ����������Ϣ
StarInfo* CGuildWarMgr::GetStarInfoEx(WORD wServerId, DWORD dwStarId)
{
	WORD wRegionId = GetRegionId(wServerId);

	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 == m_StarInfoList.end()) return NULL;

	map<DWORD, StarInfo>::iterator iter2 = iter1->second.find(dwStarId);
	return iter2 != iter1->second.end() ? &iter2->second : NULL;
}

// ȡ����Ϣ
StarTowerInfo* CGuildWarMgr::GetTowerInfo(StarInfo *pStarInfo, WORD wTowerId)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		if (pStarInfo->vTowerList[i].wTowerId == wTowerId)
			return &pStarInfo->vTowerList[i];
	}
	return NULL;
}

// ͨ������ȡ����Ϣ
StarTowerInfo* CGuildWarMgr::GetTowerInfoByType(StarInfo *pStarInfo, WORD wTowerType)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		if (GetTowerType(pStarInfo->vTowerList[i].wTowerId) == wTowerType)
			return &pStarInfo->vTowerList[i];
	}
	return NULL;
}

// ȡ��������Ϣ
StarDefenderInfo* CGuildWarMgr::GetDefenderInfo(StarTowerInfo *pTowerInfo, DWORD dwRoleId)
{
	for (int i = 0; i < pTowerInfo->vDefenderList.size(); ++i)
	{
		if (pTowerInfo->vDefenderList[i].dwRoleId == dwRoleId)
			return &pTowerInfo->vDefenderList[i];
	}
	return NULL;
}

// ȡ�����������б�
map<DWORD, StarInfo>* CGuildWarMgr::GetStarList(WORD wAreaId)
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter = m_StarInfoList.find(wAreaId);
	if (iter == m_StarInfoList.end())
	{
		m_StarInfoList.insert(make_pair(wAreaId, map<DWORD, StarInfo>()));
		iter = m_StarInfoList.find(wAreaId);
	}
	return &iter->second;
}

// ȡ����ռ��ĵ������б�
const map<DWORD, DWORD>* CGuildWarMgr::GetGuildStarList(DWORD dwGuildId)
{
	map<DWORD, map<DWORD, DWORD>>::iterator iter = m_GuildStarList.find(dwGuildId);
	return iter != m_GuildStarList.end() ? &iter->second : NULL;
}

// bool RolePowerPred(RolePower &data1, RolePower &data2)
// {
// 	if (data1.dwPower > data2.dwPower)
// 		return true;
// 
// 	if (data1.dwPower == data2.dwPower && data1.dwRoleId < data2.dwRoleId)
// 		return true;
// 
// 	return false;
// }

// ����������Ϣ
void CGuildWarMgr::ResetStarInfo(StarInfo &tStarInfo, DWORD dwBelongGuildId, bool bIsNew)
{
	tStarInfo.dwDelongGuildId = dwBelongGuildId;
	tStarInfo.dwAttackGuildId = 0;
	tStarInfo.dwLastDelongGuildId = 0;
	tStarInfo.mRewardRecordList.clear();
	tStarInfo.mDeclareWarList.clear();

	// ��ռ��
	if (bIsNew)
	{
		/*
		����ռ������ʱ��Ĭ�ϲ������ã�����ս��˳�����
		1.	���أ�1��5~13
		2.	����1��2��14��17
		3.	����2��3��15��18
		4.	����3��4��16��19
		5.	��������ʱ����
		*/

		// ��ȡ��Ա�б���ս����������
		vector<RolePower> vPlayerList;
		CGuild *pGuild = dwBelongGuildId ? CGuildManager::Instance()->GetGuildById(dwBelongGuildId) : NULL;
		if (pGuild){
			pGuild->GetMemeberPowerRankList(vPlayerList);
		}

		const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(tStarInfo.dwInfoId);
		if (!pStarCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������[TowerId:%d]!", __FUNCTION__, tStarInfo.dwInfoId);
			return;
		}
		tStarInfo.vTowerList.clear();
		for (int i = 0; i < pStarCfg->vTowerList.size(); ++i)
		{
			const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pStarCfg->vTowerList[i]);
			if (!pTowerCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ս��������[TowerId:%d]!", __FUNCTION__, pStarCfg->vTowerList[i]);
				continue;
			}

			const GuildWarOrganizeDefense_Config *pOrganizeDefCfg = g_pCfgMgr->GetGuildWarOrganizeDefense(pTowerCfg->wTowerType);
			if (!pOrganizeDefCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������ս��������[TowerType:%d]!", __FUNCTION__, pTowerCfg->wTowerType);
				continue;
			}

			StarTowerInfo tTowerInfo;
			tTowerInfo.wTowerId = pTowerCfg->dwTowerId;
			//tTowerInfo.wType = pTowerCfg->wTowerType;
			tTowerInfo.dwDurability = pTowerCfg->dwDurability;
			tTowerInfo.wMonsterBeAttackTimes = 0;
			for (int k = 0; k < pOrganizeDefCfg->vPowerIdxList.size(); ++k)
			{
				DWORD dwIdx = pOrganizeDefCfg->vPowerIdxList[k];
				DWORD dwRoleId = (dwIdx && dwIdx <= vPlayerList.size()) ? vPlayerList[dwIdx-1].dwRoleId : 0;
				if (dwRoleId)
				{
					StarDefenderInfo tDefender;
					tDefender.dwRoleId = dwRoleId;
					tDefender.wBeAttackTimes = 0;
					tTowerInfo.vDefenderList.push_back(tDefender);
				}
				MAPLOG(ERROR_LEVEL, "��Ա����[GuildId:%d,TowerId:%d,RoleId:%d]!", dwBelongGuildId, pTowerCfg->dwTowerId, dwRoleId);
			}
			tStarInfo.vTowerList.push_back(tTowerInfo);
		}
	}
	// ����ռ��(�;öȲ���)
	else
	{
		for (int i = 0; i < tStarInfo.vTowerList.size(); ++i)
		{
			StarTowerInfo &tTowerInfo = tStarInfo.vTowerList[i];
			tTowerInfo.wMonsterBeAttackTimes = 0;
			//tTowerInfo.dwDurability = 0;
			for (int k = 0; k < tTowerInfo.vDefenderList.size(); ++k)
			{
				tTowerInfo.vDefenderList[k].wBeAttackTimes = 0;
			}
		}
	}
}

// ȡ��ҹ���ս����
RoleGuildWarData& CGuildWarMgr::GetRoleGuildWarData(DWORD dwRoleId)
{
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.find(dwRoleId);
	if (iter == m_RoleGuildWarDataList.end())
	{
		RoleGuildWarData data;
		memset(&data, 0, sizeof(data));
		m_RoleGuildWarDataList.insert(make_pair(dwRoleId, data));
		iter = m_RoleGuildWarDataList.find(dwRoleId);
	}
	return iter->second;
}

// ���ռ�������
void CGuildWarMgr::AddOccupyStar(DWORD dwGuildId, WORD wStarId)
{
	if (!dwGuildId)
		return;

	map<DWORD, map<DWORD, DWORD>>::iterator iter = m_GuildStarList.find(dwGuildId);
	if (iter == m_GuildStarList.end())
	{
		m_GuildStarList.insert(make_pair(dwGuildId, map<DWORD, DWORD>()));
		iter = m_GuildStarList.find(dwGuildId);
	}
	iter->second.insert(make_pair(wStarId, wStarId));
}

// ɾ��ռ�������
void CGuildWarMgr::RemoveOccupyStar(DWORD dwGuildId, WORD wStarId)
{
	map<DWORD, map<DWORD, DWORD>>::iterator iter = m_GuildStarList.find(dwGuildId);
	if (iter != m_GuildStarList.end()){
		iter->second.erase(wStarId);
	}
}

// ɾ����ս�Ĺ���
void CGuildWarMgr::RemoveDeclareWarGuild(WORD wRegionId, DWORD dwGuildId)
{
	if (!dwGuildId) return;

	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 != m_StarInfoList.end())
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2){
			iter2->second.mDeclareWarList.erase(dwGuildId);
		}
	}
}

// �����������
void CGuildWarMgr::AddStarInfo(WORD wRegionId, const StarInfo &tStarInfo)
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter = m_StarInfoList.find(wRegionId);
	if (iter == m_StarInfoList.end()){
 		m_StarInfoList.insert(make_pair(wRegionId, map<DWORD, StarInfo>()));
		iter = m_StarInfoList.find(wRegionId);
	}

	iter->second.insert(make_pair(tStarInfo.dwInfoId, tStarInfo));
}

// �Ƴ�������
void CGuildWarMgr::RemoveDefender(StarInfo &tStarInfo, DWORD dwRoleId)
{
	for (int i = 0; i < tStarInfo.vTowerList.size(); ++i)
	{
		StarTowerInfo &tTower = tStarInfo.vTowerList[i];
		for (int k = 0; k < tTower.vDefenderList.size(); ++k)
		{
			if (tTower.vDefenderList[k].dwRoleId == dwRoleId)
			{
				tTower.vDefenderList[k] = tTower.vDefenderList[tTower.vDefenderList.size() - 1];
				tTower.vDefenderList.pop_back();
				return;
			}
		}
	}
}

// ��������
void CGuildWarMgr::BreakTowerReward(WORD wStarId, StarTowerInfo *pTowerInfo, DWORD dwAttackGuildId)
{
	// ͨ���������ķ�����
	CGuild *pGuild = CGuildManager::Instance()->GetGuildById(dwAttackGuildId);
	if (pGuild)
	{
		RewardData tRewardData;
		tRewardData.wInfoId = 31;
		tRewardData.dwTime = time(NULL);
		tRewardData.dwValue1 = wStarId;
		tRewardData.dwValue2 = pTowerInfo->wTowerId;
		pGuild->SendRewardToAllMember(tRewardData);
	}
}

// ���������
void CGuildWarMgr::HighStarActivtiyOverReward(const StarInfo *pStarInfo, BYTE byIsAttackerWin, WORD wBreakTowerNum)
{
	// ������ʤ
	if (byIsAttackerWin)
	{
		// ������ȫ����Ա���սʤ�������������ز�
		CGuild *pAttackGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwAttackGuildId);
		if (pAttackGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 32;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeWord(1, 1);	// 1=��ʾ���� 1=��ʾ��ʤ
			pAttackGuild->SendRewardToAllMember(tRewardData);
		}

		// ���ط�ȫ����Ա��ð�ο����
		CGuild *pDefenseGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwDelongGuildId);
		if (pDefenseGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 35;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeWord(2, 0);	// 2=��ʾ�ط� 0=��ʾʧ��
			pDefenseGuild->SendRewardToAllMember(tRewardData);
		}
	}
	// �ط���ʤ
	else
	{
		// ���ط�ȫ����Ա��÷��سɹ�����
		CGuild *pDefenseGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwDelongGuildId);
		if (pDefenseGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 34;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeWord(2, 1);	// 2=��ʾ�ط� 1=��ʾ��ʤ
			pDefenseGuild->SendRewardToAllMember(tRewardData);
		}

		// ���������ݹ��Ƶķ�����������ð�ο����
		CGuild *pAttackGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwAttackGuildId);
		if (pAttackGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 33;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeDWord(wBreakTowerNum, MakeWord(1, 0));  // 1=��ʾ���� 0=��ʾʧ��
			pAttackGuild->SendRewardToAllMember(tRewardData);
		}
	}
}

// �Կ�������սʱ�����
void CGuildWarMgr::HighStarDeclareWarTimeOver()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������[TowerId:%d]!", __FUNCTION__, pStarInfo->dwInfoId);
				continue;
			}

			// �ͼ����򲻴���
			if (IsLowStar(pStarCfg->wStarType))
				continue;

			// �����Ǹ������ý���Ȩ
			DWORD dwMaxActive = 0;
			DWORD dwActiveGuildId = 0;
			map<DWORD, DWORD>::iterator iter3 = pStarInfo->mDeclareWarList.begin();
			for (; iter3 != pStarInfo->mDeclareWarList.end(); ++iter3)
			{
				CGuild *pGuild = CGuildManager::Instance()->GetGuildById(iter3->first);
				if (!pGuild)
				{
					MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[GuildId:%d]!", __FUNCTION__, iter3->first);
					continue;
				}

				// ȡ��Ծ��ߵĹ���, ��Ծ��ͬ��ȡ���籨����
				if (!dwActiveGuildId || pGuild->GetActive() > dwMaxActive)
				{
					dwMaxActive = pGuild->GetActive();
					dwActiveGuildId = pGuild->GetGuildId();
				}
			}

			// ��������Ȩ�������ս�б�
			pStarInfo->mDeclareWarList.clear();

			// ��¼��ʤ����Ȩ�Ĺ���
			pStarInfo->dwAttackGuildId = dwActiveGuildId;

			MAPLOG(DEBUG_LEVEL, "[%s]����������Ȩ�Ĺ���[StarId:%d, BelongGuilId:%d, GuildId:%d]!", 
				__FUNCTION__, pStarInfo->dwInfoId, pStarInfo->dwDelongGuildId, dwActiveGuildId);


			RemoveDeclareWarGuild(iter1->first, dwActiveGuildId);
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]�߼�������սʱ�����",__FUNCTION__);
}

// �Կ���������
void CGuildWarMgr::HighStarActivityOver()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;
			pStarInfo->dwLastDelongGuildId = 0;

			// û���˽��������⴦��(1.�п����ǻ������� 2.�п����ǻ����ǰ�Ѷ�ʤ��,��Ϊ����ʤ����ʱ���Ѵ���� 3.�п���ȷʵ��ľ������ս)
			if (!pStarInfo->dwAttackGuildId)
			{
				pStarInfo->mRewardRecordList.clear();
				continue;
			}

			// ������
			HighStarActivtiyOverReward(pStarInfo, false, GetBreakTowerNum(pStarInfo));

			// ����
			ResetStarInfo(*pStarInfo, pStarInfo->dwDelongGuildId, false);
		}
	}

	// ���а���
	const AllRoleRankList *pRankList = CRankManager::Instance()->GetRankListEx(RANK_GUILD_WAR);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������а��б�[rankType:%d]!", __FUNCTION__, RANK_GUILD_WAR);
		return;
	}
	AllRoleRankList::const_iterator iter_rank_list = pRankList->begin();
	for (; iter_rank_list != pRankList->end(); ++iter_rank_list)
	{
		RoleRankList::const_iterator iter_rank_data = iter_rank_list->second.begin();
		for (; iter_rank_data != iter_rank_list->second.end(); ++iter_rank_data)
		{
			const GuildWarRankReward_Config *pRewardCfg = g_pCfgMgr->GetGuildWarRankReward(iter_rank_data->wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 36;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = iter_rank_data->wRank;
				CRewardCenterManager::AddReward(iter_rank_data->dwServerId, iter_rank_data->dwRoleId, tRewardData);				
			}
		}
	}

	// ������ս����
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.begin();
	for (; iter != m_RoleGuildWarDataList.end(); ++iter){
		iter->second.wBuyHighStarBattleTimes = 0;
		iter->second.wUsedHighStarBattleTimes = 0;
	}

	MAPLOG(ERROR_LEVEL, "[%s]�߼���������", __FUNCTION__);
}

// ����������ʼ
void CGuildWarMgr::LowStarActivityStart()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������[TowerId:%d]!", __FUNCTION__, pStarInfo->dwInfoId);
				continue;
			}

			// �߼����򲻴���
			if (IsHighStar(pStarCfg->wStarType))
				continue;

			// �����ȡ������¼
			pStarInfo->mRewardRecordList.clear();

			RemoveOccupyStar(pStarInfo->dwDelongGuildId, pStarInfo->dwInfoId);

			// ��ջ�ʤ�߼�¼
			pStarInfo->dwDelongGuildId = 0;
		}
	}
}

// ������������
void CGuildWarMgr::LowStarActivityOver()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������������[TowerId:%d]!", __FUNCTION__, pStarInfo->dwInfoId);
				continue;
			}

			// �߼����򲻴���
			if (IsHighStar(pStarCfg->wStarType))
				continue;

			// ȷ���Ǹ�������ʤ��
			DWORD dwGuildId = 0;
			DWORD dwMaxScore = 0;
			map<DWORD, DWORD>::iterator iter3 = pStarInfo->mScoreList.begin();
			for (; iter3 != pStarInfo->mScoreList.end(); ++iter3)
			{
				// ������ͬʱ���Ǹ������ȴ���
				if (iter3->second > dwMaxScore || (iter3->second == dwMaxScore && iter3->first < dwGuildId))
				{
					dwGuildId = iter3->first;
					dwMaxScore = iter3->second;
				}
			}

			// ѡ����ʤ���Ժ�,��ջ����б�
			pStarInfo->mScoreList.clear();

			// �ⲽ���Ǳ����, ��ֹǰ�����bug
			//pStarInfo->mRewardRecordList.clear();

			// ��¼�Ǹ������ʤ
			pStarInfo->dwDelongGuildId = dwGuildId;

			AddOccupyStar(dwGuildId, pStarInfo->dwInfoId);

			MAPLOG(ERROR_LEVEL, "[%s]�����õĵͼ�����ӵ��Ȩ[GuildId:%d]!", __FUNCTION__, dwGuildId);
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]������������", __FUNCTION__);
}

void CGuildWarMgr::SendGuildWarRoleData(Player *pPlayer)
{
	MSG_S2C_GUILD_WAR_ROLE_DATA_RESP msg;
	RoleGuildWarData &tGuildWarRoleData = GetRoleGuildWarData(pPlayer->GetRoleId());
	msg.m_GuildWarRoleData = tGuildWarRoleData;
	pPlayer->SendPacket(&msg);
}


// ���������б�
void CGuildWarMgr::SendStarList(Player *pPlayer)
{
	MSG_S2C_GUILD_WAR_STAR_LIST_RESP msg;

	bool isBattle = CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE);
	DWORD dwMyGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());

	GuildWarStarBaseDataForClient tStarBaseData;
	WORD wRegionId = GetRegionId(pPlayer->GetServerId());
	map<DWORD, StarInfo> *pStarList = GetStarList(wRegionId);
	if (pStarList)
	{
		map<DWORD, StarInfo>::iterator iter = pStarList->begin();
		for (; iter != pStarList->end(); ++iter)
		{
			StarInfo *pTargetStar = &iter->second;

			memset(&tStarBaseData, 0, sizeof(tStarBaseData));
			tStarBaseData.wStarId = pTargetStar->dwInfoId;
			CGuild *pGuild = CGuildManager::Instance()->GetGuildById(pTargetStar->dwDelongGuildId);
			if (pGuild){
				strcpy_s(tStarBaseData.szGuildName, sizeof(tStarBaseData.szGuildName), pGuild->GetGuildName());
				tStarBaseData.wIconId = pGuild->GetGuildIconId();
			}

			// �Ƿ���ս����(���ҹ�����)
			if (isBattle && dwMyGuildId && ((pTargetStar->dwDelongGuildId == dwMyGuildId && pTargetStar->dwAttackGuildId) || pTargetStar->dwAttackGuildId == dwMyGuildId))
				tStarBaseData.byIsInBattle = true;

			msg.m_vOccupyStarList.push_back(tStarBaseData);
		}
	}

	pPlayer->SendPacket(&msg);
}

// ���͹���ռ��������б�
void CGuildWarMgr::SendOccupyStarList(Player *pPlayer)
{
	// ����Ƿ��й���
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ�����[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP msg;

	WORD wRegionId = GetRegionId(pPlayer->GetServerId());

	BYTE byIsHighStarRewardTime = CheckHighStarActivityStatus(VSSTAR_ACTIVITY_EXPLOIT);
	BYTE byIsLowStarRewardTime = IsLowStarRewardTime();

	GuildWarOccupyStarDataForClient tOccupyStarData;
	const map<DWORD, DWORD> *pStarList = GetGuildStarList(dwGuildId);
	if (pStarList)
	{
		map<DWORD, DWORD>::const_iterator iter = pStarList->begin();
		for (; iter != pStarList->end(); ++iter)
		{
			StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), iter->first);
			tOccupyStarData.wStarId = tStarInfo.dwInfoId;
			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(tOccupyStarData.wStarId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[StarId:%d]!", __FUNCTION__, tOccupyStarData.wStarId);
				continue;
			}
			BYTE byIsHaveReward = false;
			if (IsLowStar(pStarCfg->wStarType) && byIsLowStarRewardTime)
			{
				if (tStarInfo.mRewardRecordList.find(pPlayer->GetRoleId()) == tStarInfo.mRewardRecordList.end())
					byIsHaveReward = true;
			}
			else if (IsHighStar(pStarCfg->wStarType) && byIsHighStarRewardTime)
			{
				if (tStarInfo.mRewardRecordList.find(pPlayer->GetRoleId()) == tStarInfo.mRewardRecordList.end())
					byIsHaveReward = true;
			}
			tOccupyStarData.byIsHaveReward = byIsHaveReward;
			msg.m_vOccupyStarList.push_back(tOccupyStarData);
		}
	}

	pPlayer->SendPacket(&msg);
}

// ���͹�����ս�������б�
void CGuildWarMgr::SendDeclareWarStarList(Player *pPlayer)
{
	MSG_S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP msg;
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	WORD wRegionId = GetRegionId(pPlayer->GetServerId());
	map<DWORD, StarInfo> *pStarList =  GetStarList(wRegionId);
	if (pStarList && dwGuildId)
	{
		GuildWarDeclareWarData tDeclareWarData;
		map<DWORD, StarInfo>::const_iterator iter = pStarList->begin();
		for (; iter != pStarList->end(); ++iter)
		{	
			// �ѶԴ�������ս
			if (iter->second.mDeclareWarList.find(dwGuildId) != iter->second.mDeclareWarList.end())
			{
				StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), iter->first);
				CGuild *pBelongGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwDelongGuildId);
				if (pBelongGuild){
					tDeclareWarData.wServerId = pBelongGuild->GetServerId();
					strcpy_s(tDeclareWarData.szGuildName, sizeof(tDeclareWarData.szGuildName), pBelongGuild->GetGuildName());
					tDeclareWarData.dwActive = pBelongGuild->GetActive();
				}
				else{
					memset(&tDeclareWarData, 0, sizeof(tDeclareWarData));
				}
				tDeclareWarData.wStarId = tStarInfo.dwInfoId;
				msg.m_vDeclareWarStarList.push_back(tDeclareWarData);
			}
		}
	}
	pPlayer->SendPacket(&msg);
}

// ȡͨ������
const Server_Config*  CGuildWarMgr::GetCommonCfg(DWORD dwKey, bool bOutLog)
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetGuildWarCommon(dwKey);
	if (!pServerCfg)
	{
		if (bOutLog)
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������֪ͨ����[Key:%d]!", __FUNCTION__, dwKey);
		return NULL;
	}

	return pServerCfg;
}

DWORD CGuildWarMgr::GetConfigValue(DWORD dwKey, DWORD dwDefault)
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetGuildWarCommon(dwKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������֪ͨ����[Key:%d]!", __FUNCTION__, dwKey);
		return dwDefault;
	}

	return pServerCfg->dwValue;
}

