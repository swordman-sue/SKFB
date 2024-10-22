#include "stdafx.h"
#include "UnlockActivity.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\MapServer.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "..\AccountInfo.h"


CUnlockActivity::CUnlockActivity()
{
	
}

CUnlockActivity::~CUnlockActivity()
{
	
}

void CUnlockActivity::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityMgr = pActivityMgr;
	m_pActivityData = &pPlayer->GetRoleFullInfo().tActivityInfo.tUnlockActivityData;
}


// 请求活动数据
void CUnlockActivity::OnNetActivityData()
{
	SendActivityData();
}

// 商城评分完成
void CUnlockActivity::OnNetShopScoreFinish()
{
	// 取配置
	const UnlockActivityReward_Config *pRewardCfg = g_pCfgMgr->GetUnlockActivityReward(1);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]找不到unlock活动奖励配置[reward:1]", __FUNCTION__);
		return;
	}

	// 检测是否领取过
	if (m_pActivityData->wIsScoreForShop)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到unlock活动奖励配置[reward:1]", __FUNCTION__);
		return;
	}

	m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_SHOP_SCORE_FINISH_REWARD);

	m_pActivityData->wIsScoreForShop = true;

	SendActivityData();

	MSG_S2C_SHOP_SCORE_FINISH_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 观看视频
void CUnlockActivity::OnNetWatchingVideoNotify()
{
	WORD wNewWatchingVideosTimes = m_pActivityData->wWatchingVideosTimes + 1;
	const UnlockWatchingVideoReward_Config *pRewardCfg = g_pCfgMgr->GetUnlockWatchingVideoReward(wNewWatchingVideosTimes);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]找不到unlock观看视频奖励配置[WatchingTimes:%d]", __FUNCTION__, wNewWatchingVideosTimes);
		return;
	}

	// 添加奖励
	m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_WATCHING_VIDEO_REWARD);

	// 更新观看次数
	m_pActivityData->wWatchingVideosTimes = wNewWatchingVideosTimes;

	SendActivityData();

	// 响应
	MSG_S2C_WATCHING_VIDEO_NOTIFY_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 发送活动数据
void CUnlockActivity::SendActivityData()
{
	MSG_S2C_UNLOCK_ACTIVITY_DATA_RESP msg;
	msg.m_ActivityData = *m_pActivityData;
	m_pPlayer->SendPacket(&msg);
}