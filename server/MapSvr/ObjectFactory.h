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
	CMemoryPoolFactory<AccountInfo>*	m_pAccountInfoPool;				//�ʺ���Ϣ�ڴ��
	CMemoryPoolFactory<Player>*			m_pPlayerPool;					//��Ҷ����ڴ��
	CMemoryPoolFactory<Monster>*		m_pMonsterPool;					//��������ڴ��
	ObjKeyGenerator*					m_pObjectKeyGen;				//ObjectKey������
};