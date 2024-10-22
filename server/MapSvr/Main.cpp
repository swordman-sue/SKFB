// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MapServer.h"
#include "..\Common\CommonLogic.h"

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

void DoCommand(const char * cmd);

int _tmain(int argc, _TCHAR* argv[])
{
	// ���ô��ڵĴ�С
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD size = { 150, 200 };
	SetConsoleScreenBufferSize(hOut, size); // �������û�������С

	// ���ÿ���̨�¼�������
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		printf("���ÿ���̨�¼�������ʧ��!\n");
		return -1;
	}

	printf("Disable close button, type exit to shutdown\n");

	//�����������߳�
	g_MapServerThread.StartThread();

	HWND hWnd = GetConsoleWindow();
	if (hWnd)
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
	}

	char commandbuf[256];

	while (!g_MapServerThread.IsShutDown())
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

			DoCommand(command_str);
		}

	}

	//�ȴ����߳���ȫ�˳������߳��˳�
	g_MapServerThread.Release();

	printf("Server has been closed......\n");

	return 0;
}


void DoCommand(const char * cmd)
{
	char szCmd[64] = {0};
	strcpy(szCmd, CPXValue(cmd,0,';'));
	//int nParam1 = CPXValue(cmd, 1, ';');

	// �˳�
	if (strcmp(szCmd,"exit") == 0)
	{
		printf("Exit Server......\n");
		g_MapServerThread.EndThread();
	}
	else if (strcmp(szCmd,"logs") == 0)
	{
		g_MapServer.WriteLogFlag(TRUE);
	}
	else if (strcmp(szCmd,"logClean") == 0)
	{
		g_MapServer.WriteLogFlag(false);
	}
	// ��ʾ�м��б�
	else if (strcmp(szCmd,"playerlist") == 0)
	{
		g_MapServer.ShowPlayerList();
	}
	// �㲥ͣ��ά��
	else if (strcmp(szCmd,"stopserver") == 0)
	{
		g_MapServer.StopServer();
	}
	// ��ʾ����˵��
	else if (strcmp(szCmd,"help") == 0)
	{
		printf("exit:�˳�\n");
		printf("playerlist:��ʾ�����������\n");
		printf("stopserver:�㲥ͣ��ά��\n");
	}
	// ���¼�������
	else if (strcmp(szCmd, "reload") == 0)
	{
		
	}
	// ����GM
	else if (strcmp(szCmd, "opengm") == 0)
	{
		g_MapServer.SetIsOpenGM(true);
	}
	// �ر�GM
	else if (strcmp(szCmd, "closegm") == 0)
	{
		g_MapServer.SetIsOpenGM(false);
	}
}
