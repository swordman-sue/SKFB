#pragma once
#include "../ToolKit/Singleton.h"
#include "../ToolKit/MemoryPool.h"

class Chief;
class Retinue;
//class Card;

class BattleFactory : public Singleton<BattleFactory>
{
public:
	BattleFactory();
	~BattleFactory();

	VOID			Init();
	VOID			Release();

	Chief*			AllocChief();
	VOID			FreeChief(Chief* pChief);

	Retinue*		AllocRetinue();
	VOID			FreeRetinue(Retinue* pRetinue);

	Card*			AllocCard();
	VOID			FreeCard(Card* pCard);

private:
	CMemoryPoolFactory<Chief>*		m_pChiefPool;			//��������ڴ��
	CMemoryPoolFactory<Retinue>*	m_pRetinuePool;			//��Ӷ����ڴ��
	CMemoryPoolFactory<Card>*		m_pCardPool;			//���ƶ����ڴ��
};