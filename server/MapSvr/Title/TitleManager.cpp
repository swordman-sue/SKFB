#include "stdafx.h"
#include "TitleManager.h"
#include "..\ConfigManager.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"


CTitleManager::CTitleManager()
{

}

CTitleManager::~CTitleManager()
{

}

void CTitleManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pTitleData = &pPlayer->GetRoleFullInfo().tSecondInfo.tTitleInfo;
}

// �����ɫ�ƺ���Ϣ
void CTitleManager::OnNetRoleTitleInfo()
{

	DeleteTimeOutTitle();

	SendRoleTitleInfo();
}

// ʹ�óƺ�
void CTitleManager::OnNetUseTitle(int nTitleId)
{
	WORD wFromMsgId = C2S_USE_TITLE_REQ;

	DeleteTimeOutTitle();

	// ����Ƿ���Щ�ƺ�
	if (!GetTitle(nTitleId))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]��һ�û���Щ�ƺ�[TitleId:%d]", __FUNCTION__, nTitleId);
		return;
	}

	const Title_Config *pTitleCfg = g_pCfgMgr->GetTitle(nTitleId);
	if (!pTitleCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ƺ�����[TitleId:%d]", __FUNCTION__, nTitleId);
		return;
	}

	// �ƺ�
	if (1 == pTitleCfg->nType)
	{
		m_pTitleData->nCurUseTitleId = nTitleId;
	}
	// �ǳ�
	else
	{
		m_pTitleData->nCurUseNicknameId = nTitleId;
	}
	

	m_pPlayer->UpdateFightingPower();

	SendRoleTitleInfo();

	MSG_S2C_USE_TITLE_RESP msg;
	msg.m_nTitleId = nTitleId;
	m_pPlayer->SendPacket(&msg);
}

// ��ӳƺ�
void CTitleManager::AddTitleEx(DWORD dwServerId, DWORD dwRoleId, int nTitleId)
{
	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);
	if (pPlayer)
	{
		pPlayer->GetTitleManager()->AddTitle(nTitleId, time(NULL));
	}
	else
	{
		// �������ݿ�
		RoleTmpData_Update* pQuery = RoleTmpData_Update::ALLOC();
		pQuery->SetIndex(DB_ROLE_TEMP_DATA_UPDATE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		pQuery->SetRoleId(dwRoleId);
		pQuery->SetServerId(dwServerId);

		pQuery->AddParam(time(NULL));
		pQuery->AddParam(nTitleId);
		pQuery->SetDataType(ROLE_TEMP_DATA_TITLE);
		g_MapServer.MapDBQuery(pQuery);
	}
}

// ���ͳƺ�����
void CTitleManager::SendRoleTitleInfo()
{
	MSG_S2C_ROLE_TITLE_INFO_RESP msg;
	msg.m_TitleInfo = *m_pTitleData;
	m_pPlayer->SendPacket(&msg);
}

// ��ӳƺ�
void CTitleManager::AddTitle(int nTitleId, DWORD dwAddTime, bool bNotice)
{
	DeleteTimeOutTitle();

	const Title_Config *pTitleCfg = g_pCfgMgr->GetTitle(nTitleId);
	if (!pTitleCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ƺ�����[TitleId:%d]", __FUNCTION__, nTitleId);
		return;
	}

	if (m_pTitleData->wTitleNum >= MAX_TITAL_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ƺ��б�����[num:%d]", __FUNCTION__, m_pTitleData->wTitleNum);
		return;
	}

	if (GetTitle(nTitleId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ӵ�д˳ƺ�����[TitleId:%d]", __FUNCTION__, nTitleId);
		return;
	}

	TitleData tTitleData;
	tTitleData.nTitleId = nTitleId;
	tTitleData.nOverTime = pTitleCfg->nValidTime ? dwAddTime + pTitleCfg->nValidTime * 60 : 0;
	m_pTitleData->arTitleList[m_pTitleData->wTitleNum++] = tTitleData;

	// �ƺ�
	if (1 == pTitleCfg->nType && !m_pTitleData->nCurUseTitleId)
	{
		m_pTitleData->nCurUseTitleId = nTitleId;
	}
	// �ǳ�
	else if (2 == pTitleCfg->nType && !m_pTitleData->nCurUseNicknameId)
	{
		m_pTitleData->nCurUseNicknameId = nTitleId;
	}

	if (bNotice)
	{
		SendRoleTitleInfo();
		m_pPlayer->UpdateFightingPower();
	}
}

// �Ƴ��ƺ�
void CTitleManager::RemoveTitle(int nTitleId)
{
	for (BYTE i = 0; i < m_pTitleData->wTitleNum; ++i)
	{
		if (m_pTitleData->arTitleList[i].nTitleId == nTitleId)
		{
			m_pTitleData->arTitleList[i] = m_pTitleData->arTitleList[--m_pTitleData->wTitleNum];
			return;
		}
	}
}

// ȡ�ƺ�
const TitleData* CTitleManager::GetTitle(int nTitleId)
{
	for (BYTE i = 0; i < m_pTitleData->wTitleNum; ++i)
	{
		if (m_pTitleData->arTitleList[i].nTitleId == nTitleId)
			return &m_pTitleData->arTitleList[i];
	}
	return NULL;
}

// ɾ�����ڳƺ�
void CTitleManager::DeleteTimeOutTitle()
{
	TitleData arTitleList[MAX_TITAL_NUM];
	WORD wTitleNum = 0;
	int nCurTime = time(NULL);
	bool bUpdate = false;
	for (WORD i = 0; i < m_pTitleData->wTitleNum; ++i)
	{
		TitleData &tTitle = m_pTitleData->arTitleList[i];
		// ��û���ڵ�
		if (!tTitle.nOverTime || nCurTime < tTitle.nOverTime)
		{
			arTitleList[wTitleNum++] = tTitle;
		}
		// �Ѿ�����
		else
		{
			if (tTitle.nTitleId == m_pTitleData->nCurUseTitleId)
				m_pTitleData->nCurUseTitleId = 0;
			else if (tTitle.nTitleId == m_pTitleData->nCurUseNicknameId)
				m_pTitleData->nCurUseNicknameId = 0;

			bUpdate = true;
		}
	}

	if (bUpdate)
	{
		m_pTitleData->wTitleNum = wTitleNum;
		int nCopySize = wTitleNum * sizeof(TitleData);
		memcpy_s(m_pTitleData->arTitleList, nCopySize, arTitleList, nCopySize);

		m_pPlayer->UpdateFightingPower();
	}
}