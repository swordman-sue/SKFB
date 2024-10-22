/**\file
工会管理器
\author Kevin
\date 2016/7/25
\note
*/

#pragma once
#include "DataDefine.h"
#include "ToolKit\Singleton.h"
#include "Guild.h"
#include "ToolKit\Timer.h"
class Player;

typedef std::map<DWORD, CGuild*> GUILD_MAP; // GuildId, Guild
typedef std::map<DWORD, GUILD_MAP> ALL_GUILD_MAP; // ServerId, GuildList
typedef std::map<DWORD, DWORD> GUILD_FIND_MAP;
typedef std::map<DWORD, vector<DWORD>> GUILD_APPLY_MAP;
typedef std::map<string,DWORD> GUILD_NAME_MAP;
typedef std::map<DWORD, DWORD> GUILDID_2_SERVERID_MAP;

//#define GUILD_MAX_SERIES_ID 1000000

class CGuildManager : public Singleton<CGuildManager>
{
public:
	CGuildManager();
	~CGuildManager();

	void Init();
	void Release();

	void Update();

	void OnNewDay();

	void OnNewWeek();

	// 请求加载数据
	void LoadDataRequest();

	// 加载数据
	void LoadDataFromDB(const char *pszData, int nSize);

	// 取工会数量
	int GetGuildNum();

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

public:
	// 请求工会数据
	void OnGuildData(Player *pPlayer);

	// 请求工会列表
	void OnGuildList(Player *pPlayer);

	// 请求工会成员列表
	void OnGuildMemberList(Player *pPlayer);

	// 请求留言列表
	void OnLeaveMsgList(Player *pPlayer);

	// 请求申请列表
	void OnGuildApplyList(Player *pPlayer);

	// 请求工会事件列表
	void OnGuildEventList(Player *pPlayer);

	// 创建工会
	void OnCreate(Player *pPlayer, WORD wIconId, const char *pszName);

	// 解散工会
	void OnDismiss(Player *pPlayer);

	// 申请加入 
	void OnApply(Player *pPlayer, DWORD dwGuildId);

	// 审核申请
	void OnApplyCheck(Player *pPlayer, DWORD dwTargetId, BYTE byIsAccept);

	// 取消申请
	void OnApplyCancel(Player *pPlayer, DWORD dwGuildId);

	// 踢出工会
	void OnKickOut(Player *pPlayer, DWORD dwTargetId);

	// 退出工会
	void OnLeave(Player *pPlayer);

	// 任命
	void OnAppoint(Player *pPlayer, DWORD dwTargetId, BYTE byPos);

	// 修改工会信息(修改公告,修改宣言,修改图标)
	void OnModifyGuildInfo(Player *pPlayer, BYTE byType, const char *pszText, WORD wIconId);

	// 留言
	void OnLeaveMessage(Player *pPlayer, const char *pszMessage);

	// 搜索工会
	void OnSearch(Player *pPlayer, const char *pszGuildName, DWORD dwGuildId);

	// 泉涌(建设)
	void OnNetQY(Player *pPlayer, BYTE byType);

	// 领取泉水奖励
	void OnNetQSAward(Player *pPlayer, WORD wQuanShui);

	// 技能升级
	void OnNetSkillLevelUp(Player *pPlayer, BYTE byType, WORD wSkillId, WORD wSkillLevel);

	// 请求工会技能列表
	void OnNetSkillList(Player *pPlayer);

	// 弹劾
	void OnNetImpeach(Player *pPlayer);

	// 请求工会怪物数据
	void OnNetMonsterData(Player *pPlayer);

	// 副本章节重置
	void OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord);

	// 战斗开始
	bool OnBattleStart(Player *pPlayer, BYTE byCamp);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 请求章节关卡奖励状态
	void OnNetMyMapRewardRecord(Player *pPlayer);

	// 副本
public:
	// 请求工会副本数据
	void OnNetGuildCopyData(Player *pPlayer);

	// 领取副本章节奖励
	void OnNetCopyChapterReward(Player *pPlayer, WORD wChapter);

	// 领取副本关卡奖励
	void OnNetCopyMapReward(Player *pPlayer, WORD wChapter, BYTE byCamp, BYTE byBoxIdx);

	// 请求关卡奖励记录
	void OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp);

	// 请求成员副本战绩
	void OnNetCopyBattleRecord(Player *pPlayer);

	// 购买副本挑战次数
	void OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes);

public:
	// 取工会数据
	CGuild* GetGuildById(DWORD dwGuildId);

	// 通过工会名称取工会数据
	CGuild* GetGuildByName(const char *pszGuildName);
	
	// 工会名称是否已存在
	bool IsGuildNameExist(const char *pszGuildName);

	// 通过角色ID取工会数据
	CGuild* GetGuildByRoleId(DWORD dwRoleId);

	// 取玩家所在工会的ID(0=表示木有工会)
	DWORD GetGuildIdByRoleId(DWORD dwRoleId);

	// 删除成员
	void RemoveMember(DWORD dwRoleId);

	// 删除工会
	void RemoveGuild(DWORD dwGuildId, const char *pszGuildName);

	// 添加申请数据
	void AddApplyData(DWORD dwRoleId, DWORD dwGuildId);

	// 添加查找数据
	void AddFindData(DWORD dwRoleId, DWORD dwGuildId);

	// 删除查找数据
	void RemoveFindData(DWORD dwRoleId);

	// 删除已申请的工会ID
	void RemoveApplyGuildId(DWORD dwRoleId, DWORD dwGuildId);

	// 删除所有已申请的工会信息
	void RemoveAllApplyData(DWORD dwRoleId);

	// 添加工会
	void AddGuild(DWORD dwServerId, CGuild *pGuild);

	// 取一个可用的工会ID
	DWORD GetFreeGuildId(DWORD dwServerId);

	DWORD GetSeriesId() { return m_dwSeriesId; };

	void SetLimitGoodsRefreshTime(DWORD dwRefreshTime) { m_dwLimitGoodsRefreshTime = dwRefreshTime; };
	DWORD GetLimitGoodsRefreshTime() { return m_dwLimitGoodsRefreshTime; };

	// 刷新限时商品(旧版本的刷新逻辑)
	void RefreshLimitGoodsEx();

	// 刷新限时商品 
	void RefreshLimitGoods();

	const vector<Goods>& GetLimitGoodsList() { return m_vLimitGoodsList; };

	// 合服处理
	void OnMergeServer(DWORD dwFromServerId, DWORD dwToServerId);

protected:
	// 取服务器列表
	GUILD_MAP* GetGuildList(DWORD dwServerId);

private:
	ALL_GUILD_MAP		m_AllGuildList;				// 所有工会列表 ServerId, GuildList
	GUILD_FIND_MAP		m_FindList;					// 工会查找列表 RoleId, GuildId
	GUILD_APPLY_MAP		m_ApplyList;				// 玩家申请进入工会列表 RoleId, GuildIdList
	GUILD_NAME_MAP		m_NameList;					// 工会名称列表 GuildName,GuildId
	//GUILDID_2_SERVERID_MAP m_GuildId2ServerIdList; 

	DWORD m_dwSeriesId;	// 系列ID todo::工会ID处理的逻辑有些复杂，小心使用

	// 限时商品列表
	vector<Goods> m_vLimitGoodsList;
	DWORD m_dwLimitGoodsRefreshTime;	// 限时商品刷新时间
	CDateTimers m_LimitShopRefreshTimer; // 限时商店刷新计时器
};