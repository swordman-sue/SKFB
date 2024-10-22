/**\file
Http�����߼�����
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
	// ȡ״̬��
	int GetStateCode() { return m_nReturnCode; };

	// ȡ״̬����
	string GetStateMsg() { return m_strReturnMsg; };

	// ȡ��Ӧ����
	string GetResponseData() { return m_strResponseData; };

private:
	string m_strResponseData;	// ��Ӧ����
	string m_strResponseText;	// ��Ӧ����

	int m_nReturnCode;			// ��Ӧ��
	string m_strReturnMsg;		// ��Ӧ������

	vector<string> m_vStateLineParamsList;	// ��Ӧ״̬�в���
};