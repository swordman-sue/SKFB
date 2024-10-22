#include "stdafx.h"
#include "ShopManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "..\Item\ItemManager.h"
#include "..\Guild\GuildManager.h"
#include "EquipmentManager.h"
#include "..\Treasure\TreasureManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Arena\ArenaManager.h"
#include "..\Activity\ActivityManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Activity\Christmas.h"
#include "..\GameLogic.h"

CShopManager::CShopManager()
{

}

CShopManager::~CShopManager()
{

}

void CShopManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pShopData = &m_pPlayer->GetRoleFullInfo().tShopInfo;
	UpdateShopRefreshFreeTimes(SHOP_HERO);
}

void CShopManager::OnNewRole()
{
	CreateRefreshShopGoods(SHOP_DIRECT_BUY);
}

void CShopManager::OnNewDay()
{
	// 英雄商店
	//m_pShopData->tHeroShop.wFreeRefreshTimes = 0;
	m_pShopData->tHeroShop.wCostRefreshTimes = 0;

	// 工会商店数量
	//m_pShopData->tGuildShop.wAwardGoodsRecordNum = 0;
	m_pShopData->tGuildShop.wItemGoodsRecordNum = 0;
	m_pShopData->tGuildShop.wLimitGoodsRecordNum = 0;


	// 商城
	//m_pShopData->tMallShop.byGiftBuyRecordNum = 0;
	m_pShopData->tMallShop.wItemBuyRecordNum = 0;

	// 爬塔商店
	PlayerTowerShopData &tTowerShop = m_pShopData->tTowerShop;
	if (tTowerShop.wBuyRecordNum)
	{
		for (int i = tTowerShop.wBuyRecordNum - 1; i >= 0; --i)
		{
			ShopGoodsBuyRecord &tBuyRecrod = tTowerShop.arBuyRecord[i];
			const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_TOWER, tBuyRecrod.wGoodsId);
			if (pGoodsCfg && pGoodsCfg->byType != 4)
				tBuyRecrod = tTowerShop.arBuyRecord[--tTowerShop.wBuyRecordNum];
		}
	}

	// 竞技场商店
	PlayerArenaShopData &tArenaShop = m_pShopData->tArenaShop;
	if (tArenaShop.wBuyRecordNum)
	{
		for (int i = tArenaShop.wBuyRecordNum - 1; i >= 0; --i)
		{
			ShopGoodsBuyRecord &tBuyRecrod = tArenaShop.arBuyRecord[i];
			const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_ARENA, tBuyRecrod.wGoodsId);
			if (pGoodsCfg && pGoodsCfg->byType != 2)
				tBuyRecrod = tArenaShop.arBuyRecord[--tArenaShop.wBuyRecordNum];
		}
	}
	//m_pShopData->tArenaShop.byBuyRecordNum = 0;

	// 觉醒商店
	m_pShopData->tAwakenShop.wFreeRefreshTimes = 0;
	m_pShopData->tAwakenShop.wCostRefreshTimes = 0;

	// 星魂商店
	m_pShopData->tStarSoulShop.wUsedRefreshTimes = 0;

	// 积分赛商店
	m_pShopData->tScoreMatchShop.wBuyRecordNum = 0;

	// 无尽之地道具商店
	m_pShopData->tEndlessLandShop.wItemGoodsBuyRecordNum = 0;

	// 兑换商店
	m_pShopData->tExchangeShop.wBuyRecordNum = 0;

	// 组队普通商店
	m_pShopData->tTeamDungeonShop.wNormalGoodsBuyRecordNum = 0;

	// 水晶商店
	m_pShopData->tCrystalShopData.wTodayRefreshTimes = 0;

	UpdateDirectBuyShop(SHOP_REFRESH_DAY);
	UpdateResourceWarShop(SHOP_REFRESH_DAY);
}

void CShopManager::OnNewWeek()
{
	UpdateDirectBuyShop(SHOP_REFRESH_WEEK);
	UpdateResourceWarShop(SHOP_REFRESH_WEEK);
}

void CShopManager::OnNewMonth()
{
	UpdateDirectBuyShop(SHOP_REFRESH_MONTH);
}

// 是否有红点
bool CShopManager::IsHaveRedPoint(BYTE byShopType)
{
	// 英雄商店
	if (SHOP_HERO == byShopType)
	{
		UpdateShopRefreshFreeTimes(byShopType);

		// 当英雄碎片商店免费刷新次数达到上限时
		if (!m_pShopData->tHeroShop.wFreeRefreshTimes)
			return true;
	}
	// 直购商店
	else if (SHOP_DIRECT_BUY == byShopType)
	{
		//UpdateDirectBuyShop(false);

		if (m_pShopData->tDirectBuyShop.wIsNewGoods)
			return true;
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的商店商品类型![type:%d]", __FUNCTION__, byShopType);
	}

	return false;
}

// 商品购买
void CShopManager::OnGoodsBuy(BYTE byType, WORD wGoodsId, WORD wBuyNum, WORD wContext)
{
	// 统一对购买数量做检测
	if (!wBuyNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买的商品数量为0![RoleId:%d,ShopType:%d,GoodsId:%d]", 
			__FUNCTION__, m_pPlayer->GetRoleId(), byType, wGoodsId);
		return;
	}

	WORD wFromMsgId = C2S_SHOP_GOODS_BUY_REQ;

	bool bResult;
	switch (byType)
	{
	// 商城道具商店
	case SHOP_MALL_ITEM:		
		bResult = MallItemBuy(byType, wGoodsId, wBuyNum);
		break;

	// 商城礼包商店
	case SHOP_MALL_GIFT:		
		bResult = MallGiftBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_GUILD_ITEM:		// 公会道具商品
	case SHOP_GUILD_LIMIT:		// 公会限时商品
	case SHOP_GUILD_AWARD:		// 公会奖励商品
		bResult = GuildGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 英雄商店
	case SHOP_HERO:
		bResult = HeroGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 叛军商店
	case SHOP_REBEL:
		bResult = RebelGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 爬塔商品
	case SHOP_TOWER:
		bResult = TowerGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 竞技场商品
	case SHOP_ARENA:
		bResult = ArenaGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 觉醒商品
	case SHOP_AWAKEN:
		bResult = AwakenGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 星魂商店
	case SHOP_STAR_SOUL:
		bResult = StarSoulGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 积分赛商品
	case SHOP_SCORE_MATCH:
		bResult = ScoreMatchGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 7天活动商店
	case SHOP_SEVEN_DAY:
		bResult = SevenDayShopBuy(byType, wGoodsId, wBuyNum);
		break;

	// 等级礼包商店
	case SHOP_LEVEL_GIFT:
		bResult = LevelGiftShopBuy(byType, wGoodsId, wBuyNum, wContext);
		break;

	// 无尽之地道具商店
	case SHOP_ENDLESS_LAND_ITEM:
	case SHOP_ENDLESS_LAND_REWARD:
		bResult = EndlessLandShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_DIRECT_BUY:
		bResult = DirectBuyShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_EXCHANGE:
		bResult = ExchangeShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_RECOMMEND:
		bResult = RecommendShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_TEAM_DUNGEON_NORMAL:
		bResult = TeamDungeonNormalShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_TEAM_DUNGEON_REWARD:
		bResult = TeamDungeonRewardShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_DESTINY:
		bResult = DestinyShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_HIDE:
		bResult = HideShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_CHRISTMAS_SCORE_EXCHANGE:
		bResult = ChristmasScoreExchangeShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_CHRISTMAS_ITEM_EXCHANGE:
		bResult = ChristmasItemExchangeShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_CRYSTAL:
		bResult = CrystalShopBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_HOT_SALE_GOODS:
		bResult = HotSaleGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 资源战商店
	case SHOP_RESOURCE_WAR_SHOP:
		bResult = ResourceWarGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// GM商店
	case SHOP_GM:
		bResult = GMShopGoodsBuy(byType, wGoodsId, wBuyNum, wContext);
		break;

	default:
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL,"[%s]木有处理的商店商品类型![type:%d]",__FUNCTION__,byType);
		return;
	}

	if (!bResult)
	{ 
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买失败![type:%d]", __FUNCTION__, byType);
		return;
	}

	MSG_S2C_SHOP_GOODS_BUY_RESP msg;
	msg.m_byType = byType;
	msg.m_wParam1 = wGoodsId;
	msg.m_wParam2 = wBuyNum;
	msg.m_wParam3 = wContext;
	m_pPlayer->SendPacket(&msg);

	// 监控物品购买情况
	ShopBuyRecord_Save* pQuery = ShopBuyRecord_Save::ALLOC();
	pQuery->SetIndex(DB_SHOP_BUY_RECORD_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	pQuery->SetGoodsId(wGoodsId);
	pQuery->SetShopType(byType);
	pQuery->SetBuyNum(wBuyNum);
	g_MapServer.MapDBQuery(pQuery);

	// 热云商品购买统计
	CGameLogic::Instance()->OnGoodsBuyStatistics(m_pPlayer, byType, wGoodsId, wBuyNum);
}

// 请求商品购买记录
void CShopManager::OnGoodsBuyRecord(BYTE byType)
{
	WORD wFromMsgId = C2S_SHOP_GOODS_BUY_RECORD_REQ;

	WORD wBuyRecordNum = 0;
	ShopGoodsBuyRecord *pBuyRecordList = NULL;

	// 临时数据 目前工会限时商店使用
	//ShopGoodsBuyRecord arTempBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	//WORD wTempBuyRecordNum = 0;

	switch (byType)
	{
	case SHOP_MALL_ITEM:		// 商城道具商品
		wBuyRecordNum = m_pShopData->tMallShop.wItemBuyRecordNum;
		pBuyRecordList = m_pShopData->tMallShop.arItemBuyRecord;
		break;

	case SHOP_MALL_GIFT:		// 商城礼包商品
		wBuyRecordNum = m_pShopData->tMallShop.wGiftBuyRecordNum;
		pBuyRecordList = m_pShopData->tMallShop.arGiftBuyRecord;
		break;

	case SHOP_GUILD_ITEM:		// 公会道具商品
		wBuyRecordNum = m_pShopData->tGuildShop.wItemGoodsRecordNum;
		pBuyRecordList = m_pShopData->tGuildShop.arItemGoodsRecord;
		break;

	case SHOP_GUILD_LIMIT:		// 公会限时商品
		wBuyRecordNum = m_pShopData->tGuildShop.wLimitGoodsRecordNum;
		pBuyRecordList = m_pShopData->tGuildShop.arLimitGoodsRecord;
		break;

	case SHOP_GUILD_AWARD:		// 公会奖励商品
		wBuyRecordNum = m_pShopData->tGuildShop.wAwardGoodsRecordNum;
		pBuyRecordList = m_pShopData->tGuildShop.arAwardGoodsRecord;
		break;

	case SHOP_HERO:		// 雄魂商品(可能需要特殊处理)
		wBuyRecordNum = m_pShopData->tHeroShop.wGoodsNum;
		pBuyRecordList = m_pShopData->tHeroShop.arGoodsList;
		break;

	case SHOP_TOWER:
		wBuyRecordNum = m_pShopData->tTowerShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tTowerShop.arBuyRecord;
		break;
	
	case SHOP_ARENA:
		wBuyRecordNum = m_pShopData->tArenaShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tArenaShop.arBuyRecord;
		break;

	case SHOP_AWAKEN:
		wBuyRecordNum = m_pShopData->tAwakenShop.wGoodsNum;
		pBuyRecordList = m_pShopData->tAwakenShop.arGoodsList;
		break;

	// 星魂商店
	case SHOP_STAR_SOUL:
		wBuyRecordNum = m_pShopData->tStarSoulShop.wGoodsNum;
		pBuyRecordList = m_pShopData->tStarSoulShop.arGoodsList;
		break;

	case SHOP_SCORE_MATCH:
		wBuyRecordNum = m_pShopData->tScoreMatchShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tScoreMatchShop.arBuyRecord;
		break;

	case SHOP_SEVEN_DAY:
		wBuyRecordNum = m_pShopData->tSevenShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tSevenShop.arBuyRecord;
		break;

	case SHOP_LEVEL_GIFT:
		wBuyRecordNum = m_pShopData->tLevelGiftShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tLevelGiftShop.arBuyRecord;
		break;

	case SHOP_ENDLESS_LAND_ITEM:
		wBuyRecordNum = m_pShopData->tEndlessLandShop.wItemGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tEndlessLandShop.arItemGoodsBuyRecord;
		break;

	case SHOP_ENDLESS_LAND_REWARD:
		wBuyRecordNum = m_pShopData->tEndlessLandShop.wRewardGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tEndlessLandShop.arRewardGoodsBuyRecord;
		break;

	// 直购商店
	case SHOP_DIRECT_BUY:
		wBuyRecordNum = m_pShopData->tDirectBuyShop.wGoodsNum;
		pBuyRecordList = m_pShopData->tDirectBuyShop.arGoodsList;
		break;

	// 兑换商店
	case SHOP_EXCHANGE:
		wBuyRecordNum = m_pShopData->tExchangeShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tExchangeShop.arBuyRecord;
		break;

	// 推荐商店
	case SHOP_RECOMMEND:
		wBuyRecordNum = m_pShopData->tRecommendShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tRecommendShop.arBuyRecord;
		break;

	// 组队普通商店
	case SHOP_TEAM_DUNGEON_NORMAL:
		wBuyRecordNum = m_pShopData->tTeamDungeonShop.wNormalGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tTeamDungeonShop.arNormalGoodsBuyRecord;
		break;

		// 组队奖励商店
	case SHOP_TEAM_DUNGEON_REWARD:
		wBuyRecordNum = m_pShopData->tTeamDungeonShop.wRewardGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tTeamDungeonShop.arRewardGoodsBuyRecord;
		break;

		// 圣诞节积分兑换商店
	case SHOP_CHRISTMAS_SCORE_EXCHANGE:
		wBuyRecordNum = m_pShopData->tChristmasShop.wScoreExchangeGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tChristmasShop.arScoreExchangeGoodsBuyRecord;
		break;

		// 圣诞节物品兑换商店
	case SHOP_CHRISTMAS_ITEM_EXCHANGE:
		wBuyRecordNum = m_pShopData->tChristmasShop.wItemExchangeGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tChristmasShop.arItemExchangeGoodsBuyRecord;
		break;

		// 水晶商店
	case SHOP_CRYSTAL:
		wBuyRecordNum = m_pShopData->tCrystalShopData.wGoodsNum;
		pBuyRecordList = m_pShopData->tCrystalShopData.arGoodsList;
		break;

		// 资源战商店
	case SHOP_RESOURCE_WAR_SHOP:
		wBuyRecordNum = m_pShopData->tResourceWarShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tResourceWarShop.arBuyRecord;
		break;

		// GM商店
	case SHOP_GM:
		wBuyRecordNum = m_pShopData->tGMShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tGMShop.arBuyRecord;
		break;

	default:
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的商店商品类型![type:%d]", __FUNCTION__, byType);
		return;
	}

	SendGoodsBuyRecordList(byType, wBuyRecordNum, pBuyRecordList);
}

// 商品刷新
void CShopManager::OnGoodsRefresh(BYTE byShopType, BYTE byRefreshType)
{
	WORD wFromMsgId = C2S_SHOP_REFRESH_REQ;

	if (SHOP_HERO == byShopType)
	{
		OnHeroSoulShopRefresh(byShopType, byRefreshType);
	}
	else if (SHOP_AWAKEN == byShopType)
	{
		OnNetAwakenShopRefresh(byRefreshType);
	}
	else if (SHOP_STAR_SOUL == byShopType)
	{
		OnNetStarSoulShopRefresh(byRefreshType);
	}
	else if (SHOP_CRYSTAL == byShopType)
	{
		OnNetCrystalShopRefresh();
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的商店商品类型![type:%d]", __FUNCTION__, byShopType);
		return;
	}

	MSG_S2C_SHOP_REFRESH_RESP msg;
	msg.m_byShopType = byShopType;
	msg.m_byRefreshType = byRefreshType;
	m_pPlayer->SendPacket(&msg);
}

// 出售
void CShopManager::OnGoodsSell(BYTE byObjType, DWORD *pGoodsList, BYTE byGoodsNum)
{
	WORD wFromMsgId = C2S_SHOP_GOODS_SELL_REQ;

	bool bRet = false;

	vector<MoneyData> vCurrencyList;

	// 碎片(物品)
	if (OBJECT_ITEM == byObjType)
	{
		bRet = DoSellItem(pGoodsList, byGoodsNum, vCurrencyList);
	}
	// 装备
	else if (OBJECT_EQUIPMENT == byObjType)
	{
		bRet = DoSellEquipemnt(pGoodsList, byGoodsNum, vCurrencyList);
	}
	// 英雄
	else if (OBJECT_HERO == byObjType)
	{
		bRet = DoSellHero(pGoodsList, byGoodsNum, vCurrencyList);
	}
	// 宝物
	else if (OBJECT_TREASURE == byObjType)
	{
		bRet = DoSellTreasure(pGoodsList, byGoodsNum, vCurrencyList);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]未处理的对象类型[ObjectType:%d]", __FUNCTION__, byObjType);
		return;
	}

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]出售商品失败![ObjectType:%d]", __FUNCTION__, byObjType);
		return;
	}
		
	MSG_S2C_SHOP_GOODS_SELL_RESP msg;
	msg.m_byObjType = byObjType;
	msg.m_byGoodsNum = byGoodsNum;
	msg.m_byCurrencyNum = 0;
	memcpy_s(msg.m_GoodsList, byGoodsNum*sizeof(Goods), pGoodsList, byGoodsNum*sizeof(Goods));
	for (size_t i = 0; i < vCurrencyList.size(); ++i)
	{
		if (msg.m_byCurrencyNum >= MAX_MONEY_TYPE_NUM)
			break;
		msg.m_CurrencyList[msg.m_byCurrencyNum++] = vCurrencyList[i];
	}
	m_pPlayer->SendPacket(&msg);
}

// 请求限时商品列表
void CShopManager::OnNetLimitGoodsList(BYTE byType)
{
	WORD wFromMsgId = C2S_SHOP_LIMIT_GOODS_LIST_REQ;

	if (SHOP_GUILD_LIMIT == byType)
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(m_pPlayer->GetRoleId());
		if (!pGuild)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]木有在工会不能请工会商品列表![type:%d]", __FUNCTION__, byType);
			return;
		}

		pGuild->SendLimitGoodsList(m_pPlayer);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]暂时木有处理的商店商品类型![type:%d]", __FUNCTION__, byType);
		return;
	}
}

// 处理通用商品购买
bool CShopManager::DoCommonGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wItemFromId, WORD wCurrencyFromId)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买商品[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测货币消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买货币消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测道具消耗
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买道具消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商品购买已达上限[ShopType:%d,GoodsId:%d,BuyNum:%d,MaxNum:%d,AlreadyBuyNum:%d]", 
			__FUNCTION__, byShopType, wGoodsId, wBuyNum, pGoodsCfg->wBuyNumMax, GetGoodsBuyRecordNum(byShopType, wGoodsId));
		return false;
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理货币消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, wCurrencyFromId);

	// 处理道具消耗
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pGoodsCfg->vCostItemList[i], wItemFromId);
	}

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, wItemFromId);

	return true;
}

// 商城道具购买
bool CShopManager::MallItemBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// 取商品配置
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	WORD wBuyNumLimit = pGoodsCfg->nVipIdx ? m_pPlayer->GetVipValue(pGoodsCfg->nVipIdx - 1) : pGoodsCfg->wBuyNumMax;

	// 取购买记录
	WORD wBuyRecordNum = GetGoodsBuyRecordNum(byShopType, wGoodsId);

	// 检测今日购买次数
	if (wBuyNumLimit && (wBuyRecordNum + wBuyNum) > wBuyNumLimit)
	{
		MAPLOG(ERROR_LEVEL, "[%s]今日购买次数已达上限![GoodsId:%d,MaxBuyNum:%d]",__FUNCTION__, wGoodsId, wBuyNumLimit);
		return false;
	}

	int nCostDiamond = 0;
	if (pGoodsCfg->vPriceList.size())
	{
		for (WORD i = wBuyRecordNum; i < wBuyNum + wBuyRecordNum; ++i)
		{
			int nIndex = min(pGoodsCfg->vPriceList.size() - 1, i);
			nCostDiamond += pGoodsCfg->vPriceList[nIndex];
		}
	}

	// 检测消耗(金钱)
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买商品钻石不足![GoodsId:%d,CostDiamond:%d]", __FUNCTION__, wGoodsId, nCostDiamond);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		// 角色等级不足
		MAPLOG(ERROR_LEVEL, "[%s]购买商品角色等级不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_MALL_ITEM_BUY);

	// 处理消耗(金钱)
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_MALL_ITEM_BUY);

	// 更新购买次数(有购买次数限制才需要记录)
	if (wBuyNumLimit)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 购买体力(日常任务事件)
	if (pGoodsCfg->dwEntityId == g_GetConfigValue(282))
	{
		m_pPlayer->OnUpdateDailyMission(6, wBuyNum);
	}

	// 购买精力(日常任务事件)
	if (pGoodsCfg->dwEntityId == g_GetConfigValue(283))
	{
		m_pPlayer->OnUpdateDailyMission(7, wBuyNum);
	}

	return true;
}

// 商城礼包购买
bool CShopManager::MallGiftBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// 取商品配置
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		// 找不到商品配置
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测购买次数
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		// 今日购买次数已达上限
		MAPLOG(ERROR_LEVEL, "[%s]商城礼包购买次数超过上限![GoodsId:%d,MaxBuyNum:%d]",
			__FUNCTION__, wGoodsId, pGoodsCfg->wBuyNumMax);
		return false;
	}

	// 检测消耗(金钱)
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		// 角色等级不足
		MAPLOG(ERROR_LEVEL, "[%s]购买商品角色等级不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测需要的VIP等级
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pGoodsCfg->nNeedVipLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商城商品VIP等级限制不能购买![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_MALL_ITEM_BUY);

	// 处理消耗(金钱)
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_MALL_ITEM_BUY);

	// 更新购买次数
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}

// 请求雄魂商店数据
void CShopManager::OnHeroShopReq()
{
	// 商店数据还没初始化
	if (!m_pShopData->tHeroShop.wGoodsNum || !m_pShopData->tHeroShop.dwLastRecoverTime)
	{
		CreateRefreshShopGoods(SHOP_HERO);
		m_pShopData->tHeroShop.dwLastRecoverTime = time(NULL);
	}

	UpdateShopRefreshFreeTimes(SHOP_HERO);

	// 发送商店数据
	SendRefreshShopData(SHOP_HERO);
}

// 觉醒商店数据请求
void CShopManager::OnNetAwakenShopReq()
{
	// 商店数据还没初始化
	if (!m_pShopData->tAwakenShop.wGoodsNum || !m_pShopData->tAwakenShop.dwLastRecoverTime)
	{
		CreateRefreshShopGoods(SHOP_AWAKEN);
		m_pShopData->tAwakenShop.dwLastRecoverTime = time(NULL);
	}

	UpdateShopRefreshFreeTimes(SHOP_AWAKEN);

	// 发送商店数据
	SendRefreshShopData(SHOP_AWAKEN);
}

// 刷新雄魂商店
void CShopManager::OnHeroSoulShopRefresh(BYTE byShopType, BYTE byRefreshType)
{
	UpdateShopRefreshFreeTimes(byShopType);
	
	RefreshShopData &tShopData = m_pShopData->tHeroShop;

	// 使用免费次数
	if (1 == byRefreshType)
	{
		BYTE byRefreshTimesMax = BYTE(g_GetConfigValue(CONFIG_HEROSOUL_SHOP_REFRESH_FREE_TIMES));
		if (tShopData.wFreeRefreshTimes >= byRefreshTimesMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]免费刷新次数已达上限", __FUNCTION__);
			return;
		}

		// 消耗
		tShopData.wFreeRefreshTimes++;
	}
	// 使用物品
	else if (2 == byRefreshType)
	{
		CItemManager *pItemMgr = m_pPlayer->GetItemManager();
		CHECK_POINTER(pItemMgr);

		// 检测刷新次数上限
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_HERO_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]英雄商店消耗刷新次数已达上限", __FUNCTION__);
			return;
		}

		// 刷新令不足
		DWORD dwCostItemId = g_GetConfigValue(CONFIG_SHOP_REFRESH_ITEM_ID);
		if (!pItemMgr->GetItemNum(dwCostItemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新英雄商店消耗的刷新令不足", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// 消耗
		pItemMgr->RemoveItem(dwCostItemId, 1, TRUE, FROM_HERO_SHOP_REFRESH);
	}
	// 物品雄魂
	else
	{
		// 检测刷新次数上限
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_HERO_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]消耗刷新次数已达上限", __FUNCTION__);
			return;
		}

		// 雄魂不足
		int nCostHeroSoul = int(g_GetConfigValue(CONFIG_HERO_SHOP_REFRESH_COST_HEROSOUL));
		if (m_pPlayer->GetProperty(ROLE_PRO_HERO_SOUL) < nCostHeroSoul)
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新英雄商店消耗的雄魂不足", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// 消耗
		m_pPlayer->ModifyProperty(ROLE_PRO_HERO_SOUL, -nCostHeroSoul, TRUE, FROM_HERO_SHOP_REFRESH);
	}

	// 重新生成商品
	CreateRefreshShopGoods(byShopType);

	// 发送商店数据
	SendRefreshShopData(byShopType);

	// 成就事件
	m_pPlayer->OnUpdateAchievement(13);

	// 7天活动事件(英雄商店刷新次数)
	m_pPlayer->OnUpdateSevenDayTarget(8, 1, true);
}

// 觉醒商店刷新
void CShopManager::OnNetAwakenShopRefresh(BYTE byRefreshType)
{
	BYTE byShopType = SHOP_AWAKEN;

	UpdateShopRefreshFreeTimes(byShopType);

	RefreshShopData &tShopData = m_pShopData->tAwakenShop;

	// 使用免费次数
	if (1 == byRefreshType)
	{
		BYTE byRefreshTimesMax = BYTE(g_GetConfigValue(CONFIG_HEROSOUL_SHOP_REFRESH_FREE_TIMES));
		if (tShopData.wFreeRefreshTimes >= byRefreshTimesMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]觉醒商店免费刷新次数已达上限", __FUNCTION__);
			return;
		}

		// 消耗
		tShopData.wFreeRefreshTimes++;
	}
	// 使用物品
	else if (2 == byRefreshType)
	{
		// 检测刷新次数上限(todo::目前跟雄魂商店一样次数)
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_HERO_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]消耗刷新次数已达上限", __FUNCTION__);
			return;
		}

		CItemManager *pItemMgr = m_pPlayer->GetItemManager();
		CHECK_POINTER(pItemMgr);

		// 刷新令不足
		DWORD dwCostItemId = g_GetConfigValue(CONFIG_SHOP_REFRESH_ITEM_ID);
		if (!pItemMgr->GetItemNum(dwCostItemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新觉醒商店消耗的刷新令不足", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// 消耗
		pItemMgr->RemoveItem(dwCostItemId, 1, TRUE, FROM_AWAKEN_SHOP_REFRESH);
	}
	// 使用神魂
	else
	{
		// 检测刷新次数上限(todo::目前跟雄魂商店一样次数)
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_AWAKEN_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]消耗刷新次数已达上限", __FUNCTION__);
			return;
		}

		// 神魂不足
		int nCostGodSoul = int(g_GetConfigValue(CONFIG_AWAKEN_SHOP_REFRESH_COST_HEROSOUL));
		if (m_pPlayer->GetProperty(ROLE_PRO_GOD_SOUL) < nCostGodSoul)
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新觉醒商店消耗的神魂不足", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// 消耗
		m_pPlayer->ModifyProperty(ROLE_PRO_GOD_SOUL, -nCostGodSoul, TRUE, FROM_AWAKEN_SHOP_REFRESH);
	}

	// 重新生成商品
	CreateRefreshShopGoods(byShopType);

	// 发送商店数据
	SendRefreshShopData(byShopType);

	// 成就事件
	//m_pPlayer->OnUpdateAchievement(13);
}

// 仙灵商店数据请求
void CShopManager::OnNetStarSoulShopReq()
{
	// 检测是否需要刷新
	DWORD dwCurTime = time(NULL);
	if (dwCurTime >= m_pShopData->tStarSoulShop.dwLastAutoRefreshTime + g_GetConfigValue(244) * 60)
	{
		CreateRefreshShopGoods(SHOP_STAR_SOUL);

		m_pShopData->tStarSoulShop.dwLastAutoRefreshTime = dwCurTime;
	}

	// 发送商店数据
	SendRefreshShopData(SHOP_STAR_SOUL);
}

// 仙灵商店刷新
void CShopManager::OnNetStarSoulShopRefresh(BYTE byRefreshType)
{
	BYTE byShopType = SHOP_STAR_SOUL;

	StarSoulShopData &tShopData = m_pShopData->tStarSoulShop;


	// 自动刷新(时间到)
	if (1 == byRefreshType)
	{
		DWORD dwCurTime = time(NULL);
		if (dwCurTime < tShopData.dwLastAutoRefreshTime + g_GetConfigValue(244) * 60)
		{
			MAPLOG(ERROR_LEVEL, "[%s]自动刷新时间还没到", __FUNCTION__);
			return;
		}

		tShopData.dwLastAutoRefreshTime = dwCurTime;
	}
	else
	{
		// 检测次数
		if (tShopData.wUsedRefreshTimes >= g_GetConfigValue(245))
		{
			MAPLOG(ERROR_LEVEL, "[%s]今日仙灵商店刷新次数已达上限", __FUNCTION__);
			return;
		}

		int nCostStarSoul = g_GetConfigValue(246);

		// 检测消耗
		if (m_pPlayer->GetProperty(ROLE_PRO_STAR_SOUL) < nCostStarSoul)
		{
			MAPLOG(ERROR_LEVEL, "[%s]仙灵商店刷新消耗仙魂不足", __FUNCTION__);
			return;
		}

		m_pPlayer->ModifyProperty(ROLE_PRO_STAR_SOUL, -nCostStarSoul);

		tShopData.wUsedRefreshTimes++;
	}

	// 重新生成商品
	CreateRefreshShopGoods(byShopType);

	// 发送商店数据
	SendRefreshShopData(byShopType);
}

// 水晶商店数据请求
void CShopManager::OnNetCrystalShopData()
{
	BYTE byShopType = SHOP_CRYSTAL;

	// 商店数据初始化
	if (!m_pShopData->tCrystalShopData.wGoodsNum)
		CreateRefreshShopGoods(byShopType);

	// 发送商店数据
	SendRefreshShopData(byShopType);
}

// 水晶商店刷新
void CShopManager::OnNetCrystalShopRefresh()
{
	BYTE byShopType = SHOP_CRYSTAL;

	// 检测刷新次数
	if (m_pShopData->tCrystalShopData.wTodayRefreshTimes >= g_GetConfigValue(278))
	{
		MAPLOG(ERROR_LEVEL, "[%s]水晶商店今日刷新次数已满[times:%d]", 
			__FUNCTION__, m_pShopData->tCrystalShopData.wTodayRefreshTimes);
		return;
	}

	m_pShopData->tCrystalShopData.wTodayRefreshTimes++;

	// 重新生成商品
	CreateRefreshShopGoods(byShopType);

	// 发送商店数据
	SendRefreshShopData(byShopType);
}

// 直购商店数据请求
void CShopManager::OnNetDirectBuyShopReq()
{
	//UpdateDirectBuyShop(true);
	// todo::应该不会
	if (!m_pShopData->tDirectBuyShop.wGoodsNum)
		CreateRefreshShopGoods(SHOP_DIRECT_BUY);

	SendDirectBuyShopData();
}

// 更新直购商店数据
void CShopManager::UpdateDirectBuyShop(WORD wUpdateType)
{
	WORD wShopType = SHOP_DIRECT_BUY;

	// 重新随机刷出商品
	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetRefreshShopSlotList(wShopType);
	if (!pSlotListCfg || !pSlotListCfg->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]刷新商店商品槽位列表配置为空", __FUNCTION__);
		return;
	}

	const ShopGoodsData_Config *pGoodsCfg;

	// 0=日刷新 1=周刷新 2=月刷新
	DirectBuyShopData &tDirectBuyShop = m_pShopData->tDirectBuyShop;
	for (WORD i = 0; i < tDirectBuyShop.wGoodsNum; ++i)
	{
		ShopGoodsBuyRecord &tRecord = tDirectBuyShop.arGoodsList[i];

		// 取商品配置
		pGoodsCfg = CConfigManager::Instance()->GetShopGoods(wShopType, tRecord.wGoodsId);
		if (!pGoodsCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置![GoodsId:%d]", __FUNCTION__, tRecord.wGoodsId);
			continue;
		}

		// 不是本次要刷新的类型商品
		if (wUpdateType != pGoodsCfg->wRefreshType)
			continue;

		// todo::先重置商品数量, 万一刷新失败了,就继续使用原商品
		tRecord.wBuyNum = 0;

		// 槽位越界
		if (i >= pSlotListCfg->size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]真购商店槽位越界![slot:%d]", __FUNCTION__, i);
			return;
		}

		vector<const ShopGoodsData_Config*> vRandList;
		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];

		WORD wTotalRate = 0;
		// 取筛选随机列表商品
		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
			if (pGoodsCfg && m_pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel)
			{
				wTotalRate += pGoodsCfg->wRefreshRate;
				vRandList.push_back(pGoodsCfg);
			}
		}

		// 检测总概率及随机列表是否合法
		if (!wTotalRate || !vRandList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新商店商品筛随机列表为空![TotalRate:%d,ListSize:%d,RoleLv:%d]",
				__FUNCTION__, wTotalRate, vRandList.size(), m_pPlayer->GetLevel());
			return;
		}

		// 随机商品
		int nRandValue = g_Rand(1, wTotalRate);
		int nCurRate = 0;
		for (size_t j = 0; j < vRandList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = vRandList[j];
			nCurRate += pGoodsCfg->wRefreshRate;
			if (nRandValue <= nCurRate)
			{
				tRecord.wGoodsId = pGoodsCfg->wGoodsId;
				break;
			}
		}
	}
}

// 更新直购商店数据 // 旧逻辑
// void CShopManager::UpdateDirectBuyShop(bool bIsClientRequst)
// {
// 	// 如果有商品可领取则不刷新
// 	if (m_pShopData->tDirectBuyShop.wCanTakeGoodsNum)
// 		return;
// 
// 
// 	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(201);
// 	if (!pServerCfg || !pServerCfg->vValueList.size())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到直购商店刷新时间配置", __FUNCTION__);
// 		return;
// 	}
// 
// 	bool bIsRefreshNewGoods = false;
// 	tm tDetailTime;
// 	GetTMTime(&tDetailTime, time(NULL));
// 	int nCurTime = tDetailTime.tm_hour * 100 + tDetailTime.tm_min;
// 	for (int i = pServerCfg->vValueList.size() - 1; i >= 0; --i)
// 	{
// 		int nRefreshTime = pServerCfg->vValueList[i]; // 刷新时间
// 		if (nCurTime >= nRefreshTime)
// 		{
// 			// 1.如果跟上一次刷的时间段不一样  2.如果跟上一次刷的不是同一天
// 			if (nRefreshTime != m_pShopData->tDirectBuyShop.wLastRefreshTime ||
// 				tDetailTime.tm_yday != m_pShopData->tDirectBuyShop.wLastRefreshYearDay)
// 			{
// 				CreateRefreshShopGoods(SHOP_DIRECT_BUY);
// 				m_pShopData->tDirectBuyShop.wLastRefreshTime = nRefreshTime;
// 				m_pShopData->tDirectBuyShop.wLastRefreshYearDay = tDetailTime.tm_yday;
// 				bIsRefreshNewGoods = true;
// 				m_pShopData->tDirectBuyShop.wIsNewGoods = true;
// 			}
// 			break;
// 		}
// 	}
// 
// 	if (!m_pShopData->tDirectBuyShop.wGoodsNum)
// 	{
// 		CreateRefreshShopGoods(SHOP_DIRECT_BUY);
// 		bIsRefreshNewGoods = true;
// 		m_pShopData->tDirectBuyShop.wIsNewGoods = true;
// 	}
// 
// 	// 如果是客户端请求,并且不是刚刚刷新商品
// 	if (bIsClientRequst && !bIsRefreshNewGoods)
// 		m_pShopData->tDirectBuyShop.wIsNewGoods = false;
// }

// 领取直购商品
void CShopManager::OnNetTakeDirectBuyGoods(WORD wGoodsId)
{
	WORD wFromMsgId = C2S_TAKE_DIRECT_BUY_GOODS_REQ;

	DirectBuyShopData &tShopData = m_pShopData->tDirectBuyShop;

	// 检测是否可领取
	bool bIsExist = false;
	for (WORD i = 0; i < tShopData.wCanTakeGoodsNum; ++i)
	{
		if (tShopData.arCanTakeGoodsList[i] == wGoodsId)
		{
			bIsExist = true;
			break;
		}
	}
	if (!bIsExist)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]此直购商品不可领取[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_DIRECT_BUY, wGoodsId);
	if (!pGoodsCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]找不到直购商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// 删除已领取的商品
	for (WORD i = 0; i < tShopData.wCanTakeGoodsNum; ++i)
	{
		if (tShopData.arCanTakeGoodsList[i] == wGoodsId)
		{
			tShopData.arCanTakeGoodsList[i] = tShopData.arCanTakeGoodsList[tShopData.wCanTakeGoodsNum-1];
			tShopData.wCanTakeGoodsNum--;
			break;
		}
	}

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_DIRECT_BUY_GOODS);

	SendDirectBuyShopData();

	MSG_S2C_TAKE_DIRECT_BUY_GOODS_RESP msg;
	msg.m_wGoodsId = wGoodsId;
	m_pPlayer->SendPacket(&msg);
}

// 购买雄魂商店商品
bool CShopManager::HeroGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄商店商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// 商店数据还没初始化
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到雄魂商品数据[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 购买数量限制
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]英雄商店购买数量已达上限[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买英雄商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_HERO);

 	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_HERO);
 
	// todo::此处可能有问题，应该是要叠加
	pGoods->wBuyNum = wBuyNum;

	SendRefreshShopData(SHOP_HERO);

	// 日常任务事件
	m_pPlayer->OnUpdateDailyMission(5, wBuyNum);

	// 世界等级任务
	//m_pPlayer->OnUpdateWorldTaskEvent(5);

	return true;
}

// 叛军商品购买
bool CShopManager::RebelGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买叛军商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}


	// 检测道具消耗
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买道具消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 处理道具消耗
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pGoodsCfg->vCostItemList[i].dwId,pGoodsCfg->vCostItemList[i].dwNum * wBuyNum, FROM_SHOP_REBEL);
	}


	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_REBEL);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_REBEL);

	return true;
}

// 爬塔商品购买
bool CShopManager::TowerGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// todo::检测星数

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]购买爬塔商品消耗不足[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测道具消耗
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]爬塔商品购买道具消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测购买次数限制(奖励类型商品)
// 	ShopGoodsBuyRecord *pBuyRecord = GetGoodsBuyRecord(byShopType, wGoodsId);
// 	if (pBuyRecord && pGoodsCfg->wBuyNumMax && pBuyRecord->wBuyNum >= pGoodsCfg->wBuyNumMax)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]爬塔商品购买次数限制[GoodsId:%d]", __FUNCTION__, wGoodsId);
// 		return false;
// 	}

	// 检测购买次数
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		// 今日购买次数已达上限
		MAPLOG(ERROR_LEVEL, "[%s]爬塔商品购买次数限制[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理货币消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_TOWER);

	// 处理道具消耗
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
		m_pPlayer->RemoveItem(pGoodsCfg->vCostItemList[i].dwId, pGoodsCfg->vCostItemList[i].dwNum * wBuyNum, FROM_SHOP_TOWER);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_TOWER);

	// 购买记录
	if (pGoodsCfg->byType == 4)
		AddGoodsBuyRecord(SHOP_TOWER, wGoodsId, wBuyNum);

	return true;
}

// 竞技场商品购买
bool CShopManager::ArenaGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[Shop:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	WORD wHistoryRank = CArenaManager::Instance()->GetPlayerHistoryRank(m_pPlayer->GetServerId(), m_pPlayer->GetCharID());
	// 检测是否的竞技场排名
	if (pGoodsCfg->nContext && wHistoryRank > pGoodsCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场商品购买受排名限制!", __FUNCTION__);
		return false;
	}

	// 检测消耗(货币)
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场商品购买货币消耗不足!", __FUNCTION__);
		return false;
	}

	// 检测消耗(道具)
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场商品购买道具消耗不足!", __FUNCTION__);
		return false;
	}

	// 购买次数限制
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]竞技场商品购买次数已达上限!", __FUNCTION__);
		return false;
	}

	// 检测消耗(货币)
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_ARENA);

	// 检测消耗(道具)
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item *pItem = &pGoodsCfg->vCostItemList[i];
		m_pPlayer->RemoveItem(pItem->dwId, pItem->dwNum * wBuyNum, FROM_SHOP_ARENA);
	}

	// 更新购买记录
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_ARENA);

	return true;
}

// 觉醒商品购买
bool CShopManager::AwakenGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到英雄商店商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = NULL;
	for (BYTE i = 0; i < m_pShopData->tAwakenShop.wGoodsNum; i++)
	{
		if (m_pShopData->tAwakenShop.arGoodsList[i].wGoodsId == wGoodsId)
		{
			pGoods = &m_pShopData->tAwakenShop.arGoodsList[i];
			break;
		}
	}

	// 商店数据还没初始化
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到觉醒商品数据[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 购买数量限制
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]觉醒商店购买数量已达上限[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买觉醒商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_AWAKEN);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_AWAKEN);

	pGoods->wBuyNum = wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// 星魂商品购买
bool CShopManager::StarSoulGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵商店商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// 商店数据还没初始化
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到仙灵商品数据[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 购买数量限制
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]仙灵商店购买数量已达上限[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买仙灵商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, 1, FROM_SHOP_STAR_SOUL);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_STAR_SOUL);

	pGoods->wBuyNum = wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// 积分赛商品购买
bool CShopManager::ScoreMatchGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	return DoCommonGoodsBuy(byShopType, wGoodsId, wBuyNum, FROM_SHOP_SCORE, FROM_SHOP_SCORE);
}

// 7天商店购买
bool CShopManager::SevenDayShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// 检测是否活动期间


	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买商品[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商品购买已达上限[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_SEVEN_DAY);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_SEVEN_DAY);

	return true;
}

// 等级礼包商店购买
bool CShopManager::LevelGiftShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wRoleLevel)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买商品[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商品购买已达上限[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	const LevelShopData_Config *pLevelGiftDataCfg = g_pCfgMgr->GetLevelShopData(wRoleLevel);
	if (!pLevelGiftDataCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到等级礼包配置数据[RoleLevel:%d]", __FUNCTION__, wRoleLevel);
		return false;
	}

	// 检测时间是否已过期
	const LevelShopActivityData *pActivityData = m_pPlayer->GetActivityManager()->GetLevelShopActivityData(wRoleLevel);
	if (!pActivityData || time(NULL) >= (pActivityData->dwActivateTime + pLevelGiftDataCfg->nValidTime))
	{
		MAPLOG(ERROR_LEVEL, "[%s]等级礼包数据已过期[RoleLevel:%d]", __FUNCTION__, wRoleLevel);
		return false;
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_LEVEL_GIFT);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_LEVEL_GIFT);

	return true;
}

// 无尽之地商店
bool CShopManager::EndlessLandShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 杀怪数量限制
	if (m_pPlayer->GetRoleFullInfo().tSecondInfo.tEndlessLandData.wContinueKillNumRecord < pGoodsCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]杀怪数量限制[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买无尽之地商品消耗不足[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测购买次数
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]无尽之地商品购买次数限制[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_ENDLESS_LAND);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_ENDLESS_LAND);

	// 购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}

// 直购商店通过充值购买
bool CShopManager::DirectBuyShopBuyByRecharge(WORD wGoodsId)
{
	BYTE byShopType = SHOP_DIRECT_BUY;
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到直购商店商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// 商店数据还没初始化
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到直购商品数据[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 添加可以领取的商品奖励
	m_pShopData->tDirectBuyShop.AddCanTakeGoods(wGoodsId);

	pGoods->wBuyNum += 1;

	SendRefreshShopData(byShopType);

	return true;
}

// 直购商店购买
bool CShopManager::DirectBuyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到直购商店商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// 商店数据还没初始化
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到直购商品数据[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 购买数量限制
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]直购商店购买数量已达上限[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买直购商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_DIRECT_BUY);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_DIRECT_BUY);
	
	// 添加可以领取的商品奖励
	//m_pShopData->tDirectBuyShop.AddCanTakeGoods(wGoodsId);

	pGoods->wBuyNum += wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// 兑换商店购买
bool CShopManager::ExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[Shop:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]兑换商品购买消耗不足!", __FUNCTION__);
		return false;
	}

	// 检测消息的物品
	for (size_t i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item &costItem = pGoodsCfg->vCostItemList[i];
		if (m_pPlayer->GetItemNum(costItem.dwId) < costItem.dwNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]兑换商品购买消耗的物品不足[CostItemId:%d,ConstNum:%d]!", 
				__FUNCTION__, costItem.dwId, costItem.dwNum);
		}
	}

	// 购买次数限制
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]兑换商品购买次数已达上限!", __FUNCTION__);
		return false;
	}

	// 检测消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_EXCHANGE);

	// 消耗物品
	for (size_t i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item &costItem = pGoodsCfg->vCostItemList[i];
		m_pPlayer->RemoveItem(costItem.dwId, costItem.dwNum, FROM_SHOP_EXCHANGE);
	}

	// 更新购买记录
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_EXCHANGE);

	return true;
}

// 推荐商店购买
bool CShopManager::RecommendShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// todo::检测是否活动期间


	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买商品[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商品购买已达上限[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_RECOMMEND);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_RECOMMEND);

	return true;
}

// 组队商店购买
bool CShopManager::TeamDungeonNormalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// todo::需要通关副本ID

	// 检测角色等级
// 	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买商品[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商品购买已达上限[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_TEAM_NORMAL);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_TEAM_NORMAL);

	return true;
}

// 组队奖励商店购买
bool CShopManager::TeamDungeonRewardShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
// 	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买商品[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// todo::需要通关副本ID
// 	bool bIsPass = false;
// 	TeamDungeonShopData &tShopData = m_pPlayer->GetRoleFullInfo().tShopInfo.tTeamDungeonShop;
// 	for (WORD i = 0; i < tShopData.wNormalGoodsBuyRecordNum; ++i)
// 	{
// 		if (tShopData.arRewardGoodsBuyRecord[i].wGoodsId == wGoodsId)
// 		{
// 			bIsPass = true;
// 			break;
// 		}
// 	}
// 
// 	if (!bIsPass)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]还没通关[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]商品购买已达上限[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_TEAM_REWARD);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_TEAM_REWARD);

	return true;
}

// 命运商店商品购买
bool CShopManager::DestinyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_DESTINY);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_DESTINY);

	return true;
}

// 隐藏商店商品购买
bool CShopManager::HideShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测货币消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测道具消耗
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买道具不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 处理货币消耗
	if (pGoodsCfg->vCostList.size())
		m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_HIDE);

	// 处理道具消耗
	vector<Item>::const_iterator iter = pGoodsCfg->vCostItemList.begin();
	for (; iter != pGoodsCfg->vCostItemList.end(); ++iter)
	{
		m_pPlayer->RemoveItem(iter->dwId, iter->dwNum, FROM_SHOP_HIDE);
	}

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_HIDE);

	return true;
}

// 圣诞节积分兑换商品购买
bool CShopManager::ChristmasScoreExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[Shop:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	PlayerChristmasData &tChristmasData = m_pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;

	// 检测消耗的积分
	if (tChristmasData.dwCurScore < pGoodsCfg->nSpecialCostValue * wBuyNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]需要消耗的积分不足,不能购买!", __FUNCTION__);
		return false;
	}

	// 检测需要的历史积分
	if (tChristmasData.dwTotalScore < pGoodsCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]需要消耗的积分不足,不能购买!", __FUNCTION__);
		return false;
	}

	// 购买次数限制
	if (CheckIsBuyNumLimit(byShopType, wGoodsId, wBuyNum, pGoodsCfg->wBuyNumMax))
	{
		MAPLOG(ERROR_LEVEL, "[%s]圣诞节积分兑换商品购买次数已达上限!", __FUNCTION__);
		return false;
	}

	// 消耗积分
	tChristmasData.dwCurScore -= (pGoodsCfg->nSpecialCostValue * wBuyNum);

	// 同步积分数据
	CChristmas::Instance()->SendChristmasData(m_pPlayer);

	// 更新购买记录
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_CHRISTMAS_SCORE_EXCHANGE);

	return true;
}

// 圣诞节物品兑换商品购买
bool CShopManager::ChristmasItemExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商店商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测货币消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买消耗不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 购买次数限制
	if (CheckIsBuyNumLimit(byShopType, wGoodsId, wBuyNum, pGoodsCfg->wBuyNumMax))
	{
		MAPLOG(ERROR_LEVEL, "[%s]圣诞节物品兑换商品购买次数已达上限!", __FUNCTION__);
		return false;
	}

	// 检测道具消耗
	for (size_t i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item &tCostItem = pGoodsCfg->vCostItemList[i];
		if (m_pPlayer->GetItemNum(tCostItem.dwId) < tCostItem.dwNum * wBuyNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家物品消耗不足[CostItemId:%d,CostItemNum:%d]!",
				__FUNCTION__, tCostItem.dwId, tCostItem.dwNum * wBuyNum);
			return false;
		}
	}

// 	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]商店商品购买道具不足![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// 处理货币消耗
	if (pGoodsCfg->vCostList.size())
		m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE);

	// 处理道具消耗
	vector<Item>::const_iterator iter = pGoodsCfg->vCostItemList.begin();
	for (; iter != pGoodsCfg->vCostItemList.end(); ++iter)
	{
		m_pPlayer->RemoveItem(iter->dwId, iter->dwNum * wBuyNum, FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE);
	}

	// 更新购买记录
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE);

	return true;
}

// 水晶商店购买
bool CShopManager::CrystalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到水晶商商店商品配置[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// 商店数据还没初始化
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到雄魂商品数据[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 购买数量限制
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]水晶商商店购买数量已达上限[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买水晶商商品消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_CRYSTAL);

	// 添加商品
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_CRYSTAL);

	// todo::此处可能有问题，应该是要叠加
	pGoods->wBuyNum += wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// 热卖商品购买
bool CShopManager::HotSaleGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	m_pPlayer->GetActivityManager()->GetHotSaleGoods().OnNetBuyGoods(wGoodsId);
	return true;
}

// 资源战商品购买
bool CShopManager::ResourceWarGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到商品配置[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买商品等级限制[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买商品消耗不足[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测购买次数
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]爬塔商品购买次数限制[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_RESOURCE_WAR);

	// 
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_RESOURCE_WAR);

	// 购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}

// GM商品购买
bool CShopManager::GMShopGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wGM)
{
	const ShopGoodsData_Config *pShopCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pShopCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到GM商品配置[ShopType:%d, GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pShopCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买GM商品需要的角色等级不足[GoodsId:%d,NeedRoleLv:%d,RoleLv:%d]",
			__FUNCTION__, wGoodsId, pShopCfg->wNeedRoleLevel, m_pPlayer->GetLevel());
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pShopCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买GM商品消耗不足[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// 检测GM等级
	if (m_pPlayer->GetVipGMLevel() < pShopCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]购买GM商品需要的GM等级不足[GoodsId:%d,NeedGMLv:%d,RoleGMLv:%d]", 
			__FUNCTION__, wGoodsId, pShopCfg->nContext, m_pPlayer->GetVipGMLevel());
		return false;
	}

	// 检测购买次数
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pShopCfg->wBuyNumMax && wTotalBuyNum > pShopCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]GM商品购买次数限制[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 处理消耗
	m_pPlayer->DoMoneyCost(pShopCfg->vCostList, wBuyNum, FROM_SHOP_GM);

	// 添加物品
	m_pPlayer->AddObject(pShopCfg->dwEntityId, wBuyNum * pShopCfg->dwEntityNum, TRUE, FROM_SHOP_GM);

	// 购买记录,有限购才记录
	if (pShopCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}


// 工会商品购买
bool CShopManager::GuildGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// 检测是否有工会
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(m_pPlayer->GetCharID());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会，不能购买工会商店商品", __FUNCTION__);
		return false;
	}

	const ShopGoodsData_Config *pShopCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pShopCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会商品配置[GoodsId:%d]", __FUNCTION__,wGoodsId);
		return false;
	}

	// 检测工会等级
	if (pGuild->GetGuildLevel() < pShopCfg->wNeedGuildLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]工会等级不足,不能购买工会商品[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测角色等级
	if (m_pPlayer->GetLevel() < pShopCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]角色等级不足,不能购买工会商品[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测消耗
	if (!m_pPlayer->CheckMoneyCost(pShopCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]工会商品购买消耗不足![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// 检测购买上限
	if (SHOP_GUILD_LIMIT == byShopType)
	{ // 限时商店

		if (wBuyNum != 1)
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会限时商品每次只能购买一个[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}

		// 检测个人购买数量限制
		if (GetGoodsBuyRecordNum(byShopType, wGoodsId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会商品购买已达上限[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}

		// 检测工会购买数量限制
		const Goods *pGoods = pGuild->GetLimitGoods(wGoodsId);
		if (!pGoods)
		{
			MAPLOG(ERROR_LEVEL, "[%s]非法的工会限时商品[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}
		if (pGoods->wNum >= pShopCfg->wBuyNumMax)
		{
			pGuild->SendLimitGoodsList(m_pPlayer); // todo::有可能是客户端与服务器的数据已不同步，主动推过去
			MAPLOG(GUI_LEVEL, "[%s]工会限时商品已购买完[GoodsId:%d,num:%d]", __FUNCTION__, wGoodsId, pGoods->wNum);
			return false;
		}

		pGuild->AddLimitGoodsBuyNum(wGoodsId, wBuyNum);
	}
	else
	{ // 道具,奖励商店
		if (pShopCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pShopCfg->wBuyNumMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]工会商品购买已达上限[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}
	}

	// 添加购买记录
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// 处理消耗
	m_pPlayer->DoMoneyCost(pShopCfg->vCostList, wBuyNum, FROM_SHOP_GUILD);

	// 添加商品
	m_pPlayer->AddObject(pShopCfg->dwEntityId, pShopCfg->dwEntityNum*wBuyNum, TRUE, FROM_SHOP_GUILD);

	return true;
}

// 出售物品
bool CShopManager::DoSellItem(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER_RET(pItemMgr, false);

	// 检测物品
	int nHeroSoul = 0;
	int nTowerPrestige = 0;	// 爬塔声望
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		const Item *pItem = pItemMgr->GetItem(pGoodsList[i]);
		if (!pItem)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此物品[ItemId:%d]", __FUNCTION__, pGoodsList[i]);
			continue;
		}

		const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(pItem->dwId);
		if (!pItemCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到物品的配置[ItemId:%d]", __FUNCTION__, pItem->dwId);
			continue;
		}

		// 英雄碎片
		if (pItemCfg->bySubType == ITEM_TYPE_8)
		{
			// 添加战魂
			nHeroSoul += (pItemCfg->dwSellPrice * pItem->dwNum);
		}
		// 装备碎片
		else if (pItemCfg->bySubType == ITEM_TYPE_7)
		{
			// 添加威名
			nTowerPrestige += (pItemCfg->dwSellPrice * pItem->dwNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]不是英雄或者装备碎片[sub_type:%d]",
				__FUNCTION__, pItemCfg->bySubType);
			continue;
		}

		// 删除物品
		pItemMgr->RemoveItem(pItem->dwId, pItem->dwNum, TRUE, FROM_SELL_ITEM);
	}

	if (nHeroSoul)
		m_pPlayer->ModifyProperty(ROLE_PRO_HERO_SOUL, nHeroSoul);

	if (nTowerPrestige)
		m_pPlayer->ModifyProperty(ROLE_PRO_TOWER_PRESTIGE, nTowerPrestige);

	if (nHeroSoul)
	{
		MoneyData tMoneyData;
		tMoneyData.byType = ROLE_PRO_HERO_SOUL;
		tMoneyData.nValue = nHeroSoul;
		vCurrencyList.push_back(tMoneyData);
	}

	if (nTowerPrestige)
	{
		MoneyData tMoneyData;
		tMoneyData.byType = ROLE_PRO_TOWER_PRESTIGE;
		tMoneyData.nValue = nTowerPrestige;
		vCurrencyList.push_back(tMoneyData);
	}

	return true;
}

// 出售英雄
bool CShopManager::DoSellHero(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	DWORD dwTotalGold = 0;
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		// 检测英雄
		DWORD dwUId = pGoodsList[i];
		const HeroInfo *pHero = pHeroMgr->GetHero(dwUId);
		if (!pHero)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此英雄[HeroId:%d]", __FUNCTION__, dwUId);
			continue;
		}

		const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
		if (!pHeroCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄配置[HeroInfoId:%d]", __FUNCTION__, pHero->dwIntoId);
			continue;
		}

		const HeroLevel_Config *pLevelCfg = CConfigManager::Instance()->GetHeroLevel(pHero->wQuality, pHero->wLevel);
		if (!pLevelCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到英雄的等级配置数据[quality:%d,level:%d]",
				__FUNCTION__, pHero->wQuality, pHero->wLevel);
			continue;
		}

		// 兑换成金币
		dwTotalGold += (pLevelCfg->dwTotalExp + pHeroCfg->nPrice + pHero->dwExp);

		// 删除英雄
		pHeroMgr->RemoveHero(dwUId, true, FROM_SELL_HERO);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, dwTotalGold);

	MoneyData tMoneyData;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = dwTotalGold;
	vCurrencyList.push_back(tMoneyData);

	return true;
}

// 出售装备
bool CShopManager::DoSellEquipemnt(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CEquipmentManager *pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr, false);

	DWORD dwTotalGold = 0;
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		// 检测装备
		DWORD dwUId = pGoodsList[i];
		const Equipment *pEquipment = pEquipmentMgr->GetEquipment(dwUId);
		if (!pEquipment)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此装备[EquipmentUId:%d]", __FUNCTION__, dwUId);
			continue;
		}

		const Equipment_Config *pEquipmentCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
		if (!pEquipmentCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到装备配置[EquipmentInfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
			continue;
		}

		// 兑换成金币
		dwTotalGold += pEquipment->nEnhanceCostGold;
		dwTotalGold += pEquipmentCfg->dwPrice;

		// 删除装备
		pEquipmentMgr->RemoveEquipment(dwUId, true, FROM_SELL_EQUIPMENT);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, dwTotalGold);

	MoneyData tMoneyData;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = dwTotalGold;
	vCurrencyList.push_back(tMoneyData);

	return true;
}

// 出售宝物
bool CShopManager::DoSellTreasure(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CTreasureManager *pTreasureMgr = m_pPlayer->GetTreasureManager();
	CHECK_POINTER_RET(pTreasureMgr, false);

	DWORD dwTotalGold = 0;
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		// 检测宝物
		DWORD dwUId = pGoodsList[i];
		const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwUId);
		if (!pTreasure)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家没有此宝物[TreasureId:%d]", __FUNCTION__, dwUId);
			continue;
		}

		const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
		if (!pTreasureCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]没有找到宝物配置[TreasureId:%d]", __FUNCTION__, pTreasure->dwInfoId);
			continue;
		}

		const TreasureEnhance_Config *pStrengthenCfg =
			CConfigManager::Instance()->GetTreasureStrengthen(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		if (!pStrengthenCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]没有找到宝物强化配置[qaulity:%d,strengthenlevel:%d]",
				__FUNCTION__, pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
			continue;
		}

		// 兑换成金币
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
		{
			// 圣器出售改成1金币
			dwTotalGold += 1;
		}
		else{
			dwTotalGold += (pStrengthenCfg->dwTotalExp + pTreasure->dwEnhanceExp + pTreasureCfg->nPrice);
		}

		// 删除宝物
		pTreasureMgr->RemoveTreasure(dwUId, FROM_SELL_TREASURE);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, dwTotalGold);

	MoneyData tMoneyData;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = dwTotalGold;
	vCurrencyList.push_back(tMoneyData);

	return true;
}

// 生成雄魂商店商品
// void CShopManager::CreateHeroShopGoods()
// {
// 	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetHeroShopSlotList();
// 	if (!pSlotListCfg || !pSlotListCfg->size())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]英雄商店商品槽位列表配置为空",__FUNCTION__);
// 		return;
// 	}
// 
// 	RefreshShopData &tHeroShopData = m_pShopData->tHeroShop;
// 	tHeroShopData.byGoodsNum = 0;
// 
// 	vector<const ShopGoodsData_Config*> vRandList;
// 	for (size_t i = 0; i < pSlotListCfg->size(); ++i)
// 	{
// 		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];
// 
// 		WORD wTotalRate = 0;
// 		vRandList.clear();
// 		// 取筛选随机列表商品
// 		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
// 		{
// 			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
// 			if (pGoodsCfg && m_pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel)
//  			{
// 				wTotalRate += pGoodsCfg->wRefreshRate;
// 				vRandList.push_back(pGoodsCfg);
//  			}
// 		}
// 
// 		// 检测总概率及随机列表是否合法
// 		if (!wTotalRate || !vRandList.size())
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]英雄商店商品筛随机列表为空![TotalRate:%d,ListSize:%d,RoleLv:%d]",
// 				__FUNCTION__, wTotalRate, vRandList.size(), m_pPlayer->GetLevel());
// 			return;
// 		}
// 
// 		// 随机商品
// 		int nRandValue = Rand::Instance()->irand(1, wTotalRate);
// 		int nCurRate = 0;
// 		for (size_t j = 0; j < vRandList.size(); ++j)
// 		{
// 			const ShopGoodsData_Config *pGoodsCfg = vRandList[j];
// 			nCurRate += pGoodsCfg->wRefreshRate;
// 			if (nRandValue <= nCurRate)
// 			{
// 				if (tHeroShopData.byGoodsNum < REFRESH_SHOP_GOODS_NUM)
// 				{
// 					ShopGoodsBuyRecord &tGoods = tHeroShopData.arGoodsList[tHeroShopData.byGoodsNum++];
// 					tGoods.wBuyNum = 0;
// 					tGoods.wGoodsId = pGoodsCfg->wGoodsId;
// 				}
// 				break;
// 			}
// 		}
// 	}
// }

// 创建刷新商店商品
void CShopManager::CreateRefreshShopGoods(BYTE byShopType)
{
	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetRefreshShopSlotList(byShopType);
	if (!pSlotListCfg || !pSlotListCfg->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]刷新商店商品槽位列表配置为空", __FUNCTION__);
		return;
	}

	// 重置商店数据
	ResetShopData(byShopType);

	vector<const ShopGoodsData_Config*> vRandList;
	for (size_t i = 0; i < pSlotListCfg->size(); ++i)
	{
		// todo::如果是第一次,且是指定槽位,则必出指定物品
		if (SHOP_HERO == byShopType && !m_pPlayer->CheckOnceFlag(once_hero_shop_refresh) && i == 0)
		{
			m_pPlayer->SetOnceFlag(once_hero_shop_refresh);
			AddGoodsBuyRecord(byShopType, 1, 0);
			continue;
		}

		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];

		WORD wTotalRate = 0;
		vRandList.clear();
		// 取筛选随机列表商品
		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
			if (pGoodsCfg && m_pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel)
			{
				wTotalRate += pGoodsCfg->wRefreshRate;
				vRandList.push_back(pGoodsCfg);
			}
		}

		// 检测总概率及随机列表是否合法
		if (!wTotalRate || !vRandList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]刷新商店商品筛随机列表为空![TotalRate:%d,ListSize:%d,RoleLv:%d]",
				__FUNCTION__, wTotalRate, vRandList.size(), m_pPlayer->GetLevel());
			return;
		}

		// 随机商品
		int nRandValue = g_Rand(1, wTotalRate); //Rand::Instance()->irand(1, wTotalRate);
		int nCurRate = 0;
		for (size_t j = 0; j < vRandList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = vRandList[j];
			nCurRate += pGoodsCfg->wRefreshRate;
			if (nRandValue <= nCurRate)
			{
				AddGoodsBuyRecord(byShopType, pGoodsCfg->wGoodsId, 0);
				break;
			}
		}
	}
}

// 发送觉醒商店数据
void CShopManager::SendRefreshShopData(BYTE byShopType)
{
	// 英雄商店
	if (byShopType  == SHOP_HERO)
	{
		MSG_S2C_HERO_SHOP_DATA_RESP msg;
		msg.m_HeroShop = m_pShopData->tHeroShop;
		m_pPlayer->SendPacket(&msg);
	}
	// 觉醒商店
	else if (byShopType == SHOP_AWAKEN)
	{
		MSG_S2C_AWAKEN_SHOP_DATA_RESP msg;
		msg.m_AwakenShop = m_pShopData->tAwakenShop;
		m_pPlayer->SendPacket(&msg);
	}
	// 星魂商店
	else if (byShopType == SHOP_STAR_SOUL)
	{
		MSG_S2C_STAR_SOUL_SHOP_DATA_RESP msg;
		msg.m_ShopData = m_pShopData->tStarSoulShop;
		m_pPlayer->SendPacket(&msg);
	}
	// 直购商店
	else if (byShopType == SHOP_DIRECT_BUY)
	{
		MSG_S2C_DIRECT_SHOP_DATA_RESP msg;
		msg.m_ShopData = m_pShopData->tDirectBuyShop;
		m_pPlayer->SendPacket(&msg);
	}
	// 水晶商店
 	else if (byShopType == SHOP_CRYSTAL)
 	{
		MSG_S2C_CRYSTAL_SHOP_DATA_RESP msg;
		msg.m_ShopData = m_pShopData->tCrystalShopData;
		m_pPlayer->SendPacket(&msg);
 	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的刷新商店类型[type:%d]", __FUNCTION__, byShopType);
	}
}

// 更新商店刷新免费次数
void CShopManager::UpdateShopRefreshFreeTimes(BYTE byShopType)
{
	RefreshShopData *pShopData = GetRefreshShopData(byShopType);
	if (!pShopData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]取不到刷新商店数据[type:%d]", __FUNCTION__, byShopType);
		return;
	}
	DWORD dwCurTime = DWORD(time(NULL));
	if (pShopData->wFreeRefreshTimes)
	{
		DWORD dwInterval = 60 * g_GetConfigValue(CONFIG_HERO_SHOP_REFRESH_RECOVER_INTERVAL);
		BYTE byTimes = DoRecoveryTimes(pShopData->dwLastRecoverTime, dwCurTime, dwInterval);
		pShopData->wFreeRefreshTimes = pShopData->wFreeRefreshTimes > byTimes ? pShopData->wFreeRefreshTimes - byTimes : 0;
	}
	else
	{
		pShopData->dwLastRecoverTime = dwCurTime;
	}
}

// 取刷新商店数据
RefreshShopData* CShopManager::GetRefreshShopData(BYTE byShopType)
{
	if (SHOP_AWAKEN == byShopType)
	{
		return &m_pShopData->tAwakenShop;
	}
	else if (SHOP_HERO == byShopType)
	{
		return &m_pShopData->tHeroShop;
	}
	return NULL;
}


// 更新资源战商店数据
void CShopManager::UpdateResourceWarShop(WORD wRefreshType)
{
	ResourceWarShopData &tShopData = m_pShopData->tResourceWarShop;
	for (WORD i = 0; i < tShopData.wBuyRecordNum; ++i)
	{
		ShopGoodsBuyRecord &tBuyRecord = tShopData.arBuyRecord[i];
		const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_RESOURCE_WAR_SHOP, tBuyRecord.wGoodsId);
		if (pGoodsCfg && pGoodsCfg->wRefreshType == wRefreshType)
			tBuyRecord.wBuyNum = 0;
	}
}

// 取商品购买记录
ShopGoodsBuyRecord* CShopManager::GetGoodsBuyRecord(BYTE byType, WORD wGoodsId)
{
	//ShopGoodsBuyRecord *pRecord = NULL;
	if (byType == SHOP_MALL_ITEM)
	{
		for (WORD i = 0; i < m_pShopData->tMallShop.wItemBuyRecordNum; ++i)
		{
			if (m_pShopData->tMallShop.arItemBuyRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tMallShop.arItemBuyRecord[i];
			}
		}
	}
	else if (byType == SHOP_MALL_GIFT)
	{
		for (WORD i = 0; i < m_pShopData->tMallShop.wGiftBuyRecordNum; ++i)
		{
			if (m_pShopData->tMallShop.arGiftBuyRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tMallShop.arGiftBuyRecord[i];
			}
		}
	}
	else if (byType == SHOP_GUILD_ITEM)
	{
		for (WORD i = 0; i < m_pShopData->tGuildShop.wItemGoodsRecordNum; ++i)
		{
			if (m_pShopData->tGuildShop.arItemGoodsRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tGuildShop.arItemGoodsRecord[i];
			}
		}
	}
	else if (byType == SHOP_GUILD_LIMIT)
	{
		for (WORD i = 0; i < m_pShopData->tGuildShop.wLimitGoodsRecordNum; ++i)
		{
			if (m_pShopData->tGuildShop.arLimitGoodsRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tGuildShop.arLimitGoodsRecord[i];
			}
		}
	}
	else if (byType == SHOP_GUILD_AWARD)
	{
		for (BYTE i = 0; i < m_pShopData->tGuildShop.wAwardGoodsRecordNum; ++i)
		{
			if (m_pShopData->tGuildShop.arAwardGoodsRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tGuildShop.arAwardGoodsRecord[i];
			}
		}
	}
	else if (byType == SHOP_HERO)
	{
		for (BYTE i = 0; i < m_pShopData->tHeroShop.wGoodsNum; ++i)
		{
			if (m_pShopData->tHeroShop.arGoodsList[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tHeroShop.arGoodsList[i];
			}
		}
	}
	else if (byType == SHOP_AWAKEN)
	{
		for (BYTE i = 0; i < m_pShopData->tAwakenShop.wGoodsNum; ++i)
		{
			if (m_pShopData->tAwakenShop.arGoodsList[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tAwakenShop.arGoodsList[i];
			}
		}
	}
	else if (byType == SHOP_ARENA)
	{
		for (BYTE i = 0; i < m_pShopData->tArenaShop.wBuyRecordNum; ++i)
		{
			if (m_pShopData->tArenaShop.arBuyRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tArenaShop.arBuyRecord[i];
			}
		}
	}
	else if (byType == SHOP_TOWER)
	{
		for (BYTE i = 0; i < m_pShopData->tTowerShop.wBuyRecordNum; ++i)
		{
			if (m_pShopData->tTowerShop.arBuyRecord[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tTowerShop.arBuyRecord[i];
			}
		}
	}
	else if (byType == SHOP_STAR_SOUL)
	{
		for (BYTE i = 0; i < m_pShopData->tStarSoulShop.wGoodsNum; ++i)
		{
			if (m_pShopData->tStarSoulShop.arGoodsList[i].wGoodsId == wGoodsId)
			{
				return &m_pShopData->tStarSoulShop.arGoodsList[i];
			}
		}
	}
	else if (byType == SHOP_SCORE_MATCH)
	{
		for (BYTE i = 0; i < m_pShopData->tScoreMatchShop.wBuyRecordNum; ++i)
		{
			if (m_pShopData->tScoreMatchShop.arBuyRecord[i].wGoodsId == wGoodsId)
				return &m_pShopData->tScoreMatchShop.arBuyRecord[i];
		}
	}
	else if (byType == SHOP_SEVEN_DAY)
	{
		for (BYTE i = 0; i < m_pShopData->tSevenShop.wBuyRecordNum; ++i)
		{
			if (m_pShopData->tSevenShop.arBuyRecord[i].wGoodsId == wGoodsId)
				return &m_pShopData->tSevenShop.arBuyRecord[i];
		}
	}
	else if (byType == SHOP_LEVEL_GIFT)
	{
		for (BYTE i = 0; i < m_pShopData->tLevelGiftShop.wBuyRecordNum; ++i)
		{
			if (m_pShopData->tLevelGiftShop.arBuyRecord[i].wGoodsId == wGoodsId)
				return &m_pShopData->tLevelGiftShop.arBuyRecord[i];
		}
	}
	else if (byType == SHOP_ENDLESS_LAND_ITEM)
	{
		for (BYTE i = 0; i < m_pShopData->tEndlessLandShop.wItemGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tEndlessLandShop.arItemGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	else if (byType == SHOP_ENDLESS_LAND_REWARD)
	{
		for (BYTE i = 0; i < m_pShopData->tEndlessLandShop.wRewardGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tEndlessLandShop.arRewardGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	else if (byType == SHOP_DIRECT_BUY)
	{
		for (WORD i = 0; i < m_pShopData->tDirectBuyShop.wGoodsNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tDirectBuyShop.arGoodsList[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 
	else if (byType == SHOP_EXCHANGE)
	{
		for (WORD i = 0; i < m_pShopData->tExchangeShop.wBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tExchangeShop.arBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 推荐商店
	else if (byType == SHOP_RECOMMEND)
	{
		for (WORD i = 0; i < m_pShopData->tRecommendShop.wBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tRecommendShop.arBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 组队普通商店
	else if (byType == SHOP_TEAM_DUNGEON_NORMAL)
	{
		for (WORD i = 0; i < m_pShopData->tTeamDungeonShop.wNormalGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tTeamDungeonShop.arNormalGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 组队奖励商店
	else if (byType == SHOP_TEAM_DUNGEON_REWARD)
	{
		for (WORD i = 0; i < m_pShopData->tTeamDungeonShop.wRewardGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tTeamDungeonShop.arRewardGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 圣诞节积分兑换商店
	else if (byType == SHOP_CHRISTMAS_SCORE_EXCHANGE)
	{
		for (WORD i = 0; i < m_pShopData->tChristmasShop.wScoreExchangeGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tChristmasShop.arScoreExchangeGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 圣诞节物品兑换商店
	else if (byType == SHOP_CHRISTMAS_ITEM_EXCHANGE)
	{
		for (WORD i = 0; i < m_pShopData->tChristmasShop.wItemExchangeGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tChristmasShop.arItemExchangeGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 水晶商店
	else if (byType == SHOP_CRYSTAL)
	{
		for (WORD i = 0; i < m_pShopData->tCrystalShopData.wGoodsNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tCrystalShopData.arGoodsList[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// 资源战商店
	else if (byType == SHOP_RESOURCE_WAR_SHOP)
	{
		for (WORD i = 0; i < m_pShopData->tResourceWarShop.wBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tResourceWarShop.arBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// GM商店
	else if (byType == SHOP_GM)
	{
		for (WORD i = 0; i < m_pShopData->tGMShop.wBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tGMShop.arBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]没有处理的商店类型![type:%d]", __FUNCTION__, byType);
	}
	return NULL;
}

// 取商品购买记录数量
WORD CShopManager::GetGoodsBuyRecordNum(BYTE byType, WORD wGoodsId)
{
	ShopGoodsBuyRecord *pBuyRecord = GetGoodsBuyRecord(byType, wGoodsId);
	return pBuyRecord ? pBuyRecord->wBuyNum : 0;
}

// 取商品购买记录(添加商店需要处理的接口)
const ShopGoodsBuyRecord* CShopManager::GetGoodsBuyRecordEx(BYTE byType, WORD wGoodsId)
{
	return GetGoodsBuyRecord(byType, wGoodsId);
}

// 添加商品购买记录
bool CShopManager::AddGoodsBuyRecord(BYTE byType, WORD wGoodsId, WORD wGoodsNum)
{
	ShopGoodsBuyRecord *pRecord = GetGoodsBuyRecord(byType, wGoodsId);
	if (pRecord)
	{
		pRecord->wBuyNum += wGoodsNum;
		return true;
	}

	bool bIsListFull = true;

	if (byType == SHOP_MALL_ITEM)
	{
		if (m_pShopData->tMallShop.wItemBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tMallShop.arItemBuyRecord[m_pShopData->tMallShop.wItemBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	else if (byType == SHOP_MALL_GIFT)
	{
		if (m_pShopData->tMallShop.wGiftBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tMallShop.arGiftBuyRecord[m_pShopData->tMallShop.wGiftBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 工会道具
	else if (byType == SHOP_GUILD_ITEM)
	{
		if (m_pShopData->tGuildShop.wItemGoodsRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tGuildShop.arItemGoodsRecord[m_pShopData->tGuildShop.wItemGoodsRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 工会限时
	else if (byType == SHOP_GUILD_LIMIT)
	{
		if (m_pShopData->tGuildShop.wLimitGoodsRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tGuildShop.arLimitGoodsRecord[m_pShopData->tGuildShop.wLimitGoodsRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 工会奖励
	else if (byType == SHOP_GUILD_AWARD)
	{
		if (m_pShopData->tGuildShop.wAwardGoodsRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tGuildShop.arAwardGoodsRecord[m_pShopData->tGuildShop.wAwardGoodsRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	else if (byType == SHOP_HERO)
	{
		if (m_pShopData->tHeroShop.wGoodsNum < REFRESH_SHOP_GOODS_NUM)
		{
			pRecord = &m_pShopData->tHeroShop.arGoodsList[m_pShopData->tHeroShop.wGoodsNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	else if (byType == SHOP_AWAKEN)
	{
		if (m_pShopData->tAwakenShop.wGoodsNum < REFRESH_SHOP_GOODS_NUM)
		{
			pRecord = &m_pShopData->tAwakenShop.arGoodsList[m_pShopData->tAwakenShop.wGoodsNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	else if (SHOP_TOWER == byType)
	{
		if (m_pShopData->tTowerShop.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tTowerShop.arBuyRecord[m_pShopData->tTowerShop.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 竞技场
	else if (SHOP_ARENA == byType)
	{
		if (m_pShopData->tArenaShop.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tArenaShop.arBuyRecord[m_pShopData->tArenaShop.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 星魂商店
	else if (byType == SHOP_STAR_SOUL)
	{
		if (m_pShopData->tStarSoulShop.wGoodsNum < REFRESH_SHOP_GOODS_NUM)
		{
			pRecord = &m_pShopData->tStarSoulShop.arGoodsList[m_pShopData->tStarSoulShop.wGoodsNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 积分赛商店
	else if (SHOP_SCORE_MATCH == byType)
	{
		if (m_pShopData->tScoreMatchShop.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tScoreMatchShop.arBuyRecord[m_pShopData->tScoreMatchShop.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 7天商店
	else if (SHOP_SEVEN_DAY == byType)
	{
		if (m_pShopData->tSevenShop.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &m_pShopData->tSevenShop.arBuyRecord[m_pShopData->tSevenShop.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 等级礼包商店
	else if (SHOP_LEVEL_GIFT == byType)
	{
		LevelGiftShopData &tShopData = m_pShopData->tLevelGiftShop;
		if (tShopData.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arBuyRecord[tShopData.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 无尽之地道具商店
	else if (SHOP_ENDLESS_LAND_ITEM == byType)
	{
		EndlessLandShopData &tShopData = m_pShopData->tEndlessLandShop;
		if (tShopData.wItemGoodsBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arItemGoodsBuyRecord[tShopData.wItemGoodsBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 无尽之地奖励商店
	else if (SHOP_ENDLESS_LAND_REWARD == byType)
	{
		EndlessLandShopData &tShopData = m_pShopData->tEndlessLandShop;
		if (tShopData.wRewardGoodsBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arRewardGoodsBuyRecord[tShopData.wRewardGoodsBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 无尽之地奖励商店
	else if (SHOP_DIRECT_BUY == byType)
	{
		DirectBuyShopData &tShopData = m_pShopData->tDirectBuyShop;
		if (tShopData.wGoodsNum < REFRESH_SHOP_GOODS_NUM)
		{
			pRecord = &tShopData.arGoodsList[tShopData.wGoodsNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 兑换商店
	else if (SHOP_EXCHANGE == byType)
	{
		ExchangeShopData &tShopData = m_pShopData->tExchangeShop;
		if (tShopData.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arBuyRecord[tShopData.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 推荐商店
	else if (SHOP_RECOMMEND == byType)
	{
		RecommendShopData &tShopData = m_pShopData->tRecommendShop;
		if (tShopData.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arBuyRecord[tShopData.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 组队普通商店
	else if (SHOP_TEAM_DUNGEON_NORMAL == byType)
	{
		TeamDungeonShopData &tShopData = m_pShopData->tTeamDungeonShop;
		if (tShopData.wNormalGoodsBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arNormalGoodsBuyRecord[tShopData.wNormalGoodsBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 组队奖励商店
	else if (SHOP_TEAM_DUNGEON_REWARD == byType)
	{
		TeamDungeonShopData &tShopData = m_pShopData->tTeamDungeonShop;
		if (tShopData.wRewardGoodsBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arRewardGoodsBuyRecord[tShopData.wRewardGoodsBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 星魂奖励商店
	else if (SHOP_STAR_SOUL == byType)
	{
		StarSoulShopData &tShopData = m_pShopData->tStarSoulShop;
		if (tShopData.wGoodsNum < REFRESH_SHOP_GOODS_NUM)
		{
			pRecord = &tShopData.arGoodsList[tShopData.wGoodsNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 圣诞节积分兑换商店
	else if (SHOP_CHRISTMAS_SCORE_EXCHANGE == byType)
	{
		ChristmasShopData &tShopData = m_pShopData->tChristmasShop;
		if (tShopData.wScoreExchangeGoodsBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arScoreExchangeGoodsBuyRecord[tShopData.wScoreExchangeGoodsBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 圣诞节物品兑换商店
	else if (SHOP_CHRISTMAS_ITEM_EXCHANGE == byType)
	{
		ChristmasShopData &tShopData = m_pShopData->tChristmasShop;
		if (tShopData.wItemExchangeGoodsBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arItemExchangeGoodsBuyRecord[tShopData.wItemExchangeGoodsBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 水晶商店
	else if (SHOP_CRYSTAL == byType)
	{
		CrystalShopData &tShopData = m_pShopData->tCrystalShopData;
		if (tShopData.wGoodsNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arGoodsList[tShopData.wGoodsNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// 资源战商店
	else if (SHOP_RESOURCE_WAR_SHOP == byType)
	{
		ResourceWarShopData &tShopData = m_pShopData->tResourceWarShop;
		if (tShopData.wBuyRecordNum < MAX_GOOGS_BUY_RECORD_NUM)
		{
			pRecord = &tShopData.arBuyRecord[tShopData.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	// GM商店
	else if (SHOP_GM == byType)
	{
		GMShopData &tShopData = m_pShopData->tGMShop;
		if (tShopData.wBuyRecordNum < MAX_GM_SHOP_GOODS_NUM)
		{
			pRecord = &tShopData.arBuyRecord[tShopData.wBuyRecordNum++];
			pRecord->wGoodsId = wGoodsId;
			pRecord->wBuyNum = wGoodsNum;
			return true;
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]没有处理的商店类型![type:%d]", __FUNCTION__, byType);
		bIsListFull = false;
	}

	if (bIsListFull)
		MAPLOG(ERROR_LEVEL, "[%s]商店购买记录已满![type:%d]", __FUNCTION__, byType);

	return false;
}

// 发送商品购买记录列表
void CShopManager::SendGoodsBuyRecordList(BYTE byType, BYTE byRecordNum, ShopGoodsBuyRecord *pRecordList)
{
	MSG_S2C_SHOP_GOODS_BUY_RECORD_RESP msg;
	msg.m_byType = byType;
	byRecordNum = min(byRecordNum, MAX_GOOGS_BUY_RECORD_NUM);
	msg.m_byBuyRecordNum = byRecordNum;
	memcpy_s(msg.m_arBuyRecord, MAX_GOOGS_BUY_RECORD_NUM * sizeof(ShopGoodsBuyRecord), 
		pRecordList, byRecordNum *sizeof(ShopGoodsBuyRecord));
	m_pPlayer->SendPacket(&msg);

	MAPLOG(ERROR_LEVEL, "[%s]发送商店购买记录已满![type:%d,RecordNum:%d]", __FUNCTION__, byType, byRecordNum);
}

// 重置商店数据
void CShopManager::ResetShopData(BYTE byShopType)
{
	switch (byShopType)
	{
	case SHOP_AWAKEN:
		m_pShopData->tAwakenShop.wGoodsNum = 0;
		break;
	case SHOP_STAR_SOUL:
		m_pShopData->tStarSoulShop.wGoodsNum = 0;
		break;
	case SHOP_HERO:
		m_pShopData->tHeroShop.wGoodsNum = 0;
		break;
	case SHOP_DIRECT_BUY:
		m_pShopData->tDirectBuyShop.wGoodsNum = 0;
		break;
	case SHOP_CRYSTAL:
		m_pShopData->tCrystalShopData.wGoodsNum = 0;
		break;
	default:
		MAPLOG(ERROR_LEVEL, "[%s]没有处理的商店类型![type:%d]", __FUNCTION__, byShopType);
		break;
	}
}

// 检测是否购买数量限制
bool CShopManager::CheckIsBuyNumLimit(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wBuyNumMax)
{
	// 没有购买数量限制
	if (!wBuyNumMax)
		return false;

	// 购买已达上限
	if (wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > wBuyNumMax)
		return true;

	return false;
}

// 发送直购商店数据
void CShopManager::SendDirectBuyShopData()
{
	MSG_S2C_DIRECT_SHOP_DATA_RESP msg;
	msg.m_ShopData = m_pShopData->tDirectBuyShop;
	m_pPlayer->SendPacket(&msg);
}