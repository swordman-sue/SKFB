/**\file
商店管理器
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;

class CShopManager
{
public:
	CShopManager();
	~CShopManager();

	void Init(Player *pPlayer);

	void OnNewRole();

	void OnNewDay();

	void OnNewWeek();

	void OnNewMonth();

	// 是否有红点
	bool IsHaveRedPoint(BYTE byShopType);

public:
	// 商品购买
	void OnGoodsBuy(BYTE byType, WORD wGoodsId, WORD wBuyNum, WORD wContext);

	// 请求商品购买记录
	void OnGoodsBuyRecord(BYTE byType);

	// 商品刷新
	void OnGoodsRefresh(BYTE byShopType, BYTE byRefreshType);

	// 商品出售
	void OnGoodsSell(BYTE byObjType, DWORD *pGoodsList, BYTE byGoodsNum);

	// 请求限时商品列表
	void OnNetLimitGoodsList(BYTE byType);

public:
	// 英雄商店数据请求
	void OnHeroShopReq();

	// 英雄商店刷新
	void OnHeroSoulShopRefresh(BYTE byShopType, BYTE byRefreshType);

	// 觉醒商店数据请求
	void OnNetAwakenShopReq();

	// 觉醒商店刷新
	void OnNetAwakenShopRefresh(BYTE byRefreshType);

	// 仙灵商店数据请求
	void OnNetStarSoulShopReq();

	// 星魂商店刷新
	void OnNetStarSoulShopRefresh(BYTE byRefreshType);

	// 水晶商店数据请求
	void OnNetCrystalShopData();

	// 水晶商店刷新
	void OnNetCrystalShopRefresh();

	// 直购商店数据请求
	void OnNetDirectBuyShopReq();

	// 更新直购商店数据
	void UpdateDirectBuyShop(WORD wUpdateType);

	// 领取直购商品
	void OnNetTakeDirectBuyGoods(WORD wGoodsId);

public:

	// 取商品购买记录数量
	WORD GetGoodsBuyRecordNum(BYTE byType, WORD wGoodsId);

	// 取商品购买记录(添加商店需要处理的接口)
	const ShopGoodsBuyRecord* GetGoodsBuyRecordEx(BYTE byType, WORD wGoodsId);

	// 发送直购商店数据
	void SendDirectBuyShopData();

	// 直购商店通过充值购买
	bool DirectBuyShopBuyByRecharge(WORD wGoodsId);
protected:

	// 处理通用商品购买
	bool DoCommonGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wItemFromId, WORD wCurrencyFromId);

	// 商城道具购买
	bool MallItemBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 商城礼包购买
	bool MallGiftBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 工会商品购买
	bool GuildGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 英雄商品购买
	bool HeroGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 叛军商品购买
	bool RebelGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 爬塔商品购买
	bool TowerGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 竞技场商品购买
	bool ArenaGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 觉醒商品购买
	bool AwakenGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 星魂商品购买
	bool StarSoulGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 积分赛商品购买
	bool ScoreMatchGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 7天商店购买
	bool SevenDayShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 等级礼包商店购买
	bool LevelGiftShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wRoleLevel);

	// 无尽之地商店
	bool EndlessLandShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 直购商店购买
	bool DirectBuyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 兑换商店购买
	bool ExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 推荐商店购买
	bool RecommendShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 组队普通商店购买
	bool TeamDungeonNormalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 组队奖励商店购买
	bool TeamDungeonRewardShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 命运商店商品购买
	bool DestinyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 隐藏商店商品购买
	bool HideShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 圣诞节积分兑换商品购买
	bool ChristmasScoreExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 圣诞节物品兑换商品购买
	bool ChristmasItemExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 水晶商店购买
	bool CrystalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 热卖商品购买
	bool HotSaleGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 资源战商品购买
	bool ResourceWarGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// GM商品购买
	bool GMShopGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wGM);

protected:
	// 出售物品
	bool DoSellItem(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

	// 出售英雄
	bool DoSellHero(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

	// 出售装备
	bool DoSellEquipemnt(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

	// 出售宝物
	bool DoSellTreasure(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

// 觉醒商店相关
protected:

	// 创建刷新商店商品
	void CreateRefreshShopGoods(BYTE byShopType);

	// 发送刷新商店数据
	void SendRefreshShopData(BYTE byShopType);

	// 更新商店刷新免费次数
	void UpdateShopRefreshFreeTimes(BYTE byShopType);

	// 取刷新商店数据
	RefreshShopData* GetRefreshShopData(BYTE byShopType);

	// 更新资源战商店数据
	void UpdateResourceWarShop(WORD wRefreshType);

protected:

	// 取商品购买记录(添加商店需要处理的接口)
	ShopGoodsBuyRecord* GetGoodsBuyRecord(BYTE byType, WORD wGoodsId);

	// 添加商品购买记录(添加商店需要处理的接口)
	bool AddGoodsBuyRecord(BYTE byType, WORD wGoodsId, WORD wGoodsNum);

	// 发送商品购买记录列表
	void SendGoodsBuyRecordList(BYTE byType, BYTE byRecordNum, ShopGoodsBuyRecord *pRecordList);

	// 重置商店数据
	void ResetShopData(BYTE byShopType);

	// 检测是否购买数量限制
	bool CheckIsBuyNumLimit(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wBuyNumMax);

private:
	Player *m_pPlayer;
	PlayerShopData *m_pShopData;
};