// RobotGenertor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GenertorManager.h"
#include "../ToolKit/IniFile.h"

#ifdef _DEBUG		
#	if defined(_M_IX86)	
//#		pragma comment(lib,"stlportd.5.2.lib")
#		pragma comment(lib,"../bin/ToolKit_d.lib")
#		pragma comment(lib,"../bin/Common_d.lib")
#		pragma comment(lib,"../bin/MySqlDB_d.lib")
#	elif defined(_M_X64)	
//#		pragma comment(lib,"stlport_x64d.5.2.lib")
#		pragma comment(lib,"../bin/ToolKit64_d.lib")
#		pragma comment(lib,"../bin/Common64_d.lib")
#		pragma comment(lib,"../bin/MySqlDB64_d.lib")
#		pragma comment(lib,"../bin/libprotobuf64_d.lib") 
#	else	
#		error "Can not supported on this architecture."
#	endif	
#else		
#	if defined(_M_IX86)	
#		pragma comment(lib,"../bin/ToolKit.lib")
#		pragma comment(lib,"../bin/Common.lib")
#		pragma comment(lib,"../bin/MySqlDB.lib")
#	elif defined(_M_X64)	
#		pragma comment(lib,"../bin/ToolKit64.lib")
#		pragma comment(lib,"../bin/Common64.lib")
#		pragma comment(lib,"../bin/MySqlDB64.lib")
#		pragma comment(lib,"../bin/libprotobuf64.lib") 
#	else	
#		error "Can not supported on this architecture."
#	endif	
#endif		
//#		pragma comment(lib,"stlport.5.2.lib")
//#		pragma comment(lib,"stlport_x64.5.2.lib")
int _tmain(int argc, _TCHAR* argv[])
{
	IniFile inifile;
	inifile.Init("Robot.ini");

	char DBIP[64];
	char DBName[64];
	char UserName[64];
	char Password[64];
	char FilePath[MAX_PATH];

	//��ò����ļ���
	printf("�������ļ���:");
	memset(FilePath, 0, sizeof(FilePath));
	scanf_s("%s", FilePath, sizeof(FilePath) - 1);

	//��ȡINI�����ļ�
	inifile.GetStrVal("DB", "DBIP", DBIP, sizeof(DBIP));
	inifile.GetStrVal("DB", "DBNAME", DBName, sizeof(DBName));
	inifile.GetStrVal("DB", "USERNAME", UserName, sizeof(UserName));
	inifile.GetStrVal("DB", "PASSWORD", Password, sizeof(Password));
	DWORD dwZoneID = (DWORD)inifile.GetIntVal("Robot", "ZoneID");

	DatabaseDesc dbdesc;
	dbdesc.pszDatabaseIP = DBIP;
	dbdesc.pszDatabaseName = DBName;
	dbdesc.pszUserName = UserName;
	dbdesc.pszUserPassword = Password;

	// �õ�����������
	//SYSTEM_INFO si;
	//GetSystemInfo(&si);
	//DWORD dwProcessNum = si.dwNumberOfProcessors;
	DWORD dwProcessNum = 10;

	//��ʼ��������
	if (!GenertorManager::Instance()->Init(dbdesc, FilePath, dwProcessNum, dwZoneID))
	{
		printf("��ʼ��������ʧ��.\n");
		return 0;
	}

	printf("����ʼ.\n");
	GenertorManager::Instance()->Update();
	GenertorManager::Instance()->Release();
	
	printf("\n");
	printf("�������,�밴������˳�.\n");

	char c = getchar();

	return 0;
}

