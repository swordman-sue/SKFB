#include "stdafx.h"
#include "HangUpDropMgr.h"
#include "..\Player.h"
#include "CommonLogic.h"
#include "..\ConfigManager.h"

CHangUpDropMgr::CHangUpDropMgr()
{


}

CHangUpDropMgr::~CHangUpDropMgr()
{

}

void CHangUpDropMgr::Init(Player *pPlayer)
{
	m_pPlayer = pPlayer;
	m_pHangUpDropData = &pPlayer->GetRoleFullInfo().tSecondInfo.tHangUpDropData;
}

// 请求挂机掉落数据
void CHangUpDropMgr::OnNetHangUpDropData()
{
	CheckAndInitDropData();

	SendHangUpDropData();
}

// 请求领取奖励
void CHangUpDropMgr::OnNetObtainReward()
{
	// 检测系统开放等级
	int nOpenLevel = g_pCfgMgr->GetSystemOpenLevel(160);
	if (m_pPlayer->GetLevel() < nOpenLevel)
	{
		MAPLOG(ERROR_LEVEL, "[%s]开放等级!", __FUNCTION__);
		return;
	}

	const vector<HangUpDrop_Config>* pHangUpDroListCfg = g_pCfgMgr->GetHangUpDropList();
	if (!pHangUpDroListCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到挂机掉落配置!", __FUNCTION__);
		return;
	}

	CheckAndInitDropData();

	DWORD dwMaxRewardTime = g_Hour2Second(g_GetConfigValue(316));
	DWORD dwCurTime = time(NULL);
	DWORD dwPassTime = dwCurTime - m_pHangUpDropData->dwLastObtainRewardTime;

	// 收获的时间间隔最大为12小时,如果超过则做数据纠正
	if (dwPassTime > dwMaxRewardTime){
		dwPassTime = dwMaxRewardTime;
	}

	int nRewardTimes = g_DivideBy(dwPassTime, 60);

	// 随机掉落物品
	m_TmpRewardItemList.clear();
	for (int i = 0; i < nRewardTimes; ++i)
	{
		vector<HangUpDrop_Config>::const_iterator iter = pHangUpDroListCfg->begin();
		for (; iter != pHangUpDroListCfg->end(); ++iter)
		{
			if (g_Rand() <= iter->nDropRate){
				AddRewardItem(iter->dwDropItemId, iter->dwDropItemNum);
			}
		}
	}

	// 添加获得的奖励
	for (map<DWORD, DWORD>::iterator iter = m_TmpRewardItemList.begin(); iter != m_TmpRewardItemList.end(); ++iter)
	{
		m_pPlayer->AddObject(iter->first, iter->second, TRUE, FROM_HANG_UP_DROP_OBTAIN);
	}

	// 防止没满60秒就请求过来，时间会被重置
	if (nRewardTimes)
		m_pHangUpDropData->dwLastObtainRewardTime = dwCurTime;

	SendHangUpDropData();

	// 响应
	MSG_S2C_HANG_UP_DROP_REWARD_OBTAIN_RESP msg;
	for (map<DWORD, DWORD>::iterator iter = m_TmpRewardItemList.begin(); iter != m_TmpRewardItemList.end(); ++iter)
	{
		msg.m_vRewardItemList.push_back(Item(iter->first, iter->second));
	}
	m_pPlayer->SendPacket(&msg);
}

// 发送挂机掉落数据
void CHangUpDropMgr::SendHangUpDropData()
{
	MSG_S2C_HANG_UP_DROP_DATA_RESP msg;
	msg.m_HangUpDropData = *m_pHangUpDropData;
	m_pPlayer->SendPacket(&msg);
}

// 添加奖励物品数据
void CHangUpDropMgr::AddRewardItem(DWORD dwItemId, DWORD dwItemNum)
{
	map<DWORD, DWORD>::iterator iter = m_TmpRewardItemList.find(dwItemId);
	if (iter != m_TmpRewardItemList.end()){
		iter->second += dwItemNum;
	}
	else{
		m_TmpRewardItemList.insert(make_pair(dwItemId, dwItemNum));
	}
}

// 检测并初始化掉落数据
void CHangUpDropMgr::CheckAndInitDropData()
{
	// 处理数据初始化
	if (!m_pHangUpDropData->dwLastObtainRewardTime && 
		m_pPlayer->GetLevel() >= g_pCfgMgr->GetSystemOpenLevel(160))
	{
		m_pHangUpDropData->dwLastObtainRewardTime = time(NULL);
	}
}