#pragma once

/*--------------------------------------------------------------------------------------------------------------------------*/
// 1)通用定义
/*--------------------------------------------------------------------------------------------------------------------------*/

#define INVALIDID8				0xff
#define INVALIDID16				0xffff
#define INVALIDID				(DWORD)-1

#define IP_LEN						32
#define BUFF_LEN					32
#define SECOND_OF_DAY				86400
#define SECOND_OF_MIN				60
#define MAX_DB_CONN					17
#define BUFF_LEN_16					16
#define BUFF_LEN_32					32
#define BUFF_LEN_64					64
#define BUFF_LEN_128				128
#define BUFF_LEN_256				256
#define BUFF_LEN_512				512
#define BUFF_LEN_1024				1024
											// 数据库扩展线程 DBTHREAD_COUNT

#define MAX_ACCOUNT_LEN				64		// 帐号长度上限
#define MAX_ACCOUNT_PASSWORD_LEN	64		// 帐号密码长度
#define MAX_ROLE_LEN				32
#define MAX_HERO_SKILL_NUM			10		// 英雄技能数量上限
#define MAX_HERO_BAG_NUM			700		// 角色英雄背包数量上限 
#define MAX_EQUIPMENT_NUM			400		// 角色装备数量上限
#define MAX_TREASURE_NUM			400		// 角色宝物数量上限
#define MAX_ITEM_NUM				500		// 角色物品数量上限
#define MAX_GEM_NUM					100		// 角色宝石数量上限
#define MAX_GEM_HOLE_NUM			8		// 角色宝石孔位数量上限
#define MAX_HERO_TRAIN_PROP_MUM		4		// 英雄培养属性数量上限
#define MAX_FUBEN_CHAPER_NUM		100		// 副本的章节上限
#define MAX_TOWER_CHAPTER_NUM		50		// 爬塔章节上限
#define MAX_CHAPTER_MAP_NUM			10		// 副本章节的关卡上限
#define MAX_DAILY_MAP_NUM			20		// 日常关卡数量上限
#define REFRESH_SHOP_GOODS_NUM		20		// 刷新商店商品数量
#define MAX_BATTLE_HERO_NUM			5		// 上阵战斗的英雄数量上限
#define MAX_REINFORCEMENT_NUM		8		// 援军列表上限	
#define MAX_EQUIPMENT_REFINE_COST_ITEM 5	// 装备精练消耗的物品种类上限 
#define MAX_TREASURE_ENHANCE_COST_ITEM 100	// 宝物强化消耗的物品种类上限 
#define MAX_RANK_MUM				200		// 排行榜数量上限
#define MAX_GUILD_RANK_MUM			20		// 工会排行榜数量上限
#define MAX_SPIRIT_NUM				50		// 精灵数量上限
#define MAX_GUILD_NAME_LEN			32		// 工会名称长度上限
#define MAX_GUILD_MEMBER_NUM		50		// 工会成员数量上限
#define MAX_GUILD_NOTICE_LEN		80		// 工会公告长度上限
#define MAX_GUILD_DECLARATION_LEN	80		// 工会宣言长度上限
#define MAX_GUILD_LEAVEMESSAGE_LEN	121		// 工会留言长度上限
#define MAX_GUILD_LEAVEMESSAGE_NUM	50		// 工会留言数量上限
#define MAX_GUILD_AWARD_SHOP_GOODS_NUM 20	// 工会奖励商店商品数量上限
#define	MAX_CHAT_TEXT_LEN		    128			//最大聊天文本长度
#define MAX_CHAT_VOICE_LEN			1024 * 15	// 语音内容长度
#define MAX_GOOGS_BUY_RECORD_NUM	100		// 商品购买记录数量上限	
#define MAX_BATTLE_MONSTER_NUM		5		// 上阵怪物数量上限
#define MAX_MAP_DROP_NUM			15		// 关卡掉落物品上限
#define MAX_HERO_NUM				200		// 英雄数量上限(种类)
#define MAX_LOGIN_SERVER_LIST		4		// 最近登录服务列表
#define MAX_EMAIL_COUNT				100		// 邮件数量上限
#define MAX_EMAIL_CONTEXT_NUM		5		// 邮件上下文列表数量上限
#define MAX_RANK_LEAVE_MESSAGE_NUM	10		// 排行榜留言列表数量上限
#define MAX_RANK_LEAVE_MESSAGE_LEN	64		// 排行榜留言长度上限
#define RAND_RANGE_MAX_10000		10000	// 随机范围上限为10000的宏
#define RAND_RANGE_MAX_100			100		// 随机范围上限为100的宏
#define MAX_FRIEND_NUM				100		// 好友数量上限
#define MAX_BLACK_NUM				50		// 黑名单数量上限
#define MAX_FRIEND_APPLY_NUM		50		// 好友申请列表上限
#define MAX_FRIEND_RECOMMEND_NUM	10		// 好友推荐列表上限
#define ROLE_BASE_DATA_LOAD_NUM		5000	// 角色基础数据加载数量(每次加载的数量)
#define MAX_REBEL_SHARE_NUM			10		// 叛军分享数量上限
#define MAX_EXPLOIT_AWARD_NUM		100		// 功勋奖励数量上限
#define MAX_SELL_GOODS_NUM			300		// 出售商品数量上限
#define MAX_RECYCLE_DECOMPOSE_NUM	5		// 分解回收数量上限
#define HERO_ONE_KEY_SUMMON_TIMES	10		// 英雄一键召唤次数
#define MAX_RECYCLE_RETURN_OBJ_NUM	50		// 返回返还对象列表上限
#define DUOBAO_TARGET_NUM			5		// 夺宝目标数量
#define MAX_LAND_CITY_NUM			5		// 玩家领地城池数量上限
#define MAX_LAND_PATROL_EVENT		100		// 领地巡逻事件上限
#define MAX_LAND_ROIT_EVENT			50		// 领地暴动事件上限
#define MAX_LAND_AWARD_NUM			20		// 领地奖励数量上限
#define MAX_ALL_GUILD_NUM			1000	// 所有服工会数量上限
#define MAX_GUILD_NUM				200		// 单服工会数量上限
#define MAX_BATTLE_PARAM_NUM		10		// 战斗参数数量上限
#define MAX_GUILD_APPLY_NUM			100		// 工会申请数量上限
#define MAX_GUILD_LEAVEMSG_NUM		100		// 工会留言数量上限
#define MAX_GUILD_TEXT_NUM			MAX_GUILD_NOTICE_LEN
#define MAX_ARENA_TARGET_NUM		50		// 竞技场挑战目标上限
#define MAX_ACHIEVEMENT_NUM			100		// 成就数量上限
#define MAX_SEVEN_DAY_TARGET_NUM	400		// 7天目标数据数量上限
#define MAX_DAILY_MISSION_NUM		100		// 日常任务数量上限
#define MAX_DAILY_MISSION_POINT_AWARD 10	// 日常任务积分奖励数量上限
#define MAX_ARENA_BATTLE_RECORD		10		// 竞技场挑战记录上限
#define MAX_GUILD_EVENT_NUM			50		// 工会事件上限
#define MAX_GUILD_QS_REWARD_RECORD  10		// 泉水奖励记录上限	
#define MAX_LIMIT_GOODS_NUM			50		// 限时商品数量上限
#define MAX_SWEEP_TIMES				10		// 扫荡次数上限
#define MAX_WORLD_BOSS_AWARD_NUM	100		// 世界BOSS奖励数量上限
#define MAX_WORLD_BOSS_BATTLE_RECORD_NUM	50		// 世界BOSS战报上限
#define MAX_GUILD_FB_CHAPTER_NUM	100		// 工会副本章节数量上限
#define MAX_DIRECT_BUY_GOODS_NUM	10		// 直购商品数量上限
#define MAX_BATTLE_PARAM_SIGN_LEN   64		// 战斗签名长度上限



#define VALID_NAME_LEN				24
#define MAX_TIME_LEN				32
#define INVALID_SLOT				-1
#define SCRIPT_NAME_LEN				64
#define SCRIPT_FUNC_LEN				32
#define MAX_RELATION_LEN            100
#define MAX_EMAIL_TITLE_LEN			64
#define MAX_EMAIL_SENDER_LEN		32
#define MAX_EMAIL_ATTACHMENT		5
#define MAX_EMAIL_TEXT              256		// 文本内容
//#define MAX_EMAIL_COUNT				200			// 邮件数量上限
#define MAX_EMAIL_SEND_NUM		    100
#define MAX_EMAIL_QUERY_LEN		    128

#define	MAX_CHAT_PARA_NUM		    11			//最大聊天参数个数
#define	MAX_RESTRICTIONS_NUM		10			//限购物品数量
#define	MAX_SERVER_RESTRICTIONS_NUM	100			//限购物品数量
#define MAX_GUILD_JOININ_LIST_NUM	20			//工会申请加入列表上限
#define MAX_ARENA_FIGHT_RECORD		10			// 竞技场挑战记录上限
#define MAX_ARENA_FIGHT_COUNT		100			// 竞技场同时挑战的数量上限
#define MAX_DAYMISSION_COUNT		7			// 日常任务数量上限
#define NEW_PLAYER_LEVEL			10			// 新手玩家等级上限
#define MAX_GUILD_COUNT				100			// 工会数量上限
#define MAX_CHAT_RECORD_COUNT		10			// 聊天记录上限
#define MAX_PHONE_NUMBER_COUNT		12			// 电话号码长度
#define MAX_LOGIN_CONTEXT_LEN		16			// 登录上下文长度
#define MAX_GOLD					1000000000	// 最大的金钱上限(10亿)	
#define MAX_YB						1000000000	// 最大元宝上限(10亿)
#define MAX_RECHARGE_GOODSID_LEN	32			// 充值商品ID长度
#define MAX_OFFLINE_MSG_NUM			20			// 离线留言数量上限
#define MAX_ROLE_LEVEL				100			// 角色的等级上限
#define MAX_VIP_LEVEL				15			// VIP等级上限
#define MAX_MOBILE_LEN				16			// 手机号长度上限
#define MAX_MONEY					1000000000	// 最大的货币上限(通用)
#define MIN_DATE					10000000	// 最小日期值 如: 20171019


#define MAX_EMAIL_LEN				32			// 邮件长度上限


#define MAX_WEB_COMMON_BUFF_LEN		4096		// 与web消息交互的通用BUFF大小上限

#define HTTP_RETURN_SUCCESS_CODE	200			// Http响应成功码
#define HTTP_URL_INVALID			1			// 无效的url
#define HTTP_TEXT_INVALID			2			// http响应内容出错
#define MAX_HTTP_URL_LEN			4096		// url请求长度

#define MAX_MSG_BUFF_LEN			10240		// 消息内容上限

#define	PROPERTY_INCREASE_MULTIPLE	10000		// 属性放大倍数
#define	PROPERTY_DECREASE_MULTIPLE	0.0001		// 属性降低倍数

#define MAX_REYUN_HTTP_URL_LEN		1024	// 热云url长度上限
#define MAX_REYUN_HTTP_CONTEXT_LEN	1024	// 热云http内容长度上限	
#define MAX_REYUN_HTTP_PATH_LEN		256		// 热云http path长度上限


enum
{
	WORLD_BOSS_RANK_TYPE_NUM = 2,			// 世界BOSS排行榜类型数量
	MAX_AWAKEN_EQUIP_ITEM_NUM = 5,			// 觉醒装备道具上限
	MAX_REWARD_CENTER_REWARD_NUM = 300,		// 奖励中心奖励数量上限
	MAX_GUILD_SKILL_NUM = 10,				// 工会技能数量上限
	MAX_MONEY_TYPE_NUM = 20,				// 货币类型数量上限
	MAX_CARD_NUM = 700,						// 卡牌数量上限
	MAX_POKEDEX_NUM = 200,					// 图鉴数量上限
	HERO_AWAKEN_LEVEL = 9,					// 英雄觉醒等级
	HERO_TRAIN_PRO_NUM = 4,					// 英雄培养属性数量
	MAX_ARENA_COUNT = 30000,				// 竞技场数量上限
	MAX_SERVER_NUM = 150,					// 服务器数量上限(todo::暂定100)
	MAX_GM_EVENT_NUM = 20,					// GM事件数量上限
	SCORE_MATCH_TARGET_NUM = 3,				// 积分赛挑战目标数量
	MAX_WARCRAFT_TARGET_NUM = 30,			// 争霸赛目标数量上限
	MAX_WARCRAFT_RANK_NUM = 200,			// 争霸赛排行榜数量上限
	MAX_TITAL_NUM = 30,						// 称号数量上限
	MAX_RED_POINT_NUM = 30,					// 红点数量上限
	MAX_SYSTEM_NOTICE_LEN = 200,			// 系统公告长度上限期
	MAX_GUIDE_NUM = 50,						// 引导数量上限
	MAX_BATTLE_REWARD_ITEM_NUM = 10,		// 战斗奖励物品数量上限
	MAX_SEVEN_DAY_COMPLETION_REWARD = 20,	// 7天目标完成度数量上限
	MAX_VIP_WEEK_GIFT_NUM = 32,				// VIP每周礼包数量
	MAX_LEVEL_SHOP_ACTIVITY_NUM = 20,		// 等级商店活动数据数量
	//MAX_LEVEL_GIFT_GOODS_BUY_RECORD = 200,	// 等级礼包商品购买记录数量上限	
	MAX_OPEN_SERVER_FUND_REWARD = 10,		// 开服基金奖励数量上限
	MAX_ALL_THE_PEOPLE_WELFARE_NUM = 20,	// 全民福利数量上限
	MAX_GOLBAL_DATA_NUM = 2000,				// 全局数据数量	
	ARENA_BATTLE_RECORD_LOAD_NUM = 200,		// 竞技场战斗记录加载数量(每次加载的数量) todo::暂时定为200个,为了测试
	MAX_SYSTEM_NOTICE_PARAM_LEN = MAX_ROLE_LEN * 2,	// 系统公告参数长度上限
	MAX_SYSTEM_NOTICE_PARAM_NUM = 4,			// 系统公告参数数量
	MAX_RECHARGE_NUM = 50,						// 充值数量上限
	MAX_MONTH_CAR_NUM = 5,						// 月卡数量上限
	MAX_LIMIT_PREFERENTIAL_WELFARE_NUM = 20,	// 限时优惠活动福利数量上限
	MAX_LAND_LIST_LOAD_NUM = 500,				// 领地数据列表加载数量上限 todo::暂时定为500个,为了测试
	MAX_LEVEL_GIFT_TAKE_RECORD_NUM = 100,		// 等级礼包领取记录上限
	MAX_LEAVE_MESSAGE_LENGTH = 200,				// 留言长度上限
	MAX_CONTACT_LENGTH = 64,					// 联系方式长度上限
	MAX_TQT_TAKE_RECORD_NUM = 3,				// 铜雀台领取记录数量上限
	MAX_SEARCH_GUILD_NUM = 10,					// 查找工会数量上限
	MAX_HELP_BATTLE_TARGET_NUM = 12,			// 助战目标上限
	MAX_SPIRIT_POKEDEX_NUM	= 10,				// 精灵图鉴数量上限
	GUILD_LIST_LOAD_NUM = 20,					// 工会列表加载数量(每次加载的数量) todo::暂时定为20个,为了测试
	MAX_ACTIVATE_CODE_LEN = 32,					// 激活码长度上限(todo::主要考虑支持中文)
	MAX_CHAT_RECORD_NUM = 10,					// 聊天记录上限
	MAX_ACTIVATION_RECORD_NUM = 50,				// 激活码记录数量上限
	MAX_LOG_CONTEXT_LEN = 256,					// 日志内容长度上限
	MAX_MAP_COMMENT_NUM = 30,					// 关卡评论数量上限
	MAX_MAP_COMMENT_LIKE_NUM = 100,				// 关卡评论点赞数量上限(todo::根据实际情况做调整)
	MAX_MAP_COMMENT_LOAD_NUM = 100,				// 关卡评论加载数量上限 todo::暂时定为100个,为了测试
	PVP_BATTLEFIELD_ROLE_NUM = 2,				// PVP战场角色数量
	MAX_PVP_BATTLE_REWARD_NUM = 10,				// PVP挑战奖励数量上限	
	MAX_DEFAULT_RANK_MUM = 20,					// 默认的排行榜数量上限
	PVP_ROBOT_ID = INVALIDID,					// PVP机器人ID
	ALL_SERVER_ID = 0,							// 全服ID
	MAX_LIMIT_ACTIVITY_REWARD_NUM = 20,			// 限时活动奖励数量上限
	MAX_ATTACK_CITY_PLAYER_NUM = 200,			// 攻城里的玩家数量上限
	MAX_ATTACK_CITY_MONSTER_NUM = 200,			// 攻城里的怪物数量上限 
	MAX_ATTACK_CITY_TREASURE_BOX_NUM = 100,		// 攻城里的宝箱数量上限 
	MAX_ATTACK_CITY_BOSS_REWARD_NUM = 10,		// 攻城boss奖励数量上限
	MAX_ENDLESS_LAND_CONTINUE_KILL_REWARD = 20,	// 无尽之地连杀奖励数量上限
	MAX_ROLE_TEMP_DATA_PARAM_NUM = 15,			// 角色临时数据参数数量上限
	MAX_ROLE_TEMP_DATA_TEXT_LEN = 256,			// 角色临时数据文本长度上限
	MAX_ROLE_TEMP_DATA_NUM = 50,				// 角色临时数据数量上限
	MAX_TOKEN_LEN = 512,						// token长度
	MAX_ACCOUNT_UID_LEN = 64,					// 帐号uid长度上限
	MAX_ORDER_ID_LEN = 32,						// 订单号长度上限
	MAX_ORDER_RECORD_NUM = 800,					// 订单记录数量上限
	MAX_ORDER_SIGN_LEN = 256,					// 订单签名长度上限
	MAX_GUILD_CHAPTER_NUM = 200,				// 工会章节数量上限
	MAX_SDK_ORDER_ID_LEN = 64,					// sdk订单ID长度上限
	MAX_LUXURY_SIGN_NUM = 10,					// 豪华签到数量上限
	MAX_ACTIVITY_TARGET_NUM = 100,				// 活动目标数量上限
	MAX_ACCURATE_RECOMMEND_GOODS_NUM = 50,		// 精准推荐商品数量上限
	MAX_TEAM_MEMBER_NUM	= 4,					// 队伍成员数量上限
	MAX_SCENE_PLAYER_NUM = 200,					// 场景玩家数量上限
	MAX_TASK_NUM = 50,							// 任务数量上限
	MAX_TEAM_DUNGEON_LOTTERY_TIMES = 10,		// 组队副本抽奖次数上限
	MAX_TEAM_DUNGEON_RECOMMEND_LIST_NUM = 50,	// 组队副本推荐列表数量上限
	MAX_LOGIN_REWARD_RECORD_NUM = 200,			// 登录奖励记录数量上限
	MAX_SEASON_TASK_REWARD_RECORD = 20,			// 王者赛季任务奖励记录上限
	MAX_KING_MATCH_TOP_RANK_NUM = 3,			// 王者赛高排行数量上限
	RECENTLY_KING_MATCH_SEASON_RANK_NUM = 3,	// 最近王者赛季排行记录数量		
	MAX_KING_MATCH_JOIN_NUM = 10000,			// 王者赛参与人数上限	
	MAX_TEAM_DUNGEON_NUM = 50,					// 组队副本数量上限
	MAX_TEAM_DUNGEON_RANK_NUM = 3,				// 组队副本排行数量	
	MAX_CARD_GROUP_NUM = 20,					// 卡组数量上限
	MAX_COLLECTION_GROUP_NUM = 150,				// 收藏组数量上限
	MAX_COLLECTION_BOOK_NUM = 20,				// 收藏书数量上限
	MAX_EQUIPMENT_REFINE_COST_NUM = 50,			// 装备精炼消耗数量上限
	MAX_CARD_SLOT_NUM = 8,						// 卡牌槽位数量上限
	MAX_FACEBOOK_SUMMON_REWARD_RECORD = 20,		// facebook召回奖励记录数量上限
	MAX_FACEBOOK_INVITE_REWARD_RECORD = 20,		// facebook邀请奖励记录数量上限
	MAX_FACEBOOK_ACCOUNT_LEN = 32,				// facebook帐号长度上限
	MAX_FACEBOOK_FRIEND_NUM = 100,				// facebook好友数量上限

	MAX_ONLINE_REWARD_DAY = 4,						// 在线奖励天数上限		
	MAX_ONLINE_REWARD_TIME_LIST = 4,				// 在线奖励时间列表上限	

	MAX_SEVEN_DAY_LOGIN_DAYS = 7,					// 7天登录天数上限

	MAX_RECHARGE_REWARD_DAY_NUM	 = 10,				// 充值奖励天数上限

	MAX_TOTAL_RECHARGE_REWARD_NUM = 20,				// 累充奖励数量上限

	MAX_ND_LOTTERY_TIMES = 10,		// 扭蛋抽奖次数上限

	MAX_HOT_SALE_GOODS_NUM = 10,	// 热卖商品数量上限

	MAX_DEVICE_ID_LEN = 64,		// 设备ID长度上限

	MAX_TURNTABLE_LOTTERY_TIMES = 10, // 转盘抽奖次数上限
	MAX_TURNTALE_LOTTERY_RECORD_NUM = 4, // 转抽奖记录数量上限

	MAX_CAMP_RECRUIT_SCORE_TASK_NUM = 30, // 阵营招募积分任务数量上限

	MAX_DOUBAO_PREGROSS_NUM = 20, // 夺宝进度数量上限

	MAX_DAILY_WORLD_LEVEL_TASK_NUM = 50,	// 每日世界等级任务数量上限

	MAX_SCORE_MATCH_JOIN_NUM = 10000,			// 积分赛参与人数上限
	MAX_SCORE_MATCH_TASK_NUM = 50,				// 积分赛任务数量
	MAX_SCORE_MATCH_BATTLE_TARGET_NUM = 3,		// 挑战目标数据上限

	MAX_RESOURCE_WAR_JOIN_NUM = 10000,			// 资源战参与人数上限
	MAX_RESOURCE_WAR_BATTLE_RECORD_NUM = 20,	// 资源战挑战记录上限
	MAX_RESOURCE_WAR_RESOURCE_NUM = 300,		// 资源战资源数量上限
	MAX_RESOURCE_WAR_ENEMY_NUM = 20000,			// 资源战敌人数量上限


	MAX_QUALITY_STAGE = 10,		// 品阶上限

	GUILD_WAR_OCCUPY_STAR_MAX_NUM = 10,	// 工会战占领星球数量上限

	MAX_YGAME_PAYMENT_ID_LEN = 64,		// ygame支付ID上限

	MAX_STAR_TOWER_NUM = 5,				// 星球塔数量上限
	MAX_STAR_TOWER_DEFENDER_NUM = 10,	// 星球塔防守者数量上限

	MAX_TOWER_MAP_NUM = 500,			// 爬塔关卡上限

	MAX_SUPER_DISCOUNT_ACCUMULATE_REWARD = 10,	// 超值折扣累充奖励次数上限

	MAX_HERO_AWAKEN_COST_HERO_NUM = 10,		// 英雄觉醒消耗同名英雄数量

	MAX_GAME_STATISTICS_PARAM_NUM = 200,	// 游戏统计参数列表数量上限

	MAX_GM_CMD_PARAM_NUM = 10,	// 

	MAX_HERO_BREAK_COST_HERO_NUM = 30,		// 英雄突破消耗的英雄数量上限

	MAX_GM_SHOP_GOODS_NUM = 200,			// GM商店商品数量上限

	MAX_ONLINE_DROP_REWARD_ITEM_TYPE = 50,		// 在线掉落奖励物品类型数量上限
};

//服务器类型
enum SERVER_TYPE
{
	MAP_SERVER						= 1,			//地图服务器
	GATE_SERVER						= 2,			//网关服务器
	OPEN_SERVER						= 3,			//Open服务器
	UNKNOWN_SERVER,									//未知服务器
};

// 功能模块的数据库线程编号
enum EModdleDBThreadID
{
	DB_THREAD_ARENA = 2,					// 竞技场
	DB_THREAD_LOAD_ROLE_INFO_LIST = 1,		// 角色基本信息列表
	DB_THREAD_FRIEND = 3,					// 好友
	DB_THREAD_GM	= 10,					// GM
	DB_THREAD_RANK	= 4,					// 排行榜
	DB_THREAD_GOLBAL_DATA = 5,				// 全局数据
	DB_THREAD_ARENA_BATTLE_RECORD = 6,		// 竞技场战斗记录
	DB_THREAD_ROLE_CURRENCY_LOGS = 7,		// 角色货币日志
	DB_THREAD_LAND = 8,						// 领地数据
	DB_THREAD_LEAVE_MESSAGE = 9,			// 留言
	DB_THREAD_CENTER_REWARD = 10,			// 奖励中心
	DB_THREAD_GUILD = 11,					// 工会
	DB_THREAD_COMMON = 12,					// 通用
	DB_THREAD_MAP_COMMENT = 15,				// 关卡评论
	DB_THREAD_LOG = 16,						// 日志
	DB_THREAD_RESOURCE_WAR = 13,			// 资源战


	DB_THREAD_KING_MATCH = DB_THREAD_LOAD_ROLE_INFO_LIST,	// 王者赛
	DB_THREAD_SCORE_MATCH = DB_THREAD_LOAD_ROLE_INFO_LIST,  // 积分赛
	
	// 目标线程最大是20

	DBTHREAD_ID_GUILD = MAX_DB_CONN,		// 工会
	DBTHREAD_ID_RANK,						// 排行榜
	DBTHREAD_COUNT_EX,
};


//用户在游戏中的状态
enum USER_STATE
{
	USER_INIT_STATE					= 0,		//用户初始状态
	//USER_CHOOSE_STATE				,			//用户选择角色状态
	USER_SELECT_SERVER_STATE		,			//选择服务器
	USER_SELECT_ROLE_LIST			,			//todo::查询角色列表	
	USER_READY_CREATE_ROLE_STATE	,			//准备创建角色
	USER_CREATE_ROLE_STATE			,			//创建角色
	USER_LOAD_STATE					,			//用户加载角色状态
	USER_AT_GAME_STATE				,			//用户在游戏中状态
	USER_LOGOUTING_STATE			,			//角色正在下线状态(一般都是等待异步DB写返回)
};

enum EOnlineType
{
	ONLINE_TYPE_ON = 1,               //上线
	ONLINE_TYPE_OFF,                  //下线
};

enum EPointRuleType
{
	POINTRULE_TYPE_REG = 0,
	POINTRULE_TYPE_PERFECT,
	POINTRULE_TYPE_ORDER,
	POINTRULE_TYPE_MAX,
};

// 登录类型
enum ELoginType
{
	LOGIN_TYPE_NORMAL= 0,				// 0.普通登录
	LOGIN_TYPE_RECONNECT,				// 1.重接登录
	LOGIN_TYPE_PHONE_RESISTER,			// 2.手机认证注册登录
	LOGIN_TYPE_PHONE_MODIFY_PASSWORD,	// 3.手机认证修改密码登录
	LOGIN_TYPE_INVATE_KEY,				// 4.邀请码注册登录
}; 


// 角色属性
enum PROPERTY
{
	ROLE_PRO_LEVEL = 1,					// 1.等级
	ROLE_PRO_EXP,						// 2.经验
	ROLE_PRO_GOLD,						// 3.金币
	ROLE_PRO_STAMINA,					// 4.体力
	ROLE_PRO_DIAMOND,					// 5.钻石
	ROLE_PRO_HERO_SOUL,					// 6.雄魂
	ROLE_PRO_HEAD_IMAGE,				// 7.头像			
	ROLE_PRO_ENERGY,					// 8.精力
	ROLE_PRO_LIKE_TIMES,				// 9.点赞次数
	ROLE_PRO_VIP_LEVEL,					// 10.vip等级	
	ROLE_PRO_REBEL_LEVEL,				// 11.叛军等级
	ROLE_PRO_TOWER_PRESTIGE,			// 12.爬塔声望(威名)
	ROLE_PRO_ARENA_PRESTIGE,			// 13.竞技场声望
	ROLE_PRO_REBEL_VALUE,				// 14.叛军值(战功)
	ROLE_PRO_WANTED,					// 15.通缉令
	ROLE_PRO_LAND_RIOT_SUPPRESS_TIMES,	// 16.领地暴动镇压次数
	ROLE_PRO_GOD_SOUL,					// 17.神魂
	ROLE_PRO_GUILD_ID,					// 18.工会ID
	ROLE_PRO_DAILY_INIT_LEVEL,			// 19.每日初始等级
	ROLE_PRO_GUILD_CONSTIBUTION,		// 20.当前贡献值
	ROLE_PRO_VIP_EXP,					// 21.vip经验
	ROLE_PRO_HONOR,						// 22.荣誉
	ROLE_PRO_FIGHTING_POWER,			// 23.战力
	ROLE_PRO_FAIRY_SOUL,				// 24.仙魂(todo::已经不使用)
	ROLE_PRO_SCORE_MATCH_HONOR,			// 25.积分赛荣誉
	ROLE_PRO_FRIENDSHIP_PIONT,			// 26.友情点
	ROLE_PRO_TODAY_FRIENDSHIP_PIONT,	// 27.今日获得的友情点
	ROLE_PRO_ENDLESS_STONE,				// 28.无尽石
	ROLE_PRO_NOBILITY_LEVEL,			// 29.爵位等级
	ROLE_PRO_GEM_COIN,					// 30.宝石币
	ROLE_PRO_DESTINY_COIN,				// 31.命运硬币
	ROLE_PRO_STAR_SOUL,					// 32.星魂
	ROLE_PRO_CRYSTAL,					// 33.水晶
	ROLE_PRO_RESOURCE_WAR_SCORE,		// 34.资源战积分
	ROLE_PRO_YCOIN,						// 35.YCoin
	ROLE_PRO_RECHARGE_AMOUNT,			// 36.总的充额度
	ROLE_PRO_CHAT_TIMES,				// 37.聊天次数


	// 不需要同步客户端滴
	ROLE_PRO_NOT_NOTIFY_BEGIN = 100,
	ROLE_PRO_GUILD_FB_REWARD_CHAPTER,	// 已经领取的军团副本奖励章节(todo::暂时不使用)
	ROLE_PRO_WARCRAFT_BATTLE_TIMES,		// 争霸赛挑战次数
	ROLE_PRO_BUY_WARCRAFT_BATTLE_TIMES,	// 购买的争霸赛挑战次数
	ROLE_PRO_WARCRAFT_CHEER_TIMES,		// 争霸赛助威次数
	ROLE_PRO_WARCRAFT_CHEER_TIMES_RECOVER_TIME,	// 争霸赛助威次数恢复时间
	ROLE_PRO_DUOBAO_PEACE_OVER_TIME,	// 夺宝免战结束时间
	ROLE_PRO_LEAVE_GUILD_TIME,			// 离开公会时间
	ROLE_PRO_LAST_ACTIVITY_NOTICE_ID,	// 上次活动公告的ID
	ROLE_PRO_FORBID_TO_TALK_OVER_TIME,	// 禁言结束时间
	ROLE_PRO_TODAY_HELP_BATTLE_TIMES,	// 今日助战次数
};

// 英雄属性类型
enum HeroProperty
{
	HERO_PRO_ATTACK = 1,					// 1 = 攻击				attack
	HERO_PRO_PHY_DEF,						// 2 = 物防				phy_def
	HERO_PRO_MAG_DEF,						// 3 = 法防				mag_def
	HERO_PRO_BLOOD,							// 4 = 生命				blood
	HERO_PRO_DAMAGE_DECREASE_RATE,			// 5 = 百分比减伤 %		damage_decrease_rate
	HERO_PRO_DAMAGE_DECREASE_VALUE,			// 6 = 固定值减伤		damage_decrease_value 
	HERO_PRO_DAMAGE_INCREASE_RATE,			// 7 = 伤害加成 %		damage_increase_rate 
	HERO_PRO_DAMAGE_INCREASE_VALUE,			// 8 = 伤害附加			damage_increase_value
	HERO_PRO_PENETRATION_RATE,				// 9 = 百分比穿透 %		penetration_rate
	HERO_PRO_PENETRATION_VALUE,				// 10 = 固定值穿透      penetration_value

	HERO_PRO_HIT_RATE,						// 11 = 命中率 %		hit_rate
	HERO_PRO_DODGE_RATE,					// 12 = 闪避率 %		dodge_rate
	
	HERO_PRO_CRIT_RATE,						// 13 = 暴击率 %		crit_rate
	HERO_PRO_CRIT_DEFENCE_RATE,				// 14 = 抗暴率 %		crit_defence_rate
	HERO_PRO_CRIT_RATE_CORRECT,				// 15 = 暴击强度 %		crit_rate_correct
	HERO_PRO_CRIT_DEFENCE_RATE_CORRECT,		// 16 = 抗暴击强度 %	crit_defence_rate_correct
	
	HERO_PRO_BLOCK_RATE,					// 17 = 格挡率 %		block_rate
	HERO_PRO_BLOCK_RATE_CORRECT,			// 18 = 格挡强度 %		block_rate_correct
	HERO_PRO_BEAT_BACK_CORRECT,				// 19 = 击退强度 %		beat_back_correct
	HERO_PRO_BEAT_BACK_DEFENCE_CORRECT,		// 20 = 抗击退强度%		beat_back_defence_correct
	
	HERO_PRO_ANGER,							// 21 = 怒气
	
	HERO_PRO_ATTACK_RATE,					// 22 = 攻击加成				
	HERO_PRO_PHY_DEF_RATE,					// 23 = 物防加成				
	HERO_PRO_MAG_DEF_RATE,					// 24 = 法防加成				
	HERO_PRO_BLOOD_RATE,					// 25 = 血量加成		
	
	HERO_PRO_SPEED,							// 26 = 速度

	HERO_PRO_ATTACK_INC_RATE,				// 27 = 攻击最终加成%	attack_inc_rate
	HERO_PRO_PHY_DEF_INC_RATE,				// 28 = 物防最终加成%	phy_def_inc_rate
	HERO_PRO_MAG_DEF_INC_RATE,				// 29 = 法防最终加成%	mag_def_inc_rate
	HERO_PRO_BLOOD_INC_RATE,				// 30 = 血量最终加成%	blood_inc_rate
	
	HERO_PRO_MAX,
};

// 邮件附件类型
enum EEmailAttachmentType
{
	EMAIL_ATTACHMENT_NORMAL 				= 0,	// 普通(无附件)
	EMAIL_ATTACHMENT_EXP					= 1,	// 经验
	EMAIL_ATTACHMENT_GOLD					= 2,	// 金币
	EMAIL_ATTACHMENT_MAX,
};


//查询余额类型
enum QueryBalanceType
{
	QBT_INIT		= 0,			//初始化时查询
	QBT_RECHARGE	= 1,			//充值后查询
	QBT_DELIVERY	= 2,			//发货后查询
};

// 邮件配置信息ID
enum EMailInfoId
{
	EMAIL_NEW_ROLE_WELCOME_ID		= 1,		// 新角色欢迎信
	EMAIL_CARD_EVRYDAY_REWARD1,					// 令牌卡每日奖励
	EMAIL_CARD_EVRYDAY_REWARD2,					// 令牌卡每日奖励
	EMAIL_CARD_EVRYDAY_REWARD3,					// 令牌卡每日奖励
	EMAIL_WORLDBOSS_RANK_REWARD,				// 世界BOSS攻击排名奖励
	EMAIL_WORLDBOSS_HURT_REWARD,				// 世界BOSS伤害奖励
	EMAIL_NEW_ROLE_GIFT,						// 新手礼包	
};

// 装备类型
enum EquipentType
{
	EQUIPMENT_TYPE_WEAPON = 0,	// 武器
	EQUIPMENT_TYPE_HAT,			// 帽子
	EQUIPMENT_TYPE_BELT,		// 腰带
	EQUIPMENT_TYPE_CLOTHES,		// 衣服
	EQUIPMENT_TYPE_MAX,
};

// 宝物类型
enum TreasureType
{
	TREASURE_TYPE_ATTACK = 0,	// 攻击型
	TREASURE_TYPE_DEFENCE = 1,	// 防御型
	TREASURE_TYPE_MAX,
};

// 职业类型
enum ProfessionType
{
	PROFESSION_1 = 1,	// 近战物攻型
	PROFESSION_2,		// 近战防御型
	PROFESSION_3,		// 远程物攻型
	PROFESSION_4,		// 远程法术型
	PROFESSION_5,		// 辅助型
};


// 关卡类型
enum MapType
{
	BATTLE_TYPE_INVALID = 0,		// 无效类型
	BATTLE_MAIN_MAP,				// 主线关卡
	BATTLE_DAILY_MAP,				// 日常关卡
	BATTLE_HERO_MAP,				// 英雄关卡 
	BATTLE_ELITE_MAP,				// 精英关卡
	BATTLE_TOWER_NORMAL_MAP,		// 普通爬塔关卡
	BATTLE_TOWER_ELITE_MAP,			// 精英爬塔关卡
	BATTLE_REBEL_MAP,				// 叛军
	BATTLE_LAND_MAP,				// 领地
	BATTLE_DOUBAO_MAP,				// 夺宝
	BATTLE_ARENA_MAP,				// 竞技场
	BATTLE_WORLD_BOSS_MAP,			// 世界BOSS
	BATTLE_FAIRY_MAP,				// 仙灵关卡
	BATTLE_SCORE_MATCH,				// 积分赛
	BATTLE_GUILD_MAP,				// 工会关卡
	BATTLE_ATTACK_CITY,				// 攻城
	BATTLE_ENDLESS_LAND,			// 无尽之地
	BATTLE_TEAM_DUNGEON_MONSTER,	// 组队副本怪物
	BATTLE_TEAM_DUNGEON_BOSS,		// 组队副本BOSS
	BATTLE_KING_MATCH = 19,			// 王者赛
	BATTLE_CARD_GROUP_MAP = 20,		// 卡组关卡
	BATTLE_RESOURCE_WAR = 21,		// 资源战
	BATTLE_GUILD_WAR_LOW_STAR = 22,	// 工会战低级星球
	BATTLE_GUILD_WAR_HIGH_STAR = 23,// 工会战高级星球

	BATTLE_GUIDE_MAP = 100,			// 新手引导关卡
};

// 副本章节星数奖励
enum ChapterStarAward
{
	CHAPTER_STAR_10_AWARD = 0,	// 10星奖励
	CHAPTER_STAR_20_AWARD,		// 20星奖励
	CHAPTER_STAR_30_AWARD,		// 30星奖励
	CHAPTER_STAR_AWARD_MAX,		// 星数奖励的上限
};

// 对象类型
enum OBJECT_TYPE
{
	OBJECT_NONE,
	OBJECT_ITEM,			// 1.物品
	OBJECT_EQUIPMENT,		// 2.装备
	OBJECT_HERO,			// 3.英雄
	OBJECT_TREASURE,		// 4.宝物
	OBJECT_SPIRIT,			// 5.精灵
	OBJECT_AWAKENING_ITEM,	// 6.觉醒道具
	OBJECT_CARD,			// 7.卡牌
	OBJECT_GEM,				// 8.宝石


	OBJECT_PLAYER,			// 玩家(不参与配置)
	OBJECT_MONSTER,			// 5.怪物
	OBJECT_MAX,	
};

// 全局数据类型
enum EGolbalDataType
{
	GOLBAL_DATA_WORLD_BOSS_LEVEL = 1,				// 世界BOSS等级(数据存放在对应的模块)
	GOLBAL_DATA_FUND_BUY_TIMES = 2,					// 基金购买次数
	GOLBAL_DATA_CHAPTER_PASS_RECORD = 3,			// 主线副本章节通关记录
	GOLBAL_LIMIT_PREFERENTIAL_GOODS_BUY_TIMES = 4,	// 限时优惠商品购买次数
	GOLBAL_FIGHTING_POWER_RECORD_IDX = 5,			// 战力记录索引 
	GOLBAL_ATTCK_CITY_CHAPTER = 6,					// 怪物攻城准备要挑战的章节
	GOLBAL_ATTCK_CITY_RANK_NO1_ROLE_ID = 7,			// 怪物攻城上次活动排名第一的玩家
	GOLBAL_CHRISTMAS_LOWER_BOSS_LEVEL = 8,			// 圣诞节低级boss等级
	GOLBAL_CHRISTMAS_HIGH_BOSS_LEVEL = 9,			// 圣诞节高级boss等级
	GOLBAL_CHRISTMAS_RESET_TIME = 10,				// 圣诞节数据重置时间(这个实际是全服都是一样，只是为了统一每个服单独一个)
};

// 货币操作来源
// enum ECurrencyFromID
// {
// 	FROM_FUND = 1,						// 基金
// 	FROM_ARENA_BATTLE,					// 竞技场挑战
// 	FROM_TOWER_BATTLE_TIMES_BUY,		// 爬塔挑战购买次数 
// 	FROM_TOWER_RESET,					// 爬塔重置副本 
// 	FROM_TOWER_BUY_STAR_GOODS,			// 爬塔购买星数商品
// 	FROM_GUILD_CREATE,					// 创建工会
// 	FROM_HERO_HIGH_SUMMON,				// 英雄高级召唤
// 	FROM_LAND_LEVEL_UP,				// 领地升级
// 	FROM_MAIN_FB_RESET,				// 主线副本重置
// 	FROM_ELITE_FB_RESET,				// 精英副本重置
// 	FROM_LAND_SUPPRESS_RIOT,			// 领地暴动镇压
// 	FROM_WARCRAFT_BATTLE_TIMES_BUY,	// 争霸赛挑战次数购买
// 	FROM_HERO_RESET,					// 英雄重生
// 	FROM_EQUIPMENT_RESET,				// 装备重生
// 	FROM_TREASURE_RESET,				// 宝物重生
// 	FROM_HERO_DECOMPOSE,				// 英雄分解	
// 	FROM_EQUIPMENT_DECOMPOSE,			// 装备分解
// 	FROM_SCORE_MATCH_REFRESH_TARGET,	// 积分赛刷新目标
// 	FROM_SCORE_MATCH_BATTLE_TIMES_BUY,	// 积分赛挑战次数购买
// 	FROM_MALL_ITEM_BUY,				// 商城道具购买
// 	FROM_TREASURE_SMELT,				// 宝物熔炼
// 	FROM_TREASURE_ENHANCE,				// 宝物强化
// 	FROM_TREASURE_REFINE,				// 宝物精炼
// 	FROM_WORLD_BOSS_BATTLE_TIMES_BUY,	// 世界BOSS挑战次数购买
// 	FROM_YCS,							// 摇钱树奖励
// 	FROM_TQT,							// 铜雀台奖励
// 	FROM_DOUBAO,						// 夺宝挑战
// 	FROM_EQUIP_ENHANCE,				// 装备强化
// 	FROM_FAIRY_BREAK,					// 仙灵突破	
// 	FROM_HERO_FB_BATTLE,				// 英雄副本挑战
// 	FROM_DAILY_FB_BATTLE,				// 日常副本挑战
// 	FROM_TOWER_FB_BATTLE,				// 爬塔副本挑战
// 	FROM_HERO_LEVEL_UP,				// 英雄升级
// 	FROM_HERO_BREAK,					// 英雄突破
// 	FROM_HERO_TRAIN,					// 英雄培养
// 	FROM_HERO_AWAKEN,					// 英雄觉醒
// 	FROM_ITEM_DECOMPOSE,				// 物品分解
// 	FROM_USE_ITEM,						// 使用物品
// 	FROM_NEW_ROLE,						// 新建角色
// 	FROM_REBEL_REWARD,					// 叛军奖励	
// 	FROM_REWARD_CENTER,				// 领取中心
// 	FROM_AWAKEN_SHOP_REFRESH,			// 觉醒商店刷新
// 	FROM_HERO_SHOP_REFRESH,			// 英雄商店刷新
// 	FROM_MAIN_FB_BATTLE,				// 主线副本挑战
// 	FROM_ELITE_FB_BATTLE,				// 精英副本挑战
// 	FROM_ACHIEVEMENT_AWARD,			// 领取成就奖励
// 	FROM_RECOMMEND_GIFT,				// 领取准推荐礼包	
// 	FROM_SIGN_IN,						// 签到
// 	FROM_LUXURY_SIGN_REWARD,			// 领取豪华签到奖励 
// 	FROM_DAILY_GIFT_TAKE,				// 每日礼包领取 
// 	FROM_WEEK_GIFT_BUY,				// 每周礼包购买 
// 	FROM_TODAY_RECOMMEND_REWARD,		// 领取今日推荐充值的奖励
// 	FROM_LEVEL_GIFT,					// 等级礼包领取
// 	FROM_ALL_PEOPLE_WELFARE,			// 全民福利领取
// 	FROM_LIMIT_PREFERENTIAL_WELFARE,	// 领取限时优惠福利 
// 	FROM_NEXT_DAY_REWARD,				// 领取次日奖励
// 	FROM_BUY_FUND,						// 购买开服基金
// 	FROM_CAMP_RECRUIT,					// 阵营招募
// 	FROM_FESTIVAL_DIAMOND_EXCHANGE,	// 节日活动钻石兑换
// 	FROM_ATTACK_CITY_RELIVE,			// 怪物攻城复活请求
// 	FROM_ENDLESS_KEEP_KILL_REWARD,		// 领取无尽之地连杀奖励
// 	FROM_BUY_GUILD_COPY_BATTLE_TIMES,	// 购买副本挑战次数
// 	FROM_GUILD_IMPEACH,				// 弹劾
// 	FROM_LAND_PATROL,					// 开始领地巡逻
// 	FROM_TOP_UP,						// 充值奖励
// 	FROM_GET_MONTH_CARD_REWARD,		// 领取月卡奖励
// 	FROM_CARD_RESET,					// 卡牌重生
// 	FROM_TEAM_DUNGEON_ND_LOTTERY,		// 组队副本扭蛋抽奖	
// 	FROM_TURNTABLE_LOTTERY,			// 转盘抽奖
// 	FROM_CARD_GROUP_REFRESH,			// 卡组刷新
// 	FROM_ATTACK_CITY_KILL_REWARD,		// 怪物攻城击杀奖励	
// 	FROM_ATTACK_CITY_ENCOURAGE,		// 怪物攻城鼓舞
// 	FROM_ENDLESS_ENTER_BATTLE,			// 无尽之地进入战场
// 	FROM_ENDLESS_LAND_BATTLE_OVER,		// 无尽之地战斗结束
// 	FROM_SHOP_LEVEL_GIFT,				// 等级礼包商店购买
// 	FROM_SHOP_SCORE,					// 积分赛商品购买
// 	FROM_HERO_EVOLUTION,				// 英雄进化
// 	FROM_TREASURE_QUALITY_UP,			// 宝物升品
// 	FROM_GUILD_QY,						// 工会建设
// 	FROM_GUILD_SKILL_LV_UP,			// 工会技术升级
// 	FROM_SHOP_SEVEN_DAY,				// 7天商店购买
// 	FROM_SHOP_CRYSTAL,					// 水晶商店购买	
// 	FROM_SHOP_GUILD,					// 工会商品购买
// 	FROM_SHOP_HERO,					// 英雄商品购买
// 	FROM_SHOP_REBEL,					// 叛军商品购买
// 	FROM_SHOP_TOWER,					// 爬塔商品购买
// 	FROM_SHOP_ARENA,					// 竞技场商品购买
// 	FROM_SHOP_AWAKEN,					// 觉醒商品购买	
// 	FROM_SHOP_STAR_SOUL,				// 星魂商品购买
// 	FROM_SHOP_ENDLESS_LAND,			// 无尽之地商店
// 	FROM_SHOP_DIRECT_BUY,				// 直购商店购买
// 	FROM_SHOP_EXCHANGE,				// 兑换商店购买
// 	FROM_SHOP_RECOMMEND,				// 推荐商店购买
// 	FROM_SHOP_TEAM_NORMAL,				// 组队副本普通商店购买
// 	FROM_SHOP_TEAM_REWARD,				// 组队副本奖励商店购买
// 	FROM_SHOP_DESTINY,					// 命运商店商品购买
// 	FROM_SHOP_HIDE,					// 隐藏商店
// 	FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE,	// 圣诞节物品兑换商品购买
// 	FROM_EQUIPMENT_STAR_UP,			// 装备升星	
// 	FROM_EQUIPMENT_QUALITY_UP,			// 装备升品
// 	FROM_ND_LOTTERY_REWARD,			// 扭蛋抽奖奖励	
// 	FROM_RESOURCE_WAR_BATTLE,			// 资源战挑战
// 	FROM_SHOP_RESOURCE_WAR,			// 资源战商品购买
// 	FROM_BUY_GUILD_WAR_BATTLE_TIMES,	// 购买资源战挑战次数
// 	FROM_BUY_RECHARGE,					// 购买充值档位
// 	FROM_VIP_SUMMON,					// 英雄VIP召唤
// 	FROM_SHOP_GM,						// GM商品购买
// };

// 数据来源
enum 
{
	FROM_UNKNOW,
	FROM_TOWER_BUY_STAR_GOODS,					// 爬塔购买星数商品
	FROM_NEXT_DAY_REWARD,						// 领取次日奖励
	FROM_SIGN_IN,								// 签到
	FROM_RECOMMEND_GIFT,						// 领取准推荐礼包	
	FROM_ACHIEVEMENT_AWARD,						// 领取成就奖励
	FROM_GET_LOGIN_REWARD,						// 领取登录奖励
	FROM_GET_ONLINE_REWARD,						// 领取在线奖励
	FROM_CAMP_RECRUIT,							// 阵营招募	
	FROM_GET_CAMP_RECRUIT_SCORE_TASK_REWARD,	// 领取阵营招募积分任务奖励
	FROM_ATTACK_CHRISTMAS_BOSS,					// 攻击圣诞Boss 
	FROM_GET_CHRISTMAS_BOSS_REWARD,				// 领取圣诞Boss奖励	
	FROM_GET_BIND_FB_ACCOUNT_REWARD,			// 领取帐号绑定奖励
	FROM_GET_SHARE_REWARD,						// 领取分享奖励
	FROM_GET_COMMAND_REWARD,					// 领取评论奖励	
	FROM_GET_FRIEND_INVITE_REWARD,				// 领取好友邀请奖励
	FROM_GET_FRIEND_SUMMON_REWARD,				// 领取好友召唤奖励
	FROM_GET_FESTIVAL_ACTIVITY_REWARD,			// 领取节日活动奖励
	FROM_GET_FESTIVAL_DIAMOND_EXCHANGE,			// 节日活动钻石兑换
	FROM_GET_FESTIVAL_ITEM_EXCHANGE,			// 节日活动物品兑换
	FROM_GET_HOT_SALE_RECHARGE_REWARD,			// 领取热卖商品充值奖励
	FROM_HOT_SALE_BUY_REWARD,					// 领取热卖商品购买奖励
	FROM_GET_LIMIT_FIRST_RECHARGE_REWARD,		// 领取限时活动首充奖励
	FROM_GET_RECHARGE_REWARD,					// 领取充值回馈奖励
	FROM_GET_RECHARGE_EXTRA_REWARD,				// 领取充值回馈额外奖励
	FROM_GET_SEVEN_DAY_LOGIN_REWARD,			// 领取7天登录奖励
	FROM_GET_SUPER_DISCOUNT_BUY_REWARD,			// 超值折扣购买奖励
	FROM_GET_SUPER_DISCOUNT_ACCUMULATE_REWARD,	// 超值折扣累积奖励
	FROM_GET_NEW_SUPER_DISCOUNT_BUY_REWARD,		// 新超值折扣购买奖励
	FROM_GET_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD,	// 新超值折扣累积奖励
	FROM_GET_TOTAL_RECHARGE_REWARD,					// 领取累充奖励
	FROM_SHOP_SCORE_FINISH_REWARD,				// 商城评分完成奖励
	FROM_WATCHING_VIDEO_REWARD,					// 观看视频奖励
	FROM_ATTACK_CITY_KILL_REWARD,				// 怪物攻城击杀奖励	
	FROM_ATTACK_CITY_BOSS_REWARD,				// 领取怪物攻城Boss奖励
	FROM_DAILY_MISSION_REWARD,					// 领取每日任务奖励
	FROM_DAILY_MISSION_SCORE_REWARD,			// 领取每日任务积分奖励
	FROM_FIVE_TIMES_DOUBAO,						// 5次夺宝
	FROM_DOUBAO_PREGROSS_REWARD,				// 夺宝进度奖励
	FROM_ENDLESS_ENTER_BATTLE,					// 无尽之地进入战场
	FROM_ENDLESS_LAND_BATTLE_OVER,				// 无尽之地战斗结束
	FROM_ENDLESS_LAND_COLLISION,				// 无尽之地挂机碰撞
	FROM_ENDLESS_LAND_KILL_MONSTER,				// 无尽之地杀怪奖励
	FROM_GET_MAINLINE_FB_LEVEL_REWARD,			// 领取主线副本关卡奖励
	FROM_GET_MAINLINE_FB_STAR_REWARD,			// 领取主线副本星数奖励
	FROM_GET_MAINLINE_FB_ONE_KEY_REWARD,		// 主线副本一键领取奖励
	FROM_MAINLINE_FB_CHAPTER_REWARD,			// 领取主线副本章节奖励
	FROM_ELITE_FB_LEVEL_REWARD,					// 领取精英副本关卡奖励
	FROM_ELITE_FB_STAR_REWARD,					// 领取精英副本星数奖励
	FROM_ELITE_FB_ONE_KEY_REWARD,				// 精英副本一键领取奖励
	FROM_HERO_FB_REWARD,						// 领取英雄副本奖励
	FROM_DAILY_FB_BATTLE_REWARD,				// 日常副本挑战奖励
	FROM_TOWER_NORMAL_FB_BATTLE_REWARD,			// 爬塔普通副本挑战奖励
	FROM_TOWER_ELITE_FB_BATTLE_REWARD,			// 爬塔精英副本挑战奖励	
	FROM_FB_FIRST_WIN_REWARD,					// 副本限时奖励
	FROM_FB_LIMIT_REWARD,						// 副本限时奖励
	FROM_GUILD_FB_CHAPTER_REWARD,				// 领取工会副本章节奖励
	FROM_GUILD_QS_REWARD,						// 领取工会泉水奖励
	FROM_FP_REWARD,								// 翻牌奖励
	FROM_KING_MATCH_SEASON_TASK_REWARD,			// 领取王者赛季任务奖励
	FROM_KING_MATCH_BATTLE_REWARD,				// 王者赛挑战奖励
	FROM_ND_LOTTERY_REWARD,						// 扭蛋抽奖奖励	
	FROM_ROLE_LEVEL_UP_REWARD,					// 角色升级奖励	
	FROM_NEW_ROLE_REWARD,						// 新角色奖励	
	FROM_PVP_BATTLE_REWARD,						// 领取PVP挑战奖励
	FROM_HERO_DECOMPOSE,						// 英雄分解	
	FROM_HERO_RESET,							// 英雄重生
	FROM_EQUIPMENT_DECOMPOSE,					// 装备分解
	FROM_EQUIPMENT_RESET,						// 装备重生
	FROM_TREASURE_RESET,						// 宝物重生
	FROM_CARD_DECOMPOSE,						// 卡牌分解
	FROM_CARD_RESET,							// 卡牌重生
	FROM_ITEM_COMPOSE,							// 物品合成	
	FROM_HERO_COMPOSE,							// 物品合成	
	FROM_EQUIPMENT_COMPOSE,						// 装备合成	
	FROM_TREASURE_COMPOSE,						// 宝物合成	
	FROM_REWARD_CENTER,							// 领取奖励中心奖励
	FROM_SEVEN_DAY_TARGET_REWARD,				// 领取7天目标奖励
	FROM_SEVEN_DAY_COMPLETIO_REWARD,			// 领取7天完成度奖励 
	FROM_TEAM_DUNGEON_ND_LOTTERY,				// 组队副本扭蛋抽奖
	FROM_TEAM_DUNGEON_FINISH_STAGE_TASK,		// 组队副本阶段任务完成奖励
	FROM_TEAM_DUNGEON_BOSS_REWARD,				// BOSS奖励
	FROM_WORLD_TASK_FINAL_REWARD,				// 领取世界任务最终奖励	
	FROM_FB_NORMAL_REWARD,						// 副本普通奖励物品
	FROM_GUILD_FB_LEVEL_REWARD,					// 领取工会副本关卡奖励	
	FROM_RECHARGE_MONTH_CARD,					// 月卡充值	
	FROM_WORLD_BOSS_REWARD,						// 领取世界BOSS奖励	
	FROM_WORLD_BOSS_BATTLE_REWARD,				// 世界BOSS挑战奖励	
	FROM_WORLD_LEVEL_TASK_REWARD,				// 领取世界等级任务奖励
	FROM_FB_DROP_ITEM,							// 副本掉落物品
	FROM_LAND_PATROL_AWARD,						// 领取领地巡逻奖励
	FROM_ACTIVATION_CODE_RECORD,				// 领取激活码奖励	 
	FROM_REBEL_REWARD,							// 领取叛军功勋奖励	
	FROM_WORLD_HEART_UPGRADE,					// 世界之心升级
	FROM_TREASURE_SMELT,						// 宝物熔炼
	FROM_TURNTABLE_LOTTERY,						// 转盘抽奖
	FROM_SHOP_SCORE,							// 积分赛商品购买
	FROM_MALL_ITEM_BUY,							// 商城道具购买
	FROM_DIRECT_BUY_GOODS,						// 领取直购商品
	FROM_SHOP_HERO,								// 英雄商品购买
	FROM_SHOP_REBEL,							// 叛军商品购买
	FROM_SHOP_TOWER,							// 爬塔商品购买
	FROM_SHOP_ARENA,							// 竞技场商品购买
	FROM_SHOP_AWAKEN,							// 觉醒商品购买	
	FROM_SHOP_STAR_SOUL,						// 星魂商品购买
	FROM_SHOP_SEVEN_DAY,						// 7天商店购买
	FROM_SHOP_LEVEL_GIFT,						// 等级礼包商店购买
	FROM_SHOP_ENDLESS_LAND,						// 无尽之地商店
	FROM_SHOP_DIRECT_BUY,						// 直购商店购买
	FROM_SHOP_EXCHANGE,							// 兑换商店购买
	FROM_SHOP_RECOMMEND,						// 推荐商店购买
	FROM_SHOP_TEAM_NORMAL,						// 组队副本普通商店购买
	FROM_SHOP_TEAM_REWARD,						// 组队副本奖励商店购买
	FROM_SHOP_DESTINY,							// 命运商店商品购买	
	FROM_SHOP_HIDE,								// 隐藏商店
	FROM_SHOP_CHRISTMAS_SCORE_EXCHANGE,			// 圣诞节积分兑换商品购买
	FROM_SHOP_CHRISTMAS_ITEM_EXCHANGE,			// 圣诞节物品兑换商品购买
	FROM_SHOP_CRYSTAL,							// 水晶商店购买	
	FROM_SHOP_GUILD,							// 工会商品购买	
	FROM_HERO_SUMMON,							// 英雄召唤
	FROM_OPEN_CARD,								// 翻卡
	FROM_CELLECTION_GROUP_RESET,				// 收藏组重置
	FROM_EQUIPMENT_REFINE,						// 装备精炼
	FROM_GEM_OPEN_HOLE,							// 宝石开启孔位	
	FROM_HERO_TRAIN,							// 英雄培养
	FROM_HERO_DESTINY,							// 英雄天命(潜能)
	FROM_EQUIP_AWAKENING_ITEM,					// 装备觉醒道具
	FROM_HERO_AWAKEN,							// 英雄觉醒
	FROM_USE_ITEM,								// 使用物品
	FROM_ITEM_DECOMPOSE,						// 物品分解
	FROM_DELETE_TIMEOUT_ITEM,					// 删除已过期的物品
	FROM_ROLE_NAME_MODIFY,						// 角色名称修改
	FROM_HERO_SHOP_REFRESH,						// 英雄商店刷新
	FROM_AWAKEN_SHOP_REFRESH,					// 觉醒商店刷新
	FROM_SELL_ITEM,								// 出售物品	
	FROM_SPRITE_ENHANCE,						// 精灵强化
	FROM_SPRITE_COMPOSE,						// 精灵合成
	FROM_SPRITE_TRAIN,							// 精灵培养
	FROM_SPRITE_STAR_UP,						// 精灵升星
	FROM_TREASURE_REFINE,						// 宝物精炼
	FROM_HERO_LEVEL_UP,							// 英雄升级
	FROM_SELL_HERO,								// 出售英雄
	FROM_SELL_EQUIPMENT,						// 出售装备
	FROM_SELL_TREASURE,							// 出售宝物
	FROM_TREASURE_ENHANCE,						// 宝物强化
	FROM_CARD_LEVEL_UP,							// 卡牌升级
	FROM_HERO_BREAK,							// 英雄突破
	FROM_SCORE_MATCH_BATTLE_TIMES_REWRAD,		// 领取积分赛挑战次数奖励
	FROM_HERO_EVOLUTION,						// 英雄进化
	FROM_TREASURE_QUALITY_UP,					// 宝物升品
	FROM_EQUIPMENT_STAR_UP,						// 装备升星	
	FROM_EQUIPMENT_QUALITY_UP,					// 装备升品	
	FROM_SHOP_RESOURCE_WAR,						// 资源战商品购买
	FROM_GUILD_WAR_VS_STAR_BATTLE,				// 军团战对抗星球战斗
	FROM_GUILD_WAR_SCORE_STAR_BATTLE,			// 军团战积分星球战斗
	FROM_GUILD_WAR_TAKE_STAR_REWARD,			// 军团战领取星球奖励
	FROM_DAILY_GIFT_TAKE,						// 每日礼包领取 
	FROM_WEEK_GIFT_BUY,							// 每周礼包购买 
	FROM_VIP_SUMMON,							// 英雄VIP招募
	FROM_SHOP_GM,								// GM商品购买
	FROM_LUXURY_SIGN_REWARD,					// 领取豪华签到奖励
	FROM_YCS,									// 摇钱树奖励
	FROM_TODAY_RECOMMEND_REWARD,				// 领取今日推荐充值的奖励
	FROM_LEVEL_GIFT,							// 等级礼包领取
	FROM_BUY_FUND,								// 购买开服基金
	FROM_FUND,									// 基金
	FROM_ALL_PEOPLE_WELFARE,					// 全民福利领取
	FROM_LIMIT_PREFERENTIAL_WELFARE,			// 领取限时优惠福利
	FROM_FESTIVAL_DIAMOND_EXCHANGE,				// 节日活动钻石兑换
	FROM_ARENA_BATTLE,							// 竞技场挑战
	FROM_TQT,									// 铜雀台奖励
	FROM_ATTACK_CITY_ENCOURAGE,					// 怪物攻城鼓舞
	FROM_ATTACK_CITY_RELIVE,					// 怪物攻城复活请求
	FROM_DOUBAO,								// 夺宝挑战
	FROM_ENDLESS_KEEP_KILL_REWARD,				// 领取无尽之地连杀奖励
	FROM_EQUIP_ENHANCE,							// 装备强化
	FROM_MAIN_FB_RESET,							// 主线副本重置
	FROM_TOWER_FB_BATTLE,						// 爬塔副本挑战
	FROM_TOWER_BATTLE_TIMES_BUY,				// 爬塔挑战购买次数 
	FROM_TOWER_RESET,							// 爬塔重置副本 
	FROM_MAIN_FB_BATTLE,						// 主线副本挑战
	FROM_HERO_FB_BATTLE,						// 英雄副本挑战
	FROM_ELITE_FB_BATTLE,						// 精英副本挑战						
	FROM_DAILY_FB_BATTLE,						// 日常副本挑战
	FROM_BUY_GUILD_COPY_BATTLE_TIMES,			// 购买副本挑战次数
	FROM_GUILD_CREATE,							// 创建工会
	FROM_GUILD_QY,								// 工会建设
	FROM_GUILD_SKILL_LV_UP,						// 工会技术升级
	FROM_GUILD_IMPEACH,							// 弹劾
	FROM_BUY_GUILD_WAR_BATTLE_TIMES,			// 购买资源战挑战次数
	FROM_HERO_HIGH_SUMMON,						// 英雄高级召唤
	FROM_LAND_PATROL,							// 开始领地巡逻
	FROM_LAND_LEVEL_UP,							// 领地升级
	FROM_LAND_SUPPRESS_RIOT,					// 领地暴动镇压
	FROM_NEW_ROLE,								// 新建角色
	FROM_BUY_RECHARGE,							// 购买充值档位
	FROM_GET_MONTH_CARD_REWARD,					// 领取月卡奖励
	FROM_SCORE_MATCH_REFRESH_TARGET,			// 积分赛刷新目标
	FROM_WARCRAFT_BATTLE_TIMES_BUY,				// 争霸赛挑战次数购买
	FROM_WORLD_BOSS_BATTLE_TIMES_BUY,			// 世界BOSS挑战次数购买
	FROM_FAIRY_BREAK,							// 仙灵突破	
	FROM_CARD_GROUP_REFRESH,					// 卡组刷新
	FROM_ELITE_FB_RESET,						// 精英副本重置
	FROM_TOP_UP,								// 充值奖励
	FROM_RESOURCE_WAR_BATTLE,					// 资源战挑战
	FROM_SCORE_MATCH_BATTLE_TIMES_BUY,			// 积分赛挑战次数购买
	FROM_HANG_UP_DROP_OBTAIN,					// 挂机掉落领取



	FROM_GM = 9999,								// GM添加
};

/*--------------------------------------------------------------------------------------------------------------------------*/
// 2)Map服务器相关定义
/*--------------------------------------------------------------------------------------------------------------------------*/
//游戏对象ID空间划分
#define ID_INVALID					0				//无效的ID           
#define ID_PLAYER_MIN				1				//最小玩家角色ID
#define ID_PLAYER_MAX				2000000000		//最大玩家角色ID
#define ID_MONSTER_MIN				2000000001		//最小怪物ID
#define ID_MONSTER_MAX				2050000000		//最小怪物ID
#define ID_GAMEOBJECT_MIN			2050000001		//最小游戏对象ID
#define ID_GAMEOBJECT_MAX			2166000000		//最大游戏对象ID

#define ID_ROBOT_MIN			1000000000			// 机器人最小ID
#define ID_ROBOT_MAX			2000000000			// 机器人最大ID

#define MAX_CHARACTER_NUM			5				//最大角色数量
#define SAVE_DB_INTERVAL_TIME		(1000*60*3)		//DB存储间隔时间(毫秒)
#define MAX_ATTR_NUM				255				//最大属性个数


/*--------------------------------------------------------------------------------------------------------------------------*/
//数据库相关
//存储数据库类型
enum SaveDBType
{
	SAVE_DB_TYPE_TIMING				= 0,			//定时存储DB
	SAVE_DB_TYPE_LOGOUT				= 1,			//登出存储DB
};

//DB查询索引值
enum MAP_QUERY_INDEX
{
	MAP_QUERY_CHECK_ACCOUNT,		//检测帐号					= 1
	MAP_QUERY_CREATE_ACCOUNT,		//创建帐号
	MAP_QUERY_CHAR_LIST,			//查询角色列表信息
	MAP_QUERY_CHANGE_PASSWORD,		//修改密码
	MAP_QUERY_CHANGE_MOBILE,		//修改手机号

	DB_SDK_ACCOUNT_CHECK,			// SDK帐号检测
	DB_SDK_CREATE_ACCOUNT,			// SDK创建帐号

	MAP_QUERY_CREATE_CHAR,			//创建角色					= 1
	MAP_QUERY_CHAR_INFO,			//查询角色游戏信息			= 2
	MAP_QUERY_UPDATE_CHAR,			//存储角色					= 3
	MAP_UPDATE_LAST_LOGIN_SERVER,	//更新最近登录的服务器

	MAP_QUERY_INSERT_EMAIL,       //插入邮件 群体				= 8
	MAP_QUERY_DELETETIMEOUT_EMAIL,		//							= 13
	MAP_QUERY_SAVE_All_EMAIL,		//保存角色所有邮件信息		= 23


	MAP_SERVER_GOLBALDATA_SAVE,		//服务器全局数据存储
	MAP_SERVER_GOLBALDATA_LOAD,		//服务器全局数据加载

	DB_ROLE_BASE_DATA_LIST_LOAD,	// 角色基本数据列表加载
	DB_FRIEND_DATA_UPDATE,			// 好友数据更新

	DB_ARENA_DATA_SAVE,				// 竞技场数据存储
	DB_ARENA_DATA_LOAD,				// 竞技场数据加载

	DB_ARENA_BATTLE_RECORD_SAVE,	// 竞技场战斗记录存储
	DB_ARENA_BATTLE_RECORD_LOAD,	// 竞技场战斗记录加载

	DB_ROLE_RANK_DATA_SAVE,			// 角色排行榜数据存储
	DB_ROLE_RANK_DATA_LOAD,			// 角色排行榜数据加载

	DB_SERVER_LIST_LOAD,			// 加载服务器列表请求

	DB_ROLE_CURRENCY_LOGS_SAVE,		// 角色货币日志存储

	DB_LAND_DATA_SAVE,				// 存储领地数据
	DB_LAND_DATA_LOAD,				// 加载领地数据

	DB_ARENA_RANK_VIEW_LIST_UPDATE, // 竞技场排行榜查看列表更新

	DB_LEAVE_MESSAGE_SAVE,			// 留言存储

	DB_REWARD_CENTER_DATA_SAVE,		// 奖励中心数据存储

	DB_GUILD_DATA_SAVE,				// 工会数据存储
	DB_GUILD_DATA_LOAD,				// 工会数据加载

	DB_CHAT_RECORD_SAVE,			// 聊天记录存储
	DB_CHAT_RECORD_LOAD,			// 聊天记录加载(todo::暂时不使用)

	DB_LOG_SAVE,					// 日志存储

	DB_MAP_COMMENT_DATA_SAVE,		// 关卡评论数据存储
	DB_MAP_COMMENT_DATA_LOAD,		// 关卡评论数据加载

	DB_ROLE_TEMP_DATA_UPDATE,		// 角色临时数据更新

	DB_ROLE_OFFLINE_RECORD_SAVE,	// 存储角色下线记录
	DB_ROLE_LEVEL_RECORD_SAVE,		// 存储角色等级记录

	DB_ROLE_RECHARGE_RECORD_SAVE,	// 存储充值记录

	DB_ROLE_MAP_LOSE_RECORD_SAVE,	// 存储关卡失败记录

	DB_ROLE_FIRST_PASS_MAP_RECORD_SAVE,	// 首次通关记录 

	DB_SHOP_BUY_RECORD_SAVE,	// 商店购买记录

	DB_ROLE_ITEM_LOSE_RECORD_SAVE,	// 存储物品丢失记录
	DB_ROLE_ITEM_OPERATION_RECORD_SAVE,	// 存储物品操作记录

	DB_OFFLINE_PLAYER_NUM_RECORD_SAVE,	// 存储在线玩家数量记录

	DB_ROLE_ACCOUNT_LOCK_UPDATE,	// 帐号锁住更新

	DB_GM_EVENT_DATA_LOAD,

	DB_KING_MATCH_DATA_SAVE,		// 王者赛数据存储
	DB_KING_MATCH_DATA_LOAD,		// 王者赛数据加载

	DB_TEAM_DUNGEON_RANK_DATA_SAVE,		// 组队副本排行数据存储
	DB_TEAM_DUNGEON_RANK_DATA_LOAD,		// 组队副本排行数据加载

	DB_TURNTABLE_DATA_SAVE,			// 转盘数据存储
	DB_TURNTABLE_DATA_LOAD,			// 转盘数据加载

	DB_SCORE_MATCH_DATA_SAVE,		// 积分赛数据存储
	DB_SCORE_MATCH_DATA_LOAD,		// 积分赛数据加载

	DB_NEW_ROLE_GUIDE_UPDATE,		// 新手引导更新

	DB_RESOURCE_WAR_DATA_SAVE,		// 资源战数据存储
	DB_RESOURCE_WAR_DATA_LOAD,		// 资源战数据加载

	DB_GUILD_WAR_DATA_SAVE,				// 工会战数据存储
	DB_GUILD_WAR_DATA_LOAD,				// 工会战数据加载

	DB_ACTIVATION_CODE_SELECT,			// 激活码数据检索
	DB_ACTIVATION_CODE_UPDATE,			// 激活码数据更新

	DB_GAME_STATISTICS_EVENT,			// 游戏统计事件

	DB_GM_OPERATION_RECORD_SAVE,		// 存储GM操作记录
};

/*--------------------------------------------------------------------------------------------------------------------------*/
//角色相关
//#define MAX_CHAR_LV					50				//最大角色等级
#define MAX_BIND_TARGET_NUM			10				//最大绑定目标数量

//角色种族
enum CharRace
{
	CHAR_RACE_NONE					= 0,
	CHAR_RACE_1						= 1,			//人族
	CHAR_RACE_2						= 2,			//天人族
	CHAR_RACE_3						= 3,			//妖族
	CHAR_RACE_4						= 4,			//修罗族
	CHAR_RACE_5						= 5,			//羽民族
	CHAR_RACE_6						= 6,			//巫族
	CHAR_RACE_7						= 7,			//鬼族
	CHAR_RACE_MAX,
};

//角色性别
enum CharGender
{
	CHAR_GENDER_FEMALE				= 1,			//女性
	CHAR_GENDER_MALE				= 2,			//男性
	CHAR_GENDER_MAX,
};

//游戏对象朝向
enum GameObjectOrien
{
	GO_ORIEN_NONE					= 0,
	GO_ORIEN_FRONT					= 1,			//正面
	GO_ORIEN_BACK					= 2,			//背面
	GO_ORIEN_MAX,
};

//游戏对象状态
enum GameObjectStatus
{
	GO_STATUS_NONE					= 0x00000000,	//无状态
	GO_STATUS_OVERAWE				= 0x00000001,	//威慑状态(不能攻击)
	GO_STATUS_CONTROL				= 0x00000002,	//反控状态(陷阱卡不生效)
	GO_STATUS_SLOW					= 0x00000004,	//迟缓状态(被召唤的回合不能进行攻击)
	GO_STATUS_FALSE_BODY			= 0x00000008,	//假身状态(不能被献祭)
	GO_STATUS_OVERTURN				= 0x00000010,	//翻身状态(覆盖牌被翻转切不触发翻转效果)
	GO_STATUS_ASSAULT				= 0x00000020,	//突击状态(攻击防御姿态的随从时，仍有溢出伤害)
	GO_STATUS_IMMUNE				= 0x00000040,	//免疫状态(无法被破坏)
	GO_STATUS_DOUBLE_HIT			= 0x00000080,	//连击状态(可以攻击2次)
	GO_STATUS_CRITICLE_HIT			= 0x00000100,	//暴击状态(2倍伤害)
	GO_STATUS_SUBSTITUTE			= 0x00000200,	//替身状态(让另外的随从代替自己被攻击)
	GO_STATUS_AVOIDANCE				= 0x00000400,	//免伤状态(不会受到伤害)
	GO_STATUS_BEAR					= 0x00000800,	//隐忍状态(覆盖牌受到攻击不会翻转)
	GO_STATUS_RUSH					= 0x00001000,	//冲锋状态(可以直接攻击首领)
	GO_STATUS_RESTRAIN				= 0x00002000,	//克制状态(随从不能攻击首领)
	GO_STATUS_DRAIN					= 0x00004000,	//枯竭状态(没有手牌随从不能攻击)
	GO_STATUS_SILENCE				= 0x00008000,	//沉默状态(随从脚本，法术脚本，陷阱脚本，法术效果均无法生效)
};

enum SystemIDs
{
	SystemIDs_None				=0,
	SystemIDs_DeleteEmail		=1,
	SystemIDs_ClearMission		=2,
	SystemIDs_ClearLuckyTreeNum	=3,
	SystemIDs_RandAcceptMission	=4,
	SystemIDs_Add7DaysRMoney	=5,
	SystemIDs_Add30DaysRMoney	=6,
	SystemIDs_LadderAward		=7,
	SystemIDs_QQVip				=8,
	SystemIDs_OnlineTimeReset	=9,
			
};

enum ActivityOpe
{
	ActivityOpe_Begin,
	ActivityOpe_End, 
	ActivityOpe_Update,
};

//聊天扩展数据类型
enum ChatExpandType
{
	CHAT_EXPAND_TYPE_NONE			= 0,		//无扩展数据
	CHAT_EXPAND_TYPE_MAX,
};

//聊天类型
enum ChatType
{
	//CHAT_TYPE_SYSTEM				= 0,		//系统频道
	CHAT_TYPE_P2P					= 1,		//私聊频道
	CHAT_TYPE_WORLD					= 2,		//世界频道
	CHAT_TYPE_GUILD					= 3,		//工会频道
	CHAT_TYPE_TEAM					= 4,		//队伍频道
	CHAT_TYPE_TEAM_HALL				= 5,		//组队大厅频道
	CHAT_TYPE_MAX,
};

//任务触发类型(服务器内部使用)
enum MissionTriggerType
{
	MISSION_TRIGGER_FROM_SERVER_NO_ASK  = 0,			//服务器触发(不询问客户端)
};

//平台Http请求类型
enum OPEN_QUERY_INDEX
{
	OPEN_QUERY_AUTH					=1,
	OPEN_QUERY_REG_UDATA            =2,
	OPEN_QUERY_QRY_UDATA            =3,
	OPEN_QUERY_SYNC_LGNDATA         =4,
	OPEN_QUERY_LOAD_POINTCFG        =5,

	OPEN_QUERY_WEB_COMMON			=6,

	HTTP_REYUN_STATISTIC = 7, // 热云统计

	HTTP_CHECK_USER_INFO = 8, // 检测用户信息
	HTTP_ORDER_ID = 9,	// 取订单号

	HTTP_RECHARGE_SUCCESS_REPORT = 10, // 充值成功上报

	HTTP_STATISTICS_REPORT = 11,	// 数据统计上报
};

//角色事件广播
#define MAX_BROADCAST_PARA_NUM	7
#define MAX_BROADCAST_TEXT_LEN	128

//角色事件广播类型
enum EventBroadcastType
{
	EBT_NONE					= 0,
	EBT_OPENCARDPACKET			= 1,	//抽卡
	EBT_FINANCIAL_PLAN			= 2,	//理财计划
	EBT_7DAY_OPENCARD			= 3,	//开服7天抽卡系统
	EBT_7DAY_EXCHANGE			= 4,	//开服7天兑换
	EBT_BUY_GROUPCARD			= 5,	//购买卡组
	EBT_MAX,
};


// 游戏平台类型
enum
{
	PLATFORM_TEST		= 0,	// 测试(内部使用)
	PLATFORM_ZL			= 1,	// 掌浪
	PLATFORM_360		= 2,	// 360
	PLATFORM_4399		= 3,	// 4399
	PLATFORM_UC			= 4,	// UC平台
	PLATFORM_IOS		= 5,	// IOS平台
};

// 终端类型
enum TerminalType
{
	TERMINAL_ANDROID = 1,	// 安卓
	TERMINAL_IOS = 2,		// 苹果
};

// 登录类型 登录类型(0=测试,1=用户名注册,2=用户名登录,3=手机登录,4=手机注册)
enum LoginType
{
	LOGIN_TEST = 0,				// 测试
	LOGIN_USER_NAME_REGISTER,	// 用户名注册
	LOGIN_USER_NAME_LOGIN,		// 用户名登录
	LOGIN_MOBILE_LOGIN,			// 手机登录
	LOGIN_MOBILE_REGISTER,		// 手机注册
	LOGIN_ONE_KEY,				// 一键注册
};

// 英雄召唤类型
enum HeroSummonType
{
	NORMAL_FREE_SUMMON = 1,			// 普通免费召唤
	NORMAL_ITEM_SUMMON,				// 普通道具召唤
	NORMAL_ITEM_ONEKEY_SUMMON,		// 普通道具批量召唤
	ADVANCED_FREE_SUMMON,			// 高级免费召唤
	ADVANCED_ITEM_SUMMON,			// 高级道具召唤
	ADVANCED_GOLD_SUMMON,			// 高级金钱召唤
	ADVANCED_GOLD_ONEKEY_SUMMON,	// 高级金钱批量召唤
	FRIENDSHIP_POINT_SUMMON,		// 友情点召唤
	FRIENDSHIP_POINT_ONE_KEY_SUMMON,// 友情点批量召唤
	ADVANCED_ITEM_ONEKEY_SUMMON,	// 高级道具批量召唤
};

// 回收类型
enum
{
	RECYCLE_HERO_DECOMPOSE = 1,		// 英雄分解
	RECYCLE_HERO_RESET,				// 武将重生
	RECYCLE_EQUIPMENT_DECOMPOSE,	// 装备分解
	RECYCLE_EQUIPMENT_RESET,		// 武将重生
	RECYCLE_TREASURE_RESET,			// 宝物重生
};

// 物品类型
// enum ItemLogicType
// {
// 	ITEM_LOGIC_UNKNOW = 0,	// 错误类型
// };

// 角色排行榜类型
enum RoleRankType
{
	RANK_BATTLE_VALUE = 1,		// 1.战力
	RANK_LEVEL,					// 2.等级
	RANK_TOWER,					// 3.爬塔
	RANK_ARENA,					// 4.竞技场(所的模块管理排行数据)
	RANK_REBEL_DAMAGE,			// 5.叛军伤害排行榜
	RANK_REBEL_EXPLOIT,			// 6.叛军功勋排行榜
	RANK_WORLD_BOSS_HONOR,		// 7.世界BOSS荣誉排行榜(所在的模块管理排行数据)
	RANK_WORLD_BOSS_DAMAGE,		// 8.世界BOSS伤害排行榜(所在的模块管理排行数据)
	RANK_FAIRY_POKEDEX_VALUE,	// 9.仙灵图鉴值排行
	RANK_SCORE_MATCH_SUPER,		// 10.积分赛排行榜(超能)
	RANK_MAIN_FB_STAR,			// 11.主线副本星数排行榜
	RANK_PVP_LOCAL,				// 12.本服PVP排名(对决)
	RANK_PVP_ALL,				// 13.全服PVP排名
	RANK_ATTACK_CITY,			// 14.攻城排名
	RANK_ENDLESS_KILL,			// 15.无尽之地杀敌排行
	RANK_KINGMATCH,				// 16.王者赛
	RANK_ZHANBU,				// 17.占卜排行榜(卡牌收藏值)
	RANK_TURNTABLE_LOTTERY,		// 18.转盘抽奖排行榜
	RANK_SCORE_MATCH_LEGEND,	// 19.积分赛排行榜(传奇(传说))
	RANK_SCORE_MATCH_WARCRAFT,	// 20.积分赛排行榜(魔兽(幻想))
	RANK_GUILD_WAR,				// 21.工会战排行榜
	RANK_DIAMOND_COST,			// 22.钻石消费排行榜(活动期间,跨服排行)
	RANK_RECHARGE,				// 23.充值排行榜(活动期间,跨服排行)

	RANK_ROLE_MAX,
};

// 工会排行榜
enum GuildRankType
{
	GUILD_RANK_LEVEL = 0,	// 等级排行
	GUILD_RANK_COPY	= 1,	// 副本排行

	GUILD_RANK_MAX,
};

// 工会职位
enum
{
	GUILD_POS_INVALID,			// 无效职位
	GUILD_POS_CAPTAIN = 1,		// 首领
	GUILD_POS_ASSIST,			// 副首领 
	GUILD_POS_MEMBER,			// 成员
};

// 工会属性
enum
{
	GUILD_PRO_NOTICE = 1,	// 公告
	GUILD_PRO_DECLARATION,	// 宣言
	GUILD_PRO_ICON,			// 图标
	GUILD_PRO_EXP,			// 经验
	GUILD_PRO_LEVEL,		// 等级
};

// 工会商店类型
enum 
{
	GUILD_SHOP_ITEM = 1,	// 道具商店
	GUILD_SHOP_LIMIT,		// 限时商店
	GUILD_SHOP_AWARD,		// 奖励商店
};


// 商店商品类型
enum ShopGoodsType
{
	SHOP_MALL_ITEM	= 1,				// 商城道具商品
	SHOP_MALL_GIFT	= 2,				// 商城礼包商品
	SHOP_GUILD_ITEM	= 3,				// 公会道具商店
	SHOP_GUILD_LIMIT= 4,				// 公会限时商店
	SHOP_GUILD_AWARD= 5,				// 公会奖励商店
	SHOP_HERO		= 6,				// 英雄商店
	SHOP_REBEL		= 7,				// 叛军商店
	SHOP_TOWER		= 8,				// 爬塔商店
	SHOP_ARENA		= 9,				// 竞技场商店
	SHOP_AWAKEN		= 10,				// 觉醒商店
	SHOP_STAR_SOUL	= 11,				// 星魂商店
	SHOP_SCORE_MATCH = 12,				// 积分赛商店
	SHOP_SEVEN_DAY	= 13,				// 7天活动商店
	SHOP_LEVEL_GIFT = 14,				// 等级礼包商店
	SHOP_LIMIT_PREFERENTIAL_SHOP = 15,	// 限时优惠商店
	SHOP_ENDLESS_LAND_ITEM = 16,		// 无尽之地道具商店
	SHOP_ENDLESS_LAND_REWARD = 17,		// 无尽之地奖励商店
	SHOP_DIRECT_BUY = 18,				// 直购商店
	SHOP_EXCHANGE = 19,					// 兑换商店
	SHOP_RECOMMEND = 20,				// 推荐商店
	SHOP_TEAM_DUNGEON_NORMAL = 21,		// 组队普通商店
	SHOP_TEAM_DUNGEON_REWARD = 22,		// 组队奖励商店
	SHOP_DESTINY = 23,					// 命运商店
	SHOP_HIDE = 24,						// 隐藏商店
	SHOP_CHRISTMAS_SCORE_EXCHANGE = 25,	// 圣诞节积分兑换商店	
	SHOP_CHRISTMAS_ITEM_EXCHANGE = 26,	// 圣诞节物品兑换商店	
	SHOP_CRYSTAL = 27,					// 水晶商店
	SHOP_HOT_SALE_GOODS = 28,			// 热卖商品
	SHOP_RESOURCE_WAR_SHOP = 29,		// 资源战商店
	SHOP_GM = 30,						// GM商店
};

// 物品类型
enum ItemType
{
	ITEM_TYPE_NORMAL = 1,	// 常用物品
	ITEM_TYPE_GIFT	= 2,	// 礼包
};

// 物品逻辑类型
enum ItemLogicType
{
	ITEM_LOGIC_UNKNOW = 0,	// 错误类型
	ITEM_TYPE_1 = 1,		// 体力药剂
	ITEM_TYPE_2 = 2,		// 精力药剂
	ITEM_TYPE_3 = 3,		// 装备精炼石
	ITEM_TYPE_4 = 4,		// 进阶石
	ITEM_TYPE_5 = 5,		// 培养石
	ITEM_TYPE_6 = 6,		// 潜能合剂
	ITEM_TYPE_7 = 7,		// 装备碎片
	ITEM_TYPE_8 = 8,		// 英雄碎片
	ITEM_TYPE_9 = 9,		// 宝物精炼石
	ITEM_TYPE_10 = 10,		// 金币
	ITEM_TYPE_11 = 11,		// 钻石
	ITEM_TYPE_12 = 12,		// 魂晶
	ITEM_TYPE_13 = 13,		// 宝物碎片
	ITEM_TYPE_14 = 14,		// 夺宝免战物品
	ITEM_TYPE_15 = 15,		// 征讨令
	ITEM_TYPE_20 = 20,		// 货币类型
};

// 物品礼包类型
enum ItemGiftType
{
	GIFT_TYPE_RAND = 1,			// 随机礼包
	GIFT_TYPE_MULTIPLE = 2,		// 多选一
	GIFT_TYPE_STATIC = 3,		// 固定礼包
};

// 品质类型
enum EQualityType
{
	QUALITY_WHITE,		// 0 = 白
	QUALITY_GREEN,		// 1 = 绿
	QUALITY_BLUE,		// 2 = 蓝
	QUALITY_PURPLE,		// 3 = 紫
	QUALITY_ORANGE,		// 4 = 橙
	QUALITY_RED,		// 5 = 红
	QUALITY_GOLD,		// 6 = 金

	QUALITY_NUM,		// 品质数量

	QUALITY_INVALID,
};

// 副本奖励类型
enum EFBAwardType
{
	FB_AWARD_MAP	= 1,	// 关卡奖励
	FB_AWARD_STAR	= 2,	// 星数奖励
};

// 好友精力领取状态
enum FriendEnergyGetState
{
	FIEND_ENERGY_STATE_NONE = 0,	// 还没赠送
	FIEND_ENERGY_STATE_READY,		// 可领取
	FIEND_ENERGY_STATE_TAKE,		// 已领取
};

// 更新好友数据类型
enum FriendUpdateType
{
	FRIEND_UPDATE_DEL_FRIEND = 1,		// 删除好友
	FRIEND_UPDATE_ADD_BLACK,	// 添加黑名单
	FRIEND_UPDATE_ADD_FRIEND,	// 添加好友
};

// 回收操作类型
enum RecyclingOperateType
{
	RECYCLING_OPERATE_DECOMPOSE = 1,	// 分解
	RECYCLING_OPERATE_RESET,			// 重生(重置)	
};

// VIP特权类型
enum VipType
{
	VIP_EQUIP_STRENGTHEN_CRIT_RATE = 0,			// 0	装备强化暴击率：在不同的VIP等级可配置对应的暴击率百分比
	VIP_LAND_SUPPRESS_TIMES,					// 1	领地攻讨帮助好友处理暴动次数：每天可处理暴动次数上限随着VIP等级提高而提高
	VIP_MAIN_FB_RESET_TIMES,					// 2	主线副本每日可重置次数
	VIP_TOWER_FB_RESET_TIMES,					// 3	爬塔副本每日重置次数
	VIP_HERO_SHOP_REFRESH_TIMES,				// 4	雄魂商店每日刷新次数
	VIP_HERO_FB_BATTLE_TIMES,					// 5	英雄副本进入次数
	VIP_BUY_WANTED_TIMES,						// 6	每天在商城中购买征讨令次数
	VIP_BUY_ENERGY_TIMES,						// 7	每天在商城中购买的精力丹次数
	VIP_BUY_STAMINA_TIMES,						// 8	每天在商城中购买的体力丹次数
	VIP_BUY_EXP_HERO_TIMES,						// 9	每天在商城中购买经验英雄次数
	VIP_BUY_GOLD_TIMES,							// 10	每天在商城中购买金币的次数
	VIP_BUY_ORANGE_EQUIP_TIMES,					// 11	每天在商城中购买橙色装备箱子次数
	VIP_BUY_ORANGE_TREASURE_TIMES,				// 12	每天在商城中购买橙色宝物箱子
	VIP_BUY_TOWER_FB_BATTLE_TIMES,				// 13	爬塔副本精英挑战购买次数
	VIP_THREE_MULTIPLE_PLAY_FLAG,				// 14	开启战斗3倍数数功能：固定VIP等级开启 (未处理) (只需客户端处理)
	VIP_HERO_BAG_NUM,							// 15	英雄背包格子
	VIP_EQUIPMENT_BAG_NUM,						// 16	装备背包格子
	VIP_CREATE_GUILD_FLAG,						// 17	开启公会创建权限：在对应的VIP等级改期创建VIP权限
	VIP_LAND_HIGH_PATROL,						// 18	开启高级巡逻
	VIP_LAND_BEST_PATROL,						// 19   开启终极巡逻
	VIP_TOWER_MAP_CLEAN,						// 20	开启爬塔扫荡功能 (未处理)	 			
	VIP_FIVE_TIMES_DB_FLAG,						// 21	开启宝物抢夺5次功能
	VIP_MAIN_MAP_CLEAN,							// 22   开启主线副本扫荡
	VIP_TREASURE_BAG_NUM,						// 23	宝物背包总上限
	VIP_AWAKEN_SHOP_REFRESH_TIMES,				// 24	觉醒商店刷新上限
	VIP_WORLD_BOSS_BATTLE_TIMES_BUY,			// 25   世界BOSS挑战次数购买上限	
	VIP_SCORE_MATCH_BATTLE_TIMES_BUY,			// 26   积分赛可购买的额外次数 
	VIP_ELITE_FB_RESET_TIMES,					// 27	精英副本每日可重置次数
	VIP_WARCRAFT_BATTLE_TIMES,					// 28	争霸赛每日免费挑战次数
	VIP_WARCRAFT_BUY_BATTLE_TIMES,				// 29	争霸赛每日可购买的挑战次数
	VIP_GUILD_FB_BUY_BATTLE_TIMES,				// 30	工会副本每日可购买的挑战次数
	VIP_TEAM_DUNGEON_DAILY_REWARD_TIMES,		// 31   组队副本每日奖励次数上限
	VIP_CARD_BAG_NUM,							// 32   卡牌背包格子
	VIP_ENDLESS_LAND_DAILY_BATTLE_TIMES = 35,	// 35   无尽之地每日挑战次数

	VIP_TYPE_MAX
};

// 阵营类型
enum 
{
	CAMP_SUPER  = 0,	// 超能
	CAMP_LEGEND = 1,	// 传奇(传说)
	CAMP_WARCRAFT = 2,	// 魔兽(幻想)
	CAMP_NUM,			// 阵营数量
	CAMP_BEGIN = CAMP_SUPER,
	CAMP_END = CAMP_WARCRAFT,
	CAMP_INVALID = 99,	// 无效阵营类型
};

// 世界BOSS奖励类型
enum 
{
	WORLD_BOSS_REWARD_HONOR = 1,	// 荣誉
	WORLD_BOSS_REWARD_DAMAGE = 2,	// 伤害
};

// 仙灵招募类型
enum
{
	FAIRY_RECRUIT_NORMAL_SINGLE  = 0,	// 0=普通单次招募
	FAIRY_RECRUIT_NORMAL_MANY,			// 1=普通多次招募
	FAIRY_RECRUIT_HIGH_SINGLE,			// 2=高级单次招募
	FAIRY_RECRUIT_HIGH_MANY,			// 3=高级多次招募

	FAIRY_RECRUIT_TYPE_MAX,
};

// 邮件上下文类型
enum
{
	EMAIL_CONTEXT_INT = 1,			// 数字类型
	EMAIL_CONTEXT_STRING = 2,		// 字符串类型
	EMAIL_CONTEXT_ITEM = 3,			// 物品ID
	EMAIL_CONTEXT_EQUIPMENT = 4,	// 装备ID
	EMAIL_CONTEXT_HERO = 5,			// 英雄ID
};

// 一次性标志
enum OnceFlag
{
	once_normal_summon			= 1,		// 普通召唤
	once_hero_shop_refresh		= 2,		// 英雄商店刷新
	once_get_purple_treasure	= 3,		// 抢夺紫色宝物
	once_hero_train				= 4,		// 英雄培养
};

// 系统模块ID
enum System_module_id
{
	SYSTEM_DAILY_FB			= 2,		// 日常副本(纯客户端表现)
	SYSTEM_DAILY_MISSION	= 3,		// 日常任务
	SYSTEM_HERO_SUMMON		= 9,		// 英雄招募
	SYSTEM_HERO_SHOP		= 11,		// 英雄商店
	SYSTEM_EMAIL			= 13,		// 邮件
	SYSTEM_FRIEND			= 15,		// 好友系统
	SYSTEM_REBEL			= 16,		// 叛军系统
	SYSTEM_TOWER			= 17,		// 爬塔
	SYSTEM_SPIRIT			= 18,		// 精灵(纯客户端表现)
	SYSTEM_DB				= 19,		// 夺宝(纯客户端表现)
	SYSTEM_LAND				= 20,		// 领地系统
	SYSTEM_ARENA			= 21,		// 竞技场
	SYSTEM_GUILD			= 22,		// 工会
	SYSTEM_REWARD_CENTER	= 24,		// 领奖中心
	SYSTEM_MAIN_FB			= 26,		// 主线副本
	SYSTEM_HERO_FB			= 27,		// 英雄副本
	SYSTEM_ELITE			= 28,		// 精英副本
	SYSTEM_WORLD_BOSS		= 47,		// 世界BOSS
	SYSTEM_ACHIVEMENT		= 48,		// 成就
	SYSTEM_SIGN				= 51,		// 签到
	SYSTEM_SEVEN_DAY_1		= 52,		// 七天活动
	SYSTEM_TQT				= 53,		// 铜雀台(体力领取)
	SYSTEM_YCS				= 54,		// 迎财神(金币领取)
	SYSTEM_LEVEL_SHOP		= 57,		// 等级商店
	SYSTEM_LEVEL_GIFT		= 59,		// 等级礼包
	SYSTEM_PVP				= 72,		// PVP
	SYSTEM_SGL_RCG			= 74,		// 单次返利
	SYSTEM_AMT_RCG			= 75, 		// 累充豪礼
	SYSTEM_FRT_RCG			= 76,		// 首充
	SYSTEM_LMT_RCT			= 77,		// 限时召唤
	SYSTEM_VIP_GIFT			= 79,		// VIP福利
	SYSTEM_MONTH_CARD		= 80,		// 月卡福利
	SYSTEM_ALL_PEOPLE_WELFARE = 81,		// 全民福利
	SYSTEM_FUND				= 82,		// 开服基金
	SYSTEM_ATTACK_CITY		= 84,		// 怪物攻城
	SYSTEM_LUXURY_SIGN_IN	= 93,		// 豪华签到
	SYSTEM_SUPER_DISCOUNT_ID= 94,		// 超值折扣
	SYSTEM_DIRECT_BUY		= 95,		// 直购商店
	SYSTEM_SEVEN_DAY_RANK	= 97,		// 7天排行榜
	SYSTEM_SEVEN_DAY_2		= 89,		// 七日活动2
	SYSTEM_FESTIVAL_ACTIVITY = 98,		// 节日活动
	SYSTEM_LOGIN_REWARD		= 106,		// 登录奖励
	SYSTEM_KING_MATCH		= 108,		// 王者赛
	SYSTEM_CHRISTMAS		= 119,		// 圣诞节
	SYSTEM_RECHARGE_EXTRA_REWARD = 127,		// 充值反馈(额外奖励)
	SYSTEM_TOTAL_RECHARGE_REWARD= 128,	// 累充奖励
	SYSTEM_NEW_SUPER_DISCOUNT_ID = 130,	// 新的超值折扣
	SYSTEM_ND_LOTTERY_ID = 131,			// 扭蛋抽奖
	SYSTEM_HOT_SALE_GOODS_ID = 132,		// 热卖商品
	SYSTEM_TURNTABLE	= 135,			// 转盘
	SYSTEM_WORLD_LEVEL_TASK = 142,		// 世界等级任务
};

// 工会红点类型
enum GuildRedPointType
{
	GUILD_RED_POINT_APPLY = 0,		// 入会申请列表有新的申请显示红点
	GUILD_RED_POINT_LIMIT_SHOP,		// 限时商店刷新后，限时页签，商店建筑显示红点，进入限时页签后红点消失
	GUILD_RED_POINT_REWARD_SHOP,	// 奖励商店解锁新的物品购买时，奖励页签，商店建筑显示红点，进入奖励页签红点消失
	GUILD_RED_POINT_QY,				// 军团有可剩余建设次数时，在军团建设建筑显示红点
	GUILD_RED_POINT_LEAVE_MESSAGE,	// 留言板内有新的留言，在留言按钮上显示红点，打开后红高点消失
	GUILD_RED_POINT_SKILL,			// 军团技能建筑中，当前剩余贡献值至少可学习/升级任意一次技能时，在军团技能建筑显示红点
	GUILD_RED_POINT_FB,				// 公会副本挑战
	GUILD_RED_POINT_CHAPTER_REWARD,	// 公会章节奖励
	GUILD_RED_POINT_MAP_REWARD,		// 公会关卡奖励
	GUILD_RED_POINT_MAX ,			// 工会红点类型上限
};


// PVP扮演的类型
enum
{
	PVP_ACT_RED = 0,	// 红方
	PVP_ACT_BLUE = 1,	// 蓝方
	PVP_ACT_INVALID = 255,	// 无效类型
};

// 限时活动类型
enum 
{
	LIMIT_ACTIVITY_SINGLE = 1,		// 单充
	LIMIT_ACTIVITY_ACCUMULATE = 2,	// 累充
	LIMIT_ACTIVITY_RECRUIT = 3,		// 招募
	LIMIT_ACTIVITY_FIRST = 4,		// 首充
	
};

// 挑战结果类型
enum 
{
	BATTLE_RESULT_DRAW = 0, // 平局
	BATTLE_RESULT_WIN = 1,	// 胜利
	BATTLE_RESULT_LOSE = 2, // 失败
};

// PVP挑战奖励类型
enum
{
	PVP_BATTLE_REWARD_DAILY = 1, // 每日
	PVP_BATTLE_REWARD_WEEKLY = 2, // 每周
};

// 临时数据类型
enum 
{
	ROLE_TEMP_DATA_TITLE = 1, // 称号
	ROLE_TEMP_FORBID_TO_TALK_SET= 2, // 禁言设置
	ROLE_TEMP_RECHARGE_SEND_GOODS = 3, // 充值发货
	ROLE_TEMP_SET_GUIDE = 4, // 设置引导
	ROLE_TEMP_SET_LEAVE_GUILD_TIME = 5, // 设置离开公会时间
};

// 渠道ID
enum 
{
	CHANNEL_UC = 9,			// uc
	CHANNEL_VIVO = 17,		// vivo
	CHANNEL_HUAWEI = 24,	// 华为
};

// 豪华充值状态
enum 
{
	LUXURY_RECHARGE_NOT_YET		= 0,	// 0 未达到领取条件
	LUXURY_RECHARGE_CAN_TAKE	= 1,	// 1 可以领取,
	LUXURY_RECHARGE_ALREADY_TAKE= 2,	// 2 已领取
};


// 奖励状态
enum
{
	REWARD_STATUS_UNFINISHED	= 0,	// 未完成
	REWARD_STATUS_DONE			= 1,	// 已完成
	REWARD_STATUS_ALREADY_TAKE	= 2,	// 已领取
};

// 活动时间类型
enum 
{
	ACTIVITY_TIME_APPOINT		= 1,	//  1.指定时间 开始日期[年:月 : 日] 结束日期[年:月 : 日]
	ACTIVITY_TIME_DAILY			= 2,	// 	2.每日 开始日期[0] 结束日期[0]
	ACTIVITY_TIME_WEEK			= 3,	//  3.星期 开始日期[星期几] 结束日期[星期几]
	ACTIVITY_TIME_MONTH			= 4,	//	4.月份 开始日期[几号] 结束日期[几号]
	ACTIVITY_TIME_OPEN_SERVER	= 5,	//  5.开服 开始日期[开服第几天] 结束日期[开服第几天]
	ACTIVITY_TIME_OPEN_SERVER_EX= 6,	//  6.开服 开始日期[开服第几天] 结束日期[开服第几天]
	ACTIVITY_TIME_CREATE_ROLE	= 7,	//  7.创建角色 开始日期[创建第几天] 结束日期[创建第几天] 1.创建当天算第一天, 2.包括结束当天
};

// 节目活动类型
enum
{
	LOOP_ONCE_RECHARGE = 1,				// 1、循环单充
	LOOP_ACCUMULATE_RECHARGE = 2,		// 2、循环累充
	LIMIT_RECRUIT = 3,					// 3、开服限时召唤
	ACTIVITY_ONCE_RECHARGE = 4,			// 4、活动单充
	ACTIVITY_ACCUMULATE_RECHARGE = 5,	// 5、活动累充
	DIAMOND_EXCHANGE = 6,				// 6、打折兑换（钻石换物品）
	ACTIVATE_TARGET = 7,				// 7、活跃目标
	ITEM_EXCHANGE = 8,					// 8、材料兑换（物品换物品）
	LOGIN_GIFT = 9,						// 9、登陆有礼
	OPEN_SERVER_LOGIN = 10,				// 10、开服登录
	OPEN_SVR_ACCUMULATE_RECHARGE = 11,	// 11、开服累充
	FESTIVAL_ITEM_EXCHANGE = 12,		// 12、节日材料兑换
	FESTIVAL_ACTIVITY_13 = 13,			// 13、打折贩售(同6)
	FESTIVAL_ACTIVITY_14 = 14,			// 14、节日活跃(同7)
	FESTIVAL_ACTIVITY_15 = 15,			// 15、开服活跃2(同7)
	FESTIVAL_ACTIVITY_16 = 16,			// 16、节日活跃2(同14)
	ACTIVITY_ONCE_RECHARGE_17 = 17,		// 17、活动单充2(同4,1)


	FESTIVAL_ACTIVITY_END,
	FESTIVAL_ACTIVITY_BEGIN = LOOP_ONCE_RECHARGE,
};

// 场景类型
enum 
{
	SCENE_TEAM_HALL = 201, //  组队副本大厅
};

// 任务类型
enum 
{
	TASK_ANSWER = 1,		// 1.答题
	TASK_GATHER = 2,		// 2.采集
	TASK_MONSTER = 3,		// 3.打怪
	TASK_DIALOG = 4,		// 4.对话
};

// 场景类型(用于多人同屏场景)
enum
{
	TEAM_HALL = 201,			// 组队大厅
	TEAM_WORLD_MAP = 202,		// 组队大地图
	TEAM_GATHER = 203,			// 组队采集场景
	TEAM_NORMAL_MONSTER = 204,	// 组队普通怪
	TEAM_BOSS = 205,			// 组队BOSS
};

// 热云统计
enum
{
	REYUN_STATISTICS_INSTALL = 1,	// 统计玩家第一次打开应用方法(客户端)	
	REYUN_STATISTICS_STARTUP = 2,	// 统计玩家打开应用方法(客户端)
	REYUN_STATISTICS_REGISTER = 3,	// 统计玩家账号注册
	REYUN_STATISTICS_LOGGEDIN = 4,	// 统计玩家登陆
	REYUN_STATISTICS_QUEST = 5,		// 统计玩家的任务/关卡/副本方法
	REYUN_STATISTICS_EVENT = 6,		// 统计玩家自定义事件方法
	REYUN_STATISTICS_HEARTBEAT = 7,	// 统计玩家在线的方法(客户端)
};

// 充值类型 1=月卡,2=普通充值,3=超值折扣
enum
{
	RECHARGE_MONTH_CARD = 1,		//  月卡
	RECHARGE_COMMON = 2,			//  普通充值
	RECHARGE_SUPER_DISCOUNT = 3,	//  超极折扣
	RECHARGE_DIRECT_BUY_DISCONT = 4,//  直购特惠
	RECHARGE_TODAY_RECOMMEND = 5,	//  今日推荐
	RECHARGE_ACCURATE_RECOMMEND = 6,//  精准推荐
	RECHARGE_HOT_SALE_GOODS = 7,//  热卖商品
	RECHARGE_NEW_SUPER_DISCOUNT = 8,//  新超值折扣
};

// 商品刷新类型
enum 
{
	SHOP_REFRESH_DAY,	// 每日 每日0点
	SHOP_REFRESH_WEEK,	// 每周 周一0点
	SHOP_REFRESH_MONTH,	// 每月 1号0点
};

// 游戏统计类型
enum 
{
	GAME_STATISTICS_NEW_DAY = 1,		// 每日事件
	GAME_STATISTICS_CREATE_ROLE = 2,	// 创建角色 	
	GAME_STATISTICS_LOGIN = 3,			// 登录
	GAME_STATISTICS_RECHARGE = 4,		// 充值
	GAME_STATISTICS_ONLINE = 5,			// 在线
};

// 服务器所属版本 server_belong_server_common
enum
{
	SERVER_BELONG_VERSION_COMMON = 1,	// 通用版本
	SERVER_BELONG_VERSION_996_BT = 2,	// 996BT版本
};

// 充值来源
enum
{
	RECHARGE_FROM_NORMAL = 1,	// 正常充值
	RECHARGE_FROM_TEST = 2,		// 测试充值
	RECHARGE_FROM_GM= 3,		// GM充值
};