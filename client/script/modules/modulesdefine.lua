
--[[
%% @module: 模块文件定义
%% @author: swordman sue
%% @created: 2016-07-10
--]]

Modules = Modules or {}

--模块类型
ModuleType =
{
	INVALID = "INVALID",

	RES_UPDATE = "RES_UPDATE",		 --资源更新

	LOADING = "LOADING",			 --UI/场景加载模块

	LOGIN = "LOGIN",			  	 --登陆

	ROLE = "ROLE",					 --角色

	TIME_SYNC = "TIME_SYNC",		 --时间同步

	ERROR_NOTIFY = "ERROR_NOTIFY",	 --错误通知

	STATISTICS = "STATISTICS", 		 --点击统计

	MAIN_CITY = "MAIN_CITY",		 --主城

	DUNGEON = "DUNGEON",			 --副本

	BATTLE = "BATTLE",				 --战斗

	CHAT = "CHAT",			 		 --聊天

	Pack = "Pack",		 			 --物品背包

	HERO = "HERO",					 --英雄

	EQUIP = "EQUIP",				 --装备

	CHECKROLEINFO = "CHECKROLEINFO", --查看玩家信息

	SHOP = "SHOP",              	 --商店

	EMAIL = "EMAIL",             	 --邮件

	FORGINGMASTER = "FORGINGMASTER", --锻造大师

	RANK = "RANK",             		 --排行榜

	TREASURE = "TREASURE" ,			--圣器

	FRIEND = "FRIEND",            	--好友

	RECYCLE = "RECYCLE",			--回收

	REBEL = "REBEL",				--叛军

	TOWER = "TOWER",				--爬塔

	SPIRIT = "SPIRIT",				--精灵

	MANOR = "MANOR", 				--领地

	PLUNDEREDTREASURES = "PLUNDEREDTREASURES",	--夺宝

	ARENA = "ARENA",					--竞技场

	UNION = "UNION",					--公会

	DALIYTASK_AND_ACHIEVEMENT = "DALIYTASK_AND_ACHIEVEMENT",	 --日常与成就

	POWER = "POWER",					--战力公式

	VIP = "VIP",						--VIP

	POWER = "POWER",					--战力公式

	HERO_SHOP = "HERO_SHOP",			--英雄商店

	DEBRISELL = "DEBRISELL",			--出售界面

	WORLDBOSS = "WORLDBOSS",			--世界boss

	ACCEPT_PRIZE_CENTER = "ACCEPT_PRIZE_CENTER", 	--领奖中心

	LEAD = "LEAD",						--新手指引

	STORY = "STORY",					--剧情

	APPROACH_OF_ACHIEVING = "APPROACH_OF_ACHIEVING", --获取途径

	NEW_DAY_NOTIFY = "NEW_DAY_NOTIFY",			--零点

	ACTIVITY = "ACTIVITY", 					--活动

	SEVENDAYS = "SEVENDAYS", 				--七天活动

	RECOMMEND_LINEUP = "RECOMMEND_LINEUP",  --推荐阵容

	--公告
	NOTICE = "NOTICE",  --公告

	SUNDRY = "SUNDRY",					--注定

	WORLD_HEART = "WORLD_HEART",		--世界之心

	CHARGE = "CHARGE" ,				--充值

	CHARGE_RANK = "CHARGE_RANK" , --充值排行

	STRONGER = "STRONGER",				--我要变强

	PVP = "PVP" , 						--PVP

	MONSTER_STORMCASTLE = "MONSTER_STORMCASTLE",	--怪物攻城

	TITLE = "TITLE",				--称号

	CHASM = "CHASM",				--无尽深渊

	LIMIT_DUNGEON = "LIMIT_DUNGEON" , --限时副本汇总

	MORROW = "MORROW",				--次日资源

	PUSH = "PUSH",					--推送

	KNIGHTHOOD = "KNIGHTHOOD" ,	    --爵位

	SUPERDISCOUNTVIEW = "SUPERDISCOUNTVIEW" , --超值购

	DIRECT_BUY_SHOP = "DIRECT_BUY_SHOP",	  --直购特惠

	RANK_ACTIVITY = "RANK_ACTIVITY" ,		  --冲榜活动

	GEM = "GEM",					--宝石

	SCENE = "SCENE",				--场景

	LADDER = "LADDER",				--天梯

	TEAM = "TEAM",							--组队

	DIVINATION = "DIVINATION" ,				--占卜

	ONLINE_AWARD = "ONLINE_AWARD" , 		--在线奖励
	
	LIMIT_ACTIVITY = "LIMIT_ACTIVITY",	    --限时活动

	PUSH_SEVEN = "PUSH_SEVEN",				--七天累计

	FEEDBACK = "FEEDBACK",					--充值回调

	QUEEN = "QUEEN",						--女王

	CUSTOM_PURCHASE = "CUSTOM_PURCHASE",	--自定义超值购

	TURNTABLE = "TURNTABLE", 				--转盘

	CRYSTAL_SHOP = "CRYSTAL_SHOP",			--水晶商店

	PUSH_WORLDLEVEL = "PUSH_WORLDLEVEL",	--世界等级任务

	INTEGRAL_RACE = "INTEGRAL_RACE",		--积分赛

	RESOURCE_WAR = "RESOURCE_WAR" , 		--资源战

	POLYTROPIC_MAINCITY = "POLYTROPIC_MAINCITY", --审核主界面

	------------------------------------------	
	SP_LOGIC_MGR = "SP_LOGIC_MGR",		--特殊逻辑管理器
	RED_MGR = "RED_MGR",		--特殊逻辑管理器
}

--公共模块注册表(销毁时，order越大越先销毁)
Modules.ComModulesRegistry =
{
	--资源更新
	[ModuleType.RES_UPDATE] = 
	{
		order = 1,
		class_name = "ResUpdateModule", file_list = {"modules/resupdate/resupdate_module"}
	},

	--时间同步
	[ModuleType.TIME_SYNC] = 
	{
		order = 2,
		class_name = "TimeSyncModule", file_list = {"modules/timesync/time_sync_module"}
	},

	--错误码
	[ModuleType.ERROR_NOTIFY] = 
	{
		order = 3,
		class_name = "ErrorNotifyModule", file_list = {"modules/errnotify/error_notify_module"}
	},

	--UI/场景加载
	[ModuleType.LOADING] = 
	{
		order = 4,
		class_name = "LoadingModule", file_list = {"modules/loading/loading_module"}
	},	

	--登陆
	[ModuleType.LOGIN] = 
	{
		order = 5,
		class_name = "LoginModule", file_list = {"modules/login/login_module"}
	},

	--角色
	[ModuleType.ROLE] = 
	{
		order = 6,
		class_name = "RoleModule", file_list = {"modules/role/role_module"}
	},

	--特殊逻辑管理器
	[ModuleType.SP_LOGIC_MGR] = 
	{	
		order = 7,
		class_name = "SplogicModulesMgr", 
		file_list = 
		{
			"modules/splogic/splogic_module_mgr", 
			"modules/splogic/splogic_module", 
			"modules/splogic/splogic_main_dungeon",
			"modules/splogic/splogic_multi_dungeon",
		}
	},		
}

--系统模块注册表
Modules.ModulesRegistry =
{
	--主UI
	[ModuleType.MAIN_CITY] = 
	{
		class_name = "MaincityModule", 
		file_list = 
		{
			"modules/maincity/maincitydefine",
			"modules/maincity/maincity_module",

			"modules/maincity/entry/base_maincity_entry", 
			"modules/maincity/entry/facebook_sharereward_entry", 
			"modules/maincity/entry/dungeon_entry", 
			"modules/maincity/entry/first_recharge_entry", 
			"modules/maincity/entry/fivestar_highpraise_entry", 
			"modules/maincity/entry/morrow_entry", 
			"modules/maincity/entry/online_award_entry", 
			"modules/maincity/entry/system_foreshow_entry", 
			"modules/maincity/entry/stronger_entry", 
			"modules/maincity/entry/activity_entry", 
			"modules/maincity/entry/pack_extend_entry", 
			"modules/maincity/entry/union_entry", 
			"modules/maincity/entry/ladder_entry", 
			"modules/maincity/entry/team_entry", 
			"modules/maincity/entry/team_invite_entry", 
			"modules/maincity/entry/maincity_entry_mgr", 
		}
	},

	--聊天
	[ModuleType.CHAT] = 
	{
		class_name = "ChatModule", file_list = {"modules/chat/chat_module"}
	},

	--查看角色信息
	[ModuleType.CHECKROLEINFO] = 
	{
		class_name = "CheckRoleInfoModule", file_list = {"modules/checkroleinfo/checkroleinfo_module"}
	},

	--物品背包
	[ModuleType.Pack] = 
	{
		class_name = "PackModule", 
		file_list = 
		{
			"modules/beibao/pack_module",
			"modules/beibao/pack_debris_module",
			"modules/beibao/pack_goods_module",
		}
	},

	--副本
	[ModuleType.DUNGEON] = 
	{
		class_name = "DungeonModule", 
		file_list = 
		{
			"modules/dungeon/dungeon_module",
		}
	},

	--战斗
	[ModuleType.BATTLE] = 
	{
		class_name = "BattleModule", 
		file_list = 
		{
			"modules/battle/battle_module",
			"modules/battle/battle_module_ex",
			"modules/battle/pvp_battle_module",
		}
	},

	--场景
	[ModuleType.SCENE] = 
	{
		class_name = "SceneModule", 
		file_list = 
		{
			"modules/scene/scene_module",
		}
	},

	--装备
	[ModuleType.EQUIP] = 
	{
		class_name = "EquipModule", file_list = 
		{
			"modules/equip/equip_module",
			"modules/equip/equip_up_module",
		}
	},

	--商店
	[ModuleType.SHOP] = 
	{
		class_name = "ShopModule", file_list = {"modules/shop/shop_module"}
	},

	--邮件
	[ModuleType.EMAIL] = 
	{
		class_name = "EmailModule", file_list = {"modules/email/email_module"}
	},

	--排行榜
	[ModuleType.RANK] = {
		class_name = "RankModule", file_list = {"modules/rank/rank_module"}
	},

	--好友
	[ModuleType.FRIEND] = {
		class_name = "FriendModule", file_list = {"modules/friend/friend_module"}
	},

	--英雄
	[ModuleType.HERO] = 
	{
		class_name = "HeroModule", file_list = {"modules/hero/hero_module", "modules/hero/hero_module_ex"}
	},

	--锻造大师
	[ModuleType.FORGINGMASTER] = 
	{
		class_name = "ForgeModule", file_list = {"modules/forgingmaster/forging_module"}
	},	

	--圣器
	[ModuleType.TREASURE] = 
	{
		class_name = "TreasureModule", file_list = {"modules/treasure/treasure_modules"}
	},	

	--回收
	[ModuleType.RECYCLE] = 
	{
		class_name = "RecycleModule", file_list = {"modules/recycle/recycle_module"}
	},

	--叛军
	[ModuleType.REBEL] = 
	{
		class_name = "RebelModule", file_list = {"modules/rebel/rebel_module"}
	},

	--爬塔
	[ModuleType.TOWER] = 
	{
		class_name = "TowerModule", file_list = {"modules/tower/tower_module"}
	},

	--领地
	[ModuleType.MANOR] = 
	{
		class_name = "ManorModule", file_list = {"modules/manor/manor_module"}
	},

	--精灵
	[ModuleType.SPIRIT] = 
	{	
		class_name = "Spirit", file_list = {"modules/spirit/spirit_module"}
	},	

	--夺宝
	[ModuleType.PLUNDEREDTREASURES] = 
	{	
		class_name = "PlunderedTreasuresModule", file_list = {"modules/plunderedtreasures/plunderedtreasures_module"}
	},

	--竞技场
	[ModuleType.ARENA] = 
	{	
		class_name = "ArenaModule", file_list = {"modules/arena/arena_module"}
	},

	--公会
	[ModuleType.UNION] = 
	{
		class_name = "UnionModule", file_list = {"modules/union/union_modules" ,"modules/union/union_combat/union_combat_modules" }
	},

	--VIP
	[ModuleType.VIP] = 
	{
		class_name = "VIPModule", file_list = {"modules/vip/vip_module"}
	},

	--战力公式
	[ModuleType.POWER] = 
	{
		class_name = "PowerFormula", file_list = {"modules/powerformula/power_formula_module"}
	},

	--英雄商店
	[ModuleType.HERO_SHOP] = 
	{
		class_name = "HeroShopModule", file_list = {"modules/heroshop/heroshop_module"}
	},

	--出售界面
	[ModuleType.DEBRISELL] = 
	{
		class_name = "DebriSellModule", file_list = {"modules/debrisell/debrisell_module"}
	},

	[ModuleType.WORLDBOSS] = 
	{
		class_name = "WorldBossModule", file_list = {"modules/worldboss/worldboss_module"}
	},

	[ModuleType.ACCEPT_PRIZE_CENTER] = 
	{
		class_name = "AcceptPrizeCenterModule", file_list = {"modules/accepttheprizecenter/accept_prize_center_module"}
	},

	--日常与成就
	[ModuleType.DALIYTASK_AND_ACHIEVEMENT] = 
	{
		class_name = "DaliytaskAndAchievementModule", file_list = {"modules/daliytaskandachievement/daliytask_and_achievement_module"}
	},

	--新手指引
	[ModuleType.LEAD] = 
	{
		class_name = "LeadModule", file_list = {"modules/lead/lead_module"}
	},

	--剧情
	[ModuleType.STORY] = 
	{
		class_name = "StoryModule", file_list = {"modules/lead/lead_module", "modules/story/story_module"}
	},

	--获取途径
	[ModuleType.APPROACH_OF_ACHIEVING] = 
	{
		class_name = "ApproachOfAchievingModule", file_list = {"modules/approachofachieving/approach_of_achieving_module"}
	},
	
	--签到
	[ModuleType.ACTIVITY] = 
	{
		class_name = "ActivityModule", 
		file_list = {
						"modules/activity/activity_module",
						"modules/activity/activity_welfare_module"
					}
	},

	--七天活动
	[ModuleType.SEVENDAYS] = 
	{
		class_name = "SevenDaysModule", file_list = {"modules/activity/sevendays/seven_days_module"}
	},

	--推荐阵容
	[ModuleType.RECOMMEND_LINEUP] = 
	{
		class_name = "RecommendLineupModule", file_list = {"modules/recommendlineup/recommend_lineup_module"}
	},

	--杂项
	[ModuleType.SUNDRY] = 
	{
		class_name = "SundryModule", file_list = 
		{
			"modules/charge/charge_idmap",
			"modules/sundry_module",
		}
	},

	--公告
	[ModuleType.NOTICE] = 
	{
		class_name = "NoticeModule", file_list = {"modules/notice/notice_module"}
	},

	--世界之心
	[ModuleType.WORLD_HEART] = 
	{	
		class_name = "WorldHeart", file_list = {"modules/worldheart/worldheart_module"}
	},	

	--充值
	[ModuleType.CHARGE] = 
	{	
		class_name = "ChargeModule", 
		file_list = 
		{
			"modules/charge/charge_module",
			"modules/charge/charge_idmap",
		}
	},	

	--充值排行
	[ModuleType.CHARGE_RANK] = 
	{
		class_name = "ChargeRankModule", file_list = {"modules/charge/charge_rank_module"}
	},

	--PVP
	[ModuleType.PVP] = 
	{
		class_name = "PVPModule", file_list = 
		{
			"modules/pvp/pvp_module"
		}
	},

	--我要变强
	[ModuleType.STRONGER] = 
	{	
		class_name = "StrongerModule", file_list = {"modules/stronger/stronger_module"}
	},	

	--怪物攻城
	[ModuleType.MONSTER_STORMCASTLE] = 
	{	
		class_name = "MonsterStormCastleModule", file_list = {"modules/monsterstormcastle/monster_stormcastle_module"}
	},	

	--称号
	[ModuleType.TITLE] = 
	{
		class_name = "TitleModule", file_list = {"modules/title/title_module"}
	},
	
	--称号
	[ModuleType.CHASM] = 
	{
		class_name = "ChasmModule", file_list = {"modules/chasm/chasm_module"}
	},

	--限时副本汇总
	[ModuleType.LIMIT_DUNGEON] = 
	{
		class_name = "LimitDungeonModule", file_list = {"modules/limit_dungeon/limit_dungeon_module"}
	},

	--次日资源
	[ModuleType.MORROW] = 
	{
		class_name = "MorrowModule", file_list = {"modules/push/morrow/morrow_module"}
	},

	--次日资源
	[ModuleType.PUSH] = 
	{
		class_name = "PushModule", file_list = {"modules/push/push_module"}
	},

	--爵位
	[ModuleType.KNIGHTHOOD] = 
	{
		class_name = "KnighthoodModule", file_list = {"modules/knighthood/knighthood_module"}
	},

	--超值购
	[ModuleType.SUPERDISCOUNTVIEW] = 
	{
		class_name = "SuperDiscountModule", file_list = {"modules/superdiscount/superdiscount_module"}
	},

	--直购特惠
	[ModuleType.DIRECT_BUY_SHOP] = 
	{
		class_name = "DirectBuyShopModule", file_list = {"modules/directbuyshop/direct_buy_shop_module"}
	},

	--冲榜活动
	[ModuleType.RANK_ACTIVITY] = 
	{
		class_name = "RankActivityModule", file_list = {"modules/activity/rank_activity/rank_activity_module"}
	},

	--组队
	[ModuleType.TEAM] = 
	{
		class_name = "TeamModule", file_list = {"modules/team/team_module"}
	},
	
	--原石
	[ModuleType.GEM] = 
	{
		class_name = "GemModule", file_list = {"modules/gem/gem_module"}
	},

	--天梯
	[ModuleType.LADDER] = 
	{
		class_name = "LadderModule", file_list = {"modules/ladder/ladder_module"}
	},

	--占卜
	[ModuleType.DIVINATION] = 
	{
		class_name = "DivinationModule", file_list = {"modules/divination/divination_module"}
	},

	--限时活动
	[ModuleType.LIMIT_ACTIVITY] = 
	{
		class_name = "LimitActivityModule", file_list = {"modules/limit_activity/limit_activity_module"}
	},
	
	--在线奖励
	[ModuleType.ONLINE_AWARD] = 
	{
		class_name = "OnlineawardModule", file_list = {"modules/onlineaward/onlineaward_module"}
	},

	--七天累计
	[ModuleType.PUSH_SEVEN] = 
	{
		class_name = "PushSevenModule", file_list = {"modules/push/push_seven/push_seven_module"}
	},

	--充值回调
	[ModuleType.FEEDBACK] = 
	{
		class_name = "FeedbackModule", file_list = {"modules/feedback/feedback_module"}
	},

	--女王
	[ModuleType.QUEEN] = 
	{
		class_name = "QueenModule", file_list = {"modules/queen/queen_module"}
	},

	--自定义超值购
	[ModuleType.CUSTOM_PURCHASE] = 
	{
		class_name = "CustomPurchaseModule", file_list = {"modules/custompurchase/custompurchase_module"}
	},

	--转盘
	[ModuleType.TURNTABLE] = 
	{
		class_name = "TurntableModule", file_list = {"modules/turntable/turntable_module"}
	},

	--水晶商店
	[ModuleType.CRYSTAL_SHOP] = 
	{
		class_name = "CrystalShopModule", file_list = {"modules/crystalshop/crystal_shop_module"}
	},

	--世界等级任务
	[ModuleType.PUSH_WORLDLEVEL] = 
	{
		class_name = "PushWorldLevelModule", file_list = {"modules/push/push_worldleveltask/push_worldleveltask_module"}
	},

	--积分赛
	[ModuleType.INTEGRAL_RACE] = 
	{
		class_name = "IntegralModule", file_list = {"modules/integralrace/integral_module"}
	},

	--资源战
	[ModuleType.RESOURCE_WAR] = 
	{
		class_name = "ResourceWarModule", file_list = {"modules/resourcewar/resourcewar_module"}
	},
	--审核主界面
	[ModuleType.POLYTROPIC_MAINCITY] = 
	{
		class_name = "PolytropicMaincityModule", file_list = {"modules/polytropicmaincity/polytropicmaincity_module"}
	},

	--红点
	[ModuleType.RED_MGR] = 
	{
		class_name = "RedMgr", file_list = {"modules/red/redmgr"}
	},
}