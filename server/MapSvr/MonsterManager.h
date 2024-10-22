#pragma once
#include <map>
#include "../ToolKit/Singleton.h"

class Monster;
struct MonsterEntry;
typedef std::map<DWORD, Monster*>		MonsterMap;
typedef MonsterMap::iterator			MonsterMapIter;

//怪物管理器
class MonsterManager : public Singleton<MonsterManager>
{
public:
	MonsterManager();
	~MonsterManager();

	BOOL			Init();
	VOID			Release();
	VOID			Update(DWORD dwTick);

	Monster*		CreateMonster(DWORD dwMonsterID);
	VOID			RemoveMonster(Monster* pMonster);
	Monster*		FindMonster(DWORD dwObjectKey);

public:
	MonsterEntry*	GetMonsterEntry(DWORD dwMonsterID);

private:
	MonsterMap		m_MonsterMap;			//怪物列表
};