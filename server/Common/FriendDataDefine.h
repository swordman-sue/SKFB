#ifndef _FRIEND_DATA_DEFINE_H_
#define _FRIEND_DATA_DEFINE_H_

#include "BaseDataDefine.h"

/*---------------------------------------------------------------------------------------------*/
#pragma pack(push,1)


// 好友数据
struct FriendData
{
	DWORD dwFriendId;			// 好友ID
	WORD wIsGiveEnergy;			// 是否已赠送精力(我赠送奖励给对方的标志)
	WORD wTakeEnergyState;		// 领取精力状态(0=不能领取,1=可领取,2=已领取) (我领取对方奖励的状态)
	WORD wHelpBattleTimes;		// 帮我助战次数		
};


// 好友客户端数据
struct ClientFriendData : public PlayerCommonData
{
	BYTE byIsGiveEnergy;					// 是否已赠送精力
	BYTE byTakeEnergyState;					// 领取精力状态(0=不能领取,1=可领取,2=已领取)
	//char szAccount[MAX_ACCOUNT_LEN];		// 帐号
};

// 玩家好友数据
struct PlayerFriendData
{
	FriendData arFriendList[MAX_FRIEND_NUM];	// 好友列表
	WORD wFriendNum;							// 好友数量

	DWORD arBlackList[MAX_BLACK_NUM];			// 黑名单列表
	WORD wBlackNum;								// 黑名单数量

	DWORD arApplyList[MAX_FRIEND_APPLY_NUM];	// 申请列表
	WORD wApplyNum;							// 申请列表数量

	WORD wGetEnergyTimes;						// 领取精力次数
};

#pragma pack(pop)
#endif
