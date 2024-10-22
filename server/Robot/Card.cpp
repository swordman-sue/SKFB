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

	//��ȡ��������
	m_pCardEntry = RobotManager::Instance()->GetCardEntry(dwCardID);
	if (!m_pCardEntry)
		return FALSE;

	//���û���ĺ���
	if (!GameObject::Init(pRobot))
		return FALSE;

	//��ʼ������
	if (!InitAttr())
		return FALSE;

	return TRUE;
}

VOID Card::Release()
{
	m_pChief	 = NULL;
	m_dwCardID	 = 0;
	m_pCardEntry = NULL;

	//�����û���ĺ���
	GameObject::Release();
}

VOID Card::Update(DWORD dwTick)
{
	//�ȵ��û���ĺ���
	GameObject::Update(dwTick);
}

//��ʼ������
BOOL Card::InitAttr()
{
	if (!m_pCardEntry)
		return FALSE;

	m_GameObjectAttr.SetBaseRace(m_pCardEntry->m_dwCardRace);
	m_GameObjectAttr.SetBaseAttackPower(m_pCardEntry->m_dwAttackPower);
	m_GameObjectAttr.SetBaseDefensePower(m_pCardEntry->m_dwDefensePower);

	return TRUE;
}
