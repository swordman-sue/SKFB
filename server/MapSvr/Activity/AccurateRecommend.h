/**\file
精准推荐逻辑类
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
	// 请求准推荐数据
	void OnNetAccurateRecommendData();

	// 领取准推荐礼包
	void OnNetTakeAccurateRecommendGift(WORD wGoodsId);

public:
	// 充值回调
	void OnRechargeCallback(WORD wGoodsId);

	// 是否有红点
	bool IsHaveRedPoint();

	// 发送精准推荐数据
	void SendAccurateRecommendData();

protected:

	// 更新数据
	void UpdateSuperDiscountData();

	// 取精准推荐商品
	AccurateRecommendGoods* GetAccurateRecommendGoods(WORD wGoodsId);

	// 添加精准推荐商品
	void AddAccurateRecommendGoods(WORD wGoodsId);

private:
	Player *m_pPlayer;
	PlayerAccurateRecommendData *m_pAccurateRecommendData;
	CActivityManager *m_pActivityMgr;
};