#include "stdafx.h"
#include "GenertorManager.h"
#include "Genertor.h"

GenertorManager::GenertorManager()
{
	m_pGenertorThread	  = NULL;
	m_dwGenertorThreadNum = 0;
	m_nProcessNum		  = 0;
	m_nCurProcessNum	  = 0;
}

GenertorManager::~GenertorManager()
{

}

BOOL GenertorManager::Init(DatabaseDesc& desc, char* pRobotFile, DWORD dwDBThreadNum, DWORD dwZoneID)
{
	if (!pRobotFile)
		return FALSE;

	//创建处理线程
	m_dwGenertorThreadNum = dwDBThreadNum;
	m_pGenertorThread = new Genertor[m_dwGenertorThreadNum];
	if (!m_pGenertorThread)
		return FALSE;

	//启动处理线程
	for (DWORD i = 0; i < m_dwGenertorThreadNum; ++i)
	{
		if(!m_pGenertorThread[i].Initialize(desc, dwZoneID))
		{
			delete[] m_pGenertorThread;
			return FALSE;
		}
	}

	//读取需要处理的数据文件
	if (!m_cDBCFile.Load(pRobotFile))
		return FALSE;

	//投递处理给线程
	m_nProcessNum = m_cDBCFile.GetRecordNum();
	RobotEntry* pRobotEntry = NULL;
	DWORD dwIndex = 0;
	for (int i = 0; i < m_nProcessNum; ++i)
	{
		pRobotEntry = (RobotEntry*)m_cDBCFile.IterateRecord(i);
		if (pRobotEntry)
		{
			if (dwIndex >= m_dwGenertorThreadNum)
				dwIndex = 0;

			m_pGenertorThread[dwIndex].DBProcess(pRobotEntry);
			++dwIndex;
		}
	}

	return TRUE;
}

VOID GenertorManager::Release()
{
	for (DWORD i = 0;i < m_dwGenertorThreadNum; ++i)
	{
		m_pGenertorThread[i].Release();
	}

	delete[] m_pGenertorThread;
}

VOID GenertorManager::Update()
{
	while (m_nCurProcessNum < m_nProcessNum)
	{
		for (DWORD i = 0;  i< m_dwGenertorThreadNum; ++i)
		{
			m_pGenertorThread[i].Update();
		}

		Sleep(1);
	}
}

VOID GenertorManager::ProcessResult(RobotEntry* pRobotEntry)
{
	if (!pRobotEntry)
		return;

	++m_nCurProcessNum;
	printf("处理中:................................%4d/%4d.\r", m_nCurProcessNum, m_nProcessNum);
}