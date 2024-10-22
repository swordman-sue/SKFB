/**\file
��ֵ�����ѻ�߼���
\author Kevin
\date 2019/12/19
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CRechargeAndCostRankActivity
{
public:
	CRechargeAndCostRankActivity();
	~CRechargeAndCostRankActivity();

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

public:

	// ��ӳ�ֵ���
	void AddRechargeAmount(DWORD dwAmount);

	// ���������ʯ
	void AddCostDiamond(DWORD dwDiamond);

	DWORD GetCostDiamond() { return m_pActivityData->dwCostDiamond; };
	DWORD GetRechargeAmount() { return m_pActivityData->dwRechargeAmount; };

protected:
	Player *m_pPlayer;
	CActivityManager *m_pActivityMgr;
	RechargeAndCostDiamondRankData *m_pActivityData;
};