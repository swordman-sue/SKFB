/**\file
好友管理器
\author Kevin
\date 2016/8/30
\note
*/

#pragma once
#include "DataDefine.h"
class Player;
class CMainFB;

class CFriendManager
{
	friend class CMainFB;

public:
	CFriendManager();
	~CFriendManager();

	void Init(Player *pPlayer);

	void OnNewDay();

public:
	// 请求好友列表
	void OnFriendList();										

	// 请求黑名单列表
	void OnBlackList();										

	// 请求申请列表
	void OnApplyList();										

	// 请求推荐列表
	void OnRecommendList();									

	// 添加好友
	void OnFriendAdd(DWORD dwTargetId);							

	// 删除好友(蛋疼逻辑)
	void OnFriendDel(DWORD dwTargetId);							

	// 申请好友回复(蛋疼逻辑)
	void OnFriendApplyReply(DWORD dwTargetId, BYTE byIsAccept);	

	// 添加黑名单(蛋疼逻辑)
	void OnBlackAdd(DWORD dwTargetId);							

	// 删除黑名单
	void OnBlackDel(DWORD dwTargetId);							

	// 赠送精力
	void OnEnergyGive(DWORD dwTargetId);						

	// 领取精力
	void OnTakeEnergy(DWORD *pTargetList, BYTE byTargetNum);

	// 查找目标
	void OnNetTargetFind(const char *pszTargetName);

public:
	// 添加好友申请数据
	bool AddApplyData(DWORD dwTargetId);

	// 是否在申请列表
	bool IsInApplyList(DWORD dwRoleId);

	// 添加赠送精力数据
	void AddGiveEnergyData(DWORD dwRoleId);

	// 设置精力领取状态
	void SetGetEnergyState(DWORD dwRoleId, BYTE byState);

	// 删除好友
	void RemoveFriend(DWORD dwRoleId);

	// 删除申请好友数据
	void RemoveApplyData(DWORD dwRoleId);

	// 添加好友数据
	void AddFriendData(DWORD dwFriendId);

	// 是否好友列表已满
	bool IsFriendListFull();

	// 更新好友数据回调
	void UpdateFriendDataCallback(BYTE byType, const FriendData &tFriendData);

	// 好友数据更新通知
	void FriendDataUpdateNotify(DWORD dwFriendRoleId, BYTE byType = 1);

	// 好友数据更新通知
	void FriendDataUpdateNotify(const FriendData &tFriendData, BYTE byType = 1);

	// 是否好友
	bool IsFriend(DWORD dwTargetId);

	// 是否黑名单
	bool IsBlack(DWORD dwTargetId);

	// 取好友列表
	const FriendData* GetFrindList() { return m_pFriendInfo->arFriendList; };

	// 取好友数量
	WORD GetFriendNum() { return m_pFriendInfo->wFriendNum; };

	// 取好友数量上限
	BYTE GetFriendNumMax();

	// 取好友数量上限
	static BYTE GetFriendNumMax(WORD wRoleLevel);

	// 更新数据
	bool UpdateClientFriendData(ClientFriendData &tFriendData, DWORD dwFriendId);

	// 在数据里更新好友数据
	void UpateFriendDataInDB(DWORD dwUpateRoleId, const FriendData &tFriendData, BYTE byOperateType);

	// 发送黑名单列表
	void SendBlackList();

	// 是否有红点
	bool IsHaveRedPoint();

	// 取好友数据(todo::接口命名日后调整)
	const FriendData* GetFriendDataEx(DWORD dwRoleId);
protected:

	// 取好友数据
	FriendData* GetFriendData(DWORD dwRoleId);

	// 删掉黑名单数据
	void RemoveBlackData(DWORD dwRoleId);

	// 添加黑名单
	void AddBlackData(DWORD dwTargetId);
private:
	Player *m_pPlayer;
	PlayerFriendData *m_pFriendInfo;
};