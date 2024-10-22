/**\file
ֱ����߼���
\author Kevin
\date 2017/8/31
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CDirectBuy
{
public:
	CDirectBuy(){};
	~CDirectBuy(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// ����ֵ�ۿ�����
	void OnNetSuperDiscountData();

	// ��ȡ������
	void OnNetTakeBuyReward();

	// ��ȡ�ۻ�����
	void OnNetTakeAccumulateReward();

public:
	// ��ֵ�ص�
	void OnRechargeCallback();

	// �Ƿ��к��
	bool IsHaveRedPoint();

protected:
	// ���ͳ�ֵ�ۿ�����
	void SendSuperDiscountData();

	// ��������
	void UpdateSuperDiscountData();

private:
	Player *m_pPlayer;
	SuperDiscountData *m_pSuperDiscountData;
	CActivityManager *m_pActivityMgr;

	enum
	{
		REWARD_STATUS_NOT_YET = 0,	// ��û����
		REWARD_STATUS_BUY = 1,		// �ѹ���(����ȡ)
		REWARD_STATUS_TAKE = 2,		// ����ȡ
	};
};