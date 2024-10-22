/**\file
��ֵ�ۿۻ�߼���
\author Kevin
\date 2017/8/24
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CSuperDiscount
{
public:
	CSuperDiscount(){};
	~CSuperDiscount(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:
	// ����ֵ�ۿ�����
	void OnNetSuperDiscountData();

	// ��ȡ������
	void OnNetTakeBuyReward();

	// ��ȡ�ۻ�����
	void OnNetTakeAccumulateReward(WORD wTakeTimes);

public:
	// ��ֵ�ص�
	void OnRechargeCallback();

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ���ͳ�ֵ�ۿ�����
	void SendSuperDiscountData();

protected:

	// �Ƿ��ۻ�������¼
	bool IsAccumulateRewardRecord(WORD wTimes);

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


// �µĳ�ֵ�ۿۻ�߼���
class CNewSuperDiscount
{
public:
	CNewSuperDiscount(){};
	~CNewSuperDiscount(){};

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

	// ���ͳ�ֵ�ۿ�����
	void SendSuperDiscountData();

protected:

	// ��������
	void UpdateSuperDiscountData();

private:
	Player *m_pPlayer;
	SuperDiscountData *m_pSuperDiscountData;
	CActivityManager *m_pActivityMgr;
	WORD m_wSystemId;

	enum
	{
		REWARD_STATUS_NOT_YET = 0,	// ��û����
		REWARD_STATUS_BUY = 1,		// �ѹ���(����ȡ)
		REWARD_STATUS_TAKE = 2,		// ����ȡ
	};
};