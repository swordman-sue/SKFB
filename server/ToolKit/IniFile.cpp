#include "StdAfx.h"
#include "IniFile.h"

IniFile::IniFile(void)
{
}

IniFile::~IniFile(void)
{
}

void IniFile::Init(char* szFileName)
{
	GetCurrentDirectory(sizeof(m_szFileName)/sizeof(char),m_szFileName);
	strcat(m_szFileName,"\\");
	strcat(m_szFileName,szFileName);
}

int	 IniFile::GetIntVal(char* szSession,char* szKey,int nDefault)
{
	return GetPrivateProfileInt(szSession,szKey,nDefault,m_szFileName);
}
int	IniFile::GetStrVal(char* szSession,char* szKey,char* szBuffer,DWORD dwSize,char* szDefault)
{
	return GetPrivateProfileString(szSession,szKey,szDefault,szBuffer,dwSize,m_szFileName);
}
