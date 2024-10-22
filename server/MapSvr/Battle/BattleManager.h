/**\file
ս��������
\author Kevin
\date 2016/7/21
\note
*/

#pragma once
#include "../ToolKit/Singleton.h"
class Player;
struct HeroInfo;

class CBattleManager : public Singleton<CBattleManager>
{
public:
	CBattleManager();
	~CBattleManager();

public:
	// ս����ʼ
	void OnBattleStar(Player *pPlayer, BYTE byType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, 
		DWORD dwParam4, DWORD dwPower, const char *pSign);

	// ս������
	void OnBattleOver(Player *pPlayer, BYTE byType, int nWinValue, BYTE byStar, DWORD dwContext, DWORD dwContext1,
		MonsterData *pMonsterList, BYTE byMonsterNum, DWORD dwDamage, DWORD dwPower, const char *pSign);
};