/**\file
热卖商品逻辑类
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
	// 请求热卖商品数据
	void OnNetHotSaleGoodsData();

	// 领取充值奖励
	void OnNetTakeRechargeReward(WORD wGoodsId);

	// 购买商品
	void OnNetBuyGoods(WORD wGoodsId);

public:
	// 充值通知事件
	void OnRechargeNotify(WORD wGoodsId, DWORD dwRechargeId);

	// 是否有红点
	bool IsHaveRedPoint();

	// 发送热卖商品数据
	void SendHotSaleGoodsData();

protected:

	// 更新数据
	void UpdateHotSaleGoodsData();

	// 取商品数据
	HotSaleGoods* GetGoods(WORD wGoodsId);

	// 添加商品
	void AddGoods(const HotSaleGoods &tGoods);

	// 取当前活动数据ID
	WORD GetCurActivityDataId();

private:
	Player *m_pPlayer;
	PlayerHotSaleGoodsInfo *m_pPlayerHotSaleGoodsInfo;
	CActivityManager *m_pActivityMgr;
};
