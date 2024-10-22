// 停服公告功能

#include "stdafx.h"
#include "StopMessage.h"
#include "../Common/MsgDefine.h"
#include "PlayerManager.h"
#include "ChatManager.h"
#include "../Common/DBCDefine.h"
#include "../ToolKit/DBCFileManager.h"
StopMessage::StopMessage()
{
}
StopMessage::~StopMessage()
{

}

void StopMessage::Update( DWORD dwTick )
{
// 	if (!IsOpen())
// 	{
// 		return;
// 	}
// 	if (dwTick>=m_dwTick)
// 	{
// 		if (m_dwSumTime<m_dwTick2)
// 		{
// 			Close();
// 			return;
// 		}
// 		DBCFile * pFile = DBCFileManager::Instance()->GetDBC(DBC_STOP_MESSAGE);
// 		if (NULL == pFile )
// 		{
// 			Close();
// 			return;
// 		}
// 		StopMsg* pMsg = (StopMsg*)pFile->GetRecord(1);
// 		if (NULL == pMsg )
// 		{
// 			Close();
// 			return;
// 		}
// 		m_dwSumTime = m_dwSumTime-m_dwTick2;
// 		MSG_C2M_CHAT_SYN msg;
// 		memset(&msg.m_tChatInfo, 0, sizeof(msg.tChatReq));
// 		msg.tChatReq.byChatType = CHAT_TYPE_SYSTEM;
// 		int nTime = m_dwSumTime/(60*1000);
// 		if (0 == nTime )
// 		{
// 			nTime =1;
// 		}
// 		sprintf(msg.tChatReq.szChatText,pMsg->MsgInfo,nTime);
// 		size_t nMsgLen = strlen(msg.tChatReq.szChatText);
// //		msg.tChatReq.shChatTextSize = (SHORT)nMsgLen;
// 		Player*pPlayer =  PlayerManager::Instance()->FindPlayerByCharID(m_dwCharID);
// 		if (NULL == pPlayer)
// 		{
// 			Close();
// 			return;
// 		}
// 		ChatManager::Instance()->OnNetChat(pPlayer, &msg);
// 		if (m_dwSumTime<=0)
// 		{
// 			Close();
// 		}
// 		m_dwTick = m_dwTick2;
// 
// 	}
// 	else
// 	{
// 		m_dwTick = m_dwTick - dwTick;
// 	}
}

void StopMessage::Reset( DWORD dwCharID, DWORD dwSumTime, DWORD dwTick )
{
	m_dwCharID =dwCharID;
	m_dwSumTime =dwSumTime*60*1000;  
	m_dwTick = dwTick*60*1000;
	m_dwTick2 = m_dwTick;
	Open();
}


