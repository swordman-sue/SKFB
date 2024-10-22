#include "stdafx.h"
#include "CampRecruit.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"
#include "..\MapServer.h"
#include "CommonLogic.h"
#include "..\GameLogic.h"


void CCampRecruit::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pCampRecruitData = &pPlayer->GetRoleFullInfo().tActivityInfo.tPlayerCampRecruitData;
}

// ������ļ����
void CCampRecruit::OnNetRecruitData()
{
	UpdateRecruitData();
	SendRecruitData();
}

// ��ļ����
void CCampRecruit::OnNetRecruit(WORD wRecuitType, WORD wCamp)
{
	UpdateRecruitData();

	// ����Ƿ���
	WORD wDataId = GetCurDataId();
	if (!wDataId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӫ��ļ�û�п���", __FUNCTION__);
		return;
	}

	// ȡ��ļ����
	const CampRecruit_Config *pRecruitCfg = g_pCfgMgr->GetCampRecruit(wDataId, wCamp);
	if (!pRecruitCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӫ��ļ����[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
		return;
	}

	// ȡ��ļӢ���б�
	const vector<CampRecruitHero_Config> *pRecruitHeroList =  g_pCfgMgr->GetCampRecruitHeroList(wDataId, wCamp);
	if (!pRecruitHeroList || !pRecruitHeroList->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӫ��ļӢ���б�����[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
		return;
	}

	int nCostDiamond = 0;
	DWORD dwRecruitScore = 0;
	WORD wDailyFreeRecruitTimes = 0;
	WORD wRecruitTimes = 1;

	// ��ͨ��ļ
	if (1 == wRecuitType)
	{
		nCostDiamond = pRecruitCfg->dwNormalRecruitCostDiamond;
		dwRecruitScore = pRecruitCfg->dwNormalRecruitScore;
		wDailyFreeRecruitTimes = pRecruitCfg->wDailyFreeRecruitTimes;

		// ��������ļ������ʹ����
		if (m_pCampRecruitData->wTodayAlreadyRecruitTimes >= wDailyFreeRecruitTimes)
		{
			// ������ĵ���ʯ
			if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
			{
				MAPLOG(ERROR_LEVEL, "[%s]��Ӫ��ļ���ĵ���ʯ����[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
				return;
			}

			// �۳�����
			m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CAMP_RECRUIT);
		}

		m_pCampRecruitData->wTodayAlreadyRecruitTimes++;
	}
	// һ����ļ
	else
	{
		nCostDiamond = pRecruitCfg->dwOneKeyRecruitCostDiamond;
		dwRecruitScore = pRecruitCfg->dwOneKeyRecruitScore;
		wRecruitTimes = pRecruitCfg->wOneKeyRecruitTimes;

		// ������ĵ���ʯ
		if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ӫ��ļ���ĵ���ʯ����[DataId:%d,Camp:%d]", __FUNCTION__, wDataId, wCamp);
			return;
		}

		// �۳�����
		m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_CAMP_RECRUIT);
	}

	// ��ӻ�õĻ���
	AddCampRecruitScore(wCamp, dwRecruitScore);

	// �����ܸ���
	int nTotalWeight = 0;
	vector<CampRecruitHero_Config>::const_iterator iter = pRecruitHeroList->begin();
	for (; iter != pRecruitHeroList->end(); ++iter)
	{
		nTotalWeight += iter->dwWeight;
	}

	// ������ļ���
	vector<Item> vRewardItemList;
	for (WORD i = 0; i < wRecruitTimes; ++i)
	{
		int nRandValue = g_Rand(0, nTotalWeight);
		int nCurValue = 0;
		iter = pRecruitHeroList->begin();
		for (; iter != pRecruitHeroList->end(); ++iter)
		{
			nCurValue += iter->dwWeight;
			if (nRandValue <= nCurValue)
			{
				vRewardItemList.push_back(iter->tRewardItem);

				// �Ƿ���Ҫ��¼
				if (iter->byIsNeedRecord)
					AddRecruitRecord(wCamp, iter->tRewardItem);
				break;
			}
		}
	}

	// �����ļ����Ӣ�ۻ�����Ƭ
	m_pPlayer->AddObjectList(vRewardItemList, TRUE, FROM_CAMP_RECRUIT);

	// ��Ӧ
	MSG_S2C_CAMP_RECRUIT_RESP msg;
	msg.m_wCamp = wCamp;
	msg.m_wRecuitType = wRecuitType;
	msg.m_vHeroList = vRewardItemList;
	m_pPlayer->SendPacket(&msg);
}

// ��ȡ����������
void CCampRecruit::OnNetScoreTaskReward(WORD wCamp, WORD wRewardIdx)
{
	// ��������
	UpdateRecruitData();

	// ����Ƿ���
	WORD wDataId = GetCurDataId();
	if (!wDataId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӫ��ļ�û�п���", __FUNCTION__);
		return;
	}

	// ȡ��ļ����
	const CampRecruit_Config *pRecruitCfg = g_pCfgMgr->GetCampRecruit(wDataId, wCamp);
	if (!pRecruitCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӫ��ļ����[DataId:%d,Camp:%d]", __FUNCTION__,wDataId, wCamp);
		return;
	}

	// �����Ҫ�Ļ����Ƿ��㹻
	DWORD dwCurScore = GetCampRecruitScore(wCamp);
	DWORD dwNeedScore = pRecruitCfg->GetTaskScore(wRewardIdx);
	if (dwCurScore < dwNeedScore)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ȡ��Ӫ��ļ������Ҫ�Ļ��ֲ���[DataId:%d, Camp:%d, RewardIdx:%d,CurScore:%d, NeedScore:%d]", 
			__FUNCTION__, wDataId, wCamp, wRewardIdx, dwCurScore, dwNeedScore);
		return;
	}

	// ����Ƿ�����ȡ��
	if (IsScoreTaskRewardRecord(wCamp, wRewardIdx))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӫ��ļ��������������ȡ��[DataId:%d,Camp:%d,RewardIdx:%d]", __FUNCTION__, wDataId, wCamp, wRewardIdx);
		return;
	}

	// ������
	const Item *pRewardCfg = pRecruitCfg->GetTaskRewardItem(wRewardIdx);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ������Ӫ��ļ��������������[DataId:%d,Camp:%d,RewardIdx:%d]", __FUNCTION__, wDataId, wCamp, wRewardIdx);
		return;
	}
	m_pPlayer->AddObjectData(pRewardCfg, TRUE, FROM_GET_CAMP_RECRUIT_SCORE_TASK_REWARD);

	// ��Ӽ�¼
	AddScoreTaskRewardRecord(wCamp, wRewardIdx);

	// ��Ӧ
	MSG_S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP msg;
	msg.m_wCamp = wCamp;
	msg.m_wRewardIdx = wRewardIdx;
	m_pPlayer->SendPacket(&msg);
}

// ������ļ��¼
void CCampRecruit::OnNetRecruitRecord(WORD wCamp)
{
	MSG_S2C_CAMP_RECRUIT_RECORD_RESP msg;
	msg.m_wCamp = wCamp;
	const deque<CampRecruitRecord> *pRecordList = CGameLogic::Instance()->GetCampRecruitRecordList(m_pPlayer->GetServerId(), wCamp);
	if (pRecordList)
	{
		deque<CampRecruitRecord>::const_iterator iter = pRecordList->begin();
		for (; iter != pRecordList->end(); ++iter)
		{
			//if (iter->wCamp == wCamp)
			msg.m_vRecruitRecordList.push_back(*iter);
		}
	}
	m_pPlayer->SendPacket(&msg);
}

// ������Ӫ��ļ����
void CCampRecruit::SendRecruitData()
{
	MSG_S2C_CAMP_RECRUIT_DATA_RESP msg;
	msg.m_wCurDataId = m_pCampRecruitData->wCurDataId;
	msg.m_wTodayAlreadyRecruitTimes = m_pCampRecruitData->wTodayAlreadyRecruitTimes;
	msg.m_vCampScoreList.clear();
	msg.m_RewardRecordList.clear();
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		msg.m_vCampScoreList.push_back(m_pCampRecruitData->arCampScoreList[i]);
	}
	vector<WORD> tCampRecordList;
	for (int i = 0; i < CAMP_NUM; ++i)
	{
		tCampRecordList.clear();
		//vector<WORD> &tCampRecordList = msg.m_RewardRecordList[i];
		for (int k = 0; k < m_pCampRecruitData->wScoreTaskRewardRecordNum; ++k)
		{
			WORD wRecordId = m_pCampRecruitData->arScoreTaskRewardRecordList[k];
			WORD wCamp = wRecordId * 0.01;
			if (wCamp == i)
			{
				WORD wRewardIdx = wRecordId ? wRecordId % 100 : 0;
				tCampRecordList.push_back(wRewardIdx);
			}
		}
		msg.m_RewardRecordList.push_back(tCampRecordList);
	}

	m_pPlayer->SendPacket(&msg);
}

// ������Ӫ��ļ����
void CCampRecruit::UpdateRecruitData()
{
	WORD wCurDataId = m_pActivityMgr->GetActivityDataId(146);
	if (wCurDataId != m_pCampRecruitData->wCurDataId)
	{
		m_pCampRecruitData->wCurDataId = wCurDataId;
		m_pCampRecruitData->wScoreTaskRewardRecordNum = 0;
		m_pCampRecruitData->wTodayAlreadyRecruitTimes = 0;
		memset(m_pCampRecruitData->arCampScoreList, 0, sizeof(m_pCampRecruitData->arCampScoreList));
	}
}

// ȡ��ǰ�����ID
WORD CCampRecruit::GetCurDataId()
{
	return m_pCampRecruitData->wCurDataId;
}

// �����Ӫ��ļ����
void CCampRecruit::AddCampRecruitScore(WORD wCamp, DWORD dwScore)
{
	if (wCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӪԽ��[Camp:%d]", __FUNCTION__, wCamp);
		return;
	}
	m_pCampRecruitData->arCampScoreList[wCamp] += dwScore;
}

// ȡ��Ӫ��ļ����
DWORD CCampRecruit::GetCampRecruitScore(WORD wCamp)
{
	if (wCamp >= CAMP_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӪԽ��[Camp:%d]", __FUNCTION__, wCamp);
		return 0;
	}
	return m_pCampRecruitData->arCampScoreList[wCamp];
}

WORD MakeRecordId(WORD wCamp, WORD wRewardIdx)
{
	return 100 * wCamp + wRewardIdx;
}

// �Ƿ�����ȡ����������
bool CCampRecruit::IsScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx)
{
	WORD wRecordId = MakeRecordId(wCamp, wRewardIdx);
	for (WORD i = 0; i < m_pCampRecruitData->wScoreTaskRewardRecordNum; ++i)
	{
		if (m_pCampRecruitData->arScoreTaskRewardRecordList[i] == wRecordId)
			return true;
	}
	return false;
}

// ��ӻ�����������¼
void CCampRecruit::AddScoreTaskRewardRecord(WORD wCamp, WORD wRewardIdx)
{
	if (m_pCampRecruitData->wScoreTaskRewardRecordNum >= MAX_CAMP_RECRUIT_SCORE_TASK_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ӫ��ļ������������¼�б�����[Camp:%d]", __FUNCTION__, wCamp);
		return;
	}

	WORD wRecordId = MakeRecordId(wCamp, wRewardIdx);
	m_pCampRecruitData->arScoreTaskRewardRecordList[m_pCampRecruitData->wScoreTaskRewardRecordNum++] = wRecordId;
}

// �����ļ��¼
void CCampRecruit::AddRecruitRecord(WORD wCamp, const Item &tItem)
{
	CampRecruitRecord tRecord;
	tRecord.dwRecruitTime = time(NULL);
	tRecord.tRewardData = tItem;
	strcpy_s(tRecord.szRoleName, sizeof(tRecord.szRoleName), m_pPlayer->GetRoleName());
	tRecord.wCamp = wCamp;
	CGameLogic::Instance()->AddCampRecruitRecord(m_pPlayer->GetServerId(), wCamp, tRecord);
}