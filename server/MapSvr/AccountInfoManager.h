////////////////////////////////////////////////////////////////////////////////
//	Filename     :AccountInfoManager.h
//	Created by   :Kevin
//	Created date :2014/12/17
//	Description  :
//					
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
#include <hash_map>
#include "../Common/Const.h"
#include "../Common/DataDefine.h"
#include "../toolkit/Singleton.h"

class AccountInfo;

typedef std::map<AccKey,AccountInfo*>					AccIDMap;
typedef std::map<AccKey,AccountInfo*>::iterator			AccIDMapIter;

//typedef std::hash_map<DWORD, AccountInfo*>				GuidMap;
//typedef std::hash_map<DWORD, AccountInfo*>::iterator	GuidMapIter;

typedef std::map<DWORD, AccountInfo*>					GuidMap;
typedef std::map<DWORD, AccountInfo*>::iterator			GuidMapIter;

class AccountInfoManager : public Singleton<AccountInfoManager>
{
public:
	AccountInfoManager(void);
	~AccountInfoManager(void);
public:
	AccountInfo*		GetAccountInfoByAccID(DWORD dwPlatform, const char* pszAccID);

	//��GUIDΪKEY�������
	AccountInfo*		GetAccountInfoByGUID(DWORD dwGUID);

	VOID				AddAccountInfo(AccountInfo* pAccountInfo);

	VOID				RemoveAccountInfo(AccountInfo* pAccountInfo);
	VOID				RemoveAccountInfoByGUID(DWORD dwGUID);
	VOID				Release();
	
	VOID				KickoutAccount(AccountInfo* pAccInfo);

	VOID				KickoutAllAccount();

	VOID				OnGateSvrDisconnect();

	VOID				ProcessDBLogoutAck(AccountInfo* pAccInfo);
	DWORD				GetDBThreadUserNum(BYTE byIndex);

//private:
	int					GetMapDBIndex();
	void				ReleaseMapDBIndex(int nIndex);

private:
	AccIDMap			m_AccountInfoByAccIDMap;			//��AccIDΪKEY��Map
	GuidMap				m_AccountInfoByGUIDMap;				//��GUDIΪKEY��Map
	DWORD				m_dwMapDBUser[MAX_DB_CONN];			//ÿһ��DBThread������������
};

