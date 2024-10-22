/**\file
小功能的逻辑管理器
\author Kevin
\date 2016/11/10
\note
*/

#pragma once
#include "DataDefine.h"
class Player;

// 激活码信息
// 1.激活码ID 2.奖励ID 3.是否已被领取

// 激活码奖励信息
// 1.奖励ID 2.类型(个人,共公) 3.有效时间 4.物品ID 

class CLogicManager
{
public:
	CLogicManager();
	~CLogicManager();

	void Init(Player *pPlayer);

	void OnNewDay();

////// 角色属性模块
public:
	// 请求玩家详情
	void OnNetRoleDetails(DWORD dwTargetId, BYTE byType);

	// 请求属性值恢复
	void OnNetPropertyRecover(BYTE byPropertyType);

	// 替换头像
	void OnNetReplaceHeadImage(DWORD dwHeadImageId);

	// 角色名称修改
	void OnNetRoleNameModify(const char *pszRoleName);

	// 开始游戏
	void OnNetStartGame();

	// 引导更新
	void OnNetGuideUpdate(int nKey, int nValue);

	// 玩家留言
	void OnNetLeaveMessage(const char *pszContact, const char *pszMessage);

	// 请求助战目标列表(todo::旧的玩家,目前已不使用,但先保留)
	void OnNetHelpBattleTargetListEx();

	// 请求助战目标列表(新逻辑)
	void OnNetHelpBattleTargetList();

	// 领取活动礼包(todo::目前暂时不使用)
	void OnNetTakeActivityGiftEx(const char *pszCode);

	// 领取活动礼包(todo::目前暂时不使用)
	void OnNetTakeActivityGift(const char *pszCode);

	// 领取礼包码
	void OnNetTakeActivityGift_New(const char *pszCode);

	// 处理领取活动礼包
	void DoTakeActivityGift(const char *pszCode, bool bIsExist, DWORD dwLimitTime, DWORD dwRewardId, WORD wStatus);

	// 请求活动公告
	void OnNetActivityNotice();

	// 进入场景
	void OnNetEnterScene(WORD wSceneType, DWORD dwContext);

	// 离开场景
	void OnNetLeaveScene(WORD wSceneType);

	// 行走
	void OnNetGoTo(WORD wSceneType, MapPos tSrcPos, MapPos tDestPos);

	// 物品锁定
	void OnNetLockItem(WORD wSceneType, DWORD dwSceneItemUId);

	// 物品拾取
	void OnNetPickUpItem(WORD wSceneType, DWORD dwSceneItemUId);

	// 新手引导更新
	void OnNetNewRoleGuideUpdate(int nKey, int nValue);


public:
	// 领取礼包码回调
	void OnTakeActivityGiftCallback(const char *pszCode, int nResult, int nStatus, DWORD dwRewardId, DWORD dwExpireTime, int nBatch, int nType);


// 关卡评论
public:
	// 请求关卡评论列表
	void OnNetMapCommentList(WORD wChapter);

	// 关卡评论
	void OnNetMapComment(WORD wChapter, const char *pszText);

	// 关卡评论点赞
	void OnNetMapCommentLike(WORD wChapter, WORD wCommentId);

	// 发送评论列表
	void SendMapCommentList(WORD wChapter);

// 战力头衔(爵位)
public:
	// 激活爵位
	void OnNetAcivateNobility();

// 扭蛋抽奖
public:
	// 扭蛋抽奖
	void OnNetNDLottery(WORD wTimes, BYTE byIsCostItem);

	// 请求扭蛋抽奖记录
	void OnNetNDLotteryRecordList();

public:
	void SetGuide(int nKey, int nValue);

private:
	Player *m_pPlayer;

	UINT64 m_lLastLeaveMessageTime;	// 上次留言时间

	DWORD m_dwLastTakeActivityGiftTime;	// 领取礼包码时间
};