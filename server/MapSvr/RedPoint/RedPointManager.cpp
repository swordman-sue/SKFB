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

// �������б�
void CRedPointManager::OnNetRedPointList()
{
#define CHECK_RED_PIONT(sys_id) 	if (IsHaveRedPoint(sys_id)) \
								msg.m_RedPointList[msg.m_wRedPointNum++] = sys_id;

	MSG_S2C_RED_POINT_LIST_RESP msg;
	msg.m_wRedPointNum = 0;

	// �Ѿ�
	CHECK_RED_PIONT(SYSTEM_REBEL);

	// Ӣ���̵�
	CHECK_RED_PIONT(SYSTEM_HERO_SHOP);

	// ���
	CHECK_RED_PIONT(SYSTEM_LAND);

	// ����
	CHECK_RED_PIONT(SYSTEM_TOWER);

	// �ճ�����
	CHECK_RED_PIONT(SYSTEM_DAILY_MISSION);

	// �ɾ�
	CHECK_RED_PIONT(SYSTEM_ACHIVEMENT);

	// ��������
// 	if (IsHaveRedPoint(SYSTEM_REWARD_CENTER))
// 		msg.m_RedPointList[msg.m_wRedPointNum++] = SYSTEM_REWARD_CENTER;

	// ���߸���
	CHECK_RED_PIONT(SYSTEM_MAIN_FB);

	// Ӣ�۸���
	CHECK_RED_PIONT(SYSTEM_HERO_FB);

	// Ӣ����ļ
	CHECK_RED_PIONT(SYSTEM_HERO_SUMMON);

	// ����ϵͳ
	CHECK_RED_PIONT(SYSTEM_FRIEND);

	// ǩ��
	CHECK_RED_PIONT(SYSTEM_SIGN);

	// 7��1
	CHECK_RED_PIONT(SYSTEM_SEVEN_DAY_1);

	// 7��2
	CHECK_RED_PIONT(SYSTEM_SEVEN_DAY_2);

	// ͭȸ̨(������ȡ)
	CHECK_RED_PIONT(SYSTEM_TQT);

	// ӭ����(�����ȡ)
	CHECK_RED_PIONT(SYSTEM_YCS);

	// �ȼ��̵�
	CHECK_RED_PIONT(SYSTEM_LEVEL_SHOP);

	// �ȼ����
	CHECK_RED_PIONT(SYSTEM_LEVEL_GIFT);

	// �ʼ�
	CHECK_RED_PIONT(SYSTEM_EMAIL);

	// ������
	CHECK_RED_PIONT(SYSTEM_ARENA);

	// ����BOSS
	CHECK_RED_PIONT(SYSTEM_WORLD_BOSS);

	// ����
	CHECK_RED_PIONT(SYSTEM_GUILD);

	// ��Ӣ����
	CHECK_RED_PIONT(SYSTEM_ELITE);

	// ȫ����
	CHECK_RED_PIONT(SYSTEM_ALL_PEOPLE_WELFARE);

	// ��������
	CHECK_RED_PIONT(SYSTEM_FUND);

	// VIP����
	CHECK_RED_PIONT(SYSTEM_VIP_GIFT);

	// �¿�����
	CHECK_RED_PIONT(SYSTEM_MONTH_CARD);

	// PVP
	CHECK_RED_PIONT(SYSTEM_PVP);

	// ���η���
	//CHECK_RED_PIONT(SYSTEM_SGL_RCG);

	// �۳����
	//CHECK_RED_PIONT(SYSTEM_AMT_RCG);

	// �׳�
	CHECK_RED_PIONT(SYSTEM_FRT_RCG);

	// ��ʱ�ٻ�
	//CHECK_RED_PIONT(SYSTEM_LMT_RCT);

	// ���﹥��
	CHECK_RED_PIONT(SYSTEM_ATTACK_CITY);

	// ������ֵ
	CHECK_RED_PIONT(SYSTEM_LUXURY_SIGN_IN);

	// ��ֵ�ۿ�
	CHECK_RED_PIONT(SYSTEM_SUPER_DISCOUNT_ID);

	// ֱ���̵�
	CHECK_RED_PIONT(SYSTEM_DIRECT_BUY);

	// ���ջ
	CHECK_RED_PIONT(SYSTEM_FESTIVAL_ACTIVITY);

	// ��¼����
	CHECK_RED_PIONT(SYSTEM_LOGIN_REWARD);

	// ������
	CHECK_RED_PIONT(SYSTEM_KING_MATCH);

	// ʥ����
	CHECK_RED_PIONT(SYSTEM_CHRISTMAS);

	// ��ֵ�ۿ�
	CHECK_RED_PIONT(SYSTEM_NEW_SUPER_DISCOUNT_ID);

	// ������Ʒ
	CHECK_RED_PIONT(SYSTEM_HOT_SALE_GOODS_ID);

	// ����ȼ�����
	CHECK_RED_PIONT(SYSTEM_WORLD_LEVEL_TASK);
	
	m_pPlayer->SendPacket(&msg);
}

// ���֪ͨ
void CRedPointManager::OnRedPointNotify(WORD wSystemId)
{
	if (!IsHaveRedPoint(wSystemId))
		return;

	MSG_S2C_RED_POINT_NOTIFY msg;
	msg.m_wSystemId = wSystemId;
	m_pPlayer->SendPacket(&msg);
}


// �Ƿ��к��
bool CRedPointManager::IsHaveRedPoint(WORD wSystemId)
{
	// ����Ƿ��п���
// 	if (m_pPlayer->GetLevel < g_pCfgMgr->GetSystemOpenLevel(wSystemId))
// 		return false;

	CFubenManager *pFBMgr = m_pPlayer->GetFubenManager();

	bool bIsHave = false;
	switch (wSystemId)
	{
	// �Ѿ�
	case SYSTEM_REBEL:
		bIsHave = CRebelManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// Ӣ���̵�
	case SYSTEM_HERO_SHOP:
		bIsHave = m_pPlayer->GetShopManager()->IsHaveRedPoint(SHOP_HERO);
		break;

	// ���
	case SYSTEM_LAND:
		bIsHave = CLandManager::Instance()->IsHaveRedPoint(m_pPlayer->GetRoleId());
		break;

	// ����
	case SYSTEM_TOWER:
		bIsHave = pFBMgr->GetTower().IsHaveRedPoint();
		break;

	// �ճ�����
	case SYSTEM_DAILY_MISSION:
		bIsHave = m_pPlayer->GetDailyMissionManager()->IsHaveRedPoint();
		break;

	// �ɾ�
	case SYSTEM_ACHIVEMENT:
		bIsHave = m_pPlayer->GetAchievementManager()->IsHaveRedPoint();
		break;

	// ��������
	case SYSTEM_REWARD_CENTER:
		bIsHave = m_pPlayer->GetRewardCenterManager()->IsHaveRedPoint(m_pPlayer);
		break;

	// ���߸���
	case SYSTEM_MAIN_FB:
		bIsHave = pFBMgr->GetMainFB().IsHaveRedPoint();
		break;

	// Ӣ�۸���
	case SYSTEM_HERO_FB:
		bIsHave = pFBMgr->GetHeroFB().IsHaveRedPoint();
		break;

	// Ӣ����ļ
	case SYSTEM_HERO_SUMMON:
		bIsHave = m_pPlayer->GetHeroManager()->IsHaveRedPoint();
		break;

	// ����ϵͳ
	case SYSTEM_FRIEND:
		bIsHave = m_pPlayer->GetFriendManager()->IsHaveRedPoint();
		break;

	// ����1
	case SYSTEM_SEVEN_DAY_1:
		bIsHave = m_pPlayer->GetSevenDayManager()->IsHaveRedPoint();
		break;

	// ����2
	case SYSTEM_SEVEN_DAY_2:
		bIsHave = m_pPlayer->GetSevenDayManager()->IsHaveRedPointEx();
		break;

	// ǩ��
	// ͭȸ̨(������ȡ)
	// ӭ����(�����ȡ)
	// �ȼ��̵�
	// ��¼����
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

	// �¿�
	case SYSTEM_MONTH_CARD:
		bIsHave = m_pPlayer->GetRechargeManager()->IsHaveRedPoint(wSystemId);
		break;

	// �ʼ�
	case SYSTEM_EMAIL:
		bIsHave = m_pPlayer->GetEmailManager()->IsHaveRedPoint();
		break;

	// ������
	case SYSTEM_ARENA:
		bIsHave = CArenaManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// ����BOSS
	case SYSTEM_WORLD_BOSS:
		bIsHave = CWorldBossManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// ����
	case SYSTEM_GUILD:
		bIsHave = CGuildManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// ��Ӣ����
	case SYSTEM_ELITE:
		bIsHave = m_pPlayer->GetFubenManager()->GetEliteFB().IsHaveRedPoint();
		break;

	// PVP
	case SYSTEM_PVP:
		bIsHave = CPVPManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// ���η���
	// �۳����
	// �׳�
	// ��ʱ�ٻ�
	//case SYSTEM_LMT_RCT:
	case SYSTEM_FRT_RCG:
	//case SYSTEM_SGL_RCG:
	//case SYSTEM_AMT_RCG:
		bIsHave = m_pPlayer->GetActivityManager()->GetLimitActivity().IsHaveRedPoint(wSystemId);
		break;

	// ���﹥��
	case SYSTEM_ATTACK_CITY:
		bIsHave = CAttackCityManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	case SYSTEM_SUPER_DISCOUNT_ID:
		bIsHave = m_pPlayer->GetActivityManager()->GetSuperDiscount().IsHaveRedPoint();
		break;

	case SYSTEM_NEW_SUPER_DISCOUNT_ID:
		bIsHave = m_pPlayer->GetActivityManager()->GetNewSuperDiscount().IsHaveRedPoint();
		break;

	// ֱ���̵�
	case SYSTEM_DIRECT_BUY:
		bIsHave = m_pPlayer->GetShopManager()->IsHaveRedPoint(SHOP_DIRECT_BUY);
		break;

	// ���ջ
	case SYSTEM_FESTIVAL_ACTIVITY:
		bIsHave = m_pPlayer->GetActivityManager()->GetFestivalActivity().IsHaveRedPoint();
		break;

	// ������
	case SYSTEM_KING_MATCH:
		bIsHave = CKingMatchManager::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// ʥ����
	case SYSTEM_CHRISTMAS:
		bIsHave = CChristmas::Instance()->IsHaveRedPoint(m_pPlayer);
		break;

	// ������Ʒ
	case SYSTEM_HOT_SALE_GOODS_ID:
		bIsHave = m_pPlayer->GetActivityManager()->GetHotSaleGoods().IsHaveRedPoint();
		break;

	// ����ȼ�
	case SYSTEM_WORLD_LEVEL_TASK:
		bIsHave = m_pPlayer->GetWorldLevelTaskManager()->IsHaveRedPoint();
		break;

	default:
		MAPLOG(ERROR_LEVEL, "[%s]ľ�д���ĺ��[SystemId:%d]", __FUNCTION__, wSystemId);
		break;
	}
	return bIsHave;
}

// ��ӡ���״̬ for test
void CRedPointManager::PrintRedPointStatus()
{
#define PRINT_RED_PIONT_STATUS(sys_id) 	if (IsHaveRedPoint(sys_id)) \
											MAPLOG(ERROR_LEVEL, "�к��[SystemId:%d]", sys_id);

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


