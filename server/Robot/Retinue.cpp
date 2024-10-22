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

	//���û���ĺ���
	if (!GameObject::Init(pRobot))
		return FALSE;

	//��ʼ������
	if (!InitAttr(byOrien))
		return FALSE;

	return TRUE;
}

VOID Retinue::Release()
{
	m_pChief		= NULL;
	m_dwRetinueID	= 0;
	m_pRetinueEntry	= NULL;

	//�����û���ĺ���
	GameObject::Release();
}

VOID Retinue::Update(DWORD dwTick)
{
	//�ȵ��û���ĺ���
	GameObject::Update(dwTick);
}

//��ʼ���������
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

//����׼��������
DWORD Retinue::GetSacrificeNum() const
{
	if (!m_pRetinueEntry)
		return 0;

	CardEntry* pCardEntry = RobotManager::Instance()->GetCardEntry(m_pRetinueEntry->m_dwCardID);
	if (!pCardEntry)
		return 0;

	return pCardEntry->m_dwSacrificeNum;
}

//�������Ƿ��ܹ�����
BOOL Retinue::IsEnableAttack()
{
	if (!m_pChief)
		return FALSE;

	//�������
	if (IsDead())
		return FALSE;

	//������Ϊ0
	if (GetAttackPower() <= 0)
		return FALSE;

	//���û���ж���
	if (GetAction() <= 0)
		return FALSE;

	//��Ӵ�������״̬
	if (HasStatus(GO_STATUS_OVERAWE))
		return FALSE;

	//��Ӵ��ڿݽ�״̬
	if (HasStatus(GO_STATUS_DRAIN) && m_pChief->GetHandCardNum() <= 0)
		return FALSE;

	return TRUE;
}
