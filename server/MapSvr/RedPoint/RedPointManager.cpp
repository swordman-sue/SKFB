#include "stdafx.h"
#include "RedPointManager.h"
#include "..\Player.h"
#include "..\Land\LandManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\Rebel\RebelManager.h"
#include "..\Shop\ShopManager.h"
#include "..\DailyMission\DailyMissionManager.h"
#include "..\Achievement\AchievementManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Hero\HeroManager.h"
#include "..\Friend\FriendManager.h"
#include "..\ConfigManager.h"
#include "..\Activity\ActivityManager.h"
#include "..\SevenDayActivity\SevenDayManager.h"
#include "..\Email\EmailManager.h"
#include "..\Arena\ArenaManager.h"
#include "..\WorldBoss\WorldBossManager.h"
#include "..\Guild\GuildManager.h"
#include "..\Recharge\RechargeManager.h"
#include "..\PVP\PVPManager.h"
#include "..\AttackCity\AttackCityManager.h"
#include "..\KingMatch\KingMatchManager.h"
#include "..\Activity\Christmas.h"
#include "..\WorldLevelTask\WorldLevelTaskManager.h"

CRedPointManager::CRedPointManager()
{

}

CRedPointManager::~CRedPointManager()
{

}

void CRedPointManager::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
}

// 请求红点列表
void CRedPointManager::OnNetRedPointList()
{
#define CHECK_RED_PIONT(sys_id) 	if (IsHaveRedPoint(sys_id)) \
								msg.m_RedPointList[msg.m_wRedPointNum++] = sys_id;

	MSG_S2C_RED_POINT_LIST_RESP msg;
	msg.m_wRedPointNum = 0;

	// 叛军
	CHECK_RED_PIONT(SYSTEM_REBEL);

	// 英雄商店
	CHECK_RED_PIONT(SYSTEM_HERO_SHOP);

	// 领地
	CHECK_RED_PIONT(SYSTEM_LAND);

	// 爬塔
	CHECK_RED_PIONT(SYSTEM_TOWER);

	// 日常任务
	CHECK_RED_PIONT(SYSTEM_DAILY_MISSION);

	// 成就
	CHECK_RED_PIONT(SYSTEM_ACHIVEMENT);

	// 奖励中心
// 	if (IsHaveRedPoint(SYSTEM_REWARD_CENTER))
// 		msg.m_RedPointList[msg.m_wRedPointNum++] = SYSTEM_REWARD_CENTER;

	// 主线副本
	CHECK_RED_PIONT(SYSTEM_MAIN_FB);

	// 英雄副本
	CHECK_RED_PIONT(SYSTEM_HERO_FB);

	// 英雄招募
	CHECK_RED_PIONT(SYSTEM_HERO_SUMMON);

	// 好友系统
	CHECK_RED_PIONT(SYSTEM_FRIEND);

	// 签到
	CHECK_RED_PIONT(SYSTEM_SIGN);

	// 7天1
	CHECK_RED_PIONT(SYSTEM_SEVEN_DAY_1);

	// 7天2
	CHECK_RED_PIONT(SYSTEM_SEVEN_DAY_2);

	// 铜雀台(体力领取)
	CHECK_RED_PIONT(SYSTEM_TQT);

	// 迎财神(金币领取)
	CHECK_RED_PIONT(SYSTEM_YCS);

	// 等级商店
	CHECK_RED_PIONT(SYSTEM_LEVEL_SHOP);

	// 等级礼包
	CHECK_RED_PIONT(SYSTEM_LEVEL_GIFT);

	// 邮件
	CHECK_RED_PIONT(SYSTEM_EMAIL);

	// 竞技场
	CHECK_RED_PIONT(SYSTEM_ARENA);

	// 世界BOSS
	CHECK_RED_PIONT(SYSTEM_WORLD_BOSS);

	// 工会
	CHECK_RED_PIONT(SYSTEM_GUILD);

	// 精英副本
	CHECK_RED_PIONT(SYSTEM_ELITE);

	// 全民福利
	CHECK_RED_PIONT(SYSTEM_ALL_PEOPLE_WELFARE);

	// 开服基金
	CHECK_RED_PIONT(SYSTEM_FUND);

	// VIP福利
	CHECK_RED_PIONT(SYSTEM_VIP_GIFT);

	// 月卡福利
	CHECK_RED_PIONT(SYSTEM_MONTH_CARD);

	// PVP
	CHECK_RED_PIONT(SYSTEM_PVP);

	// 单次返利
	//CHECK_RED_PIONT(SYSTEM_SGL_RCG);

	// 累充豪礼
	//CHECK_RED_PIONT(SYSTEM_AMT_RCG);

	// 首充
	CHECK_RED_PIONT(SYSTEM_FRT_RCG);

	// 限时召唤
	//CHECK_RED_PIONT(SYSTEM_LMT_RCT);

	// 怪物攻城
	CHECK_RED_PIONT(SYSTEM_ATTACK_CITY);

	// 豪华充值
	CHECK_RED_PIONT(SYSTEM_LUXURY_SIGN_IN);

	// 超值折扣
	CHECK_RED_PIONT(SYSTEM_SUPER_DISCOUNT_ID);

	// 直购商店
	CHECK_RED_PIONT(SYSTEM_DIRECT_BUY);

	// 节日活动
	CHECK_RED_PIONT(SYSTEM_FESTIVAL_ACTIVITY);

	// 登录奖励
	CHECK_RED_PIONT(SYSTEM_LOGIN_REWARD);

	// 王者赛
	CHECK_RED_PIONT(SYSTEM_KING_MATCH);

	// 圣诞节
	CHECK_RED_PIONT(SYSTEM_CHRISTMAS);

	// 超值折扣
	CHECK_RED_PIONT(SYSTEM_NEW_SUPER_DISCOUNT_ID);

	// 热卖商品
	CHECK_RED_PIONT(SYSTEM_HOT_SALE_GOODS_ID);

	// 世界等级任务
	CHECK_RED_PIONT(SYSTEM_WORLD_LEVEL_TASK);
	
	m_pPlayer->SendPacket(&msg);
}

// 红点通知
void CRedPointManager::OnRedPointNotify(WORD wSystemId)
{
	if (!IsHaveRedPoint(wSystemId))
		return;

	MSG_S2C_RED_POINT_NOTIFY msg;
	msg.m_wSystemId = wSystemId;
	m_pPlayer->SendPacket(&msg);
}


// 是否有红点
bool CRedPointManager::IsHaveRedPoint(WORD wSystemId)
{
	// 检测是否有开放
// 	if (m_pPlayer->GetLevel < g_pCfgMgr->GetSystemOpenLevel(wSystemId))
// 		return false;

	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();

	bool bIsHave = false;
	switch (wSystemId)
	{
	// 叛军
	case SYSTEM_REBEL:
		bIsHave = CRebelManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 英雄商店
	case SYSTEM_HERO_SHOP:
		bIsHave = m_pPlayer->GetShopManager()->IsHaveRedPoint(SHOP_HERO);
		break;

	// 领地
	case SYSTEM_LAND:
		bIsHave = CLandManager::Instance()->IsHaveRedPoint(m_pPlayer->GetRoleId());
		break;

	// 爬塔
	case SYSTEM_TOWER:
		bIsHave = pFBMgr->GetTower().IsHaveRedPoint();
		break;

	// 日常任务
	case SYSTEM_DAILY_MISSION:
		bIsHave = m_pPlayer->GetDailyMissionManager()->IsHaveRedPoint();
		break;

	// 成就
	case SYSTEM_ACHIVEMENT:
		bIsHave = m_pPlayer->GetAchievementManager()->IsHaveRedPoint();
		break;

	// 奖励中心
	case SYSTEM_REWARD_CENTER:
		bIsHave = m_pPlayer->GetRewardCenterManager()->IsHaveRedPoint(m_pPlayer);
		break;

	// 主线副本
	case SYSTEM_MAIN_FB:
		bIsHave = pFBMgr->GetMainFB().IsHaveRedPoint();
		break;

	// 英雄副本
	case SYSTEM_HERO_FB:
		bIsHave = pFBMgr->GetHeroFB().IsHaveRedPoint();
		break;

	// 英雄招募
	case SYSTEM_HERO_SUMMON:
		bIsHave = m_pPlayer->GetHeroManager()->IsHaveRedPoint();
		break;

	// 好友系统
	case SYSTEM_FRIEND:
		bIsHave = m_pPlayer->GetFriendManager()->IsHaveRedPoint();
		break;

	// 七天活动1
	case SYSTEM_SEVEN_DAY_1:
		bIsHave = m_pPlayer->GetSevenDayManager()->IsHaveRedPoint();
		break;

	// 七天活动2
	case SYSTEM_SEVEN_DAY_2:
		bIsHave = m_pPlayer->GetSevenDayManager()->IsHaveRedPointEx();
		break;

	// 签到
	// 铜雀台(体力领取)
	// 迎财神(金币领取)
	// 等级商店
	// 登录奖励
	case SYSTEM_SIGN:
	case SYSTEM_TQT:
	case SYSTEM_YCS:
	case SYSTEM_LEVEL_SHOP:
	case SYSTEM_LEVEL_GIFT:
	case SYSTEM_FUND:
	case SYSTEM_ALL_PEOPLE_WELFARE:
	case SYSTEM_VIP_GIFT:
	case SYSTEM_LUXURY_SIGN_IN:
	case SYSTEM_LOGIN_REWARD:
		bIsHave = m_pPlayer->GetActivityManager()->IsHaveRedPoint(wSystemId);
		break;

	// 月卡
	case SYSTEM_MONTH_CARD:
		bIsHave = m_pPlayer->GetRechargeManager()->IsHaveRedPoint(wSystemId);
		break;

	// 邮件
	case SYSTEM_EMAIL:
		bIsHave = m_pPlayer->GetEmailManager()->IsHaveRedPoint();
		break;

	// 竞技场
	case SYSTEM_ARENA:
		bIsHave = CArenaManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 世界BOSS
	case SYSTEM_WORLD_BOSS:
		bIsHave = CWorldBossManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 工会
	case SYSTEM_GUILD:
		bIsHave = CGuildManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 精英副本
	case SYSTEM_ELITE:
		bIsHave = m_pPlayer->GetFubenManager()->GetEliteFB().IsHaveRedPoint();
		break;

	// PVP
	case SYSTEM_PVP:
		bIsHave = CPVPManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 单次返利
	// 累充豪礼
	// 首充
	// 限时召唤
	//case SYSTEM_LMT_RCT:
	case SYSTEM_FRT_RCG:
	//case SYSTEM_SGL_RCG:
	//case SYSTEM_AMT_RCG:
		bIsHave = m_pPlayer->GetActivityManager()->GetLimitActivity().IsHaveRedPoint(wSystemId);
		break;

	// 怪物攻城
	case SYSTEM_ATTACK_CITY:
		bIsHave = CAttackCityManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	case SYSTEM_SUPER_DISCOUNT_ID:
		bIsHave = m_pPlayer->GetActivityManager()->GetSuperDiscount().IsHaveRedPoint();
		break;

	case SYSTEM_NEW_SUPER_DISCOUNT_ID:
		bIsHave = m_pPlayer->GetActivityManager()->GetNewSuperDiscount().IsHaveRedPoint();
		break;

	// 直购商店
	case SYSTEM_DIRECT_BUY:
		bIsHave = m_pPlayer->GetShopManager()->IsHaveRedPoint(SHOP_DIRECT_BUY);
		break;

	// 节日活动
	case SYSTEM_FESTIVAL_ACTIVITY:
		bIsHave = m_pPlayer->GetActivityManager()->GetFestivalActivity().IsHaveRedPoint();
		break;

	// 王者赛
	case SYSTEM_KING_MATCH:
		bIsHave = CKingMatchManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 圣诞节
	case SYSTEM_CHRISTMAS:
		bIsHave = CChristmas::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// 热卖商品
	case SYSTEM_HOT_SALE_GOODS_ID:
		bIsHave = m_pPlayer->GetActivityManager()->GetHotSaleGoods().IsHaveRedPoint();
		break;

	// 世界等级
	case SYSTEM_WORLD_LEVEL_TASK:
		bIsHave = m_pPlayer->GetWorldLevelTaskManager()->IsHaveRedPoint();
		break;

	default:
		MAPLOG(ERROR_LEVEL, "[%s]木有处理的红点[SystemId:%d]", __FUNCTION__, wSystemId);
		break;
	}
	return bIsHave;
}

// 打印红点状态 for test
void CRedPointManager::PrintRedPointStatus()
{
#define PRINT_RED_PIONT_STATUS(sys_id) 	if (IsHaveRedPoint(sys_id)) \
											MAPLOG(ERROR_LEVEL, "有红点[SystemId:%d]", sys_id);

	//PRINT_RED_PIONT_STATUS(SYSTEM_DAILY_FB);
	PRINT_RED_PIONT_STATUS(SYSTEM_DAILY_MISSION);
	PRINT_RED_PIONT_STATUS(SYSTEM_HERO_SUMMON);
	PRINT_RED_PIONT_STATUS(SYSTEM_HERO_SHOP);
	PRINT_RED_PIONT_STATUS(SYSTEM_FRIEND);
	PRINT_RED_PIONT_STATUS(SYSTEM_REBEL);
	PRINT_RED_PIONT_STATUS(SYSTEM_TOWER);
	//PRINT_RED_PIONT_STATUS(SYSTEM_SPIRIT);
	//PRINT_RED_PIONT_STATUS(SYSTEM_DB);
	PRINT_RED_PIONT_STATUS(SYSTEM_LAND);
	PRINT_RED_PIONT_STATUS(SYSTEM_REWARD_CENTER);
	PRINT_RED_PIONT_STATUS(SYSTEM_MAIN_FB);
	PRINT_RED_PIONT_STATUS(SYSTEM_HERO_FB);
	PRINT_RED_PIONT_STATUS(SYSTEM_ACHIVEMENT);
	PRINT_RED_PIONT_STATUS(SYSTEM_SIGN);
	PRINT_RED_PIONT_STATUS(SYSTEM_SEVEN_DAY_1);
	PRINT_RED_PIONT_STATUS(SYSTEM_SEVEN_DAY_2);
	PRINT_RED_PIONT_STATUS(SYSTEM_TQT);
	PRINT_RED_PIONT_STATUS(SYSTEM_YCS);
	PRINT_RED_PIONT_STATUS(SYSTEM_LEVEL_SHOP);
	PRINT_RED_PIONT_STATUS(SYSTEM_LEVEL_GIFT);
	PRINT_RED_PIONT_STATUS(SYSTEM_EMAIL);
	PRINT_RED_PIONT_STATUS(SYSTEM_ARENA);
	PRINT_RED_PIONT_STATUS(SYSTEM_WORLD_BOSS);
	PRINT_RED_PIONT_STATUS(SYSTEM_GUILD);
	PRINT_RED_PIONT_STATUS(SYSTEM_ELITE);
	PRINT_RED_PIONT_STATUS(SYSTEM_ALL_PEOPLE_WELFARE);
	PRINT_RED_PIONT_STATUS(SYSTEM_FUND);
	PRINT_RED_PIONT_STATUS(SYSTEM_VIP_GIFT);
	PRINT_RED_PIONT_STATUS(SYSTEM_MONTH_CARD);
	PRINT_RED_PIONT_STATUS(SYSTEM_PVP);
	//PRINT_RED_PIONT_STATUS(SYSTEM_LMT_RCT);
	PRINT_RED_PIONT_STATUS(SYSTEM_FRT_RCG);
	//PRINT_RED_PIONT_STATUS(SYSTEM_SGL_RCG);
	//PRINT_RED_PIONT_STATUS(SYSTEM_AMT_RCG);
	PRINT_RED_PIONT_STATUS(SYSTEM_ATTACK_CITY);
	PRINT_RED_PIONT_STATUS(SYSTEM_LUXURY_SIGN_IN);
	PRINT_RED_PIONT_STATUS(SYSTEM_SUPER_DISCOUNT_ID);
	PRINT_RED_PIONT_STATUS(SYSTEM_DIRECT_BUY);
	PRINT_RED_PIONT_STATUS(SYSTEM_FESTIVAL_ACTIVITY);
	PRINT_RED_PIONT_STATUS(SYSTEM_LOGIN_REWARD);
	PRINT_RED_PIONT_STATUS(SYSTEM_KING_MATCH);
	PRINT_RED_PIONT_STATUS(SYSTEM_CHRISTMAS);
	PRINT_RED_PIONT_STATUS(SYSTEM_NEW_SUPER_DISCOUNT_ID);
	PRINT_RED_PIONT_STATUS(SYSTEM_HOT_SALE_GOODS_ID);
	PRINT_RED_PIONT_STATUS(SYSTEM_WORLD_LEVEL_TASK);
}


