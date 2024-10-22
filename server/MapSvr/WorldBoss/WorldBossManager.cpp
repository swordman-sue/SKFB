#include "stdafx.h"
#include "WorldBossManager.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"
#include "ToolKit\Rand.h"
#include "..\Player.h"
#include "..\PlayerBaseDataManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\MapServer.h"


CWorldBossManager::CWorldBossManager()
{
	m_lProcessTime = GetTickCount64();
	m_bActivity = false;
	m_bGM = false;
	m_nActivityStatus = ACTIVITY_IDLE;
	UpdateWeekDay();
}
CWorldBossManager::~CWorldBossManager()
{

}

void CWorldBossManager::Init()
{
	m_nActivityStatus = ACTIVITY_IDLE;
	m_lProcessTime = GetTickCount64();
}

void CWorldBossManager::Update()
{
	// 500���봦��һ��
	if (GetTickCount64() > m_lProcessTime + 500)
	{
		// �ȴ�������
 		if (ACTIVITY_WAIT_FOR_REWARD == m_nActivityStatus)
 		{
 			// ����Ƿ���Է�����
			if (IsRewardTime())
				DoRankReward();
 		}
 		// ���
 		else if (ACTIVITY_ING == m_nActivityStatus)
 		{
 			// ����Ƿ����
			if (IsOverTime())
			{
				ActivityOver();
			}
			else
			{
				map<DWORD, WorldBossData>::iterator iter = m_WorldBossList.begin();
				for (; iter != m_WorldBossList.end(); ++iter)
				{
					WORD wServerId = iter->second.wServerId;
					WorldBossData &tWorldBossData = GetWorldBossData(wServerId);
					if (tWorldBossData.dwReliveTime && IsBossDead(wServerId) && time(NULL) >= tWorldBossData.dwReliveTime)
					{
						InitBoss(wServerId);
					}
				}
			}
 				
 		}
 		// ����(���û��ʼ)
 		else
 		{
 			// ����Ƿ�ʼ
			if (IsStartTime())
 				ActivityStart();
 		}

		m_lProcessTime = GetTickCount64();
	}
}

void CWorldBossManager::OnNewDay()
{
	UpdateWeekDay();
}

// �������ڼ�
void CWorldBossManager::UpdateWeekDay()
{
	tm curTime;
	GetTMTime(&curTime, time(NULL));
	m_nWeekDay = curTime.tm_wday;
}

// ���ʼ
void CWorldBossManager::ActivityStart()
{
	//m_wBossLevel = 1;	// ���ʼ�����´�1����ʼ��
	m_WorldBossList.clear();
	m_AllRankList.clear();
	m_PlayerWorldBossDataList.clear();
	m_bGM = false;
	m_bActivity = true;
	m_nActivityStatus = ACTIVITY_ING;
	InitBoss();
	const WorldBossOpenTime_Config *pOpenTimeCfg = CConfigManager::Instance()->GetWorldBossOpenTime(m_nWeekDay);
	if (!pOpenTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����ʱ������[WeekDay:%d]", __FUNCTION__, m_nWeekDay);
		return;
	}
	m_nActivityTime = pOpenTimeCfg->nOverTime;
	MAPLOG(GUI_LEVEL, "����BOSS���ʼ!");

	MSG_S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY msg;
	g_MapServer.SendMsgToGate(0, S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY, &msg);
}

// �����
void CWorldBossManager::ActivityOver()
{
	m_bGM = false;
	m_bActivity = false;
	m_nActivityStatus = ACTIVITY_WAIT_FOR_REWARD;
	const WorldBossOpenTime_Config *pOpenTimeCfg = CConfigManager::Instance()->GetWorldBossOpenTime(m_nWeekDay);
	if (!pOpenTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����ʱ������[WeekDay:%d]", __FUNCTION__, m_nWeekDay);
		return;
	}
	m_nActivityTime = pOpenTimeCfg->nRewardTime;
	MAPLOG(GUI_LEVEL, "����BOSS�����!");

	MSG_S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY msg;
	g_MapServer.SendMsgToGate(0, S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY, &msg);
}

void CWorldBossManager::SetBossLevel(WORD wServerId, WORD wBossLevel)
{
	GetWorldBossData(wServerId).wBossLevel = wBossLevel;
}

WORD CWorldBossManager::GetBossLevel(WORD wServerId)
{
	return GetWorldBossData(wServerId).wBossLevel;
}

// ѡ����Ӫ
void CWorldBossManager::OnNetSelectCamp(Player *pPlayer, BYTE byCamp)
{
	WORD wFromMsgId = C2S_WORLD_BOSS_SELECT_CAMP_REQ;

	// �Ѿ�ѡ���
// 	if (GetPlayerWorldBossData(pPlayer->GetRoleId()))
// 	{
// 		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
// 		MAPLOG(GUI_LEVEL, "[%s]�����ѡ����Ӫ!", __FUNCTION__);
// 		return;
// 	}
// 
// 	if (byCamp >= CAMP_NUM)
// 	{
// 		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
// 		MAPLOG(GUI_LEVEL, "[%s]��Ӫ��������[Camp:%d]!", __FUNCTION__, byCamp);
// 		return;
// 	}
// 
// 	PlayerWorldBoseData tPlayerBossData;
// 	tPlayerBossData.byBattleTimes = 0;
// 	tPlayerBossData.byCamp = byCamp;
// 	tPlayerBossData.dwLastRecoveryTime = 0;
// 	tPlayerBossData.byBuyBattleTimes = 0;
// 	tPlayerBossData.dwLastRecoveryTime = time(NULL);
// 	m_PlayerWorldBossDataList.insert(make_pair(pPlayer->GetRoleId(), tPlayerBossData));

	if (byCamp >= CAMP_NUM)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]��Ӫ��������[Camp:%d]!", __FUNCTION__, byCamp);
		return;
	}

	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ����������BOSS����!", __FUNCTION__);
		return;
	}

	pPlayerBossData->byBattleTimes = 0;
	pPlayerBossData->byCamp = byCamp;
	pPlayerBossData->byBuyBattleTimes = 0;
	pPlayerBossData->dwLastRecoveryTime = time(NULL);
	pPlayerBossData->vHonorRewardRecordList.clear();
	pPlayerBossData->vDamageRewardRecordList.clear();


	MSG_S2C_WORLD_BOSS_SELECT_CAMP_RESP msg;
	msg.m_byCamp = byCamp;
	pPlayer->SendPacket(&msg);
	//SendWorldBossData(pPlayer);
}

// ��������BOSS����
void CWorldBossManager::OnNetWorldBossData(Player *pPlayer)
{
	SendWorldBossData(pPlayer);
}

// ��ȡ����
void CWorldBossManager::OnNetReward(Player *pPlayer, BYTE byRewardType, WORD *pAwardList, BYTE byAwardNum)
{
	WORD wFromMsgId = C2S_WORLD_BOSS_REWARD_REQ;

	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ����������BOSS����!", __FUNCTION__);
		return;
	}

	// ��������
	if (WORLD_BOSS_REWARD_HONOR == byRewardType)
	{
		for (BYTE i = 0; i < byAwardNum; ++i)
		{
			WORD wRewardId = pAwardList[i];

			// ����Ƿ�����ȡ
			for (size_t i = 0; i < pPlayerBossData->vHonorRewardRecordList.size(); ++i)
			{
				if (pPlayerBossData->vHonorRewardRecordList[i] == wRewardId)
				{
					pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
					MAPLOG(GUI_LEVEL, "[%s]����BOSS������������ȡ��[RewardId:%d]!", __FUNCTION__, wRewardId);
					return;
				}
			}

			const WorldBossHonorReward_Config *pRewardCfg = g_pCfgMgr->GetWorldBossHonorReward(wRewardId);
			if (!pRewardCfg)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(GUI_LEVEL, "[%s]�Ҳ�������BOSS������������[RewardId:%d]!", __FUNCTION__, wRewardId);
				return;
			}

			RoleRankData *pRankData = GetPlayerRankData(pPlayer->GetServerId(), RANK_WORLD_BOSS_HONOR, pPlayerBossData->byCamp, pPlayer->GetRoleId());
			if (!pRankData)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(GUI_LEVEL, "[%s]�Ҳ����������BOSS�������а�����!", __FUNCTION__);
				return;
			}

			if (pRankData->dwValue < pRewardCfg->nNeedHonor)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(GUI_LEVEL, "[%s]�������㲻����ȡ����!", __FUNCTION__);
				return;
			}

			pPlayer->AddObjectData(&pRewardCfg->tRewardItem, TRUE, FROM_WORLD_BOSS_REWARD);

			pPlayerBossData->vHonorRewardRecordList.push_back(wRewardId);
		}
	}
	// �˺�����
	else
	{
		for (BYTE i = 0; i < byAwardNum; ++i)
		{
			WORD wRewardId = pAwardList[i];

			// ����Ƿ�����ȡ
			for (size_t i = 0; i < pPlayerBossData->vDamageRewardRecordList.size(); ++i)
			{
				if (pPlayerBossData->vDamageRewardRecordList[i] == wRewardId)
				{
					pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
					MAPLOG(GUI_LEVEL, "[%s]����BOSS�˺���������ȡ��[RewardId:%d]!", __FUNCTION__, wRewardId);
					return;
				}
			}

			const WorldBossKillReward_Config *pRewardCfg = g_pCfgMgr->GetWorldBossKillReward(wRewardId);
			if (!pRewardCfg)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(GUI_LEVEL, "[%s]�Ҳ�������BOSS�˺���������[RewardId:%d]!", __FUNCTION__, wRewardId);
				return;
			}
	
			if (GetWorldBossData(pPlayer->GetServerId()).wBossLevel < pRewardCfg->nNeedBossLv)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
				MAPLOG(GUI_LEVEL, "[%s]BOSS�ȼ����㲻����ȡ����!", __FUNCTION__);
				return;
			}

			pPlayer->AddObjectData(&pRewardCfg->tRewardItem, TRUE, FROM_WORLD_BOSS_REWARD);

			pPlayerBossData->vDamageRewardRecordList.push_back(wRewardId);
		}
	}

	MSG_S2C_WORLD_BOSS_REWARD_RESP msg;
	msg.m_byRewardType = byRewardType;
	msg.m_byRewardNum = byAwardNum;
	memcpy_s(msg.m_RewardList, byAwardNum*sizeof(WORD), pAwardList, byAwardNum*sizeof(WORD));
	pPlayer->SendPacket(&msg);
}

// ������ȡ��¼
void CWorldBossManager::OnNetRewardRecord(Player *pPlayer, BYTE byRewardType)
{
	SendRewardRecord(pPlayer, byRewardType);
}

// ����ս��
void CWorldBossManager::OnNetBattleRecord(Player *pPlayer)
{
	MSG_S2C_WORLD_BOSS_BATTLE_RECORD_RESP msg;
	msg.m_byRecordNum = 0;
	WorldBossData &tWorldBossData = GetWorldBossData(pPlayer->GetServerId());
	for (size_t i = 0; i < tWorldBossData.vBattleRecordList.size(); ++i)
	{
		if (msg.m_byRecordNum >= MAX_WORLD_BOSS_BATTLE_RECORD_NUM)
			break;
		msg.m_RecordList[msg.m_byRecordNum++] = tWorldBossData.vBattleRecordList[i];
	}
	pPlayer->SendPacket(&msg);
}

// ������ս����
void CWorldBossManager::OnNetBuyBattleTimes(Player *pPlayer, BYTE byBuyTimes)
{
	WORD wFromMsgId = C2S_WORLD_BOSS_BATTLE_TIMES_BUY_REQ;

	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ����������BOSS����!", __FUNCTION__);
		return;
	}


	if (pPlayerBossData->byBuyBattleTimes + byBuyTimes > pPlayer->GetVipValue(VIP_WORLD_BOSS_BATTLE_TIMES_BUY))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]��������BOSS��ս��������Ѵ�����!", __FUNCTION__);
		return;
	}

	// �������
	const Server_Config *pSvrCfg = g_pCfgMgr->GetCommonCfg(70, true);
	if (!pSvrCfg || !pSvrCfg->vValueList.size())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]�Ҳ�������BOSS��ս�������������б�����Ϊ��!", __FUNCTION__);
		return;
	}
	int nCostDiamond = 0;
	for (BYTE i = pPlayerBossData->byBuyBattleTimes; i < pPlayerBossData->byBuyBattleTimes + byBuyTimes; ++i)
	{
		if (i >= pSvrCfg->vValueList.size())
		{
			nCostDiamond += pSvrCfg->vValueList[pSvrCfg->vValueList.size()-1];
		}
		else
		{
			nCostDiamond += pSvrCfg->vValueList[i];
		}
	}
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]��������BOSS��ս����������ʯ����[diamond:%d]!", __FUNCTION__, nCostDiamond);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_WORLD_BOSS_BATTLE_TIMES_BUY);

	pPlayerBossData->byBuyBattleTimes += byBuyTimes;

	SendWorldBossData(pPlayer);

	// ��Ӧ
	MSG_S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP msg;
	msg.m_byBuyTimes = byBuyTimes;
	pPlayer->SendPacket(&msg);
}

// ��ʼս��
bool CWorldBossManager::OnBattleStart(Player *pPlayer, WORD wBossLevel)
{
	// ����Ƿ��ڻ��
	if (!m_bActivity)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���ڲ�������BOSS����ʱ��",__FUNCTION__);
		return false;
	}

	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ���ҵ��������BOSS����(Ӧ����ľ��ѡ��Ӫ)!", __FUNCTION__);
		return false;
	}

	// ������ս����
	if (pPlayerBossData->byBattleTimes >= g_GetConfigValue(53) + pPlayerBossData->byBuyBattleTimes)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����BOSS��ս�����Ѵ�����!", __FUNCTION__);
		return false;
	}

	// �Ƿ�������
	if (IsBossDead(pPlayer->GetServerId()))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����BOSS������!", __FUNCTION__);
		return false;
	}

	// �۳���ս����
	pPlayerBossData->byBattleTimes++;

	// ��ӹ�������
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	if (pFBMgr)
	{
		WorldBossData &tWorldBossData = GetWorldBossData(pPlayer->GetServerId());
		for (size_t i = 0; i < tWorldBossData.vMonsterList.size(); ++i)
			pFBMgr->AddMonsterData(tWorldBossData.vMonsterList[i]);
	}

	return true;
}

// ս������ 
bool CWorldBossManager::OnBattleOver(Player *pPlayer, BYTE byIsWin, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	DWORD dwServerId = pPlayer->GetServerId();
	WorldBossData &tWorldBossData = GetWorldBossData(dwServerId);

	bool isBossDead = IsBossDead(dwServerId);
	
	// ����Ѿ��������ȸ����ȼ��ͻ��1,����Ҫ����
	WORD wBossLevel = isBossDead ? tWorldBossData.wBossLevel - 1 : tWorldBossData.wBossLevel;

	const WorldBoss_Config *pWorldBossCfg = CConfigManager::Instance()->GetWorldBoss(wBossLevel);
	if (!pWorldBossCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����[BossLv:%d]!", __FUNCTION__, wBossLevel);
		return false;
	}

	DWORD dwTotalDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		dwTotalDamage += pMonsterList[i].dwDamage;
	}

	// �������
	int nHonor = ceil(int(dwTotalDamage * 0.01));

	// ��ù�ѫ(ս��)
	DWORD dwExploit = GetExploit();

	// ��ս��������
	pPlayer->ModifyProperty(ROLE_PRO_HONOR, nHonor);				// �������
	pPlayer->ModifyProperty(ROLE_PRO_REBEL_VALUE, dwExploit);		// ��ӹ�ѫ(ս��)

	MAPLOG(SPECAIL_LEVEL, "��ҹ�������BOSS���[RoleId:%d,TotalDamage:%d, Honor:%d,Exploit:%d,IsBossDead:%d]!",
		pPlayer->GetRoleId(), dwTotalDamage, nHonor, dwExploit, isBossDead);

	// ����һ������
	if (tWorldBossData.bHaveLuckReward && pWorldBossCfg->nLuckRewardRate >= g_Rand())
	{
		pPlayer->AddObjectData(&pWorldBossCfg->tLuckRewardItem, TRUE, FROM_WORLD_BOSS_BATTLE_REWARD);
		tWorldBossData.bHaveLuckReward = false;
		AddBattleRecord(dwServerId, pPlayer->GetRoleName(), NULL);
	}

	// ����ڽ��У�Boss��û����
	if (m_bActivity && !isBossDead)
	{
		// ����BOSS�˺�
		UpdateBossDamage(dwServerId, pMonsterList, byMonsterNum);

		// ���һ������
		if (IsBossDead(dwServerId))
		{
			// ����㲥
			CSystemNotice sys_notice(12, dwServerId);
			sys_notice.AddParam(tWorldBossData.wBossLevel);
			sys_notice.AddParam(pPlayer->GetRoleName());
			sys_notice.SendMsg();

			pPlayer->AddObjectData(&pWorldBossCfg->tKillRewardItem, TRUE, FROM_WORLD_BOSS_BATTLE_REWARD);
			AddBattleRecord(dwServerId, NULL, pPlayer->GetRoleName());

			// Boss����
			tWorldBossData.wBossLevel++;

			tWorldBossData.dwReliveTime = time(NULL) + g_GetConfigValue(54);
		}

		// �洢ȫ������
		g_MapServer.SaveGolbalData(pPlayer->GetServerId(), GOLBAL_DATA_WORLD_BOSS_LEVEL, tWorldBossData.wBossLevel);
	}

	// �������
	if (nHonor)
		AddRankData(pPlayer, RANK_WORLD_BOSS_HONOR, nHonor);
	if (dwTotalDamage)
		AddRankData(pPlayer, RANK_WORLD_BOSS_DAMAGE, dwTotalDamage);

	SendWorldBossData(pPlayer);

	// ��¼��õĹ�ѫ(ս��)
	pPlayer->GetFubenManager()->SetParam(3, nHonor);
	pPlayer->GetFubenManager()->SetParam(4, dwExploit);

	// �����˺�
	MSG_S2C_WORLD_BOSS_DAMAGE_NOTIFY damage_notify_msg;
	damage_notify_msg.m_dwDamage = dwTotalDamage;
	damage_notify_msg.m_dwCurTotalHP = GetBossRemainHP(dwServerId);
	strcpy_s(damage_notify_msg.m_szAttackName, sizeof(damage_notify_msg.m_szAttackName) - 1, pPlayer->GetRoleName());
	PlayerManager::Instance()->SendPacketAround(&damage_notify_msg, dwServerId, pPlayer->GetRoleId());

	return true;
}

// ȡ���а��б�
AllRoleRankList* CWorldBossManager::GetTypeRankList(BYTE byRankType)
{
	// �����������
	if (RANK_WORLD_BOSS_HONOR != byRankType && RANK_WORLD_BOSS_DAMAGE != byRankType)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����BOSS�����а����ͳ���[RankType:%d]!", __FUNCTION__, byRankType);
		return NULL;
	}

	BYTE byRankTypeIdx = RANK_WORLD_BOSS_HONOR == byRankType ? 0 : 1;

	m_vTempList.clear();
	map<DWORD, WorldBossRankList>::iterator all_rank_iter = m_AllRankList.begin();
	for (; all_rank_iter != m_AllRankList.end(); ++all_rank_iter)
	{
		for (int i = 0; i < CAMP_NUM; ++i)
		{
			RoleRankList &vRankList = all_rank_iter->second.vRankList[byRankTypeIdx][i];
			WORD wFindId = all_rank_iter->first * 10 + i;
			AllRoleRankList::iterator temp_rank_iter = m_vTempList.find(wFindId);
			if (temp_rank_iter != m_vTempList.end())
			{
				RoleRankList::iterator rank_data_iter = vRankList.begin();
				for (; rank_data_iter != vRankList.end(); ++rank_data_iter)
				{
					temp_rank_iter->second.push_back(*rank_data_iter);
				}
			}
			else
			{
				m_vTempList.insert(make_pair(wFindId, vRankList));
			}
		}
	}
	return &m_vTempList;
}

// ������а�����
void CWorldBossManager::AddRankData(BYTE byRankType, WORD wFindId, const RoleRankList &vRankList)
{
	WORD wServerId = wFindId * 0.1;
	BYTE byCamp = wFindId % 10;
	RoleRankList *pRankList = GetRankList(wServerId, byRankType, byCamp);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS���а�����[server_id:%d,rank_type:%d,camp:%d]",
			__FUNCTION__, wServerId, byRankType, byCamp);
		return;
	}

	RoleRankList::const_iterator iter = vRankList.begin();
	for (; iter != vRankList.end(); ++iter)
	{
		pRankList->push_back(*iter);
	}
}

// �Ƿ��к��
bool CWorldBossManager::IsHaveRedPoint(Player *pPlayer)
{
	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
		return false;

	// ��ûѡ����Ӫ
	if (CAMP_INVALID == pPlayerBossData->byCamp)
		return false;

	RoleRankData *pRankData = GetPlayerRankData(pPlayer->GetServerId(), RANK_WORLD_BOSS_HONOR, pPlayerBossData->byCamp, pPlayer->GetRoleId());
	if (!pRankData)
		return false;

	// �����������
	map<DWORD, WorldBossHonorReward_Config>::iterator iter = g_pCfgMgr->m_WorldBossHonorRewardList.begin();
	for (; iter != g_pCfgMgr->m_WorldBossHonorRewardList.end(); ++iter)
	{
		// ����ֵ����
		if (pRankData->dwValue < iter->second.nNeedHonor)
			continue;

		// ����Ƿ�����ȡ
		bool bIsTake = false;
		for (size_t i = 0; i < pPlayerBossData->vHonorRewardRecordList.size(); ++i)
		{
			// ����ȡ
			if (pPlayerBossData->vHonorRewardRecordList[i] == iter->second.wRewardId)
			{
				bIsTake = true;
				break;
			}
		}
		if (!bIsTake) return true;
	}

	// ���ս���̵�(todo::��ʱ����Ҫ����)

	return false;
}

// �������а�����
void CWorldBossManager::ResetRankData(WORD wServerId)
{
	m_AllRankList.erase(wServerId);
// 	map<DWORD, WorldBossRankList>::iterator list_iter = m_AllRankList.find(wServerId);
// 	if (list_iter != m_AllRankList.end())
// 	{
// 		m_AllRankList.erase(list_iter);
// 	}

	map<DWORD, PlayerWorldBoseData>::iterator iter = m_PlayerWorldBossDataList.begin();
	for (; iter != m_PlayerWorldBossDataList.end(); ++iter)
	{
		DWORD dwRoleServerId = PlayerBaseDataManager::Instance()->GetServerIdByRoleId(iter->first);
		if (wServerId == dwRoleServerId)
		{
			// ɾ������ָ����һ������
			iter = m_PlayerWorldBossDataList.erase(iter);
		}
	}
}

// �Ƿ񿪷�ʱ��
bool CWorldBossManager::IsStartTime()
{
	const WorldBossOpenTime_Config *pOpenTimeCfg = CConfigManager::Instance()->GetWorldBossOpenTime(m_nWeekDay);
	if (!pOpenTimeCfg)
	{
		//MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����ʱ������[WeekDay:%d]", __FUNCTION__, m_nWeekDay);
		return false;
	}
	tm tNow;
	GetTMTime(&tNow, time(NULL));
	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
	if (nCurTime < pOpenTimeCfg->nStartTime || nCurTime > pOpenTimeCfg->nOverTime)
		return false;

	return true;
}

// �Ƿ�����ʱ��
bool CWorldBossManager::IsOverTime()
{
	tm tNow;
	GetTMTime(&tNow, time(NULL));
	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
	return nCurTime >= m_nActivityTime ? true : false;
// 	const WorldBossOpenTime_Config *pOpenTimeCfg = CConfigManager::Instance()->GetWorldBossOpenTime(m_nWeekDay);
// 	if (!pOpenTimeCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����ʱ������[WeekDay:%d]", __FUNCTION__, m_nWeekDay);
// 		return false;
// 	}
// 	tm tNow;
// 	GetTMTime(&tNow, time(NULL));
// 	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
// 	return nCurTime >= pOpenTimeCfg->nOverTime ? true : false;
}

// �Ƿ񷢽���ʱ��
bool CWorldBossManager::IsRewardTime()
{
	tm tNow;
	GetTMTime(&tNow, time(NULL));
	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
	return nCurTime >= m_nActivityTime ? true : false;
// 	const WorldBossOpenTime_Config *pOpenTimeCfg = CConfigManager::Instance()->GetWorldBossOpenTime(m_nWeekDay);
// 	if (!pOpenTimeCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����ʱ������[WeekDay:%d]", __FUNCTION__, m_nWeekDay);
// 		return false;
// 	}
// 	tm tNow;
// 	GetTMTime(&tNow, time(NULL));
// 	int nCurTime = tNow.tm_hour * 100 + tNow.tm_min;
// 	return nCurTime >= pOpenTimeCfg->nRewardTime ? true : false;
}

// �Ƿ�BOSS������
bool CWorldBossManager::IsBossDead(WORD wServerId)
{
	WorldBossData &tWorldBossData = GetWorldBossData(wServerId);

	for (size_t i = 0; i < tWorldBossData.vMonsterList.size(); ++i)
	{
		if (tWorldBossData.vMonsterList[i].dwDamage < tWorldBossData.vMonsterList[i].dwTotalHP)
			return false;
	}

	return true;
}

// ��ǰBOSSʣ��Ѫ��
DWORD CWorldBossManager::GetBossRemainHP(WORD wServerId)
{
	WorldBossData &tWorldBossData = GetWorldBossData(wServerId);

	DWORD dwRemainHP = 0;
	for (size_t i = 0; i < tWorldBossData.vMonsterList.size(); ++i)
	{
		if (tWorldBossData.vMonsterList[i].dwDamage < tWorldBossData.vMonsterList[i].dwTotalHP)
		{
			dwRemainHP += (tWorldBossData.vMonsterList[i].dwTotalHP - tWorldBossData.vMonsterList[i].dwDamage);
		}
	}
	return dwRemainHP;
}

// ȡ��ѫ
DWORD CWorldBossManager::GetExploit()
{
	// �̶���ѫֵ
	DWORD dwExploit = g_GetConfigValue(57);

	// �󱩻�����(��ѡһ)
	const Server_Config *pCommonCfg = CConfigManager::Instance()->GetCommonCfg(59, true);
	if (!pCommonCfg || pCommonCfg->vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����BOSS����󱩻��������ó���!", __FUNCTION__);
		return dwExploit;
	}
	bool bCrit = g_Rand() <= pCommonCfg->vValueList[1] ? true : false;
	if (bCrit)
		dwExploit += pCommonCfg->vValueList[0];

	// ���˱�������(��ѡһ)
	if (!bCrit)
	{
		pCommonCfg = CConfigManager::Instance()->GetCommonCfg(58, true);
		if (!pCommonCfg || pCommonCfg->vValueList.size() != 2)
		{
			MAPLOG(ERROR_LEVEL, "[%s]����BOSS�������˽������ó���!", __FUNCTION__);
			return dwExploit;
		}
		bCrit = g_Rand() <= pCommonCfg->vValueList[1] ? true : false;
		if (bCrit)
			dwExploit += pCommonCfg->vValueList[0];
	}

	return dwExploit;
}

// ����˺�
void CWorldBossManager::AddRankData(Player *pPlayer, BYTE byRankType, DWORD dwValue)
{
	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ���ҵ��������BOSS����!", __FUNCTION__);
		return;
	}

	// ��ûѡ����Ӫ
	if (CAMP_INVALID == pPlayerBossData->byCamp)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ûѡ����Ӫ!", __FUNCTION__);
		return;
	}

	RoleRankData* pRankData = GetPlayerRankData(pPlayer->GetServerId(), byRankType, pPlayerBossData->byCamp, pPlayer->GetRoleId());
	if (pRankData)
	{
		// �������а�(ȡ�ۻ�)
		if (RANK_WORLD_BOSS_HONOR == byRankType)
		{
			pRankData->dwValue += dwValue;
			pRankData->dwUpdateTime = time(NULL);
		}
		// �˺����а�(ȡ��߼�¼)
		else
		{
			if (dwValue > pRankData->dwValue)
			{
				pRankData->dwValue = dwValue;
				pRankData->dwUpdateTime = time(NULL);
			}
		}
	}
	else
	{
		RoleRankList *pRankList = GetRankList(pPlayer->GetServerId(), byRankType, pPlayerBossData->byCamp);
		if (!pRankList)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS�����б�[ServerId:%d,RankType:%d,Camp:%d]!",
				__FUNCTION__, pPlayer->GetServerId(), byRankType, pPlayerBossData->byCamp);
			return;
		}

		RoleRankData RankData;
		RankData.dwRoleId = pPlayer->GetRoleId();
		RankData.dwValue = dwValue;
		RankData.wLevel = pPlayer->GetLevel();
		RankData.dwUpdateTime = time(NULL);
		RankData.wRank = pRankList->size() + 1;
		RankData.dwServerId = pPlayer->GetServerId();
		pRankList->push_back(RankData);
	}

	UpdateRank(pPlayer->GetServerId(), byRankType, pPlayerBossData->byCamp);
}

// ����BOSS�˺�
void CWorldBossManager::UpdateBossDamage(WORD wServerId, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	WorldBossData &tWorldBossData = GetWorldBossData(wServerId);

	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		vector<MonsterData>::iterator iter = tWorldBossData.vMonsterList.begin();
		for (; iter != tWorldBossData.vMonsterList.end(); ++iter)
		{
			if (pMonsterList[i].byIndex == iter->byIndex)
				iter->dwDamage += pMonsterList[i].dwDamage;
		}
	}
}

// ��ʼ��Boss
void CWorldBossManager::InitBoss(WORD wServerId)
{
	// ָ����������ʼ��
	if (wServerId)
	{
		WorldBossData &tWorldBossData = GetWorldBossData(wServerId);

		//tWorldBossData.wBossLevel = 1; // ���ʼ�����´�1����ʼ��
		tWorldBossData.bHaveLuckReward = true;
		tWorldBossData.dwReliveTime = 0;

		const WorldBoss_Config *pWorldBossCfg = CConfigManager::Instance()->GetWorldBoss(tWorldBossData.wBossLevel);
		if (!pWorldBossCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����[BossLv:%d]!", __FUNCTION__, tWorldBossData.wBossLevel);
			return;
		}

		tWorldBossData.vMonsterList.clear();
		if (!g_pCfgMgr->GetMapMonsterList(pWorldBossCfg->wMapId, &tWorldBossData.vMonsterList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ȡ����BOSS�����б�ʧ��[MapId:%d]!", __FUNCTION__, pWorldBossCfg->wMapId);
			return;
		}

		// ����㲥(boss����)
		CSystemNotice sys_notice(14, wServerId);
		sys_notice.AddParam(tWorldBossData.wBossLevel);
		sys_notice.SendMsg();
	}
	// ���з�������ʼ��
	else
	{
		const map<DWORD, ServerInfo> &mServerList = g_MapServer.GetServerList();
		map<DWORD, ServerInfo>::const_iterator iter = mServerList.begin();
		for (; iter != mServerList.end(); ++iter)
		{
			WorldBossData &tWorldBossData = GetWorldBossData(iter->second.dwServerId);

			tWorldBossData.wBossLevel = 1; // ���ʼ�����´�1����ʼ��
			tWorldBossData.bHaveLuckReward = true;
			tWorldBossData.dwReliveTime = 0;

			const WorldBoss_Config *pWorldBossCfg = CConfigManager::Instance()->GetWorldBoss(tWorldBossData.wBossLevel);
			if (!pWorldBossCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS����[BossLv:%d]!", __FUNCTION__, tWorldBossData.wBossLevel);
				return;
			}

			tWorldBossData.vMonsterList.clear();
			if (!g_pCfgMgr->GetMapMonsterList(pWorldBossCfg->wMapId, &tWorldBossData.vMonsterList))
			{
				MAPLOG(ERROR_LEVEL, "[%s]ȡ����BOSS�����б�ʧ��[MapId:%d]!", __FUNCTION__, pWorldBossCfg->wMapId);
				return;
			}

			MAPLOG(GUI_LEVEL, "����BOSS��ʼ���ɹ�[ServerId:%d, BossLv:%d, MapId:%d]!", 
				tWorldBossData.wServerId, tWorldBossData.wBossLevel, pWorldBossCfg->wMapId);
		}
	}
}

// ���н���
void CWorldBossManager::DoRankReward()
{
	//const WorldBossRankReward_Config *pRankRewardCfg;
	map<DWORD, WorldBossRankList>::iterator iter = m_AllRankList.begin();
	for (; iter != m_AllRankList.end(); ++iter)
	{
		for (BYTE i = CAMP_BEGIN; i <= CAMP_END; ++i)
		{
			// �������н���
			RoleRankList *pHonorRankList = &iter->second.vRankList[0][i];
			for (size_t k = 0; k < pHonorRankList->size(); ++k)
			{
				RoleRankData *pRankData = &pHonorRankList->at(k);
				const WorldBossRankReward_Config *pRewardCfg = g_pCfgMgr->GetWorldBossRankReward(pRankData->wRank);
				if (pRewardCfg)
				{
					// ���͵���������
					RewardData tRewardData;
					tRewardData.wInfoId = 4;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = pRankData->wRank;
					CRewardCenterManager::AddReward(iter->first, pRankData->dwRoleId, tRewardData);

					MAPLOG(ERROR_LEVEL, "[%s]����BOSS������������[role_id:%d,rank:%d]",
						__FUNCTION__, pRankData->dwRoleId, pRankData->wRank);
				}
			}

			// �˺����н���
			RoleRankList *pDamageRankList = &iter->second.vRankList[1][i];
			for (size_t k = 0; k < pDamageRankList->size(); ++k)
			{
				RoleRankData *pRankData = &pDamageRankList->at(k);
				const WorldBossRankReward_Config *pRewardCfg = g_pCfgMgr->GetWorldBossRankReward(pRankData->wRank);
				if (pRewardCfg)
				{
					// ���͵���������
					RewardData tRewardData;
					tRewardData.wInfoId = 5;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = pRankData->wRank;
					CRewardCenterManager::AddReward(iter->first, pRankData->dwRoleId, tRewardData);

					MAPLOG(ERROR_LEVEL, "[%s]����BOSS�����˺�����[role_id:%d,rank:%d]",
						__FUNCTION__, pRankData->dwRoleId, pRankData->wRank);
				}
			}
		}
	}
	m_nActivityStatus = ACTIVITY_IDLE; 
	MAPLOG(ERROR_LEVEL, "����BOSS����Ž���!");
}

// ȡ�����б�
RoleRankList* CWorldBossManager::GetRankList(DWORD dwServerId, BYTE byRanType, BYTE byCamp)
{
	map<DWORD, WorldBossRankList>::iterator iter = m_AllRankList.find(dwServerId);
	if (iter == m_AllRankList.end())
	{
		WorldBossRankList vRankList;
		m_AllRankList.insert(make_pair(dwServerId, vRankList));
		iter = m_AllRankList.find(dwServerId);
	}

	// �����������
	if (RANK_WORLD_BOSS_HONOR != byRanType && RANK_WORLD_BOSS_DAMAGE != byRanType)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����BOSS�����а����ͳ���[RankType:%d]!", __FUNCTION__, byRanType);
		return NULL;
	}

	// �����Ӫ
	if (byCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����BOSS�����а���Ӫ���ͳ���[Camp:%d]!", __FUNCTION__, byCamp);
		return NULL;
	}

	BYTE byRankTypeIdx = RANK_WORLD_BOSS_HONOR == byRanType ? 0 : 1;

	return &iter->second.vRankList[byRankTypeIdx][byCamp];
}

// ȡ�����б�
RoleRankList* CWorldBossManager::GetRankList(Player *pPlayer, BYTE byRanType, BYTE byCamp)
{
	return GetRankList(pPlayer->GetServerId(), byRanType, byCamp);
}

// ȡ�����������
RoleRankData* CWorldBossManager::GetPlayerRankData(DWORD dwServerId, BYTE byRanType, BYTE byCamp, DWORD dwRoleId)
{
	RoleRankList *pRankList = GetRankList(dwServerId, byRanType, byCamp);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�������BOSS�����б�[ServerId:%d,RankType:%d,Camp:%d]!",
			__FUNCTION__, dwServerId, byRanType, byCamp);
		return NULL;
	}

	for (RoleRankList::iterator iter = pRankList->begin(); iter != pRankList->end(); ++iter)
	{
		if (iter->dwRoleId == dwRoleId)
			return &*iter;
	}

	return NULL;
}

// ȡ�����������
RoleRankData* CWorldBossManager::GetPlayerRankData(Player *pPlayer, BYTE byRanType)
{
	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (!pPlayerBossData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵ�����BOSS����[ServerId:%d]!", __FUNCTION__, pPlayer->GetServerId());
		return NULL;
	}

	return GetPlayerRankData(pPlayer->GetServerId(), byRanType, pPlayerBossData->byCamp, pPlayer->GetRoleId());
}

// ȡ�������BOSS����
PlayerWorldBoseData* CWorldBossManager::GetPlayerWorldBossData(DWORD dwRoleId)
{
	map<DWORD, PlayerWorldBoseData>::iterator iter = m_PlayerWorldBossDataList.find(dwRoleId);
	if (iter == m_PlayerWorldBossDataList.end())
	{
		PlayerWorldBoseData tPlayerBossData;
		tPlayerBossData.byBattleTimes = 0;
		tPlayerBossData.byCamp = CAMP_INVALID;
		tPlayerBossData.byBuyBattleTimes = 0;
		tPlayerBossData.dwLastRecoveryTime = time(NULL);
		tPlayerBossData.vDamageRewardRecordList.clear();
		tPlayerBossData.vHonorRewardRecordList.clear();
		m_PlayerWorldBossDataList.insert(make_pair(dwRoleId, tPlayerBossData));
	}

	return iter != m_PlayerWorldBossDataList.end() ? &iter->second : NULL;
}

// ��������BOSS����
void CWorldBossManager::SendWorldBossData(Player *pPlayer)
{
	DWORD dwServerId = pPlayer->GetServerId();
	DWORD dwRoleId = pPlayer->GetRoleId();

	WorldBossData &tWorldBossData = GetWorldBossData(dwServerId);

	// UpdateBattleTimes(dwRoleId);

	MSG_S2C_WORLD_BOSS_DATA_RESP msg;
	msg.m_byCamp = CAMP_INVALID;
	msg.m_dwMyHonor = 0;
	msg.m_dwMyDamage = 0;
	msg.m_wMyHonorRank = 0;
	msg.m_wMyDamageRank = 0;
	msg.m_wBossLevel = tWorldBossData.wBossLevel;
	msg.m_byHonorRankNum = 0;
	msg.m_dwRemainHP = GetBossRemainHP(dwServerId);
	msg.m_byBattleTimes = 0;
	msg.m_dwLastRecoveryTime = 0;
	msg.m_byBuyBattleTimes = 0;
	msg.m_dwReliveTime = tWorldBossData.dwReliveTime;
	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(dwRoleId);
	if (pPlayerBossData && pPlayerBossData->byCamp != CAMP_INVALID)
	{
		msg.m_byCamp = pPlayerBossData->byCamp;
		msg.m_byBattleTimes = pPlayerBossData->byBattleTimes;
		msg.m_dwLastRecoveryTime = pPlayerBossData->dwLastRecoveryTime;
		msg.m_byBuyBattleTimes = pPlayerBossData->byBuyBattleTimes;
		
		RoleRankData *pRankData;

		// �ҵ�����
		pRankData = GetPlayerRankData(dwServerId, RANK_WORLD_BOSS_HONOR, pPlayerBossData->byCamp, dwRoleId);
		if (pRankData)
		{
			msg.m_dwMyHonor = pRankData->dwValue;
			msg.m_wMyHonorRank = pRankData->wRank;
		}
			

		// �ҵ��˺�
		pRankData = GetPlayerRankData(dwServerId, RANK_WORLD_BOSS_DAMAGE, pPlayerBossData->byCamp, dwRoleId);
		if (pRankData)
		{
			msg.m_dwMyDamage = pRankData->dwValue;
			msg.m_wMyDamageRank = pRankData->wRank;
		}
	}

	for (BYTE i = CAMP_BEGIN; i <= CAMP_END; ++i)
	{
		RoleRankList *pRankList = GetRankList(dwServerId, RANK_WORLD_BOSS_HONOR, i);
		ClientRoleRankData &tRankData = msg.m_HonorRankList[msg.m_byHonorRankNum++];
		memset(&tRankData, 0, sizeof(tRankData));
		if (pRankList && pRankList->size())
		{
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(dwServerId, pRankList->at(0).dwRoleId);
			tRankData = *pBaseData;
			tRankData.wRank = pRankList->at(0).wRank;
			tRankData.dwValue = pRankList->at(0).dwValue;
		}
	}
	pPlayer->SendPacket(&msg);
}

// ���ͽ�����ȡ��¼
void CWorldBossManager::SendRewardRecord(Player *pPlayer, BYTE byRewardType)
{
	MSG_S2C_WORLD_BOSS_REWARD_RECORD_RESP msg;
	msg.m_byRecordNum = 0;
	msg.m_byRewardType = byRewardType;

	PlayerWorldBoseData *pPlayerBossData = GetPlayerWorldBossData(pPlayer->GetRoleId());
	if (pPlayerBossData/* && pPlayerBossData->byCamp != CAMP_INVALID*/)
	{
		// ��������
		if (WORLD_BOSS_REWARD_HONOR == byRewardType)
		{
			for (size_t i = 0; i < pPlayerBossData->vHonorRewardRecordList.size(); ++i)
			{
				if (msg.m_byRecordNum >= MAX_WORLD_BOSS_AWARD_NUM)
					break;
				msg.m_RecordList[msg.m_byRecordNum++] = pPlayerBossData->vHonorRewardRecordList[i];
			}
		}
		// �˺�����
		else
		{
			for (size_t i = 0; i < pPlayerBossData->vDamageRewardRecordList.size(); ++i)
			{
				if (msg.m_byRecordNum >= MAX_WORLD_BOSS_AWARD_NUM)
					break;
				msg.m_RecordList[msg.m_byRecordNum++] = pPlayerBossData->vDamageRewardRecordList[i];
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// ���ս��
void CWorldBossManager::AddBattleRecord(WORD wServerId, const char *pszLuckAttackName, const char *pszKillName)
{
	WorldBossData &tWorldBossData = GetWorldBossData(wServerId);

	// ����ǵ�ǰ�ȼ��������
	for (size_t i = 0; i < tWorldBossData.vBattleRecordList.size(); ++i)
	{
		if (tWorldBossData.vBattleRecordList[i].wBossLevel == tWorldBossData.wBossLevel)
		{
			if (pszLuckAttackName)
			{
				strcpy_s(tWorldBossData.vBattleRecordList[i].szLuckAttackName, MAX_ROLE_LEN - 1, pszLuckAttackName);
				tWorldBossData.vBattleRecordList[i].dwLuckAttackTime = time(NULL);
			}
				
			if (pszKillName)
			{
				strcpy_s(tWorldBossData.vBattleRecordList[i].szKillName, MAX_ROLE_LEN - 1, pszKillName);
				tWorldBossData.vBattleRecordList[i].dwKillTime = time(NULL);
			}			
			return;
		}
	}

	WorldBossBattleRecord tRecord;
	memset(&tRecord, 0, sizeof(tRecord));
	tRecord.wBossLevel = tWorldBossData.wBossLevel;
	if (pszLuckAttackName)
	{
		strcpy_s(tRecord.szLuckAttackName, MAX_ROLE_LEN - 1, pszLuckAttackName);
		tRecord.dwLuckAttackTime = time(NULL);
	}
	if (pszKillName)
	{
		strcpy_s(tRecord.szKillName, MAX_ROLE_LEN - 1, pszKillName);
		tRecord.dwKillTime = time(NULL);
	}

	tWorldBossData.vBattleRecordList.push_back(tRecord);
}

// ������ս����
void CWorldBossManager::UpdateBattleTimes(DWORD dwRoleId)
{
	PlayerWorldBoseData *pBossData = GetPlayerWorldBossData(dwRoleId);
	if (pBossData && pBossData->byCamp != CAMP_INVALID)
	{
		BYTE byTimes = DoRecoveryTimes(pBossData->dwLastRecoveryTime, time(NULL), g_GetConfigValue(151));
		pBossData->byBattleTimes = pBossData->byBattleTimes > byTimes ? pBossData->byBattleTimes - byTimes : 0;
	}
}

// ȡ����BOSS����
WorldBossData& CWorldBossManager::GetWorldBossData(WORD wServerId)
{
	map<DWORD, WorldBossData>::iterator iter = m_WorldBossList.find(wServerId);
	if (iter == m_WorldBossList.end())
	{
		WorldBossData tWorldBossData;
		tWorldBossData.wServerId = wServerId;
		tWorldBossData.wBossLevel = 1;
		tWorldBossData.dwReliveTime = 0;
		m_WorldBossList.insert(make_pair(wServerId, tWorldBossData));
		iter = m_WorldBossList.find(wServerId);
	}
	return iter->second;
}

bool RoleRankDataPredEx(RoleRankData &data1, RoleRankData &data2)
{
	// ��һ����ֵ�����
	if (data1.dwValue > data2.dwValue)
		return true;

	// ��һ����ֵ���
	if (data1.dwValue == data2.dwValue && data1.dwParam1 > data2.dwParam1)
		return true;

	// ��һ,������ֵ�����
	if (data1.dwValue == data2.dwValue && data1.dwParam1 == data2.dwParam1 && data1.dwUpdateTime < data2.dwUpdateTime)
		return true;

	return false;
}

// �������а�
void CWorldBossManager::UpdateRank(DWORD dwServerId, BYTE byRanType, BYTE byCamp)
{
	RoleRankList *pRankList = GetRankList(dwServerId, byRanType, byCamp);
	if (pRankList)
	{
		std::sort(pRankList->begin(), pRankList->end(), RoleRankDataPredEx);
	}

	// ��������
	pRankList = GetRankList(dwServerId, byRanType, byCamp);
	if (pRankList)
	{
		WORD wRank = 0;
		vector<RoleRankData>::iterator iter = pRankList->begin();
		for (; iter != pRankList->end(); ++iter)
		{
			iter->wRank = ++wRank;
		}
	}
}
