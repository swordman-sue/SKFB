#pragma once
#include "Object.h"
//#include "Attributes.h"
#include "../Common/ErrorCode.h"

class Chief;
class Character;
class NetMsgBody;
class Battle;

//游戏对象类
class GameObject : public Object
{
public:
	GameObject(void);
	virtual ~GameObject(void);
	
	virtual BOOL			Init(Battle* pBattle, Character* pCharacter);
	virtual VOID			Release();
	virtual VOID			Update(DWORD dwTick);

	//对象相关
	virtual Chief*			GetChief() = 0;
	Character*				GetOwner() const { return m_pOwner; }
	Battle*					GetBattle() const { return NULL; }
	DWORD					GetChiefObjectKey();
	VOID					AddToGameObjectMap(GameObject* pGameObject);
	VOID					RemoveFromGameObjectMap(GameObject* pGameObject);
	GameObject*				FindFromGameObjectMap(DWORD dwObjectKey);
	virtual DWORD			GetRetinueID() const { return 0; }
	virtual DWORD			GetReitnueCardID() const { return 0; }
	virtual BYTE			GetRetinueType() const { return 0; }
	virtual BYTE			GetRetinueSubType() const { return 0; }
	virtual DWORD			GetCardID() const { return 0; }
	virtual BYTE			GetCardType() const { return 0; }
	virtual BYTE			GetCardSubType() const { return 0; }
	virtual VOID			SetMurdererKey(DWORD dwKey) { }
	VOID					SetDead(GameObject* pMurderer);
	BOOL					IsConformCond(BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//属性相关
	//DWORD					GetRace() const { return m_GameObjectAttr.GetRace(); }
	virtual BYTE			GetColor() const { return 0; }
	virtual BYTE			GetLevel() const { return 0; }
	virtual BOOL			IsBattleRetinue() { return FALSE; }
	virtual BOOL			IsMagicRetinue() { return FALSE; }
	virtual BOOL			IsSpellRetinue() { return FALSE; }
	virtual BOOL			IsTrapRetinue() { return FALSE; }
	virtual BOOL			IsEquipmentRetinue() { return FALSE; }
	virtual BOOL			IsEnvRetinue() { return FALSE; }
	virtual BOOL			IsDestroyByTrigger() { return FALSE; }
	//BOOL					IsAlive() { return (m_GameObjectAttr.GetHp() > 0); }
//	BOOL					IsDead() { return !IsAlive(); }
	BOOL					IsFriend(GameObject* pGameObject) { return (pGameObject ? GetChief() == pGameObject->GetChief() : FALSE); }
	BOOL					IsSelf(GameObject* pGameObject) { return (this == pGameObject); }
	INT						GetAttr(DWORD dwAttrType, DWORD dwAttrKind) const;
	VOID					SetAttr(DWORD dwAttrType, DWORD dwAttrKind, INT nValue);
	VOID					AddAttr(DWORD dwAttrType, DWORD dwAttrKind, int nValue);
	VOID					ClearAttr(DWORD dwAttrType);
	BOOL					IsSelfRound();

// 	DWORD					GetMaxHp() const { return m_GameObjectAttr.GetMaxHp(); }
// 	VOID					SetMaxHp(DWORD dwMaxHp) { m_GameObjectAttr.SetBaseMaxHp(dwMaxHp); }
// 	DWORD					GetHp() const { return m_GameObjectAttr.GetHp(); }
// 	VOID					SetHp(DWORD dwHp) { m_GameObjectAttr.SetBaseHp(dwHp); }
// 	DWORD					GetAttackPower() const { return m_GameObjectAttr.GetAttackPower(); }
// 	VOID					SetAttackPower(DWORD dwAttackPower) { m_GameObjectAttr.SetBaseAttackPower(dwAttackPower); }
// 	DWORD					GetDefensePower() const { return m_GameObjectAttr.GetDefensePower(); }
// 	VOID					SetDefensePower(DWORD dwDefensePower) { m_GameObjectAttr.SetBaseDefensePower(dwDefensePower); }
// 	DWORD					GetAction() const { return m_GameObjectAttr.GetAction(); }
// 	VOID					SetAction(DWORD dwAction) { m_GameObjectAttr.SetBaseAction(dwAction); }
// 	DWORD					GetOrien() const { return m_GameObjectAttr.GetOrien(); }
// 	VOID					SetOrien(DWORD dwOrien) { m_GameObjectAttr.SetBaseOrien(dwOrien); }
	BOOL					HasStatus(GameObjectStatus enStatus);
	VOID					SetStatus(GameObjectStatus enStatus);
	VOID					ClearStatus(GameObjectStatus enStatus);
// 	DWORD					GetSummonTimes() const { return m_GameObjectAttr.GetSummonTimes(); }
// 	VOID					SetSummonTimes(DWORD dwTimes) { m_GameObjectAttr.SetBaseSummonTimes(dwTimes); }
	virtual GameObject*		GetBindObject() { return NULL; }
	virtual VOID			SetBindObject(GameObject* pGameObject) {}
	virtual VOID			ClearBindObject(GameObject* pGameObject, BOOL bSync) {}
	virtual BOOL			IsTrigger() { return FALSE; }
	virtual VOID			SetTrigger() {}
	virtual VOID			TriggerTrap() {}
	virtual INT				GetScriptData(BYTE byIndex) const { return 0; }
	virtual VOID			SetScriptData(BYTE byIndex, INT nVal) {}
	VOID					AddChiefHp(INT nVal);
	VOID					AddEnemyChiefHp(INT nVal);
	DWORD					GetEnv() const { return 0; }
	VOID					SetEnv(DWORD dwEnv);
	VOID					ClearEnv();

	//脚本相关
	virtual DWORD			GetSacrificeNum() const { return 0; }
	VOID					DiscardHandCard(BYTE byCardNum);

	//法术相关
	ERROR_CODE				CastSpell(DWORD dwSpellID, DWORD* pdwTargetKey = NULL, BYTE byTargetNum = 0, BOOL bAutoComplete = TRUE);
	VOID					CompleteCastSpell();
	BOOL					AddAura(DWORD dwAuraID);

	//消息相关
	BOOL					SendPacket(NetMsgBody* pMsg);
	BOOL					SendPacketToBoth(NetMsgBody* pMsg);
	VOID					SendUpdateAttrCmd();

	//事件相关
	//VOID					TriggerBattleEvent(BattleEvent enBattleEvent, GameObject* pTrigger, GameObject* pPara);
	virtual VOID			OnBattleRoundStart() {}
	virtual VOID			OnBattleRoundEnd() { /*m_SpellManager.OnRoundFinish();*/ }
	virtual VOID			OnDead() {}
	virtual VOID			OnTurn(BOOL bIsTriggerScript = TRUE) {}
	virtual VOID			OnCover(BOOL bIsTriggerScript = TRUE) {}

protected:
	Character*				m_pOwner;			//所属角色
	//Battle*					m_pBattle;			//所属战斗
	//GameObjectAttr			m_GameObjectAttr;	//游戏对象属性
	//SpellManager			m_SpellManager;		//法术管理器
};
