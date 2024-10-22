#include "stdafx.h"
#include "ResourceWarManager.h"
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
#include "..\Guild\GuildManager.h"

// �����Դuid
DWORD MakeResourceUId(WORD wChapter, WORD wResourceId)
{
	return wChapter * 10000 + wResourceId;
}

// �����Դuid
void SplitResourceUId(DWORD dwUId, WORD &wChapter, WORD &wResourceId)
{
	if (dwUId)
	{
		wChapter = dwUId * 0.0001;
		wResourceId = dwUId % 10000;
	}
	else
	{
		wChapter = 0;
		wResourceId = 0;
	}
}

CResourceWarManager::CResourceWarManager()
{
	m_bLoadDataFromDB = false;
	m_dwLastProccessTimer = time(NULL);
}

CResourceWarManager::~CResourceWarManager()
{

}

void CResourceWarManager::Init()
{
	m_bLoadDataFromDB = false;
	m_dwLastProccessTimer = time(NULL);
}

void CResourceWarManager::Update()
{
	if (!m_bLoadDataFromDB)
		return;

	DWORD dwCurTime = time(NULL);
	//bool bIsDoExtraReward;
	ResourceWarResouceData *pWarData;
	map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter1 = m_ResourceList.begin();
	for (; iter1 != m_ResourceList.end(); ++iter1)
	{
		map<DWORD, ResourceWarResouceData>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			pWarData = &iter2->second;

			// ��������
			WORD wBaseRewardIntervalTime = g_Minute2Second(pWarData->wBaseRewardIntervalTime);
			int nTotalBaseRewardTimes = g_DivideBy(pWarData->dwOccupyOverTime - pWarData->dwOccupyStartTime, wBaseRewardIntervalTime);
			if (dwCurTime >= pWarData->dwLastBaseRewardTime + wBaseRewardIntervalTime && 
				pWarData->wAlreadySendBaseRewardTimes < nTotalBaseRewardTimes)
			{
				// ������
				DoBaseReward(pWarData);

				pWarData->dwLastBaseRewardTime += wBaseRewardIntervalTime;
			}

			// ��������
			WORD wExtraRewardNeedTime = g_Minute2Second(pWarData->wCurExtraRewardNeedTime);
			if (dwCurTime >= pWarData->dwLastExtraRewardTime + wExtraRewardNeedTime && 
				pWarData->wAlreadySendExtraRewardTimes <= pWarData->wKeepOccupyTimes)
			{
				// ������
				DoExtraReward(pWarData);

				pWarData->dwLastExtraRewardTime += wExtraRewardNeedTime;
			}

			// ռ��ʱ�����,��һ�������Ƿ�ֹ����ͣ��ά�������
			if (dwCurTime >= pWarData->dwOccupyOverTime && 
				pWarData->wAlreadySendBaseRewardTimes >= nTotalBaseRewardTimes)
			{
				DoOccupyTimeOver(pWarData);

				// �˳�, һ��ֻ����һ��
				break;
			}
		}
	}

	// ���ڴ洢
	if (dwCurTime >= m_dwLastProccessTimer + 60)
	{
		SaveDataToDB();
		m_dwLastProccessTimer = dwCurTime;
	}
}

void CResourceWarManager::OnLogin(Player *pPlayer)
{

}

void CResourceWarManager::OnLogout(Player *pPlayer)
{
	// ������ս��
	map<DWORD, DWORD>::iterator iter = m_InBattlePlayerList.find(pPlayer->GetRoleId());
	if (iter != m_InBattlePlayerList.end())
	{
		OnNetBattleOver(pPlayer, false);
	}
}

// �洢���ݵ����ݿ�
void CResourceWarManager::SaveDataToDB()
{
	if (!m_bLoadDataFromDB)
		return;

	UINT64 lCurTime = GetTickCount64();

	PB::ResourceWarData PBResouceWarData;

	WORD wJoinRoleNum = 0;	// ��������
	WORD wEnemyNum = 0;		// ��������
	WORD wOccupyResourceNum = 0;	// ռ������

	// ��Դ�б�
	for (map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter_list = m_ResourceList.begin(); iter_list != m_ResourceList.end(); ++iter_list)
	{
		wOccupyResourceNum += iter_list->second.size();

		for (map<DWORD, ResourceWarResouceData>::iterator iter = iter_list->second.begin(); iter != iter_list->second.end(); ++iter)
		{
			ResourceWarResouceData *pResourceData = &iter->second;
			::PB::ResourceWarResourceData *pPBResourceData = PBResouceWarData.add_resource_data_list();
			pPBResourceData->set_chapter(pResourceData->wChapter);
			pPBResourceData->set_resource_id(pResourceData->wResourceId);
			pPBResourceData->set_resource_type(pResourceData->wResourceType);
			pPBResourceData->set_occupy_start_time(pResourceData->dwOccupyStartTime);
			pPBResourceData->set_occupy_over_time(pResourceData->dwOccupyOverTime);
			pPBResourceData->set_owner_id(pResourceData->dwOwnerId);
			pPBResourceData->set_keep_occupy_times(pResourceData->wKeepOccupyTimes);
			//pPBResourceData->set_cur_occupy_reward_idx(pResourceData->wCurOccupyRewardIdx);
			pPBResourceData->set_last_base_reward_time(pResourceData->dwLastBaseRewardTime);
			pPBResourceData->set_last_extra_reward_time(pResourceData->dwLastExtraRewardTime);
			pPBResourceData->set_base_reward_interval_time(pResourceData->wBaseRewardIntervalTime);
			pPBResourceData->set_cur_extra_reward_need_time(pResourceData->wCurExtraRewardNeedTime);
			pPBResourceData->set_already_send_base_reward_times(pResourceData->wAlreadySendBaseRewardTimes);
			pPBResourceData->set_already_send_extra_reward_times(pResourceData->wAlreadySendExtraRewardTimes);
		}
	}


	// �����б�
	for (map<DWORD, map<DWORD, ResourceWarEnemy>>::iterator iter_list = m_EnemyList.begin(); iter_list != m_EnemyList.end(); ++iter_list)
	{
		wEnemyNum += iter_list->second.size();

		for (map<DWORD, ResourceWarEnemy>::iterator iter = iter_list->second.begin(); iter != iter_list->second.end(); ++iter)
		{
			ResourceWarEnemy *pEnemyData = &iter->second;
			::PB::ResourceWarEnemy *pPBEnemyData = PBResouceWarData.add_enemy_list();
			pPBEnemyData->set_role_id(iter_list->first);					// ��������ID
			pPBEnemyData->set_be_attack_times(pEnemyData->wAttackTimes);	// ����������
			pPBEnemyData->set_enemy_id(pEnemyData->dwRoleId);				// ����ID	
		}
	}


	// ս���б�
	for (deque<ResourceWardBattleRecord>::iterator iter = m_BattleRecordList.begin(); iter != m_BattleRecordList.end(); ++iter)
	{
		ResourceWardBattleRecord *pBattleRecord = &*iter;
		::PB::ResourceWarBattleRecord *pPBBattleRecord = PBResouceWarData.add_battle_record_list();
		pPBBattleRecord->set_chapter(pBattleRecord->wChapter);
		pPBBattleRecord->set_resource_id(pBattleRecord->wResourceId);
		pPBBattleRecord->set_attacker_id(pBattleRecord->dwAttackerId);
		pPBBattleRecord->set_defender_id(pBattleRecord->dwDefenderId);
		pPBBattleRecord->set_battle_time(pBattleRecord->dwBattleTime);
	}


	// �����Դս�����б�
	for (map<DWORD, RoleResourceWarData>::iterator iter = m_RoleResourceWarDataList.begin(); iter != m_RoleResourceWarDataList.end(); ++iter)
	{
		RoleResourceWarData *pRoleResourceWardData = &iter->second;
		::PB::RoleResourceWarData *pPBRoleResourceWardData = PBResouceWarData.add_role_resource_war_data_list();
		pPBRoleResourceWardData->set_role_id(iter->first);
		pPBRoleResourceWardData->set_technology_level(pRoleResourceWardData->wTechnologyLevel);
		pPBRoleResourceWardData->set_total_accupy_time(pRoleResourceWardData->dwTotalAccupyTime);
		pPBRoleResourceWardData->set_reward_resource_war_score(pRoleResourceWardData->nRewardResourceWarScore);
	}

	wJoinRoleNum = m_RoleResourceWarDataList.size();

	// ��Դ�б�
	string strSaveData("");
	if (!PBResouceWarData.AppendToString(&strSaveData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ϵ�л���Դս����ʧ��", __FUNCTION__);
		return;
	}
	if (strSaveData.size() > MAX_RESOURCE_WAR_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Դս�洢���ݹ���[size:%d]", __FUNCTION__, strSaveData.size());
		return;
	}

	ResourceWarData_Save* pQuery = ResourceWarData_Save::ALLOC();
	pQuery->SetIndex(DB_RESOURCE_WAR_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_RESOURCE_WAR);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	pQuery->SetJoinRoleNum(wJoinRoleNum);
	pQuery->SetEnemyNum(wEnemyNum);
	pQuery->SetOccupyResourceNum(wOccupyResourceNum);
	g_MapServer.MapDBQuery(pQuery);

	DWORD dwCostTime = GetTickCount64() - lCurTime;
	if (dwCostTime)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ϵ�л���Դս��������ʱ��(%d)����!", __FUNCTION__, dwCostTime);
	}
}

// ��������
void CResourceWarManager::LoadDataFromDB(const char *pData, int nSize)
{
	m_bLoadDataFromDB = true;

	if (!nSize)
		return;

	PB::ResourceWarData PBResourceWarData;
	if (!PBResourceWarData.ParseFromArray(pData, nSize))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ϵ�л���Դս���ݳ���[size:%d]!", __FUNCTION__, nSize);
		return;
	}


	// ��Դ�б�
	ResourceWarResouceData tResourceData;
	for (int i = 0; i < PBResourceWarData.resource_data_list_size(); ++i)
	{
		const PB::ResourceWarResourceData &tPBResourceData = PBResourceWarData.resource_data_list(i);

		tResourceData.wChapter						= tPBResourceData.chapter();
		tResourceData.wResourceId					= tPBResourceData.resource_id();
		tResourceData.wResourceType					= tPBResourceData.resource_type();
		tResourceData.dwOccupyStartTime				= tPBResourceData.occupy_start_time();
		tResourceData.dwOccupyOverTime				= tPBResourceData.occupy_over_time();
		tResourceData.dwOwnerId						= tPBResourceData.owner_id();
		tResourceData.wKeepOccupyTimes				= tPBResourceData.keep_occupy_times();
		//tResourceData.wCurOccupyRewardIdx			= tPBResourceData.cur_occupy_reward_idx();
		tResourceData.dwLastBaseRewardTime			= tPBResourceData.last_base_reward_time();
		tResourceData.dwLastExtraRewardTime			= tPBResourceData.last_extra_reward_time();
		tResourceData.wBaseRewardIntervalTime		= tPBResourceData.base_reward_interval_time();
		tResourceData.wCurExtraRewardNeedTime		= tPBResourceData.cur_extra_reward_need_time();
		tResourceData.wAlreadySendBaseRewardTimes	= tPBResourceData.already_send_base_reward_times();
		tResourceData.wAlreadySendExtraRewardTimes = tPBResourceData.already_send_extra_reward_times();

		AddResourceWarData(tResourceData);

		AddFindRoleResourceData(tResourceData.dwOwnerId, tResourceData.wChapter, tResourceData.wResourceId);
	}

	// �����б�
	ResourceWarEnemy tEnemy;
	for (int i = 0; i < PBResourceWarData.enemy_list_size(); ++i)
	{
		const PB::ResourceWarEnemy &tPBEnemy = PBResourceWarData.enemy_list(i);

		DWORD dwRoleId = tPBEnemy.role_id();
		tEnemy.dwRoleId = tPBEnemy.enemy_id();
		tEnemy.wAttackTimes = tPBEnemy.be_attack_times();

		AddEnemyData(dwRoleId, tEnemy);
	}

	// ս���б�
	ResourceWardBattleRecord tBattleReocrd;
	for (int i = 0; i < PBResourceWarData.battle_record_list_size(); ++i)
	{
		const PB::ResourceWarBattleRecord &tPBBattleRecord = PBResourceWarData.battle_record_list(i);

		tBattleReocrd.dwAttackerId	= tPBBattleRecord.attacker_id();
		tBattleReocrd.dwDefenderId	= tPBBattleRecord.defender_id();
		tBattleReocrd.dwBattleTime	= tPBBattleRecord.battle_time();
		tBattleReocrd.wChapter		= tPBBattleRecord.chapter();
		tBattleReocrd.wResourceId	= tPBBattleRecord.resource_id();

		AddBattleRecord(tBattleReocrd);
	}

	// �����Դս�����б�
	RoleResourceWarData tRoleResourceWarData;
	for (int i = 0; i < PBResourceWarData.role_resource_war_data_list_size(); ++i)
	{
		const PB::RoleResourceWarData &tPBRoleResourceWarData = PBResourceWarData.role_resource_war_data_list(i);

		DWORD dwRoleId = tPBRoleResourceWarData.role_id();
		tRoleResourceWarData.wTechnologyLevel = tPBRoleResourceWarData.technology_level();
		tRoleResourceWarData.dwTotalAccupyTime = tPBRoleResourceWarData.total_accupy_time();
		tRoleResourceWarData.nRewardResourceWarScore = tPBRoleResourceWarData.reward_resource_war_score();

		AddRoleResourceWarData(dwRoleId, tRoleResourceWarData);
	}

	MAPLOG(GUI_LEVEL, "�ɹ�������Դս����[ResourceNum:%d,EnemyNum:%d,BattleRecordNum:%d,RoleResourceWarDataNum:%d]!", 
		PBResourceWarData.resource_data_list_size(), PBResourceWarData.enemy_list_size(), 
		PBResourceWarData.battle_record_list_size(), PBResourceWarData.role_resource_war_data_list_size());
}

// ������������
void CResourceWarManager::LoadDataRequest()
{
	ResourceWarData_Load* pQuery = ResourceWarData_Load::ALLOC();
	pQuery->SetIndex(DB_RESOURCE_WAR_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_RESOURCE_WAR);
	g_MapServer.MapDBQuery(pQuery);
}

// ���ʼ
bool CResourceWarManager::DoActivityStart()
{
	m_ResourceList.clear();
	m_RoleResourceFindList.clear();
	m_EnemyList.clear();
	m_BattleRecordList.clear();
	m_BeBattleResourceList.clear();
	m_InBattlePlayerList.clear();
	return true;
}

// ������Դս����
void CResourceWarManager::OnNetResourceWardData(Player *pPlayer)
{
	// ���ȼ��Ƿ��㹻
	WORD wNeedRoleLevel = g_pCfgMgr->GetSystemOpenLevel(157);
	if (pPlayer->GetLevel() < wNeedRoleLevel)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Դս�ȼ�����[NeedRoleLevel:%d]", __FUNCTION__, wNeedRoleLevel);
		return;
	}

	// ����Ƿ��ڻ��
	if (!IsActivity())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Դսû�ڻ��", __FUNCTION__);
		return;
	}

	SendResourceWardData(pPlayer);
}

// ����ս��
void CResourceWarManager::OnNetBattleRecord(Player *pPlayer)
{
	MSG_S2C_RESOURCE_WAR_BATTLE_RECORD_RESP msg;
	ResourceWardRecordClient tBattleRecord;
	deque<ResourceWardBattleRecord>::iterator iter = m_BattleRecordList.begin();
	for (; iter != m_BattleRecordList.end();++iter)
	{
		tBattleRecord.dwBattleTime = iter->dwBattleTime;
		tBattleRecord.wChapter = iter->wChapter;
		tBattleRecord.wResourceId = iter->wResourceId;
		const PlayerBaseData *pAttackerData = g_GetPlayerBaseData(iter->dwAttackerId);
		if (pAttackerData)
			strcpy_s(tBattleRecord.szAttackerName, sizeof(tBattleRecord.szAttackerName), pAttackerData->szRoleName);
		const PlayerBaseData *pDefenderData = g_GetPlayerBaseData(iter->dwDefenderId);
		if (pDefenderData)
			strcpy_s(tBattleRecord.szDefenderName, sizeof(tBattleRecord.szDefenderName), pDefenderData->szRoleName);
		msg.m_vBattleRecordList.push_back(tBattleRecord);
	}
	pPlayer->SendPacket(&msg);
}

// ��������б�
void CResourceWarManager::OnNetEnemyList(Player *pPlayer)
{
	MSG_S2C_RESOURCE_WAR_ENEMY_LIST_RESP msg;
	ResourceWarEnemyClient tEnemyClient;
	map<DWORD, map<DWORD, ResourceWarEnemy>>::iterator iter1 = m_EnemyList.find(pPlayer->GetRoleId());
	if (iter1 != m_EnemyList.end())
	{
		map<DWORD, ResourceWarEnemy>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			ResourceWarEnemy *pEnemyData = &iter2->second;
			tEnemyClient.wAttackTimes = pEnemyData->wAttackTimes;
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pEnemyData->dwRoleId);
			if (pBaseData)
				memcpy_s(&tEnemyClient.tCommonData, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));	
			ResourceWarResouceData *pWarData = FindResourceWarData(pEnemyData->dwRoleId);
			tEnemyClient.wChapter = pWarData ? pWarData->wChapter : 0;
			tEnemyClient.wResourceId = pWarData ? pWarData->wResourceId : 0;
			msg.m_vEnemyList.push_back(tEnemyClient);
		}
	}
	pPlayer->SendPacket(&msg);
}

// �����½�����
void CResourceWarManager::OnNetChapterData(Player *pPlayer, WORD wChapter)
{
	SendChapterData(pPlayer, wChapter);
}

// ������Դ��������
void CResourceWarManager::OnNetSameResourceList(Player *pPlayer, WORD wResourceType)
{
	MSG_S2C_RESOURCE_WAR_SAME_RESOURCE_RESP msg;
	msg.m_wResourceType = wResourceType;
	ResourceWarBaseData tBaseData;
	map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter1 = m_ResourceList.begin();
	for (; iter1 != m_ResourceList.end(); ++iter1)
	{
		map<DWORD, ResourceWarResouceData>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			if (iter2->second.wResourceType == wResourceType)
			{
				tBaseData.wChapter = iter2->second.wChapter;
				tBaseData.wResourceId = iter2->second.wResourceId;
				msg.m_vResourceDataList.push_back(tBaseData);
			}
		}
	}
	pPlayer->SendPacket(&msg);
}

// ���������Դ
void CResourceWarManager::OnNetGuildResourceList(Player *pPlayer)
{
	MSG_S2C_RESOURCE_WAR_GUILD_RESOURCE_RESP msg;

	// ȡ��������
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
	if (pGuild)
	{
		ResourceWarBaseData tBaseData;
		const GuildData &tGuildData = pGuild->GetGuildData();
		for (WORD i = 0; i < tGuildData.byMemberNum; ++i)
		{
			DWORD dwRoleId = tGuildData.arMemberList[i].dwRoleId;
			DWORD dwUID = GetRoleAccupyResourceUId(dwRoleId);
			if (dwUID)
			{
				SplitResourceUId(dwUID, tBaseData.wChapter, tBaseData.wResourceId);
				msg.m_vResourceDataList.push_back(tBaseData);
			}
		}
	}
	pPlayer->SendPacket(&msg);
}

// �����Ƽ�
void CResourceWarManager::OnNetUpgradeTechnology(Player *pPlayer)
{
	RoleResourceWarData &tRoleResourceWarData = GetRoleResourceWarData(pPlayer->GetRoleId());

	WORD wNextLevel = tRoleResourceWarData.wTechnologyLevel + 1;

	// ȡ�Ƽ�����
	const ResourceWarTechnology_Config *pTechnologyCfg = g_pCfgMgr->GetResourceWarTecknology(wNextLevel);
	if (!pTechnologyCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս�Ƽ�����[TechnologyLevel:%d]", __FUNCTION__, wNextLevel);
		return;
	}

	// ���������Ҫ��ʱ��
	if (tRoleResourceWarData.dwTotalAccupyTime < pTechnologyCfg->dwNeedAccupyTime)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Դս��Ҫ��ʱ�䲻��[TechnologyLevel:%d,NeedAccupyTime:%d,CurAccupyTime:%d]", 
			__FUNCTION__, wNextLevel, pTechnologyCfg->dwNeedAccupyTime, tRoleResourceWarData.dwTotalAccupyTime);
		return;
	}

	// ���¿Ƽ��ȼ�
	tRoleResourceWarData.wTechnologyLevel = wNextLevel;

	// ����������
	SendResourceWardData(pPlayer);

	// ��Ӧ
	MSG_S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP msg;
	msg.m_wTechnologyLevel = wNextLevel;
	pPlayer->SendPacket(&msg);
}

// �ӳ�ռ��
void CResourceWarManager::OnNetKeepOccupy(Player *pPlayer)
{
	ResourceWarResouceData *pResourceWarData = FindResourceWarData(pPlayer->GetRoleId());
	if (!pResourceWarData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ռ�����Դ[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(pResourceWarData->wResourceId);
	if (!pResourceCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս����Դ����[Chapter:%d,ResourceId:%d]",
			__FUNCTION__, pResourceWarData->wChapter, pResourceWarData->wResourceId);
		return;
	}

	// ����ӳ������Ƿ��Ѵ����� todo::��1����ΪĬ�Ͼ������ӳ�һ��
	if (pResourceWarData->wKeepOccupyTimes + 1 >= pResourceCfg->vExtraRewardDataList.size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ӳ�ռ����Դ�����Ѵ�����[Chapter:%d,ResourceId:%d,KeepOccupyTimes:%d]",
			__FUNCTION__, pResourceWarData->wChapter, pResourceWarData->wResourceId, pResourceWarData->wKeepOccupyTimes);
		return;
	}

	const ResourceWarExtraReward *pExtraRewardCfg = &pResourceCfg->vExtraRewardDataList[pResourceWarData->wKeepOccupyTimes+1];

	// �������
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pExtraRewardCfg->nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�ӳ�ռ����Դ�������ó���!!![Chapter:%d,ResourceId:%d,KeepOccupyTimes:%d]",
			__FUNCTION__, pResourceWarData->wChapter, pResourceWarData->wResourceId, pResourceWarData->wKeepOccupyTimes);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pExtraRewardCfg->nCostDiamond, TRUE);

	pResourceWarData->wKeepOccupyTimes++;
	pResourceWarData->dwOccupyOverTime += g_Minute2Second(pExtraRewardCfg->vRewardNeedTime);

	// ����������
	SendResourceWardData(pPlayer);

	// ��Ӧ
	MSG_S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP msg;
	pPlayer->SendPacket(&msg);

	MAPLOG(ERROR_LEVEL, "[%s]�ӳ�ռ����Դʱ��[RoleId:%d, Chapter:%d, ResourceId:%d]",
		__FUNCTION__,pPlayer->GetRoleId(), pResourceWarData->wChapter, pResourceWarData->wResourceId);
}

// ����ռ��
void CResourceWarManager::OnNetGiveUpOccupy(Player *pPlayer)
{
	ResourceWarResouceData *pWarData = FindResourceWarData(pPlayer->GetRoleId());
	if (!pWarData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ������ռ�����Դ[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MAPLOG(ERROR_LEVEL, "[%s]����ռ����Դ[RoleId:%d, Chapter:%d, ResourceId:%d]",
		__FUNCTION__,pPlayer->GetRoleId(), pWarData->wChapter, pWarData->wResourceId);

	// ����ۻ�ʱ��
	UpdateAccupyResourceTime(pPlayer->GetRoleId(), time(NULL) - pWarData->dwLastExtraRewardTime);

	// ���Ż�������
	SendBaseReward(pPlayer->GetRoleId(), pWarData->wChapter, pWarData->wResourceId);

	RemoveFindRoleResourceData(pPlayer->GetRoleId());

	RemoveResourceWarData(pWarData->wChapter, pWarData->wResourceId);

	SendResourceWardData(pPlayer);

	// ��Ӧ
	MSG_S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP msg;
	pPlayer->SendPacket(&msg);
}

// ս����ʼ
bool CResourceWarManager::OnNetBattleStart(Player *pPlayer, WORD wChapter, WORD wResourceId)
{
	// ���ȼ��Ƿ��㹻
	WORD wNeedRoleLevel = g_pCfgMgr->GetSystemOpenLevel(157);
	if (pPlayer->GetLevel() < wNeedRoleLevel)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Դս�ȼ�����[NeedRoleLevel:%d]", __FUNCTION__, wNeedRoleLevel);
		return false;
	}

	// ����Ƿ��ڻ��
	if (!IsActivity())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��Դսû�ڻ��", __FUNCTION__);
		return false;
	}

	// ����Ƿ�����ռ����Դ
	if (FindResourceWarData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�����ռ��������Դ[role_id:%d]",__FUNCTION__,pPlayer->GetRoleId());
		return false;
	}

	// �����Դ�Ƿ��ѱ���ս��
	if (UpdateAndCheckIsBeBattle(wChapter, wResourceId))
	{
		// ����ͬ������
		SendChapterData(pPlayer, wChapter);

		pPlayer->SendErrorMsg(ERROR_RESOURCE_WAR_BE_BATTLE);
		MAPLOG(GUI_LEVEL, "[%s]��Դ�ѱ���ս��[role_id:%d,wChapter:%d,wResourceType:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), wChapter, wResourceId);
		return false;
	}
	
	const ResourceWarChapter_Config *pResWarChapterCfg = g_pCfgMgr->GetResourceWarChapter(wChapter);
	if (!pResWarChapterCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս���½�����[wChapter:%d]", __FUNCTION__, wChapter);
		return false;
	}
	if (pResWarChapterCfg->resourceList.find(wResourceId) == pResWarChapterCfg->resourceList.end())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�½ڸ���Դ��ƥ��[Chapter:%d,ResourceId:%d]", __FUNCTION__, wChapter, wResourceId);
		return false;
	}

	// ȡ��Դ����
	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(wResourceId);
	if (!pResourceCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս����Դ����[ResourceId:%d]", __FUNCTION__, wResourceId);
		return false;
	}

	// �������
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < pResourceCfg->nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Դս��ս���ĵ���ʯ����[role_id:%d,Chapter:%d,ResourceId:%d,CostDiamond:%d]",
			__FUNCTION__, pPlayer->GetRoleId(), wChapter, wResourceId, pResourceCfg->nCostDiamond);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// ��ս��Ŀ��(0=��ʾ����,��0��ʾ���)
	DWORD dwTargetId = 0;

	// ��Դ�ѱ�ռ��,����ս���
	ResourceWarResouceData *pResourceWarData = FindResourceWarData(wChapter, wResourceId);
	if (pResourceWarData)
	{
		dwTargetId = pResourceWarData->dwOwnerId;

		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// ����ս��
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}
	// ��Դû��ռ��,����ս����
	else
	{
		// todo::Ŀǰ��ʱ���ô���
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -pResourceCfg->nCostDiamond, TRUE, FROM_RESOURCE_WAR_BATTLE);

	// �����Դ����ս��
	AddBeBattleResource(wChapter, wResourceId);

	// ������ս�е���Դ
	pFBMgr->SetBattleInfo(wChapter, wResourceId);

	AddInBattlePlayer(pPlayer->GetRoleId(), wChapter, wResourceId);

	SendChapterData(pPlayer, wChapter);

	return true;
}

// ս������
bool CResourceWarManager::OnNetBattleOver(Player *pPlayer, BYTE byIsWin)
{
	// ����Ƿ�����ռ����Դ
	if (FindResourceWarData(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�����ռ��������Դ[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	DWORD dwResourceUId = GetBeBattleResourceUId(pPlayer->GetRoleId());
	if (!dwResourceUId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ҵ�ǰû����ս��Դ[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	WORD wChapter = 0, wResourceId = 0;
	SplitResourceUId(dwResourceUId, wChapter, wResourceId);

	// ȡ��Դ����
	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(wResourceId);
	if (!pResourceCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս����Դ����[wResourceId:%d]", __FUNCTION__, wResourceId);
		return false;
	}

	DWORD dwCurTime = time(NULL);

	if (byIsWin)
	{
		DWORD dwOldOwnerId = 0;

		ResourceWarResouceData *pResourceWarData = FindResourceWarData(wChapter, wResourceId);
		// ��ռ������Դ
		if (pResourceWarData)
		{
			dwOldOwnerId = pResourceWarData->dwOwnerId;

			// Ŀ�����ۻ�ʱ��
			UpdateAccupyResourceTime(pResourceWarData->dwOwnerId, time(NULL) - pResourceWarData->dwLastExtraRewardTime);
			// ���Ż�������
			SendBaseReward(pResourceWarData->dwOwnerId, pResourceWarData->wChapter, pResourceWarData->wResourceId);

			// ��ӳ���
			AddEnemyData(pResourceWarData->dwOwnerId, pPlayer->GetRoleId());

			// ���ս��
			ResourceWardBattleRecord tWardRecord;
			tWardRecord.dwAttackerId = pPlayer->GetRoleId();
			tWardRecord.dwDefenderId = pResourceWarData->dwOwnerId;
			tWardRecord.dwBattleTime = dwCurTime;
			tWardRecord.wChapter = wChapter;
			tWardRecord.wResourceId = wResourceId;
			AddBattleRecord(tWardRecord);

			// �Ӳ����б����õ�
			RemoveFindRoleResourceData(pResourceWarData->dwOwnerId);

			// ��Դ��ռ���Ժ����Ŀ������������������
			Player *pOldOwner = PlayerManager::Instance()->FindPlayerByRoleId(pResourceWarData->dwOwnerId);
			if (pOldOwner)
				SendResourceWardData(pOldOwner);

			pResourceWarData->dwOwnerId = pPlayer->GetRoleId();
			pResourceWarData->dwOccupyStartTime = dwCurTime;
			pResourceWarData->dwLastBaseRewardTime = dwCurTime;
			pResourceWarData->dwLastExtraRewardTime = dwCurTime;
			pResourceWarData->wKeepOccupyTimes = 0;
			pResourceWarData->wAlreadySendBaseRewardTimes = 0;
			pResourceWarData->wAlreadySendExtraRewardTimes = 0;
			pResourceWarData->wBaseRewardIntervalTime = pResourceCfg->wBaseProfitIntervalTime;
			pResourceWarData->wCurExtraRewardNeedTime = pResourceCfg->vExtraRewardDataList.at(0).vRewardNeedTime;
			pResourceWarData->dwOccupyOverTime = dwCurTime + g_Minute2Second(pResourceWarData->wCurExtraRewardNeedTime);
		}
		// ��ռ������Դ
		else
		{
			ResourceWarResouceData tResourceWarData;
			tResourceWarData.wChapter = wChapter;
			tResourceWarData.wResourceId = wResourceId;
			tResourceWarData.wResourceType = pResourceCfg->wResourceType;

			tResourceWarData.dwOwnerId = pPlayer->GetRoleId();
			tResourceWarData.dwOccupyStartTime = dwCurTime;
			tResourceWarData.dwLastBaseRewardTime = dwCurTime;
			tResourceWarData.dwLastExtraRewardTime = dwCurTime;
			tResourceWarData.wKeepOccupyTimes = 0;
			tResourceWarData.wAlreadySendBaseRewardTimes = 0;
			tResourceWarData.wAlreadySendExtraRewardTimes = 0;
			tResourceWarData.wBaseRewardIntervalTime = pResourceCfg->wBaseProfitIntervalTime;
			tResourceWarData.wCurExtraRewardNeedTime = pResourceCfg->vExtraRewardDataList.at(0).vRewardNeedTime;
			tResourceWarData.dwOccupyOverTime = dwCurTime + g_Minute2Second(tResourceWarData.wCurExtraRewardNeedTime);
			AddResourceWarData(tResourceWarData);
		}

		// �����Դ��¼
		MAPLOG(ERROR_LEVEL, "��һ����Դ[NewOwnerId:%d,OldOwnerId:%d,Chapter:%d,ResourceId:%d]",
			pPlayer->GetRoleId(), dwOldOwnerId, wChapter, wResourceId);

		AddFindRoleResourceData(pPlayer->GetRoleId(), wChapter, wResourceId);
	}
	else
	{
		// todo::Ŀǰ�䲻��Ҫ����
	}

	RemoveBeBattleResource(wChapter, wResourceId);

	RemoveInBattlePlayer(pPlayer->GetRoleId());

	return true;
}

// ����ռ��ʱ�����
void CResourceWarManager::DoOccupyTimeOver(const ResourceWarResouceData *pResourceWarData)
{
	// ������
	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(pResourceWarData->wResourceId);
	if (pResourceCfg)
	{
		// todo::������
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս����Դ����[wChapter:%d,wResourceId:%d]", 
			__FUNCTION__, pResourceWarData->wChapter, pResourceWarData->wResourceId);
	}

	MAPLOG(ERROR_LEVEL, "��Դռ��ʱ�����[RoleId:%d, Chapter:%d, ResourceId:%d]",
		pResourceWarData->dwOwnerId, pResourceWarData->wChapter, pResourceWarData->wResourceId);

	// ����ۻ�ʱ��
	//UpdateAccupyResourceTime(pResourceWarData->dwOwnerId, pResourceWarData->wCurExtraRewardNeedTime);

	// ���Ż�������
	SendBaseReward(pResourceWarData->dwOwnerId, pResourceWarData->wChapter, pResourceWarData->wResourceId);

	RemoveFindRoleResourceData(pResourceWarData->dwOwnerId);

	RemoveResourceWarData(pResourceWarData->wChapter, pResourceWarData->wResourceId);
}

// �����������
void CResourceWarManager::DoBaseReward(ResourceWarResouceData *pResourceWarData)
{
	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(pResourceWarData->wResourceId);
	if (!pResourceCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս����Դ����[wChapter:%d,wResourceId:%d]",
			__FUNCTION__, pResourceWarData->wChapter, pResourceWarData->wResourceId);
		return;
	}

	// ������
	int nAddRate = GetBaseRewardAddRate(pResourceWarData->dwOwnerId);
	int nRewardResourceWarScore = double(PROPERTY_DECREASE_MULTIPLE * nAddRate + 1) * pResourceCfg->nBaseRewardResourceWarSocre;
	RoleResourceWarData &tRoleResourceWarData = GetRoleResourceWarData(pResourceWarData->dwOwnerId);
	tRoleResourceWarData.nRewardResourceWarScore += nRewardResourceWarScore;

	// �����ѷ������Ĵ���
	pResourceWarData->wAlreadySendBaseRewardTimes++;

// 	MAPLOG(GUI_LEVEL, "������Դս��������[RoleId:%d,Chapter:%d,ResourceId:%d,RewardTimes:%d,AddRate:%d,Score:%d]",
// 		pResourceWarData->dwOwnerId, pResourceWarData->wChapter, pResourceWarData->wResourceId,
// 		pResourceWarData->wAlreadySendBaseRewardTimes, nAddRate, nRewardResourceWarScore);
}

// ������⽱��
void CResourceWarManager::DoExtraReward(ResourceWarResouceData *pResourceWarData)
{
	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(pResourceWarData->wResourceId);
	if (!pResourceCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս����Դ����[wChapter:%d,wResourceId:%d]",
			__FUNCTION__, pResourceWarData->wChapter, pResourceWarData->wResourceId);
		return;
	}

	// ����ۻ�ʱ��
	UpdateAccupyResourceTime(pResourceWarData->dwOwnerId, pResourceWarData->wCurExtraRewardNeedTime * 60);

	// ͨ���������ķ�����
	DWORD dwResourceUId = MakeResourceUId(pResourceWarData->wChapter, pResourceWarData->wResourceId);
	RewardData tRewardData;
	tRewardData.wInfoId = 30;
	tRewardData.dwTime = time(NULL);
	tRewardData.dwValue1 = dwResourceUId;
	tRewardData.dwValue2 = pResourceWarData->wAlreadySendExtraRewardTimes;
	DWORD dwServerId = PlayerBaseDataManager::Instance()->GetServerIdByRoleId(pResourceWarData->dwOwnerId);
	CRewardCenterManager::AddReward(dwServerId, pResourceWarData->dwOwnerId, tRewardData);

	// �����ѷ������Ĵ���
	pResourceWarData->wAlreadySendExtraRewardTimes++;

	// ���µ�ǰ������Ҫ�ļ��ʱ��
	if (pResourceWarData->wAlreadySendExtraRewardTimes <= pResourceWarData->wKeepOccupyTimes &&
		pResourceWarData->wAlreadySendExtraRewardTimes < pResourceCfg->vExtraRewardDataList.size())
	{
		pResourceWarData->wCurExtraRewardNeedTime = pResourceCfg->vExtraRewardDataList[pResourceWarData->wAlreadySendExtraRewardTimes].vRewardNeedTime;
	}

	MAPLOG(GUI_LEVEL, "������Դս���⽱��[RoleId:%d,Chapter:%d,ResourceId:%d,RewardTimes:%d]",
		pResourceWarData->dwOwnerId, pResourceWarData->wChapter, pResourceWarData->wResourceId, pResourceWarData->wAlreadySendExtraRewardTimes);
}

// ȡ���⽱���б�
const vector<Item>* CResourceWarManager::GetExtraRewardList(DWORD dwResourceUId, DWORD dwRewardIdx)
{
	WORD wChapter = 0, wResourceId = 0;
	SplitResourceUId(dwResourceUId, wChapter, wResourceId);

	const ResourceWarResource_Config *pResourceCfg = g_pCfgMgr->GetResourceWarResource(wResourceId);
	if (!pResourceCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Դս��Դ����[ResourceId:%d]", __FUNCTION__, wResourceId);
		return NULL;
	}

	if (dwRewardIdx >= pResourceCfg->vExtraRewardDataList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Դս���⽱��������Խ��[ResourceId:%d,RewardIdx:%d]", __FUNCTION__, wResourceId, dwRewardIdx);
		return NULL;
	}

	return &pResourceCfg->vExtraRewardDataList[dwRewardIdx].tRewardItemList;
}

// ���ռ��ʱ��(GMʹ��)
void CResourceWarManager::AddOccupyTimeForGM(Player *pPlayer, DWORD dwAddTime)
{
	RoleResourceWarData &tRoleResourceWarData = GetRoleResourceWarData(pPlayer->GetRoleId());
	tRoleResourceWarData.dwTotalAccupyTime += dwAddTime;
	SendResourceWardData(pPlayer);
}

// ����ռ����Դʱ��
void CResourceWarManager::UpdateAccupyResourceTime(DWORD dwRoleId, DWORD dwAddTime)
{
	RoleResourceWarData &tRoleResourceWarData = GetRoleResourceWarData(dwRoleId);
	tRoleResourceWarData.dwTotalAccupyTime += dwAddTime;
}

// ����ռ�콱��
void CResourceWarManager::SendBaseReward(DWORD dwRoleId, WORD wChapter, WORD wResourceId)
{
	RoleResourceWarData &tRoleResourceWarData = GetRoleResourceWarData(dwRoleId);
	if (tRoleResourceWarData.nRewardResourceWarScore > 0)
	{
		// ͨ���������ķ�����
		DWORD dwResourceUId = MakeResourceUId(wChapter, wResourceId);
		RewardData tRewardData;
		tRewardData.wInfoId = 29;
		tRewardData.dwTime = time(NULL);
		tRewardData.dwValue1 = dwResourceUId;
		tRewardData.dwValue2 = tRoleResourceWarData.nRewardResourceWarScore;
		DWORD dwServerId = PlayerBaseDataManager::Instance()->GetServerIdByRoleId(dwRoleId);
		CRewardCenterManager::AddReward(dwServerId, dwRoleId, tRewardData);

		MAPLOG(GUI_LEVEL, "������Դս��������[RoleId:%d,Chapter:%d,ResourceId:%d,ResourceWarScore:%d]",
			dwRoleId, wChapter, wResourceId, tRoleResourceWarData.nRewardResourceWarScore);

		// �ѷ���������������
		tRoleResourceWarData.nRewardResourceWarScore = 0;
	}
}

// ������Դս����
void CResourceWarManager::SendResourceWardData(Player *pPlayer)
{
	MSG_S2C_PLAYER_RESOURCE_WAR_DATA_RESP msg;
	memset(&msg.m_RoleResourceData, 0, sizeof(msg.m_RoleResourceData));
	ResourceWarResouceData *pResourceData = FindResourceWarData(pPlayer->GetRoleId());
	if (pResourceData)
	{
		memcpy_s(&msg.m_RoleResourceData, sizeof(ResourceWarResouceData), pResourceData, sizeof(ResourceWarResouceData));
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
		if (pGuild)
			strcpy_s(msg.m_szGuildName, sizeof(msg.m_szGuildName), pGuild->GetGuildName());
	}
	RoleResourceWarData &tRoleResourceWarData = GetRoleResourceWarData(pPlayer->GetRoleId());
	msg.m_dwTotalAccupyTime = tRoleResourceWarData.dwTotalAccupyTime;
	msg.m_wTechnologyLevel = tRoleResourceWarData.wTechnologyLevel;
	msg.m_wGuildAccupyNum = GetGuildAccupyMemberNum(pPlayer->GetRoleId());

	//MAPLOG(ERROR_LEVEL, "ռ��ͬһ�½ڵĹ����Ա����[GuildAccupyNum:%d]", msg.m_wGuildAccupyNum);

	pPlayer->SendPacket(&msg);
}

// �����½�����
void CResourceWarManager::SendChapterData(Player *pPlayer, WORD wChapter)
{
	MSG_S2C_RESOURCE_WAR_CHAPTER_DATA_RESP msg;
	msg.m_wChapter = wChapter;
	ResourceWarDataClient tResourceData;
	map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter1 = m_ResourceList.find(wChapter);
	if (iter1 != m_ResourceList.end())
	{
		map<DWORD, ResourceWarResouceData>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			memset(&tResourceData, 0, sizeof(tResourceData));
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter2->second.dwOwnerId);
			if (pBaseData)
				memcpy_s(&tResourceData.tOwnerInfo, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));
			tResourceData.wResourceId = iter2->second.wResourceId;
			msg.m_vChapterResourceList.push_back(tResourceData);
		}
	}
	pPlayer->SendPacket(&msg);
}

// �Ƿ��ڻ��
bool CResourceWarManager::IsActivity()
{
	return true;
}

// ���ս��
void CResourceWarManager::AddBattleRecord(const ResourceWardBattleRecord &tRecord)
{
	// ��ǰ�����
	m_BattleRecordList.push_front(tRecord);

	// �Ӻ���ȡ�� todo::Ŀǰ�ݶ�20��, �պ����Ϊ�����ļ�
	if (m_BattleRecordList.size() > 20)
		m_BattleRecordList.pop_back();
}

// �����Դս����
void CResourceWarManager::AddResourceWarData(const ResourceWarResouceData &tResourceWarData)
{
	map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter = m_ResourceList.find(tResourceWarData.wChapter);
	if (iter != m_ResourceList.end())
	{
		iter->second.insert(make_pair(tResourceWarData.wResourceId, tResourceWarData));
	}
	else
	{
		map<DWORD, ResourceWarResouceData> list;
		list.insert(make_pair(tResourceWarData.wResourceId, tResourceWarData));
		m_ResourceList.insert(make_pair(tResourceWarData.wChapter, list));
	}
}

// �Ƴ���Դս����
void CResourceWarManager::RemoveResourceWarData(WORD wChapter, WORD wResourceId)
{
	map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter = m_ResourceList.find(wChapter);
	if (iter != m_ResourceList.end())
	{
		iter->second.erase(wResourceId);
	}
}

// ��ӵ�������
void CResourceWarManager::AddEnemyData(DWORD dwRoleId, DWORD dwEnemyId)
{
	map<DWORD, ResourceWarEnemy> *pEnemyList = GetEnemyList(dwRoleId);
	// ���е����б�
	if (pEnemyList)
	{
		map<DWORD, ResourceWarEnemy>::iterator iter = pEnemyList->find(dwEnemyId);
		// �˵����Ѵ���
		if (iter != pEnemyList->end())
		{
			// ���±���������(��ʤ)
			iter->second.wAttackTimes++;
		}
		else
		{
			ResourceWarEnemy data;
			data.dwRoleId = dwEnemyId;
			data.wAttackTimes = 1;
			pEnemyList->insert(make_pair(dwEnemyId, data));
		}
	}
	else
	{
		map<DWORD, ResourceWarEnemy> list;
		ResourceWarEnemy data;
		data.dwRoleId = dwEnemyId;
		data.wAttackTimes = 1;
		list.insert(make_pair(dwEnemyId, data));
		m_EnemyList.insert(make_pair(dwRoleId, list));
	}
}

// ��ӵ�������
void CResourceWarManager::AddEnemyData(DWORD dwRoleId, const ResourceWarEnemy &tEnemy)
{
	map<DWORD, ResourceWarEnemy> *pEnemyList = GetEnemyList(dwRoleId);
	// ���е����б�
	if (pEnemyList)
	{
		pEnemyList->insert(make_pair(tEnemy.dwRoleId, tEnemy));
	}
	else
	{
		map<DWORD, ResourceWarEnemy> list;
		list.insert(make_pair(tEnemy.dwRoleId, tEnemy));
		m_EnemyList.insert(make_pair(dwRoleId, list));
	}
}

// ȡ��Դս����
ResourceWarResouceData* CResourceWarManager::FindResourceWarData(WORD wChapter, WORD wResourceId)
{
	map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter1 = m_ResourceList.find(wChapter);
	if (iter1 != m_ResourceList.end())
	{
		map<DWORD, ResourceWarResouceData>::iterator iter2 = iter1->second.find(wResourceId);
		if (iter2 != iter1->second.end())
			return &iter2->second;
	}
	return NULL;
}

// ȡ��Դս����
ResourceWarResouceData* CResourceWarManager::FindResourceWarData(DWORD dwRoleId)
{
	DWORD dwUID = GetRoleAccupyResourceUId(dwRoleId);
	if (!dwUID)
		return NULL;

	WORD wChapter,wResourceId;
	SplitResourceUId(dwUID, wChapter, wResourceId);

	return FindResourceWarData(wChapter, wResourceId);
}

// ȡ��ҵ����б�
map<DWORD, ResourceWarEnemy>* CResourceWarManager::GetEnemyList(DWORD dwRoleId)
{
	map<DWORD, map<DWORD, ResourceWarEnemy>>::iterator iter = m_EnemyList.find(dwRoleId);
	return iter != m_EnemyList.end() ? &iter->second : NULL;
}

// ���²�����Ƿ���ս
bool CResourceWarManager::UpdateAndCheckIsBeBattle(WORD wChapter, WORD wResourceId)
{
	DWORD dwKey = MakeResourceUId(wChapter, wResourceId);
	map<DWORD, DWORD>::iterator iter = m_BeBattleResourceList.find(dwKey);
	if (iter != m_BeBattleResourceList.end())
	{
		// todo::��ʱ3����
		if (time(NULL) >= iter->second + 180)
		{
			m_BeBattleResourceList.erase(dwKey);
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

// ��ӱ���ս����Դ
void CResourceWarManager::AddBeBattleResource(WORD wChapter, WORD wResourceId)
{
	DWORD dwUId = MakeResourceUId(wChapter, wResourceId);
	m_BeBattleResourceList.insert(make_pair(dwUId, DWORD(time(NULL))));
}

// �Ƴ�����ս����Դ
void CResourceWarManager::RemoveBeBattleResource(WORD wChapter, WORD wResourceId)
{
	DWORD dwUId = MakeResourceUId(wChapter, wResourceId);
	m_BeBattleResourceList.erase(dwUId);
}

// �������ս�е����
void CResourceWarManager::AddInBattlePlayer(DWORD dwRoleId, WORD wChapter, WORD wResourceId)
{
	DWORD dwUId = MakeResourceUId(wChapter, wResourceId);
	m_InBattlePlayerList.insert(make_pair(dwRoleId, dwUId));
}

// �Ƴ�����ս�е����
void CResourceWarManager::RemoveInBattlePlayer(DWORD dwRoleId)
{
	m_InBattlePlayerList.erase(dwRoleId);
}

// ȡ���ڱ���ս����Դuid
DWORD CResourceWarManager::GetBeBattleResourceUId(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_InBattlePlayerList.find(dwRoleId);
	return iter != m_InBattlePlayerList.end() ? iter->second : 0;
}
// ȡ����Ѿ�ռ�����Դuid
DWORD CResourceWarManager::GetRoleAccupyResourceUId(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_RoleResourceFindList.find(dwRoleId);
	return iter != m_RoleResourceFindList.end() ? iter->second : 0;
}

// ��Ӳ��������Դ����
void CResourceWarManager::AddFindRoleResourceData(DWORD dwRoleId, WORD wChapter, WORD wResourceId)
{
	DWORD dwUId = MakeResourceUId(wChapter, wResourceId);
	m_RoleResourceFindList.insert(make_pair(dwRoleId, dwUId));
}

// �Ƴ����������Դ����
void CResourceWarManager::RemoveFindRoleResourceData(DWORD dwRoleId)
{
	m_RoleResourceFindList.erase(dwRoleId);
}

// ȡ��ҵĻ��������ӳɱ���
int CResourceWarManager::GetBaseRewardAddRate(DWORD dwRoleId)
{
	RoleResourceWarData &tRoleResourceWarData =  GetRoleResourceWarData(dwRoleId);

	int nAddRate = 0;

	// �Ƽ��ӳ�
	const ResourceWarTechnology_Config *pTechnologyCfg = g_pCfgMgr->GetResourceWarTecknology(tRoleResourceWarData.wTechnologyLevel);
	if (pTechnologyCfg)
		nAddRate = pTechnologyCfg->nBaseRewardAddRate;

	// ����ӳ�
	WORD wMemberNum = GetGuildAccupyMemberNum(dwRoleId);
	const ResourceWarGuildProfit_Config *pGuildProfitCfg = g_pCfgMgr->GetResourceWarGuildProfit(wMemberNum);
	if (pGuildProfitCfg)
		nAddRate += pGuildProfitCfg->nBaseRewardAddRate;

	return nAddRate;
}

// ȡͬһ�½ڹ����Առ������
WORD CResourceWarManager::GetGuildAccupyMemberNum(DWORD dwRoleId)
{
	WORD wMemberNum = 0;

	// ����ӳ�
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(dwRoleId);
	DWORD dwUID = dwGuildId ? GetRoleAccupyResourceUId(dwRoleId) : 0;
	if (dwUID)
	{
		WORD wChapter, wResourceId;
		SplitResourceUId(dwUID, wChapter, wResourceId);
		map<DWORD, map<DWORD, ResourceWarResouceData>>::iterator iter_list = m_ResourceList.find(wChapter);
		if (iter_list != m_ResourceList.end())
		{
			map<DWORD, ResourceWarResouceData>::iterator iter = iter_list->second.begin();
			for (; iter != iter_list->second.end(); ++iter)
			{
				// ͬһ�������
				if (dwGuildId == CGuildManager::Instance()->GetGuildIdByRoleId(iter->second.dwOwnerId))
					wMemberNum++;
			}
		}
	}

	return wMemberNum;
}

// ȡ�����Դս����
RoleResourceWarData& CResourceWarManager::GetRoleResourceWarData(DWORD dwRoleId)
{
	map<DWORD, RoleResourceWarData>::iterator iter = m_RoleResourceWarDataList.find(dwRoleId);
	if (iter == m_RoleResourceWarDataList.end())
	{
		RoleResourceWarData data;
		memset(&data, 0, sizeof(data));
		m_RoleResourceWarDataList.insert(make_pair(dwRoleId, data));
		iter = m_RoleResourceWarDataList.find(dwRoleId);
	}

	return iter->second;
}

// ��������Դս����
void CResourceWarManager::AddRoleResourceWarData(DWORD dwRoleId, const RoleResourceWarData &tData)
{
	m_RoleResourceWarDataList.insert(make_pair(dwRoleId, tData));
}
