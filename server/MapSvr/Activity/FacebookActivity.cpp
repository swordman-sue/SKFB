#include "stdafx.h"
#include "FacebookActivity.h"
#include "..\ConfigManager.h"
#include "CommonLogic.h"
#include "..\MapServer.h"
#include "..\Player.h"
#include "ActivityManager.h"
#include "..\SystemNotice\SystemNotice.h"
#include "..\Friend\FriendManager.h"
#include "..\PlayerBaseDataManager.h"
#include "..\AccountInfo.h"


CFacebookActivity::CFacebookActivity()
{
	
}

CFacebookActivity::~CFacebookActivity()
{
	
}

void CFacebookActivity::Init(Player *pPlayer, CActivityManager *pActivityMgr)
{
	m_pPlayer = pPlayer;
	m_pActivityData = &pPlayer->GetRoleFullInfo().tActivityInfo.tFacebookActivityData;
}

// 登录
void CFacebookActivity::OnLogin()
{
	CheckInviteFriendIsLogin();

	CheckRecallFriendIsLogin();
}

// 登出
void CFacebookActivity::OnLogout()
{
	
}

// 请求活动数据
void CFacebookActivity::OnNetActivityData()
{
	SendActivityData();
}

// 领取帐号绑定奖励
void CFacebookActivity::OnNetTakeAccountBindingReward()
{
	// 检测是否已领取奖励
	if (m_pActivityData->wIsAlreadyTakeAccountBindingReward)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]奖励已领取过", __FUNCTION__);
		return;
	}

	// 领取帐号绑定配置
	const FacebookAccountBinding_Config *pAccountBindingCfg = g_pCfgMgr->GetFacebookAccountBinding(1);
	if (!pAccountBindingCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到帐号绑定配置", __FUNCTION__);
		return;
	}

	// 添加奖励
	m_pPlayer->AddObjectList(pAccountBindingCfg->vRewardList, TRUE, FROM_GET_BIND_FB_ACCOUNT_REWARD);

	// 记录已领取
	m_pActivityData->wIsAlreadyTakeAccountBindingReward = true;

	// 响应
	MSG_S2C_TAKE_FACEBOOK_ACCOUNT_BINDING_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 领取分享奖励
void CFacebookActivity::OnNetTakeShareReward()
{
	// 检测是否已通过指定的章节
	if (!m_pActivityData->dwReadyToShareTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]还没通过主线副本指定章节不能领取", __FUNCTION__);
		return;
	}

	// 检测是否已领取过
	if (1 == m_pActivityData->dwReadyToShareTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]分享奖励已领取过", __FUNCTION__);
		return;
	}

	// todo::检测是否过了可领取的时间

	// 取分享配置
	const FacebookShare_Config *pShareCfg = g_pCfgMgr->GetFacebookShare(2);
	if (!pShareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到帐号分享配置", __FUNCTION__);
		return;
	}

	// 添加奖励
	m_pPlayer->AddObjectList(pShareCfg->vRewardList, TRUE, FROM_GET_SHARE_REWARD);

	// 记录已领取
	m_pActivityData->dwReadyToShareTime = 1;

	// 响应
	MSG_S2C_TAKE_FACEBOOK_SHARE_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 领取评论奖励
void CFacebookActivity::OnNetTakeCommendReward()
{
	// 检测是否已通过指定的章节
	if (!m_pActivityData->dwReadyToCommendTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]还没通过主线副本指定章节不能领取", __FUNCTION__);
		return;
	}

	// 检测是否已领取过
	if (1 == m_pActivityData->dwReadyToCommendTime)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]评论奖励已领取过", __FUNCTION__);
		return;
	}

	// todo::检测是否过了可领取的时间

	// 取分享配置
	const FacebookCommend_Config *pShareCfg = g_pCfgMgr->GetFacebookCommend(3);
	if (!pShareCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]取不到帐号评论配置", __FUNCTION__);
		return;
	}

	// 添加奖励
	m_pPlayer->AddObjectList(pShareCfg->vRewardList, TRUE, FROM_GET_COMMAND_REWARD);

	// 记录已领取
	m_pActivityData->dwReadyToCommendTime = 1;

	// 响应
	MSG_S2C_TAKE_FACEBOOK_COMMEND_REWARD_RESP msg;
	m_pPlayer->SendPacket(&msg);
}

// 好友邀请
void CFacebookActivity::OnNetFriendInvite(const FacebookFriendInfo *pFriendInfo)
{
	// 添加已发出邀请的好友列表
	for (WORD i = 0; i < pFriendInfo->wFriendNum; ++i)
	{
		const char *pszFriendAccount = pFriendInfo->arFriendList[i];

		AddInviteRecord(pszFriendAccount);

		MAPLOG(ERROR_LEVEL, "[%s]邀请好友[%s]", m_pPlayer->GetUTF8RoleName(), pszFriendAccount);
	}

	// 每日任务中添加好友邀请任务
	m_pPlayer->OnUpdateDailyMission(27, pFriendInfo->wFriendNum);

	// 成就中添加好友邀请累计次数成就
	m_pPlayer->OnUpdateAchievement(29, pFriendInfo->wFriendNum);

	// 响应
	MSG_S2C_FACEBOOK_FRIEND_INVITE_RESP msg;
	memcpy_s(&msg.m_InviteFriendInfo, sizeof(FacebookFriendInfo), pFriendInfo, sizeof(FacebookFriendInfo));
	m_pPlayer->SendPacket(&msg);
}

// 好友召唤
void CFacebookActivity::OnNetFriendSummon(DWORD *pFriendList, WORD wFriendNum)
{
	CFriendManager *pFriendMgr = m_pPlayer->GetFriendManager();

	DWORD dwCurTime = time(NULL);
	DWORD dwSummonNeedOfflineTime = g_Day2Second(5); // 召回的需要的离线时间

	for (WORD i = 0; i < wFriendNum; ++i)
	{
		DWORD dwFriendId = pFriendList[i];

		// 检测是否好友
		const FriendData *pFriendData = pFriendMgr->GetFriendDataEx(dwFriendId);
		if (!pFriendData)
		{
			m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
			MAPLOG(ERROR_LEVEL, "[%s]目标不是好友[role_id:%d]", __FUNCTION__, dwFriendId);
			return;
		}

		// 是否离线到指定的时间
		const PlayerBaseData *pBaseData = g_GetPlayerBaseData(m_pPlayer->GetServerId(), pFriendData->dwFriendId, false);
		if (!pBaseData)
		{
			MAPLOG(ERROR_LEVEL, "[%s]找不到好友的基础数据[role_id:%d]", __FUNCTION__, dwFriendId);
			continue;
		}
		if (dwCurTime < pBaseData->dwOfflineTime + dwSummonNeedOfflineTime)
		{
			MAPLOG(ERROR_LEVEL, "[%s]好友离线时间不足,不能召回[role_id:%d]", __FUNCTION__, dwFriendId);
			continue;
		}

		// 添加召回好友记录
		AddSummonRecord(dwFriendId);

		MAPLOG(ERROR_LEVEL, "[%s]请求召回好友[%d]", m_pPlayer->GetUTF8RoleName(), dwFriendId);
	}

	// 每日任务中添加好友召回任务目标
	m_pPlayer->OnUpdateDailyMission(26, wFriendNum);

	// 成就中添加好友召回累计成就目标
	m_pPlayer->OnUpdateAchievement(28, wFriendNum);

	// 响应
	MSG_S2C_FACEBOOK_FRIEND_SUMMON_RESP msg;
	msg.m_wFriendNum = wFriendNum;
	memcpy_s(msg.m_arFiendList, sizeof(msg.m_arFiendList), pFriendList, wFriendNum*sizeof(DWORD));
	m_pPlayer->SendPacket(&msg);
}

// 领取好友邀请奖励
void CFacebookActivity::OnNetTakeFriendInviteReward(WORD wInviteNum)
{
	// 检测是否已完成邀请人数
	if (m_pActivityData->wInviteSuccessFriendNum < wInviteNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]成功邀请好友的数量不足[num:%d]", __FUNCTION__, wInviteNum);
		return;
	}

	// 取好友邀请配置
	const FacebookFriendInvite_Config *pInviteCfg = g_pCfgMgr->GetFacebookFriendInvite(wInviteNum);
	if (!pInviteCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]成功邀请好友的数量不足[num:%d]", __FUNCTION__, wInviteNum);
		return;
	}

	// 检测是否已领取过
	if (IsTakeInviteReward(wInviteNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]好友邀请奖励已领取[num:%d]", __FUNCTION__, wInviteNum);
		return;
	}

	// 发放奖励
	m_pPlayer->AddObjectList(pInviteCfg->vRewardList, TRUE, FROM_GET_FRIEND_INVITE_REWARD);

	// 记录已领取
	AddInviteRewardRecord(wInviteNum);

	// 响应
	MSG_S2C_TAKE_FACEBOOK_FRIEND_INVITE_REWARD_RESP msg;
	msg.m_wInviteNum = wInviteNum;
	m_pPlayer->SendPacket(&msg);
}

// 领取好友召唤奖励
void CFacebookActivity::OnNetTakeFriendSummonReward(WORD wSummonNum)
{
	// 检测是否已完成召回人数
	if (m_pActivityData->wSummonSuccessFriendNum < wSummonNum)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]成功召回好友的数量不足[num:%d]", __FUNCTION__, wSummonNum);
		return;
	}

	// 取好友邀请配置
	const FacebookFriendSummon_Config *pSummonCfg = g_pCfgMgr->GetFacebookFriendSummon(wSummonNum);
	if (!pSummonCfg)
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]成功召回好友的数量不足[num:%d]", __FUNCTION__, wSummonNum);
		return;
	}

	// 检测是否已领取过
	if (IsTakeSummonReward(wSummonNum))
	{
		m_pPlayer->SendErrorMsg(ERROR_UNKNOW);
		MAPLOG(ERROR_LEVEL, "[%s]好友召回奖励已领取[num:%d]", __FUNCTION__, wSummonNum);
		return;
	}

	// 发放奖励
	m_pPlayer->AddObjectList(pSummonCfg->vRewardList, TRUE, FROM_GET_FRIEND_SUMMON_REWARD);

	// 记录已领取
	AddInviteRewardRecord(wSummonNum);

	// 响应
	MSG_S2C_TAKE_FACEBOOK_FRIEND_SUMMON_REWARD_RESP msg;
	msg.m_wSummonNum = wSummonNum;
	m_pPlayer->SendPacket(&msg);
}

// 请求已经邀请的好友记录
void CFacebookActivity::OnNetFriendInviteRecord()
{
	// 响应
	MSG_S2C_FACEBOOK_FRIEND_INVITE_RECORD_RESP msg;
	msg.m_InviteFriendRecord.wFriendNum = 0;
	m_pPlayer->SendPacket(&msg);
}


// 发送活动数据
void CFacebookActivity::SendActivityData()
{
	MSG_S2C_FACEBOOK_ACTIVITY_DATA_RESP msg;
	msg.m_tActivityData = *m_pActivityData;
	m_pPlayer->SendPacket(&msg);
}

// 是否已领取邀请奖励
bool CFacebookActivity::IsTakeInviteReward(WORD wInviteNum)
{
	for (WORD i = 0; i < m_pActivityData->wInviteRewardRecordNum; ++i)
	{
		if (m_pActivityData->arInviteRewardRecord[i] == wInviteNum)
			return true;
	}

	return false;
}

// 是否已领取召回奖励
bool CFacebookActivity::IsTakeSummonReward(WORD wSummonNum)
{
	for (WORD i = 0; i < m_pActivityData->wSummonRewardRecordNum; ++i)
	{
		if (m_pActivityData->arSummonRewardRecord[i] == wSummonNum)
			return true;
	}

	return false;
}

// 添加邀请奖励记录
void CFacebookActivity::AddInviteRewardRecord(WORD wInviteNum)
{
	if (m_pActivityData->wInviteRewardRecordNum >= MAX_FACEBOOK_INVITE_REWARD_RECORD)
	{
		MAPLOG(ERROR_LEVEL, "[%s]邀请奖励记录列表已满[num:%d,role_id:%d]", 
			__FUNCTION__, m_pActivityData->wInviteRewardRecordNum, m_pPlayer->GetRoleId());
		return;
	}

	m_pActivityData->arInviteRewardRecord[m_pActivityData->wInviteRewardRecordNum++] = wInviteNum;
}

// 添加召回奖励记录
void CFacebookActivity::AddSummonRewardRecord(WORD wSummonNum)
{
	if (m_pActivityData->wSummonRewardRecordNum >= MAX_FACEBOOK_SUMMON_REWARD_RECORD)
	{
		MAPLOG(ERROR_LEVEL, "[%s]召回奖励记录列表已满[num:%d,role_id:%d]",
			__FUNCTION__, m_pActivityData->wSummonRewardRecordNum, m_pPlayer->GetRoleId());
		return;
	}

	m_pActivityData->arSummonRewardRecord[m_pActivityData->wSummonRewardRecordNum++] = wSummonNum;
}

// 添加召回好友记录
void CFacebookActivity::AddSummonRecord(DWORD dwFriendId)
{
	if (IsSummonRecord(dwFriendId))
		return;

	if (m_pActivityData->wSummonFriendRecordNum >= MAX_FRIEND_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]召回记录列表已满",__FUNCTION__);
		return;
	}

	RecallFriendData &tRecallData = m_pActivityData->arSummonFriendRecord[m_pActivityData->wSummonFriendRecordNum++];
	tRecallData.dwFriendId = dwFriendId;
	tRecallData.dwTime = time(NULL);
}

// 删除召回记录
void CFacebookActivity::DelSummonRecord(DWORD dwFriendId)
{
	for (WORD i = 0; i < m_pActivityData->wSummonFriendRecordNum; i++)
	{
		if (m_pActivityData->arSummonFriendRecord[i].dwFriendId == dwFriendId)
		{
			m_pActivityData->arSummonFriendRecord[i] = m_pActivityData->arSummonFriendRecord[--m_pActivityData->wSummonFriendRecordNum];
			break;
		}
	}
}

// 添加邀请好友记录
void CFacebookActivity::AddInviteRecord(const char *pszFacebookAccount)
{
	if (IsInviteRecord(pszFacebookAccount))
		return;

	FacebookFriendInfo &tFriendInfo = m_pActivityData->tSendInviteFriendInfo;

	if (tFriendInfo.wFriendNum >= MAX_FACEBOOK_FRIEND_NUM)
	{
		MAPLOG(ERROR_LEVEL, "[%s]邀请记录列表已满", __FUNCTION__);
		return;
	}

	strcpy_s(tFriendInfo.arFriendList[tFriendInfo.wFriendNum], 
		sizeof(tFriendInfo.arFriendList[tFriendInfo.wFriendNum]), pszFacebookAccount);

	tFriendInfo.wFriendNum++;
}

// 删除邀请记录
void CFacebookActivity::DelInviteRecord(const char *pszFacebookAccount)
{
	DWORD dwPlatformType = m_pPlayer->GetAccountInfo()->GetChannelUID();
	FacebookFriendInfo &tInviteFriendInfo = m_pActivityData->tSendInviteFriendInfo;
	for (WORD i = 0; i < tInviteFriendInfo.wFriendNum; ++i)
	{
		// 检测是否相等
		if (strcmp(pszFacebookAccount, tInviteFriendInfo.arFriendList[i]) == 0)
		{
			strcpy_s(tInviteFriendInfo.arFriendList[i], sizeof(tInviteFriendInfo.arFriendList[i]),
				tInviteFriendInfo.arFriendList[tInviteFriendInfo.wFriendNum - 1]);
			tInviteFriendInfo.wFriendNum--;
			break;
		}
	}
}

// 是否召回记录
bool CFacebookActivity::IsSummonRecord(DWORD dwFriendId)
{
	for (WORD i = 0; i < m_pActivityData->wSummonFriendRecordNum; ++i)
	{
		if (m_pActivityData->arSummonFriendRecord[i].dwFriendId == dwFriendId)
			return true;
	}
	return false;
}

// 是否邀请记录
bool CFacebookActivity::IsInviteRecord(const char *pszFacebookAccount)
{
	for (WORD i = 0; i < m_pActivityData->tSendInviteFriendInfo.wFriendNum; ++i)
	{
		if (strcmp(m_pActivityData->tSendInviteFriendInfo.arFriendList[i], pszFacebookAccount) == 0)
			return true;
	}
	return false;
}

// 检测被邀请好友是否登录
void CFacebookActivity::CheckInviteFriendIsLogin()
{
	vector<string> vDeleteList;
	DWORD dwPlatformType = m_pPlayer->GetAccountInfo()->GetChannelUID();
	FacebookFriendInfo &tInviteFriendInfo = m_pActivityData->tSendInviteFriendInfo;
	for (WORD i = 0; i < tInviteFriendInfo.wFriendNum; ++i)
	{
		char *pAccount = tInviteFriendInfo.arFriendList[i];
		if (PlayerBaseDataManager::Instance()->IsInvalidAccount(dwPlatformType, pAccount))
		{
			vDeleteList.push_back(pAccount);
		}
	}

	// 删除已经邀请进来的
	for (vector<string>::iterator iter = vDeleteList.begin(); iter != vDeleteList.end(); ++iter)
	{
		DelInviteRecord(iter->c_str());
	}

	m_pActivityData->wInviteSuccessFriendNum += vDeleteList.size();
}

// 检测被召回好友是否登录
void CFacebookActivity::CheckRecallFriendIsLogin()
{
	DWORD dwCurTime = time(NULL);
	vector<DWORD> vDeleteList;
	for (WORD i = 0; i < m_pActivityData->wSummonFriendRecordNum; ++i)
	{
		DWORD dwFriendId = m_pActivityData->arSummonFriendRecord[i].dwFriendId;
		const PlayerBaseData *pBaseData = PlayerBaseDataManager::Instance()->GetPlayerBaseDataById(dwFriendId);
		if (!pBaseData)
			continue;

		// 在线 或者 离线时间小于5天
		if (!pBaseData->dwOfflineTime || g_GetPassTime(pBaseData->dwOfflineTime, dwCurTime) < g_Day2Second(5))
		{
			vDeleteList.push_back(dwFriendId);
		}
	}

	// 删除已经邀请进来的
	for (vector<DWORD>::iterator iter = vDeleteList.begin(); iter != vDeleteList.end(); ++iter)
	{
		DelSummonRecord(*iter);
	}

	m_pActivityData->wSummonSuccessFriendNum += vDeleteList.size();
}

// 设置准备评论时间
void CFacebookActivity::SetReadyToCommendTime(DWORD dwTime)
{
	m_pActivityData->dwReadyToCommendTime = dwTime;
}

// 设置准备分享时间
void CFacebookActivity::SetReadyToShareTime(DWORD dwTime)
{
	m_pActivityData->dwReadyToShareTime = dwTime;
}