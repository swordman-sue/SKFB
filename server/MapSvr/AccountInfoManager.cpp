////////////////////////////////////////////////////////////////////////////////
//	Filename     :AccountInfoManager.cpp
//	Created by   :Kevin
//	Created date :2014/12/17
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AccountInfoManager.h"
#include "ObjectFactory.h"
#include "AccountInfo.h"
#include "PlayerManager.h"
#include "Player.h"
#include "..\Common\CommonLogic.h"

AccountInfoManager::AccountInfoManager(void)
{
	//��ʼ��DBThread������������
	memset(m_dwMapDBUser, 0, sizeof(m_dwMapDBUser));
}

AccountInfoManager::~AccountInfoManager(void)
{

}

//��ZoneID,AccIDΪKEY�������
AccountInfo* AccountInfoManager::GetAccountInfoByAccID(DWORD dwPlatform, const char* pszAccID)
{
	AccKey tAccKey = {0};

	tAccKey.m_dwPlatform = dwPlatform;
	strncpy(tAccKey.m_szAccID, pszAccID, sizeof(tAccKey.m_szAccID));
	AccIDMapIter it = m_AccountInfoByAccIDMap.find(tAccKey);

	if(it!=m_AccountInfoByAccIDMap.end())
		return it->second;
	else
		return NULL;
}

//��GUIDΪKEY�������
AccountInfo* AccountInfoManager::GetAccountInfoByGUID(DWORD dwGUID)
{
	GuidMapIter it = m_AccountInfoByGUIDMap.find(dwGUID);

	if(it!=m_AccountInfoByGUIDMap.end())
		return it->second;
	else
		return NULL;
}

VOID AccountInfoManager::AddAccountInfo(AccountInfo* pAccountInfo)
{
   CHECK_POINTER(pAccountInfo);

   AccountInfo* pInfo = GetAccountInfoByAccID(pAccountInfo->GetChannelUID(), pAccountInfo->GetAccID());
	if(pInfo)
	{
		MAPLOG(ERROR_LEVEL, "AddAccountInfoʧ��!");
		return;
	}

	//����DBThread������
	pAccountInfo->SetDBIndex(GetMapDBIndex());
	
	//׷����������
	AccKey tAccKey = {0};
	tAccKey.m_dwPlatform = pAccountInfo->GetChannelUID();
	strncpy(tAccKey.m_szAccID, pAccountInfo->GetAccID(), sizeof(tAccKey.m_szAccID));
	m_AccountInfoByAccIDMap.insert(std::make_pair(tAccKey,pAccountInfo));

	m_AccountInfoByGUIDMap.insert(std::make_pair(pAccountInfo->GetGuid(),pAccountInfo));
}

VOID AccountInfoManager::RemoveAccountInfo(AccountInfo* pAccountInfo)
{
	if(!pAccountInfo)
		return;

	//�ͷ�DBThread������
	ReleaseMapDBIndex(pAccountInfo->GetDBIndex());

	AccKey tAccKey = {0};
	tAccKey.m_dwPlatform = pAccountInfo->GetChannelUID();
	strncpy(tAccKey.m_szAccID, pAccountInfo->GetAccID(), sizeof(tAccKey.m_szAccID));
	m_AccountInfoByAccIDMap.erase(tAccKey);
	m_AccountInfoByGUIDMap.erase(pAccountInfo->GetGuid());
}

VOID AccountInfoManager::RemoveAccountInfoByGUID(DWORD dwGUID)
{
	AccountInfo* pInfo = GetAccountInfoByGUID(dwGUID);
	if(!pInfo)
		return;

	//�ͷ�DBThread������
	ReleaseMapDBIndex(pInfo->GetDBIndex());

	AccKey tAccKey = {0};
	tAccKey.m_dwPlatform = pInfo->GetChannelUID();
	strncpy(tAccKey.m_szAccID, pInfo->GetAccID(), sizeof(tAccKey.m_szAccID));
	m_AccountInfoByAccIDMap.erase(tAccKey);
	m_AccountInfoByGUIDMap.erase(pInfo->GetGuid());
}

VOID AccountInfoManager::Release()
{
	for (AccIDMapIter it = m_AccountInfoByAccIDMap.begin();it != m_AccountInfoByAccIDMap.end(); ++it)
	{
		if(it->second)
			ObjectFactory::Instance()->FreeAccountInfo(it->second);
	}

	m_AccountInfoByAccIDMap.clear();
	m_AccountInfoByGUIDMap.clear();
}

//�߳����
VOID AccountInfoManager::KickoutAccount(AccountInfo* pAccInfo)
{
	CHECK_POINTER(pAccInfo)

	USER_STATE UserState = pAccInfo->GetUserState();

	//��ǰ�û�����������״̬
	if (UserState == USER_LOGOUTING_STATE)
	{	
		MAPLOG(GUI_LEVEL, "�߳����ʧ��UserState == USER_LOGOUTING_STATE[%s]", pAccInfo->GetAccID());
		return;
	}
	//��ǰ�û�����ѡ���ɫ״̬  todo::��ǰ�û�����ѡ��������򴴽���ɫ״̬
	else if (UserState == USER_SELECT_SERVER_STATE || 
		UserState == USER_CREATE_ROLE_STATE || 
		UserState == USER_READY_CREATE_ROLE_STATE)
	{
		//�Ƴ��˺���Ϣ
		RemoveAccountInfo(pAccInfo);
		ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
	}
	//��ǰ�û����ڼ��ؽ�ɫ״̬
	else if (UserState == USER_LOAD_STATE)
	{
		//��ɫ���ݼ���״̬�������ݼ������״̬
		//1 �Ƴ������Ϣ
		Player* pPlayer = pAccInfo->GetPlayer();
		if(!pPlayer)
		{
			MAPLOG(GUI_LEVEL, "KickoutAccount(USER_LOAD_STATE):������Ҷ���ʧ�ܣ�ACCID=%s.", pAccInfo->GetAccID());
			RemoveAccountInfo(pAccInfo);
			ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
			return;
		}
		PlayerManager::Instance()->RemovePlayer(pPlayer);

		//2 �Ƴ��˺���Ϣ
		RemoveAccountInfo(pAccInfo);
		ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
	}
	//��ǰ�û�������Ϸ��״̬
	else if (UserState == USER_AT_GAME_STATE)
	{
		//��ɫ����Ϸ�У���Ҫ�ȱ��������Ϣ��DB
		Player* pPlayer = pAccInfo->GetPlayer();
		if(!pPlayer)
		{
			MAPLOG(GUI_LEVEL, "KickoutAccount(USER_AT_GAME_STATE):������Ҷ���ʧ�ܣ�ACCID=%s.", pAccInfo->GetAccID());
			RemoveAccountInfo(pAccInfo);
			ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
			return;
		}
		
		//���Logout
		pPlayer->OnLogout();

		//�������ݵ�DB
		pPlayer->OnSave(SAVE_DB_TYPE_LOGOUT);

		//�������״̬Ϊ������,�ȴ�DB������Ϻ���к�������
		pAccInfo->SetUserState(USER_LOGOUTING_STATE);
	}
}

VOID AccountInfoManager::KickoutAllAccount()
{
	AccIDMapIter it;

	for (it = m_AccountInfoByAccIDMap.begin();it!=m_AccountInfoByAccIDMap.end();)
	{
		AccountInfo* pInfo = it->second;

		++it;

		if(pInfo)
		{
			KickoutAccount(pInfo);
		}
	}
}

VOID AccountInfoManager::OnGateSvrDisconnect()
{
	KickoutAllAccount();
}

VOID AccountInfoManager::ProcessDBLogoutAck(AccountInfo* pAccInfo)
{
	if (pAccInfo)
	{
		if (pAccInfo->GetUserState() == USER_LOGOUTING_STATE)
		{
			PlayerManager::Instance()->RemovePlayer(pAccInfo->GetPlayer());
			RemoveAccountInfo(pAccInfo);
			ObjectFactory::Instance()->FreeAccountInfo(pAccInfo);
		}
		else
		{
			//���״̬��ƥ�䣬д��־
			Player* pPlayer = pAccInfo->GetPlayer();
			MAPLOG(GUI_LEVEL, "������ߴ���״̬��ƥ��CharID=%u,UserState=%d", pPlayer->GetCharID(), pAccInfo->GetUserState());
		}
	}
}

DWORD AccountInfoManager::GetDBThreadUserNum(BYTE byIndex)
{
	if (byIndex >= MAX_DB_CONN)
		return 0;

	return m_dwMapDBUser[byIndex];
}

int	AccountInfoManager::GetMapDBIndex()
{
	DWORD dwMinUserNum = MAXDWORD;
	int nBestIndex = 1;

	for (int i = 1;i<MAX_DB_CONN;++i)
	{
		if(m_dwMapDBUser[i]<dwMinUserNum)
		{
			dwMinUserNum = m_dwMapDBUser[i];
			nBestIndex = i;
		}
	}

	//���ӷ�����������
	++m_dwMapDBUser[nBestIndex];

	return nBestIndex;
}

void AccountInfoManager::ReleaseMapDBIndex(int nIndex)
{
	if(nIndex<0 || nIndex>=MAX_DB_CONN)
		return;

	if(m_dwMapDBUser[nIndex]==0)
	{
		MAPLOG(GUI_LEVEL,"ReleaseMapDBIndex���ó���");
		return;
	}

	--m_dwMapDBUser[nIndex];
}