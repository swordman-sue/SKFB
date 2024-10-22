#include "stdafx.h"
#include "RebelManager.h"
#include "..\ConfigManager.h"
#include "..\Player.h"
#include "ToolKit\Rand.h"
#include "CommonLogic.h"
#include "ToolKit\Singleton.h"
#include "..\Rank\RankManager.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\PlayerBaseDataManager.h"
#include "..\RedPoint\RedPointManager.h"

CRebelManager::CRebelManager()
{
	m_dwSeriesId = 1;
	m_lProcessTime = GetTickCount64();
}

CRebelManager::~CRebelManager()
{

}

void CRebelManager::Init()
{
	m_dwSeriesId = 1;
	m_nRebelShareMax = 3;
	m_lProcessTime = GetTickCount64();
}

void CRebelManager::Update()
{
	if (!m_RebelList.size())
		return;

	DWORD curTime = time(NULL);
	REBEL_MAP::iterator iter = m_RebelList.begin();
	for (; iter != m_RebelList.end(); ++iter)
	{
		RebelData *pRebel = &iter->second;
		if (!pRebel->byIsRemove && curTime >= pRebel->dwCreateTime + pRebel->dwLiveTime && !pRebel->IsDead())
		{
			RemoveRebelData(pRebel->dwRebelId);
			break;
		}

		// 存活时间已经过很久的(10分钟)
		if (pRebel->byIsRemove && curTime >= pRebel->dwCreateTime + pRebel->dwLiveTime + 60 * 1)
		{
			m_RebelList.erase(iter);
			break;
		}
	}

	// todo::定期的更新叛军列表
}

// 请求叛军数据 
void CRebelManager::OnRebelDataReq(Player *pPlayer)
{
	SendRebelData(pPlayer);
}

// 领取叛军功勋奖励
void CRebelManager::OnRebelAwardReq(Player *pPlayer, WORD *pAwardList, BYTE byAwardNum)
{
	WORD wFromMsgId = C2S_REBEL_EXPLOIT_AWARD_REQ;

	if (!pAwardList || !byAwardNum)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]领取叛军功勋奖励参数不合法", __FUNCTION__);
		return;
	}

	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	WORD wInitRoleLevel = pPlayer->GetDailyInitLevel();

	const RoleRebel_Config *pRoleRebelCfg = CConfigManager::Instance()->GetRoleRebel(wInitRoleLevel);
	if (!pRoleRebelCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色的叛军配置数据[Level:%d]", __FUNCTION__, wInitRoleLevel);
		return;
	}

	MSG_S2C_REBEL_EXPLOIT_AWARD_RESP msg;
	msg.m_wAwardNum = 0;

	for (BYTE i = 0; i < byAwardNum; ++i)
	{
		WORD wAwardId = pAwardList[i];
		const RebelExploit_Config *pExploitCfg = CConfigManager::Instance()->GetRebelExploit(pRoleRebelCfg->wAwardListId, wAwardId);
		if (!pExploitCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到叛军功勋配置[AwardListId:%d, AwardId:%d]",
				__FUNCTION__, pRoleRebelCfg->wAwardListId, wAwardId);
			continue;
		}

		// 检测功勋是否足够
		if (pPlayerRebelData->dwTodayExploit < pExploitCfg->nNeedExploit)
		{
			MAPLOG(ERROR_LEVEL, "[%s]功勋不足,不能领取奖励[AwardListId:%d, AwardId:%d]",
				__FUNCTION__, pRoleRebelCfg->wAwardListId, wAwardId);
			continue;
		}

		// 检测是否已领取过
		if (pPlayerRebelData->IsInAwardRecordList(wAwardId))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]叛军功勋奖励已领取过[AwardListId:%d,AwardId:%d]",
				__FUNCTION__, pRoleRebelCfg->wAwardListId, wAwardId);
			return;
		}

		// 物品奖励
		if (pExploitCfg->wAwardItemId)
		{
			if (!pPlayer->AddObject(pExploitCfg->wAwardItemId, pExploitCfg->wAwardItemNum, TRUE, FROM_REBEL_REWARD))
			{
				MAPLOG(ERROR_LEVEL, "[%s]添加功勋奖励物品失败[AwardId:%d,AwardNum:%d]",
					__FUNCTION__, pExploitCfg->wAwardItemId, pExploitCfg->wAwardItemNum);
				continue;
			}
		}
		// 金币奖励
		else
		{
			pPlayer->ModifyProperty(ROLE_PRO_GOLD, pExploitCfg->nAwardGold, TRUE, FROM_REBEL_REWARD);
		}


		// 记录已经领取过的奖励
		pPlayerRebelData->AddAwardRecord(wAwardId);

		if (msg.m_wAwardNum < MAX_EXPLOIT_AWARD_NUM)
			msg.m_AwardList[msg.m_wAwardNum++] = wAwardId;
	}

	pPlayer->SendPacket(&msg);
}

// 请求功勋奖励记录
void CRebelManager::OnNetAwardRecordList(Player *pPlayer)
{
	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	MSG_S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP msg;
	msg.m_byAwardNum = pPlayerRebelData->wAwardRecordNum;

	int nCopySize = sizeof(WORD)* pPlayerRebelData->wAwardRecordNum;
	memcpy_s(msg.m_AwardList, nCopySize, pPlayerRebelData->arAwardRecordList, nCopySize);

	pPlayer->SendPacket(&msg);
}

// 分享叛军
void CRebelManager::OnNetRebelShare(Player *pPlayer)
{
	WORD wFromMsgId = C2S_REBEL_SHARE_REQ;

	DWORD dwRebelId = GetSelfRebelId(pPlayer->GetCharID());
	if (!dwRebelId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家当前木有叛军，分享失败[RebelId:%d]",
			__FUNCTION__, dwRebelId);
		return;
	}

	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	// 检测是否已经分享过
	if (pPlayerRebelData->wIsAlreadyShare)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]叛军已经分享过[RebelId:%d]",
			__FUNCTION__, pPlayerRebelData->dwTodayDamage);
		return;
	}

	CFriendManager *pFriendMgr = pPlayer->GetFriendManager();
	CHECK_POINTER(pFriendMgr);

	BYTE byFriendNum = pFriendMgr->GetFriendNum();
	const FriendData *pFrendList = pFriendMgr->GetFrindList();

	for (BYTE i = 0; i < byFriendNum; ++i)
	{
		const FriendData &tFriendData = pFrendList[i];
		AddShareData(tFriendData.dwFriendId, dwRebelId);

		// 如果目标在线则通知
		Player *pPlayer = PlayerManager::Instance()->FindPlayerByCharID(tFriendData.dwFriendId);
		if (pPlayer && pPlayer->GetRedPointManager())
			pPlayer->GetRedPointManager()->OnRedPointNotify(SYSTEM_REBEL);
	}

	pPlayerRebelData->wIsAlreadyShare = true;

	MSG_S2C_REBEL_SHARE_RESP msg;
	pPlayer->SendPacket(&msg);

	// 日常任务
	pPlayer->OnUpdateDailyMission(17);
}

// 开始战斗
bool CRebelManager::OnBattleStart(Player *pPlayer, DWORD dwRebelId, WORD wAttackType)
{
	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]叛军不存在[RebelId:%d]",__FUNCTION__, dwRebelId);
		return false;
	}

	const RebelBattle_Config *pRebelBattleCfg = CConfigManager::Instance()->GetRebelBattle(pRebelData->wLevel);
	if (!pRebelBattleCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到叛军等级配置[RebelLv:%d]", __FUNCTION__, pRebelData->wLevel);
		return false;
	}

	// 是否已死亡
	if (pRebelData->IsDead())
	{
		SendRebelData(pPlayer); // 如果种情况可能是数据已经不同步
		MAPLOG(ERROR_LEVEL, "[%s]叛军已经挂掉[RebelId:%d]", __FUNCTION__, dwRebelId);
		return false;
	}

	int nCostWanted = 0;

	// 普通攻击
	if (1 == wAttackType)
	{
		nCostWanted = g_GetConfigValue(38);
	}
	// 全力一击
	else
	{
		nCostWanted = g_GetConfigValue(39);
		if (IsCostHalveActivity())
			nCostWanted *= 0.5;
	}

	// 检测通缉令
	if (pPlayer->GetProperty(ROLE_PRO_WANTED) < nCostWanted)
	{
		MAPLOG(ERROR_LEVEL, "[%s]通缉令不足,不能战斗[AttackType:%d]", __FUNCTION__, wAttackType);
		return false;
	}

	// 添加怪物数据
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	for (size_t i = 0; i < pRebelData->vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pRebelData->vMonsterList[i]);
	}

	pFBMgr->SetBattleInfo(0, wAttackType, pRebelData->wMapId);
	pFBMgr->SetTargetId(dwRebelId);

	return true;
}

// 战斗结束
bool CRebelManager::OnBattleOver(Player *pPlayer, DWORD dwRebelId, WORD wAttackType, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	// todo::作为测试使用
// 	MAPLOG(ERROR_LEVEL, "开始输出伤害......");
// 	for (BYTE i = 0; i < byMonsterNum; ++i)
// 	{
// 		MonsterData *pMonster = &pMonsterList[i];
// 		MAPLOG(ERROR_LEVEL, "怪物伤害信息[Index:%d,Damage:%d]", pMonster->byIndex, pMonster->dwDamage);
// 	}
// 	MAPLOG(ERROR_LEVEL, "结束输出伤害......");


	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]叛军不存在[RebelId:%d]", __FUNCTION__, dwRebelId);
		return false;
	}

	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	int nCostWanted = 0;

	// 普通攻击
	if (1 == wAttackType)
	{
		nCostWanted = g_GetConfigValue(38);
	}
	// 全力一击
	else
	{
		nCostWanted = g_GetConfigValue(39);
		if (IsCostHalveActivity())
			nCostWanted *= 0.5;
	}

	// 获得战功=消耗的征讨令*50
	int nRebelValue = nCostWanted * 50;
	
	// 还木有死亡
	if (!pRebelData->IsDead())
	{
		// 更新伤害记录
		UpdateRebelDamage(dwRebelId, pMonsterList, byMonsterNum);

		if (pRebelData->IsDead())
		{
			// 自己的叛军
			if (pRebelData->dwRoleId == pPlayer->GetCharID())
			{
				pPlayerRebelData->wRebelLevel++;
			}
			// 好友的叛军
			else
			{
				Player *pDiscoverer = PlayerManager::Instance()->FindPlayerByCharID(pRebelData->dwRoleId);
				if (pDiscoverer)
					pDiscoverer->ModifyProperty(ROLE_PRO_REBEL_LEVEL, 1, false);
			}

			// 击杀者奖励
			RewardData tKillReward;
			tKillReward.wInfoId = 2;
			tKillReward.dwTime = time(NULL);
			tKillReward.dwValue1 = pRebelData->wMapId;
			tKillReward.dwValue2 = pRebelData->byQuality;
			pPlayer->GetRewardCenterManager()->AddReward(tKillReward);

			// 发现者奖励
			RewardData tDiscoverReward;
			tDiscoverReward.wInfoId = 3;
			tDiscoverReward.dwTime = time(NULL);
			tDiscoverReward.dwValue1 = pRebelData->wMapId;
			tDiscoverReward.dwValue2 = pRebelData->byQuality;
			//strcpy_s(tDiscoverReward.szText, sizeof(tDiscoverReward.szText) - 1, pPlayer->GetRoleName());
			CRewardCenterManager::AddReward(pPlayer->GetServerId(), pRebelData->dwRoleId, tDiscoverReward);

			RemoveRebelData(dwRebelId);
		}
	}

	pPlayer->ModifyProperty(ROLE_PRO_WANTED, -nCostWanted);

	DWORD dwTotalDamage = 0;
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		dwTotalDamage += pMonsterList[i].dwDamage;
	}
	pPlayerRebelData->dwTodayDamage += dwTotalDamage;
	
	DWORD dwTodayExploit = 0;

	// 获得功勋=(本次造成伤害/1000)向上取整
	if (dwTotalDamage)
		dwTodayExploit = ceil(double(dwTotalDamage * 0.001));

	// 活动期间双倍奖励
	if (IsDoubleAwardActivity())
		dwTodayExploit = dwTodayExploit * 2;

	pPlayerRebelData->dwTodayExploit += dwTodayExploit;

	pPlayer->ModifyProperty(ROLE_PRO_REBEL_VALUE, nRebelValue);

	WORD wOldDamageRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_DAMAGE);
	WORD wOldExploitRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_EXPLOIT);

	// 更新排行榜
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_REBEL_DAMAGE, pPlayer->GetRoleId());
	if (!pRankData || dwTotalDamage > pRankData->dwValue)	
		CRankManager::Instance()->AddRoleRankData(RANK_REBEL_DAMAGE, pPlayer, dwTotalDamage, 0, 0, 200);  // 伤害榜是单次最高伤害
	CRankManager::Instance()->AddRoleRankData(RANK_REBEL_EXPLOIT, pPlayer, pPlayerRebelData->dwTodayExploit,0,0, 200);

	WORD wNewDamageRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_DAMAGE);
	WORD wNewExploitRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_EXPLOIT);

	// todo::暂时这样处理
	CFubenManager *pMapMgr = pPlayer->GetFubenManager();
	if (pMapMgr)
	{
		BYTE byIndex = 0;
		pMapMgr->SetParam(byIndex++, wOldDamageRank);
		pMapMgr->SetParam(byIndex++, wOldExploitRank);
		pMapMgr->SetParam(byIndex++, wNewDamageRank);
		pMapMgr->SetParam(byIndex++, wNewExploitRank);

		pMapMgr->SetParam(byIndex++, nRebelValue);
	}

	// 日常任务
	pPlayer->OnUpdateDailyMission(18);

	// 成就事件
	pPlayer->OnUpdateAchievement(16);

	// 节日活动(累计攻击叛军次数)
	pPlayer->OnFestivalActiveTarget(ACTIVATE_TARGET, 16, 1, true);

	// 节日活跃目标(累计攻击叛军次数)
	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_14, 16, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_15, 16, 1, true);

	pPlayer->OnFestivalActiveTarget(FESTIVAL_ACTIVITY_16, 16, 1, true);

	// 异度入侵刷战功
	if (dwTodayExploit)
		pPlayer->OnUpdateSevenDayTarget(10, dwTodayExploit, true);

	return true;
}

// 随机叛军
DWORD CRebelManager::CreateRebel(Player *pPlayer)
{
	// 检测是否开放
	if (g_pCfgMgr->GetSystemOpenLevel(SYSTEM_REBEL) > pPlayer->GetLevel())
		return 0;

	// 当前已有叛军
	DWORD dwRebelId = GetSelfRebelId(pPlayer->GetCharID());
	if (dwRebelId)
		return 0;

	int nRate = CConfigManager::Instance()->GetServerCfg(29);
	if (Rand::Instance()->irand(0, RAND_RANGE_MAX_10000) < nRate)
	{
		PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

		WORD wRebelLevel = pPlayerRebelData->wRebelLevel;

		// todo::暂时先这样处理，查到BUG以后再去掉
// 		if (!pPlayerRebelData->wRebelLevel)
// 			pPlayerRebelData->wRebelLevel = 1;
		if (!wRebelLevel)
			MAPLOG(ERROR_LEVEL, "[%s]叛军当前等级[Level:%d]", __FUNCTION__, wRebelLevel);

		WORD wDailyInitRoleLevel = pPlayer->GetDailyInitLevel();

		const RoleRebel_Config *pRoleRebelCfg = CConfigManager::Instance()->GetRoleRebel(wDailyInitRoleLevel);
		if (!pRoleRebelCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到角色的叛军配置数据[Level:%d]", __FUNCTION__, wDailyInitRoleLevel);
			return 0;
		}

		// 随机叛军品质
		int byRandValue = Rand::Instance()->irand(0, pRoleRebelCfg->GetTotalRate());
		BYTE byRandQuality = pRoleRebelCfg->GetQuality(byRandValue);
		if (QUALITY_INVALID == byRandQuality)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到叛军品质几率数据[rate:%d]", __FUNCTION__, byRandValue);
			return 0;
		}

		// 叛军品质配置
		const RebelQuality_Config *pQualityCfg = CConfigManager::Instance()->GetRebelQuality(byRandQuality);
		if (!pQualityCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到叛军品质配置[quality:%d]", __FUNCTION__, byRandQuality);
			return 0;
		}

		// 随机关卡
		const RebelBattle_Config *pBattleCfg = CConfigManager::Instance()->GetRebelBattle(wRebelLevel);
		if (!pBattleCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到叛军战役配置[RebelLv:%d]", __FUNCTION__, wRebelLevel);
			return 0;
		}
		if (!pBattleCfg->vRandMapList.size())
		{
			MAPLOG(ERROR_LEVEL, "[%s]叛军随机关卡列表为空[RebelLv:%d]", __FUNCTION__, wRebelLevel);
			return 0;
		}
		WORD wMapId = pBattleCfg->vRandMapList[g_Rand(1, pBattleCfg->vRandMapList.size())-1];

		dwRebelId = m_dwSeriesId++;

		RebelData tRebelData;
		tRebelData.dwRebelId = dwRebelId;
		tRebelData.wLevel = wRebelLevel;
		tRebelData.byQuality = byRandQuality;
		tRebelData.dwRoleId = pPlayer->GetCharID();
		strcpy_s(tRebelData.szRoleName, sizeof(tRebelData.szRoleName) - 1, pPlayer->GetRoleName());
		tRebelData.dwCreateTime = time(NULL);
		//tRebelData.isDead = false;
		tRebelData.dwLiveTime = pQualityCfg->dwLiveTime * 60;
		tRebelData.wMapId = wMapId;

		// 取关卡怪物数据
		if (!g_pCfgMgr->GetMapMonsterList(wMapId, &tRebelData.vMonsterList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]取不到叛军关卡的怪物列表[MapId:%d]", __FUNCTION__, wMapId);
			return 0;
		}

		// 品质影响的
		float fBloodIncRate = 0;
		for (size_t i = 0; i < pQualityCfg->vProList.size(); ++i)
		{
			if (HERO_PRO_BLOOD == pQualityCfg->vProList[i].wType)
			{
				fBloodIncRate = pQualityCfg->vProList[i].nValue;
				break;
			}
		}
		// 战役影响的
		float fBloodRate = 0;
		for (size_t i = 0; i < pBattleCfg->vAddProList.size(); ++i)
		{
			if (HERO_PRO_BLOOD_RATE == pBattleCfg->vAddProList[i].wType)
			{
				fBloodRate = pBattleCfg->vAddProList[i].nValue;
				break;
			}
		}
		// 血量按公式扩大
		for (size_t i = 0; i < tRebelData.vMonsterList.size(); ++i)
		{
			DWORD &dwTotalHP = tRebelData.vMonsterList[i].dwTotalHP;
			dwTotalHP *= (1 + fBloodRate * PROPERTY_DECREASE_MULTIPLE);
			dwTotalHP *= (1 + fBloodIncRate * PROPERTY_DECREASE_MULTIPLE);
		}

		AddRebelData(tRebelData);

		pPlayerRebelData->wIsAlreadyShare = false;

		MSG_S2C_REBEL_DISCOVER_NOTIFY msg;
		msg.m_byQaulity = byRandQuality;
		msg.m_wLevel = wRebelLevel;
		msg.m_wMapId = wMapId;
		msg.m_dwRebelId = dwRebelId;
		pPlayer->SendPacket(&msg);

		MAPLOG(GUI_LEVEL, "[%s]触发叛军[RebleId:%d]", __FUNCTION__, dwRebelId);

		// 推送红点提示
		if (pPlayer->GetRedPointManager())
			pPlayer->GetRedPointManager()->OnRedPointNotify(SYSTEM_REBEL);
	}

	return dwRebelId;
}

// 添加叛军数据
void CRebelManager::AddRebelData(const RebelData &tRebelData)
{
	if (GetSelfRebelId(tRebelData.dwRoleId))
	{
		MAPLOG(ERROR_LEVEL, "[%s]当前已有叛军数据,添加失败[RoleId:%d]", __FUNCTION__, tRebelData.dwRoleId);
		return;
	}

	m_RebelList.insert(make_pair(tRebelData.dwRebelId, tRebelData));

	m_RebelDiscoverList.insert(make_pair(tRebelData.dwRoleId, tRebelData.dwRebelId));
}

// 添加分享数据
void CRebelManager::AddShareData(DWORD dwShareTargetId, DWORD dwRebelId)
{
	RebelData *pRebel = GetRebelData(dwRebelId);
	if (!pRebel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到叛军数据[RebelId:%d]", __FUNCTION__, dwRebelId);
		return;
	}

	// 记录已经分享出去的
	pRebel->vSharePlayerList.push_back(dwShareTargetId);

	// 记录玩家收到的分享
	vector<DWORD> *pRebelList = GetShareRebelList(dwShareTargetId);
	if (pRebelList)
	{
		pRebelList->push_back(dwRebelId);
	}
	else
	{
		vector<DWORD> RebelList;
		RebelList.push_back(dwRebelId);
		m_RebelShareList.insert(make_pair(dwShareTargetId, RebelList));
	}
}

// 删除分离数据
void CRebelManager::RemoveShareData(DWORD dwShareTargetId, DWORD dwRebelId)
{
	vector<DWORD> *pRebelList = GetShareRebelList(dwShareTargetId);
	if (!pRebelList)
		return;

	for (vector<DWORD>::iterator iter = pRebelList->begin(); iter != pRebelList->end(); ++iter)
	{
		if (*iter == dwRebelId)
		{
			pRebelList->erase(iter);
			break;
		}
	}
}

// 删除叛军数据
void CRebelManager::RemoveRebelData(DWORD dwRebelId)
{
	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData/* && pRebelData->IsDead()*/)
	{
		MAPLOG(ERROR_LEVEL, "[%s]叛军已不存在[RebelId:%d]", __FUNCTION__, dwRebelId);
		return;
	}

	// 删除发现者的数据
	m_RebelDiscoverList.erase(pRebelData->dwRoleId);

	// 删除分享出去的数据
	for (int i = 0; i < pRebelData->vSharePlayerList.size(); ++i)
	{
		RemoveShareData(pRebelData->vSharePlayerList[i], dwRebelId);
	}

	pRebelData->byIsRemove = true;
}

// 取叛军数据
RebelData* CRebelManager::GetRebelData(DWORD dwRebelId)
{
	REBEL_MAP::iterator iter = m_RebelList.find(dwRebelId);
	return iter != m_RebelList.end() ? &iter->second : NULL;
}

// 取自己发现的叛军ID
DWORD CRebelManager::GetSelfRebelId(DWORD dwRoleId)
{
	REBEL_DISCOVER_MAP::iterator iter =  m_RebelDiscoverList.find(dwRoleId);
	return iter != m_RebelDiscoverList.end() ? iter->second : NULL;
}

// 发送叛军数据
void CRebelManager::SendRebelData(Player *pPlayer)
{
	PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;

	MSG_S2C_REBEL_DATA_RESP msg;
	memset(&msg.m_RebelData, 0, sizeof(msg.m_RebelData));
	msg.m_RebelData.wExploitRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_EXPLOIT);
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_REBEL_DAMAGE, pPlayer->GetRoleId());
	//msg.m_RebelData.wDamageRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_REBEL_DAMAGE);
	msg.m_RebelData.wDamageRank = pRankData ? pRankData->wRank : 0;
	msg.m_RebelData.dwTodayDamage = pRankData ? pRankData->dwValue : 0;
	msg.m_RebelData.dwTodayExploit = pPlayerRebelData->dwTodayExploit;
	//msg.m_RebelData.dwTodayDamage = pPlayerRebelData->dwTodayDamage;
	msg.m_RebelData.byIsAlreadyShare = pPlayerRebelData->wIsAlreadyShare;
	msg.m_RebelData.wInitRoleLevel = pPlayer->GetDailyInitLevel();

	// 自己发现的叛军
	DWORD dwRebelId = GetSelfRebelId(pPlayer->GetCharID());
	RebelData *pRebelData = dwRebelId ? GetRebelData(dwRebelId) : NULL;
	if (pRebelData)
	{
		ClientRebelData tRebelData;
		tRebelData.Copy(*pRebelData);
		msg.m_RebelData.AddRebelData(tRebelData);
	}

	// 别人分享的叛军
	vector<DWORD>* pRebelIdList = GetShareRebelList(pPlayer->GetCharID());
	if (pRebelIdList)
	{
		vector<DWORD>::iterator iter = pRebelIdList->begin();
		for (; iter != pRebelIdList->end(); ++iter)
		{
			pRebelData = GetRebelData(*iter);
			if (!pRebelData)
			{
				MAPLOG(ERROR_LEVEL, "[%s]查找不到别人分享的叛军数据[RebelId:%d]", __FUNCTION__, *iter);
				continue;
			}

			ClientRebelData tRebelData;
			tRebelData.Copy(*pRebelData);
			msg.m_RebelData.AddRebelData(tRebelData);
		}
	}

	pPlayer->SendPacket(&msg);
}

// 是否有红点
bool CRebelManager::IsHaveRedPoint(Player *pPlayer)
{
	// 当叛军主界面有可击杀的叛军时(自己发现的)
	if (GetSelfRebelId(pPlayer->GetRoleId()))
		return true;

	// 当叛军主界面有可击杀的叛军时(好友分享的)
	vector<DWORD> *pFriendRebelList = GetShareRebelList(pPlayer->GetRoleId());
	if (pFriendRebelList && pFriendRebelList->size())
		return true;

	// 当叛军主界面内有功勋奖励可领取时
	const PlayerRebelData *pPlayerRebelData = &pPlayer->GetRoleFullInfo().tRebelInfo;
	if (!pPlayerRebelData->dwTodayExploit)
		return false;
	WORD wInitRoleLevel = pPlayer->GetDailyInitLevel();
	const RoleRebel_Config *pRoleRebelCfg = g_pCfgMgr->GetRoleRebel(wInitRoleLevel);
	if (!pRoleRebelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到角色的叛军配置数据[Level:%d]", __FUNCTION__, wInitRoleLevel);
		return false;
	}
	const map<DWORD, RebelExploit_Config> &ExpoitCfgList = g_pCfgMgr->GetRebelExpoitList();
	map<DWORD, RebelExploit_Config>::const_iterator iter = ExpoitCfgList.begin();
	for (; iter != ExpoitCfgList.end(); ++iter)
	{
		if (iter->second.dwListId == pRoleRebelCfg->wAwardListId &&				// 同一个列表
			iter->second.nNeedExploit <= pPlayerRebelData->dwTodayExploit &&	// 功勋满足
			!pPlayerRebelData->IsInAwardRecordList(iter->second.dwAwardId))		// 还木领取过
			return true;
	}

	return false;
}

// 取别人分享给自己的叛军列表
vector<DWORD>* CRebelManager::GetShareRebelList(DWORD dwRoleId)
{
	REBEL_SHARE_MAP::iterator iter = m_RebelShareList.find(dwRoleId);
	return iter != m_RebelShareList.end() ? &iter->second : NULL;
}

// 是否消耗减半活动
bool CRebelManager::IsCostHalveActivity()
{
	int nConfigKey = 36;
	const Server_Config *pServerCfg = CConfigManager::Instance()->GetCommonCfg(nConfigKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置数据[key:%d]", __FUNCTION__, nConfigKey);
		return false;
	}

	if (pServerCfg->vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]全力一击消耗减半时间配置出错", __FUNCTION__);
		return false;
	}

	int nBeginTime = pServerCfg->vValueList[0];
	int nEndTime = pServerCfg->vValueList[1];

	DATETIME datetime(time(NULL));
	int nCurTime = datetime.hour * 100 + datetime.minute;

	return nCurTime >= nBeginTime && nCurTime <= nEndTime ? true : false;
}

// 是否奖励翻倍活动
bool CRebelManager::IsDoubleAwardActivity()
{
	int nConfigKey = 37;
	const Server_Config *pServerCfg = CConfigManager::Instance()->GetCommonCfg(nConfigKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到通用配置数据[key:%d]", __FUNCTION__, nConfigKey);
		return false;
	}

	if (pServerCfg->vValueList.size() != 2)
	{
		MAPLOG(ERROR_LEVEL, "[%s]攻打叛军功勋翻倍时间配置出错", __FUNCTION__);
		return false;
	}

	int nBeginTime = pServerCfg->vValueList[0];
	int nEndTime = pServerCfg->vValueList[1];

	DATETIME datetime(time(NULL));
	int nCurTime = datetime.hour * 100 + datetime.minute;


	return nCurTime >= nBeginTime && nCurTime <= nEndTime ? true : false;
}

// 更新叛军伤害
void CRebelManager::UpdateRebelDamage(DWORD dwRebelId, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	RebelData *pRebelData = GetRebelData(dwRebelId);
	if (!pRebelData)
	{
		MAPLOG(ERROR_LEVEL, "[%s]叛军不存在[RebelId:%d]", __FUNCTION__, dwRebelId);
		return;
	}

	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		MonsterData &tMonster = pMonsterList[i];
		if (tMonster.byIndex >= MAX_BATTLE_MONSTER_NUM)
		{
			MAPLOG(ERROR_LEVEL, "[%s]怪物的站位错误![pos:%d]", __FUNCTION__, tMonster.byIndex);
			continue;
		}
		for (size_t k = 0; k < pRebelData->vMonsterList.size(); ++k)
		{
			if (pRebelData->vMonsterList[k].byIndex == tMonster.byIndex)
			{
				pRebelData->vMonsterList[k].dwDamage += tMonster.dwDamage;
				break;
			}
		}
	}
}