#include "stdafx.h"
#include "SystemNotice.h"
#include "..\PlayerManager.h"
#include "..\MapServer.h"
#include "..\GateServerSession.h"


CSystemNotice::CSystemNotice(WORD wNoticeId, DWORD dwServerId)
{
	m_msg.m_wNoticeId = wNoticeId;
	m_msg.m_wParamListNum = 0;
	m_msg.m_wServerId = WORD(dwServerId);
	memset(m_msg.m_szText, 0, sizeof(m_msg.m_szText));
}

CSystemNotice::~CSystemNotice()
{}


// 添加参数
void CSystemNotice::AddParam(const char *pszParam)
{
	if (m_msg.m_wParamListNum >= MAX_SYSTEM_NOTICE_PARAM_NUM)
	{
		MAPLOG(ERROR_LEVEL, "公告参数列表已满[NoticeId:%d]", m_msg.m_wNoticeId);
		return;
	}
	SystemNoticeParam *pParam = &m_msg.m_ParamList[m_msg.m_wParamListNum++];
	sprintf_s(pParam->szText, sizeof(pParam->szText) - 1, "%s", pszParam);
}

// 添加参数
void CSystemNotice::AddParam(DWORD dwParam)
{
	if (m_msg.m_wParamListNum >= MAX_SYSTEM_NOTICE_PARAM_NUM)
	{
		MAPLOG(ERROR_LEVEL, "公告参数列表已满[NoticeId:%d]", m_msg.m_wNoticeId);
		return;
	}
	SystemNoticeParam *pParam = &m_msg.m_ParamList[m_msg.m_wParamListNum++];
	sprintf_s(pParam->szText, sizeof(pParam->szText) - 1, "%d", dwParam);
}
	// 设置文本
void CSystemNotice::SetText(const char *pszText)
{
	strcpy_s(m_msg.m_szText, sizeof(m_msg.m_szText) - 1, pszText);
}

// 发送公告
void CSystemNotice::SendMsg(DWORD dwServerId)
{
	g_MapServer.GetGateSession()->SendPacket(0, S2C_SYSTEM_NOTICE, &m_msg);
	//PlayerManager::Instance()->SendPacketAround(&m_msg, dwServerId);
}