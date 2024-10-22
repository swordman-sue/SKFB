

--[[
%% @module: 事件定义
%% @author: swordman sue
%% @created: 2016-07-10
--]]

Macros = Macros or {}

Macros.Event = Macros.Event or {}

--主状态事件
Macros.Event.GameState = Macros.Event.GameState or 
{
	--登陆账号成功
	LOGIN_ACCOUNT_SUCCESS = "login_account_success",

	--账号删除
	LOGIN_ACCOUNT_REMOVE = "login_account_remove",

	--选服反馈
	SELECT_SERVER_RESP = "select_server_resp",

	--进入SceneChanging状态事件
	SCENE_CHANGE_STATE_ENTER = "scene_change_state_enter",

	--离开SceneChanging状态事件
	SCENE_CHANGE_STATE_QUIT = "scene_change_state_quit",

	--进入GamePlaying状态事件
	GAME_PLAY_STATE_ENTER = "game_play_state_enter",

	--离开GamePlaying状态事件
	GAME_PLAY_STATE_QUIT = "game_play_state_quit",

	--获取服务器配置
	REQUEST_SERVER_CONFIG = "REQUEST_SERVER_CONFIG",

	--获取服务器列表
	REQUEST_SERVER_LIST = "REQUEST_SERVER_LIST",

	--创建角色
	ESTABLISH = "ESTABLISH" , 
}

--场景对象事件
Macros.Event.SceneObj = Macros.Event.SceneObj or 
{
	HERO_VO_ADD = "hero_vo_add",
	HERO_VO_REMOVE = "hero_vo_remove",
	HERO_MOVE_END = "hero_move_end",
	HERO_DEAD = "hero_dead",	

	MONSTER_VO_ADD = "monster_vo_add",
	MONSTER_VO_REMOVE = "monster_vo_remove",
	MONSTER_MOVE_END = "monster_move_end",
	MONSTER_DEAD = "monster_dead",
	MONSTER_DO_BIRTH = "monster_do_birth",

	BULLET_VO_ADD = "bullet_vo_add",
	BULLET_VO_REMOVE = "bullet_vo_remove",	

	DROPBOX_DEAD = "dropbox_dead",

	OBJ_MOVE_START = "obj_move_start",
	OBJ_MOVE_END = "obj_move_end",
	OBJ_HP_CHANGE = "obj_hp_change",

	COMMON_ANGER_CHANGE = "common_anger_change",

	COMMON_ANGER_CHANGE_OF_ENEMY = "common_anger_change_of_enemy",

	OBJ_ANGER_OR_JOINT_SKILL_RELEASE = "obj_anger_or_joint_skill_release",
	OBJ_ANGER_OR_JOINT_SKILL_RELEASE_END = "obj_anger_or_joint_skill_release_end",

	OBJ_FORBID_ATTACK_CHANGE = "obj_forbid_attack_change",
	OBJ_FORBID_SP_SKILL_CHANGE = "obj_forbid_sp_skill_change",
	OBJ_FORBID_ANGER_OR_JOINT_SKILL_CHANGE = "obj_forbid_anger_or_joint_skill_change",

	OBJ_SPRITE_SKILL_RELEASE = "obj_sprite_skill_release",
	OBJ_SPRITE_SKILL_RELEASE_END = "obj_sprite_skill_release_end",

	OBJ_SKILL_RELEASE = "obj_skill_release",
	
	MT_ITEM_PICK_UP = "MT_ITEM_PICK_UP",
	MT_MONSTER_KILLED = "MT_MONSTER_KILLED",

	OBJ_ANGER_OR_JOINT_QTE = "obj_anger_or_joint_qte",
	OBJ_ANGER_OR_JOINT_QTE_END = "obj_anger_or_joint_qte_end",
}

--界面基础事件
Macros.Event.View = Macros.Event.View or
{
	VIEW_OPEN = "view_open",
	VIEW_CLOSE = "view_close",
	TAB_VIEW_OPEN = "tab_view_open",
	TAB_VIEW_CLOSE = "tab_view_close",
	TEMPLATE_OPEN = "template_open",

	NO_VIEW_OPEN = "no_view_open",

	ONLY_CITY_VIEW_OPEN = "only_city_view_open",
	NOT_ONLY_CITY_VIEW_OPEN = "not_only_city_view_open",
	MAINCITY_VISIBLE = "MAINCITY_VISIBLE",
	MAINCITY_HIDE = "MAINCITY_HIDE",

	VIEW_EFFECT_FINISHED = "view_effect_finished",
	TEMPLATE_EFFECT_FINISHED = "template_effect_finished",

	UPDATE_MAINCITY_EFFECT = "UPDATE_MAINCITY_EFFECT" , 
}

--网络相关事件
Macros.Event.Net = Macros.Event.Net or 
{
	ON_SEND_PROTOCOL = "on_send_protocol",
	SERVER_CONNECTING = "server_connecting",
	SERVER_CONNECTED = "server_connected",	
	SERVER_DISCONNECTED = "server_disconnected",	
	NETWORK_STATE_CHANGED_NOTIFY = "network_state_changed_notify",
	NETWORK_INVALID_NOTIFY = "network_invalid_notify",
}

--时间同步事件
Macros.Event.TimeSyncModule = Macros.Event.TimeSyncModule or 
{
	--首次同步时间
	TIME_SYNC_FIRST = "time_sync_first",
}

--系统模块事件
Macros.Event.SystemModule = Macros.Event.SystemModule or
{
	--系统开启
	SYSTEM_OPENED = "system_opened",
}

--角色事件
Macros.Event.RoleModule = Macros.Event.RoleModule or 
{
	--等级改变(old_lev, new_lev)
	LEVEL_CHANGE = "role_level_change",

	--属性改变(old_var, new_var)
	PROP_CHANGE = "role_prop_change",

	--头像改变
	HEAD_CHANGE = "role_head_change",

	--角色改名反馈
	ROLE_NAME_CHANGED = "role_name_changed_resp",	

	--升级截断
	UP_LEVEL_TRUNCATION = "upLevelTruncation",

	--属性恢复
	PROP_RECOVERY_COUNTDOWN = "prop_recovery_countdown",
}

--战斗事件
Macros.Event.BattleModule = Macros.Event.BattleModule or
{
	--战役开始
	BATTLE_START = "battle_start",

	--战斗结束
	BATTLE_END = "battle_end",

	--战役倒计时
	BATTLE_TIME_STEP = "battle_time_step",	

	--从战斗结算界面返回对应界面
	BATTLE_END_RETURN_RELEVANT_VIEW = "battle_end_return_relevant_view",

	--战斗中所有英雄阵亡
	BATTLE_ALL_HERO_DEAD = "battle_all_hero_dead",	

	--战斗中所有怪物阵亡
	BATTLE_ALL_MONSTER_DEAD = "battle_all_monster_dead",	

	--进入战斗跳过
	BATTLE_SKIP = "battle_skip",

	--结束战斗跳过
	BATTLE_SKIP_ABORT = "battle_skip_abort",

	--PVP查找目标响应
	PVP_SEARCH_TARGET_RESP = "pvp_search_target_resp",

	--PVP战斗准备通知
	PVP_READY_NOTIFY = "pvp_ready_notify",

	--战斗QTE连击
	BATTLE_QTE_COMBO = "battle_qte_combo",
}

--背包事件
Macros.Event.PackModule = Macros.Event.PackModule or 
{
	--刷新普通道具
	UPDATE_GOODS = "updata_goods" ,
	--所有的更新
	UPDATE_PACK_DATA = "pack_update_data",
	ADD_GET_PACK = "pack_addGetPack" ,
	REMOVE_PACK = "pack_remove" ,
	UPDATE_PACK = "pack_update" ,
	GET_CHOOSECARD_DATA = "choosecard_data",
	--使用物品成功
	USE_ITEM_SUCCESS = "use_item_success",
	--所有的更新
	UPDATE_PACK_DATA = "pack_update_data",
	--物品时间刷新
	ITEM_TIME_UP = "item_time_up",

}

--英雄
Macros.Event.HeroModule = Macros.Event.HeroModule or
{
	--英雄增加
	HERO_ADD = "hero_add",
	--英雄移除
	HERO_REMOVE = "hero_remove",
	--英雄更新
	HERO_UPDATE = "hero_update",
	--英雄改变
	HERO_CHANGE = "HERO_CHANGE",
	--英雄阵位更新
	HERO_LINEUP_UPDATE = "hero_lineup_update",
	--英雄升级成功
	HERO_UPGRADE_SUCC = "hero_upgrade_succ",
	--英雄进阶成功
	HERO_BREAK_SUCC = "hero_break_succ",	
	--英雄初级召唤界面更新
	HERO_RECRUIT_UPDATE = "hero_recruit__update",
	--英雄详细信息
	HERO_DETAIL_INFO_RESP = "hero_detail_info_resp",
	--英雄培养
	HERO_TRAIN_RESP = "hero_train_resp",
	--援军上阵
	HERO_REINFORCEMENTS_ONLINE = "hero_reinforcements_online",
	--援军卸阵
	HERO_REINFORCEMENTS_OFF_BATTLE = "HERO_REINFORCEMENTS_OFF_BATTLE",
	--援军上阵回馈
	HERO_REINFORCEMENTS_ON_BATTLE = "HERO_REINFORCEMENTS_ON_BATTLE",
	--英雄潜能
	HERO_POTENTIAL_RESP = "hero_potential_resp",
	--更新碎片
	HERO_UP_DEBRIS = "hero_up_debris",
	--英雄觉醒
	HERO_AWAKEN_RESP = "hero_awaken_resp",
	--英雄装备觉醒道具
	HERO_EQUIP_AWAKEN_PROP_RESP = "hero_equip_awaken_prop_resp",
	--英雄觉醒道具合成
	HERO_AWAKEN_PROP_COMPOSE_RESP = "hero_awaken_prop_compose_resp",
	--英雄招募界面免费更新
	HERO_RECRUIT_FREE_UPDATE = "hero_recruit_free_update",
	--英雄招募界面数据更新
	HERO_RECRUIT_DATA_UPDATE = "hero_recruit_data_update",
	--英雄批量招募界面
	HERO_MORE_RECRUIT_CONTINUTE = "hero_more_recruit_continute",
	--英雄收集
	HERO_GATHER = "hero_gather",
	--英雄培养次数选择
	HERO_TRAIN_TIMES_CHOOSE = "hero_train_times_choose",
	--英雄培养次数选择
	HERO_TRAIN_TIMES_CHOOSE = "hero_train_times_choose",
	--英雄/援军的上阵/下阵
	HERO_ON_OR_OFF_FIGHT = "HERO_ON_OR_OFF_FIGHT",
	--英雄阵位开启
	HERO_LINEUP_POS_OPENED = "hero_lineup_pos_opened",
	--援军阵位开启
	REINFORCEMENT_LINEUP_POS_OPENED = "reinforcement_lineup_pos_opened",
	--英雄图鉴调整透明度
	HERO_HAND_BOOK_ADJUST_OPACITY = "hero_hand_book_adjust_opacity",
	--英雄培养替换回调
	HERO_TRAIN_REPLACE_RESP = "hero_train_replace_resp",
	--英雄培养领取奖励回调
	HERO_TRAIN_GET_REWARD_RESP = "hero_train_get_reward_resp",
	--英雄飘字预处理
	HERO_WIND_WORDS_PREPROCESS = "hero_wind_words_update",
	--招募提示
	HERO_RECRUIT_NOTICE = "hero_recruit_notice",
	--更新卡牌通知
	HERO_UPDATE_DIVINATION_NOTICE = "hero_update_divination_notice",
	--英雄进化响应通知
	HERO_EVOLUTION_NOTICE = "hero_evolution_notice",
}

--装备
Macros.Event.EquipModule = Macros.Event.EquipModule or 
{
	--更新碎片
	EQUIP_UP_DEBRIS = "equip_up_debris",

	--添加装备
	EQUIP_ADD_EQUIP = "equip_add_equip",

	--删除装备
	EQUIP_REMOVE_EQUIP = "equip_remove_equip",

	--更新装备
	EQUIP_UP_EQUIP = "equip_up_equip",

	--改变装备
	EQUIP_CHANGE_EQUIP = "EQUIP_CHANGE_EQUIP",

	--一键强化更新装备
	ONE_KEY_ENHANCE_EQUIP = "one_key_enhance_equip",

	--穿上装备
	EQUIP_CROSS_EQUIP = "equip_cross_equip",

	--卸下装备
	EQUIP_DISCHARGE_EQUIP = "equip_discharge_equip",

	--强化装备
	EQUIP_INTENSIFY	= "equip_intensify",

	--精炼
	EQUIP_REFINE	= "equip_reine",

	--升星
	EQUIP_UPSTAR	= "equip_star",

	--进阶装备
	EQUIP_ADVANCE   = "equip_Advance"
}

--圣器
Macros.Event.TreasureModule = Macros.Event.TreasureModule or
{
	--圣器强化
	TREASURE_INTENSIFY = "Treasure_intensify",	
	--圣器精炼
	TREASURE_REFINE = "Treasure_refine",
	--新增圣器
	TREASURE_ADDED = "Treasure_added",
	--移除圣器
	TREASURE_REMOVE = "Treasure_Remove" ,
	--更新圣器
	TREASURE_UPDATE = "TREASURE_UPDATE" ,
	--改变圣器
	TREASURE_CHANGE = "TREASURE_CHANGE",
	--圣器列表选择
	TREASURE_SELECT = "Treasure_select",
	--圣器选择列表切换
	TREASURE_SELECT_TAB = "Treasure_select_tab",
	--圣器穿戴
	TREASURE_CROSS = "Treasure_cross",
	--卸下圣器
	TREASURE_DISCHARGE = "Treasure_discharge",	
	--进阶圣器
	TREASURE_ADVANCE = "Treasure_Advance"
}

--商店
Macros.Event.ShopModule = Macros.Event.ShopModule or
{
	--记录
	RECORD_LIST = "record_list",
	--购买成功
	BUY_SUCCESS = "buy_sucess",
}

--排行榜
Macros.Event.RankModule = Macros.Event.RankModule or
{
	--战力榜响应
	RANK_LIST_REQ = "rank_list_req",
	--等级榜响应
	RANK_LEVEL_LIST_REQ = "rank_level_list_req",
	--充值
	RANK_RECHARGE_REQ = "rank_recharge_req",
}

--好友
Macros.Event.FriendModule = Macros.Event.FriendModule or
{
	ADD_ITEM = "add_item",
	DELETE_ITEM = "delete_item",	
	MOVE_ITEM = "move_item",
	INIT_CORRESPONDING_PANEL = "init_corresponding_panel",
}

--聊天
Macros.Event.ChatModule = Macros.Event.ChatModule or
{
	CHAT_DATA = "chat_data",			--聊天信息
	CHAT_QUIETLY = "chat_quietly",	--私聊
	CHAT_CD = "chat_cd",				--CD
	CHAT_CHECK = "chat_check",		--玩家信息
	CHAT_MAIN = "chat_main",			--主界面
}

--回收
Macros.Event.RecycleModule = Macros.Event.RecycleModule or
{
	RECYCLE_DISMANTLE = "dismantle",	--拆解
	RECYCLE_REBIRTH = "rebirth",		--重生
	RECYCLE_SKY = "sky",		--清空

}

--叛军
Macros.Event.RebelModule = Macros.Event.RebelModule or
{
	--叛军信息反馈
	REBEL_DATA_RESP = "rebel_data_resp",

	--功勋奖励记录
	REBEL_AWARD_RECORD = "rebel_award_recoro",	

	--排行榜
	REBEL_RANK = "rebel_rank",	
}

--副本界面
Macros.Event.DungeonModule = Macros.Event.DungeonModule or 
{
	--主线副本数据反馈
	MAIN_DUNGEON_DATA_RESP = "main_dungeon_data_resp",

	--英雄副本数据反馈
	HERO_DUNGEON_DATA_RESP = "hero_dungeon_data_resp",

	--精英副本数据反馈
	ELITE_DUNGEON_DATA_RESP = "elite_dungeon_data_resp",


	--日常副本数据反馈
	DAILY_DUNGEON_DATA_RESP = "daily_dungeon_data_resp",

	--副本奖励领取反馈
	DUNGEON_AWARD_GET_RESP = "dungeon_award_get_resp",

	--副本一键领取领取
	DUNGEON_ONEKEY_AWARD_GET_RESP = "dungeon_onekey_award_get_resp",

	--副本扫荡反馈
	DUNGEON_CLEAN_RESP = "dungeon_clean_resp",

	--副本滚动条目选择
	DUNGEON_SCROLL_ITEM_CHOOSE_RESP = "dungeon_scroll_item_choose_resp",

	--通关副本
	DUNGEON_PASS_GAME_LEVEL = "dungeon_pass_game_level",

	--爬塔副本首次通关
	TOWER_GAMELEVEL_PASSED_FIRST_TIME = "tower_gamelevel_passed_first_time",

	--领取主线副本章节奖励
	TAKE_MAIN_FB_CHAPTER_REWARD = "take_main_fb_chapter_reward",

	--更新副本关卡数据
	UPDATE_DUNGEON_GAMELEVEL_DATA = "update_dungeon_gamelevel_data",

	--英雄副本特殊关卡居中检测请求
	HERO_DUNGEON_GAMELEVEL_CENTER_CHECK_REQ = "hero_dungeon_gamelevel_center_check_req",

	--英雄副本特殊关卡居中检测回应
	HERO_DUNGEON_GAMELEVEL_CENTER_CHECK_RESP = "hero_dungeon_gamelevel_center_check_resp",

	--副本精灵通用技能设置
	DUNGEON_SPIRIT_COMMON_SKILL_CHOOSE = "dungeon_spirit_common_skill_choose",

	--关闭挑战界面
	DUNGEON_CHALLANGE_CLOSE = "dungeon_challange_close",

	--副本重置
	DUNGEON_MAINLINE_RESET = "dungeon_mainline_reset",

	--关卡留言列表
	GAMELEVEL_COMMENT = "gamelevel_comment",

	--关卡留言响应
	GAMELEVEL_COMMENT_RESP = "gamelevel_comment_resp",

	--关卡留言点赞响应
	GAMELEVEL_COMMENT_PARISE_RESP = "gamelevel_comment_parise_resp",

	--英雄降临弹出
	HERO_ADVENT_POPUP = "hero_advent_popup",

	--屏蔽关卡界面点击
	SHIELD_GAMELEVEL_TOUCH = "shield_gamelevel_touch",

	--英雄降临之后继续其他步骤
	AFTER_HERO_ADVENT_POPUP = "after_hero_advent_popup",

	--刷新助战列表
	REFRASH_ASSIST_LIST = "refrash_assist_list",

	--弹幕显示
	REFRASH_MESSAGE_BOARD = "refrash_message_board",

	--触发助战界面指引
	FRIEND_ASSIST_LEAD = "friend_assist_lead",
}

--爬塔
Macros.Event.TowerModule = Macros.Event.TowerModule or 
{
	TOWER_RANK = "tower_rank",
	TOWER_ELITE_TIME = "tower_elite_time",
	TOWER_UP = "tower_up",
	TOWER_RESET = "tower_reset",
	TOWER_SWEEP = "tower_sweep",
	TOWER_BUFF = "tower_buff",
	TOWER_RECORD = "tower_record",
}

--世界之心
Macros.Event.WorldHeartModule = Macros.Event.WorldHeartModule or 
{
	--世界之心升级
	WORLD_HEART_LEVELUP = "world_heart_levelup",
	--世界之心激活
	WORLD_HEART_LIGHTUP = "world_heart_lightup",
}

--领地
Macros.Event.ManorModule = Macros.Event.ManorModule or 
{
	MANOR_UP = "manor_up",
	MANOR_EVENT_UP = "manor_event_up",
	MANOR_SKILL = "manor_skill",
	MANOR_PATROL_FRIEND = "manor_patrol_friend",
	MANOR_FRIEND = "manor_friend"
}

--夺宝
Macros.Event.PlunderedTreasuresModule = Macros.Event.PlunderedTreasuresModule or 
{
	PLUNDEREDTREASURES_TARGET_LIST = "plunderedtreasures_target_list",
	PLUNDEREDTREASURES_REFRASH_TARGET_LIST = "plunderedtreasures_refrash_target_list",
	PLUNDEREDTREASURES_FIVETIMES = "plunderedtreasures_fivetimes",
	PLUNDEREDTREASURES_REFRASHVIEW = "plunderedtreasures_refrashview",
	PLUNDEREDTREASURES_REFRASHAVOIDWARTIME = "plunderedtreasures_refrashavoidwartime",
	PLUNDEREDTREASURES_OPENWAR = "plunderedtreasures_openwar",
	PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW = "plunderedtreasures_close_fightenemyview",
	--夺宝进度奖励领取
	PLUNDEREDTREASURES_PROGRESS_REWARD = "plunderedtreasures_progress_reward",
}

--公会
Macros.Event.UnionModule = Macros.Event.UnionModule or 
{
	UNION_ESTABLISH = "union_establish",
	--工会数据
	UNION_DATE = "union_date",
	--工会大厅刷新
	UNION_HALL_DATE_UP = "union_hall_date_up",
	--工会申请列表
	UNION_APPLY = "union_apply",
	--工会成员列表
	UNION_MEMBER = "union_member",
	--动态
	UNION_TRENDS = "union_trends",
	--购买记录
	UNION_BUY_RECORD = "union_buy_record",
	--购买
	UNION_BUY = "union_buy",
	--限时
	UNION_DESENO = "union_Deseno",
	--建设
	UNION_MYUNION = "union_myunion",
	--搜索列表
	UNION_JOIN = "union_join",
	--技能列表
	UNION_KILL = "union_kill",
	--留言
	UNION_GUESTBOOK = "union_guestbook",
	--公会排行榜
	UNION_RANK = "union_rank",
	--公会副本
	UNION_FB = "union_fb",
	--公会关卡信息
	UNION_FB_DATA = "union_fbData",
	--公会关卡信息
	UNION_UP_TIME = "union_up_time",
	--公会奖励领取
	UNION_AWARD_GET = "union_award_get",
	--公会副本奖励记录
	UNION_REWARD_RECORD = " union_reward_record",
	--公会副本伤害记录
	UNION_HARM_RECORD = " union_harm_record",
	--公会副本奖励记录
	UNION_GET_RECORD = " union_get_record",
	--公会副本击杀
	UNION_MURBER = " union_murder",
	--公会关卡奖励
	UNION_FB_AWARD = "union_fb_award",
	--弹劾
	UNION_ACCUSE = "union_accuse",

	--占领刷新
	UNION_OCCUPY = "union_occupy",
	--占领奖励
	UNION_OCCUPY_AWARD = "union_occupy_award",
	--星球信息
	UNION_STAR_INFO = "union_star_info",
	--星球信息
	UNION_STAR_LST = "union_star_lst",
 	--星球宣战
 	UNION_WAR_UP = "union_war_up",
 	--次数购买
 	UNION_BUY_TIME = "union_buy_time",
 	--塔
 	UNION_TOWER = "union_tower",
 	--布防
 	UNION_DEFENSE = "union_defense"
}

--竞技场
Macros.Event.ArenaModule = Macros.Event.ArenaModule or 
{
	ARENA_UPDTAE_SHOPLIST = "arena_update_shoplist",
	ARENA_UPDTAE_SHOPMONEY = "arena_update_shopmoney",
	ARENA_UPDTAE_ANIMATION = "arena_update_animation",

	--竞技场快速挑战结果
	ARENA_QUICK_BATTLE_RESP = "arena_quick_battle_resp",

	--竞技场快速挑战条目回调
	ARENA_QUICK_BATTLE_CELL_RESP = "arena_quick_battle_cell_resp",

	--竞技场英雄动作响应
	ARENA_QUICK_BATTLE_ACTION_RESP = "arena_quick_battle_action_resp",
}

--日常任务与成就
Macros.Event.DaliyMissionAndAchievementModule = Macros.Event.DaliyMissionAndAchievementModule or 
{
	REFRASH_DALIYMISSION_LIST = "refrash_daliymission_list",
	REFRASH_DALIYMISSION_POINT = "refrash_daliymission_point",
	ACHIEVEMENT_LIST = "achievement_list",
	REFRASH_ACHIEVEMENT_LIST = "refrash_achievement_list",
	REFRASH_ACHIEVEMENT_CELL = "refrash_achievement_cell",
}

--获取途径
Macros.Event.ApproachOfAchievingModule = Macros.Event.ApproachOfAchievingModule or 
{
	OPEN_APPROACHVIEW = "open_approachview",
	DELETE_APPROACHVIEW = "delete_approachView",
}

--世界boss
Macros.Event.WorldBossModule = Macros.Event.WorldBossModule or 
{
	WORLDBOSS_INFO = "worldboss_info",
	WORLDBOSS_CAST = "worldboss_cast",
	WORLDBOSS_SCORE = "worldboss_score",
	WORLDBOSS_AWARD = "worldboss_award",
	WORLDBOSS_RANK = "worldboss_rank",
	WORLDBOSS_NOTICE = "worldboss_notice",
}

--领奖中心
Macros.Event.AcceptPrizeCenterModule = Macros.Event.AcceptPrizeCenterModule or 
{
	REWARD_DELETE_ITEM = "reward_delete_item",
}

--VIP
Macros.Event.VIPModule = Macros.Event.VIPModule or 
{
	VIP_BUY = "VIP_buy",
}

--新手指引
Macros.Event.LeadModule = Macros.Event.LeadModule or 
{
	--开始
	START = "lead_start",
	--结束
	STOP = "lead_stop",

	--指引切换到下一个步骤
	CHECK_NEXT_SECTION = "lead_check_next_section",

	--特殊开始
	START_SP = "lead_start_sp",
}

--剧情
Macros.Event.StoryModule = Macros.Event.StoryModule or 
{
	RECOMMEND_LINEUP = "story_recommend_lineup",
}

--杂项
Macros.Event.SundryModule = Macros.Event.SundryModule or 
{
	--某个模块系统
	SINGLE_SYSTEM_RED_DOT_UPDATE = "single_system_red_dot_update",
	--新一天的通知
	NEW_DAY_NOTIFY = "new_day_notify",	
	--客服历史
	ISSUE_HISTORY = "issue_history",	
	--选择
	SELECT_PATH = "select_path",
}

--活动
Macros.Event.ActivityModule = Macros.Event.ActivityModule or 
{
	ACTIVITY_REGISTRATION = "Activity_Registration",
	
	ACTIVITY_MEDICINE = "Activity_Medicine",

	ACTIVITY_SEVENDAY = "Activity_SevenDay",

	ACTIVITY_SEVENDAY_GET = "Activity_SevenDay_Get",

	ACTIVITY_SEVENDAY_BUY = "Activity_SevenDay_Buy",

	ACTIVITY_SEVENDAY_CHOOSE_BUY = "Activity_SevenDay_Choose_Buy",

	ACTIVITY_GIFT_BAG = "Activity_Gifi_Bag",

	--摇钱树
	ACTIVITY_MONEY = "Activity_Monet",
	--摇钱树领取
	ACTIVITY_GET_MONEY = "Activity_Get_Monet",

	--冲级特惠
	ACTIVITY_SHOP = "Activity_Shop",
	--冲级特惠记录
	ACTIVITY_SHOP_RECORD = "Activity_Shop_record",

	--等级礼包
	ACTIVITY_LEVEL_GIFTBAG = "Activity_Level_GiftBag",

	--vip礼包
	ACTIVITY_VIP_GIFT = "Activity_Vip_Gift",

	--基金
	ACTIVITY_FUND = "Activity_Fund",
	--基金记录
	ACTIVITY_FUND_RECORD = "Activity_Fund_Record",

	--壕签
	ACTIVITY_LUXURY = "Activity_Luxury",

	--登录奖励
	REGISTER_AWARD = "register_award",

	--单次充值
	ACTIVITY_ONE_RECHARGE = "Activity_One_Recharge",

	--累次招募
	ACTIVITY_RECRUIT = "Activity_Recruit",

	--首充FirstRecharge
	ACTIVITY_FIRST_RECHARGE = "Activity_First_Recharge",

	--今日推荐
	ACTIVITY_RECOMMEND = "ACTIVITY_RECOMMEND" , 

	--今日推荐festival
	ACTIVITY_FESTIVAL = "ACTIVITY_FESTIVAL" , 

	--今日推荐festival
	ACTIVITY_FESTIVAL_RED = "ACTIVITY_FESTIVAL_RED" , 
}

--功能预告
Macros.Event.RecommendLineupModule = Macros.Event.RecommendLineupModule or 
{
	
}

--特殊逻辑事件
Macros.Event.SpLogicModule = Macros.Event.SpLogicModule or 
{
	PAUSE = "pause",
	RESUME = "resume",

	GAME_PLAYING_START_HANDLE = "game_playing_start_handle",
}

Macros.Event.HeroShopModule = Macros.Event.HeroShopModule or 
{
	HERO_SHOP = "hero_shop",
	AWAKEN_SHOP = "awaken_shop",
}

Macros.Event.ErrorNotifyModule = Macros.Event.ErrorNotifyModule or 
{
	ERROR_NOTIFY = "error_notify",
}

Macros.Event.SpiritModule = Macros.Event.SpiritModule or 
{
	SPIRIT_DATA_RESP = "spirit_data_resp",
	SPIRIT_ON_FIGHT_RESP = "spirit_on_fight_resp",

	--强化
	SPIRIT_ENHANCE_RESP = "spirit_enhance_resp",
	--培养
	SPIRIT_TRAIN_RESP = "spirit_train_resp",
	--精灵祝福
	SPIRIT_BLESS_RESP = "spirit_bless_resp",
}

Macros.Event.MaincityModule = Macros.Event.MaincityModule or 
{
	RESET_REGION_ENTRYS = "reset_cityicon_resp",
	REFRASH_HERO_ADVENT_RESP = "refrash_hero_advent_resp",
	OPEN_TEAM_INVITE_MESSAGE = "open_team_invite_message",
	ONLINE_REWARD_PUSH = "online_reard_push",
	RECOMMEND_LINEUP = "recommend_lineup",
}

Macros.Event.ChargeModule = Macros.Event.ChargeModule or 
{
	CHARGE_DATA_RESP = "CHARGE_DATA_RESP",
	RECHARGE_SUCCESS = "recharge_success",
	RECHARGE_CANCEL = "recharge_cancel",
	RECHARGE_FAILED = "recharge_failed",
	RECHARGE_COUNTDOWN = "recharge_countdown",
}

Macros.Event.PVPModule = Macros.Event.PVPModule or 
{
	PVP_DATA = "pvp_data",
	PVP_RANK = "pvp_rank",
	PVP_AWARD = "pvp_award",
	PVP_CANCEL = "pvp_cancel",
}

Macros.Event.MonsterStormCastleModule = Macros.Event.MonsterStormCastleModule or 
{
	ATTACKCITY_ENTER_RESP = "attackcity_enter_resp",
	ATTACKCITY_ENTER_NOTICE = "attackcity_enter_notice",
	ATTACKCITY_LEAVE_NOTICE = "attackcity_leave_notice",
	ATTACKCITY_GOTO_NOTICE = "attackcity_goto_notice",
	ATTACKCITY_MONTSER_LIST_NOTICE = "attackcity_monster_list_notice",
	ATTACKCITY_MONTSER_DATA_NOTICE = "attackcity_monster_data_notice",
	TREASURE_BOX_LIST_NOTICE = "treasure_box_list_notice",
	TREASURE_BOX_DISAPPEAR_NOTICE = "treasure_box_disappear_notice",
	ATTACKCITY_TAKE_BOSSREWARD_RESP = "attackcity_take_bossreward_resp",
	ATTACKCITY_BOSS_DEAD = "attackcity_boss_dead",
}

Macros.Event.TitleModule = Macros.Event.TitleModule or 
{
	TITLE = "Title",
}

Macros.Event.ChasmModule = Macros.Event.ChasmModule or 
{
	CHASM_GET_PACK = "Chasm_get_pack",

	CHASM_DATA = "Chasm_data",

	CHASM_AWARD = "Chasm_award",

	CHASM_RANK = "Chasm_rank",

	CHASM_SHOP = "Chasm_shop",

	CHASM_WAVE = "Chasm_wave",

	CHASM_FIRST_LOSE_LEAD = "Chasm_first_lose_lead",
}


Macros.Event.MorrowModule = Macros.Event.MorrowModule or 
{
	MORROW_AWARD = "Morrow_award",
}

Macros.Event.KnighthoodModule = Macros.Event.KnighthoodModule or 
{
	KNIGHTHOOD = "Knighthood",
}

Macros.Event.SuperDiscountModule = Macros.Event.SuperDiscountModule or 
{
	SUPERDISCOUNT = "SuperDiscount",
}

--直购特惠
Macros.Event.DirectBuyModule = Macros.Event.DirectBuyModule or
{
	--直购商品数据反馈
	DIRECT_SHOP_DATA_RESP = "direct_shop_data_resp",

	--领取直购商品反馈
	TAKE_DIRECT_BUY_GOODS_RESP = "take_direct_buy_goods_resp",

	--兑换商品数据
	CONVERT_DATA_RESP = "direct_convert_data_resp",

	--精准推荐购买数据
	DIRECT_HOT_RECOMMENT_SHOP_DATA_RESP = "direct_hot_recomment_shop_data_resp",

	--精准推荐领取
	TAKE_ACCURATE_RECOMMEND_GIFT_RESP = "take_accurate_gift_resp",

	--热卖商品数据响应
	HOT_SALE_GOODS_DATA_RESP = "hot_sale_goods_data_resp",
}

--宝石
Macros.Event.GemModule = Macros.Event.GemModule or
{
	--选择英雄
	SELECT_HERO = "select_hero",
	--刷新内容
	UP_CONTENT = "up_content",
	--刷新列表
	UP_LST = "up_lst",
	--合成
	COMPOUND = "compound",
	--分解
	DECOMPOSE = "decompose",
	--镶嵌
	INLAY = "inlay",
	--卸下
	DISCHARGE = "discharge",
}

--邮件
Macros.Event.EmailModule = Macros.Event.EmailModule or
{
	--关闭邮件界面
	CLOSE_EMAIL_VIEW = "close_email_view",
}

--组队
Macros.Event.TeamModule = Macros.Event.TeamModule or
{	
	--处理邀请信息队列变化
	INVITED_TO_JOIN_TEAM_NOTICE = "invited_to_join_team_notice",
	--解散队伍通知
	BREAK_THE_TEAM = "break_the_team",
	--离开队伍通知
	LEAVE_THE_TEAM = "leave_the_team",
	--前往NPC
	MOVE_TO_NPC_NOTICE = "move_to_npc_notice",
	--结束剧情响应
	TEAM_DUNGEON_DIALOG_OVER_RESP = "team_dungeon_dialog_over_resp",
	--新任务通知
	TEAM_DUNGEON_NEW_TASK_NOTIFY = "team_dungeon_new_task_notify",
	--答题通知
	TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY = "team_dungeon_answer_question_notify",
	--答题结束后显示正确答案
	TEAM_DUNGEON_SHOW_RIGHT_ANSWER = "team_dungeon_show_right_answer",
	--组队界面显隐
	TEAM_DUNGEON_CREATE_TEAM_VIEW_VISIBLE = "team_dungeon_create_team_view_visible",
	--组队获取好友列表
	TEAM_DUNGEON_GET_FRIEND_LIST = "team_dungeon_get_friend_list",
	--拒绝加入队伍
	TEAM_DUNGEON_REJECT_JOIN_TEAM = "team_dungeon_reject_join_team",
	--新成员加入队伍
	TEAM_DUNGEON_NEW_MEMBER_JOIN_TEAM = "team_dungeon_new_member_join_team",
	--抽奖响应
	TEAM_DUNGEON_LOTTERY_RESP = "team_dungeon_lottery_resp", 
	--任务剩余时间
	TEAM_DUNGEON_REMAIN_TIME = "team_dungeon_retain_time", 
	--任务时间到了要去打BOSS
	TEAM_DUNGEON_TASK_TIME_TO_BOSS = "team_dungeon_task_time_to_boss", 
	--飞能量到法阵
	TEAM_DUNGEON_FLY_TO_ENERGY_ARRAY = "team_dungeon_fly_to_energy_array", 
	--击杀BOSS
	TEAM_DUNGEON_BOSS_DEAD = "team_dungeon_boss_dead",
	--BOSS战开始
	TEAM_DUNGEON_BOSS_BATTLE_START = "team_dungeon_boss_battle_start",
	--BOSS战结算
	TEAM_DUNGEON_BOSS_BATTLE_RESULT = "team_dungeon_boss_battle_result",
	--组队副本BOSS血量变化
	TEAM_DUNGEON_BOSS_HP_CHANGE = "team_dungeon_boss_hp_change",
	--副本通关记录数据
	TEAM_DUNGEON_PASS_RECORD_DATA = "team_dungeon_pass_record_data",
	--组队副本数据响应
	TEAM_DUNGEON_DATA_RESP = "team_dungeon_data_resp",
	--组队副本商店购买记录响应
	TEAM_DUNGEON_SHOP_RECORD_RESP = "team_dungeon_shop_record_resp",
	--匹配成功进入世界地图
	TEAM_DUNGEON_MATCH_SUCCESS = "team_dungeon_match_success",
	--快捷聊天
	TEAM_DUNGEON_QUICK_CHAT_RESP = "team_dungeon_quick_chat_resp",
}

--NewLst
Macros.Event.NewModule = Macros.Event.NewModule or
{	
	-- --背包
	-- PACK = "Pack",
	-- --装备
	-- EQUIP = "Equip",
	-- --圣器
	-- TREASURE = "Treasure",
	-- --英雄
	-- HERO = "Hero",
	PACK_EXTEND = "Bag_Extend",
}

--天梯
Macros.Event.LadderModule = Macros.Event.LadderModule or
{	
	--天梯信息
	LADDER = "Ladder" , 
	--天梯匹配
	LADDER_VS = "Ladder_VS" , 
	--天梯排行榜
	LADDER_RANK = "Ladder_Rank" , 
	--天梯任务
	LADDER_TASK = "Ladder_Task" ,
	--天梯任务
	LADDER_HONOR = "Ladder_Honor" ,
}

--卡牌
Macros.Event.DivinationModule = Macros.Event.DivinationModule or
{	
	DIVINATION = " Divination",

	--占卜增加卡牌
	DIVINATION_ADD_CARD = "DIVINATION_ADD_CARD" , 
	--占卜删除卡牌
	DIVINATION_REMOVE_CARD = "DIVINATION_REMOVE_CARD" , 
	--占卜更新卡牌
	DIVINATION_UPDATE_CARD = "DIVINATION_UPDATE_CARD",
	--占卜改变卡牌
	DIVINATION_CHANGE_CARD = "DIVINATION_CHANGE_CARD",
	--占卜升级
	DIVINATION_UP = "Divination_Up" ,	
	--占卜进阶
	DIVINATION_BREAK = "Divination_Break" ,
	--命运商店刷新
	DIVINATION_DESTINY = "Divination_Destiny",
	--卡牌排行
	DIVINATION_RANK = "Divination_Rank",
	--卡牌占卜发牌
	DIVINATION_DEAL = "Divination_Deal",
	--卡牌占卜翻牌
	DIVINATION_DRAW = "Divination_Draw",
	--卡牌占卜洗牌
	DIVINATION_SHUFFLE = "Divination_Shuffle",
	--收藏更新
	COLLECT_UP = "Collect_Up",
	--收藏书更新
	COLLECT_BOOK_UP = "Collect_Book_Up",
	--收藏书成就
	COLLECT_ACHIEVEMENT = "COLLECT_ACHIEVEMENT",

	--穿戴卡牌
	DIVINATION_DRESS = "Divination_Cross",
	--卸下卡牌
	DIVINATION_DISCHARGE = "Divination_Discharge",	--星魂商店
	
	DIVINATION_STAR_SHOP = "Divination_Star_Shop",

	--倒计时
	DIVINATION_LOW_RENEW = "Divination_Low_Renew",
	--倒计时
	DIVINATION_HIGH_RENEW = "Divination_High_Renew",
	--激活成就
	DIVINATION_COLLECT_ACHIEVEMENT_ACTIVATED = "Divination_Collect_Achievement_Activated",
	--兑换商店
	DIVINATION_CONVERT = "Divination_Convert",

	--选择
	DIVINATION_SELECT = " Divination_select",
}

--出售界面
Macros.Event.DebriSellModule = Macros.Event.DebriSellModule or
{	
	--出售界面刷新
	DEDRISELL_UP = "DebriSellUp" , 
}

--限时活动
Macros.Event.LimitActivityModule = Macros.Event.LimitActivityModule or
{	
	--圣诞活动
	--进入战斗响应
	CHRISTMAS_BATTLE_ENTER_RESP = "christmas_bttle_enter_resp", 
	--请求圣诞节数据响应
	CHRISTMAS_DATA_RESP = "christmas_data_resp", 
	--攻击Boss响应
	CHRISTMAS_ATTACK_BOSS_RESP = "christmas_attack_boss_resp",
	--攻击Boss响应
	GET_SHOPDATA_BY_SHOPTYPE = "get_shopdata_by_shoptype",
	--领取奖励响应
	GET_BOSS_REWARD_RESP = "get_boss_reward_resp",
	--Boss死亡通知
	CHRISTMAS_BOSSDEATH_NOTIFY = "christmas_bossdeath_notify",  
	--购买物品通知
	CHRISTMAS_SHOP_BUY_NOTIFY = "christmas_shop_buy_notify",  
	--扭蛋机抽奖响应
	ND_LOTTERY_RESP = "nd_lottery_resp",
	--抽奖记录响应
	ND_LOTTERYRECORD_RESP = "nd_lotteryrecord_resp",
	--阵营招募数据响应
	CAMP_RECRUIT_DATA_RESP = "camp_recruit_data_resp",
	--阵营招募记录响应
	CAMP_RECRUIT_RECORD_RESP = "camp_recruit_record_resp",
	--阵营招募响应
	CAMP_RECRUIT_RESP = "camp_recruit_resp",
	--阵营招募积分任务奖励响应
	CAMP_RECRUIT_SCORE_TASK_REWARD_RESP = "camp_recruit_score_task_reward_resp",

	REFRASH_ACTIVITY_TIME_RESP = "refrash_limit_activity_time_resp",
	REFRASH_ACTIVITY_END = "refrash_limit_activity_end",	
}

--在线奖励
Macros.Event.OnlineawardModule = Macros.Event.OnlineawardModule or
{	
	--在线奖励信息
	ONLINE_REWARD_DATA = "OnlineRewardData" , 

	--在线倒计时
	ONLINE_COUNTDOWN = "OnlineCountdown"
}

--七天在线
Macros.Event.PushSevenModule = Macros.Event.PushSevenModule or
{	
	--七天在线信息
	PUSH_SEVEN_DATA = "PushSevenData" , 

	--七天在线信息
	PUSH_SEVEN_AWARD = "PushSevenAward" , 

	--七天下次
	PUSH_SEVEN_NEX = "PushSevenNex" , 

}

--充值回馈
Macros.Event.FeedbackModule = Macros.Event.FeedbackModule or
{	
	--充值回馈信息
	FEEDBACK_DATA = "Feedback_data" , 
}

--女王
Macros.Event.QueenModule = Macros.Event.QueenModule or
{	
	--女王信息
	QUEEN_DATA = "Queen_data" , 

	--在线倒计时
	QUEEN_COUNTDOWN = "QueenCountdown"
}

--自定义超值购
Macros.Event.CustomPurchaseModule = Macros.Event.CustomPurchaseModule or
{	
	--信息刷新
	CUSTOM_PURCHASE_DATA = "CustomPurchase_Data" , 

	--领取奖励
	DISCOUNT_BUY_REWARD = "Discount_Buy_Reward"
}

--转圈
Macros.Event.TurntableModule = Macros.Event.TurntableModule or
{	
	--转圈
	WHIRL_DATA = "whirl_Data" , 

	TURNTABLE_DATA = "turntable_data"
}

--水晶
Macros.Event.CrystalShopModule = Macros.Event.CrystalShopModule or
{	
	--转圈
	CRYSTAL_BUY_DATA = "Crystal_Buy_Data" , 

	CRYSTAL_FLIP_OVER = "Crystal_Flip_Over" ,
}

--世界之心等级
Macros.Event.PushWorldLevelModule = Macros.Event.PushWorldLevelModule or
{	
	--世界之心等级数据
	WORLDLEVEL_DATA = "WorldLevelData" , 
}

--积分赛
Macros.Event.IntegralModule = Macros.Event.IntegralModule or
{	
	--积分赛数据响应
	SCORE_MATCHDATA_RESP = "ScoreMatchDataResp", 
	--积分赛荣耀商店购买物品响应
	GLORY_SHOP_BUY_ITEM_RESP = "GloryShopBuyItemResp",
	--领取挑战任务奖励响应
	SCORE_MATCH_TAKE_TASKREWARD_RESP = "ScoreMatchTakeTaskRewardResp",
}

--资源战
Macros.Event.ResourceWarModule = Macros.Event.ResourceWarModule or
{	
	--资源战数据响应
	RESOURCE_WAR_DATA = "ResourceWarData", 

	--资源战数据响应
	RESOURCE_WAR_ENEMY = "ResourceWarEnemy", 

	--资源战战报
	RESOURCE_BATTLE_RECORD = "ResourceBattleRecord" ,

	--资源战公会
	RESOURCE_UNION = "ResourceUnion" , 

	--资源战科技
	RESOURCE_SKILL = "ResourceSkill" , 

	--章节
	RESOURCE_CHAPTER = "ResourceChapter",

	--商店
	RESOURCE_SHOP = "ResourceShop",

}

--红点更新
Macros.Event.RedModule = Macros.Event.RedModule or
{	
	--红点更新
	RED_UPDATE = "RedUpData", 
}