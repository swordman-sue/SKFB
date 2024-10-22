/**\file
Http请求逻辑类型
\author Kevin
\date 2016/8/30
\note
*/

#pragma once

#define HTTP_RESPONSE_DATA_SIZE 2048

class CHttpRequest
{
public:
	CHttpRequest();
	~CHttpRequest();

public:
	static int SendHttpRequest(const _TCHAR* url,char *szOutBuff, DWORD &dwBuffCount, DWORD dwMaxCount);

	bool SendRequest_Post(std::string host, int nPort, std::string path, std::string content);

	bool SendRequest_Get(std::string host, int nPort, std::string path, std::string content);

	bool SendRequest_Http(const char* pHost, int nPort, const char *pRequest, int nRequestSize);

public:
	// 取状态码
	int GetStateCode() { return m_nReturnCode; };

	// 取状态描述
	string GetStateMsg() { return m_strReturnMsg; };

	// 取响应数据
	string GetResponseData() { return m_strResponseData; };

private:
	string m_strResponseData;	// 响应数据
	string m_strResponseText;	// 响应正文

	int m_nReturnCode;			// 响应码
	string m_strReturnMsg;		// 响应码描述

	vector<string> m_vStateLineParamsList;	// 响应状态行参数
};