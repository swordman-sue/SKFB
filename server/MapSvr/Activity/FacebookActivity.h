/**\file
facebook活动逻辑类
\author Kevin
\date 2017/12/13
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CActivityManager;

class CFacebookActivity
{
public:
	CFacebookActivity();
	~CFacebookActivity();

	void Init(Player *pPlayer, CActivityManager *pActivityMgr);

	// 登录
	void OnLogin();

	// 登出
	void OnLogout();

public:
	// 请求活动数据
	void OnNetActivityData();

	// 领取帐号绑定奖励
	void OnNetTakeAccountBindingReward();

	// 领取分享奖励
	void OnNetTakeShareReward();

	// 领取评论奖励(强行加入fb活动)
	void OnNetTakeCommendReward();

	// 好友邀请
	void OnNetFriendInvite(const FacebookFriendInfo *pFriendInfo);

	// 好友召唤
	void OnNetFriendSummon(DWORD *pFriendList, WORD wFriendNum);

	// 领取好友邀请奖励
	void OnNetTakeFriendInviteReward(WORD wInviteNum);

	// 领取好友召唤奖励
	void OnNetTakeFriendSummonReward(WORD wSummonNum);

	// 请求已经邀请的好友记录
	void OnNetFriendInviteRecord();

public:
	// 设置准备评论时间
	void SetReadyToCommendTime(DWORD dwTime);

	// 设置准备分享时间
	void SetReadyToShareTime(DWORD dwTime);

protected:

	// 发送活动数据
	void SendActivityData();

	// 是否已领取邀请奖励
	bool IsTakeInviteReward(WORD wInviteNum);

	// 是否已领取召回奖励
	bool IsTakeSummonReward(WORD wSummonNum);

	// 添加邀请奖励记录
	void AddInviteRewardRecord(WORD wInviteNum);

	// 添加召回奖励记录
	void AddSummonRewardRecord(WORD wSummonNum);

	// 添加召回记录
	void AddSummonRecord(DWORD dwFriendId);

	// 删除召回记录
	void DelSummonRecord(DWORD dwFriendId);

	// 添加邀请记录
	void AddInviteRecord(const char *pszFacebookAccount);

	// 删除邀请记录
	void DelInviteRecord(const char *pszFacebookAccount);

	// 是否召回记录
	bool IsSummonRecord(DWORD dwFriendId);

	// 是否邀请记录
	bool IsInviteRecord(const char *pszFacebookAccount);
	
	// 检测被邀请好友是否登录
	void CheckInviteFriendIsLogin();

	// 检测被召回好友是否登录
	void CheckRecallFriendIsLogin();

protected:
	Player *m_pPlayer;
	FacebookActivityData *m_pActivityData; 
};