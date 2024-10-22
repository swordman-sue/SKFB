#pragma once
#include "GameObject.h"
#include "../Common/DBCDefine.h"

class Chief;

//随从类
class Retinue : public GameObject
{
public:
	Retinue();
	~Retinue();

	BOOL					Init(Robot* pRobot, Chief* pChief, RetinueEntry* pRetinueEntry, BYTE byOrien);
	VOID					Release();
	VOID					Update(DWORD dwTick);

	BOOL					InitAttr(BYTE byOrien);
	Chief*					GetChief() { return m_pChief; }
	VOID					ChangeChief(Chief* pChief) { if (pChief) m_pChief = pChief; }

	BYTE					GetColor() const { return (m_pRetinueEntry ? (BYTE)m_pRetinueEntry->m_dwRetinueColor : 0); }
	BYTE					GetLv() const { return (m_pRetinueEntry ? (BYTE)m_pRetinueEntry->m_dwRetinueLv : 0); }
	BOOL					IsBattleRetinue() { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueType == RETINUE_TYPE_BATTLE : FALSE); }
	BOOL					IsMagicRetinue() { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueType == RETINUE_TYPE_MAGIC : FALSE); }
	BOOL					IsSpellRetinue() { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueSubType == MAGIC_RETINUE_TYPE_SPELL : FALSE); }
	BOOL					IsTrapRetinue() { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueSubType == MAGIC_RETINUE_TYPE_TRAP : FALSE); }
	BOOL					IsEquipmentRetinue() { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueSubType == MAGIC_RETINUE_TYPE_EQUIPMENT : FALSE); }
	BOOL					IsEnvRetinue() { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueSubType == MAGIC_RETINUE_TYPE_ENV : FALSE); }
	BOOL					IsEnterGrave() { return (m_pRetinueEntry ? (BOOL)m_pRetinueEntry->m_dwEnterGrave : FALSE); }
	BOOL					IsDestroyByTrigger() { return (m_pRetinueEntry ? (BOOL)m_pRetinueEntry->m_dwIsDestroyByTrigger : FALSE); }
	DWORD					GetRetinueID() const { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwRetinueID : 0); }
	DWORD					GetReitnueCardID() const { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwCardID : 0); }
	DWORD					GetCardID() const { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwCardID : 0); }
	BYTE					GetRetinueType() const { return (m_pRetinueEntry ? (BYTE)m_pRetinueEntry->m_dwRetinueType : 0); }
	BYTE					GetRetinueSubType() const { return (m_pRetinueEntry ? (BYTE)m_pRetinueEntry->m_dwRetinueSubType : 0); }
	DWORD					GetAttackSpellID() const { return (m_pRetinueEntry ? m_pRetinueEntry->m_dwAttackSpellID : 0); }
	DWORD					GetSacrificeNum() const;
	BOOL					IsEnableAttack();

	static RetinueEntry*	GetRetinueEntry(DWORD dwRetinueID);

private:
	Chief*					m_pChief;				//所属首领
	DWORD					m_dwRetinueID;			//随从ID
	RetinueEntry*			m_pRetinueEntry;		//随从配置文件
};