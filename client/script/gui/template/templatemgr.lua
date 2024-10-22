
--[[
%% @module: UI模板管理器
%% @author: swordman sue
%% @created: 2016-07-08
--]]

GUI = GUI or {}

--TODO：添加创建模板的接口(自动require相关文件)
GUI.TemplateMgr = GUI.TemplateMgr or BaseClass(EventAndTimerWrapper)

GUI.TemplateRegistry =
{
	--物品信息
	ITEM_INFO = {"TemplItemInfo", "templ_item_info"},	
	GIFT_INFO = {"TemplGiftInfo", "templ_gift_info"},

	--装备详情
	TEMPL_EQUIP_INFO = {"templEquipInfo", "templ_equip_info"},		
	TEMPL_EQUIP_INFO_PAGE = {"TemplEquipInfoPage", "templ_equip_info_page"},	
	TEMPL_EQUIP_DETAIY_ITEM = {"TemplEquipDetaiyItem", "templ_equip_detaiy_item"},		
	TEMPL_EQUIP_DETAIY_ITEM_PROPERTY = {"TemplEquipInfoProperty", "templ_equip_info_property"},	

	--卡牌详情
	TEMPL_CARD_INFO = {"LogicGUI.TemplCardInfo", "templ_equip_info" , "lgui/card/templ_card_info" },	

	--英雄信息
	HERO_INFO = {"TemplHeroInfo", "templ_hero_info"},	

	--换头像
	ROLE_FACELIFT = {"TemplRoleFacelift", "templ_role_facelift"},	
	--换名字
	ROLE_RENAME = {"TemplRoleRename", "templ_role_rename"},	

	--登陆相关
	LOGIN_SERVER_ITEM = {"TemplLoginServerItem", "templ_login_serveritem"},
	LOGIN_SERVER_ZONE = {"TemplLoginServerZone", "templ_login_serverzone"},

	--SDK界面
	TEMPL_SDKREGISTER = {"TemplSelfSdkRegister", "templ_self_sdk_register"},

	--活动公告
	TEMPL_ACTIVITY_NOTICE = {"Modules.TemplActivityNotice", "templ_notice", "modules/notice/templ_activity_notice"},

	--系统公告(走马灯)
	TEMPL_SYSTEM_NOTICE = {"Modules.TemplSystemNotice", "", "modules/notice/templ_system_notice"},

	--战斗相关
	BATTLE_CHOOSE_CARD = {"TemplChooseCard","templ_battle_choosecard"},	

	--主线副本、英雄副本
	DUNGEON_CHAPTER = {"TemplDungeonChapter", "templ_dungeon_chapter"},	
	DUNGEON_GAMELEVEL = {"TemplDungeonGameLevel", "templ_dungeon_gamelevel"},
	HERO_DUNGEON_GAMELEVEL = {"TemplHeroDungeonGameLevel", "templ_hero_dungeon_gamelevel"},
	ELITE_DUNGEON_GAMELEVEL = {"TemplEliteDungeonGameLevel", "templ_elite_dungeon_gamelevel"},
	DUNGEON_BOXINFO = {"TemplDungeonBoxInfo", "templ_dungeon_boxinfo"},
	DUNGEON_GAMELEVEL_CHALLENGE = {"TemplDungeonGameLevelChallenge", "templ_dungeon_gamelevel_challenge"},
	DUNGEON_PASSEDALL_BOXTIP = {"TemplDungeonPassedAllTip","templ_dungeon_passed_all_tip"},
	DUNGEON_FIRSTWIN = {"Modules.TemplDungeonFirstWinView", "templ_dungeon_firstwin_view", "lgui/templ_dungeon_firstwin_view"},
	HERO_DUNGEON_SPECIAL_GAMELEVEL_CHALLENGE = {"TemplHeroDungeonSpecialGameLevelChallenge", "templ_hero_dungeon_special_gamelevel_challenge"},
	DUNGEON_EXTERNAL_AREA_VIEW = {"TemplDungeonExternalAreaView","templ_dungeon_external_area_view"},
	DUNGEON_EXTERNAL_AREA_CELL = {"TemplDungeonExternalAreaCell","templ_dungeon_external_area_cell"},
	DUNGEON_ASSIST_IN_FIGHTING_CELL = {"TemplDungeonAssistInFightingCell","templ_assist_in_fighting_cell"},
	DUNGEON_MESSAGE_CELL = {"TemplDungeonMessageCell","templ_dungeon_message_board_cell"},
	SPIRIT_COMMON_SKILL_CHOOSE_VIEW = {"TemplSpiritCommonSkillChooseView","templ_spirit_common_skill_view"},
	DUNGEON_MESSAGE_BOARD = {"TemplMessageBoard","templ_message_board"},

	--副本扫荡
	DUNGEON_CLEAN_RESULT = {"TemplDungeonCleanResult", "templ_dungeon_clean_result"},
	DUNGEON_CLEAN_RESULT_ITEM = {"TemplDungeonCleanResultItem", "templ_dungeon_clean_result_item"},
	DUNGEON_CLEAN_RESULT_TOTAL_ITEM = {"TemplDungeonCleanResultTotalItem", "templ_dungeon_clean_result_total_item"},

	--日常副本
	DUNGEON_DAILY_DIFFICULTLEVEL = {"TemplDailyDifficultLevel", "templ_daliy_difficultlevel"},
	DUNGEON_DAILY_ITEM = {"TemplDailyDungeonItem", "templ_daliy_dungeonitem"},
	DUNGEON_PASSED_NEW_DIFFICULTLEVEL = {"TemplDailyDungeonPassedNewDifficultLevel", "templ_passed_new_difficultlevel_view"},

	--查看玩家信息
	CHECKROLEINFO = {"LogicGUI.TemplCheckRoleInfo","checkroleinfo" , "lgui/chat/templ_checkroleinfo"},	--聊天信息框

	--背包相关
	PACK_ONEUSE = {"LogicGUI.TemplPackOneuse", "templ_pack_oneuse" , "lgui/pack/templ_pack_oneuse" },	--单个使用
	PACK_SELECT = {"LogicGUI.TemplPackSelect", "templ_pack_select" , "lgui/pack/templ_pack_select" },	--选择使用
	PACK_ITEM = {"TemplPackItem","templ_pack_item"},

	--出售条目
	SALE_ITEM = {"SaleItem","sale_item"},
	SALE_SCREEN = {"Modules.TemplDebrisellScreen","sale_screen","lgui/templ_debrisell_screen"},

	--装备相关
	EQUIP_PROP_ITEM = {"TemplEquipPropItem", "templ_equip_propItem"},	--装备条目
	EQUIP_DEDRIS_ITEM = {"TemplEquipDebrisItem", "templ_equip_debrisItem"},	--装备条目
	
	EQUIP_INTENSIFY = {"TemplEquipIntensify", "templ_equipintensify"},	--装备强化
	EQUIP_REFINE = {"TemplEquipRefine", "templ_equiprefine"},	--装备精炼
	EQUIP_EXCHANGE = {"TemplEquipExchange","templ_equip_exchange"},--装备更换

	EQUIP_ONEKEY_NATURE = {"TemplEquipOnekeyNature","templ_equip_onekey_nature"},--强化属性
	EQUIP_ONEKEY_MASTRE = {"TemplEquipOnekeyMaster","templ_equip_onekey_master"},--大师属性

	EQUIP_UP_STAR = {"TemplEquipUpstar","templ_equip_upstar"},--装备升星

	--锻造大师
	EQUIP_FORGE = {"TemplEquipForge" , "templ_forge"},		--锻造大师	
	EQUIP_FORGE_ITEM = {"TemplForgeItem" , "templ_forge_item"},		--锻造大师

	--商店相关
	SHOP_COMMON_TOOLS = {"TemplShopViewTools", "templ_store_cell"},   --通用条目
	SHOP_BUY_TIPVIEW = {"LogicGUI.TemplShopBuy","templ_store_buytip","lgui/shop_part/templ_shop_buy"},

	--英雄相关
	HERO_LINEUP_ITEM = {"TemplHeroLineupItem", "templ_hero_lineup_item"},
	HERO_LINEUP_INFO = {"TemplHeroLineupInfo", "templ_hero_lineup_info"},		
	HERO_BREAK_SUCC = {"TemplHeroBreakSucc", "templ_hero_break_succ"},	
	HERO_POTENTIAL_SUCC = {"TemplHeroPotentialSucc", "templ_hero_potential_success"},	
	HERO_TRAIN_VIEW = {"TemplHeroTrainView", "templ_hero_train_veiw"},	
	HERO_TRAIN_PRE_LOOK_VIEW = {"TemplHeroTrainPreLookView", "hero_train_prelook"},
	HERO_TRAIN_PRE_LOOK_CELL = {"TemplHeroTrainPreLookCell", "hero_train_prelookcell"},
	HERO_TRAIN_TIMES_CHOOSE_VIEW = {"TemplHeroTrainTimesChooseView", "hero_train_times_chooseview"},
	HERO_TRAIN_BREAK_REWARD_VIEW = {"TemplHeroTrainBreakRewardView", "templ_hero_train_break_reward"},
	HERO_ONEKEY_LEVELUP_TIPS_ONE = {"TemplHeroOnekeyLevelUpTipsOne","templ_hero_onekey_levelup_tips_one"},
	HERO_ONEKEY_LEVELUP_TIPS_TWO = {"TemplHeroOnekeyLevelUpTipsTwo","templ_hero_onekey_levelup_tips_two"},
	HERO_DIVINATION_LINEUP_INFO = {"TemplHeroDivinationInfo", "templ_hero_divination_info"},	

	--英雄觉醒
	HERO_AWAKEN_PROP = {"TemplHeroAwakenPropView","templ_hero_awakenProp"},
	HERO_AWAKEN_PROP_COMPOUND = {"TemplHeroAwakenPropCompoundView","templ_hero_awakenProp_compound"},
	HERO_AWAKEN_PROP_PRELOOK = {"TemplHeroAwakenPropPreLookView","templ_hero_awakenProp_preLook"},
	HERO_AWAKEN_PROP_PRELOOK_CELL = {"TemplHeroAwakenPropPreLookCell","templ_hero_awakenProp_preLookCell"},
	HERO_AWAKEN_PROP_HANDBOOK_CELL = {"TemplHeroAwakenPropHandBookCell","templ_hero_awakenProp_handBookCell"},
	HERO_AWAKEN_SPECIAL_APPROACH_CELL = {"TemplSpecialApproachOfAchievingCell","templ_awaken_approach_of_achieving_cell"},

	--英雄招募
	HERO_RECRUIT_PRELOOK = {"TemplHeroRecruitPreLook","templ_hero_recruit_prelook"},
	HERO_RECRUIT_TENCALL = {"TemplHeroRecruitTenCall","templ_hero_recruit_tencall"},
	HERO_RECRUIT_TENCALLCELL = {"TemplHeroRecruitTenCallCell","templ_hero_recruit_tencallcell"},
	HERO_SINGLE_RECRUIT_VIEW = {"LogicGUI.TemplRecruitSingleHeroView","templ_hero_recruit_onecall", "lgui/templ_recruit_singleHeroView"},
	VIP_RECRUIT_PRELOOK = {"TemplVipRecruitPrelook","templ_vip_recruit_prelook"},
	VIP_RECRUIT_PRELOOK_CELL = {"TemplVipRecruitPrelookCell","templ_vip_recruit_prelook_cell"},

	--英雄图鉴
	HERO_HANDBOOK_MAINVIEW = {"TemplHeroHandBookMainView","templ_hero_handbook_mainview"},
	HERO_HANDBOOK_HEADICON = {"TemplHeroHandBookHeadIcon","templ_handbook_heroicon_cell"},
	HERO_HANDBOOK_HEROMESSAGEVIEW = {"TemplHeroHandBookMessageView","templ_hero_hanbook_heromessage"},
	HERO_HANDBOOK_HEROMESSAGECELL = {"TemplHeroHandBookMessageCell","templ_hero_hanbook_heromessagecell"},
	HERO_HANDBOOK_HEROCHIPVIEW = {"GUI.TempHeroHandbookChipView","templ_hero_hanbook_herochip","lgui/treasure/plundered_treasures_chipview"},
	HERO_HANDBOOK_HEROTOGETHERATTACKVIEW = {"TemplHeroHandBookTogetherAttackView","templ_hero_hanbook_herotogetherattack"},
	HERO_HANDBOOK_HEROFETTERVIEW = {"TemplHeroHandBookFetterView","templ_hero_hanbook_herofetter"},
	HERO_HANDBOOK_HEROFETTERVIEWCELL = {"TemplHeroHandBookFetterViewCell","templ_hero_hanbook_herofettercell"},

	--英雄宿命信息
	HERO_REINFORCEMENTS_CELL = {"TemplReinforcementsCell","templ_reinforcementscell"},
	HERO_REINFORCEMENTS = {"TemplHeroReinforcements", "templ_reinforcements"},	
	HERO_REINFORCEMENTS_INFO = {"TemplHeroReinforcementsInfo", "templ_reinforcementsinfo"},	

	--英雄进化
	HERO_EVOLUTION_SUCC = {"TemplHeroEvolutionSucc","templ_hero_evolution_succ_view"},

	--获取途径
	APPROACH_OF_ACHIEVINGVIEW = {"LogicGUI.ApproachOfAchievingLayer","approach_of_achieving", "lgui/approach_of_achieving/approach_of_achieving_layer"},
	APPROACH_OF_ACHIEVINGCELL = {"TemplApproachOfAchievingCell","templ_approach_of_achieving_cell"},

	--英雄条目
	TEMPL_HERO_SHOP_ITEM = {"TemplHeroShopItem" , "templ_hero_shop_item"},
	
	--邮件相关
	EMAIL_SERVER_CELL = {"TemplEmailCell", "templ_email_cell"},

	--排行榜相关
	RANK_VIEW_CELL = {"TemplRankCell", "templ_rank_cell"},
	PLAYER_DETAILMESSAGE = {"LogicGUI.TemplPlayerDetailMessage", "templ_rank_best_detailmessage", "lgui/templ_player_detailmessage"},

	--分解
	RECYCLE_PREVIEW = {"TemplResolvePreview","templ_resolve_preview"},

	--叛军战斗入口
	REBEL_COMBAT = {"TemplRebelCombat","templ_rebel_combat"},
	--叛军商店
	REBEL_SHOP = {"TemplRebelShop","templ_rebel_shop"},
	REBEL_SHOP_ITEM = {"TemplRebelShopItem","templ_rebel_shop_item"},
	--叛军功勋奖励
	REBEL_AWARD = {"TemplRebelAward","templ_rebel_award"},
	--叛军排位
	REBEL_RANK = {"TemplRebelRank","templ_rebel_rank"},
	REBEL_RANK_ITEM = {"TemplRebelRankItem","templ_rebel_rank_item"},
	REBEL_RANK_FIND = {"LogicGUI.RebelLayer","templ_rebel_find" , "lgui/rebel/templ_rebel_find" },

	--好友相关
	FRIEND_ALL_CELL = {"TemplFriendAllCell","templ_friend_cell"},

	--爬塔商店
	TOWER_SHOP_ITEM = {"TowerShopItem","tower_shop_item"},
	TOWER_RANK = {"TemplTowerRank","templ_tower_rank"},
	--爬塔奖励
	TOWER_AWARD = {"TemplTowerAward","templ_tower_award"},
	--爬塔难度选择
	TOWER_DIFFICULTY_SELECT = {"TemplTowerDifficultySelect","templ_tower_difficulty_select"},
	--爬塔精英选择
	TOWER_EILTE = {"TemplTowerElite","templ_tower_elite"},
	TOWER_EILTE_ITEM = {"TemplTowerEliteItem","templ_tower_elite_item"},

	--无双密藏
	TOWER_TREASURE = {"TemplTowerTreasure","templ_tower_treasure"},

	--buff
	TOWER_BUFF = {"TemplTowerBuff","templ_tower_buff"},

	--扫荡
	TOWER_CLEAR = {"TemplTowerClear","tower_clear"},
	--扫荡条目
	TOWER_RESULT = {"TemplTowerResult","templ_tower_result"},
	--结算条目
	TOWER_RESULT_TOTAL = {"TemplTowerResultTotal","templ_tower_result_total"},
	
	--世界之心系统
	WORLD_HEART_PROPERTY = {"TemplWorldHeartPropertyView","templ_worldheart_propertyview"},
	WORLD_HEART_LIGHTUP = {"TemplWorldHeartLightUpView","templ_worldheart_lightup"},
	SPIRIT_INTRODUCE = {"TemplSpiritIntroduceView","templ_spirit_introduce_view"},

	--精灵系统
	TEMPL_SPIRITICON_CELL = {"TemplSpiritIconCell","templ_spiritIcon_cell"},
	TEMPL_SPIRIT_HANDBOOK_CELL = {"TemplSpiritHandBookCell","templ_spirit_handbook_cell"},
	TEMPL_SPIRIT_INTENSIFY_MASTER_VIEW = {"TemplSpiritIntensifyMasterView","templ_spirit_intensify_master_view"},
	TEMPL_SPIRIT_INTENSIFY_MASTER_CELL = {"TemplSpiritIntensifyMasterCell","templ_spirit_intensify_master_cell"},
	TEMPL_SPIRIT_ONEKEY_TRAIN = {"TemplSpiritOneKeyTrain","templ_spirit_onekey_train"},
	TEMPL_SPIRIT_SUBLIMATION_VIEW = {"TemplSpiritSublimationView","templ_spirit_sublimation_view"},
	TEMPL_SPIRIT_SUBLIMATION_SUCC = {"TemplSpiritSublimationSucc","templ_spirit_sublimation_succ"},
	TEMPL_SPIRIT_STARUP_SUCC = {"TemplSpiritStarUpSucc","templ_spirit_starup_succ"},
	
	--领地
	--选择界面
	MANOR_START = {"TemplManorStart","templ_manor_start"},
	--挑战界面
	MANOR_CHALLENGE = {"TemplManorChallenge","templ_manor_challenge"},
	--巡逻界面
	MANOR_PATROL = {"TemplManorPatrol","templ_manor_patrol"},
	--难度选择界面
	MANOR_DIFFICULTY = {"TempManorDifficulty","templ_manor_difficulty"},
	--奖励界面
	MANOR_DOUBEL_AWARD = {"TempManorDoubleAward","temp_manor_double_award"},
	--奖励界面
	MANOR_AWARD = {"TempManorAward","temp_manor_award"},

	--夺宝
	PLUNDERED_TREASURESLIST_CELL = {"TemplPlunderedTreasureListCell","templ_plundered_treasures_listcell"},
	PLUNDERED_TREASURES_AVOIDVIEW = {"TemplPlunderedTreasuresAvoidView","templ_plundered_treasures_avoidwar"},
	PLUNDERED_TREASURES_BUYDAOJUVIEW = {"TemplPlunderedTreasuresBuyDaoJuView","templ_plundered_treasures_buydaoju"},
	PLUNDERED_TREASURES_FIGHTENEMY = {"TemplPlunderedTreasuresFightEnemy","templ_plundered_treasures_fightenemy"},
	PLUNDERED_TREASURES_FIGHTENEMYVIEW = {"TemplPlunderedTreasuresFightEnemyView","templ_plundered_treasures_fightenemyview"},
	PLUNDERED_TREASURES_REPORT = {"TemplPlunderedTreasuresReport","templ_plundered_treasures_report"},
	PLUNDERED_TREASURES_REPORTVIEW = {"TemplPlunderedTreasuresReportView","templ_plundered_treasures_reportview"},
	PLUNDERED_TREASURES_RELIEVESTATETIPVIEW = {"TemplPlunderedTreasuresRelieveStateTipView","templ_plundered_treasures_relievestate_tipview"},

	PLUNDERED_TREASURES_DETAILMESSAGE = {"LogicGUI.TemplPlunderedTreasuresDetailMessageView","plundered_treasure_detailmessageview", "lgui/treasure/plundered_treasures_detailmessageview"},
	PLUNDERED_TREASURES_TREASUREMESSAGE = {"GUI.TemplPlunderedTreasuresDetailMessageView","templ_plundered_treasure_treasuremessage", "lgui/treasure/templ_plundered_treasures_detailmessageview"},

	PLUNDERED_TREASURES_ONEKEY_COMPOSE = {"TemplPlunderedTreasuresOneKeyCompose","templ_plundered_treasures_onekey_compose"},
	PLUNDERED_TREASURES_ONEKEY_TIPS = {"TemplPlunderedTreasuresOneKeyTips","templ_onekey_plundered_treasures_tips"},
	QUICK_PLUNDERED_TREASURES_CELL = {"TemplQuickPlunderedTreasuresCell","templ_quick_plundered_treasures_cell"},

	--竞技场
	ARENA_FIGHTREPORT_VIEW = {"TemplArenaReportView","templ_arena_fightreportview"},
	ARENA_FIGHTREPORT_CELL = {"TemplArenaReportCell","templ_arena_fightreportcell"},
	ARENA_HEROMESSAGE_CELL = {"TemplArenaHeroMessageCell","templ_arena_heromessagecell"},
	ARENA_RANK_VIEW = {"TemplArenaRankView","templ_arena_rankview"},
	ARENA_RANK_CELL = {"TemplArenaRankCell","templ_arena_rankcell"},
	ARENA_RANK_ENCOURAGE_CELL = {"TemplArenaRankEncourageCell","templ_arena_rank_encouragecell"},
	ARENA_REPUTATIONSHOPBUY_VIEW = {"TemplArenaReputationShopBuyView","templ_arena_reputationshopbuyview"},
	ARENA_REPUTATIONSHOP_CELL = {"TemplArenaReputationShopCell","templ_arena_reputationshopcell"},
	ARENA_RANK_BREAK_VIEW = {"TemplArenaRankBreakView","templ_arena_rank_break"},
	ARENA_FIGHT_FIVETIMES_VIEW = {"TemplArenaFightFiveTimesView","templ_arena_fight_fivetimes_view"},
	ARENA_FIGHT_FIVETIMES_CELL = {"TemplArenaFightFiveTimesCell","templ_arena_fight_fivetimes_cell"},

	--留言板
	UNION_LEAVE_WORD = {"TemplUnionLeaveWord","union_leave_word"},
	--留言板条目
	UNION_LEAVE_WORD_ITEM = {"TemplUnionLeaveWordItem","union_leave_word_item"},

	--排行榜
	UNION_RANK = {"TemplUnionRank","union_rank"},
	--排行榜条目
	UNION_RANK_ITEM = {"TemplUnionRankItem" , "union_rank_item"},
	
	-- 公会
	UNION_ESTABLISH = {"LogicGUI.TemplUnionEstablish","union_establish" , "lgui/templ_union_establish/templ_union_establish" },
	--搜索
	UNION_JOIN = {"LogicGUI.TemplUnionJoin","union_join" , "lgui/templ_union_join/templ_union_join" },
	--搜索条目
	UNION_JOIN_ITEM = {"UnionJoinItem","union_join_item"},
	--图标墙
	UNION_ICON_WALL = {"TemplUnionIconWall","templ_union_icon_wall"},
	--图标	
	UNION_KILL_ITEM = {"TemplUnionKillItem","templ_union_kill_item"},
	--审查框
	UNION_AUDIT = {"TemplUnionAudit","templ_union_audit"},
	--审查条
	UNION_AUDIT_ITEM = {"TemplUnionAuditItem","templ_union_audit_item"},
	--成员条
	UNION_HALL_LIST_ITEM = {"TemplUnionHallListItem","templ_union_hall_list_item"},
	--输入框
	UNION_IMPORT = {"TemplUnionImport","union_import"},
	--公会商店条目
	UNION_SHOP_ITEM = {"TemplUnionShopItem","union_shop_item"},
	--公会建设
	UNION_CONSTRUCTION = {"TemplUnionConstruction ","union_construction"},
	--建设成功
	UNION_CONSTRUCTION_SUCCEED = {"TemplUnionConstructionSucceed ","union_construction_succeed"},

	--公会副本条目
	TEMPL_UNION_TRANSCRIPT_ITEM = { "TemplUnionTranscriptItem" , "union_transcript_item" },
	
	--公会进战斗
	TEMPL_UNION_COMPILE = { "TemplUnionCombat" , "union_combat" },

	--公会通关奖励
	TEMPL_UNION_PLAYER_AWARD = { "TemplUnionPlayerAward" , "union_playeraward" },

	--公会通关奖励条目
	TEMPL_UNION_PLAYER_AWARD_ITEM = { "TemplUnionPlayerAwardItem" , "union_playeraward_item" },
		
	--公会副本宝箱
	TEMPL_UNION_TRANSCRIPT_BOX = { "TemplUnionTranscriptBox" , "union_transcript_box" },
		
	--公会副本成员战绩
	TEMPL_UNION_TRANSCRIPT_RECORD = { "TemplUnionTranscriptRecord" , "union_transcript_record" },

	--公会副本成员战绩条目
	TEMPL_UNION_TRANSCRIPT_RECORD_ITEM = { "TemplUnionTranscriptRecordItem" , "union_transcript_record_item" },

	--公会副本重置
	TEMPL_UNION_TRANSCRIPT_RESET = { "TemplUnionTranscriptReset" , "union_transcript_reset" },

	--公会副本斩杀
	TEMPL_UNION_TRANSCRIPT_PLAYER = { "TemplUnionTranscriptPlayer" , "templ_union_transcript_player" },

	--日常任务与成就
	DALIYTASK_AND_ACHIEVEMENT_CELL = {"TemplDaliyTaskAndAchievementCell","templ_daliy_task_and_achievementcell"},

	--世界boss
	--世界boss 势力弹窗
	TEMPL_WORLDBOSS_GROUP = {"TemplWorldbossGroup","templ_worldboss_group"},
	TEMPL_WORLDBOSS_AWARD = {"TemplWorldbossAward","templ_worldboss_award"},
	TEMPL_WORLDBOSS_AWARD_ITEM = {"TemplWorldbossAwardItem","templ_worldboss_award_item"},

	--世界boss 排行榜
	TEMPL_WORLDBOSS_RANK = {"TemplWorldbossRank","templ_worldboss_rank"},
	TEMPL_WORLDBOSS_RANK_ITEM = {"TemplWorldbossRankItem","templ_worldboss_rank_item"},

	--世界boss 战报
	TEMPL_WORLDBOSS_SCORE = {"TemplWorldbossScore","templ_worldboss_score"},
	--世界boss次数购买
	TEMPL_WORLDBOSS_BUYTIME = {"TemplWorldbossBuytime","templ_worldboss_buytime"},

	--领地技能
	TEMPL_MANOR_SKILL = {"TempManorSkill","temp_manor_skill"},
	TEMPL_MANOR_SKILL_ITEM = {"TempManorSkillItme","temp_manor_skill_itme"},

	--领地好友
	TEMPL_MANOR_FRIEND = {"TempManorFriend","temp_manor_friend"},
	TEMPL_MANOR_FRIEND_ITEM = {"TempManorFriendItem","temp_manor_friend_item"},

	--七天活动
	TEMPL_ACTIVITY_SEVEN_DAYS = {"TemplActivitySevenDays","templ_activity_sevenDays"},
	TEMPL_ACTIVITY_SEVEN_DAYS_PREVIEW = {"TemplActivitySevenDaysPreview","templ_activity_sevenDaysPreview"},
	TEMPL_ACTIVITY_SEVEN_DAYS_PREVIEW_ITEM = {"TemplActivitySevenDaysPreviewItem","templ_activity_sevenDaysPreview_item"},
	
	--等级礼包
	TEMPL_ACTIVITY_LEVEL_ITEM = {"TemplActivityLevelItem","templ_activity_level_item"},
	--等级礼包
	TEMPL_ACTIVITY_LEVEL_GIFTBAG_ITEM = {"TemplActivityLevelgiftBagItem","templ_activity_levelgiftBag_item"},
	--摇钱树
	TEMPL_ACTIVITY_TIPS = {"TemplActivityTips","templ_activity_tips"},

	--vip
	TEMPL_VIP_ITEM = {"TemplVipItem","templ_vip_item"},

	--福利条目
	TEMPL_ACTIVITY_WELFARE_ITEM = {"TemplActivityWelfareItem","templ_activity_welfare_item"},

	--推荐阵容
	TEMPL_RECOMMENDLINEUP_PAGE = {"TemplRecommendLineupPage","templ_recommend_lineup_view"},
	--功能预告
	TEMPL_SYSTEM_FORESHOW_CELL = {"TemplSystemForeShowScrollItem","templ_system_foreshow_cell"},

	--基金条目
	TEMPL_ACTIVIITY_FUND_ITEM = {"TemplActivityFundItem","templ_activity_fund_item"},

	--累充条目
	TEMPL_ACTIVIITY_ONE_RECHARGE_ITEM = {"TemplActivityOneRechargeItem","templ_activity_oneRecharge_item"},

	-------------------------------
	--弹窗
	-------------------------------
	--规则弹窗
	TEMPL_RULE = {"LogicGUI.TemplRule","templ_rule","lgui/templ_rule"},


	
	--确认弹窗
	TEMPL_AFFIRM_TIPS = {"TemplAffirmTips","templ_affirm_tips"},
	
	--获得弹窗
	TEMPL_ACQUIRE_TIPS = {"LogicGUI.TemplAcquireTips","templ_acquire_tips" , "lgui/tips/templ_acquire_tips" },
	TEMPL_ACQUIRE_TIPS_1 = {"LogicGUI.TemplAcquireTips_1","templ_acquire_tips" , "lgui/tips/templ_acquire_tips1" },
	TEMPL_LITTLE_ACQUIRE_TIPS = {"LogicGUI.TemplLittleAcquireTips","templ_acquire_tips" , "lgui/tips/templ_acquire_tips" },

	--合成弹窗
	TEMPL_COMPOUND_TIPS = {"LogicGUI.TemplCompoundTips","templ_acquire_tips" , "lgui/tips/templ_compound_tips" },
	--进化弹窗
	TEMPL_REMOULD_SHOW = {"LogicGUI.TemplRemouldShow","templ_acquire_tips" , "lgui/templ_remould_show" },

	--礼包获得弹窗
	TEMPL_GIFTBAG_GAIN = {"LogicGUI.TemplGiftbagGain","templ_giftbag_gain" , "lgui/pack/templ_giftbag_gain"},
	--领奖中心
	TEMPL_ACCEPT_PRIZE_CENTER_CELL = {"TemplAcceptPrizeCenterCell","award_center_item"},
	--提示
	TEMPL_TIPS_VIEW = {"TemplTipsView","tips"},
	
	EQUIP_CROSS = {"TemplEquipCross","templ_equip_cross"},

	--爬塔属性
	TEMP_TOWER_PROPERTY = {"TempTowerProperty","temp_tower_property"},

	--增加的属性提示
	TEMPL_BOUNCE_ADD_PROPERTY = {"TemplBounceAddProperty","templ_bounce_add_property"},	

	--充值条目
	TEMPL_RECHARGE_ITEM = {"TemplChargeItem","templ_recharge_item"},	

	--pvp排行榜
	TEMPL_PVP_RANK = {"TemplPvpRank","templ_pvp_rank"},	
	TEMPL_PVP_RANK_ITEM = {"TemplPvpRankItem","templ_pvp_rank_item"},	

	--pvp奖励
	TEMPL_PVP_AWARD = {"TemplPvpAward","templ_pvp_award"},	
	TEMPL_PVP_AWARD_ITEM = {"TemplPvpAwardItem","templ_pvp_award_item"},	

	--pvp挑战界面
	TEMPL_PVP_CHALLENGE = {"TemplPvpChallenge","templ_pvp_challenge"},	

	--精灵设置
	TEMPL_SPRITE_SET = {"LogicGUI.TemplSpriteSet","templ_sprite_set","lgui/spirit/templ_spirit_setskill"},	

	--精灵设置
	TEMPL_SPRITE_SET_INFO = {"TemplSpriteSetInfo","templ_sprite_set_info"},	
	--精灵祝福
	TEMPL_SPRITE_BLESS = {"LogicGUI.TemplSpriteBlessView","templ_spirit_bless_view", "lgui/spirit/templ_spirit_bless_view"},
	--精灵祝福条目
	TEMPL_SPRITE_BLESS_CELL = {"TemplSpriteBlessCell","templ_heroenter_spiritbless_cell"},
	--精灵详情
	TEMPL_SPRITE_DETAIL = {"LogicGUI.TemplSpriteDetailView","templ_spirit_detail_view", "lgui/spirit/templ_spirit_detail_view"},
	--精灵祝福精灵信息
	TEMPL_SPRITE_BLESS_SKILL = {"Modules.TemplSpriteBlessSkill","templ_spirit_bless_skill_view", "lgui/spirit/templ_spirit_bless_skill_view"},

	--我要变强
	TEMPL_STRONGER_SYSTEM_CELL = {"TemplStrongerSystemCell","templ_stronger_cell"},	

	--怪物攻城
	TEMPL_MONSTER_STROMCASTLE_ENCOURAGE = {"TemplMonsterStormCastleEncourage","templ_monster_stormcastle_encourage"},	
	TEMPL_MONSTER_STROMCASTLE_REWARD_VIEW = {"TemplMonsterStormCastleRewardView","templ_monster_stormcastle_reward_view"},	
	TEMPL_MONSTER_STROMCASTLE_REWARD_CELL = {"TemplMonsterStormCastleRewardCell","templ_monster_stormcastle_reward_cell"},
	TEMPL_MONSTER_STROMCASTLE_RANK_CELL = {"TemplMonsterStormCastleIntergralRankCell","templ_monster_stormcastle_rank_cell"},

	--无尽深渊排行榜	
	TEMPL_CHASM_RANK = {"TemplChasmRank","templ_chasm_rank"},	
	TEMPL_CHASM_RANK_ITEM = {"TemplChasmRankItem","templ_chasm_rank_item"},	

	--直购特惠商品
	TEMPL_DIRECT_BUY_ITEM = {"TemplDirectBuyItem", "templ_direct_buy_item"},

	--宝石信息
	TEMPL_GEM_INFO = {"LogicGUI.TemplGemInfo", "templ_gem_info" , "lgui/gem/templ_gem_info"},
	--宝石合成
	TEMPL_GEM_COMPOUND = {"TemplGemCompound", "templ_gem_compound"},
	--宝石分解
	TEMPL_GEM_RESOLVE = {"TemplGemResolve", "templ_gem_resolve"},
	--宝石属性
	TEMPL_GEM_IPROPERTY = {"TemplGemIproperty", "templ_gem_iproperty"},

	--天梯任务
	TEMP_LADDER_TASK = {"TempLadderTask", "temp_ladder_task"},
	--天梯晋级奖励
	TEMP_LADDER_PROMOTION = {"TempLadderPromotion", "temp_ladder_promotion"},

	--天梯匹配
	TEMP_LADDER_VS = {"Laddervs", "ladder_vs"},

	--创建队伍
	TEMPL_CREATE_THE_TEAM = {"TemplCreateTheTeam", "templ_create_the_team"},
	--邀请玩家界面
	TEMPL_INVITE_TEAM_TEAMMATE_VIEW = {"TemplInviteTeamMateView", "templ_invite_teammate_view"},
	--邀请玩家条目
	TEMPL_INVITE_TEAM_TEAMMATE_CELL = {"TemplInviteTeamMateCell", "templ_invite_teammate_cell"},
	--荣耀排行榜
	TEMPL_TEAM_GLORY_RANK_VIEW = {"TemplTeamGloryRankView", "templ_team_gloryrank_view"},
	--副本结算
	TEMPL_TEAM_TRANSCRIPT_COUNT = {"LogicGUI.TemplTeamTranscriptCount", "templ_team_transcript_count" , "lgui/team/templ_team_transcript_count" },
	--组队扭蛋机奖励预览
	TEMPL_TEAM_LOTTERY_REWARD_PRELOOK = {"TemplTeamLotteryRewardPreLook", "templ_team_lottery_reawrd_prelook"},
	--组队扭蛋机奖励预览条目
	TEMPL_TEAM_LOTTERY_REWARD_CELL = {"TemplTeamLotteryRewardCell", "templ_team_lottery_reward_cell"},

	--一键升级
	TEMPL_EQUIP_ONEKEY_REFINE = {"TemplEquipOnekeyRefine", "templ_equip_onekey_refine"},

	--占星预览
	TEMPL_DIVINATION_PREVIEW = {"TemplDivinationPreview", "templ_divination_preview"},

	--占星界面
	TEMPL_DIVINATION_SCRYER = {"TemplDivinationScryer", "templ_divination_scryer"},

	--收藏项
	TEMPL_DIVINATION_BOOK_INFO = {"TemplDivinationBookInfo", "templ_divination_book_info"},

	--占星挑战界面
	TEMPL_DIVINATION_CHALLENGE = {"TemplDivinationChallenge", "templ_divination_challenge"},
	--占星NPC组件
	TEMPL_DIVINATION_NPC = {"TemplDivinationNpc", "templ_divination_npc"},

	--占星兑换界面
	TEMPL_DIVINATION_CONVERT = {"TemplDivinationConvert", "templ_divination_convert"},

	--卡牌获得
	TEMPL_DIVINATION_GAIN = {"LogicGUI.TemplDivinationGain", "templ_divination_gain" , "lgui/divination/templ_divination_gain" },
	--卡牌收藏
	TEMPL_DIVINATION_COLLECT = {"LogicGUI.TemplDivinationCollect", "templ_divination_collect" , "lgui/divination/templ_divination_collect" },

	--卡牌获得
	TEMPL_DIVINATION_BREAKVIEW = {"TemplDivinationBreakview", "templ_divination_breakview"},

	--好友召唤
	TEMPL_CALL_FRIENDS = {"TemplCallFriends", "templ_call_friends"},

	--客服回复
	TEMPL_EMAIL_REPLY = {"TemplEmailReply", "templ_email_reply"},

	--转盘积分奖励
	TEMPL_TURNTABLE_AWARD = {"TempTurntableAward", "temp_turntable_award"},

	--转盘积分排行
	TEMPL_TURNTABLE_RANK = {"TemplTurntableRank", "templ_turntable_rank"},

	--阵营招募记录
	TEMPL_CAMPRECRUIT_RECORD_CELL = {"TemplCampRecruitRecordCell", "templ_camp_recruit_record_cell"},
	TEMPL_CAMPRECRUIT_PRELOOK = {"TemplCampRecruitPreLook", "templ_camp_recruit_prelook"},

	--积分赛
	TEMPL_INTEGRALRACE_RANK_VIEW = {"TemplIntegralRaceRankView", "templ_integralrace_rank_view"},
	TEMPL_INTEGRALRACE_RANK_CELL = {"TemplIntegralRaceRankCell", "templ_integralrace_rank_cell"},
	TEMPL_INTEGRALRACE_RANK_SP_CELL = {"TemplIntegralRaceRankSpCell", "templ_integralrace_rank_sp_cell"},

	--援军助威
	TEMPL_REINFORCEMENTS_CHEER_HERO_CELL = {"TemplReinforcementsCheerHeroCell", "templ_reinforcements_cheer_hero_cell"},

	--[[限时活动]]--
	--圣诞活动
	--圣诞BOSS奖励
	TEMPL_CHRISTMAS_BOSSREWARD_VIEW = {"TemplChristmasBossRewardView", "templ_christmas_boss_reward_view"},	
	--圣诞商店条目
	TEMPL_CHRISTMAS_SHOP_CELL = {"TemplChristmasShopItem", "templ_christmas_shop_cell"},
	--春节活动	
	--圣诞商店条目
	TEMPL_SPRINGFESTIVAL_SHOP_CELL = {"TemplSpringFestivalShopItem", "templ_springfestival_shop_cell"},
}

function GUI.TemplateMgr.CreateInstance()
	if not GUITemplateMgr then
		GUITemplateMgr = GUI.TemplateMgr.New()
	end
end

function GUI.TemplateMgr.DelInstance()
	if GUITemplateMgr then
		GUITemplateMgr:DeleteMe()
		GUITemplateMgr = nil
	end
end

function GUI.TemplateMgr:__init()
	--公共模板(进游戏加载)
	self.__common_template_list = {}

	--模块界面所需模板(按需加载)
	self.__module_template_list = {}
end

function GUI.TemplateMgr:__delete()
	if self.__scene_template_map then
		for template, _ in pairs(self.__scene_template_map) do
			template:DeleteMe(false)
		end
		self.__scene_template_map = nil
	end

	for template_name, template_info in pairs(self.__common_template_list) do
		if template_info.model_widget then
			CPPNodeUnity.Release(template_info.model_widget)
		end
	end
	self.__common_template_list = nil

	for template_name, template_info in pairs(self.__module_template_list) do
		if template_info.model_widget then
			CPPNodeUnity.Release(template_info.model_widget)
		end
	end
	self.__module_template_list = nil
end

function GUI.TemplateMgr:PrintCache()
	print(8, "---------------GUI.TemplateMgr:PrintCache---------------")
	
	if self.__scene_template_map then
		for template, _ in pairs(self.__scene_template_map) do
			print(8, template:GetName())
		end
	end

	for template_name, template_info in pairs(self.__module_template_list) do
		print(8, template_name, template_info.layout_path, template_info.count)
	end
end

--创建模板实例
function GUI.TemplateMgr:CreateTemplateInstance(registry_info, is_load_script_sync, ...)
	if not registry_info then
		LogTool.LogError("[GUITemplateMgr:CreateTemplateInstance] invalid registry_info")		
		return
	end

	local template
	local varying_params = {...}
	local function script_loaded_callback()
		--获取模板类
		local template_class_name = registry_info[1]
		local template_class_name_list = CPPGameLib.Split(template_class_name, '.')
		local template_class
		for i = 1, #template_class_name_list do
			template_class = template_class or _G
			template_class = template_class[template_class_name_list[i]]
		end
		if not template_class then
			LogTool.LogError("[GUITemplateMgr:CreateTemplateInstance] couldn't found template class(%s)", template_class_name)
			return
		end

		--创建模板类对象
		template = template_class.New(unpack(varying_params))
	end

	local lua_script_path = registry_info[3]
	if lua_script_path then
		if is_load_script_sync then
			GlobalRequireToolBatch:LoadFilesSync({file_list = {lua_script_path}, callback = script_loaded_callback})						
		else
			GlobalRequireToolBatch:LoadFiles({file_list = {lua_script_path}, callback = script_loaded_callback})						
		end
	else
		script_loaded_callback()
	end

	return template
end

--创建模板实例

--新增模板
function GUI.TemplateMgr:AddLayout(template_list, template_name, layout_path, lua_script_path, add_count)
	if template_list[template_name] ~= nil then
		if add_count == true then
			template_list[template_name].count = template_list[template_name].count + 1
		end
		return
	end

	local template_info = {}
	template_info.count = 1
	template_info.model_widget = nil
	template_info.template_name = template_name
	template_info.layout_path = layout_path		
	template_info.lua_script_path = lua_script_path
	template_info.callback_list = {}
	template_list[template_name] = template_info
end

--删除模板
function GUI.TemplateMgr:RemoveLayout(template_list, template_name)
    if template_list[template_name] then
    	template_list[template_name].count = template_list[template_name].count - 1
    	if template_list[template_name].count == 0 then
        	template_list[template_name] = nil
        end
    end
end

--加载模板
function GUI.TemplateMgr:LoadLayout(template_list, template_name, callback)
	local template_info = template_list[template_name]
	if not template_info then
		if callback then
			callback(false, "", "")
		end
		return
	end

	if template_info.load_ret ~= nil then
		if callback then
			callback(template_info.load_ret, template_info.template_name, template_info.layout_path)
		end
		return
	end

	CPPViewHelper.CreateWidgetFromFlatBuffersFileAsync(template_info.layout_path, GlobalCallbackMgr:AddCallBackFunc(function()
		template_info.model_widget = CPPViewHelper.GetDFFWidget()
		if template_info.model_widget then
			CPPNodeUnity.Retain(template_info.model_widget)
			template_info.model_widget:RemoveSelf(false)
		end

		local function real_callback()
			if callback then
				callback(template_info.load_ret, template_info.template_name, template_info.layout_path)
			end
		end

		template_info.load_ret = template_info.model_widget ~= nil
		if template_info.load_ret then
			--加载模板类所在的脚本文件
			if template_info.lua_script_path then
				GlobalRequireToolBatch:LoadFiles({file_list = {template_info.lua_script_path}, callback = real_callback})				
				return
			end			
		else
			LogTool.LogError("[GUI.TemplateMgr:LoadLayout] 模板加载失败. template_name:%s layout_path:%s", template_info.template_name, template_info.layout_path)
		end

		real_callback()

	end))
	-- LogTool.LogInfo("[GUI.TemplateMgr:LoadLayout] load %s", template_info.template_name)	
end

--卸载模板
function GUI.TemplateMgr:UnLoadLayout(template_list, template_name)
    local template_info = template_list[template_name]
    if template_info and template_info.model_widget and template_info.count == 1 then
		CPPNodeUnity.Release(template_info.model_widget)
        template_info.model_widget = nil
		-- LogTool.LogInfo("[GUI.TemplateMgr:UnLoadLayout] unload %s", template_name)	
    end
end

--克隆模板
function GUI.TemplateMgr:Clone(template_name)
	local template_info = self.__common_template_list[template_name] or self.__module_template_list[template_name]	
	if not template_info or not template_info.load_ret then
		LogTool.LogError("[GUI.TemplateMgr:Clone] 模板还未加载. template_name:%s", template_name)		
		return nil
	end

	return template_info.model_widget:Clone()
end

--卸载并移除模板
function GUI.TemplateMgr:UnLoadAndRemoveLayout(template_list, template_name)
    self:UnLoadLayout(template_list, template_name)
    self:RemoveLayout(template_list, template_name)
end

--新增公共模板
function GUI.TemplateMgr:AddCommonLayout(template_name, layout_path)
	self:AddLayout(self.__common_template_list, template_name, layout_path)
end

--加载所有公共模板
function GUI.TemplateMgr:LoadAllCommonLayout(callback)
	local template_num = 0	
	for template_name, _ in pairs(self.__common_template_list) do
		template_num = template_num + 1
		self:LoadLayout(self.__common_template_list, template_name, callback)
	end
	if template_num <= 0 then
		if callback then
			callback(true)
		end
	end
end

--卸载并移除公共模板
function GUI.TemplateMgr:UnLoadAndRemoveCommonLayout(template_name)
	self:UnLoadAndRemoveLayout(self.__common_template_list, template_name)
end

--新增并加载系统模板
function GUI.TemplateMgr:AddAndLoadModuleLayout(template_name, layout_path, lua_script_path, callback)
	self:AddLayout(self.__module_template_list, template_name, layout_path, lua_script_path, true)
	self:LoadLayout(self.__module_template_list, template_name, callback)
end

--卸载并移除系统模板
function GUI.TemplateMgr:UnLoadAndRemoveModuleLayout(template_name)
	self:UnLoadAndRemoveLayout(self.__module_template_list, template_name)
end

--添加模板实例到场景
function GUI.TemplateMgr:AddToScene(template)
	self.__scene_template_map = self.__scene_template_map or {}
	self.__scene_template_map[template] = true

	template:SetAnchorPoint(0.5, 0.5)

	--根节点多分辨率适配(代码布局的模板，需自行适配)
	if template:GetLayoutMode() ~= GUI.Template.LayoutMode.CODE then
		GUI.MultiResolutionAdapter.Adapt(template:GetNode())
	end

	GlobalScene:AddToRenderGroup(template:GetNode(), template:GetRenderQueue())	
	PosTool.Center(template)

	template:MoveToFront()
end

--从场景移除模板实例
function GUI.TemplateMgr:DelFromScene(template)
	if not self.__scene_template_map then
		return
	end

	if self.__scene_template_map[template] then
		self.__scene_template_map[template] = nil
	end
end

--根据模板类型获取场景中的模板实例
function GUI.TemplateMgr:GetTemplateByType(registry_info)
	local registry_info = GUI.TemplateRegistry[registry_info]
	if self.__scene_template_map then
		for template, _ in pairs(self.__scene_template_map) do
			if template:GetName() == registry_info[1] then
				return template
			end
		end
	end	
end

--处理模板打开时的缩放效果
function GUI.TemplateMgr:HandleTemplateOpenEffect(template, finished_callback)
	if not template then
		return
	end

	self:__LazyInitOpenEffectConfig()

	local function do_finished_callback()
		if finished_callback then
			finished_callback()
		end
	end

	local registry_info = template:GetRegistryInfo()
	if not self.__open_effect_config or not self.__open_effect_config[registry_info] then	
		--不需打开效果的模板，过滤
		do_finished_callback()
		return
	end

	if self.__in_template_open_effecting then
		--打开效果中，过滤
		do_finished_callback()
		return
	end

	ActionManager.GetInstance():ViewOrTemplateOpenEffect(template, function()
		self.__template_in_open_effecting = nil
		self.__in_template_open_effecting = false
		self:FireNextFrame(Macros.Event.View.TEMPLATE_EFFECT_FINISHED)

		--完成回调
		do_finished_callback()
	end)

	self.__template_in_open_effecting = template
	self.__in_template_open_effecting = true	
end

--处理模板打开时的缩放效果(结束)
function GUI.TemplateMgr:HandleTemplateOpenEffectEnd(template)
	if self.__template_in_open_effecting == template then
		self.__template_in_open_effecting = nil
		self.__in_template_open_effecting = false
	end
end

--是否模板打开效果中
function GUI.TemplateMgr:IsInTemplateOpenEffect()
	return self.__in_template_open_effecting
end

--处理模板打开音效
function GUI.TemplateMgr:HandleViewOpenSound(registry_info)
	if not registry_info then
		return
	end

	self:__LazyInitTemplateSoundConfig()

	local info = self.__template_sound_config and self.__template_sound_config[registry_info] or nil
	if not info then
		return
	end

	if info.open_sound then
		GlobalSoundMgr:PlayEffectByConfigID(info.open_sound)
	end
end

--处理模板关闭音效
function GUI.TemplateMgr:HandleViewCloseSound(registry_info)
	if not registry_info then
		return
	end

	self:__LazyInitTemplateSoundConfig()

	local info = self.__template_sound_config and self.__template_sound_config[registry_info] or nil
	if not info then
		return
	end

	if info.close_sound then
		GlobalSoundMgr:PlayEffectByConfigID(info.close_sound)
	end
end

--销毁模板
function GUI.TemplateMgr:Dump()
	if self.__scene_template_map then
		local keep_list
		for template, _ in pairs(self.__scene_template_map) do
			if template.is_auto_close then
				template:DeleteMe(false)
			else
				keep_list = keep_list or {}
				table.insert(keep_list, template)
			end
		end

		if keep_list then
			self.__scene_template_map = {}
			for _, template in ipairs(keep_list) do
				self.__scene_template_map[template] = true
			end
		else
			self.__scene_template_map = nil
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--初始化界面打开效果表
function GUI.TemplateMgr:__LazyInitOpenEffectConfig()
	local config_view_open_effects = CPPGameLib.GetConfig("ViewOpenEffect", nil, nil, true)	
	if not config_view_open_effects then
		return
	end
	
	if not self.__open_effect_config then
		self.__open_effect_config = {}
		local registry_info
		for _, info in pairs(config_view_open_effects) do
			if not info.type or info.type == 0 then
				registry_info = GUI.TemplateRegistry[info.type_name] or 0
				self.__open_effect_config[registry_info] = true
			end
		end
	end
end

--初始化界面音效表
function GUI.TemplateMgr:__LazyInitTemplateSoundConfig()
	local config_view_sounds = CPPGameLib.GetConfig("ViewSound", nil, nil, true)
	if not config_view_sounds then
		return
	end

	if not self.__template_sound_config then
		self.__template_sound_config = {}
		local registry_info
		for _, info in pairs(config_view_sounds) do
			if info.type == 1 then
				registry_info = GUI.TemplateRegistry[info.sub_type]
				if registry_info then
					self.__template_sound_config[registry_info] = info
				else
					LogTool.LogError("初始化界面音效表 无效的模板类型%s", info.sub_type)
				end
			end
		end
	end
end
