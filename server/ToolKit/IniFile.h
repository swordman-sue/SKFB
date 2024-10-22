#pragma once

class IniFile
{
public:
	IniFile(void);
	~IniFile(void);

public:
	void	Init(char* szFileName);
	int		GetIntVal(char* szSession,char* szKey,int nDefault = 0);
	int		GetStrVal(char* szSession,char* szKey,char* szBuffer,DWORD dwSize,char* szDefault=NULL);

private:
	char	m_szFileName[256];

};
