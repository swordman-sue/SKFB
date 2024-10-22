#include "stdafx.h"
#include "ObjectFactory.h"
#include "AccountInfo.h"
#include "Player.h"
#include "Monster.h"

ObjectFactory::ObjectFactory()
{
	m_pAccountInfoPool	= new CMemoryPoolFactory<AccountInfo>;
	m_pPlayerPool		= new CMemoryPoolFactory<Player>;
	m_pMonsterPool		= new CMemoryPoolFactory<Monster>;
	m_pObjectKeyGen		= new ObjKeyGenerator;
}

ObjectFactory::~ObjectFactory()
{
	if(m_pAccountInfoPool)
	{
		delete m_pAccountInfoPool;
		m_pAccountInfoPool = NULL;
	}

	if(m_pPlayerPool)
	{
		delete m_pPlayerPool;
		m_pPlayerPool = NULL;
	}

	if(m_pMonsterPool)
	{
		delete m_pMonsterPool;
		m_pMonsterPool = NULL;
	}

	if(m_pObjectKeyGen)
	{
		delete m_pObjectKeyGen;
		m_pObjectKeyGen = NULL;
	}
}

VOID ObjectFactory::Init(DWORD dwPlayerNum)
{
	m_pAccountInfoPool->Initialize(dwPlayerNum, dwPlayerNum / 2);
	m_pPlayerPool->Initialize(dwPlayerNum, dwPlayerNum / 2);
	m_pMonsterPool->Initialize(dwPlayerNum, dwPlayerNum / 2);
	m_pObjectKeyGen->Create(ID_MONSTER_MIN, ID_MONSTER_MAX);
}

VOID ObjectFactory::Release()
{
	if(m_pAccountInfoPool)
		m_pAccountInfoPool->Release();

	if(m_pPlayerPool)
		m_pPlayerPool->Release();

	if(m_pMonsterPool)
		m_pMonsterPool->Release();
}

AccountInfo* ObjectFactory::AllocAccountInfo()
{
	AccountInfo* pInfo = m_pAccountInfoPool->Alloc();
	if(pInfo)
		pInfo->Init();

	return pInfo;
}

VOID ObjectFactory::FreeAccountInfo(AccountInfo* pAccountInfo)
{
	if(pAccountInfo)
		m_pAccountInfoPool->Free(pAccountInfo);
}

Player* ObjectFactory::AllocPlayer()
{
	Player* pPlayer = m_pPlayerPool->Alloc();
	if(pPlayer)
	{
		pPlayer->SetObjectType(OBJECT_PLAYER);
		pPlayer->Init();
	}

	return pPlayer;
}

VOID ObjectFactory::FreePlayer(Player* pPlayer)
{
	if(pPlayer)
	{
		pPlayer->Release();
		m_pPlayerPool->Free(pPlayer);
	}
}

Monster* ObjectFactory::AllocMonster()
{
	Monster* pMonster = m_pMonsterPool->Alloc();
	if(pMonster)
	{
		pMonster->SetObjectKey(m_pObjectKeyGen->Getkey());
		pMonster->SetObjectType(OBJECT_MONSTER);
	}

	return pMonster;
}

VOID ObjectFactory::FreeMonster(Monster* pMonster)
{
	if(pMonster)
	{
		m_pObjectKeyGen->RestoreKey(pMonster->GetObjectKey());
		m_pMonsterPool->Free(pMonster);
	}
}