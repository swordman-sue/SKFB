/**\file
win下http请求逻辑处理类(目前暂时不使用)
\author Kevin
\date 2016/8/30
\note
*/

#pragma once

#pragma comment(lib, "Winhttp.lib")

#include <windows.h>
#include <Winhttp.h>
#include <string>
using namespace std;

static wchar_t *SZ_AGENT = L"HttpOpenClient";
static const int INT_BUFFERSIZE = 10240;

class WinHttpClient
{
public:
	WinHttpClient();
	~WinHttpClient(void);

	bool				InitHttpConnect(const wstring &HostName, DWORD dwPort);
	DWORD				SendHttpRequest(const wchar_t* pszRequestURL,const wchar_t* httpVerb = L"GET");
	DWORD				SendHttpRequestEx(const TCHAR* pszRequestURL,const wchar_t* httpVerb = L"GET");
	const char*			GetResponseContent(void);
	wstring				GetResponseCharset(void);
	wstring				GetRequestHost(void);

private:
	bool				ParseCharset(wchar_t* pHeader);

private:
	HINTERNET			m_sessionHandle;
	HINTERNET			m_hConnectHandle;

	wstring				m_requestHost;
	wstring				m_responseCharset;
	unsigned char		m_ResponseData[INT_BUFFERSIZE];
	//unsigned int16		m_nResponseDataSize;
};
