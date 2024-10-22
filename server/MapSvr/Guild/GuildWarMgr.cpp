#include "stdafx.h"
#include "GuildWarMgr.h"
#include "MsgDefine.h"
#include "..\Player.h"
#include "..\ConfigManager.h"
#include "ToolKit\Rand.h"
#include "..\Item\ItemManager.h"
#include "CommonLogic.h"
#include "..\PlayerBaseDataManager.h"
#include "..\Rank\RankManager.h"
#include "..\MapSvrQuery.h"
#include "..\MapServer.h"
#include "data_define.pb.h"
#include "GuildManager.h"
#include "..\Fuben\FubenManager.h"
#include "..\RewardCenter\RewardCenterManager.h"
#include "DBCDefine.h"



CGuildWarMgr::CGuildWarMgr()
{
	m_bIsLoadDataFromDB = false;
	m_dwSaveTimer = 0;
}

CGuildWarMgr::~CGuildWarMgr()
{

}

void CGuildWarMgr::Init()
{
	m_bIsLoadDataFromDB = false;

	// 高级星球宣战结束时间计时器(决定进攻者时间)
	const vector<GuildWarVSActivityTime_Config> *pDeclareWarOverTimeCfg = g_pCfgMgr->GetGuildWarVSActivityTime(3);
	if (pDeclareWarOverTimeCfg)
	{
		CDateTimers timer;
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pDeclareWarOverTimeCfg->begin();
		for (; iter != pDeclareWarOverTimeCfg->end(); ++iter)
		{
			int nWeekDay = iter->dwDecideAttackerTime * 0.0001;
			int nTime = iter->dwDecideAttackerTime % 10000;
			timer.AddTriggerTime(nTime);
			timer.SetWeekDay(nWeekDay);
			m_vHighStarDeclareWarOverTimer.push_back(timer);
		}
	}

	// 对战时间结束时间,决定获胜者时间
	const vector<GuildWarVSActivityTime_Config> *pVSOverTimeCfg = g_pCfgMgr->GetGuildWarVSActivityTime(2);
	if (pVSOverTimeCfg)
	{
		CDateTimers timer;
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pVSOverTimeCfg->begin();
		for (; iter != pVSOverTimeCfg->end(); ++iter)
		{
			int nWeekDay = iter->dwEndTime * 0.0001;
			int nTime = iter->dwEndTime % 10000;
			timer.AddTriggerTime(nTime);
			timer.SetWeekDay(nWeekDay);
			m_vHighStarActivityOverTimer.push_back(timer);
		}
	}

	// 积分星球活动结束时间
	const Server_Config *pSvrCfg = GetCommonCfg(4);
	if (pSvrCfg){
		m_LowStarActivityOverTimer.AddTriggerTime(pSvrCfg->dwValue);
	}
}

void CGuildWarMgr::Release()
{

}

void CGuildWarMgr::Update()
{
	if (!m_bIsLoadDataFromDB)
		return;

	// 积分星球活动结束时间
	if (m_LowStarActivityOverTimer.IsTrigger())
	{
		LowStarActivityOver();
		m_LowStarActivityOverTimer.SetCDOverTime();
	}

	// 高级星球宣战结束时间(决定进攻者时间)
	for (int i = 0; i < m_vHighStarDeclareWarOverTimer.size(); ++i)
	{
		CDateTimers &tTimer = m_vHighStarDeclareWarOverTimer[i];
		if (tTimer.IsTrigger())
		{
			HighStarDeclareWarTimeOver();
			tTimer.SetCDOverTime();
		}
	}

	// 高级星球活动结束时间
	for (int i = 0; i < m_vHighStarActivityOverTimer.size(); ++i)
	{
		CDateTimers &tTimer = m_vHighStarActivityOverTimer[i];
		if (tTimer.IsTrigger())
		{
			HighStarActivityOver();
			tTimer.SetCDOverTime();
		}
	}

	// 定时存储
	DWORD dwCurTime = time(NULL);
	if (dwCurTime >= m_dwSaveTimer + 60)
	{
		SaveDataToDB();
		m_dwSaveTimer = dwCurTime;
	}
}

void CGuildWarMgr::OnNewDay()
{
	// 重置每日挑战次数
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.begin();
	for (; iter != m_RoleGuildWarDataList.end(); ++iter){
		iter->second.wUsedLowStarBattleTimes = 0;
	}

	map<DWORD, map<DWORD, StarInfo>>::iterator iterRegion = m_StarInfoList.begin();
	for (; iterRegion != m_StarInfoList.end(); ++iterRegion)
	{
		map<DWORD, StarInfo>::iterator iterStarInfo = iterRegion->second.begin();
		for (; iterStarInfo != iterRegion->second.end(); ++iterStarInfo)
		{
			iterStarInfo->second.mRewardRecordList.clear();
		}
	}

	LowStarActivityStart();
}

// 请求加载数据
void CGuildWarMgr::LoadDataRequest()
{
	GuildWarData_Load* pQuery = GuildWarData_Load::ALLOC();
	pQuery->SetIndex(DB_GUILD_WAR_DATA_LOAD);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	g_MapServer.MapDBQuery(pQuery);
}

// 存储数据到数据库
void CGuildWarMgr::SaveDataToDB()
{
	UINT64 lCurTime = g_GetCurrentTime();

	PB::GuildWarData PBGuildWarData;

	DWORD dwStarNum = 0;
	map<DWORD, map<DWORD, StarInfo>>::iterator iterStarList = m_StarInfoList.begin();
	for (; iterStarList != m_StarInfoList.end(); ++iterStarList)
	{
		dwStarNum += iterStarList->second.size();
		map<DWORD, StarInfo>::iterator iterStar = iterStarList->second.begin();
		for (; iterStar != iterStarList->second.end(); ++iterStar)
		{
			StarInfo *pStarInfo = &iterStar->second;

			PB::StarInfo *pPBStar = PBGuildWarData.add_star_list();
			pPBStar->set_region_id(iterStarList->first);
			pPBStar->set_info_id(pStarInfo->dwInfoId);
			pPBStar->set_belong_guild_id(pStarInfo->dwDelongGuildId);
			pPBStar->set_attack_guild_id(pStarInfo->dwAttackGuildId);
			pPBStar->set_last_belong_guild_id(pStarInfo->dwLastDelongGuildId);

			// 奖励领取记录列表
			map<DWORD, DWORD>::iterator iterRewardRecord = pStarInfo->mRewardRecordList.begin();
			for (; iterRewardRecord != pStarInfo->mRewardRecordList.end(); ++iterRewardRecord){
				pPBStar->add_took_reward_record_list(iterRewardRecord->first);
			}

			// 宣战列表
			map<DWORD, DWORD>::iterator iterDeclareWar = pStarInfo->mDeclareWarList.begin();
			for (; iterDeclareWar != pStarInfo->mDeclareWarList.end(); ++iterDeclareWar){
				pPBStar->add_declare_war_list(iterDeclareWar->first);
			}

			// 积分列表
			map<DWORD, DWORD>::iterator iterScore = pStarInfo->mScoreList.begin();
			for (; iterScore != pStarInfo->mScoreList.end(); ++iterScore){
				PB::GuildWarScore *pPBScore = pPBStar->add_score_list();
				pPBScore->set_guild_id(iterScore->first);
				pPBScore->set_score(iterScore->second);
			}

			// 塔列表
			vector<StarTowerInfo>::iterator iterTower = pStarInfo->vTowerList.begin();
			for (; iterTower != pStarInfo->vTowerList.end(); ++iterTower){
				PB::StarTowerInfo *pPBTower = pPBStar->add_tower_list();
				pPBTower->set_tower_id(iterTower->wTowerId);
				pPBTower->set_durability(iterTower->dwDurability);
				pPBTower->set_monster_be_attack_times(iterTower->wMonsterBeAttackTimes);

				// 防守者列表
				vector<StarDefenderInfo>::iterator iterDefender = iterTower->vDefenderList.begin();
				for (; iterDefender != iterTower->vDefenderList.end(); ++iterDefender){
					PB::StarDefenderInfo *pPBDefender = pPBTower->add_defender_list();
					pPBDefender->set_role_id(iterDefender->dwRoleId);
					pPBDefender->set_be_attack_times(iterDefender->wBeAttackTimes);
				}
			}
		}
	}

	// 角色工会战数据
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.begin();
	for (; iter != m_RoleGuildWarDataList.end(); ++iter)
	{
		PB::RoleGuildWarData *pRoleGuildWarData = PBGuildWarData.add_role_guild_war_data_list();
		pRoleGuildWarData->set_role_id(iter->first);
		pRoleGuildWarData->set_buy_high_star_battle_times(iter->second.wBuyHighStarBattleTimes);
		pRoleGuildWarData->set_used_high_star_battle_times(iter->second.wUsedLowStarBattleTimes);
		pRoleGuildWarData->set_used_low_star_battle_times(iter->second.wUsedHighStarBattleTimes);
	}

	string strSaveData("");
	if (!PBGuildWarData.AppendToString(&strSaveData))
	{
		MAPLOG(ERROR_LEVEL, "[%s]系列化工会战数据失败", __FUNCTION__);
		return;
	}
	if (strSaveData.size() > MAX_GUILD_WAR_DATA_SIZE)
	{
		MAPLOG(ERROR_LEVEL, "[%s]工会战存储数据过大[size:%d]", __FUNCTION__, strSaveData.size());
		return;
	}

	GuildWarData_Save* pQuery = GuildWarData_Save::ALLOC();
	pQuery->SetIndex(DB_GUILD_WAR_DATA_SAVE);
	pQuery->SetThreadIndex(DB_THREAD_GUILD);
	pQuery->SetJoinRoleNum(m_RoleGuildWarDataList.size());
	pQuery->SetStarNum(dwStarNum);
	pQuery->SetDataBuff(strSaveData.c_str(), strSaveData.size());
	g_MapServer.MapDBQuery(pQuery);

	DWORD dwCostTime = g_GetCurrentTime() - lCurTime;
	if (dwCostTime){
		MAPLOG(ERROR_LEVEL, "[%s]系列化工会战数据消耗(%d)毫秒!", __FUNCTION__, dwCostTime);
	}
}

// 加载数据
void CGuildWarMgr::LoadDataFromDB(const char *pszData, int nSize)
{
	m_RoleGuildWarDataList.clear();
	m_StarInfoList.clear();

	PB::GuildWarData PBGuildWarData;
	if (!PBGuildWarData.ParseFromArray(pszData, nSize))
	{
		MAPLOG(ERROR_LEVEL, "[%s]反系列化工会战数据出错!", __FUNCTION__);
		return;
	}
	
	// 角色工会战数据
	//RoleGuildWarData tRoleGuildWarData;
	for (int i = 0; i < PBGuildWarData.role_guild_war_data_list_size(); ++i)
	{
		const ::PB::RoleGuildWarData &tPBRoleGuildWarData = PBGuildWarData.role_guild_war_data_list(i);
		RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(tPBRoleGuildWarData.role_id());
		tRoleGuildWarData.wBuyHighStarBattleTimes = tPBRoleGuildWarData.buy_high_star_battle_times();
		tRoleGuildWarData.wUsedLowStarBattleTimes = tPBRoleGuildWarData.used_low_star_battle_times();
		tRoleGuildWarData.wUsedHighStarBattleTimes = tPBRoleGuildWarData.used_high_star_battle_times();
	}

	// 星球列表
	for (int i = 0; i < PBGuildWarData.star_list_size(); ++i)
	{
		const ::PB::StarInfo &tPBStarInfo = PBGuildWarData.star_list(i);
		StarInfo tStarInfo;
		DWORD dwRegionId = tPBStarInfo.region_id();
		tStarInfo.dwInfoId = tPBStarInfo.info_id();
		tStarInfo.dwDelongGuildId = tPBStarInfo.belong_guild_id();
		tStarInfo.dwAttackGuildId = tPBStarInfo.attack_guild_id();
		tStarInfo.dwLastDelongGuildId = tPBStarInfo.last_belong_guild_id();

		// 奖励领取记录
		for (int k = 0; k < tPBStarInfo.took_reward_record_list_size(); ++k){
			DWORD dwRoleId = tPBStarInfo.took_reward_record_list(k);
			tStarInfo.mRewardRecordList.insert(make_pair(dwRoleId, dwRoleId));
		}

		// 积分列表
		for (int k = 0; k < tPBStarInfo.score_list_size(); ++k){
			const PB::GuildWarScore &tPBScoreData = tPBStarInfo.score_list(k);
			tStarInfo.mScoreList.insert(make_pair(tPBScoreData.guild_id(), tPBScoreData.score()));
		}

		// 宣战列表
		for (int k = 0; k < tPBStarInfo.declare_war_list_size(); ++k){
			DWORD dwGuildId = tPBStarInfo.declare_war_list(k);
			tStarInfo.mDeclareWarList.insert(make_pair(dwGuildId, dwGuildId));
		}

		// 塔列表
		for (int k = 0; k < tPBStarInfo.tower_list_size(); ++k){
			const PB::StarTowerInfo &tPBStarTower = tPBStarInfo.tower_list(k);
			StarTowerInfo tTower;
			tTower.wTowerId = tPBStarTower.tower_id();
			tTower.dwDurability = tPBStarTower.durability();
			tTower.wMonsterBeAttackTimes = tPBStarTower.monster_be_attack_times();
			for (int m = 0; m < tPBStarTower.defender_list_size(); ++m){
				const PB::StarDefenderInfo &tPBDefender = tPBStarTower.defender_list(m);
				StarDefenderInfo tDefender;
				tDefender.dwRoleId = tPBDefender.role_id();
				tDefender.wBeAttackTimes = tPBDefender.be_attack_times();
				tTower.vDefenderList.push_back(tDefender);
			}
			tStarInfo.vTowerList.push_back(tTower);
		}

		AddStarInfo(dwRegionId, tStarInfo);

		if (tStarInfo.dwDelongGuildId){
			AddOccupyStar(tStarInfo.dwDelongGuildId, tStarInfo.dwInfoId);
		}
	}


	m_bIsLoadDataFromDB = true;
	m_dwSaveTimer = time(NULL);
}

// 是否有红点
bool CGuildWarMgr::IsHaveRedPoint(Player *pPlayer)
{
	return true;
}

// 占领星球
void CGuildWarMgr::OnOccupyStarForGM(Player *pPlayer, WORD wStarId)
{
	// 检测是否有工会
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会里[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	// 检测是否有此星球
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
	if (!pStarCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]麻蛋,这是啥ID啊[wStarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	WORD wRegionId = GetRegionId(pPlayer->GetServerId());

	StarInfo &tStarInfo = GetStarInfoByRegionId(wRegionId, wStarId);

	// 当前有人占领,有人进攻,有人宣战,则不能获得
	if (tStarInfo.dwAttackGuildId || tStarInfo.dwAttackGuildId || tStarInfo.mScoreList.size() || tStarInfo.mDeclareWarList.size())
	{
		MAPLOG(ERROR_LEVEL, "[%s]当前有人占领或有人进攻或者有人宣战[wStarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	AddOccupyStar(dwGuildId, tStarInfo.dwInfoId);

	// 重置星球信息
	ResetStarInfo(tStarInfo, dwGuildId, true);
}

// 战斗开始(积分模式)
bool CGuildWarMgr::OnNetLowStarBattleStart(Player *pPlayer, DWORD dwStarId)
{
	// 检测是否活动时间
	if (!IsLowStarBattleTime())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]不是积分星球挑战时间[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// todo::检测需要的等级

	// 检测是否有工会
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会里,不能挑战星球[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// 取星球配置
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(dwStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// 取星球数据
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	// 检测挑战次数
	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	if (tRoleGuildWarData.wUsedLowStarBattleTimes >= GetConfigValue(1))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]低级星球挑战次数已达上限[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	pPlayer->GetFubenManager()->SetTargetId(dwStarId);

	return true;
}

// 战斗结束(积分模式)
bool CGuildWarMgr::OnNetLowStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate)
{
	// 检测是否有工会
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会里,不能挑战星球[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	DWORD dwStarId = pPlayer->GetFubenManager()->GetTargetId();
	if (!dwStarId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]正在挑战的目标星球ID为空[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// 取星球配置
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(dwStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// 取星球数据
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	DWORD dwScore = 0;

	// 计算积分
	if (byIsWin)
	{
		// 战胜获得积分=20+math.floor(剩余血量百分比*20)
		dwScore = 20 + floor(double(nRemainHPRate * 0.01) * 20);

		pPlayer->AddObjectList(pStarCfg->vWinnerRewardList, TRUE, FROM_GUILD_WAR_SCORE_STAR_BATTLE);
	}
	else
	{
		// 战败获得积分=20-math.floor(剩余血量百分比*10)
		dwScore = 20 - floor(double(nRemainHPRate * 0.01) * 10);

		pPlayer->AddObjectList(pStarCfg->vLoserRewardList, TRUE, FROM_GUILD_WAR_SCORE_STAR_BATTLE);
	}

	AddGuildScore(tStarInfo, pGuild->GetGuildId(), dwScore);

	pPlayer->GetFubenManager()->SetParam(4, dwScore);

	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	tRoleGuildWarData.wUsedLowStarBattleTimes++;

	return true;
}

// 战斗开始(星球战模式)
bool CGuildWarMgr::OnNetHighStarBattleStart(Player *pPlayer, DWORD dwStarId, WORD wTowerId, DWORD dwTargetId)
{
	// 检测当前是否战斗时间
	if (!CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当前不是战斗时间!", __FUNCTION__);
		return false;
	}

	// 检测是否有工会
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会里,不能挑战星球[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// 检测玩家可挑战次数
	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	if (tRoleGuildWarData.wUsedHighStarBattleTimes >= GetConfigValue(1) + tRoleGuildWarData.wBuyHighStarBattleTimes)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]高级星球挑战次数已达上限[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	// 取星球数据
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	// 检测是否获得进攻权
	if (tStarInfo.dwAttackGuildId != dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]没有获得进攻权[StarId:%d,AttackGuildId:%d]!", __FUNCTION__, dwStarId, tStarInfo.dwAttackGuildId);
		return false;
	}

	// 取星球配置
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(dwStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, dwStarId);
		return false;
	}

	// 取塔信息
	StarTowerInfo *pTowerInfo = GetTowerInfo(&tStarInfo, wTowerId);
	if (!pTowerInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]不是本星球的塔[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// 取塔配置
	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(wTowerId);
	if (!pTowerCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到塔的配置[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// 塔是否已破
	if (IsTowerBreak(tStarInfo.dwDelongGuildId, pTowerInfo))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]星球塔已倒下[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// 检测当前能否攻击基地
	if (pTowerCfg->wTowerType == STAR_TOWER_1 && !IsCanAttackBoss(&tStarInfo))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当前不能攻击基地[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	// 已经有人占领
	if (tStarInfo.dwDelongGuildId)
	{ ///// 挑战角色
		
		StarDefenderInfo *pDefender = GetDefenderInfo(pTowerInfo, dwTargetId);
		if (!pDefender)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]找不到防守者信息[StarId:%d,TowerId:%d,Defender:%d]!", 
				__FUNCTION__, dwStarId, wTowerId, dwTargetId);
			return false;
		}

		// 检测目标的防守次数
		if (pDefender->wBeAttackTimes >= pTowerCfg->wRoleDefenseTimes)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]防守者已趴下,莫奸尸！！！[StarId:%d,TowerId:%d,Defender:%d]!",
				__FUNCTION__, dwStarId, wTowerId, dwTargetId);
			return false;
		}

		// 发送防守者信息给客户端
		const PlayerBaseData *pTargetBaseData = g_GetPlayerBaseData(dwTargetId, false);
		if (pTargetBaseData)
		{
			pFBMgr->SetBattleTargetData(pTargetBaseData->vToBattleHeroList, pTargetBaseData->vSpriteList);

			// 设置战力
			pFBMgr->SetTargetFightingPower(pTargetBaseData->dwFightingPower);
		}
	}
	else
	{ //// 挑战怪物
		
		// todo::暂时不需要处理 pass
	}

	// 记录临时数据
	pFBMgr->SetParam(0, dwStarId);
	pFBMgr->SetParam(1, wTowerId);
	pFBMgr->SetParam(2, dwTargetId);

	return true;
}

// 战斗结束(星球战模式)
bool CGuildWarMgr::OnNetHighStarBattleOver(Player *pPlayer, BYTE byIsWin, int nRemainHPRate)
{
	// 数据合法性检测
	if (nRemainHPRate > 100)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]剩余血量百分比非法[RoleId:%d, RemainHPRate:%d]!", 
			__FUNCTION__, pPlayer->GetRoleId(), nRemainHPRate);
		return false;
	}

	CFubenManager *pFBMgr = pPlayer->GetFubenManager();
	CHECK_POINTER_RET(pFBMgr, false);

	DWORD dwStarId = pFBMgr->GetParam(0);
	WORD wTowerId = pFBMgr->GetParam(1);
	DWORD dwTargetId = pFBMgr->GetParam(2);

	// 检测是否有工会
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(pPlayer->GetRoleId());
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会里,不能挑战星球[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return false;
	}

	DWORD dwGuildId = pGuild->GetGuildId();

	// 取星球数据
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);

	// 检测是否获得进攻权
	if (tStarInfo.dwAttackGuildId != dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]没有获得进攻权[StarId:%d,AttackGuildId:%d]!", __FUNCTION__, dwStarId, tStarInfo.dwAttackGuildId);
		return false;
	}

	// 取塔信息
	StarTowerInfo *pTowerInfo = GetTowerInfo(&tStarInfo, wTowerId);
	if (!pTowerInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]不是本星球的塔[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	// 取塔配置
	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(wTowerId);
	if (!pTowerCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到塔的配置[StarId:%d,TowerId:%d]!", __FUNCTION__, dwStarId, wTowerId);
		return false;
	}

	bool bIsBreak = IsTowerBreak(tStarInfo.dwDelongGuildId, pTowerInfo);

	DWORD dwDurability = 0;
	if (byIsWin)
	{
		// 防止在玩家上报之前已被其他人干倒
		if (!bIsBreak)
		{
			// 已经有人占领
			if (tStarInfo.dwDelongGuildId)
			{ ///// 挑战角色
				StarDefenderInfo *pDefender = GetDefenderInfo(pTowerInfo, dwTargetId);
				if (pDefender)
					pDefender->wBeAttackTimes++;
			}
			else
			{ //// 挑战怪物
				pTowerInfo->wMonsterBeAttackTimes++;
			}
		}

		// 战胜耐久度扣除公式：50+math.floor(剩余血量百分比*50) floor:向下取整
		dwDurability = 50 + floor(double(nRemainHPRate * 0.01) * 50);

		// 个人挑战获胜奖励
		pPlayer->AddObjectList(pTowerCfg->vWinnerRewardList, TRUE, FROM_GUILD_WAR_VS_STAR_BATTLE);
	}
	else
	{
		// 战败耐久度扣除公式：50-math.floor(剩余血量百分比*40)	
		dwDurability = 50 - floor(double(nRemainHPRate * 0.01) * 40);

		// 个人挑战失败奖励
		pPlayer->AddObjectList(pTowerCfg->vLoserRewardList, TRUE, FROM_GUILD_WAR_VS_STAR_BATTLE);
	}

	// 更新耐久度
	pTowerInfo->dwDurability >= dwDurability ? pTowerInfo->dwDurability -= dwDurability : pTowerInfo->dwDurability = 0;

	MAPLOG(DEBUG_LEVEL, "[%s]耐久度[CurDurability:%d,ChangeDurability:%d]!", __FUNCTION__, pTowerInfo->dwDurability, dwDurability);

	// 工会获得活跃度
	//pGuild->AddActive(dwDurability);

	// 检测塔是否被干倒
	if (!bIsBreak && IsTowerBreak(tStarInfo.dwDelongGuildId, pTowerInfo))
	{
		// 每攻破一路防线后攻击方全部团员可获得额外奖励(通过奖励中心发放)
		BreakTowerReward(dwStarId, pTowerInfo, tStarInfo.dwAttackGuildId);

		// 如果是基地则占领星球, 对战结束
		if (pTowerCfg->wTowerType == STAR_TOWER_1)
		{
			DWORD dwOldDelongGuildId = tStarInfo.dwDelongGuildId;

			// 活动结束奖励
			HighStarActivtiyOverReward(&tStarInfo, true);

			RemoveOccupyStar(tStarInfo.dwDelongGuildId, tStarInfo.dwInfoId);
			AddOccupyStar(dwGuildId, tStarInfo.dwInfoId);

			// 重置星球信息
			ResetStarInfo(tStarInfo, dwGuildId, true);

			// 记录上次占领者(如果是抢人就记录工会ID,如果是抢怪就记录特殊ID)
			tStarInfo.dwLastDelongGuildId = dwOldDelongGuildId ? dwOldDelongGuildId : 999;

			MAPLOG(ERROR_LEVEL, "[%s]挑战获得星球塔[StarId:%d, OldDelongGuildId:%d, NewDelongGuildId:%d, RoleId:%d]!", 
				__FUNCTION__, tStarInfo.dwInfoId, dwOldDelongGuildId, dwGuildId, dwGuildId);
		}
	}

	pFBMgr->SetParam(4,dwDurability);

	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());
	tRoleGuildWarData.wUsedHighStarBattleTimes++;

	// 通知军团成员星球数据更新
	MSG_S2C_GUILD_WAR_STAR_UPDATE_NOTIFY msg;
	msg.m_wStarId = dwStarId;
	msg.m_wTowerId = wTowerId;
	pGuild->SendMsgToAllMember(&msg);

	// 处理个人积分排行榜 todo::暂时不知道公式
	// CRankManager::Instance()->AddRoleRankData(RANK_GUILD_WAR, pPlayer, 1);

	// todo::响应给其他军团成员

	return true;
}

// 请求工会战数据
void CGuildWarMgr::OnNetGuildWarData(Player *pPlayer)
{
	SendStarList(pPlayer);

	SendOccupyStarList(pPlayer);

	SendGuildWarRoleData(pPlayer);
}

// 请求本军团已占领的星球
void CGuildWarMgr::OnNetGuildOccupyStarList(Player *pPlayer)
{
	SendOccupyStarList(pPlayer);
}

// 请求本军团已宣战的星球列表
void CGuildWarMgr::OnNetGuildDeclareWarStarList(Player *pPlayer)
{
	SendDeclareWarStarList(pPlayer);
}

// 请求星球列表
void CGuildWarMgr::OnNetStarList(Player *pPlayer)
{
	SendStarList(pPlayer);
}

// 宣战
void CGuildWarMgr::OnNetDeclareWar(Player *pPlayer, DWORD dwStarId)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// 检测是否宣战时间
	if (!CheckHighStarActivityStatus(VSSTAR_ACTIVITY_DECLARE_WAR))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]当前不是宣战时间!", __FUNCTION__);
		return;
	}

	// 是否有工会
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能宣战[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// 是否会长，或者副会长
	BYTE byMemberPos = pGuild->GetMemberPos(dwRoleId);
	if (byMemberPos != GUILD_POS_CAPTAIN && byMemberPos != GUILD_POS_ASSIST)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有权限宣战[RoleId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	WORD wRegionId = GetRegionId(pPlayer->GetServerId());

	// 检测可宣战数量上限
	WORD wDeclareWarNum = GetAlreadyDeclareWarNum(wRegionId, pGuild->GetGuildId());
	if (wDeclareWarNum >= GetConfigValue(2))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]宣战星球数量已达上限[DeclareWarNum:%d]!", __FUNCTION__, wDeclareWarNum);
		return;
	}

	// 检测目标可接受宣战数量上限
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), dwStarId);
	if (tStarInfo.mDeclareWarList.size() >= GetConfigValue(3))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]目标星球可接受宣战数量已达上限[StarId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	// 检测目前已占领的数量
	WORD wOccupyStarNum = GetOccupyHighStarNum(pGuild->GetServerId(), pGuild->GetGuildId());
	if (wOccupyStarNum >= GetConfigValue(13))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]工会占领的星球已达上限,不能再宣战啦[StarId:%d,OccupyStarNum:%d]!", __FUNCTION__, dwRoleId, wOccupyStarNum);
		return;
	}

	// 检测是否自己的星球
	if (pGuild->GetGuildId() == tStarInfo.dwDelongGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]不能对自己占领的星球宣战[StarId:%d]!", __FUNCTION__, tStarInfo.dwInfoId);
		return;
	}

	// 检测是否重复宣战
	if (tStarInfo.mDeclareWarList.find(dwStarId) != tStarInfo.mDeclareWarList.end())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]重复对同一星球宣战[StarId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	tStarInfo.mDeclareWarList.insert(make_pair(pGuild->GetGuildId(), pGuild->GetGuildId()));

	// 响应
	MSG_S2C_GUILD_WAR_DECLARE_WAR_RESP msg;
	msg.m_wStarId = dwStarId;
	pPlayer->SendPacket(&msg);
}

// 布防
void CGuildWarMgr::OnNetOrganizeDefence(Player *pPlayer, WORD wStarId, WORD wTowerType, DWORD dwMemberId, BYTE byIsToBattle)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// 战斗中不能撤离
	if (CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]战斗中不能布防[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// 是否有工会
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能撤离[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// 是否会长，或者副会长
	BYTE byMemberPos = pGuild->GetMemberPos(dwRoleId);
	if (byMemberPos != GUILD_POS_CAPTAIN && byMemberPos != GUILD_POS_ASSIST)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有权限撤离[RoleId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	// 取星球配置
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	// 只有高级星球才能布防
	if (!IsHighStar(pStarCfg->wStarType))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]低级星球不能布防[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);

	// 检测是否所属的工会
	if (tStarInfo.dwDelongGuildId != pGuild->GetGuildId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]不是本工会所属的星球[GuildId:%d,StarId:%d]!", __FUNCTION__, pGuild->GetGuildId(), wStarId);
		return;
	}

	// 上阵
	if (byIsToBattle)
	{
		// 检测成员是否已防守了其他的塔
		if (IsInDefense(&tStarInfo, dwMemberId))
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]该玩家正在防守中[StarId:%d,RoleId:%d]!", __FUNCTION__, wStarId, dwMemberId);
			return;
		}

		// 取要防守的塔数据
		StarTowerInfo *pTowerInfo = GetTowerInfoByType(&tStarInfo, wTowerType);
		if (!pTowerInfo)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]找不到防守塔数据[StarId:%d,TowerType:%d]!", __FUNCTION__, wStarId, wTowerType);
			return;
		}

		// 取塔配置
		const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pTowerInfo->wTowerId);
		if (!pTowerCfg)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]找不到防守塔配置[StarId:%d,TowerId:%d]!", __FUNCTION__, wStarId, pTowerInfo->wTowerId);
			return;
		}

		// 检测防守人数上限
		if (pTowerInfo->vDefenderList.size() >= pTowerCfg->wDefenderNumMax)
		{
			pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]防守塔人数已达上限[StarId:%d,TowerId:%d,Persons:%d]!", 
				__FUNCTION__, wStarId, pTowerInfo->wTowerId, pTowerInfo->vDefenderList.size());
			return;
		}

		// 进入对应的塔防守
		StarDefenderInfo tDefender;
		tDefender.dwRoleId = dwMemberId;
		tDefender.wBeAttackTimes = 0;
		pTowerInfo->vDefenderList.push_back(tDefender);

// 		vector<StarTowerInfo>::iterator iter = tStarInfo.vTowerList.begin();
// 		for (; iter != tStarInfo.vTowerList.end(); ++iter)
// 		{
// 			if (GetTowerType(iter->wTowerId) == wTowerType)
// 			{
// 				StarDefenderInfo tDefender;
// 				tDefender.dwRoleId = dwMemberId;
// 				tDefender.wBeAttackTimes = 0;
// 				iter->vDefenderList.push_back(tDefender);
// 				break;
// 			}
// 		}
	}
	// 下阵
	else
	{
		// 如果是基地,则至少要保留一个人
		if (STAR_TOWER_1 == wTowerType)
		{
			WORD wCurDefenseNum = GetCurTowerDefenseNum(&tStarInfo, wTowerType);
			if (wCurDefenseNum <= 1)
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]基地防守至少要保留一个[StarId:%d,TowerType:%d,CurDefenseNum:%d]!",
					__FUNCTION__, wStarId, wTowerType, wCurDefenseNum);
				return;
			}
		}

		vector<StarTowerInfo>::iterator iter = tStarInfo.vTowerList.begin();
		for (; iter != tStarInfo.vTowerList.end(); ++iter)
		{
			if (GetTowerType(iter->wTowerId) == wTowerType)
			{
				for (size_t i = 0; i < iter->vDefenderList.size(); i++)
				{
					if (iter->vDefenderList[i].dwRoleId == dwMemberId)
					{
						// 当前位置被最后位置的覆盖,并拿掉最后位置的数据
						iter->vDefenderList[i] = iter->vDefenderList[iter->vDefenderList.size() - 1];
						iter->vDefenderList.pop_back();
						break;
					}
				}
				break;
			}
		}
	}

	MSG_S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP msg;
	msg.m_wStarId = wStarId;
	msg.m_wTowerType = wTowerType;
	msg.m_dwRoleId = dwMemberId;
	msg.m_byIsToBattle = byIsToBattle;
	pPlayer->SendPacket(&msg);
}

// 撤离
void CGuildWarMgr::OnNetEvacuate(Player *pPlayer, WORD wStarId)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// 战斗中不能撤离
	if (CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]战斗中不能撤离星球[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// 是否有工会
	CGuild *pGuild = CGuildManager::Instance()->GetGuildByRoleId(dwRoleId);
	if (!pGuild)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能撤离[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	// 是否会长，或者副会长
	BYTE byMemberPos = pGuild->GetMemberPos(dwRoleId);
	if (byMemberPos != GUILD_POS_CAPTAIN && byMemberPos != GUILD_POS_ASSIST)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有权限撤离[RoleId:%d,Pos:%d]!", __FUNCTION__, dwRoleId, byMemberPos);
		return;
	}

	// 取星球配置
	const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
	if (!pStarCfg)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	// 积分星球不能撤离
	if (pStarCfg->wStarType <= GUILD_WAR_STAR_BLUE)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到积分星球不能撤离[StarId:%d]!", __FUNCTION__, wStarId);
		return;
	}

	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);

	// 检测是否所属的工会
	if (tStarInfo.dwDelongGuildId != pGuild->GetGuildId())
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]不是本工会所属的星球[GuildId:%d,StarId:%d]!", __FUNCTION__, pGuild->GetGuildId(), wStarId);
		return;
	}

	RemoveOccupyStar(tStarInfo.dwDelongGuildId, tStarInfo.dwInfoId);

	// 归还星球
	ResetStarInfo(tStarInfo, 0, false);

	// 响应
	MSG_S2C_GUILD_WAR_EVACUATE_RESP msg;
	msg.m_wStarId = wStarId;
	pPlayer->SendPacket(&msg);
}

// 购买对抗模式挑战次数
void CGuildWarMgr::OnNetBuyHighStarBattleTimes(Player *pPlayer)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// 是否有工会
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(dwRoleId);
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能购买星球挑战次数[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	RoleGuildWarData &tRoleGuildWarData = GetRoleGuildWarData(pPlayer->GetRoleId());

	// 检测购买次数上限
	if (tRoleGuildWarData.wBuyHighStarBattleTimes >= GetConfigValue(12))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]购买高级星球挑战次数已达上限[RoleId:%d,AlreadyBuyTimes:%d]!", 
			__FUNCTION__, dwRoleId, tRoleGuildWarData.wBuyHighStarBattleTimes);
		return;
	}

	// 检测消耗
	int nCostDiamond = GetConfigValue(10);
	if (pPlayer->GetProperty(ROLE_PRO_DIAMOND) < nCostDiamond)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]购买高级星球挑战次数消耗钻石不足[CostDiamond:%d]!", __FUNCTION__, nCostDiamond);
		return;
	}

	// 处理消耗
	pPlayer->ModifyProperty(ROLE_PRO_DIAMOND, -nCostDiamond, TRUE, FROM_BUY_GUILD_WAR_BATTLE_TIMES);

	// 添加挑战次数
	tRoleGuildWarData.wBuyHighStarBattleTimes++;

	// 响应
	MSG_S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP msg;
	msg.m_wCurBuyTimes = tRoleGuildWarData.wBuyHighStarBattleTimes;
	pPlayer->SendPacket(&msg);
}

// 领取星球奖励
void CGuildWarMgr::OnNetTakeStarReward(Player *pPlayer, WORD wTakeType, WORD *pStarList, WORD wStarNum)
{
	DWORD dwRoleId = pPlayer->GetRoleId();

	// 是否有工会
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(dwRoleId);
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会,不能领取积分星球奖励[RoleId:%d]!", __FUNCTION__, dwRoleId);
		return;
	}

	for (WORD i = 0; i < wStarNum; ++i)
	{
		WORD wStarId = pStarList[i];

		// 取星球配置
		const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(wStarId);
		if (!pStarCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, wStarId);
			continue;
		}

		StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);

		// 检测是否已领取过
		if (tStarInfo.mRewardRecordList.find(dwRoleId) != tStarInfo.mRewardRecordList.end())
		{
			MAPLOG(ERROR_LEVEL, "[%s]星球的奖励已领取过[RoleId:%d,GuildId:%d,StarId:%d]!",
				__FUNCTION__, dwRoleId, dwGuildId, wStarId);
			continue;
		}
		
		// 检测是否属于本工会的星球
		if (tStarInfo.dwDelongGuildId != dwGuildId)
		{
			MAPLOG(ERROR_LEVEL, "[%s]星球不属于玩家所在工会的[RoleId:%d,RoleGuildId:%d,StarId:%d,StarBelongGuildId;%d]!",
				__FUNCTION__, dwRoleId, dwGuildId, wStarId, tStarInfo.dwDelongGuildId);
			continue;
		}

		// 低级星球
		if (IsLowStar(pStarCfg->wStarType))
		{
			// 检测是否领取奖励时间
			if (!IsLowStarRewardTime())
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]当前不是领取积分星球奖励时间[RoleId:%d,StarId:%d]!", __FUNCTION__, dwRoleId, wStarId);
				continue;
			}

			//  添加奖励
			pPlayer->AddObjectList(pStarCfg->vLocalProductList, TRUE, FROM_GUILD_WAR_TAKE_STAR_REWARD);
		}
		// 高级星球
		else
		{
			// 检测是否领取奖励时间
			if (!CheckHighStarActivityStatus(VSSTAR_ACTIVITY_EXPLOIT))
			{
				pPlayer->SendErrorMsg(ERROR_UNKNOW);
				MAPLOG(ERROR_LEVEL, "[%s]当前不是领取对抗星球奖励时间[RoleId:%d,StarId:%d]!", __FUNCTION__, dwRoleId, wStarId);
				continue;
			}

			//  添加奖励
			pPlayer->AddObjectList(pStarCfg->vDailyRewardList, TRUE, FROM_GUILD_WAR_TAKE_STAR_REWARD);
		}

		// 记录已领取
		tStarInfo.mRewardRecordList.insert(make_pair(dwRoleId, dwRoleId));
	}

	// 响应
	MSG_S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP msg;
	msg.m_wTakeType = wTakeType;
	msg.m_wStarNum = wStarNum;
	memcpy_s(msg.m_arStarList, sizeof(msg.m_arStarList), pStarList, sizeof(WORD)*wStarNum);
	pPlayer->SendPacket(&msg);
}

// 请求星球详细
void CGuildWarMgr::OnNetStarDetail(Player *pPlayer, WORD wStarId)
{
	MSG_S2C_GUILD_WAR_STAR_DETAIL_RESP msg;
	StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), wStarId);
	msg.m_wStarId = tStarInfo.dwInfoId;
	BYTE byIsBattleOver = tStarInfo.dwLastDelongGuildId ? true : false;

	MAPLOG(SPECAIL_LEVEL, "[%s]星球塔信息[StarId:%d,LastDelongGuildId:%d,RoleId:%d,IsBattleOver:%d]!",
		__FUNCTION__, tStarInfo.dwInfoId, tStarInfo.dwLastDelongGuildId, pPlayer->GetRoleId(), byIsBattleOver);


	msg.m_byIsBattleOver = byIsBattleOver;
	memset(&msg.m_tBelongGuildData, 0, sizeof(msg.m_tBelongGuildData));
	memset(&msg.m_tAttackGuildData, 0, sizeof(msg.m_tAttackGuildData));
	{
		CGuild *pBelongGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwDelongGuildId);
		if (pBelongGuild)
		{
			StarBelongGuildData &tBelongGuild = msg.m_tBelongGuildData;
			tBelongGuild.dwGuildId = pBelongGuild->GetGuildId();
			tBelongGuild.dwActive = pBelongGuild->GetActive();
			tBelongGuild.wServerId = pBelongGuild->GetServerId();
			tBelongGuild.wIconId = pBelongGuild->GetGuildIconId();
			strcpy_s(tBelongGuild.szGuildName, sizeof(tBelongGuild.szGuildName), pBelongGuild->GetGuildName());
		}
	}
	{
		// 战斗结束，并且活动还没结束
		CGuild *pAttackGuild = NULL;
		if (byIsBattleOver && tStarInfo.dwLastDelongGuildId != 999) // 999=表示从怪物手上抢过来的
		{
			pAttackGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwLastDelongGuildId);
		}
		else if (tStarInfo.dwAttackGuildId)
		{
			pAttackGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwAttackGuildId);
		}

		//CGuild *pAttackGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwAttackGuildId);
		if (pAttackGuild)
		{
			StarAttackGuildData &tAttackGuild = msg.m_tAttackGuildData;
			tAttackGuild.dwGuildId = pAttackGuild->GetGuildId();
			tAttackGuild.dwActive = pAttackGuild->GetActive();
			tAttackGuild.wServerId = pAttackGuild->GetServerId();
			tAttackGuild.wIconId = pAttackGuild->GetGuildIconId();
			strcpy_s(tAttackGuild.szGuildName, sizeof(tAttackGuild.szGuildName), pAttackGuild->GetGuildName());
		}
	}

	map<DWORD, DWORD> *pGuildList = NULL;
	bool bIsScore = false;

	// 宣战工会列表
	if (tStarInfo.mDeclareWarList.size()){
		pGuildList = &tStarInfo.mDeclareWarList;
	}
	// 挑战工会列表
	else if (tStarInfo.mScoreList.size()){
		pGuildList = &tStarInfo.mScoreList;
		bIsScore = true;
	}

	if (pGuildList)
	{
		CGuild *pGuild;
		GuildWarStarBaseData tGuildData;
		map<DWORD, DWORD>::iterator iter = pGuildList->begin();
		for (; iter != pGuildList->end(); ++iter)
		{
			pGuild = CGuildManager::Instance()->GetGuildById(iter->first);
			if (pGuild)
			{
				tGuildData.dwContext = bIsScore ? iter->second : pGuild->GetActive();
				tGuildData.wServerId = pGuild->GetServerId();
				tGuildData.wIconId = pGuild->GetGuildIconId();
				strcpy_s(tGuildData.szGuildName, sizeof(tGuildData.szGuildName), pGuild->GetGuildName());
				msg.m_vDeclareWarGuildList.push_back(tGuildData);
			}
		}
	}

	pPlayer->SendPacket(&msg);
}

// 请求塔列表
void CGuildWarMgr::OnNetTowerList(Player *pPlayer, DWORD dwStarId)
{
	WORD wServerId = pPlayer->GetServerId();
	StarInfo *pStarInfo = GetStarInfoEx(wServerId, dwStarId);
	if (!pStarInfo)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球数据[server_id:%d,StarId:%d]!", __FUNCTION__, wServerId, dwStarId);
		return;
	}

	MSG_S2C_GUILD_WAR_TOWER_LIST_RESP msg;
	msg.m_wServerId = wServerId;
	msg.m_wStarId = dwStarId;

	GuildWarTowerForClient tTowerData;
	StarDefenderForClient tDefender;
	vector<StarTowerInfo>::iterator iter1 = pStarInfo->vTowerList.begin();
	for (; iter1 != pStarInfo->vTowerList.end(); ++iter1)
	{
		tTowerData.wTowerId = iter1->wTowerId;
		tTowerData.dwDurability = iter1->dwDurability;
		tTowerData.wMonsterBeAttackTimes = iter1->wMonsterBeAttackTimes;
		tTowerData.vDefenderList.clear();
		vector<StarDefenderInfo>::iterator iter2 = iter1->vDefenderList.begin();
		for (; iter2 != iter1->vDefenderList.end(); ++iter2)
		{
			tDefender.dwRoleId = iter2->dwRoleId;
			tDefender.wBeAttackTimes = iter2->wBeAttackTimes;

			const PlayerBaseData *pBaseData = g_GetPlayerBaseData(iter2->dwRoleId, false);
			if (pBaseData){
				tDefender.wHeadIcon = pBaseData->wHeadIcon;
				tDefender.dwFightingPower = pBaseData->dwFightingPower;
				strcpy_s(tDefender.szRoleName, sizeof(tDefender.szRoleName), pBaseData->szRoleName);
			}

			tTowerData.vDefenderList.push_back(tDefender);
		}
		msg.m_vTowerList.push_back(tTowerData);
	}


	pPlayer->SendPacket(&msg);
}

// 解散工会事件
void CGuildWarMgr::OnDismissGuild(WORD wServerId, DWORD dwGuildId)
{
	WORD wRegionId = GetRegionId(wServerId);

	// 自己占领的(归还)
	const map<DWORD, DWORD> *pGuildStarList = GetGuildStarList(dwGuildId);
	if (pGuildStarList)
	{
		map<DWORD, DWORD>::const_iterator iter = pGuildStarList->begin();
		for (; iter != pGuildStarList->end(); ++iter)
		{
			StarInfo &tStarInfo = GetStarInfoByRegionId(wRegionId, dwGuildId);

			// 归还星球
			ResetStarInfo(tStarInfo, 0, false);
		}
	}
	m_GuildStarList.erase(dwGuildId);


	// 他人占领的(清状态)
	map<DWORD, StarInfo> *pStarlist = GetStarList(wRegionId);
	if (pStarlist)
	{
		map<DWORD, StarInfo>::iterator iter = pStarlist->begin();
		for (; iter != pStarlist->end(); ++iter){

			// 清空宣布
			iter->second.mDeclareWarList.erase(dwGuildId);

			// 清空积分
			iter->second.mScoreList.erase(dwGuildId);
		}
	}
}

// 退出工会事件
void CGuildWarMgr::OnLeave(CGuild *pGuild, DWORD dwRoleId)
{
	DWORD dwGuildId = pGuild->GetGuildId();
	const map<DWORD, DWORD> *pGuildStarList = GetGuildStarList(dwGuildId);
	if (!pGuildStarList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到工会占领的星球[GuildId:%d]!", __FUNCTION__, dwGuildId);
		return;
	}

	map<DWORD, DWORD>::const_iterator iter = pGuildStarList->begin();
	for (; iter != pGuildStarList->end(); ++iter)
	{
		StarInfo &tStarInfo = GetStarInfo(pGuild->GetServerId(), iter->first);
			
		// 检测是否参与防守中, 并移出
		bool bIsDefense = false;
		for (int i = 0; i < tStarInfo.vTowerList.size(); ++i)
		{
			StarTowerInfo &tTower = tStarInfo.vTowerList[i];
			for (int k = 0; k < tTower.vDefenderList.size(); ++k)
			{
				if (tTower.vDefenderList[k].dwRoleId == dwRoleId)
				{
					// 移出退出者
					tTower.vDefenderList[k] = tTower.vDefenderList[tTower.vDefenderList.size() - 1];
					tTower.vDefenderList.pop_back();				
						
					// 如果是基地,且当前已无人防守,自动将当前军团中战力最高的玩家设置到基地的布防列表中（不管他之前空闲还是在其他防御塔布防）
					if (!tTower.vDefenderList.size() && STAR_TOWER_1 == GetTowerType(tTower.wTowerId))
					{
						vector<RolePower> vPlayerList;
						pGuild->GetMemeberPowerRankList(vPlayerList);
						if (vPlayerList.size())
						{
							RemoveDefender(tStarInfo, vPlayerList[0].dwRoleId);

							StarDefenderInfo tDefender;
							tDefender.dwRoleId = vPlayerList[0].dwRoleId;
							tDefender.wBeAttackTimes = 0;
							tTower.vDefenderList.push_back(tDefender);
						}

						MAPLOG(SPECAIL_LEVEL, "基地最后一个玩家退出工会[GuildId:%d,StarId:%d,RoleId:%d,MemberNum:%d,TopPowerRoleId:%d]!",
							dwGuildId, tStarInfo.dwInfoId, dwRoleId, vPlayerList.size(), vPlayerList[0].dwRoleId);
					}

					MAPLOG(SPECAIL_LEVEL, "玩家退出工会[GuildId:%d,StarId:%d]!", dwGuildId, tStarInfo.dwInfoId);
						
					bIsDefense = true;
					break;
				}
			}

			if (bIsDefense)
				break;
		}
	}
}

// 取工会已经宣战的数量
WORD CGuildWarMgr::GetAlreadyDeclareWarNum(WORD wRegionId, DWORD dwGuildId)
{
	WORD wNum = 0;
	map<DWORD, StarInfo> *pStarlist = GetStarList(wRegionId);
	if (pStarlist)
	{
		map<DWORD, StarInfo>::iterator iter = pStarlist->begin();
		for (; iter != pStarlist->end(); ++iter)
		{
			if (iter->second.mDeclareWarList.end() != iter->second.mDeclareWarList.find(dwGuildId))
				wNum++;
		}
	}
	return wNum;
}

// 取占领高级星球的数量
WORD CGuildWarMgr::GetOccupyHighStarNum(WORD wServerId, DWORD dwGuildId)
{
	WORD wNum = 0;
	const map<DWORD, DWORD> *pGuildStarList = GetGuildStarList(dwGuildId);
	if (pGuildStarList)
	{
		map<DWORD, DWORD>::const_iterator iter = pGuildStarList->begin();
		for (; iter != pGuildStarList->end(); ++iter)
		{
			StarInfo *pStarInfo = GetStarInfoEx(wServerId, dwGuildId);
			if (!pStarInfo)
				continue;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
				continue;

			if (IsHighStar(pStarCfg->wStarType) && pStarInfo->dwDelongGuildId == dwGuildId)
				++wNum;
		}
	}
	return wNum;
}

// 取区域ID
WORD CGuildWarMgr::GetRegionId(WORD wServerId)
{
	const GuildWarRegion_Config *pRegionCfg = g_pCfgMgr->GetGuildWarRegionByServerId(wServerId);
	return pRegionCfg ? pRegionCfg->wRegionId : 0;
}

// 检测是否工会的星球
bool CGuildWarMgr::IsGuildStar(DWORD dwGuildId, DWORD dwStarId)
{
	const map<DWORD, DWORD> *pStarList = GetGuildStarList(dwGuildId);
	if (pStarList && pStarList->find(dwStarId) != pStarList->end())
		return true;

	return false;
}

// 是否积分星球挑战时间
bool CGuildWarMgr::IsLowStarBattleTime()
{
	const Server_Config *pStarCfg = GetCommonCfg(5);
	const Server_Config *pEndCfg = GetCommonCfg(6);
	if (!pStarCfg || !pEndCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到低级星球每日挑战时间配置!", __FUNCTION__);
		return false;
	}

	tm tmTime;
	time_t t = time(NULL);
	localtime_s(&tmTime, &t);

	DWORD dwCurTime = tmTime.tm_hour * 100 + tmTime.tm_min;

	return dwCurTime >= pStarCfg->dwValue && dwCurTime <= pEndCfg->dwValue;
}

// 是否积分星球奖励时间
bool CGuildWarMgr::IsLowStarRewardTime()
{
	const Server_Config *pStarCfg = GetCommonCfg(7);
	const Server_Config *pEndCfg = GetCommonCfg(8);
	if (!pStarCfg || !pEndCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到低级星球每日奖励时间配置!", __FUNCTION__);
		return false;
	}

	tm tmTime;
	time_t t = time(NULL);
	localtime_s(&tmTime, &t);

	DWORD dwCurTime = tmTime.tm_hour * 100 + tmTime.tm_min;

	return dwCurTime >= pStarCfg->dwValue && dwCurTime <= pEndCfg->dwValue;
}




// 检测高级星球活动状态
bool CGuildWarMgr::CheckHighStarActivityStatus(int nActivityStatus)
{
	const vector<GuildWarVSActivityTime_Config> *pTimeCfg = g_pCfgMgr->GetGuildWarVSActivityTime(nActivityStatus);
	if (!pTimeCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到高级星球活动时间配置[TimeType:%d]!", __FUNCTION__, nActivityStatus);
		return VSSTAR_ACTIVITY_CLOSE;
	}

	tm tmTime;
	time_t t = time(NULL);
	localtime_s(&tmTime, &t);
	DWORD dwCurTime = g_CorrectWeekDay(tmTime.tm_wday) * 10000 + tmTime.tm_hour * 100 + tmTime.tm_min;

	// 宣战, 战斗
	if (VSSTAR_ACTIVITY_DECLARE_WAR == nActivityStatus || VSSTAR_ACTIVITY_BATTLE == nActivityStatus)
	{
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pTimeCfg->begin();
		for (; iter != pTimeCfg->end(); ++iter)
		{
			if (dwCurTime >= iter->dwBeginTime && dwCurTime <= iter->dwEndTime)
				return true;
		}
	}
	// 开采
	else if (VSSTAR_ACTIVITY_EXPLOIT == nActivityStatus)
	{
		vector<GuildWarVSActivityTime_Config>::const_iterator iter = pTimeCfg->begin();
		for (; iter != pTimeCfg->end(); ++iter)
		{
			for (int i = 0; i < iter->vTakeRewardTime.size(); ++i)
			{
				if (iter->vTakeRewardTime[i] == g_CorrectWeekDay(tmTime.tm_wday))
					return true;
			}
		}
	}
	else
	{
		MAPLOG(ERROR_LEVEL, "[%s]未处理的高级星球活动时间[TimeType:%d]!", __FUNCTION__, nActivityStatus);
	}

	return false;
}

// 检测塔是否已破
bool CGuildWarMgr::IsTowerBreak(DWORD dwBelongGuildId, const StarTowerInfo *pTowerInfo)
{
	// 耐久已为0
	if (!pTowerInfo->dwDurability)
		return true;

	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pTowerInfo->wTowerId);
	if (!pTowerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球塔的配置[TowerId:%d]!", __FUNCTION__, pTowerInfo->wTowerId);
		return false;
	}

	// 人守
	if (dwBelongGuildId)
	{
		// 无人防守
		if (!pTowerInfo->vDefenderList.size())
			return true;

		for (int i = 0; i < pTowerInfo->vDefenderList.size(); ++i)
		{
			// 还有防守次数
			if (pTowerInfo->vDefenderList[i].wBeAttackTimes < pTowerCfg->wRoleDefenseTimes)
				return false;
		}
	}
	// 怪守
	else
	{
		// 还有防守次数
		if (pTowerInfo->wMonsterBeAttackTimes < pTowerCfg->wMonsterDefenseTimes)
			return false;
	}

	return true;
}

// 取破塔数量
WORD CGuildWarMgr::GetBreakTowerNum(const StarInfo *pStarInfo)
{
	WORD wBreakTowerNum = 0;
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		if (IsTowerBreak(pStarInfo->dwDelongGuildId, &pStarInfo->vTowerList[i]))
			wBreakTowerNum++;
	}
	return wBreakTowerNum;
}

// 是否可攻击基地
bool CGuildWarMgr::IsCanAttackBoss(const StarInfo *pStarInfo)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		const StarTowerInfo *pTower = &pStarInfo->vTowerList[i];
		const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pTower->wTowerId);
		if (!pTowerCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到星球塔的配置[TowerId:%d]!", __FUNCTION__, pTower->wTowerId);
			return false;
		}

		if (pTowerCfg->wTowerType != STAR_TOWER_1)
		{
			// 只要有其中一个塔被破,就可以攻击
			if (IsTowerBreak(pStarInfo->dwDelongGuildId, pTower))
				return true;
		}
	}

	return false;
}

// 是否低级星球
bool CGuildWarMgr::IsLowStar(WORD wStarType)
{
	return wStarType <= GUILD_WAR_STAR_BLUE ? true : false;
}

// 是否高级星球
bool CGuildWarMgr::IsHighStar(WORD wStarType)
{
	return wStarType >= GUILD_WAR_STAR_PURPLE ? true : false;
}

// 是否正在防守中
bool CGuildWarMgr::IsInDefense(const StarInfo *pStarInfo, DWORD dwRoleId)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		const StarTowerInfo *pTower = &pStarInfo->vTowerList[i];
		for (int k = 0;  k < pTower->vDefenderList.size(); ++k)
		{
			if (pTower->vDefenderList[k].dwRoleId == dwRoleId)
				return true;
		}
	}
	return false;
}

// 取塔类型
WORD CGuildWarMgr::GetTowerType(WORD wTowerId)
{
	const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(wTowerId);
	if (!pTowerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球塔的配置[TowerId:%d]!", __FUNCTION__, wTowerId);
		return 0;
	}

	return pTowerCfg->wTowerType;
}

// 取星球塔当前防守人数
WORD CGuildWarMgr::GetCurTowerDefenseNum(const StarInfo *pStarInfo, WORD wTowerType)
{
	vector<StarTowerInfo>::const_iterator iter = pStarInfo->vTowerList.begin();
	for (; iter != pStarInfo->vTowerList.end(); ++iter)
	{
		if (GetTowerType(iter->wTowerId) == wTowerType)
			return iter->vDefenderList.size();
	}

	return 0;
}

// 添加工会积分
void CGuildWarMgr::AddGuildScore(StarInfo &tStarInfo, DWORD dwGuildId, DWORD dwScore)
{
	map<DWORD, DWORD>::iterator iter = tStarInfo.mScoreList.find(dwGuildId);
	if (iter != tStarInfo.mScoreList.end()){
		iter->second += dwScore;
	}
	else{
		tStarInfo.mScoreList.insert(make_pair(dwGuildId, dwScore));
	}
}

// 取星球信息
StarInfo& CGuildWarMgr::GetStarInfo(WORD wServerId, DWORD dwStarId)
{
	WORD wRegionId = GetRegionId(wServerId);

	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 == m_StarInfoList.end())
	{
		m_StarInfoList.insert(make_pair(wRegionId, map<DWORD, StarInfo>()));
		iter1 = m_StarInfoList.find(wRegionId);
	}

	map<DWORD, StarInfo>::iterator iter2 = iter1->second.find(dwStarId);
	if (iter2 == iter1->second.end())
	{
		StarInfo tStar;
		tStar.dwInfoId = dwStarId;
		ResetStarInfo(tStar, 0, true);
		iter1->second.insert(make_pair(dwStarId, tStar));
		iter2 = iter1->second.find(dwStarId);
	}

	return iter2->second;
}

// 取星球信息(如果目前没有则会创建)
StarInfo& CGuildWarMgr::GetStarInfoByRegionId(WORD wRegionId, DWORD dwStarId)
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 == m_StarInfoList.end())
	{
		m_StarInfoList.insert(make_pair(wRegionId, map<DWORD, StarInfo>()));
		iter1 = m_StarInfoList.find(wRegionId);
	}

	map<DWORD, StarInfo>::iterator iter2 = iter1->second.find(dwStarId);
	if (iter2 == iter1->second.end())
	{
		StarInfo tStar;
		tStar.dwInfoId = dwStarId;
		ResetStarInfo(tStar, 0, true);
		iter1->second.insert(make_pair(dwStarId, tStar));
		iter2 = iter1->second.find(dwStarId);
	}

	return iter2->second;
}

// 查找星球信息
StarInfo* CGuildWarMgr::GetStarInfoEx(WORD wServerId, DWORD dwStarId)
{
	WORD wRegionId = GetRegionId(wServerId);

	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 == m_StarInfoList.end()) return NULL;

	map<DWORD, StarInfo>::iterator iter2 = iter1->second.find(dwStarId);
	return iter2 != iter1->second.end() ? &iter2->second : NULL;
}

// 取塔信息
StarTowerInfo* CGuildWarMgr::GetTowerInfo(StarInfo *pStarInfo, WORD wTowerId)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		if (pStarInfo->vTowerList[i].wTowerId == wTowerId)
			return &pStarInfo->vTowerList[i];
	}
	return NULL;
}

// 通过类型取塔信息
StarTowerInfo* CGuildWarMgr::GetTowerInfoByType(StarInfo *pStarInfo, WORD wTowerType)
{
	for (int i = 0; i < pStarInfo->vTowerList.size(); ++i)
	{
		if (GetTowerType(pStarInfo->vTowerList[i].wTowerId) == wTowerType)
			return &pStarInfo->vTowerList[i];
	}
	return NULL;
}

// 取防守者信息
StarDefenderInfo* CGuildWarMgr::GetDefenderInfo(StarTowerInfo *pTowerInfo, DWORD dwRoleId)
{
	for (int i = 0; i < pTowerInfo->vDefenderList.size(); ++i)
	{
		if (pTowerInfo->vDefenderList[i].dwRoleId == dwRoleId)
			return &pTowerInfo->vDefenderList[i];
	}
	return NULL;
}

// 取服务器星球列表
map<DWORD, StarInfo>* CGuildWarMgr::GetStarList(WORD wAreaId)
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter = m_StarInfoList.find(wAreaId);
	if (iter == m_StarInfoList.end())
	{
		m_StarInfoList.insert(make_pair(wAreaId, map<DWORD, StarInfo>()));
		iter = m_StarInfoList.find(wAreaId);
	}
	return &iter->second;
}

// 取工会占领的的星球列表
const map<DWORD, DWORD>* CGuildWarMgr::GetGuildStarList(DWORD dwGuildId)
{
	map<DWORD, map<DWORD, DWORD>>::iterator iter = m_GuildStarList.find(dwGuildId);
	return iter != m_GuildStarList.end() ? &iter->second : NULL;
}

// bool RolePowerPred(RolePower &data1, RolePower &data2)
// {
// 	if (data1.dwPower > data2.dwPower)
// 		return true;
// 
// 	if (data1.dwPower == data2.dwPower && data1.dwRoleId < data2.dwRoleId)
// 		return true;
// 
// 	return false;
// }

// 重置星球信息
void CGuildWarMgr::ResetStarInfo(StarInfo &tStarInfo, DWORD dwBelongGuildId, bool bIsNew)
{
	tStarInfo.dwDelongGuildId = dwBelongGuildId;
	tStarInfo.dwAttackGuildId = 0;
	tStarInfo.dwLastDelongGuildId = 0;
	tStarInfo.mRewardRecordList.clear();
	tStarInfo.mDeclareWarList.clear();

	// 新占领
	if (bIsNew)
	{
		/*
		初次占领星球时，默认布防设置：按照战力顺序放置
		1.	基地：1、5~13
		2.	防线1：2、14、17
		3.	防线2：3、15、18
		4.	防线3：4、16、19
		5.	人数不足时空着
		*/

		// 获取成员列表并按战力进行排序
		vector<RolePower> vPlayerList;
		CGuild *pGuild = dwBelongGuildId ? CGuildManager::Instance()->GetGuildById(dwBelongGuildId) : NULL;
		if (pGuild){
			pGuild->GetMemeberPowerRankList(vPlayerList);
		}

		const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(tStarInfo.dwInfoId);
		if (!pStarCfg)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到星球的配置[TowerId:%d]!", __FUNCTION__, tStarInfo.dwInfoId);
			return;
		}
		tStarInfo.vTowerList.clear();
		for (int i = 0; i < pStarCfg->vTowerList.size(); ++i)
		{
			const GuildWarTower_Config *pTowerCfg = g_pCfgMgr->GetGuildWarTower(pStarCfg->vTowerList[i]);
			if (!pTowerCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到工会战塔的配置[TowerId:%d]!", __FUNCTION__, pStarCfg->vTowerList[i]);
				continue;
			}

			const GuildWarOrganizeDefense_Config *pOrganizeDefCfg = g_pCfgMgr->GetGuildWarOrganizeDefense(pTowerCfg->wTowerType);
			if (!pOrganizeDefCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到工会战布防配置[TowerType:%d]!", __FUNCTION__, pTowerCfg->wTowerType);
				continue;
			}

			StarTowerInfo tTowerInfo;
			tTowerInfo.wTowerId = pTowerCfg->dwTowerId;
			//tTowerInfo.wType = pTowerCfg->wTowerType;
			tTowerInfo.dwDurability = pTowerCfg->dwDurability;
			tTowerInfo.wMonsterBeAttackTimes = 0;
			for (int k = 0; k < pOrganizeDefCfg->vPowerIdxList.size(); ++k)
			{
				DWORD dwIdx = pOrganizeDefCfg->vPowerIdxList[k];
				DWORD dwRoleId = (dwIdx && dwIdx <= vPlayerList.size()) ? vPlayerList[dwIdx-1].dwRoleId : 0;
				if (dwRoleId)
				{
					StarDefenderInfo tDefender;
					tDefender.dwRoleId = dwRoleId;
					tDefender.wBeAttackTimes = 0;
					tTowerInfo.vDefenderList.push_back(tDefender);
				}
				MAPLOG(ERROR_LEVEL, "成员布防[GuildId:%d,TowerId:%d,RoleId:%d]!", dwBelongGuildId, pTowerCfg->dwTowerId, dwRoleId);
			}
			tStarInfo.vTowerList.push_back(tTowerInfo);
		}
	}
	// 继续占领(耐久度不变)
	else
	{
		for (int i = 0; i < tStarInfo.vTowerList.size(); ++i)
		{
			StarTowerInfo &tTowerInfo = tStarInfo.vTowerList[i];
			tTowerInfo.wMonsterBeAttackTimes = 0;
			//tTowerInfo.dwDurability = 0;
			for (int k = 0; k < tTowerInfo.vDefenderList.size(); ++k)
			{
				tTowerInfo.vDefenderList[k].wBeAttackTimes = 0;
			}
		}
	}
}

// 取玩家工会战数据
RoleGuildWarData& CGuildWarMgr::GetRoleGuildWarData(DWORD dwRoleId)
{
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.find(dwRoleId);
	if (iter == m_RoleGuildWarDataList.end())
	{
		RoleGuildWarData data;
		memset(&data, 0, sizeof(data));
		m_RoleGuildWarDataList.insert(make_pair(dwRoleId, data));
		iter = m_RoleGuildWarDataList.find(dwRoleId);
	}
	return iter->second;
}

// 添加占领的星球
void CGuildWarMgr::AddOccupyStar(DWORD dwGuildId, WORD wStarId)
{
	if (!dwGuildId)
		return;

	map<DWORD, map<DWORD, DWORD>>::iterator iter = m_GuildStarList.find(dwGuildId);
	if (iter == m_GuildStarList.end())
	{
		m_GuildStarList.insert(make_pair(dwGuildId, map<DWORD, DWORD>()));
		iter = m_GuildStarList.find(dwGuildId);
	}
	iter->second.insert(make_pair(wStarId, wStarId));
}

// 删除占领的星球
void CGuildWarMgr::RemoveOccupyStar(DWORD dwGuildId, WORD wStarId)
{
	map<DWORD, map<DWORD, DWORD>>::iterator iter = m_GuildStarList.find(dwGuildId);
	if (iter != m_GuildStarList.end()){
		iter->second.erase(wStarId);
	}
}

// 删除宣战的工会
void CGuildWarMgr::RemoveDeclareWarGuild(WORD wRegionId, DWORD dwGuildId)
{
	if (!dwGuildId) return;

	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.find(wRegionId);
	if (iter1 != m_StarInfoList.end())
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2){
			iter2->second.mDeclareWarList.erase(dwGuildId);
		}
	}
}

// 添加星球数据
void CGuildWarMgr::AddStarInfo(WORD wRegionId, const StarInfo &tStarInfo)
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter = m_StarInfoList.find(wRegionId);
	if (iter == m_StarInfoList.end()){
 		m_StarInfoList.insert(make_pair(wRegionId, map<DWORD, StarInfo>()));
		iter = m_StarInfoList.find(wRegionId);
	}

	iter->second.insert(make_pair(tStarInfo.dwInfoId, tStarInfo));
}

// 移除防守者
void CGuildWarMgr::RemoveDefender(StarInfo &tStarInfo, DWORD dwRoleId)
{
	for (int i = 0; i < tStarInfo.vTowerList.size(); ++i)
	{
		StarTowerInfo &tTower = tStarInfo.vTowerList[i];
		for (int k = 0; k < tTower.vDefenderList.size(); ++k)
		{
			if (tTower.vDefenderList[k].dwRoleId == dwRoleId)
			{
				tTower.vDefenderList[k] = tTower.vDefenderList[tTower.vDefenderList.size() - 1];
				tTower.vDefenderList.pop_back();
				return;
			}
		}
	}
}

// 破塔奖励
void CGuildWarMgr::BreakTowerReward(WORD wStarId, StarTowerInfo *pTowerInfo, DWORD dwAttackGuildId)
{
	// 通过奖励中心发奖励
	CGuild *pGuild = CGuildManager::Instance()->GetGuildById(dwAttackGuildId);
	if (pGuild)
	{
		RewardData tRewardData;
		tRewardData.wInfoId = 31;
		tRewardData.dwTime = time(NULL);
		tRewardData.dwValue1 = wStarId;
		tRewardData.dwValue2 = pTowerInfo->wTowerId;
		pGuild->SendRewardToAllMember(tRewardData);
	}
}

// 活动结束奖励
void CGuildWarMgr::HighStarActivtiyOverReward(const StarInfo *pStarInfo, BYTE byIsAttackerWin, WORD wBreakTowerNum)
{
	// 攻方获胜
	if (byIsAttackerWin)
	{
		// 攻击方全部团员获得战胜奖励及该星球特产
		CGuild *pAttackGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwAttackGuildId);
		if (pAttackGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 32;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeWord(1, 1);	// 1=表示攻方 1=表示获胜
			pAttackGuild->SendRewardToAllMember(tRewardData);
		}

		// 防守方全部团员获得安慰奖励
		CGuild *pDefenseGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwDelongGuildId);
		if (pDefenseGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 35;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeWord(2, 0);	// 2=表示守方 0=表示失败
			pDefenseGuild->SendRewardToAllMember(tRewardData);
		}
	}
	// 守方获胜
	else
	{
		// 防守方全部团员获得防守成功奖励
		CGuild *pDefenseGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwDelongGuildId);
		if (pDefenseGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 34;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeWord(2, 1);	// 2=表示守方 1=表示获胜
			pDefenseGuild->SendRewardToAllMember(tRewardData);
		}

		// 攻击方根据攻破的防线数量，获得安慰奖励
		CGuild *pAttackGuild = CGuildManager::Instance()->GetGuildById(pStarInfo->dwAttackGuildId);
		if (pAttackGuild)
		{
			RewardData tRewardData;
			tRewardData.wInfoId = 33;
			tRewardData.dwTime = time(NULL);
			tRewardData.dwValue1 = pStarInfo->dwInfoId;
			tRewardData.dwValue2 = MakeDWord(wBreakTowerNum, MakeWord(1, 0));  // 1=表示攻方 0=表示失败
			pAttackGuild->SendRewardToAllMember(tRewardData);
		}
	}
}

// 对抗星球宣战时间结束
void CGuildWarMgr::HighStarDeclareWarTimeOver()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到星球的配置[TowerId:%d]!", __FUNCTION__, pStarInfo->dwInfoId);
				continue;
			}

			// 低级星球不处理
			if (IsLowStar(pStarCfg->wStarType))
				continue;

			// 决定那个工会获得进攻权
			DWORD dwMaxActive = 0;
			DWORD dwActiveGuildId = 0;
			map<DWORD, DWORD>::iterator iter3 = pStarInfo->mDeclareWarList.begin();
			for (; iter3 != pStarInfo->mDeclareWarList.end(); ++iter3)
			{
				CGuild *pGuild = CGuildManager::Instance()->GetGuildById(iter3->first);
				if (!pGuild)
				{
					MAPLOG(ERROR_LEVEL, "[%s]找不到工会数据[GuildId:%d]!", __FUNCTION__, iter3->first);
					continue;
				}

				// 取活跃最高的工会, 活跃相同则取最早报名的
				if (!dwActiveGuildId || pGuild->GetActive() > dwMaxActive)
				{
					dwMaxActive = pGuild->GetActive();
					dwActiveGuildId = pGuild->GetGuildId();
				}
			}

			// 决定进攻权后清空宣战列表
			pStarInfo->mDeclareWarList.clear();

			// 记录获胜进攻权的工会
			pStarInfo->dwAttackGuildId = dwActiveGuildId;

			MAPLOG(DEBUG_LEVEL, "[%s]获得星球进攻权的工会[StarId:%d, BelongGuilId:%d, GuildId:%d]!", 
				__FUNCTION__, pStarInfo->dwInfoId, pStarInfo->dwDelongGuildId, dwActiveGuildId);


			RemoveDeclareWarGuild(iter1->first, dwActiveGuildId);
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]高级星球宣战时间结束",__FUNCTION__);
}

// 对抗星球活动结束
void CGuildWarMgr::HighStarActivityOver()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;
			pStarInfo->dwLastDelongGuildId = 0;

			// 没有人进攻则特殊处理(1.有可能是积分星球 2.有可能是活动结束前已定胜负,因为决定胜负的时候已处理过 3.有可能确实是木有人宣战)
			if (!pStarInfo->dwAttackGuildId)
			{
				pStarInfo->mRewardRecordList.clear();
				continue;
			}

			// 发奖励
			HighStarActivtiyOverReward(pStarInfo, false, GetBreakTowerNum(pStarInfo));

			// 重置
			ResetStarInfo(*pStarInfo, pStarInfo->dwDelongGuildId, false);
		}
	}

	// 排行榜奖励
	const AllRoleRankList *pRankList = CRankManager::Instance()->GetRankListEx(RANK_GUILD_WAR);
	if (!pRankList)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到排行榜列表[rankType:%d]!", __FUNCTION__, RANK_GUILD_WAR);
		return;
	}
	AllRoleRankList::const_iterator iter_rank_list = pRankList->begin();
	for (; iter_rank_list != pRankList->end(); ++iter_rank_list)
	{
		RoleRankList::const_iterator iter_rank_data = iter_rank_list->second.begin();
		for (; iter_rank_data != iter_rank_list->second.end(); ++iter_rank_data)
		{
			const GuildWarRankReward_Config *pRewardCfg = g_pCfgMgr->GetGuildWarRankReward(iter_rank_data->wRank);
			if (pRewardCfg)
			{
				RewardData tRewardData;
				tRewardData.wInfoId = 36;
				tRewardData.dwTime = time(NULL);
				tRewardData.dwValue1 = iter_rank_data->wRank;
				CRewardCenterManager::AddReward(iter_rank_data->dwServerId, iter_rank_data->dwRoleId, tRewardData);				
			}
		}
	}

	// 重置挑战次数
	map<DWORD, RoleGuildWarData>::iterator iter = m_RoleGuildWarDataList.begin();
	for (; iter != m_RoleGuildWarDataList.end(); ++iter){
		iter->second.wBuyHighStarBattleTimes = 0;
		iter->second.wUsedHighStarBattleTimes = 0;
	}

	MAPLOG(ERROR_LEVEL, "[%s]高级星球活动结束", __FUNCTION__);
}

// 积分星球活动开始
void CGuildWarMgr::LowStarActivityStart()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到星球的配置[TowerId:%d]!", __FUNCTION__, pStarInfo->dwInfoId);
				continue;
			}

			// 高级星球不处理
			if (IsHighStar(pStarCfg->wStarType))
				continue;

			// 清空领取奖励记录
			pStarInfo->mRewardRecordList.clear();

			RemoveOccupyStar(pStarInfo->dwDelongGuildId, pStarInfo->dwInfoId);

			// 清空获胜者记录
			pStarInfo->dwDelongGuildId = 0;
		}
	}
}

// 积分星球活动结束
void CGuildWarMgr::LowStarActivityOver()
{
	map<DWORD, map<DWORD, StarInfo>>::iterator iter1 = m_StarInfoList.begin();
	for (; iter1 != m_StarInfoList.end(); ++iter1)
	{
		map<DWORD, StarInfo>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			StarInfo *pStarInfo = &iter2->second;

			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(pStarInfo->dwInfoId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到星球的配置[TowerId:%d]!", __FUNCTION__, pStarInfo->dwInfoId);
				continue;
			}

			// 高级星球不处理
			if (IsHighStar(pStarCfg->wStarType))
				continue;

			// 确认那个工会获得胜利
			DWORD dwGuildId = 0;
			DWORD dwMaxScore = 0;
			map<DWORD, DWORD>::iterator iter3 = pStarInfo->mScoreList.begin();
			for (; iter3 != pStarInfo->mScoreList.end(); ++iter3)
			{
				// 积分相同时比那个工会先创建
				if (iter3->second > dwMaxScore || (iter3->second == dwMaxScore && iter3->first < dwGuildId))
				{
					dwGuildId = iter3->first;
					dwMaxScore = iter3->second;
				}
			}

			// 选出获胜者以后,清空积分列表
			pStarInfo->mScoreList.clear();

			// 这步不是必需的, 防止前面出了bug
			//pStarInfo->mRewardRecordList.clear();

			// 记录那个工会获胜
			pStarInfo->dwDelongGuildId = dwGuildId;

			AddOccupyStar(dwGuildId, pStarInfo->dwInfoId);

			MAPLOG(ERROR_LEVEL, "[%s]工会获得的低级星球拥有权[GuildId:%d]!", __FUNCTION__, dwGuildId);
		}
	}

	MAPLOG(ERROR_LEVEL, "[%s]积分星球活动结束", __FUNCTION__);
}

void CGuildWarMgr::SendGuildWarRoleData(Player *pPlayer)
{
	MSG_S2C_GUILD_WAR_ROLE_DATA_RESP msg;
	RoleGuildWarData &tGuildWarRoleData = GetRoleGuildWarData(pPlayer->GetRoleId());
	msg.m_GuildWarRoleData = tGuildWarRoleData;
	pPlayer->SendPacket(&msg);
}


// 发送星球列表
void CGuildWarMgr::SendStarList(Player *pPlayer)
{
	MSG_S2C_GUILD_WAR_STAR_LIST_RESP msg;

	bool isBattle = CheckHighStarActivityStatus(VSSTAR_ACTIVITY_BATTLE);
	DWORD dwMyGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());

	GuildWarStarBaseDataForClient tStarBaseData;
	WORD wRegionId = GetRegionId(pPlayer->GetServerId());
	map<DWORD, StarInfo> *pStarList = GetStarList(wRegionId);
	if (pStarList)
	{
		map<DWORD, StarInfo>::iterator iter = pStarList->begin();
		for (; iter != pStarList->end(); ++iter)
		{
			StarInfo *pTargetStar = &iter->second;

			memset(&tStarBaseData, 0, sizeof(tStarBaseData));
			tStarBaseData.wStarId = pTargetStar->dwInfoId;
			CGuild *pGuild = CGuildManager::Instance()->GetGuildById(pTargetStar->dwDelongGuildId);
			if (pGuild){
				strcpy_s(tStarBaseData.szGuildName, sizeof(tStarBaseData.szGuildName), pGuild->GetGuildName());
				tStarBaseData.wIconId = pGuild->GetGuildIconId();
			}

			// 是否在战斗中(跟我关联的)
			if (isBattle && dwMyGuildId && ((pTargetStar->dwDelongGuildId == dwMyGuildId && pTargetStar->dwAttackGuildId) || pTargetStar->dwAttackGuildId == dwMyGuildId))
				tStarBaseData.byIsInBattle = true;

			msg.m_vOccupyStarList.push_back(tStarBaseData);
		}
	}

	pPlayer->SendPacket(&msg);
}

// 发送工会占领的星球列表
void CGuildWarMgr::SendOccupyStarList(Player *pPlayer)
{
	// 检测是否有工会
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	if (!dwGuildId)
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没在工会里[RoleId:%d]!", __FUNCTION__, pPlayer->GetRoleId());
		return;
	}

	MSG_S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP msg;

	WORD wRegionId = GetRegionId(pPlayer->GetServerId());

	BYTE byIsHighStarRewardTime = CheckHighStarActivityStatus(VSSTAR_ACTIVITY_EXPLOIT);
	BYTE byIsLowStarRewardTime = IsLowStarRewardTime();

	GuildWarOccupyStarDataForClient tOccupyStarData;
	const map<DWORD, DWORD> *pStarList = GetGuildStarList(dwGuildId);
	if (pStarList)
	{
		map<DWORD, DWORD>::const_iterator iter = pStarList->begin();
		for (; iter != pStarList->end(); ++iter)
		{
			StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), iter->first);
			tOccupyStarData.wStarId = tStarInfo.dwInfoId;
			const GuildWarStar_Config *pStarCfg = g_pCfgMgr->GetGuildWarStar(tOccupyStarData.wStarId);
			if (!pStarCfg)
			{
				MAPLOG(ERROR_LEVEL, "[%s]找不到星球配置[StarId:%d]!", __FUNCTION__, tOccupyStarData.wStarId);
				continue;
			}
			BYTE byIsHaveReward = false;
			if (IsLowStar(pStarCfg->wStarType) && byIsLowStarRewardTime)
			{
				if (tStarInfo.mRewardRecordList.find(pPlayer->GetRoleId()) == tStarInfo.mRewardRecordList.end())
					byIsHaveReward = true;
			}
			else if (IsHighStar(pStarCfg->wStarType) && byIsHighStarRewardTime)
			{
				if (tStarInfo.mRewardRecordList.find(pPlayer->GetRoleId()) == tStarInfo.mRewardRecordList.end())
					byIsHaveReward = true;
			}
			tOccupyStarData.byIsHaveReward = byIsHaveReward;
			msg.m_vOccupyStarList.push_back(tOccupyStarData);
		}
	}

	pPlayer->SendPacket(&msg);
}

// 发送工会宣战的星球列表
void CGuildWarMgr::SendDeclareWarStarList(Player *pPlayer)
{
	MSG_S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP msg;
	DWORD dwGuildId = CGuildManager::Instance()->GetGuildIdByRoleId(pPlayer->GetRoleId());
	WORD wRegionId = GetRegionId(pPlayer->GetServerId());
	map<DWORD, StarInfo> *pStarList =  GetStarList(wRegionId);
	if (pStarList && dwGuildId)
	{
		GuildWarDeclareWarData tDeclareWarData;
		map<DWORD, StarInfo>::const_iterator iter = pStarList->begin();
		for (; iter != pStarList->end(); ++iter)
		{	
			// 已对此星球宣战
			if (iter->second.mDeclareWarList.find(dwGuildId) != iter->second.mDeclareWarList.end())
			{
				StarInfo &tStarInfo = GetStarInfo(pPlayer->GetServerId(), iter->first);
				CGuild *pBelongGuild = CGuildManager::Instance()->GetGuildById(tStarInfo.dwDelongGuildId);
				if (pBelongGuild){
					tDeclareWarData.wServerId = pBelongGuild->GetServerId();
					strcpy_s(tDeclareWarData.szGuildName, sizeof(tDeclareWarData.szGuildName), pBelongGuild->GetGuildName());
					tDeclareWarData.dwActive = pBelongGuild->GetActive();
				}
				else{
					memset(&tDeclareWarData, 0, sizeof(tDeclareWarData));
				}
				tDeclareWarData.wStarId = tStarInfo.dwInfoId;
				msg.m_vDeclareWarStarList.push_back(tDeclareWarData);
			}
		}
	}
	pPlayer->SendPacket(&msg);
}

// 取通用配置
const Server_Config*  CGuildWarMgr::GetCommonCfg(DWORD dwKey, bool bOutLog)
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetGuildWarCommon(dwKey);
	if (!pServerCfg)
	{
		if (bOutLog)
			MAPLOG(ERROR_LEVEL, "[%s]找不到星球通知配置[Key:%d]!", __FUNCTION__, dwKey);
		return NULL;
	}

	return pServerCfg;
}

DWORD CGuildWarMgr::GetConfigValue(DWORD dwKey, DWORD dwDefault)
{
	const Server_Config *pServerCfg = g_pCfgMgr->GetGuildWarCommon(dwKey);
	if (!pServerCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到星球通知配置[Key:%d]!", __FUNCTION__, dwKey);
		return dwDefault;
	}

	return pServerCfg->dwValue;
}

