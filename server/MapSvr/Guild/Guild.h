/**\file
工会逻辑类
\author Kevin
\date 2016/7/25
\note
*/

#pragma once
#include "DataDefine.h"
#include "data_define.pb.h"
class Player;
class CGuildManager;

// 最近泉水(设计)信息
struct LastQYInfo
{
	DWORD dwRoleId;	// 泉水玩家ID
	DWORD dwTime;	// 泉水时间
	DWORD dwValue;	// 泉水值
	BYTE byQSType;	// 泉水类型
};

class CGuild
{
	friend class CGuildManager;
public:
	CGuild();
	~CGuild();

	bool Init(Player *pCreater, CGuildManager *pGuildMgr, WORD wIconId, const char *pszName);
	void Release();

	void OnNewDay();

	void OnNewWeek();

	// 存储数据到数据库
	void SaveDataToDB(bool isDelete = false);

	// 从数据库加载数据
	void LoadDataFromDB(CGuildManager *pGuildMgr, PB::GuildInfo *pPBGuildInfo);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer);

	// 是否有红点
	bool IsHaveRedPoint(Player *pPlayer, const GuildMember *pMember, BYTE byType);

	// 拷贝数据
	void Copy(CGuild *pGuild);

//////////// 副本相关
public:

	// 战斗开始
	bool OnBattleStart(Player *pPlayer, BYTE byCamp);

	// 战斗结束
	bool OnBattleOver(Player *pPlayer, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 领取副本章节奖励
	void OnNetCopyChapterReward(Player *pPlayer, WORD wChapter);

	// 领取副本关卡奖励
	void OnNetCopyMapReward(Player *pPlayer,WORD wChapter, BYTE byCamp, BYTE byBoxIdx);

	// 请求关卡奖励记录
	void OnNetCopyMapRewardRecord(Player *pPlayer, WORD wChapter, BYTE byCamp);

	// 请求成员副本战绩
	void OnNetCopyBattleRecord(Player *pPlayer);

	// 购买副本挑战次数
	void OnNetBuyCopyBattleTimes(Player *pPlayer, BYTE byBuyTimes);

	// 副本章节重置
	void OnNetFBChapterReset(Player *pPlayer, BYTE byIsHistoryRecord);

	// 请求章节关卡奖励状态
	void OnNetMyMapRewardRecord(Player *pPlayer);
public:

	// 创建副本怪物
	void CreateMonster(bool bResetMemberData = true);

	// 是否怪物已死亡
	bool IsMonsterDead(BYTE byCamp);

	// 是否所有阵营怪物都挂掉
	bool IsAllCampMonsterDead();

	// 更新怪物伤害
	void UpdateMonsterDamage(BYTE byCamp, MonsterData *pMonsterList, BYTE byMonsterNum);

	// 当前怪物HP
	DWORD GetMonsterCurHP(BYTE byCamp);

	// 是否副本开放时间
	bool IsFBOpenTime();

	// 添加副本伤害记录
	void AddFBDamageRecord(DWORD dwRoleId, DWORD dwDamage);

public:

	// 取工会Id
	DWORD GetGuildId() { return m_tData.dwId; };

	// 取工会所在服务器ID
	DWORD GetServerId() { return m_dwSeverId; };

	// 设置工会所有的服务器ID
	void SetServerId(DWORD dwServerId) { m_dwSeverId = dwServerId; };

	// 取工会名称
	const char* GetGuildName() { return m_tData.szName; };

	// 取会长名称
	const char* GetCaptainName();

	// 取会长ID
	DWORD GetCaptainId();

	// 取工会等级
	WORD GetGuildLevel() { return m_tData.wLevel; };

	// 设置工会图标
	void SetGuildIconId(WORD wIconId) { m_tData.wIconId = wIconId; };

	// 取工会图标
	WORD GetGuildIconId() { return  m_tData.wIconId; };

	// 设置宣言
	void SetDeclaration(const char *pszDeclaration);

	// 设置公告
	void SetNotice(const char *pszNotice);

	// 添加经验
	void AddExp(DWORD dwExp);
	
	// 设置经验
	void SetExp(DWORD dwExp) { m_tData.dwExp = dwExp; };

	// 添加活跃
	void AddActive(DWORD dwActive) { m_tData.dwActive += dwActive; };

	// 取活跃
	DWORD GetActive() { return m_tData.dwActive; };

	// 添加贡献值
	void AddContribution(DWORD dwRoleId, int nContribution);

	// 添加泉水值
	void AddQSValue(DWORD dwValue) { m_tData.dwQuanShui += dwValue; };

	// 工会聊天
	void Chat(NetMsgBody *pMsg);

	const GuildData& GetGuildData() { return m_tData; };

	// 填充工会数据
	void FillGuildData(GuildBaseData &tGuildData);

	// 设置成员职位
	void SetMemberPos(DWORD dwRoleId, BYTE byPos);

	// 取成员职位
	BYTE GetMemberPos(DWORD dwRoleId);

	// 取某个职位上的人数据
	BYTE GetMemberNumByPos(BYTE byPos);

	// 取成员泉涌类型
	BYTE GetMemberQYType(DWORD dwRoleId);

	// 添加成员泉涌次数
	void SetMemberQYType(DWORD dwRoleId, BYTE byType);

	// 取工会泉涌次数
	BYTE GetGuildQYTimes() { return m_tData.byQYTimes; };

	// 添加工会泉涌总次数
	void AddGuildQYTimes(BYTE byTimes = 1) { m_tData.byQYTimes += byTimes; };

	// 是否已领取了冰泉奖励
	bool IsGetBQReward(DWORD dwRoleId, WORD wQuanShui);

	// 添加泉水奖励
	void AddQSRewardRecord(DWORD dwRoleId, WORD wQuanShui);

	// 取泉水
	int GetQuanShui() { return m_tData.dwQuanShui; };

	// 取技能
	const SkillData* GetSkillData(WORD wSkillId);

	// 添加技能
	void AddSkillData(WORD wSkillId, WORD wSkillLevel);

	// 添加聊天记录
	void AddChatRecord(const ChatRecord &tRecord);

	// 重置挑战次数 todo:: for test
	void ResetBattleTimes(Player *pPlayer);
	void ResetQYTimes(Player *pPlayer);

	// 给所有成员发奖励
	void SendRewardToAllMember(const RewardData &tRewardData);

public:
	// 发送工会数据
	void SendGuildData(Player *pPlayer);

	// 发送工会数据
	void SendGuildData(DWORD dwRoleId);

	// 发送成员列表
	void SendMemberList(Player *pPlayer);

	// 发送留言消息列表 
	void SendLeaveMsgList(Player *pPlayer);

	// 发送申请列表
	void SendApplyList(Player *pPlayer);

	// 发送事件列表 
	void SendEventList(Player *pPlaye);

	// 发送限时商品购买记录
	//void SendLimitGoodsBuyRecord(Player *pPlayer);

	// 发送限时商品列表
	void SendLimitGoodsList(Player *pPlayer);

	// 发送技能列表
	void SendSkillList(Player *pPlayer);

	// 发送聊天记录
	void SendChatRecordList(Player *pPlayer);

	// 发送工会副本数据
	void SendCopyData(Player *pPlayer);

	// 发送怪物数据
	void SendMonsterData(Player *pPlayer);

	// 发送消息给所有成员
	void SendMsgToAllMember(NetMsgBody * pMsg);

	// 广播工会技能数据
	void SendSkillDataToAll();
public:
	// 添加成员
	bool AddMember(const GuildMember *pMember);
	
	// 删除成员
	bool RemoveMember(DWORD dwRoleId);

	// 取成员数据
	const GuildMember* GetMemberData(DWORD dwRoleId);

	// 取会长信息
	const GuildMember* GetCaptainData();

	// 取工会成员数量
	BYTE GetMemberNum() { return m_tData.byMemberNum; };

	// 解散
	void Dismiss();

	// 工会成员是否已满
	bool IsMemberFull();

	// 添加申请玩家数据
	void AddApplyData(Player *pPlayer);

	// 删除申请玩家数据
	void RemoveApplyData(DWORD dwRoleId);

	// 取申请玩家数据
	GuildApplyData* GetApplyData(DWORD dwRoleId);

	// 检测是否在申请列表
	bool IsInApplyList(DWORD dwRoleId);

	// 添加留言消息
	void AddLeaveMessage(const GuildLeaveMessage &sLeaveMsg);

	// 取商品购买记录
	//const ShopGoodsBuyRecord* GetGoodsBuyRecord(WORD wGoodsId);

	// 添加商品购买记录
	//bool AddGoodsBuyRecord(WORD wGoodsId, WORD wGoodsNum);

	// 更新限时商品 
	void UpdateLimitGoods(vector<Goods> &vGoodsList);

	// 添加事件
	void AddEvent(const GuildEvent &tEvent);

	// 添加事件
	void AddEvent(WORD wEventId, const char *pszTargetName, DWORD dwValue = 0);

	// 设置最近泉水信息
	void SetLastQSInfo(DWORD dwRoleId, DWORD dwValue, BYTE byType);

	// 取限时商品
	const Goods* GetLimitGoods(WORD wGoodsId);

	// 添加限时商品购买数量
	void AddLimitGoodsBuyNum(WORD wGoodsId, WORD wBuyNum = 1);

	// 取限时商品列表
	const vector<Goods>& GetLimitGoodsList() { return m_vLimitGoodsList; };

	// 取成员战力排行列表
	void GetMemeberPowerRankList(vector<RolePower> &vPlayerList);

protected:
	// 查找成员数据
	GuildMember* FindMemberData(DWORD dwRoleId);

	// 更新挑战次数
	void UpdateBattleTimes(GuildMember *pMember);


protected:

	// 为合服准备的接口，目前暂时使用不上
	const list<GuildEvent>& GetEventList() { return m_EventList; };
	const list<GuildLeaveMessage>& GetMessageList() { return m_MessageList; };
	const vector<GuildApplyData>& GetApplyData() { return m_ApplyList; };
	const vector<SkillData>& GetSkillDataList() { return m_SkillList; };
	const map<DWORD, DWORD>& GetDamageRecordList() { return m_DamageRecordList; };
	const map<WORD, GuildFBMapReward>& GetMapRewardList() { return m_MapRewardRecordList; };
	const list<ChatRecord>& GetChatRecord() { return m_ChatRecordList; };
	const LastQYInfo& GetLastQYInfo() { return m_tLastQYInfo; };

private:
	CGuildManager *m_pGuildMgr;

	DWORD m_dwSeverId;

	DWORD m_dwCreateMonterTime;	// 创建怪物时间

	// 工会信息
	GuildData m_tData;	

	// 工会事件列表
	list<GuildEvent> m_EventList;

	// 留言列表
	list<GuildLeaveMessage> m_MessageList; 

	// 申请进入列表
	vector<GuildApplyData> m_ApplyList;

	// 限时物品购买记录列表
	//map<WORD, ShopGoodsBuyRecord> m_GoodsBuyRecordList;

	// 限时商品列表
	//map<WORD, Goods> m_LimitGoodsList;
	vector<Goods> m_vLimitGoodsList;

	// 技能列表
	vector<SkillData> m_SkillList;

	// 工会副本怪物列表(当前正在挑战的章节怪物)
	vector<MonsterData> m_arMonsterList[CAMP_NUM];
	string m_KillerList[CAMP_NUM];

	// 伤害记录列表[role_id,damage]
	map<DWORD, DWORD> m_DamageRecordList;	

	// 关卡奖励领取记录[chapter,reward]
	map<WORD, GuildFBMapReward> m_MapRewardRecordList;

	// 聊天记录列表
	list<ChatRecord> m_ChatRecordList;

	// 最近泉涌信息
	LastQYInfo m_tLastQYInfo;
};