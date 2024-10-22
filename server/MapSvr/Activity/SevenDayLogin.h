/**\file
7天登录逻辑类
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
	// 请求7天登录数据
	void OnNetSevenDayLoginData();

	// 领取7天登录奖励
	void OnNetTakeSevenDayLoginReward(WORD wDay);

protected:

	// 发送7天登录数据
	void SendSevenDayLoginData();

private:
	Player *m_pPlayer;
	SevenDayLoginData *m_pSevenDayLoginData;
	CActivityManager *m_pActivityMgr;
};