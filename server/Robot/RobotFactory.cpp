#include "stdafx.h"
#include "RobotFactory.h"
#include "Robot.h"

RobotFactory::RobotFactory()
{
	m_pRobotPool = new CMemoryPoolFactory<Robot>;
}

RobotFactory::~RobotFactory()
{
	if (m_pRobotPool)
	{
		delete m_pRobotPool;
		m_pRobotPool = NULL;
	}
}

//��ʼ���������ڴ��
VOID RobotFactory::Init(DWORD dwRobotPoolSize)
{
	m_pRobotPool->Initialize(dwRobotPoolSize, dwRobotPoolSize/2);
}

//�ͷŻ������ڴ��
VOID RobotFactory::Release()
{
	if (m_pRobotPool)
	{
		delete m_pRobotPool;
		m_pRobotPool = NULL;
	}
}

//����һ���������ڴ�
Robot* RobotFactory::AllocRobot()
{
	return m_pRobotPool->Alloc();
}

//����һ���������ڴ�
VOID RobotFactory::FreeRobot(Robot* pRobot)
{
	if (!pRobot)
		return;

	m_pRobotPool->Free(pRobot);
}