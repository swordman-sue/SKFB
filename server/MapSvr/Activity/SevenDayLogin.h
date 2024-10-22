/**\file
7���¼�߼���
\author Kevin
\date 2018/1/23
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CSevenDayLogin
{
public:
	CSevenDayLogin(){};
	~CSevenDayLogin(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	void OnLogin();

public:
	// ����7���¼����
	void OnNetSevenDayLoginData();

	// ��ȡ7���¼����
	void OnNetTakeSevenDayLoginReward(WORD wDay);

protected:

	// ����7���¼����
	void SendSevenDayLoginData();

private:
	Player *m_pPlayer;
	SevenDayLoginData *m_pSevenDayLoginData;
	CActivityManager *m_pActivityMgr;
};