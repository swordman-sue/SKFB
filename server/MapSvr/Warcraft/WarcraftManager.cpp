#include "stdafx.h"
#include "WarcraftManager.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\PlayerBaseDataManager.h"
#include "CommonLogic.h"

CWarcraftManager::CWarcraftManager()
{

}

CWarcraftManager::~CWarcraftManager()
{
	
}

void CWarcraftManager::Init()
{

}

// ��������������
void CWarcraftManager::OnNetWarcraftData(Player *pPlayer)
{
	SendWarcraftData(pPlayer);
}

// ������ս����
void CWarcraftManager::OnNetBuyBattleTimes(Player *pPlayer)
{
	WORD wFromMsgId = C2S_WARCRAFT_BUY_BATTLE_TIMES_REQ;

	// ����Ƿ����ʸ���
	WarcraftRankData *pRankData = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pRankData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�������а���,���ܹ�����ս����", __FUNCTION__);
		return;
	}

	// ��⹺�����
	int nBuyTimes = pPlayer->GetProperty(ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES);
	if (nBuyTimes >= pPlayer->GetVipValue(VIP_WARCRAFT_BUY_BATTLE_TIMES))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�������а���,���ܹ�����ս����", __FUNCTION__);
		return;
	}

	// �������
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(101);
	if (!pServerCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ͨ������[key:%d]", __FUNCTION__, 101);
		return;
	}
	int nCostDiamond = pServerCfg->GetListValue(nBuyTimes);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������������ս�������Ĳ���", __FUNCTION__);
		return;
	}

	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_WARCRAFT_BATTLE_TIMES_BUY);

	pPlayer->ModifyProperty(ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES, 1);

	MSG_S2C_WARCRAFT_BUY_BATTLE_TIMES_RESP msg;
	pPlayer->SendPacket(&msg);
}

// ������սĿ��
void CWarcraftManager::OnNetBattleTargetList(Player *pPlayer)
{
	SendTargetList(pPlayer);
}

// �������а��б�
void CWarcraftManager::OnNetRankList(Player *pPlayer)
{
	MSG_S2C_WARCRAFT_RANK_LIST_RESP msg;
	msg.m_wRankNum = 0;

	for (vector<WarcraftRankData>::iterator iter = m_RankList.begin(); iter != m_RankList.end(); ++iter)
	{
		// ֻȡ���������
		if (iter->dwServerId != pPlayer->GetServerId())
			continue;

		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter->dwServerId, iter->dwRoleId);
		if (pBaseData)
		{
			ClientWarcraftRankData &tRankData = msg.m_RankList[msg.m_wRankNum++];
			memcpy_s(&tRankData, sizeof(WarcraftRankData), &iter, sizeof(WarcraftRankData));
			tRankData.dwHeadImageId = pBaseData->wHeadIcon;
			strcpy_s(tRankData.szRoleName, sizeof(tRankData.szRoleName) - 1, pBaseData->szRoleName);
		}
	}

	pPlayer->SendPacket(&msg);
}

// ����
void CWarcraftManager::OnNetCheer(Player *pPlayer, DWORD dwTargetId)
{
	WORD wFromMsgId = C2S_WARCRAFT_CHEER_REQ;

	// ���Ŀ������
	WarcraftRankData *pRankData = GetPlayerRankData(dwTargetId);
	if (!pRankData)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������Ŀ��ľ�������а�[TargetId:%d]", __FUNCTION__, dwTargetId);
		return;
	}

	// �����������
	if (pPlayer->GetProperty(ROLE_PRO_WARCRAFT_CHEER_TIMES) >= 3)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]���������Ѵ�����", __FUNCTION__);
		return;
	}
	
	pPlayer->ModifyProperty(ROLE_PRO_WARCRAFT_CHEER_TIMES);
	pPlayer->SetProperty(ROLE_PRO_WARCRAFT_CHEER_TIMES_RECOVER_TIME, time(NULL));

	// ���±���������
	pRankData->wBeCheerTimes++;

	// ��Ӧ
	MSG_S2C_WARCRAFT_CHEER_RESP msg;
	msg.m_dwTargetId = dwTargetId;
	pPlayer->SendPacket(&msg);
}

// ��ʼս��
bool CWarcraftManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// todo::�����ս����
	if (pPlayer->GetProperty(ROLE_PRO_WARCRAFT_BATTLE_TIMES) >= 10)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս��������", __FUNCTION__);
		return false;
	}

	WarcraftRankData *pRankData = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pRankData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�������а���,ľ����ս�ʸ�", __FUNCTION__);
		return false;
	}

	// ���Ŀ��
	vector<WarcraftRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetRoleId(), &vTargetList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ��������սĿ���б�ʧ��", __FUNCTION__);
		return false;
	}

	bool bFound = false;
	for (int i = 0; i < vTargetList.size(); ++i)
	{
		if (vTargetList[i].dwRoleId == dwTargetId)
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
	pFBMgr->SetBattleInfo(0, 0, 0);
	pFBMgr->SetTargetId(dwTargetId);

	// ��������,����Ҫ��Ӣ�۷���ȥ
	const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, true);
	if (pTargetBaseData)
	{
		pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);
		pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
	}

	pPlayer->ModifyProperty(ROLE_PRO_WARCRAFT_BATTLE_TIMES, 1, false);

	return true;
}


// ����ս��
bool CWarcraftManager::OnBattleOver(Player *pPlayer, BYTE byIsWin)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	WarcraftRankData *pSrc = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pSrc)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս����ʱ,�Ҳ�����ս������", __FUNCTION__);
		return false;
	}
	WarcraftRankData *pTarget = GetPlayerRankData(pFBMgr->GetTargetId());
	if (!pTarget)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������ս����ʱ,�Ҳ���Ŀ������", __FUNCTION__);
		return false;
	}

	DWORD dwScore, dwScoreMatchHonor;

	// ���� = 50+����ս��/5��
	dwScore = 50 + pTarget->dwFightingPower * 0.0005;

	// ʤ��
	if (byIsWin)
	{
		// ���Ŀ����������Ҹ�
		if (pSrc->wRank > pTarget->wRank)
		{
			// ����λ��
			swap(*pSrc, *pTarget);	// �������ݽ���
			swap(pSrc->wRank, pTarget->wRank);	// ��������
			swap(pSrc, pTarget);	// ������ַ
		}

		// ����ѫ�� = ���� = 50+����ս��/5��
		dwScoreMatchHonor = dwScore;
	}
	// ʧ��
	else
	{
		// ʧ��=ʤ������/2  ����ȡ��
		dwScoreMatchHonor = dwScore * 0.5 + ((dwScore % 2) ? 1 : 0);
	}

	pPlayer->ModifyProperty(ROLE_PRO_SCORE_MATCH_HONOR, dwScoreMatchHonor);

	return true;
}

// ��������������
void CWarcraftManager::SendWarcraftData(Player *pPlayer)
{
	MSG_S2C_WARCRAFT_DATA_RESP msg;
	msg.m_byWarcraftBattleTimes = pPlayer->GetProperty(ROLE_PRO_WARCRAFT_BATTLE_TIMES);
	msg.m_byBuyWarcraftBattleTimes = pPlayer->GetProperty(ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES);
	pPlayer->SendPacket(&msg);
}

// ����Ŀ���б�
void CWarcraftManager::SendTargetList(Player *pPlayer)
{
	WarcraftRankData *pRankData = GetPlayerRankData(pPlayer->GetRoleId());
	if (!pRankData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�������а���,����������սĿ��", __FUNCTION__);
		return;
	}

	vector<WarcraftRankData> vTargetList;
	if (!GetTargetList(pPlayer->GetRoleId(), &vTargetList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ȡĿ���б�ʧ��!", __FUNCTION__);
		return;
	}

 	MSG_S2C_WARCRAFT_TARGET_LIST_RESP msg;
	msg.m_wRank = pRankData->wRank;
 	msg.m_byTargetNum = 0;
 	for (vector<WarcraftRankData>::iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter)
 	{
 		if (msg.m_byTargetNum >= MAX_ARENA_TARGET_NUM)
 			break;
 
		WarcraftTargetData &tTargetData = msg.m_TargetList[msg.m_byTargetNum++];
 		memset(&tTargetData, 0, sizeof(tTargetData));
		tTargetData.dwRoleId = iter->dwRoleId;
		tTargetData.wRank = iter->wRank;
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter->dwServerId, iter->dwRoleId);
 		if (pBaseData)
 		{
			strcpy_s(tTargetData.szRoleName, sizeof(tTargetData.szRoleName) - 1, pBaseData->szRoleName);
 			tTargetData.wHeadIcon = pBaseData->wHeadIcon;
 			tTargetData.dwFightingPower = pBaseData->dwFightingPower;
 		}
 	}
 
 	pPlayer->SendPacket(&msg);
}

// ȡ��սĿ���б�
bool CWarcraftManager::GetTargetList(DWORD dwRoleId, vector<WarcraftRankData> *pOutTargetList)
{
	pOutTargetList->clear();

	WarcraftRankData *pRankData = GetPlayerRankData(dwRoleId);
	if (!pRankData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�������а���,����������սĿ��", __FUNCTION__);
		return false;
	}

	WORD wShowRank = 10;

	// ȡǰ10��
	for (vector<WarcraftRankData>::iterator iter = m_RankList.begin(); iter != m_RankList.end(); ++iter)
	{
		if (pOutTargetList->size() >= wShowRank)
			break;
		pOutTargetList->push_back(*iter);
	}

	WORD wRank = pRankData->wRank;

	const WarcraftRank_Config *pRankCfg = g_pCfgMgr->GetWarcraftRank(wRank);
 	if (!pRankCfg)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������������[Rank:%d]", __FUNCTION__, wRank);
		return false;
 	}

	// ��ֹΪ0�����
	WORD wTargetRankInterval = min(pRankCfg->wTargetRankInterval, 1);

	// ������10��֮��
	vector<WarcraftRankData> vTmpRankList;
	if (wRank > wShowRank)
	{
		// ����Ŀ��(ֻȡ9��)
		int nTargetRank = wRank - wTargetRankInterval * 9;
		nTargetRank = max(nTargetRank, 0);
		while (wTargetRankInterval && nTargetRank < wRank)
		{
			vTmpRankList.push_back(m_RankList[nTargetRank - 1]);
			nTargetRank += wTargetRankInterval;
		}

		for (size_t i = 0; i < vTmpRankList.size(); ++i)
		{
			bool bHave = false;
			for (size_t j = 0; j < pOutTargetList->size(); ++j)
			{
				if (pOutTargetList->at(j).dwRoleId == vTmpRankList[i].dwRoleId)
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
			if (m_RankList.size() > nTargetRank)
				pOutTargetList->push_back(m_RankList[nTargetRank - 1]);
		}
	}
	// ������10��֮��
	else
	{
		pOutTargetList->push_back(m_RankList[wShowRank]);
		pOutTargetList->push_back(m_RankList[wShowRank + 1]);
	}

	return true;
}

// ��Ӿ��������а�����
void CWarcraftManager::AddArenaRankData(DWORD dwRoleId, WORD wRank)
{
	m_ArenaRankList.insert(make_pair(dwRoleId, wRank));
}

// ȡ�����������
WarcraftRankData* CWarcraftManager::GetPlayerRankData(DWORD dwRoleId)
{
	for (size_t i = 0; i < m_RankList.size(); ++i)
	{
		if (m_RankList[i].dwRoleId == dwRoleId)
			return &m_RankList[i];
	}
	return NULL;
}