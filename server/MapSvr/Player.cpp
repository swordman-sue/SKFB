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

//��ʼ�����
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

	//���DB�洢�ļ��ʱ��
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

	//�����û���ĺ���
	return Character::Init();
}

//�ͷ����
VOID Player::Release()
{
// 	if (m_pOrderMgr)
// 		m_pOrderMgr->Release();


	//�����û���ĺ���
	Character::Release();
}

//�������
VOID Player::Update(DWORD dwTick)
{
	//���û���汾��
	Character::Update(dwTick);

	//DB�洢��ʱ������
	if (m_cSaveDBTimers.IsExpired(FALSE))
	{
		//�����洢DB�¼�
		OnSave(SAVE_DB_TYPE_TIMING);
		//����DB�洢��ʱ��
		m_cSaveDBTimers.SetTimer(SAVE_DB_INTERVAL_TIME);
	}

	//����������ʱ��
	//UpdateChatInterval(dwTick);
}

//�������
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
	// ���ȼ���������
	WORD wMaxLevel = g_pCfgMgr->m_RoleLevelList.size();
	wLevel = min(wMaxLevel, wLevel);

	WORD wOldLevel = m_tRoleFullInfo.tRoleBaseInfo.wLevel;

	m_tRoleFullInfo.tRoleBaseInfo.wLevel = wLevel;

	OnUpdateAchievement(2, wLevel, false);

	// �����������⴦��
// 	OnUpdateAchievement(24, wLevel, false);
// 	OnUpdateAchievement(25, wLevel, false);
// 	OnUpdateAchievement(26, wLevel, false);
// 	OnUpdateAchievement(27, wLevel, false);

	// 7��Ŀ��(�ȼ���)
	OnUpdateSevenDayTarget(2, wLevel, false);

	// 7��Ŀ��(�ȼ�ͻ��)
	OnUpdateSevenDayTarget(16, wLevel, false);

	// �ȼ�����
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

	// todo::�����������, ֧��ԭ���ӿڲ���
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

// ����������
void Player::FillBaseData(PlayerBaseData *pBaseData)
{
	// ��������
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

	// �����������
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

			// ����
			m_pPropertyManager->FillSlotProList(i, tToBattleHero.arPorList, tToBattleHero.byProNum);
			pBaseData->vToBattleHeroList.push_back(tToBattleHero);
		}
	}
}

// �������Ӣ������
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

			// ����
			m_pPropertyManager->FillSlotProList(i, tToBattleHero.arPorList, tToBattleHero.byProNum);
			vHeroList.push_back(tToBattleHero);

			// ף����������
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
		// �Ƿ��о����츳���ܶ��⽱��
		if (m_bIsHaveSpriteTalentSkillReward && nValue > 0)
		{
			BYTE byFBType = m_pFubenMgr->GetBattleType();
			map<int, int> mCurrencyInfoList;
			m_pSpiritMgr->GetExtraRewardCurrencyInfo(byFBType, mCurrencyInfoList);
			int nExtraRewardValue = m_pSpiritMgr->GetExtraRewardCurrencyValue(mCurrencyInfoList, wPropertyType, nValue);
			nValue += nExtraRewardValue;
		}

		// ��ɫ����
		if (ROLE_PRO_EXP == wPropertyType)
		{
			AddExp(nValue);
		}
		// vip����
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

		// ��¼������־
		if (bSaveLog)
			SaveCurrencyLog(wPropertyType, nValue, wFromId);

		// ����� 
		if (nValue < 0 && ROLE_PRO_DIAMOND == wPropertyType)
		{
			int nCostDiamond = abs(nValue);

			// ���ջ
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
 	nValue = max(nValue, 0); // ��ֹ���ָ��������

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
			// ���������ָ�ʱ��
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
			// ���¾����ָ�ʱ��
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
			// ����ͨ����ָ�ʱ��
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
		MAPLOG(ERROR_LEVEL, "[%s]�޷�ʶ�����������[PropertyType:%d]", __FUNCTION__, wPropertyType);
		return;
 	}
 
	if (bNotice && nOldValue != nValue && wPropertyType < ROLE_PRO_NOT_NOTIFY_BEGIN)
 	{
		NotifyProperty(wPropertyType, nValue);
 	}

	// �����б䶯
	if (nOldValue != nValue)
		RoleInfoUpdate();
}

INT32 Player::GetProperty(WORD wPropertyType)
{
 	switch(wPropertyType)
 	{
	case ROLE_PRO_LEVEL:					return m_tRoleFullInfo.tRoleBaseInfo.wLevel;			// �ȼ�
	case ROLE_PRO_EXP:						return m_tRoleFullInfo.tRoleBaseInfo.dwExp;				// ����	
	case ROLE_PRO_GOLD:						return m_tRoleFullInfo.tRoleBaseInfo.dwGold;			// ͭǮ	
	case ROLE_PRO_STAMINA:					return m_tRoleFullInfo.tRoleBaseInfo.dwStamina;			// ����	
	case ROLE_PRO_DIAMOND:					return m_tRoleFullInfo.tRoleBaseInfo.dwDiamond;			// שʯ
	case ROLE_PRO_HERO_SOUL:				return m_tRoleFullInfo.tRoleBaseInfo.nHeroSoul;			// �ۻ�
	case ROLE_PRO_HEAD_IMAGE:				return m_tRoleFullInfo.tRoleBaseInfo.dwHeadImage;		// ͷ��
	case ROLE_PRO_ENERGY:					return m_tRoleFullInfo.tRoleBaseInfo.dwEnergy;			// ����
	case ROLE_PRO_LIKE_TIMES:				return m_tRoleFullInfo.tRoleBaseInfo.wClickLikeTimes;	// ���޴���
	case ROLE_PRO_VIP_LEVEL:				return m_tRoleFullInfo.tRoleBaseInfo.sVipData.wLevel;	// vip�ȼ�
	case ROLE_PRO_REBEL_LEVEL:				return m_tRoleFullInfo.tRebelInfo.wRebelLevel;			// �Ѿ��ȼ�
	case ROLE_PRO_REBEL_VALUE:				return m_tRoleFullInfo.tRoleBaseInfo.dwRebelValue;		// ս��(�Ѿ�ֵ)
	case ROLE_PRO_WANTED:					return m_tRoleFullInfo.tRoleBaseInfo.nWanted;			// ͨ����(������)
	case ROLE_PRO_TOWER_PRESTIGE:			return m_tRoleFullInfo.tRoleBaseInfo.nTowerPrestige;	// ��������(����)
	case ROLE_PRO_ARENA_PRESTIGE:			return m_tRoleFullInfo.tRoleBaseInfo.nArenaPrestige;	// ����������
	case ROLE_PRO_DAILY_INIT_LEVEL:			return m_tRoleFullInfo.tRoleBaseInfo.wDailyInitLevel;	// ÿ�ճ�ʼ�ȼ�
	case ROLE_PRO_GUILD_CONSTIBUTION:		return m_tRoleFullInfo.tRoleBaseInfo.dwCurContribution;	// ��ǰ���ṱ��ֵ
	case ROLE_PRO_VIP_EXP:					return m_tRoleFullInfo.tRoleBaseInfo.sVipData.dwExp;	// ÿ�ճ�ʼ�ȼ�
	case ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES: return m_tRoleFullInfo.tRoleBaseInfo.wRiotSuppressTimes;// ��ر�����ѹ����
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
 		MAPLOG(GUI_LEVEL,"[%s]�޷�ʶ�����������[PropertyType:%d]",__FUNCTION__,wPropertyType);
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
	// ��Ʒ
	if (byType == OBJECT_ITEM || byType == OBJECT_AWAKENING_ITEM)
	{
		m_pItemMgr->AddItem(dwObjectId, dwOjbectNum, bNotice, wFromId);
	}
	// Ӣ��
	else if (byType == OBJECT_HERO)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pHeroMgr->AddHero(dwObjectId, bNotice, wFromId);
		}	
	}
	// ����
	else if (byType == OBJECT_TREASURE)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pTreasureMgr->AddTreasure(dwObjectId, bNotice, wFromId);
		}
	}
	// װ��
	else if (byType == OBJECT_EQUIPMENT)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pEquipmentMgr->AddEquipent(dwObjectId, bNotice, wFromId);
		}	
	}
	// ����
	else if (byType == OBJECT_SPIRIT)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pSpiritMgr->AddSpirit(dwObjectId, wFromId);
		}
	}
	// ��ʯ
	else if (byType == OBJECT_GEM)
	{
		m_pGemManager->AddGem(dwObjectId, dwOjbectNum, bNotice, wFromId);
	}
	// ����
	else if (byType == OBJECT_CARD)
	{
		for (DWORD i = 0; i < dwOjbectNum; ++i)
		{
			m_pZhanBuManager->AddCard(dwObjectId, bNotice, wFromId);
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]δ����Ķ�������[type:%d]", __FUNCTION__, byType);
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

// д������־
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

// ��Ӿ���
void Player::AddExp(DWORD dwExp)
{
	// todo::����BUG
	//MAPLOG(ERROR_LEVEL, "��Ӿ���[RoleId:%d,Exp:%d]", GetRoleId(), dwExp);

	m_tRoleFullInfo.tRoleBaseInfo.dwExp += dwExp;

	bool bLevelUp = false;
	const RoleLevel_Config *pRoleLvCfg;
	while (true)
	{
		// �Ҳ��������ļ����˳�
		pRoleLvCfg = CConfigManager::Instance()->GetRoleLevel(GetLevel());
		if (!pRoleLvCfg)
		{
			MAPLOG(ERROR_LEVEL, "�Ҳ�����ɫ�ȼ������ļ�[Level:%d]",GetLevel());
			break;
		}

		// ���鲻�㹻����,���˳�
		if (!pRoleLvCfg->dwExp || m_tRoleFullInfo.tRoleBaseInfo.dwExp < pRoleLvCfg->dwExp)
			break;

		m_tRoleFullInfo.tRoleBaseInfo.dwExp -= pRoleLvCfg->dwExp;
		ModifyProperty(ROLE_PRO_LEVEL, 1);
		ModifyProperty(ROLE_PRO_STAMINA, pRoleLvCfg->dwStamina);
		ModifyProperty(ROLE_PRO_ENERGY, pRoleLvCfg->dwEnergy);
		bLevelUp = true;

		// ������������
		const RoleLevel_Config *pLvUpRewardCfg = CConfigManager::Instance()->GetRoleLevel(GetLevel());
		if (pLvUpRewardCfg && pLvUpRewardCfg->vRewardItemList.size())
			AddObjectList(pLvUpRewardCfg->vRewardItemList, TRUE, FROM_ROLE_LEVEL_UP_REWARD);

		RoleInfoUpdate();

		UpdateFightingPower();

		//m_pHangUpDropMgr->OnLevelUp();

		// ��¼�ȼ�����
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

	// �������
	if (bLevelUp)
	{
		MSG_S2C_ROLE_LEVEL_UP_NOTIFY msg;
		msg.m_wLevel = GetLevel();
		SendPacket(&msg);

		CRankManager::Instance()->AddRoleRankData(RANK_LEVEL, this, GetLevel(), 0);
		CRankManager::Instance()->AddRoleRankData(RANK_BATTLE_VALUE, this, GetFightingPower(), 0);
	}
}

//������Ҽ����¼�
VOID Player::OnLoad(RoleFullInfo& sRoleInfo)
{	
	SetRoleFullInfo(sRoleInfo);
}

//������Ҵ洢�¼�
VOID Player::OnSave(SaveDBType enSaveDBType)
{
	if(!m_pAccountInfo || m_pAccountInfo->GetUserState() != USER_AT_GAME_STATE)
		return;

	RoleBaseInfo &tBaseInfo = m_tRoleFullInfo.tRoleBaseInfo;

	// ������һ�������

	// �����������ʱ��(��ʵ����һ�����������)
	tBaseInfo.dwLastLogoutTime = time(NULL);

	// ��������ʱ��
	if (SAVE_DB_TYPE_LOGOUT == enSaveDBType)
	{
		DWORD dwLoginTime = tBaseInfo.dwLastLoginTime;
		DWORD dwLogouTime = tBaseInfo.dwLastLogoutTime;
		// ��ɫ����ʱ��
		if (dwLogouTime > dwLoginTime){
			tBaseInfo.dwOnlineTime += (dwLogouTime - dwLoginTime);
		}

		// ���߽�������ʱ�����
		DWORD dwPassTime = dwLogouTime - m_tRoleFullInfo.tActivityInfo.tOnlineRewardData.dwStartTime;
		m_tRoleFullInfo.tActivityInfo.tOnlineRewardData.dwOnlineTime += dwPassTime;
	}

	// �������(todo::����д����һ���ķ���)
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

	//��������ݿⷢ������
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
		MAPLOG(GUI_LEVEL, "�������[RoleId:%d,CharRole=%s,Guid:%d,LastLogoutTime:%d]", 
			GetRoleId(), GetUTF8RoleName(), GetGuid(), GetLastLogoutTime());

		// todo::Ϊ�������BUG�����־
		PrintPVPData(false);
	}
}

//������ҵ�¼�¼�
VOID Player::OnLogin()
{
	// todo::�˴����߼�˳����Ҫ��������һ��

	bool bIsNewRole = IsNewRole();
	bool bIsNewDay = IsNewDay();
	bool bIsNewWeek = IsNewWeek();
	bool bIsNewMonth = IsNewMonth();

	DWORD dwLastLogoutTime = GetLastLogoutTime();

	// ����Ƿ��½���ɫ
	if (bIsNewRole)
		OnNewRole();

	//��ʱ���н�ɫ���������ɫ���б��ݶ���
	PlayerManager::Instance()->AddPlayerName(this);

	//������ҵ�½ʱ��
	DWORD dwCurTime = time(NULL);
	m_tRoleFullInfo.tRoleBaseInfo.dwLastLoginTime = dwCurTime;

	// ��������ת������
	DoTempData(false);

	// �µ�һ��
	if (bIsNewDay)
		OnNewDay();

	// �µ�һ��
	if (bIsNewWeek)
		OnNewWeek();

	// �µ��·�
	if (bIsNewMonth)
		OnNewMonth();

	PlayerBaseDataManager::Instance()->OnPlayerLogin(this);

	// todo::���ݾɰ汾����
	m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage = max(m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage, 1);
	//m_tRoleFullInfo.tRoleBaseInfo.wNobilityLevel = max(m_tRoleFullInfo.tRoleBaseInfo.wNobilityLevel, 1);

	// ����ս��
	//UpdateFightingPower();


	//������һ�����Ϣ
	SendRoleBaseInfo();

	//����Ӣ���б�
	m_pHeroMgr->SendHeroBaseList();

	// ���������б�
	m_pHeroMgr->SendFormationList();

	// ����ս��
	UpdateFightingPower();

	DWORD dwOpenDay = g_MapServer.GetAlreadyOpenServerDay(GetServerId());

	OnUpdateSevenDayTarget(1, dwOpenDay, false);

	// ͬ��������ʱ��
// 	{
// 		MSG_S2C_SERVER_TIME_RESP msg;
// 		msg.m_dwServerTime = (DWORD)time(NULL);
// 		SendPacket(&msg);
// 	}
	// ���ݾ����ݼ��ݴ���
// 	if (!m_pGemManager->IsInitDefaultOpenHole())
// 		m_pGemManager->InitDefaultOpenHole();

	m_pActivityManager->GetFestivalActivity().OnNetFestivalLogin();
	m_pActivityManager->GetSevenDayLogin().OnLogin();
	m_pActivityManager->GetFacebookActivity().OnLogin();

	//�������Ϣ���ͻ���֪ͨ�������
	//MSG_M2C_LOAD_CHAR_FINISH_CMD msg;
	//SendPacket(&msg);

	// ͬ�����ݵ�����
	MSG_M2G_CLIENT_INFO client_info_msg;
	client_info_msg.m_wServerId = GetServerId();
	client_info_msg.m_dwRoleId = GetRoleId();
	SendPacket(&client_info_msg);

	MSG_S2C_LOGIN_ENTER_GAME_NOTIFY enter_game_msg;
	SendPacket(&enter_game_msg);

	MAPLOG(GUI_LEVEL, "��ҵ�¼�ɹ�,���ͻ�������[RoleId:%d,Guid:%d,LastLogoutTime:%d]", GetRoleId(), GetGuid(), dwLastLogoutTime);

	// todo::Ϊ�������BUG�����־
	PrintPVPData(true);

	// ����㲥
	WORD wRank = CArenaManager::Instance()->GetPlayerRank(GetServerId(), GetRoleId());
	if (wRank && wRank <= 1)
	{
		CSystemNotice sys_notice(5, GetServerId());
		sys_notice.AddParam(GetRoleName());
		sys_notice.SendMsg();
	}

	m_pFubenMgr->GetEliteFB().Login();

	// �򲹶��߼�
	m_pItemMgr->DeleteTimeOutItem();
	
	// �����ݼ����߼�����
	m_pGemManager->InitDefaultOpenHole();

	// ��¼���߽�����ʼ��ʱʱ���
	m_tRoleFullInfo.tActivityInfo.tOnlineRewardData.dwStartTime = time(NULL);

	// ���ʥ�������Ƿ���Ҫ����
	CheckAndResetChristmasData();


	// todo::BUG���� 
//  	FestivalActivity &tOpenSvrAccumulateRecharge = m_tRoleFullInfo.tActivityInfo.tFestivalActivityInfo.tOpenSvrAccumulateRecharge;
//  	if (m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount && !tOpenSvrAccumulateRecharge.wTargetNum)
//  	{
//  		// �����۳�
//  		m_pActivityManager->GetFestivalActivity().DoAccumulateRechargeEvent(OPEN_SVR_ACCUMULATE_RECHARGE, 
//  			m_tRoleFullInfo.tRoleBaseInfo.dwTotalRechargeAmount);
//  	}

	// �������а�(��Ҫ�ǿ��ǺϷ������,��Ϊ�Ϸ����а�Ჹ���,����һ���ȼ��Ŵ���)
	if (GetLevel() >= 20 && !CRankManager::Instance()->GetRoleRankDataEx(GetServerId(), RANK_LEVEL, GetRoleId()))
	{
		CRankManager::Instance()->AddRoleRankData(RANK_LEVEL, this, GetLevel(), 0);
		CRankManager::Instance()->AddRoleRankData(RANK_BATTLE_VALUE, this, GetFightingPower(), 0);
		CRankManager::Instance()->AddRoleRankData(RANK_MAIN_FB_STAR, this, m_pFubenMgr->GetMainFB().GetTotalStar());
		CRankManager::Instance()->AddRoleRankData(RANK_TOWER, this, m_pFubenMgr->GetTower().GetMaxStarRecord());
		CRankManager::Instance()->AddRoleRankData(RANK_ENDLESS_KILL, this, m_pEndlessLandManager->GetContinueKillNumRecord());
		CRankManager::Instance()->AddRoleRankData(RANK_ZHANBU, this, m_pZhanBuManager->GetHistoryCollectionValue());
	}

	// ���ŷ���ֵ�����ȼ�(Ŀǰֻ��һ���Żᷢ��)
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

			MAPLOG(ERROR_LEVEL, "����ҷ��ŷ�ⷵ����ʯ[RoleId:%d,Diamond:%d]", GetRoleId(), pRebateCfg->nDiamond);

			m_tRoleFullInfo.tRoleBaseInfo.wIsAlreadyTakeClosedTestRebate = true;
		}
	}

	// ͳ����ҵ�½
	CGameLogic::Instance()->OnLoginStatistics(this);

	//CGameLogic::Instance()->OnGameStatistics_Login(this, bIsNewDay);

	// ���µ�¼����
	m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes++;
	// ��Ӧ�ó����������
	if (m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes >= 450000)
	{
		m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes = 1;
		MAPLOG(ERROR_LEVEL, "[RoleId:%d]��ҵ�¼���������쳣,��¼��������450000��",GetRoleId());
	}
}

//������ҵǳ��¼�
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

//������������¼�
VOID Player::OnLevelUp()
{
	// ֪ͨ�ͻ��˱�������
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
	tVipGiftData.nVipLevelRewardRecord = tVipGiftData.wInitVipLevel - 1; // ÿ�տ�����ȡ��ǰ�ȼ���

	m_tRoleFullInfo.tSecondInfo.tLoginRewardData.wLoginDays++;

	m_tRoleFullInfo.tSecondInfo.tKingMatchData.wTodayWinTimes = 0;

	// ��Ӹ���ÿ����ս����
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

	// ���߸�������
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

	// Ӣ�۸�������
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

	// �ճ���������
	{
		DailyFBData &tDailyFBData = m_tRoleFullInfo.tFubenInfo.tDailyFubenData;
		for (WORD i = 0; i < tDailyFBData.wChapterNum; ++i)
		{
			tDailyFBData.arChapterList[i].byBattleTimes = 0;
		}
	}

	// ������������
	{
		TowerFBData &tTowerFBData = m_tRoleFullInfo.tFubenInfo.tTowerFubenData;
		tTowerFBData.byTodayResetTimes = 0;
		tTowerFBData.byEliteMapBattleTimes = 0;
		tTowerFBData.byBuyEliteMapBattleTimes = 0;
	}

	// ��Ӣ����
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

	// �Ѿ�����
	PlayerRebelData &tRebelData = m_tRoleFullInfo.tRebelInfo;
	tRebelData.wAwardRecordNum = 0;
	tRebelData.dwTodayDamage = 0;
	tRebelData.dwTodayExploit = 0;

	// ����� todo::Ӧ���Ƶ����������ȥ����
	PlayerActivityData &tActivityData = m_tRoleFullInfo.tActivityInfo;
	tActivityData.tYCSData.byTodayGetTimes = 0;
	tActivityData.tTQTData.wTakeRecordNum = 0;
	tActivityData.tSuperDiscountData.wStatus = 0;
	tActivityData.tOnlineRewardData.dwOnlineTime = 0;
	tActivityData.tNewSuperDiscountData.wStatus = 0;
	tActivityData.tUnlockActivityData.wWatchingVideosTimes = 0;
	tActivityData.tPlayerCampRecruitData.wTodayAlreadyRecruitTimes = 0;

	// Ǳ��(todo::�ѵ���Ϊ�����)
// 	PlayerHeroInfo &tHeroInfo = m_tRoleFullInfo.tHeroInfo;
// 	for (WORD i = 0; i < tHeroInfo.wHeroNum; ++i)
// 	{
// 		tHeroInfo.arHeroList[i].dwDestinyExp = 0;
// 	}

	// ������ս��������
	PlayerFriendData &tFriendInfo = m_tRoleFullInfo.tFriendInfo;
	for (WORD i = 0; i < tFriendInfo.wFriendNum; ++i)
	{
		tFriendInfo.arFriendList[i].wHelpBattleTimes = 0;
	}

	// ���µ�¼�¼�,�������������Ҳ���������µ�¼
	m_pActivityManager->GetFestivalActivity().OnNetFestivalLogin();

	// ɾ��������Ʒ
	m_pItemMgr->DeleteTimeOutItem();

	RoleInfoUpdate();

	// todo:::
	if (g_MapServer.GetServerBelongVersion() == SERVER_BELONG_VERSION_996_BT)
		m_tRoleFullInfo.tHeroInfo.wHighSummonTimes = 0;

	// ͳ����ҵ�½
	//CGameLogic::Instance()->OnLoginStatistics(this);

	CGameLogic::Instance()->OnGameStatistics_Login(this, true);

	MAPLOG(SPECAIL_LEVEL, "���ÿ���¼�����[RoleId:%d]", GetRoleId());
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

	MAPLOG(SPECAIL_LEVEL, "[%s]���ÿ���¼�����[RoleId:%d]", __FUNCTION__, GetRoleId());
}

// �µ�һ��(ÿ����1��0��)
void Player::OnNewMonth()
{
	m_pShopMgr->OnNewMonth();

	MAPLOG(SPECAIL_LEVEL, "[%s]���ÿ���¼�����[RoleId:%d]", __FUNCTION__, GetRoleId());
}

// ������ʱ����
VOID Player::DoTempData(bool isSave)
{
	if (m_tRoleFullInfo.tTempDatas.wTempDataNum)
	{
		for (WORD i = 0; i < m_tRoleFullInfo.tTempDatas.wTempDataNum; ++i)
		{
			RoleTempData &tTempData = m_tRoleFullInfo.tTempDatas.arTempDataList[i];
			// �ƺ�
			if (ROLE_TEMP_DATA_TITLE == tTempData.wType)
			{
				m_pTitleManager->AddTitle(tTempData.arParamList[1], tTempData.arParamList[0], false);
			}
			// ��������
			else if (ROLE_TEMP_FORBID_TO_TALK_SET == tTempData.wType)
			{
				// ����
				if (tTempData.arParamList[1])
				{
					m_tRoleFullInfo.tRoleBaseInfo.dwForbidToTalkOverTime = tTempData.arParamList[0];
				}
				// ���
				else
				{
					m_tRoleFullInfo.tRoleBaseInfo.dwForbidToTalkOverTime = 0;
				}
			}
			// ��ֵ����
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
				
				// ��ֵ�ɹ�,���һ�û�����
				m_pRechargeManager->OnNetRechargeCallback(callbackData);

				MAPLOG(GUI_LEVEL, "������ֵ����[role_id:%d,amount:%d,channel:%d,order_id:%s]",
					GetRoleId(), callbackData.nAmount, callbackData.dwChannel, callbackData.szSDKOrderId);
			}
			// ��������
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

			// ������������

			// �ʼ�

			// ����
		}

		m_tRoleFullInfo.tTempDatas.wTempDataNum = 0;

		RoleInfoUpdate(isSave);
	}
}

BOOL Player::IsNewRole()
{
	return m_tRoleFullInfo.tRoleBaseInfo.dwLastLoginTime ? FALSE : TRUE;
}

// �½���ɫ
VOID Player::OnNewRole()
{
	RoleBaseInfo &tBaseInfo = m_tRoleFullInfo.tRoleBaseInfo;

	DWORD dwCurTime = DWORD(time(NULL));

	// �����
	m_tRoleFullInfo.tActivityInfo.tVipGiftData.nVipLevelRewardRecord = -1;

	// �Ѿ����ݳ�ʼ��
	m_tRoleFullInfo.tRebelInfo.wRebelLevel = 1;
	//m_tRoleFullInfo.tRebelInfo.wInitRoleLevel = GetLevel();

	// �������ݳ�ʼ��
	m_tRoleFullInfo.tSpiritInfo.tHeartOfWorld.wStage = 1;

	// ÿ�յȼ���ʼ��
	tBaseInfo.wDailyInitLevel = GetLevel();

	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wCurWave = 1;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wTodayBattleTimes = 0;
	m_tRoleFullInfo.tSecondInfo.tEndlessLandData.wIsFirstBattle = true;


	// ���Իָ�ʱ��
	tBaseInfo.dwLastStaminaRecoverTime = dwCurTime;
	tBaseInfo.dwLastEnergyRecoverTime = dwCurTime;
	tBaseInfo.dwLastWantedRecoverTime = dwCurTime;

	// ��ɫ���Գ�ʼ��
	const RoleLevel_Config *pLevelCfg = g_pCfgMgr->GetRoleLevel(GetLevel());
	if (pLevelCfg)
	{
		tBaseInfo.dwStamina = pLevelCfg->dwStaminaMax;
		tBaseInfo.dwEnergy = pLevelCfg->dwEnergyMax;
		tBaseInfo.nWanted = pLevelCfg->dwWantedMax;
	}

	// �����
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
		// ͷ��
		tBaseInfo.dwHeadImage = pInitCfg->nHeadImage;

		// ��Ʒ
		if (pInitCfg->vItemList.size())
			AddObjectList(pInitCfg->vItemList, FALSE, FROM_NEW_ROLE_REWARD);

		// ����
		for (size_t i = 0; i < pInitCfg->vProList.size(); ++i)
		{
			ModifyProperty(pInitCfg->vProList[i].wType, pInitCfg->vProList[i].nValue, FALSE, FROM_NEW_ROLE);
		}

		//DWORD dwPlatfromType = GetAccountInfo()->GetPlatformType();
		//MAPLOG(GUI_LEVEL, "��������[PlatfromType��%d]", dwPlatfromType);

		const ClientInfo *pClientInfo = CGameLogic::Instance()->GetClientInfo(m_pAccountInfo->GetChannelUID(), m_pAccountInfo->GetAccID());
		BYTE byTerminalType = pClientInfo ? pClientInfo->byTerminalType : 0;

		// Ӣ��
		if (TERMINAL_IOS == byTerminalType && g_MapServer.IsExamineServer())	// ����״̬(ֻ��ios����Ҫ����) 
		{
			// (todo::����ʹ�õĹ���)
			const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(269);
			if (pServerCfg && pServerCfg->vValueList.size())
			{
				DWORD dwHeroInfoId = pServerCfg->GetListValue(g_Rand(0, pServerCfg->vValueList.size() - 1));
				DWORD dwHeroUID = m_pHeroMgr->AddHero(dwHeroInfoId, false, FROM_NEW_ROLE);
				m_tRoleFullInfo.tHeroInfo.arFormationList[0].dwHeroUID = dwHeroUID;
			}
			else
			{
				MAPLOG(ERROR_LEVEL, "�Ҳ������Ӣ���б�����[key=269]");
			}

			MAPLOG(GUI_LEVEL, "����Ŵ�����ɫ[role_id��%d,terminal_type:%d]", GetRoleId(), byTerminalType);
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

				// �ɾ�
				OnUpdateAchievement(21, m_pHeroMgr->GetToBattleHeroNum(), false);
			}
		}
	}

	// �½���ɫ�ʼ�
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

//������Ϣ
BOOL Player::SendPacket(NetMsgBody * pMsg)
{
	if (m_pSession == NULL)
	{
		return FALSE;
	}
	return m_pSession->SendPacket(GetGuid(), pMsg->GetType(), pMsg);
}

//������һ�����Ϣ
VOID Player::SendRoleBaseInfo()
{ 
	// װ���б�
	// �����б�

	static MSG_S2C_ROLE_INFO_NOTIFY msg;
	// ��ɫ������Ϣ
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
	MAPLOG(GUI_LEVEL, "[%s]���PVP����[RoleId:%d,Score:%d,WeekWinTimes:%d,IsLogin:%d]",
		__FUNCTION__, GetRoleId(), tPVPData.dwScore, tPVPData.wWeekWinTimes, isLogin);
}

BOOL Player::IsNewDay()
{
	//ת���ϴ�����ʱ��
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
// 	//ת���ϴ�����ʱ��
//  	time_t nLogout = m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime;
//  	tm tLast;
//  	localtime_s(&tLast, &nLogout);
//  
//  	time_t tCurTime = time(NULL);
//  	tm tCur;
//  	localtime_s(&tCur, &tCurTime);
//  
//  	// ����ͬһ��,������һ
//  	if (tLast.tm_yday != tCur.tm_yday && 1 == tCur.tm_wday)
//  		return TRUE;
//  
//  	return FALSE;

	// �ϴ�����ʱ��
	//WORD wLastWeekOfYear = g_GetWeekOfYear(m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime);
	//WORD wCurWeekOfYear = g_GetWeekOfYear(time(NULL));
	//return wLastWeekOfYear != wCurWeekOfYear ? TRUE : FALSE;

	DWORD dwLastMondayBeginTime = GetLastMondayBeginTime();
	return m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime < dwLastMondayBeginTime ? TRUE : FALSE;
}

BOOL Player::IsNewMonth()
{
	//ת���ϴ�����ʱ��
	time_t nLogout = m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime;
	tm tLast;
	localtime_s(&tLast, &nLogout);

	time_t tCurTime = time(NULL);
	tm tCur;
	localtime_s(&tCur, &tCurTime);

	// ����ͬһ�·�
	if (tLast.tm_mon != tCur.tm_mon)
		return TRUE;

	return FALSE;
}


//��������ű�
ERROR_CODE Player::HandleMissionScript(const char* pszScriptName, const char* pszScriptEventName, INT nEventPara)
{
// 	if (!pszScriptName || *pszScriptName == 0)
// 		return ERROR_CODE_MISSION_INVALID_SCRIPT_NAME;
// 
// 	if (!pszScriptEventName || *pszScriptEventName == 0)
// 		return ERROR_CODE_MISSION_INVALID_SCRIPT_EVENT_NAME;

	//ȡ�������¼�Table,���Բ�����
	LuaTable eventTable;
	if (!LuaInterface::Instance()->GetGlobal(pszScriptName, eventTable))
		return ERROR_CODE_OK;

	//ȡ�������¼��ص�����,���Բ�����
	LuaFunction callback;
	if (!eventTable.Get(pszScriptEventName, callback))
		return ERROR_CODE_OK;

	return ERROR_CODE_OK;
}

// ��ɫ��Ϣ����
void Player::RoleInfoUpdate(bool isSave)
{
	g_MapServer.AddUpdatePlayer(GetRoleId(), GetServerId(), isSave);
}

// ���³ɾ�
void Player::OnUpdateAchievement(WORD wAchievementId, int nValue, bool bAdd)
{
	// todo::��ʱ��������,�����û᲻�������������...
	if (!nValue)
		return;

	m_pAchievementMgr->OnUpdateAchievement(wAchievementId, nValue, bAdd);
}


// ȡVIP��Ȩֵ
int Player::GetVipValue(BYTE byType)
{
	return m_pVipMgr->GetValue(byType);
}
// �����ճ�����
void Player::OnUpdateDailyMission(WORD wMissionType, int nValue, bool bAdd)
{
	m_pDailyMissionMgr->OnUpdateMission(wMissionType, nValue, bAdd);

	// ����ȼ�����
	m_pWorldLevelTaskMgr->UpdateTaskEvent(wMissionType, nValue, bAdd);
}

// ȡ��Ʒ����
DWORD Player::GetItemNum(DWORD dwItemId)
{
	return m_pItemMgr->GetItemNum(dwItemId);
}

// ɾ����Ʒ
void Player::RemoveItem(DWORD dwItemId, DWORD dwItemNum, WORD wFromId)
{
	m_pItemMgr->RemoveItem(dwItemId, dwItemNum, TRUE, wFromId);
}

// ɾ����Ʒ
void Player::RemoveItem(const Item &tItem, WORD wFromId)
{
	m_pItemMgr->RemoveItem(tItem.dwId, tItem.dwNum, TRUE, wFromId);
}

// ս������
void Player::UpdateFightingPower()
{
	DWORD dwOldFightingPower = GetProperty(ROLE_PRO_FIGHTING_POWER);

	DWORD dwFightingPower = m_pPropertyManager->GetAllFightingPower();

	SetProperty(ROLE_PRO_FIGHTING_POWER, dwFightingPower, false);

	NotifyProperty(ROLE_PRO_FIGHTING_POWER, dwFightingPower);

	// ���ս���б䶯
	if (dwFightingPower != dwOldFightingPower)
	{
		// ���»������ݹ�������ս��
		PlayerBaseDataManager::Instance()->SetFightingPower(GetServerId(), GetRoleId(), dwFightingPower);

		// �ɾ��¼�
		OnUpdateAchievement(8, dwFightingPower, false);

		// 7���¼�(ս��ͻ��)
		OnUpdateSevenDayTarget(17, dwFightingPower, false);

		// �������а�
		CRankManager::Instance()->AddRoleRankData(RANK_BATTLE_VALUE, this, dwFightingPower, 0);

		// ����
		const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(135);
		if (pServerCfg)
		{
			ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(GetServerId());
			DWORD dwTargetFightingPower = pServerCfg->GetListValue(tGlobalData.dwFightingPowerRecordIdx);
			// ����Ŀ��ս��
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

// ȡ���Ἴ��
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

// ��ӹ��Ἴ��
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

// ��乤�Ἴ���б�
void Player::FillGuildSkillList(vector<SkillData> &vSkillList)
{
	vSkillList.clear();
	for (WORD i = 0; i < m_tRoleFullInfo.tGuildSkillData.wSkillNum; ++i)
	{
		vSkillList.push_back(m_tRoleFullInfo.tGuildSkillData.arSkillList[i]);
	}
}

// ��ѯĿ����ϸ��Ϣ�ص�
void Player::SelectTargetDetailsCallBack(BYTE byRequestType, const RoleFullInfo *pTargetFullInfo)
{	
	// todo::�պ���ܻ����
	
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

		// ���Ӣ����Ϣ
		pClientFormation->bySlot = i;
		pClientFormation->wHeroInfoId = pHero->dwIntoId;
		pClientFormation->byAwakenStar = pHero->wAwakenStar;
		pClientFormation->wBreakLevel = pHero->wBreakLevel;
		pClientFormation->byQaulity = pHero->wQuality;

		// ���װ����Ϣ
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

		// ��䱦����Ϣ
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

// ����������
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

// �����Ʒ����
bool Player::CheckItemCost(const vector<Item> &vCostItemList, WORD wNum)
{
	for (size_t i = 0; i < vCostItemList.size(); ++i)
	{
		const Item &tCostItem = vCostItemList[i];
		if (GetItemNum(tCostItem.dwId) < tCostItem.dwNum * wNum)
		{
			MAPLOG(ERROR_LEVEL, "[%s]�����Ʒ���Ĳ���[CostItemId:%d,CostItemNum:%d]!",
				__FUNCTION__, tCostItem.dwId, tCostItem.dwNum * wNum);
			return false;
		}
	}
	return true;
}

// �����������
void Player::DoMoneyCost(const vector<CostData> &vCostList, WORD wNum, WORD wFromId)
{
	for (size_t i = 0; i < vCostList.size(); ++i)
	{
		ModifyProperty(vCostList[i].wType, -vCostList[i].nValue * wNum, TRUE, wFromId);
	}
}

// ȡ��ǰʹ�õĳƺ�Id
int Player::GetCurUseTitleId()
{
	return m_tRoleFullInfo.tSecondInfo.tTitleInfo.nCurUseTitleId;
}

// ���һ���Ա�־
BOOL Player::CheckOnceFlag(int nFlagIdx)
{
	return ISBIT(m_tRoleFullInfo.tRoleBaseInfo.nOnceFlagList, nFlagIdx);
}

// ����һ���Ա�־
VOID Player::SetOnceFlag(int nFlagIdx)
{
	SETBIT(m_tRoleFullInfo.tRoleBaseInfo.nOnceFlagList, nFlagIdx);
}

// ���һ���Ա�־
VOID Player::ClearOnceFlag(int nFlagIdx)
{
	CLRBIT2(m_tRoleFullInfo.tRoleBaseInfo.nOnceFlagList, nFlagIdx);
}

// ����7��Ŀ������
void Player::OnUpdateSevenDayTarget(WORD wLogicType)
{
	// ����Ƿ���
// 	if (!m_pActivityManager->IsActivityOpen(52))
// 		return;

	m_pSevenDayManager->OnUpdateTarget(wLogicType);
}

// ����7��Ŀ������
void Player::OnUpdateSevenDayTarget(WORD wLogicType, DWORD dwX, bool bAdd)
{
	// ����Ƿ���
// 	if (!m_pActivityManager->IsActivityOpen(52))
// 		return;

	m_pSevenDayManager->OnUpdateTarget(wLogicType, dwX, bAdd);
}

// ���ջ�ԾĿ���¼�
void Player::OnFestivalActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd)
{
	m_pActivityManager->GetFestivalActivity().OnActiveTarget(wActivityType, wLogicType, dwX, bAdd);
}

// �洢��Ʒ��ʧ��¼
void Player::SaveItemLoseRecord(DWORD dwItemId, DWORD dwItemNum)
{
	// �������ݿ�
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

// ��ס�ʺ�
void Player::LockAccount()
{
	AccountLock_Update* pQuery = AccountLock_Update::ALLOC();
	pQuery->SetIndex(DB_ROLE_ACCOUNT_LOCK_UPDATE);
	pQuery->SetThreadIndex(DB_THREAD_COMMON);
	pQuery->SetAccount(GetAccID());
	pQuery->SetPlatform(m_pAccountInfo->GetChannelUID());
	g_MapServer.MapDBQuery(pQuery);
}

// �������������¼�
void Player::OnUpdateWorldTaskEvent(WORD wTaskType, int nContext)
{
	m_pWorldLevelTaskMgr->UpdateTaskEvent(wTaskType, nContext);
}

// ��Ⲣ�������ʥ��������
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