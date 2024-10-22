#pragma once
#include "../Common/Const.h"
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"
#include "../ToolKit/ObjKeyGenerator.h"

class AccountInfo;
class Player;
class Monster;

class ObjectFactory : public Singleton<ObjectFactory>
{
public:
	ObjectFactory(void);
	~ObjectFactory(void);

	VOID			Init(DWORD dwPlayerNum);
	VOID			Release();

	AccountInfo*	AllocAccountInfo();
	VOID			FreeAccountInfo(AccountInfo* pAccountInfo);

	Player*			AllocPlayer();
	VOID			FreePlayer(Player* pPlayer);

	Monster*		AllocMonster();
	VOID			FreeMonster(Monster* pMonster);

private:
	CMemoryPoolFactory<AccountInfo>*	m_pAccountInfoPool;				//帐号信息内存池
	CMemoryPoolFactory<Player>*			m_pPlayerPool;					//玩家对象内存池
	CMemoryPoolFactory<Monster>*		m_pMonsterPool;					//怪物对象内存池
	ObjKeyGenerator*					m_pObjectKeyGen;				//ObjectKey生成器
};