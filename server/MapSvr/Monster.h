#pragma once
#include "Character.h"
#include "../Common/DBCDefine.h"

class Level;

//怪物类
class Monster : public Character
{
public:
	Monster();
	~Monster();

	BOOL			Init(Level* pLevel, DWORD dwMonsterID);
	VOID			Release();
	VOID			Update(DWORD dwTick);

	DWORD			GetMonsterID() const { return m_pMonsterEntry ? m_pMonsterEntry->m_dwMonsterID : 0; }
	BYTE			GetRace() const { return m_pMonsterEntry ? (BYTE)m_pMonsterEntry->m_dwMonsterRace : 0; }
	const char*		GetRoleName() { return m_pMonsterEntry ? m_pMonsterEntry->m_szMonsterName : NULL; }
	DWORD			GetHp() const { return m_pMonsterEntry ? m_pMonsterEntry->m_dwHp : 0; }
	USHORT			GetLevel() const { return m_pMonsterEntry ? (USHORT)m_pMonsterEntry->m_dwMonsterLv : 0; }
	VOID			FillBattleCard(DWORD* pGroupCard, BYTE& byCardNum);

	//Level*			GetLevel() const { return m_pLevel; }
	MonsterEntry*	GetMonsterEntry() const { return m_pMonsterEntry; }

private:
	Level*			m_pLevel;				//所属关卡
	MonsterEntry*	m_pMonsterEntry;		//怪物配置文件		
};