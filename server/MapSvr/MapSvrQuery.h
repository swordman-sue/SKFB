#pragma once

#include "QueryForUser.h"
#include "QueryPoolDefine.h"
#include "../Common/Const.h"
#include "../Common/DataDefine.h"


enum
{
	MAX_ROLE_RANK_LIST_DATA_SIZE = sizeof(RoleRankData)* MAX_RANK_MUM * MAX_SERVER_NUM + 1 + MAX_SERVER_NUM * sizeof(DWORD),	// 角色排行榜列表数据大小上限
	MAX_ARENA_LIST_DATA_SIZE = sizeof(ArenaRankData)*MAX_ARENA_COUNT + 1,	// 竞技场列表数据大小上限
	MAX_GUILD_DATA_SIZE = sizeof(GuildData)* 2 + sizeof(GuildEvent)*MAX_GUILD_EVENT_NUM + sizeof(GuildApplyData)*MAX_GUILD_APPLY_NUM +
	sizeof(GuildLeaveMessage)*MAX_GUILD_LEAVEMSG_NUM + sizeof(SkillData)*MAX_GUILD_SKILL_NUM,
	MAX_MAP_COMMENT_DATA_SIZE = sizeof(MapComment)* MAX_MAP_COMMENT_NUM + sizeof(DWORD),
	MAX_KING_MATCH_STAGE_DATA_SIZE = sizeof(KingMatchBattleTarget)*MAX_KING_MATCH_JOIN_NUM+1,
	MAX_TEAM_DUNGEON_RANK_DATA_SIZE = sizeof(TeamDungeonRank)* MAX_TEAM_DUNGEON_RANK_NUM * MAX_TEAM_DUNGEON_NUM,
	MAX_TURNTABLE_LIST_DATA_SIZE = (MAX_TURNTALE_LOTTERY_RECORD_NUM * sizeof(TurntableLotteryRecord)+sizeof(TurntableBaseData)) * MAX_SERVER_NUM,
	MAX_SCORE_MATCH_DATA_SIZE = sizeof(ScoreMatchData)*MAX_SCORE_MATCH_JOIN_NUM + 1,
	MAX_RESOURCE_WAR_DATA_SIZE = (sizeof(RoleResourceWarData)+sizeof(DWORD))*MAX_RESOURCE_WAR_JOIN_NUM + sizeof(ResourceWardBattleRecord)*MAX_RESOURCE_WAR_BATTLE_RECORD_NUM 
	+ (sizeof(ResourceWarEnemy) + sizeof(DWORD)) * MAX_RESOURCE_WAR_ENEMY_NUM +sizeof(ResourceWarResouceData)* MAX_RESOURCE_WAR_RESOURCE_NUM + 1,

	GUILD_WAR_TOWER_DATA_SIZE = sizeof(StarTowerBaseInfo)+sizeof(StarDefenderInfo)*MAX_STAR_TOWER_DEFENDER_NUM,	// 工会战塔数据大小
	GUILD_WAR_STAR_DATA_SIZE = GUILD_WAR_TOWER_DATA_SIZE * MAX_STAR_TOWER_NUM + sizeof(StarBaseInfo) + sizeof(DWORD) * MAX_GUILD_MEMBER_NUM + sizeof(DWORD)*2*50,	// 工会战星球数据大小		
	MAX_GUILD_WAR_DATA_SIZE = GUILD_WAR_STAR_DATA_SIZE * 500 + sizeof(RoleGuildWarData)* MAX_GUILD_MEMBER_NUM * 200,	// 工会战数据大小上限


	MAX_ROLE_BASE_INFO_SIZE = sizeof(RoleBaseInfo) * 2,													// 角色基本信息大小上限
	MAX_ROLE_SECOND_INFO_SIZE = sizeof(RoleSecondInfo),													// 角色二级信息大小上限
	MAX_ROLE_EQUIPMENT_INFO_SIZE = sizeof(Equipment)* MAX_EQUIPMENT_NUM,								// 角色装备信息大小上限
	MAX_ROLE_HERO_INFO_SIZE = sizeof(PlayerHeroInfo)+(2 * HERO_TRAIN_PRO_NUM)*MAX_HERO_BAG_NUM,			// 角色英雄信息大小上限
	MAX_ROLE_TREASURE_INFO_SIZE = sizeof(PlayerTreasureInfo),											// 角色宝物信息大小上限
	MAX_ROLE_ITEM_INFO_SIZE = sizeof(PlayerItemInfo),													// 角色物品信息大小上限
	MAX_ROLE_EMAIL_INFO_SIZE = sizeof(PlayerEmailInfo),													// 角色邮件信息大小上限
	MAX_ROLE_FB_INFO_SIZE = sizeof(PlayerFubenInfo)* 2,													// 角色副本信息大小上限
	MAX_ROLE_SHOP_INFO_SIZE = sizeof(PlayerShopData),													// 角色商店信息大小上限
	MAX_ROLE_SPIRIT_INFO_SIZE = sizeof(PlayerSpriteInfo),												// 角色精灵信息大小上限
	MAX_ROLE_FRIEND_INFO_SIZE = sizeof(PlayerFriendData),												// 角色好友信息大小上限
	MAX_ROLE_REBEL_INFO_SIZE = sizeof(PlayerRebelData),													// 角色叛军信息大小上限
	MAX_ROLE_ACHIEVEMENT_INFO_SIZE = sizeof(PlayerAchievementData)+2 * MAX_ACHIEVEMENT_NUM,				// 角色成就信息大小上限
	MAX_ROLE_DAILY_MISSION_INFO_SIZE = sizeof(PlayerDailyMissionData)+MAX_DAILY_MISSION_NUM,			// 角色日常任务信息大小上限
	MAX_ROLE_GUILD_SKILL_DATA_SIZE = sizeof(PlayerGuildSkillData),										// 角色工会技能数据大不上限
	MAX_ROLE_SIGNIN_DATA_SIZE = sizeof(SignInData)* 2,													// 角色签到数据大小上限
	MAX_ROLE_FAIRY_INFO_SIZE = sizeof(PlayerZhanBuInfo)* 2,												// 角色占卜数据大小上限	
	MAX_ROLE_REWARD_DATA_SIZE = sizeof(RewardData)* MAX_REWARD_CENTER_REWARD_NUM,						// 角色奖励信息大小上限
	MAX_ACTIVITY_DATA_SIZE = sizeof(PlayerActivityData)+sizeof(YCSData), //+sizeof(BYTE)*MAX_SEVEN_DAY_TARGET_NUM + 2,		// 角色活动数据大小上限
	MAX_RECHARGE_DATA_SIZE = sizeof(RoleRechargeData),													// 充值信息大小上限
	MAX_DATA_SIZE = sizeof(PlayerRewardData)+MAX_REWARD_CENTER_REWARD_NUM,								// 玩家奖励数据大小上限
	MAX_DOUBAO_DATA_SIZE = sizeof(PlayerDuobaoInfo)+sizeof(WORD),										// 夺宝数据大小上限
	MAX_ROLE_TEMP_DATA_SIZE = sizeof(RoleTempDatas),													// 临时数据大小上限

	MAX_ROLE_SECOND_INFO_SIZE_EX = MAX_ROLE_SECOND_INFO_SIZE + MAX_ROLE_EQUIPMENT_INFO_SIZE + MAX_ROLE_HERO_INFO_SIZE + MAX_ROLE_TREASURE_INFO_SIZE +
	MAX_ROLE_ITEM_INFO_SIZE + MAX_ROLE_FB_INFO_SIZE + MAX_ROLE_SHOP_INFO_SIZE + MAX_ROLE_SPIRIT_INFO_SIZE + MAX_ROLE_REBEL_INFO_SIZE + 
	MAX_ROLE_ACHIEVEMENT_INFO_SIZE + MAX_ROLE_DAILY_MISSION_INFO_SIZE + MAX_ROLE_SIGNIN_DATA_SIZE + MAX_DOUBAO_DATA_SIZE + MAX_ROLE_FAIRY_INFO_SIZE,
};		

//检测玩家帐号
class CheckAccountQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(CheckAccountQuery)

public:
	virtual inline BOOL	Init()
	{
		m_dwZoneID = 0;
		m_byLoginType = 0;
		m_bIsLock = false;
		m_byTerminalType = 0;
		memset(m_szAccID,0,sizeof(m_szAccID));
		memset(m_szPassword,0,sizeof(m_szPassword));
		memset(m_szOldPassword,0,sizeof(m_szOldPassword));
		memset(m_arLastLoginServerList, 0, sizeof(m_arLastLoginServerList));
		memset(m_szMobile, 0, sizeof(m_szMobile));

		return TRUE;
	}

	const char*		GetAccID() { return m_szAccID; }
	VOID			SetAccID(const char* pszAccID) { if (pszAccID) strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)); }

	DWORD			GetZoneID() { return m_dwZoneID; }
	VOID			SetZoneID(DWORD dwZoneID) { m_dwZoneID = dwZoneID; }

	DWORD			GetChannelUID() { return m_dwChannelUID; }
	VOID			SetChannelUID(DWORD dwChannelUID) { m_dwChannelUID = dwChannelUID; }

	BYTE			GetTerminalType() { return m_byTerminalType; };
	VOID			SetTerminalype(BYTE byTerminalType) { m_byTerminalType = byTerminalType; };

 	const char*		GetPassword() { return m_szPassword; }
 	VOID			SetPassword(const char* pszPassword) { if (pszPassword) strncpy(m_szPassword, pszPassword, sizeof(m_szPassword)); }

	BYTE			GetLoginType() { return m_byLoginType; };
	VOID			SetLoginType(BYTE byLoginType) {m_byLoginType = byLoginType;};

	char*			GetOldPassword() { return m_szOldPassword; }
	VOID			SetOldPassword(char* pszPassword) { if (pszPassword) strncpy(m_szOldPassword, pszPassword, sizeof(m_szOldPassword)); }

	const char*		GetMobile() { return m_szMobile; }
	VOID			SetMobile(char* pszMobile) { if (pszMobile) strcpy_s(m_szMobile, sizeof(m_szMobile)-1, pszMobile); }

	VOID			SetLastLoginServer(int nIndex, LastLoginServerData &sServerData)
	{
		if (nIndex >= MAX_LOGIN_SERVER_LIST)
			return;

		m_arLastLoginServerList[nIndex] = sServerData;
	}
	LastLoginServerData* GetLastLoginServer(int nIndex)
	{
		if (nIndex >= MAX_LOGIN_SERVER_LIST)
			return NULL;

		return &m_arLastLoginServerList[nIndex];
	};

	VOID			SetIsLock(bool bIsLock) { m_bIsLock = bIsLock; };
	bool			IsLock() { return m_bIsLock; };
	VOID			SetUnlockTime(DWORD dwUnlockTime) { m_dwUnlockTime = dwUnlockTime; };
	DWORD			GetUnlockTime() { return m_dwUnlockTime; };


private:
	//查询条件
	DWORD			m_dwZoneID;									//玩家分区ID
	char			m_szAccID[MAX_ACCOUNT_LEN];					//账号ID
	char			m_szPassword[MAX_ACCOUNT_PASSWORD_LEN];		//帐号密码
	BYTE			m_byLoginType;								//登录类型
	DWORD			m_dwChannelUID;								//渠道UID
	BYTE			m_byTerminalType;							//终端类型

	// 查询结果
	bool			m_bIsLock;									// 是否已被锁
	DWORD			m_dwUnlockTime;								// 解锁时间
 	//int				m_nCheckResult;								//检测结果
	char			m_szOldPassword[MAX_ACCOUNT_PASSWORD_LEN];		//旧的帐号密码
	char			m_szMobile[MAX_MOBILE_LEN];	// 手机号		
	LastLoginServerData	m_arLastLoginServerList[MAX_LOGIN_SERVER_LIST];	//最近登录的服务器列表
};


// 创建帐号
class CreateAccountQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(CreateAccountQuery)

public:
	virtual inline BOOL	Init()
	{
		m_dwZoneID = 0;
		m_byLoginType = 0;
		m_dwChannelUID = 0;
		memset(m_szAccID, 0, sizeof(m_szAccID));
		memset(m_szPassword, 0, sizeof(m_szPassword));
		memset(m_szMobile, 0, sizeof(m_szMobile));
		return TRUE;
	}

	const char*		GetAccID() { return m_szAccID; }
	VOID			SetAccID(const char* pszAccID) { if (pszAccID) strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)); }

	DWORD			GetZoneID() { return m_dwZoneID; }
	VOID			SetZoneID(DWORD dwZoneID) { m_dwZoneID = dwZoneID; }

	DWORD			GetChannelUID() { return m_dwChannelUID; }
	VOID			SetChannelUID(DWORD dwChannelUID) { m_dwChannelUID = dwChannelUID; }

	BYTE			GetTerminalType() { return m_byTerminalType; };
	VOID			SetTerminalType(BYTE byTerminal) { m_byTerminalType = byTerminal; };

	const char*		GetPassword() { return m_szPassword; }
	VOID			SetPassword(const char* pszPassword) { if (pszPassword) strncpy(m_szPassword, pszPassword, sizeof(m_szPassword)); }

	BYTE			GetLoginType() { return m_byLoginType; };
	VOID			SetLoginType(BYTE byLoginType) { m_byLoginType = byLoginType; };

	const char*		GetMobile() { return m_szMobile; }
	VOID			SetMobile(const char* pszMobile) { if (pszMobile) strncpy(m_szMobile, pszMobile, sizeof(m_szMobile)); }
private:
	//查询条件
	char			m_szAccID[MAX_ACCOUNT_LEN];					// 账号ID
	char			m_szPassword[MAX_ACCOUNT_PASSWORD_LEN];		// 帐号密码
	BYTE			m_byLoginType;								// 登录类型
	BYTE			m_byTerminalType;							// 终端类型
	DWORD			m_dwZoneID;									// 玩家分区ID
	DWORD			m_dwChannelUID;								// 渠道UID
	char			m_szMobile[MAX_MOBILE_LEN];					// 手机号
};

//查询玩家角色列表
class SelectCharListQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(SelectCharListQuery)

public:
	virtual inline BOOL	Init()
	{
		m_dwZoneID = 0;
		m_dwPlatform = 0;
		m_dwRoleId = 0;
		memset(m_szAccID,0,sizeof(m_szAccID));
		memset(m_szRoleName, 0, sizeof(m_szRoleName));

		return TRUE;
	}

	char*			GetAccID() { return m_szAccID; }
	VOID			SetAccID(const char* pszAccID) { if (pszAccID) strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)-1); }

	DWORD			GetZoneID() { return m_dwZoneID; }
	VOID			SetZoneID(DWORD dwZoneID) { m_dwZoneID = dwZoneID; }

	DWORD			GetPlatform() { return m_dwPlatform; }
	VOID			SetPlatform(DWORD dwPlatform) { m_dwPlatform = dwPlatform; }

	DWORD			GetRoleId() { return m_dwRoleId; }
	VOID			SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; }

	char*			GetRoleName() { return m_szRoleName; }
	VOID			SetRoleName(const char* pszRoleName) { if (pszRoleName) strncpy(m_szRoleName, pszRoleName, sizeof(m_szRoleName)-1); }
private:
	//查询条件
	DWORD			m_dwZoneID;							//玩家分区ID
	char			m_szAccID[MAX_ACCOUNT_LEN];			//玩家账号ID
	DWORD			m_dwPlatform;						//渠道平台

	//查询结果
	DWORD			m_dwRoleId;							//角色ID
	char			m_szRoleName[MAX_ROLE_LEN];			//角色名
};

//创建角色的游戏信息
class CreateCharInfoQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(CreateCharInfoQuery)
public:

	virtual inline BOOL				Init()
	{
		memset(m_szAccID,0,sizeof(m_szAccID));
		m_CharID = 0;
		memset(m_szCharName,0,sizeof(m_szCharName));
		m_byGender = 0;
		m_byIsNameRepeat = 0;
		m_dwHeadIconId = 0;
		m_byIsRoleExist = 0;

		return TRUE;
	}

	DWORD			GetZoneID() { return m_dwZoneID; }
	VOID			SetZoneID(DWORD dwZoneID) { m_dwZoneID = dwZoneID; }

	char*			GetAccID() { return m_szAccID; }
	VOID			SetAccID(char* pszAccID) { if (pszAccID) strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)); }

	DWORD			GetCharID() { return m_CharID; }
	VOID			SetCharID(DWORD dwCharID) { m_CharID = dwCharID; }

	CHAR*			GetRoleName() { return m_szCharName; }
	VOID			SetRoleName(CHAR* szCharName) { if (m_szCharName) strncpy(m_szCharName, szCharName, sizeof(m_szCharName)); }

	BYTE			GetGender() { return m_byGender; }
	VOID			SetGender(BYTE byGender) { m_byGender = byGender; }

	bool			IsNameRepeat() { return m_byIsNameRepeat ? true : false; };
	VOID			SetNameRepeat(BYTE byIsRepeat) { m_byIsNameRepeat = byIsRepeat; };

	BYTE			IsRoleExist() { return m_byIsRoleExist; };
	VOID			SetIsRoleExist(BYTE byIsRoleExist) { m_byIsRoleExist = byIsRoleExist; };

	VOID			SetHeadIconId(DWORD	dwHeadIconId) { m_dwHeadIconId = dwHeadIconId; };
	DWORD			GetHeadIconId() { return m_dwHeadIconId; };

	DWORD			GetChannelUID() { return m_dwChannelUID; }
	VOID			SetChannelUID(DWORD dwChannelUID) { m_dwChannelUID = dwChannelUID; }

	BYTE			GetTerminalType() { return m_byTerminalType; }
	VOID			SetTerminalType(BYTE byTerminalType) { m_byTerminalType = byTerminalType; }


	BYTE			GetCreateType() { return m_byCreateType; }
	VOID			SetCreateType(BYTE byType) { m_byCreateType = byType; }

private:
	DWORD			m_dwZoneID;								//分区ID
	char			m_szAccID[MAX_ACCOUNT_LEN];				//账号ID
	DWORD			m_CharID;								//角色ID
	CHAR			m_szCharName[MAX_ROLE_LEN];			//角色名
	//BYTE			m_byVocation;							//职业
	BYTE			m_byGender;								//性别
	DWORD			m_dwHeadIconId;							//头像图标ID
	DWORD			m_dwChannelUID;							//渠道UID
	BYTE			m_byCreateType;							//创建类型
	BYTE			m_byTerminalType;						//渠道类型

	// 创建结果
	BYTE			m_byIsNameRepeat;						//是否重名
	BYTE			m_byIsRoleExist;						//是否角色已存在
};

//查询角色的游戏信息
class SelectCharInfoQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(SelectCharInfoQuery)
public:
	virtual inline BOOL	Init()
	{
		m_bySelectType = 0;
		m_dwCharID = 0;
		m_dwChannelType = 0;
		memset(&m_tRoleFullInfo, 0, sizeof(m_tRoleFullInfo));
		return TRUE;
	}

	DWORD			GetCharID() { return m_dwCharID; }
	VOID			SetCharID(DWORD dwCharID) { m_dwCharID = dwCharID; }

	BYTE			GetSelectType() { return m_bySelectType; };
	void			SetSelectType(BYTE bySelectType) { m_bySelectType = bySelectType; };

	BYTE			GetRequestType() { return m_byRequestType; };
	void			SetRequestType(BYTE byRequestType) { m_byRequestType = byRequestType; };

	DWORD			GetChannelType() { return m_dwChannelType; }
	VOID			SetChannelType(DWORD dwChannelType) { m_dwChannelType = dwChannelType; }

	RoleFullInfo&	GetRoleFullInfo() { return m_tRoleFullInfo; }

private:
	//查询条件
	DWORD			m_dwCharID;								//角色ID
	BYTE			m_bySelectType;							//查询类型
	BYTE			m_byRequestType;						//客户端请求类型
	DWORD			m_dwChannelType;						//渠道类型

	//查询结果
	RoleFullInfo	m_tRoleFullInfo;						//玩家的完整游戏信息
};

// 更新角色游戏信息
class UpdateRoleInfoQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(UpdateRoleInfoQuery)
public:
	virtual inline BOOL	Init()
	{
		m_dwCharID = 0;
		m_bySaveType = 0;
		m_dwServerId = 0;
		memset(&m_tRoleFullInfo, 0, sizeof(m_tRoleFullInfo));

		return TRUE;
	}

	DWORD GetCharID() { return m_dwCharID; }
	VOID SetCharID(DWORD dwCharID) { m_dwCharID = dwCharID; }

	DWORD GetServerD() { return m_dwServerId; }
	VOID SetServerID(DWORD dwServerId) { m_dwServerId = dwServerId; }

	UCHAR GetSaveType() { return m_bySaveType; }
	VOID SetSaveType(BYTE bySaveType) { m_bySaveType = bySaveType; }

	RoleFullInfo& GetRoleFullInfo() { return m_tRoleFullInfo; }
	VOID SetRoleFullData(RoleFullInfo& tRoleFullInfo)
	{ 
		//m_tRoleFullInfo = tCharFullInfo;
		memcpy(&m_tRoleFullInfo, &tRoleFullInfo, sizeof(RoleFullInfo));
	}

private:
	DWORD			m_dwCharID;								//角色ID			
	BYTE			m_bySaveType;							//存储类型
	DWORD			m_dwServerId;							//服务器ID
	RoleFullInfo	m_tRoleFullInfo;						//角色游戏信息

};

// 更新角色属性
class UpdateRolePropertyQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(UpdateRolePropertyQuery)
public:
	virtual inline BOOL	Init()
	{
		m_dwRoleId = 0;
		m_byPropertyType = 0;
		m_dwValue = 0;
		memset(m_szValue, 0, sizeof(m_szValue));

		return TRUE;
	}

	DWORD			GetRoleId() { return m_dwRoleId; }
	VOID			SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; }

	DWORD			GetValue() { return m_dwValue; }
	VOID			SetValue(DWORD dwValue) { m_dwValue = dwValue; }

	BYTE			GetType() { return m_byPropertyType; }
	VOID			SetType(BYTE byValue) { m_byPropertyType = byValue; }

	const char*		GetStringValue() { return m_szValue; }
	VOID			SetStringValue(const char *pszValue) 
	{ 
		strncpy_s(m_szValue, sizeof(m_szValue)-1, pszValue, sizeof(m_szValue)-1); 
	}

private:
	DWORD			m_dwRoleId;			// 角色ID			
	BYTE			m_byPropertyType;	// 属性类型
	DWORD			m_dwValue;			// 数字值
	char			m_szValue[128];		// 字符值
};


// 更新最近登录的服务器
class UpdateLastLoginServer : public QueryForUser
{
	_DECLARE_QUERY_POOL(UpdateLastLoginServer)
public:
	virtual inline BOOL	Init()
	{
		m_dwServerId = 0;
		m_dwChannelType = 0;
		memset(m_szAccID, 0, sizeof(m_szAccID));


		return TRUE;
	}

	char*			GetAccID() { return m_szAccID; }
	VOID			SetAccID(const char* pszAccID) 
	{ 
		if (pszAccID) 
			strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)-1); 
	}

	DWORD			GetServerID() { return m_dwServerId; }
	VOID			SetServerID(DWORD dwServerId) { m_dwServerId = dwServerId; }

	DWORD			GetChannelType() { return m_dwChannelType; }
	VOID			SetChannelType(DWORD dwChannelType) { m_dwChannelType = dwChannelType; }

private:
	//查询条件
	DWORD			m_dwServerId;						//服务器ID
	DWORD			m_dwChannelType;						//渠道类型
	char			m_szAccID[MAX_ACCOUNT_LEN];			//玩家账号ID
};

class InsertEmailQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(InsertEmailQuery)
public:
	virtual inline BOOL	Init()
	{
		memset(&m_EmailInfo, 0, sizeof(m_EmailInfo));
		return TRUE;
	}
	EmailInfo&	GetEmailInfo() { return m_EmailInfo; }
	void        SetEmailInfo(EmailInfo& Info){ m_EmailInfo = Info;}

private:
	//查询结果
	EmailInfo	m_EmailInfo;						//邮件信息
};
class DeleteEmailQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(DeleteEmailQuery)
public:
	virtual inline BOOL	Init()
	{
		m_dwEmailID = 0;
		m_dwCharID = 0;
		return TRUE;
	}
	DWORD	    GetEmailID() { return m_dwEmailID; }
	void        SetEmailID(DWORD dwID){ m_dwEmailID = dwID;}
	void        SetCharID(DWORD dwID)
	{
		m_dwCharID = dwID;
		return;
	}
	DWORD        GetCharID(){return m_dwCharID;}

private:
	DWORD	    m_dwEmailID;						
	DWORD       m_dwCharID;

};


class DeleteAllEmailQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(DeleteAllEmailQuery)
public:
	virtual inline BOOL	Init()
	{
		
		m_dwCharID = 0;
		return TRUE;
	}
	void        SetCharID(DWORD dwID)
	{
		m_dwCharID = dwID;
		return;
	}
	DWORD        GetCharID(){return m_dwCharID;}

private:					
	DWORD       m_dwCharID;

};
class UpdateEmailQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(UpdateEmailQuery)
public:
	virtual inline BOOL	Init()
	{
		memset(&m_EmailInfo, 0, sizeof(m_EmailInfo));
		m_dwCharID = 0;
		return TRUE;
	}
	EmailInfo&	GetEmailInfo() { return m_EmailInfo; }
	void        SetEmailInfo(EmailInfo& Info){ m_EmailInfo = Info;}
	void        SetCharID(DWORD dwID)
	{
		m_dwCharID = dwID;
		return;
	}
	DWORD        GetCharID(){return m_dwCharID;}

private:
	//查询结果
	EmailInfo	m_EmailInfo;						//社会关系信息
	DWORD       m_dwCharID;
};


class DeleteEmailByConditionQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(DeleteEmailByConditionQuery)
public:
	virtual inline BOOL	Init()
	{
		m_CurrTime = 0;
		return TRUE;
	}
	DWORD	GetCurrTime() { return m_CurrTime; }
	void	SetCurrTime( DWORD dwCurrTime){ m_CurrTime = dwCurrTime;}

private:
	//查询结果
	DWORD	m_CurrTime;						
};


// zhangy:保存所有邮件到数据库
class SaveAllEmailQuery : public QueryForUser
{
	_DECLARE_QUERY_POOL(SaveAllEmailQuery)
public:
	virtual inline BOOL	Init()
	{
		memset(&m_tCharEmailInfo, 0, sizeof(m_tCharEmailInfo));
		m_dwCharID = 0;
		m_dwCharEmailInfoNum = 0;
		return TRUE;
	}
	PlayerEmailInfo&	GetCharEmailInfo() { return m_tCharEmailInfo; };
	DWORD			GetCharId(){ return m_dwCharID;};
	DWORD			GetCharEmailInfoNum(){return m_dwCharEmailInfoNum;};

	VOID			SetCharEmailInfo(PlayerEmailInfo& tCharEmailInfo) { m_tCharEmailInfo = tCharEmailInfo; };
	VOID			SetCharId(DWORD dwCharID){m_dwCharID = dwCharID;};
	VOID			SetCharEmailInfoNum(DWORD dwCharEmailInfoNum){m_dwCharEmailInfoNum = dwCharEmailInfoNum;};
private:
	PlayerEmailInfo		m_tCharEmailInfo;
	DWORD				m_dwCharID;
	DWORD				m_dwCharEmailInfoNum;
};

// 角色同名检测
class SameRoleNameCheck : public QueryForUser
{
	_DECLARE_QUERY_POOL(SameRoleNameCheck)

public:
	virtual inline BOOL	Init()
	{
		memset(m_szRoleName,0,sizeof(m_szRoleName));
		return TRUE;
	}

	void SetSameName(bool bSame){ m_bSameName = bSame; };
	bool IsSameName() { return m_bSameName; };

	char* GetRoleName() { return m_szRoleName; }
	VOID SetRoleName(char* pszRoleName) 
	{ 
		if (pszRoleName)  
			strcpy_s(m_szRoleName, MAX_GUILD_NAME_LEN+1, pszRoleName); 
	}
private:
	// 查询条件
	char m_szRoleName[MAX_GUILD_NAME_LEN+1];

	// 结果集
	bool m_bSameName;	// 是否同名
};

// 帐号密码修改
class AccountPasswordModify : public QueryForUser
{
	_DECLARE_QUERY_POOL(AccountPasswordModify)

public:
	virtual inline BOOL	Init()
	{
		m_dwZoneID = 0;
		memset(m_szAccID,0,sizeof(m_szAccID));
		memset(m_szPassword,0,sizeof(m_szPassword));
		return TRUE;
	}

	const char*		GetAccID() { return m_szAccID; }
	VOID			SetAccID(const char* pszAccID) 
	{ if (pszAccID) strncpy(m_szAccID, pszAccID, sizeof(m_szAccID)); }

	DWORD			GetZoneID() { return m_dwZoneID; }
	VOID			SetZoneID(DWORD dwZoneID) { m_dwZoneID = dwZoneID; }

	const char*		GetPassword() { return m_szPassword; }
	VOID			SetPassword(const char* pszPassword) 
	{ if (pszPassword) strncpy(m_szPassword, pszPassword, sizeof(m_szPassword)); }

private:
	//查询条件
	DWORD			m_dwZoneID;								//玩家分区ID
	char			m_szAccID[MAX_ACCOUNT_LEN];				//账号ID
	char			m_szPassword[MAX_ACCOUNT_PASSWORD_LEN];			//帐号密码
};

// 帐号手机号修改
class AccountMobileModify : public QueryForUser
{
	_DECLARE_QUERY_POOL(AccountMobileModify)

public:
	virtual inline BOOL	Init()
	{
		m_dwZoneID = 0;
		memset(m_szAccID, 0, sizeof(m_szAccID));
		memset(m_szMobile, 0, sizeof(m_szMobile));
		return TRUE;
	}

	const char*		GetAccID() { return m_szAccID; }
	VOID			SetAccID(const char* pszAccID)
	{
		if (pszAccID) strncpy(m_szAccID, pszAccID, sizeof(m_szAccID));
	}

	DWORD			GetZoneID() { return m_dwZoneID; }
	VOID			SetZoneID(DWORD dwZoneID) { m_dwZoneID = dwZoneID; }

	const char*		GetMobile() { return m_szMobile; }
	VOID			SetMobile(const char* pszMobile)
	{
		if (pszMobile) strncpy(m_szMobile, pszMobile, sizeof(m_szMobile));
	}

private:
	//查询条件
	DWORD			m_dwZoneID;								//玩家分区ID
	char			m_szAccID[MAX_ACCOUNT_LEN];				//账号ID
	char			m_szMobile[MAX_MOBILE_LEN];			//手机号
};


 // 离线消息保存
 class OfflineMsg_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(OfflineMsg_Save)

 public:
	 enum
	 {
		Operation_Result_Success = 0,		// 操作成功
		Operation_Result_Target_Invalid,	// 目标不存在
		Operation_Result_Num_Limit,			// 数量限制
	 };

 public:
	 virtual inline BOOL Init()
	 {
		// memset(&m_tOfflineMsg, 0, sizeof(m_tOfflineMsg));
		 m_byOperationResult = 0;
		 return TRUE;
	 }
	
	 //void SetOfflineMsg(const OfflineMsg &tMsg) { memcpy_s(&m_tOfflineMsg, sizeof(OfflineMsg),  &tMsg, sizeof(OfflineMsg));};
	// const OfflineMsg& GetOfflineMsg() { return m_tOfflineMsg; };

	 void SetOperationResult(BYTE byResult) { m_byOperationResult = byResult; };
	 BYTE GetOperationResult() { return m_byOperationResult; };

	 void SetTargetName(const char *pszTargetName) { strcpy_s(m_szTargetName, MAX_ROLE_LEN+1, pszTargetName); };
	 const char* GetTargetName() { return m_szTargetName; };

	 DWORD GetTargetId() { return m_dwTargetId; };
	 void SetTargetId(DWORD dwTargetId) { m_dwTargetId = dwTargetId; };

 private:
	 // 查询条件
	//OfflineMsg m_tOfflineMsg;
	char m_szTargetName[MAX_ROLE_LEN+1]; // 目标名称
	DWORD m_dwTargetId;
	
	 // 操作结果
	BYTE m_byOperationResult;
 };


 // 聊天记录存储
 class ChatRecordList_Save: public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ChatRecordList_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_arRecordList, 0, sizeof(m_arRecordList));
		 m_byCount = 0;
		 return TRUE;
	 }

	 const ChatRecord* GetList() { return m_arRecordList; };
	 BYTE GetListCount() { return m_byCount; };
	 void AddData(const ChatRecord &rData)
	 {
		 if (m_byCount >= MAX_CHAT_RECORD_COUNT)
			 return;

		 memcpy_s(&m_arRecordList[m_byCount++], sizeof(ChatRecord), &rData, sizeof(ChatRecord));
	 }

 private:
	 // 查询条件
	 ChatRecord m_arRecordList[MAX_CHAT_RECORD_COUNT];
	 BYTE m_byCount;														
	 // 结果集
 };


 // 加载聊天记录
 class ChatRecordList_Load: public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ChatRecordList_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_arRecordList, 0, sizeof(m_arRecordList));
		 m_byCount = 0;
		 return TRUE;
	 }

	 const ChatRecord* GetList() { return m_arRecordList; };
	 BYTE GetListCount() { return m_byCount; };
	 void AddData(const ChatRecord &rData)
	 {
		 if (m_byCount >= MAX_CHAT_RECORD_COUNT)
			 return;

		 memcpy_s(&m_arRecordList[m_byCount++], sizeof(ChatRecord), &rData, sizeof(ChatRecord));
	 }

 private:
	 // 查询条件
	 ChatRecord m_arRecordList[MAX_CHAT_RECORD_COUNT];
	 BYTE m_byCount;														
	 // 结果集
 };


 // 服务器全局数据存储
 class ServerGolbalData_Save: public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ServerGolbalData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_byGolbalDataNum = 0;
		 return TRUE;
	 }

	 void AddGolbalData(WORD wServerId, int nKey, DWORD dwValue)
	 {
		 if (m_byGolbalDataNum >= MAX_GOLBAL_DATA_NUM)
			 return;

		 ServerGolbalData &tData = m_arGolbalDataList[m_byGolbalDataNum++];
		 tData.wServerId = wServerId;
		 tData.nKey = nKey;
		 tData.dwValue = dwValue;
	 }

	 const ServerGolbalData* GetGolbalDataList() { return m_arGolbalDataList; };
	 BYTE GetGolbalDataNum() { return m_byGolbalDataNum; };

 private:
	 ServerGolbalData m_arGolbalDataList[MAX_GOLBAL_DATA_NUM];
	 BYTE m_byGolbalDataNum;
 };

 // 服务器全局数据加载
 class ServerGolbalData_Load: public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ServerGolbalData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 m_wGolbalDataNum = 0;
		 return TRUE;
	 }

	 void AddGolbalData(WORD wServerId, int nKey, DWORD dwValue)
	 {
		 if (m_wGolbalDataNum >= MAX_GOLBAL_DATA_NUM)
			 return;
		
		 ServerGolbalData &tData = m_arGolbalDataList[m_wGolbalDataNum++];
		 tData.wServerId = wServerId;
		 tData.nKey = nKey;
		 tData.dwValue = dwValue;
	 }

	 const ServerGolbalData* GetGolbalDataList() { return m_arGolbalDataList; };
	 WORD GetGolbalDataNum() { return m_wGolbalDataNum; };

 private:
	 ServerGolbalData m_arGolbalDataList[MAX_GOLBAL_DATA_NUM];
	 WORD m_wGolbalDataNum;
 };


 // 角色基本数据列表加载
 class RoleBaseDataList_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleBaseDataList_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_BaseDataList, 0, sizeof(m_BaseDataList));
		 m_wBaseDataListNum = 0;
		 m_dwMinRoleId = 0;
		 m_dwTotalDataNum = 0;
		 return TRUE;
	 }

	 void AddData(const DBPlayerBaseData &sBaseData)
	 {
		 if (m_wBaseDataListNum >= ROLE_BASE_DATA_LOAD_NUM)
			 return;

		 m_BaseDataList[m_wBaseDataListNum++] = sBaseData;
	 }

	 DBPlayerBaseData *GetList() { return m_BaseDataList; };
	 WORD GetListNum() { return m_wBaseDataListNum; };

	 DWORD GetMinRoleId() { return m_dwMinRoleId; };
	 void SetMinRoleId(DWORD dwMinRoleId) { m_dwMinRoleId = dwMinRoleId; };

	 DWORD GetTotalDataNum() { return m_dwTotalDataNum; };
	 void SetTotalDataNum(DWORD dwTotalDataNum) { m_dwTotalDataNum = dwTotalDataNum; };

 private:
	 DBPlayerBaseData m_BaseDataList[ROLE_BASE_DATA_LOAD_NUM];
	 WORD m_wBaseDataListNum;
	 DWORD m_dwTotalDataNum;
	 DWORD m_dwMinRoleId;
 };

 // 好友数据更新
 class FriendDataUpdate_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(FriendDataUpdate_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tFriendData, 0, sizeof(m_tFriendData));
		 return TRUE;
	 }

	 void SetOperateType(BYTE byType) { m_byOperateType = byType; };
	 BYTE GetOperateType() { return m_byOperateType; };

	 void SetUpdateRoleId(DWORD dwTargetId) { m_dwUpateRoleId = dwTargetId; };
	 DWORD GetUpdateRoleId() { return m_dwUpateRoleId; };

	 FriendData&  GetFriendData() { return m_tFriendData; };
	 void SetFriendData(const FriendData &sData) { m_tFriendData = sData; };

 private:
	 BYTE m_byOperateType;	// 操作类型(1=删除好友，2=申请好友回复,3=添加黑名单) 原因
	 DWORD m_dwUpateRoleId;	// 要更新的角色	
	 FriendData m_tFriendData;
 };

 // 竞技场数据加载
 class ArenaDataList_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ArenaDataList_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 m_dwServerId = 0;
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_ARENA_LIST_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

 private:
	 DWORD m_dwServerId;
	 char m_szDataBuff[MAX_ARENA_LIST_DATA_SIZE];
	 int m_nDataBuffSize;
 };

 // 竞技场数据存储
 class ArenaDataList_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ArenaDataList_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 m_dwServerId = 0;
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_ARENA_LIST_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

 private:
	 DWORD m_dwServerId;
	 char m_szDataBuff[MAX_ARENA_LIST_DATA_SIZE];
	 int m_nDataBuffSize;
 };

 // 竞技场战报存储
 class ArenaBattleRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ArenaBattleRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tPlayerBattleRecord, 0, sizeof(m_tPlayerBattleRecord));
		 return TRUE;
	 }

	 void SetRoleId(DWORD dwRoleId) { m_tPlayerBattleRecord.dwRoleId = dwRoleId; };
	 DWORD GetRoleId() { return m_tPlayerBattleRecord.dwRoleId; };

	 // 取战斗记录
	 const ArenaBattleRecord* GetBattleRecordList() { return m_tPlayerBattleRecord.arBattleRecordList; };
	 int GetBattleRecordNum() { return m_tPlayerBattleRecord.wBattleRecordNum; };

	 // 添加战斗记录
	 void AddBattleRecord(const ArenaBattleRecord *pBattleRecord)
	 {
		 if (m_tPlayerBattleRecord.wBattleRecordNum >= MAX_ARENA_LIST_DATA_SIZE)
			 return;
		 memcpy_s(&m_tPlayerBattleRecord.arBattleRecordList[m_tPlayerBattleRecord.wBattleRecordNum++], 
			 sizeof(ArenaBattleRecord), pBattleRecord, sizeof(ArenaBattleRecord));
	 }

	 const PlayerArenaBattleRecord* GetPlayerBattleRecord() { return &m_tPlayerBattleRecord; };

 private:
	 PlayerArenaBattleRecord m_tPlayerBattleRecord;
 };

 // 竞技场战报加载
 class ArenaBattleRecord_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ArenaBattleRecord_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_PlayerBattleRecrodList, 0, sizeof(m_PlayerBattleRecrodList));
		 m_nPlayerBattleRecordNum = 0;
		 m_nTotalPlayerBattleRecrodNum = 0;
		 m_dwMinRoleId = 0;
		 return TRUE;
	 }

	 void SetMinRoleId(DWORD dwMinRoleId) { m_dwMinRoleId = dwMinRoleId; }
	 DWORD GetMinRoleId() { return m_dwMinRoleId; };

	 void SetTotalBattleRecordNum(int nRecordNum) { m_nTotalPlayerBattleRecrodNum = nRecordNum; };
	 int GetTotalBattleRecordNum() { return m_nTotalPlayerBattleRecrodNum; };

	 // 取玩家战斗记录
	 const PlayerArenaBattleRecord* GetPlayerBattleRecordList() { return m_PlayerBattleRecrodList; };
	 int GetPlayerBattleRecordNum() { return m_nPlayerBattleRecordNum; };

	 // 添加玩家战斗记录
	 void AddPlayerBattleRecord(const PlayerArenaBattleRecord *pBattleRecord)
	 {
		 if (m_nPlayerBattleRecordNum >= ARENA_BATTLE_RECORD_LOAD_NUM)
			 return;
		 int nDataSize = sizeof(PlayerArenaBattleRecord);
		 memcpy_s(&m_PlayerBattleRecrodList[m_nPlayerBattleRecordNum++], nDataSize,pBattleRecord, nDataSize);
	 }

 private:
	 PlayerArenaBattleRecord m_PlayerBattleRecrodList[ARENA_BATTLE_RECORD_LOAD_NUM];
	 int m_nPlayerBattleRecordNum;
	 int m_nTotalPlayerBattleRecrodNum;
	 DWORD m_dwMinRoleId;
 };

 // 工会数据存储
 class GuildData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(GuildData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szGuildName, 0, sizeof(m_szGuildName));
		 m_dwServerId = 0;
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };
	 const char* GetGuildName() { return m_szGuildName; };
	 void SetGuildName(const char *pszGuildName)
	 {
		 strcpy_s(m_szGuildName, sizeof(m_szGuildName)-1, pszGuildName);
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_GUILD_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 void SetIsDelete(bool isDelete) { m_bIsDelete = isDelete; };
	 bool GetIsDelete() { return m_bIsDelete; };
 private:
	 DWORD m_dwServerId;
	 bool m_bIsDelete;
	 char m_szDataBuff[MAX_GUILD_DATA_SIZE];
	 int m_nDataBuffSize;
	 char m_szGuildName[MAX_GUILD_NAME_LEN+1];
 };

 // 工会数据缓存
 struct GuildDataBuff
 {
	 char szBuff[MAX_GUILD_DATA_SIZE + 1];
	 int nBuffSize;
	 DWORD dwGuildId;
 };

 // 工会数据加载
 class GuildData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(GuildData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_GuildDataList, 0, sizeof(m_GuildDataList));
		 m_nGuildNum = 0;
		 m_nTotalGuildNum = 0;
		 m_dwMinGuildId = 0;
		 return TRUE;
	 }

	 void SetMinGuildId(DWORD dwMinGuildId) { m_dwMinGuildId = dwMinGuildId; }
	 DWORD GetMinGuildId() { return m_dwMinGuildId; };

	 void SetTotalGuildNum(int nRecordNum) { m_nTotalGuildNum = nRecordNum; };
	 int GetTotalGuildNum() { return m_nTotalGuildNum; };

	 // 取工会数据列表
	 const GuildDataBuff* GetGuildList(){ return m_GuildDataList; };
	 int GetGuildNum() { return m_nGuildNum; };

	 // 添加工会数据
	 void AddGuildData(const GuildDataBuff *pDataBuff)
	 {
		 if (m_nGuildNum >= ARENA_BATTLE_RECORD_LOAD_NUM)
			 return;

		 memcpy_s(&m_GuildDataList[m_nGuildNum++], sizeof(GuildDataBuff), pDataBuff, sizeof(GuildDataBuff));
		 //GuildDataBuff &tBuff = m_GuildDataList[m_nGuildNum++];
		// memcpy_s(tBuff.szBuff, MAX_GUILD_DATA_SIZE - 1, pGuildData, nDataSize);
		// tBuff.nBuffSize = nDataSize;
	 }

 private:
	 GuildDataBuff m_GuildDataList[GUILD_LIST_LOAD_NUM];
	 int m_nGuildNum;
	 int m_nTotalGuildNum;
	 DWORD m_dwMinGuildId;
 };


 // 角色排行榜数据存储
 class RoleRankList_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleRankList_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 m_byRankType = 0;
		 m_dwRankNum = 0;
		 return TRUE;
	 }

	 void SetRankType(BYTE byRankType) { m_byRankType = byRankType; };
	 BYTE GetRankType() { return m_byRankType; };

	 void SetRankNum(DWORD dwRankNum) { m_dwRankNum = dwRankNum; };
	 DWORD GetRankNum() { return m_dwRankNum; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_ROLE_RANK_LIST_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };

 private:
	 //DWORD m_dwServerId;
	 BYTE m_byRankType;	// 排行类型
	 DWORD m_dwRankNum;	// 排行数量
	 char m_szDataBuff[MAX_ROLE_RANK_LIST_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_ROLE_RANK_LIST_DATA_SIZE * 2 + 1];
 };


 // 角色排行榜数据加载
 class RoleRankList_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleRankList_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 m_nDataBuffSize = 0;
		 m_byRankType = 0;
		 return TRUE;
	 }

	 void SetRankType(BYTE byRankType) { m_byRankType = byRankType; };
	 BYTE GetRankType() { return m_byRankType; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_ROLE_RANK_LIST_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

 private:
	 BYTE m_byRankType;	// 排行类型
	 char m_szDataBuff[MAX_ROLE_RANK_LIST_DATA_SIZE];
	 int m_nDataBuffSize;
 };

 // 加载GM事件列表
 class GMEventList_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(GMEventList_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_EventList, 0, sizeof(m_EventList));
		 m_nEventNum = 0;
		 return TRUE;
	 }

	 int GetEventNum() { return m_nEventNum; };

	 // 添加事件数据
	 void AddEventData(const GMEventData &tEventData)
	 {
		 if (m_nEventNum >= MAX_GM_EVENT_NUM)
			 return;

		 m_EventList[m_nEventNum++] = tEventData;
	 }

	 // 取事件数据
	 const GMEventData* GetEventData(int nIndex)
	 {
		 if (m_nEventNum >= MAX_GM_EVENT_NUM)
			 return NULL;

		 return &m_EventList[nIndex];
	 }

 private:
	 GMEventData m_EventList[MAX_GM_EVENT_NUM];
	 int m_nEventNum;
 };


 // 服务器列表加载
 class ServerList_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ServerList_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_ServerList, 0, sizeof(m_ServerList));
		 m_nServerNum = 0;
		 return TRUE;
	 }

	 const ServerInfo* GetServerList() { return m_ServerList; };
	 void AddServerInfo(const ServerInfo *pServer)
	 {
		 if (m_nServerNum >= MAX_SERVER_NUM)
			 return;

		 m_ServerList[m_nServerNum++] = *pServer;
	 }
	 int GetServerNun() { return m_nServerNum; };

 private:
	 ServerInfo m_ServerList[MAX_SERVER_NUM];
	 int m_nServerNum;
 };


 // 角色货币日志存储
 class RoleCurrencyLogs_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleCurrencyLogs_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_dwRoleId = 0;
		 m_wRoleLevel = 0;
		 m_wFromId = 0;
		 m_nCurrencyType = 0;
		 m_nCurrencyValue = 0;
		 m_nCurCurrencyValue = 0;
		 return TRUE;
	 }

	 void SetRoleId(DWORD dwRoleId){ m_dwRoleId = dwRoleId; };
	 void SetRoleLevel(WORD wRoleLevel){ m_wRoleLevel = wRoleLevel; };
	 void SetFromId(WORD wFromId) { m_wFromId = wFromId; };
	 void SetCurrencyType(int nCurrencyType){ m_nCurrencyType = nCurrencyType; };
	 void SetCurrencyValue(int nCurrencyValue){ m_nCurrencyValue = nCurrencyValue; };
	 void SetCurCurrencyValue(int nCurrencyValue){ m_nCurCurrencyValue = nCurrencyValue; };

	 DWORD GetRoleId() { return m_dwRoleId; };
	 WORD GetRoleLevel() { return m_wRoleLevel; };
	 WORD GetFromId() { return m_wFromId; };
	 int GetCurrencyType() { return m_nCurrencyType; };
	 int GetCurrencyValue() { return m_nCurrencyValue; };
	 int GetCurCurrencyValue() { return m_nCurCurrencyValue; };

 private:
	 DWORD m_dwRoleId;	
	 WORD m_wRoleLevel;			// 角色等级
	 WORD m_wFromId;			// 来源ID
	 int m_nCurrencyType;		// 货币类型
	 int m_nCurrencyValue;		// 货币值
	 int m_nCurCurrencyValue;	// 当前货币值
 };



// 领地数据存储
 class LandData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(LandData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tPlayerLandInfo, 0, sizeof(m_tPlayerLandInfo));
		 return TRUE;
	 }

	 void SetRoleId(DWORD dwRoleId){ m_dwRoleId = dwRoleId; };
	 DWORD GetRoleId() { return m_dwRoleId; };

	 void SetLandInfo(const PlayerLandInfo &tLandInfo)
	 {
		 memcpy_s(&m_tPlayerLandInfo, sizeof(PlayerLandInfo), &tLandInfo, sizeof(PlayerLandInfo));
	 }
	 const PlayerLandInfo& GetLandInfo(){ return m_tPlayerLandInfo; };

 private:
	 DWORD m_dwRoleId;
	 PlayerLandInfo m_tPlayerLandInfo;
 };

 // 领地数据加载
 class LandData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(LandData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 m_wLandNum = 0;
		 m_nTotalLandNum = 0;
		 m_dwMinRoleId = 0;
		 return TRUE;
	 }

	 void AddLandInfo(DWORD dwRoleId, const PlayerLandInfo &tLandInfo)
	 {
		 if (m_wLandNum >= MAX_LAND_LIST_LOAD_NUM)
			 return;
		 m_arRoleIdList[m_wLandNum] = dwRoleId;
		 memcpy_s(&m_arLandList[m_wLandNum], sizeof(PlayerLandInfo), &tLandInfo, sizeof(PlayerLandInfo));
		 ++m_wLandNum;
	 }
	
	 const PlayerLandInfo* GetLandList() { return m_arLandList; };
	 WORD GetLandListNum() { return m_wLandNum; };
	 const DWORD* GetRoleIdList() { return m_arRoleIdList; };

	 void SetMinRoleId(DWORD dwMinRoleId) { m_dwMinRoleId = dwMinRoleId; };
	 DWORD GetMinRoleId() { return m_dwMinRoleId; };

	 void SetTotalLandNum(int nTotalLandNum) { m_nTotalLandNum = nTotalLandNum; };
	 int GetTotalLandNum() { return m_nTotalLandNum; };

 private:
	 DWORD m_arRoleIdList[MAX_LAND_LIST_LOAD_NUM];
	 PlayerLandInfo m_arLandList[MAX_LAND_LIST_LOAD_NUM];
	 WORD m_wLandNum;
	 int m_nTotalLandNum;
	 DWORD m_dwMinRoleId;
 };

 // 竞技场排行榜查看列表刷新
 class ArenaRankViewList_Update : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ArenaRankViewList_Update)

 public:
	 virtual inline BOOL Init()
	 {
		 m_dwServerId = 0;
		 return TRUE;
	 }

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };

 private:
	 DWORD m_dwServerId;
 };
 

 // 留言
 class LeaveMessage_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(LeaveMessage_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_dwRoleId = 0;
		 m_dwServerId = 0;
		 memset(m_szContack, 0, sizeof(m_szContack));
		 memset(m_szMessage, 0, sizeof(m_szMessage));
		 return TRUE;
	 }

	 void SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; };
	 DWORD GetRoleId() { return m_dwRoleId; };

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };

	 void SetContack(const char *pszContack) { strcpy_s(m_szContack, sizeof(m_szContack)-1, pszContack); };
	 const char* GetContack() { return m_szContack; };

	 void SetMessage(const char *pszMessage) { strcpy_s(m_szMessage, sizeof(m_szMessage)-1, pszMessage); };
	 const char* GetMesssage() { return m_szMessage; };

 private:
	 DWORD m_dwRoleId;
	 DWORD m_dwServerId;
	 char m_szContack[MAX_CONTACT_LENGTH + 1]; // 联系方式
	 char m_szMessage[MAX_LEAVE_MESSAGE_LENGTH + 1]; // 留言内容
 };

 // 奖励中心数据存储
 class RewardCenterData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RewardCenterData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_dwRoleId = 0;
		 m_dwServerId = 0;
		 memset(&m_RewardData, 0, sizeof(m_RewardData));
		 return TRUE;
	 }

	 void SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; };
	 DWORD GetRoleId() { return m_dwRoleId; };

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };

	 void SetRewardData(const RewardData tRewardData){ memcpy_s(&m_RewardData, sizeof(RewardData), &tRewardData, sizeof(RewardData)); };
	 const RewardData& GetRewardData() { return  m_RewardData; }
 private:
	 DWORD m_dwRoleId;
	 DWORD m_dwServerId;
	 RewardData m_RewardData;
 };

 
// 日志存储
 class Log_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(Log_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_nLevel = 0;
		 memset(m_szContext, 0, sizeof(m_szContext));
		 return TRUE;
	 }

	 void SetLevel(int nLevel) { m_nLevel = nLevel; };
	 int GetLevel() { return m_nLevel; };

	 void SetContext(const char *pszContext) { strcpy_s(m_szContext, sizeof(m_szContext)-1, pszContext); };
	 const char* GetContext() { return m_szContext; };

 private:
	 int m_nLevel;	// 日志等级
	 char m_szContext[MAX_LOG_CONTEXT_LEN + 1]; // 日志内容
 };

 // 关卡评论存储
 class MapComment_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(MapComment_Save)

 public:
	 virtual inline BOOL Init()
	 {
// 		 m_dwRoleId = 0;
// 		 m_dwServerId = 0;
// 		 m_dwMapId = 0;
		 m_wCommentNum = 0;
		 m_dwId = 0;
		 return TRUE;
	 }

// 	 void SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; };
// 	 DWORD GetRoleId() { return m_dwRoleId; };
// 
// 	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
// 	 DWORD GetServerId() { return m_dwServerId; };
// 
// 	 void SetMapId(DWORD dwMapId) { m_dwMapId = dwMapId; };
// 	 DWORD GetMapId() { return m_dwMapId; };

	 void SetId(DWORD dwId) { m_dwId = dwId; };
	 DWORD GetId() { return m_dwId; };

	 const MapComment* GetCommentList() { return  m_CommentList; }
	 void AddComment(const MapComment *pRewardData)
	 {
		 if (m_wCommentNum < MAX_MAP_COMMENT_NUM)
		 {
			 //m_CommentList[m_wCommentNum++] = *pRewardData;
			 memcpy_s(&m_CommentList[m_wCommentNum++], sizeof(MapComment), pRewardData, sizeof(MapComment));
		 }
	 };

	 WORD GetCommentNum() { return m_wCommentNum; };
	
 private:
// 	 DWORD m_dwRoleId;
// 	 DWORD m_dwServerId;
// 	 DWORD m_dwMapId;
	 DWORD m_dwId;
	 MapComment m_CommentList[MAX_MAP_COMMENT_NUM];
	 WORD m_wCommentNum;
 };

 // 关卡评论数据加载
 class MapComment_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(MapComment_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 //memset(&m_BaseDataList, 0, sizeof(m_BaseDataList));
		 m_wCommentNum = 0;
		 m_dwTotalCommentNum = 0;
		 m_dwMinId = 0;
		 return TRUE;
	 }

	 void AddData(const MapComments &tCommentData)
	 {
		 if (m_wCommentNum >= ROLE_BASE_DATA_LOAD_NUM)
			 return;

		 m_CommentList[m_wCommentNum++] = tCommentData;
	 }

	 const MapComments *GetList() { return m_CommentList; };
	 WORD GetListNum() { return m_wCommentNum; };

	 DWORD GetMinId() { return m_dwMinId; };
	 void SetMinId(DWORD dwMinId) { m_dwMinId = dwMinId; };

	 DWORD GetTotalDataNum() { return m_dwTotalCommentNum; };
	 void SetTotalDataNum(DWORD dwTotalDataNum) { m_dwTotalCommentNum = dwTotalDataNum; };

 private:
	 MapComments m_CommentList[MAX_MAP_COMMENT_LOAD_NUM];
	 WORD m_wCommentNum;
	 DWORD m_dwTotalCommentNum;
	 DWORD m_dwMinId;
 };


 // 角色临时数据更新
 class RoleTmpData_Update : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleTmpData_Update)

 public:
	 virtual inline BOOL Init()
	 {
		 m_dwRoleId = 0;
		 m_dwServerId = 0;
		 memset(&m_tTempData, 0, sizeof(m_tTempData));
		 return TRUE;
	 }

	 void SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; };
	 DWORD GetRoleId() { return m_dwRoleId; };

	 void SetServerId(DWORD dwServerId) { m_dwServerId = dwServerId; };
	 DWORD GetServerId() { return m_dwServerId; };

	 void AddParam(DWORD dwValue)
	 {
		 if (m_tTempData.wParamNum >= MAX_ROLE_TEMP_DATA_PARAM_NUM)
			 return;

		 m_tTempData.arParamList[m_tTempData.wParamNum++] = dwValue;
	 }
	 void SetText(const char *pText)
	 {
		 strcpy_s(m_tTempData.szText, sizeof(m_tTempData.szText) - 1, pText);
	 }

	 void SetDataType(WORD wType) { m_tTempData.wType = wType; };

	 // 取临时数据
	 const RoleTempData& GetTempData() { return m_tTempData; };
 private:
	 DWORD m_dwRoleId;
	 DWORD m_dwServerId;
	 RoleTempData m_tTempData;
 };


 // 角色下线记录存储
 class RoleOfflineRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleOfflineRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tOfflineRecord, 0, sizeof(m_tOfflineRecord));
		 return TRUE;
	 }

	 void SetRecord(const RoleOfflineRecord &tRecord)
	 { memcpy_s(&m_tOfflineRecord, sizeof(RoleOfflineRecord), &tRecord, sizeof(RoleOfflineRecord)); };
	 const RoleOfflineRecord& GetRecord() { return m_tOfflineRecord; };

 private:
	 RoleOfflineRecord m_tOfflineRecord;
 };


 // 角色等级记录存储
 class RoleLevelRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleLevelRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tLevelRecord, 0, sizeof(m_tLevelRecord));
		 return TRUE;
	 }

	 void SetRecord(const RoleLevelRecord &tRecord)
	 {
		 memcpy_s(&m_tLevelRecord, sizeof(RoleLevelRecord), &tRecord, sizeof(RoleLevelRecord));
	 };
	 const RoleLevelRecord& GetRecord() { return m_tLevelRecord; };

 private:
	 RoleLevelRecord m_tLevelRecord;
 };

 // 角色充值记录存储
 class RoleRechargeRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleRechargeRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tRechargeRecord, 0, sizeof(m_tRechargeRecord));
		 return TRUE;
	 }

	 void SetRecord(const RechargeRecord &tRecord)
	 {
		 memcpy_s(&m_tRechargeRecord, sizeof(RechargeRecord), &tRecord, sizeof(RechargeRecord));
	 };
	 const RechargeRecord& GetRecord() { return m_tRechargeRecord; };

 private:
	 RechargeRecord m_tRechargeRecord;
 };


 // 角色充值记录存储
 class RoleMapLoseRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleMapLoseRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tRecord, 0, sizeof(m_tRecord));
		 return TRUE;
	 }

	 void SetRecord(const MapLoseRecord &tRecord)
	 {
		 memcpy_s(&m_tRecord, sizeof(MapLoseRecord), &tRecord, sizeof(MapLoseRecord));
	 };
	 const MapLoseRecord& GetRecord() { return m_tRecord; };

 private:
	 MapLoseRecord m_tRecord;
 };

 // 角色首次通关记录
 class RoleFirstPassMapRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(RoleFirstPassMapRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tRecord, 0, sizeof(m_tRecord));
		 return TRUE;
	 }

	 void SetRecord(const FirstPassMapRecord &tRecord)
	 {
		 memcpy_s(&m_tRecord, sizeof(FirstPassMapRecord), &tRecord, sizeof(FirstPassMapRecord));
	 };
	 const FirstPassMapRecord& GetRecord() { return m_tRecord; };

 private:
	 FirstPassMapRecord m_tRecord;
 };

 // 商店购买记录
 class ShopBuyRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ShopBuyRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_wShopType = 0;
		 return TRUE;
	 }
		
	 void SetShopType(WORD wShopType) { m_wShopType = wShopType; };
	 WORD GetShopType() { return m_wShopType; };

	 void SetGoodsId(DWORD dwGoodsId) { m_dwGoodsId = dwGoodsId; };
	 DWORD GetGoodsId() { return m_dwGoodsId; };

	 void SetBuyNum(DWORD wBuyNum) { m_wBuyNum = wBuyNum; };
	 WORD GetBuyNum() { return m_wBuyNum; };

 private:
	 WORD m_wShopType;
	 DWORD m_dwGoodsId;
	 WORD m_wBuyNum;
 };


 // 物品丢失记录存储
 class ItemLoseRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ItemLoseRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tLoseRecord, 0, sizeof(m_tLoseRecord));
		 return TRUE;
	 }

	 void SetRecord(const ItemLoseRecord &tRecord)
	 {
		 memcpy_s(&m_tLoseRecord, sizeof(ItemLoseRecord), &tRecord, sizeof(ItemLoseRecord));
	 };
	 const ItemLoseRecord& GetRecord() { return m_tLoseRecord; };

 private:
	 ItemLoseRecord m_tLoseRecord;
 };

 // 物品操作记录存储
 class ItemOperationRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ItemOperationRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_tRecord, 0, sizeof(m_tRecord));
		 return TRUE;
	 }

	 void SetRecord(const ItemOperationRecord &tRecord)
	 {
		 memcpy_s(&m_tRecord, sizeof(ItemOperationRecord), &tRecord, sizeof(ItemOperationRecord));
	 };

	 const ItemOperationRecord& GetRecord() { return m_tRecord; };

 private:
	 ItemOperationRecord m_tRecord;
 };

 // 在线玩家数量记录存储
 class OnlinePlayerNumRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(OnlinePlayerNumRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_wServerNum = 0;
		 return TRUE;
	 }

	 // 添加服务器在线数据
	 void AddServerOnlineData(WORD wServerId, WORD wPlayerNum)
	 {
		 if (m_wServerNum >= MAX_SERVER_NUM)
			 return;

		 ServerOnlineData onlineData;
		 onlineData.wServerId = wServerId;
		 onlineData.wOnlinePlayerNum = wPlayerNum;
		 m_arServerList[m_wServerNum++] = onlineData;
	 }

	 WORD GetServerNum() { return m_wServerNum; };
	 const ServerOnlineData* GetOnlineData(WORD wIndex)
	 {
		 if (wIndex >= m_wServerNum || wIndex >= MAX_SERVER_NUM)
			 return NULL;

		 return &m_arServerList[wIndex];
	 }

 private:
	 ServerOnlineData m_arServerList[MAX_SERVER_NUM];
	 WORD m_wServerNum;
 };


 // 帐号锁住更新
 class AccountLock_Update : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(AccountLock_Update)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(&m_szAccount, 0, sizeof(m_szAccount));
		 return TRUE;
	 }

	 const char* GetAccount() { return m_szAccount; };
	 void SetAccount(const char *pszAccount)
	 { 
		 strcpy_s(m_szAccount, sizeof(m_szAccount), pszAccount);
	 };

	 void SetPlatform(DWORD dwPlatform) { m_dwPlatform = dwPlatform; };
	 DWORD GetPlatform() { return m_dwPlatform; };

 private:
	 char m_szAccount[MAX_ACCOUNT_LEN];	//玩家账号
	 DWORD m_dwPlatform; //渠道平台
 };


 // 王者赛数据加载
 class KingMatchData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(KingMatchData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 //memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 m_wCurSeason = 1;
		 m_wDataBelongSeason = 0;
		 return TRUE;
	 }

	 void SetCurSeason(WORD wCurSeason) { m_wCurSeason = wCurSeason; };
	 WORD GetCurSeason() { return m_wCurSeason; };

	 void SetDataBelongSeason(WORD wDataBelongSeason) { m_wDataBelongSeason = wDataBelongSeason; };
	 WORD GetDataBelongSeason() { return m_wDataBelongSeason; };

	 // 取数据Buff
	 char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_KING_MATCH_STAGE_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };
	 void SetDataBuffSize(int nBuffSize) { m_nDataBuffSize = nBuffSize; };

	 //char* GetTempBuff() { return m_szTempBuff; };

 private:
	 WORD m_wCurSeason;			// 当前赛季
	 WORD m_wDataBelongSeason;	// 数据所属赛季
	 char m_szDataBuff[MAX_KING_MATCH_STAGE_DATA_SIZE];
	 int m_nDataBuffSize;

	 //char m_szTempBuff[MAX_KING_MATCH_STAGE_DATA_SIZE];
 };

 // 王者赛数据存储
 class KingMatchData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(KingMatchData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 m_wCurSeason = 1;
		 m_wDataBelongSeason = 0;
		 m_wBattleTargetNum = 0;
		 return TRUE;
	 }

	 void SetCurSeason(WORD wCurSeason) { m_wCurSeason = wCurSeason; };
	 WORD GetCurSeason() { return m_wCurSeason; };

	 void SetDataBelongSeason(WORD wDataBelongSeason) { m_wDataBelongSeason = wDataBelongSeason; };
	 WORD GetDataBelongSeason() { return m_wDataBelongSeason; };

	 void SetBattleTargetNum(WORD wBattleTargetNum) { m_wBattleTargetNum = wBattleTargetNum; };
	 WORD GetBattleTargetNum() { return m_wBattleTargetNum; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_KING_MATCH_STAGE_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:
	 WORD m_wCurSeason;			// 当前赛季
	 WORD m_wDataBelongSeason;	// 数据所属赛季
	 WORD m_wBattleTargetNum;
	 char m_szDataBuff[MAX_KING_MATCH_STAGE_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_KING_MATCH_STAGE_DATA_SIZE * 2 + 1];
 };

 // 存储组队副本排行榜数据
 class TeamDungeonRankData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(TeamDungeonRankData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_TEAM_DUNGEON_RANK_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:
	 char m_szDataBuff[MAX_TEAM_DUNGEON_RANK_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_TEAM_DUNGEON_RANK_DATA_SIZE * 2 + 1];
 };

 // 加载组队副本排行榜数据
 class TeamDungeonRankData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(TeamDungeonRankData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_TEAM_DUNGEON_RANK_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:
	 char m_szDataBuff[MAX_TEAM_DUNGEON_RANK_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_TEAM_DUNGEON_RANK_DATA_SIZE * 2 + 1];
 };

 // 激活码数据查询
 class ActivationCodeData_Query : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ActivationCodeData_Query)

 public:
	 virtual inline BOOL Init()
	 {
		 return TRUE;
	 }

// 	 // 取数据Buff
// 	 const char* GetDataBuff() { return m_szDataBuff; };
// 
// 	 // 设置数据Buff
// 	 void SetDataBuff(const char *pszBuff, int nBuffSize)
// 	 {
// 		 if (nBuffSize >= MAX_TEAM_DUNGEON_RANK_DATA_SIZE)
// 			 return;
// 		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
// 		 m_nDataBuffSize = nBuffSize;
// 	 }
// 
// 	 int GetDataBuffSize() { return m_nDataBuffSize; };
// 
// 	 char* GetTempBuff() { return m_szTempBuff; };
// 
//  private:
// 	 char m_szActivationCode[MAX_ACTIVATE_CODE_LEN + 1];
 };


 // 转盘数据存储
 class TurntableData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(TurntableData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_TURNTABLE_LIST_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };

 private:
	 char m_szDataBuff[MAX_TURNTABLE_LIST_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_TURNTABLE_LIST_DATA_SIZE * 2 + 1];
 };


 // 转盘数据加载
 class TurntableData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(TurntableData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_TURNTABLE_LIST_DATA_SIZE)
			 return;

		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 int GetDataBuffMaxSize() { return MAX_TURNTABLE_LIST_DATA_SIZE; };

	 char* GetTempBuff() { return m_szTempBuff; };

 private:
	 char m_szDataBuff[MAX_TURNTABLE_LIST_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_TURNTABLE_LIST_DATA_SIZE];
 };
 

 // 新手引导更新
 class NewRoleGuide_Update : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(NewRoleGuide_Update)

 public:
	 virtual inline BOOL Init()
	 {
		 m_nValue = 0;
		 m_dwRoleId = 0;
		 return TRUE;
	 }

	 void SetValue(int nValue) { m_nValue = nValue; };
	 int GetValue() { return m_nValue; };

	 void SetRoleId(DWORD dwRoleId) { m_dwRoleId = dwRoleId; };
	 DWORD GetRoleId() { return m_dwRoleId; };

 private:
	 int m_nValue;	// 新手引导值
	 DWORD m_dwRoleId; 
 };

 // 积分赛数据加载
 class ScoreMatchData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ScoreMatchData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 //memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 m_dwCurActivityIndex = 1;
		 m_byActivityStatus = 0;
		 m_dwActivityOverTime = 0;
		 return TRUE;
	 }

	 void SetCurActivityIndex(DWORD dwCurActivityIndex) { m_dwCurActivityIndex = dwCurActivityIndex; };
	 DWORD GetCurActivityIndex() { return m_dwCurActivityIndex; };

	 void SetActivityStatus(BYTE byActivityStatus) { m_byActivityStatus = byActivityStatus; };
	 BYTE GetActivityStatus() { return m_byActivityStatus; };

	 void SetActivityOverTime(DWORD dwActivityOverTime) { m_dwActivityOverTime = dwActivityOverTime; };
	 DWORD GetActivityOverTime() { return m_dwActivityOverTime; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_SCORE_MATCH_DATA_SIZE)
			 return;

		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };
	 void SetDataBuffSize(int nSize) { m_nDataBuffSize = nSize; };

	// char* GetTempBuff() { return m_szTempBuff; };
 private:

	 DWORD m_dwCurActivityIndex;
	 BYTE m_byActivityStatus;
	 DWORD m_dwActivityOverTime;

	 char m_szDataBuff[MAX_SCORE_MATCH_DATA_SIZE];
	 int m_nDataBuffSize;

	// char m_szTempBuff[MAX_SCORE_MATCH_DATA_SIZE * 2 + 1];
 };
 
 // 积分赛数据存储
 class ScoreMatchData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ScoreMatchData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 m_dwCurActivityIndex = 1;
		 m_byActivityStatus = 0;
		 m_dwActivityOverTime = 0;
		 m_wJoinRoleNum = 0;
		 return TRUE;
	 }

	 void SetCurActivityIndex(DWORD dwCurActivityIndex) { m_dwCurActivityIndex = dwCurActivityIndex; };
	 DWORD GetCurActivityIndex() { return m_dwCurActivityIndex; };

	 void SetActivityStatus(BYTE byActivityStatus) { m_byActivityStatus = byActivityStatus; };
	 BYTE GetActivityStatus() { return m_byActivityStatus; };

	 void SetActivityOverTime(DWORD dwActivityOverTime) { m_dwActivityOverTime = dwActivityOverTime; };
	 DWORD GetActivityOverTime() { return m_dwActivityOverTime; };

	 void SetJoinRoleNum(WORD wJoinRoleNum) { m_wJoinRoleNum = wJoinRoleNum; };
	 WORD GetJoinRoleNum() { return m_wJoinRoleNum; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_SCORE_MATCH_DATA_SIZE)
			 return;

		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:

	 DWORD m_dwCurActivityIndex;
	 BYTE m_byActivityStatus;
	 DWORD m_dwActivityOverTime;
	 WORD m_wJoinRoleNum;

	 char m_szDataBuff[MAX_SCORE_MATCH_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_SCORE_MATCH_DATA_SIZE * 2 + 1];
 };


 // 资源战数据存储
 class ResourceWarData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ResourceWarData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 m_wEnemyNum = 0;
		 m_wOccupyResourceNum = 0;
		 m_wJoinRoleNum = 0;
		 return TRUE;
	 }


	 void SetJoinRoleNum(WORD wJoinRoleNum) { m_wJoinRoleNum = wJoinRoleNum; };
	 WORD GetJoinRoleNum() { return m_wJoinRoleNum; };

	 void SetEnemyNum(WORD wEnemyNum) { m_wEnemyNum = wEnemyNum; };
	 WORD GetEnemyNum() { return m_wEnemyNum; };

	 void SetOccupyResourceNum(WORD wOccupyResourceNum) { m_wOccupyResourceNum = wOccupyResourceNum; };
	 WORD GetOccupyResourceNum() { return m_wOccupyResourceNum; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_RESOURCE_WAR_DATA_SIZE)
			 return;

		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:

	 WORD m_wJoinRoleNum;		// 参与人数
	 WORD m_wEnemyNum;			// 敌人数量
	 WORD m_wOccupyResourceNum;	// 占领的资源数量

	 char m_szDataBuff[MAX_RESOURCE_WAR_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_RESOURCE_WAR_DATA_SIZE * 2 + 1];
 };

 // 资源战数据加载
 class ResourceWarData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ResourceWarData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_RESOURCE_WAR_DATA_SIZE)
			 return;

		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };
	 void SetDataBuffSize(int nSize) { m_nDataBuffSize = nSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:

	 char m_szDataBuff[MAX_RESOURCE_WAR_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_RESOURCE_WAR_DATA_SIZE * 2 + 1];
 };


 // 工会战数据存储
 class GuildWarData_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(GuildWarData_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 m_nDataBuffSize = 0;
		 m_dwStarNum = 0;
		 m_dwJoinRoleNum = 0;
		 return TRUE;
	 }

	 void SetStarNum(DWORD dwStarNum) { m_dwStarNum = dwStarNum; };
	 DWORD GetStarNum() { return m_dwStarNum; };

	 void SetJoinRoleNum(DWORD dwJoinRoleNum) { m_dwJoinRoleNum = dwJoinRoleNum; };
	 DWORD GetJoinRoleNum() { return m_dwJoinRoleNum; };

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_GUILD_WAR_DATA_SIZE)
			 return;
		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:
	 char m_szDataBuff[MAX_GUILD_WAR_DATA_SIZE];
	 int m_nDataBuffSize;
	 DWORD m_dwStarNum;
	 DWORD m_dwJoinRoleNum;

	 char m_szTempBuff[MAX_GUILD_WAR_DATA_SIZE * 2 + 1];
 };


 // 工会战数据加载
 class GuildWarData_Load : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(GuildWarData_Load)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szDataBuff, 0, sizeof(m_szDataBuff));
		 memset(m_szTempBuff, 0, sizeof(m_szTempBuff));
		 m_nDataBuffSize = 0;
		 return TRUE;
	 }

	 // 取数据Buff
	 const char* GetDataBuff() { return m_szDataBuff; };

	 // 设置数据Buff
	 void SetDataBuff(const char *pszBuff, int nBuffSize)
	 {
		 if (nBuffSize >= MAX_GUILD_WAR_DATA_SIZE)
			 return;

		 memcpy_s(m_szDataBuff, sizeof(m_szDataBuff), pszBuff, nBuffSize);
		 m_nDataBuffSize = nBuffSize;
	 }

	 int GetDataBuffSize() { return m_nDataBuffSize; };
	 void SetDataBuffSize(int nSize) { m_nDataBuffSize = nSize; };

	 char* GetTempBuff() { return m_szTempBuff; };
 private:

	 char m_szDataBuff[MAX_GUILD_WAR_DATA_SIZE];
	 int m_nDataBuffSize;

	 char m_szTempBuff[MAX_GUILD_WAR_DATA_SIZE * 2 + 1];
 };

 // 检索激活码数据
 class ActivationCode_Select : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ActivationCode_Select)

 public:
	 virtual inline BOOL Init()
	 {
		 m_nResult = 0;
		 m_nStatus = 0;
		 m_nBatch = 0;
		 m_nType = 0;
		 m_dwRoleId = 0;
		 m_dwRewardId = 0;
		 m_dwExpireTime = 0;
		 memset(m_szActivationCode, 0, sizeof(m_szActivationCode));
		 return TRUE;
	 }

	 DWORD			GetRoleID() { return m_dwRoleId; }
	 VOID			SetRoleID(DWORD dwRoleId) { m_dwRoleId = dwRoleId; }

	 const char* GetActivationCode() { return m_szActivationCode; }
	 VOID SetActivationCode(const char* pszActivationCode)
	 {
		 if (pszActivationCode)
			 strncpy(m_szActivationCode, pszActivationCode, sizeof(m_szActivationCode));
	 }

	 // 类型
	 void SetType(int nType) { m_nType = nType; };
	 int GetType() { return m_nType; };

	 // 状态
	 void SetStatus(int nStatus) { m_nStatus = nStatus; };
	 int GetStatus() { return m_nStatus; };

	 // 批次
	 void SetBatch(int nBatch) { m_nBatch = nBatch; };
	 int GetBatch() { return m_nBatch; };

	 // 结果
	 void SetResult(int nResult) { m_nResult = nResult; };
	 int GetResult() { return m_nResult; };

	 // 奖励
	 void SetRewardId(DWORD dwRewardId) { m_dwRewardId = dwRewardId; };
	 DWORD GetRewardId() { return m_dwRewardId; };

	 // 有效时间
	 void SetExpireTime(DWORD dwExpireTime) { m_dwExpireTime = dwExpireTime; };
	 DWORD GetExpireTime() { return m_dwExpireTime; };

 private:
	 //查询条件
	 DWORD			m_dwRoleId;	// 玩家Id
	 char			m_szActivationCode[MAX_ACTIVATE_CODE_LEN];	// 激活码

	 // 结果数据
	 int			m_nResult;		// 查询结果(0=正常 1=激活码不存在 2=奖励不存在 3=已领取同批次激活码)
	 int			m_nStatus;		// 状态(0=可领取 1=已被领取)
	 int			m_nBatch;		// 批次
	 int			m_nType;		// 类型(0=个人,1=通用)
	 DWORD			m_dwRewardId;	// 奖励ID
	 DWORD			m_dwExpireTime;	// 过期时间
 };


 // 激活码数据更新
 class ActivationCode_Update : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ActivationCode_Update)

 public:
	 virtual inline BOOL Init()
	 {
		 m_nBatch = 0;
		 m_dwRoleId = 0;
		 memset(m_szActivationCode, 0, sizeof(m_szActivationCode));
		 return TRUE;
	 }

	 DWORD			GetRoleID() { return m_dwRoleId; }
	 VOID			SetRoleID(DWORD dwRoleId) { m_dwRoleId = dwRoleId; }

	 const char* GetActivationCode() { return m_szActivationCode; }
	 VOID SetActivationCode(const char* pszActivationCode)
	 {
		 if (pszActivationCode)
			 strncpy(m_szActivationCode, pszActivationCode, sizeof(m_szActivationCode));
	 }

	 // 批次
	 void SetBatch(int nBatch) { m_nBatch = nBatch; };
	 int GetBatch() { return m_nBatch; };


 private:
	 DWORD			m_dwRoleId;									// 玩家Id
	 char			m_szActivationCode[MAX_ACTIVATE_CODE_LEN];	// 激活码
	 int			m_nBatch;									// 批次
 };


 // 聊天记录存储
 class ChatRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(ChatRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 memset(m_szSender, 0, sizeof(m_szSender));
		 memset(m_szTarget, 0, sizeof(m_szTarget));
		 memset(m_szContext, 0, sizeof(m_szContext));
		 m_wChatType = 0;
		 m_wServerId = 0;
		 return TRUE;
	 }

	 void SetChatType(WORD wChatType) { m_wChatType = wChatType; };
	 DWORD GetChatType() { return m_wChatType; };

	 void SetServerId(WORD wServerId) { m_wServerId = wServerId; };
	 DWORD GetServerId() { return m_wServerId; };

	 // 发送者
	 const char* GetSender() { return m_szSender; }
	 VOID SetSender(const char* pszSender){
		 if (pszSender)
			 strncpy(m_szSender, pszSender, sizeof(m_szSender));
	 }

	 // 目标
	 const char* GetTarget() { return m_szTarget; }
	 VOID SetTarget(const char* pszTarget){
		 if (pszTarget)
			 strncpy(m_szTarget, pszTarget, sizeof(m_szTarget));
	 }

	 // 聊天内容
	 const char* GetContext() { return m_szContext; }
	 VOID SetContext(const char* pszContext){
		 if (pszContext)
			 strncpy(m_szContext, pszContext, sizeof(m_szContext));
	 }

 private:
	 WORD m_wChatType;						// 聊天类型
	 WORD m_wServerId;						// 服务器ID
	 char m_szSender[MAX_ROLE_LEN];			// 发送者
	 char m_szTarget[MAX_ROLE_LEN];			// 目标(私聊才有效)
	 char m_szContext[MAX_CHAT_TEXT_LEN];	// 聊天内容
 };

 // 游戏统计事件
 class CGameStatistics_Event : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(CGameStatistics_Event)

 public:
	 virtual inline BOOL Init()
	 {
		 m_wServerId = 0;
		 m_wType = 0;
		 m_dwTime = 0;
		 m_wParamNum = 0;
		 m_dwContext = 0;
		 return TRUE;
	 }

	 void SetTime(DWORD dwTime) { m_dwTime = dwTime; };
	 DWORD GetTime() { return m_dwTime; };

	 void AddParam(DWORD dwParam)
	 {
		 if (m_wParamNum >= MAX_GAME_STATISTICS_PARAM_NUM)
			 return;
		 m_arParamList[m_wParamNum++] = dwParam;
	 }
	 void SetParam(WORD wIdx, DWORD dwParam)
	 {
		 if (wIdx >= MAX_GAME_STATISTICS_PARAM_NUM)
			 return;
		 m_arParamList[wIdx] = dwParam;
	 }
	 DWORD GetParam(WORD wIdx)
	 {
		 if (wIdx >= MAX_GAME_STATISTICS_PARAM_NUM)
			 return 0;
		 return m_arParamList[wIdx];
	 }


	 WORD GetServerNum() { return m_wParamNum; };

	 WORD GetType() { return m_wType; };
	 void SetType(WORD wType) { m_wType = wType; };

	 WORD GetServerId() { return m_wServerId; };
	 void SetServerId(WORD wServerId) { m_wServerId = wServerId; };

	 DWORD GetContext() { return m_dwContext; };
	 void SetContext(DWORD dwContext) { m_dwContext = dwContext; };

 private:
	 WORD m_wType;											// 游戏统计类型
	 DWORD m_dwTime;										// 统计时间戳
	 WORD m_wServerId;										// 服务器ID
	 DWORD m_arParamList[MAX_GAME_STATISTICS_PARAM_NUM];	// 服务器列表
	 WORD m_wParamNum;
	 DWORD m_dwContext;										
 };


 // GM操作记录
 class CGMOperationRecord_Save : public QueryForUser
 {
	 _DECLARE_QUERY_POOL(CGMOperationRecord_Save)

 public:
	 virtual inline BOOL Init()
	 {
		 m_wGMCmd = 0;
		 m_dwTime = 0;
		 m_wParamNum = 0;
		 return TRUE;
	 }

	 void SetGMCmd(WORD wGMCmd){ m_wGMCmd = wGMCmd; };
	 WORD GetGMCmd(){ return m_wGMCmd; };

	 void SetTime(DWORD dwTime) { m_dwTime = dwTime; };
	 DWORD GetTime() { return m_dwTime; };

	 void AddParam(DWORD dwParam)
	 {
		 if (m_wParamNum >= MAX_GM_CMD_PARAM_NUM)
			 return;
		 m_arParamList[m_wParamNum++] = dwParam;
	 }
	 void SetParam(WORD wIdx, DWORD dwParam)
	 {
		 if (wIdx >= MAX_GM_CMD_PARAM_NUM)
			 return;
		 m_arParamList[wIdx] = dwParam;
	 }
	 DWORD GetParam(WORD wIdx)
	 {
		 if (wIdx >= MAX_GM_CMD_PARAM_NUM)
			 return 0;
		 return m_arParamList[wIdx];
	 }

 private:
	 WORD m_wGMCmd;								// gm命令
	 DWORD m_dwTime;							// 记录时间戳
	 DWORD m_arParamList[MAX_GM_CMD_PARAM_NUM];	// 服务器列表
	 WORD m_wParamNum;
 };