#include "stdafx.h"
#include "RobotManager.h"
#include "RobotFactory.h"
#include "Robot.h"
#include "../Common/DBCDefine.h"
#include "../ToolKit/IniFile.h"
#include "../ToolKit/Rand.h"

NetworkObject* CreateServerSideAcceptedObject()
{
	return NULL;
}

VOID DestroyServerSideAcceptedObject(NetworkObject* pNetworkObject)
{

}

VOID DestroyServerSideConnectedObject(NetworkObject* pNetworkObject)
{

}

RobotManager::RobotManager()
{
	memset(m_szSvrIP, 0, sizeof(m_szSvrIP));
	m_wSvrPort = 0;
	m_RobotSet.clear();
	m_bEnableSecurity = FALSE;
	m_nOpenChat = 0;
	m_dwBattleMode = 0;
}

RobotManager::~RobotManager()
{

}

BOOL RobotManager::Init()
{
	if (!m_RobotLoginDBCFile.Load("RobotLogin.txt"))
	{
		printf("无效的机器人登录文件.\n");
		return FALSE;
	}
	
	m_dwSendNum = 0;

	m_bTest = false;
	

	//加载卡牌配置文件 
// 	if (!m_CardDBCFile.Load("CardEntry.txt"))
// 	{
// 		printf("加载卡牌配置文件失败.\n");
// 		return FALSE;
// 	}

	//加载随从配置文件
	if (!m_RetinueDBCFile.Load("RetinueEntry.txt"))
	{
		printf("加载随从配置文件失败.\n");
		return FALSE;
	}

	//读取设定文件
	IniFile cIniFile;
	cIniFile.Init("Robot.ini");

	//读取服务器IP和端口
	cIniFile.GetStrVal("Server", "ServerIP", m_szSvrIP, sizeof(m_szSvrIP));
	m_wSvrPort = cIniFile.GetIntVal("Server", "ServerPort");

	//读取机器人配置
	m_nOpenChat = cIniFile.GetIntVal("Robot", "OpenChat");
	m_dwZoneID  = (DWORD)cIniFile.GetIntVal("Robot", "ZoneID");
	m_dwBattleMode = (DWORD)cIniFile.GetIntVal("Robot", "BattleMode");
	m_bIsTestAttackCity = cIniFile.GetIntVal("Robot", "IsTestAttackCity");
	m_dwSelectZoneId = cIniFile.GetIntVal("Robot", "SelectZoneId");
	
	

	//配置网络参数
	IOHANDLER_DESC desc;
	desc.dwIoHandlerKey = 0;
	desc.dwMaxAcceptSession = 0;
	desc.dwMaxConnectSession = cIniFile.GetIntVal("IOHANDLER", "MaxConnectSession");
	desc.dwMaxPacketSize = 5120;
	desc.dwNumberOfAcceptThreads = 0;
	desc.dwNumberOfConnectThreads = 1;
	desc.dwNumberOfIoThreads = 1;
	desc.dwRecvBufferSize = cIniFile.GetIntVal("IOHANDLER", "RecvBufferSize");
	desc.dwSendBufferSize = cIniFile.GetIntVal("IOHANDLER", "SendBufferSize");
	desc.dwTimeOut = 0;
	desc.fnCreateAcceptedObject = CreateServerSideAcceptedObject;
	desc.fnDestroyAcceptedObject = DestroyServerSideAcceptedObject;
	desc.fnDestroyConnectedObject = DestroyServerSideConnectedObject;
	desc.bSecurity = cIniFile.GetIntVal("IOHANDLER", "EnableSecurity");

	m_bEnableSecurity = desc.bSecurity;

	//初始化网络引擎
	if(!m_IOCPServer.Init(&desc, 1))
	{
		printf("IOCP初始化失败.\n");
		return FALSE;
	}

	//机器人文件为空
	

	//加载/连接所有机器人

	RobotLoginEntry* pRobotLoginEntry = NULL;
	for (int i = 0; i < m_RobotLoginDBCFile.GetRecordNum(); ++i)
	{
		pRobotLoginEntry = (RobotLoginEntry*)m_RobotLoginDBCFile.IterateRecord(i);
		if (!pRobotLoginEntry)
			continue;
		if (!pRobotLoginEntry->m_szFileName)
		{
			printf("无效的机器人配置文件.\n");
			return FALSE;
		}
		if (i>=5)
		{
			break;;
		}
		
		//加载机器人配置文件
		if (!m_RobotDBCFile[i].Load(pRobotLoginEntry->m_szFileName))
		{
			printf("加载机器人配置文件%s失败.\n",pRobotLoginEntry->m_szFileName);
			continue;
		}
		int nRecNum = m_RobotDBCFile[i].GetRecordNum();
		if (nRecNum <= 0)
		{
			printf("机器人记录不存在.\n");
			continue;
		}

		RobotEntry* pRobotEntry = NULL;
		Robot* pRobot = NULL;
		for (int j = 0; j < nRecNum; ++j)
		{
			pRobotEntry = (RobotEntry*)m_RobotDBCFile[i].IterateRecord(j);
			if (!pRobotEntry)
				continue;

			pRobot = RobotFactory::Instance()->AllocRobot();
			if (!pRobot)
				continue;

			if (!pRobot->Init(pRobotEntry))
			{
				RobotFactory::Instance()->FreeRobot(pRobot);
				continue;
			}
			pRobot->SetBattleType(pRobotLoginEntry->m_dwBattleType);

			//连接到服务器
			m_IOCPServer.Connect( 0, pRobot->GetRobotSession(), m_szSvrIP, m_wSvrPort);

			//加入机器人列表
			m_RobotSet.insert(pRobot);
			Sleep(1);
			
		}
		Sleep(100);


	}

	

	if (m_RobotSet.empty())
	{
		printf("机器人连接服务器失败.\n");
		return FALSE;
	}

	return TRUE;
}

VOID RobotManager::Release()
{
	Robot* pRobot;
	for (RobotSetIter iter = m_RobotSet.begin(); iter != m_RobotSet.end(); ++iter)
	{
		pRobot = *iter;
		if (!pRobot)
			continue;

		pRobot->Release();
		RobotFactory::Instance()->FreeRobot(pRobot);
	}

	m_RobotSet.clear();
}

VOID RobotManager::Update(DWORD dwTick)
{
	//更新网络
	m_IOCPServer.Update();

	//更新机器人
	Robot* pRobot = NULL;
	for (RobotSetIter iter = m_RobotSet.begin(); iter != m_RobotSet.end(); ++iter)
	{
		pRobot = *iter;
		if (!pRobot)
			continue;

 		pRobot->Update(dwTick);
	}

	if (m_bTest)
	{
		Test();
		m_bTest = false;
	}
}

// 通知命令行测试的接口
VOID RobotManager::Test()
{
	Robot* pRobot = NULL;
	for (RobotSetIter iter = m_RobotSet.begin(); iter != m_RobotSet.end(); ++iter)
	{
		pRobot = *iter;
		if (!pRobot)
			continue;

		pRobot->Test();
	}
}

VOID RobotManager::OnDisconnect(Robot* pRobot)
{
	if (!pRobot)
		return;

	RobotSetIter iter = m_RobotSet.find(pRobot);
	if (iter != m_RobotSet.end())
	{
		m_RobotSet.erase(iter);

		pRobot->Release();
		RobotFactory::Instance()->FreeRobot(pRobot);
	}
}

CardEntry* RobotManager::GetCardEntry(DWORD dwCardID)
{
	if (!dwCardID)
		return NULL;

	return (CardEntry*)m_CardDBCFile.GetRecord(dwCardID);
}

RetinueEntry* RobotManager::GetRetinueEntry(DWORD dwRetinueID)
{
	if (!dwRetinueID)
		return NULL;

	return (RetinueEntry*)m_RetinueDBCFile.GetRecord(dwRetinueID);
}