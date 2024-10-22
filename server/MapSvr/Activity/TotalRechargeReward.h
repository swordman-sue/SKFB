/**\file
�۳佱��
\author Kevin
\date 2018/2/7
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CTotalRechargeReward
{
public:
	CTotalRechargeReward(){};
	~CTotalRechargeReward(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// �����۳�����
	void OnNetTotalRechargeData();

	// ��ȡ�۳佱��
	void OnNetTakeTotalRechargeReward(WORD wRewardId);

public:
	// �����۳�����
	void SendTotalRechargeData();

	// ����Ƿ�����¼
	bool IsRewardRecord(WORD wRewardId);

	// ��ӽ�����¼
	void AddRewardRecord(WORD wRewardId);

	// ��ֵ֪ͨ�¼�
	void OnRechargeNotify(DWORD dwAmount);

	// ���»����
	void UpdateActivityData();

private:
	Player *m_pPlayer;
	TotalRechargeRewardData *m_pTotalRechargeRewardData;
	CActivityManager *m_pActivityMgr;
};