#include "stdafx.h"
#include "WorldLevelTaskManager.h"
#include "..\Player.h"
#include "..\MapServer.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"

CWorldLevelTaskMananger::CWorldLevelTaskMananger()
{

}

CWorldLevelTaskMananger::~CWorldLevelTaskMananger()
{

}

void CWorldLevelTaskMananger::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_PlayerWorldLevelTaskData = &pPlayer->GetRoleFullInfo().tSecondInfo.tRoleWorldLevelTaskData;
}

void CWorldLevelTaskMananger::OnNewDay()
{
	UpdateTask();
}

// ��������ȼ���������
void CWorldLevelTaskMananger::OnNetWorldLevelTaskData()
{
	// ��û��ʼ�������
	if (!m_PlayerWorldLevelTaskData->wTaskEventNum)
		UpdateTask();

	SendWorldLevelTaskData();
}

// ��ȡ������
void CWorldLevelTaskMananger::OnNetTakeTaskReward(WORD wTaskId)
{
	// ȡ��������
	WorldLevelTaskEvent *pTaskEvent = GetTaskEvent(wTaskId);
	if (!pTaskEvent)
	{
		MAPLOG(ERROR_LEVEL, "���û�д�����ȼ�����[TaskId:%d]", wTaskId);
		return;
	}

	// ����Ƿ�����ȡ
	if (pTaskEvent->wIsAlreadyTake)
	{
		MAPLOG(ERROR_LEVEL, "�������ȡ������[TaskId:%d]", wTaskId);
		return;
	}

	// ȡ��������
	const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(wTaskId);
	if (!pTaskEventCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ������¼�����[TaskId:%d]", wTaskId);
		return;
	}

	// ����Ƿ����������
	if (pTaskEvent->nContext < pTaskEventCfg->nX)
	{
		MAPLOG(ERROR_LEVEL, "����ȼ�����û���[TaskId:%d]", wTaskId);
		return;
	}

	WORD wOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());
	const WorldTaskLevel_Config *pTaskLevelCfg = g_pCfgMgr->GetWorldTaskLevelData(wOpenServerDays);
	if (!pTaskLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ�������������[TaskId:%d]", wTaskId);
		return;
	}

	// ȡ����
	const WorldLevelTaskIdList_Config *pTasIdkListCfg = g_pCfgMgr->GetWorldLevelTaskIdListByTaskId(wTaskId);
	if (!pTasIdkListCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ�����ID�б�����[TaskId:%d]", wTaskId);
		return;
	}

	int nRate = g_GetConfigValue(279);

	// ��ΪҪ��Ϊ����,Ҫ��֤����Ϊ0�����
	nRate = max(nRate, 1);

	// ������
	// ��ȡ���� = math.min(math.max(����ȼ� - ����ȼ�, 0), ϵ��) / ϵ�� * ������
	// ע����ʼ�ȼ�Խ�ߣ����齱��Խ��
	double fTemp = min(max(int(pTaskLevelCfg->wWorldLevel - pTasIdkListCfg->wTaskLevel), 0), nRate);
	int nExp = fTemp / nRate * pTasIdkListCfg->nExp;
	m_pPlayer->AddExp(nExp);

	// ���߽������� = math.floor((1 + math.min(math.max(����ȼ� - ����ȼ� + ϵ��, 0), ϵ��) / ϵ��) * �����������)
	// ע����ʼ�ȼ�Խ�ߣ����߽���Խ��	
	for (int i = 0; i < pTaskEventCfg->vRewardList.size(); ++i)
	{
		Item item = pTaskEventCfg->vRewardList[i];
		fTemp = min(max(pTasIdkListCfg->wTaskLevel - pTaskLevelCfg->wWorldLevel + nRate, 0), nRate);
		int nItemNum = (1 + fTemp / nRate) * item.dwNum;
		item.dwNum = nItemNum;
		m_pPlayer->AddObjectData(&item, TRUE, FROM_WORLD_LEVEL_TASK_REWARD);
	}

	// ��¼����ȡ������
	pTaskEvent->wIsAlreadyTake = true;

	SendWorldLevelTaskData();

	// ��Ӧ
	MSG_S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP msg;
	msg.m_wTaskId = wTaskId;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ���ս���
void CWorldLevelTaskMananger::OnNetTakeFinalReward()
{
	// ����Ƿ�����ȡ����
	if (m_PlayerWorldLevelTaskData->wIsAlreadyTakeFinalReward)
	{
		MAPLOG(ERROR_LEVEL, "�������ȡ������ȼ��������ս���");
		return;
	}

	// ����Ƿ����е����������
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		WorldLevelTaskEvent &tTaskEvent = m_PlayerWorldLevelTaskData->arTaskEventList[i];
		const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(tTaskEvent.wTaskId);
		if (!pTaskEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ������¼�����[TaskId:%d]", tTaskEvent.wTaskId);
			return;
		}

		if (tTaskEvent.nContext < pTaskEventCfg->nX)
		{
			MAPLOG(ERROR_LEVEL, "����ȼ������¼���û���[TaskId:%d, Cur:%d, Need:%d]", 
				tTaskEvent.wTaskId, tTaskEvent.nContext, pTaskEventCfg->nX);
			return;
		}
	}

	WORD wOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	const WorldTaskLevel_Config *pTaskLevelCfg = g_pCfgMgr->GetWorldTaskLevelData(wOpenServerDays);
	if (!pTaskLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ�����������ȼ�����[OpenServerDays:%d]", wOpenServerDays);
		return;
	}

	WORD wInitLevel = m_pPlayer->GetDailyInitLevel();
	map<WORD, vector<Item>>::const_iterator iter = pTaskLevelCfg->vRewardList.find(wInitLevel);
	if (iter == pTaskLevelCfg->vRewardList.end())
	{
		MAPLOG(ERROR_LEVEL, "�Ҳ��������������ս�������[OpenServerDays:%d,RoleInitLevel:%d]", 
			wOpenServerDays, wInitLevel);
		return;
	}

	// ������
	m_pPlayer->AddObjectList(iter->second, TRUE, FROM_WORLD_TASK_FINAL_REWARD);

	// ��¼����ȡ
	m_PlayerWorldLevelTaskData->wIsAlreadyTakeFinalReward = true;

	// ��Ӧ
	MSG_S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ���������¼�
void CWorldLevelTaskMananger::UpdateTaskEvent(WORD wTaskType, int nValue, bool isAdd)
{
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		WorldLevelTaskEvent &tTaskEvent = m_PlayerWorldLevelTaskData->arTaskEventList[i];
		if (tTaskEvent.wTaskType == wTaskType)
		{ 
 			// ����ȡ
			if (tTaskEvent.wIsAlreadyTake)
 				continue;
 
 			if (isAdd)
				tTaskEvent.nContext += nValue;
 			else
				tTaskEvent.nContext = nValue;
 
			const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(tTaskEvent.wTaskId);
 			if (!pTaskEventCfg)
 			{
				MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ������¼�����[TaskId:%d]", tTaskEvent.wTaskId);
 				continue;
 			}
 
 			// ���ݾ���
			tTaskEvent.nContext = min(tTaskEvent.nContext, pTaskEventCfg->nX);
		}
	}
}

// ���͵ȼ���������
void CWorldLevelTaskMananger::SendWorldLevelTaskData()
{
	MSG_S2C_WORLD_LEVEL_TASK_DATA_RESP msg;
	msg.m_Data = *m_PlayerWorldLevelTaskData;

	// �����߼�,Ϊ�˷���ͻ��˲���������ĵȼ�Ҳ����ȥ
// 	for (WORD i = 0; i < msg.m_Data.wTaskEventNum; ++i)
// 	{
// 		WorldLevelTaskEvent &tTaskEvent = msg.m_Data.arTaskEventList[i];
// 		const WorldLevelTaskIdList_Config *pCfg = g_pCfgMgr->GetWorldLevelTaskIdListByTaskId(tTaskEvent.wTaskId);
// 		tTaskEvent.nContext = pCfg ? pCfg->wTaskLevel : 0;
// 	}

	m_pPlayer->SendPacket(&msg);
}

// ��������
void CWorldLevelTaskMananger::UpdateTask()
{
	// ��������
	m_PlayerWorldLevelTaskData->wIsAlreadyTakeFinalReward = false;
	m_PlayerWorldLevelTaskData->wTaskEventNum = 0;

	RoleBaseInfo &tBaseInfo = m_pPlayer->GetRoleFullInfo().tRoleBaseInfo;

	WORD wOpenServerDays = g_MapServer.GetAlreadyOpenServerDay(m_pPlayer->GetServerId());

	const WorldTaskLevel_Config *pTaskLevelCfg = g_pCfgMgr->GetWorldTaskLevelData(wOpenServerDays);
	if (!pTaskLevelCfg)
		return;

	// ��������ȼ�
	// 1.�ȼ������� = ��ʼ�ȼ�
	// 2.�ȼ������� = ��ʼ�ȼ� + math.min(math.max(����ȼ� - ��ʼ�ȼ�, 5), 9)
	WORD wTaskLevelMin = tBaseInfo.wDailyInitLevel;
	WORD wTaskLevelMax = wTaskLevelMin + min(max(int(pTaskLevelCfg->wWorldLevel - wTaskLevelMin), 5), 9);

	for (WORD i = wTaskLevelMin; i <= wTaskLevelMax; ++i)
	{
		// ȡ����
		const WorldLevelTaskIdList_Config *pTasIdkListCfg = g_pCfgMgr->GetWorldLevelTaskIdList(i);
		if (!pTasIdkListCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ�����ID�б�����[TaskLevel:%d]", i);
			return;
		}
		// ����б��Ƿ�Ϊ��
		if (!pTasIdkListCfg->vTaskIdList.size())
		{
			MAPLOG(ERROR_LEVEL, "����ȼ�����ID�����б�Ϊ��[TaskLevel:%d]", i);
			return;
		}

		// �������ID
		WORD wTaskId = pTasIdkListCfg->vTaskIdList[g_Rand(0, pTasIdkListCfg->vTaskIdList.size()-1)];

		const WorldLevelTaskEvent_Config *pTastEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(wTaskId);
		if (!pTastEventCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ�������ȼ������¼�����[TaskId:%d]", wTaskId);
			return;
		}

		WorldLevelTaskEvent tTaskEvent;
		tTaskEvent.wTaskId = wTaskId;
		tTaskEvent.nContext = 0;
		tTaskEvent.wIsAlreadyTake = false;
		tTaskEvent.wTaskType = pTastEventCfg->wTaskType;
		tTaskEvent.wTaskLevel = pTasIdkListCfg->wTaskLevel;
		AddTaskEvent(tTaskEvent);
	}
}

// �Ƿ��к��
bool CWorldLevelTaskMananger::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		WorldLevelTaskEvent &tTaskEvent = m_PlayerWorldLevelTaskData->arTaskEventList[i];

		// ����ȡ
		if (tTaskEvent.wIsAlreadyTake)
			continue;

		// �н�������ȡ
		const WorldLevelTaskEvent_Config *pTaskEventCfg = g_pCfgMgr->GetWorldLevelTaskEvent(tTaskEvent.wTaskId);
		if (pTaskEventCfg && tTaskEvent.nContext >= pTaskEventCfg->nX)
			return true;
	}

	return false;
}

// ��������¼�
void CWorldLevelTaskMananger::AddTaskEvent(WorldLevelTaskEvent &tTaskEvent)
{
	if (m_PlayerWorldLevelTaskData->wTaskEventNum >= MAX_DAILY_WORLD_LEVEL_TASK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "�����¼��б�����[MaxNum:%d]", MAX_DAILY_WORLD_LEVEL_TASK_NUM);
		return;
	}

// 	WorldLevelTaskEvent tTaskEvent;
// 	tTaskEvent.wTaskId = wTaskId;
// 	tTaskEvent.nContext = 0;
// 	tTaskEvent.wIsAlreadyTake = false;
	m_PlayerWorldLevelTaskData->arTaskEventList[m_PlayerWorldLevelTaskData->wTaskEventNum++] = tTaskEvent;
}

// ȡ�����¼�
WorldLevelTaskEvent* CWorldLevelTaskMananger::GetTaskEvent(WORD wTaskId)
{
	for (WORD i = 0; i < m_PlayerWorldLevelTaskData->wTaskEventNum; ++i)
	{
		if (m_PlayerWorldLevelTaskData->arTaskEventList[i].wTaskId == wTaskId)
			return &m_PlayerWorldLevelTaskData->arTaskEventList[i];
	}
	return NULL;
}
