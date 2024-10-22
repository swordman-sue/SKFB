#pragma once 
#include "GenertorManager.h"
#include "../ToolKit/ThreadBase.h"
#include "../ToolKit/CriticalSection.h"
#include "../ToolKit/Linkedlist.h"

struct RobotEntry;
class  SqlDatabase;

class Genertor : public ThreadBase
{
public:
	Genertor(VOID);
	virtual ~Genertor(VOID);

public:
	BOOL						Initialize(DatabaseDesc& desc, DWORD dwZoneID);
	VOID						Update();
	virtual VOID				Release();
	BOOL						DBProcess(RobotEntry* pRobotEntry);
	size_t						GetProcessListSize() { return m_pProcessList->GetCount(); }

protected:
	virtual VOID				EndThread();
	virtual DWORD				run();
	BOOL						CreateRobot(RobotEntry* pRobotEntry);

private:
	SqlDatabase*				m_pDatabase;
	BOOL						m_bEndProcess;
	CriticalSection *			m_pProcessCS;			//�����б������
	CriticalSection *			m_pProcessResultCS;		//�������б������
	CLinkedlist<RobotEntry*>*	m_pProcessList;			//�������б�
	CLinkedlist<RobotEntry*>*	m_pProcessResultList;	//�������б�
	DWORD						m_dwZoneID;				//����ID
};
