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
	// Ӣ���̵�
	//m_pShopData->tHeroShop.wFreeRefreshTimes = 0;
	m_pShopData->tHeroShop.wCostRefreshTimes = 0;

	// �����̵�����
	//m_pShopData->tGuildShop.wAwardGoodsRecordNum = 0;
	m_pShopData->tGuildShop.wItemGoodsRecordNum = 0;
	m_pShopData->tGuildShop.wLimitGoodsRecordNum = 0;


	// �̳�
	//m_pShopData->tMallShop.byGiftBuyRecordNum = 0;
	m_pShopData->tMallShop.wItemBuyRecordNum = 0;

	// �����̵�
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

	// �������̵�
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

	// �����̵�
	m_pShopData->tAwakenShop.wFreeRefreshTimes = 0;
	m_pShopData->tAwakenShop.wCostRefreshTimes = 0;

	// �ǻ��̵�
	m_pShopData->tStarSoulShop.wUsedRefreshTimes = 0;

	// �������̵�
	m_pShopData->tScoreMatchShop.wBuyRecordNum = 0;

	// �޾�֮�ص����̵�
	m_pShopData->tEndlessLandShop.wItemGoodsBuyRecordNum = 0;

	// �һ��̵�
	m_pShopData->tExchangeShop.wBuyRecordNum = 0;

	// �����ͨ�̵�
	m_pShopData->tTeamDungeonShop.wNormalGoodsBuyRecordNum = 0;

	// ˮ���̵�
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

// �Ƿ��к��
bool CShopManager::IsHaveRedPoint(BYTE byShopType)
{
	// Ӣ���̵�
	if (SHOP_HERO == byShopType)
	{
		UpdateShopRefreshFreeTimes(byShopType);

		// ��Ӣ����Ƭ�̵����ˢ�´����ﵽ����ʱ
		if (!m_pShopData->tHeroShop.wFreeRefreshTimes)
			return true;
	}
	// ֱ���̵�
	else if (SHOP_DIRECT_BUY == byShopType)
	{
		//UpdateDirectBuyShop(false);

		if (m_pShopData->tDirectBuyShop.wIsNewGoods)
			return true;
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д�����̵���Ʒ����![type:%d]", __FUNCTION__, byShopType);
	}

	return false;
}

// ��Ʒ����
void CShopManager::OnGoodsBuy(BYTE byType, WORD wGoodsId, WORD wBuyNum, WORD wContext)
{
	// ͳһ�Թ������������
	if (!wBuyNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�������Ʒ����Ϊ0![RoleId:%d,ShopType:%d,GoodsId:%d]", 
			__FUNCTION__, m_pPlayer->GetRoleId(), byType, wGoodsId);
		return;
	}

	WORD wFromMsgId = C2S_SHOP_GOODS_BUY_REQ;

	bool bResult;
	switch (byType)
	{
	// �̳ǵ����̵�
	case SHOP_MALL_ITEM:		
		bResult = MallItemBuy(byType, wGoodsId, wBuyNum);
		break;

	// �̳�����̵�
	case SHOP_MALL_GIFT:		
		bResult = MallGiftBuy(byType, wGoodsId, wBuyNum);
		break;

	case SHOP_GUILD_ITEM:		// ���������Ʒ
	case SHOP_GUILD_LIMIT:		// ������ʱ��Ʒ
	case SHOP_GUILD_AWARD:		// ���ά����Ʒ
		bResult = GuildGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// Ӣ���̵�
	case SHOP_HERO:
		bResult = HeroGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// �Ѿ��̵�
	case SHOP_REBEL:
		bResult = RebelGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// ������Ʒ
	case SHOP_TOWER:
		bResult = TowerGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// ��������Ʒ
	case SHOP_ARENA:
		bResult = ArenaGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// ������Ʒ
	case SHOP_AWAKEN:
		bResult = AwakenGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// �ǻ��̵�
	case SHOP_STAR_SOUL:
		bResult = StarSoulGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// ��������Ʒ
	case SHOP_SCORE_MATCH:
		bResult = ScoreMatchGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// 7���̵�
	case SHOP_SEVEN_DAY:
		bResult = SevenDayShopBuy(byType, wGoodsId, wBuyNum);
		break;

	// �ȼ�����̵�
	case SHOP_LEVEL_GIFT:
		bResult = LevelGiftShopBuy(byType, wGoodsId, wBuyNum, wContext);
		break;

	// �޾�֮�ص����̵�
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

	// ��Դս�̵�
	case SHOP_RESOURCE_WAR_SHOP:
		bResult = ResourceWarGoodsBuy(byType, wGoodsId, wBuyNum);
		break;

	// GM�̵�
	case SHOP_GM:
		bResult = GMShopGoodsBuy(byType, wGoodsId, wBuyNum, wContext);
		break;

	default:
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL,"[%s]ľ�д�����̵���Ʒ����![type:%d]",__FUNCTION__,byType);
		return;
	}

	if (!bResult)
	{ 
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ����ʧ��![type:%d]", __FUNCTION__, byType);
		return;
	}

	MSG_S2C_SHOP_GOODS_BUY_RESP msg;
	msg.m_byType = byType;
	msg.m_wParam1 = wGoodsId;
	msg.m_wParam2 = wBuyNum;
	msg.m_wParam3 = wContext;
	m_pPlayer->SendPacket(&msg);

	// �����Ʒ�������
	ShopBuyRecord_Save* pQuery = ShopBuyRecord_Save::ALLOC();
	pQuery->SetIndex(DB_SHOP_BUY_RECORD_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	pQuery->SetGoodsId(wGoodsId);
	pQuery->SetShopType(byType);
	pQuery->SetBuyNum(wBuyNum);
	g_MapServer.MapDBQuery(pQuery);

	// ������Ʒ����ͳ��
	CGameLogic::Instance()->OnGoodsBuyStatistics(m_pPlayer, byType, wGoodsId, wBuyNum);
}

// ������Ʒ�����¼
void CShopManager::OnGoodsBuyRecord(BYTE byType)
{
	WORD wFromMsgId = C2S_SHOP_GOODS_BUY_RECORD_REQ;

	WORD wBuyRecordNum = 0;
	ShopGoodsBuyRecord *pBuyRecordList = NULL;

	// ��ʱ���� Ŀǰ������ʱ�̵�ʹ��
	//ShopGoodsBuyRecord arTempBuyRecord[MAX_GOOGS_BUY_RECORD_NUM];
	//WORD wTempBuyRecordNum = 0;

	switch (byType)
	{
	case SHOP_MALL_ITEM:		// �̳ǵ�����Ʒ
		wBuyRecordNum = m_pShopData->tMallShop.wItemBuyRecordNum;
		pBuyRecordList = m_pShopData->tMallShop.arItemBuyRecord;
		break;

	case SHOP_MALL_GIFT:		// �̳������Ʒ
		wBuyRecordNum = m_pShopData->tMallShop.wGiftBuyRecordNum;
		pBuyRecordList = m_pShopData->tMallShop.arGiftBuyRecord;
		break;

	case SHOP_GUILD_ITEM:		// ���������Ʒ
		wBuyRecordNum = m_pShopData->tGuildShop.wItemGoodsRecordNum;
		pBuyRecordList = m_pShopData->tGuildShop.arItemGoodsRecord;
		break;

	case SHOP_GUILD_LIMIT:		// ������ʱ��Ʒ
		wBuyRecordNum = m_pShopData->tGuildShop.wLimitGoodsRecordNum;
		pBuyRecordList = m_pShopData->tGuildShop.arLimitGoodsRecord;
		break;

	case SHOP_GUILD_AWARD:		// ���ά����Ʒ
		wBuyRecordNum = m_pShopData->tGuildShop.wAwardGoodsRecordNum;
		pBuyRecordList = m_pShopData->tGuildShop.arAwardGoodsRecord;
		break;

	case SHOP_HERO:		// �ۻ���Ʒ(������Ҫ���⴦��)
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

	// �ǻ��̵�
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

	// ֱ���̵�
	case SHOP_DIRECT_BUY:
		wBuyRecordNum = m_pShopData->tDirectBuyShop.wGoodsNum;
		pBuyRecordList = m_pShopData->tDirectBuyShop.arGoodsList;
		break;

	// �һ��̵�
	case SHOP_EXCHANGE:
		wBuyRecordNum = m_pShopData->tExchangeShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tExchangeShop.arBuyRecord;
		break;

	// �Ƽ��̵�
	case SHOP_RECOMMEND:
		wBuyRecordNum = m_pShopData->tRecommendShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tRecommendShop.arBuyRecord;
		break;

	// �����ͨ�̵�
	case SHOP_TEAM_DUNGEON_NORMAL:
		wBuyRecordNum = m_pShopData->tTeamDungeonShop.wNormalGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tTeamDungeonShop.arNormalGoodsBuyRecord;
		break;

		// ��ӽ����̵�
	case SHOP_TEAM_DUNGEON_REWARD:
		wBuyRecordNum = m_pShopData->tTeamDungeonShop.wRewardGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tTeamDungeonShop.arRewardGoodsBuyRecord;
		break;

		// ʥ���ڻ��ֶһ��̵�
	case SHOP_CHRISTMAS_SCORE_EXCHANGE:
		wBuyRecordNum = m_pShopData->tChristmasShop.wScoreExchangeGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tChristmasShop.arScoreExchangeGoodsBuyRecord;
		break;

		// ʥ������Ʒ�һ��̵�
	case SHOP_CHRISTMAS_ITEM_EXCHANGE:
		wBuyRecordNum = m_pShopData->tChristmasShop.wItemExchangeGoodsBuyRecordNum;
		pBuyRecordList = m_pShopData->tChristmasShop.arItemExchangeGoodsBuyRecord;
		break;

		// ˮ���̵�
	case SHOP_CRYSTAL:
		wBuyRecordNum = m_pShopData->tCrystalShopData.wGoodsNum;
		pBuyRecordList = m_pShopData->tCrystalShopData.arGoodsList;
		break;

		// ��Դս�̵�
	case SHOP_RESOURCE_WAR_SHOP:
		wBuyRecordNum = m_pShopData->tResourceWarShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tResourceWarShop.arBuyRecord;
		break;

		// GM�̵�
	case SHOP_GM:
		wBuyRecordNum = m_pShopData->tGMShop.wBuyRecordNum;
		pBuyRecordList = m_pShopData->tGMShop.arBuyRecord;
		break;

	default:
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д�����̵���Ʒ����![type:%d]", __FUNCTION__, byType);
		return;
	}

	SendGoodsBuyRecordList(byType, wBuyRecordNum, pBuyRecordList);
}

// ��Ʒˢ��
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
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д�����̵���Ʒ����![type:%d]", __FUNCTION__, byShopType);
		return;
	}

	MSG_S2C_SHOP_REFRESH_RESP msg;
	msg.m_byShopType = byShopType;
	msg.m_byRefreshType = byRefreshType;
	m_pPlayer->SendPacket(&msg);
}

// ����
void CShopManager::OnGoodsSell(BYTE byObjType, DWORD *pGoodsList, BYTE byGoodsNum)
{
	WORD wFromMsgId = C2S_SHOP_GOODS_SELL_REQ;

	bool bRet = false;

	vector<MoneyData> vCurrencyList;

	// ��Ƭ(��Ʒ)
	if (OBJECT_ITEM == byObjType)
	{
		bRet = DoSellItem(pGoodsList, byGoodsNum, vCurrencyList);
	}
	// װ��
	else if (OBJECT_EQUIPMENT == byObjType)
	{
		bRet = DoSellEquipemnt(pGoodsList, byGoodsNum, vCurrencyList);
	}
	// Ӣ��
	else if (OBJECT_HERO == byObjType)
	{
		bRet = DoSellHero(pGoodsList, byGoodsNum, vCurrencyList);
	}
	// ����
	else if (OBJECT_TREASURE == byObjType)
	{
		bRet = DoSellTreasure(pGoodsList, byGoodsNum, vCurrencyList);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]δ����Ķ�������[ObjectType:%d]", __FUNCTION__, byObjType);
		return;
	}

	if (!bRet)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒʧ��![ObjectType:%d]", __FUNCTION__, byObjType);
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

// ������ʱ��Ʒ�б�
void CShopManager::OnNetLimitGoodsList(BYTE byType)
{
	WORD wFromMsgId = C2S_SHOP_LIMIT_GOODS_LIST_REQ;

	if (SHOP_GUILD_LIMIT == byType)
	{
		CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(m_pPlayer->GetRoleId());
		if (!pGuild)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]ľ���ڹ��᲻���빤����Ʒ�б�![type:%d]", __FUNCTION__, byType);
			return;
		}

		pGuild->SendLimitGoodsList(m_pPlayer);
	}
	else
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]��ʱľ�д�����̵���Ʒ����![type:%d]", __FUNCTION__, byType);
		return;
	}
}

// ����ͨ����Ʒ����
bool CShopManager::DoCommonGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wItemFromId, WORD wCurrencyFromId)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ�����Ʒ[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ����������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����������
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ����������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ʒ�����Ѵ�����[ShopType:%d,GoodsId:%d,BuyNum:%d,MaxNum:%d,AlreadyBuyNum:%d]", 
			__FUNCTION__, byShopType, wGoodsId, wBuyNum, pGoodsCfg->wBuyNumMax, GetGoodsBuyRecordNum(byShopType, wGoodsId));
		return false;
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// �����������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, wCurrencyFromId);

	// �����������
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pGoodsCfg->vCostItemList[i], wItemFromId);
	}

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, wItemFromId);

	return true;
}

// �̳ǵ��߹���
bool CShopManager::MallItemBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// ȡ��Ʒ����
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	WORD wBuyNumLimit = pGoodsCfg->nVipIdx ? m_pPlayer->GetVipValue(pGoodsCfg->nVipIdx - 1) : pGoodsCfg->wBuyNumMax;

	// ȡ�����¼
	WORD wBuyRecordNum = GetGoodsBuyRecordNum(byShopType, wGoodsId);

	// �����չ������
	if (wBuyNumLimit && (wBuyRecordNum + wBuyNum) > wBuyNumLimit)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���չ�������Ѵ�����![GoodsId:%d,MaxBuyNum:%d]",__FUNCTION__, wGoodsId, wBuyNumLimit);
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

	// �������(��Ǯ)
	if (m_pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ��ʯ����![GoodsId:%d,CostDiamond:%d]", __FUNCTION__, wGoodsId, nCostDiamond);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		// ��ɫ�ȼ�����
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ��ɫ�ȼ�����![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_MALL_ITEM_BUY);

	// ��������(��Ǯ)
	m_pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_MALL_ITEM_BUY);

	// ���¹������(�й���������Ʋ���Ҫ��¼)
	if (wBuyNumLimit)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������(�ճ������¼�)
	if (pGoodsCfg->dwEntityId == g_GetConfigValue(282))
	{
		m_pPlayer->OnUpdateDailyMission(6, wBuyNum);
	}

	// ������(�ճ������¼�)
	if (pGoodsCfg->dwEntityId == g_GetConfigValue(283))
	{
		m_pPlayer->OnUpdateDailyMission(7, wBuyNum);
	}

	return true;
}

// �̳��������
bool CShopManager::MallGiftBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// ȡ��Ʒ����
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		// �Ҳ�����Ʒ����
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��⹺�����
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		// ���չ�������Ѵ�����
		MAPLOG(ERROR_LEVEL, "[%s]�̳�������������������![GoodsId:%d,MaxBuyNum:%d]",
			__FUNCTION__, wGoodsId, pGoodsCfg->wBuyNumMax);
		return false;
	}

	// �������(��Ǯ)
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		// ��ɫ�ȼ�����
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ��ɫ�ȼ�����![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �����Ҫ��VIP�ȼ�
	if (m_pPlayer->GetProperty(ROLE_PRO_VIP_LEVEL) < pGoodsCfg->nNeedVipLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̳���ƷVIP�ȼ����Ʋ��ܹ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_MALL_ITEM_BUY);

	// ��������(��Ǯ)
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_MALL_ITEM_BUY);

	// ���¹������
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}

// �����ۻ��̵�����
void CShopManager::OnHeroShopReq()
{
	// �̵����ݻ�û��ʼ��
	if (!m_pShopData->tHeroShop.wGoodsNum || !m_pShopData->tHeroShop.dwLastRecoverTime)
	{
		CreateRefreshShopGoods(SHOP_HERO);
		m_pShopData->tHeroShop.dwLastRecoverTime = time(NULL);
	}

	UpdateShopRefreshFreeTimes(SHOP_HERO);

	// �����̵�����
	SendRefreshShopData(SHOP_HERO);
}

// �����̵���������
void CShopManager::OnNetAwakenShopReq()
{
	// �̵����ݻ�û��ʼ��
	if (!m_pShopData->tAwakenShop.wGoodsNum || !m_pShopData->tAwakenShop.dwLastRecoverTime)
	{
		CreateRefreshShopGoods(SHOP_AWAKEN);
		m_pShopData->tAwakenShop.dwLastRecoverTime = time(NULL);
	}

	UpdateShopRefreshFreeTimes(SHOP_AWAKEN);

	// �����̵�����
	SendRefreshShopData(SHOP_AWAKEN);
}

// ˢ���ۻ��̵�
void CShopManager::OnHeroSoulShopRefresh(BYTE byShopType, BYTE byRefreshType)
{
	UpdateShopRefreshFreeTimes(byShopType);
	
	RefreshShopData &tShopData = m_pShopData->tHeroShop;

	// ʹ����Ѵ���
	if (1 == byRefreshType)
	{
		BYTE byRefreshTimesMax = BYTE(g_GetConfigValue(CONFIG_HEROSOUL_SHOP_REFRESH_FREE_TIMES));
		if (tShopData.wFreeRefreshTimes >= byRefreshTimesMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		// ����
		tShopData.wFreeRefreshTimes++;
	}
	// ʹ����Ʒ
	else if (2 == byRefreshType)
	{
		CItemManager *pItemMgr = m_pPlayer->GetItemManager();
		CHECK_POINTER(pItemMgr);

		// ���ˢ�´�������
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_HERO_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]Ӣ���̵�����ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		// ˢ�����
		DWORD dwCostItemId = g_GetConfigValue(CONFIG_SHOP_REFRESH_ITEM_ID);
		if (!pItemMgr->GetItemNum(dwCostItemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ��Ӣ���̵����ĵ�ˢ�����", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// ����
		pItemMgr->RemoveItem(dwCostItemId, 1, TRUE, FROM_HERO_SHOP_REFRESH);
	}
	// ��Ʒ�ۻ�
	else
	{
		// ���ˢ�´�������
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_HERO_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		// �ۻ겻��
		int nCostHeroSoul = int(g_GetConfigValue(CONFIG_HERO_SHOP_REFRESH_COST_HEROSOUL));
		if (m_pPlayer->GetProperty(ROLE_PRO_HERO_SOUL) < nCostHeroSoul)
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ��Ӣ���̵����ĵ��ۻ겻��", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// ����
		m_pPlayer->ModifyProperty(ROLE_PRO_HERO_SOUL, -nCostHeroSoul, TRUE, FROM_HERO_SHOP_REFRESH);
	}

	// ����������Ʒ
	CreateRefreshShopGoods(byShopType);

	// �����̵�����
	SendRefreshShopData(byShopType);

	// �ɾ��¼�
	m_pPlayer->OnUpdateAchievement(13);

	// 7���¼�(Ӣ���̵�ˢ�´���)
	m_pPlayer->OnUpdateSevenDayTarget(8, 1, true);
}

// �����̵�ˢ��
void CShopManager::OnNetAwakenShopRefresh(BYTE byRefreshType)
{
	BYTE byShopType = SHOP_AWAKEN;

	UpdateShopRefreshFreeTimes(byShopType);

	RefreshShopData &tShopData = m_pShopData->tAwakenShop;

	// ʹ����Ѵ���
	if (1 == byRefreshType)
	{
		BYTE byRefreshTimesMax = BYTE(g_GetConfigValue(CONFIG_HEROSOUL_SHOP_REFRESH_FREE_TIMES));
		if (tShopData.wFreeRefreshTimes >= byRefreshTimesMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����̵����ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		// ����
		tShopData.wFreeRefreshTimes++;
	}
	// ʹ����Ʒ
	else if (2 == byRefreshType)
	{
		// ���ˢ�´�������(todo::Ŀǰ���ۻ��̵�һ������)
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_HERO_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		CItemManager *pItemMgr = m_pPlayer->GetItemManager();
		CHECK_POINTER(pItemMgr);

		// ˢ�����
		DWORD dwCostItemId = g_GetConfigValue(CONFIG_SHOP_REFRESH_ITEM_ID);
		if (!pItemMgr->GetItemNum(dwCostItemId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ�¾����̵����ĵ�ˢ�����", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// ����
		pItemMgr->RemoveItem(dwCostItemId, 1, TRUE, FROM_AWAKEN_SHOP_REFRESH);
	}
	// ʹ�����
	else
	{
		// ���ˢ�´�������(todo::Ŀǰ���ۻ��̵�һ������)
		if (tShopData.wCostRefreshTimes >= m_pPlayer->GetVipValue(VIP_AWAKEN_SHOP_REFRESH_TIMES))
		{
			MAPLOG(ERROR_LEVEL, "[%s]����ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		// ��겻��
		int nCostGodSoul = int(g_GetConfigValue(CONFIG_AWAKEN_SHOP_REFRESH_COST_HEROSOUL));
		if (m_pPlayer->GetProperty(ROLE_PRO_GOD_SOUL) < nCostGodSoul)
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ�¾����̵����ĵ���겻��", __FUNCTION__);
			return;
		}

		tShopData.wCostRefreshTimes++;

		// ����
		m_pPlayer->ModifyProperty(ROLE_PRO_GOD_SOUL, -nCostGodSoul, TRUE, FROM_AWAKEN_SHOP_REFRESH);
	}

	// ����������Ʒ
	CreateRefreshShopGoods(byShopType);

	// �����̵�����
	SendRefreshShopData(byShopType);

	// �ɾ��¼�
	//m_pPlayer->OnUpdateAchievement(13);
}

// �����̵���������
void CShopManager::OnNetStarSoulShopReq()
{
	// ����Ƿ���Ҫˢ��
	DWORD dwCurTime = time(NULL);
	if (dwCurTime >= m_pShopData->tStarSoulShop.dwLastAutoRefreshTime + g_GetConfigValue(244) * 60)
	{
		CreateRefreshShopGoods(SHOP_STAR_SOUL);

		m_pShopData->tStarSoulShop.dwLastAutoRefreshTime = dwCurTime;
	}

	// �����̵�����
	SendRefreshShopData(SHOP_STAR_SOUL);
}

// �����̵�ˢ��
void CShopManager::OnNetStarSoulShopRefresh(BYTE byRefreshType)
{
	BYTE byShopType = SHOP_STAR_SOUL;

	StarSoulShopData &tShopData = m_pShopData->tStarSoulShop;


	// �Զ�ˢ��(ʱ�䵽)
	if (1 == byRefreshType)
	{
		DWORD dwCurTime = time(NULL);
		if (dwCurTime < tShopData.dwLastAutoRefreshTime + g_GetConfigValue(244) * 60)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Զ�ˢ��ʱ�仹û��", __FUNCTION__);
			return;
		}

		tShopData.dwLastAutoRefreshTime = dwCurTime;
	}
	else
	{
		// ������
		if (tShopData.wUsedRefreshTimes >= g_GetConfigValue(245))
		{
			MAPLOG(ERROR_LEVEL, "[%s]���������̵�ˢ�´����Ѵ�����", __FUNCTION__);
			return;
		}

		int nCostStarSoul = g_GetConfigValue(246);

		// �������
		if (m_pPlayer->GetProperty(ROLE_PRO_STAR_SOUL) < nCostStarSoul)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����̵�ˢ�������ɻ겻��", __FUNCTION__);
			return;
		}

		m_pPlayer->ModifyProperty(ROLE_PRO_STAR_SOUL, -nCostStarSoul);

		tShopData.wUsedRefreshTimes++;
	}

	// ����������Ʒ
	CreateRefreshShopGoods(byShopType);

	// �����̵�����
	SendRefreshShopData(byShopType);
}

// ˮ���̵���������
void CShopManager::OnNetCrystalShopData()
{
	BYTE byShopType = SHOP_CRYSTAL;

	// �̵����ݳ�ʼ��
	if (!m_pShopData->tCrystalShopData.wGoodsNum)
		CreateRefreshShopGoods(byShopType);

	// �����̵�����
	SendRefreshShopData(byShopType);
}

// ˮ���̵�ˢ��
void CShopManager::OnNetCrystalShopRefresh()
{
	BYTE byShopType = SHOP_CRYSTAL;

	// ���ˢ�´���
	if (m_pShopData->tCrystalShopData.wTodayRefreshTimes >= g_GetConfigValue(278))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ˮ���̵����ˢ�´�������[times:%d]", 
			__FUNCTION__, m_pShopData->tCrystalShopData.wTodayRefreshTimes);
		return;
	}

	m_pShopData->tCrystalShopData.wTodayRefreshTimes++;

	// ����������Ʒ
	CreateRefreshShopGoods(byShopType);

	// �����̵�����
	SendRefreshShopData(byShopType);
}

// ֱ���̵���������
void CShopManager::OnNetDirectBuyShopReq()
{
	//UpdateDirectBuyShop(true);
	// todo::Ӧ�ò���
	if (!m_pShopData->tDirectBuyShop.wGoodsNum)
		CreateRefreshShopGoods(SHOP_DIRECT_BUY);

	SendDirectBuyShopData();
}

// ����ֱ���̵�����
void CShopManager::UpdateDirectBuyShop(WORD wUpdateType)
{
	WORD wShopType = SHOP_DIRECT_BUY;

	// �������ˢ����Ʒ
	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetRefreshShopSlotList(wShopType);
	if (!pSlotListCfg || !pSlotListCfg->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]ˢ���̵���Ʒ��λ�б�����Ϊ��", __FUNCTION__);
		return;
	}

	const ShopGoodsData_Config *pGoodsCfg;

	// 0=��ˢ�� 1=��ˢ�� 2=��ˢ��
	DirectBuyShopData &tDirectBuyShop = m_pShopData->tDirectBuyShop;
	for (WORD i = 0; i < tDirectBuyShop.wGoodsNum; ++i)
	{
		ShopGoodsBuyRecord &tRecord = tDirectBuyShop.arGoodsList[i];

		// ȡ��Ʒ����
		pGoodsCfg = CConfigManager::Instance()->GetShopGoods(wShopType, tRecord.wGoodsId);
		if (!pGoodsCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����![GoodsId:%d]", __FUNCTION__, tRecord.wGoodsId);
			continue;
		}

		// ���Ǳ���Ҫˢ�µ�������Ʒ
		if (wUpdateType != pGoodsCfg->wRefreshType)
			continue;

		// todo::��������Ʒ����, ��һˢ��ʧ����,�ͼ���ʹ��ԭ��Ʒ
		tRecord.wBuyNum = 0;

		// ��λԽ��
		if (i >= pSlotListCfg->size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]�湺�̵��λԽ��![slot:%d]", __FUNCTION__, i);
			return;
		}

		vector<const ShopGoodsData_Config*> vRandList;
		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];

		WORD wTotalRate = 0;
		// ȡɸѡ����б���Ʒ
		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
			if (pGoodsCfg && m_pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel)
			{
				wTotalRate += pGoodsCfg->wRefreshRate;
				vRandList.push_back(pGoodsCfg);
			}
		}

		// ����ܸ��ʼ�����б��Ƿ�Ϸ�
		if (!wTotalRate || !vRandList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ���̵���Ʒɸ����б�Ϊ��![TotalRate:%d,ListSize:%d,RoleLv:%d]",
				__FUNCTION__, wTotalRate, vRandList.size(), m_pPlayer->GetLevel());
			return;
		}

		// �����Ʒ
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

// ����ֱ���̵����� // ���߼�
// void CShopManager::UpdateDirectBuyShop(bool bIsClientRequst)
// {
// 	// �������Ʒ����ȡ��ˢ��
// 	if (m_pShopData->tDirectBuyShop.wCanTakeGoodsNum)
// 		return;
// 
// 
// 	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(201);
// 	if (!pServerCfg || !pServerCfg->vValueList.size())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ���̵�ˢ��ʱ������", __FUNCTION__);
// 		return;
// 	}
// 
// 	bool bIsRefreshNewGoods = false;
// 	tm tDetailTime;
// 	GetTMTime(&tDetailTime, time(NULL));
// 	int nCurTime = tDetailTime.tm_hour * 100 + tDetailTime.tm_min;
// 	for (int i = pServerCfg->vValueList.size() - 1; i >= 0; --i)
// 	{
// 		int nRefreshTime = pServerCfg->vValueList[i]; // ˢ��ʱ��
// 		if (nCurTime >= nRefreshTime)
// 		{
// 			// 1.�������һ��ˢ��ʱ��β�һ��  2.�������һ��ˢ�Ĳ���ͬһ��
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
// 	// ����ǿͻ�������,���Ҳ��Ǹո�ˢ����Ʒ
// 	if (bIsClientRequst && !bIsRefreshNewGoods)
// 		m_pShopData->tDirectBuyShop.wIsNewGoods = false;
// }

// ��ȡֱ����Ʒ
void CShopManager::OnNetTakeDirectBuyGoods(WORD wGoodsId)
{
	WORD wFromMsgId = C2S_TAKE_DIRECT_BUY_GOODS_REQ;

	DirectBuyShopData &tShopData = m_pShopData->tDirectBuyShop;

	// ����Ƿ����ȡ
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
		MAPLOG(ERROR_LEVEL, "[%s]��ֱ����Ʒ������ȡ[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	const ShopGoodsData_Config *pGoodsCfg = g_pCfgMgr->GetShopGoods(SHOP_DIRECT_BUY, wGoodsId);
	if (!pGoodsCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);;
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ����Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return;
	}

	// ɾ������ȡ����Ʒ
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

// �����ۻ��̵���Ʒ
bool CShopManager::HeroGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ���̵���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// �̵����ݻ�û��ʼ��
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ۻ���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ������������
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���̵깺�������Ѵ�����[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����Ӣ����Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_HERO);

 	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_HERO);
 
	// todo::�˴����������⣬Ӧ����Ҫ����
	pGoods->wBuyNum = wBuyNum;

	SendRefreshShopData(SHOP_HERO);

	// �ճ������¼�
	m_pPlayer->OnUpdateDailyMission(5, wBuyNum);

	// ����ȼ�����
	//m_pPlayer->OnUpdateWorldTaskEvent(5);

	return true;
}

// �Ѿ���Ʒ����
bool CShopManager::RebelGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����Ѿ���Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}


	// ����������
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ����������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �����������
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		m_pPlayer->RemoveItem(pGoodsCfg->vCostItemList[i].dwId,pGoodsCfg->vCostItemList[i].dwNum * wBuyNum, FROM_SHOP_REBEL);
	}


	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_REBEL);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_REBEL);

	return true;
}

// ������Ʒ����
bool CShopManager::TowerGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// todo::�������

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ���Ĳ���[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ����������
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ����������Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��⹺���������(����������Ʒ)
// 	ShopGoodsBuyRecord *pBuyRecord = GetGoodsBuyRecord(byShopType, wGoodsId);
// 	if (pBuyRecord && pGoodsCfg->wBuyNumMax && pBuyRecord->wBuyNum >= pGoodsCfg->wBuyNumMax)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�����������[GoodsId:%d]", __FUNCTION__, wGoodsId);
// 		return false;
// 	}

	// ��⹺�����
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		// ���չ�������Ѵ�����
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�����������[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �����������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_TOWER);

	// �����������
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
		m_pPlayer->RemoveItem(pGoodsCfg->vCostItemList[i].dwId, pGoodsCfg->vCostItemList[i].dwNum * wBuyNum, FROM_SHOP_TOWER);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_TOWER);

	// �����¼
	if (pGoodsCfg->byType == 4)
		AddGoodsBuyRecord(SHOP_TOWER, wGoodsId, wBuyNum);

	return true;
}

// ��������Ʒ����
bool CShopManager::ArenaGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[Shop:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	WORD wHistoryRank = CArenaManager::Instance()->GetPlayerHistoryRank(m_pPlayer->GetServerId(), m_pPlayer->GetCharID());
	// ����Ƿ�ľ���������
	if (pGoodsCfg->nContext && wHistoryRank > pGoodsCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������Ʒ��������������!", __FUNCTION__);
		return false;
	}

	// �������(����)
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������Ʒ����������Ĳ���!", __FUNCTION__);
		return false;
	}

	// �������(����)
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������Ʒ����������Ĳ���!", __FUNCTION__);
		return false;
	}

	// �����������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��������Ʒ��������Ѵ�����!", __FUNCTION__);
		return false;
	}

	// �������(����)
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_ARENA);

	// �������(����)
	for (int i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item *pItem = &pGoodsCfg->vCostItemList[i];
		m_pPlayer->RemoveItem(pItem->dwId, pItem->dwNum * wBuyNum, FROM_SHOP_ARENA);
	}

	// ���¹����¼
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_ARENA);

	return true;
}

// ������Ʒ����
bool CShopManager::AwakenGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ���̵���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
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

	// �̵����ݻ�û��ʼ��
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ������������
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����̵깺�������Ѵ�����[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]���������Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_AWAKEN);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_AWAKEN);

	pGoods->wBuyNum = wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// �ǻ���Ʒ����
bool CShopManager::StarSoulGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ��������̵���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// �̵����ݻ�û��ʼ��
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ������������
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����̵깺�������Ѵ�����[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����������Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, 1, FROM_SHOP_STAR_SOUL);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_STAR_SOUL);

	pGoods->wBuyNum = wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// ��������Ʒ����
bool CShopManager::ScoreMatchGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	return DoCommonGoodsBuy(byShopType, wGoodsId, wBuyNum, FROM_SHOP_SCORE, FROM_SHOP_SCORE);
}

// 7���̵깺��
bool CShopManager::SevenDayShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// ����Ƿ��ڼ�


	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ�����Ʒ[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ʒ�����Ѵ�����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_SEVEN_DAY);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_SEVEN_DAY);

	return true;
}

// �ȼ�����̵깺��
bool CShopManager::LevelGiftShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wRoleLevel)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ�����Ʒ[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ʒ�����Ѵ�����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	const LevelShopData_Config *pLevelGiftDataCfg = g_pCfgMgr->GetLevelShopData(wRoleLevel);
	if (!pLevelGiftDataCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ȼ������������[RoleLevel:%d]", __FUNCTION__, wRoleLevel);
		return false;
	}

	// ���ʱ���Ƿ��ѹ���
	const LevelShopActivityData *pActivityData = m_pPlayer->GetActivityManager()->GetLevelShopActivityData(wRoleLevel);
	if (!pActivityData || time(NULL) >= (pActivityData->dwActivateTime + pLevelGiftDataCfg->nValidTime))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�ȼ���������ѹ���[RoleLevel:%d]", __FUNCTION__, wRoleLevel);
		return false;
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_LEVEL_GIFT);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_LEVEL_GIFT);

	return true;
}

// �޾�֮���̵�
bool CShopManager::EndlessLandShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ɱ����������
	if (m_pPlayer->GetRoleFullInfo().tSecondInfo.tEndlessLandData.wContinueKillNumRecord < pGoodsCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ɱ����������[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�����޾�֮����Ʒ���Ĳ���[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��⹺�����
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�޾�֮����Ʒ�����������[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_ENDLESS_LAND);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_ENDLESS_LAND);

	// �����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}

// ֱ���̵�ͨ����ֵ����
bool CShopManager::DirectBuyShopBuyByRecharge(WORD wGoodsId)
{
	BYTE byShopType = SHOP_DIRECT_BUY;
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ���̵���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// �̵����ݻ�û��ʼ��
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ����Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��ӿ�����ȡ����Ʒ����
	m_pShopData->tDirectBuyShop.AddCanTakeGoods(wGoodsId);

	pGoods->wBuyNum += 1;

	SendRefreshShopData(byShopType);

	return true;
}

// ֱ���̵깺��
bool CShopManager::DirectBuyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ���̵���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// �̵����ݻ�û��ʼ��
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ֱ����Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ������������
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ֱ���̵깺�������Ѵ�����[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ֱ����Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_DIRECT_BUY);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_DIRECT_BUY);
	
	// ��ӿ�����ȡ����Ʒ����
	//m_pShopData->tDirectBuyShop.AddCanTakeGoods(wGoodsId);

	pGoods->wBuyNum += wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// �һ��̵깺��
bool CShopManager::ExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[Shop:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�һ���Ʒ�������Ĳ���!", __FUNCTION__);
		return false;
	}

	// �����Ϣ����Ʒ
	for (size_t i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item &costItem = pGoodsCfg->vCostItemList[i];
		if (m_pPlayer->GetItemNum(costItem.dwId) < costItem.dwNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�һ���Ʒ�������ĵ���Ʒ����[CostItemId:%d,ConstNum:%d]!", 
				__FUNCTION__, costItem.dwId, costItem.dwNum);
		}
	}

	// �����������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�һ���Ʒ��������Ѵ�����!", __FUNCTION__);
		return false;
	}

	// �������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_EXCHANGE);

	// ������Ʒ
	for (size_t i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item &costItem = pGoodsCfg->vCostItemList[i];
		m_pPlayer->RemoveItem(costItem.dwId, costItem.dwNum, FROM_SHOP_EXCHANGE);
	}

	// ���¹����¼
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_EXCHANGE);

	return true;
}

// �Ƽ��̵깺��
bool CShopManager::RecommendShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// todo::����Ƿ��ڼ�


	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ�����Ʒ[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ʒ�����Ѵ�����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_RECOMMEND);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_RECOMMEND);

	return true;
}

// ����̵깺��
bool CShopManager::TeamDungeonNormalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// todo::��Ҫͨ�ظ���ID

	// ����ɫ�ȼ�
// 	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ�����Ʒ[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ʒ�����Ѵ�����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_TEAM_NORMAL);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_TEAM_NORMAL);

	return true;
}

// ��ӽ����̵깺��
bool CShopManager::TeamDungeonRewardShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
// 	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ�����Ʒ[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// todo::��Ҫͨ�ظ���ID
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
// 		MAPLOG(ERROR_LEVEL, "[%s]��ûͨ��[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺������
	if (pGoodsCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ʒ�����Ѵ�����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_TEAM_REWARD);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_TEAM_REWARD);

	return true;
}

// �����̵���Ʒ����
bool CShopManager::DestinyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_DESTINY);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_DESTINY);

	return true;
}

// �����̵���Ʒ����
bool CShopManager::HideShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����������
	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ������߲���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �����������
	if (pGoodsCfg->vCostList.size())
		m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_HIDE);

	// �����������
	vector<Item>::const_iterator iter = pGoodsCfg->vCostItemList.begin();
	for (; iter != pGoodsCfg->vCostItemList.end(); ++iter)
	{
		m_pPlayer->RemoveItem(iter->dwId, iter->dwNum, FROM_SHOP_HIDE);
	}

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_HIDE);

	return true;
}

// ʥ���ڻ��ֶһ���Ʒ����
bool CShopManager::ChristmasScoreExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[Shop:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	PlayerChristmasData &tChristmasData = m_pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;

	// ������ĵĻ���
	if (tChristmasData.dwCurScore < pGoodsCfg->nSpecialCostValue * wBuyNum)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ҫ���ĵĻ��ֲ���,���ܹ���!", __FUNCTION__);
		return false;
	}

	// �����Ҫ����ʷ����
	if (tChristmasData.dwTotalScore < pGoodsCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��Ҫ���ĵĻ��ֲ���,���ܹ���!", __FUNCTION__);
		return false;
	}

	// �����������
	if (CheckIsBuyNumLimit(byShopType, wGoodsId, wBuyNum, pGoodsCfg->wBuyNumMax))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ʥ���ڻ��ֶһ���Ʒ��������Ѵ�����!", __FUNCTION__);
		return false;
	}

	// ���Ļ���
	tChristmasData.dwCurScore -= (pGoodsCfg->nSpecialCostValue * wBuyNum);

	// ͬ����������
	CChristmas::Instance()->SendChristmasData(m_pPlayer);

	// ���¹����¼
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_CHRISTMAS_SCORE_EXCHANGE);

	return true;
}

// ʥ������Ʒ�һ���Ʒ����
bool CShopManager::ChristmasItemExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����̵���Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ�������Ĳ���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �����������
	if (CheckIsBuyNumLimit(byShopType, wGoodsId, wBuyNum, pGoodsCfg->wBuyNumMax))
	{
		MAPLOG(ERROR_LEVEL, "[%s]ʥ������Ʒ�һ���Ʒ��������Ѵ�����!", __FUNCTION__);
		return false;
	}

	// ����������
	for (size_t i = 0; i < pGoodsCfg->vCostItemList.size(); ++i)
	{
		const Item &tCostItem = pGoodsCfg->vCostItemList[i];
		if (m_pPlayer->GetItemNum(tCostItem.dwId) < tCostItem.dwNum * wBuyNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����Ʒ���Ĳ���[CostItemId:%d,CostItemNum:%d]!",
				__FUNCTION__, tCostItem.dwId, tCostItem.dwNum * wBuyNum);
			return false;
		}
	}

// 	if (!m_pPlayer->CheckItemCost(pGoodsCfg->vCostItemList))
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]�̵���Ʒ������߲���![ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
// 		return false;
// 	}

	// �����������
	if (pGoodsCfg->vCostList.size())
		m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE);

	// �����������
	vector<Item>::const_iterator iter = pGoodsCfg->vCostItemList.begin();
	for (; iter != pGoodsCfg->vCostItemList.end(); ++iter)
	{
		m_pPlayer->RemoveItem(iter->dwId, iter->dwNum * wBuyNum, FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE);
	}

	// ���¹����¼
	if (pGoodsCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE);

	return true;
}

// ˮ���̵깺��
bool CShopManager::CrystalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���ˮ�����̵���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	ShopGoodsBuyRecord *pGoods = GetGoodsBuyRecord(byShopType, wGoodsId);

	// �̵����ݻ�û��ʼ��
	if (!pGoods)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ����ۻ���Ʒ����[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ������������
	if (pGoods->wBuyNum + wBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ˮ�����̵깺�������Ѵ�����[GoodsId:%d]", __FUNCTION__, pGoods->wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ˮ������Ʒ���Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_CRYSTAL);

	// �����Ʒ
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, pGoodsCfg->dwEntityNum * wBuyNum, TRUE, FROM_SHOP_CRYSTAL);

	// todo::�˴����������⣬Ӧ����Ҫ����
	pGoods->wBuyNum += wBuyNum;

	SendRefreshShopData(byShopType);

	return true;
}

// ������Ʒ����
bool CShopManager::HotSaleGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	m_pPlayer->GetActivityManager()->GetHotSaleGoods().OnNetBuyGoods(wGoodsId);
	return true;
}

// ��Դս��Ʒ����
bool CShopManager::ResourceWarGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	const ShopGoodsData_Config *pGoodsCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pGoodsCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pGoodsCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�ȼ�����[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pGoodsCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ���Ĳ���[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ��⹺�����
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pGoodsCfg->wBuyNumMax && wTotalBuyNum > pGoodsCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�����������[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pGoodsCfg->vCostList, wBuyNum, FROM_SHOP_RESOURCE_WAR);

	// 
	m_pPlayer->AddObject(pGoodsCfg->dwEntityId, wBuyNum * pGoodsCfg->dwEntityNum, TRUE, FROM_SHOP_RESOURCE_WAR);

	// �����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}

// GM��Ʒ����
bool CShopManager::GMShopGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wGM)
{
	const ShopGoodsData_Config *pShopCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pShopCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���GM��Ʒ����[ShopType:%d, GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pShopCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����GM��Ʒ��Ҫ�Ľ�ɫ�ȼ�����[GoodsId:%d,NeedRoleLv:%d,RoleLv:%d]",
			__FUNCTION__, wGoodsId, pShopCfg->wNeedRoleLevel, m_pPlayer->GetLevel());
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pShopCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]����GM��Ʒ���Ĳ���[ShopType:%d,GoodsId:%d]", __FUNCTION__, byShopType, wGoodsId);
		return false;
	}

	// ���GM�ȼ�
	if (m_pPlayer->GetVipGMLevel() < pShopCfg->nContext)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����GM��Ʒ��Ҫ��GM�ȼ�����[GoodsId:%d,NeedGMLv:%d,RoleGMLv:%d]", 
			__FUNCTION__, wGoodsId, pShopCfg->nContext, m_pPlayer->GetVipGMLevel());
		return false;
	}

	// ��⹺�����
	WORD wTotalBuyNum = wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId);
	if (pShopCfg->wBuyNumMax && wTotalBuyNum > pShopCfg->wBuyNumMax)
	{
		MAPLOG(ERROR_LEVEL, "[%s]GM��Ʒ�����������[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��������
	m_pPlayer->DoMoneyCost(pShopCfg->vCostList, wBuyNum, FROM_SHOP_GM);

	// �����Ʒ
	m_pPlayer->AddObject(pShopCfg->dwEntityId, wBuyNum * pShopCfg->dwEntityNum, TRUE, FROM_SHOP_GM);

	// �����¼,���޹��ż�¼
	if (pShopCfg->wBuyNumMax)
		AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	return true;
}


// ������Ʒ����
bool CShopManager::GuildGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum)
{
	// ����Ƿ��й���
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(m_pPlayer->GetCharID());
	if (!pGuild)
	{
		MAPLOG(ERROR_LEVEL, "[%s]���û�ڹ��ᣬ���ܹ��򹤻��̵���Ʒ", __FUNCTION__);
		return false;
	}

	const ShopGoodsData_Config *pShopCfg = CConfigManager::Instance()->GetShopGoods(byShopType, wGoodsId);
	if (!pShopCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���������Ʒ����[GoodsId:%d]", __FUNCTION__,wGoodsId);
		return false;
	}

	// ��⹤��ȼ�
	if (pGuild->GetGuildLevel() < pShopCfg->wNeedGuildLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]����ȼ�����,���ܹ��򹤻���Ʒ[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ����ɫ�ȼ�
	if (m_pPlayer->GetLevel() < pShopCfg->wNeedRoleLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]��ɫ�ȼ�����,���ܹ��򹤻���Ʒ[GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// �������
	if (!m_pPlayer->CheckMoneyCost(pShopCfg->vCostList, wBuyNum))
	{
		MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�������Ĳ���![GoodsId:%d]", __FUNCTION__, wGoodsId);
		return false;
	}

	// ��⹺������
	if (SHOP_GUILD_LIMIT == byShopType)
	{ // ��ʱ�̵�

		if (wBuyNum != 1)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������ʱ��Ʒÿ��ֻ�ܹ���һ��[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}

		// �����˹�����������
		if (GetGoodsBuyRecordNum(byShopType, wGoodsId))
		{
			MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�����Ѵ�����[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}

		// ��⹤�Ṻ����������
		const Goods *pGoods = pGuild->GetLimitGoods(wGoodsId);
		if (!pGoods)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ƿ��Ĺ�����ʱ��Ʒ[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}
		if (pGoods->wNum >= pShopCfg->wBuyNumMax)
		{
			pGuild->SendLimitGoodsList(m_pPlayer); // todo::�п����ǿͻ�����������������Ѳ�ͬ���������ƹ�ȥ
			MAPLOG(GUI_LEVEL, "[%s]������ʱ��Ʒ�ѹ�����[GoodsId:%d,num:%d]", __FUNCTION__, wGoodsId, pGoods->wNum);
			return false;
		}

		pGuild->AddLimitGoodsBuyNum(wGoodsId, wBuyNum);
	}
	else
	{ // ����,�����̵�
		if (pShopCfg->wBuyNumMax && wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > pShopCfg->wBuyNumMax)
		{
			MAPLOG(ERROR_LEVEL, "[%s]������Ʒ�����Ѵ�����[GoodsId:%d]", __FUNCTION__, wGoodsId);
			return false;
		}
	}

	// ��ӹ����¼
	AddGoodsBuyRecord(byShopType, wGoodsId, wBuyNum);

	// ��������
	m_pPlayer->DoMoneyCost(pShopCfg->vCostList, wBuyNum, FROM_SHOP_GUILD);

	// �����Ʒ
	m_pPlayer->AddObject(pShopCfg->dwEntityId, pShopCfg->dwEntityNum*wBuyNum, TRUE, FROM_SHOP_GUILD);

	return true;
}

// ������Ʒ
bool CShopManager::DoSellItem(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CItemManager *pItemMgr = m_pPlayer->GetItemManager();
	CHECK_POINTER_RET(pItemMgr, false);

	// �����Ʒ
	int nHeroSoul = 0;
	int nTowerPrestige = 0;	// ��������
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		const Item *pItem = pItemMgr->GetItem(pGoodsList[i]);
		if (!pItem)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���û�д���Ʒ[ItemId:%d]", __FUNCTION__, pGoodsList[i]);
			continue;
		}

		const Item_Config *pItemCfg = CConfigManager::Instance()->GetItem(pItem->dwId);
		if (!pItemCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ�����Ʒ������[ItemId:%d]", __FUNCTION__, pItem->dwId);
			continue;
		}

		// Ӣ����Ƭ
		if (pItemCfg->bySubType == ITEM_TYPE_8)
		{
			// ���ս��
			nHeroSoul += (pItemCfg->dwSellPrice * pItem->dwNum);
		}
		// װ����Ƭ
		else if (pItemCfg->bySubType == ITEM_TYPE_7)
		{
			// �������
			nTowerPrestige += (pItemCfg->dwSellPrice * pItem->dwNum);
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]����Ӣ�ۻ���װ����Ƭ[sub_type:%d]",
				__FUNCTION__, pItemCfg->bySubType);
			continue;
		}

		// ɾ����Ʒ
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

// ����Ӣ��
bool CShopManager::DoSellHero(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CHeroManager *pHeroMgr = m_pPlayer->GetHeroManager();
	CHECK_POINTER_RET(pHeroMgr, false);

	DWORD dwTotalGold = 0;
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		// ���Ӣ��
		DWORD dwUId = pGoodsList[i];
		const HeroInfo *pHero = pHeroMgr->GetHero(dwUId);
		if (!pHero)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���û�д�Ӣ��[HeroId:%d]", __FUNCTION__, dwUId);
			continue;
		}

		const Hero_Config *pHeroCfg = g_pCfgMgr->GetHero(pHero->dwIntoId);
		if (!pHeroCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ������[HeroInfoId:%d]", __FUNCTION__, pHero->dwIntoId);
			continue;
		}

		const HeroLevel_Config *pLevelCfg = CConfigManager::Instance()->GetHeroLevel(pHero->wQuality, pHero->wLevel);
		if (!pLevelCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���Ӣ�۵ĵȼ���������[quality:%d,level:%d]",
				__FUNCTION__, pHero->wQuality, pHero->wLevel);
			continue;
		}

		// �һ��ɽ��
		dwTotalGold += (pLevelCfg->dwTotalExp + pHeroCfg->nPrice + pHero->dwExp);

		// ɾ��Ӣ��
		pHeroMgr->RemoveHero(dwUId, true, FROM_SELL_HERO);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, dwTotalGold);

	MoneyData tMoneyData;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = dwTotalGold;
	vCurrencyList.push_back(tMoneyData);

	return true;
}

// ����װ��
bool CShopManager::DoSellEquipemnt(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CEquipmentManager *pEquipmentMgr = m_pPlayer->GetEquipmentManager();
	CHECK_POINTER_RET(pEquipmentMgr, false);

	DWORD dwTotalGold = 0;
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		// ���װ��
		DWORD dwUId = pGoodsList[i];
		const Equipment *pEquipment = pEquipmentMgr->GetEquipment(dwUId);
		if (!pEquipment)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���û�д�װ��[EquipmentUId:%d]", __FUNCTION__, dwUId);
			continue;
		}

		const Equipment_Config *pEquipmentCfg = g_pCfgMgr->GetEquipment(pEquipment->dwInfoId);
		if (!pEquipmentCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���װ������[EquipmentInfoId:%d]", __FUNCTION__, pEquipment->dwInfoId);
			continue;
		}

		// �һ��ɽ��
		dwTotalGold += pEquipment->nEnhanceCostGold;
		dwTotalGold += pEquipmentCfg->dwPrice;

		// ɾ��װ��
		pEquipmentMgr->RemoveEquipment(dwUId, true, FROM_SELL_EQUIPMENT);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, dwTotalGold);

	MoneyData tMoneyData;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = dwTotalGold;
	vCurrencyList.push_back(tMoneyData);

	return true;
}

// ���۱���
bool CShopManager::DoSellTreasure(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList)
{
	CTreasureManager *pTreasureMgr = m_pPlayer->GetTreasureManager();
	CHECK_POINTER_RET(pTreasureMgr, false);

	DWORD dwTotalGold = 0;
	for (BYTE i = 0; i < byGoodsNum; ++i)
	{
		// ��ⱦ��
		DWORD dwUId = pGoodsList[i];
		const Treasure *pTreasure = pTreasureMgr->GetTreasure(dwUId);
		if (!pTreasure)
		{
			MAPLOG(ERROR_LEVEL, "[%s]���û�д˱���[TreasureId:%d]", __FUNCTION__, dwUId);
			continue;
		}

		const Treasure_Config *pTreasureCfg = CConfigManager::Instance()->GetTreasure(pTreasure->dwInfoId);
		if (!pTreasureCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]û���ҵ���������[TreasureId:%d]", __FUNCTION__, pTreasure->dwInfoId);
			continue;
		}

		const TreasureEnhance_Config *pStrengthenCfg =
			CConfigManager::Instance()->GetTreasureStrengthen(pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
		if (!pStrengthenCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]û���ҵ�����ǿ������[qaulity:%d,strengthenlevel:%d]",
				__FUNCTION__, pTreasureCfg->wQuality, pTreasure->wEnhanceLevel);
			continue;
		}

		// �һ��ɽ��
		if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
		{
			// ʥ�����۸ĳ�1���
			dwTotalGold += 1;
		}
		else{
			dwTotalGold += (pStrengthenCfg->dwTotalExp + pTreasure->dwEnhanceExp + pTreasureCfg->nPrice);
		}

		// ɾ������
		pTreasureMgr->RemoveTreasure(dwUId, FROM_SELL_TREASURE);
	}

	m_pPlayer->ModifyProperty(ROLE_PRO_GOLD, dwTotalGold);

	MoneyData tMoneyData;
	tMoneyData.byType = ROLE_PRO_GOLD;
	tMoneyData.nValue = dwTotalGold;
	vCurrencyList.push_back(tMoneyData);

	return true;
}

// �����ۻ��̵���Ʒ
// void CShopManager::CreateHeroShopGoods()
// {
// 	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetHeroShopSlotList();
// 	if (!pSlotListCfg || !pSlotListCfg->size())
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]Ӣ���̵���Ʒ��λ�б�����Ϊ��",__FUNCTION__);
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
// 		// ȡɸѡ����б���Ʒ
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
// 		// ����ܸ��ʼ�����б��Ƿ�Ϸ�
// 		if (!wTotalRate || !vRandList.size())
// 		{
// 			MAPLOG(ERROR_LEVEL, "[%s]Ӣ���̵���Ʒɸ����б�Ϊ��![TotalRate:%d,ListSize:%d,RoleLv:%d]",
// 				__FUNCTION__, wTotalRate, vRandList.size(), m_pPlayer->GetLevel());
// 			return;
// 		}
// 
// 		// �����Ʒ
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

// ����ˢ���̵���Ʒ
void CShopManager::CreateRefreshShopGoods(BYTE byShopType)
{
	const vector<RefreshShopSlot_Config> *pSlotListCfg = CConfigManager::Instance()->GetRefreshShopSlotList(byShopType);
	if (!pSlotListCfg || !pSlotListCfg->size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]ˢ���̵���Ʒ��λ�б�����Ϊ��", __FUNCTION__);
		return;
	}

	// �����̵�����
	ResetShopData(byShopType);

	vector<const ShopGoodsData_Config*> vRandList;
	for (size_t i = 0; i < pSlotListCfg->size(); ++i)
	{
		// todo::����ǵ�һ��,����ָ����λ,��س�ָ����Ʒ
		if (SHOP_HERO == byShopType && !m_pPlayer->CheckOnceFlag(once_hero_shop_refresh) && i == 0)
		{
			m_pPlayer->SetOnceFlag(once_hero_shop_refresh);
			AddGoodsBuyRecord(byShopType, 1, 0);
			continue;
		}

		const RefreshShopSlot_Config &tSlotCfg = (*pSlotListCfg)[i];

		WORD wTotalRate = 0;
		vRandList.clear();
		// ȡɸѡ����б���Ʒ
		for (size_t j = 0; j < tSlotCfg.vGoodsList.size(); ++j)
		{
			const ShopGoodsData_Config *pGoodsCfg = tSlotCfg.vGoodsList[j];
			if (pGoodsCfg && m_pPlayer->GetLevel() >= pGoodsCfg->wNeedRoleLevel)
			{
				wTotalRate += pGoodsCfg->wRefreshRate;
				vRandList.push_back(pGoodsCfg);
			}
		}

		// ����ܸ��ʼ�����б��Ƿ�Ϸ�
		if (!wTotalRate || !vRandList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]ˢ���̵���Ʒɸ����б�Ϊ��![TotalRate:%d,ListSize:%d,RoleLv:%d]",
				__FUNCTION__, wTotalRate, vRandList.size(), m_pPlayer->GetLevel());
			return;
		}

		// �����Ʒ
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

// ���;����̵�����
void CShopManager::SendRefreshShopData(BYTE byShopType)
{
	// Ӣ���̵�
	if (byShopType  == SHOP_HERO)
	{
		MSG_S2C_HERO_SHOP_DATA_RESP msg;
		msg.m_HeroShop = m_pShopData->tHeroShop;
		m_pPlayer->SendPacket(&msg);
	}
	// �����̵�
	else if (byShopType == SHOP_AWAKEN)
	{
		MSG_S2C_AWAKEN_SHOP_DATA_RESP msg;
		msg.m_AwakenShop = m_pShopData->tAwakenShop;
		m_pPlayer->SendPacket(&msg);
	}
	// �ǻ��̵�
	else if (byShopType == SHOP_STAR_SOUL)
	{
		MSG_S2C_STAR_SOUL_SHOP_DATA_RESP msg;
		msg.m_ShopData = m_pShopData->tStarSoulShop;
		m_pPlayer->SendPacket(&msg);
	}
	// ֱ���̵�
	else if (byShopType == SHOP_DIRECT_BUY)
	{
		MSG_S2C_DIRECT_SHOP_DATA_RESP msg;
		msg.m_ShopData = m_pShopData->tDirectBuyShop;
		m_pPlayer->SendPacket(&msg);
	}
	// ˮ���̵�
 	else if (byShopType == SHOP_CRYSTAL)
 	{
		MSG_S2C_CRYSTAL_SHOP_DATA_RESP msg;
		msg.m_ShopData = m_pShopData->tCrystalShopData;
		m_pPlayer->SendPacket(&msg);
 	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д����ˢ���̵�����[type:%d]", __FUNCTION__, byShopType);
	}
}

// �����̵�ˢ����Ѵ���
void CShopManager::UpdateShopRefreshFreeTimes(BYTE byShopType)
{
	RefreshShopData *pShopData = GetRefreshShopData(byShopType);
	if (!pShopData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]ȡ����ˢ���̵�����[type:%d]", __FUNCTION__, byShopType);
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

// ȡˢ���̵�����
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


// ������Դս�̵�����
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

// ȡ��Ʒ�����¼
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
	// �Ƽ��̵�
	else if (byType == SHOP_RECOMMEND)
	{
		for (WORD i = 0; i < m_pShopData->tRecommendShop.wBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tRecommendShop.arBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// �����ͨ�̵�
	else if (byType == SHOP_TEAM_DUNGEON_NORMAL)
	{
		for (WORD i = 0; i < m_pShopData->tTeamDungeonShop.wNormalGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tTeamDungeonShop.arNormalGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// ��ӽ����̵�
	else if (byType == SHOP_TEAM_DUNGEON_REWARD)
	{
		for (WORD i = 0; i < m_pShopData->tTeamDungeonShop.wRewardGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tTeamDungeonShop.arRewardGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// ʥ���ڻ��ֶһ��̵�
	else if (byType == SHOP_CHRISTMAS_SCORE_EXCHANGE)
	{
		for (WORD i = 0; i < m_pShopData->tChristmasShop.wScoreExchangeGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tChristmasShop.arScoreExchangeGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// ʥ������Ʒ�һ��̵�
	else if (byType == SHOP_CHRISTMAS_ITEM_EXCHANGE)
	{
		for (WORD i = 0; i < m_pShopData->tChristmasShop.wItemExchangeGoodsBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tChristmasShop.arItemExchangeGoodsBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// ˮ���̵�
	else if (byType == SHOP_CRYSTAL)
	{
		for (WORD i = 0; i < m_pShopData->tCrystalShopData.wGoodsNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tCrystalShopData.arGoodsList[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// ��Դս�̵�
	else if (byType == SHOP_RESOURCE_WAR_SHOP)
	{
		for (WORD i = 0; i < m_pShopData->tResourceWarShop.wBuyRecordNum; ++i)
		{
			ShopGoodsBuyRecord &tBuyRecord = m_pShopData->tResourceWarShop.arBuyRecord[i];
			if (tBuyRecord.wGoodsId == wGoodsId)
				return &tBuyRecord;
		}
	}
	// GM�̵�
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
		MAPLOG(ERROR_LEVEL, "[%s]û�д�����̵�����![type:%d]", __FUNCTION__, byType);
	}
	return NULL;
}

// ȡ��Ʒ�����¼����
WORD CShopManager::GetGoodsBuyRecordNum(BYTE byType, WORD wGoodsId)
{
	ShopGoodsBuyRecord *pBuyRecord = GetGoodsBuyRecord(byType, wGoodsId);
	return pBuyRecord ? pBuyRecord->wBuyNum : 0;
}

// ȡ��Ʒ�����¼(����̵���Ҫ����Ľӿ�)
const ShopGoodsBuyRecord* CShopManager::GetGoodsBuyRecordEx(BYTE byType, WORD wGoodsId)
{
	return GetGoodsBuyRecord(byType, wGoodsId);
}

// �����Ʒ�����¼
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
	// �������
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
	// ������ʱ
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
	// ���ά��
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
	// ������
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
	// �ǻ��̵�
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
	// �������̵�
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
	// 7���̵�
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
	// �ȼ�����̵�
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
	// �޾�֮�ص����̵�
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
	// �޾�֮�ؽ����̵�
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
	// �޾�֮�ؽ����̵�
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
	// �һ��̵�
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
	// �Ƽ��̵�
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
	// �����ͨ�̵�
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
	// ��ӽ����̵�
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
	// �ǻ꽱���̵�
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
	// ʥ���ڻ��ֶһ��̵�
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
	// ʥ������Ʒ�һ��̵�
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
	// ˮ���̵�
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
	// ��Դս�̵�
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
	// GM�̵�
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
		MAPLOG(ERROR_LEVEL, "[%s]û�д�����̵�����![type:%d]", __FUNCTION__, byType);
		bIsListFull = false;
	}

	if (bIsListFull)
		MAPLOG(ERROR_LEVEL, "[%s]�̵깺���¼����![type:%d]", __FUNCTION__, byType);

	return false;
}

// ������Ʒ�����¼�б�
void CShopManager::SendGoodsBuyRecordList(BYTE byType, BYTE byRecordNum, ShopGoodsBuyRecord *pRecordList)
{
	MSG_S2C_SHOP_GOODS_BUY_RECORD_RESP msg;
	msg.m_byType = byType;
	byRecordNum = min(byRecordNum, MAX_GOOGS_BUY_RECORD_NUM);
	msg.m_byBuyRecordNum = byRecordNum;
	memcpy_s(msg.m_arBuyRecord, MAX_GOOGS_BUY_RECORD_NUM * sizeof(ShopGoodsBuyRecord), 
		pRecordList, byRecordNum *sizeof(ShopGoodsBuyRecord));
	m_pPlayer->SendPacket(&msg);

	MAPLOG(ERROR_LEVEL, "[%s]�����̵깺���¼����![type:%d,RecordNum:%d]", __FUNCTION__, byType, byRecordNum);
}

// �����̵�����
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
		MAPLOG(ERROR_LEVEL, "[%s]û�д�����̵�����![type:%d]", __FUNCTION__, byShopType);
		break;
	}
}

// ����Ƿ�����������
bool CShopManager::CheckIsBuyNumLimit(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wBuyNumMax)
{
	// û�й�����������
	if (!wBuyNumMax)
		return false;

	// �����Ѵ�����
	if (wBuyNum + GetGoodsBuyRecordNum(byShopType, wGoodsId) > wBuyNumMax)
		return true;

	return false;
}

// ����ֱ���̵�����
void CShopManager::SendDirectBuyShopData()
{
	MSG_S2C_DIRECT_SHOP_DATA_RESP msg;
	msg.m_ShopData = m_pShopData->tDirectBuyShop;
	m_pPlayer->SendPacket(&msg);
}