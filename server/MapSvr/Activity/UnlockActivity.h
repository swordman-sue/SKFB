/**\file
unlock��߼���
\author Kevin
\date 2018/04/28
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CUnlockActivity
{
public:
	CUnlockActivity();
	~CUnlockActivity();

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);


public:
	// ��������
	void OnNetActivityData();

	// �̳��������
	void OnNetShopScoreFinish();

	// �ۿ���Ƶ֪ͨ
	void OnNetWatchingVideoNotify();

protected:

	// ���ͻ����
	void SendActivityData();

protected:
	Player *m_pPlayer;
	CActivityManager *m_pActivityMgr;
	UnlockActivityData *m_pActivityData;
};