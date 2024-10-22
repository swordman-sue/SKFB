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

//初始化机器人内存池
VOID RobotFactory::Init(DWORD dwRobotPoolSize)
{
	m_pRobotPool->Initialize(dwRobotPoolSize, dwRobotPoolSize/2);
}

//释放机器人内存池
VOID RobotFactory::Release()
{
	if (m_pRobotPool)
	{
		delete m_pRobotPool;
		m_pRobotPool = NULL;
	}
}

//申请一条机器人内存
Robot* RobotFactory::AllocRobot()
{
	return m_pRobotPool->Alloc();
}

//回收一条机器人内存
VOID RobotFactory::FreeRobot(Robot* pRobot)
{
	if (!pRobot)
		return;

	m_pRobotPool->Free(pRobot);
}