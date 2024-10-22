//

#include "stdafx.h"
#include "RobotFactory.h"
#include "RobotManager.h"
#include "PacketHandler.h"
/*#include "BattleFactory.h"*/

#ifdef _DEBUG		
#	if defined(_M_IX86)	
//#		pragma comment(lib,"stlportd.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine_d.lib")
#		pragma comment(lib,"../bin/ToolKit_d.lib")
#		pragma comment(lib,"../bin/Common_d.lib")
#	elif defined(_M_X64)
//#		pragma comment(lib,"stlport_x64d.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64_d.lib")
#		pragma comment(lib,"../bin/ToolKit64_d.lib")
#		pragma comment(lib,"../bin/Common64_d.lib")
#	else	
#		error "Can not supported on this architecture."
#	endif	
#else		
#	if defined(_M_IX86)
//#		pragma comment(lib,"stlport.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine.lib")
#		pragma comment(lib,"../bin/ToolKit.lib")
#		pragma comment(lib,"../bin/Common.lib")
#	elif defined(_M_X64)
//#		pragma comment(lib,"stlport_x64.5.2.lib")
#		pragma comment(lib,"../bin/NetworkEngine64.lib")
#		pragma comment(lib,"../bin/ToolKit64.lib")
#		pragma comment(lib,"../bin/Common64.lib")
#	else	
#		error "Can not supported on this architecture."
#	endif	
#endif

#define MAX_ROBOT_NUM		500

// ����̨�¼�������
BOOL WINAPI ConsoleHandler(DWORD event)
{
	switch(event)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_CLOSE_EVENT:
		//g_MapServerThread.EndThread();
		for(;;) 
		{
		}
		break;
	}

	return TRUE;
}

BOOL g_bThreadExitFlg;

unsigned int __stdcall ProcessRobot( LPVOID param )
{

	char	szTitile[128];
	char	szMsg[256];
	DWORD	dwInterval = 0;

	GetConsoleTitleA(szTitile, sizeof(szTitile));

	while(!g_bThreadExitFlg)
	{
		static DWORD lastUpdateTick = GetTickCount();

		DWORD curTick = GetTickCount();

		DWORD deltaTick;

		if (curTick>=lastUpdateTick)
		{
			deltaTick = curTick - lastUpdateTick;
		}
		else
		{
			deltaTick = 0xFFFFFFFF - lastUpdateTick + curTick + 1;
		}

		lastUpdateTick = curTick;

		//��������ͨ��
		RobotManager::Instance()->Update(deltaTick);
		//printf("����ս����������:[%d]\n",RobotManager::Instance()->GetSendNum());

		if (dwInterval >= 1000)
		{

			sprintf(szMsg, "%s ���ӻ�������:%d", szTitile, RobotManager::Instance()->GetRobotNum());
			SetConsoleTitleA(szMsg);
			dwInterval -= 1000;
		}
		else
		{
			dwInterval += deltaTick;
		}

		Sleep(1);
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// ���ÿ���̨�¼�������
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		printf("���ÿ���̨�¼�������ʧ��!\n");
		return -1;
	}

	HWND hWnd = GetConsoleWindow();
	if (hWnd)
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
	}

	printf("Disable close button, type exit to shutdown\n");

	//ע����Ϣ
	printf("��ʼע����Ϣ.\n");
	PacketHandler::Instance()->Resister();


	//��ʼ�������˹�����
	RobotFactory::Instance()->Init(MAX_ROBOT_NUM);
	if (!RobotManager::Instance()->Init())
	{
		printf("��ʼ�������˹�����ʧ��.\n");
		return 0;
	}

	//��ʼ��ս������
	//BattleFactory::Instance()->Init();

	//�����̴߳��������
	unsigned int nThreadID = 0;
	g_bThreadExitFlg = FALSE;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ProcessRobot, NULL, 0, &nThreadID);

	char commandbuf[256];
	while (1)
	{
		fflush(stdout);

		char *command_str = fgets(commandbuf,sizeof(commandbuf),stdin);
		if (command_str != NULL)
		{
			for(int x=0;command_str[x];x++)
			{
				if(command_str[x]=='\r'||command_str[x]=='\n')
				{
					command_str[x]=0;
					break;
				}
			}

			if (strcmp(command_str, "test") == 0)
			{
				printf("test func......\n");
				RobotManager::Instance()->SetTest();
			}

			if (strcmp(command_str,"exit") == 0)
			{
				printf("Exit Server......\n");
				break;
			}
		}
	}

	//�ȴ������߳̽���
	g_bThreadExitFlg = TRUE;
	WaitForSingleObject(hThread, INFINITE);

	//�ͷ���ҹ�����
	RobotManager::Instance()->Release();
	RobotManager::DestroyInstance();
	RobotFactory::Instance()->Release();
	RobotFactory::DestroyInstance();

	return 0;
}

