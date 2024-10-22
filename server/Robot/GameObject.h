#pragma once

#include "../Common/Const.h"
//#include "../MapSvr/Attributes.h"

class Robot;
class Chief;

class GameObject
{
public:
	GameObject(void);
	virtual ~GameObject(void);

public:
	virtual BOOL		Init(Robot* pRobot);
	virtual VOID		Release();
	virtual VOID		Update( DWORD dwDeltaTick );

	DWORD				GetObjectKey() const { return m_dwObjectKey; }
	VOID				SetObjectKey(DWORD dwObjectKey) { m_dwObjectKey = dwObjectKey; }
	OBJECT_TYPE			GetObjectType() const { return m_enObjectType; }
	VOID				SetObjectType(OBJECT_TYPE enType) { m_enObjectType = enType; }

	//对象相关
	virtual Chief*		GetChief() = 0;
	Robot*				GetOwner() const { return m_pOwner; }
	DWORD				GetChiefObjectKey();
	virtual DWORD		GetRetinueID() const { return 0; }
	virtual DWORD		GetReitnueCardID() const { return 0; }
	virtual BYTE		GetRetinueType() const { return 0; }
	virtual BYTE		GetRetinueSubType() const { return 0; }
	virtual DWORD		GetCardID() const { return 0; }
	virtual BYTE		GetCardType() const { return 0; }
	virtual BYTE		GetCardSubType() const { return 0; }

	//属性相关
//	DWORD				GetRace() const { return m_GameObjectAttr.GetRace(); }
	virtual BYTE		GetColor() const { return 0; }
	virtual BYTE		GetLv() const { return 0; }
	virtual BOOL		IsBattleRetinue() { return FALSE; }
	virtual BOOL		IsMagicRetinue() { return FALSE; }
	virtual BOOL		IsSpellRetinue() { return FALSE; }
	virtual BOOL		IsTrapRetinue() { return FALSE; }
	virtual BOOL		IsEquipmentRetinue() { return FALSE; }
	virtual BOOL		IsEnvRetinue() { return FALSE; }
	virtual BOOL		IsDestroyByTrigger() { return FALSE; }
//	BOOL				IsAlive() { return (m_GameObjectAttr.GetHp() > 0); }
//	BOOL				IsDead() { return !IsAlive(); }
	BOOL				IsFriend(GameObject* pGameObject) { return (pGameObject ? GetChief() == pGameObject->GetChief() : FALSE); }
	BOOL				IsSelf(GameObject* pGameObject) { return (this == pGameObject); }
	INT					GetAttr(DWORD dwAttrType, DWORD dwAttrKind) const;
	VOID				SetAttr(DWORD dwAttrType, DWORD dwAttrKind, INT nValue);
	VOID				AddAttr(DWORD dwAttrType, DWORD dwAttrKind, int nValue);
	VOID				ClearAttr(DWORD dwAttrType);

// 	DWORD				GetMaxHp() const { return m_GameObjectAttr.GetMaxHp(); }
// 	VOID				SetMaxHp(DWORD dwMaxHp) { m_GameObjectAttr.SetBaseMaxHp(dwMaxHp); }
// 	DWORD				GetHp() const { return m_GameObjectAttr.GetHp(); }
// 	VOID				SetHp(DWORD dwHp) { m_GameObjectAttr.SetBaseHp(dwHp); }
// 	DWORD				GetAttackPower() const { return m_GameObjectAttr.GetAttackPower(); }
// 	VOID				SetAttackPower(DWORD dwAttackPower) { m_GameObjectAttr.SetBaseAttackPower(dwAttackPower); }
// 	DWORD				GetDefensePower() const { return m_GameObjectAttr.GetDefensePower(); }
// 	VOID				SetDefensePower(DWORD dwDefensePower) { m_GameObjectAttr.SetBaseDefensePower(dwDefensePower); }
// 	DWORD				GetAction() const { return m_GameObjectAttr.GetAction(); }
// 	VOID				SetAction(DWORD dwAction) { m_GameObjectAttr.SetBaseAction(dwAction); }
// 	DWORD				GetOrien() const { return m_GameObjectAttr.GetOrien(); }
// 	VOID				SetOrien(DWORD dwOrien) { m_GameObjectAttr.SetBaseOrien(dwOrien); }
// 	BOOL				HasStatus(GameObjectStatus enStatus);
// 	VOID				SetStatus(GameObjectStatus enStatus);
// 	VOID				ClearStatus(GameObjectStatus enStatus);
// 	DWORD				GetSummonTimes() const { return m_GameObjectAttr.GetSummonTimes(); }
// 	VOID				SetSummonTimes(DWORD dwTimes) { m_GameObjectAttr.SetBaseSummonTimes(dwTimes); }

protected:
	Robot*				m_pOwner;				//所属机器人
	DWORD				m_dwObjectKey;			//游戏对象ObjectKey
	OBJECT_TYPE			m_enObjectType;			//游戏对象类型
//	GameObjectAttr		m_GameObjectAttr;		//游戏对象属性
};