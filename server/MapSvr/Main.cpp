// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MapServer.h"
#include "..\Common\CommonLogic.h"

// 控制台事件处理函数
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
	// 设置窗口的大小
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD size = { 150, 200 };
	SetConsoleScreenBufferSize(hOut, size); // 重新设置缓冲区大小

	// 设置控制台事件处理函数
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		printf("设置控制台事件处理器失败!\n");
		return -1;
	}

	printf("Disable close button, type exit to shutdown\n");

	//开启服务器线程
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

	//等待子线程完全退出后，主线程退出
	g_MapServerThread.Release();

	printf("Server has been closed......\n");

	return 0;
}


void DoCommand(const char * cmd)
{
	char szCmd[64] = {0};
	strcpy(szCmd, CPXValue(cmd,0,';'));
	//int nParam1 = CPXValue(cmd, 1, ';');

	// 退出
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
	// 显示列家列表
	else if (strcmp(szCmd,"playerlist") == 0)
	{
		g_MapServer.ShowPlayerList();
	}
	// 广播停服维护
	else if (strcmp(szCmd,"stopserver") == 0)
	{
		g_MapServer.StopServer();
	}
	// 显示命令说明
	else if (strcmp(szCmd,"help") == 0)
	{
		printf("exit:退出\n");
		printf("playerlist:显示所有在线玩家\n");
		printf("stopserver:广播停服维护\n");
	}
	// 重新加载配置
	else if (strcmp(szCmd, "reload") == 0)
	{
		
	}
	// 开启GM
	else if (strcmp(szCmd, "opengm") == 0)
	{
		g_MapServer.SetIsOpenGM(true);
	}
	// 关闭GM
	else if (strcmp(szCmd, "closegm") == 0)
	{
		g_MapServer.SetIsOpenGM(false);
	}
}
