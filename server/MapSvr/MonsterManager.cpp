#include "stdafx.h"
#include "MonsterManager.h"
#include "Monster.h"
#include "ObjectFactory.h"
#include "../Common/DBCDefine.h"
#include "../ToolKit/DBCFileManager.h"

MonsterManager::MonsterManager()
{
	m_MonsterMap.clear();
}

MonsterManager::~MonsterManager()
{

}

BOOL MonsterManager::Init()
{
	m_MonsterMap.clear();

	return TRUE;
}

VOID MonsterManager::Release()
{
	Monster* pMonster = NULL;
	for(MonsterMapIter iter = m_MonsterMap.begin(); iter != m_MonsterMap.end(); ++iter)
	{
		pMonster = iter->second;
		if (!pMonster)
			continue;

		pMonster->Release();
		ObjectFactory::Instance()->FreeMonster(pMonster);
	}

	m_MonsterMap.clear();
}

VOID MonsterManager::Update(DWORD dwTick)
{
	for(MonsterMapIter iter = m_MonsterMap.begin(); iter != m_MonsterMap.end(); ++iter)
	{
		if (!iter->second)
			continue;

		iter->second->Update(dwTick);
	}
}

Monster* MonsterManager::CreateMonster(DWORD dwMonsterID)
{
	if (!dwMonsterID)
		return NULL;

	Monster* pMonster = ObjectFactory::Instance()->AllocMonster();
	if (!pMonster)
		return NULL;

	//if (!pMonster->Init(dwMonsterID))
	//{
	//	ObjectFactory::Instance()->FreeMonster(pMonster);
	//	return NULL;
	//}

	m_MonsterMap.insert(std::make_pair(pMonster->GetObjectKey(), pMonster));

	return pMonster;
}

VOID MonsterManager::RemoveMonster(Monster* pMonster)
{
	if (!pMonster)
		return;

	MonsterMapIter iter = m_MonsterMap.find(pMonster->GetObjectKey());
	if (iter != m_MonsterMap.end())
		m_MonsterMap.erase(iter);

	pMonster->Release();
	ObjectFactory::Instance()->FreeMonster(pMonster);
}

Monster* MonsterManager::FindMonster(DWORD dwObjectKey)
{
	if (!dwObjectKey)
		return NULL;

	MonsterMapIter iter = m_MonsterMap.find(dwObjectKey);
	if (iter == m_MonsterMap.end())
		return NULL;

	return iter->second;
}

MonsterEntry* MonsterManager::GetMonsterEntry(DWORD dwMonsterID)
{
	if (!dwMonsterID)
		return NULL;

	DBCFile* pDBCFile = DBCFileManager::Instance()->GetDBC(DBC_MONSTER_ENTRY);
	if (!pDBCFile)
		return NULL;

	return (MonsterEntry*)pDBCFile->GetRecord(dwMonsterID);
}