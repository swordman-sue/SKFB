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


// ��������
void CUnlockActivity::OnNetActivityData()
{
	SendActivityData();
}

// �̳��������
void CUnlockActivity::OnNetShopScoreFinish()
{
	// ȡ����
	const UnlockActivityReward_Config *pRewardCfg = g_pCfgMgr->GetUnlockActivityReward(1);
	if (!pRewardCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���unlock���������[reward:1]", __FUNCTION__);
		return;
	}

	// ����Ƿ���ȡ��
	if (m_pActivityData->wIsScoreForShop)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���unlock���������[reward:1]", __FUNCTION__);
		return;
	}

	m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_SHOP_SCORE_FINISH_REWARD);

	m_pActivityData->wIsScoreForShop = true;

	SendActivityData();

	MSG_S2C_SHOP_SCORE_FINISH_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// �ۿ���Ƶ
void CUnlockActivity::OnNetWatchingVideoNotify()
{
	WORD wNewWatchingVideosTimes = m_pActivityData->wWatchingVideosTimes + 1;
	const UnlockWatchingVideoReward_Config *pRewardCfg = g_pCfgMgr->GetUnlockWatchingVideoReward(wNewWatchingVideosTimes);
	if (!pRewardCfg)
	{
		MAPLOG(ERROR_LEVEL, "[%s]�Ҳ���unlock�ۿ���Ƶ��������[WatchingTimes:%d]", __FUNCTION__, wNewWatchingVideosTimes);
		return;
	}

	// ��ӽ���
	m_pPlayer->AddObjectList(pRewardCfg->vRewardItemList, TRUE, FROM_WATCHING_VIDEO_REWARD);

	// ���¹ۿ�����
	m_pActivityData->wWatchingVideosTimes = wNewWatchingVideosTimes;

	SendActivityData();

	// ��Ӧ
	MSG_S2C_WATCHING_VIDEO_NOTIFY_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// ���ͻ����
void CUnlockActivity::SendActivityData()
{
	MSG_S2C_UNLOCK_ACTIVITY_DATA_RESP msg;
	msg.m_ActivityData = *m_pActivityData;
	m_pPlayer->SendPacket(&msg);
}