#include "stdafx.h"
#include "EndlessLandManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\AccountInfo.h"
#include "..\Rank\RankManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Activity\ActivityManager.h"

CEndlessLandManager::CEndlessLandManager()
{
	m_bIsInBattle = false;
}

CEndlessLandManager::~CEndlessLandManager()
{
		
}

void CEndlessLandManager::Init(Player *pPlayer)
{
	m_bIsInBattle = false;
	m_pPlayer = pPlayer;
	m_pEndlessLandData = &pPlayer->GetRoleFullInfo().tSecondInfo.tEndlessLandData;
}

// ����֮����������
void CEndlessLandManager::OnNetEndlessData()
{
	SendEndlessLandData();
}

// ����ս��
void CEndlessLandManager::OnNetEnter()
{
	// ��ǰ��ս����
	WORD wCurBattleTimes = m_pEndlessLandData->wTodayBattleTimes + 1;

	// ���ÿ�տ���ս����
	WORD wDailyBattleTimes = m_pPlayer->GetVipValue(VIP_ENDLESS_LAND_DAILY_BATTLE_TIMES);
	if (wCurBattleTimes > wDailyBattleTimes)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������ս�����Ѵ�����[MaxBattleTimes:%d]", __FUNCTION__, wDailyBattleTimes);
		return;
	}

	// ȡ����
	const EndlessLandBattleCost_Config *pCostCfg = g_pCfgMgr->GetEndlessLandBattleCost(wCurBattleTimes);
	if (!pCostCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�����޾�֮����ս��������[BattleTimes:%d]", __FUNCTION__, wCurBattleTimes);
		return;
	}

	// �������
	if (m_pPlayer->GetItemNum(pCostCfg->tCostItem.dwId) < pCostCfg->tCostItem.dwNum && 
		m_pPlayer->GetProperty(pCostCfg->tCostCurrency.wType) < pCostCfg->tCostCurrency.nValue)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮����ս���Ĳ���,������ս[BattleTimes:%d]", __FUNCTION__, wCurBattleTimes);
		return;
	}

	// �������������Ĳ���
	if (m_pEndlessLandData->wHistoryWaveRecord)
	{
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(m_pEndlessLandData->wHistoryWaveRecord);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_ENTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[wave:%d]", __FUNCTION__, m_pEndlessLandData->wHistoryWaveRecord);
			return;
		}

		m_pEndlessLandData->wCurWave = pWaveCfg->wInitWave;
	}

	// ��ɱ�������¿�ʼ����
	m_pEndlessLandData->wCurContinueKillNum = 0;

	// ÿ���״���ս�н��� todo::�Ѿ�����Ϊÿ����ս���н���
	for (WORD i = 1; i < m_pEndlessLandData->wCurWave; ++i)
	{
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(i);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_ENTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[wave:%d]", __FUNCTION__, i);
			return;
		}

		// ������
		m_pPlayer->ModifyProperty(ROLE_PRO_ENDLESS_STONE, pWaveCfg->nEndlessStone, TRUE, FROM_ENDLESS_ENTER_BATTLE);
		m_pPlayer->AddObjectList(pWaveCfg->vRewardItemList, TRUE, FROM_ENDLESS_ENTER_BATTLE);

		// ������
		for (int k = 0; k < pWaveCfg->vDropDataList.size(); ++k)
		{
			const DropData &sDropData = pWaveCfg->vDropDataList[k];
			if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < sDropData.wRate)
			{
				m_pPlayer->AddObject(sDropData.dwDropObjectId, sDropData.wDropObjectNum, TRUE, FROM_ENDLESS_ENTER_BATTLE);
			}
		}

		// ��¼��ɱ����
		//m_pEndlessLandData->wCurContinueKillNum += pWaveCfg->wMonsterNum;
	}

	// ������ɱ����
	for (WORD i = 1; i < m_pEndlessLandData->wCurWave; ++i)
	{
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(i);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_ENTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[wave:%d]", __FUNCTION__, i);
			return;
		}
		// ��¼��ɱ����
		m_pEndlessLandData->wCurContinueKillNum += pWaveCfg->wMonsterNum;
	}

	// �޾���Ԩ��ɱ%sֻ����
	m_pPlayer->OnUpdateSevenDayTarget(24, m_pEndlessLandData->wCurContinueKillNum, true);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	// ���ջ�ԾĿ��
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 9, m_pEndlessLandData->wCurContinueKillNum, true);

	// ��������(��Ʒ)
	if (m_pPlayer->GetItemNum(pCostCfg->tCostItem.dwId) >= pCostCfg->tCostItem.dwNum)
	{
		m_pPlayer->RemoveItem(pCostCfg->tCostItem.dwId, pCostCfg->tCostItem.dwNum, FROM_ENDLESS_ENTER_BATTLE);
	}
	// ��������(����)
	else
	{
		m_pPlayer->ModifyProperty(pCostCfg->tCostCurrency.wType, -pCostCfg->tCostCurrency.nValue, TRUE, FROM_ENDLESS_ENTER_BATTLE);
	}

	//m_pEndlessLandData->wTodayWaveRecord = m_pEndlessLandData->wCurWave - 1;

	m_pEndlessLandData->wIsFirstBattle = false;
	m_pEndlessLandData->wTodayBattleTimes = wCurBattleTimes;

	m_bIsInBattle = true;

	// ͬ������
	SendEndlessLandData();

	// ��Ӧ
	MSG_S2C_ENDLESS_LAND_ENTER_RESP msg;
	m_pPlayer->SendPacket(&msg);
 
	// �ճ�����(�����޾���Ԩ�ش�)
	m_pPlayer->OnUpdateDailyMission(23);

	// ���ջ(�ۼ���ս�޾���Ԩ����)
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 17, 1, true);

	// ���ջ�ԾĿ��(�ۼ���ս�޾���Ԩ����)
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 17, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 17, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 17, 1, true);
}

// ��ʼս��
bool CEndlessLandManager::OnNetBattleStart()
{
	if (!m_bIsInBattle)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_START_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮�ػ�û����ս��״̬[roleid:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return false;
	}

// 	MSG_S2C_ENDLESS_LAND_BATTLE_START_RESP msg;
// 	m_pPlayer->SendPacket(&msg);

	return true;
}

// ��ʼ����
bool CEndlessLandManager::OnNetBattleOver(BYTE byIsWin, WORD wWave)
{
	if (!m_bIsInBattle)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮�ػ�û����ս��״̬[roleid:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return false;
	}

	// ��Ⲩ���Ƿ�������
	if (wWave != m_pEndlessLandData->wCurWave)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮����ս��������[roleid:%d,wave:%d]", __FUNCTION__, m_pPlayer->GetRoleId(), wWave);
		return false;
	}

	int nMultiple = CActivityManager::GetLimitProductMultiple(BATTLE_ENDLESS_LAND);

	WORD wCurRewardWave = 0; // m_pEndlessLandData->wCurWave;

	if (byIsWin)
	{
		// ������
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(m_pEndlessLandData->wCurWave);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[wave:%d]", __FUNCTION__, m_pEndlessLandData->wCurWave);
			return false;
		}

		// ����ɱ�Ĺ��Ƿ��㹻
		if (m_pEndlessLandData->wCurWaveKillNum < pWaveCfg->wMonsterNum)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_BATTLE_OVER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]��ɱ�������������[wave:%d,cur_kill_num:%d,need_kill_num:%d]", __FUNCTION__,
				m_pEndlessLandData->wCurWave, m_pEndlessLandData->wCurWaveKillNum, pWaveCfg->wMonsterNum);
			return false;
		}

		// ����ÿ��ֻ����һ�ν���
		//if (m_pEndlessLandData->wCurWave > m_pEndlessLandData->wTodayWaveRecord)
		//{
			// ������
			m_pPlayer->ModifyProperty(ROLE_PRO_ENDLESS_STONE, pWaveCfg->nEndlessStone * nMultiple, TRUE, FROM_ENDLESS_LAND_BATTLE_OVER);
			m_pPlayer->AddObjectList(pWaveCfg->vRewardItemList, TRUE, FROM_ENDLESS_LAND_BATTLE_OVER);

			// ������
			for (int k = 0; k < pWaveCfg->vDropDataList.size(); ++k)
			{
				const DropData &sDropData = pWaveCfg->vDropDataList[k];
				if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < sDropData.wRate)
				{
					m_pPlayer->AddObject(sDropData.dwDropObjectId, sDropData.wDropObjectNum, TRUE, FROM_ENDLESS_LAND_BATTLE_OVER);
				}
			}

			wCurRewardWave = m_pEndlessLandData->wCurWave;
		//}

		m_pEndlessLandData->wTodayWaveRecord = max(m_pEndlessLandData->wTodayWaveRecord, m_pEndlessLandData->wCurWave);
		m_pEndlessLandData->wHistoryWaveRecord = max(m_pEndlessLandData->wHistoryWaveRecord, m_pEndlessLandData->wCurWave);

		// ����һ��
		m_pEndlessLandData->wCurWave++;

		// ��һ����ɱ����������
		m_pEndlessLandData->wCurWaveKillNum = 0;

		// �ɾ��¼�(��Ԩ���ɱ��Xֻ)
		m_pPlayer->OnUpdateAchievement(25, m_pEndlessLandData->wHistoryWaveRecord, false);
	}

	// ��Ӧ
	MSG_S2C_ENDLESS_LAND_BATTLE_OVER_RESP msg;
	msg.m_byIsWin = byIsWin;
	msg.m_wRewardWave = wCurRewardWave;
	m_pPlayer->SendPacket(&msg);

	return true;
}

// ɱ��
void CEndlessLandManager::OnNetKillMonster(BYTE byIndex, DWORD dwMonsterId)
{
	if (!m_bIsInBattle)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮�ػ�û����ս��״̬[roleid:%d]", __FUNCTION__, m_pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_ENDLESS_LAND_KILL_MONSTER_RESP msg;
	msg.m_byIndex = byIndex;
	msg.m_dwMonsterId = dwMonsterId;
	msg.m_wDropItemNum = 0;

	// ���������ս�״ν���
	if (m_pEndlessLandData->wCurWave > m_pEndlessLandData->wTodayWaveRecord)
	{
		// ȡ����
		const EndlessLandWave_Config *pWaveCfg = g_pCfgMgr->GetEndlessLandWave(m_pEndlessLandData->wCurWave);
		if (!pWaveCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[wave:%d]", __FUNCTION__, m_pEndlessLandData->wCurWave);
			return;
		}

		// ����±�ĺϷ���
		if (byIndex >= pWaveCfg->vMonsterList.size())
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�����±�Խ��[index:%d]", __FUNCTION__, byIndex);
			return;
		}

		DWORD dwMonsterId = pWaveCfg->vMonsterList[byIndex];
		const Monster_Config *pMonsterCfg = g_pCfgMgr->GetMonster(dwMonsterId);
		if (!pMonsterCfg)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KILL_MONSTER_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������������[index:%d]", __FUNCTION__, byIndex);
			return;
		}

		// ����������
		for (int k = 0; k < pMonsterCfg->vDropList.size(); ++k)
		{
			const DropData &sDropData = pMonsterCfg->vDropList[k];
			if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < sDropData.wRate)
			{
				m_pPlayer->AddObject(sDropData.dwDropObjectId, sDropData.wDropObjectNum, TRUE, FROM_ENDLESS_LAND_KILL_MONSTER);
				
				if (msg.m_wDropItemNum < MAX_MAP_DROP_NUM)
				{
					MonsterDropData &tDropData = msg.m_DropItemList[msg.m_wDropItemNum++];
					tDropData.dwObjId = sDropData.dwDropObjectId;
					tDropData.wObjNum = sDropData.wDropObjectNum;
				}
			}
		}
	}

	// ��ɱ��������
	m_pEndlessLandData->wCurContinueKillNum++;
	m_pEndlessLandData->wCurWaveKillNum++;

	m_pEndlessLandData->wContinueKillNumRecord = max(m_pEndlessLandData->wContinueKillNumRecord, m_pEndlessLandData->wCurContinueKillNum);

	// �������а�
	CRankManager::Instance()->AddRoleRankData(RANK_ENDLESS_KILL, m_pPlayer, m_pEndlessLandData->wContinueKillNumRecord);

	// �޾���Ԩ��ɱ%sֻ����
	m_pPlayer->OnUpdateSevenDayTarget(24, 1, true);

	// ���ջ
	m_pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 9, 1, true);

	// ���ջ�ԾĿ��
	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 9, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 9, 1, true);

	m_pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 9, 1, true);

	// ��Ӧ
	m_pPlayer->SendPacket(&msg);

// 	MAPLOG(ERROR_LEVEL, "[%s]�޾�֮��ɱ���ϱ�[role_id:%d,cur_wave:%d,cur_kill_num:%d]",
// 		__FUNCTION__, m_pPlayer->GetRoleId(), m_pEndlessLandData->wCurWave, m_pEndlessLandData->wCurWaveKillNum);
}

// �һ���ײ
void CEndlessLandManager::OnNetCollision()
{
	const EndlessLandCollision_Config *pCollisionCfg = NULL;

	// �����մ����Ƿ��Ѵ�����
	if (m_pEndlessLandData->wTodayCollisionRewardTimes < g_GetConfigValue(176))
	{
		// ȡ�����б�
		const vector<EndlessLandCollision_Config> &vCollisionList = g_pCfgMgr->GetEndlessLandCollisionList();

		// �����ܸ���
// 		vector<EndlessLandCollision_Config>::const_iterator iter = vCollisionList.begin();
// 		DWORD dwTotalRate = 0;
// 		for (; iter != vCollisionList.end(); ++iter)
// 		{
// 			dwTotalRate += iter->dwRate;
// 		}

		// ���
		DWORD dwRandValue = g_Rand();

		// �����Ǹ�����
		vector<EndlessLandCollision_Config>::const_iterator iter = vCollisionList.begin();
		DWORD dwCurRate = 0;
		for (; iter != vCollisionList.end(); ++iter)
		{
			dwCurRate += iter->dwRate;
			if (dwRandValue < dwCurRate)
			{
				pCollisionCfg = &*iter;
				break;
			}
		}

		// ����
		if (pCollisionCfg)
			m_pPlayer->AddObjectList(pCollisionCfg->vRewardItemList, TRUE, FROM_ENDLESS_LAND_COLLISION);

		++m_pEndlessLandData->wTodayCollisionRewardTimes;
	}

	// ��Ӧ
	MSG_S2C_ENDLESS_LAND_COLLISION_RESP msg;
	msg.m_wTodayCollisionRewardTimes = m_pEndlessLandData->wTodayCollisionRewardTimes;
	msg.m_wRewardId = pCollisionCfg ? pCollisionCfg->wRewardId : 0;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ��ɱ����
void CEndlessLandManager::OnNetTakeKeepKillReward(WORD wKeepKillNum)
{
	// ȡ��������
	const EndlessLandContinueKillReward_Config *pRewardCfg = g_pCfgMgr->GetEndlessLandKeepKillReward(wKeepKillNum);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����޾�֮����ɱ��������[kill_num:%d]", __FUNCTION__, wKeepKillNum);
		return;
	}

	// �����ɱ�������Ƿ��㹻
	if (m_pEndlessLandData->wContinueKillNumRecord < wKeepKillNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮����ɱ��������,������ȡ[kill_num:%d]", __FUNCTION__, wKeepKillNum);
		return;
	}


	// ����Ƿ��Ѿ���ȡ��
	if (wKeepKillNum <= m_pEndlessLandData->wLastTakeContinueKillReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ENDLESS_LAND_KEEP_KILL_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮����ɱ��ȡ����ȡ��[kill_num:%d]", __FUNCTION__, wKeepKillNum);
		return;
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, pRewardCfg->nDiamond, TRUE, FROM_ENDLESS_KEEP_KILL_REWARD);

	m_pEndlessLandData->wLastTakeContinueKillReward = wKeepKillNum;

	SendEndlessLandData();

	// ��Ӧ
	MSG_S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP msg;
	msg.m_wKeepKillNum = wKeepKillNum;
	m_pPlayer->SendPacket(&msg);

	// �������Ԩ�д��Ŀ������ʯ����ʱ����ȫ�����й���
	// �������ݣ�������������޾���Ԩ��ɱ�ķ����ۼ�ɱ��XXXX�������Y��ʯ��
	CSystemNotice sys_notice(17, m_pPlayer->GetServerId());
	sys_notice.AddParam(m_pPlayer->GetRoleName());
	sys_notice.AddParam(wKeepKillNum);
	sys_notice.AddParam(pRewardCfg->nDiamond);
	sys_notice.SendMsg();
}

// �����޾�֮������
void CEndlessLandManager::SendEndlessLandData()
{
	MSG_S2C_ENDLESS_LAND_DATA_RESP msg;
	msg.m_tEndlessLandData = *m_pEndlessLandData;
	m_pPlayer->SendPacket(&msg);
}

// ȡ�����ɱ��¼
DWORD CEndlessLandManager::GetContinueKillNumRecord()
{
	return m_pEndlessLandData->wContinueKillNumRecord;
}

// ����Ƿ�����ȡ����ɱ����
// bool CEndlessLandManager::IsTakeKeepKillReward(WORD wKeepKillNum)
// {
// 	for (WORD i = 0; i < m_pEndlessLandData->wContinueKillRewardRecordNum; ++i)
// 	{
// 		if (m_pEndlessLandData->arContinueKillRewardRecord[i] == wKeepKillNum)
// 			return true;
// 	}
// 	return false;
// }