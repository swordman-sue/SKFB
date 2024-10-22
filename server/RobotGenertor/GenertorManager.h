#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/DBCFileManager.h"

//数据库描述信息
struct DatabaseDesc
{
	char*	pszDatabaseIP;
	char*	pszDatabaseName;
	char*	pszUserName;
	char*	pszUserPassword;
};

class  Genertor;
struct RobotEntry;

class GenertorManager : public Singleton<GenertorManager>
{
public:
	GenertorManager();
	~GenertorManager();

	BOOL			Init(DatabaseDesc& desc, char* pRobotFile, DWORD dwDBThreadNum, DWORD dwZoneID);
	VOID			Release();
	VOID			Update();
	VOID			ProcessResult(RobotEntry* pRobotEntry);

private:
	Genertor*		m_pGenertorThread;
	DWORD			m_dwGenertorThreadNum;
	INT				m_nProcessNum;
	INT				m_nCurProcessNum;
	DBCFile			m_cDBCFile;
};