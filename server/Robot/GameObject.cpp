#include "stdafx.h"
#include "GameObject.h"
#include "Robot.h"
#include "Chief.h"

GameObject::GameObject(void)
{
	m_pOwner		= NULL;
	m_dwObjectKey	= 0;
	m_enObjectType	= OBJECT_NONE;

	//ע����Ϸ��������
	m_GameObjectAttr.RegisterAll();
}

GameObject::~GameObject(void)
{

}

BOOL GameObject::Init(Robot* pRobot)
{
	m_pOwner = pRobot;
	m_dwObjectKey	= 0;

	//��ʼ����Ϸ��������
	m_GameObjectAttr.Init();

	return TRUE;
}

VOID GameObject::Release()
{

}

VOID GameObject::Update( DWORD dwDeltaTick )
{

}

DWORD GameObject::GetChiefObjectKey()
{
	Chief* pChief = GetChief();
	if (!pChief)
		return 0;

	return pChief->GetObjectKey();
}

//�������ֵ
INT GameObject::GetAttr(DWORD dwAttrType, DWORD dwAttrKind) const
{
	if (dwAttrType >= GOAT_MAX)
		return 0;

	if (dwAttrKind >= eATTR_KIND_MAX)
		return 0;

	return m_GameObjectAttr.GetAttrVal(dwAttrType, dwAttrKind);
}

//��������ֵ
VOID GameObject::SetAttr(DWORD dwAttrType, DWORD dwAttrKind, INT nValue)
{
	if (dwAttrType >= GOAT_MAX)
		return;

	if (dwAttrKind >= eATTR_KIND_MAX)
		return;

	m_GameObjectAttr.SetAttrVal(dwAttrType, dwAttrKind, nValue);
}

//��������ֵ
VOID GameObject::AddAttr(DWORD dwAttrType, DWORD dwAttrKind, int nValue)
{
	if (dwAttrType >= GOAT_MAX)
		return;

	if (dwAttrKind >= eATTR_KIND_MAX)
		return;

	m_GameObjectAttr.AddAttrVal(dwAttrType, (eATTR_KIND)dwAttrKind, nValue);
}

//�޸�����ֵ
VOID GameObject::ClearAttr(DWORD dwAttrType)
{
	if (dwAttrType >= GOAT_MAX)
		return;

	m_GameObjectAttr.ClearAttrVal(dwAttrType);
}

//�Ƿ����ĳ��״̬
BOOL GameObject::HasStatus(GameObjectStatus enStatus)
{
	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
	return (dwState & enStatus) != 0;
}

//����ĳ��״̬
VOID GameObject::SetStatus(GameObjectStatus enStatus)
{
	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
	dwState |= enStatus;
	m_GameObjectAttr.SetBaseStatus(dwState);
}

//���ĳ��״̬
VOID GameObject::ClearStatus(GameObjectStatus enStatus)
{
	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
	dwState &= ~enStatus;
	m_GameObjectAttr.SetBaseStatus(dwState);
}