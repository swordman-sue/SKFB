// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OpenServer.h"

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
		g_OpenServerThread.EndThread();
		for(;;) 
		{
		}
		break;
	}

	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	// ���ô��ڵĴ�С
// 	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
// 	COORD size = { 150, 200 };
// 	SetConsoleScreenBufferSize(hOut, size); // �������û�������С

	// ���ÿ���̨�¼�������
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		printf("���ÿ���̨�¼�������ʧ��!\n");
		return -1;
	}

	printf("Disable close button, type exit to shutdown\n");

	//�����������߳�
	g_OpenServerThread.StartThread();

	HWND hWnd = GetConsoleWindow();
	if (hWnd)
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
	}

	char commandbuf[256];

	while (!g_OpenServerThread.IsShutDown())
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


			if (strcmp(command_str,"exit") == 0)
			{
				printf("Exit Server......\n");
				g_OpenServerThread.EndThread();
			}

		}
	}

	//�ȴ����߳���ȫ�˳������߳��˳�
	g_OpenServerThread.Release();

	printf("Server has been closed......\n");

	return 0;
}