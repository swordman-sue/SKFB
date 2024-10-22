#include "stdafx.h"
#include "HttpRequest.h"
#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include "..\Common\CommonLogic.h"

#define MAXSIZE 1024
#pragma comment(lib, "Wininet.lib")

CHttpRequest::CHttpRequest()
{
#ifdef WIN32
	//此处一定要初始化一下，否则gethostbyname返回一直为空
	WSADATA wsa = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

CHttpRequest::~CHttpRequest()
{

}

int CHttpRequest::SendHttpRequest(const _TCHAR* url,char *szOutBuff, DWORD &dwBuffCount, DWORD dwMaxCount)
{
	int nRetValue = 1;

	dwBuffCount = 0;

	DWORD dwTempCount;
	HINTERNET hSession = InternetOpen(_T("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (hHttp != NULL)
		{
			while (true)
			{
				dwTempCount = 0;
				InternetReadFile(hHttp, szOutBuff+dwBuffCount, dwMaxCount - 1, &dwTempCount);
				dwBuffCount += dwTempCount;
				if (!dwTempCount)
				{
					break;
				}
			}

			szOutBuff[dwBuffCount] = '\0';
			InternetCloseHandle(hHttp);
			hHttp = NULL;
		}
		else
		{
			nRetValue = 0;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;

		if (0 == dwBuffCount)
			nRetValue = 0;
	}
	else
	{
		// todo::暂时也按目标无效处理
		nRetValue = 0;
	}

	return nRetValue;
}

bool CHttpRequest::SendRequest_Post(std::string host, int nPort, std::string path, std::string content)
{
	//POST请求方式
	// 	std::stringstream stream;
	// 	stream << "POST " << path;
	// 	stream << " HTTP/1.0\r\n";
	// 	stream << "Host: " << host << "\r\n";
	// 	stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	// 	stream << "Content-Type:application/x-www-form-urlencoded\r\n";
	// 	stream << "Content-Length:" << post_content.length() << "\r\n";
	// 	stream << "Connection:close\r\n\r\n";
	// 	stream << post_content.c_str();

	// 填充请求参数
	char szLength[32] = { 0 };
	sprintf_s(szLength, 32, "%d", content.length());
	std::string strSendBuff;
	strSendBuff += "POST ";
	strSendBuff += path;
	strSendBuff += " HTTP/1.0\r\n";
	strSendBuff += "Host: ";
	strSendBuff += host;
	strSendBuff += "\r\n";
	strSendBuff += "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	strSendBuff += "Content-Type:application/x-www-form-urlencoded\r\n";
	strSendBuff += "Content-Length:";
	strSendBuff += szLength;
	strSendBuff += "\r\n";
	strSendBuff += "Connection:close\r\n\r\n";
	strSendBuff += content.c_str();

	char szBuff[1024] = { 0 };
	g_GB2312ToUTF8(strSendBuff.c_str(), szBuff);

	// 请求
	//return SendRequest_Http(host.c_str(), nPort, strSendBuff.c_str(), strSendBuff.size());
	return SendRequest_Http(host.c_str(), nPort, szBuff, strlen(szBuff));
}

bool CHttpRequest::SendRequest_Get(std::string host, int nPort, std::string path, std::string content)
{
	//GET请求方式
	// 	std::stringstream stream;
	// 	stream << "GET " << path << "?" << get_content;
	// 	stream << " HTTP/1.0\r\n";
	// 	stream << "Host: " << host << "\r\n";
	// 	stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	// 	stream << "Connection:close\r\n\r\n";

	// 填充请求参数
	std::string strSendBuff;
	strSendBuff += "GET ";
	strSendBuff += path;
	strSendBuff += "?";
	strSendBuff += content;
	strSendBuff += " HTTP/1.0\r\n";
	strSendBuff += "Host: ";
	strSendBuff += host;
	strSendBuff += "\r\n";
	strSendBuff += "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	strSendBuff += "Connection:close\r\n\r\n";

	// 请求
	return SendRequest_Http(host.c_str(), nPort, strSendBuff.c_str(), strSendBuff.size());
}

bool CHttpRequest::SendRequest_Http(const char* pHost, int nPort, const char *pRequest, int nRequestSize)
{
	UINT64 lRequestStartTime = g_GetCurrentTime();

	int sockfd;
	struct sockaddr_in address;
	struct hostent *pServer;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_port = htons(nPort);
	pServer = gethostbyname(pHost);

	if (!pServer)
	{
		OPENLOG(ERROR_LEVEL, "[%s]Http请求连接失败[host:%s,port:%d]", __FUNCTION__, pHost, nPort);
		return false;
	}

	memcpy((char *)&address.sin_addr.s_addr, (char*)pServer->h_addr, pServer->h_length);

	// 检测连接是否成功 todo::需要设置连接超市操作
	if (-1 == connect(sockfd, (struct sockaddr *)&address, sizeof(address)))
	{
		DWORD dwCostTime = g_GetCurrentTime() - lRequestStartTime;
		OPENLOG(ERROR_LEVEL, "[%s]Http请求连接失败[host:%s,port:%d,cost:%d]", __FUNCTION__, pHost, nPort, dwCostTime);
		return false;
	}

#ifdef WIN32
	send(sockfd, pRequest, nRequestSize, 0);
#else
	write(sockfd, pRequest, nRequestSize);
#endif
	char szTmp[1024];
	int nReturnSize;
	m_strResponseData.clear();

	// 接收响应数据(一次接收一小段)
	do 
	{
		memset(szTmp, 0, sizeof(szTmp));
#ifdef WIN32
		nReturnSize = recv(sockfd, szTmp, sizeof(szTmp)-1, 0);
#else
		nReturnSize = read(sockfd, szTmp, sizeof(szTmp)-1);
#endif
		if (nReturnSize > 0)
		{
			szTmp[nReturnSize] = 0;
			m_strResponseData += szTmp;
		}
	} while (nReturnSize > 0);

#ifdef WIN32
	closesocket(sockfd);
#else
	close(sockfd);
#endif

	// 检测响应数据是否为空
	if (!m_strResponseData.size())
	{
		OPENLOG(ERROR_LEVEL, "[%s]Http请求响应数据为空[host:%s,port:%d]", __FUNCTION__, pHost, nPort);
		return false;
	}

	// 计算状态行数据长度
	int nOffset = 0;
	while (m_strResponseData.at(nOffset) != 0x0D)
	{
		nOffset++;
		if (nOffset >= m_strResponseData.size())
			break;
	}

	char szStateLine[128] = { 0 };
	
	// 检测状态行长度是否合法
	if (nOffset >= sizeof(szStateLine))
	{
		OPENLOG(ERROR_LEVEL, "[%s]Http请求响应状态行数据过大[host:%s,port:%d,state_line_data_size:%d]", 
			__FUNCTION__, pHost, nPort, nOffset);
		return false;
	}

	// 取状态行数据
	strncpy_s(szStateLine, sizeof(szStateLine)-1, m_strResponseData.c_str(), nOffset);

	// 取状态行参数
	m_vStateLineParamsList.clear();
	int nStateLineParams = GetStringSpacingNum(szStateLine, ' ');
	for (int i = 0; i <= nStateLineParams; ++i)
	{
		string strParams = CPXValue(szStateLine, i, ' ');
		m_vStateLineParamsList.push_back(strParams);
	}

	// 检测状态参数数量是否合法 todo::不一定只有3个
	if (m_vStateLineParamsList.size() < 3)
	{
		OPENLOG(ERROR_LEVEL, "[%s]Http请求响应状态行参数出错[host:%s,port:%d,state_line_params_num:%d]",
			__FUNCTION__, pHost, nPort, m_vStateLineParamsList.size());
		return false;
	}

	m_nReturnCode = atoi(m_vStateLineParamsList[1].c_str());
	m_strReturnMsg = m_vStateLineParamsList[2];

	// 取响应正文
	int nIdx = (int)m_strResponseData.find('{');
	if (nIdx >= 0)
	{
		//m_strResult = m_strResult.substr(nIdx,m_strResult.length()-1);
		m_strResponseText = m_strResponseData.substr(nIdx, m_strResponseData.length());
	}

	UINT64 lRequestOverTime = g_GetCurrentTime();

	// 监控时间消耗
	if (lRequestOverTime > lRequestStartTime)
	{
		OPENLOG(ERROR_LEVEL, "[%s]Http请求消耗时间[host:%s,port:%d,cost_time:%d]",
			__FUNCTION__, pHost, nPort, lRequestOverTime - lRequestStartTime);
	}

	return true;
}
