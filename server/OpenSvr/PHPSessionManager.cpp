#include "StdAfx.h"
#include "PHPSessionManager.h"
#include "PHPSession.h"
#include "PHPSessionFactory.h"
//#include "MapServerSession.h"
#include <time.h>

PHPSessionManager::PHPSessionManager(void)
{
	time( &m_tNowTime );
	m_tLastTime = m_tNowTime;
}

PHPSessionManager::~PHPSessionManager(void)
{

}

//�����ҵ�������
bool PHPSessionManager::AddUser(PHPSession* pUser)
{
	_ASSERT(pUser != NULL);

	if(!pUser)
		return false;
	
	//�����Ƿ��Ѿ����ڴ����
	Guild_Map::iterator it = m_GUIDMap.find(pUser->GetGUID());

	//�����ܳ����ظ�GUID�����
	_ASSERT(it==m_GUIDMap.end());

	if(it != m_GUIDMap.end())
		return false;

	//��������б�
	m_GUIDMap.insert(std::make_pair(pUser->GetGUID(),pUser));

	return true;
}

//ͨ��GUID�������
PHPSession* PHPSessionManager::FindUserByGUID(DWORD dwGUID)
{
	Guild_Map::iterator itguid = m_GUIDMap.find(dwGUID);
	if(itguid != m_GUIDMap.end())
		return itguid->second;

	return NULL;
}

//�ӹ�����ɾ�����
bool PHPSessionManager::RemoveUser(DWORD dwGUID)
{
	m_GUIDMap.erase(dwGUID);

	return true;
}

//�ͷŹ�����
VOID PHPSessionManager::Release( PHPSessionFactory* pUserFactory )
{
	Guild_Map::iterator it;

	PHPSession* pUser = NULL;
	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();++it)
	{
		pUser = it->second;
		if(pUser)
			pUserFactory->FreePHPSession(pUser);
	}

	m_GUIDMap.clear();
}

//��ͼ�������Ͽ������¼�����
void PHPSessionManager::OnMapSvrDisconnect()
{
	Guild_Map::iterator it;
	PHPSession* pUser = NULL;

	for (it = m_GUIDMap.begin();it!=m_GUIDMap.end();)
	{
		pUser = it->second;

		++it;

		if(pUser)
		{
			//pUser->SetDisConnectFromMap(TRUE);
			pUser->Disconnect();
		}
	}
}

//���ӳ�ʱ���
void PHPSessionManager::CheckTimeOut()
{
  	time( &m_tNowTime );

	// ���ʱ���� todo::�ݶ�5��
	if (m_tNowTime >= m_tLastTime + 5)
	{
		PHPSession* pSession = NULL;
		for (Guild_Map::iterator it = m_GUIDMap.begin(); it != m_GUIDMap.end();)
		{
			pSession = it->second;

			// ���ڴ˴���Ϊ�˷�ֹԭ������Ϊ��
			++it;

			if (m_tNowTime >= pSession->GetCreateTime() + 120)
			{
				OPENLOG(GUI_LEVEL, "���ӳ�ʱ���Ͽ�[GUID:%d]", pSession->GetGUID());

				//�ͻ��˳�ʱ
				pSession->Disconnect();
			}
		}

		m_tLastTime = m_tNowTime;
	}
}

void PHPSessionManager::Update()
{
	CheckTimeOut();
}