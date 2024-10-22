#include "StdAfx.h"
#include "UserManager.h"
#include "User.h"
#include "UserFactory.h"
#include "MapServerSession.h"
#include <time.h>

UserManager::UserManager(void)
{
	time( &m_tNowTime );
	m_tLastTime = m_tNowTime;

	m_UserLoopListCount = 0;

	m_ulCheckTimeOutTimer = GetTickCount64();
}

UserManager::~UserManager(void)
{

}

//�����ҵ�������
bool UserManager::AddUser(User* pUser)
{
	_ASSERT(pUser != NULL);

	if(!pUser)
		return false;
	
	//�����Ƿ��Ѿ����ڴ����
	UserMap::iterator it =	m_GUIDMap.find(pUser->GetGUID());

	//�����ܳ����ظ�GUID�����
	_ASSERT(it==m_GUIDMap.end());

	if(it != m_GUIDMap.end())
		return false;

	//��������б�
	m_GUIDMap.insert(std::make_pair(pUser->GetGUID(),pUser));

	// ���±����б�
	pUser->SetIndex(m_UserLoopListCount);
	m_UserLoopList[m_UserLoopListCount] = pUser;
	++m_UserLoopListCount;

	return true;
}

//ͨ��GUID�������
User* UserManager::FindUserByGUID(DWORD dwGUID)
{
	UserMap::iterator itguid = m_GUIDMap.find(dwGUID);
	if(itguid != m_GUIDMap.end())
		return itguid->second;

	return NULL;
}

//�ӹ�����ɾ�����
bool UserManager::RemoveUser(DWORD dwGUID)
{
	User* pRemoveUser = FindUserByGUID(dwGUID);
	if (m_UserLoopListCount && pRemoveUser)
	{
		User *pLastUser = m_UserLoopList[m_UserLoopListCount - 1];
		pLastUser->SetIndex(pRemoveUser->GetIndex());
		m_UserLoopList[pRemoveUser->GetIndex()] = pLastUser;
		--m_UserLoopListCount;
	}

	m_GUIDMap.erase(dwGUID);

	return true;
}

//�ͷŹ�����
VOID UserManager::Release( UserFactory* pUserFactory )
{
	UserMap::iterator it;

	User* pUser = NULL;
	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();++it)
	{
		pUser = it->second;
		if(pUser)
			pUserFactory->FreeUser(pUser);
	}

	m_GUIDMap.clear();
	m_UserLoopListCount = 0;
}

//��ͼ�������Ͽ������¼�����
void UserManager::OnMapSvrDisconnect()
{
	UserMap::iterator it;
	User* pUser = NULL;

	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();)
	{
		pUser = it->second;

		++it;

		if(pUser)
		{
			pUser->SetDisConnectFromMap(TRUE);
			pUser->Disconnect();
		}
	}
}

//���ӳ�ʱ���
void UserManager::CheckTimeOut()
{
	//time_t tTimeGap;
	time(&m_tNowTime);

	// 10����һ��
	if (GetTickCount64() >= m_ulCheckTimeOutTimer + 5000)
	{
		User* pUser = NULL;
		for (DWORD i = 0; i < m_UserLoopListCount; ++i)
		{
			pUser = m_UserLoopList[i];
			if (!pUser)
				continue;

			if (m_tNowTime >= pUser->GetStamp() + g_GateServer.GetSocketTimeOut())
			{
				GATELOG(GUI_LEVEL, "��������ʱ���Ͽ�[GUID:%d]", pUser->GetGUID());

				//�ͻ��˳�ʱ
				pUser->Disconnect();
			}	
		}

		m_ulCheckTimeOutTimer = GetTickCount64();
	}

	m_tLastTime = m_tNowTime;
}

//���ӳ�ʱ���
// void UserManager::CheckTimeOut()
// {
// 	//time_t tTimeGap;
// 	time(&m_tNowTime);
// 
// 	// ���ʱ����
// 	if ((m_tNowTime - m_tLastTime) < g_GateServer.GetCheckTimeGap())
// 	{
// 		return;
// 	}
// 
// 	//UserMap::iterator it;
// 	User* pUser = NULL;
// 	for (UserMap::iterator it = m_GUIDMap.begin(); it != m_GUIDMap.end();)
// 	{
// 		pUser = it->second;
// 
// 		// ���ڴ˴���Ϊ�˷�ֹԭ������Ϊ��
// 		++it;
// 
// 		// ��������5���Ӻ��������
// 		if (pUser && m_tNowTime >= pUser->GetCreateTime() + 300)
// 		{
// 			if (pUser->GetSendFlag() == TRUE)
// 			{
// 				pUser->Process(DWORD(m_tNowTime));
// 				if (m_tNowTime >= pUser->GetStamp() + g_GateServer.GetSocketTimeOut())
// 				{
// 					GATELOG(GUI_LEVEL, "��������ʱ���Ͽ�[GUID:%d]", pUser->GetGUID());
// 
// 					//�ͻ��˳�ʱ
// 					pUser->Disconnect();
// 				}
// 			}
// 			else
// 			{
// 				if (m_tNowTime >= pUser->GetCreateTime() + g_GateServer.GetConnectTimeOut())
// 				{
// 					GATELOG(GUI_LEVEL, "���ӳ�ʱ���Ͽ�[GUID:%d]", pUser->GetGUID());
// 
// 					//�ͻ��˳�ʱ
// 					pUser->Disconnect();
// 				}
// 			}
// 		}
// 	}
// 
// 	m_tLastTime = m_tNowTime;
// }

void UserManager::Update()
{
	CheckTimeOut();
}

void UserManager::SendPacket(WORD dwMsgID, unsigned char * pData, WORD wDataSize, DWORD dwServerId)
{
	if (dwServerId)
	{
		for (DWORD i = 0; i < m_UserLoopListCount; ++i)
		{
			User *pUser = m_UserLoopList[i];
			if (pUser->GetServerId() == dwServerId)
				pUser->SendPacket(dwMsgID, pData, wDataSize);
		}
	}
	else
	{
		for (DWORD i = 0; i < m_UserLoopListCount; ++i)
		{
			m_UserLoopList[i]->SendPacket(dwMsgID, pData, wDataSize);
		}
	}
}