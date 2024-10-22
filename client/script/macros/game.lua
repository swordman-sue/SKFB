
Macros = Macros or {}

Macros.Game =
{
	LoginType = 
	{
	 	--测试
		TEST = 0 ,
		--用户名注册
		USER_REGIST = 1 , 
		--用户名登陆
		USER_LOGIN = 2 , 
		--手机登陆
		HANDSET = 3 , 
		--手机注册
		HANDSET_REGIST = 4 ,
		--一键登陆
		ONEKEY_LOGIN = 5 ,
	},

	--服务器状态
	ServerStatus = 
	{
		--运行中
		RUN = 1,

		--维护中
		MAINTAIN = 2,
	},

	--区状态
	ZoneStatus = 
	{
		HOT = 1,
		NEW = 2,
		MAINTAIN = 3,
	},

	--现金的国际货币代码
	CashCurrencyType = 
	{
		--人民币
		CNY = "CNY",

		--美元
		USD = "USD",

		--日元
		JPY = "JPY",

		--台币
		TWD = "TWD",

		--马币
		MYR = "MYR",

		--港币
		HKD = "HKD",

		--越南盾
		VND = "VND",

		--泰铢
		THB = "VND",

		--印度尼西亚卢比
		IDR = "VND",
	},

	--角色创建类型
	RoleCreateType = 
	{
		NORMAL = 0,
		LEAD = 1,
	},

	--角色属性类型
	RolePropType = 
	{
		-- 1=等级
		LEVEL = 1,
		-- 2=经验
		EXP = 2,
		-- 3=金币
		GOLD = 3,
		-- 4=体力
		STAMINA = 4,
		-- 5=钻石
		DIAMOND = 5,
		-- 6=雄魂
		HERO_SOUL = 6,
		-- 7=头像	
		HEAD_ICON = 7,
		-- 8=精力 
		ENERGY = 8,
		-- 9=点赞次数
		CLICK_LIKE_TIMES = 9,
		-- 10=vip等级
		VIP_LEVEL = 10,
		-- 12=威名(爬塔)
		TOWER_PRESTIGE = 12,
		-- 13=声望(竞技场)
		ARENA_PRESTIGE = 13,
		-- 14=战功(叛军)
		REBEL_VALUE = 14,
		-- 15=通缉令
		WANTED = 15,
		-- 16=领地暴动镇压次数
		LAND_RIOT_SUPPRESS_TIMES = 16,		
		-- 17=神魂
		GODSOUL = 17,
		-- 18=工会ID
		GUILD_ID = 18,
		-- 19=角色每日初始等级
		DAILY_INIT_LEVEL = 19,
		-- 20=工会贡献值
		GUILD_CONTRIBUTION = 20,
		-- 21=VIP经验
		TEAM_PRO_VIP = 21,
		-- 22=世界BOSS荣誉
		WORLD_BOSS_HONOR = 22,
		-- 23=战力
		BATTLE_VALUE = 23,
		-- 24=仙魂
		FAIRY_SOUL = 24,
		-- 25=积分赛荣誉
		SCORE_MATCH_HONOR = 25,
		-- 26=总的友情点
		TOTAL_FRIENDSHIP_POINT = 26,
		-- 27=今日友情点
		TODAY_FRIENDSHIP_POINT = 27,
		-- 28=无尽石
		ENDLESS_STORE = 28 , 
		-- 29=爵位
		NOBILITY = 29,
		-- 30=宝石
		GEM = 30,
		-- 31= 命运
		DESTINY_COIN = 31,
		-- 32=星魂
		CARD_CURRENCY = 32,
		-- 33=水晶
		CRYSTAL = 33 , 
		-- 34=资源赛积分
		RESOURCE_WAR = 34 ,
		-- 35=越南货币
		YCOIN = 35 ,
		-- 36=充值总额度
		TOTAL_RECHARGE_AMOUNT = 36
	},

	--英雄类型
	HeroType = 
	{
		--普通英雄
		NORMAL = 1,
		--经验英雄
		EXP = 2,
	},
   
	--英雄职业
	HeroProfession = 
	{
		--近战物理
		PHYSICAL_NEAR = 1,
		--近战防御
		DEFENCE_NEAR = 2,
		--远程物理
		PHYSICAL_REMOTE = 3,
		--远程法术
		MAGIC_REMOTE = 4,
		--辅助
		ASSIST = 5,
	},

	--阵位类型
	HeroLineup = 
	{
		HERO_LINEUP = 1,			--	英雄阵位
		REINFORCEMENT = 2,			--	援军阵位
	},

	--性别
	Sex = 
	{
		FEMALE = 1,
		MALE = 2,
	},

	--副本类型(用于单人/多人战斗场景)
	FBType = 
	{
		--默认
		DEFAULT = -1,

		--主城
		MAIN_CITY = 0,

		--主线副本
		MAIN_DUNGEON = 1,

		--日常副本
		DAILY_DUNGEON = 2,

		--英雄副本
		HERO_DUNGEON = 3,

		--精英副本
		ELITE_DUNGEON = 4,

		--普通爬塔
		TOWER_COMMON = 5,

		--精英爬塔
		TOWER_ELITE = 6,

		--叛军副本
		REBEL = 7,

		--领地挑战
		MANOR_DARE = 8,

		--夺宝
		PLUNDERED_TREASURES = 9,
		
		--竞技场
		ARENA = 10,

		--世界boss
		WORLD_BOSS = 11,

		--积分赛
		INTEGRAL_RACE = 13,

		--公会
		UNION = 14,		

		--怪物攻城
		ATTACK_CITY = 15,

		--深渊
		CHASM = 16,

		--组队普通战斗
		TEAM_NORMAL_BATTLE = 17,

		--组队BOSS战斗
		TEAM_BOSS_BATTLE = 18,

		--天梯赛
		LADDER = 19,

		--占卜战斗
		DIVINATION = 20,

		--资源战
		RESOURCE_WAR = 21,

		--公会低级
		UNION_LOW = 22,

		--公会高级
		UNION_TALL = 23,

		------特殊------
		----------------
		--新手副本
		LEAD = 100,

		--1V1
		PVP = 101,
	},

	--场景类型(用于多人同屏场景)
	SceneType = 
	{
		BEG_TAG = 201,

		--组队大厅
		TEAM_HALL = 201,

		--组队大地图
		TEAM_WORLD_MAP = 202,

		--组队采集场景
		TEAM_GATHER = 203,

		--组队普通怪遭遇场景
		TEAM_NORMAL_ENCOUNTER = 204,

		--组队BOSS怪遭遇场景
		TEAM_BOSS_ENCOUNTER = 205,
	},

	--副本出生方式
	FBBirthType = 
	{
		--静态站位
		STATIC = 0,
		--跑步进入
		RUN_INTO = 1,	
	},

	--副本奖励类型
	FBRewardType = 
	{
		--关卡宝箱
		GAMELEVEL = 1,
		--星级宝箱
		STAR = 2,
	},

	--单个关卡星级上限
	FBMaxStarPerMap = 3,

	--技能类型
	SkillType = 
	{
		--普攻
		NORMAL = 1,
		--近身技能(远程职业专属)
		NEAR = 2,
		--特殊技能
		SPECIAL = 3,
		--怒气技能
		ANGER = 4,
		--合击技能(主将)
		JOINT = 5,
		--合击技能(副将)
		JOINT_ATTACHED = 6,
		--真.怒气技能
		ANGER_PLUS = 7,
		--真.合击技能(主将)
		JOINT_PLUS = 8,
		--精灵技能
		SPRITE = 9,
	},

	--特殊技能类型(用于怪物特殊AI)
	SpSkillType =
	{
		--正常技能
		NORMAL = 0,

		--假.怒气/合击技能(除了不消耗怒气，与怒气/合击技能一致)
		FALSE_ANGER_OR_JOINT = 1,

		--不可打断技能
		UN_INTERRUPTABLE = 2,
	},

	--技能作用阵营
	SkillTgtCamp = 
	{
		--友方
		FRIEND = 1,
		--敌方
		ENEMY = 2,
	},

	--技能命中目标逻辑
	--[[
		1 近战目标(1|类型|抬手距离|伤害距离，0=单体；1=群体)
		2 全体随机N个(2|N)
		3 范围(3|范围左边界|范围右边界，目标进入范围内释放)
		4 全体(4)
		5 特殊目标（5|选取目标|范围，范围配0表示特定目标）
		    1 生命值最低(5|1)
		    2 最后排1个(5|2)
		    3 职业(5|3|职业类型)
		    4 最前排1个(5|4)
			5 随机范围(5|5|随机点类型|范围)
			（随机点类型：0当前镜头内随机，1施法者到对面边界内随机，2外部输入点）		    
    --]]	
	SkillTgtLogic = 
	{
		NEAR = 1,
		RAND_IN_ALL = 2,
		RANGE = 3,
		ALL = 4,
		SPECIAL = 5,
	},

	--BUFF命中目标逻辑
	--[[
		1 根据技能命中目标逻辑查找目标
		2 特殊敌方目标/特殊敌方范围（2|选取目标|范围，范围配0表示特定目标）
		    选取目标：
		        0 = 最近敌人
		        1 = 最远敌人
		        2 = 血量最少敌人
		3 特殊我方范围（3|选取目标|范围，范围配0表示特定目标）
		    选取目标：
		        0 = 自身
		        1 = 最前队友
		        2 = 最后队友
		        3 = 血量最少队友
		4 己方全体（4）
		5 不指定目标的图腾BUFF（5）
	--]]
	BuffTgtLogic = 
	{
		BY_SKILL_TGT_LOGIC = 1,
		SP_ENEMY = 2,
		SP_TEAMMATE = 3,
		ALL_TEAMMATE = 4,
		BUFF_TOTEMS_FOR_SPRITE = 5,
	},

	--[[
		精灵祝福技能触发模式
		1|技能类型|触发几率=使用对应类型且作用于敌方的技能时触发	
		2|技能类型|触发几率=受到对应类型且作用于敌方的技能时触发	
		3|技能类型|触发几率=使用对应类型且作用于友方的技能时触发
		4|buff类型|触发几率=受到对应类型的buff命中时触发	
		5|触发几率=回血时触发
	--]]
	SpiritBlessSkillTriggerMode = 
	{
		ATTACK_WITH_SKILL = 1,
		BE_ATTACKED_BY_SKILL = 2,
		TREAT_WITH_SKILL = 3,
		BE_ATTACHED_BY_BUFF = 4,
		BE_TREATED = 5,
	},

	--对象类型
	ObjType = 
	{
		--货币
		CURRENCY = 0,
		--物品
		ITEM = 1,
		--装备
		EQUIP = 2,
		--英雄
		HERO = 3,
		--圣器
		TREASURE = 4,
		--精灵
		SPIRIT = 5,
		--觉醒
		AWAKEN_TOOL = 6 ,
		--占卜
		FAIRY = 7,
		--宝石
		GEM = 8 ,

	},

	--物品大类型
	ItemBigType = 
	{
		NORMAL = 1,			--	普通
		GIFT = 2,			--	礼包
	},

	--普通物品类型	
	NormalItemSubType = 
	{
		VIGOR = 1,			--	精力
		FORM = 2,			--	体力
		REFINE = 3,			--	装备精炼材料
		ADVANCE = 4,		--	英雄进阶材料
		TRAIN = 5,			--	英雄培养材料
		POTENCY = 6,		--  英雄潜能材料
		EQUIPDEBRIS = 7,	--	装备碎片
		HERODEBRIS = 8,		--	英雄碎片
		TREASURE_DEBRIS = 13,	--	圣器碎片
		PLUNDERED_TREASURES = 14, --夺宝免战物品
		WANTED = 15,			  --通缉令
		HERO_RECRUIT = 16,		  --英雄招募券
		ELITE_HERO_RECRUIT = 17,  --精英招募券
		SPIRIT = 18,			  --精灵契约
		CURRENCY = 20,			  --货币（参数1=货币类型；参数2=货币值）
		SPIRIT_TRAIN = 21,		  --精灵培养材料
		SPIRIT_SUBLIMATE = 22,	  --精灵升华材料
		SPIRIT_INTENSIFY = 23,	  --精灵强化材料
		SPIRIT_DEBRIS = 24,	  	  --精灵碎片
	},

	--礼包物品类型
	GiftItemSubType = 
	{
		RANDOM = 1,			--	随机礼包
		MCQ	= 2 ,			--	多选礼包
		FIXATION = 3,		--	固定礼包
	},

	--装备类型
	EquipType = 
	{
		WEAPON = 0,			-- 武器
		HELMET = 1,			-- 衣服
		BELT = 2,			-- 鞋子
		CLOTH = 3,			-- 护腕
		
	},

	--圣器类型
	TreasureType = 
	{
		EMBLEM = 0 , 			--符文
		GAD = 1,				--徽章
		EXP = 2, 				--经验
	},

	--战斗属性
	BattleProp = 
	{
		ATTACK = 1,							-- 1=攻击	
		PHYSICAL_DEFENCE = 2,				-- 2=物防
		MAGIC_DEFENCE = 3,					-- 3=法防
		HP = 4,								-- 4=生命
		DAMAGE_DECREASE_RATE = 5, 			-- 5=百分比减伤%	
		DAMAGE_DECREASE_VALUE = 6,			-- 6=固定值减伤
		DAMAGE_INCREASE_RATE  = 7,			-- 7=伤害加成%
		DAMAGE_INCREASE_VALUE = 8 ,			-- 8=伤害附加
		PENETRATION_RATE = 9 ,				-- 9=百分比穿透%
		PENETRATION_VALUE = 10,				-- 10=固定值穿透
		
		HIT_RATE = 11,						-- 11=命中率%
		DODGE_RATE = 12,					-- 12=闪避率%

		CRIT_RATE = 13,						-- 13=暴击率% 
		CRIT_DEFENCE_RATE = 14,				-- 14=抗暴率%
		CRIT_RATE_CORRECT = 15,				-- 15=暴击强度%	
		CRIT_DEFENCE_RATE_CORRECT = 16,		-- 16=抗暴击强度%

		BLOCK_RATE = 17,					-- 17=格挡率%
		BLOCK_RATE_CORRECT = 18,			-- 18=格挡强度%
		BEAT_BACK_CORRECT = 19,				-- 19=击退强度%
		BEAT_BACK_DEFENCE_CORRECT = 20,		-- 20=抗击退强度%

		ANGER = 21,							-- 21=怒气		

		ATTACK_RATE = 22,					-- 22=攻击加成				
		PHYSICAL_DEFENCE_RATE = 23,			-- 23=物防加成				
		MAGIC_DEFENCE_RATE = 24,			-- 24=法防加成				
		HP_RATE = 25,						-- 25=血量加成				

		SPEED = 26,							-- 26=速度

		ATTACK_RATE_FINAL = 27,				-- 27=攻击最终加成%
		PHYSICAL_DEFENCE_RATE_FINAL = 28,	-- 28=物防最终加成%
		MAGIC_DEFENCE_RATE_FINAL = 29,		-- 29=法防最终加成%
		HP_RATE_FINAL = 30, 				-- 30=血量最终加成%		
	},

	--战斗结算条件(用于爬塔、日常副本)
	BattleWinCondition = 
	{
		--己方血量不低于
		MY_HP_MORE_THAN = 1,

		--我方死亡人数不超过x
		MY_DEAD_NUM_LESS_THAN = 2,

		--x秒内获胜
		TIME_LESS_THAN = 3,

		--时间结束之后根据伤害算奖励
		KILL_ALL_ENEMY = 4,

		--常规
		NORMAL = 5,
	},

	--[[
	新手指引触发条件类型
		1、 战役开始（1|关卡ID|战役索引）
		2、 所有怪物被击杀（2|关卡ID|战役索引）
		3、 击杀怪物（3|关卡ID|战役索引|怪物ID）
		4、 怒气达到X（4|关卡ID|固定值|索引）
		5、 通关一个副本（5|关卡ID）
		6、 某个系统开启（6|系统ID）
		7、 角色名改变
		8、 阵位开启（8|阵位索引）
		9、 圣器第一次满足合成
		10、援军阵位开启（10|援军阵位索引）
		11、第一次通关爬塔
		12、精灵系统开启后第一次进入副本
		13、怒气达到X（13|关卡ID|万分比）
		14、首次战斗失败
		17、第一次打开选择好友助战
	--]]
	LeadConditionType = 
	{
		BATTLE_START = 1,
		ALL_MONSTER_DEAD = 2,
		MONSTER_DEAD = 3,
		ANGER_ACHIEVE_FIXED = 4,
		PASS_GAME_LEVEL = 5,
		SYSTEM_OPENED = 6,
		ROLE_NAME_CHANGED = 7,
		HERO_LINEUP_POS_OPENED = 8,
		TREASURE_COMPOSE_FIRST_TIME = 9,
		REINFORCEMENT_LINEUP_POS_OPENED = 10,
		TOWER_GAMELEVEL_PASSED_FIRST_TIME = 11,
		BATTLE_START_AFTER_SPIRIT_OPENED = 12,
		ANGER_ACHIEVE_PERCENT = 13,
		BATTLE_FAILED_FIRST_TIME = 14,
		HERO_ADVENT_FIRST_TIME_OPENED = 15,
		CHASM_PASSED_FIRST_TIME = 16,
	},	

	--[[
	新手指引操作类型
		1、主UI入口
		2、界面选项卡
		3、界面关闭按钮
		4、界面控件
		5、子界面控件
		6、模板控件
		7、特殊	
	--]]
	LeadOperateType = 
	{
		CITY_ENTRY = 1,
		VIEW_TAB = 2,
		VIEW_CLOSE_BTN = 3,
		VIEW_CTRL = 4,
		TAB_VIEW_CTRL = 5,
		TEMPLATE_CTRL = 6,
		SPECIAL = 7,
	},

	--新手指引特殊ID
	LeadSpecialID =
	{
		--战斗加速
		BATTLE_ACC = 10,

		--释放强化版怒气/合击技能
		RELEASE_ANGER_OR_JOINT_PLUS_SKILL = 98,

		--释放精灵技能
		RELEASE_SPRITE_SKILL_1 = 99,
		RELEASE_SPRITE_SKILL_2 = 100,
		RELEASE_SPRITE_SKILL_3 = 101,
		RELEASE_SPRITE_SKILL_4 = 102,
		RELEASE_SPRITE_SKILL_5 = 103,

		--好友助战指引
		FRIEND_ASSIST_LEAD = 104,

		--英雄降临弹出
		HERO_ADVENT_POPUP_ONE = 105,
		HERO_ADVENT_POPUP_TWO = 106,
		HERO_ADVENT_POPUP_THREE = 107,

		--助战界面选择好友界面
		ASSIST_CHOOSE_FRIEND = 46,

		--释放强化版怒气/合击技能
		RELEASE_ANGER_OR_JOINT_PLUS_SKILL_WEAK = 96,
		RELEASE_ANGER_OR_JOINT_PLUS_SKILL_WEAK2 = 97,
	},

	--新手指引特殊操作ID
	LeadOperateSpecialID = 
	{
		--选择主线章节（章节索引，从1开始）
		MAIN_DUNGEON_SELECT_CHAPTER = 701,
		--选择主线关卡（关卡ID）
		MAIN_DUNGEON_SELECT_GAMELEVEL = 702,
		--选择主线星级宝箱（宝箱索引，从1开始）
		MAIN_DUNGEON_SELECT_STAR_BOX = 703,
		--选择主线关卡宝箱（关卡ID）
		MAIN_DUNGEON_SELECT_GAMELEVEL_BOX = 706,
		--英雄释放怒气技能
		HERO_RELEASE_ANGER_SKILL = 704,
		--英雄释放合击技能
		HERO_RELEASE_JOINT_SKILL = 705,
		--英雄上阵按钮
		HERO_LINEUP_BTN = 707,
		--英雄出战按钮（英雄ID）
		HERO_BATTLE_BTN = 708,
		--阵容切换英雄
		LINEUP_CHANGE_HERO = 709,
		--阵容选择英雄
		LINEUP_SELECT_HERO = 710,
		--英雄升级添加材料
		HERO_UPGRADE_ADD_MATERIAL = 711,
		--英雄升级选择材料
		HERO_UPGRADE_SELECT_MATERIAL = 712,
		--英雄进阶继续
		HERO_BREAK_CONTINUE = 714,
		--英雄招募召唤1次
		HERO_RECRUIT_ONE_TIME = 715,
		--背包使用物品（物品ID）
		PACK_USE_ITEM = 716,
		--背包使用礼包（物品ID）
		PACK_USE_GIFT = 717,
		--战队升级预告前往（第一个）
		ROLE_UPDATE_GOTO = 718,
		--日常成就前往（成就ID）
		ACHIEVEMENT_GOTO = 719,
		--日常成就领奖（成就ID）
		ACHIEVEMENT_GET_REWARD = 720,
		--阵容装备格子（装备类型）
		LINEUP_EQUIP_GRID = 721,
		--阵容圣器格子（圣器类型）
		LINEUP_TREASURE_GRID = 722,
		--装备穿戴（格子索引，从1开始）
		EQUIP_WEAR = 724,
		--竞技场挑战最后一个
		ARENA_CHALLENGE_LAST = 725,
		--英雄回收-自动选择
		HERO_RECOVERY_AUTO_CHOICE = 726,
		--英雄回收-分解
		HERO_RECOVERY_DECOMPOSE = 727,
		--圣器争夺-选择圣器（圣器ID）
		PLUNDERED_TREASURES_CHOOSE_TREASURE = 729,
		--圣器争夺-选择碎片（碎片ID）
		PLUNDERED_TREASURES_CHOOSE_FRAGMENT = 730,
		--挑战界面-选择入口（系统ID）
		CHALLENGE_CHOOSE_ENTRY = 733,
		--爬塔界面-选择关卡（爬塔关卡索引）
		TOWER_CHOOSE_GAMELEVEL = 734,
		--爬塔商店-购买（商品ID）
		TOWER_SHOP_BUY = 735,
		--装备界面-装备（格子索引，从1开始）
		EQUIP_CHOOSE = 736,
		--装备界面-碎片-合成（格子索引，从1开始）
		EQUIP_FRAGMENT_COMPOSE = 737,
		--选择英雄章节（章节索引，从1开始）
		HERO_DUNGEON_SELECT_CHAPTER = 739,
		--选择英雄副本关卡（关卡ID）
		HERO_DUNGEON_SELECT_GAMELEVEL = 740,
		--阵容-援军
		LEINUP_REINFORCEMENTS_ENTRY = 741,
		--阵容-援军-阵位（阵位索引，从1开始）
		LEINUP_REINFORCEMENTS_POS = 742,
		--圣器争夺-敌人列表-抢夺（第一个机器人）
		PLUNDERED_TREASURES_ENEMYLIST_GRAB = 743,
		--英雄商店-购买商品（商品索引，从1开始）
		HERO_SHOP_BUY = 744,
		--英雄培养-培养
		HERO_TRAIN_BTN_TRAIN = 745,
		--英雄培养-替换
		HERO_TRAIN_BTN_REPLACE = 746,
		--英雄提升-选择英雄
		HERO_PROMOTE_SELECT_HERO = 748,
		--圣器穿戴（格子索引，从1开始）
		TREASURE_WEAR = 749,
		--主界面-英雄阵位（索引，从1开始）
		CITY_HERO_LINEUP_BTN = 750,
		--装备精炼，精炼石品质（索引，从1开始）
		EQUIP_REFINE = 751,
		--战队升级-继续
		TEAM_UPDATE_CONTINUE = 752,
		--声望商店购买（商品ID）
		ARENA_REPUTATION_SHOP = 753,
		--英雄获得展示界面-继续
		HERO_OBTAIN_VIEW_CONTINUE = 754,		
		--召唤英雄展示点击继续
		HERO_RECRUIT_HERO_OBTAIN_VIEW_CONTINUE = 755,		
		--竞技场排名奖励点击继续
		ARENA_RANK_BREAK_CONTINUE = 756,		
		--主线关卡界面-阵容
		MAIN_DUNGEON_GAMELEVEL_HERO_LINEUP = 757,		
		--主线关卡界面-排行榜
		MAIN_DUNGEON_GAMELEVEL_RANK = 758,	
		--战斗失败界面-战力提升入口
		BATTLE_FAILED_POWER_PROMOTE_ENTRY = 759,	
		--日常活动领取等级礼包（礼包ID）
		ACTIVITY_GET_REWARD = 760,
		--世界之心激活天赋界面点击继续
		WORLD_HEART_TALENT_CONTINUTE = 761,
		--深渊商店-购买（商品ID）
		CHASM_SHOP_BUY = 762,
		--爵位系统-激活徽章
		KNIGHTHOOD_LIGHT_UP = 763,
		--结算界面-点击继续
		BATTLE_RESULT_CONTINUE = 764,
		--好友助战-点击继续
		FRIEND_ASSIST_CONTINUE = 765,
		--精灵界面-跳转精灵
		SPIRIT_SKIP_SPIRIT = 423,
		--精灵界面-选择精灵
		SPIRIT_CHOOSE_SPIRIT = 766,
		--夺宝界面-退出按钮
		PLUNDERED_TREASURES_EXIT_BTN = 767,
		--主界面点击建筑
		MAINCITY_CLICK_BUILDING = 768,
		--声望商店-奖励
		ARENA_SHOP_TAB_RANK_ENCOURAGE = 769,
	},

	--新手指引进度标记类型
	LeadProgressTagType = 
	{
		--1 ~ 10000：新手指引进度
		--新手指引进度上限
		LEAD_MAX_LEAD = 10000,

		--100000 ~ 1000000：通关记录
		LEAD_PASS_GAMELEVEL_BEG = 100000,

		--1000001 ~ 正无穷：其他
		--爬塔副本首次通关
		LEAD_TOWER_DUNGEON_PASSED = 1000001,
		--圣器首次满足合成条件
		LEAD_TREASURE_COMPOSABLE_FIRST_TIME = 1000002,
		--精灵系统开启后第一次进入副本
		LEAD_BATTLE_START_AFTER_SPIRIT_OPENED = 1000003,
		--首次战斗失败
		LEAD_BATTLE_FAILED_FIRST_TIME = 1000004,		

		--深渊首次通关
		CHASM_PASSED_FIRST_TIME = 1000006,

		--1001000 ~ 1002000 主线副本章节弹窗
		--英雄降临弹出
		LEAD_HERO_ADVENT_FIRST_TIME = 1001000,
	},

	--特殊系统ID(用于系统操作音效)
	SystemIDForSound = {
		COMMON = 10000,
		TAB = 20000,
	},	
	
	--品质类型
	QualityType = 
	{		
		WHITE = 0,
		GREEN = 1,
		BLUE = 2,
		PURPLE = 3,
		ORANGE = 4,
		RED = 5,

		MAX = 5,
	},

	--聊天
	CHAT = 
	{
		--私聊
		PRIVATE = 1,
		--世界
		WORLD = 2,
		--公会
		UNION = 3,
		--队伍
		TEAM = 4,
		
		--跨服boss
		BOSS = 6 ,
	},

	--商店
	SHOP = 
	{
		--商店道具
		STOREPROP = 1,		
		--商店礼包
		STOREOEM = 2,
		--公会道具
		UNIONPROP = 3,
		--公会限时
		UNIONTIMEPROP = 4,
		--公会奖励
		UNIONAWARDPROP = 5,
		--雄魂商品
		SOUL = 6,
		--叛军商品
		REBEL = 7,
		--爬塔商品
		TOWER = 8,
		--竞技场商品
		ARENA = 9,
		--觉醒商店
		AWAKEN_SHOP = 10,
		--星魂商品
		STAR_SOUL = 11,
		--积分赛商店
		SHOP_SCORE_MATCH = 12,
		--活动打折
		ACTIVITY_DISCOUNT = 13,
		--等级礼包购买
		LEVEL_GIFTBAG = 14,
		--无尽之地道具商店
		CHASM_SHOP = 16,
		--无尽之地奖励商店
		CHASM_AWARD = 17,
		--直购特惠
		DIRECT_BUY = 18,
		--兑换特惠
		CONCERT_BUY = 19,
		--推荐商店
		SHOP_RECOMMEND = 20,
		--原石道具
		GEM_PROP = 21,
		--原石奖励
		GEM_REWARD = 22,
		--命运商店
		DESTINY =  23 ,
		--隐藏商店
		HIDE =  24 ,
		--圣诞节积分兑换商店
		CHRISTMAS_INTEGRAL =  25 ,
		--圣诞节物品兑换商店
		CHRISTMAS_PROP =  26 ,
		--水晶
		CRYSTAL = 27 ,
		--活动热卖
		DIRECT_HOT_BUY = 28 , 
		--资源战
		RESOURCE_WAR_SHOP = 29 ,
		--GM商店
		GM_SHOP = 30,
	},

	RankType ={
		-- 战力
		BATTLE = 1,
		-- 等级
		LEVEL = 2,
		-- 爬塔
		KRUUNU = 3,
		-- 竞技场
		ARENA = 4,
		-- 叛军伤害排行榜
		REBEL_HARM = 5,
		-- 叛军功勋排行榜
		REBEL_FEATS = 6,
		-- 世界boss 荣誉排行榜
		WORLDBOSS_HONOR = 7,
		-- 世界BOSS伤害排行榜
		WORLDBOSS_HARM = 8,
		-- 积分赛排行榜(超能)
		INTEGRAL_RACE_SUPER = 10,
		-- 主线副本星数
		MAIN_DUNGEON_STAR = 11,
		-- 本服排行
		ONESELF_SERVER = 12,
		-- 全服排行
		ALL_SERVER = 13,
		-- 怪物攻城
		MONSTER_STORMCITY = 14,
		-- 无尽
		CHASM = 15,
		--天梯
		LADDER = 16 ,
		--占卜
		CARD = 17 ,
		--排行
		RANK_TURNTABLE_LOTTERY = 18 ,
		-- 积分赛排行榜(传奇(传说))
		INTEGRAL_RACE_LEGEND = 19,
		-- 积分赛排行榜(魔兽(幻想))
		INTEGRAL_RACE_WARCRAFT = 20,
		--公会战
		UNION_WAR = 21 ,

		--消费
		CONSUME = 22 ,
		--充值
		RECHARGE = 23 ,
	},

	TowerShop = {
		--商品
		SHANGPING = 1,
		--紫装
		ZIZHUANG = 2,
		--橙装
		CHENGZHUANG = 3,
		--奖励
		JIANGLI = 4,
		--红装
		HONGZHUANG = 5 ,
	},

	VIPType = {
        INTENSIFY_CRIT = 1,                 --强化暴击
        DISPOSE_RIOT_TIME = 2,              --处理暴动次数
        MAINLINE_RESET = 3,                 --主线重置次数
        KRUUNI_RESET = 4,                   --爬塔重置次数
        HERO_SHOP_UP_MAX = 5,               --英雄商店刷新上限
        HERO_TRANSCRIPT = 6,                --英雄副本次数
        BUY_WANTED_MAX = 7,                 --通缉令购买上限
        BUY_VIGOR_MAX = 8,                  --精力丹购买上限
        BUY_MUSCLE_MAX = 9,                 --体力丹购买上限
        BUY_DOG_FOOD_MAX = 10,              --经验英雄购买上限

        BUY_GOLD_MAX = 11,                  --金币购买上限
        BUY_EQUIP_MAX = 12,                 --橙品装备箱购买上限
        BUY_ARTIFACT_MAX = 13,              --橙品圣器箱购买上限
        BUY_KRUUNU_ELITE_MAX = 14,          --爬塔精英购买上限
        BATTLE_ACC_MAX = 15,                --开启战斗3档加速
        HERO_PACK_MAX = 16,                 --英雄背包总上限
        EQUIP_PACK_MAX = 17,             	--装备背包总上限
        ESTABLISH_UNION = 18,               --开启公会创建权限
        EXPERT_PATROL = 19,                 --开启高级巡逻

        FINAL_PATROL = 20,                  --开启终极巡逻
        KRUUNU_SWEEP = 21,                  --开启爬塔扫荡
        OPEN_ARTIFACT_REPEATEDLY = 22,      --开启圣器抢夺5次
        OPEN_MAIN_DUNGEON_CLEAN = 23,       --开启主线副本扫荡
        TREASURE_PACK_MAX = 24 ,			--圣器背包上限
        AWAKEN_SHOP_MAX = 25 ,				--觉醒商店上限

        WORLDBOSS_BUY_TIME = 26 ,			--世界boss体力购买次数
        INTEGRAL_RACE_BUY_TIME = 27 ,		--积分赛可购买的挑战次数次数
        ELITELINE_RESET = 28,                 --精英副本重置次数
        UNION_TRANSCRIPT_BUY_TIME = 31 ,			--公会副本挑战次数
        TEAM_DUNGEON_FIGHT_TIME = 32,		--组队副本挑战次数
        DIVINATION_PACK_MAX = 33,             	--卡牌背包总上限

        ONE_KEY_TOWER = 34,             	--一键爬塔扫荡

        CHASM_TIMES = 36,	             	--深渊次数
    },
    
  	SpecialItem = {
        MUSCLE = 10000,  					--体力药剂
        VIGOR = 10001,  					--精力药剂
        LOW_AVOIDWAR = 10016,	--初级免战令
        HIGH_AVOIDWAR = 10017,	--高级免战令
        WANTED = 10015,	--通缉令
    },

    --英雄信息
    HeroMessage_Type = {
    	SKILL = 1, --技能
    	REINFORCEMENT = 2, --宿命
    	TALENT = 3,	--天赋
	},
	
	--请求英雄详细信息类型
    HeroMessageReq_Type = {
    	FRIEND = 1, --好友
    	RANK = 2, --排行榜
    	CHAT = 3, --聊天
	},

	--好友类型
    FRIEND_TYPE = {
    	FRIEND_LIST = 1, --好友列表
    	GET_ENERY = 2, --领取奖励
    	ADD_FRIEND = 3, --添加好友
    	APPLY_LIST = 4,	--申请列表
    	BLACK_LIST = 5,	--黑名单
	},

	--势力
	FORCE = {
			CAMP_SUPER = 0,		-- 超能
			CAMP_LEGEND = 1,	-- 传说
			CAMP_WARCRAFT = 2,	-- 幻想
			CAMP_INVALID = 99,	-- 无效阵营类型
	},

	--竞技场商店类型
	ARENA_SHOP = {
		REPUTATION_SHOP = 1, --声望商店
		RANK_ENCOURAGE = 2,	 --排名奖励
	},

	--觉醒星级上限
	HERO_AWAKE_STAR_MAX = 5,
	--觉醒等级上限
	HERO_AWAKE_LEVEL_MAX = 9,
	--英雄招募类型
	HERO_RECRUIT = {
		LOW_RECRUIT = 1,  --初级召唤
		HIGH_RECRUIT = 2, --高级召唤
		MORE_RECRUIT = 3, --批量招募
		DUNGEON_SHOW = 4, --副本英雄展示
		VIP_RECRUIT = 5,  --VIP招募
	},

	--招募详细类型
	HERO_RECRUIT_DETAIL_TYPE = {
		UPDATE_RECRUIT_TIME = 0, --更新招募时间
		LOWFREE_RECRUIT = 1,  --普通免费召唤
		LOWPROP_RECRUIT = 2,	 --普通道具召唤
		LOWMOREPROP_RECRUIT = 3, --普通道具批量召唤
		HIGHFREE_RECRUIT = 4,  --高级免费召唤
		HIGHPROP_RECRUIT = 5,  --高级道具召唤
		HIGHMONEY_RECRUIT = 6, --高级金钱召唤
		HIGHMOREMONEY_RECRUIT = 7,  --高级金钱批量召唤		
		FRIENDPOINT_RECRUIT = 8,	--友情点单次招募
		FRIENDPOINT_MORERECRUIT = 9,--友情点批量招募
		HIGHMOREPROP_RECRUIT = 10, --高级道具批量召唤
		VIP_RECRUIT = 11, 		--高级召唤
		RECRUIT_ONE_TIME = 100,	--召唤一次数据调用
	},

	--英雄培养货币消耗类型
	HERO_TRAIN_COST_TYPE = {
		ONLY_ITEM = 1,			--丹药
		ITEM_AND_GOLD = 2,		--丹药和金币
		ITEM_AND_DIAMOND = 3,	--丹药和钻石
	},

	--飘字类型
	WIND_WORDS_TYPE = {
		UPSLOPE = 1, --往上飘
		MOVING_TO_LOCATION = 2,	--移向某个位置
	},

	--阵容飘字信息类型
	WIND_WORDS_LINEUP_TYPE = {
		EQUIP_MESSAGE = 1,
		TREASURE_MESSAGE = 2,
		LINEUP_MESSGAE = 3,
		REINFORCEMENT_MESSAGE = 4,
		EQUIP_ONEKEY_ENHANCE_MESSAGE = 5,
		DIVINATION_MESSAGE = 6,
	},

	--特殊逻辑中的弹窗优先级(数值越小优先级越高)
	SpDialogPriority = 
	{	
		--指引
		LEAD = 90,

		--战队升级界面
		ROLE_UPGRADE = 100,

		--章节通关效果
		CHAPTER_PASSED_ALL = 110,
	},

	--系统ID(同x系统入口.xml中的系统ID)
	SystemID = {
		-- 征战
		DUNGEON 								= 1 ,
		-- 主线关卡
		DUNGEON_GAMELEVEL						= 26 ,
		-- 英雄关卡
		HERO_DUNGEON_GAMELEVEL					= 27 ,
		-- 精英关卡
		DUNGEON_ELITE							= 28 ,
		-- 副本扫荡
		DUNGEON_CLEAN_UP 						= 29 ,
		-- 日常副本
		DUNGEON_DAILY							= 2 ,

		-- 物品背包
		Pack									= 4 ,

		-- 装备背包
		EQUIP									= 5 ,
		-- 装备穿戴
		EQUIP_DRESS								= 37 ,
		-- 装备强化
		EQUIP_INTENSIFY							= 38 ,
		-- 装备精炼
		EQUIP_REFINE							= 39 ,

		-- 圣器背包
		TREASURE								= 6 ,
		-- 圣器穿戴
		TREASURE_DRESS							= 41 ,
		-- 圣器强化
		TREASURE_INTENSIFY						= 42 ,
		-- 圣器精炼
		TREASURE_REFINE							= 43 ,

		-- 英雄背包
		HERO_BAG								= 7 ,
		-- 英雄阵容
		HERO_LINEUP								= 8 ,
		-- 英雄召唤
		HERO_RECRUIT							= 9 ,
		-- 英雄图鉴
		HERO_HANDBOOK							= 10 ,
		-- 英雄商店
		HERO_SHOP								= 11 ,
		-- 英雄升级
		HERO_UPGRADE							= 35 ,
		-- 英雄进阶
		HERO_BREAK								= 36 ,
		-- 英雄培养
		HERO_TRAIN								= 40 ,
		-- 英雄潜能
		HERO_POTENTIAL							= 44 ,
		-- 英雄觉醒
		HERO_AWAKEN								= 45 ,

		-- 挑战
		CHALLENGE								= 25 ,
		-- 叛军
		REBEL									= 16 ,
		-- 勇者试炼
		TOWER									= 17 ,
		-- 圣器争夺
		PLUNDEREDTREASURES						= 19 ,
		-- 寻访遗迹
		MANOR									= 20 ,
		-- 竞技场
		ARENA									= 21 ,
		-- 世界BOSS
		WORLDBOSS								= 47 ,

		-- 商城
		SHOP									= 12 ,
		-- 声望商店
		ARENA_REPUTATIONSHOP					= 30 ,
		-- 神装商店
		TOWER_SHOP								= 33 ,
		-- 战功商店
		REBEL_SHOP								= 34 ,
		-- 觉醒商店
		HERO_AWAKEN_SHOP						= 46 ,

		-- 邮件
		EMAIL									= 13 ,
		-- 排行榜
		RANK									= 14 ,
		-- 好友
		FRIEND									= 15 ,

		-- 世界之心
		WORLD_HEART								= 18 ,

		-- 军团
		UNION									= 22 ,
	
		-- 回收
		RECYCLE									= 23 ,
	
		-- 领奖中心
		ACCEPT_PRIZE_CENTER						= 24 ,

		-- 成就任务
		ACHIEVEMENT								= 48 ,
		-- 日常任务
		DALIYTASK								= 3 ,
		-- 日常任务与成就
		DALIYTASK_AND_ACHIEVEMENT				= 56 ,

		-- 主界面
		MAIN_CITY								= 49 ,

		--VIP
		VIP 									= 50,

		--每日签到
		DAILY_SIGNIN 							= 51,
		
		--七天活动
		SEVEN_DEYS								= 52,

		--铜雀台
		TQT										= 53,

		--摇钱树
		YCS 									= 54,

		--日常活动
		ACTIVITY 								= 55,	

		--等级商店
		LEVEL_SHOP	 							= 57,	

		--功能预告
		SYSTEM_FORESHOW 						= 58,		

		--等级礼包
		LEVEL_GIFTBAG 							= 59,	

		--爬塔扫荡
		TOWER_SWEEP 							= 60,

		--援军
		REINFORCEMENT     						= 61,

		--CDK
		CDKEY 									= 62,

		--精灵
		SPIRIT 									= 63,
		--精灵强化
		SPIRIT_ENHANCE 							= 64,
		--精灵升华
		SPIRIT_SUBLIMATE 						= 87,
		--精灵祝福
		SPIRIT_BLESS 							= 138,

		--好友助战
		FRIEND_ASSIST							= 65,
		--推荐阵容
		RECOMMEND_LINEUP 						= 66,	
		--背包扩展
		PACK_EXTEND								= 67,	
		--商店扩展
		SHOP_EXTEND								= 68,
		--军团商店
		UNION_SHOP								= 69,
		--充值
		CHARGE									= 70,
		--英雄降临
		HERO_ADVENT								= 71,
		--pvp
		PVP 									= 72,
		--我要变强
		STRONGER								= 73,

		--单充
		ONE_RECHARGE							= 74,
		--累充
		GRAND_TOTAL_RECHARGE					= 75,
		--首充
		FIRST_RECHARGE 							= 76,  
		--限时招募
		RECRUIT									= 77,
		-- 限时活动
		DESENO_ACTIVITY 						= 78,
		-- VIP福利
		VIP_WELFARE 							= 79,
		-- 月卡福利
		LUNA_WELFARE 							= 80,
		-- 全民福利
		FUND									= 81,
		-- 等级基金
		REBATE 									= 82,
		--怪物攻城
		MONSTER_STORMCASTLE 					= 84,  
		--限时副本汇总(组队、PVP、世界BOSS、怪物攻城)
		LIMIT_DUNGEON							= 86,  
		--无尽深渊
		CHASM 									= 88,
		--七天活动II
		SEVEN_DEYS_II							= 89,
		--次日登录
		MORROW 									= 90,		
		--限时招募推送
		DESENO_RECRUIT 							= 91,
		--爵位
		KNIGHTHOOD 								= 92,
		--豪华充值
		LUXURY									= 93,
		--超值折扣
		SUPERDISCOUNTVIEW						= 94,
		--直购特惠
		DIRECT_BUY								= 95,
		--直购特惠推送界面
		DIRECT_BUY_SHOW							= 96,
		--冲榜
		RANK_ACTIVITY							= 97,
		--限时活动
		LIMIT_ACTIVITY							= 98,
		--精准推送
		DIRECT_HOT_RECOMMENT 					= 99,
		--直购特惠
		SPECIAL 								= 100,
		--原石
		GEM 									= 101,
		--组队
		TEAM                                    = 102,
		--宝石商店
		RAW_GEMSTONE_SHOP                       = 103,
		--精英爬塔
		TOWER_ELITE 							= 105,
		--登录奖励
		REGISTER_AWARD							= 106,
		--万圣节
		HALLOWEEN 								= 107,
		--天梯赛
		LADDER 									= 108,
		--双十一
		SINGLEDOG 								= 109,
		--组队邀请
		TEAM_INVITE 							= 110,
		--一键夺宝
		ONE_KEY_PLUNDEREDTREASURES 				= 111,		
		--一键爬塔
		ONE_KEY_TOWER 							= 112,	
		--占卜	
		DIVINATION 								= 113,
		--卡牌穿戴
		DIVINATION_DRESS 					    = 114,
		--卡牌背包
		DIVINATION_PACK 					    = 115,
		--星魂商店
		DIVINATION_SHOP							= 117,
		--精英副本外域入侵
		ELITE_DUNGEON_BREAK 					= 118,
		--圣诞活动
		CHRISTMAS_ACTIVITY 						= 119,
		--角色信息
		ROLE_INFO 								= 124,
		--七日登录
		PUSH_SEVEN								= 126,
		--充值回馈
		FEEDBACK								= 127,
		--女王
		QUEEN									= 128,
		--自定义超值购
		CUSTOM_PURCHASE 						= 130,

		--活动热卖
		HOT_SALE 								= 132,
		--转盘
		TURNTABLE 								= 135,
		--水晶商店
		CRYSTAL_SHOP							= 141,
		--世界等级任务
		PUSH_WORLDLEVEL							= 142,

		--海外要求
		--好友召唤
		SUMMON_FRIENDS	 						= 120,
		--好友邀请
		INVITE_FRIENDS 							= 121,
		--FaceBook分享
		FACEBOOK_SHARE_REWARD 					= 122,
		--在线奖励
		ONLINE_AWARD 							= 125,
		--春节活动
		SPRING_FESTIVAL 						= 129,
		--扭蛋机
		GASHAPONMACHINE 						= 131,
		--五星好评
		FIVESTAR_HIGHPRAISE 					= 133,
		--限时组队
		LIMIT_TEAM 								= 134,
		--UNLOCK定制1（点赞）
		UNLOCK_FABEBOOK 						= 139,
		--UNLOCK定制2（广告）
		UNLOCK_AD								= 140,
		--端午
		DRAGONBOAT_FESTIVAL						= 145,
		--阵营招募
		CAMP_RECRUIT							= 146,
		--积分赛
		INTEGRAL_RACE							= 147,
		--竞技扩展
		ARENA_EXTRA								= 149,
		--英雄进化
		HERO_EVOLUTION							= 150,
		--荣耀商店
		INTEGRAL_GLORY_SHOP						= 151,
		--圣器进化
		TREASUREREMOULD							= 152,
		--装备进化
		EQUIP_REMOULD 							= 153,
		--援军助威
		REINFORCEMENTS_CHEER 					= 154,
		--装备升星
		EQUIP_UP_STAR 							= 155,
		--VIP招募
		VIP_RECRUIT 							= 158,
		--充值排行
		RECHARGE_RANK							= 159,

		MAX 									= 88888888,
	},

	--世界之心每阶级数
	WORLD_HEART_STAGE_NUM = 6,

	MAX_TEAMMATE_NUM = 4,

	RebelAttackType = 
	{
		--普通攻击
		NORMAL = 1, 
		--全力一击
		FULL = 2,
	},
	--活动
	--七天活动 购买类型状态
	ACTIVITY_SEVEN_DAYS_SHOP = {
		--打折
		DISCOUNT 								= 16,
		--抢购
		ROD 									= 99,
	},
	--七天活动 字符拼装的状态
	ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY = {
		-- 从这一天到活动结束登陆，任意时间登陆
		DEBARK 									= 1 ,
		-- 等级奖励
		LEVEL 									= 2 ,
		-- 累计充值
		CHARGE 								= 3 ,
		-- 主线副本通关进度
		CLEARANCE 								= 4 ,
		-- 装备强化
		EQUIP_INTENSIFY 						= 5 ,
		-- 竞技场
		ATHLETICS 								= 6 ,
		-- 圣器收集
		TREASURE_GATHER							= 7 ,
		-- 英雄商店刷新次数
		HEROSHOP_UP_ITEM 						= 8 ,
		-- 英雄突破
		HERO_BREAK      						= 9 ,
		-- 异度入侵刷战功
		EXPLOITS								= 10 ,
		-- 潜能提升等级
		POTENCY_UP_LEVEL						= 11 ,
		-- 爬塔星数累计
		STAR 									= 12 ,
		-- 装备升品
		EQUIP_LIFT								= 13 ,
		-- 装备精炼
		EQUIP_REFINE 							= 14 ,
		-- 英雄培养
		EQUIP_TRAIN 							= 15 ,
		-- 等级突破
		LEVEL_BREAK 							= 16 ,
		-- 战力突破
		BATTLE_BREAK 							= 17 ,
		-- 打折销售
		DISCOUNT 								= 18 ,
		-- 半价抢购
		HALF_PAY 								= 19 ,
	},	

	--活动类型
	ACTIVITY_TYPE = {
		ASSIGN 									= 1,
		DAYS 									= 2,
		WEEKLY 									= 3,
		MONTHLY 								= 4,		 
		SEVEN									= 5,
		SEVEN_2									= 6,
		OPEN									= 7,
	},

	--节日活动页签类型
	ACTIVITY_FESTIVAL_TAB = {
		--1、循环单充
		ONCE_PAY 								= 1,
		--2、循环累充
		ADDUP_PAY 								= 2,
		--3、开服限时召唤
		CALL									= 3,
		--4、节日单充
		PAY_GIFT								= 4,
		--5、节日累充
		ADDUP_GIFT								= 5,
		--6、打折贩售
		SALE 									= 6,
		--7、活跃目标
		ACTIVE									= 7,
		--8、材料兑换
		CONVER 									= 8,	
		--9、节日登陆
		FESTIVAL								= 9,
		--10、开服登陆
		OPEN_SERVICE							= 10,		
		--11、开服累充
		OPEN_ADD_UP								= 11,	
		--12、节日材料兑换
		FESTIVAL_DISCOUNT						= 12,	
		-- 13、节日打折贩售
		FESTIVAL_SALE							= 13,	
		-- 14、节日活跃目标
		FESTIVAL_ACTIVE							= 14,	
		-- 15、开服活跃
		OPEN_ACTIVE								= 15,
		-- 16、节日活跃	
		FESTIVAL_ACTIVE_2						= 16,
		-- 17、节日单冲
		FESTIVAL_ONCE_PAY						= 17,
	},

	--节日活动逻辑类型
	ACTIVITY_FESTIVAL = {
		--单充
		ONCE_PAY 								= 1,
		--累充
		ADDUP_PAY 								= 2,
		--打折
		SALE 									= 3,
		--兑换
		CONVER 									= 4,
		--登录
		ENTER									= 5,

		--主线
		MEAN_LINE								= 6,
		--圣器
		PYX										= 7,
		--竞技场
		ARENA									= 8,
		--深渊
		CHASM									= 9,
		--召唤
		CALL									= 10,

		--开服
		OPEN_SERVICE							= 11,
		--组队副本通关
		TEAM_PLAYER 							= 12,
		--每日通关精英副本
		EXERYDAY_ELITE 							= 13,
		--每日战胜勇者试炼精英关卡
		EXERYDAY_TOWER_ELITE 					= 14,
		--累计勇者试炼重置
		OVERALL_TRIAL 							= 15,
		
		--累计攻击叛军
		OVERALL_REBEL 							= 16,
		--累计挑战无尽深渊
		OVERALL_ACCRO 							= 17,
		--累计挑战王者赛
		OVERALL_KING 							= 18,
		--累计占卜翻牌
		OVERALL_LOW_AUGURY 						= 19,
		--累计高级占卜翻牌
		OVERALL_HIGH_AUGURY 					= 20,

		--历收藏值最高达到
		CARD_COLLECT							= 31,
		--活动期间消耗
		ACTIVITY_CONSUME						= 32,
		--英雄培养
		HERO_TRAIN 			 					= 33,
		--精灵培养
		STRING_TRAIN							= 34,
		--.累计扭蛋次数
		TOMY_ITEMS								= 35,

		--累计装备精炼次数
		EQUIP_REFINE							= 36,
		--累计特殊扭蛋次数
		ND_LOTTERY 								= 37,
	},

	--限时活动类型
	LIMIT_ACTIVITY_TYPE = {
		ONE_RECHARGE							= 1,
		GRAND_TOTAL_RECHARGE 					= 2,
		RECRUIT 								= 3,
		INITIAL 								= 4,		 
	},

	--系统通知参数类型
	NOTICE_PARAM_TYPE = {
		NUM 									= 1,	--数字
		STRING 									= 2,	--字符串
		PACK 									= 3,	--物品
		EQUIP 									= 4,	--装备
		HERO 									= 5,	--英雄
		CARD 									= 6,	--卡牌
		WORLDBOSS								= 7,	--世界boss
		MONSTER_STROMCASTLE						= 8,	--怪物攻城
		LADDER									= 9,	--天梯
		CHRISTMAS_BOOS							= 10,	--圣诞boos
	},

	SELECT_TYPE = {
		EQUIP 									= 1,	--装备
		EQUIP_DEBRIS 							= 2,	--装备碎片		
		HERO 									= 3,	--英雄
		HERO_DEBRIS								= 4,	--英雄碎片
		TREASURE 								= 5, 	--圣器
	},
	--战力变化类型
	COMBAT_CHANGE =
	{
		--英雄进阶
		HERO_BREAK = "HERO_BREAK",
		--英雄潜能
		HERO_POTENTIAL = "HERO_POTENTIAL",
		--英雄觉醒
		HERO_AWAKEN = "HERO_AWAKEN",
		--装备精炼
		EQUIP_REFINE = "EQUIP_REFINE",
		--装备强化
		EQUIP_INTENSIFY = "EQUIP_INTENSIFY",	
		--圣器精炼
		TREASURE_REFINE = "TREASURE_REFINE",
		--圣器强化
		TREASURE_INTENSIFY = "TREASURE_INTENSIFY",
		--装备强化
		ONE_KEY_EQUIP_INTENSIFY = "ONE_KEY_EQUIP_INTENSIFY",
		--爵位
		KNIGHTHOOD = "KNIGHTHOOD"
	},
	--公会
	UNION =
	{
		ICON = 3 ,
		NOTICE = 1,
		MANIFESTO = 2,
		DISSOLVE = 4,
		APPLY = 5,
		SKILL = 6,
		RESET = 7,
		DECLARE_WAR = 8 , 
	},
	--公会技能
	-- 升级的类型(1=工会,2=个人)
	UNION_SKILL = {
		GUILD = 1,
		PERSON = 2,
	},
	--公会红点
	UNION_RED = {
		AUDIT = 0,      	--入会申请列表有新的申请显示红点
		DESENO_SHOP = 1 , 	--限时商店刷新后
		AWARD = 2 ,			--奖励商店解锁新的物品购买时
		CONSTRUCTION = 3 ,	--军团有可剩余建设次数时
		MESSAGE = 4 , 		--留言板内有新的留言
		KILLS = 5 , 		--军团技能中
		TRANSCRIPT = 6 , 	--军团副本
		TRANSCRIPT_AWARD = 7 , 	--军团副本奖励
		TRANSCRIPT_LEVLE_AWARD = 8 , 	--军团副本奖励
		TRANSCRIPT_CONSTRUCTION_AWARD = 9 , 	--军团副本奖励
		TRANSCRIPT = 10 , 	--军团战
	},
	RechargeType = {
		MONTH_CARD = 1,		--月卡
		COMMON = 2,			--普通充值
		SUPER_DISCOUNTVIEW = 3 , --超值折扣
		DIRECT_BUY = 4 , --直购特惠
		RECOMMEND_BUY = 5 , --推荐购买
		ACCURATE_RECOMMEND = 6, --精准推送
		HOT_SALE = 7, --热卖商品
		NEW_SUPER_DISCOUNTVIEW = 8 , --新超值折扣
	},
	--PVP挑战奖励类型
	PVP_REWARD_TYPE = {
		DAY = 1 ,
		WEEK = 2 ,
	},
	-- 称号
	TITLE_TYPE = {
		TITLE = 1 ,
		PREFIX = 2 , 
	},
	--无尽商店
	CHASM_SHOP = {
		SHOP = 1,
		AWARD = 2,
	},
	--七天活动类型
	SEVEN_TYPE = {
		SEVEN = 1 , 
		EIGHT = 2 ,
	},

	--战力压制触发类型
	CombatSuppressionTriggerType = 
	{
		--不触发
		NONE = 0,
		--影响我方
		EFFECT_ME = 1,
		--影响双方
		EFFECT_ALL = 2,	
	},

	--必须指引关键点过滤类型
	NecessaryLeadKeyPointFilterType = 
	{
		--领取宝箱
		RECEIVE_BOX_REWARD = 1,
		--英雄出战
		HERO_ON_FIGHTING = 2,
		--英雄升级
		HERO_UPGRADE = 3,
		--英雄招募
		HERO_RECRUIT = 4,
		--世界之心点亮
		WORLD_HEART_LIGHT_UP = 5,
		--精灵出战
		SPIRIT_ON_FIGHTING = 6,
		--英雄进阶
		HERO_BREAK = 7,
		--一键领取宝箱
		ONE_KEY_RECEIVE_BOX_REWARD = 8,

	},

	--获取途径特殊处理
	APPROACH_SPECIAL_TYPE = {
		--主线副本所有关卡
		MAIN_DUNGEON_ALL_GAMELEVEL = 997,
		--英雄副本所有关卡
		HERO_DUNGEON_ALL_GAMELEVEL = 998,
		--精英副本所有关卡
		ELITE_DUNGEON_ALL_GAMELEVEL = 999,
	},

	--原石商店类型
	RAW_GEMSTONE_SHOP = {
		--商店
		SHOP = 1,
		--奖励
		REWARD = 2,
	},

	TEAM_TASK_TYPE = {
		--答题
		QUESTION = 1,
		--采集
		GATHER = 2,
		--打怪
		BATTLE_MONSTER = 3,
		--对话
		CONTENT = 4,
	},

	--天梯等级
	LADDER_LEVEL = {
		LEVEL_1 = 1 ,
		LEVEL_2 = 2 ,
		LEVEL_3 = 3 ,
		LEVEL_4 = 4 ,
		LEVEL_5 = 5 ,
		LEVEL_6 = 6 ,
		LEVEL_7 = 7 ,
	} ,

	--卡牌类型(抽卡)
	CARD_TYPE = {
		LOW = 1 , 
		TALL = 2 , 
	},

	--卡牌类型
	CARD_QUALITY_TYPE = {
		EXP = 0 , 
	},

	--同一个模块下的不同界面 
	--/自定义超值购
	BE_DIFFERENT = {
		OVERFLOW_GEM = 23 , 

		OVERFLOW_HERO = 400 , 
	},

	--助战目标类型
	ASSIST_TAGGET = {
		STRANGER = 0,	--陌生人
		FRIEND = 1,	--好友
		GUILD = 2,		--军团
	},

	--渠道特殊
	--好友
	FACE_BOOK_FRIENDS = {
	    --好友召回
		SUMMON = 1 , 
	    --好友邀请
		INVITE = 2 , 
	},

	--祝福类型
	BLESS_TYPE = {
		BLESS = 1,	--祝福
		REPLACE_BLESS = 2,	--替换
		CANCEL_BLESS = 3,	--取消
	},

	--QTE评分类型
	QTE_GRADE_TYPE = {
		GRADE_INVALID = 0, --无评分
		GRADE_GREAT = 1,   --GREAT
		GRADE_PERFECT = 2, --PERFECT
	},
}

--角色属性Key
Macros.Game.RolePropKey =
{
	[Macros.Game.RolePropType.LEVEL] = "level",
	[Macros.Game.RolePropType.EXP] = "exp",
	[Macros.Game.RolePropType.GOLD] = "gold",
	[Macros.Game.RolePropType.STAMINA] = "stamina",
	[Macros.Game.RolePropType.DIAMOND] = "diamond",
	[Macros.Game.RolePropType.HERO_SOUL] = "hero_soul",
	[Macros.Game.RolePropType.HEAD_ICON] = "head_icon",
	[Macros.Game.RolePropType.ENERGY] = "energy",
	[Macros.Game.RolePropType.CLICK_LIKE_TIMES] = "click_like_times",
	[Macros.Game.RolePropType.VIP_LEVEL] = "vip_level",
	[Macros.Game.RolePropType.REBEL_VALUE] = "rebel_value",
	[Macros.Game.RolePropType.WANTED] = "wanted",
	[Macros.Game.RolePropType.TOWER_PRESTIGE] = "tower_prestige",
	[Macros.Game.RolePropType.ARENA_PRESTIGE] = "arena_prestige",	
	[Macros.Game.RolePropType.GODSOUL] = "god_soul",
	[Macros.Game.RolePropType.GUILD_ID] = "guild_id",
	[Macros.Game.RolePropType.DAILY_INIT_LEVEL] = "daily_init_level",
	[Macros.Game.RolePropType.GUILD_CONTRIBUTION] = "guild_contribution",
	[Macros.Game.RolePropType.TEAM_PRO_VIP] = "vip_exp",
	[Macros.Game.RolePropType.WORLD_BOSS_HONOR] = "honor",
	[Macros.Game.RolePropType.BATTLE_VALUE] = "battle_value",
	[Macros.Game.RolePropType.FAIRY_SOUL] = "fairy_soul",
	[Macros.Game.RolePropType.SCORE_MATCH_HONOR] = "score_match_honor",
	[Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT] = "total_friendship_point",
	[Macros.Game.RolePropType.TODAY_FRIENDSHIP_POINT] = "today_friendship_point",
	[Macros.Game.RolePropType.ENDLESS_STORE] = "endless_store",
	[Macros.Game.RolePropType.NOBILITY] = "nobility_level",
	[Macros.Game.RolePropType.GEM] = "gem_coin",
	[Macros.Game.RolePropType.CARD_CURRENCY] = "star_soul",
	[Macros.Game.RolePropType.DESTINY_COIN] = "destiny_coin",
	[Macros.Game.RolePropType.CRYSTAL] = "crystal",
	[Macros.Game.RolePropType.RESOURCE_WAR] = "resource_war_score",
	[Macros.Game.RolePropType.YCOIN] = "ycoin",
	[Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT] = "total_recharge_amount",
}

--英雄职业名
Macros.Game.HeroProfessionNameMap =
{
	[Macros.Game.HeroProfession.PHYSICAL_NEAR] = CPPGameLib.GetString("hero_pro_name_physical_near"),
	[Macros.Game.HeroProfession.DEFENCE_NEAR] = CPPGameLib.GetString("hero_pro_name_defence_near"),
	[Macros.Game.HeroProfession.PHYSICAL_REMOTE] = CPPGameLib.GetString("hero_pro_name_physical_remote"),
	[Macros.Game.HeroProfession.MAGIC_REMOTE] = CPPGameLib.GetString("hero_pro_name_magic_remote"),
	[Macros.Game.HeroProfession.ASSIST] = CPPGameLib.GetString("hero_pro_name_assist"),
}

--战斗属性Key
Macros.Game.BattlePropKey = 
{
	[Macros.Game.BattleProp.ATTACK] = "attack",
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE] = "physical_defence",
	[Macros.Game.BattleProp.MAGIC_DEFENCE] = "magic_defence",
	[Macros.Game.BattleProp.HP] = "hp",
	[Macros.Game.BattleProp.DAMAGE_DECREASE_RATE] = "damage_decrease_rate",
	[Macros.Game.BattleProp.DAMAGE_DECREASE_VALUE] = "damage_decrease_value",
	[Macros.Game.BattleProp.DAMAGE_INCREASE_RATE] = "damage_increase_rate",
	[Macros.Game.BattleProp.DAMAGE_INCREASE_VALUE] = "damage_increase_value",
	[Macros.Game.BattleProp.PENETRATION_RATE] = "penetration_rate",
	[Macros.Game.BattleProp.PENETRATION_VALUE] = "penetration_value",
	[Macros.Game.BattleProp.HIT_RATE] = "hit_rate",
	[Macros.Game.BattleProp.DODGE_RATE] = "dodge_rate",
	[Macros.Game.BattleProp.CRIT_RATE] = "crit_rate",
	[Macros.Game.BattleProp.CRIT_DEFENCE_RATE] = "crit_defence_rate",
	[Macros.Game.BattleProp.CRIT_RATE_CORRECT] = "crit_rate_correct",
	[Macros.Game.BattleProp.CRIT_DEFENCE_RATE_CORRECT] = "crit_defence_rate_correct",
	[Macros.Game.BattleProp.BLOCK_RATE] = "block_rate",
	[Macros.Game.BattleProp.BLOCK_RATE_CORRECT] = "block_rate_correct",
	[Macros.Game.BattleProp.BEAT_BACK_CORRECT] = "beat_back_correct",
	[Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT] = "beat_back_defence_correct",	
	[Macros.Game.BattleProp.ANGER] = "anger",	
	[Macros.Game.BattleProp.ATTACK_RATE] = "attack_rate",	
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE] = "physical_defence_rate",	
	[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE] = "magic_defence_rate",	
	[Macros.Game.BattleProp.HP_RATE] = "hp_rate",	
	[Macros.Game.BattleProp.SPEED] = "speed",	
	[Macros.Game.BattleProp.ATTACK_RATE_FINAL] = "attack_rate_final",	
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE_FINAL] = "physical_defence_rate_final",	
	[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE_FINAL] = "magic_defence_rate_final",	
	[Macros.Game.BattleProp.HP_RATE_FINAL] = "hp_rate_final",	
}

--战斗属性名
Macros.Game.BattlePropName = 
{
	[Macros.Game.BattleProp.ATTACK] = CPPGameLib.GetString("nature1"),
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE] = CPPGameLib.GetString("nature2"),
	[Macros.Game.BattleProp.MAGIC_DEFENCE] = CPPGameLib.GetString("nature3"),
	[Macros.Game.BattleProp.HP] = CPPGameLib.GetString("nature4"),
	[Macros.Game.BattleProp.DAMAGE_DECREASE_RATE] = CPPGameLib.GetString("nature5"),
	[Macros.Game.BattleProp.DAMAGE_DECREASE_VALUE] = CPPGameLib.GetString("nature6"),
	[Macros.Game.BattleProp.DAMAGE_INCREASE_RATE] = CPPGameLib.GetString("nature7"),
	[Macros.Game.BattleProp.DAMAGE_INCREASE_VALUE] = CPPGameLib.GetString("nature8"),
	[Macros.Game.BattleProp.PENETRATION_RATE] = CPPGameLib.GetString("nature9"),
	[Macros.Game.BattleProp.PENETRATION_VALUE] = CPPGameLib.GetString("nature10"),
	[Macros.Game.BattleProp.HIT_RATE] = CPPGameLib.GetString("nature11"),
	[Macros.Game.BattleProp.DODGE_RATE] = CPPGameLib.GetString("nature12"),
	[Macros.Game.BattleProp.CRIT_RATE] = CPPGameLib.GetString("nature13"),
	[Macros.Game.BattleProp.CRIT_DEFENCE_RATE] = CPPGameLib.GetString("nature14"),
	[Macros.Game.BattleProp.CRIT_RATE_CORRECT] = CPPGameLib.GetString("nature15"),
	[Macros.Game.BattleProp.CRIT_DEFENCE_RATE_CORRECT] = CPPGameLib.GetString("nature16"),
	[Macros.Game.BattleProp.BLOCK_RATE] = CPPGameLib.GetString("nature17"),
	[Macros.Game.BattleProp.BLOCK_RATE_CORRECT] = CPPGameLib.GetString("nature18"),
	[Macros.Game.BattleProp.BEAT_BACK_CORRECT] = CPPGameLib.GetString("nature19"),
	[Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT] = CPPGameLib.GetString("nature20"),
	[Macros.Game.BattleProp.ANGER] = CPPGameLib.GetString("nature21"),

	[Macros.Game.BattleProp.ATTACK_RATE] = CPPGameLib.GetString("nature22"),
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE] = CPPGameLib.GetString("nature23"),
	[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE] = CPPGameLib.GetString("nature24"),
	[Macros.Game.BattleProp.HP_RATE] = CPPGameLib.GetString("nature25"),
	[Macros.Game.BattleProp.SPEED] = CPPGameLib.GetString("nature26"),

	[Macros.Game.BattleProp.ATTACK_RATE_FINAL] = CPPGameLib.GetString("nature27"),
	[Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE_FINAL] = CPPGameLib.GetString("nature28"),
	[Macros.Game.BattleProp.MAGIC_DEFENCE_RATE_FINAL] = CPPGameLib.GetString("nature29"),
	[Macros.Game.BattleProp.HP_RATE_FINAL] = CPPGameLib.GetString("nature30"),
}

--货币类型列表
Macros.Game.CurrencyTypeList = 
{
	-- 3=金币
	Macros.Game.RolePropType.GOLD,
	-- 4=体力
	Macros.Game.RolePropType.STAMINA,
	-- 5=钻石
	Macros.Game.RolePropType.DIAMOND,
	-- 6=雄魂
	Macros.Game.RolePropType.HERO_SOUL,
	-- 8=精力 
	Macros.Game.RolePropType.ENERGY,
	-- 12=声望(爬塔)
	Macros.Game.RolePropType.TOWER_PRESTIGE,
	-- 13=威名(竞技场)
	Macros.Game.RolePropType.ARENA_PRESTIGE,
	-- 14=战功(叛军)
	Macros.Game.RolePropType.REBEL_VALUE,
	-- 15=通缉令
	Macros.Game.RolePropType.WANTED,
	-- 17=神魂
	Macros.Game.RolePropType.GODSOUL,
	-- 20=工会贡献值
	Macros.Game.RolePropType.GUILD_CONTRIBUTION,
	-- 26=总的友情点
	Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT,
}

--必须指引关键点过滤类型对应的协议
Macros.Game.NecessaryLeadKeyPointFilterTypeProtocol = 
{
	--领取宝箱
	[Macros.Game.NecessaryLeadKeyPointFilterType.RECEIVE_BOX_REWARD] = Net.ProtocolNo.C2S_FB_AWARD_GET_REQ,
	--英雄出战
	[Macros.Game.NecessaryLeadKeyPointFilterType.HERO_ON_FIGHTING] = Net.ProtocolNo.C2S_HERO_GOTO_BATTLE_REQ,
	--英雄升级
	[Macros.Game.NecessaryLeadKeyPointFilterType.HERO_UPGRADE] = Net.ProtocolNo.C2S_HERO_UPGRADE_REQ,
	--英雄招募
	[Macros.Game.NecessaryLeadKeyPointFilterType.HERO_RECRUIT] = Net.ProtocolNo.C2S_HERO_RECRUIT_REQ,
	--世界之心点亮
	[Macros.Game.NecessaryLeadKeyPointFilterType.WORLD_HEART_LIGHT_UP] = Net.ProtocolNo.C2S_HEART_OF_WORLD_LEVEL_UP_REQ,
	--精灵出战
	[Macros.Game.NecessaryLeadKeyPointFilterType.SPIRIT_ON_FIGHTING] = Net.ProtocolNo.C2S_SPIRIT_USE_REQ,
	--英雄进阶
	[Macros.Game.NecessaryLeadKeyPointFilterType.HERO_BREAK] = Net.ProtocolNo.C2S_HERO_BREACH_REQ,
	--一键领取宝箱
	[Macros.Game.NecessaryLeadKeyPointFilterType.ONE_KEY_RECEIVE_BOX_REWARD] = Net.ProtocolNo.C2S_FB_ONE_KEY_TAKE_REWARD_REQ,
}

--不侵权的英雄ID列表
Macros.Game.SafeHeroListForExamine = {30300,30310,30320,30330,30340,31300,31310,31320,31330,31340,31301,31341,31400,31430,31401,31431,31402,31420,31438,31408,31441,31419,31439,31530,32300,32310,32320,32330,32340,32341,32400,32430,32401,32431,32449}

--不侵权的怪物ID列表
Macros.Game.SafeMonsterListForExamine = {99984,99985,99986,99987,99988,99989,99990,99991,99992,99993,99994,99995,99996,99997,99998}

--是否近战类职业
function Macros.Game.IsProfessionNear(pro)
	return pro == Macros.Game.HeroProfession.PHYSICAL_NEAR or
			pro == Macros.Game.HeroProfession.DEFENCE_NEAR
end

--是否物理职业
function Macros.Game.IsProfessionPhysical(pro)
	return pro == Macros.Game.HeroProfession.PHYSICAL_NEAR or 
	pro == Macros.Game.HeroProfession.DEFENCE_NEAR or pro == Macros.Game.HeroProfession.PHYSICAL_REMOTE	
end

--是否高级怪
function Macros.Game.IsSeniorMonster(monster_id)
	local config_monster = CPPGameLib.GetConfig("Monster", monster_id, false)
	if not config_monster then
		return
	end
	return config_monster.quality >= Macros.Game.QualityType.PURPLE
end
