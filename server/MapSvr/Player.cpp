#include "stdafx.h"
#include "Player.h"
#include "AccountInfo.h"
#include "ServerSession.h"
#include "MapSvrQuery.h"
#include "PlayerManager.h"
#include "TimeoutManager.h"
#include "SysTimesManager.h"
#include "../Common/DBCDefine.h"
#include "../Common/PacketStruct_Code.h"
#include "../Common/MsgDefine.h"
#include "../ToolKit/Rand.h"
#include "../ToolKit/DBCFileManager.h"
#include "../LuaEngine/LuaInterface.h"
#include "../Common/CommonLogic.h"
#include "EmailSystem.h"
#include "OpenServerSession.h"
#include "ConfigManager.h"
#include "ChatManager.h"
#include "login.pb.h"
#include "NetBase.h"
#include "header.pb.h"
#include <atlenc.h>
#include "Item/ItemManager.h"
#include "Hero/HeroManager.h"
#include "Treasure/TreasureManager.h"
#include "EquipmentManager.h"
#include "Fuben/FubenManager.h"
#include "Shop/ShopManager.h"
#include "Rank/RankManager.h"
#include "Friend/FriendManager.h"
#include "Guild/GuildManager.h"
#include "PlayerBaseDataManager.h"
#include "Recycling/RecyclingManager.h"
#include "Duobao/DuobaoManager.h"
#include "Spirit\SpiritManager.h"
#include "Email/EmailManager.h"
#include "Achievement/AchievementManager.h"
#include "DailyMission/DailyMissionManager.h"
#include "Vip/VipManager.h"
#include "RewardCenter/RewardCenterManager.h"
#include "Logic/LogicManager.h"
#include "Battle/BattleManager.h"
#include "Property/PropertyManager.h"
#include "SevenDayActivity/SevenDayManager.h"
#include "ZhanBu/ZhanBuManager.h"
#include "data_define.pb.h"
#include "Title/TitleManager.h"
#include "Activity/ActivityManager.h"
#include "RedPoint/RedPointManager.h"
#include "MapServer.h"
#include "Arena/ArenaManager.h"
#include "SystemNotice/SystemNotice.h"
#include "Recharge/RechargeManager.h"
#include "Land/LandManager.h"
#include "PVP/PVPManager.h"
#include "AttackCity/AttackCityManager.h"
#include "EndlessLand/EndlessLandManager.h"
#include "Gem/GemManager.h"
#include "TeamDungeon/TeamDungeonManager.h"
#include "Jsoncpp/include/json/value.h"
#include "Jsoncpp/include/json/reader.h"
#include "KingMatch/KingMatchManager.h"
#include "Activity/Christmas.h"
#include "GameLogic.h"
#include "WorldLevelTask/WorldLevelTaskManager.h"
#include "ScoreMatch/ScoreMatchManager.h"
#include "ResourceWar/ResourceWarManager.h"
#include "HangUpDrop/HangUpDropMgr.h"

Player::Player()
{
	memset(&m_tRoleFullInfo, 0, sizeof(m_tRoleFullInfo));
	//m_pOrderMgr = new COrderManager;
	m_pItemMgr = new CItemManager();
	m_pHeroMgr = new CHeroManager();
	m_pTreasureMgr = new CTreasureManager();
	m_pEquipmentMgr = new CEquipmentManager();
	m_pFubenMgr = new CFubenManager();
	m_pSpiritMgr = new CSpriteManager();
	m_pShopMgr = new CShopManager();
	m_pEmailManager = new EmailManager();
	m_pFriendMgr = new CFriendManager();
	m_pRecyclingMgr = new CRecyclingManager();
	m_pDuobaoMgr = new CDuobaoManager();
	m_pAchievementMgr = new CAchievementManager();
	m_pDailyMissionMgr = new CDailyMissionManager();
	m_pVipMgr = new CVipManager();
	m_pLogicMgr = new CLogicManager();
	m_pPropertyManager = new CPropertyManager();
	m_pSevenDayManager = new CSevenDayManager();
	m_pZhanBuManager = new CZhanBuManager();
	m_pTitleManager = new CTitleManager();
	m_pActivityManager = new CActivityManager();
	m_pRedPointManager = new CRedPointManager();
	m_pRewardManager = new CRewardCenterManager();
	m_pRechargeManager = new CRechargeManager();
	m_pEndlessLandManager = new CEndlessLandManager();
	m_pGemManager = new CGemManager();
	m_pWorldLevelTaskMgr = new CWorldLevelTaskMananger();
	m_pHangUpDropMgr = new CHangUpDropMgr();
}

Player::~Player()
{
	if (m_pItemMgr)
	{
		//m_pItemMgr->Release();
		delete m_pItemMgr;
		m_pItemMgr = NULL;
	}

	if (m_pHeroMgr)
	{
		//m_pHeroMgr->Release();
		delete m_pHeroMgr;
		m_pHeroMgr = NULL;
	}

	if (m_pTreasureMgr)
	{
		//m_pTreasureMgr->Release();
		delete m_pTreasureMgr;
		m_pTreasureMgr = NULL;
	}

	if (m_pEquipmentMgr)
	{
		//m_pEquipmentMgr->Release();
		delete m_pEquipmentMgr;
		m_pEquipmentMgr = NULL;
	}

	if (m_pFubenMgr)
	{
		//m_pFubenMgr->Release();
		delete m_pFubenMgr;
		m_pFubenMgr = NULL;
	}

	if (m_pSpiritMgr)
	{
		//m_pSpiritMgr->Release();
		delete m_pSpiritMgr;
		m_pSpiritMgr = NULL;
	}

	if (m_pShopMgr)
	{
		//m_pShopMgr->Release();
		delete m_pShopMgr;
		m_pShopMgr = NULL;
	}

	if (m_pEmailManager)
	{
		//m_pEmailManager->Release();
		delete m_pEmailManager;
		m_pEmailManager = NULL;
	}

	if (m_pFriendMgr)
	{
		delete m_pFriendMgr;
		m_pFriendMgr = NULL;
	}

	if (m_pRecyclingMgr)
	{
		delete m_pRecyclingMgr;
		m_pRecyclingMgr = NULL;
	}

	if (m_pDuobaoMgr)
	{
		delete m_pDuobaoMgr;
		m_pDuobaoMgr = NULL;
	}

	if (m_pAchievementMgr)
	{
		delete m_pAchievementMgr;
		m_pAchievementMgr = NULL;
	}

	if (m_pDailyMissionMgr)
	{
		delete m_pDailyMissionMgr;
		m_pDailyMissionMgr = NULL;
	}

	if (m_pVipMgr)
	{
		delete m_pVipMgr;
		m_pVipMgr = NULL;
	}

	if (m_pLogicMgr)
	{
		delete m_pLogicMgr;
		m_pLogicMgr = NULL;
	}

	if (m_pPropertyManager)
	{
		delete m_pPropertyManager;
		m_pPropertyManager = NULL;
	}


	if (m_pSevenDayManager)
	{
		delete m_pSevenDayManager;
		m_pSevenDayManager = NULL;
	}

	if (m_pZhanBuManager)
	{
		delete m_pZhanBuManager;
		m_pZhanBuManager = NULL;
	}

	if (m_pTitleManager)
	{
		delete m_pTitleManager;
		m_pTitleManager = NULL;
	}

	if (m_pActivityManager)
	{
		delete m_pActivityManager;
		m_pActivityManager = NULL;
	}

	if (m_pRedPointManager)
	{
		delete m_pRedPointManager;
		m_pRedPointManager = NULL;
	}

	if (m_pRewardManager)
	{
		delete m_pRewardManager;
		m_pRewardManager = NULL;
	}

	if (m_pRechargeManager)
	{
		delete m_pRechargeManager;
		m_pRechargeManager = NULL;
	}

	if (m_pEndlessLandManager)
	{
		delete m_pEndlessLandManager;
		m_pEndlessLandManager = NULL;
	}

	if (m_pGemManager)
	{
		delete m_pGemManager;
		m_pGemManager = NULL;
	}

	if (m_pWorldLevelTaskMgr)
	{
		delete m_pWorldLevelTaskMgr;
		m_pWorldLevelTaskMgr = NULL;
	}

	if (m_pHangUpDropMgr)
	{
		delete m_pHangUpDropMgr;
		m_pHangUpDropMgr = NULL;
	}
}

//初始化玩家
BOOL Player::Init()
{
	memset(&m_tRoleFullInfo, 0, sizeof(m_tRoleFullInfo));
	memset(m_szAccID, 0, sizeof(m_szAccID));
	memset(m_szUTF8RoleName, 0, sizeof(m_szUTF8RoleName));
	memset(&m_tMapPos, 0, sizeof(m_tMapPos));
	m_dwGuid = 0;
	m_pAccountInfo = NULL;
	m_pSession = NULL;
	//m_nForbidTalkTime = 0;
	m_dwLastHeartBeatTime = 0;
	m_wHeartBeatFastTimes = 0;
	m_dwNetworkSeriesId = 0;
	m_dwTeamId = 0;
	m_wTeamDungeonId = 0;
	m_dwLastChatTime = 0;
	m_bIsHaveSpriteTalentSkillReward = false;

	//随机DB存储的间隔时间
	m_cSaveDBTimers.SetTimer(Rand::Instance()->urand(SAVE_DB_INTERVAL_TIME, SAVE_DB_INTERVAL_TIME * 2));

	//memset(&m_dwChatInterval, 0, sizeof(m_dwChatInterval));

// 	if (m_pOrderMgr)
// 		m_pOrderMgr->Init(this);

	if (m_pItemMgr)
		m_pItemMgr->Init(this);

	if (m_pHeroMgr)
		m_pHeroMgr->Init(this);

	if (m_pTreasureMgr)
		m_pTreasureMgr->Init(this);

	if (m_pEquipmentMgr)
		m_pEquipmentMgr->Init(this);

	if (m_pFubenMgr)
		m_pFubenMgr->Init(this);

	if (m_pSpiritMgr)
		m_pSpiritMgr->Init(this);

	if (m_pShopMgr)
		m_pShopMgr->Init(this);

	if (m_pEmailManager)
		m_pEmailManager->Init(this);

	if (m_pFriendMgr)
		m_pFriendMgr->Init(this);

	if (m_pRecyclingMgr)
		m_pRecyclingMgr->Init(this);

	if (m_pDuobaoMgr)
		m_pDuobaoMgr->Init(this);

	if (m_pAchievementMgr)
		m_pAchievementMgr->Init(this);

	if (m_pDailyMissionMgr)
		m_pDailyMissionMgr->Init(this);

	if (m_pVipMgr)
		m_pVipMgr->Init(this);

	if (m_pLogicMgr)
		m_pLogicMgr->Init(this);

	if (m_pPropertyManager)
		m_pPropertyManager->Init(this);

	if (m_pSevenDayManager)
		m_pSevenDayManager->Init(this);

	if (m_pZhanBuManager)
		m_pZhanBuManager->Init(this);

	if (m_pTitleManager)
		m_pTitleManager->Init(this);

	if (m_pActivityManager)
		m_pActivityManager->Init(this);

	if (m_pRedPointManager)
		m_pRedPointManager->Init(this);

	if (m_pRewardManager)
		m_pRewardManager->Init(this);

	if (m_pRechargeManager)
		m_pRechargeManager->Init(this);

	if (m_pEndlessLandManager)
		m_pEndlessLandManager->Init(this);

	if (m_pGemManager)
		m_pGemManager->Init(this);

	if (m_pWorldLevelTaskMgr)
		m_pWorldLevelTaskMgr->Init(this);

	if (m_pHangUpDropMgr)
		m_pHangUpDropMgr->Init(this);

	//最后调用基类的函数
	return Character::Init();
}

//释放玩家
VOID Player::Release()
{
// 	if (m_pOrderMgr)
// 		m_pOrderMgr->Release();


	//最后调用基类的函数
	Character::Release();
}

//更新玩家
VOID Player::Update(DWORD dwTick)
{
	//调用基类版本的
	Character::Update(dwTick);

	//DB存储计时器到期
	if (m_cSaveDBTimers.IsExpired(FALSE))
	{
		//触发存储DB事件
		OnSave(SAVE_DB_TYPE_TIMING);
		//重置DB存储计时器
		m_cSaveDBTimers.SetTimer(SAVE_DB_INTERVAL_TIME);
	}

	//更新聊天间隔时间
	//UpdateChatInterval(dwTick);
}

//创建玩家
VOID Player::Create( ServerSession* pSession, char* pszAccID, DWORD dwCharID, DWORD dwGuid, AccountInfo* pAccountInfo )
{
	m_pSession = pSession;
	if (pszAccID)
		strncpy(m_szAccID, pszAccID, sizeof(m_szAccID));
	SetObjectKey( dwCharID );
	m_tRoleFullInfo.tRoleBaseInfo.dwRoleId = dwCharID;
	SetGuid(dwGuid);
	m_pAccountInfo = pAccountInfo;
	m_pAccountInfo->SetPlayer(this);
	m_dwLastHeartBeatTime = 0;
	m_wHeartBeatFastTimes = 0;
	m_dwNetworkSeriesId = 0;
	m_dwTeamId = 0;
}

VOID Player::SetLevel(WORD wLevel)
{
	// 做等级上限限制
	WORD wMaxLevel = g_pCfgMgr->m_RoleLevelList.size();
	wLevel = min(wMaxLevel, wLevel);

	WORD wOldLevel = m_tRoleFullInfo.tRoleBaseInfo.wLevel;

	m_tRoleFullInfo.tRoleBaseInfo.wLevel = wLevel;

	OnUpdateAchievement(2, wLevel, false);

	// 新手引导特殊处理
// 	OnUpdateAchievement(24, wLevel, false);
// 	OnUpdateAchievement(25, wLevel, false);
// 	OnUpdateAchievement(26, wLevel, false);
// 	OnUpdateAchievement(27, wLevel, false);

	// 7天目标(等级奖)
	OnUpdateSevenDayTarget(2, wLevel, false);

	// 7天目标(等级突破)
	OnUpdateSevenDayTarget(16, wLevel, false);

	// 等级礼包活动
	m_pActivityManager->RoleLevelUp(wOldLevel+1, wLevel);
}

WORD Player::GetServerId()
{ 
	return g_MapServer.GetCurServerId(m_pAccountInfo->GetZoneID(), GetRoleId());
}

WORD Player::GetInitServerId()
{
	return m_pAccountInfo->GetZoneID();
}

VOID Player::SetRoleName(const char* pszRoleName)
{  
	if(pszRoleName) 
		strcpy_s(m_tRoleFullInfo.tRoleBaseInfo.szRoleName, MAX_ROLE_LEN, pszRoleName);

	ChangeLocalChar(GetRoleName(), m_szUTF8RoleName, sizeof(m_szUTF8RoleName));
};

VOID Player::SetMapPos(int nX, int nY)
{ 
	m_tMapPos.wMapX = nX;
	m_tMapPos.wMapY = nY;
};

WORD Player::GetRoleCreateDays()
{
	return GetPassDays(m_tRoleFullInfo.tRoleBaseInfo.dwRoleCreateTime, time(NULL));
}

DWORD Player::GetChannelType()
{
	return m_pAccountInfo->GetChannelUID();
}

WORD Player::GetVipGMLevel()
{
	return g_pCfgMgr->GetVipGMLevel(m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount);
}

void Player::AddRechargeAmount(DWORD dwRechargeAmount)
{
	m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount += dwRechargeAmount;

	// todo::特殊情况处理, 支持原来接口不变
	NotifyProperty(ROLE_PRO_RECHARGE_AMOUNT, m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount);
}

VOID Player::SetRoleFullInfo(RoleFullInfo& tRoleFullInfo)
{ 
	m_tRoleFullInfo = tRoleFullInfo;
	//memcpy(&m_tRoleFullInfo, &tCharFullInfo, sizeof(m_tRoleFullInfo)); 

	ChangeLocalChar(GetRoleName(), m_szUTF8RoleName, sizeof(m_szUTF8RoleName));
}

void Player::FillCommonData(PlayerCommonData *pBaseData)
{
	memset(pBaseData, 0, sizeof(PlayerCommonData));
	pBaseData->dwRoleId = GetCharID();
	pBaseData->wLevel = GetLevel();
	pBaseData->wHeadIcon = GetProperty(ROLE_PRO_HEAD_IMAGE);
	pBaseData->dwFightingPower = GetFightingPower();
	pBaseData->byVipLevel = GetProperty(ROLE_PRO_VIP_LEVEL);
	pBaseData->dwSpriteId = m_pSpiritMgr->GetToBattleSpiritId();
	pBaseData->dwTitleId = m_pTitleManager->GetCurUseTitleId();
	pBaseData->dwNicknameId = m_pTitleManager->GetCurUseNicknameId();
	pBaseData->wServerId = GetServerId();
	strcpy_s(pBaseData->szRoleName, sizeof(pBaseData->szRoleName) - 1, GetRoleName());
	strcpy_s(pBaseData->szAccount, sizeof(pBaseData->szAccount) - 1, GetAccID());
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(GetCharID());
	if (pGuild)
	{
		strcpy_s(pBaseData->szGuildName, sizeof(pBaseData->szGuildName) - 1, pGuild->GetGuildName());
	}
}

// 填充基本数据
void Player::FillBaseData(PlayerBaseData *pBaseData)
{
	// 重置数据
	//pBaseData->Init();

	pBaseData->dwRoleId = GetCharID();
	pBaseData->wLevel = GetLevel();
	pBaseData->wHeadIcon = GetProperty(ROLE_PRO_HEAD_IMAGE);
	pBaseData->dwFightingPower = GetFightingPower();
	pBaseData->byVipLevel = GetProperty(ROLE_PRO_VIP_LEVEL);
	pBaseData->dwTitleId = m_pTitleManager->GetCurUseTitleId();
	pBaseData->dwNicknameId = m_pTitleManager->GetCurUseNicknameId();
	pBaseData->wServerId = GetServerId();
	pBaseData->dwPlatform = GetChannelType();
	//pBaseData->dwOfflineTime = time(NULL);
	strcpy_s(pBaseData->szRoleName, sizeof(pBaseData->szRoleName) - 1, GetRoleName());
	strcpy_s(pBaseData->szAccount, sizeof(pBaseData->szAccount) - 1, GetAccID());
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(GetCharID());
	if (pGuild)
		strcpy_s(pBaseData->szGuildName, sizeof(pBaseData->szGuildName) - 1, pGuild->GetGuildName());
	else
		memset(pBaseData->szGuildName, 0, sizeof(pBaseData->szGuildName));

	// 填充阵型数据
	pBaseData->vToBattleHeroList.clear();
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_tRoleFullInfo.tHeroInfo.arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? m_pHeroMgr->GetHero(pFormation->dwHeroUID) : NULL;
		if (pHero)
		{
			ToBattleHeroData tToBattleHero;
			tToBattleHero.wHeroInfoId = pHero->dwIntoId;
			tToBattleHero.wLevel = pHero->wLevel;
			tToBattleHero.byQuality = pHero->wQuality;
			tToBattleHero.byProNum = 0;
			tToBattleHero.byPos = i + 1;

			// 属性
			m_pPropertyManager->FillSlotProList(i, tToBattleHero.arPorList, tToBattleHero.byProNum);
			pBaseData->vToBattleHeroList.push_back(tToBattleHero);
		}
	}
}

// 填充上阵英雄数据
void Player::FillToBattleHeroData(vector<ToBattleHeroData> &vHeroList)
{
	vHeroList.clear();
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		Formation *pFormation = &m_tRoleFullInfo.tHeroInfo.arFormationList[i];
		const HeroInfo *pHero = pFormation->dwHeroUID ? m_pHeroMgr->GetHero(pFormation->dwHeroUID) : NULL;
		if (pHero)
		{
			ToBattleHeroData tToBattleHero;
			memset(&tToBattleHero, 0, sizeof(tToBattleHero));
			tToBattleHero.wHeroInfoId = pHero->dwIntoId;
			tToBattleHero.wLevel = pHero->wLevel;
			tToBattleHero.byQuality = pHero->wQuality;
			tToBattleHero.byProNum = 0;
			tToBattleHero.byPos = i + 1;

			// 属性
			m_pPropertyManager->FillSlotProList(i, tToBattleHero.arPorList, tToBattleHero.byProNum);
			vHeroList.push_back(tToBattleHero);

			// 祝福精灵数据
			const SpiritData *pSpriteData = pFormation->dwBlessSpriteId ? m_pSpiritMgr->GetSpiritEx(pFormation->dwBlessSpriteId) : NULL;
			if (pSpriteData)
			{
				tToBattleHero.tBlessSpriteData.dwSpriteId = pSpriteData->dwId;
				tToBattleHero.tBlessSpriteData.wStar = pSpriteData->wStarLevel;
				tToBattleHero.tBlessSpriteData.wEnhanceLevel = pSpriteData->wEnhanceLevel;
				tToBattleHero.tBlessSpriteData.wTrainStage = pSpriteData->wTrainStage;
			}
		}
	}
}

void Player::ModifyPropertyList(const vector<Property> &vProList, BOOL bNotice, WORD wFromId)
{
	for (size_t i = 0; i < vProList.size(); ++i)
	{
		ModifyProperty(vProList[i].wType, vProList[i].nValue, bNotice, wFromId);
	}
}

void Player::ModifyProperty(WORD wPropertyType, INT32 nValue, BOOL bNotice, WORD wFromId, BOOL bSaveLog)
{
	if (0 != nValue)
	{
		// 是否有精灵天赋技能额外奖励
		if (m_bIsHaveSpriteTalentSkillReward && nValue > 0)
		{
			BYTE byFBType = m_pFubenMgr->GetBattleType();
			map<int, int> mCurrencyInfoList;
			m_pSpiritMgr->GetExtraRewardCurrencyInfo(byFBType, mCurrencyInfoList);
			int nExtraRewardValue = m_pSpiritMgr->GetExtraRewardCurrencyValue(mCurrencyInfoList, wPropertyType, nValue);
			nValue += nExtraRewardValue;
		}

		// 角色经验
		if (ROLE_PRO_EXP == wPropertyType)
		{
			AddExp(nValue);
		}
		// vip经验
		else if (ROLE_PRO_VIP_EXP == wPropertyType)
		{
			m_pVipMgr->AddExp(nValue);
		}
		else
		{
			int nCurValue = 0;
			if (ROLE_PRO_HEAD_IMAGE == wPropertyType || ROLE_PRO_GUILD_ID == wPropertyType 
				|| ROLE_PRO_DUOBAO_PEACE_OVER_TIME == wPropertyType)
				nCurValue = nValue;
			else
				nCurValue = GetProperty(wPropertyType) + nValue;

			SetProperty(wPropertyType, nCurValue, bNotice);
		}

		// 记录货币日志
		if (bSaveLog)
			SaveCurrencyLog(wPropertyType, nValue, wFromId);

		// 活动消耗 
		if (nValue < 0 && ROLE_PRO_DIAMOND == wPropertyType)
		{
			int nCostDiamond = abs(nValue);

			// 节日活动
			OnFestivalActiveTarget(ACTIVATE_TARGET, 32, nCostDiamond, true);
			OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 32, nCostDiamond, true);
			OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 32, nCostDiamond, true);
			OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 32, nCostDiamond, true);

			m_pActivityManager->GetRechargeAndCostRankActivity().AddCostDiamond(nCostDiamond);
		}
	}
}

void Player::SetProperty(WORD wPropertyType, INT32 nValue, BOOL bNotice)
{
 	nValue = max(nValue, 0); // 防止出现负数的情况

 	int nOldValue = GetProperty(wPropertyType);
 
 	switch(wPropertyType)
 	{
 	case ROLE_PRO_LEVEL:
  		SetLevel(nValue);
  		break;

	case ROLE_PRO_EXP:
 		m_tRoleFullInfo.tRoleBaseInfo.dwExp = nValue;
 		break;

 	case ROLE_PRO_GOLD:										
 		m_tRoleFullInfo.tRoleBaseInfo.dwGold = min(nValue, MAX_GOLD);
 		break;

	case ROLE_PRO_STAMINA:
		{
			// 更新体力恢复时间
			if (nValue < nOldValue)
			{
				const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(GetLevel());
				if (pLevelCfg && m_tRoleFullInfo.tRoleBaseInfo.dwStamina >= pLevelCfg->dwStaminaMax)
					m_tRoleFullInfo.tRoleBaseInfo.dwLastStaminaRecoverTime = time(NULL);
			}

			m_tRoleFullInfo.tRoleBaseInfo.dwStamina = nValue;
		}
		break;

	case ROLE_PRO_DIAMOND:
		m_tRoleFullInfo.tRoleBaseInfo.dwDiamond = nValue;
 		break;

	case ROLE_PRO_HERO_SOUL:
		m_tRoleFullInfo.tRoleBaseInfo.nHeroSoul = nValue;
 		break;

	case ROLE_PRO_ENERGY:
		{
			// 更新精力恢复时间
			if (nValue < nOldValue)
			{
				const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(GetLevel());
				if (pLevelCfg && m_tRoleFullInfo.tRoleBaseInfo.dwEnergy >= pLevelCfg->dwEnergyMax)
					m_tRoleFullInfo.tRoleBaseInfo.dwLastEnergyRecoverTime = time(NULL);
			}

			m_tRoleFullInfo.tRoleBaseInfo.dwEnergy = nValue;
		}
		break;

	case ROLE_PRO_LIKE_TIMES:
		m_tRoleFullInfo.tRoleBaseInfo.wClickLikeTimes = nValue;
		break;

	case ROLE_PRO_VIP_LEVEL:
		m_tRoleFullInfo.tRoleBaseInfo.sVipData.wLevel = nValue;
		break;

	case ROLE_PRO_REBEL_LEVEL:
		m_tRoleFullInfo.tRebelInfo.wRebelLevel = nValue;
		break;

	case ROLE_PRO_REBEL_VALUE:
		m_tRoleFullInfo.tRoleBaseInfo.dwRebelValue = nValue;
		break;

	case ROLE_PRO_WANTED:
		{
			// 更新通缉令恢复时间
			if (nValue < nOldValue)
			{
				const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(GetLevel());
				if (pLevelCfg && m_tRoleFullInfo.tRoleBaseInfo.nWanted >= pLevelCfg->dwWantedMax)
					m_tRoleFullInfo.tRoleBaseInfo.dwLastWantedRecoverTime = time(NULL);
			}

			m_tRoleFullInfo.tRoleBaseInfo.nWanted = nValue;
		}
		break;

	case ROLE_PRO_TOWER_PRESTIGE:
		m_tRoleFullInfo.tRoleBaseInfo.nTowerPrestige = nValue;
		break;

	case ROLE_PRO_ARENA_PRESTIGE:
		m_tRoleFullInfo.tRoleBaseInfo.nArenaPrestige = nValue;
		break;

	case ROLE_PRO_DAILY_INIT_LEVEL:
		m_tRoleFullInfo.tRoleBaseInfo.wDailyInitLevel = nValue;
		break;

	case ROLE_PRO_GUILD_CONSTIBUTION:
		m_tRoleFullInfo.tRoleBaseInfo.dwCurContribution = nValue;
		break;

	case ROLE_PRO_VIP_EXP:
		m_tRoleFullInfo.tRoleBaseInfo.sVipData.dwExp = nValue;
		break;

	case ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES:
		m_tRoleFullInfo.tRoleBaseInfo.wRiotSuppressTimes = nValue;
		break;

	case ROLE_PRO_HONOR:
		m_tRoleFullInfo.tRoleBaseInfo.nHonor = nValue;
		break;

	case ROLE_PRO_GOD_SOUL:
		m_tRoleFullInfo.tRoleBaseInfo.nGodSoul = nValue;
		break;

	case ROLE_PRO_HEAD_IMAGE:
		m_tRoleFullInfo.tRoleBaseInfo.dwHeadImage = nValue;
		break;

	case ROLE_PRO_WARCRAFT_BATTLE_TIMES: 
		m_tRoleFullInfo.tRoleBaseInfo.wWarcraftBattleTimes = nValue;
		break;

	case ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES: 
		m_tRoleFullInfo.tRoleBaseInfo.wBuyWarcraftBattleTimes = nValue;
		break;

	case ROLE_PRO_SCORE_MATCH_HONOR:
		m_tRoleFullInfo.tRoleBaseInfo.nScoreMatchHonor = nValue;
		break;

	case ROLE_PRO_DUOBAO_PEACE_OVER_TIME:
		m_tRoleFullInfo.tRoleBaseInfo.dwDBPeaceOverTime = nValue;
		break;

	case ROLE_PRO_LEAVE_GUILD_TIME:
		m_tRoleFullInfo.tRoleBaseInfo.dwLeaveGuildTime = nValue;
		break;

	case ROLE_PRO_LAST_ACTIVITY_NOTICE_ID:
		m_tRoleFullInfo.tRoleBaseInfo.wLastActivityNoticeId = nValue;
		break;

	case ROLE_PRO_FRIENDSHIP_PIONT:
		m_tRoleFullInfo.tRoleBaseInfo.wFriendshipPoint = nValue;
		break;

	case ROLE_PRO_TODAY_FRIENDSHIP_PIONT:
		m_tRoleFullInfo.tRoleBaseInfo.wTodayFriendshipPoint = nValue;
		break;

	case ROLE_PRO_ENDLESS_STONE:
		m_tRoleFullInfo.tRoleBaseInfo.nEndlessStore = nValue;
		break;

	case ROLE_PRO_FORBID_TO_TALK_OVER_TIME:
		m_tRoleFullInfo.tRoleBaseInfo.dwForbidToTalkOverTime = nValue;
		break;

	case ROLE_PRO_NOBILITY_LEVEL:
		m_tRoleFullInfo.tRoleBaseInfo.wNobilityLevel = nValue;
		break;

	case ROLE_PRO_FIGHTING_POWER:
		m_tRoleFullInfo.tRoleBaseInfo.dwFightingPower = nValue;
		break;

	case ROLE_PRO_GEM_COIN:
		m_tRoleFullInfo.tRoleBaseInfo.nGemCoin = nValue;
		break;

	case ROLE_PRO_DESTINY_COIN:
		m_tRoleFullInfo.tRoleBaseInfo.nDestinyCoin = nValue;
		break;

	case ROLE_PRO_STAR_SOUL:
		m_tRoleFullInfo.tRoleBaseInfo.nStarSoul = nValue;
		break;

	case ROLE_PRO_TODAY_HELP_BATTLE_TIMES:
		m_tRoleFullInfo.tRoleBaseInfo.wTodayUsedHelpBattleTimes = nValue;
		break;

	case ROLE_PRO_CRYSTAL:
		m_tRoleFullInfo.tRoleBaseInfo.nCrystal = nValue;
		break;
		
	case ROLE_PRO_RESOURCE_WAR_SCORE:
		m_tRoleFullInfo.tRoleBaseInfo.nResouceWarScore = nValue;
		break;

	case ROLE_PRO_YCOIN:
		m_tRoleFullInfo.tRoleBaseInfo.nYCoin = nValue;
		break;

	case ROLE_PRO_RECHARGE_AMOUNT:
		m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount = nValue;
		break;

	case ROLE_PRO_CHAT_TIMES:
		m_tRoleFullInfo.tRoleBaseInfo.wTodayChatTimes = nValue;
		break;

 	default:										
		MAPLOG(ERROR_LEVEL, "[%s]无法识别的属性类型[PropertyType:%d]", __FUNCTION__, wPropertyType);
		return;
 	}
 
	if (bNotice && nOldValue != nValue && wPropertyType < ROLE_PRO_NOT_NOTIFY_BEGIN)
 	{
		NotifyProperty(wPropertyType, nValue);
 	}

	// 数据有变动
	if (nOldValue != nValue)
		RoleInfoUpdate();
}

INT32 Player::GetProperty(WORD wPropertyType)
{
 	switch(wPropertyType)
 	{
	case ROLE_PRO_LEVEL:					return m_tRoleFullInfo.tRoleBaseInfo.wLevel;			// 等级
	case ROLE_PRO_EXP:						return m_tRoleFullInfo.tRoleBaseInfo.dwExp;				// 经验	
	case ROLE_PRO_GOLD:						return m_tRoleFullInfo.tRoleBaseInfo.dwGold;			// 铜钱	
	case ROLE_PRO_STAMINA:					return m_tRoleFullInfo.tRoleBaseInfo.dwStamina;			// 体力	
	case ROLE_PRO_DIAMOND:					return m_tRoleFullInfo.tRoleBaseInfo.dwDiamond;			// 砖石
	case ROLE_PRO_HERO_SOUL:				return m_tRoleFullInfo.tRoleBaseInfo.nHeroSoul;			// 雄魂
	case ROLE_PRO_HEAD_IMAGE:				return m_tRoleFullInfo.tRoleBaseInfo.dwHeadImage;		// 头像
	case ROLE_PRO_ENERGY:					return m_tRoleFullInfo.tRoleBaseInfo.dwEnergy;			// 精力
	case ROLE_PRO_LIKE_TIMES:				return m_tRoleFullInfo.tRoleBaseInfo.wClickLikeTimes;	// 点赞次数
	case ROLE_PRO_VIP_LEVEL:				return m_tRoleFullInfo.tRoleBaseInfo.sVipData.wLevel;	// vip等级
	case ROLE_PRO_REBEL_LEVEL:				return m_tRoleFullInfo.tRebelInfo.wRebelLevel;			// 叛军等级
	case ROLE_PRO_REBEL_VALUE:				return m_tRoleFullInfo.tRoleBaseInfo.dwRebelValue;		// 战功(叛军值)
	case ROLE_PRO_WANTED:					return m_tRoleFullInfo.tRoleBaseInfo.nWanted;			// 通缉令(征讨令)
	case ROLE_PRO_TOWER_PRESTIGE:			return m_tRoleFullInfo.tRoleBaseInfo.nTowerPrestige;	// 爬塔声望(威名)
	case ROLE_PRO_ARENA_PRESTIGE:			return m_tRoleFullInfo.tRoleBaseInfo.nArenaPrestige;	// 竞技场声望
	case ROLE_PRO_DAILY_INIT_LEVEL:			return m_tRoleFullInfo.tRoleBaseInfo.wDailyInitLevel;	// 每日初始等级
	case ROLE_PRO_GUILD_CONSTIBUTION:		return m_tRoleFullInfo.tRoleBaseInfo.dwCurContribution;	// 当前工会贡献值
	case ROLE_PRO_VIP_EXP:					return m_tRoleFullInfo.tRoleBaseInfo.sVipData.dwExp;	// 每日初始等级
	case ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES: return m_tRoleFullInfo.tRoleBaseInfo.wRiotSuppressTimes;// 领地暴动镇压次数
	case ROLE_PRO_HONOR:					return m_tRoleFullInfo.tRoleBaseInfo.nHonor;
	case ROLE_PRO_GOD_SOUL:					return m_tRoleFullInfo.tRoleBaseInfo.nGodSoul;
	case ROLE_PRO_WARCRAFT_BATTLE_TIMES:	return m_tRoleFullInfo.tRoleBaseInfo.wWarcraftBattleTimes;
	case ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES:return m_tRoleFullInfo.tRoleBaseInfo.wBuyWarcraftBattleTimes;
	case ROLE_PRO_SCORE_MATCH_HONOR:		return m_tRoleFullInfo.tRoleBaseInfo.nScoreMatchHonor;
	case ROLE_PRO_DUOBAO_PEACE_OVER_TIME:	return m_tRoleFullInfo.tRoleBaseInfo.dwDBPeaceOverTime;
	case ROLE_PRO_LEAVE_GUILD_TIME:			return m_tRoleFullInfo.tRoleBaseInfo.dwLeaveGuildTime;
	case ROLE_PRO_LAST_ACTIVITY_NOTICE_ID:	return m_tRoleFullInfo.tRoleBaseInfo.wLastActivityNoticeId;
	case ROLE_PRO_FRIENDSHIP_PIONT:			return m_tRoleFullInfo.tRoleBaseInfo.wFriendshipPoint;
	case ROLE_PRO_TODAY_FRIENDSHIP_PIONT:	return m_tRoleFullInfo.tRoleBaseInfo.wTodayFriendshipPoint;
	case ROLE_PRO_ENDLESS_STONE:			return m_tRoleFullInfo.tRoleBaseInfo.nEndlessStore;
	case ROLE_PRO_FORBID_TO_TALK_OVER_TIME: return m_tRoleFullInfo.tRoleBaseInfo.dwForbidToTalkOverTime;
	case ROLE_PRO_NOBILITY_LEVEL:			return m_tRoleFullInfo.tRoleBaseInfo.wNobilityLevel;
	case ROLE_PRO_FIGHTING_POWER:			return m_tRoleFullInfo.tRoleBaseInfo.dwFightingPower;
	case ROLE_PRO_GEM_COIN:					return m_tRoleFullInfo.tRoleBaseInfo.nGemCoin;
	case ROLE_PRO_DESTINY_COIN:				return m_tRoleFullInfo.tRoleBaseInfo.nDestinyCoin;
	case ROLE_PRO_STAR_SOUL:				return m_tRoleFullInfo.tRoleBaseInfo.nStarSoul;
	case ROLE_PRO_TODAY_HELP_BATTLE_TIMES:	return m_tRoleFullInfo.tRoleBaseInfo.wTodayUsedHelpBattleTimes;
	case ROLE_PRO_CRYSTAL:					return m_tRoleFullInfo.tRoleBaseInfo.nCrystal;
	case ROLE_PRO_RESOURCE_WAR_SCORE:		return m_tRoleFullInfo.tRoleBaseInfo.nResouceWarScore;
	case ROLE_PRO_YCOIN:					return m_tRoleFullInfo.tRoleBaseInfo.nYCoin;
	case ROLE_PRO_RECHARGE_AMOUNT:			return m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount;
	case ROLE_PRO_CHAT_TIMES:				return m_tRoleFullInfo.tRoleBaseInfo.wTodayChatTimes;

 	default:			
 		MAPLOG(GUI_LEVEL,"[%s]无法识别的属性类型[PropertyType:%d]",__FUNCTION__,wPropertyType);
 		break;
 	}

	return 0;
}

void Player::NotifyProperty(WORD wPropertyType, INT32 nValue)
{
	static MSG_S2C_ROLE_PROPERTY_NOTIFY msg;
	msg.m_tProperty.wType = wPropertyType;
	msg.m_tProperty.nValue = nValue;
	SendPacket(&msg);
}

DWORD Player::GetFightingPower(bool bIsUpdate)
{
	if (bIsUpdate)
		UpdateFightingPower();

	return GetProperty(ROLE_PRO_FIGHTING_POWER);
}

bool Player::AddObject(DWORD dwObjectId, DWORD dwOjbectNum, BOOL bNotice, WORD wFromId)
{
	return AddObjectEx(GetObejctTypeById(dwObjectId), dwObjectId, dwOjbectNum, bNotice, wFromId);
}

bool Player::AddObjectEx(BYTE byType, DWORD dwObjectId, DWORD dwOjbectNum, BOOL bNotice, WORD wFromId)
{
	// 物品
	if (byType == OBJECT_ITEM || byType == OBJECT_AWAKENING_ITEM)
	{
		m_pItemMgr->AddItem(dwObjectId, dwOjbectNum, bNotice, wFromId);
	}
	// 英雄
	else if (byType == OBJECT_HERO)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pHeroMgr->AddHero(dwObjectId, bNotice, wFromId);
		}	
	}
	// 宝物
	else if (byType == OBJECT_TREASURE)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pTreasureMgr->AddTreasure(dwObjectId, bNotice, wFromId);
		}
	}
	// 装备
	else if (byType == OBJECT_EQUIPMENT)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pEquipmentMgr->AddEquipent(dwObjectId, bNotice, wFromId);
		}	
	}
	// 精灵
	else if (byType == OBJECT_SPIRIT)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pSpiritMgr->AddSpirit(dwObjectId, wFromId);
		}
	}
	// 宝石
	else if (byType == OBJECT_GEM)
	{
		m_pGemManager->AddGem(dwObjectId, dwOjbectNum, bNotice, wFromId);
	}
	// 卡牌
	else if (byType == OBJECT_CARD)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pZhanBuManager->AddCard(dwObjectId, bNotice, wFromId);
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]未处理的对象类型[type:%d]", __FUNCTION__, byType);
		return false;
	}

	RoleInfoUpdate();

	return true;
}

bool Player::AddObjectData(const Item *pItem, BOOL bNotice, WORD wFromId)
{
	return AddObject(pItem->dwId, pItem->dwNum, bNotice, wFromId);
}

bool Player::AddObjectList(const vector<Item> &vItemList, BOOL bNotice, WORD wFromId)
{
	for (size_t i = 0; i < vItemList.size(); ++i)
	{
		if (!AddObjectData(&vItemList[i], bNotice, wFromId))
			return false;
	}
	return true;
}

// 写货币日志
void Player::SaveCurrencyLog(WORD wPropertyType, INT32 nValue, WORD wFromId)
{
	if (ROLE_PRO_DIAMOND == wPropertyType
/*		|| ROLE_PRO_GOLD == wPropertyType*/
		|| ROLE_PRO_HERO_SOUL == wPropertyType
		|| ROLE_PRO_TOWER_PRESTIGE == wPropertyType
		|| ROLE_PRO_ARENA_PRESTIGE == wPropertyType
		|| ROLE_PRO_REBEL_VALUE == wPropertyType
		|| ROLE_PRO_YCOIN == wPropertyType)
	{
		RoleCurrencyLogs_Save* pQuery = RoleCurrencyLogs_Save::ALLOC();
		pQuery->SetIndex(DB_ROLE_CURRENCY_LOGS_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_ROLE_CURRENCY_LOGS);
		pQuery->SetCurrencyType(wPropertyType);
		pQuery->SetCurrencyValue(nValue);
		pQuery->SetCurCurrencyValue(GetProperty(wPropertyType));
		pQuery->SetFromId(wFromId);
		pQuery->SetRoleId(GetRoleId());
		pQuery->SetRoleLevel(GetLevel());
		g_MapServer.MapDBQuery(pQuery);
	}
}

// 添加经验
void Player::AddExp(DWORD dwExp)
{
	// todo::查找BUG
	//MAPLOG(ERROR_LEVEL, "添加经验[RoleId:%d,Exp:%d]", GetRoleId(), dwExp);

	m_tRoleFullInfo.tRoleBaseInfo.dwExp += dwExp;

	bool bLevelUp = false;
	const RoleLevel_Config *pRoleLvCfg;
	while (true)
	{
		// 找不到配置文件则退出
		pRoleLvCfg = CConfigManager::Instance()->GetRoleLevel(GetLevel());
		if (!pRoleLvCfg)
		{
			MAPLOG(ERROR_LEVEL, "找不到角色等级配置文件[Level:%d]",GetLevel());
			break;
		}

		// 经验不足够升级,则退出
		if (!pRoleLvCfg->dwExp || m_tRoleFullInfo.tRoleBaseInfo.dwExp < pRoleLvCfg->dwExp)
			break;

		m_tRoleFullInfo.tRoleBaseInfo.dwExp -= pRoleLvCfg->dwExp;
		ModifyProperty(ROLE_PRO_LEVEL, 1);
		ModifyProperty(ROLE_PRO_STAMINA, pRoleLvCfg->dwStamina);
		ModifyProperty(ROLE_PRO_ENERGY, pRoleLvCfg->dwEnergy);
		bLevelUp = true;

		// 处理升级奖励
		const RoleLevel_Config *pLvUpRewardCfg = CConfigManager::Instance()->GetRoleLevel(GetLevel());
		if (pLvUpRewardCfg && pLvUpRewardCfg->vRewardItemList.size())
			AddObjectList(pLvUpRewardCfg->vRewardItemList, TRUE, FROM_ROLE_LEVEL_UP_REWARD);

		RoleInfoUpdate();

		UpdateFightingPower();

		//m_pHangUpDropMgr->OnLevelUp();

		// 记录等级数据
		RoleBaseInfo &tBaseInfo = m_tRoleFullInfo.tRoleBaseInfo;
		PlayerFubenInfo &tFubenInfo = m_tRoleFullInfo.tFubenInfo;
		MainFBData &tMainFBData = m_tRoleFullInfo.tFubenInfo.tMainFubenData;
		RoleLevelRecord_Save* pQuery = RoleLevelRecord_Save::ALLOC();
		pQuery->SetIndex(DB_ROLE_LEVEL_RECORD_SAVE);
		pQuery->SetThreadIndex(DB_THREAD_COMMON);
		RoleLevelRecord tLevelRecord;
		tLevelRecord.dwRoleId = tBaseInfo.dwRoleId;
		tLevelRecord.wLevel = tBaseInfo.wLevel;
		tLevelRecord.wVipLevel = tBaseInfo.sVipData.wLevel;
		tLevelRecord.dwDiamond = tBaseInfo.dwDiamond;
		tLevelRecord.dwGold = tBaseInfo.dwGold;
		tLevelRecord.wServerId = GetServerId();
		tLevelRecord.wMainFBChapter = tMainFBData.wChapter;
		tLevelRecord.wMainFBMap = tMainFBData.wChapter ? tMainFBData.arChapterList[tMainFBData.wChapter-1].byMapIdx : 0;
		tLevelRecord.nGuideX = tBaseInfo.nGuideX;
		tLevelRecord.nGuideY = tBaseInfo.nGuideY;
		tLevelRecord.dwFightingPower = GetFightingPower();
		tLevelRecord.dwRecordTime = time(NULL);
		tLevelRecord.wHeroFBChapter = tFubenInfo.tHeroFubenData.wChapter;
		tLevelRecord.wEliteFBChapter = tFubenInfo.tEliteFBData.wChapter;
		tLevelRecord.wTowerMapLevel = tFubenInfo.tTowerFubenData.wMapLevelRecord;
		tLevelRecord.wLandNum = CLandManager::Instance()->GetPlayerLandNum(GetRoleId());
		tLevelRecord.wWorldHeartStage = m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage;
		tLevelRecord.wArenaHistoryRank = CArenaManager::Instance()->GetPlayerHistoryRank(GetServerId(), GetRoleId());
		tLevelRecord.wItemNum = m_tRoleFullInfo.tItemInfo.wListNum;
		tLevelRecord.wEquipmentNum = m_tRoleFullInfo.tEquipmentInfo.wEquipmentNum;
		tLevelRecord.wHeroNum = m_tRoleFullInfo.tHeroInfo.wHeroNum;
		tLevelRecord.wTreasureNum = m_tRoleFullInfo.tTreasureInfo.wTreasureNum;
		pQuery->SetRecord(tLevelRecord);
		g_MapServer.MapDBQuery(pQuery);
	}

	NotifyProperty(ROLE_PRO_EXP, m_tRoleFullInfo.tRoleBaseInfo.dwExp);

	// 如果升级
	if (bLevelUp)
	{
		MSG_S2C_ROLE_LEVEL_UP_NOTIFY msg;
		msg.m_wLevel = GetLevel();
		SendPacket(&msg);

		CRankManager::Instance()->AddRoleRankData(RANK_LEVEL, this, GetLevel(), 0);
		CRankManager::Instance()->AddRoleRankData(RANK_BATTLE_VALUE, this, GetFightingPower(), 0);
	}
}

//处理玩家加载事件
VOID Player::OnLoad(RoleFullInfo& sRoleInfo)
{	
	SetRoleFullInfo(sRoleInfo);
}

//处理玩家存储事件
VOID Player::OnSave(SaveDBType enSaveDBType)
{
	if(!m_pAccountInfo || m_pAccountInfo->GetUserState() != USER_AT_GAME_STATE)
		return;

	RoleBaseInfo &tBaseInfo = m_tRoleFullInfo.tRoleBaseInfo;

	// 保存玩家基本数据

	// 更新玩家下线时间(其实并不一定是真的下线)
	tBaseInfo.dwLastLogoutTime = time(NULL);

	// 更新在线时间
	if (SAVE_DB_TYPE_LOGOUT == enSaveDBType)
	{
		DWORD dwLoginTime = tBaseInfo.dwLastLoginTime;
		DWORD dwLogouTime = tBaseInfo.dwLastLogoutTime;
		// 角色在线时间
		if (dwLogouTime > dwLoginTime){
			tBaseInfo.dwOnlineTime += (dwLogouTime - dwLoginTime);
		}

		// 在线奖励在线时间更新
		DWORD dwPassTime = dwLogouTime - m_tRoleFullInfo.tActivityInfo.tOnlineRewardData.dwStartTime;
		m_tRoleFullInfo.tActivityInfo.tOnlineRewardData.dwOnlineTime += dwPassTime;
	}

	// 填充属性(todo::这种写法有一定的风险)
	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const int *pSrcList = m_pPropertyManager->GetSlotProList(i);
		if (pSrcList)
		{
			int *pDestList = tBaseInfo.arFormationProList[i];
			int nCopySize = sizeof(int)* HERO_PRO_MAX;
			memcpy_s(pDestList, nCopySize, pSrcList, nCopySize);
		}
	}

	tBaseInfo.dwUpdateTimes++;

	tBaseInfo.wArenaHistroyRank = CArenaManager::Instance()->GetPlayerHistoryRank(GetServerId(), GetRoleId());
	tBaseInfo.wLandNum = CLandManager::Instance()->GetPlayerLandNum(GetRoleId());

	//最后向数据库发送请求
	UpdateRoleInfoQuery* pQuery = UpdateRoleInfoQuery::ALLOC();
	pQuery->SetIndex(MAP_QUERY_UPDATE_CHAR);
	pQuery->SetThreadIndex(m_pAccountInfo->GetDBIndex());
	pQuery->SetKey(m_tRoleFullInfo.tRoleBaseInfo.dwRoleId);
	pQuery->SetCharID(m_tRoleFullInfo.tRoleBaseInfo.dwRoleId);
	pQuery->SetSaveType((BYTE)enSaveDBType);
	pQuery->SetServerID(GetServerId());
	pQuery->SetRoleFullData(m_tRoleFullInfo);
	g_MapServer.MapDBQuery(pQuery);

	if (enSaveDBType == SAVE_DB_TYPE_LOGOUT)
	{		
		MAPLOG(GUI_LEVEL, "玩家下线[RoleId:%d,CharRole=%s,Guid:%d,LastLogoutTime:%d]", 
			GetRoleId(), GetUTF8RoleName(), GetGuid(), GetLastLogoutTime());

		// todo::为方便查找BUG输出日志
		PrintPVPData(false);
	}
}

//处理玩家登录事件
VOID Player::OnLogin()
{
	// todo::此处的逻辑顺序需要重新整理一下

	bool bIsNewRole = IsNewRole();
	bool bIsNewDay = IsNewDay();
	bool bIsNewWeek = IsNewWeek();
	bool bIsNewMonth = IsNewMonth();

	DWORD dwLastLogoutTime = GetLastLogoutTime();

	// 检测是否新建角色
	if (bIsNewRole)
		OnNewRole();

	//此时才有角色名，加入角色名列表（暂定）
	PlayerManager::Instance()->AddPlayerName(this);

	//更新玩家登陆时间
	DWORD dwCurTime = time(NULL);
	m_tRoleFullInfo.tRoleBaseInfo.dwLastLoginTime = dwCurTime;

	// 离线数据转化处理
	DoTempData(false);

	// 新的一天
	if (bIsNewDay)
		OnNewDay();

	// 新的一周
	if (bIsNewWeek)
		OnNewWeek();

	// 新的月份
	if (bIsNewMonth)
		OnNewMonth();

	PlayerBaseDataManager::Instance()->OnPlayerLogin(this);

	// todo::兼容旧版本代码
	m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage = max(m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage, 1);
	//m_tRoleFullInfo.tRoleBaseInfo.wNobilityLevel = max(m_tRoleFullInfo.tRoleBaseInfo.wNobilityLevel, 1);

	// 计算战力
	//UpdateFightingPower();


	//发送玩家基本信息
	SendRoleBaseInfo();

	//发送英雄列表
	m_pHeroMgr->SendHeroBaseList();

	// 发送阵型列表
	m_pHeroMgr->SendFormationList();

	// 更新战力
	UpdateFightingPower();

	DWORD dwOpenDay = g_MapServer.GetAlreadyOpenServerDay(GetServerId());

	OnUpdateSevenDayTarget(1, dwOpenDay, false);

	// 同步服务器时间
// 	{
// 		MSG_S2C_SERVER_TIME_RESP msg;
// 		msg.m_dwServerTime = (DWORD)time(NULL);
// 		SendPacket(&msg);
// 	}
	// 兼容旧数据及容错处理
// 	if (!m_pGemManager->IsInitDefaultOpenHole())
// 		m_pGemManager->InitDefaultOpenHole();

	m_pActivityManager->GetFestivalActivity().OnNetFestivalLogin();
	m_pActivityManager->GetSevenDayLogin().OnLogin();
	m_pActivityManager->GetFacebookActivity().OnLogin();

	//最后发送消息给客户端通知加载完成
	//MSG_M2C_LOAD_CHAR_FINISH_CMD msg;
	//SendPacket(&msg);

	// 同步数据到网关
	MSG_M2G_CLIENT_INFO client_info_msg;
	client_info_msg.m_wServerId = GetServerId();
	client_info_msg.m_dwRoleId = GetRoleId();
	SendPacket(&client_info_msg);

	MSG_S2C_LOGIN_ENTER_GAME_NOTIFY enter_game_msg;
	SendPacket(&enter_game_msg);

	MAPLOG(GUI_LEVEL, "玩家登录成功,发送基本数据[RoleId:%d,Guid:%d,LastLogoutTime:%d]", GetRoleId(), GetGuid(), dwLastLogoutTime);

	// todo::为方便查找BUG输出日志
	PrintPVPData(true);

	// 处理广播
	WORD wRank = CArenaManager::Instance()->GetPlayerRank(GetServerId(), GetRoleId());
	if (wRank && wRank <= 1)
	{
		CSystemNotice sys_notice(5, GetServerId());
		sys_notice.AddParam(GetRoleName());
		sys_notice.SendMsg();
	}

	m_pFubenMgr->GetEliteFB().Login();

	// 打补丁逻辑
	m_pItemMgr->DeleteTimeOutItem();
	
	// 旧数据兼容逻辑处理
	m_pGemManager->InitDefaultOpenHole();

	// 记录在线奖励开始计时时间点
	m_tRoleFullInfo.tActivityInfo.tOnlineRewardData.dwStartTime = time(NULL);

	// 检测圣诞数据是否需要重置
	CheckAndResetChristmasData();


	// todo::BUG补救 
//  	FestivalActivity &tOpenSvrAccumulateRecharge = m_tRoleFullInfo.tActivityInfo.tFestivalActivityInfo.tOpenSvrAccumulateRecharge;
//  	if (m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount && !tOpenSvrAccumulateRecharge.wTargetNum)
//  	{
//  		// 开服累充
//  		m_pActivityManager->GetFestivalActivity().DoAccumulateRechargeEvent(OPEN_SVR_ACCUMULATE_RECHARGE, 
//  			m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount);
//  	}

	// 更新排行榜(主要是考虑合服的情况,因为合服排行榜会补清掉,大于一定等级才处理)
	if (GetLevel() >= 20 && !CRankManager::Instance()->GetRoleRankDataEx(GetServerId(), RANK_LEVEL, GetRoleId()))
	{
		CRankManager::Instance()->AddRoleRankData(RANK_LEVEL, this, GetLevel(), 0);
		CRankManager::Instance()->AddRoleRankData(RANK_BATTLE_VALUE, this, GetFightingPower(), 0);
		CRankManager::Instance()->AddRoleRankData(RANK_MAIN_FB_STAR, this, m_pFubenMgr->GetMainFB().GetTotalStar());
		CRankManager::Instance()->AddRoleRankData(RANK_TOWER, this, m_pFubenMgr->GetTower().GetMaxStarRecord());
		CRankManager::Instance()->AddRoleRankData(RANK_ENDLESS_KILL, this, m_pEndlessLandManager->GetContinueKillNumRecord());
		CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, this, m_pZhanBuManager->GetHistoryCollectionValue());
	}

	// 发放封测充值返利等级(目前只有一服才会发放)
	if (GetServerId() == 1 && !m_tRoleFullInfo.tRoleBaseInfo.wIsAlreadyTakeClosedTestRebate && GetLevel() >= g_GetConfigValue(217))
	{
		const ClosedTestRebate_Config *pRebateCfg = g_pCfgMgr->GetClosedTestRebate(GetAccID());
		if (pRebateCfg)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 19;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = GetRoleId();
			CRewardCenterManager::AddReward(GetServerId(), GetRoleId(), tRewardData);

			MAPLOG(ERROR_LEVEL, "给玩家发放封测返利钻石[RoleId:%d,Diamond:%d]", GetRoleId(), pRebateCfg->nDiamond);

			m_tRoleFullInfo.tRoleBaseInfo.wIsAlreadyTakeClosedTestRebate = true;
		}
	}

	// 统计玩家登陆
	CGameLogic::Instance()->OnLoginStatistics(this);

	//CGameLogic::Instance()->OnGameStatistics_Login(this, bIsNewDay);

	// 更新登录次数
	m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes++;
	// 不应该出现这种情况
	if (m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes >= 450000)
	{
		m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes = 1;
		MAPLOG(ERROR_LEVEL, "[RoleId:%d]玩家登录次数出现异常,登录次数超过450000次",GetRoleId());
	}
}

//处理玩家登出事件
VOID Player::OnLogout()
{
	//CSceneManager::Instance()->PlayerLeave(this);
	PlayerBaseDataManager::Instance()->OnPlayerLogout(this);
	CPVPManager::Instance()->OnLogout(this);
	CAttackCityManager::Instance()->OnLogout(this);
	CTeamDungeonManager::Instance()->OnLogout(this);
	CKingMatchManager::Instance()->OnLogout(this);
	CChristmas::Instance()->OnLogout(this);
	CScoreMatchManager::Instance()->OnLogout(this);
	CResourceWarManager::Instance()->OnLogout(this);
	m_pActivityManager->OnLogout();
}

//处理玩家升级事件
VOID Player::OnLevelUp()
{
	// 通知客户端表现升级
	static MSG_S2C_ROLE_LEVEL_UP_NOTIFY msg;
	msg.m_wLevel = GetLevel();
	SendPacket(&msg);
}

VOID Player::OnNewDay()
{
	m_tRoleFullInfo.tRoleBaseInfo.wClickLikeTimes = 0;
	//m_tRoleFullInfo.tRebelInfo.wInitRoleLevel = GetLevel();
	m_tRoleFullInfo.tRoleBaseInfo.wDailyInitLevel = GetLevel();
	m_tRoleFullInfo.tRoleBaseInfo.wRiotSuppressTimes = 0;
	m_tRoleFullInfo.tHeroInfo.wNormalFreeSummonTimes = 0;
	m_tRoleFullInfo.tRoleBaseInfo.wTodayFriendshipPoint = 0;
	m_tRoleFullInfo.tSecondInfo.tPVPData.wTodayBattleTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tPVPData.wDailyRewardTakeRecordNum = 0;
	m_tRoleFullInfo.tRoleBaseInfo.wTodayUsedHelpBattleTimes = 0;
	m_tRoleFullInfo.tRoleBaseInfo.wLoginDays++;
	m_tRoleFullInfo.tDuobaoInfo.wDailyDoubaoTimes = 0;
	m_tRoleFullInfo.tDuobaoInfo.wPregrossRewardRecordNum = 0;
	m_tRoleFullInfo.tRoleBaseInfo.wTodayChatTimes = 0;
	


	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wCurWave = 1;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wTodayWaveRecord = 0;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wTodayBattleTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wCurContinueKillNum = 0;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wTodayCollisionRewardTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tLuxurySignData.wSignDataNum = 0;


	VipGiftData &tVipGiftData = m_tRoleFullInfo.tActivityInfo.tVipGiftData;
	tVipGiftData.wInitVipLevel = GetProperty(ROLE_PRO_VIP_LEVEL);
	tVipGiftData.nVipLevelRewardRecord = tVipGiftData.wInitVipLevel - 1; // 每日可以领取当前等级的

	m_tRoleFullInfo.tSecondInfo.tLoginRewardData.wLoginDays++;

	m_tRoleFullInfo.tSecondInfo.tKingMatchData.wTodayWinTimes = 0;

	// 组队副本每日挑战次数
	m_tRoleFullInfo.tSecondInfo.tTeamDungeonData.wTodayBattleTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tTeamDungeonData.wTodayKillRewardTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tTeamDungeonData.wTodayMVPRewardTimes = 0;

	m_tRoleFullInfo.tZhanBuInfo.wTodayLowCardGroupOpenCardTimes = 0;
	m_tRoleFullInfo.tZhanBuInfo.wTodayLowCardGroupCostRefreshTimes = 0;
	m_tRoleFullInfo.tZhanBuInfo.wTodayHighCardGroupCostRefreshTimes = 0;

	m_pFriendMgr->OnNewDay();
	m_pFubenMgr->OnNewDay();
	m_pShopMgr->OnNewDay();
	m_pDailyMissionMgr->OnNewDay();
	m_pActivityManager->OnNewDay();
	m_pWorldLevelTaskMgr->OnNewDay();

	// 主线副本重置
	{
		MainFBData &tMainFBData = m_tRoleFullInfo.tFubenInfo.tMainFubenData;
		for (WORD i = 0; i < tMainFBData.wChapter; ++i)
		{
			ChapterData *pChapter = &tMainFBData.arChapterList[i];
			for (BYTE k = 0; k < pChapter->byMapIdx; ++k)
			{
				pChapter->arMapList[k].byBattleTimes = 0;
				pChapter->arMapList[k].byResetTimes = 0;
			}
		}
	}

	// 英雄副本重置
	{
		HeroFBData &tHeroFBData = m_tRoleFullInfo.tFubenInfo.tHeroFubenData;
		tHeroFBData.byBattleTimes = 0;
		for (WORD i = 0; i < tHeroFBData.wChapter; ++i)
		{
			HeroFubenChapterData *pChapter = &tHeroFBData.arChapterList[i];
			for (BYTE k = 0; k < pChapter->byMapIdx; ++k)
			{
				pChapter->arMapList[k].byBattleTimes = 0;
			}
		}
	}

	// 日常副本重置
	{
		DailyFBData &tDailyFBData = m_tRoleFullInfo.tFubenInfo.tDailyFubenData;
		for (WORD i = 0; i < tDailyFBData.wChapterNum; ++i)
		{
			tDailyFBData.arChapterList[i].byBattleTimes = 0;
		}
	}

	// 爬塔副本重置
	{
		TowerFBData &tTowerFBData = m_tRoleFullInfo.tFubenInfo.tTowerFubenData;
		tTowerFBData.byTodayResetTimes = 0;
		tTowerFBData.byEliteMapBattleTimes = 0;
		tTowerFBData.byBuyEliteMapBattleTimes = 0;
	}

	// 精英副本
	{
		EliteFBData &tEliteFBData = m_tRoleFullInfo.tFubenInfo.tEliteFBData;
		for (WORD i = 0; i < tEliteFBData.wChapter; ++i)
		{
			EliteFBChapter *pChapter = &tEliteFBData.arChapterList[i];
			memset(&pChapter->tEnemyData, 0, sizeof(pChapter->tEnemyData));
			for (BYTE k = 0; k < pChapter->byMapIdx; ++k)
			{
				pChapter->arMapList[k].byBattleTimes = 0;
				pChapter->arMapList[k].byResetTimes = 0;
			}
		}
	}

	// 叛军数据
	PlayerRebelData &tRebelData = m_tRoleFullInfo.tRebelInfo;
	tRebelData.wAwardRecordNum = 0;
	tRebelData.dwTodayDamage = 0;
	tRebelData.dwTodayExploit = 0;

	// 活动数据 todo::应该移到活动管理器里去处理
	PlayerActivityData &tActivityData = m_tRoleFullInfo.tActivityInfo;
	tActivityData.tYCSData.byTodayGetTimes = 0;
	tActivityData.tTQTData.wTakeRecordNum = 0;
	tActivityData.tSuperDiscountData.wStatus = 0;
	tActivityData.tOnlineRewardData.dwOnlineTime = 0;
	tActivityData.tNewSuperDiscountData.wStatus = 0;
	tActivityData.tUnlockActivityData.wWatchingVideosTimes = 0;
	tActivityData.tPlayerCampRecruitData.wTodayAlreadyRecruitTimes = 0;

	// 潜能(todo::已调整为不清掉)
// 	PlayerHeroInfo &tHeroInfo = m_tRoleFullInfo.tHeroInfo;
// 	for (WORD i = 0; i < tHeroInfo.wHeroNum; ++i)
// 	{
// 		tHeroInfo.arHeroList[i].dwDestinyExp = 0;
// 	}

	// 好友助战次数重置
	PlayerFriendData &tFriendInfo = m_tRoleFullInfo.tFriendInfo;
	for (WORD i = 0; i < tFriendInfo.wFriendNum; ++i)
	{
		tFriendInfo.arFriendList[i].wHelpBattleTimes = 0;
	}

	// 重新登录事件,隔天如果是在线也当着是重新登录
	m_pActivityManager->GetFestivalActivity().OnNetFestivalLogin();

	// 删除过期物品
	m_pItemMgr->DeleteTimeOutItem();

	RoleInfoUpdate();

	// todo:::
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
		m_tRoleFullInfo.tHeroInfo.wHighSummonTimes = 0;

	// 统计玩家登陆
	//CGameLogic::Instance()->OnLoginStatistics(this);

	CGameLogic::Instance()->OnGameStatistics_Login(this, true);

	MAPLOG(SPECAIL_LEVEL, "玩家每日事件触发[RoleId:%d]", GetRoleId());
}

VOID Player::OnNewWeek()
{
	PlayerPVPData &tPVPData = m_tRoleFullInfo.tSecondInfo.tPVPData;
	tPVPData.wWeekBattleTimes = 0;
	tPVPData.wWeeklyRewardTakeRecordNum = 0;
	tPVPData.dwScore = 0;
	tPVPData.wWeekWinTimes = 0;

	if (m_pActivityManager)
		m_pActivityManager->RefreshWeekGift();

	m_pShopMgr->OnNewWeek();

	MAPLOG(SPECAIL_LEVEL, "[%s]玩家每周事件触发[RoleId:%d]", __FUNCTION__, GetRoleId());
}

// 新的一月(每个月1号0点)
void Player::OnNewMonth()
{
	m_pShopMgr->OnNewMonth();

	MAPLOG(SPECAIL_LEVEL, "[%s]玩家每月事件触发[RoleId:%d]", __FUNCTION__, GetRoleId());
}

// 处理临时数据
VOID Player::DoTempData(bool isSave)
{
	if (m_tRoleFullInfo.tTempDatas.wTempDataNum)
	{
		for (WORD i = 0; i < m_tRoleFullInfo.tTempDatas.wTempDataNum; ++i)
		{
			RoleTempData &tTempData = m_tRoleFullInfo.tTempDatas.arTempDataList[i];
			// 称号
			if (ROLE_TEMP_DATA_TITLE == tTempData.wType)
			{
				m_pTitleManager->AddTitle(tTempData.arParamList[1], tTempData.arParamList[0], false);
			}
			// 禁言设置
			else if (ROLE_TEMP_FORBID_TO_TALK_SET == tTempData.wType)
			{
				// 禁言
				if (tTempData.arParamList[1])
				{
					m_tRoleFullInfo.tRoleBaseInfo.dwForbidToTalkOverTime = tTempData.arParamList[0];
				}
				// 解禁
				else
				{
					m_tRoleFullInfo.tRoleBaseInfo.dwForbidToTalkOverTime = 0;
				}
			}
			// 充值发货
			else if (ROLE_TEMP_RECHARGE_SEND_GOODS == tTempData.wType)
			{
				RechargeCallbackData callbackData;
				memset(&callbackData, 0, sizeof(callbackData));
				DWORD dwTime = tTempData.arParamList[0];
				callbackData.byStatus = tTempData.arParamList[1];
				callbackData.wRechargeId = tTempData.arParamList[2];
				callbackData.dwCPOrderId = tTempData.arParamList[3];
				callbackData.nAmount = tTempData.arParamList[4];
				callbackData.dwChannel = tTempData.arParamList[5];
				callbackData.wRechargeType = tTempData.arParamList[6];
				callbackData.wGoodsId = tTempData.arParamList[7];
				callbackData.dwYDChannelId = tTempData.arParamList[8];
				callbackData.wCurrencyType = tTempData.arParamList[9];
				callbackData.byIsMyCard = tTempData.arParamList[10];
				callbackData.byIsCheckRechargeStrId = tTempData.arParamList[11];
				callbackData.byIsNotCheck = tTempData.arParamList[12];
				callbackData.wFrom = tTempData.arParamList[13];

				Json::Value root;
				Json::Reader reader;
				if (reader.parse(tTempData.szText, root))
				{
					Json::Value json_ip = root["ip"];
					if (!json_ip.isNull() && json_ip.isString())
					{
						strcpy_s(callbackData.szIP, sizeof(callbackData.szIP), json_ip.asString().c_str());
					}

					Json::Value json_sdk_order_id = root["sdk_order_id"];
					if (!json_sdk_order_id.isNull() && json_sdk_order_id.isString())
					{
						strcpy_s(callbackData.szSDKOrderId, sizeof(callbackData.szSDKOrderId), json_sdk_order_id.asString().c_str());
					}

					Json::Value json_pay_time = root["pay_time"];
					if (!json_pay_time.isNull() && json_pay_time.isString())
					{
						strcpy_s(callbackData.szPayTime, sizeof(callbackData.szPayTime), json_pay_time.asString().c_str());
					}
					Json::Value json_recharge_str_id = root["recharge_str_id"];
					if (!json_recharge_str_id.isNull() && json_recharge_str_id.isString())
					{
						strcpy_s(callbackData.szRechargeStrId, sizeof(callbackData.szRechargeStrId), json_recharge_str_id.asString().c_str());
					}
				}
				
				// 充值成功,并且还没处理过
				m_pRechargeManager->OnNetRechargeCallback(callbackData);

				MAPLOG(GUI_LEVEL, "补发充值奖励[role_id:%d,amount:%d,channel:%d,order_id:%s]",
					GetRoleId(), callbackData.nAmount, callbackData.dwChannel, callbackData.szSDKOrderId);
			}
			// 设置引导
			else if (ROLE_TEMP_SET_GUIDE == tTempData.wType)
			{
// 				DWORD dwServerId = tTempData.arParamList[0];
// 				DWORD dwRoleId = tTempData.arParamList[1];
				DWORD dwGuideKey = tTempData.arParamList[2];
				DWORD dwGuideValue = tTempData.arParamList[3];
				m_pLogicMgr->SetGuide(dwGuideKey, dwGuideValue);
			}
			else if (ROLE_TEMP_SET_LEAVE_GUILD_TIME == tTempData.wType)
			{		
				DWORD dwLeaveTime = tTempData.arParamList[1];	// 1 leave_time
				//DWORD dwGuildId = tTempData.arParamList[2];		// 2 guild_id
				//DWORD dwServerId = tTempData.arParamList[3];	// 3 server_id
				ModifyProperty(ROLE_PRO_LEAVE_GUILD_TIME, dwLeaveTime);
			}

			// 奖励中心数据

			// 邮件

			// 好友
		}

		m_tRoleFullInfo.tTempDatas.wTempDataNum = 0;

		RoleInfoUpdate(isSave);
	}
}

BOOL Player::IsNewRole()
{
	return m_tRoleFullInfo.tRoleBaseInfo.dwLastLoginTime ? FALSE : TRUE;
}

// 新建角色
VOID Player::OnNewRole()
{
	RoleBaseInfo &tBaseInfo = m_tRoleFullInfo.tRoleBaseInfo;

	DWORD dwCurTime = DWORD(time(NULL));

	// 活动数据
	m_tRoleFullInfo.tActivityInfo.tVipGiftData.nVipLevelRewardRecord = -1;

	// 叛军数据初始化
	m_tRoleFullInfo.tRebelInfo.wRebelLevel = 1;
	//m_tRoleFullInfo.tRebelInfo.wInitRoleLevel = GetLevel();

	// 精灵数据初始化
	m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage = 1;

	// 每日等级初始化
	tBaseInfo.wDailyInitLevel = GetLevel();

	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wCurWave = 1;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wTodayBattleTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wIsFirstBattle = true;


	// 属性恢复时间
	tBaseInfo.dwLastStaminaRecoverTime = dwCurTime;
	tBaseInfo.dwLastEnergyRecoverTime = dwCurTime;
	tBaseInfo.dwLastWantedRecoverTime = dwCurTime;

	// 角色属性初始化
	const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(GetLevel());
	if (pLevelCfg)
	{
		tBaseInfo.dwStamina = pLevelCfg->dwStaminaMax;
		tBaseInfo.dwEnergy = pLevelCfg->dwEnergyMax;
		tBaseInfo.nWanted = pLevelCfg->dwWantedMax;
	}

	// 周礼包
	if (m_pActivityManager)
		m_pActivityManager->RefreshWeekGift();

	m_pActivityManager->InitLuxurySignInData();

	m_pActivityManager->ResetTodayRecommendRechargeData();

	m_pGemManager->OnNewRole();

	m_pWorldLevelTaskMgr->UpdateTask();

	m_pShopMgr->OnNewRole();

	const RoleInit_Config *pInitCfg = g_pCfgMgr->GetRoleInit();
	if (pInitCfg)
	{
		// 头像
		tBaseInfo.dwHeadImage = pInitCfg->nHeadImage;

		// 物品
		if (pInitCfg->vItemList.size())
			AddObjectList(pInitCfg->vItemList, FALSE, FROM_NEW_ROLE_REWARD);

		// 属性
		for (size_t i = 0; i < pInitCfg->vProList.size(); ++i)
		{
			ModifyProperty(pInitCfg->vProList[i].wType, pInitCfg->vProList[i].nValue, FALSE, FROM_NEW_ROLE);
		}

		//DWORD dwPlatfromType = GetAccountInfo()->GetPlatformType();
		//MAPLOG(GUI_LEVEL, "渠道类型[PlatfromType：%d]", dwPlatfromType);

		const ClientInfo *pClientInfo = CGameLogic::Instance()->GetClientInfo(m_pAccountInfo->GetChannelUID(), m_pAccountInfo->GetAccID());
		BYTE byTerminalType = pClientInfo ? pClientInfo->byTerminalType : 0;

		// 英雄
		if (TERMINAL_IOS == byTerminalType && g_MapServer.IsExamineServer())	// 提审状态(只有ios才需要提审) 
		{
			// (todo::提审使用的功能)
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(269);
			if (pServerCfg && pServerCfg->vValueList.size())
			{
				DWORD dwHeroInfoId = pServerCfg->GetListValue(g_Rand(0, pServerCfg->vValueList.size() - 1));
				DWORD dwHeroUID = m_pHeroMgr->AddHero(dwHeroInfoId, false, FROM_NEW_ROLE);
				m_tRoleFullInfo.tHeroInfo.arFormationList[0].dwHeroUID = dwHeroUID;
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "找不到随机英雄列表配置[key=269]");
			}

			MAPLOG(GUI_LEVEL, "提审号创建角色[role_id：%d,terminal_type:%d]", GetRoleId(), byTerminalType);
		}
		else
		{
			if (pInitCfg->vHeroList.size())
			{
				for (size_t i = 0; i < pInitCfg->vHeroList.size(); ++i)
				{
					DWORD dwHeroUID = m_pHeroMgr->AddHero(pInitCfg->vHeroList[i], false, FROM_NEW_ROLE);
					if (i < MAX_BATTLE_HERO_NUM)
						m_tRoleFullInfo.tHeroInfo.arFormationList[i].dwHeroUID = dwHeroUID;
				}

				// 成就
				OnUpdateAchievement(21, m_pHeroMgr->GetToBattleHeroNum(), false);
			}
		}
	}

	// 新建角色邮件
// 	DWORD dwEmailId = g_GetConfigValue(296);
// 	if (dwEmailId)
// 	{
// 		EmailInfo tEailInfo(dwEmailId);
// 		EmailManager::AddEmail(GetRoleId(), tEailInfo, false);
// 	}
}

BOOL Player::IsTodayCreateRole()
{
	return IsTheSameDay(m_tRoleFullInfo.tRoleBaseInfo.dwRoleCreateTime, time(NULL));
}

//发送消息
BOOL Player::SendPacket(NetMsgBody * pMsg)
{
	if (m_pSession == NULL)
	{
		return FALSE;
	}
	return m_pSession->SendPacket(GetGuid(), pMsg->GetType(), pMsg);
}

//发送玩家基本信息
VOID Player::SendRoleBaseInfo()
{ 
	// 装备列表
	// 宝物列表

	static MSG_S2C_ROLE_INFO_NOTIFY msg;
	// 角色基本信息
	memcpy_s(&msg.m_RoleInfo, sizeof(msg.m_RoleInfo), &m_tRoleFullInfo.tRoleBaseInfo,
		sizeof(m_tRoleFullInfo.tRoleBaseInfo));

	//msg.m_RoleInfo.dwFightingPower = GetFightingPower();
	msg.m_dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(GetRoleId());
	const ServerInfo *pServerInfo = g_MapServer.GetServerInfo(GetServerId());
	msg.m_dwOpenServerTime = pServerInfo ? pServerInfo->dwOpenTime : 0;
	msg.m_dwServerId = GetServerId();

	SendPacket(&msg);
}

VOID Player::SendErrorMsg(WORD wErrorCode, WORD wFromMsgId)
{
	static MSG_S2C_GAME_ERROR_NOTIFY msg;
	msg.m_wErrorCode = wErrorCode;
	msg.m_wFromMsgId = wFromMsgId;
	SendPacket(&msg);
}

void Player::PrintPVPData(bool isLogin)
{
	PlayerPVPData &tPVPData = m_tRoleFullInfo.tSecondInfo.tPVPData;
	MAPLOG(GUI_LEVEL, "[%s]玩家PVP数据[RoleId:%d,Score:%d,WeekWinTimes:%d,IsLogin:%d]",
		__FUNCTION__, GetRoleId(), tPVPData.dwScore, tPVPData.wWeekWinTimes, isLogin);
}

BOOL Player::IsNewDay()
{
	//转换上次下线时间
	time_t nLogout = m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime;
	tm tLast;
	localtime_s(&tLast, &nLogout);

	time_t tCurTime = time(NULL);
	tm tCur;
	localtime_s(&tCur, &tCurTime);
	
	return tLast.tm_yday != tCur.tm_yday ? TRUE : FALSE;
}

BOOL Player::IsNewWeek()
{
// 	//转换上次下线时间
//  	time_t nLogout = m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime;
//  	tm tLast;
//  	localtime_s(&tLast, &nLogout);
//  
//  	time_t tCurTime = time(NULL);
//  	tm tCur;
//  	localtime_s(&tCur, &tCurTime);
//  
//  	// 不是同一天,且是周一
//  	if (tLast.tm_yday != tCur.tm_yday && 1 == tCur.tm_wday)
//  		return TRUE;
//  
//  	return FALSE;

	// 上次下线时间
	//WORD wLastWeekOfYear = g_GetWeekOfYear(m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime);
	//WORD wCurWeekOfYear = g_GetWeekOfYear(time(NULL));
	//return wLastWeekOfYear != wCurWeekOfYear ? TRUE : FALSE;

	DWORD dwLastMondayBeginTime = GetLastMondayBeginTime();
	return m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime < dwLastMondayBeginTime ? TRUE : FALSE;
}

BOOL Player::IsNewMonth()
{
	//转换上次下线时间
	time_t nLogout = m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime;
	tm tLast;
	localtime_s(&tLast, &nLogout);

	time_t tCurTime = time(NULL);
	tm tCur;
	localtime_s(&tCur, &tCurTime);

	// 不是同一月份
	if (tLast.tm_mon != tCur.tm_mon)
		return TRUE;

	return FALSE;
}


//处理任务脚本
ERROR_CODE Player::HandleMissionScript(const char* pszScriptName, const char* pszScriptEventName, INT nEventPara)
{
// 	if (!pszScriptName || *pszScriptName == 0)
// 		return ERROR_CODE_MISSION_INVALID_SCRIPT_NAME;
// 
// 	if (!pszScriptEventName || *pszScriptEventName == 0)
// 		return ERROR_CODE_MISSION_INVALID_SCRIPT_EVENT_NAME;

	//取得任务事件Table,可以不存在
	LuaTable eventTable;
	if (!LuaInterface::Instance()->GetGlobal(pszScriptName, eventTable))
		return ERROR_CODE_OK;

	//取得任务事件回调函数,可以不存在
	LuaFunction callback;
	if (!eventTable.Get(pszScriptEventName, callback))
		return ERROR_CODE_OK;

	return ERROR_CODE_OK;
}

// 角色信息更新
void Player::RoleInfoUpdate(bool isSave)
{
	g_MapServer.AddUpdatePlayer(GetRoleId(), GetServerId(), isSave);
}

// 更新成就
void Player::OnUpdateAchievement(WORD wAchievementId, int nValue, bool bAdd)
{
	// todo::暂时这样处理,不晓得会不会出现其他问题...
	if (!nValue)
		return;

	m_pAchievementMgr->OnUpdateAchievement(wAchievementId, nValue, bAdd);
}


// 取VIP特权值
int Player::GetVipValue(BYTE byType)
{
	return m_pVipMgr->GetValue(byType);
}
// 更新日常任务
void Player::OnUpdateDailyMission(WORD wMissionType, int nValue, bool bAdd)
{
	m_pDailyMissionMgr->OnUpdateMission(wMissionType, nValue, bAdd);

	// 世界等级任务
	m_pWorldLevelTaskMgr->UpdateTaskEvent(wMissionType, nValue, bAdd);
}

// 取物品数量
DWORD Player::GetItemNum(DWORD dwItemId)
{
	return m_pItemMgr->GetItemNum(dwItemId);
}

// 删除物品
void Player::RemoveItem(DWORD dwItemId, DWORD dwItemNum, WORD wFromId)
{
	m_pItemMgr->RemoveItem(dwItemId, dwItemNum, TRUE, wFromId);
}

// 删除物品
void Player::RemoveItem(const Item &tItem, WORD wFromId)
{
	m_pItemMgr->RemoveItem(tItem.dwId, tItem.dwNum, TRUE, wFromId);
}

// 战力更新
void Player::UpdateFightingPower()
{
	DWORD dwOldFightingPower = GetProperty(ROLE_PRO_FIGHTING_POWER);

	DWORD dwFightingPower = m_pPropertyManager->GetAllFightingPower();

	SetProperty(ROLE_PRO_FIGHTING_POWER, dwFightingPower, false);

	NotifyProperty(ROLE_PRO_FIGHTING_POWER, dwFightingPower);

	// 如果战力有变动
	if (dwFightingPower != dwOldFightingPower)
	{
		// 更新基础数据管理器的战力
		PlayerBaseDataManager::Instance()->SetFightingPower(GetServerId(), GetRoleId(), dwFightingPower);

		// 成就事件
		OnUpdateAchievement(8, dwFightingPower, false);

		// 7天活动事件(战力突破)
		OnUpdateSevenDayTarget(17, dwFightingPower, false);

		// 更新排行榜
		CRankManager::Instance()->AddRoleRankData(RANK_BATTLE_VALUE, this, dwFightingPower, 0);

		// 公告
		const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(135);
		if (pServerCfg)
		{
			ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(GetServerId());
			DWORD dwTargetFightingPower = pServerCfg->GetListValue(tGlobalData.dwFightingPowerRecordIdx);
			// 到达目标战力
			if (dwFightingPower >= dwTargetFightingPower && tGlobalData.dwFightingPowerRecordIdx < pServerCfg->vValueList.size())
			{
				tGlobalData.dwFightingPowerRecordIdx++;
				g_MapServer.SaveGolbalData(GetServerId(), GOLBAL_FIGHTING_POWER_RECORD_IDX, tGlobalData.dwFightingPowerRecordIdx);

				CSystemNotice sys_notice(15, GetServerId());
				sys_notice.AddParam(GetRoleName());
				sys_notice.AddParam(dwTargetFightingPower);
				sys_notice.SendMsg();
			}
		}
	}
}

// 取工会技能
const SkillData* Player::GetGuildSkill(WORD wSkillId)
{
	for (WORD i = 0; i < m_tRoleFullInfo.tGuildSkillData.wSkillNum; ++i)
	{
		if (m_tRoleFullInfo.tGuildSkillData.arSkillList[i].wSkillId == wSkillId)
		{
			return &m_tRoleFullInfo.tGuildSkillData.arSkillList[i];
		}
	}
	return NULL;
}

// 添加工会技能
void Player::AddGuildSkill(WORD wSkillId, WORD wSkillLevel)
{
	PlayerGuildSkillData &tGuilSkill = m_tRoleFullInfo.tGuildSkillData;
	for (WORD i = 0; i < tGuilSkill.wSkillNum; ++i)
	{
		if (tGuilSkill.arSkillList[i].wSkillId == wSkillId)
		{
			tGuilSkill.arSkillList[i].wLevel = wSkillLevel;
			return;
		}
	}

	tGuilSkill.arSkillList[tGuilSkill.wSkillNum].wSkillId = wSkillId;
	tGuilSkill.arSkillList[tGuilSkill.wSkillNum].wLevel = wSkillLevel;
	++tGuilSkill.wSkillNum;
}

// 填充工会技能列表
void Player::FillGuildSkillList(vector<SkillData> &vSkillList)
{
	vSkillList.clear();
	for (WORD i = 0; i < m_tRoleFullInfo.tGuildSkillData.wSkillNum; ++i)
	{
		vSkillList.push_back(m_tRoleFullInfo.tGuildSkillData.arSkillList[i]);
	}
}

// 查询目标详细信息回调
void Player::SelectTargetDetailsCallBack(BYTE byRequestType, const RoleFullInfo *pTargetFullInfo)
{	
	// todo::日后可能会调整
	
	const PlayerBaseData *pBaseData = g_GetPlayerBaseData(GetServerId(), pTargetFullInfo->tRoleBaseInfo.dwRoleId);
	if (!pBaseData)
	{
		SendErrorMsg(ERROR_CHAT_TARGET_INVALID);
		return;
	}

	MSG_S2C_ROLE_DETAILS_RESP msg;
	memset(&msg.m_Details, 0, sizeof(msg.m_Details));
	msg.m_byType = byRequestType;
	memcpy_s(&msg.m_Details, sizeof(PlayerCommonData), pBaseData, sizeof(PlayerCommonData));

	for (BYTE i = 0; i < MAX_BATTLE_HERO_NUM; ++i)
	{
		const Formation *pFormation = &pTargetFullInfo->tHeroInfo.arFormationList[i];
		const HeroInfo *pHero = NULL;
		if (pFormation->dwHeroUID)
		{
			for (WORD k = 0; k < pTargetFullInfo->tHeroInfo.wHeroNum; ++k)
			{
				if (pTargetFullInfo->tHeroInfo.arHeroList[k].dwUID == pFormation->dwHeroUID)
				{
					pHero = &pTargetFullInfo->tHeroInfo.arHeroList[k];
					break;
				}
			}
		}
		if (!pHero)
			continue;

		ClientFormation *pClientFormation = &msg.m_Details.arFormationList[msg.m_Details.byFormationNum++];

		// 填充英雄信息
		pClientFormation->bySlot = i;
		pClientFormation->wHeroInfoId = pHero->dwIntoId;
		pClientFormation->byAwakenStar = pHero->wAwakenStar;
		pClientFormation->wBreakLevel = pHero->wBreakLevel;
		pClientFormation->byQaulity = pHero->wQuality;

		// 填充装备信息
		pClientFormation->byEquipmentNum = 0;
		for (BYTE j = 0; j < EQUIPMENT_TYPE_MAX; ++j)
		{
			DWORD dwEquipmentId = pFormation->arEquipmentList[j];
			if (dwEquipmentId)
			{
				const Equipment *pEquipment = NULL;
				for (WORD k = 0; k < pTargetFullInfo->tEquipmentInfo.wEquipmentNum; ++k)
				{
					if (pTargetFullInfo->tEquipmentInfo.sEquipmentList[k].dwId == dwEquipmentId)
					{
						pEquipment = &pTargetFullInfo->tEquipmentInfo.sEquipmentList[k];
						break;
					}
				}

				if (pEquipment)
				{
					FormationEquipment *pTmp = &pClientFormation->arEquipmentList[pClientFormation->byEquipmentNum++];
					pTmp->wInfoId = pEquipment->dwInfoId;
					pTmp->wStrengthenLevel = pEquipment->wEnhanceLevel;
					pTmp->byRefineLevel = pEquipment->wRefineLevel;
				}
			}
		}

		// 填充宝物信息
		pClientFormation->byTreasureNum = 0;
		for (BYTE j = 0; j < TREASURE_TYPE_MAX; ++j)
		{
			DWORD dwTreasureId = pFormation->arTreasureList[j];
			if (dwTreasureId)
			{
				const Treasure *pTreasure = NULL;
				for (WORD k = 0; k < pTargetFullInfo->tTreasureInfo.wTreasureNum; ++k)
				{
					if (pTargetFullInfo->tTreasureInfo.sTreasureList[k].dwId == dwTreasureId)
					{
						pTreasure = &pTargetFullInfo->tTreasureInfo.sTreasureList[k];
						break;
					}
				}

				if (pTreasure)
				{
					FormationTreasure *pTmp = &pClientFormation->arTreasureList[pClientFormation->byTreasureNum++];
					pTmp->wInfoId = pTreasure->dwInfoId;
					pTmp->wStrengthenLevel = pTreasure->wEnhanceLevel;
					pTmp->byRefineLevel = pTreasure->wRefineLevel;
				}
			}
		}
	}

	msg.m_Details.byIsFriend = m_pFriendMgr->IsFriend(pTargetFullInfo->tRoleBaseInfo.dwRoleId);
	msg.m_Details.byIsBlack = m_pFriendMgr->IsBlack(pTargetFullInfo->tRoleBaseInfo.dwRoleId);

	SendPacket(&msg);
}

// 检测货币消耗
bool Player::CheckMoneyCost(const vector<CostData> &vCostList, WORD wNum)
{
	for (size_t i = 0; i < vCostList.size(); ++i)
	{
		const CostData *pCost = &vCostList.at(i);
		if (GetProperty(pCost->wType) < wNum * pCost->nValue)
			return false;
	}
	return true;
}

// 检测物品消耗
bool Player::CheckItemCost(const vector<Item> &vCostItemList, WORD wNum)
{
	for (size_t i = 0; i < vCostItemList.size(); ++i)
	{
		const Item &tCostItem = vCostItemList[i];
		if (GetItemNum(tCostItem.dwId) < tCostItem.dwNum * wNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]玩家物品消耗不足[CostItemId:%d,CostItemNum:%d]!",
				__FUNCTION__, tCostItem.dwId, tCostItem.dwNum * wNum);
			return false;
		}
	}
	return true;
}

// 处理货币消耗
void Player::DoMoneyCost(const vector<CostData> &vCostList, WORD wNum, WORD wFromId)
{
	for (size_t i = 0; i < vCostList.size(); ++i)
	{
		ModifyProperty(vCostList[i].wType, -vCostList[i].nValue * wNum, TRUE, wFromId);
	}
}

// 取当前使用的称号Id
int Player::GetCurUseTitleId()
{
	return m_tRoleFullInfo.tSecondInfo.tTitleInfo.nCurUseTitleId;
}

// 检测一次性标志
BOOL Player::CheckOnceFlag(int nFlagIdx)
{
	return ISBIT(m_tRoleFullInfo.tRoleBaseInfo.nOnceFlagList, nFlagIdx);
}

// 设置一次性标志
VOID Player::SetOnceFlag(int nFlagIdx)
{
	SETBIT(m_tRoleFullInfo.tRoleBaseInfo.nOnceFlagList, nFlagIdx);
}

// 清除一次性标志
VOID Player::ClearOnceFlag(int nFlagIdx)
{
	CLRBIT2(m_tRoleFullInfo.tRoleBaseInfo.nOnceFlagList, nFlagIdx);
}

// 更新7天目标数据
void Player::OnUpdateSevenDayTarget(WORD wLogicType)
{
	// 检测是否活动期
// 	if (!m_pActivityManager->IsActivityOpen(52))
// 		return;

	m_pSevenDayManager->OnUpdateTarget(wLogicType);
}

// 更新7天目标数据
void Player::OnUpdateSevenDayTarget(WORD wLogicType, DWORD dwX, bool bAdd)
{
	// 检测是否活动期
// 	if (!m_pActivityManager->IsActivityOpen(52))
// 		return;

	m_pSevenDayManager->OnUpdateTarget(wLogicType, dwX, bAdd);
}

// 节日活跃目标事件
void Player::OnFestivalActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd)
{
	m_pActivityManager->GetFestivalActivity().OnActiveTarget(wActivityType, wLogicType, dwX, bAdd);
}

// 存储物品丢失记录
void Player::SaveItemLoseRecord(DWORD dwItemId, DWORD dwItemNum)
{
	// 插入数据库
	ItemLoseRecord_Save* pQuery = ItemLoseRecord_Save::ALLOC();
	pQuery->SetIndex(DB_ROLE_ITEM_LOSE_RECORD_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	ItemLoseRecord tItemLoseRecord;
	tItemLoseRecord.dwRoleId = GetRoleId();
	tItemLoseRecord.dwItemId = dwItemId;
	tItemLoseRecord.dwItemNum = dwItemNum;
	pQuery->SetRecord(tItemLoseRecord);
	g_MapServer.MapDBQuery(pQuery);
}

// 锁住帐号
void Player::LockAccount()
{
	AccountLock_Update* pQuery = AccountLock_Update::ALLOC();
	pQuery->SetIndex(DB_ROLE_ACCOUNT_LOCK_UPDATE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	pQuery->SetAccount(GetAccID());
	pQuery->SetPlatform(m_pAccountInfo->GetChannelUID());
	g_MapServer.MapDBQuery(pQuery);
}

// 更新世界任务事件
void Player::OnUpdateWorldTaskEvent(WORD wTaskType, int nContext)
{
	m_pWorldLevelTaskMgr->UpdateTaskEvent(wTaskType, nContext);
}

// 检测并重置玩家圣诞节数据
void Player::CheckAndResetChristmasData()
{
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(GetServerId());
	PlayerChristmasData &tChristmasData = m_tRoleFullInfo.tActivityInfo.tChristmasData;
	if (tChristmasData.dwLastResetDataTime < tGlobalData.dwChristmasLastResetTime)
	{
		//PlayerChristmasData &tChristmasData = m_tRoleFullInfo.tActivityInfo.tChristmasData;
		memset(&tChristmasData, 0, sizeof(tChristmasData));
		tChristmasData.dwLastResetDataTime = time(NULL);

		ChristmasShopData &tChristmasShop = m_tRoleFullInfo.tShopInfo.tChristmasShop;
		memset(&tChristmasShop, 0, sizeof(tChristmasShop));
	}
}