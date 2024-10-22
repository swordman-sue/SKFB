#include "stdafx.h"
#include "BattleManager.h"
#include "../Common/MsgDefine.h"
#include "..\Player.h"
#include "..\Fuben\FubenManager.h"
#include "DataDefine.h"
#include "..\ConfigManager.h"

DWORD GetDamage()
{
	// �����˺� = max((����*���� / (���� + max(����*(1 - �ٷֱȴ�͸) - �̶�ֵ��͸, 0))*(1 + �˺��ӳ� - �ٷֱȼ���) + (�˺����� - �̶�ֵ����)), 0)*�����˺�ϵ��*�˺���������ֵ
	int nAttack = 0;
	int nDeff = 0;
	float fThroutRate = 0;
	float fStaticThrout = 0;
	float fDamageAdd = 0;
	float fDamageIncre = 0;
	float fSkillDamageRate = 0;
	float fDamageRepair = 0;
	int nBaseDamage = max((nAttack * nAttack / (nAttack + max(nDeff * (1 - fThroutRate) - fStaticThrout, 0)) * (1 + fDamageAdd - fDamageIncre) + (1 - 1)), 0);
	int nDamage = nBaseDamage * fSkillDamageRate * fDamageRepair;
	return nDamage;
}

CBattleManager::CBattleManager()
{

}

CBattleManager::~CBattleManager()
{

}


// ս����ʼ
void CBattleManager::OnBattleStar(Player *pPlayer, BYTE byType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, 
	DWORD dwParam4, DWORD dwPower, const char *pSign)
{
	if (pPlayer->GetFubenManager())
		pPlayer->GetFubenManager()->OnBattleStart(byType, dwParam1, dwParam2, dwParam3, dwParam4, dwPower, pSign);
}

// ս������
void CBattleManager::OnBattleOver(Player *pPlayer, BYTE byType, int nWinValue, BYTE byStar, DWORD dwContext, DWORD dwContext1,
	MonsterData *pMonsterList, BYTE byMonsterNum, DWORD dwDamage, DWORD dwPower, const char *pSign)
{
	if (pPlayer->GetFubenManager())
		pPlayer->GetFubenManager()->OnBattleOver(byType, nWinValue, byStar, dwContext, dwContext1, pMonsterList, byMonsterNum, dwDamage, dwPower, pSign);
}