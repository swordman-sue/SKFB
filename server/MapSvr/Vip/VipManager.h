/**\file
Vip管理器
\author Kevin
\date 2016/10/22
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

class CVipManager
{
public:
	CVipManager();
	~CVipManager();

	void Init(Player *pPlayer);

public:

	// 添加VIP经验
	void AddExp(DWORD dwExp);

	// 取VIP特权值
	int GetValue(BYTE byType);

private:
	Player *m_pPlayer;
};