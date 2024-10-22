/**\file
��ֵ����
\author Kevin
\date 2018/2/1
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CRechargeReward
{
public:
	CRechargeReward(){};
	~CRechargeReward(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// �����ֵ��������
	void OnNetRechargeRewardData();

	// ��ȡ��ֵ����
	void OnNetTakeRechargeReward(WORD wDay);

	// ��ȡ���⽱��
	void OnNetTakeExtraReward();

public:
	// ��ֵ�ص�
	void OnRechargeCallback(WORD wRechargeId);

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ���ͳ�ֵ��������
	void SendRechargeRewardData();

	// ȡÿ������
	WORD GetDailyData(DWORD dwDay);

	// ����ÿ������
	void SetDailyData(DWORD dwDay, WORD wValue);

	// ȡ�ѳ�ֵ������
	WORD GetAlreadyRechargeDay();

protected:
	
	// ���»����
	void UpdateActivityData();

private:
	Player *m_pPlayer;
	RecahrgeRewardData *m_pRechargeRewardData;
	CActivityManager *m_pActivityMgr;

	enum
	{
		REWARD_STATUS_NOT_YET = 0,	// ��û��ֵ
		REWARD_STATUS_RECHARGE = 1,		// �ѳ�ֵ(����ȡ)
		REWARD_STATUS_TAKE = 2,		// ����ȡ
	};
};