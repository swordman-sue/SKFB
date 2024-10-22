#include "StdAfx.h"
#include "GameObject.h"
#include "Character.h"
#include "../Common/MsgDefine.h"

GameObject::GameObject(void)
{
	m_pOwner	= NULL;
	//m_pBattle	= NULL;

	//ע����Ϸ��������
	//m_GameObjectAttr.RegisterAll();
}

GameObject::~GameObject(void)
{

}

BOOL GameObject::Init(Battle* pBattle, Character* pCharacter)
{
	if (!pBattle || !pCharacter)
		return FALSE;

	m_pOwner	= pCharacter;
	//m_pBattle	= pBattle;

	//��ʼ����Ϸ��������
	//m_GameObjectAttr.Init();

	//��ʼ������������
// 	if (!m_SpellManager.Init(this))
// 		return FALSE;

	//�����û���ĺ���
	return Object::Init();
}

VOID GameObject::Release()
{
	m_pOwner	= NULL;
	//m_pBattle	= NULL;

	//m_SpellManager.Release();

	//�����û���ĺ���
	Object::Release();
}

VOID GameObject::Update( DWORD dwTick )
{
	Object::Update(dwTick);

	//m_SpellManager.Update(dwTick);

	SendUpdateAttrCmd();
}

DWORD GameObject::GetChiefObjectKey()
{
// 	Chief* pChief = GetChief();
// 	if (!pChief)
		return 0;

	//return pChief->GetObjectKey();
}

VOID GameObject::AddToGameObjectMap(GameObject* pGameObject)
{
// 	if (!pGameObject)
// 		return;
// 
// 	if (!m_pBattle)
// 		return;
// 
// 	m_pBattle->AddToGameObjectMap(pGameObject);
}

VOID GameObject::RemoveFromGameObjectMap(GameObject* pGameObject)
{
// 	if (!pGameObject)
// 		return;
// 
// 	if (!m_pBattle)
// 		return;
// 
// 	m_pBattle->RemoveFromGameObjectMap(pGameObject);
}

GameObject*	GameObject::FindFromGameObjectMap(DWORD dwObjectKey)
{
// 	if (!dwObjectKey)
// 		return NULL;
// 
// 	if (!m_pBattle)
// 		return NULL;
// 
// 	return m_pBattle->FindFromGameObjectMap(dwObjectKey);
	return NULL;
}

//��������
VOID GameObject::SetDead(GameObject* pMurderer)
{
// 	if (IsDead())
// 		return;
// 
// 	//���ö���HpΪ0
// 	SetHp(0);

	//��������Key
	if (pMurderer)
		SetMurdererKey(pMurderer->GetObjectKey());


	//ͬ���ͻ��˸�������
	//SendUpdateAttrCmd();

	////��������¼�
	//if (GetObjectType() == OBJECT_RETINUE)
	//{
	//	//����ɱ��Ŀ���¼�
	//	if (pMurderer && pMurderer->GetObjectType() == OBJECT_RETINUE)
	//		pMurderer->TriggerBattleEvent(BATTLE_EVENT_KILL, pMurderer, this);

	//	//����Ŀ�������¼�
	//	if (pMurderer)
	//		TriggerBattleEvent(BATTLE_EVENT_DEAD, this, pMurderer);
	//}

	////���Ұ�Ŀ��
	//GameObject* pGameObject = GetBindObject();
	//if (pGameObject)
	//{
	//	//�����
	//	pGameObject->ClearBind();

	//	//�����
	//	ClearBind();

	//	//ͬ���ͻ��˸�������
	//	SendUpdateAttrCmd();
	//}

	////�����󶨵�װ���������(����SetDead�������������ѭ��)
	//if (pGameObject && pGameObject->IsEquipmentRetinue())
	//	pGameObject->SetDead(NULL);
}

//�������Ƿ�����ָ��������
BOOL GameObject::IsConformCond(BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	return TRUE;

// 	if (byCondType < STC_NONE || byCondType >= STC_MAX)
// 		return FALSE;
// 
// 	if (byCondSign < STCS_NONE || byCondSign >= STCS_MAX)
// 		return FALSE;
// 
// 	//������������,���������ֵ
// 	DWORD dwVal = 0;
// 	switch((SpellTargetCond)byCondType)
// 	{
// 		//����
// 	case STC_RACE:
// 		{
// 			dwVal = GetRace();
// 		}
// 		break;
// 
// 		//�ȼ�
// 	case STC_LV:
// 		{
// 			dwVal = GetLevel();
// 		}
// 		break;
// 
// 		//������
// 	case STC_AP:
// 		{
// 			dwVal = GetAttackPower();
// 		}
// 		break;
// 
// 		//������
// 	case STC_DP:
// 		{
// 			dwVal = GetDefensePower();
// 		}
// 		break;
// 
// 		//���ID
// 	case STC_RETINUE_ID:
// 		{
// 			dwVal = GetRetinueID();
// 		}
// 		break;
// 
// 		//�������
// 	case STC_RETINUE_TYPE:
// 		{
// 			dwVal = GetRetinueType();
// 		}
// 		break;
// 
// 		//���������
// 	case STC_RETINUE_SUB_TYPE:
// 		{
// 			dwVal = GetRetinueSubType();
// 		}
// 		break;
// 
// 		//����ID
// 	case STC_CARD_ID:
// 		{
// 			dwVal = GetCardID();
// 		}
// 		break;
// 
// 		//��������
// 	case STC_CARD_TYPE:
// 		{
// 			dwVal = GetCardType();
// 		}
// 		break;
// 
// 		//����������
// 	case STC_CARD_SUB_TYPE:
// 		{
// 			dwVal = GetCardSubType();
// 		}
// 		break;
// 
// 		//������ɫ
// 	case STC_COLOR:
// 		{
// 			dwVal = GetColor();
// 		}
// 		break;
// 
// 		//�׼��������
// 	case STC_SACRIFICE_NUM:
// 		{
// 			dwVal = GetSacrificeNum();
// 		}
// 		break;
// 
// 	default:
// 		return TRUE;
// 	}
// 
// 	//���������ֵΪ0�϶�����������
// 	if (dwVal == 0)
// 		return FALSE;
// 
// 	//�������
// 	switch ((SpellTargetCondSign)byCondSign)
// 	{
// 	case STCS_EQUAL:
// 		return (dwVal == dwCondVal);
// 
// 	case STCS_UNEQUAL:
// 		return (dwVal != dwCondVal);
// 
// 	case STCS_GREAT_EQUAL:
// 		return (dwVal >= dwCondVal);
// 
// 	case STCS_GREAT:
// 		return (dwVal > dwCondVal);
// 
// 	case STCS_LESS_EQUAL:
// 		return (dwVal <= dwCondVal);
// 
// 	case STCS_LESS:
// 		return (dwVal < dwCondVal);
// 
// 	default:
// 		return TRUE;
// 	}
}

//�������ֵ
INT GameObject::GetAttr(DWORD dwAttrType, DWORD dwAttrKind) const
{
	return 0;
// 	if (dwAttrType >= GOAT_MAX)
// 		return 0;
// 
// 	if (dwAttrKind >= eATTR_KIND_MAX)
// 		return 0;
// 
// 	return m_GameObjectAttr.GetAttrVal(dwAttrType, dwAttrKind);
}

//��������ֵ
VOID GameObject::SetAttr(DWORD dwAttrType, DWORD dwAttrKind, INT nValue)
{
// 	if (dwAttrType >= GOAT_MAX)
// 		return;
// 
// 	if (dwAttrKind >= eATTR_KIND_MAX)
// 		return;
// 
// 	m_GameObjectAttr.SetAttrVal(dwAttrType, dwAttrKind, nValue);
}

//��������ֵ
VOID GameObject::AddAttr(DWORD dwAttrType, DWORD dwAttrKind, int nValue)
{
// 	if (dwAttrType >= GOAT_MAX)
// 		return;
// 
// 	if (dwAttrKind >= eATTR_KIND_MAX)
// 		return;
// 
// 	m_GameObjectAttr.AddAttrVal(dwAttrType, (eATTR_KIND)dwAttrKind, nValue);
}

//�޸�����ֵ
VOID GameObject::ClearAttr(DWORD dwAttrType)
{
// 	if (dwAttrType >= GOAT_MAX)
// 		return;
// 
// 	m_GameObjectAttr.ClearAttrVal(dwAttrType);
}

//����Ƿ�Ϊ�ҷ�ս���غ�
BOOL GameObject::IsSelfRound()
{
// 	if (!m_pBattle)
// 		return FALSE;
// 
// 	if (m_pBattle->GetCurChief() != GetChief())
// 		return FALSE;

	return TRUE;
}

//�Ƿ����ĳ��״̬
BOOL GameObject::HasStatus(GameObjectStatus enStatus)
{
// 	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
// 	return (dwState & enStatus) != 0;
	return FALSE;
}

//����ĳ��״̬
VOID GameObject::SetStatus(GameObjectStatus enStatus)
{
// 	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
// 	dwState |= enStatus;
// 	m_GameObjectAttr.SetBaseStatus(dwState);
}

//���ĳ��״̬
VOID GameObject::ClearStatus(GameObjectStatus enStatus)
{
// 	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
// 	dwState &= ~enStatus;
// 	m_GameObjectAttr.SetBaseStatus(dwState);
}

//������ӳ�HP
VOID GameObject::AddChiefHp(INT nVal)
{
// 	if (!nVal)
// 		return;
// 
// 	Chief* pChief = GetChief();
// 	if (!pChief)
// 		return;
// 
// 	//����
// 	if (nVal > 0)
// 	{
// 		//������������Hp
// 		if (pChief->GetHp() + nVal < pChief->GetMaxHp())
// 			pChief->SetHp(pChief->GetHp() + nVal);
// 		else
// 			pChief->SetHp(pChief->GetMaxHp());
// 	}
// 	//�˺�
// 	else
// 	{
// 		//�����˺��۳�Hp
// 		if (pChief->GetHp() > (DWORD)abs(nVal))
// 			pChief->SetHp(pChief->GetHp() + nVal);
// 		else
// 			pChief->SetDead(NULL);
// 	}
// 
// 	//ͬ���ͻ��˸���Hp����
// 	pChief->SendUpdateAttrCmd();
}

//���з�����ӳ�HP
VOID GameObject::AddEnemyChiefHp(INT nVal)
{
// 	if (!nVal)
// 		return;
// 
// 	Chief* pChief = GetChief();
// 	if (!pChief)
// 		return;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return;
// 
// 	//����
// 	if (nVal > 0)
// 	{
// 		//������������Hp
// 		if (pEnemyChief->GetHp() + nVal < pEnemyChief->GetMaxHp())
// 			pEnemyChief->SetHp(pEnemyChief->GetHp() + nVal);
// 		else
// 			pEnemyChief->SetHp(pEnemyChief->GetMaxHp());
// 	}
// 	//�˺�
// 	else
// 	{
// 		//�����˺��۳�Hp
// 		if (pEnemyChief->GetHp() > (DWORD)abs(nVal))
// 			pEnemyChief->SetHp(pEnemyChief->GetHp() + nVal);
// 		else
// 			pEnemyChief->SetDead(NULL);
// 	}
// 
// 	//ͬ���ͻ��˸���Hp����
// 	pEnemyChief->SendUpdateAttrCmd();
}

//����ս������
VOID GameObject::SetEnv(DWORD dwEnv)
{
// 	if (m_pBattle)
// 	{
// 		if (m_pBattle->SetEnv(GetObjectKey(), dwEnv))
// 			TriggerBattleEvent(BATTLE_EVENT_ENV_CHANGE, this, NULL);
// 	}
}

//���ս������
VOID GameObject::ClearEnv()
{
// 	if (m_pBattle)
// 	{
// 		if (m_pBattle->ClearEnv(GetObjectKey()))
// 			TriggerBattleEvent(BATTLE_EVENT_ENV_CHANGE, this, NULL);
// 	}
}

//��������
VOID GameObject::DiscardHandCard(BYTE byCardNum)
{
// 	Chief* pChief = GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->DiscardHandCard(byCardNum);
}

//�ͷŷ���
ERROR_CODE GameObject::CastSpell(DWORD dwSpellID, DWORD* pdwTargetKey, BYTE byTargetNum, BOOL bAutoComplete)
{
	return  ERROR_CODE_OK ;// m_SpellManager.CastSpell(dwSpellID, pdwTargetKey, byTargetNum, bAutoComplete);
}

//���ʩ�ŷ���
VOID GameObject::CompleteCastSpell()
{
	//m_SpellManager.CompleteCastSpell();
}

//���ӹ⻷
BOOL GameObject::AddAura(DWORD dwAuraID)
{
	return TRUE; // m_SpellManager.AddAura(dwAuraID);
}

BOOL GameObject::SendPacket(NetMsgBody* pMsg)
{
	if (!m_pOwner)
		return FALSE;

	return m_pOwner->SendPacket(pMsg);
}

BOOL GameObject::SendPacketToBoth(NetMsgBody* pMsg)
{
// 	Chief* pChief = GetChief();
// 	if (!pChief)
		return FALSE;

// 	//�˴�����ʹ���Լ�������Ϣ,��Ϊ����ķ���Ч�����޸��������ָ��
// 	if (!pChief->SendPacket(pMsg))
// 		return FALSE;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return FALSE;
// 
// 	return pEnemyChief->SendPacket(pMsg);
}

//���͸���������Ϣ
VOID GameObject::SendUpdateAttrCmd()
{
// 	if (!m_GameObjectAttr.GetUpdateFlag())
// 		return;

	//�趨��Ϣ������
// 	MSG_M2C_GAME_OBJECT_ATTR_CMD msg;
// 	BYTE byCount = (BYTE)m_GameObjectAttr.GetAttrNum();
// 	BitSet* pMsk = m_GameObjectAttr.GetAttrMask();
// 
// 	msg.m_dwObjectKey = GetObjectKey();
// 	msg.m_byCount = 0;
// 	for (BYTE i = 0; i < byCount && i < GOAT_MAX; ++i)
// 	{
// 		if(pMsk->GetBit(i))
// 		{
// 			msg.m_tAttrInfo[msg.m_byCount].m_dwVal = m_GameObjectAttr[i][eATTR_KIND_CALC];
// 			msg.m_tAttrInfo[msg.m_byCount].m_byIndex = i;
// 			++msg.m_byCount;
// 		}
// 	}

	//������������
// 	m_GameObjectAttr.ClearMask();
// 
// 	//������Ϣ
// 	SendPacketToBoth(&msg);
}

//����ս���¼�
// VOID GameObject::TriggerBattleEvent(BattleEvent enBattleEvent, GameObject* pTrigger, GameObject* pPara)
// {
// // 	if (!pTrigger)
// // 		return;
// // 
// // 	//�ҷ���Ӧ�¼�����
// // 	Chief* pChief = pTrigger->GetChief();
// // 	if (!pChief)
// // 		return;
// // 
// // 	pChief->ProcessBattleEvent(enBattleEvent, pTrigger, pPara);
// // 
// // 	//�з���Ӧ�¼�����
// // 	Chief* pEnemyChief = pChief->GetEnemyChief();
// // 	if (!pEnemyChief)
// // 		return;
// // 
// // 	pEnemyChief->ProcessBattleEvent(enBattleEvent, pTrigger, pPara, FALSE);
// }