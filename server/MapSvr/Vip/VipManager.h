/**\file
Vip������
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

	// ���VIP����
	void AddExp(DWORD dwExp);

	// ȡVIP��Ȩֵ
	int GetValue(BYTE byType);

private:
	Player *m_pPlayer;
};