#include "stdafx.h"
#include "Monster.h"
#include "../Common/DBCDefine.h"
#include "../ToolKit/DBCFileManager.h"

Monster::Monster()
{
	m_pLevel = NULL;
	m_pMonsterEntry = NULL;
}

Monster::~Monster()
{

}

BOOL Monster::Init(Level* pLevel, DWORD dwMonsterID)
{
	if (!pLevel || !dwMonsterID)
		return FALSE;

	m_pLevel = pLevel;

	//读取怪物配置信息
	DBCFile* pDBCFile = DBCFileManager::Instance()->GetDBC(DBC_MONSTER_ENTRY);
	if (!pDBCFile)
		return NULL;

	m_pMonsterEntry = (MonsterEntry*)pDBCFile->GetRecord(dwMonsterID);
	if (!m_pMonsterEntry)
		return FALSE;

	return TRUE;
}

VOID Monster::Release()
{

}

VOID Monster::Update(DWORD dwTick)
{

}

VOID Monster::FillBattleCard(DWORD* pGroupCard, BYTE& byCardNum)
{
	if (!m_pMonsterEntry)
		return;

	byCardNum = 0;

// 	for (BYTE i = 0; i < MAX_BATTLE_CARD_NUM; ++i)
// 	{
// 		if (!m_pMonsterEntry->m_dwCardID[i])
// 			continue;
// 
// 		pGroupCard[byCardNum++] = m_pMonsterEntry->m_dwCardID[i];
// 	}
}