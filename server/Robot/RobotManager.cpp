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
		printf("��Ч�Ļ����˵�¼�ļ�.\n");
		return FALSE;
	}
	
	m_dwSendNum = 0;

	m_bTest = false;
	

	//���ؿ��������ļ� 
// 	if (!m_CardDBCFile.Load("CardEntry.txt"))
// 	{
// 		printf("���ؿ��������ļ�ʧ��.\n");
// 		return FALSE;
// 	}

	//������������ļ�
	if (!m_RetinueDBCFile.Load("RetinueEntry.txt"))
	{
		printf("������������ļ�ʧ��.\n");
		return FALSE;
	}

	//��ȡ�趨�ļ�
	IniFile cIniFile;
	cIniFile.Init("Robot.ini");

	//��ȡ������IP�Ͷ˿�
	cIniFile.GetStrVal("Server", "ServerIP", m_szSvrIP, sizeof(m_szSvrIP));
	m_wSvrPort = cIniFile.GetIntVal("Server", "ServerPort");

	//��ȡ����������
	m_nOpenChat = cIniFile.GetIntVal("Robot", "OpenChat");
	m_dwZoneID  = (DWORD)cIniFile.GetIntVal("Robot", "ZoneID");
	m_dwBattleMode = (DWORD)cIniFile.GetIntVal("Robot", "BattleMode");
	m_bIsTestAttackCity = cIniFile.GetIntVal("Robot", "IsTestAttackCity");
	m_dwSelectZoneId = cIniFile.GetIntVal("Robot", "SelectZoneId");
	
	

	//�����������
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

	//��ʼ����������
	if(!m_IOCPServer.Init(&desc, 1))
	{
		printf("IOCP��ʼ��ʧ��.\n");
		return FALSE;
	}

	//�������ļ�Ϊ��
	

	//����/�������л�����

	RobotLoginEntry* pRobotLoginEntry = NULL;
	for (int i = 0; i < m_RobotLoginDBCFile.GetRecordNum(); ++i)
	{
		pRobotLoginEntry = (RobotLoginEntry*)m_RobotLoginDBCFile.IterateRecord(i);
		if (!pRobotLoginEntry)
			continue;
		if (!pRobotLoginEntry->m_szFileName)
		{
			printf("��Ч�Ļ����������ļ�.\n");
			return FALSE;
		}
		if (i>=5)
		{
			break;;
		}
		
		//���ػ����������ļ�
		if (!m_RobotDBCFile[i].Load(pRobotLoginEntry->m_szFileName))
		{
			printf("���ػ����������ļ�%sʧ��.\n",pRobotLoginEntry->m_szFileName);
			continue;
		}
		int nRecNum = m_RobotDBCFile[i].GetRecordNum();
		if (nRecNum <= 0)
		{
			printf("�����˼�¼������.\n");
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

			//���ӵ�������
			m_IOCPServer.Connect( 0, pRobot->GetRobotSession(), m_szSvrIP, m_wSvrPort);

			//����������б�
			m_RobotSet.insert(pRobot);
			Sleep(1);
			
		}
		Sleep(100);


	}

	

	if (m_RobotSet.empty())
	{
		printf("���������ӷ�����ʧ��.\n");
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
	//��������
	m_IOCPServer.Update();

	//���»�����
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

// ֪ͨ�����в��ԵĽӿ�
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