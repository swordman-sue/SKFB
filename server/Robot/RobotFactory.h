#pragma once

#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"

class Robot;
class RobotFactory : public Singleton<RobotFactory>
{
public:
	RobotFactory();
	~RobotFactory();

public:
	VOID			Init(DWORD dwRobotPoolSize);
	VOID			Release();

	Robot*			AllocRobot();
	VOID			FreeRobot(Robot *pRobot);

private:
	CMemoryPoolFactory<Robot>*	m_pRobotPool;		//»úÆ÷ÈËÄÚ´æ³Ø
};