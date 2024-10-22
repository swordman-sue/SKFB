/**\file
������Ʒ�߼���
\author Kevin
\date 2018/02/26
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CHotSaleGoods
{
public:
	CHotSaleGoods(){};
	~CHotSaleGoods(){};

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	VOID OnNewDay();

public:
	// ����������Ʒ����
	void OnNetHotSaleGoodsData();

	// ��ȡ��ֵ����
	void OnNetTakeRechargeReward(WORD wGoodsId);

	// ������Ʒ
	void OnNetBuyGoods(WORD wGoodsId);

public:
	// ��ֵ֪ͨ�¼�
	void OnRechargeNotify(WORD wGoodsId, DWORD dwRechargeId);

	// �Ƿ��к��
	bool IsHaveRedPoint();

	// ����������Ʒ����
	void SendHotSaleGoodsData();

protected:

	// ��������
	void UpdateHotSaleGoodsData();

	// ȡ��Ʒ����
	HotSaleGoods* GetGoods(WORD wGoodsId);

	// �����Ʒ
	void AddGoods(const HotSaleGoods &tGoods);

	// ȡ��ǰ�����ID
	WORD GetCurActivityDataId();

private:
	Player *m_pPlayer;
	PlayerHotSaleGoodsInfo *m_pPlayerHotSaleGoodsInfo;
	CActivityManager *m_pActivityMgr;
};
