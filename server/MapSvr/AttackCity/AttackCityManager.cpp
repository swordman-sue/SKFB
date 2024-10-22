#include "stdafx.h"
#include "AttackCityManager.h"
#include "..\Player.h"
#include "..\PlayerManager.h"
#include "..\PlayerBaseDataManager.h"
#include "ToolKit\Rand.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "..\Guild\GuildManager.h"
#include "..\Email\EmailManager.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\Hero\HeroManager.h"
#include "..\AccountInfo.h"
#include "..\Friend\FriendManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\Rank\RankManager.h"
#include "..\Item\ItemManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "..\Spirit\SpiritManager.h"
#include "..\Title\TitleManager.h"
#include "..\SystemNotice\SystemNotice.h"

CAttackCityManager::CAttackCityManager()
{
	m_bStart = false;
	m_byActivityStatus = 0;
	m_dwSeriesId = 0;
	DWORD dwCurTime = time(NULL);
	m_dwProcessRefreshBoxTimer = dwCurTime;
	//m_dwProcessMonsterTimer = dwCurTime;
}

CAttackCityManager::~CAttackCityManager()
{

}

void CAttackCityManager::Init()
{
	//m_wCurWave = 1;
	m_dwRefreshTreasureBoxCD = g_GetConfigValue(169);
	m_dwRefreshMonsterCD = g_GetConfigValue(170);
}

// 开始
void CAttackCityManager::Start()
{
	m_bStart = true;
}

void CAttackCityManager::Update()
{
	if (!m_bStart)
		return;

	tm tmDetailTime;
	GetTMTime(&tmDetailTime, time(NULL));
	const AttackCityActivityTime_Config *pActivityTimeCfg = g_pCfgMgr->GetAttackCityActivityTime(tmDetailTime.tm_wday);
	// 今天木有活动
	if (!pActivityTimeCfg)
		return;

	int nCurTime = tmDetailTime.tm_hour * 100 + tmDetailTime.tm_min;

	// 空闲
	if (0 == m_byActivityStatus)
	{
		// 检测活动是否开始
		if (nCurTime >= pActivityTimeCfg->nStartTime && nCurTime <= pActivityTimeCfg->nOverTime)
			OnActivityStart();
	}
	// 活动中
	else if (1 == m_byActivityStatus)
	{
		OnActivityIng();

		// 检测活动是否结束
		if (nCurTime >= pActivityTimeCfg->nOverTime)
			OnActivityOver();
	}
	// 等待发奖励
	else if (2 == m_byActivityStatus)
	{
		OnReward();
	}
}

// 登出
void CAttackCityManager::OnLogout(Player *pPlayer)
{
	DoLeaveAttackCity(pPlayer);
}

// 战斗开始
bool CAttackCityManager::OnBattleStart(Player *pPlayer, DWORD dwTargetId)
{
	// 取怪物数据
	AttackCityMap *pMap = GetAttackCityMap(pPlayer->GetServerId(), dwTargetId);
	if (!pMap)
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_DIE);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]怪物不存在或者已死亡[target_id:%d]", dwTargetId);
		return false;
	}

	// 检测是否已死亡
	if (/*IsMonsterDead(*pMap)*/pMap->bIsDead)
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_DIE);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]怪物已死亡[target_id:%d]", dwTargetId);
		return false;
	}

	const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(pMap->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]找不到攻城章节配置[chapter:%d]", pMap->wChapter);
		return false;
	}

	// 怪物是否正在战斗中
	//if (pMap->bIsInBattle)
	if (pMap->dwStartBattleTime && time(NULL) < pMap->dwStartBattleTime + g_GetConfigValue(209))
	{
		pPlayer->SendErrorMsg(ERROR_ATTCK_CITY_TARGET_INVALID);
		MAPLOG(ERROR_LEVEL, "[OnBattleStart]怪物正在战斗中...[target_id:%d]", dwTargetId);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	for (size_t i = 0; i < pMap->vMonsterList.size(); ++i)
	{
		pFBMgr->AddMonsterData(pMap->vMonsterList[i]);
	}

	pFBMgr->SetBattleInfo(pMap->wChapter, 0, pMap->dwMapId);
	pFBMgr->SetTargetId(dwTargetId);

	// 如果是BOSS关,加入挑战列表
	if (pChapterCfg->byIsBoss)
		AddBattleBossPlayer(pPlayer);

	// boss不需要显示战斗状态
	if (!pChapterCfg->byIsBoss)
		pMap->dwStartBattleTime = time(NULL);
		//pMap->bIsInBattle = true;

	// 记录正在挑战的怪物
	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	tPlayerAttackCityData.dwBattleTargetId = dwTargetId;

	// 广播怪物数据变动
	MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE msg;
	msg.m_tMonster = *pMap;
	SendToAll(&msg, pPlayer->GetServerId());

	return true;
}

// 战斗结束
bool CAttackCityManager::OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum, DWORD dwHPRemainRate)
{
	CFubenManager *pFBMgr = pPlayer->GetFubenManager();

	DWORD dwTargetId = pFBMgr->GetTargetId();

	// 取怪物数据
 	AttackCityMap *pMap = GetAttackCityMap(pPlayer->GetServerId(), dwTargetId);
 	if (!pMap)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]怪物不存在或者已死亡[target_id:%d]",__FUNCTION__, dwTargetId);
 		return false;
 	}

	const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(pMap->wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城波配置[wave:%d]", __FUNCTION__, pMap->wChapter);
		return false;
	}

	// for test
// 	if (pChapterCfg->byIsBoss)
// 		MAPLOG(ERROR_LEVEL, "[%s]攻击攻城BOSS结束上报[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());

 	// 怪物是否正在战斗中(只有小怪需要才会这样) 
	if (!pChapterCfg->byIsBoss && !pMap->dwStartBattleTime)
 	{
 		MAPLOG(ERROR_LEVEL, "[%s]怪物没在战斗中...[target_id:%d]",__FUNCTION__, dwTargetId);
 		return false;
 	}

	//pMap->bIsInBattle = false;
	pMap->dwStartBattleTime = 0;

	// 为了配合客户端结算表现
	pFBMgr->SetParam(2, pMap->wChapter);

	pFBMgr->SetMapId(pMap->dwMapId);

	// 如果还没挂掉再进去
	if (!IsMonsterDead(pMap->vMonsterList))
	{
		// 更新伤害
		UpdateMonsterDamage(pMap->vMonsterList, pMonsterList, byMonsterNum);

		PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
		tPlayerAttackCityData.dwBattleOverTime = time(NULL);
		tPlayerAttackCityData.wReliveCD = 0;
		tPlayerAttackCityData.wWeakCD = 0;
		tPlayerAttackCityData.dwBattleTargetId = 0;

		// 发放挑战奖励
		pPlayer->ModifyPropertyList(pChapterCfg->vBattleMoneyList, TRUE, FROM_ATTACK_CITY_KILL_REWARD);

		// 广播怪物数据变动
		MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE msg;
		msg.m_tMonster = *pMap;
		SendToAll(&msg, pPlayer->GetServerId());

		ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());

		bool bIsKiller = false;

		// 检测是否已死亡
		if (IsMonsterDead(pMap->vMonsterList))
		{
			bIsKiller = true;

			// 发放击杀奖励
			pPlayer->AddObjectList(pChapterCfg->vKillRewardItemList, TRUE, FROM_ATTACK_CITY_KILL_REWARD);

			// 是否当前所有的怪都已死亡
			if (IsAllMonsterDead(pPlayer->GetServerId()))
				OnAllMonsterDead(pPlayer->GetServerId(), pMap->wChapter, pPlayer);

			// 如果是小怪则进入虚弱状态
			if (!pChapterCfg->byIsBoss)
				tPlayerAttackCityData.wWeakCD = g_pCfgMgr->GetAttackCityWeakCD(dwHPRemainRate);

			// 标记已死亡
			pMap->bIsDead = true;

			pFBMgr->SetIsMeKill(true);

			MAPLOG(ERROR_LEVEL, "[%s]Boss击杀者[role_name:%s][monster_id:%d]", 
				__FUNCTION__, pPlayer->GetUTF8RoleName(), pMap->dwUID);
		}
		else
		{
			tPlayerAttackCityData.wReliveCD = g_GetConfigValue(167);
		}

		SendPlayerAttackCityData(pPlayer);

		// 计算获得的积分
		DWORD dwScore = CalculateScore(pChapterCfg->dwBattleScore, pChapterCfg->dwKillScore, tPlayerAttackCityData.byEncourageTimes, bIsKiller);
		tPlayerAttackCityData.dwScore += dwScore;

		// 处理排行榜
		WORD wOldRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_ATTACK_CITY);
		CRankManager::Instance()->AddRoleRankData(RANK_ATTACK_CITY, pPlayer, tPlayerAttackCityData.dwScore);
		WORD wNewRank = CRankManager::Instance()->GetRoleRank(pPlayer->GetRoleId(), pPlayer->GetServerId(), RANK_ATTACK_CITY);

		// 前3名排名有变化
		if (wOldRank != wNewRank && wNewRank <= 3)
			SendTopRankList(pPlayer, true);

		// 如果攻击者排名有变化 或者 击杀的是BOSS, 则需要重新同步每个人的排名
		if (wOldRank != wNewRank || (bIsKiller && pChapterCfg->byIsBoss))
		{
			// 从新同步每个的排名
			SynRankToAll(pPlayer->GetServerId());
		}
		// 否则只同步攻击者的积分
		else
		{
			// 同步积分
			const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_ATTACK_CITY, pPlayer->GetRoleId());
			if (pRankData)
			{
				MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE msg;
				msg.m_wRank = pRankData->wRank;
				msg.m_dwScore = pRankData->dwValue;
				pPlayer->SendPacket(&msg);
			}
		}

		// 如果BOSS关卡
		if (pChapterCfg->byIsBoss)
			RemoveBattleBossPlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());
	}

	return true;
}

// 击杀当前所有怪物(测试使用)
void CAttackCityManager::OnKillCurAllMonster(DWORD dwServerId)
{
	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tChapterList.begin();
	for (; iter != tChapterList.end(); ++iter)
	{
		for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
		{
			AttackCityMap &tMap = iter->second.vMapList[i];
			// 只处理没在战斗中的,还没挂掉的
			if (!tMap.dwStartBattleTime && !tMap.bIsDead)
			{
				for (size_t k = 0; k < tMap.vMonsterList.size(); ++k)
				{
					MonsterData &tMonster = tMap.vMonsterList[k];
					tMonster.dwDamage = tMonster.dwTotalHP;
				}
				tMap.bIsDead = true;
			}
		}
	}
}

// 活动开始
void CAttackCityManager::OnActivityStart()
{
	// 记录提前进入的玩家
	vector<Player*> vPlayerList;
	map<DWORD, ServerAttackCityData>::iterator svr_iter = m_SvrAttackCityDataList.begin();
	for (; svr_iter != m_SvrAttackCityDataList.end(); ++svr_iter)
	{
		map<DWORD, Player*>::iterator player_iter = svr_iter->second.mPlayerList.begin();
		for (; player_iter != svr_iter->second.mPlayerList.end(); ++player_iter)
		{
			vPlayerList.push_back(player_iter->second);
		}
	}

	m_byActivityStatus = 1;
	m_PlayerAttackCityDataList.clear();
	m_LockBoxPlayerList.clear();
	m_dwSeriesId = 1;
	m_dwProcessRefreshBoxTimer = 0;
	m_dwRefreshTreasureBoxCD = g_GetConfigValue(169);
	m_dwRefreshMonsterCD = g_GetConfigValue(170);
	m_SvrAttackCityDataList.clear();

 	const map<DWORD, ServerInfo> &tServerList = g_MapServer.GetServerList();
 	map<DWORD, ServerInfo>::const_iterator iter = tServerList.begin();
 	for (; iter != tServerList.end(); ++iter)
 	{
		DWORD dwServerId = iter->second.dwServerId;
		ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);

		// 重置章节(波数) todo::此处玩法再确认一哈
		const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter);
		if (!pChapterCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到攻城章节配置数据[chapter:%d]", __FUNCTION__, tGlobalData.wAttackCityChapter);
			return;
		}
		tGlobalData.wAttackCityChapter = pChapterCfg->wResetInitChapter;
		
		ServerAttackCityData tAttackCityData;
		tAttackCityData.dwServerId = dwServerId;
		tAttackCityData.dwRefreshMonsterTime = 0;
		tAttackCityData.dwNO1RoleId = tGlobalData.dwAttackCityRankNO1RoleId;
		tAttackCityData.wFromChapter = tGlobalData.wAttackCityChapter;
		m_SvrAttackCityDataList.insert(make_pair(dwServerId, tAttackCityData));

		// 重置排行榜数据
		CRankManager::Instance()->ResetRoleRank(dwServerId, RANK_ATTACK_CITY);

		// 活动广播
		MSG_S2C_ATTACK_CITY_ACTIVITY_START_NOTICE msg;
		SendToAll(&msg, dwServerId);
 	}

	// 提前进入的玩家重新进入
	for (size_t i = 0; i < vPlayerList.size(); ++i)
	{
		OnNetAttackCityEnter(vPlayerList[i]);
	}

	MAPLOG(ERROR_LEVEL, "[%s]怪物攻城活动已开始...", __FUNCTION__);
}

// 活动中
void CAttackCityManager::OnActivityIng()
{
	DWORD dwCurTime = time(NULL);
	if (dwCurTime >= m_dwProcessRefreshBoxTimer + m_dwRefreshTreasureBoxCD)
	{
		CreateTreasureBox();
		m_dwProcessRefreshBoxTimer = dwCurTime;
	}

	// 攻城BOSS死亡后刷怪时间(秒)
	DWORD dwAfterBossDeadRefreshMonsterCD = g_GetConfigValue(171);

	map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.begin();
	for (; iter != m_SvrAttackCityDataList.end(); ++iter)
	{
		ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(iter->second.dwServerId);
		const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter - 1);
		DWORD dwRefreshMonsterCD = pChapterCfg && pChapterCfg->byIsBoss ? dwAfterBossDeadRefreshMonsterCD : m_dwRefreshMonsterCD;
		if (dwCurTime >= iter->second.dwRefreshMonsterTime + dwRefreshMonsterCD)
			CreateMonster(iter->second.dwServerId);
	}
}

// 活动结束
void CAttackCityManager::OnActivityOver()
{
	m_byActivityStatus = 2;
	m_dwActivityOverTime = time(NULL);

	// 活动广播
	map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.begin();
	for (; iter != m_SvrAttackCityDataList.end(); ++iter)
	{
		MSG_S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE msg;
		SendToAll(&msg, iter->second.dwServerId);
	}

	MAPLOG(ERROR_LEVEL, "[%s]怪物攻城活动已结束...", __FUNCTION__);
}

// 发奖励
void CAttackCityManager::OnReward()
{
	// 发奖励
	if (m_dwActivityOverTime && time(NULL) >= m_dwActivityOverTime + g_GetConfigValue(192))
	{
		m_byActivityStatus = 0;
		m_dwActivityOverTime = 0;

		const AttackCityRankReward_Config *pRewardCfg = NULL;

		map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.begin();
		for (; iter != m_SvrAttackCityDataList.end(); ++iter)
		{
			const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(iter->second.dwServerId, RANK_ATTACK_CITY);
			if (!pRankList)
				continue;

			for (size_t i = 0; i < pRankList->size(); ++i)
			{
				const RoleRankData &tRankData = pRankList->at(i);
				pRewardCfg = g_pCfgMgr->GetAttackCityRankReward(tRankData.wRank);
				if (pRewardCfg)
				{
					// 发奖励
					RewardData tRewardData;
					tRewardData.wInfoId = 13;
					tRewardData.dwTime = time(NULL);
					tRewardData.dwValue1 = tRankData.wRank;
					CRewardCenterManager::AddReward(iter->second.dwServerId, tRankData.dwRoleId, tRewardData);
					
					MAPLOG(ERROR_LEVEL, "[%s]怪物攻城排行发放奖励[role_id:%d,rank:%d]",
						__FUNCTION__, tRankData.dwRoleId, tRankData.wRank);
				}

				// 记录排名第一的玩家
				if (0 == i)
				{
					ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(iter->second.dwServerId);
					tGlobalData.dwAttackCityRankNO1RoleId = tRankData.dwRoleId;
					g_MapServer.SaveGolbalData(iter->second.dwServerId, GOLBAL_ATTCK_CITY_RANK_NO1_ROLE_ID, tRankData.dwRoleId);
				}
			}

			// 重置排行榜数据
			CRankManager::Instance()->ResetRoleRank(iter->second.dwServerId, RANK_ATTACK_CITY);

			// 清怪物
			iter->second.mWaveDataList.clear();

			// 清宝箱
			iter->second.mTreasureBoxList.clear();
		}
		MAPLOG(ERROR_LEVEL, "[%s]怪物攻城活动已发奖励...", __FUNCTION__);
	}
}

// 攻城数据请求
void CAttackCityManager::OnNetAttackCityData(Player *pPlayer)
{
	SendPlayerAttackCityData(pPlayer);
}

// 进入攻城请求
void CAttackCityManager::OnNetAttackCityEnter(Player *pPlayer)
{
	// 检测是否已在城里
	if (IsInCity(pPlayer->GetServerId(),pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENTER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已在城里[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// todo::检测是否活动期间,所在服务器是否有此活动

	MapPos tMapPost;
	if (!RandMapPos(tMapPost, 158))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENTER_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]随机地图坐标点失败[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	pPlayer->SetMapPos(tMapPost.wMapX, tMapPost.wMapY);

	//MAPLOG(ERROR_LEVEL, "随机坐标[role_id:%d,x:%d,y:%d]", pPlayer->GetRoleId(), tMapPost.nX, tMapPost.nY);

	// 广播给其他玩家,有人加入
	MSG_S2C_ATTACK_CITY_ENTER_NOTICE notice;
	pPlayer->FillCommonData(&notice.m_tPlayerData);
	notice.m_tMapPos = pPlayer->GetMapPos();
	SendToAll(&notice, pPlayer->GetServerId());

	map<DWORD, Player*> &tPlayerList = GetPlayerList(pPlayer->GetServerId());

	// 添加到玩家列表
	tPlayerList.insert(make_pair(pPlayer->GetRoleId(), pPlayer));

	// 主动推攻城数据
	SendPlayerAttackCityData(pPlayer);

	MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE rank_notice;
	rank_notice.m_wRank = 0;
	rank_notice.m_dwScore = 0;
	const RoleRankData *pRankData = CRankManager::Instance()->GetRoleRankDataEx(pPlayer->GetServerId(), RANK_ATTACK_CITY, pPlayer->GetRoleId());
	if (pRankData)
	{
		rank_notice.m_wRank = pRankData->wRank;
		rank_notice.m_dwScore = pRankData->dwValue;
	}
	else
	{
		PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
		rank_notice.m_dwScore = tPlayerAttackCityData.dwScore;
	}
	pPlayer->SendPacket(&rank_notice);

	// 响应
	MSG_S2C_ATTACK_CITY_ENTER_RESP resp;
	resp.m_wPlayerNum = 0;
	map<DWORD, Player*>::iterator iter = tPlayerList.begin();
	for (; iter != tPlayerList.end(); ++iter)
	{
		if (resp.m_wPlayerNum < MAX_ATTACK_CITY_PLAYER_NUM)
		{
			PlayerCommonData &tPlayerData = resp.m_PlayerList[resp.m_wPlayerNum];
			resp.m_PlayerMapPos[resp.m_wPlayerNum] = iter->second->GetMapPos();
			iter->second->FillCommonData(&tPlayerData);
			resp.m_wPlayerNum++;
		}

	}
	pPlayer->SendPacket(&resp);

	// 发送怪物列表
	SendMonsterList(pPlayer);

	// 发送宝箱列表
	SendTreasureBoxList(pPlayer);

	// 发送高排名列表
	SendTopRankList(pPlayer);
}

// 离开攻城请求
void CAttackCityManager::OnNetAttackCityLeave(Player *pPlayer)
{
	DoLeaveAttackCity(pPlayer);

	MSG_S2C_ATTACK_CITY_LEAVE_RESP resp;
	pPlayer->SendPacket(&resp);
}

// 行走请求
void CAttackCityManager::OnNetGoto(Player *pPlayer, MapPos tSrcPos, MapPos tDestPos)
{
	// 检测是否已在城里
	if (!IsInCity(pPlayer->GetServerId(),pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_GOTO_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在城里[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	pPlayer->SetMapPos(tDestPos.wMapX, tDestPos.wMapY);

	// 响应
	MSG_S2C_ATTACK_CITY_GOTO_RESP resp;
	resp.m_tSrcPos = tSrcPos;
	resp.m_tDestPos = tDestPos;
	pPlayer->SendPacket(&resp);

	// todo::随机是否需要同步(当人数到达一定数量时)
	map<DWORD, Player*> &playerList = GetPlayerList(pPlayer->GetServerId());
	if (playerList.size() >= 8)
	{
		int nRandValue = g_Rand(1, 100);
		if (nRandValue >= 20)
			return;
	}

	MSG_S2C_ATTACK_CITY_GOTO_NOTICE notice;
	notice.m_dwRoleId = pPlayer->GetRoleId();
	notice.m_tSrcPos = tSrcPos;
	notice.m_tDestPos = tDestPos;
	SendToAll(&notice, pPlayer->GetServerId());

// 	MAPLOG(ERROR_LEVEL, "行走坐标[role_id:%d,src_x:%d, src_y:%d, dest_x:%d, dest_y:%d]", 
// 		pPlayer->GetRoleId(), tSrcPos.nX, tSrcPos.nY, tDestPos.nX, tDestPos.nY);
}

// 领取Boss奖励
void CAttackCityManager::OnNetTakeBossReward(Player *pPlayer, WORD wChapter)
{
	const AttackCityBossReward_Config *RewardCfg = g_pCfgMgr->GetAttackCityBossReward(wChapter);
	if (!RewardCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城BOSS奖励配置[chapter:%d]", __FUNCTION__, wChapter);
		return;
	}

	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());

	// 检测是否已领取过
	for (size_t i = 0; i < tPlayerAttackCityData.vTakeBossRewardRecordList.size(); ++i)
	{
		if (tPlayerAttackCityData.vTakeBossRewardRecordList[i] == wChapter)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ);
			MAPLOG(ERROR_LEVEL, "[%s]攻城Boss奖励已领取过[chapter:%d]", __FUNCTION__, wChapter);
			return;
		}
	}

	// 添加奖励
	pPlayer->AddObjectList(RewardCfg->vRewardItemList, TRUE, FROM_ATTACK_CITY_BOSS_REWARD);

	// 领取记录
	tPlayerAttackCityData.vTakeBossRewardRecordList.push_back(wChapter);

	SendPlayerAttackCityData(pPlayer);

	MSG_S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP msg;
	msg.m_wChapter = wChapter;
	pPlayer->SendPacket(&msg);
}

// 鼓舞
void CAttackCityManager::OnNetEncourage(Player *pPlayer)
{
	// 是否在城里
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENCOURAGE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在城里[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	PlayerAttackCityData &tAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	
	WORD wEncourageTimes = tAttackCityData.byEncourageTimes + 1;
	const AttackCityEncourage_Config *pEncourageCfg = g_pCfgMgr->GetAttackCityEncourage(wEncourageTimes);
	if (!pEncourageCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENCOURAGE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城鼓舞配置[times:%d]", __FUNCTION__, wEncourageTimes);
		return;
	}

	// 检测消耗
	if (pPlayer->GetProperty(pEncourageCfg->tCostData.wType) < pEncourageCfg->tCostData.nValue)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_ATTACK_CITY_ENCOURAGE_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]攻城鼓舞消耗不足[times:%d]", __FUNCTION__, wEncourageTimes);
		return;
	}

	tAttackCityData.byEncourageTimes++;
	SendPlayerAttackCityData(pPlayer);

	// 处理消耗
	pPlayer->ModifyProperty(pEncourageCfg->tCostData.wType, -pEncourageCfg->tCostData.nValue, TRUE, FROM_ATTACK_CITY_ENCOURAGE);

	MSG_S2C_ATTACK_CITY_ENCOURAGE_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 宝箱锁定
void CAttackCityManager::OnNetTreasureBoxLock(Player *pPlayer, DWORD dwTreasureBoxUId)
{
	// 是否在城里
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TREASURE_BOX_LOCK_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在城里[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测当前是否已在锁住其他宝箱
	if (IsLockBox(pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, C2S_TREASURE_BOX_LOCK_REQ);
		MAPLOG(ERROR_LEVEL, "[%s]玩家已经锁住了其他宝箱[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_TREASURE_BOX_LOCK_RESP msg;
	msg.m_dwUID = dwTreasureBoxUId;

	// 宝箱是否存在
	TreasureBox *pTreasureBox = GetTreasureBox(pPlayer->GetServerId(), dwTreasureBoxUId);
	if (!pTreasureBox)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]宝箱不存在或者已被拾取[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	DWORD dwCurTime = time(NULL);

	// 检测宝想是否已被锁住(延后1秒)
	if (pTreasureBox->dwLockPlayerId && dwCurTime <= pTreasureBox->dwLockTime + g_GetConfigValue(166) + 1)
	{
		msg.m_dwUID = 0;
		pPlayer->SendPacket(&msg);
		MAPLOG(GUI_LEVEL, "[%s]宝箱已被锁住[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	pTreasureBox->dwLockPlayerId = pPlayer->GetRoleId();
	pTreasureBox->dwLockTime = dwCurTime;

	msg.m_dwUID = dwTreasureBoxUId;
	pPlayer->SendPacket(&msg);
}

// 宝箱拾取
void CAttackCityManager::OnNetTreasureBoxPickUp(Player *pPlayer, DWORD dwTreasureBoxUId)
{
	WORD wFromMsgId = C2S_TREASURE_BOX_PICK_UP_REQ;

	// 是否在城里
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在城里[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 宝箱是否存在
	TreasureBox *pTreasureBox = GetTreasureBox(pPlayer->GetServerId(), dwTreasureBoxUId);
	if (!pTreasureBox)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]宝箱不存在[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	// 检测锁住宝箱跟拾取的是否同一个玩家
	if (pTreasureBox->dwLockPlayerId != pPlayer->GetRoleId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]锁住宝箱跟拾取宝箱不是同一个玩家[box_uid:%d,lock_id:%d,pick_up_id:%d]",
			__FUNCTION__, dwTreasureBoxUId, pTreasureBox->dwLockPlayerId, pPlayer->GetRoleId());
		return;
	}

	// 检测CD时间
	if (time(NULL) < pTreasureBox->dwLockTime + g_GetConfigValue(166))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(GUI_LEVEL, "[%s]不能拾取,宝箱CD中...[uid:%d]", __FUNCTION__, dwTreasureBoxUId);
		return;
	}

	// 一定是获得使用的随机礼包
	vector<Item> vItemList;
	if (!pPlayer->GetItemManager()->UseItem(pTreasureBox->dwInfoId, 1, &vItemList))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]使用攻城拾取物品失败[ItemId:%d]", __FUNCTION__, pTreasureBox->dwInfoId);
		return;
	}

	// 删除锁宝箱状态
	RemoveLockBoxPlayer(pPlayer->GetRoleId());

	DWORD dwInfoId = pTreasureBox->dwInfoId;

	// 删除已被拾取的宝箱
	RemoveTreasureBox(pPlayer->GetServerId(), dwTreasureBoxUId);

	// 广播
	MSG_S2C_TREASURE_BOX_DISAPPEAR_NOTICE notice;
	notice.m_dwUID = dwTreasureBoxUId;
	SendToAll(&notice, pPlayer->GetServerId());

	// 响应
	MSG_S2C_TREASURE_BOX_PICK_UP_RESP resp;
	resp.m_dwUID = dwTreasureBoxUId;
	resp.m_dwInfoId = dwInfoId;
	resp.m_vItemList = vItemList;
	pPlayer->SendPacket(&resp);
}

// 复活请求
void CAttackCityManager::OnNetRelive(Player *pPlayer, BYTE byIsFreeRelive)
{
	WORD wFromMsgId = C2S_ATTACK_CITY_RELIVE_REQ;

	// 是否在城里
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在城里[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	PlayerAttackCityData &tAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());

	// 免费复活
	if (byIsFreeRelive)
	{
		// 检测时间
		if (time(NULL) < tAttackCityData.wReliveCD + g_GetConfigValue(167))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]攻城死亡复活还在CD中...[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
			return;
		}

		// 广播位置
		MSG_S2C_ATTACK_CITY_ENTER_NOTICE notice;
		pPlayer->FillCommonData(&notice.m_tPlayerData);
		if (!RandMapPos(notice.m_tMapPos, 158))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]随机地图坐标点失败[role_id:%d]", __FUNCTION__, pPlayer->GetRoleId());
			return;
		}
		SendToAll(&notice, pPlayer->GetServerId());
	}
	// 消耗复活
	else
	{
		// 检测消耗
		int nCostDiamond = g_GetConfigValue(168);
		if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW, wFromMsgId);
			MAPLOG(ERROR_LEVEL, "[%s]攻城死亡复活消耗的钻石不足[cost:%d]", __FUNCTION__, nCostDiamond);
			return;
		}

		// 处理消耗
		pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_ATTACK_CITY_RELIVE);
	}

	// 重置复活数据
	tAttackCityData.wReliveCD = 0;
	tAttackCityData.wWeakCD = 0;

	// 主动同步数据
	SendPlayerAttackCityData(pPlayer);

	// 响应
	MSG_S2C_ATTACK_CITY_RELIVE_RESP msg;
	msg.m_byIsFreeRelive = byIsFreeRelive;
	pPlayer->SendPacket(&msg);
}

// 是否在城里
bool CAttackCityManager::IsInCity(DWORD dwServerId, DWORD dwRoleId)
{
	map<DWORD, Player*> &tPlayerList = GetPlayerList(dwServerId);
	map<DWORD, Player*>::iterator iter = tPlayerList.find(dwRoleId);
	return iter != tPlayerList.end() ? true : false;
}

// 广播消息
void CAttackCityManager::SendToAll(NetMsgBody *pMsg, DWORD dwServerId)
{
	map<DWORD, Player*> &tPlayerList = GetPlayerList(dwServerId);
	map<DWORD, Player*>::iterator iter = tPlayerList.begin();
	for (; iter != tPlayerList.end(); ++iter)
	{
		iter->second->SendPacket(pMsg);
	}
}

// 随机地图坐标点
bool CAttackCityManager::RandMapPos(MapPos &tMapPos, WORD wKey)
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetCommonCfg(wKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城随机坐标配置[key:%d]", __FUNCTION__, wKey);
		return false;
	}

	if (4 == pServerCfg->vValueList.size())
	{
		tMapPos.wMapX = g_Rand(pServerCfg->GetListValue(0), pServerCfg->GetListValue(1));
		tMapPos.wMapY = g_Rand(pServerCfg->GetListValue(2), pServerCfg->GetListValue(3));
	}
	else if (2 == pServerCfg->vValueList.size())
	{
		tMapPos.wMapX = pServerCfg->GetListValue(0);
		tMapPos.wMapY = pServerCfg->GetListValue(1);
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]攻城随机坐标配置出错[key:%d]", __FUNCTION__, wKey);
	}

	return true;
}

// 创建怪物
void CAttackCityManager::CreateMonster()
{
	const map<DWORD, ServerInfo> &tServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = tServerList.begin();
	for (; iter != tServerList.end(); ++iter)
	{
		CreateMonster(iter->second.dwServerId);
	}
}

// 创建怪物
void CAttackCityManager::CreateMonster(DWORD dwServerId)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(dwServerId);
	tServerAttackCityData.dwRefreshMonsterTime = time(NULL);

	// todo::可能需要同步刷怪时间

	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
	const AttackCityChapter_Config *pWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter);
	if (!pWaveMonsterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城波数怪物[wave:%d]", __FUNCTION__, tGlobalData.wAttackCityChapter);
		return;
	}

	// 如果当前要创建的是boss关卡,则前面的小怪必需都要清完,才会出来
	if (pWaveMonsterCfg->byIsBoss && !IsAllMonsterDead(dwServerId))
		return;

	// 如是当前挑战的是BOSS,则必需要打死，才会刷下一波
	if (tGlobalData.wAttackCityChapter > 1)
	{
		const AttackCityChapter_Config *pPreWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter - 1);
		if (pPreWaveMonsterCfg->byIsBoss && !IsAllMonsterDead(dwServerId))
			return;
	}

	MSG_S2C_ATTACK_CITY_MONSTER_LIST_NOTICE monster_list_notice;
	monster_list_notice.m_wCurChapter = g_MapServer.GetServerGlobalData(dwServerId).wAttackCityChapter;
	monster_list_notice.m_wMonsterNum = 0;

	//MAPLOG(ERROR_LEVEL, "[%s]当前攻城章节[chapter:%d]", __FUNCTION__, monster_list_notice.m_wCurChapter);

	double fBloodRate = 0;
	for (size_t i = 0; i < pWaveMonsterCfg->vMonsterProList.size(); ++i)
	{
		const Property &tPro = pWaveMonsterCfg->vMonsterProList[i];
		if (tPro.wType == HERO_PRO_BLOOD_RATE)
		{
			fBloodRate = tPro.nValue;
			break;
		}
	}

	// 创建怪物
	AttackCityChapter tChapterData;
	tChapterData.wChapter = tGlobalData.wAttackCityChapter;
	AttackCityMap tMap;
	tMap.bIsDead = false;
	tMap.dwStartBattleTime = 0;
	tMap.wChapter = tGlobalData.wAttackCityChapter;
	WORD wRandPosKey = pWaveMonsterCfg->byIsBoss ? 163 : 165;
	for (WORD i = 0; i < pWaveMonsterCfg->wRefreshMonsterNum; ++i)
	{
// 		int nIndex = g_Rand(0, pWaveMonsterCfg->vMapList.size() - 1);
// 		tMap.dwMapId = pWaveMonsterCfg->vMapList[nIndex];

		// 随机关卡
		DWORD dwTotalRate = 0;
		for (size_t k = 0; k < pWaveMonsterCfg->vMapRateList.size();++k)
		{
			dwTotalRate += pWaveMonsterCfg->vMapRateList[k];
		}
		tMap.dwMapId = 0;
		DWORD dwRandValue = g_Rand(0, dwTotalRate);
		DWORD dwCurRate = 0;
		for (size_t k = 0; k < pWaveMonsterCfg->vMapRateList.size(); ++k)
		{
			dwCurRate += pWaveMonsterCfg->vMapRateList[k];
			if (dwRandValue <= dwCurRate && k < pWaveMonsterCfg->vMapList.size())
			{
				tMap.dwMapId = pWaveMonsterCfg->vMapList[k];
				break;
			}
		}

		if (!RandMapPos(tMap.tMapPos, wRandPosKey))
		{
			MAPLOG(ERROR_LEVEL, "[%s]随机攻城怪物坐失败[wave:%d,mapid:%d]", __FUNCTION__, tChapterData.wChapter, tMap.dwMapId);
			return;
		}
		tMap.vMonsterList.clear();
		if (!g_pCfgMgr->GetMapMonsterList(tMap.dwMapId, &tMap.vMonsterList))
		{
			MAPLOG(ERROR_LEVEL, "[%s]取攻城怪物列表失败[MapId:%d]!", __FUNCTION__, tMap.dwMapId);
			return;
		}
		// 影响属性
		for (size_t k = 0; k < tMap.vMonsterList.size(); k++)
		{
			//tMap.vMonsterList[k].dwTotalHP += nAddHPValue * PROPERTY_DECREASE_MULTIPLE;
			tMap.vMonsterList[k].dwTotalHP *= (1 + fBloodRate * PROPERTY_DECREASE_MULTIPLE);
		}

		tMap.dwUID = GetNewSeriesId();
		tChapterData.vMapList.push_back(tMap);

		monster_list_notice.m_arMonsterList[monster_list_notice.m_wMonsterNum++] = tMap;
	}


	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	tChapterList.insert(make_pair(tChapterData.wChapter, tChapterData));

	SendToAll(&monster_list_notice, dwServerId);

	// 指向下一波(准备刷出的)
	tGlobalData.wAttackCityChapter++;
	g_MapServer.SaveGolbalData(dwServerId, GOLBAL_ATTCK_CITY_CHAPTER, tGlobalData.wAttackCityChapter);
}

// 删除怪物
void CAttackCityManager::RemoveMonster(DWORD dwServerId, WORD wChapter, DWORD dwMonsterUId)
{
	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tChapterList.find(wChapter);
	if (iter != tChapterList.end())
	{
		vector<AttackCityMap>::iterator monster_iter = iter->second.vMapList.begin();
		for (; monster_iter != iter->second.vMapList.end(); ++monster_iter)
		{
			if (monster_iter->dwUID == dwMonsterUId)
			{
				iter->second.vMapList.erase(monster_iter);
				break;
			}
		}
	}
}

// 创建宝箱
void CAttackCityManager::CreateTreasureBox()
{
	const map<DWORD, ServerInfo> &tServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = tServerList.begin();
	for (; iter != tServerList.end(); ++iter)
	{
		CreateTreasureBox(iter->second.dwServerId);
	}
}

// 创建宝箱
void CAttackCityManager::CreateTreasureBox(DWORD dwServerId)
{
	// todo::检测此服务器是否有此活动

	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
	const AttackCityChapter_Config *pWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter);
	if (!pWaveMonsterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城波数怪物[wave:%d]", __FUNCTION__, tGlobalData.wAttackCityChapter);
		return;
	}

	MSG_S2C_TREASURE_BOX_LIST_NOTICE msg;
	msg.m_wTreasureBoxNum = 0;

	TreasureBox tBox;
	tBox.dwCreateTime = time(NULL);
	tBox.dwInfoId = pWaveMonsterCfg->dwRefreshItemId;
	tBox.dwLockPlayerId = 0;
	tBox.dwLockTime = 0;

	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);

	WORD wCreateNum = g_Rand(pWaveMonsterCfg->wRefreshItemNumMin, pWaveMonsterCfg->wRefreshItemNumMax);
	for (WORD i = 0; i < wCreateNum; ++i)
	{
		// 如果到达指定的数量则不再创建
		if (tAttackCityData.mTreasureBoxList.size() >= g_GetConfigValue(175))
			break;

		tBox.dwUID = GetNewSeriesId();
		RandMapPos(tBox.tMapPos, 165);
		if (AddTreasureBox(dwServerId, tBox))
		{
			if (msg.m_wTreasureBoxNum < MAX_ATTACK_CITY_TREASURE_BOX_NUM)
				msg.m_TreasureBoxList[msg.m_wTreasureBoxNum++] = tBox;
		}
	}

	if (msg.m_wTreasureBoxNum)
		SendToAll(&msg, dwServerId);
}

// 创建Boss宝箱
void CAttackCityManager::CreateBossBox(DWORD dwServerId, WORD wChapter)
{
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
	const AttackCityChapter_Config *pWaveMonsterCfg = g_pCfgMgr->GetAttackCityChapter(wChapter);
	if (!pWaveMonsterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城波数怪物[wave:%d]", __FUNCTION__, wChapter);
		return;
	}

	MSG_S2C_TREASURE_BOX_LIST_NOTICE msg;
	msg.m_wTreasureBoxNum = 0;

	TreasureBox tBox;
	tBox.dwCreateTime = time(NULL);
	tBox.dwInfoId = pWaveMonsterCfg->dwBossItemId;
	tBox.dwLockPlayerId = 0;
	tBox.dwLockTime = 0;

	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);

	WORD wCreateNum = g_Rand(pWaveMonsterCfg->wBossItemNumMin, pWaveMonsterCfg->wBossItemNumMax);
	for (WORD i = 0; i < wCreateNum; ++i)
	{
		// 如果到达指定的数量则不再创建 todo::boss刷的宝箱不受数量限制
// 		if (tAttackCityData.mTreasureBoxList.size() >= g_GetConfigValue(175))
// 			break;

		tBox.dwUID = GetNewSeriesId();
		RandMapPos(tBox.tMapPos, 164);
		if (AddTreasureBox(dwServerId, tBox))
		{
			if (msg.m_wTreasureBoxNum < MAX_ATTACK_CITY_TREASURE_BOX_NUM)
				msg.m_TreasureBoxList[msg.m_wTreasureBoxNum++] = tBox;
		}
	}

	if (msg.m_wTreasureBoxNum)
		SendToAll(&msg, dwServerId);
}

// 当前怪物是否已全部死亡
bool CAttackCityManager::IsAllMonsterDead(DWORD dwServerId)
{
	map<WORD, AttackCityChapter> &tChapterList = GetAttackCityChapterList(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tChapterList.begin();

	// 遍历波
	for (; iter != tChapterList.end(); ++iter)
	{
		// 遍历关卡
		for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
		{
			// 遍历怪物
			AttackCityMap &tMap = iter->second.vMapList[i];
			if (!IsMonsterDead(tMap))
				return false;
		}
	}

	return true;
}

// 是否怪物已死亡
bool CAttackCityManager::IsMonsterDead(const AttackCityMap &tMap)
{
	for (size_t i = 0; i < tMap.vMonsterList.size(); ++i)
	{
		const MonsterData &tMonster = tMap.vMonsterList[i];
		if (tMonster.dwDamage < tMonster.dwTotalHP)
			return false;
	}
	return true;
}

// 发送怪物列表
void CAttackCityManager::SendMonsterList(Player *pPlayer, WORD wChapter, bool bIsToAll)
{
	MSG_S2C_ATTACK_CITY_MONSTER_LIST_NOTICE msg;
	msg.m_wCurChapter = g_MapServer.GetServerGlobalData(pPlayer->GetServerId()).wAttackCityChapter - 1;
	msg.m_wCurChapter = max(msg.m_wCurChapter, 1);

	//MAPLOG(ERROR_LEVEL, "[%s]当前攻城章节[chapter:%d]", __FUNCTION__, msg.m_wCurChapter);


	msg.m_wMonsterNum = 0;

	// 如果是在活动中则发送怪物及宝箱列表
	if (1 == m_byActivityStatus)
	{
		map<WORD, AttackCityChapter> &tWaveDataList = GetAttackCityChapterList(pPlayer->GetServerId());
		if (wChapter)
		{
			map<WORD, AttackCityChapter>::iterator iter = tWaveDataList.find(wChapter);
			if (iter != tWaveDataList.end())
			{
				for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
				{
					if (msg.m_wMonsterNum < MAX_ATTACK_CITY_MONSTER_NUM)
					{
						AttackCityMap &tMonster = iter->second.vMapList[i];
						msg.m_arMonsterList[msg.m_wMonsterNum++] = tMonster;
					}
				}
			}
		}
		else
		{
			map<WORD, AttackCityChapter>::iterator iter = tWaveDataList.begin();
			for (; iter != tWaveDataList.end(); ++iter)
			{
				for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
				{
					AttackCityMap &tMonster = iter->second.vMapList[i];
					// 只发送没有死亡的,因为目标的逻辑是死亡的怪物没有删除
					if (/*!IsMonsterDead(tMonster)*/!tMonster.bIsDead)
					{
						if (msg.m_wMonsterNum < MAX_ATTACK_CITY_MONSTER_NUM)
							msg.m_arMonsterList[msg.m_wMonsterNum++] = tMonster;
					}
				}
			}
		}
	}

	if (bIsToAll)
		SendToAll(&msg, pPlayer->GetServerId());
	else
		pPlayer->SendPacket(&msg);
}

// 发送宝箱列表
void CAttackCityManager::SendTreasureBoxList(Player *pPlayer)
{
	// 如果是在活动中则发送怪物及宝箱列表
	if (1 == m_byActivityStatus)
	{
		MSG_S2C_TREASURE_BOX_LIST_NOTICE msg;
		msg.m_wTreasureBoxNum = 0;

		ServerAttackCityData &tAttackCityData = GetAttackCityData(pPlayer->GetServerId());
		map<DWORD, TreasureBox>::iterator iter = tAttackCityData.mTreasureBoxList.begin();
		for (; iter != tAttackCityData.mTreasureBoxList.end(); ++iter)
		{
			if (msg.m_wTreasureBoxNum >= MAX_ATTACK_CITY_TREASURE_BOX_NUM)
				break;

			msg.m_TreasureBoxList[msg.m_wTreasureBoxNum++] = iter->second;
		}

		pPlayer->SendPacket(&msg);
	}
}

// 发送玩家攻城数据
void CAttackCityManager::SendPlayerAttackCityData(Player *pPlayer)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	MSG_S2C_ATTACK_CITY_DATA_RESP msg;
	msg.m_AttackCityData = tPlayerAttackCityData;
	msg.m_wFromChapter = tServerAttackCityData.wFromChapter;
	msg.m_dwKillBossTime = 0;
	msg.m_dwNo1RoleHeadImgId = 0;
	memset(msg.m_szNo1RoleName, 0, sizeof(msg.m_szNo1RoleName));
	
	
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(pPlayer->GetServerId());
	if (tGlobalData.wAttackCityChapter)
	{
		const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(tGlobalData.wAttackCityChapter - 1);
		if (pChapterCfg && pChapterCfg->byIsBoss && IsAllMonsterDead(pPlayer->GetServerId()))
			msg.m_dwKillBossTime = tServerAttackCityData.dwRefreshMonsterTime;
	}

	if (tGlobalData.dwAttackCityRankNO1RoleId)
	{
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), tGlobalData.dwAttackCityRankNO1RoleId);
		if (pBaseData)
		{
			msg.m_dwNo1RoleHeadImgId = pBaseData->wHeadIcon;
			strcpy_s(msg.m_szNo1RoleName, sizeof(msg.m_szNo1RoleName), pBaseData->szRoleName);
		}
	}


	pPlayer->SendPacket(&msg);
}

// 发送高排名玩家列表
void CAttackCityManager::SendTopRankList(Player *pPlayer, bool bIsToAll)
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(pPlayer->GetServerId(), RANK_ATTACK_CITY);
	if (pRankList)
	{
		MSG_S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE msg;
		msg.m_wListNum = 0;
		for (size_t i = 0; i < pRankList->size(); ++i)
		{
			const RoleRankData &tRankData =	pRankList->at(i);
			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(pPlayer->GetServerId(), tRankData.dwRoleId, false);

			if (msg.m_wListNum < MAX_DEFAULT_RANK_MUM)
				msg.m_arRankList[msg.m_wListNum++] = *pBaseData;
		}

		if (bIsToAll)
			SendToAll(&msg, pPlayer->GetServerId());
		else
			pPlayer->SendPacket(&msg);
	}
}

// 给所有人都同步排名
void CAttackCityManager::SynRankToAll(DWORD dwServerId)
{
	const RoleRankList *pRankList = CRankManager::Instance()->GetServerRoleRankListEx(dwServerId, RANK_ATTACK_CITY);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城排行榜数据[serverid:%d]", __FUNCTION__, dwServerId);
		return;
	}

	ServerAttackCityData &tAttackCity = GetAttackCityData(dwServerId);
	RoleRankList::const_iterator rank_iter = pRankList->begin();
	for (; rank_iter != pRankList->end(); ++rank_iter)
	{
		map<DWORD, Player*>::iterator player_iter = tAttackCity.mPlayerList.find(rank_iter->dwRoleId);
		if (player_iter != tAttackCity.mPlayerList.end())
		{
			MSG_S2C_ATTACK_CITY_RANK_UPDATE_NOTICE msg;
			msg.m_wRank = rank_iter->wRank;
			msg.m_dwScore = rank_iter->dwValue;
			player_iter->second->SendPacket(&msg);
		}
	}
}

// 是否有红点
bool CAttackCityManager::IsHaveRedPoint(Player *pPlayer)
{
	// 检测是否在活动中
	if (1 != m_byActivityStatus)
		return false;


	// 取玩家攻城数据
	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(pPlayer->GetServerId());
	for (WORD wChapter = tServerAttackCityData.wFromChapter; wChapter < tGlobalData.wAttackCityChapter; ++wChapter)
	{
		// 检测此是否有BOSS奖励
		if (!g_pCfgMgr->GetAttackCityBossReward(wChapter))
			continue;


		bool bExist = false;
		vector<WORD>::iterator iter = tPlayerAttackCityData.vTakeBossRewardRecordList.begin();
		for (; iter != tPlayerAttackCityData.vTakeBossRewardRecordList.end(); ++iter)
		{
			if (wChapter == *iter)
			{
				bExist = true;
				break;
			}
		}

		// 还没领取
		if (!bExist)
			return true;
	}

	return false;
}

// 取玩家列表
map<DWORD, Player*>& CAttackCityManager::GetPlayerList(DWORD dwServerId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	return tAttackCityData.mPlayerList;
}

// 取波数据
map<WORD, AttackCityChapter>& CAttackCityManager::GetAttackCityChapterList(DWORD dwServerId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	return tAttackCityData.mWaveDataList;
}

// 取攻城数据
ServerAttackCityData& CAttackCityManager::GetAttackCityData(DWORD dwServerId)
{
	map<DWORD, ServerAttackCityData>::iterator iter = m_SvrAttackCityDataList.find(dwServerId);
	if (iter == m_SvrAttackCityDataList.end())
	{
		ServerAttackCityData tAttackCityData;
		tAttackCityData.dwServerId = dwServerId;
		tAttackCityData.dwRefreshMonsterTime = 0;
		tAttackCityData.dwKillBossTime = 0;
		tAttackCityData.dwNO1RoleId = 0;
		m_SvrAttackCityDataList.insert(make_pair(dwServerId, tAttackCityData));
		iter = m_SvrAttackCityDataList.find(dwServerId);
	}
	return iter->second;
}

// 取宝箱
TreasureBox* CAttackCityManager::GetTreasureBox(DWORD dwServerId, DWORD dwUId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	map<DWORD, TreasureBox>::iterator iter = tAttackCityData.mTreasureBoxList.find(dwUId);
	return iter != tAttackCityData.mTreasureBoxList.end() ? &iter->second : NULL;
}

// 取关卡数据
AttackCityMap* CAttackCityManager::GetAttackCityMap(DWORD dwServerId, DWORD dwMapUId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	map<WORD, AttackCityChapter>::iterator iter = tAttackCityData.mWaveDataList.begin();
	for (; iter != tAttackCityData.mWaveDataList.end(); ++iter)
	{
		for (size_t i = 0; i < iter->second.vMapList.size(); ++i)
		{
			if (iter->second.vMapList[i].dwUID == dwMapUId)
				return &iter->second.vMapList[i];
		}
	}
	return NULL;
}

// 取玩家攻城数据
PlayerAttackCityData& CAttackCityManager::GetPlayerAttackCityData(DWORD dwRoleId)
{
	map<DWORD, PlayerAttackCityData>::iterator iter = m_PlayerAttackCityDataList.find(dwRoleId);
	if (iter == m_PlayerAttackCityDataList.end())
	{
		PlayerAttackCityData tAttackCityData;
		memset(&tAttackCityData, 0, sizeof(tAttackCityData));
		m_PlayerAttackCityDataList.insert(make_pair(dwRoleId, tAttackCityData));
		iter = m_PlayerAttackCityDataList.find(dwRoleId);
	}
	return iter->second;
}

// 添加宝箱
bool CAttackCityManager::AddTreasureBox(DWORD dwServerId, const TreasureBox &tBox)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	// 如果数量到达上限时,则清掉早期的
 	if (tAttackCityData.mTreasureBoxList.size() >= MAX_ATTACK_CITY_TREASURE_BOX_NUM)
 	{
		MAPLOG(ERROR_LEVEL, "[%s]怪物攻城宝箱数量超过上限[serverid:%d]", __FUNCTION__, dwServerId);
 		return false;
 	}
	tAttackCityData.mTreasureBoxList.insert(make_pair(tBox.dwUID, tBox));
	return true;
}

// 删除宝箱
void CAttackCityManager::RemoveTreasureBox(DWORD dwServerId, DWORD dwUId)
{
	ServerAttackCityData &tAttackCityData = GetAttackCityData(dwServerId);
	map<DWORD, TreasureBox>::iterator iter = tAttackCityData.mTreasureBoxList.find(dwUId);
	if (iter != tAttackCityData.mTreasureBoxList.end())
	{
		tAttackCityData.mTreasureBoxList.erase(iter);
	}
}

// 是否在锁住宝箱中
bool CAttackCityManager::IsLockBox(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_LockBoxPlayerList.find(dwRoleId);
	return iter != m_LockBoxPlayerList.end() ? true : false;
}

// 添加锁住宝箱的玩家
void CAttackCityManager::AddLockBoxPlayer(DWORD dwRoleId)
{
	m_LockBoxPlayerList.insert(make_pair(dwRoleId, dwRoleId));
}

// 删除锁住宝箱的玩家
void CAttackCityManager::RemoveLockBoxPlayer(DWORD dwRoleId)
{
	map<DWORD, DWORD>::iterator iter = m_LockBoxPlayerList.find(dwRoleId);
	if (iter != m_LockBoxPlayerList.end())
	{
		m_LockBoxPlayerList.erase(iter);
	}
}

// 更新怪物伤害
void CAttackCityManager::UpdateMonsterDamage(vector<MonsterData> &vSrcMonsterList, MonsterData *pMonsterList, BYTE byMonsterNum)
{
	for (BYTE i = 0; i < byMonsterNum; ++i)
	{
		vector<MonsterData>::iterator iter = vSrcMonsterList.begin();
		for (; iter != vSrcMonsterList.end(); ++iter)
		{
			MonsterData &tMonster = pMonsterList[i];
			if (tMonster.byIndex == iter->byIndex)
			{
				iter->dwDamage += tMonster.dwDamage;

				// 伤害纠正
				if (iter->dwTotalHP && iter->dwDamage > iter->dwTotalHP)
					iter->dwDamage = iter->dwTotalHP;
			}
		}
	}
}

// 是否怪物死亡
bool CAttackCityManager::IsMonsterDead(vector<MonsterData> &vMonsterList)
{
	for (size_t i = 0; i < vMonsterList.size(); ++i)
	{
		if (vMonsterList[i].dwDamage < vMonsterList[i].dwTotalHP)
			return false;
	}
	return true;
}

// 处理离开攻城
void CAttackCityManager::DoLeaveAttackCity(Player *pPlayer)
{
	// 检测是否已在城里
	if (!IsInCity(pPlayer->GetServerId(), pPlayer->GetRoleId()))
		return;

	// 删除离开的玩家
	ServerAttackCityData &tAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	// 参战玩家列表
	map<DWORD, Player*>::iterator iter1 = tAttackCityData.mPlayerList.find(pPlayer->GetRoleId());
	if (iter1 != tAttackCityData.mPlayerList.end())
	{
		tAttackCityData.mPlayerList.erase(iter1);
	}

	// 正在战斗的玩家列表(boss关)
	RemoveBattleBossPlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());

	// 广播给其他人,有人退出
	MSG_S2C_ATTACK_CITY_LEAVE_NOTICE notice;
	notice.m_dwRoleId = pPlayer->GetRoleId();
	SendToAll(&notice, pPlayer->GetServerId());

	// 删除锁箱子的状态
	RemoveLockBoxPlayer(pPlayer->GetRoleId());


	PlayerAttackCityData &tPlayerAttackCityData = GetPlayerAttackCityData(pPlayer->GetRoleId());
	if (tPlayerAttackCityData.dwBattleTargetId)
	{
		// 去掉怪物挑战状态
		AttackCityMap *pMap = GetAttackCityMap(pPlayer->GetServerId(), tPlayerAttackCityData.dwBattleTargetId);
		if (pMap && pMap->dwStartBattleTime)
		{
			pMap->dwStartBattleTime = 0;
			MSG_S2C_ATTACK_CITY_MONSTER_DATA_NOTICE msg;
			msg.m_tMonster = *pMap;
			SendToAll(&msg, pPlayer->GetServerId());
		}

		// 清空当前挑战的目标ID
		tPlayerAttackCityData.dwBattleTargetId = 0;
	}
}

// 处理怪物死亡
bool CAttackCityManager::OnAllMonsterDead(DWORD dwServerId, WORD wChapter, Player *pKiller)
{
	const AttackCityChapter_Config *pChapterCfg = g_pCfgMgr->GetAttackCityChapter(wChapter);
	if (!pChapterCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到攻城波配置[wave:%d]", __FUNCTION__, wChapter);
		return false;
	}

	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(dwServerId);

	// 检测是BOSS
	if (pChapterCfg->byIsBoss)
	{
		// 处理正在挑战中的玩家结算
		map<DWORD, Player*>::iterator iter = tServerAttackCityData.mBattlePlayerList.begin();
		for (; iter != tServerAttackCityData.mBattlePlayerList.end(); ++iter)
		{
			// for test
			//MAPLOG(ERROR_LEVEL, "[%s]攻城BOSS被击杀开始发奖励[role_id:%d]", __FUNCTION__, iter->second->GetRoleId());

			// 击杀者不需要处理
			if (iter->second->GetRoleId() == pKiller->GetRoleId())
				continue;

			DWORD dwScore = pChapterCfg->dwBattleScore;

			PlayerAttackCityData &tOtherAttackCityData = GetPlayerAttackCityData(iter->second->GetRoleId());
			// 处理鼓舞获得的额外积分
			if (tOtherAttackCityData.byEncourageTimes)
			{
				const AttackCityEncourage_Config *pEncourageCfg = g_pCfgMgr->GetAttackCityEncourage(tOtherAttackCityData.byEncourageTimes);
				if (!pEncourageCfg)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到鼓舞配置[times:%d]", __FUNCTION__, tOtherAttackCityData.byEncourageTimes);
					return false;
				}
				dwScore *= (1 + pEncourageCfg->nScoreAddRate * PROPERTY_DECREASE_MULTIPLE);
			}
			tOtherAttackCityData.dwScore += dwScore;
			CRankManager::Instance()->AddRoleRankData(RANK_ATTACK_CITY, iter->second, tOtherAttackCityData.dwScore);
		}

		// Boss宝箱
		CreateBossBox(dwServerId, wChapter);

		// 把整个列表清空
		tServerAttackCityData.mBattlePlayerList.clear();

		// 处理广播
		CSystemNotice sys_notice(16, dwServerId);
		sys_notice.AddParam(wChapter);
		sys_notice.AddParam(pKiller->GetRoleName());
		sys_notice.SendMsg();

	}

	WORD wAttackCityChapter = g_MapServer.GetServerGlobalData(dwServerId).wAttackCityChapter;
	const AttackCityChapter_Config *pPreChapterCfg = g_pCfgMgr->GetAttackCityChapter(wAttackCityChapter - 1);
	if (pPreChapterCfg)
	{
		// 如果是BOSS,则过一段时间再刷怪
		if (pPreChapterCfg->byIsBoss)
		{
			tServerAttackCityData.dwRefreshMonsterTime = time(NULL);
			tServerAttackCityData.dwKillBossTime = time(NULL);
		}
		// 如果是小怪马上刷出下一波
		else
		{
			CreateMonster(dwServerId);
			tServerAttackCityData.dwKillBossTime = 0;
		}
	}

	return true;
}

// 计算积分
DWORD CAttackCityManager::CalculateScore(DWORD dwBattleScore, DWORD dwKillScore, WORD wEncourageTimes, bool bIsKiller)
{
	// 挑战积分
	DWORD dwScore = dwBattleScore;

	// 击杀积分
	if (bIsKiller)
		dwScore += dwKillScore;

	// 处理鼓舞获得的额外积分
	if (wEncourageTimes)
	{
		const AttackCityEncourage_Config *pEncourageCfg = g_pCfgMgr->GetAttackCityEncourage(wEncourageTimes);
		if (!pEncourageCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到鼓舞配置[times:%d]", __FUNCTION__, wEncourageTimes);
			return dwScore;
		}
		dwScore *= (1 + float(pEncourageCfg->nScoreAddRate) * PROPERTY_DECREASE_MULTIPLE);
	}

	return dwScore;
}

// 删除正在挑战BOSS的玩家
void CAttackCityManager::RemoveBattleBossPlayer(DWORD dwServerId, DWORD dwRoleId)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(dwServerId);
	map<DWORD, Player*>::iterator iter = tServerAttackCityData.mBattlePlayerList.find(dwRoleId);
	if (iter != tServerAttackCityData.mBattlePlayerList.end())
		tServerAttackCityData.mBattlePlayerList.erase(iter);
}

// 添加挑战BOSS的玩家
void CAttackCityManager::AddBattleBossPlayer(Player *pPlayer)
{
	ServerAttackCityData &tServerAttackCityData = GetAttackCityData(pPlayer->GetServerId());
	tServerAttackCityData.mBattlePlayerList.insert(make_pair(pPlayer->GetRoleId(), pPlayer));
}
