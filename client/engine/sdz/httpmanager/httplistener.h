#ifndef HTTPLISTENER_H
#define HTTPLISTENER_H

#include "httpstruct.h"

class HttpListener
{
public:
	virtual void OnCallBack(unsigned int http_id, const RequestTask& request_task) = 0;
	virtual void Free()=0;
};

class FileHttpListener : public HttpListener
{
public:
	FileHttpListener(const std::string& file_name)
		: m_file_name(file_name)
	{

	}
protected:
	std::string m_file_name;
};

class UrlHttpListener : public HttpListener
{
public:
	UrlHttpListener(const std::string& url): m_url(url)
	{
	}
protected:
	std::string m_url;
};

#endif
