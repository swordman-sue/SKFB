#include "stdafx.h"
#include "Card.h"
#include "Chief.h"
#include "RobotManager.h"
#include "../Common/DBCDefine.h"

Card::Card()
{
	m_pChief	 = NULL;
	m_dwCardID	 = 0;
	m_pCardEntry = NULL;
}

Card::~Card()
{

}

BOOL Card::Init(Robot* pRobot, Chief* pChief, DWORD dwCardID)
{
	if (!pChief || !dwCardID)
		return FALSE;

	m_pChief = pChief;
	m_dwCardID = dwCardID;

	//读取卡牌配置
	m_pCardEntry = RobotManager::Instance()->GetCardEntry(dwCardID);
	if (!m_pCardEntry)
		return FALSE;

	//调用基类的函数
	if (!GameObject::Init(pRobot))
		return FALSE;

	//初始化属性
	if (!InitAttr())
		return FALSE;

	return TRUE;
}

VOID Card::Release()
{
	m_pChief	 = NULL;
	m_dwCardID	 = 0;
	m_pCardEntry = NULL;

	//最后调用基类的函数
	GameObject::Release();
}

VOID Card::Update(DWORD dwTick)
{
	//先调用基类的函数
	GameObject::Update(dwTick);
}

//初始化属性
BOOL Card::InitAttr()
{
	if (!m_pCardEntry)
		return FALSE;

	m_GameObjectAttr.SetBaseRace(m_pCardEntry->m_dwCardRace);
	m_GameObjectAttr.SetBaseAttackPower(m_pCardEntry->m_dwAttackPower);
	m_GameObjectAttr.SetBaseDefensePower(m_pCardEntry->m_dwDefensePower);

	return TRUE;
}
