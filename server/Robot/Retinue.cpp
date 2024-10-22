#include "stdafx.h"
#include "Retinue.h"
#include "Chief.h"
#include "Card.h"
#include "RobotManager.h"
#include "../Common/DBCDefine.h"
#include "../Common/PacketStruct_MSG.h"

Retinue::Retinue()
{
	m_pChief		= NULL;
	m_pRetinueEntry	= NULL;
}

Retinue::~Retinue()
{

}

BOOL Retinue::Init(Robot* pRobot, Chief* pChief, RetinueEntry* pRetinueEntry, BYTE byOrien)
{
	if (!pRetinueEntry)
		return FALSE;

	m_pChief		= pChief;
	m_pRetinueEntry = pRetinueEntry;

	//调用基类的函数
	if (!GameObject::Init(pRobot))
		return FALSE;

	//初始化属性
	if (!InitAttr(byOrien))
		return FALSE;

	return TRUE;
}

VOID Retinue::Release()
{
	m_pChief		= NULL;
	m_dwRetinueID	= 0;
	m_pRetinueEntry	= NULL;

	//最后调用基类的函数
	GameObject::Release();
}

VOID Retinue::Update(DWORD dwTick)
{
	//先调用基类的函数
	GameObject::Update(dwTick);
}

//初始化随从属性
BOOL Retinue::InitAttr(BYTE byOrien)
{
	if (!m_pChief || !m_pRetinueEntry)
		return FALSE;

	if (byOrien != GO_ORIEN_FRONT && byOrien != GO_ORIEN_BACK)
		return FALSE;

	m_GameObjectAttr.SetBaseRace(m_pRetinueEntry->m_dwRetinueRace);
	m_GameObjectAttr.SetBaseMaxHp(1);
	m_GameObjectAttr.SetBaseHp(1);
	m_GameObjectAttr.SetBaseAttackPower(m_pRetinueEntry->m_dwAttackPower);
	m_GameObjectAttr.SetBaseDefensePower(m_pRetinueEntry->m_dwDefensePower);
	if (byOrien == GO_ORIEN_BACK)
		m_GameObjectAttr.SetBaseAction(0);
	else
		m_GameObjectAttr.SetBaseAction(m_pRetinueEntry->m_dwAction);
	m_GameObjectAttr.SetBaseOrien(byOrien);

	return TRUE;
}

//获得献祭随从数量
DWORD Retinue::GetSacrificeNum() const
{
	if (!m_pRetinueEntry)
		return 0;

	CardEntry* pCardEntry = RobotManager::Instance()->GetCardEntry(m_pRetinueEntry->m_dwCardID);
	if (!pCardEntry)
		return 0;

	return pCardEntry->m_dwSacrificeNum;
}

//检测随从是否能够攻击
BOOL Retinue::IsEnableAttack()
{
	if (!m_pChief)
		return FALSE;

	//随从死亡
	if (IsDead())
		return FALSE;

	//攻击力为0
	if (GetAttackPower() <= 0)
		return FALSE;

	//随从没有行动力
	if (GetAction() <= 0)
		return FALSE;

	//随从处于威慑状态
	if (HasStatus(GO_STATUS_OVERAWE))
		return FALSE;

	//随从处于枯竭状态
	if (HasStatus(GO_STATUS_DRAIN) && m_pChief->GetHandCardNum() <= 0)
		return FALSE;

	return TRUE;
}
