#pragma once
#include "Character.h"
#include "../Common/DataDefine.h"
#include "../ToolKit/Timer.h"
#include "../Common/DBCDefine.h"
#include "../Common/MsgDefine.h"
#include "common_msg.pb.h"
#include "header.pb.h"

class AccountInfo;
class ServerSession;
class NetMsgBody;
class CItemManager;
class CTreasureManager;
class CHeroManager;
class CEquipmentManager;
class CFubenManager;
class CSpriteManager;
class CShopManager;
class EmailManager;
class CFriendManager;
class CRecyclingManager;
class CDuobaoManager;
class CAchievementManager;
class CDailyMissionManager;
class CVipManager;
class CLogicManager;
class CPropertyManager;
class CSevenDayManager;
class CZhanBuManager;
class CTitleManager;
class CActivityManager;
class CRedPointManager;
class CRewardCenterManager;
class CRechargeManager;
class CEndlessLandManager;
class CGemManager;
class CWorldLevelTaskMananger;
class CHangUpDropMgr;


//玩家类
class Player : public Character
{
public:
	Player();
	~Player();

	BOOL				Init();
	VOID				Release();
	VOID				Update(DWORD dwTick);
	void			    UpdateOnlineTime( DWORD dwTick );
	VOID				Create( ServerSession* pSession, char* pszAccID, DWORD dwCharID, DWORD dwGuid, AccountInfo* pAccountInfo );

	//帐号相关
	DWORD				GetGuid() const { return m_dwGuid; }
	VOID				SetGuid(DWORD dwGuid) { m_dwGuid = dwGuid; }
	WORD				GetServerId();
	WORD				GetInitServerId();
	DWORD				GetCharID() const { return m_tRoleFullInfo.tRoleBaseInfo.dwRoleId; }
	DWORD				GetRoleId() { return m_tRoleFullInfo.tRoleBaseInfo.dwRoleId; };
	AccountInfo*		GetAccountInfo() { return m_pAccountInfo; }
	const char*			GetAccID() { return m_tRoleFullInfo.tRoleBaseInfo.szAccount; }
	VOID				SetRoleName(const char* pszRoleName);
	const char*			GetUTF8RoleName() { return m_szUTF8RoleName; };
	const char*			GetRoleName() { return m_tRoleFullInfo.tRoleBaseInfo.szRoleName; };
	virtual USHORT		GetLevel() const { return m_tRoleFullInfo.tRoleBaseInfo.wLevel; }
	VOID				SetLevel(WORD wLevel);
	WORD				GetDailyInitLevel() { return m_tRoleFullInfo.tRoleBaseInfo.wDailyInitLevel; };
	DWORD				GetLoginTimes()	{ return m_tRoleFullInfo.tRoleBaseInfo.dwLoginTimes; };
	VOID				SetLastHeartBeatTime(DWORD dwLastHeartBeatTime) { m_dwLastHeartBeatTime = dwLastHeartBeatTime; };
	DWORD				GetLastHeartBeatTime() { return m_dwLastHeartBeatTime; };
	void				SetHeartBeatFastTimes(WORD wHeartBeatFastTimes) { m_wHeartBeatFastTimes = wHeartBeatFastTimes; };
	WORD				GetHeartBeatFastTimes() { return m_wHeartBeatFastTimes; };
	void				SetNetworkSeriesId(DWORD dwNetworkSeriesId) { m_dwNetworkSeriesId = dwNetworkSeriesId; };
	DWORD				GetNetworkSeriesId() { return m_dwNetworkSeriesId; };
	const MapPos&		GetMapPos(){ return m_tMapPos; };
	VOID				SetMapPos(int nX, int nY);
	void				AddRechargeAmount(DWORD dwRechargeAmount);
	DWORD				GetTeamId() { return m_dwTeamId; };
	void				SetTeamId(DWORD dwTeamId) { m_dwTeamId = dwTeamId; };
	WORD				GetTeamDungeonId() { return m_wTeamDungeonId; };
	void				SetTeamDungeonId(WORD wTeamDungeonId) { m_wTeamDungeonId = wTeamDungeonId; };
	WORD				GetRoleCreateDays();
	DWORD				GetRoleCreateTime() { return m_tRoleFullInfo.tRoleBaseInfo.dwRoleCreateTime; };
	WORD				GetRoleLoginDays() { return m_tRoleFullInfo.tRoleBaseInfo.wLoginDays; };
	DWORD				GetLastChatTime() { return m_dwLastChatTime; };
	void				SetLastChatTime(DWORD dwLastChatTime) { m_dwLastChatTime = dwLastChatTime; };
	DWORD				GetLastLogoutTime() { return m_tRoleFullInfo.tRoleBaseInfo.dwLastLogoutTime; };
	DWORD				GetLastLoginTime() { return m_tRoleFullInfo.tRoleBaseInfo.dwLastLoginTime; };
	DWORD				GetChannelType();
	bool				IsHaveSpriteTalentSkillReward() { return m_bIsHaveSpriteTalentSkillReward; };
	void				SetIsHaveSpriteTalentSkillReward(bool bIsHave) { m_bIsHaveSpriteTalentSkillReward = bIsHave; };
	WORD				GetVipGMLevel();
	
	
	//属性相关
	VOID				SetRoleFullInfo(RoleFullInfo& tCharFullInfo);
	RoleFullInfo&		GetRoleFullInfo() { return m_tRoleFullInfo; }

	// 填充基本数据
	void				FillCommonData(PlayerCommonData *pBaseData);

	// 填充基本数据
	void				FillBaseData(PlayerBaseData *pBaseData);

	// 填充上阵英雄数据
	void				FillToBattleHeroData(vector<ToBattleHeroData> &vHeroList);

	void				ModifyPropertyList(const vector<Property> &vProList, BOOL bNotice = TRUE, WORD wFromId = 0);
	void				ModifyProperty(WORD wPropertyType, INT32 nValue = 1, BOOL bNotice = TRUE, WORD wFromId = 0, BOOL bSaveLog = TRUE);
	void				SetProperty(WORD wPropertyType, INT32 nValue, BOOL bNotice = TRUE);
	INT32				GetProperty(WORD wPropertyType);
	void				NotifyProperty(WORD wPropertyType, INT32 nValue);
	DWORD				GetFightingPower(bool bIsUpdate = false);

	bool				AddObject(DWORD dwObjectId, DWORD dwOjbectNum = 1, BOOL bNotice = TRUE, WORD wFromId = 0);
	bool				AddObjectEx(BYTE byType, DWORD dwObjectId, DWORD dwOjbectNum = 1, BOOL bNotice = TRUE, WORD wFromId = 0);
	bool				AddObjectData(const Item *pItem, BOOL bNotice = TRUE, WORD wFromId = 0);
	bool				AddObjectList(const vector<Item> &vItemList, BOOL bNotice = TRUE, WORD wFromId = 0);

	// 存储货币日志
	void				SaveCurrencyLog(WORD wPropertyType, INT32 nValue, WORD wFromId);




	// 添加经验
	void				AddExp(DWORD dwExp);

	//事件处理	
	VOID				OnLoad(RoleFullInfo& sRoleInfo);
	VOID				OnSave(SaveDBType enSaveDBType);
	VOID				OnLogin();
	VOID				OnLogout();
	VOID				OnLevelUp();

	// 新的一天(每日0点)
	VOID				OnNewDay();

	// 新的一周(每周一0点)
	VOID				OnNewWeek();

	// 新的一月(每个月1号0点)
	void				OnNewMonth();

	// 处理临时数据
	VOID				DoTempData(bool isSave = true);

	// 是否新建角色
	BOOL				IsNewRole();
	VOID				OnNewRole();
	BOOL				IsTodayCreateRole();

	//消息相关
	BOOL				SendPacket(NetMsgBody * pMsg);
	VOID				SendRoleBaseInfo();
	VOID				SendErrorMsg(WORD wErrorCode, WORD wFromMsgId = 0);

 	ERROR_CODE			HandleMissionScript(const char* pszScriptName, const char* pszScriptEventName, INT nEventPara = 0);

	//任务脚本用接口
	BOOL				IsAcceptMission(DWORD dwMissionID);
	VOID				TriggerMission(DWORD dwMissionID);

	// 角色信息更新
	void				RoleInfoUpdate(bool isSave = true);

////// 模块逻辑接口
	// 更新成就
	void				OnUpdateAchievement(WORD wAchievementId, int nValue = 1, bool bAdd = true);

	// 取VIP特权值
	int					GetVipValue(BYTE byType);

	// 更新日常任务
	void				OnUpdateDailyMission(WORD wMissionType, int nValue = 1, bool bAdd = true);

	// 取物品数量
	DWORD				GetItemNum(DWORD dwItemId);

	// 删除物品
	void				RemoveItem(DWORD dwItemId, DWORD dwItemNum = 1, WORD wFromId = 0);

	// 删除物品
	void				RemoveItem(const Item &tItem, WORD wFromId);

	// 更新战力
	void				UpdateFightingPower();

	// 取工会技能
	const SkillData*	GetGuildSkill(WORD wSkillId);

	// 添加工会技能
	void				AddGuildSkill(WORD wSkillId, WORD wSkillLevel);

	// 填充工会技能列表
	void				FillGuildSkillList(vector<SkillData> &vSkillList);

	// 查询目标详细信息回调
	void				SelectTargetDetailsCallBack(BYTE byRequestType, const RoleFullInfo *pTargetFullInfo);

	// 检测货币消耗
	bool				CheckMoneyCost(const vector<CostData> &vCostList, WORD wNum = 1);

	// 检测物品消耗
	bool				CheckItemCost(const vector<Item> &vCostItemList, WORD wNum = 1);

	// 处理货币消耗
	void				DoMoneyCost(const vector<CostData> &vCostList, WORD wNum = 1, WORD wFromId = 0);

	// 取当前使用的称号Id
	int					GetCurUseTitleId();

	// 检测一次性标志
	BOOL CheckOnceFlag(int nFlagIdx);

	// 设置一次性标志
	VOID SetOnceFlag(int nFlagIdx);

	// 清除一次性标志
	VOID ClearOnceFlag(int nFlagIdx);

	// 更新7天目标数据
	void OnUpdateSevenDayTarget(WORD wLogicType);

	// 更新7天目标数据
	void OnUpdateSevenDayTarget(WORD wLogicType, DWORD dwX, bool bAdd);

	// 节日活跃目标事件
	void OnFestivalActiveTarget(WORD wActivityType, WORD wLogicType, DWORD dwX, bool bAdd);

	// 存储物品丢失记录
	void SaveItemLoseRecord(DWORD dwItemId, DWORD dwItemNum = 1);

	// 锁住帐号
	void LockAccount();

	// 更新世界任务事件
	void OnUpdateWorldTaskEvent(WORD wTaskType, int nContext = 1);

	// 检测并重置玩家圣诞节数据
	void CheckAndResetChristmasData();


public:

	EmailManager*			GetEmailManager()		{ return m_pEmailManager; }
	CItemManager*			GetItemManager()		{ return m_pItemMgr; };
	CHeroManager*			GetHeroManager()		{ return m_pHeroMgr; };
	CEquipmentManager*		GetEquipmentManager()	{ return m_pEquipmentMgr; };
	CTreasureManager*		GetTreasureManager()	{ return m_pTreasureMgr; };
	CFubenManager*			GetFubenManager()		{ return m_pFubenMgr; };
	CSpriteManager*			GetSpiritManager()		{ return m_pSpiritMgr; };
	CShopManager*			GetShopManager()		{ return m_pShopMgr; };
	CFriendManager*			GetFriendManager()		{ return m_pFriendMgr; };
	CRecyclingManager*		GetRecyclingManager()	{ return m_pRecyclingMgr; };
	CDuobaoManager*			GetDuobaoManager()		{ return m_pDuobaoMgr; };
	CAchievementManager*	GetAchievementManager() { return m_pAchievementMgr; };
	CDailyMissionManager*	GetDailyMissionManager(){ return m_pDailyMissionMgr; };
	CVipManager*			GetVipManager()			{ return m_pVipMgr; };
	CLogicManager*			GetLogicManager()		{ return m_pLogicMgr; };
	CPropertyManager*		GetPropertyManager()	{ return m_pPropertyManager; };
	CSevenDayManager*		GetSevenDayManager()	{ return m_pSevenDayManager; };
	CZhanBuManager*			GetZhanBuManager()		{ return m_pZhanBuManager; };
	CTitleManager*			GetTitleManager()		{ return m_pTitleManager; };
	CActivityManager*		GetActivityManager()	{ return m_pActivityManager; };
	CRedPointManager*		GetRedPointManager()	{ return m_pRedPointManager; };
	CRewardCenterManager*	GetRewardCenterManager(){ return m_pRewardManager; };
	CRechargeManager*		GetRechargeManager()	{ return m_pRechargeManager; };
	CEndlessLandManager*	GetEndlessLandManager() { return m_pEndlessLandManager; };
	CGemManager*			GetGemManager()			{ return m_pGemManager; };
	CWorldLevelTaskMananger*GetWorldLevelTaskManager(){ return m_pWorldLevelTaskMgr; };
	CHangUpDropMgr*			GetHangUpDropManager()	{ return m_pHangUpDropMgr; };
	

	// 测试接口
public:
	void PrintPVPData(bool isLogin);


private:
	BOOL				IsNewDay();
	BOOL				IsNewWeek();
	BOOL				IsNewMonth();

private:
	char				m_szAccID[MAX_ACCOUNT_LEN];				//玩家账号ID
	DWORD				m_dwGuid;								//玩家标识ID(标识GateSvr上对应的玩家)
	AccountInfo*		m_pAccountInfo;							//玩家账号信息
	RoleFullInfo		m_tRoleFullInfo;						//玩家完整的游戏信息
	Timers				m_cSaveDBTimers;						//DB存储计时器

	EmailManager*			m_pEmailManager;                     	//邮件管理器
	CItemManager*			m_pItemMgr;								//物品管理器
	CHeroManager*			m_pHeroMgr;								//英雄管理器
	CTreasureManager*		m_pTreasureMgr;							//宝物管理器
	CEquipmentManager*		m_pEquipmentMgr;						//装备管理器
	CFubenManager*			m_pFubenMgr;							//副本管理器
	CSpriteManager*			m_pSpiritMgr;							//精灵管理器
	CShopManager*			m_pShopMgr;								//商店管理器
	CFriendManager*			m_pFriendMgr;							//好友管理器
	CRecyclingManager*		m_pRecyclingMgr;						//回收管理器
	CDuobaoManager*			m_pDuobaoMgr;							//夺宝管理器
	CAchievementManager*	m_pAchievementMgr;						//成就管理器
	CDailyMissionManager*	m_pDailyMissionMgr;						//任务管理器
	CVipManager*			m_pVipMgr;								//VIP管理器
	CLogicManager*			m_pLogicMgr;							//小功能逻辑管理器
	CPropertyManager*		m_pPropertyManager;						//属性管理器
	CSevenDayManager*		m_pSevenDayManager;						//7天管理器
	CZhanBuManager*			m_pZhanBuManager;						//占卜管理器
	CTitleManager*			m_pTitleManager;						//称号管理器
	CActivityManager*		m_pActivityManager;						//活动管理器
	CRedPointManager*		m_pRedPointManager;						//红点管理器
	CRewardCenterManager*	m_pRewardManager;						//奖励管理器
	CRechargeManager*		m_pRechargeManager;						//充值管理器
	CEndlessLandManager*	m_pEndlessLandManager;					//无尽之地管理器
	CGemManager*			m_pGemManager;							//宝石管理器
	CWorldLevelTaskMananger*m_pWorldLevelTaskMgr;					//世界等级任务管理器
	CHangUpDropMgr*			m_pHangUpDropMgr;						//挂机掉落管理器


	//Timers				m_StaTimes;								//体力恢复计时器
	char				m_szUTF8RoleName[MAX_ROLE_LEN+1];			//角色名
	DWORD				m_dwLastHeartBeatTime;						//最近一次心跳时间
	WORD				m_wHeartBeatFastTimes;						//心跳过快次数
	MapPos				m_tMapPos;									//地图坐标 
	DWORD				m_dwNetworkSeriesId;						//网络系列ID
	DWORD				m_dwTeamId;									//队伍ID
	WORD				m_wTeamDungeonId;							//组队副本ID
	DWORD				m_dwLastChatTime;							//最近一次聊天时间
	bool				m_bIsHaveSpriteTalentSkillReward;			//是否有精灵天赋技能奖励

	int	m_arProList[MAX_BATTLE_HERO_NUM][HERO_PRO_MAX];			// 阵型属性列表

	ServerSession*		m_pSession;								//网关服务器会话对象
};
