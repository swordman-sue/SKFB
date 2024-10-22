#pragma once
#include "GameObject.h"
#include "../Common/DBCDefine.h"

class Chief;

class Card : public GameObject
{
public:
	Card();
	~Card();

	BOOL		Init(Robot* pRobot, Chief* pChief, DWORD dwCardID);
	VOID		Release();
	VOID		Update(DWORD dwTick);

	//属性相关
	BOOL		InitAttr();
	Chief*		GetChief() { return m_pChief; }
	VOID		ChangeChief(Chief* pChief) { if (pChief) m_pChief = pChief; }
	BYTE		GetRace() const { return (m_pCardEntry ? (BYTE)m_pCardEntry->m_dwCardRace : 0); }
	BYTE		GetColor() const { return (m_pCardEntry ? (BYTE)m_pCardEntry->m_dwCardColor : 0); }
	BYTE		GetLv() const { return (m_pCardEntry ? (BYTE)m_pCardEntry->m_dwCardLv : 0); }
	DWORD		GetCardID() const { return (m_pCardEntry ? m_pCardEntry->m_dwCardID : 0); }
	BYTE		GetCardType() const { return (m_pCardEntry ? (BYTE)m_pCardEntry->m_dwCardType : 0); }
	BYTE		GetCardSubType() const { return (m_pCardEntry ? (BYTE)m_pCardEntry->m_dwCardSubType : 0); }
	DWORD		GetSummonRetinueID() const { return (m_pCardEntry ? m_pCardEntry->m_dwSummonRetinueID : 0); }
	DWORD		GetSacrificeNum() const { return (m_pCardEntry ? m_pCardEntry->m_dwSacrificeNum : 0); }
	DWORD		GetSacrificeRetinueID() const { return (m_pCardEntry ? m_pCardEntry->m_dwSacrificeRetinueID : 0); }
	DWORD		GetSacrificeRetinueRace() const { return (m_pCardEntry ? m_pCardEntry->m_dwSacrificeRetinueRace : 0); }
	DWORD		GetSpellID() const { return (m_pCardEntry ? m_pCardEntry->m_dwSpellID : 0); }
	BOOL		IsEnterGrave() { return (m_pCardEntry ? (BOOL)m_pCardEntry->m_IsEnterGrave : FALSE); }
	BOOL		IsRushCard() { return (m_pCardEntry ? (BOOL)m_pCardEntry->m_IsRushCard : FALSE); }
	DWORD		GetTargetType() const { return (m_pCardEntry ? m_pCardEntry->m_dwTargetType : 0); }
	CardEntry*	GetCardEntry() const { return m_pCardEntry; }

private:
	Chief*		m_pChief;			//所属首领
	DWORD		m_dwCardID;			//卡牌ID
	CardEntry*	m_pCardEntry;		//卡牌配置文件
};