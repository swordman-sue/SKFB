#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "DailyMissionManager.h"


CDailyMissionManager::CDailyMissionManager()
{

}

CDailyMissionManager::~CDailyMissionManager()
{

}

void CDailyMissionManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pDailyMissionInfo = &pPlayer->GetRoleFullInfo().tDailyMissionInfo;
}

// ������������
void CDailyMissionManager::OnNewDay()
{
	memset(m_pDailyMissionInfo, 0, sizeof(PlayerDailyMissionData));
}

// ���������б�
void CDailyMissionManager::OnNetMissionList()
{
	SendMissionList();
}

// ��ȡ������
void CDailyMissionManager::OnNetMissionAward(WORD wMissionId)
{
	// ȡ�ɾ�����
	const DailyMission_Config *pMissionCfg = CConfigManager::Instance()->GetDailyMission(wMissionId);
	if (!pMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ����ճ�������������![MissionId:%d]", wMissionId);
		return;
	}

	//  ��ҳɾ�����
	DailyMissionData *pMissionData = GetMissionData(wMissionId);
	if (!pMissionData)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ�����ҵ��ճ���������![MissionId:%d]", wMissionId);
		return;
	}

	// ����Ƿ�����ȡ
	if (pMissionData->byIsGetAward)
	{
		MAPLOG(ERROR_LEVEL, "�ճ�����������ȡ![MissionId:%d]", wMissionId);
		return;
	}

	// ����Ƿ����ȡ
	if (pMissionData->dwValue < pMissionCfg->dwValue)
	{
		MAPLOG(ERROR_LEVEL, "�ճ�����û��ɲ�����ȡ![MissionId:%d]", wMissionId);
		return;
	}

	// ����
	m_pPlayer->AddObjectList(pMissionCfg->vRewardItemList, TRUE, FROM_DAILY_MISSION_REWARD);

	// ��û���
	m_pDailyMissionInfo->wPoint += pMissionCfg->wPoint;

	// ��þ���ֵ
	m_pPlayer->AddExp(pMissionCfg->dwBaseExp * m_pPlayer->GetDailyInitLevel());

	// ��¼����ȡ����
	pMissionData->byIsGetAward = true;

	SendMissionData(pMissionData);

 	MSG_S2C_DAILY_MISSION_AWARD_RESP msg;
	msg.m_wMissionId = wMissionId;
 	m_pPlayer->SendPacket(&msg);
}

// ���ֽ���
void CDailyMissionManager::OnNetPointAward(WORD wPoint)
{
	// ����Ƿ�����ȡ��
	if (CheckPointAward(wPoint))
	{
		MAPLOG(ERROR_LEVEL, "�ճ�������ֽ�������ȡ��![Point:%d]", wPoint);
		return;
	}

	const RoleLvDailyMission_Config *pMissonLvCfg = CConfigManager::Instance()->GetRoleLvDailyMission(m_pPlayer->GetLevel());
	if (!pMissonLvCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ����ȼ��ճ���������![Level:%d]", m_pPlayer->GetLevel());
		return;
	}

	const RoleLvDailyMission_Config::PointData *pPointCfg = pMissonLvCfg->GetPointData(wPoint);
	if (!pPointCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ����ճ������������![score:%d]", wPoint);
		return;
	}

	m_pPlayer->AddObject(pPointCfg->wRewardItemId, TRUE, FROM_DAILY_MISSION_SCORE_REWARD);

	AddPointAwardRecord(wPoint);

	MSG_S2C_DAILY_MISSION_POINT_AWARD_RESP msg;
	msg.m_wPoint = wPoint;
	m_pPlayer->SendPacket(&msg);
}

// ���������б�
void CDailyMissionManager::SendMissionList()
{
	MSG_S2C_DAILY_MISSION_LIST_RESP msg;
	msg.m_DailyMission = *m_pDailyMissionInfo;
	m_pPlayer->SendPacket(&msg);
}

// ������������
void CDailyMissionManager::SendMissionData(const DailyMissionData *pData)
{
	MSG_S2C_DAILY_MISSION_DATA_NOTIFY msg;
	msg.m_DailyMissionData = *pData;
	m_pPlayer->SendPacket(&msg);
}

// ��������
void CDailyMissionManager::DoUpdateMission(WORD wMissionId, int nValue, bool bAdd)
{
	const DailyMission_Config *pMissionCfg = CConfigManager::Instance()->GetDailyMission(wMissionId);
	if (!pMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ����ճ�������������![MissionId:%d]", wMissionId);
		return;
	}

	//  ��ҳɾ�����
	DailyMissionData *pMissionData = GetMissionData(wMissionId);
	if (!pMissionData)
	{
		DailyMissionData tData;
		tData.wMissionId = wMissionId;
		tData.dwValue = 0;
		tData.byIsGetAward = false;
		AddMissionData(tData);
		pMissionData = GetMissionData(wMissionId);
		if (!pMissionData)
			return;
	}

	// ���������
	if (pMissionData->dwValue >= pMissionCfg->dwValue)
		return;

	// ���ֵ
	if (bAdd)
	{
		pMissionData->dwValue += nValue;

		// ��ֹ��������
		pMissionData->dwValue = min(pMissionData->dwValue, pMissionCfg->dwValue);
	}
	// ����ֵ
	else
	{		// ʹ����Ѽ�¼
		if (nValue <= pMissionData->dwValue)
			return;

		pMissionData->dwValue = nValue;
	}

	SendMissionData(pMissionData);

	// ��ɫ��Ϣ����
	m_pPlayer->RoleInfoUpdate();
}

// ��������
void CDailyMissionManager::OnUpdateMission(WORD wType, int nValue, bool bAdd)
{
	// ȡ�ȼ���Ӧ�������б�
	WORD wInitTeamLv = m_pPlayer->GetProperty(ROLE_PRO_DAILY_INIT_LEVEL);
	const RoleLvDailyMission_Config *pRoleLvMissionCfg = CConfigManager::Instance()->GetRoleLvDailyMission(wInitTeamLv);
	if (!pRoleLvMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "ȡ����ս�ӵȼ���Ӧ���ճ���������![TeamLevel:%d]", wInitTeamLv);
		return;
	}

	const vector<WORD> *pMissionList = CConfigManager::Instance()->GetTypeDailyMissionList(wType);
	if (!pMissionList)
	{
		MAPLOG(ERROR_LEVEL, "ȡ�����ճ����͵������б�![type:%d]", wType);
		return;
	}

	for (size_t i = 0; i < pMissionList->size(); ++i)
	{
		WORD wMissionId = pMissionList->at(i);
		if (pRoleLvMissionCfg->IsMission(wMissionId))
		{
			DoUpdateMission(wMissionId, nValue, bAdd);
			break;
		}
	}
}

// �Ƿ��к��
bool CDailyMissionManager::IsHaveRedPoint()
{
	// ��⿪�ŵȼ�
	if (m_pPlayer->GetLevel() < g_pCfgMgr->GetSystemOpenLevel(SYSTEM_DAILY_MISSION))
		return false;

	// ȡ�ȼ���Ӧ�������б�
	WORD wInitRoleLevel = m_pPlayer->GetProperty(ROLE_PRO_DAILY_INIT_LEVEL);
	const RoleLvDailyMission_Config *pRoleLvMissionCfg = CConfigManager::Instance()->GetRoleLvDailyMission(wInitRoleLevel);
	if (!pRoleLvMissionCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ����ս�ӵȼ���Ӧ���ճ���������![TeamLevel:%d]", __FUNCTION__, wInitRoleLevel);
		return false;
	}

	// ���ճ�������������ɵ����񣬿���ȡ������ʱ
	for (WORD i = 0; i < m_pDailyMissionInfo->wDailyMissionNum; ++i)
	{
		DailyMissionData *pMssionData = &m_pDailyMissionInfo->arDailyMissionList[i];
		// �Ѿ���ȡ��
		if (pMssionData->byIsGetAward)
			continue;

		const DailyMission_Config *pMissionCfg = g_pCfgMgr->GetDailyMission(pMssionData->wMissionId);
		if (!pMissionCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ճ�������������![MissionId:%d]", __FUNCTION__, pMssionData->wMissionId);
			return false;
		}

		// �Ѵ����ȡ����(Ŀ��ֵ,��ɫ�ȼ�)
		if (pMssionData->dwValue >= pMissionCfg->dwValue && m_pPlayer->GetLevel() >= pMissionCfg->wNeedRoleLv)
			return true;
	}

	// ���ճ�������,�п���ȡ�Ļ��ֱ��佱��ʱ
	for(size_t i = 0; i < pRoleLvMissionCfg->vPointList.size(); ++i)
	{
		const RoleLvDailyMission_Config::PointData *pPointDataCfg = &pRoleLvMissionCfg->vPointList[i];
		// ����������,��ľ����ȡ��
		if (m_pDailyMissionInfo->wPoint >= pPointDataCfg->wPoint && !CheckPointAward(pPointDataCfg->wPoint))
			return true;
	}

	return false;
}

// ȡ��������
DailyMissionData* CDailyMissionManager::GetMissionData(WORD wMissionId)
{
	for (WORD i = 0; i < m_pDailyMissionInfo->wDailyMissionNum; ++i)
	{
		if (m_pDailyMissionInfo->arDailyMissionList[i].wMissionId == wMissionId)
		{
			return &m_pDailyMissionInfo->arDailyMissionList[i];
		}
	}
	return NULL;
}

// �����������
void CDailyMissionManager::AddMissionData(const DailyMissionData &tMission)
{
	if (m_pDailyMissionInfo->wDailyMissionNum >= MAX_DAILY_MISSION_NUM)
	{
		MAPLOG(ERROR_LEVEL, "�ճ������б�����!");
		return;
	}

	m_pDailyMissionInfo->arDailyMissionList[m_pDailyMissionInfo->wDailyMissionNum++] = tMission;
}

// ����Ƿ�����ȡ��
bool CDailyMissionManager::CheckPointAward(WORD wPoint)
{
	for (WORD i = 0; i < m_pDailyMissionInfo->wPointAwardRecordNum; ++i)
	{
		if (m_pDailyMissionInfo->arPointAwardRecord[i] == wPoint)
		{
			return true;
		}
	}
	return false;
}

// ��ӻ��ֽ�������
void CDailyMissionManager::AddPointAwardRecord(WORD wPoint)
{
	if (m_pDailyMissionInfo->wPointAwardRecordNum >= MAX_DAILY_MISSION_POINT_AWARD)
	{
		MAPLOG(ERROR_LEVEL, "�ճ�������ֽ����б�����!");
		return;
	}

	m_pDailyMissionInfo->arPointAwardRecord[m_pDailyMissionInfo->wPointAwardRecordNum++] = wPoint;
}