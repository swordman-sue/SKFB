#include "stdafx.h"
#include "Christmas.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\MapServer.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\SystemNotice\SystemNotice.h"

CChristmas::CChristmas()
{

}

CChristmas::~CChristmas()
{

}

void CChristmas::Init()
{

}

// 登出
void CChristmas::OnLogout(Player *pPlayer)
{
	if (IsInBattle(pPlayer->GetServerId(), pPlayer->GetRoleId()))
	{
		RemoveBattlePlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());
	}
}

// 是否有红点
bool CChristmas::IsHaveRedPoint(Player *pPlayer)
{
	PlayerChristmasData &tChristmasData = pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;
	
	// 低级boss奖励可领取
	ChristmasBossData &tLowBossData = GetBossData(pPlayer->GetServerId(), CHRISTMAS_BOSS_LOWER);
	if (tChristmasData.wAlreadyTakeLowerBossRewardLevel + 1 < tLowBossData.wBossLevel)
		return true;

	// 低级boss奖励可领取
	ChristmasBossData &tHighBossData = GetBossData(pPlayer->GetServerId(), CHRISTMAS_BOSS_HIGH);
	if (tChristmasData.wAlreadyTakeHighBossRewardLevel + 1 < tHighBossData.wBossLevel)
		return true;

	return false;
}

// 重置数据
void CChristmas::ResetData()
{
	DWORD dwTime = time(NULL);
	const map<DWORD, ServerInfo> &mServerList = g_MapServer.GetServerList();
	map<DWORD, ServerInfo>::const_iterator iter = mServerList.begin();
	for (; iter != mServerList.end(); ++iter)
	{
		DWORD dwServerId = iter->second.dwServerId;
		g_MapServer.UpdateGolbalData(dwServerId, GOLBAL_CHRISTMAS_LOWER_BOSS_LEVEL, 0);
		g_MapServer.UpdateGolbalData(dwServerId, GOLBAL_CHRISTMAS_HIGH_BOSS_LEVEL, 0);
		g_MapServer.UpdateGolbalData(dwServerId, GOLBAL_CHRISTMAS_RESET_TIME, dwTime);
	}

	vector<Player*> vPlayerList;
	PlayerManager::Instance()->GetAllPlayer(vPlayerList);
	for (int i = 0; i < vPlayerList.size(); ++i){
		vPlayerList[i]->CheckAndResetChristmasData();
	}

	m_ChristmasDataList.clear();
	m_BattlePlayerList.clear();
}

// 请求圣诞节数据
void CChristmas::OnNetChristmasData(Player *pPlayer)
{
	if (!IsInBattle(pPlayer->GetServerId(), pPlayer->GetRoleId()))
		AddBattlePlayer(pPlayer);

	SendChristmasData(pPlayer);
}

// 进入战斗
void CChristmas::OnNetEnterBattle(Player *pPlayer)
{
	AddBattlePlayer(pPlayer);

	// 响应
	MSG_S2C_CHRISTMAS_BATTLE_ENTER_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 离开战斗
void CChristmas::OnNetLeaveBattle(Player *pPlayer)
{
	RemoveBattlePlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());

	// 响应
	MSG_S2C_CHRISTMAS_BATTLE_LEAVE_RESP msg;
	pPlayer->SendPacket(&msg);
}

// 攻击Boss
void CChristmas::OnNetAttackBoss(Player *pPlayer, WORD wBossType, DWORD dwSnowBallId)
{
	ChristmasBossData &tBossData = GetBossData(pPlayer->GetServerId(), wBossType);

	// 检测是否活动期间 todo::为了方便测试,暂时不检测
	if (!CActivityManager::IsActivityOpen(SYSTEM_CHRISTMAS, pPlayer))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]圣诞节活动还没开始或已结束", __FUNCTION__);
		return;
	}

	// 检测是否有此物品
	if (!pPlayer->GetItemNum(dwSnowBallId))
	{
		pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]玩家没有此雪球物品[SnowballId:%d]", __FUNCTION__, dwSnowBallId);
		return;
	}

	// 取boss配置
	const ChristmasBoss_Config *pBossCfg = g_pCfgMgr->GetChristmasBoss(wBossType);
	if (!pBossCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到Boss配置[type:%d,boss_level:%d]", __FUNCTION__, wBossType, tBossData.wBossLevel);
		return;
	}

	// 取boss等级配置
	const ChristmasBossLevel_Config *pBossLvCfg = g_pCfgMgr->GetChristmasBossLevel(wBossType, tBossData.wBossLevel);
	if (!pBossLvCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到Boss等级配置[type:%d,boss_level:%d]", __FUNCTION__, wBossType, tBossData.wBossLevel);
		return;
	}

	// todo::检测此雪球是可以对boss使用

	// 检测BOSS是否已死亡
	DWORD dwCurTime = time(NULL);
	if (tBossData.dwDeadTime && dwCurTime < tBossData.dwDeadTime + pBossCfg->dwReviveTime)
	{
		pPlayer->SendErrorMsg(ERROR_CHRISTMAS_BOSS_DEAD);
		MAPLOG(ERROR_LEVEL, "[%s]Boss还没复活[type:%d,boss_level:%d]", __FUNCTION__, wBossType, tBossData.wBossLevel);
		return;
	}

	// 取雪球配置
	const ChristmasSnowball_Config *pSnowballCfg = g_pCfgMgr->GetChristmasSnowball(dwSnowBallId);
	if (!pSnowballCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到雪球配置[SnowballId:%d]", __FUNCTION__, dwSnowBallId);
		return;
	}

	// 随机伤害奖励
	DWORD dwRandRewardItemId = 0;
	DWORD dwRandRewardItemNum = 0;
	DWORD dwRandValue = g_Rand();
	DWORD dwCurRate = 0;
	int nRandIdx = -1;
	for (size_t i = 0; i < pSnowballCfg->vRewardRateList.size(); i++)
	{
		dwCurRate += pSnowballCfg->vRewardRateList[i];
		if (dwRandValue <= dwCurRate)
		{
			nRandIdx = i;
			break;
		}
	}
	// 有命中
	if (nRandIdx >= 0)
	{
		// 物品ID
		if (pSnowballCfg->vRewardItemList.size() > nRandIdx)
			dwRandRewardItemId = pSnowballCfg->vRewardItemList[nRandIdx];

		// 物品数量
		if (pSnowballCfg->vRewardNumList.size() > nRandIdx)
			dwRandRewardItemNum = pSnowballCfg->vRewardNumList[nRandIdx];
	}

	// 删除已使用的雪球
	pPlayer->RemoveItem(dwSnowBallId, 1, FROM_ATTACK_CHRISTMAS_BOSS);

	// 添加随机奖励物品
	if (dwRandRewardItemId && dwRandRewardItemNum)
		pPlayer->AddObject(dwRandRewardItemId, dwRandRewardItemNum, TRUE, FROM_ATTACK_CHRISTMAS_BOSS);

	// 随机伤害
	DWORD dwDamage = g_Rand(pSnowballCfg->dwDamageMin, pSnowballCfg->dwDamageMax);

	// 死亡
	if (dwDamage > tBossData.dwCurHP)
	{
		tBossData.dwCurHP = 0;
		tBossData.dwDeadTime = dwCurTime;
		tBossData.wBossLevel++;

		// 击杀奖励
		pPlayer->AddObjectList(pBossLvCfg->vKillRewardList, TRUE, FROM_ATTACK_CHRISTMAS_BOSS);

		const ChristmasBossLevel_Config *pNextBossLvCfg = g_pCfgMgr->GetChristmasBossLevel(wBossType, tBossData.wBossLevel);
		if (pNextBossLvCfg)
		{
			tBossData.dwCurHP = pNextBossLvCfg->dwBossHP;
		}

		// 广播死亡
		MSG_S2C_CHRISTMAS_BOSS_DEAD_NOTIFY dead_noftiy;
		dead_noftiy.m_wBossType = wBossType;
		dead_noftiy.m_wBossLevel = tBossData.wBossLevel;
		dead_noftiy.m_dwDeadTime = dwCurTime;
		dead_noftiy.m_dwBossHP = tBossData.dwCurHP;
		dead_noftiy.m_dwKilLerId = pPlayer->GetRoleId();
		SendMsgToBattlePlayer(pPlayer->GetServerId(), &dead_noftiy);

		// 存储,更新boss等级
		int nKey = CHRISTMAS_BOSS_LOWER == wBossType ? GOLBAL_CHRISTMAS_LOWER_BOSS_LEVEL : GOLBAL_CHRISTMAS_HIGH_BOSS_LEVEL;
		g_MapServer.SaveGolbalData(pPlayer->GetServerId(), nKey, tBossData.wBossLevel);
		ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(pPlayer->GetServerId());
		if (CHRISTMAS_BOSS_LOWER == wBossType)
			tGlobalData.wChristmasLowerBossLevel = tBossData.wBossLevel;
		else
			tGlobalData.wChristmasHighBossLevel = tBossData.wBossLevel;

		// 公告整个服务器
		// 处理广播
		CSystemNotice sys_notice(22, pPlayer->GetServerId());
		sys_notice.AddParam(wBossType);
		sys_notice.AddParam(pPlayer->GetRoleName());
		sys_notice.SendMsg();
	}
	// 伤害
	else
	{
		tBossData.dwCurHP -= dwDamage;
	}

	// 广播伤害(todo::调整到死亡消息的后面,满足客户端的表现)
	MSG_S2C_CHRISTMAS_ATTACK_BOSS_NOTIFY damage_notify;
	damage_notify.m_wBossType = wBossType;
	damage_notify.m_dwDamage = dwDamage;
	damage_notify.m_tRewardItem.dwId = dwRandRewardItemId;
	damage_notify.m_tRewardItem.dwNum = dwRandRewardItemNum;
	strcpy_s(damage_notify.m_szAttackerName, sizeof(damage_notify.m_szAttackerName), pPlayer->GetRoleName());
	SendMsgToBattlePlayer(pPlayer->GetServerId(), &damage_notify);

	// 获得积分(积分=伤害)
	PlayerChristmasData &tPlayerChristmasData = pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;
	tPlayerChristmasData.dwCurScore += dwDamage;
	tPlayerChristmasData.dwTotalScore += dwDamage;
	SendChristmasData(pPlayer);

	// 响应
	MSG_S2C_CHRISTMAS_ATTACK_BOSS_RESP resp;
	resp.m_wBossType = wBossType;
	resp.m_dwSnowBallId = dwSnowBallId;
	resp.m_dwDamage = dwDamage;
	resp.m_tRewardItem.dwId = dwRandRewardItemId;
	resp.m_tRewardItem.dwNum = dwRandRewardItemNum;
	pPlayer->SendPacket(&resp);
}

// 领取奖励
void CChristmas::OnNetTakeBossReward(Player *pPlayer, WORD wBossType)
{
	ChristmasBossData &tBossData = GetBossData(pPlayer->GetServerId(), wBossType);

	PlayerChristmasData &tPlayerChristmasData = pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;

	WORD &wBossLevel = CHRISTMAS_BOSS_LOWER == wBossType ? tPlayerChristmasData.wAlreadyTakeLowerBossRewardLevel :
		tPlayerChristmasData.wAlreadyTakeHighBossRewardLevel;

	WORD wNextBossLevel = wBossLevel + 1;

	// 检测是否可领取
	if (wNextBossLevel >= tBossData.wBossLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]此boss等级奖励不可领取[type:%d,boss_level:%d]", __FUNCTION__, wBossType, wNextBossLevel);
		return;
	}

	// 发奖励
	const ChristmasBossLevel_Config *pBossLevelCfg = g_pCfgMgr->GetChristmasBossLevel(wBossType, wNextBossLevel);
	if (!pBossLevelCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到boss等级配置[type:%d,boss_level:%d]", __FUNCTION__, wBossType, wNextBossLevel);
		return;
	}
	pPlayer->AddObjectList(pBossLevelCfg->vBossRewardList, TRUE, FROM_GET_CHRISTMAS_BOSS_REWARD);

	// 更新已领取奖励的进度
	wBossLevel = wNextBossLevel;

	// 响应
	MSG_S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP msg;
	msg.m_wBossType = wBossType;
	pPlayer->SendPacket(&msg);
}

// 删除正在挑战的玩家
void CChristmas::RemoveBattlePlayer(DWORD dwServerId, DWORD dwRoleId)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter = m_BattlePlayerList.find(dwServerId);
	if (iter != m_BattlePlayerList.end())
	{
		iter->second.erase(dwRoleId);
	}
}

// 添加正在挑战的玩家
void CChristmas::AddBattlePlayer(Player *pPlayer)
{
	// todo::此处接口待优化

	if (IsInBattle(pPlayer->GetServerId(), pPlayer->GetRoleId()))
		RemoveBattlePlayer(pPlayer->GetServerId(), pPlayer->GetRoleId());

	map<DWORD, map<DWORD, Player*>>::iterator iter1 = m_BattlePlayerList.find(pPlayer->GetServerId());
	if (iter1 == m_BattlePlayerList.end())
	{
		m_BattlePlayerList.insert(make_pair(pPlayer->GetServerId(), map<DWORD, Player*>()));
		iter1 = m_BattlePlayerList.find(pPlayer->GetServerId());
	}

	iter1->second.insert(make_pair(pPlayer->GetRoleId(), pPlayer));
}

// 是否正在挑战中
bool CChristmas::IsInBattle(DWORD dwServerId, DWORD dwRoleId)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter1 = m_BattlePlayerList.find(dwServerId);
	if (iter1 != m_BattlePlayerList.end())
	{
		map<DWORD, Player*>::iterator iter2 = iter1->second.find(dwRoleId);
		if (iter2 != iter1->second.end())
			return true;
	}
	return false;
}

// 发送圣诞数据
void CChristmas::SendChristmasData(Player *pPlayer)
{
	PlayerChristmasData &tChridtmasData = pPlayer->GetRoleFullInfo().tActivityInfo.tChristmasData;

	ChristmasData &tChristmasData = GetChristmasData(pPlayer->GetServerId());

	MSG_S2C_CHRISTMAS_DATA_RESP msg;
	msg.m_wLowerBossLevel = tChristmasData.tLowerBossData.wBossLevel;
	msg.m_dwLowerBossHP = tChristmasData.tLowerBossData.dwCurHP;
	msg.m_dwLowerBossDeadTime = tChristmasData.tLowerBossData.dwDeadTime;
	msg.m_wHighBossLevel = tChristmasData.tHighBossData.wBossLevel;
	msg.m_dwHighBossHP = tChristmasData.tHighBossData.dwCurHP;
	msg.m_dwHighBossDeadTime = tChristmasData.tHighBossData.dwDeadTime;

	msg.m_wAlreadyTakeLowerBossRewardLevel = tChridtmasData.wAlreadyTakeLowerBossRewardLevel;
	msg.m_wAlreadyTakeHighBossRewardLevel = tChridtmasData.wAlreadyTakeHighBossRewardLevel;
	msg.m_dwTotalScore = tChridtmasData.dwTotalScore;
	msg.m_dwCurScore = tChridtmasData.dwCurScore;

	pPlayer->SendPacket(&msg);
}

// 取BOSS数据
ChristmasBossData& CChristmas::GetBossData(DWORD dwServerId, WORD wBossType)
{
	ChristmasData &tChristmasData = GetChristmasData(dwServerId);

	return CHRISTMAS_BOSS_LOWER == wBossType ? tChristmasData.tLowerBossData : tChristmasData.tHighBossData;
}

// 发送消息给战斗中的玩家
void CChristmas::SendMsgToBattlePlayer(DWORD dwServerId, NetMsgBody *pMsg)
{
	map<DWORD, map<DWORD, Player*>>::iterator iter1 = m_BattlePlayerList.find(dwServerId);
	if (iter1 != m_BattlePlayerList.end())
	{
		map<DWORD, Player*>::iterator iter2 = iter1->second.begin();
		for (; iter2 != iter1->second.end(); ++iter2)
		{
			if (iter2->second)
				iter2->second->SendPacket(pMsg);
		}
	}
}

// 圣诞节数据
ChristmasData& CChristmas::GetChristmasData(DWORD dwServerId)
{
	map<DWORD, ChristmasData>::iterator iter = m_ChristmasDataList.find(dwServerId);
	if (iter == m_ChristmasDataList.end())
	{
		ServerGlobalData &tGlobalData = g_MapServer.GetServerGlobalData(dwServerId);
		ChristmasData tChristmasData;

		// 低级BOSS初始化
		tChristmasData.tLowerBossData.dwDeadTime = 0;
		tChristmasData.tLowerBossData.wBossLevel = max(tGlobalData.wChristmasLowerBossLevel, 1);
		const ChristmasBossLevel_Config *pLowerBossLvCfg = 
			g_pCfgMgr->GetChristmasBossLevel(CHRISTMAS_BOSS_LOWER, tChristmasData.tLowerBossData.wBossLevel);
		if (pLowerBossLvCfg)
		{
			tChristmasData.tLowerBossData.dwCurHP = pLowerBossLvCfg->dwBossHP;
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到Boss配置[type:%d,level:%d]",__FUNCTION__, 
				CHRISTMAS_BOSS_LOWER, tChristmasData.tLowerBossData.wBossLevel);
		}

		// 高级BOSS初始化
		tChristmasData.tHighBossData.dwDeadTime = 0;
		tChristmasData.tHighBossData.wBossLevel = max(tGlobalData.wChristmasHighBossLevel, 1);
		const ChristmasBossLevel_Config *pHighBossLvCfg =
			g_pCfgMgr->GetChristmasBossLevel(CHRISTMAS_BOSS_HIGH, tChristmasData.tHighBossData.wBossLevel);
		if (pHighBossLvCfg)
		{
			tChristmasData.tHighBossData.dwCurHP = pHighBossLvCfg->dwBossHP;
		}
		else
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到Boss配置[type:%d,level:%d]", __FUNCTION__,
				CHRISTMAS_BOSS_LOWER, tChristmasData.tLowerBossData.wBossLevel);
		}

		m_ChristmasDataList.insert(make_pair(dwServerId, tChristmasData));
		iter = m_ChristmasDataList.find(dwServerId);
	}

	return iter->second;
}

// 初始化BOSS
// void CChristmas::InitBoss(WORD wBossType, WORD wBossLevel)
// {
// 	ChristmasBossData &tBossData = GetBossData(wBossType);
// 	tBossData.wBossLevel = wBossLevel;
// 	tBossData.dwDeadTime = 0;
// 	tBossData.dwCurHP = 0;
// 
// 	// 低级BOSS初始化
// 	const ChristmasBossLevel_Config *pLowerBossLvCfg = g_pCfgMgr->GetChristmasBossLevel(wBossType, tBossData.wBossLevel);
// 	if (!pLowerBossLvCfg)
// 	{
// 		MAPLOG(ERROR_LEVEL, "[%s]找不到Boss配置[type:%d,level:%d]", __FUNCTION__,wBossType, tBossData.wBossLevel);
// 		return;
// 	}
// 
// 	tBossData.dwCurHP = pLowerBossLvCfg->dwBossHP;
// }