/**\file
��ʱ�����߼���
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

	// ��¼�
	void OnNetActivityEvent();


private:
	Player *m_pPlayer;
	LimitActivityData *m_pLimitActivityData;

	// ��ʱ�
	CActivityManager *m_pActivityMgr;
};