
--[[
%% @module: 模块界面文件定义
%% @author: swordman sue
%% @created: 2016-07-10
--]]

BaseViewMgr = BaseViewMgr or BaseClass(EventAndTimerWrapper)

--界面类型
BaseViewType =
{
	INVALID = "INVALID",

	RES_UPDATE = "RES_UPDATE",							--资源更新
	UI_LOADING = "UI_LOADING",							--UI加载界面
	UI_LOADING_II = "UI_LOADING_II",					--UI加载界面
	SCENE_LOADING = "SCENE_LOADING",					--场景加载界面
	NET_LOADING = "NET_LOADING",						--网络加载界面
	NET_CONNECT_LOADING = "NET_CONNECT_LOADING",		--网络连接加载界面

	LOGIN = "LOGIN",									--登陆
	LOGIN_SERVER_LIST = "LOGIN_SERVER_LIST",			--选服

	MAIN_CITY = "MAIN_CITY",							--主界面
	BATTLE = "BATTLE",									--战斗
	BATTLE_CHASM = "BATTLE_CHASM",						--战斗(深渊)

	CHAT = "CHAT",										--聊天

	Pack = "Pack",							--物品背包

	ROLEINFO = "ROLEINFO",								--角色信息
	ROLENEW = "ROLENEW",								--角色创建
	ROLEUPGRADE = "ROLEUPGRADE",						--角色升级信息

	DUNGEON = "DUNGEON",								--副本
	DUNGEON_DAILY = "DUNGEON_DAILY",					--日常副本
	DUNGEON_GAMELEVEL = "DUNGEON_GAMELEVEL",			--副本关卡
	HERO_DUNGEON_GAMELEVEL = "HERO_DUNGEON_GAMELEVEL",	--英雄副本关卡
	ELITE_DUNGEON_GAMELEVEL = "ELITE_DUNGEON_GAMELEVEL",	--精英副本关卡
	
	EQUIP = "EQUIP", 								--装备
	EQUIP_DRESS = "EQUIP_DRESS",					--装备列表
	EQUIPINTENSIFY = "EQUIPINTENSIFY",				--装备强化
	EQUIPTRADE = "EQUIPTRADE", 						--更换装备
	EQUIP_UP_STAR = "EQUIP_UP_STAR",				--装备升星
	EQUIP_REMOULD = "EQUIP_REMOULD",			    --装备进化
	                         
	HERO_BAG = "HERO_BAG",         	            	--英雄背包
	HERO_LINEUP = "HERO_LINEUP",                    --英雄阵容
	HERO_PROMOTE = "HERO_PROMOTE",                  --英雄提升
	HERO_RECRUIT = "HERO_RECRUIT",					--英雄招募
	HERO_HANDBOOK = "HERO_HANDBOOK",				--英雄图鉴
	HERO_HANDBOOK_DETAIL = "HERO_HANDBOOK_DETAIL",	--英雄图鉴二层界面
	HERO_SHOP = "HERO_SHOP",						--英雄商店
	HERO_EVOLUTION = "HERO_EVOLUTION",				--英雄进化

	TREASURE = "TREASURE",							--圣器
	TREASURE_LIFT = "TREASURE_LIFT",		--圣器提升
	TREASURE_DRESS = "TREASURE_DRESS",				--圣器穿戴
	TREASURECRADE = "TREASURECRADE", 				--圣器切换
	TREASURDEVOUR = "TREASURDEVOUR",				--圣器吞吃
	TREASUREREMOULD = "TREASUREREMOULD",			--圣器进化

	DEBRISELL = "DebriSell",						--碎片出售

	SHOP = "SHOP",                          		--商店

	EMAIL = "EMAIL",                        		--邮件

	RANK = "RANK",                          		--排行榜

	HANDBOOK = "HANDBOOK",                     	    --图鉴

	FRIEND = "FRIEND",                        		--好友

	RECYCLE = "RECYCLE",							--回收

	RESOLVE_LST = "RESOLVE_LST",					--分解列表界面

	RECYCLE_LST = "RECYCLE_LST",					--重生列表界面

	REBEL = "REBEL",								--叛军
	REBELSHOP = "REBELSHOP",						--叛军商店

	TOWER = "TOWER",								--爬塔
	TOWERSHOP = "TOWERSHOP",						--爬塔

	MANOR = "MANOR" ,								--领地
	MANORHEROLST = "MANORHEROLST",					--领地巡逻英雄选择

	PLUNDEREDTREASURES = "PLUNDEREDTREASURES",		--夺宝
	PLUNDEREDTREASURESCHOOSE_ENEMYVIEW = "PLUNDEREDTREASURESCHOOSE_ENEMYVIEW",		--夺宝挑战对手
	QUICK_PLUNDEREDTREASURES = "QUICK_PLUNDEREDTREASURES",		--快速夺宝
	SPIRIT = "SPIRIT",								--精灵系统
	SPIRIT_HANDBOOK = "SPIRIT_HANDBOOK",			--精灵图鉴
	ARENA = "ARENA",								--竞技场

	UNION = "UNION",								--公会
	UNION_HALL = "UNION_HALL",						--公会大厅
	-- UNION_CONTRIBUTE = "UNION_CONTRIBUTE",			--贡献大厅
	UNION_SHOP = "UNION_SHOP",						--公会商店
	UNION_SKILL = "UNION_SKILL", 					--公会技能
	UNION_TRANSCRIPT = "UNION_TRANSCRIPT", 			--公会副本
	UNION_TRANSCRIPT_TWO = "UNION_TRANSCRIPT_TWO", 			--公会副本

	UNION_COMBAT = "UNION_COMBAT" , 				--军团战
	UNION_COMBAT_BATTLEGROUND = "UNION_COMBAT_BATTLEGROUND" ,	--军团

	VIP = "VIP",									--vip

	WORLDBOSS = "WORLDBOSS",						--世界boss

	ARENA_REPUTATIONSHOP = "ARENA_REPUTATIONSHOP",					--竞技场声望商店
	DALIYTASK_AND_ACHIEVEMENT = "DALIYTASK_AND_ACHIEVEMENT",		--日常任务与成就

	APPROACH_OF_ACHIEVING =  "APPROACH_OF_ACHIEVING",

	ACCEPT_PRIZE_CENTER = "ACCEPT_PRIZE_CENTER",	--领奖中心

	CHALLENGE = "CHALLENGE",						--挑战列表

	STORY = "STORY",								--剧情

	ACTIVITY = "ACTIVITY",							--活动签到

	DESENO_ACTIVITY = "DESENO_ACTIVITY" , 			--限时活动

	SEVENDAYS = "SEVENDAYS",						--七天
	RECOMMEND_LINEUP = "RECOMMEND_LINEUP",			--推荐阵容

	WORLD_HEART = "WORLD_HEART",					--世界之心

	SHOP_FAILARMY = "SHOP_FAILARMY",				--商店整合1

	CHARGE = "CHARGE",							--充值

	CHARGE_RANK = "CHARGE_RANK" , 				--充值排行

	HERO_ADVENT = "HERO_ADVENT",					--英雄降临

	PVP = "PVP",

	FIRST_RECHARGE = "FIRST_RECHARGE",				 -- 首充

	PVP_LOSE = "PVP_LOSE",							--pvp输了

	STRONGER = "STRONGER",							--我要变强

	MONSTER_STORMCASTLE = "MONSTER_STORMCASTLE",	--怪物攻城

	TITLE = "TITLE" ,								--称号

	CHASM = "CHASM" ,								--深渊

	CHASM_SWEEP = "CHASM_SWEEP" ,					--深渊扫荡

	CHASM_WIN = "CHASM_WIN" , 						--深渊结算

	CHASM_RANK = "CHASM_RANK",						--深渊排行榜

	CHASM_SHOP = "CHASM_SHOP" , 					--深渊商店

	LIMIT_DUNGEON = "LIMIT_DUNGEON", 				--限时副本汇总

	MORROW = "MORROW" , 							--次日

	KNIGHTHOOD = "KNIGHTHOOD" , 					--爵位

	SUPERDISCOUNTVIEW = "SUPERDISCOUNTVIEW" , 		--超值购

	DIRECT_BUY_SHOP = "DIRECT_BUY_SHOP",			--直购特惠

	DIRECT_SHOP = "DIRECT_SHOP" , 

	PUSH_SEVEN = "PUSH_SEVEN" ,					--七日登录 

	DIRECT_HOT_RECOMMENT = "DIRECT_HOT_RECOMMENT" , --精准推送

	TEAM = "TEAM",									--组队

	RANK_ACTIVITY = "RANK_ACTIVITY",				--冲榜活动

	SPECIAL_ACTIVITY = "SPECIAL_ACTIVITY",			--特殊活动

	GEM = "GEM",									--宝石

	LADDER = "LADDER",								--天梯

	LADDER_SEASON_AWARD_VIEW = "LADDER_SEASON_AWARD_VIEW" , --天梯赛季奖励

	LADDER_HONOR_VIEW = "LADDER_HONOR_VIEW" , 		--天梯赛季殿堂

	LADDER_LOWER_RESULT = "LADDER_LOWER_RESULT",	--天梯赛低段位战斗结算界面

	LADDER_SENIOR_RESULT = "LADDER_SENIOR_RESULT",	--天梯赛王者段位战斗结算界面

	TEAM_WORLD_MAP = "TEAM_WORLD_MAP",				--世界地图

	TEAM_GATHER_AND_BATTLE = "TEAM_GATHER_AND_BATTLE",	--组队采集和战斗

	TEAM_BOSS_BATTLE = "TEAM_BOSS_BATTLE",	--组队BOSS战斗

	RAW_GEMSTONE_SHOP = "RAW_GEMSTONE_SHOP",		--原石商店界面

	DIVINATION = "DIVINATION",		--占卜主界面

	DIVINATION_SCRYER = "DIVINATION_SCRYER" , 			--占星师

	DIVINATION_INTENSIFY = "DIVINATION_INTENSIFY" , 	--占星术卡牌提升

	DIVINATION_SHOP = "DIVINATION_SHOP" ,				--占星术商店

	DIVINATION_PACK = "DIVINATION_PACK" , 				--占星背包

	DIVINATION_DESTINY_SHOP = "DIVINATION_DESTINY_SHOP" , --占星术命运商店

	DIVINATION_COLLECT = "DIVINATION_COLLECT",			--占星术收藏界面

	DIVINATION_RANK = "DIVINATION_RANK",				--占星排行榜

	DIVINATION_BOOK = "DIVINATION_BOOK",				--占星 书

	DIVINATION_DRESS = "DIVINATION_DRESS" , 			--占星穿戴

	ONLINE_AWARD = "ONLINE_AWARD" , 				--在线奖励

	--节日
	HALLOWEEN = "HALLOWEEN",						--万圣节
	--推送天梯
	PUSH_LADDER = "PUSH_LADDER" , 					--天梯
	--定制
	CUSTOM = "CUSTOM",								--定制
	CALL_FRIENDS = "CALL_FRIENDS", 					--好友召唤
	
	FACEBOOK_SHARE_REWARD = "FACEBOOK_SHARE_REWARD",	--分享Facebook奖励

	--回馈
	FEEDBACK = "FEEDBACK" , 						--充值回馈	
	--女王
	QUEEN = "QUEEN" , 								--女王	
	--自定义超值购
	CUSTOM_PURCHASE = "CUSTOM_PURCHASE" ,			--自定义超值购

	FIVESTAR_HIGHPRAISE = "FIVESTAR_HIGHPRAISE",	--五星好评

	--转盘
	TURNTABLE = "TURNTABLE",	                    --转盘
	--联系客服
	TEMPL_SERVICE = "TEMPL_SERVICE" , 				--联系客服

	--助战
	ASSIST = "ASSIST" , 							--助战

	--水晶商店
	CRYSTAL_SHOP = "CRYSTAL_SHOP",					--水晶商店

	--世界等级任务
	PUSH_WORLDLEVEL = "PUSH_WORLDLEVEL",	--世界等级任务

	--积分赛
	INTEGRAL_RACE = "INTEGRAL_RACE",		--积分赛
	--积分赛荣耀商店
	INTEGRAL_GLORY_SHOP = "INTEGRAL_GLORY_SHOP",		--积分赛荣耀商店
	--荣耀角斗场
	GLORY_ARENA = "GLORY_ARENA",					--荣耀角斗场
	--援军助威
	REINFORCEMENTS_CHEER = "REINFORCEMENTS_CHEER",	--援军助威
	--审核主界面
	POLYTROPIC_MAINCITY = "POLYTROPIC_MAINCITY", --审核主界面

	--资源战
	RESOURCE_WAR = "RESOURCE_WAR" , 				--资源战
	--资源战
	RESOURCE_SHOP = "RESOURCE_SHOP" , 				--资源战

	--[[ 限时活动 --]]
	--圣诞节
	CHRISTMAS = "CHRISTMAS",						--圣诞节
	--春节
	SPRING_FESTIVAL = "SPRING_FESTIVAL",		    --春节
	--扭蛋机抽奖
	GASHAPONMACHINE = "GASHAPONMACHINE",		    --扭蛋机抽奖
	--阵营招募
	CAMP_RECRUIT = "CAMP_RECRUIT",		    		--阵营招募
	--端午节
	DRAGONBOAT_FESTIVAL = "DRAGONBOAT_FESTIVAL",	--端午节

	--[[ 战斗结算 ]] --胜利
	BATTLE_RESULT_WIN = "BATTLE_RESULT_WIN",			--战斗胜利
	BATTLE_RESULT_LOSE = "BATTLE_RESULT_LOSE",			--战斗失败
	SP_BATTLE_RESULT_WIN = "SP_BATTLE_RESULT_WIN",		--特殊战斗胜利
	SP_BATTLE_RESULT_LOSE = "SP_BATTLE_RESULT_LOSE",	--特殊战斗失败
	DAILY_DUNGEON_RESULT = "DAILY_DUNGEON_RESULT",	--日常副本
	HERO_DUNGEON_RESULT_WIN = "HERO_DUNGEON_RESULT_WIN",	--英雄副本
	ELITE_DUNGEON_RESULT_WIN = "ELITE_DUNGEON_RESULT_WIN",  --精英副本
	TOWER_COMMON_RESULT_WIN = "TOWER_COMMON_RESULT_WIN", 	--爬塔普通
	TOWER_ELITE_RESULT_WIN = "TOWER_ELITE_RESULT_WIN" , 	--爬塔精英
	REBEL_RESULT_WIN = "REBEL_RESULT_WIN" , 				--叛军副本
	MANOR_DARE_RESULT_WIN = "MANOR_DARE_RESULT_WIN" , 		--领地挑战
	PLUNDERED_TREASURES_RESULT_WIN = "PLUNDERED_TREASURES_RESULT_WIN"  ,	--夺宝副本
	ARENA_RESULT_WIN = "ARENA_RESULT_WIN" , 								--竞技场
	WORLD_BOSS_RESULT_WIN = "WORLD_BOSS_RESULT_WIN" , 		--世界boss
	INTEGRAL_RACE_RESULT_WIN = "INTEGRAL_RACE_RESULT_WIN" , --积分赛
	UNION_RESULT_WIN = "UNION_RESULT_WIN" , 				--工会战
	ATTACK_CITY_RESULT_WIN = "ATTACK_CITY_RESULT_WIN" , 	--怪物攻城
	TEAM_NORMAL_BATTLE_RESULT_WIN = "TEAM_NORMAL_BATTLE_RESULT_WIN" ,  		--组队战斗
	TEAM_BOSS_BATTLE_RESULT_WIN = "TEAM_BOSS_BATTLE_RESULT_WIN" ,           --组队boss
	DIVINATION_RESULT_WIN = "DIVINATION_RESULT_WIN" , 		--占卜战斗
	RESOURCE_WAR_RESULT_WIN = "RESOURCE_WAR_RESULT_WIN" , 	--资源战
	UNION_LOW_RESULT_WIN = "UNION_LOW_RESULT_WIN" , 		--军团低级
	UNION_TALL_RESULT_WIN = "UNION_TALL_RESULT_WIN" , 		--军团高级
	DAILY_DUNGEON_RESULT_WIN = "DAILY_DUNGEON_RESULT_WIN",	--日常副本

	--[[ 战斗结算 ]] --失败
	ARENA_RESULT_LOSE = "ARENA_RESULT_LOSE" , 								--竞技场
	PLUNDERED_TREASURES_RESULT_LOSE = "PLUNDERED_TREASURES_RESULT_LOSE" , 	--夺宝结算

}

--子界面类型
BaseViewSubType =
{
	--登陆
	Login = 
	{
		LOGIN = 1,			--登陆界面
		LOGIN_II = 2,		--登陆界面II
		CREATE_ROLE = 3,	--创角界面
	},

	--副本
	DUNGEON = 
	{
		MAIN = 1,			--主线副本(view_params：章节索引)
		HERO = 2,			--英雄副本
		ELITE = 3,			--精英副本
	},

	--英雄阵容
	HeroLineup = 
	{
		MAIN = 1,			--阵容界面(view_params: 阵位索引)
		SELECT = 2,			--选择英雄界面
		REINFORCEMENTS = 3,	--援军
	},

	--英雄提升
	HeroPromote = 
	{
		DETAIL = 1,			--详情界面
		UPGRADE = 2,		--升级界面
		BREAK = 3,			--进阶界面
		TRAIN = 4,			--培养界面
		POTENTIAL = 5,		--潜能界面
		AWAKEN = 6, 		--觉醒界面
	},	

	--排行榜
	Rank = 
	{
		BATTLE =1,
		LEVEL =2,
	},	
	
	--夺宝
	PlunderedTreasures = 
	{
		MAIN_VIEW =1,
		CHOOSE_ENEMY =2,
	},
}

--非全屏界面注册表
BaseViewMgr.NonFullScreenViewRegistry = 
{
	[BaseViewType.CHAT] = true,					--聊天

	[BaseViewType.ROLEINFO] = true,
	[BaseViewType.ROLEUPGRADE] = true,
	[BaseViewType.EMAIL] = true,
	[BaseViewType.ACCEPT_PRIZE_CENTER] = true,
	[BaseViewType.HERO_HANDBOOK_DETAIL] = true,
	[BaseViewType.STORY] = true,
	[BaseViewType.HERO_ADVENT] = true,
	[BaseViewType.STRONGER] = true,	
	[BaseViewType.FIRST_RECHARGE] = true,	
	[BaseViewType.TITLE] = true,	
	[BaseViewType.MORROW] = true,	

	[BaseViewType.BATTLE_RESULT_WIN] = true,
	[BaseViewType.BATTLE_RESULT_LOSE] = true,
	[BaseViewType.SP_BATTLE_RESULT_WIN] = true,
	[BaseViewType.SP_BATTLE_RESULT_LOSE] = true,
	[BaseViewType.PVP_LOSE] = true,

	[BaseViewType.SUPERDISCOUNTVIEW] = true,
	[BaseViewType.ONLINE_AWARD] = true,
	[BaseViewType.QUEEN] = true,

	[BaseViewType.CUSTOM_PURCHASE] = true,
	[BaseViewType.GASHAPONMACHINE] = true,
		
	--节日
	[BaseViewType.PUSH_LADDER] = true,
	[BaseViewType.CUSTOM] = true,
	[BaseViewType.PUSH_SEVEN] = true,
	[BaseViewType.FEEDBACK] = true,
	[BaseViewType.PUSH_WORLDLEVEL] = true,
	[BaseViewType.CAMP_RECRUIT] = true,
	
	--渠道
	[BaseViewType.CALL_FRIENDS] = true,
	[BaseViewType.FACEBOOK_SHARE_REWARD] = true,
	[BaseViewType.FIVESTAR_HIGHPRAISE] = true,	
	[BaseViewType.TEMPL_SERVICE] = true,

	[BaseViewType.CHARGE_RANK] = true ,			
}

--界面栈模式(默认为关闭模式)
BaseViewMgr.ViewStackMode = 
{
	--界面打开后，关闭所有栈底界面
	--界面关闭前，打开栈顶界面
	CLOSE = 1,

	--界面打开后，隐藏所有栈底界面
	--界面关闭前，显示栈顶界面
	VISIBLE = 2,
}

--界面栈过滤表
BaseViewMgr.ViewStackFilterMap = 
{
	[BaseViewType.RES_UPDATE] = true,
	[BaseViewType.UI_LOADING] = true,
	[BaseViewType.UI_LOADING_II] = true,
	[BaseViewType.SCENE_LOADING] = true,
	[BaseViewType.NET_LOADING] = true,	
	[BaseViewType.NET_CONNECT_LOADING] = true,	

	[BaseViewType.LOGIN] = true,
	[BaseViewType.LOGIN_SERVER_LIST] = true,

	[BaseViewType.STORY] = true,
	[BaseViewType.MAIN_CITY] = true,
	[BaseViewType.ROLEUPGRADE] = true,
	[BaseViewType.TREASURDEVOUR] = true,
	[BaseViewType.HERO_HANDBOOK_DETAIL] = true,
	--[BaseViewType.PLUNDEREDTREASURESCHOOSE_ENEMYVIEW] = true,

	[BaseViewType.BATTLE] = true,
	[BaseViewType.BATTLE_CHASM] = true,
	[BaseViewType.CHASM_WIN] = true,
	[BaseViewType.POLYTROPIC_MAINCITY] = true,

	[BaseViewType.DAILY_DUNGEON_RESULT_WIN] = true,
}

--子界面音效过滤表
BaseViewMgr.SubViewSoundFilterMap = 
{
}

--界面注册表
BaseViewMgr.ViewsRegistry =
{	
	[BaseViewType.RES_UPDATE] = 
	{
		class_name = "ResUpdateView", 
		file_list = {"modules/resupdate/resupdate_view"}
	},

	[BaseViewType.UI_LOADING] = 
	{
		class_name = "UILoadingView", 
		file_list = {"modules/loading/ui_loading_view"}
	},

	[BaseViewType.UI_LOADING_II] = 
	{
		class_name = "UILoadingViewII", 
		file_list = {"modules/loading/ui_loading_view", "modules/loading/ui_loading_view_ii"}
	},

	[BaseViewType.SCENE_LOADING] = 
	{
		class_name = "SceneLoadingView", 
		file_list = {"modules/loading/ui_loading_view", "modules/loading/scene_loading_view"}
	},

	[BaseViewType.NET_LOADING] = 
	{
		class_name = "NetLoadingView", 
		file_list = {"modules/loading/ui_loading_view", "modules/loading/net_loading_view"}
	},

	[BaseViewType.NET_CONNECT_LOADING] = 
	{
		class_name = "NetLoadingView", 
		file_list = {"modules/loading/ui_loading_view", "modules/loading/net_loading_view"}
	},

	[BaseViewType.LOGIN] = 
	{
		class_name = "LoginView", 
		file_list = 
		{
			"modules/login/login_view",
			"modules/login/templ_self_sdk_register",
		}
	},

	[BaseViewType.LOGIN_SERVER_LIST] = 
	{
		class_name = "LoginServerListView", 
		file_list = 
		{
			"modules/login/templ_serveritem",
			"modules/login/templ_serverzone",
			"modules/login/login_serverlist_view",
		}
	},

	[BaseViewType.MAIN_CITY] = 
	{
		class_name = "MaincityView",
		file_list = 
		{	
			"modules/maincity/maincity", 
			"modules/maincity/maincity_bg",
			"modules/maincity/maincity_hero_lineup_layer",
		}
	},

	[BaseViewType.BATTLE] = 
	{
		class_name = "BattleView", 
		file_list = 
		{
			"modules/battle/battle_view",
			"modules/battle/battle_view_sprite",
			"modules/battle/templ_hero_oper",			
			"modules/battle/templ_hero_oper_assist",
		}
	},	

	[BaseViewType.BATTLE_CHASM] = 
	{
		class_name = "BattleChasmView", 
		file_list = 
		{
			"modules/battle/battle_view",
			"modules/battle/battle_view_sprite",
			"modules/battle/templ_hero_oper",			
			"modules/battle/templ_hero_oper_assist",
			"modules/battle/chasm_battle_view",
		}
	},	

	[BaseViewType.CHAT] = 
	{
		class_name = "ChatView", 
		file_list = 
		{	
			"modules/chat/chat_view", 
			"modules/chat/templ_chat_list_layer",
			"modules/chat/templ_chat_face_layer",
		}
	},

	[BaseViewType.Pack] = 
	{
		class_name = "PackView", 
		file_list = 
		{
			"modules/beibao/pack_view",
			"modules/beibao/templ_pack_goods",
			"modules/beibao/templ_pack_awaken",

			"modules/hero/promote/templ_hero_awakenProp_handbookview",
			"modules/hero/promote/templ_hero_awakenProp_handbookcell",	
			"modules/hero/promote/templ_hero_awakenProp_view",
			"modules/hero/promote/templ_hero_awakenPropCompound_view",				
		}	
	},

	[BaseViewType.ROLEINFO] = 
	{
		class_name = "RoleInfoView", 
		file_list = 
		{
			"modules/role/role_info_view",
			"modules/role/role_info_tabview",
			"modules/role/setting_tabview",
			"modules/role/templ_role_facelift",
			"modules/role/templ_role_rename",
		}	
	},

	[BaseViewType.TEMPL_SERVICE] = 
	{
		class_name = "TemplService", 
		file_list = 
		{
			"modules/role/templ_service",
		}	
	},


	[BaseViewType.ROLEUPGRADE] = 
	{
		class_name = "RoleUpgradeView", 
		file_list = 
		{
			"modules/role/role_upgrade_view",	
		}	
	},				

	[BaseViewType.DUNGEON] = 
	{
		class_name = "DungeonView", 
		file_list = 
		{
			"modules/dungeon/main/templ_chapter",
			"modules/dungeon/hero/templ_hero_chapter",
			"modules/dungeon/main/main_dungeon_tabview",
			"modules/dungeon/hero/hero_dungeon_tabview",
			"modules/dungeon/elite/elite_dungeon_tabview",
			"modules/dungeon/dungeon_view",
			"modules/dungeon/templ_dungeon_message_cell",
			"modules/dungeon/hero/templ_hero_special_gamelevel_challenge",
			"modules/dungeon/elite/templ_elite_chapter",
			"modules/dungeon/elite/dungeon_external_area_view",
			"modules/dungeon/elite/dungeon_external_area_cell",
			"modules/dungeon/elite/dungeon_external_area_challenge_view",
			"modules/dungeon/templ_dungeon_add_assist_role",
		}	
	},	

	[BaseViewType.DUNGEON_GAMELEVEL] = 
	{
		class_name = "DungeonGameLevelView", 
		file_list = 
		{
			"modules/dungeon/templ_boxinfo",
			"modules/dungeon/templ_dungeon_add_assist_role",
			"modules/dungeon/main/templ_gamelevel_challenge",
			"modules/dungeon/templ_dungeon_message_cell",
			"modules/dungeon/main/templ_gamelevel",
			"modules/dungeon/main/dungeon_gamelevel_view",
			"modules/dungeon/clean/templ_dungeon_clean_result",
			"modules/dungeon/clean/templ_dungeon_clean_result_item",
			"modules/dungeon/clean/templ_dungeon_clean_result_total_item",
			"modules/dungeon/templ_dungeon_passedall_tip",
			"modules/dungeon/main/templ_message_board",
		}	
	},	

	[BaseViewType.HERO_DUNGEON_GAMELEVEL] = 
	{
		class_name = "HeroDungeonGameLevelView", 
		file_list = 
		{
			"modules/dungeon/templ_boxinfo",
			"modules/dungeon/hero/templ_hero_gamelevel_challenge",
			"modules/dungeon/main/templ_gamelevel",
			"modules/dungeon/hero/templ_hero_gamelevel",
			"modules/dungeon/hero/hero_dungeon_gamelevel_view",	
		}	
	},	

	[BaseViewType.ELITE_DUNGEON_GAMELEVEL] = 
	{
		class_name = "EliteDungeonGameLevelView", 
		file_list = 
		{
			"modules/dungeon/main/dungeon_gamelevel_view",
			"modules/dungeon/main/templ_gamelevel_challenge",
			"modules/dungeon/elite/elite_dungeon_gamelevel_view",
			"modules/dungeon/elite/templ_elite_gamelevel",
			"modules/dungeon/templ_boxinfo",
			"modules/dungeon/clean/templ_dungeon_clean_result",
			"modules/dungeon/clean/templ_dungeon_clean_result_item",
			"modules/dungeon/clean/templ_dungeon_clean_result_total_item",
			"modules/dungeon/templ_dungeon_passedall_tip",
		}	
	},

	[BaseViewType.DUNGEON_DAILY] = 
	{
		class_name = "DailyDungeonView", 
		file_list = 
		{
			"modules/dungeon/daily/daily_view",
			"modules/dungeon/daily/templ_daily_page",
			"modules/dungeon/daily/templ_dailydungeon_passed_newdifficultlevel_view",
			"modules/dungeon/clean/templ_dungeon_clean_result",
			"modules/dungeon/clean/templ_dungeon_clean_result_item",
			"modules/dungeon/clean/templ_dungeon_clean_result_total_item",
		}	
	},	

	[BaseViewType.EQUIP] = 
	{
		class_name = "EquipView", 
		file_list = 
		{
			"modules/equip/equipmain/equip_view",					--主界面
			"modules/equip/equipmain/equip_pack_goods",			--装备界面
			"modules/equip/equipmain/equip_pack_debris",			--碎片界面
		}			
	},

	[BaseViewType.EQUIP_DRESS] = 
	{
		class_name = "EquipCrossListView", 
		file_list = 
		{
			"modules/equip/equipcross/equip_cross_list_view",		--穿戴装备列表
		}			
	},

	[BaseViewType.EQUIPTRADE] = 
	{
		class_name = "EquipTradeView", 
		file_list = 
		{
			"modules/equip/equipcross/equip_trade",		--穿戴更改列表
		}			
	},

	[BaseViewType.EQUIPINTENSIFY] = 
	{
		class_name = "EquipLiftView", 
		file_list = 
		{
			"modules/equip/equipIntensify/equip_liftview",				--装备提升界面
			"modules/equip/equipIntensify/equip_lift_info_tab_view",			--装备信息			
			"modules/equip/equipIntensify/equip_lift_intensify_tab_view",		--装备强化
			"modules/equip/equipIntensify/equip_lift_refine_tab_view",			--装备精炼
			"modules/equip/equipIntensify/equip_font_effects/equipment_font_effects",	--装备特效
			"modules/equip/equipIntensify/templ_equip_onekey_refine",			--一键精炼
		}			
	},

	[BaseViewType.EQUIP_UP_STAR] = 
	{
		class_name = "EquipUpSatrView", 
		file_list = 
		{
			"modules/equip/equipIntensify/equip_upsatrview",				--装备提升界面
		}			
	},

	[BaseViewType.EQUIP_REMOULD] = 
	{
		class_name = "EquipRemouldView", 
		file_list = 
		{
			"modules/equip/equipIntensify/temp_equip_remould",				--装备进化界面
		}			
	},

	[BaseViewType.TREASURE] = 
	{
		class_name = "TreasureView", 
		file_list = 
		{
			"modules/treasure/treasure_main/treasure_view",				--圣器主界面
			"modules/treasure/treasure_main/treasure_goods",
		}			
	},	
	
	[BaseViewType.TREASURE_LIFT] = 
	{
		class_name = "TreasureLiftView", 
		file_list = 
		{
			"modules/treasure/treasure_lift/treasure_liftview",				--圣器提升界面
			"modules/treasure/treasure_lift/treasure_lift_info_tabview",	--圣器信息
			"modules/treasure/treasure_lift/treasure_lift_intensify_tabview",	--圣器强化
			"modules/treasure/treasure_lift/treasure_lift_refine_tabview",		--圣器精炼	
			"modules/treasure/treasure_lift/treasure_font_effects/treasure_font_effects",	--圣器特效		
			"modules/treasure/treasure_lift/templ_treasure_onekey_intensify",				--圣器一键强化						
		}			
	},	
	[BaseViewType.TREASURE_DRESS] = 
	{
		class_name = "TreasureCrossListView", 
		file_list = 
		{
			"modules/treasure/treasure_cross/treasure_cross_list_view",		--圣器穿戴列表
		}	
	},

	[BaseViewType.TREASURECRADE] = 
	{
		class_name = "TreasureTradeView", 
		file_list = 
		{
			"modules/treasure/treasure_cross/treasure_trade",		--穿戴更改列表
		}			
	},
	[BaseViewType.TREASURDEVOUR] = 
	{
		class_name = "TreasureDevour", 
		file_list = 
		{
			"modules/treasure/treasure_lift/treasure_material_lst",		--材料选择列表
		}			
	},
	[BaseViewType.TREASUREREMOULD] = 
	{
		class_name = "TreasureRemouldView", 
		file_list = 
		{
			"modules/treasure/treasure_lift/templ_treasure_remould",		--圣器进化
		}			
	},
	[BaseViewType.SHOP] = 
	{
		class_name = "ShopView", 
		file_list = 
		{
			"modules/shop/shop_view",
			"modules/shop/templ_shop_all_cell",
			"modules/shop/shop_buy_count_view",
		}			
	},
	
	[BaseViewType.EMAIL] = 
	{
		class_name = "EmailView", 
		file_list = 
		{
			"modules/email/email_view",
			"modules/email/templ_emailcell",	
			"modules/email/templ_email_reply",		
		}			
	},

	[BaseViewType.RANK] = 
	{
		class_name = "RankView", 
		file_list = 
		{
			"modules/rank/rank_view",
			"modules/rank/templ_rank_cell",		
		}			
	},

	[BaseViewType.FRIEND] = 
	{
		class_name = "FriendView", 
		file_list = 
		{
			"modules/friend/friend_view",
			"modules/friend/templ_all_friend_cell",
		}			
	},
	
 	[BaseViewType.HANDBOOK] = 
	{
		class_name = "HandBookView", 
		file_list = 
		{
			"modules/handbook/handbook_view",
				
		}			
	},


	[BaseViewType.HERO_BAG] = 
	{
		class_name = "HeroBagView", 
		file_list = 
		{
			"modules/hero/bag/hero_bag_view",
			"modules/hero/bag/hero_bag_goods",		--英雄列表
			"modules/hero/bag/hero_bag_debris",		--英雄碎片列表
		}			
	},	

	[BaseViewType.HERO_LINEUP] = 
	{
		class_name = "HeroLineupView", 
		file_list = 
		{
			"lgui/hero/templ_hero_lineup_info",	
			"lgui/hero/templ_hero_divination_info",		
			"modules/hero/lineup/hero_lineup_view",
			"modules/hero/lineup/hero_lineup_item",
			"modules/hero/lineup/templ_hero_lineup_item",
			"modules/hero/lineup/templ_hero_reinforcements",
			"modules/hero/lineup/templ_hero_reinforcementsinfo",
			"modules/hero/lineup/templ_reinforcementscell",
			"modules/hero/lineup/templ_hero_forging",
		}			
	},	

	[BaseViewType.HERO_PROMOTE] = 
	{
		class_name = "HeroPromoteView", 
		file_list = 
		{
			"modules/hero/promote/templ_hero_break_succ",		
			"modules/hero/promote/templ_hero_potential_succ",		
			"modules/hero/promote/templ_hero_train_attr_ball",					
			"modules/hero/promote/hero_promote_view",
			"modules/hero/promote/hero_detail_tabview",			
			"modules/hero/promote/hero_upgrade_tabview",
			"modules/hero/promote/hero_upgrade_role_tabview",
			"modules/hero/promote/hero_upgrade_material_view",
			"modules/hero/promote/hero_break_tabview",
			"modules/hero/promote/hero_break_material_view",
			"modules/hero/promote/hero_train_tabview",
			"modules/hero/promote/hero_potential_tabview",
			"modules/hero/promote/hero_awaken_tabview",
			"modules/hero/promote/templ_hero_awakenProp_view",
			"modules/hero/promote/templ_hero_awakenPropCompound_view",
			"modules/hero/promote/templ_hero_awakenProp_preLookview",
			"modules/hero/promote/templ_hero_awakenProp_preLookcell",	
			"modules/hero/promote/templ_hero_awakenProp_handbookview",
			"modules/hero/promote/templ_hero_awakenProp_handbookcell",		
			"modules/hero/promote/templ_hero_train_veiw",	
			"modules/hero/promote/templ_hero_train_preLookview",	
			"modules/hero/promote/templ_hero_train_preLookcell",	
			"modules/hero/promote/templ_hero_train_times_chooseveiw",
			"modules/hero/lineup/templ_hero_lineup_item",
			"modules/hero/promote/templ_hero_train_break_reward_view",
			"modules/hero/promote/templ_hero_awaken_succ",
			"modules/hero/promote/templ_hero_onekey_levelup_tips_one",
			"modules/hero/promote/templ_hero_onekey_levelup_tips_two",
		}			
	},	
	
	[BaseViewType.REBEL] = 
	{
		class_name = "RebelView", 
		file_list = 
		{
			"modules/rebel/rebel_view",
			"modules/rebel/rebel_shop/templ_rebel_shop",
			"modules/rebel/rebel_combat/templ_rebel_combat",
			"modules/rebel/rebel_award/templ_rebel_award",
			"modules/rebel/rebel_rank/templ_rebel_rank",
		}	

	},

	[BaseViewType.REBELSHOP] = 
	{
		class_name = "RebelShopView", 
		file_list = 
		{
			"modules/rebel/rebel_shop/rebel_shop_view",
		}	

	},

	[BaseViewType.RECYCLE] = 
	{
		class_name = "RecycleView", 
		file_list = 
		{
			"modules/recycle/recycle_view",
			"modules/recycle/templ_recycle_resolve",
			"modules/recycle/templ_recycle_rebirth",
			"modules/recycle/recycle_get_info_layer",

			--分解
			"modules/recycle/recycle_resolve/templ_hero_resolve",
			"modules/recycle/recycle_resolve/templ_equip_resolve",
			"modules/recycle/recycle_resolve/templ_card_resolve",
			--重生
			"modules/recycle/recycle_rebirth/templ_hero_rebirth",
			"modules/recycle/recycle_rebirth/templ_equip_rebirth",
			"modules/recycle/recycle_rebirth/templ_treasure_rebirth",
			"modules/recycle/recycle_rebirth/templ_card_rebirth",
		}			
	},
	[BaseViewType.RESOLVE_LST] = 
	{
		class_name = "ResolveLstView", 
		file_list = 
		{
			"modules/recycle/resolve_lst_view",
		}			
	},		
	[BaseViewType.RECYCLE_LST] = 
	{
		class_name = "RecycleLstView", 
		file_list = 
		{
			"modules/recycle/recycle_lst_view",
		}			
	},	
	[BaseViewType.HERO_RECRUIT] = 
	{
		class_name = "HeroRecruitView", 
		file_list = 
		{
			"modules/hero/recruit/hero_recruit_view",
			"modules/hero/recruit/templ_recruit_prelookview",
			"modules/hero/recruit/templ_recruit_tencallview",
			"modules/hero/recruit/templ_recruit_tencallviewcell",			
			"modules/hero/recruit/templ_vip_recruit_prelookview",	
		}			
	},
	[BaseViewType.HERO_HANDBOOK] = 
	{
		class_name = "HeroHandBookMainView", 
		file_list = 
		{
			"modules/hero/handbook/hero_handbook_mainview",
			"modules/hero/handbook/templ_handbook_heroicon",
			"modules/hero/handbook/templ_handbook_mainview",

		}			
	},
	[BaseViewType.TOWER] = 
	{
		class_name = "TowerView", 
		file_list = 
		{
			"modules/tower/tower_view",
			"modules/tower/tower_onview",
			"modules/tower/tower_setview",
			"modules/tower/temp_tower_monster",
			"modules/tower/temp_tower_property",
			"modules/tower/tower_rank/temp_tower_rank",
			"modules/tower/tower_award/temp_tower_award",
			"modules/tower/tower_difficulty_select/templ_tower_difficulty_select",
			"modules/tower/tower_elite/templ_tower_elite",
			"modules/tower/tower_treasure/temp_tower_treasure",
			"modules/tower/tower_buff/templ_tower_buff",
			"modules/tower/tower_buff/templ_tower_boxinfo",
			"modules/tower/tower_sweep/templ_tower_sweep",
			"modules/tower/tower_reset/temp_tower_reset",
		}
	},
	[BaseViewType.TOWERSHOP] = 
	{
		class_name = "TowerShop", 
		file_list = 
		{
			"modules/tower/tower_shop/tower_shop",
		}	
	},
	[BaseViewType.HERO_HANDBOOK_DETAIL] = 
	{
		class_name = "HeroHandBookDetailView", 
		file_list = 
		{
			"modules/hero/handbook/hero_handbook_detailview",
			"modules/hero/handbook/templ_hero_hanbook_heromessageview",
			"modules/hero/handbook/templ_hero_hanbook_heromessageviewcell",
			"modules/hero/handbook/templ_hero_hanbook_herochipview",
			"modules/hero/handbook/templ_hero_hanbook_herotogetherattack",
			"modules/hero/handbook/templ_hero_hanbook_herofetterview",
			"modules/hero/handbook/templ_hero_hanbook_herofetterviewcell",

		}			
	},
	[BaseViewType.MANOR] = 
	{
		class_name = "ManorView", 
		file_list = 
		{
			"modules/manor/manor_view",
			--城市
			"modules/manor/manor_city",
			--好友
			"modules/manor/manor_friend_view",
			-- --领地技能
			"modules/manor/manor_skill/temp_manor_skill",
			--领地挑战
			"modules/manor/manor_manor/temp_manor_challenge",
			--领地添加选择英雄
			"modules/manor/manor_manor/manor_addHeroLst",
			--城池详情
			"modules/manor/manor_manor/temp_manor_particulars",
			--领地巡逻
			"modules/manor/manor_manor/templ_manor_patrol",
			--领地奖励
			"modules/manor/manor_manor/temp_manor_double_award",
			--领地巡逻中
			"modules/manor/manor_manor/temp_manor_observe",
		}	
	},
	[BaseViewType.MANORHEROLST] = 
	{
		class_name = "ManorAddHeroLst", 
		file_list = 
		{
			--领地添加选择英雄
			"modules/manor/manor_manor/manor_addHeroLst",
		}
	},	
	[BaseViewType.SPIRIT] = 
	{
		class_name = "SpiritView", 
		file_list = 
		{
			"modules/spirit/spirit_view",
			"modules/spirit/templ_spirit_onekey_train",
			"modules/spirit/templ_spirit_sublimation_view",
			"modules/spirit/templ_spirit_sublimation_succ",
			"modules/spirit/templ_spirit_starup_succ",
		}			
	},
	[BaseViewType.SPIRIT_HANDBOOK] = 
	{
		class_name = "SpiritHandBookView", 
		file_list = 
		{
			"modules/spirit/spirit_handbook_view",
			"modules/spirit/templ_spirit_handbook_cell",
			"modules/spirit/templ_spirit_intensify_master_view",
		}			
	},
	[BaseViewType.PLUNDEREDTREASURES] = 
	{
		class_name = "PlunderedTreasuresView", 
		file_list = 
		{
			"lgui/templ_choosecard",
			"modules/plunderedtreasures/plunderedtreasures_view",
			"modules/plunderedtreasures/templ_plundered_treasureList_cell",			
			"modules/plunderedtreasures/templ_plundered_treasures_reportview",
			"modules/plunderedtreasures/templ_plundered_treasures_reportcell",
			"modules/plunderedtreasures/templ_plundered_treasures_avoidwarview",
			"modules/plunderedtreasures/templ_plundered_treasures_buydaojuview",
			"modules/plunderedtreasures/templ_plundered_treasures_relievestate_tipview",	
			"modules/plunderedtreasures/templ_plundered_treasures_onekey_compose",	
			"modules/plunderedtreasures/templ_plundered_treasures_onekey_tips",	
			"modules/plunderedtreasures/templ_plundered_treasures_onekey_view",
			"modules/plunderedtreasures/templ_plundered_treasures_onekey_cell",
			"modules/plunderedtreasures/templ_plundered_treasures_reward",
		}			
	},
	[BaseViewType.PLUNDEREDTREASURESCHOOSE_ENEMYVIEW] = 
	{
		class_name = "PlunderedTreasuresChooseenemyView", 
		file_list = 
		{
			"lgui/templ_choosecard",
			"modules/plunderedtreasures/plunderedtreasures_chooseenemyview",
			"modules/plunderedtreasures/templ_plunderedtreasures_chooseenemy",			
		}			
	},
	[BaseViewType.QUICK_PLUNDEREDTREASURES] = 
	{
		class_name = "QuickPlunderedTreasuresView", 
		file_list = 
		{
			"modules/plunderedtreasures/quick_plunderedtreasures_view",
			"modules/plunderedtreasures/templ_quick_plunderedtreasures_cell",			
		}			
	},		
	[BaseViewType.ARENA] = 
	{
		class_name = "ArenaView", 
		file_list = 
		{
			"lgui/templ_choosecard",		
			"modules/arena/arena_view",		
			"modules/arena/templ_arena_heromessagecell",	
			"modules/arena/arena_fightreportview",
			"modules/arena/arena_fightreportcell",	
			"modules/arena/arena_rankview",	
			"modules/arena/arena_rank_encouragecell",
			"modules/arena/arena_rankcell",	
			"modules/arena/templ_arena_rank_breakview",
			"modules/arena/templ_arena_fightfivetimesview",
			"modules/arena/templ_arena_fightfivetimescell",
		}			
	},		
	[BaseViewType.ARENA_REPUTATIONSHOP] = 
	{
		class_name = "ArenaReputationShopView", 
		file_list = 
		{
			"modules/arena/arena_reputationshopview",
			"modules/arena/templ_arena_reputationshopcell",	
			"modules/arena/templ_arena_reputationshopbuyview",		
		}			
	},
	[BaseViewType.UNION] = 
	{
		class_name = "UnionView", 
		file_list = 
		{
			"modules/union/union_view",
			"modules/union/union_hall/templ_union_compile",

			--留言
			"modules/union/templ_union_leave_word/templ_union_leave_word",
			--排行榜
			"modules/union/templ_union_rank/templ_union_rank",
			--搜索
			-- "modules/union/templ_union_join/templ_union_join",
			--建设
			"modules/union/union_contribute/union_contribute_view",
			--输入框
			"modules/union/union_hall/templ_union_import",
		}	
	},

	[BaseViewType.UNION_COMBAT] = 
	{
		class_name = "UnionCombatView", 
		file_list = 
		{
			"modules/union/union_combat/union_combat_view",
			"modules/union/union_combat/union_combat_popup/temp_union_combat_star",
			"modules/union/union_combat/union_combat_popup/temp_union_combat_starinfo",

			--宣战 or 积分
			"modules/union/union_combat/union_combat_popup/temp_union_combat_declarewar",
			--我的宣战
			"modules/union/union_combat/union_combat_popup/temp_union_combat_mydeclarewar",
			--次数购买
			"modules/union/union_combat/union_combat_popup/temp_union_combat_buy",
			--防御塔界面
			"modules/union/union_combat/union_combat_popup/temp_union_combat_defense",
		}	
	},

	[BaseViewType.UNION_COMBAT_BATTLEGROUND] = 
	{
		class_name = "UnionCombatBattlegroundView", 
		file_list = 
		{
			"modules/union/union_combat/union_combat_battleground_view",
		}	
	},

	[BaseViewType.UNION_HALL] = 
	{
		class_name = "UnionHallView", 
		file_list = 
		{
			--主页面
			"modules/union/union_hall/union_hall_view",
			--大厅
			"modules/union/union_hall/union_hall_hall",
			--列表
			"modules/union/union_hall/union_hall_list",
			--动态
			"modules/union/union_hall/union_hall_dynamic",

			--军团图标
			"modules/union/union_hall/templ_union_icon_wall",
			--审核列表
			"modules/union/union_hall/templ_union_audit",
		}	
	},

	[BaseViewType.UNION_SHOP] = 
	{
		class_name = "UnionShopView", 
		file_list = 
		{
			"modules/union/union_shop/union_shop",
		}	
	},

	[BaseViewType.UNION_SKILL] = 
	{
		class_name = "UnionSkillView", 
		file_list = 
		{
			"modules/union/union_skill/union_skill_view",
		}	
	},

	[BaseViewType.UNION_TRANSCRIPT] = 
	{
		class_name = "UnionTranscriptView", 
		file_list = 
		{
			"modules/union/union_transcript/union_transcript",
			"modules/union/union_transcript/templ_union_transcript_item",
			"modules/union/union_transcript/templ_union_combat",
			"modules/union/union_transcript/templ_union_playeraward",
			"modules/union/union_transcript/templ_union_buytime",
			"modules/union/union_transcript/templ_union_transcript_record",
			"modules/union/union_transcript/templ_union_transcript_reset",
		}
	},

	[BaseViewType.UNION_TRANSCRIPT_TWO] = 
	{
		class_name = "UnionTranscriptTwoView", 
		file_list = 
		{
			"modules/union/union_transcript/union_transcript_two",
			"modules/union/union_transcript/templ_union_transcript_box",
		}
	},
	

	[BaseViewType.VIP] = 
	{
		class_name = "VIPView", 
		file_list = 
		{
			"modules/vip/vip_view",
			"modules/vip/templ_vip_item",
			"modules/shop/shop_buy_count_view",
		}	
	},
	
	[BaseViewType.HERO_SHOP] = 
	{
		class_name = "HeroShopView", 
		file_list = 
		{
			"modules/heroshop/heroshop_view",
			"modules/heroshop/templ_hero_shop_item",
		}	
	},
	
	[BaseViewType.DALIYTASK_AND_ACHIEVEMENT] = 
	{
		class_name = "DaliytaskAndAchievementView", 
		file_list = 
		{
			"modules/daliytaskandachievement/daliytask_and_achievement_view",
			"modules/daliytaskandachievement/templ_daliytask_cell",
			"modules/daliytaskandachievement/award_layer",
			"modules/daliytaskandachievement/templ_achievement_cell",
		}	
	},

	[BaseViewType.DEBRISELL] = 
	{
		class_name = "DebriSellView", 
		file_list = 
		{
			"modules/debrisell/debrisell_view",
		}	
	},

	[BaseViewType.WORLDBOSS] = 
	{
		class_name = "WorldBossView", 
		file_list = 
		{
			"modules/worldboss/worldboss_view",
			--势力弹框
			"modules/worldboss/templ_worldboss_group",
			--奖励
			"modules/worldboss/templ_worldboss_award",
			--排行榜
			"modules/worldboss/templ_worldboss_honor",	
			--战报	
			"modules/worldboss/templ_worldboss_score",	
			--次数购买
			"modules/worldboss/templ_worldboss_buytime",		
		}	
	},

	[BaseViewType.ACCEPT_PRIZE_CENTER] = 
	{
		class_name = "AcceptPrizeCenterView", 
		file_list = 
		{
			"modules/accepttheprizecenter/accept_prize_center_view",
			--领奖条目
			"modules/accepttheprizecenter/templ_accept_prize_center_cell",					
		}	
	},

	[BaseViewType.CHALLENGE] = 
	{
		class_name = "ChallengeView", 
		file_list = 
		{
			"modules/challenge/challenge_view",
		}	
	},	

	[BaseViewType.STORY] = 
	{
		class_name = "StoryView",
		file_list = 
		{
			"modules/story/story_view",
		}	
	},

	[BaseViewType.ACTIVITY] = 
	{
		class_name = "ActivityView",
		file_list = 
		{
			"modules/activity/activity_view",

			"modules/activity/medicine/templ_medicine",
			
			"modules/activity/daily_signin/templ_daily_signin",

			"modules/activity/fairymoney/templ_fairymoney",

			"modules/activity/levelshop/templ_levelshop",

			"modules/activity/levelgiftbag/templ_levelgiftbag",		

			"modules/activity/cdkey/templ_cdkey",		

			"modules/activity/fund/templ_fund",	

			"modules/activity/welfare/templ_welfare",	

			"modules/activity/member/templ_member",		

			"modules/activity/recharge_welfare/templ_recharge_welfare",	

			"modules/activity/justfull/templ_justfull",
			
			"modules/activity/register_award/templ_register_award",

		}	
	},

	[BaseViewType.DESENO_ACTIVITY] = 
	{
		class_name = "DesenoActivityView",
		file_list = 
		{
			"modules/activity/desenoactivity_view",
			
			"modules/activity/one_recharge/templ_one_recharge",	

			"modules/activity/grand_total_recharge/templ_grand_total_recharge",

			"modules/activity/summon/templ_summon",
			
			"modules/activity/justfull/templ_justfull",
		}	
	},

	[BaseViewType.SPECIAL_ACTIVITY] = 
	{
		class_name = "SpecialActivityView",
		file_list = 
		{
			"modules/activity/special/special_view",
			"modules/activity/special/templ_special",
			"modules/activity/special/templ_special_1",
		}	
	},

	[BaseViewType.FIRST_RECHARGE] = 
	{
		class_name = "FirstRechargeView",
		file_list = 
		{
			--首充
			"modules/push/first_recharge/first_recharge_view",	
		}	
	},

	[BaseViewType.SEVENDAYS] = 
	{
		class_name = "SevenDaysView",
		file_list = 
		{
			"modules/activity/sevendays/seven_days_view",
			"modules/activity/sevendays/templ_activity_sevenDays",
		}	
	},

	[BaseViewType.RANK_ACTIVITY] = 
	{
		class_name = "RankActivityView",
		file_list = 
		{
			"modules/activity/rank_activity/rank_activity_view",
			"modules/arena/arena_rankview",	
			"modules/arena/arena_rank_encouragecell",
			"modules/arena/arena_rankcell",	
		}	
	},

	[BaseViewType.RECOMMEND_LINEUP] = 
	{
		class_name = "RecommendLineupView",
		file_list = 
		{
			"modules/recommendlineup/recommend_lineup_view",
			"modules/recommendlineup/recommend_lineup/templ_recommend_lineup_page",
			"modules/recommendlineup/system_foreshow/templ_system_foreshow_scrollitem",
		}	
	},

	[BaseViewType.WORLD_HEART] = 
	{
		class_name = "WorldHeartView",
		file_list = 
		{
			"modules/worldheart/worldheart_view",
			"modules/worldheart/templ_worldheart_lightup_view",
			"modules/worldheart/templ_worldheart_propertyview",
			"modules/worldheart/templ_spirit_introduce_view",
		}	
	},

	[BaseViewType.SHOP_FAILARMY] = 
	{
		class_name = "ShopFailarmyView",
		file_list = 
		{
			"modules/shop_failarmy/shop_failarmy_view",
		}	
	},

	[BaseViewType.CHARGE] = 
	{
		class_name = "ChargeView",
		file_list = 
		{
			"modules/charge/charge_view",
		}	
	},

	--充值排行榜
	[BaseViewType.CHARGE_RANK] = 
	{
		class_name = "ChargeRankView",
		file_list = 
		{
			"modules/charge/charge_rank_view", 
		}	
	},

	[BaseViewType.HERO_ADVENT] = 
	{
		class_name = "HeroAdventView",
		file_list = 
		{
			"modules/heroadvent/heroadvent_view",
		}	
	},

	[BaseViewType.PVP] = 
	{
		class_name = "PVPView",
		file_list = 
		{
			"modules/pvp/pvp_view",
			"modules/pvp/templ_pvp_rank",
			"modules/pvp/templ_pvp_award",
			"modules/pvp/templ_pvp_challenge",
		}	
	},	

	[BaseViewType.PVP_LOSE] = 
	{
		class_name = "PVPLoseView",
		file_list = 
		{
			"modules/pvp/pvp_battle_result_lose_view",
		}	
	},	

	[BaseViewType.STRONGER] = 
	{
		class_name = "StrongerView",
		file_list = 
		{
			"modules/stronger/stronger_view",
			"modules/stronger/templ_stronger_system_cell",
		}	
	},

	[BaseViewType.MONSTER_STORMCASTLE] = 
	{
		class_name = "MonsterStormCastleView",
		file_list = 
		{
			"modules/monsterstormcastle/monster_stormcastle_view",
			"modules/monsterstormcastle/templ_monster_stormcastle_encourage",
			"modules/monsterstormcastle/templ_monster_stormcastle_reward_view",
			"modules/monsterstormcastle/templ_monster_stormcastle_reward_cell",
			"modules/monsterstormcastle/templ_monster_stormcastle_intergral_rank_view",
			"modules/monsterstormcastle/templ_monster_stormcastle_intergral_rank_cell",
			"modules/monsterstormcastle/templ_monster_stormcastle_rank_cell",
		}	
	},	
	--称号
	[BaseViewType.TITLE] = 
	{
		class_name = "TitleView",
		file_list = 
		{
			--首充
			"modules/title/title_view",	
		}	
	},
	--深渊
	[BaseViewType.CHASM] = 
	{
		class_name = "ChasmView",
		file_list = 
		{
			--深渊
			"modules/chasm/chasm_view",	
			"modules/chasm/templ_chasm_affirm_tips",	
		}	
	},
	--深渊扫荡
	[BaseViewType.CHASM_SWEEP] = 
	{
		class_name = "ChasmSweepView",
		file_list = 
		{
			--深渊扫荡
			"modules/chasm/chasm_sweep_view",	
		}	
	},
	--深渊胜利
	[BaseViewType.CHASM_WIN] = 
	{
		class_name = "ChasmWinView",
		file_list = 
		{
			"modules/chasm/chasm_battle_result_win_view",
		}	
	},	
	--深渊排行榜
	[BaseViewType.CHASM_RANK] = 
	{
		class_name = "ChasmRankView",
		file_list = 
		{
			"modules/chasm/chasm_rank/chasm_rank_view",
		}	
	},	
	--深渊商店
	[BaseViewType.CHASM_SHOP] = 
	{
		class_name = "ChasmShopView",
		file_list = 
		{
			"modules/chasm/chasm_shop/chasm_shop_view",
		}	
	},	
	--限时副本汇总
	[BaseViewType.LIMIT_DUNGEON] = 
	{
		class_name = "LimitDungeonView",
		file_list = 
		{
			"modules/limit_dungeon/limit_dungeon_view",	
		}	
	},	
	--次日
	[BaseViewType.MORROW] = 
	{
		class_name = "MorrowView",
		file_list = 
		{
			--次日
			"modules/push/morrow/morrow_view", 
		}	
	},	
	--天梯
	[BaseViewType.PUSH_LADDER] = 
	{
		class_name = "PushLadderView",
		file_list = 
		{
			--天梯
			"modules/push/push_ladder/push_ladder_view", 
		}	
	},	

	[BaseViewType.KNIGHTHOOD] = 
	{
		class_name = "KnighthoodView",
		file_list = 
		{
			--限时招募
			"modules/knighthood/knighthood_view", 
		}	
	},	
	--超值购
	[BaseViewType.SUPERDISCOUNTVIEW] = 
	{
		class_name = "SuperDiscountView",
		file_list = 
		{
			--超值购
			"modules/superdiscount/superdiscount_view", 
			"modules/superdiscount/temp_superdiscount_award", 
		}	
	},	

	--自定义超值购
	[BaseViewType.CUSTOM_PURCHASE] = 
	{
		class_name = "CustompurchaseView",
		file_list = 
		{
			--超值购
			"modules/custompurchase/custompurchase_view", 
		}	
	},	

	--直购特惠
	[BaseViewType.DIRECT_BUY_SHOP] = 
	{
		class_name = "DirectBuyShopView",
		file_list = 
		{
			"modules/directbuyshop/direct_buy_shop_view",
			"modules/directbuyshop/direct_buy_shop_tabview",
			"modules/directbuyshop/direct_buy_convert_tabview",
			"modules/directbuyshop/direct_buy_hot_recomment_tabview",	
			"modules/directbuyshop/direct_buy_hot_tabview",		
		}
	},
	--精准推送
	[BaseViewType.DIRECT_HOT_RECOMMENT] = 
	{
		class_name = "DirectBuyHotRecommentTabview",
		file_list = 
		{
			"modules/directbuyshop/direct_buy_hot_recomment_tabview",
		}
	},
	--七日登录
	[BaseViewType.PUSH_SEVEN] = 
	{
		class_name = "PushSevenView",
		file_list = 
		{
			"modules/push/push_seven/push_seven_view",
		}
	},

	--组队副本
	[BaseViewType.TEAM] = 
	{
		class_name = "TeamView",
		file_list = 
		{
			"modules/team/team_view",
			"modules/team/templ_create_the_team",
			"modules/team/templ_invite_teammate_view",
			"modules/team/templ_invite_teammate_cell",
			"modules/team/templ_team_gloryrank_view",
			"modules/team/templ_team_lottery_reward_prelook",
			"modules/team/templ_team_lottery_reward_cell",
		}
	},	

	--宝石
	[BaseViewType.GEM] = 
	{
		class_name = "GemView",
		file_list = 
		{
			"modules/gem/gem_view",
			"modules/gem/gem_icon_layer",
			"modules/gem/gem_pack_layer",
			"modules/gem/templ_gem_iproperty",
		}
	},	

	--天梯赛
	[BaseViewType.LADDER] = 
	{
		class_name = "LadderView",
		file_list = 
		{
			"modules/ladder/ladder_view",
			"modules/ladder/temp_ladder_task",
			"modules/ladder/temp_ladder_promotion",
			"modules/ladder/ladder_vs/temp_ladder_vs_view",
		}
	},	
	
	--天梯赛赛季奖励
	[BaseViewType.LADDER_SEASON_AWARD_VIEW] = 
	{
		class_name = "LadderSeasonAwardView",
		file_list = 
		{
			"modules/ladder/ladder_season_award/ladder_season_award_view",
		}
	},	

	--天梯赛殿堂
	[BaseViewType.LADDER_HONOR_VIEW] = 
	{
		class_name = "LadderHonorView",
		file_list = 
		{
			"modules/ladder/ladder_honor/ladder_honor_view",
		}
	},	

	--天梯赛低段位战斗结算界面
	[BaseViewType.LADDER_LOWER_RESULT] = 
	{
		class_name = "LadderLowerBattleResult",
		file_list = 
		{
			"modules/ladder/ladder_lower_result",
		}
	},

	--天梯赛王者段位战斗结算界面
	[BaseViewType.LADDER_SENIOR_RESULT] = 
	{
		class_name = "LadderSeniorBattleResult",
		file_list = 
		{
			"modules/ladder/ladder_senior_result",
		}
	},

	--世界地图
	[BaseViewType.TEAM_WORLD_MAP] = 
	{
		class_name = "TeamWorldMapView",
		file_list = 
		{
			"modules/team/team_world_map_view",
			"modules/team/team_world_map_reward_view",
		}
	},	


	--组队采集和战斗
	[BaseViewType.TEAM_GATHER_AND_BATTLE] = 
	{
		class_name = "TeamGatherAndBattleView",
		file_list = 
		{
			"modules/team/team_gather_and_battle_view",
		}
	},	

	--组队原石商店
	[BaseViewType.RAW_GEMSTONE_SHOP] = 
	{
		class_name = "RawGemStoneShopView",
		file_list = 
		{
			"modules/team/raw_gemstone_shop_view",
		}
	},	

	--组队BOSS战斗
	[BaseViewType.TEAM_BOSS_BATTLE] = 
	{
		class_name = "TeamBossBattleView",
		file_list = 
		{
			"modules/team/team_boss_battle_view",
		}
	},

	--定制推送
	[BaseViewType.CUSTOM] = 
	{
		class_name = "PushCustomView",
		file_list = 
		{
			--定制推送
			"modules/push/push_custom/push_custom_view", 
		}	
	},	

	--占卜牌
	[BaseViewType.DIVINATION] = 
	{
		class_name = "DivinationView",
		file_list = 
		{
			--占卜主界面
			"modules/divination/divination_view",
			--预览
			-- "modules/divination/templ_divination_preview",
			"modules/divination/divinationPreview/templ_divination_reward_prelook", 
			"modules/divination/divinationPreview/templ_divination_reward_cell", 
		}	
	},	

	--占星师
	[BaseViewType.DIVINATION_SCRYER] = 
	{
		class_name = "DivinationScryerView",
		file_list = 
		{
			--占卜主界面
			"modules/divination/divinationScryer/divinationDcryerView/divination_scryer_view", 
			"modules/divination/divinationScryer/divinationDcryerView/templ_divination_challenge", 
			"modules/divination/divinationScryer/divinationDcryerView/templ_divination_card", 
			
			"modules/divination/divinationScryer/temp_divination_item", 
			"modules/divination/divinationScryer/templ_divination_convert", 
			"modules/divination/divinationScryer/templ_divination_breakview", 
		}	
	},	

	--占星师商店
	[BaseViewType.DIVINATION_SHOP] = 
	{
		class_name = "DivinationShopView",
		file_list = 
		{
			--占卜主界面
			"modules/divination/divinationShop/divinationshop_view", 
		}	
	},	

	--占星师卡牌提升
	[BaseViewType.DIVINATION_INTENSIFY] = 
	{
		class_name = "DivinationIntensifyView",
		file_list = 
		{
			--占卜提升主界面
			"modules/divination/divinationIntensify/divination_intensify", 
			--占卜信息
			"modules/divination/divinationIntensify/templ_divination_minute_info", 
			--占卜强化
			"modules/divination/divinationIntensify/templ_divination_intensify", 
			--占卜精炼
			"modules/divination/divinationIntensify/templ_divination_refine", 
			--占卜精炼
			"modules/divination/divinationIntensify/divination_font_effects/divination_font_effects", 
		}	
	},	

	--占星师背包
	[BaseViewType.DIVINATION_PACK] = 
	{
		class_name = "DivinationPackView",
		file_list = 
		{
			--占星师背包
			"modules/divination/divinationPack/divination_pack_view", 
		}	
	},	

	--占星师命运商店
	[BaseViewType.DIVINATION_DESTINY_SHOP] = 
	{
		class_name = "DivinationDestinyShopView",
		file_list = 
		{
			--占星师命运商店
			"modules/divination/divinationDestinyShop/divinationdestinyshop_view", 
		}	
	},	

	--占星师收藏室
	[BaseViewType.DIVINATION_COLLECT] = 
	{
		class_name = "DivinationCollectView",
		file_list = 
		{
			--占星师命运商店
			"modules/divination/divinationCollect/divination_collect_view", 
		}	
	},	

	--占星师收藏室_书
	[BaseViewType.DIVINATION_BOOK] = 
	{
		class_name = "DivinationBookView",
		file_list = 
		{
			--占星师收藏室_书
			"modules/divination/divinationCollect/divination_book_view", 
			"modules/divination/divinationCollect/templ_divination_book_info", 
		}	
	},	

	--占星师排行榜
	[BaseViewType.DIVINATION_RANK] = 
	{
		class_name = "DivinationRankView",
		file_list = 
		{
			--占星师排行榜
			"modules/divination/divinationRank/divination_rank_view", 
		}	
	},	
	
	--卡牌穿戴
	[BaseViewType.DIVINATION_DRESS] = 
	{
		class_name = "DivinationCrossView",
		file_list = 
		{
			--卡牌穿戴
			"modules/divination/divination_cross/divination_cross_list_view", 
			"modules/divination/divination_cross/divination_trade", 
		}	
	},	

	--圣诞节
	[BaseViewType.CHRISTMAS] = 
	{
		class_name = "ChristmasView",
		file_list = 
		{
			--卡牌穿戴
			"modules/limit_activity/Christmas/christmas_view", 
			"modules/limit_activity/Christmas/templ_christmas_boss_reward_view", 
			"modules/limit_activity/Christmas/templ_christmas_shop_item", 
		}	
	},

	--在线奖励
	[BaseViewType.ONLINE_AWARD] = 
	{
		class_name = "OnlineawardView",
		file_list = 
		{
			--在线奖励
			"modules/onlineaward/onlineaward_view", 
		}	
	},

	--在线奖励
	[BaseViewType.FEEDBACK] = 
	{
		class_name = "FeedbackView",
		file_list = 
		{
			--在线奖励
			"modules/feedback/feedback_view", 
		}	
	},

	--女王
	[BaseViewType.QUEEN] = 
	{
		class_name = "QueenView",
		file_list = 
		{
			--女王
			"modules/queen/queen_view", 
		}	
	},

	--转盘
	[BaseViewType.TURNTABLE] = 
	{
		class_name = "TurntableView",
		file_list = 
		{
			--转盘
			"modules/turntable/turntable_view", 
			--积分奖励
			"modules/turntable/temp_turntable_award",
			--积分排行
			"modules/turntable/temp_turntable_rank",  
		}	
	},

	--春节活动
	[BaseViewType.SPRING_FESTIVAL] = 
	{
		class_name = "SpringFestivalView",
		file_list = 
		{
			--春节活动
			"modules/limit_activity/SpringFestival/springfestival_view", 
			"modules/limit_activity/Christmas/templ_christmas_boss_reward_view", 
			"modules/limit_activity/SpringFestival/templ_springfestival_shop_item", 
		}	
	},

	--扭蛋机抽奖
	[BaseViewType.GASHAPONMACHINE] = 
	{
		class_name = "GashaponMachineView",
		file_list = 
		{
			--扭蛋机抽奖
			"modules/limit_activity/GashaponMachine/gashaponmachine_view", 
			"modules/team/templ_team_lottery_reward_prelook",
			"modules/team/templ_team_lottery_reward_cell",
		}	
	},

	--阵营招募
	[BaseViewType.CAMP_RECRUIT] = 
	{
		class_name = "CampRecruitView",
		file_list = 
		{
			--阵营招募
			"modules/limit_activity/CampRecruit/camprecruit_view", 
			"modules/limit_activity/CampRecruit/award_layer", 
			"modules/limit_activity/CampRecruit/templ_camp_recruit_record_cell", 
			"modules/limit_activity/CampRecruit/templ_handbook_prelook",
			"modules/limit_activity/CampRecruit/templ_handbook_heroicon",
		}	
	},	

	--助战
	[BaseViewType.ASSIST] = 
	{
		class_name = "AssistView",
		file_list = 
		{
			--助战
			"modules/assist/assist_view", 
			"modules/assist/templ_assist_in_fighting_cell", 
		}	
	},	

	--水晶商店
	[BaseViewType.CRYSTAL_SHOP] = 
	{
		class_name = "CrystalShopView",
		file_list = 
		{
			"modules/crystalshop/crystal_shop_view",
			"modules/crystalshop/crystal_buy_tabview",
			"modules/crystalshop/crystal_recharge_tabview",		
		}
	},

	--世界等级任务
	[BaseViewType.PUSH_WORLDLEVEL] = 
	{
		class_name = "PushWorldLevelView",
		file_list = 
		{
			"modules/push/push_worldleveltask/push_worldleveltask_view",
		}
	},

	--端午节
	[BaseViewType.DRAGONBOAT_FESTIVAL] = 
	{
		class_name = "DragonBoatFestivalView",
		file_list = 
		{
			--阵营招募
			"modules/limit_activity/DragonBoatFestival/dragonboat_festival_view", 
			"modules/limit_activity/Christmas/templ_christmas_boss_reward_view", 
			"modules/limit_activity/DragonBoatFestival/templ_dungeonboat_festival_shop_item",
		}	
	},	

	--积分赛
	[BaseViewType.INTEGRAL_RACE] = 
	{
		class_name = "IntegralRaceView",
		file_list = 
		{
			--积分赛
			"modules/integralrace/integral_race_view", 
			"modules/integralrace/templ_choose_camp", 
			"modules/integralrace/templ_challenge_task", 
			"modules/integralrace/templ_integralrace_rank_view", 	
			"modules/integralrace/templ_integralrace_rank_cell", 				
		}	
	},	

	--积分赛荣耀商店
	[BaseViewType.INTEGRAL_GLORY_SHOP] = 
	{
		class_name = "IntegralGloryShopView",
		file_list = 
		{
			--积分赛荣耀商店
			"modules/integralrace/integral_glory_shop_view", 
			"modules/integralrace/templ_integral_glory_shop_cell", 
			"modules/integralrace/templ_integral_gloryshop_buyview", 			
		}	
	},	

	--荣耀角斗场
	[BaseViewType.GLORY_ARENA] = 
	{
		class_name = "GloryArenaView",
		file_list = 
		{
			--荣耀角斗场
			"modules/integralrace/glory_arena", 			
		}	
	},	

	--英雄进化
	[BaseViewType.HERO_EVOLUTION] = 
	{
		class_name = "HeroEvolutionView",
		file_list = 
		{
			--英雄进化
			"modules/hero/evolution/hero_evolution_view", 		
			"modules/hero/evolution/templ_hero_evolution_succ", 		
		}	
	},	

	--援军助威
	[BaseViewType.REINFORCEMENTS_CHEER] = 
	{
		class_name = "ReinforcementsCheerView",
		file_list = 
		{
			--援军助威
			"modules/hero/reinforcementscheer/reinforcements_cheer_view", 	
			"modules/hero/reinforcementscheer/templ_heroreinforcements_lineup_cell", 			
		}	
	},

	--资源战
	[BaseViewType.RESOURCE_WAR] = 
	{
		class_name = "ResourcesWarView",
		file_list = 
		{
			--资源战
			"modules/resourcewar/resourcewar_main/resourcewar_main_view", 
			"modules/resourcewar/resourcewar_main/templ_resource_ui", 	
			"modules/resourcewar/resourcewar_main/templ_resource_element", 	

			--敌人
			"modules/resourcewar/resourcewar_popup/templ_resource_enemy", 
			--战报
			"modules/resourcewar/resourcewar_popup/templ_resource_shark",
			--技能
			"modules/resourcewar/resourcewar_popup/templ_resource_tech", 
			--军团
			"modules/resourcewar/resourcewar_popup/templ_resource_union", 	
			--搜索
			"modules/resourcewar/resourcewar_popup/templ_resource_seek", 	

			--矿场
			"modules/resourcewar/resourcewar_popup/templ_resource_mine", 	
		}	
	},

	--资源战商店
	[BaseViewType.RESOURCE_SHOP] = 
	{
		class_name = "ResourceWarView",
		file_list = 
		{
			--资源战
			"modules/resourcewar/resourcewar_main/resourcewar_shop_view", 
		}	
	},
	
	--审核主界面
	[BaseViewType.POLYTROPIC_MAINCITY] = 
	{
		class_name = "PolytropicMaincityView",
		file_list = 
		{
			--审核主界面	
			"modules/polytropicmaincity/templ_polytropictype_recommend_item", 
			"modules/polytropicmaincity/templ_polytropictype_spirit_item", 
			"modules/polytropicmaincity/templ_polytropictype_worldheart_item", 
			"modules/polytropicmaincity/templ_polytropictype_vip_item", 	
			"modules/polytropicmaincity/templ_polytropictype_handbook_item", 
			"modules/polytropicmaincity/templ_polytropictype_shop_item", 
			"modules/polytropicmaincity/templ_polytropictype_dungeon_item", 		
			"modules/polytropicmaincity/templ_polytropictype_activity_item", 
			"modules/polytropicmaincity/templ_polytropictype_arena_item", 
			"modules/polytropicmaincity/templ_polytropictype_challenge_item", 	
			"modules/polytropicmaincity/polytropicmaincity_view", 	
		}	
	},

	----------------------------------------------------------------------------
	--战斗结算
	----------------------------------------------------------------------------
	[BaseViewType.BATTLE_RESULT_WIN] = 
	{
		class_name = "BattleResultWinView", 
		file_list = 
		{
			"modules/battle/templ_common_reward",			
			"modules/battle/battle_result_win_view",
		}
	},	

	[BaseViewType.BATTLE_RESULT_LOSE] = 
	{
		class_name = "BattleResultLoseView", 
		file_list = 
		{
			"modules/battle/battle_result_lose_view",
		}
	},	

	[BaseViewType.SP_BATTLE_RESULT_WIN] = 
	{
		class_name = "SpBattleResultWinBase", 
		file_list = 
		{
			"modules/battle/templ_common_reward",			
			"modules/battle/battle_win/sp_battle_result_win_base",
		}
	},

	[BaseViewType.SP_BATTLE_RESULT_LOSE] = 
	{
		class_name = "SpBattleResultLoseBase", 
		file_list = 
		{
			"modules/battle/templ_common_reward",
			"modules/battle/battle_result_lose_view",
			-- "modules/battle/battle_win/sp_battle_result_win_base",
			"modules/battle/battle_lose/sp_battle_result_lose_base",
		}
	},	

	-------------------------------------------------------------------------------------------
	--战斗结算(胜利)
	-------------------------------------------------------------------------------------------
	--日常副本结算
	[BaseViewType.DAILY_DUNGEON_RESULT_WIN] = 
	{
		class_name = "DailyDungeonResultWinView",
		file_list = 
		{
			--日常副本结算	
			"modules/battle/battle_win/daily_dungeon_result_view", 
		}	
	},

	--英雄副本结算
	[BaseViewType.HERO_DUNGEON_RESULT_WIN] = 
	{
		class_name = "HeroDungeonResultWinView",
		file_list = 
		{
			--英雄副本结算	
			"modules/battle/battle_win/hero_dungeon_result_view", 
		}	
	},

	--精英副本结算
	[BaseViewType.ELITE_DUNGEON_RESULT_WIN] = 
	{
		class_name = "EliteDungeonResultWinView",
		file_list = 
		{
			--精英副本结算	
			"modules/battle/battle_win/elite_dungeon_result_view", 
		}	
	},

	--普通爬塔结算
	[BaseViewType.TOWER_COMMON_RESULT_WIN] = 
	{
		class_name = "TowerCommonResultWinView",
		file_list = 
		{
			--普通爬塔结算	
			"modules/battle/battle_win/tower_common_result_view", 
		}	
	},

	--精英爬塔结算
	[BaseViewType.TOWER_ELITE_RESULT_WIN] = 
	{
		class_name = "TowerEliteResultWinView",
		file_list = 
		{
			--精英爬塔结算	
			"modules/battle/battle_win/tower_elite_result_view", 
		}	
	},

	--叛军副本结算
	[BaseViewType.REBEL_RESULT_WIN] = 
	{
		class_name = "RebelResultWinView",
		file_list = 
		{
			--叛军副本结算	
			"modules/battle/battle_win/rebel_result_view", 
		}	
	},

	--领地挑战结算
	[BaseViewType.MANOR_DARE_RESULT_WIN] = 
	{
		class_name = "ManorDareResultWinView",
		file_list = 
		{
			--领地挑战结算	
			"modules/battle/battle_win/manor_dare_result_view", 
		}	
	},

	--夺宝结算
	[BaseViewType.PLUNDERED_TREASURES_RESULT_WIN] = 
	{
		class_name = "PlunderedTreasuresResultWinView",
		file_list = 
		{
			--夺宝结算	
			"modules/battle/battle_win/plundered_treasures_result_view", 
		}	
	},

	--竞技场
	[BaseViewType.ARENA_RESULT_WIN] = 
	{
		class_name = "ArenaResultWinView",
		file_list = 
		{
			--竞技场	
			"modules/battle/battle_win/arena_result_view", 
		}	
	},

	--世界boss/未测试
	[BaseViewType.WORLD_BOSS_RESULT_WIN] = 
	{
		class_name = "WorldBossResultWinView",
		file_list = 
		{
			--世界boss	
			"modules/battle/battle_win/world_boss_result_view", 
		}	
	},

	--积分赛/未测试
	[BaseViewType.INTEGRAL_RACE_RESULT_WIN] = 
	{
		class_name = "IntegralRaceResultWinView",
		file_list = 
		{
			--积分赛	
			"modules/battle/battle_win/integral_race_result_view", 
		}	
	},

	--怪物攻城/未测试
	[BaseViewType.ATTACK_CITY_RESULT_WIN] = 
	{
		class_name = "AttackCityResultWinView",
		file_list = 
		{
			--怪物攻城	
			"modules/battle/battle_win/attack_city_result_view", 
		}	
	},

	--组队普通战斗/未测试
	[BaseViewType.TEAM_NORMAL_BATTLE_RESULT_WIN] = 
	{
		class_name = "TeamNormalResultWinView",
		file_list = 
		{
			--组队普通战斗	
			"modules/battle/battle_win/team_normal_result_view", 
		}	
	},

	--组队BOSS战斗/未测试
	[BaseViewType.TEAM_BOSS_BATTLE_RESULT_WIN] = 
	{
		class_name = "TeamBossResultWinView",
		file_list = 
		{
			--组队BOSS战斗	
			"modules/battle/battle_win/team_boss_result_view", 
		}	
	},

	--占卜战斗
	[BaseViewType.DIVINATION_RESULT_WIN] = 
	{
		class_name = "DivinatuonResultWinView",
		file_list = 
		{
			--占卜战斗	
			"modules/battle/battle_win/divinatuon_result_view", 
		}	
	},

	--资源战/未测试
	[BaseViewType.RESOURCE_WAR_RESULT_WIN] = 
	{
		class_name = "ResourceWarResultWinView",
		file_list = 
		{
			--资源战	
			"modules/battle/battle_win/resource_war_result_view", 
		}	
	},

	--公会
	[BaseViewType.UNION_RESULT_WIN] = 
	{
		class_name = "UnionResultWinView",
		file_list = 
		{
			--公会	
			"modules/battle/battle_win/union_result_view", 
		}	
	},

	--公会低级/未测试
	[BaseViewType.UNION_LOW_RESULT_WIN] = 
	{
		class_name = "UnionLowResultWinView",
		file_list = 
		{
			--公会低级	
			"modules/battle/battle_win/union_low_result_view", 
		}	
	},

	--公会高级/未测试
	[BaseViewType.UNION_TALL_RESULT_WIN] = 
	{
		class_name = "UnionTallResultWinView",
		file_list = 
		{
			--公会高级	
			"modules/battle/battle_win/union_tall_result_view", 
		}	
	},

	-------------------------------------------------------------------------------------------
	--战斗结算(失败)
	-------------------------------------------------------------------------------------------
	--竞技场
	[BaseViewType.ARENA_RESULT_LOSE] = 
	{
		class_name = "ArenaResultLoseView",
		file_list = 
		{
			--竞技场	
			"modules/battle/battle_lose/arena_result_view", 
		}	
	},

	--夺宝结算
	[BaseViewType.PLUNDERED_TREASURES_RESULT_LOSE] = 
	{
		class_name = "PlunderedTreasuresResultLoseView",
		file_list = 
		{
			--夺宝结算	
			"modules/battle/battle_lose/plundered_treasures_result_view", 
		}	
	},	
}