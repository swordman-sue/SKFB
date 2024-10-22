/**\file
限时产出逻辑类
\author Kevin
\date 2018/03/23
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;


class CLimitProduct
{
public:
	CLimitProduct(){};
	~CLimitProduct(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:

	// 活动事件
	void OnNetActivityEvent();


private:
	Player *m_pPlayer;
	LimitActivityData *m_pLimitActivityData;

	// 限时活动
	CActivityManager *m_pActivityMgr;
};