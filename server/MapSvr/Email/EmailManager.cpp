/**\file
    工会管理类
   \author Kevin
   \date 2015/8/4
   \note 邮件流程: 登录(从数据库加载邮件数据)--->游戏中(操作缓存数据)--->下线(存储数据到数据库)  
*/

#include "stdafx.h"
#include "EmailManager.h"
#include "../ToolKit/Rand.h"
#include "../Common/CommonLogic.h"
#include "../Common/DBCDefine.h"
#include "../Common/MsgDefine.h"
#include "../Player.h"
#include "../MapSvrQuery.h"
#include "../PlayerManager.h"
#include "../MapServer.h"
#include "../ConfigManager.h"

DWORD EmailManager::m_dwSerialId = 1;

EmailManager::EmailManager()
{
	
}

EmailManager::~EmailManager()
{

}

bool EmailManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pEmailInfo = &pPlayer->GetRoleFullInfo().tEmailInfo;
	return true;
}

// 请求邮件列表
VOID EmailManager::OnNetEmailList()
{
	DeleteTimeOutEmail();
	SendEmailList();
	m_pEmailInfo->wIsHaveNewEmail = false;
}


void  EmailManager::SendEmailList()
{
	if (! m_pPlayer)
		return;

	// todo::模拟测试数据
//  	{
//   		{
//   			// 发邮件通知(夺宝)
//   			EmailInfo tEailInfo(5);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_ITEM, 14401);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
//   			EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
//   		}
//   
//   		// 发邮件通知目标(防守成功)
//   		{
//   		// 发邮件通知
//   		EmailInfo tEailInfo(1);
//   		tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
//   		EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, false);
//   		}
//   
//   		// 竞技场防守失败(排名降低)
//   		{
//   			// 发邮件通知
//   			EmailInfo tEailInfo(2);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
//   			tEailInfo.AddContext(EMAIL_CONTEXT_INT, 5);
//   			EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, false);
//   		}
//   
//   		// 发邮件通知目标(好友)
//   		{
//   			EmailInfo tEailInfo(3);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
//   			EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, false);
//   		}
//   
//   		// 竞技场防守失败(排名不变)
//   		{
//   			// 发邮件通知
//   			EmailInfo tEailInfo(4);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_STRING, m_pPlayer->GetRoleName());
//   			EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
//   		}
//   
//   		// 发邮件通知(充值)
//   		{
//   			EmailInfo tEailInfo(6);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_INT, 78);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_INT, 78 * 10);
//   			tEailInfo.AddContext(EMAIL_CONTEXT_INT, 600);
//   			EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
//   		}
// 
// 		// 发邮件通知(充值)
// 		{
// 			EmailInfo tEailInfo(0);
// 			strcpy_s(tEailInfo.szTitle, sizeof(tEailInfo.szTitle), "dadiao");
// 			strcpy_s(tEailInfo.szText, sizeof(tEailInfo.szText), "dsfsadfasdfasd fuck you");
// 			EmailManager::AddEmail(m_pPlayer->GetRoleId(), tEailInfo, true);
// 		}
//  	}

	MSG_S2C_EMAIL_LIST_RESP msg;
	msg.m_byEmailNum = m_pEmailInfo->wEmailCount;
	int nEmailSize = msg.m_byEmailNum*sizeof(EmailInfo);
	memcpy_s(msg.m_EmailList, nEmailSize, m_pEmailInfo->arEmailList, nEmailSize);

	m_pPlayer->SendPacket(&msg);
}
bool EmailManager::AddEmail(EmailInfo &sEmail,bool bIsSend)
{
	CHECK_POINTER_RET(m_pPlayer, false)

	if (m_pEmailInfo->wEmailCount >= MAX_EMAIL_COUNT)
	{
// 		int nDeleteNum = 10;
// 		memcpy_s(m_pEmailInfo->arEmailList, sizeof(m_pEmailInfo->arEmailList), 
// 			&m_pEmailInfo->arEmailList[nDeleteNum], sizeof(EmailInfo)* (m_pEmailInfo->wEmailCount - nDeleteNum))

		MAPLOG(GUI_LEVEL, "玩家的邮件已满!(RoleName:%s)!", m_pPlayer->GetUTF8RoleName());
		return false;
	}

	EmailInfo *pEmailInfo = GetEmailInfo(sEmail.dwID);
	if (pEmailInfo)
	{
		MAPLOG(GUI_LEVEL, "重复添加邮件!(RoleName:%s,EmailId:%d)!",
			m_pPlayer->GetUTF8RoleName(), sEmail.dwID);
		return false;
	}

	m_pEmailInfo->arEmailList[m_pEmailInfo->wEmailCount++] = sEmail;
	m_pEmailInfo->wIsHaveNewEmail = true;

	if (bIsSend)
	{ 
		static MSG_S2C_EMAIL_NEW_NOTIFY msg;
		msg.m_tEmailInfo = sEmail;
		m_pPlayer->SendPacket(&msg);
	}

	// 角色信息更新
	m_pPlayer->RoleInfoUpdate();

	return true;
}

// 添加邮件 
bool EmailManager::AddEmail(DWORD dwRoleId, DWORD dwInfoId, EmailContext *pContextList, WORD wContextNum, bool bIsSend)
{ 
 	// 填充邮件信息
 	EmailInfo sEmail;
	sEmail.CleanUp();
	sEmail.dwID = EmailManager::GetEmailId();
	sEmail.wInfoId = dwInfoId;
	sEmail.dwReceiveTime = time(NULL);

	if (pContextList && wContextNum)
	{
		int nCopySize = wContextNum * sizeof(EmailContext);
		memcpy_s(sEmail.arContextList, nCopySize, pContextList, nCopySize);
		sEmail.wContextNum = wContextNum;
	}
 
 	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId); 
 
 	// 在缓存添加邮件
 	if (pPlayer)
 	{
 		EmailManager *pEmailMgr =  pPlayer->GetEmailManager();
		pEmailMgr->AddEmail(sEmail, bIsSend);
 	}
 	else
 	{
 		// 直接在数据库插入
 		InsertEmailQuery* pQuery = InsertEmailQuery::ALLOC();
 		pQuery->SetIndex(MAP_QUERY_INSERT_EMAIL);
 		pQuery->SetThreadIndex(0);
		pQuery->SetEmailInfo(sEmail);
 		pQuery->SetKey(dwRoleId);
 		g_MapServer.MapDBQuery(pQuery);
 	}

	return true;
}

// 添加邮件
bool EmailManager::AddEmail(DWORD dwRoleId, EmailInfo &tEmailInfo, bool bIsSend)
{
	if (!tEmailInfo.dwID)
		tEmailInfo.dwID = EmailManager::GetEmailId();

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);

	// 在缓存添加邮件
	if (pPlayer)
	{
		EmailManager *pEmailMgr = pPlayer->GetEmailManager();
		pEmailMgr->AddEmail(tEmailInfo, bIsSend);
	}
	else
	{
		// 直接在数据库插入
		InsertEmailQuery* pQuery = InsertEmailQuery::ALLOC();
		pQuery->SetIndex(MAP_QUERY_INSERT_EMAIL);
		pQuery->SetThreadIndex(0);
		pQuery->SetEmailInfo(tEmailInfo);
		pQuery->SetKey(dwRoleId);
		g_MapServer.MapDBQuery(pQuery);
	}
	return true;
}

// 添加邮件
bool EmailManager::AddEmailByGM(DWORD dwRoleId, EmailInfo &tEmailInfo)
{
	// 填充邮件信息
	//tEmailInfo.dwID = EmailManager::GetEmailId();

	Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(dwRoleId);

	// 在缓存添加邮件
	if (pPlayer)
	{
		EmailManager *pEmailMgr = pPlayer->GetEmailManager();
		pEmailMgr->AddEmail(tEmailInfo);
	}
	else
	{
		// 直接在数据库插入
		InsertEmailQuery* pQuery = InsertEmailQuery::ALLOC();
		pQuery->SetIndex(MAP_QUERY_INSERT_EMAIL);
		pQuery->SetThreadIndex(0);
		pQuery->SetEmailInfo(tEmailInfo);
		pQuery->SetKey(dwRoleId);
		g_MapServer.MapDBQuery(pQuery);
	}

	return true;
}

VOID EmailManager::SaveAllEmailIntoDB()
{
	SaveAllEmailQuery* pQuery = SaveAllEmailQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_SAVE_All_EMAIL);
	pQuery->SetThreadIndex(0);
	pQuery->SetCharId(m_pPlayer->GetCharID());
	pQuery->SetCharEmailInfo(*m_pEmailInfo);
	g_MapServer.MapDBQuery(pQuery);
}

// 是否有红点
bool EmailManager::IsHaveRedPoint()
{
	return m_pEmailInfo->wIsHaveNewEmail ? true : false;
}

void EmailManager::DeleteTimeOutEmail()
{
	if (!m_pEmailInfo->wEmailCount)
		return;

	DWORD dwCurrTime = (DWORD)time(NULL);
	DWORD dwValidTime = g_GetConfigValue(130) * 60 * 60;

	EmailInfo arEmailList[MAX_EMAIL_COUNT];	// 邮件列表
	WORD wEmailCount = 0;					// 邮件数量

	int nEmailSize = sizeof(EmailInfo);

	for (int i = 0; i < m_pEmailInfo->wEmailCount; ++i)
	{
		// 过滤过期邮件
		if (dwCurrTime < m_pEmailInfo->arEmailList[i].dwReceiveTime + dwValidTime)
			memcpy_s(&arEmailList[wEmailCount++], nEmailSize, &m_pEmailInfo->arEmailList[i], nEmailSize);
	}

	// 数据有变动
	if (wEmailCount < m_pEmailInfo->wEmailCount)
	{
		memcpy_s(m_pEmailInfo->arEmailList, sizeof(m_pEmailInfo->arEmailList), arEmailList, nEmailSize * wEmailCount);
		m_pEmailInfo->wEmailCount = wEmailCount;
	}

// 	for (int i = m_pEmailInfo->wEmailCount - 1; i >= 0; --i)
// 	{
// 		// 有效时间未到
// 		if (dwCurrTime < m_pEmailInfo->arEmailList[i].dwReceiveTime + dwValidTime)
// 			continue;
// 
// 		if (i + 1 < m_pEmailInfo->wEmailCount)
// 		{
// 			int nRemainNum = m_pEmailInfo->wEmailCount - i - 1; // 剩余数量
// 			int nDataSize = sizeof(EmailInfo)* nRemainNum;	
// 			memcpy_s(m_pEmailInfo->arEmailList, nDataSize, &m_pEmailInfo->arEmailList[i + 1], nDataSize);
// 			m_pEmailInfo->wEmailCount = nRemainNum;
// 		}
// 		else
// 		{
// 			m_pEmailInfo->wEmailCount = 0;
// 		}
// 
// 		break;
// 	}
}

EmailInfo* EmailManager::GetEmailInfo(DWORD dwEmailId)
{
	for (BYTE i = 0; i < m_pEmailInfo->wEmailCount; ++i)
	{
		if (m_pEmailInfo->arEmailList[i].dwID == dwEmailId)
		{
			return &m_pEmailInfo->arEmailList[i];
		}
	}

	return NULL;
}

DWORD EmailManager::GetEmailId()
{
	static int nLastMin = 0;

	time_t t = time(NULL);
	tm tTime;
	localtime_s(&tTime, &t);
	if (m_dwSerialId >= 1000)
	{
		if (nLastMin == tTime.tm_min)
		{
			tTime.tm_min++;
		}

		nLastMin = tTime.tm_min;
		m_dwSerialId = 1;
	}
	else
	{
		m_dwSerialId++;
	}
	// 年日 + 小时 + 分钟 + 系列ID
	return tTime.tm_yday * 10000000 + tTime.tm_hour * 100000 + tTime.tm_min * 1000 + m_dwSerialId;	
}


