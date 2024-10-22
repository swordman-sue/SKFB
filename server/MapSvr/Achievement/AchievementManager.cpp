#include "stdafx.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Battle\BattleManager.h"
#include "AchievementManager.h"
#include "AchievementDataDefine.h"

CAchievementManager::CAchievementManager()
{

}

CAchievementManager::~CAchievementManager()
{

}

void CAchievementManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pAchievementInfo = &pPlayer->GetRoleFullInfo().tAchievementInfo;
}

// ����ɾ��б�
void CAchievementManager::OnNetAchievementList()
{
	SendAchievementList();
}

// ��ȡ�ɾͽ���
void CAchievementManager::OnNetAchievementAward(WORD wAchievementId)
{
	// ȡ�ɾ�����
	const Achievement_Config *pAchievementCfg = CConfigManager::Instance()->GetAchievement(wAchievementId);
	if (!pAchievementCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ɾ���������![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	//  ��ҳɾ�����
	AchievementData *pAchievementData = GetAchievementData(wAchievementId);
	if (!pAchievementData)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ҵĳɾ�����![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	// ����Ƿ�����ȡ
	if (pAchievementData->byCurRewardLevel >= pAchievementData->byLv)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�ɾͽ�������ȡ![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	const Achievement_Config::LvData *pLvDataCfg = pAchievementCfg->GetLvData(pAchievementData->byCurRewardLevel + 1);
	if (!pLvDataCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ACHIEVEMENT_AWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ɾ͵ȼ���������[Id:%d,Lv:%d]", 
			__FUNCTION__, wAchievementId, pAchievementData->byCurRewardLevel + 1);
		return;
	}

	// ����
	m_pPlayer->AddObject(pLvDataCfg->tRewardItem.dwId, pLvDataCfg->tRewardItem.dwNum, true, FROM_ACHIEVEMENT_AWARD);

	// ��¼��������ȡ
	pAchievementData->byCurRewardLevel++;

	MSG_S2C_ACHIEVEMENT_AWARD_RESP msg;
	msg.m_wAchievementId = wAchievementId;
	m_pPlayer->SendPacket(&msg);
}

// ���ͳɾ��б�
void CAchievementManager::SendAchievementList()
{
	MSG_S2C_ACHIEVEMENT_LIST_RESP msg;
	msg.m_wAchievementNum = m_pAchievementInfo->wAchievementNum;
	int nCopySize = sizeof(AchievementData)* m_pAchievementInfo->wAchievementNum;
	memcpy_s(msg.m_AhievementList, nCopySize, m_pAchievementInfo->arAchievementList, nCopySize);
	m_pPlayer->SendPacket(&msg);
}

// ���ͳɾ�����
void CAchievementManager::SendAchievementData(const AchievementData *pData)
{
	MSG_S2C_ACHIEVEMENT_DATA_NOTIFY msg;
	memcpy_s(&msg.m_tAchievementData, sizeof(AchievementData), pData, sizeof(AchievementData));
	m_pPlayer->SendPacket(&msg);
}

// ���³ɾ�
void CAchievementManager::OnUpdateAchievement(WORD wAchievementId, int nValue, bool bAdd)
{
	// ȡ�ɾ�����
	const Achievement_Config *pAchievementCfg = CConfigManager::Instance()->GetAchievement(wAchievementId);
	if (!pAchievementCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ɾ���������![AchievementId:%d]",__FUNCTION__, wAchievementId);
		return;
	}

	//  ��ҳɾ�����
	AchievementData *pAchievementData = GetAchievementData(wAchievementId);
	if (!pAchievementData)
	{
		AchievementData tData;
		memset(&tData, 0, sizeof(tData));
		tData.wId = wAchievementId;
		AddAchievementData(tData);
		pAchievementData = GetAchievementData(wAchievementId);
	}

	// �ɾ������
	if (pAchievementData->byLv >= pAchievementCfg->vLvDataList.size())
		return;

	// ���ֵ
	if (bAdd)
	{
		pAchievementData->dwValue += nValue;
	}
	// ����ֵ
	else
	{
		// ʹ����Ѽ�¼
		if (nValue <= pAchievementData->dwValue)
			return;

		pAchievementData->dwValue = nValue;
	}

// todo::ԭ�����߼�
// 	const Achievement_Config::LvData *pLvDataCfg = pAchievementCfg->GetLvData(pAchievementData->byLv + 1);
// 	if (!pLvDataCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ɾ͵ȼ�����![AchievementId:%d,Lv:%d]",
// 			__FUNCTION__, wAchievementId, pAchievementData->byLv + 1);
// 		break;
// 	}
// 
// 	if (pAchievementData->dwValue >= pLvDataCfg->wValue)
// 	{
// 		pAchievementData->byLv++;
// 	}

	while (true)
	{
		const Achievement_Config::LvData *pLvDataCfg = pAchievementCfg->GetLvData(pAchievementData->byLv + 1);
		if (pLvDataCfg && pAchievementData->dwValue >= pLvDataCfg->wValue)
		{
			pAchievementData->byLv++;
		}
		else
		{
			break;
		}
	}

	SendAchievementData(pAchievementData);

	// ��¼��ɫ��Ϣ����
	m_pPlayer->RoleInfoUpdate();
}

// �Ƿ��к��
bool CAchievementManager::IsHaveRedPoint()
{
	for (WORD i = 0; i < m_pAchievementInfo->wAchievementNum; ++i)
	{
		AchievementData *pAchievement = &m_pAchievementInfo->arAchievementList[i];
		if (pAchievement->byLv > pAchievement->byCurRewardLevel)
			return true;
	}

	return false;
}

AchievementData* CAchievementManager::GetAchievementData(WORD wAchievementId)
{
	for (WORD i = 0; i < m_pAchievementInfo->wAchievementNum; ++i)
	{
		if (m_pAchievementInfo->arAchievementList[i].wId == wAchievementId)
		{
			return &m_pAchievementInfo->arAchievementList[i];
		}
	}
	return NULL;
}

// ��ӳɾ�����
void CAchievementManager::AddAchievementData(const AchievementData &tAchievement)
{
	if (m_pAchievementInfo->wAchievementNum >= MAX_ACHIEVEMENT_NUM)
		return;

	m_pAchievementInfo->arAchievementList[m_pAchievementInfo->wAchievementNum++] = tAchievement;
}