#include "StdAfx.h"
#include "AccountInfo.h"

AccountInfo::AccountInfo(void)
:	m_UserState(USER_INIT_STATE),
	m_bySelectedCharIndex(-1),
	m_byCharNum(0),
	m_pCurPlayer(NULL),
	m_nDBIndex(0),
	m_dwZoneID(0),
	m_dwCurCharID(0),
	m_bRobot(false)
{
	memset(m_szAccID, 0, sizeof(m_szAccID));
	memset(m_szPassword, 0, sizeof(m_szPassword));
}

AccountInfo::~AccountInfo(void)
{
}

VOID AccountInfo::Init()
{
	m_dwZoneID = 0;
	memset(m_szAccID, 0, sizeof(m_szAccID));
	memset(m_szPassword, 0, sizeof(m_szPassword));
	m_dwGuid = 0;
	SetUserState(USER_INIT_STATE);
	m_bySelectedCharIndex = -1;
	m_byCharNum = 0;
	memset(m_bOccupiedSlot,0,sizeof(m_bOccupiedSlot));
	memset(m_listInfo,0,sizeof(m_listInfo));
	m_pCurPlayer = NULL;
	m_nDBIndex = 0;

	memset(m_szToken, 0, sizeof(m_szToken));

	memset(m_szMobile, 0, sizeof(m_szMobile));

// 	memset(m_OpenKey,0,sizeof(m_OpenKey));
// 	memset(m_PF,0,sizeof(m_PF));
// 	memset(m_UserIP,0,sizeof(m_UserIP));
// 	memset(m_PFKey,0,sizeof(m_PFKey));
	m_bRobot = false;
}

VOID AccountInfo::Create(DWORD dwZoneID, const char* pszAccID, const char* pszPassword, DWORD dwGuid, 
	DWORD dwChannelUID, BYTE byTerminalType, const char *pszMobile)
{
	m_dwZoneID = dwZoneID;
	m_dwChannelUID = dwChannelUID;
	m_dwGuid = dwGuid;
	m_byTerminalType = byTerminalType;

	if (pszAccID)
		strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)-1);

	if (pszPassword)
		strncpy(m_szPassword, pszPassword, sizeof(m_szPassword)-1);

	if (pszMobile)
		strncpy(m_szMobile, pszMobile, sizeof(m_szMobile)-1);
}

CharListInfo* AccountInfo::GetPlayerListInfoByIndex(BYTE byIndex)
{
	 if(byIndex < 0 || byIndex >= MAX_CHARACTER_NUM)
		 return NULL;

	 if(m_bOccupiedSlot[byIndex] == FALSE)
		 return NULL;

	 return &m_listInfo[byIndex];
}

CharListInfo* AccountInfo::GetPlayerListInfoByCharID(DWORD dwCharID)
{
	for (BYTE i = 0; i < MAX_CHARACTER_NUM; ++i)
	{
		if(m_listInfo[i].m_dwCharID == dwCharID)
			return &m_listInfo[i];
	}

	return NULL;
}

VOID AccountInfo::SetPlayerListInfo(const CharListInfo& charinfo,BYTE byIndex)
{
	if(byIndex < 0 || byIndex >= MAX_CHARACTER_NUM)
		return;

	memcpy(&m_listInfo[byIndex], &charinfo, sizeof(m_listInfo[byIndex]));
}

VOID AccountInfo::SetSelectedCharIndex(BYTE bySelectedCharIndex) 
{ 
	(bySelectedCharIndex < 0 || bySelectedCharIndex >= MAX_CHARACTER_NUM) ? m_bySelectedCharIndex = -1 : m_bySelectedCharIndex = bySelectedCharIndex; 
}


DWORD AccountInfo::GetSelectedCharID()
{
	CharListInfo* pInfo = GetPlayerListInfoByIndex(m_bySelectedCharIndex);
	if(pInfo)
		return pInfo->m_dwCharID;
	else
		return 0;
}

const char*	AccountInfo::GetSelectedCharName()
{
	CharListInfo* pInfo = GetPlayerListInfoByIndex(m_bySelectedCharIndex);
	if(pInfo)
		return (const char*)pInfo->m_szCharName;
	else
		return NULL;
}

VOID AccountInfo::SetOccupiedSolt(DWORD dwSoltIndex,BOOL val) 
{ 
	if(dwSoltIndex < 0 || dwSoltIndex >= MAX_CHARACTER_NUM) 
		return; 
	m_bOccupiedSlot[dwSoltIndex] = val;
}

BOOL AccountInfo::FindEmptySlot(BYTE& dwSoltIndex) const
{
	for (int i = 0; i < MAX_CHARACTER_NUM; ++i)
	{
		if (m_bOccupiedSlot[i] == FALSE)
		{
			dwSoltIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

// void AccountInfo::SetOpenKey(const char* szOpenKey) 
// { 
// 	if (szOpenKey)
// 	{
// 		strncpy(m_OpenKey,szOpenKey,sizeof(m_OpenKey));
// 		m_OpenKey[MAX_OPENKEY_LEN] = '\0';
// 	}
// }
// 
// void AccountInfo::SetPF(const char* szPF) 
// { 
// 	if (szPF)
// 	{
// 		strncpy(m_PF,szPF,sizeof(m_PF));
// 		m_PF[MAX_PF_LEN] = '\0';
// 	}
// }
// 
// void AccountInfo::SetUserIP(const char* szUserIP) 
// { 
// 	if (szUserIP)
// 	{
// 		strncpy(m_UserIP,szUserIP,sizeof(m_UserIP));
// 		m_UserIP[MAX_USER_IP_LEN] = '\0';
// 	}
// }
// 
// void AccountInfo::SetPFKey(const char* szPFKey)
// {
// 	if (szPFKey)
// 	{
// 		strncpy(m_PFKey,szPFKey,sizeof(m_PFKey));
// 		m_PFKey[MAX_PFKEY_LEN] = '\0';
// 	}
// }