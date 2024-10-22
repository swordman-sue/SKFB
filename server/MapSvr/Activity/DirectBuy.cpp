#include "stdafx.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\ConfigManager.h"
#include "DirectBuy.h"

void CDirectBuy::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pActivityMgr = pActivityMgr;
	m_pPlayer = pPlayer;
	//m_pSuperDiscountData = &pPlayer->GetRoleFullInfo().tActivityInfo.tSuperDiscountData;
}