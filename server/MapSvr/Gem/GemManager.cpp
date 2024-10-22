#include "stdafx.h"
#include "GemManager.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "DBCDefine.h"
#include "..\Hero\HeroManager.h"
#include "..\ConfigManager.h"
#include "..\MapServer.h"


CGemManager::CGemManager()
{

}

CGemManager::~CGemManager()
{

}

// ��ʼ��
void CGemManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pPlayerGemInfo = &pPlayer->GetRoleFullInfo().tSecondInfo.tGemInfo;
}

// �½���ɫ
void CGemManager::OnNewRole()
{
	InitDefaultOpenHole();
}

// ��ʼ��Ĭ�Ͽ��ŵĿ�λ
void CGemManager::InitDefaultOpenHole()
{
	// �Ѿ���ʼ������
	if (m_pPlayerGemInfo->dwHoleInitTime)
		return;

	// ����Ĭ�Ͽ����Ŀ�λ
	WORD wMaxHole = 0;
	for (WORD i = 1; i <= MAX_GEM_HOLE_NUM; ++i)
	{
		const GemHole_Config *pHoleCfg = g_pCfgMgr->GetGemHole(i);
		if (pHoleCfg && pHoleCfg->wIsDefaultOpen && i > wMaxHole)
			wMaxHole = i;
	}

	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		m_pPlayerGemInfo->arHoleOpenList[i] = wMaxHole;
	}

	m_pPlayerGemInfo->dwHoleInitTime = time(NULL);
}

// ����Ƿ��ѳ�ʼĬ�Ͽ��ŵĿ�λ
bool CGemManager::IsInitDefaultOpenHole()
{
	for (WORD i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		if (m_pPlayerGemInfo->arHoleOpenList[i])
			return true;
	}
	return false;
}

// ����ʯ�б�
void CGemManager::OnNetGemList()
{
	SendGemDataList();
}

// �ϳ�
void CGemManager::OnNetCompose(DWORD dwGemId, WORD wComposeNum)
{
	const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
	if (!pGemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ʯ����[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// ����Ƿ���Ժϳ�
	if (!pGemCfg->dwNewGemFromCompose)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�˱�ʯ���ܺϳ�[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// ���ϳ����ĵ�����
	if (GetGemNum(dwGemId) < wComposeNum * pGemCfg->wComposeCostNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ�ϳ����ĵ���������[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// ɾ���ϳ����ĵı�ʯ
	RemoveGem(dwGemId, wComposeNum * pGemCfg->wComposeCostNum);

	// ��ӻ�õ��±�ʯ
	AddGem(pGemCfg->dwNewGemFromCompose, wComposeNum);

	MSG_S2C_GEM_COMPOSE_RESP msg;
	msg.m_dwGemId = dwGemId;
	msg.m_wGemNum = wComposeNum;
	m_pPlayer->SendPacket(&msg);
}

// ��Ƕ
void CGemManager::OnNetInlay(WORD wFormationSlot, GemInlay *pInlayList, WORD wInlayNum)
{
	// wFormationSlot ��0��ʼ
	// wHole ��1��ʼ

	if (!wInlayNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ƕ�ı�ʯ�б�Ϊ��", __FUNCTION__);
		return;
	}

	const Formation *pFormation = m_pPlayer->GetHeroManager()->GetFormationByIndex(wFormationSlot);
	if (!pFormation)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[Formation:%d]", __FUNCTION__, wFormationSlot);
		return;
	}

	for (WORD i = 0; i < wInlayNum; ++i)
	{
		DWORD dwGemId = pInlayList[i].dwGemId;
		WORD wHole = pInlayList[i].wHole;

		// ȡ��ʯ����
		const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
		if (!pGemCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ʯ����[GemId:%d]", __FUNCTION__, dwGemId);
			continue;
		}

		// ����λ�Ϸ���
		if (!wHole || wHole > MAX_GEM_HOLE_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ���Ϸ�[Hole:%d]", __FUNCTION__, wHole);
			return;
		}

		// ��ⱦʯ�Ƿ����
		if (!GetGemNum(dwGemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��Ƕ�ı�ʯ������[GemId:%d]", __FUNCTION__, dwGemId);
			continue;
		}

		// ����Ƿ���
		if (!IsHoleOpen(wFormationSlot, wHole))
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ��û����[Formation:%d,Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
			continue;
		}

		DWORD dwOldGemId = pFormation->arGemList[wHole - 1];

		// ����ͬһ����λ�ϱ�ʯ������������
		const GemInlay_Config *pInlayCfg = g_pCfgMgr->GetGemInlay(pGemCfg->wType);
		if (!pInlayCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ʯ��Ƕ����[type:%d]", __FUNCTION__, pGemCfg->wType);
			continue;
		}
		WORD wSimilarGemNum = GetFormationSlotSimilarGemNum(wFormationSlot, pGemCfg->wType, dwOldGemId);
		if (wSimilarGemNum >= pInlayCfg->wInlayNumLimit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]ͬһ����λ��ͬ�౦ʯ��������[type:%d]", __FUNCTION__, pGemCfg->wType);
			continue;
		}

		// todo::��֪���Ƿ���Ҫ����

		// ���ԭ��λ���Ƿ��б�ʯ,����黹
		if (dwOldGemId)
			AddGem(dwOldGemId);

		// ɾ��׼����Ƕ�ı�ʯ
		RemoveGem(dwGemId);

		// ��Ƕ
		m_pPlayer->GetHeroManager()->SetGemHole(wFormationSlot, wHole-1, dwGemId);
	}

	// ��������
	m_pPlayer->UpdateFightingPower();

	m_pPlayer->GetHeroManager()->SendFormationList();

	MSG_S2C_GEM_INLAY_RESP msg;
	msg.m_wFormationSlot = wFormationSlot;
	msg.m_wGemInlayNum = wInlayNum;
	memcpy_s(msg.m_arGemInlayList, sizeof(msg.m_arGemInlayList), pInlayList, wInlayNum*sizeof(GemInlay));
	m_pPlayer->SendPacket(&msg);
}

// ж��
void CGemManager::OnNetUnload(WORD wFormationSlot, WORD wHole)
{
	// wFormationSlot ��0��ʼ
	// wHole ��1��ʼ

 	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]���Ͳ�λԽ��[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
 		return;
 	}

	if (!wHole || wHole > MAX_GEM_HOLE_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ���Ϸ�[Hole:%d]", __FUNCTION__, wHole);
		return;
	}

	// ����Ƿ��ѿ���
	if (!IsHoleOpen(wFormationSlot, wHole))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ��û����[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// ȡ��������
	const Formation *pFormation = m_pPlayer->GetHeroManager()->GetFormationByIndex(wFormationSlot);
	if (!pFormation)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����������[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return;
	}

	// ����λ�ϵı�ʯΪ��
	DWORD dwGemId = pFormation->arGemList[wHole - 1];
	if (!dwGemId)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ��û����Ƕ��ʯ[FormationSlot:%d,Hole:%d]", __FUNCTION__, wFormationSlot,wHole);
		return;
	}

	// ��տ�λ�ı�ʯ
	m_pPlayer->GetHeroManager()->SetGemHole(wFormationSlot, wHole-1, 0);

	// �黹ж�µı�ʯ
	AddGem(dwGemId);

	// ��������
	m_pPlayer->UpdateFightingPower();

	m_pPlayer->GetHeroManager()->SendFormationList();

	MSG_S2C_GEM_UNLOAD_RESP msg;
	msg.m_wFormationSlot = wFormationSlot;
	msg.m_wGemHole = wHole;
	m_pPlayer->SendPacket(&msg);
}

// ������λ
void CGemManager::OnNetHoleOpen(WORD wFormationSlot, WORD wHole)
{
	// wFormationSlot ��0��ʼ
	// wHole ��1��ʼ

	// ȡ����
	const GemHole_Config *pHoleCfg = g_pCfgMgr->GetGemHole(wHole);
	if (!pHoleCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ʯ��λ����[Hole:%d]", __FUNCTION__, wHole);
		return;
	}

	// ����Ƿ�Ĭ�Ͽ���
	if (pHoleCfg->wIsDefaultOpen)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ��Ĭ�Ͽ���[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// ����Ƿ��ѿ���
	if (IsHoleOpen(wFormationSlot, wHole))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��λ�ѿ���[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// ����Ƿ�Խ�翪��
	WORD dwCurOpenHole = GetOpenHole(wFormationSlot);
	if (wHole > dwCurOpenHole + 1)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]Խ�翪����ʯ��λ[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// ���ȼ�
	if (m_pPlayer->GetLevel() < pHoleCfg->wNeedRoleLevel)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ����λ�ȼ�����[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// ������ĵ���
	if (pHoleCfg->vCostItem.dwId && m_pPlayer->GetItemNum(pHoleCfg->vCostItem.dwId) < pHoleCfg->vCostItem.dwNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ�������ĵ��߲���[FormationSlot:%d, Hole:%d]", __FUNCTION__, wFormationSlot, wHole);
		return;
	}

	// ɾ�����ĵ���Ʒ
	m_pPlayer->RemoveItem(pHoleCfg->vCostItem.dwId, pHoleCfg->vCostItem.dwNum, FROM_GEM_OPEN_HOLE);

	// ������λ
	OpenHole(wFormationSlot, wHole);

	MSG_S2C_GEM_HOLE_OPEN_RESP msg;
	msg.m_wFormationSlot = wFormationSlot;
	msg.m_wGemHole = wHole;
	m_pPlayer->SendPacket(&msg);
}

// �ֽ�
void CGemManager::OnNetDecompose(DWORD dwGemId, DWORD dwGemNum)
{
	// ȡ����
	const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
	if (!pGemCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����ʯ����[GemId:%d]", __FUNCTION__, dwGemId);
		return;
	}

	// ��ⱦʯ������
	if (GetGemNum(dwGemId) < dwGemNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]��ʯ��������[GemId:%d, GemNum:%d]", __FUNCTION__, dwGemId, dwGemNum);
		return;
	}

	RemoveGem(dwGemId, dwGemNum);

	m_pPlayer->ModifyProperty(ROLE_PRO_GEM_COIN, pGemCfg->nGemCoin * dwGemNum);

	MSG_S2C_GEM_DECOMPOSE_RESP msg;
	msg.m_dwGemId = dwGemId;
	msg.m_dwGemNum = dwGemNum;
	m_pPlayer->SendPacket(&msg);
}

// ��ӱ�ʯ
bool CGemManager::AddGem(DWORD dwGemId, DWORD dwGemNum, bool bNotify, WORD wFromId)
{
	Gem *pGem = GetGem(dwGemId);
	if (pGem)
	{
		pGem->dwNum += dwGemNum;
	}
	else
	{
		if (m_pPlayerGemInfo->wGemNum >= MAX_GEM_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]��ʯ�б�����[num:%d]", __FUNCTION__, m_pPlayerGemInfo->wGemNum);
			return false;
		}

		Gem &tGem = m_pPlayerGemInfo->arGemList[m_pPlayerGemInfo->wGemNum++];
		tGem.dwId = dwGemId;
		tGem.dwNum = dwGemNum;
	}

	// ͬ������
	if (bNotify)
	{
		MSG_S2C_GEM_DATA_NOTIFY msg;
		msg.m_dwGemId = dwGemId;
		msg.m_dwGemNum = dwGemNum;
		msg.m_byFlag = 1;
		m_pPlayer->SendPacket(&msg);
	}

	// ��¼���
	g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwGemId, dwGemNum, true, wFromId);

	return true;
}

// ɾ����ʯ
DWORD CGemManager::RemoveGem(DWORD dwGemId, DWORD dwGemNum, bool bNotify, WORD wFromId)
{
	Gem *pGem;
	WORD wRemoveNum = 0;
	for (WORD i = 0; i < m_pPlayerGemInfo->wGemNum; ++i)
	{
		pGem = &m_pPlayerGemInfo->arGemList[i];
		if (pGem->dwId == dwGemId)
		{
			// ��¼���
			g_MapServer.OnStatisticsReport(m_pPlayer->GetRoleId(), dwGemId, dwGemNum, false, wFromId);

			// ���㹻�������۳�
			if (pGem->dwNum > dwGemNum)
			{
				wRemoveNum = dwGemNum;
				pGem->dwNum -= dwGemNum;
			}
			// ��������,����б����õ�
			else
			{
				wRemoveNum = pGem->dwNum;
				*pGem = m_pPlayerGemInfo->arGemList[--m_pPlayerGemInfo->wGemNum];
			}
			break;
		}
	}

	if (bNotify && wRemoveNum)
	{
		MSG_S2C_GEM_DATA_NOTIFY msg;
		msg.m_dwGemId = dwGemId;
		msg.m_dwGemNum = wRemoveNum;
		msg.m_byFlag = 0;
		m_pPlayer->SendPacket(&msg);
	}

	return wRemoveNum;
}

// ȡ��ʯ
Gem* CGemManager::GetGem(DWORD dwGemId)
{
	for (WORD i = 0; i < m_pPlayerGemInfo->wGemNum; ++i)
	{
		if (m_pPlayerGemInfo->arGemList[i].dwId == dwGemId)
			return &m_pPlayerGemInfo->arGemList[i];
	}
	return NULL;
}

// ȡ��ʯ����
DWORD CGemManager::GetGemNum(DWORD dwGemId)
{
	Gem *pGem = GetGem(dwGemId);
	return pGem ? pGem->dwNum : 0;
}

// ����λ�Ƿ��ѿ���
bool CGemManager::IsHoleOpen(WORD wFormationSlot, WORD wHole)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ͳ�λԽ��[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return false;
	}

	// ���С�ڵȼ���ǰ�ı�ʾ�ѿ�����
	if (wHole <= m_pPlayerGemInfo->arHoleOpenList[wFormationSlot])
		return true;

	return false;
}

// ȡ���͵�ǰ�ѿ��ŵ�����λ
WORD CGemManager::GetOpenHole(WORD wFormationSlot)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ͳ�λԽ��[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return 0;
	}

	return m_pPlayerGemInfo->arHoleOpenList[wFormationSlot];
}

// ������λ
void CGemManager::OpenHole(WORD wFormationSlot, WORD wHole)
{
	if (wFormationSlot >= MAX_BATTLE_HERO_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���Ͳ�λԽ��[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return;
	}
	m_pPlayerGemInfo->arHoleOpenList[wFormationSlot] = wHole;
}

// ȡ��λ��ͬ���ͱ�ʯ����
WORD CGemManager::GetFormationSlotSimilarGemNum(WORD wFormationSlot, WORD wGemTeyp, DWORD dwExceptGemId)
{
	const Formation *pFormation = m_pPlayer->GetHeroManager()->GetFormationByIndex(wFormationSlot);
	if (!pFormation)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ�������Ͳ�λ�ϵ�����[FormationSlot:%d]", __FUNCTION__, wFormationSlot);
		return 0;
	}

	WORD wNum = 0;
	for (WORD i = 0; i < MAX_GEM_HOLE_NUM; ++i)
	{
		DWORD dwGemId = pFormation->arGemList[i];
		if (dwGemId && dwGemId != dwExceptGemId)
		{
			const Gem_Config *pGemCfg = g_pCfgMgr->GetGem(dwGemId);
			if (pGemCfg && pGemCfg->wType == wGemTeyp)
				wNum++;
		}
	}

	return wNum;
}

// ���ͱ�ʯ�б�
void CGemManager::SendGemDataList()
{
	MSG_S2C_GEM_LIST_RESP msg;
	msg.m_PlayerGemInfo = *m_pPlayerGemInfo;
	m_pPlayer->SendPacket(&msg);
}