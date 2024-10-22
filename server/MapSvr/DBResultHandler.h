#pragma once

class QueryResult;
class AccountInfo;

class DBResultHandler
{
public:
	DBResultHandler(void);
	~DBResultHandler(void);

public:
	// 检测账号结果
	static void ProcessCheckAccountQueryResult(QueryResult * pData);	

	// 检测账号结果(sdk)
	static void ProcessSDKAccountCheckResult(QueryResult * pData);

	// 创建账号结果
	static void ProcessCreateAccountQueryResult(QueryResult * pData);

	// 创建账号结果(sdk)
	static void ProcessSDKCreateAccountResult(QueryResult * pData);

	// 查询服务器角色结果
	static void ProcessSelectZoneRoleQueryResult(QueryResult* pData);

	// 创建角色结果
	static void ProcessCreateCharQueryResult(QueryResult* pData);		

	// 选择角色结果
	static void ProcessSelectCharInfoQueryResult(QueryResult* pData);	

	// 更新角色信息
	static void ProcessUpdateCharQueryResult(QueryResult* pData);

	// 更新角色属性
	//static void DoUpdateRolePropertyQueryResult(QueryResult * pData);

	// 帐号密码修改结果
	static void ProcessAccountPasswordModifyResult(QueryResult * pData); 

	// 帐号手机号修改结果
	static void ProcessAccountMobileChangeResult(QueryResult * pData);

	// 更新最近登录服务器
	static void ProcessUpdateLastLoginServer(QueryResult * pData);


	// 存储全局数据
	static void SaveGolbalDataResult(QueryResult * pData);
	
	// 加载全局数据
	static void LoadGolbalDataResult(QueryResult * pData);

	// 加载角色基本数据列表
	static void LoadRoleBaseDataListResult(QueryResult * pData);

	// 更新好友数据
	static void UpdateFriendDataResult(QueryResult * pData);

	// 存储竞技场数据结果
	static void SaveArenaDataResult(QueryResult * pData);

	// 加载竞技场数据结果
	static void LoadArenaDataResult(QueryResult * pData);

	// 存储角色排行榜数据结果
	static void SaveRoleRankDataResult(QueryResult * pData);

	// 加载角色排行榜数据结果
	static void LoadRoleRankDataResult(QueryResult * pData);

	// 存储邮件数据
	static void SaveEmailDataResult(QueryResult * pData);

	// 加载并删除GM事件结果
	static void LoadAndDeleteGMEventResult(QueryResult * pData);

	// 加载服务器列表结果
	static void LoadServerListResult(QueryResult * pData);

	// 存储竞技场战斗记录数据结果
	static void SaveArenaBattleRecordResult(QueryResult * pData);

	// 加载竞技场数据结果
	static void LoadArenaBattleRecordResult(QueryResult * pData);

	// 存储角色货币日志结果
	static void SaveRoleCurrencyLogsResult(QueryResult * pData);

	// 存储领地数据结果
	static void SaveLandDataResult(QueryResult * pData);

	// 加载领地数据结果
	static void LoadLandDataResult(QueryResult * pData);
	
	// 竞技场排行榜查看列表更新结果
	static void UpdateArenaRankViewListResult(QueryResult * pData);

	// 存贷留言结果
	static void SaveLeaveMessageResult(QueryResult * pData);

	// 存储奖励中心数据结果
	static void SaveRewardCenterDataResult(QueryResult * pData);

	// 更新称号临时数据结果
	static void UpdateRoleTempDataResult(QueryResult * pData);

	// 存储角色下线记录结果
	static void SaveRoleOfflineRecordResult(QueryResult * pData);

	// 存储角色下线记录结果
	static void SaveRoleLevelRecordResult(QueryResult * pData);

	// 存储角色充值记录结果
	static void SaveRoleRechargeRecordResult(QueryResult * pData);

	// 存储关卡失败记录结果
	static void SaveRoleMapLoseRecordResult(QueryResult * pData);

	// 存储关卡失败记录结果
	static void SaveRoleFirstPassMapRecordResult(QueryResult * pData);

	// 存储商店购买记录结果
	static void SaveShopBuyRecordResult(QueryResult * pData);

	// 存储物品丢失记录结果
	static void SaveItemLoseRecordResult(QueryResult * pData);

	// 存储物品操作记录结果
	static void SaveItemOperationRecordResult(QueryResult * pData);

	// 存储在线玩家数量记录结果
	static void SaveOfflinePlayerNumRecordResult(QueryResult * pData);
		
	// 存储工会数据结果
	static void SaveGuildDataResult(QueryResult * pData);

	// 加载工会数据结果
	static void LoadGuildDataResult(QueryResult * pData);

	// 存储聊天记录
	static void SaveChatRecordResult(QueryResult * pData);

	// 加载聊天记录
	static void LoadChatRecordResult(QueryResult * pData);

	// 存储日志结果
	static void SaveLogsResult(QueryResult * pData);

	// 存储关卡评论结果
	static void SaveMapCommentResult(QueryResult * pData);

	// 加载关卡评论结果
	static void LoadMapCommentResult(QueryResult * pData);

	// 更新帐号结果
	static void UpdateAccountLockResult(QueryResult * pData);

	// 存储王者赛数据结果
	static void SaveKingMatchDataResult(QueryResult * pData);
	
	// 加载王者赛数据结果
	static void LoadKingMatchDataResult(QueryResult * pData);

	// 存储积分赛数据结果
	static void SaveScoreMatchDataResult(QueryResult * pData);
	
	// 加载积分赛数据结果
	static void LoadScoreMatchDataResult(QueryResult * pData);

	// 存储组队副本数据结果
	static void SaveTeamDungeonRankDataResult(QueryResult * pData);

	// 加载组队副本数据结果
	static void LoadTeamDungeonRankDataResult(QueryResult * pData);

	// 存储转盘数据结果
	static void SaveTurntableDataResult(QueryResult * pData);

	// 加载转盘数据结果
	static void LoadTurntableDataResult(QueryResult * pData);

	// 更新新手引导结果
	static void UpdateNewRoleGuideResult(QueryResult * pData);

	// 存储资源战数据结果
	static void SaveResourceWarDataResult(QueryResult * pData);

	// 加载资源战数据结果
	static void LoadResourceWarDataResult(QueryResult * pData);

	// 存储工会战数据结果
	static void SaveGuildWarDataResult(QueryResult * pData);

	// 加载工会战数据结果
	static void LoadGuildWarDataResult(QueryResult * pData);

	// 检索激活码数据结果
	static void SelectActivityCodeResult(QueryResult * pData);

	// 更新激活码数据结果
	static void UpdateActivityCodeResult(QueryResult * pData);

	// 游戏统计事件结果
	static void GameStatisticsEventResult(QueryResult * pData);

	// 存储GM操作记录
	static void GMOperationRecordSaveResult(QueryResult * pData);
	
};
