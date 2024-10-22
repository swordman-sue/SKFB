#include "StdAfx.h"
#include "GameObject.h"
#include "Character.h"
#include "../Common/MsgDefine.h"

GameObject::GameObject(void)
{
	m_pOwner	= NULL;
	//m_pBattle	= NULL;

	//注册游戏对象属性
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

	//初始化游戏对象属性
	//m_GameObjectAttr.Init();

	//初始化法术管理器
// 	if (!m_SpellManager.Init(this))
// 		return FALSE;

	//最后调用基类的函数
	return Object::Init();
}

VOID GameObject::Release()
{
	m_pOwner	= NULL;
	//m_pBattle	= NULL;

	//m_SpellManager.Release();

	//最后调用基类的函数
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

//设置死亡
VOID GameObject::SetDead(GameObject* pMurderer)
{
// 	if (IsDead())
// 		return;
// 
// 	//设置对象Hp为0
// 	SetHp(0);

	//设置凶手Key
	if (pMurderer)
		SetMurdererKey(pMurderer->GetObjectKey());


	//同步客户端更新属性
	//SendUpdateAttrCmd();

	////触发相关事件
	//if (GetObjectType() == OBJECT_RETINUE)
	//{
	//	//触发杀死目标事件
	//	if (pMurderer && pMurderer->GetObjectType() == OBJECT_RETINUE)
	//		pMurderer->TriggerBattleEvent(BATTLE_EVENT_KILL, pMurderer, this);

	//	//触发目标死亡事件
	//	if (pMurderer)
	//		TriggerBattleEvent(BATTLE_EVENT_DEAD, this, pMurderer);
	//}

	////查找绑定目标
	//GameObject* pGameObject = GetBindObject();
	//if (pGameObject)
	//{
	//	//解除绑定
	//	pGameObject->ClearBind();

	//	//解除绑定
	//	ClearBind();

	//	//同步客户端更新属性
	//	SendUpdateAttrCmd();
	//}

	////最后处理绑定的装备随从死亡(避免SetDead函数互相调用死循环)
	//if (pGameObject && pGameObject->IsEquipmentRetinue())
	//	pGameObject->SetDead(NULL);
}

//检测对象是否满足指定的条件
BOOL GameObject::IsConformCond(BYTE byCondType, BYTE byCondSign, DWORD dwCondVal)
{
	return TRUE;

// 	if (byCondType < STC_NONE || byCondType >= STC_MAX)
// 		return FALSE;
// 
// 	if (byCondSign < STCS_NONE || byCondSign >= STCS_MAX)
// 		return FALSE;
// 
// 	//按照条件类型,获得条件数值
// 	DWORD dwVal = 0;
// 	switch((SpellTargetCond)byCondType)
// 	{
// 		//种族
// 	case STC_RACE:
// 		{
// 			dwVal = GetRace();
// 		}
// 		break;
// 
// 		//等级
// 	case STC_LV:
// 		{
// 			dwVal = GetLevel();
// 		}
// 		break;
// 
// 		//攻击力
// 	case STC_AP:
// 		{
// 			dwVal = GetAttackPower();
// 		}
// 		break;
// 
// 		//防御力
// 	case STC_DP:
// 		{
// 			dwVal = GetDefensePower();
// 		}
// 		break;
// 
// 		//随从ID
// 	case STC_RETINUE_ID:
// 		{
// 			dwVal = GetRetinueID();
// 		}
// 		break;
// 
// 		//随从类型
// 	case STC_RETINUE_TYPE:
// 		{
// 			dwVal = GetRetinueType();
// 		}
// 		break;
// 
// 		//随从子类型
// 	case STC_RETINUE_SUB_TYPE:
// 		{
// 			dwVal = GetRetinueSubType();
// 		}
// 		break;
// 
// 		//卡牌ID
// 	case STC_CARD_ID:
// 		{
// 			dwVal = GetCardID();
// 		}
// 		break;
// 
// 		//卡牌类型
// 	case STC_CARD_TYPE:
// 		{
// 			dwVal = GetCardType();
// 		}
// 		break;
// 
// 		//卡牌子类型
// 	case STC_CARD_SUB_TYPE:
// 		{
// 			dwVal = GetCardSubType();
// 		}
// 		break;
// 
// 		//对象颜色
// 	case STC_COLOR:
// 		{
// 			dwVal = GetColor();
// 		}
// 		break;
// 
// 		//献祭随从数量
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
// 	//检测条件数值为0肯定不满足条件
// 	if (dwVal == 0)
// 		return FALSE;
// 
// 	//检测条件
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

//获得属性值
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

//设置属性值
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

//增加属性值
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

//修改属性值
VOID GameObject::ClearAttr(DWORD dwAttrType)
{
// 	if (dwAttrType >= GOAT_MAX)
// 		return;
// 
// 	m_GameObjectAttr.ClearAttrVal(dwAttrType);
}

//检测是否为我方战斗回合
BOOL GameObject::IsSelfRound()
{
// 	if (!m_pBattle)
// 		return FALSE;
// 
// 	if (m_pBattle->GetCurChief() != GetChief())
// 		return FALSE;

	return TRUE;
}

//是否存在某种状态
BOOL GameObject::HasStatus(GameObjectStatus enStatus)
{
// 	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
// 	return (dwState & enStatus) != 0;
	return FALSE;
}

//设置某种状态
VOID GameObject::SetStatus(GameObjectStatus enStatus)
{
// 	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
// 	dwState |= enStatus;
// 	m_GameObjectAttr.SetBaseStatus(dwState);
}

//清除某种状态
VOID GameObject::ClearStatus(GameObjectStatus enStatus)
{
// 	DWORD dwState = (DWORD)m_GameObjectAttr.GetBaseStatus();
// 	dwState &= ~enStatus;
// 	m_GameObjectAttr.SetBaseStatus(dwState);
}

//给首领加成HP
VOID GameObject::AddChiefHp(INT nVal)
{
// 	if (!nVal)
// 		return;
// 
// 	Chief* pChief = GetChief();
// 	if (!pChief)
// 		return;
// 
// 	//治疗
// 	if (nVal > 0)
// 	{
// 		//根据治疗增加Hp
// 		if (pChief->GetHp() + nVal < pChief->GetMaxHp())
// 			pChief->SetHp(pChief->GetHp() + nVal);
// 		else
// 			pChief->SetHp(pChief->GetMaxHp());
// 	}
// 	//伤害
// 	else
// 	{
// 		//根据伤害扣除Hp
// 		if (pChief->GetHp() > (DWORD)abs(nVal))
// 			pChief->SetHp(pChief->GetHp() + nVal);
// 		else
// 			pChief->SetDead(NULL);
// 	}
// 
// 	//同步客户端更新Hp属性
// 	pChief->SendUpdateAttrCmd();
}

//给敌方首领加成HP
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
// 	//治疗
// 	if (nVal > 0)
// 	{
// 		//根据治疗增加Hp
// 		if (pEnemyChief->GetHp() + nVal < pEnemyChief->GetMaxHp())
// 			pEnemyChief->SetHp(pEnemyChief->GetHp() + nVal);
// 		else
// 			pEnemyChief->SetHp(pEnemyChief->GetMaxHp());
// 	}
// 	//伤害
// 	else
// 	{
// 		//根据伤害扣除Hp
// 		if (pEnemyChief->GetHp() > (DWORD)abs(nVal))
// 			pEnemyChief->SetHp(pEnemyChief->GetHp() + nVal);
// 		else
// 			pEnemyChief->SetDead(NULL);
// 	}
// 
// 	//同步客户端更新Hp属性
// 	pEnemyChief->SendUpdateAttrCmd();
}

//设置战场环境
VOID GameObject::SetEnv(DWORD dwEnv)
{
// 	if (m_pBattle)
// 	{
// 		if (m_pBattle->SetEnv(GetObjectKey(), dwEnv))
// 			TriggerBattleEvent(BATTLE_EVENT_ENV_CHANGE, this, NULL);
// 	}
}

//清除战场环境
VOID GameObject::ClearEnv()
{
// 	if (m_pBattle)
// 	{
// 		if (m_pBattle->ClearEnv(GetObjectKey()))
// 			TriggerBattleEvent(BATTLE_EVENT_ENV_CHANGE, this, NULL);
// 	}
}

//丢弃手牌
VOID GameObject::DiscardHandCard(BYTE byCardNum)
{
// 	Chief* pChief = GetChief();
// 	if (!pChief)
// 		return;
// 
// 	pChief->DiscardHandCard(byCardNum);
}

//释放法术
ERROR_CODE GameObject::CastSpell(DWORD dwSpellID, DWORD* pdwTargetKey, BYTE byTargetNum, BOOL bAutoComplete)
{
	return  ERROR_CODE_OK ;// m_SpellManager.CastSpell(dwSpellID, pdwTargetKey, byTargetNum, bAutoComplete);
}

//完成施放法术
VOID GameObject::CompleteCastSpell()
{
	//m_SpellManager.CompleteCastSpell();
}

//附加光环
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

// 	//此处不能使用自己发送消息,因为特殊的法术效果会修改首领对象指针
// 	if (!pChief->SendPacket(pMsg))
// 		return FALSE;
// 
// 	Chief* pEnemyChief = pChief->GetEnemyChief();
// 	if (!pEnemyChief)
// 		return FALSE;
// 
// 	return pEnemyChief->SendPacket(pMsg);
}

//发送更新属性消息
VOID GameObject::SendUpdateAttrCmd()
{
// 	if (!m_GameObjectAttr.GetUpdateFlag())
// 		return;

	//设定消息包数据
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

	//重置属性掩码
// 	m_GameObjectAttr.ClearMask();
// 
// 	//发送消息
// 	SendPacketToBoth(&msg);
}

//触发战斗事件
// VOID GameObject::TriggerBattleEvent(BattleEvent enBattleEvent, GameObject* pTrigger, GameObject* pPara)
// {
// // 	if (!pTrigger)
// // 		return;
// // 
// // 	//我方相应事件处理
// // 	Chief* pChief = pTrigger->GetChief();
// // 	if (!pChief)
// // 		return;
// // 
// // 	pChief->ProcessBattleEvent(enBattleEvent, pTrigger, pPara);
// // 
// // 	//敌方相应事件处理
// // 	Chief* pEnemyChief = pChief->GetEnemyChief();
// // 	if (!pEnemyChief)
// // 		return;
// // 
// // 	pEnemyChief->ProcessBattleEvent(enBattleEvent, pTrigger, pPara, FALSE);
// }