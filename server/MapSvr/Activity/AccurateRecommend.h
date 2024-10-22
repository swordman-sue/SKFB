/**\file
��׼�Ƽ��߼���
\author Kevin
\date 2017/9/16
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CAccurateRecommend
{
public:
	CAccurateRecommend(){};
	~CAccurateRecommend(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	void OnNewDay();

public:
	// ����׼�Ƽ�����
	void OnNetAccurateRecommendData();

	// ��ȡ׼�Ƽ����
	void OnNetTakeAccurateRecommendGift(WORD wGoodsId);

public:
	// ��ֵ�ص�
	void OnRechargeCallback(WORD wGoodsId);

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ���;�׼�Ƽ�����
	void SendAccurateRecommendData();

protected:

	// ��������
	void UpdateSuperDiscountData();

	// ȡ��׼�Ƽ���Ʒ
	AccurateRecommendGoods* GetAccurateRecommendGoods(WORD wGoodsId);

	// ��Ӿ�׼�Ƽ���Ʒ
	void AddAccurateRecommendGoods(WORD wGoodsId);

private:
	Player *m_pPlayer;
	PlayerAccurateRecommendData *m_pAccurateRecommendData;
	CActivityManager *m_pActivityMgr;
};