#pragma once

class SqlDatabase;
class QueryResult;

class DBThreadHandler
{
public:
	DBThreadHandler(void){}
	~DBThreadHandler(void){}
public:
/////////////////////////////////////
// 帐号及角色相关
	// 检测账号
	static void DoCheckAccountQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// 帐号检测(sdk)(todo::待优化)
	static void DoSDKAccountCheck(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 创建账号
	static void DoCreateAccountQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 创建账号(sdk)
	static void DoSDKCreateAccount(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 查询分区角色(todo::待优化)
	static void DoSelectZoneRoleQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 创建角色
	static void DoCreateRoleQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 查询角色信息
	static void DoSelectRoleInfoQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新角色信息
	static void DoUpdateRoleInfoQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新角色属性
	//static void DoUpdateRolePropertyQuery(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 帐号密码修改
	static void ProcessAccountPasswordModify(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 手机号修改
	static void ProcessAccountMobileChange(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新最近登录的服务器
	static void DoUpateLastLoginServer(SqlDatabase* pSqlDatabase, QueryResult * pData);


	// 存储全局数据
	static void SaveGolbalData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载全局数据
	static void LoadGolbalData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载角色基本数据列表
	static void LoadRoleBaseDataList(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新好友数据
	static void UpdateFriendData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储竞技场数据
	static void SaveArenaData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载竞技场数据
	static void LoadArenaData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储排行榜数据
	static void SaveRoleRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载排行榜数据
	static void LoadRoleRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储邮件数据
	static void SaveEmailData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载并删除GM事件
	static void  LoadAndDeleteGMEvent(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载服务器列表
	static void LoadServerList(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储竞技场战斗记录数据
	static void SaveArenaBattleRecordData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载竞技场战斗记录数据
	static void LoadArenaBattleRecordData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存贷角色货币日志
	static void SaveRoleCurencyLogs(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储领地数据
	static void SaveLandData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载领地数据
	static void LoadLandData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 竞技场排行榜查看列表更新
	static void UpdateArenaRankViewList(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储留言
	static void SaveLeaveMessage(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储奖励中心数据
	static void SaveRewardCenterData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新角色临时数据
	static void UpdateRoleTempData(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// 存储角色下线记录
	static void SaveRoleOfflineRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储角色等级记录
	static void SaveRoleLevelRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储角色充值记录
	static void SaveRoleRechargeRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储关卡失败记录
	static void SaveRoleMapLoseRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储首次通关记录
	static void SaveRoleFirstPassMapRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// 存储商店购买记录
	static void SaveShopBuyRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储工会数据
	static void SaveGuildData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载工会数据
	static void LoadGuildData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储聊天记录
	static void SaveChatRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载聊天记录
	static void LoadChatRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储日志
	static void SaveLogs(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储关卡评论
	static void SaveMapComment(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载关卡评论
	static void LoadMapComment(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储物品丢失记录
	static void SaveItemLoseRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储物品操作记录
	static void SaveItemOperationRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新帐号锁
	static void UpdateAccountLock(SqlDatabase* pSqlDatabase, QueryResult * pData);	

	// 存储王者赛数据
	static void SaveKingMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载王者赛数据
	static void LoadKingMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储积分赛数据
	static void SaveScoreMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载积分赛数据
	static void LoadScoreMatchData(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// 存储组队副本数据
	static void SaveTeamDungeonRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载组队副本数据
	static void LoadTeamDungeonRankData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储在线玩家数量数据
	static void SaveOfflinePlayerNumRecord(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// 更新合服数据 
	static void UpdateServerData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储转盘数据
	static void SaveTurntableData(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// 加载转盘数据
	static void LoadTurntableData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新新手引导
	static void UpdateNewRoleGuide(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储资源战数据
	static void SaveResourceWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载资源战数据
	static void LoadResourceWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储工会战数据
	static void SaveGuildWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 加载工会战数据
	static void LoadGuildWarData(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 检索激活码数据
	static void SelectActivityCode(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 更新激活码数据
	static void UpdateActivityCode(SqlDatabase* pSqlDatabase, QueryResult * pData);
	
	// 游戏统计事件
	static void GameStatisticsEvent(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 存储GM操作记录
	static void GMOperationRecordSave(SqlDatabase* pSqlDatabase, QueryResult * pData);

	// 通用接口	
public:

	static void UpdateRetentionRate(SqlDatabase* pSqlDatabase, QueryResult * pQuery, DWORD dwCreateRoleTime, DWORD Days, const char *pszKeyName);
};