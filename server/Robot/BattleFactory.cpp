#include "stdafx.h"
#include "BattleFactory.h"
#include "Chief.h"
#include "Retinue.h"
#include "Card.h"
#include "../Common/Const.h"

BattleFactory::BattleFactory()
{
	m_pChiefPool	= new CMemoryPoolFactory<Chief>;
	m_pRetinuePool	= new CMemoryPoolFactory<Retinue>;
	m_pCardPool		= new CMemoryPoolFactory<Card>;
}

BattleFactory::~BattleFactory()
{
	if(m_pChiefPool)
	{
		delete m_pChiefPool;
		m_pChiefPool = NULL;
	}

	if(m_pRetinuePool)
	{
		delete m_pRetinuePool;
		m_pRetinuePool = NULL;
	}

	if (m_pCardPool)
	{
		delete m_pCardPool;
		m_pCardPool = NULL;
	}
}

VOID BattleFactory::Init()
{
	m_pChiefPool->Initialize(2000, 1000);
	m_pRetinuePool->Initialize(20000, 10000);
	m_pCardPool->Initialize(200000, 100000);
}

VOID BattleFactory::Release()
{

	if(m_pChiefPool)
		m_pChiefPool->Release();

	if(m_pRetinuePool)
		m_pRetinuePool->Release();

	if (m_pCardPool)
		m_pCardPool->Release();
}

Chief* BattleFactory::AllocChief()
{
	Chief* pChief = m_pChiefPool->Alloc();
	if(pChief)
	{
		pChief->SetObjectType(OBJECT_CHIEF);
	}

	return pChief;
}

VOID BattleFactory::FreeChief(Chief* pChief)
{
	if(pChief)
	{
		m_pChiefPool->Free(pChief);
	}
}

Retinue* BattleFactory::AllocRetinue()
{
	Retinue* pRetinue = m_pRetinuePool->Alloc();
	if(pRetinue)
	{
		pRetinue->SetObjectType(OBJECT_RETINUE);
	}

	return pRetinue;
}

VOID BattleFactory::FreeRetinue(Retinue* pRetinue)
{
	if(pRetinue)
	{
		m_pRetinuePool->Free(pRetinue);
	}
}

Card* BattleFactory::AllocCard()
{
	Card* pCard = m_pCardPool->Alloc();
	if(pCard)
	{
		pCard->SetObjectType(OBJECT_CARD);
	}

	return pCard;
}

VOID BattleFactory::FreeCard(Card* pCard)
{
	if(pCard)
	{
		m_pCardPool->Free(pCard);
	}
}