#include "stdafx.h"
#include "WinHttpClient.h"
#include "HttpRequest.h"

WinHttpClient::WinHttpClient()
: m_requestHost(L""),
m_sessionHandle(NULL),
m_hConnectHandle(NULL),
m_responseCharset(L"")
{
}

WinHttpClient::~WinHttpClient(void)
{
	if (m_hConnectHandle != NULL)
	{
		WinHttpCloseHandle(m_sessionHandle);
	}

	if (m_sessionHandle != NULL)
	{
		WinHttpCloseHandle(m_sessionHandle);
	}
}

bool WinHttpClient::InitHttpConnect(const wstring &HostName, DWORD dwPort)
{
	m_requestHost = HostName;

	if (m_requestHost.size() <= 0)
	{
		return false;
	}

	if (m_sessionHandle == NULL)
	{
		m_sessionHandle = WinHttpOpen(SZ_AGENT,WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
		if (m_sessionHandle == NULL)
		{
			return false;
		}
	}

	if (WinHttpSetTimeouts(m_sessionHandle,0,3000,3000,3000)==FALSE)
	{
		return false;
	}

	m_hConnectHandle = WinHttpConnect(m_sessionHandle, m_requestHost.c_str(), (WORD)dwPort, 0);

	if (m_hConnectHandle)
	{
		return true;
	}
	else
	{
		return false;
	}
}

DWORD WinHttpClient::SendHttpRequest(const wchar_t* pszRequestURL,const wchar_t* httpVerb)
{
	if (!pszRequestURL || wcslen(pszRequestURL)==0)
	{
		return 1;
	}

	if (!httpVerb || wcslen(httpVerb)==0)
	{
		return 1;
	}

	HINTERNET hRequest = NULL;
	hRequest = WinHttpOpenRequest(m_hConnectHandle,httpVerb,pszRequestURL,NULL,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,0);

	if (!hRequest)
	{
		return 2;
	}

	if (WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,NULL)==FALSE)
	{
		goto SENDERROR;
	}

	if (WinHttpReceiveResponse(hRequest, NULL)==FALSE)
	{
		goto SENDERROR;
	}
	
	DWORD dwSize = 0;
	BOOL bResult = FALSE;
	
	
	wchar_t szHeader[1024];
	DWORD nszHeader = sizeof(szHeader);

	//获取所有头信息
	if (WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_RAW_HEADERS_CRLF,WINHTTP_HEADER_NAME_BY_INDEX,szHeader,&nszHeader,WINHTTP_NO_HEADER_INDEX)==FALSE)
	{
		goto SENDERROR;
	}
	


	//解析头信息
	if (ParseCharset(szHeader) == false)
	{
		goto SENDERROR;
	}

	memset(m_ResponseData, 0, sizeof(m_ResponseData));
	unsigned int responseByteCount = 0;

	unsigned char* pResponse = m_ResponseData;
	do
	{
		dwSize = 0;

		if (WinHttpQueryDataAvailable(hRequest, &dwSize)==FALSE)
		{
			goto SENDERROR;
		}
		
		if (dwSize)
		{
			DWORD dwRead = 0;
			if (WinHttpReadData(hRequest,pResponse,dwSize,&dwRead)==FALSE)
			{
				goto SENDERROR;
			}

			pResponse += dwRead;
			responseByteCount += dwRead;
		}
	
	}while (dwSize > 0);


	UINT codePage = CP_UTF8;
	DWORD dwFlag = 0;
	if (_wcsnicmp(m_responseCharset.c_str(), L"utf-8", 5) == 0)
	{
		codePage = CP_UTF8;
		dwFlag = 0;
	}
	else
	{
		codePage = CP_ACP;
		dwFlag = MB_PRECOMPOSED;
	}

	wchar_t szResponseData[INT_BUFFERSIZE];

	MultiByteToWideChar(CP_UTF8,0, (LPCSTR)m_ResponseData, responseByteCount + 1, szResponseData, INT_BUFFERSIZE);

	WideCharToMultiByte(CP_ACP, 0, szResponseData, -1 , (LPSTR)m_ResponseData, sizeof(m_ResponseData), 0, 0);

	WinHttpCloseHandle(hRequest);

	return 0;

SENDERROR:

	WinHttpCloseHandle(hRequest);

	return GetLastError();
}

DWORD WinHttpClient::SendHttpRequestEx(const TCHAR* pszRequestURL,const wchar_t* httpVerb)
{
	DWORD dwReadCount = 0;
	return CHttpRequest::SendHttpRequest((const TCHAR*)pszRequestURL, (char*)m_ResponseData, dwReadCount, INT_BUFFERSIZE);
}

bool WinHttpClient::ParseCharset(wchar_t* pHeader)
{
	if (!pHeader)
	{
		return false;
	}

	wchar_t* szcharset = L"charset=";
	size_t ncharset = wcslen(szcharset);

	wchar_t* pTempFind = wcsstr(pHeader,szcharset);
	if (!pTempFind)
	{
		return false;
	}

	pTempFind += ncharset;

	wchar_t* szrn = L"\r\n";
	size_t nszrn = wcslen(szrn);

	wchar_t* pEndFind = wcsstr(pTempFind,szrn);
	if (!pEndFind)
	{
		return false;
	}

	wchar_t charset[128];
	memset(charset,0,sizeof(charset));
	wcsncpy(charset,pTempFind,pEndFind - pTempFind);
	charset[pEndFind - pTempFind] = L'\0';
	m_responseCharset = charset;
	return true;
}

wstring WinHttpClient::GetResponseCharset(void)
{
	return m_responseCharset;
}

wstring WinHttpClient::GetRequestHost(void)
{
	return m_requestHost;
}

const char* WinHttpClient::GetResponseContent(void)
{
	return (const char*)m_ResponseData;
}