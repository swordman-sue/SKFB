/**\file
Http�������߼���
\author Kevin
\date 2016/8/30
\note
*/

#pragma once

#include <map>
#include <string>
#include "QueryPoolDefine.h"
#include "../Common/Const.h"
#include "../Common/DataDefine.h"

typedef std::map<std::string,std::string>						PARAM_MAP;
typedef std::map<std::string,std::string>::iterator				PARAM_MAP_ITER;

#define MAX_FUN_NAME_LEN		64
#define MAX_APP_KEY				64
#define MAX_HTTP_VERB			16
#define MAX_HTTP_URL			MAX_HTTP_URL_LEN

enum
{
	HTTP_REQUEST_GET = 1, 
	HTTP_REQUEST_POST = 2,
};

class HttpQueryResult
{
public:
	HttpQueryResult(void);
	virtual ~HttpQueryResult(void);

	virtual BOOL		Init();
	virtual	VOID		Release();

	inline DWORD		GetIndex() { return m_dwIndex;	}
	inline VOID			SetIndex( DWORD dwIdx ) { m_dwIndex = dwIdx; }

	inline unsigned int	GetQueryResult() { return m_QueryResult; }
	inline VOID			SetQueryResult(unsigned int uires){ m_QueryResult = uires; }

	inline DWORD		GetKey() { return m_dwKey; }
	inline VOID			SetKey(DWORD dwKey) { m_dwKey = dwKey; }

	void				SetFunName(const char* szFunName);
	void				SetSecret(const char* szSecret);
	void				SetHttpVerb(const char* szHttpVerb);
	void				SetHttpUrl(const char* szHttpVerb);

	void				AddParam(const char* szKey,const char* szValue);
	char*				BuildHttpRequest(char* buffer);
	wchar_t*			ChangeToWildChar(char* buffer,wchar_t* wbuffer,int cchWideChar);
	void				SetHttpResult(const char* szResult);
	const char*			GetHttpResult();

	inline BYTE		    GetOpeNum() { return m_byOpeNum;	}
	inline bool         IsCanContinue(){ return m_byOpeNum<10;}
	inline VOID		    AddOpeNum() { ++m_byOpeNum;	}
	bool                IsAginOpeError(int nError);
	DWORD				GetRequestTime() { return m_dwRequestTime; };

	// �Ż����¼ӽӿ�
public:
	void				SetHost(const char *pszHost) { m_strHost = pszHost; };
	const char*			GetHost() { return m_strHost.c_str(); };
	void				SetPath(const char *pszPath) { m_strPath = pszPath; };
	const char*			GetPath() { return m_strPath.c_str(); };
	void				SetContext(const char *pszContext) { m_strContext = pszContext; };
	const char*			GetContext() { return m_strContext.c_str(); };
	void				SetPort(int nPort) { m_nPort = nPort; }
	int					GetPort() { return m_nPort; }
	void				SetRequestType(int nRequestType) { m_RequestType = nRequestType; };
	int					GetRequestType() { return m_RequestType; };

private:
	DWORD				m_dwIndex;
	unsigned int		m_QueryResult;
	DWORD				m_dwKey;
	char				m_FunName[MAX_FUN_NAME_LEN];	//������
	char				m_Secret[MAX_APP_KEY];			//����ǩ����ȫ��
	char				m_HttpVerb[MAX_HTTP_VERB];		//http���÷�ʽ
	PARAM_MAP			m_ParamMap;						//�����б�
	std::string			m_strResult;					//�������
	BYTE                m_byOpeNum ;                    //��������æ֮����������ȷ�������
	char				m_szUrl[MAX_HTTP_URL];
	DWORD				m_dwRequestTime;				//��ʼ����ʱ��(����ʹ��)

// �Ż�����Ҫ���ֶ�
	string				m_strHost;		// ����
	int					m_nPort;		// �˿�
	string				m_strPath;		// ·��
	string				m_strContext;	// ������
	int					m_RequestType;	// ��������(0=other,1=get,2=post)
};

// ��֤�û�
class HttpCheckUser : public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpCheckUser, HttpQueryResult)
public:

	virtual BOOL Init()
	{
		//���û���汾
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_byTerminalType = 0;
		m_dwCPChannelType = 0;
		m_dwSDKChannelType = 0;
		memset(m_szAccount, 0, sizeof(m_szAccount));
		memset(m_szToken, 0, sizeof(m_szToken));
		memset(m_szIP, 0, sizeof(m_szIP));
		memset(m_szDeviceId, 0, sizeof(m_szDeviceId));
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	void	SetCPChannelType(DWORD dwChannelType) { m_dwCPChannelType = dwChannelType; };
	DWORD	GetCPChannelType() { return m_dwCPChannelType; };

	void	SetTerminalType(BYTE byTerminalType) { m_byTerminalType = byTerminalType; };
	BYTE	GetTerminalype() { return m_byTerminalType; };

	void	SetSDKChannelType(DWORD dwChannelType) { m_dwSDKChannelType = dwChannelType; };
	DWORD	GetSDKChannelType() { return m_dwSDKChannelType; };

	const char*	GetAccount() { return m_szAccount; }
	void SetAccount(const char* pszAccount)
	{
		strcpy_s(m_szAccount, sizeof(m_szAccount), pszAccount);
		m_szAccount[MAX_ACCOUNT_LEN - 1] = '\0';
	}

	const char*	GetToken() { return m_szToken; }
	void SetToken(const char* pszToken)
	{
		strcpy_s(m_szToken, sizeof(m_szToken), pszToken);
		m_szToken[MAX_TOKEN_LEN - 1] = '\0';
	}

	const char*	GetIP() { return m_szIP; }
	void SetIP(const char* pszIP)
	{
		strcpy_s(m_szIP, sizeof(m_szIP), pszIP);
		m_szIP[IP_LEN - 1] = '\0';
	}

	const char*	GetDeviceId() { return m_szDeviceId; }
	void SetDeviceId(const char* pszIP)
	{
		strcpy_s(m_szDeviceId, sizeof(m_szDeviceId), pszIP);
		m_szDeviceId[MAX_DEVICE_ID_LEN - 1] = '\0';
	}
private:
	DWORD	m_dwGUID;
	WORD	m_wLoginType;		// ��¼����
	BYTE	m_byTerminalType;	// �ն�����(1=android, 2=ios)
	DWORD	m_dwCPChannelType;	// CP��������
	DWORD	m_dwSDKChannelType;	// skd	��������
	char	m_szAccount[MAX_ACCOUNT_LEN];	// �ʺ�
	char	m_szToken[MAX_TOKEN_LEN];		// token

	char	m_szIP[IP_LEN];						// IP
	char	m_szDeviceId[MAX_DEVICE_ID_LEN];	// �豸ID deviceid
};


// ���󶩵�
class HttpRequestOrder : public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpRequestOrder, HttpQueryResult)
public:

	virtual BOOL Init()
	{
		//���û���汾
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_dwChannelType = 0;
		memset(m_szAccount, 0, sizeof(m_szAccount));
		memset(m_szToken, 0, sizeof(m_szToken));
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	void	SetChannelType(DWORD dwChannelType) { m_dwChannelType = dwChannelType; };
	DWORD	GetChannelType() { return m_dwChannelType; };

	const char*	GetAccount() { return m_szAccount; }
	void SetAccount(const char* pszAccount)
	{
		strcpy_s(m_szAccount, sizeof(m_szAccount), pszAccount);
		m_szAccount[MAX_ACCOUNT_LEN - 1] = '\0';
	}

	const char*	GetToken() { return m_szToken; }
	void SetToken(const char* pszToken)
	{
		strcpy_s(m_szToken, sizeof(m_szToken), pszToken);
		m_szToken[MAX_TOKEN_LEN - 1] = '\0';
	}
private:
	DWORD	m_dwGUID;
	DWORD	m_dwChannelType;	// ��������
	char	m_szAccount[MAX_ACCOUNT_LEN];	// �ʺ�
	char	m_szToken[MAX_TOKEN_LEN];		// token
};


// ��֤������ HttpAuthQuery
class HttpAuthenticationCode : public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpAuthenticationCode, HttpQueryResult)
public:

	virtual BOOL Init()
	{
		//���û���汾
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_byLoginType = 0;
		m_wCode = 0;
		memset(m_szMobile,0,sizeof(m_szMobile));
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	void	SetLoginType(BYTE byType) {  m_byLoginType = byType; };
	BYTE	GetLoginType() { return m_byLoginType; };

	void	SetCode(WORD wCode) { m_wCode = wCode; };
	WORD	GetCode() { return m_wCode; };

	const char*	GetMobile() { return m_szMobile; }
	void	SetMobile(const char* szMobile) 
	{ 
		if (szMobile)
		{
			strncpy(m_szMobile, szMobile, sizeof(m_szMobile));
			m_szMobile[MAX_MOBILE_LEN] = '\0';
		}
	}

	const char*	GetPassword() { return m_szPassword; }
	void	SetPassword(const char* pszPassword) 
	{ 
		if (pszPassword)
		{
			strncpy(m_szPassword, pszPassword, sizeof(m_szPassword));
			m_szPassword[MAX_ACCOUNT_PASSWORD_LEN] = '\0';
		}
	}
private:
	DWORD	m_dwGUID;
	char	m_szMobile[MAX_MOBILE_LEN+1];
	char	m_szPassword[MAX_ACCOUNT_PASSWORD_LEN+1];
	BYTE	m_byLoginType;	// ��¼����
	WORD	m_wCode;		// ��֤��
};

// ����ͳ��֪ͨ
class HttpReyunStatisticNotify :public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpReyunStatisticNotify, HttpQueryResult)

	virtual BOOL INIT()
	{
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_dwMSGID = 0;
		memset(m_szAcctID, 0, sizeof(m_szAcctID));
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	DWORD	GetMSGID() { return m_dwMSGID; }
	void	SetMSGID(DWORD msgid) { m_dwMSGID = msgid; }

	const char*	GetAcctID() { return m_szAcctID; }
	void	SetAcctID(const char* pszAcctID)
	{
		if (pszAcctID)
		{
			strncpy(m_szAcctID, pszAcctID, sizeof(m_szAcctID));
			m_szAcctID[MAX_ACCOUNT_LEN] = '\0';
		}
	}

private:
	DWORD	m_dwGUID;
	DWORD   m_dwMSGID;
	char	m_szAcctID[MAX_ACCOUNT_LEN + 1];
};

class HttpRegUserData :public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpRegUserData,HttpQueryResult)

	virtual BOOL INIT()
	{
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_dwMSGID = 0;
		memset(m_szAcctID, 0, sizeof(m_szAcctID));
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	DWORD	GetMSGID() { return m_dwMSGID; }
	void	SetMSGID(DWORD msgid) { m_dwMSGID = msgid; }

	const char*	GetAcctID() { return m_szAcctID; }
	void	SetAcctID(const char* pszAcctID)
	{
		if (pszAcctID)
		{
			strncpy(m_szAcctID, pszAcctID, sizeof(m_szAcctID));
			m_szAcctID[MAX_ACCOUNT_LEN] = '\0';
		}
	}

private:
	DWORD	m_dwGUID;
	DWORD   m_dwMSGID;
	char	m_szAcctID[MAX_ACCOUNT_LEN + 1];
};


class HttpWebCommonData :public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpWebCommonData,HttpQueryResult)

	virtual BOOL INIT()
	{
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_dwMSGID = 0;
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	DWORD	GetMSGID() { return m_dwMSGID; }
	void	SetMSGID(DWORD msgid) { m_dwMSGID = msgid; }

private:
	DWORD	m_dwGUID;
	DWORD   m_dwMSGID;
};

// ��ֵ�ɹ��ϱ�
class HttpRechargeSuccessReport :public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpRechargeSuccessReport, HttpQueryResult)

	virtual BOOL INIT()
	{
		HttpQueryResult::Init();

		m_dwGUID = 0;
		m_dwMSGID = 0;
		return TRUE;
	}

	DWORD	GetGUID() { return m_dwGUID; }
	void	SetGUID(DWORD guid) { m_dwGUID = guid; }

	DWORD	GetMSGID() { return m_dwMSGID; }
	void	SetMSGID(DWORD msgid) { m_dwMSGID = msgid; }

private:
	DWORD	m_dwGUID;
	DWORD   m_dwMSGID;
};

// ����ͳ���ϱ�
class HttpStatisticsReport :public HttpQueryResult
{
	_DECLARE_QUERY_POOL(HttpStatisticsReport, HttpQueryResult)

	virtual BOOL INIT()
	{
		HttpQueryResult::Init();
		return TRUE;
	}
};