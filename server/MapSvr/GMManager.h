#pragma once
#include "../ToolKit/Singleton.h"
#include <map>
#include <string>
#include "Const.h"

class Player;

enum DB_GM_CMD
{
	DB_GM_SYETM_MAIL = 1,					// 系统邮件
	DB_GM_REWARD_CENTER = 2,				// 奖励中心发奖励(指定服务器)
	DB_GM_RELOAD_SERVER_CONFIG = 3,			// 重新加载配置文件
	DB_GM_RELOAD_SERVER_LIST = 4,			// 重新加载服务器列表
	DB_GM_SYSTEM_NOTICE = 5,				// 系统公告
	DB_GM_RANK_VIEW_REFRESH = 6,			// 竞技场排行榜查看列表刷新
	DB_GM_SEND_REWARD_TO_ONE = 7,			// 发放奖励(个人)
	DB_GM_SEND_REWARD_TO_ALL = 8,			// 发放奖励(服务器)
	DB_GM_KICK_OUT = 9,						// 踢人下线
	DB_GM_SILENCE = 10,						// 禁言
	DB_GM_KICK_OUT_ALL = 11,				// 踢人所有人下线
	DB_GM_DEL_RANK_DATA = 12,				// 删除排行榜用户数据
	DB_GM_RANK_REFRESH = 13,				// 排行榜重新排序
	DB_GM_RESET_GUIDE = 14,					// 重置引导 
	DB_GM_CLIENT_UPDATE_NOTIFY = 15,		// 广播客户端更新
	DB_GM_SEND_ARENA_REWARD = 16,			// 发放竞技场奖励
	DB_GM_SEND_WORLD_BOSS_REWARD = 17,		// 发放世界
	DB_GM_SEND_SEVEN_DAY_RANK_REWARD = 18,	// 发放7天排行奖励 
	DB_GM_RELOAD_INIT_FILE = 19,			// 重新加载init文件
	DB_GM_SAVE_ALL_DATA = 20,				// 所有数据存储
	DB_GM_RECHARGE = 21,					// 充值
	DB_GM_SYETM_MAIL_EX = 22,				// 系统邮件
	DB_GM_RESET_CHRISTMAS = 23,				// 重置圣诞节活动
	DB_GM_SERVER = 6978,					// 合服操作

};


// GM指令
enum GMCmd
{
	GM_MY_TEST = 9999,
	GM_SET_LEVEL = 1,						//1.设置等级
	GM_ADD_EXP,								//2.添加经验
	GM_SET_VIP,								//3.设置VIP等级
	GM_SET_GOLD,							//4.设置金币
	GM_SET_STAMINA,							//5 设置体力
	GM_SET_DIAMOND,							//6 设置钻石
	GM_SET_ENERGY,							//7 设置精力 
	GM_ADD_HERO,							//8 添加英雄
	GM_ADD_EQUIPMENT,						//9 添加装备
	GM_ADD_ITEM,							//10 添加物品
	GM_ADD_TREASURE,						//11 添加宝物
	GM_ADD_REBEL,							//12 添加叛军
	GM_ADD_REBEL_VALUE,						//13 添加叛军值(战功)
	GM_ADD_REBEL_DAMAGE,					//14 添加叛军伤害值
	GM_ADD_REBEL_EXPLOIT,					//15 添加叛军功勋值
	GM_ADD_TOWER_PRESTIGE,					//16 添加爬塔声望(威名)
	GM_ADD_TOWER_STAR,						//17 添加爬塔星数
	GM_ADD_LAND_PATROL_TIME,				//18 添加领地巡逻时间
	GM_ADD_ARENA_PRESTIGE,					//19 添加竞技场声望
	GM_ADD_GUILD_QS,						//20 添加工会泉水	
	GM_ADD_GUILD_CONTRIBUTION,				//21 添加工会贡献值	
	GM_ADD_VIP_EXP,							//22 添加VIP经验	
	GM_ADD_DAILY_MISSION_POINT,				//23 添加日常任务积分
	GM_ADD_ACHIEVEMENT_VALUE,				//24 添加成就副本值
	GM_ADD_HERO_SOUL,						//25 添加雄魂
	GM_WORLD_BOSS_START,					//26 世界活动开始
	GM_WORLD_BOSS_OVER,						//27 世界活动结束
	GM_ADD_GUILD_EXP,						//28 添加工会经验
	GM_SET_BOSS_LEVEL,						//29 设置BOSS等级
	GM_ADD_BOSS_HONOR,						//30 添加BOSS荣誉
	GM_ADD_BOSS_DAMAGE,						//31 添加BOSS伤害
	GM_PASS_ALL_MAIN_MAP,					//32 通关所有主线副本关卡
	GM_SET_PASS_TOWER_NORMAL_MAP_IDX,		//33 设置爬塔普通关卡通关数
	GM_SET_PASS_TOWER_ELITE_MAP_IDX,		//34 设置爬塔精英关卡通关数
	GM_ADD_TITLE,							//35 添加称号
	GM_USE_TITLE,							//36 使用称号
	GM_ADD_GOD_SOUL,						//37 添加神魂
	GM_SET_TOWER_RESET_TIMES,				//38 设置爬塔重置次数
	GM_SET_HERO_FB_CHAPTER,					//39 设置英雄副本章节
	GM_RESET_HERO_FB_BATTLE_TIMES,			//40 重置英雄副本挑战次数
	GM_SET_DAILY_FB_BATTLE_TIMES,			//41 重置日常副本挑战次数
	GM_PRINT_RED_PIONT_STATUS,				//42 打印红点状态
	GM_ELITE_FB_ENEMY_REFRESH,				//43 精英副本外敌刷新
	GM_ADD_SPIRIT,							//44 添加精灵
	GM_SET_FRIENDSHIP_POINT,				//45 设置友情点
	GM_SET_PASS_ELITE_MAP,					//46 设置通关精英关卡
	GM_RESET_GUILD_FB_BATTLE_TIMES,			//47 重置工会副本挑战次数
	GM_RESET_GUILD_QY_TIMES,				//48 重置泉涌次数
	GM_RESET_KICK_OUT,						//49 操自己
	GM_REFRESH_WEEK_GIFT,					//50 刷新周礼包
	GM_MONTH_CARD_BUY,						//51 月卡购买
	GM_RECHARGE,							//52 充值
	GM_RESET_ELITE_FB_BATTLE_TIMES,			//53 重置精英副本挑战次数
	GM_SET_ENDLESS_STONE,					//54 设置无尽石
	GM_RESET_ENDLESS_DATA,					//55 重置无尽之地数据
	GM_SUPER_DISCOUNT_RECHARGE,				//56 超值折扣充值
	GM_SIGNIN,								//57 签到
	GM_REMOVE_ROLE_RANK,					//58 删除角色排行榜
	GM_UPDATE_ROLE_RANK,					//59 重新排序角色排行榜
	GM_ADD_GEM,								//60 添加宝石
	GM_KILL_ATTACK_CITY_MONSTER,			//61 击杀怪物攻城当前所有怪物
	GM_SET_KING_MATCH_SEASON,				//62 设置王者赛当前赛季
	GM_SET_KING_MATCH_STAGE,				//63 设置王者赛段位
	GM_SET_GEM_CION,						//64 设置宝石币
	GM_PRINT_LAND_REWARD,					//65 打印领地奖励
	GM_ADD_CARD,							//66 添加卡牌
	GM_ADD_DESTINY_COIN,					//67 命运硬币
	GM_ADD_STAR_SOUL,						//68 星魂
	GM_ADD_COLLETION_VALUE,					//69 添加收藏值
	GM_ADD_CHRISTMAS_SCORE,					//70 添加圣诞积分
	GM_SET_HEARTOFWORLD,					//71 设置世界之心
	GM_NEW_SUPER_DISCOUNT_RECHARGE,			//72 超值折扣充值
	GM_ADD_OCCUPY_TIME,						//73 添加占领时间(GM使用)
	GM_RESET_TOWER,							//74 重置爬塔
	GM_BUY_RECHARGE,						//75 购买充值
	GM_ADD_YCOIN,							//76 添加ycoin
	GM_NEW_DAY,								//77 单个玩家直接重置一天
	GM_PRINT_HERO_PRO = 78,					//78 输出英雄属性
	GM_DELETE_ITEM,							//79 删除玩家道具
	GM_SET_PVP_SCORE,						//80 修改玩家pvp积分
	GM_NEW_WEEK,							//81 单个玩家直接重置一周
	GM_ACCUPY_STAR,							//82 占领星球
	GM_ADD_RECHARGE_AMOUNT,					//83 添加充值额度

	DBGM_START = 0,

};

// 系统奖励
struct SystemReward
{
	DWORD dwServerId;	// 服务器ID
	WORD wMinLevel;		// 等级下限
	WORD wMaxLevel;		// 等级上限
	BYTE byRewardType;	// 奖励类型

	RewardData tRewardData;	// 奖励数据
};

// 系统广播信息
struct SystemNoticeInfo
{
	WORD wServerId;	// 广播的服务器ID(0=表示所有服务器)
	WORD wNoticeId;	// 公告ID
	char szText[MAX_SYSTEM_NOTICE_LEN+1]; // 公告内容

	int nNoticeTimes;
};

// GM发邮件信息
struct GMSendEmailInfo
{
	DWORD dwServerId;
	char szTitle[MAX_EMAIL_TITLE_LEN]; // 标题
	char szText[MAX_EMAIL_TEXT]; // 内容	
};

//命令行管理器
class GMManager : public Singleton<GMManager>
{
public:
	GMManager();
	~GMManager();

public:
	BOOL		Init();
	VOID		Release();
	void		Update();

protected:
	// 处理发邮件
	void ProcessSendEmail();

	// 处理系统奖励
	void ProcessSystemReward();


public:
	void		OnNetGMMsg(Player *pPlayer, WORD wCmdId, DWORD dwValue = 0, DWORD dwValue1 = 0, DWORD dwValue2 = 0);

public:
	// 来自数据库的命令
	void		OnCmdFromDB(WORD wCmdId, const DWORD *pParamList, int nParamNum, const char *pszText = NULL, const char *pszText1 = NULL);

private:
	DWORD m_dwProceesTime;
	list<SystemReward> m_vSystemRewardList;	// 系统奖励列表

	DWORD m_dwProcessNoticeTime;
	DWORD m_dwSendNoticeInterval;	// 发公告的间隔时间(秒)
	list<SystemNoticeInfo> m_vSystemNoticeList;	// 系统广播列表

	DWORD m_dwProcessEmailTime;
	list<GMSendEmailInfo> m_vGMSendEmailList;	// GM发邮件列表

	DWORD m_dwCPOrderId;

	DWORD m_dwActivePlayerLimitTime;	// 活跃玩家限制时间
};