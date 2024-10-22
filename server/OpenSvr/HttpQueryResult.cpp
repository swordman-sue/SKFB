#include "stdafx.h"
#include "HttpQueryResult.h"

_IMPL_QUERY_POOL(HttpCheckUser)
_IMPL_QUERY_POOL(HttpAuthenticationCode)
_IMPL_QUERY_POOL(HttpRegUserData)
_IMPL_QUERY_POOL(HttpWebCommonData)
_IMPL_QUERY_POOL(HttpReyunStatisticNotify)
_IMPL_QUERY_POOL(HttpRequestOrder);
_IMPL_QUERY_POOL(HttpRechargeSuccessReport);
_IMPL_QUERY_POOL(HttpStatisticsReport);


HttpQueryResult::HttpQueryResult(void)
{
}

HttpQueryResult::~HttpQueryResult(void)
{
}

BOOL HttpQueryResult::Init()
{
	memset(m_FunName,0,sizeof(m_FunName));
	memset(m_Secret,0,sizeof(m_Secret));
	memset(m_HttpVerb,0,sizeof(m_HttpVerb));
	memset(m_szUrl, 0, sizeof(m_szUrl));

	m_strResult.clear();
	m_strContext.clear();
	
	m_byOpeNum = 1;

	m_ParamMap.clear();
	m_dwRequestTime = GetTickCount();

	m_RequestType = 0;

	return TRUE;
}
bool HttpQueryResult::IsAginOpeError(int nError)
{
	if(    -57 == nError 
		|| -58 == nError
		|| -60 == nError
		|| -61 == nError )
	{
		return true;
	}
	return false;

}

VOID HttpQueryResult::Release()
{
	memset(m_FunName,0,sizeof(m_FunName));
	memset(m_Secret,0,sizeof(m_Secret));
	memset(m_HttpVerb,0,sizeof(m_HttpVerb));

	m_ParamMap.clear();
}

void HttpQueryResult::SetFunName(const char* szFunName)
{
	if (szFunName)
	{
		strncpy(m_FunName,szFunName,sizeof(m_FunName));
		m_FunName[MAX_FUN_NAME_LEN-1] = '\0';
	}
}

void HttpQueryResult::SetSecret(const char* szSecret)
{
	if (szSecret)
	{
		sprintf(m_Secret,"%s%s",szSecret,"&");
	}
}

void HttpQueryResult::SetHttpVerb(const char* szHttpVerb)
{
	if (szHttpVerb)
	{
		strncpy(m_HttpVerb,szHttpVerb,sizeof(m_HttpVerb));
		m_HttpVerb[MAX_HTTP_VERB-1] = '\0';
	}
}

void HttpQueryResult::SetHttpUrl(const char* szHttpUrl)
{
	if (szHttpUrl)
	{
		strncpy(m_szUrl,szHttpUrl,sizeof(m_szUrl));
		m_szUrl[MAX_HTTP_URL-1] = '\0';
	}
}

void HttpQueryResult::AddParam(const char* szKey, const char* szValue)
{
	if (szKey && szValue)
	{
		m_ParamMap.insert(std::make_pair(szKey, szValue));
	}
}

// char* HttpQueryResult::BuildHttpRequest(char* buffer)
// {
// 	//sprintf_s(szHttpBuff, 1024,"http://m.4399api.com/openapi/oauth-check.html?state=%s&uid=%s", pszToken, pszUID);
// 
// 	strcpy(buffer,"OpenReq.php");
// 
// 	strcat(buffer,"?");
// 
// 	PARAM_MAP_ITER it;
// 
// 	int nsize = (INT)m_ParamMap.size();
// 	int i = 0;
// 
// 	for (it = m_ParamMap.begin();it!=m_ParamMap.end();++it)
// 	{
// 		const std::string& strkey = it->first;
// 		std::string& strval = it->second;
// 
// 		strcat(buffer,strkey.c_str());
// 		strcat(buffer,"=");
// 		strcat(buffer,strval.c_str());
// 
// 		++i;
// 
// 		if (i < nsize)
// 		{
// 			strcat(buffer,"&");
// 		}
// 
// 	}
// 
// 	return buffer;
// }

char* HttpQueryResult::BuildHttpRequest(char* buffer)
{
	strcpy(buffer,m_szUrl);

	if (m_ParamMap.size())
	{
		strcat(buffer,"?");

		PARAM_MAP_ITER it;

		int nsize = (INT)m_ParamMap.size();
		int i = 0;

		for (it = m_ParamMap.begin();it!=m_ParamMap.end();++it)
		{
			const std::string& strkey = it->first;
			std::string& strval = it->second;

			strcat(buffer,strkey.c_str());
			strcat(buffer,"=");
			strcat(buffer,strval.c_str());

			++i;

			if (i < nsize)
			{
				strcat(buffer,"&");
			}
		}
	}

	return buffer;
}

wchar_t* HttpQueryResult::ChangeToWildChar(char* buffer,wchar_t* wbuffer,int cchWideChar)
{
	MultiByteToWideChar(CP_ACP,0, (LPCSTR)buffer, -1, wbuffer, cchWideChar);

	return wbuffer;
}

void HttpQueryResult::SetHttpResult(const char* szResult)
{
	m_strResult = szResult;
	int nIdx = (int)m_strResult.find('{'); 
	if(nIdx>=0)
	{
		//m_strResult = m_strResult.substr(nIdx,m_strResult.length()-1);
		m_strResult = m_strResult.substr(nIdx,m_strResult.length());
	}
}

const char*	HttpQueryResult::GetHttpResult()
{
	return m_strResult.c_str();
}
