ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Map = ConfigAdapter.Map or {}

ConfigAdapter.Map.MapPath = 
{   

	["Common"] = Resource.PathTool.GetCnfPath("config_common"),
	["ErrorCode"] = Resource.PathTool.GetCnfPath("config_error_code"),
	["SystemEntry"] = Resource.PathTool.GetCnfPath("config_system_entry"),
	["RoleLevel"] = Resource.PathTool.GetCnfPath("config_role_level"),
	["Hero"] = Resource.PathTool.GetCnfPath("config_hero"),
	["HeroLevel"] = Resource.PathTool.GetCnfPath("config_hero_level"),
	["HeroProperty"] = Resource.PathTool.GetCnfPath("config_hero_property"),
	["HeroLineup"] = Resource.PathTool.GetCnfPath("config_hero_lineup"),
	["HeroBreak"] = Resource.PathTool.GetCnfPath("config_hero_break"),
	["HeroBreakLevelup"] = Resource.PathTool.GetCnfPath("config_hero_break_levelup"),
	["HeroCompound"] = Resource.PathTool.GetCnfPath("config_hero_compound"),
	["HeroTrain"] = Resource.PathTool.GetCnfPath("config_hero_train"),
	["HeroDestiny"] = Resource.PathTool.GetCnfPath("config_hero_destiny"),
	["HeroTrainCost"] = Resource.PathTool.GetCnfPath("config_hero_train_cost"),
	["HeroEvolution"] = Resource.PathTool.GetCnfPath("config_hero_evolution"),
	["HeroQualifications"] = Resource.PathTool.GetCnfPath("config_hero_qualifications"),
	["Monster"] = Resource.PathTool.GetCnfPath("config_monster"),
	["MonsterAI"] = Resource.PathTool.GetCnfPath("config_monster_ai"),
	["Skill"] = Resource.PathTool.GetCnfPath("config_skill"),
	["SkillLevel"] = Resource.PathTool.GetCnfPath("config_skill_level"),
	["SkillBullet"] = Resource.PathTool.GetCnfPath("config_skill_bullet"),
	["SkillBuff"] = Resource.PathTool.GetCnfPath("config_skill_buff"),
	["Effect"] = Resource.PathTool.GetCnfPath("config_effect"),
	["MainFuben"] = Resource.PathTool.GetCnfPath("config_main_fuben"),
	["DungeonStarReward"] = Resource.PathTool.GetCnfPath("config_dungeon_star_reward"),
	["HeroFBChapter"] = Resource.PathTool.GetCnfPath("config_hero_fb_chapter"),
	["DailyChapter"] = Resource.PathTool.GetCnfPath("config_daily_chapter"),	
	["GameLevel"] = Resource.PathTool.GetCnfPath("config_game_level"),
	["Battle"] = Resource.PathTool.GetCnfPath("config_battle"),
	["BattleSpriteToggle"] = Resource.PathTool.GetCnfPath("config_battle_sprite_toggle"),
	["PowerPromoteEntry"] = Resource.PathTool.GetCnfPath("config_power_promote_entry"),
	["CityIcon"] = Resource.PathTool.GetCnfPath("config_cityicon"),
	["HeadAnim"] = Resource.PathTool.GetCnfPath("config_head_anim"),
	["FloatAnim"] = Resource.PathTool.GetCnfPath("config_float_anim"),
	["Item"] = Resource.PathTool.GetCnfPath("config_item"),
	["ItemShop"] = Resource.PathTool.GetCnfPath("config_item_shop"),
	["GiftShop"] = Resource.PathTool.GetCnfPath("config_gift_shop"),
	["HeroHighRecruit"] = Resource.PathTool.GetCnfPath("config_hero_highrecruit"),
	["HeroLowRecruit"] = Resource.PathTool.GetCnfPath("config_hero_lowrecruit"),
	["HeroHandBook"] = Resource.PathTool.GetCnfPath("config_hero_handbook"),
	["HeartOfWorld"] = Resource.PathTool.GetCnfPath("config_heart_of_world"),
	["DuobaoMap"] = Resource.PathTool.GetCnfPath("config_duobao_map"),
	["DuobaoDrop"] = Resource.PathTool.GetCnfPath("config_duobao_drop"),
	["DoubaoPregrossReward"] = Resource.PathTool.GetCnfPath("config_buobao_pregross_reward"),
	["DailyMission"] = Resource.PathTool.GetCnfPath("config_daily_mission"),
	["TeamLevelDailyMission"] = Resource.PathTool.GetCnfPath("config_team_level_daily_mission"),
	["Achievement"] = Resource.PathTool.GetCnfPath("config_achievement"),
	["ApproachOfAchieving"] = Resource.PathTool.GetCnfPath("config_approach_of_achieving"),
	["HeroAwakening"] = Resource.PathTool.GetCnfPath("config_hero_awakening"),
	["AwakeningItem"] = Resource.PathTool.GetCnfPath("config_awakening_item"),
	["AwakenShop"] = Resource.PathTool.GetCnfPath("config_awaken_shop"),
	["HuoBi"] = Resource.PathTool.GetCnfPath("config_huobi"),
	["UpPrevue"] = Resource.PathTool.GetCnfPath("config_up_prevue"),
	["RewardCenter"] = Resource.PathTool.GetCnfPath("config_reward_center"),
	["PropertyRecover"] = Resource.PathTool.GetCnfPath("config_property_recover"),
	["Lead"] = Resource.PathTool.GetCnfPath("config_lead"),
	["LeadOperation"] = Resource.PathTool.GetCnfPath("config_lead_operation"),
	["NecessaryLead"] = Resource.PathTool.GetCnfPath("config_necessary_lead"),
	["LeadFB"] = Resource.PathTool.GetCnfPath("config_lead_fb"),
	["LeadAssociatedFB"] = Resource.PathTool.GetCnfPath("config_lead_associated_fb"),
	["LeadBattleResultGL"] = Resource.PathTool.GetCnfPath("config_lead_battleresult_gl"),
	["Story"] = Resource.PathTool.GetCnfPath("config_story"),
	["WorldStarStory"] = Resource.PathTool.GetCnfPath("config_world_star_story"),
	["NameList"] = Resource.PathTool.GetCnfPath("config_name_list"),
	["SystemNotice"] = Resource.PathTool.GetCnfPath("config_system_notice"),
	["SignIn"] = Resource.PathTool.GetCnfPath("config_sign_in"),
	["SevenDayActivity"] = Resource.PathTool.GetCnfPath("config_seven_day_activity"),
	["SevenDayTargetData"] = Resource.PathTool.GetCnfPath("config_seven_day_target_data"),
	["SevenDayShop"] = Resource.PathTool.GetCnfPath("config_seven_day_shop"),
	["SevenDayTargetCompletionReward"] = Resource.PathTool.GetCnfPath("seven_day_target_completion_reward"),
	["Activity"] = Resource.PathTool.GetCnfPath("config_activity"),
	["LevelGift"] = Resource.PathTool.GetCnfPath("config_level_gift"),
	["Sound"] = Resource.PathTool.GetCnfPath("config_sound"),
	["ViewSound"] = Resource.PathTool.GetCnfPath("config_view_sound"),
	["SystemOperSound"] = Resource.PathTool.GetCnfPath("config_system_oper_sound"),
	["ViewOpenEffect"] = Resource.PathTool.GetCnfPath("config_view_open_effect"),
	["HeroRecommendLineup"] = Resource.PathTool.GetCnfPath("config_hero_recommend_lineup"),
	["HeroSystemForeshow"] = Resource.PathTool.GetCnfPath("config_hero_system_foreshow"),
	["LevelShop"] = Resource.PathTool.GetCnfPath("config_level_shop"),
	["LevelShopGoods"] = Resource.PathTool.GetCnfPath("config_level_shop_goods"),
	["MainFBChapterReward"] = Resource.PathTool.GetCnfPath("config_main_fb_chapter_reward"),
	["DailyFBReward"] = Resource.PathTool.GetCnfPath("config_daily_fb_reward"),
	["EliteFBChapter"] = Resource.PathTool.GetCnfPath("config_elite_fb_chapter"),
	["HeroVipRecruit"] = Resource.PathTool.GetCnfPath("config_hero_viprecruit"),


	["CombatSuppression"] = Resource.PathTool.GetCnfPath("config_combat_suppression"),
	["ElitenFBGetStar"] = Resource.PathTool.GetCnfPath("config_elite_fb_getstar"),
	["Camp"] = Resource.PathTool.GetCnfPath("config_camp"),
	["ActivityNotice"] = Resource.PathTool.GetCnfPath("config_activity_notice"),
	["Cellphone"] = Resource.PathTool.GetCnfPath("config_cellphone"),
	["GameTipsList"] = Resource.PathTool.GetCnfPath("config_game_tips_list"),
	["RoleLevelTips"] = Resource.PathTool.GetCnfPath("config_role_level_tips"),	
	["GameLevelTips"] = Resource.PathTool.GetCnfPath("config_gamelevel_tips"),

	["ActionCount"] = Resource.PathTool.GetCnfPath("config_action_count"),
	["SysPush"] = Resource.PathTool.GetCnfPath("config_sys_push"),
	["CustomEventTrack"] = Resource.PathTool.GetCnfPath("config_event_track"),
	["AdjustEventTrack"] = Resource.PathTool.GetCnfPath("config_adjust_event_track"),

	--竞技场
	["Arena"] = Resource.PathTool.GetCnfPath("config_arena_rank")	,
	["ArenaFirstRankAward"] = Resource.PathTool.GetCnfPath("config_arena_first_rank") ,
	["ArenaRobot"] = Resource.PathTool.GetCnfPath("config_arena_robot") ,
	["ArenaShop"] = Resource.PathTool.GetCnfPath("config_arena_shop") ,
	["ArenaAward"] = Resource.PathTool.GetCnfPath("config_arena_award") ,
	["ArenaRankTips"] = Resource.PathTool.GetCnfPath("config_arena_rank_tips") ,
	["ArenaFightFiveTimes"] = Resource.PathTool.GetCnfPath("config_arena_fight_five_times") ,
	--世界boss
	["WorldBossOpenTime"] = Resource.PathTool.GetCnfPath("config_world_boss_open_time") ,
	["WorldBoss"] = Resource.PathTool.GetCnfPath("config_world_boss") ,
	["WorldBossRankReward"] = Resource.PathTool.GetCnfPath("config_world_boss_rank_reward") ,
	["WorldBossNonorReward"] = Resource.PathTool.GetCnfPath("config_world_boss_nonor_reward") ,
	["WorldBossKillReward"] = Resource.PathTool.GetCnfPath("config_world_boss_kill_reward") ,
	["WorldBossRule"] = Resource.PathTool.GetCnfPath("config_world_boss_rule") ,
	--魂晶商店
	["HeroShop"] = Resource.PathTool.GetCnfPath("config_hero_shop") ,
	--铜雀台
	["TQT"] = Resource.PathTool.GetCnfPath("config_tqt") ,
	--日常挑战
	["Challenge"] = Resource.PathTool.GetCnfPath("config_challenge") ,
	--vip福利
	["VipDailyGift"] = Resource.PathTool.GetCnfPath("config_vip_daily_gift") ,
	["VipWeekGift"] = Resource.PathTool.GetCnfPath("config_vip_week_gift") ,
	--充值
	["Recharge"] = Resource.PathTool.GetCnfPath("config_recharge") ,
	["MonthCard"] = Resource.PathTool.GetCnfPath("config_month_card") ,
	["RechargeMyCard"] = Resource.PathTool.GetCnfPath("config_recharge_mycard") ,
	["YGamePayment"] = Resource.PathTool.GetCnfPath("config_ygame_payment") ,
	--军团
	["Guild"] = Resource.PathTool.GetCnfPath("config_guild") ,
	["GuildIcon"] = Resource.PathTool.GetCnfPath("config_guild_icon") ,
	["GuildPrivilege"] = Resource.PathTool.GetCnfPath("config_guild_privilege") ,
	["GuildQY"] = Resource.PathTool.GetCnfPath("config_guild_qy") ,
	["GuildQSReward"] = Resource.PathTool.GetCnfPath("config_guild_qs_reward") ,
	["GuildEvent"] = Resource.PathTool.GetCnfPath("config_guild_event") ,
	["GuildSkill"] = Resource.PathTool.GetCnfPath("config_guild_skill") ,
	["GuildFB"] = Resource.PathTool.GetCnfPath("config_guild_fb") ,
	["GuildBuilding"] = Resource.PathTool.GetCnfPath("config_guild_building") ,
	
	--军团副本
	["GuildItemShop"] = Resource.PathTool.GetCnfPath("config_guild_item_shop") ,	
	["GuildLimitShop"] = Resource.PathTool.GetCnfPath("config_guild_limit_shop") ,	
	["GuildAwardShop"] = Resource.PathTool.GetCnfPath("config_guild_award_shop") ,	
	--规则
	["Rule_Hero_Info"] = Resource.PathTool.GetCnfPath("config_rule_hero_info") ,	
	["Rule_Hero_Potency"] = Resource.PathTool.GetCnfPath("config_rule_hero_potency") ,
	["Rule_Equip_Master"] = Resource.PathTool.GetCnfPath("config_rule_equip_master") ,
	["Rule_Hero_Awaken"] = Resource.PathTool.GetCnfPath("config_rule_hero_awaken") ,
	["Rule_Reinforcement"] = Resource.PathTool.GetCnfPath("config_rule_reinforcement") ,

	["Rule_World_Heart"] = Resource.PathTool.GetCnfPath("config_rule_worldheart") ,
	["Rule_Sprite"] = Resource.PathTool.GetCnfPath("config_rule_sprite") ,
	["Rule_Union_Hall"] = Resource.PathTool.GetCnfPath("config_rule_union_hall") ,
	["Rule_Union_Build"] = Resource.PathTool.GetCnfPath("config_rule_union_build") ,
	["Rule_Union_Skill"] = Resource.PathTool.GetCnfPath("config_rule_union_skill") ,

	["Rule_Union_Ectype"] = Resource.PathTool.GetCnfPath("config_rule_union_ectype") ,
	["Rule_Recycle"] = Resource.PathTool.GetCnfPath("config_rule_recycle") ,
	["Rule_Activity_SignIn"] = Resource.PathTool.GetCnfPath("config_rule_activity_signin") ,
	["Rule_Activity_Muscle"] = Resource.PathTool.GetCnfPath("config_rule_activity_muscle") ,
	["Rule_Activity_Money"] = Resource.PathTool.GetCnfPath("config_rule_activity_money") ,

	["Rule_HeroShop"] = Resource.PathTool.GetCnfPath("config_rule_heroshop") ,
	["Rule_AwakenShop"] = Resource.PathTool.GetCnfPath("config_rule_awakenshop") ,
	["Rule_VIP"] = Resource.PathTool.GetCnfPath("config_rule_vip") ,
	["Rule_Athletics"] = Resource.PathTool.GetCnfPath("config_rule_athletics") ,
	["Rule_RobPyx"] = Resource.PathTool.GetCnfPath("config_rule_robpyx") ,

	["Rule_Trial"] = Resource.PathTool.GetCnfPath("config_rule_trial") ,
	["Rule_Everyday"] = Resource.PathTool.GetCnfPath("config_rule_everyday") ,
	["Rule_Relic"] = Resource.PathTool.GetCnfPath("config_rule_relic") ,
	["Rule_Invade"] = Resource.PathTool.GetCnfPath("config_rule_invade") ,
	["Rule_WorldBoss"] = Resource.PathTool.GetCnfPath("config_rule_worldboss") ,

	["Rule_PVP"] = Resource.PathTool.GetCnfPath("config_rule_pvp") ,
	["Rule_Chasm"] = Resource.PathTool.GetCnfPath("config_rule_chasm") ,
	["Rule_Monster"] = Resource.PathTool.GetCnfPath("config_rule_monster") ,
	["Rule_Ladder"] = Resource.PathTool.GetCnfPath("config_rule_ladder"),
	["Rule_Gem"] = Resource.PathTool.GetCnfPath("config_rule_Gem"),

	["Rule_Divination"] = Resource.PathTool.GetCnfPath("config_rule_divination"),
	["Rule_Divination_Book"] = Resource.PathTool.GetCnfPath("config_rule_divination_book"),
	["Rule_Divination_Draw"] = Resource.PathTool.GetCnfPath("config_rule_divination_draw"),
	["Rule_Christmas"] = Resource.PathTool.GetCnfPath("config_rule_christmas"),
	["Rule_GashaponMachine"] = Resource.PathTool.GetCnfPath("config_rule_gashaponmachine"),
	["Rule_IntegralRace"] = Resource.PathTool.GetCnfPath("config_rule_integralRace"),
	["Rule_UnionCombat"] = Resource.PathTool.GetCnfPath("config_union_combat"),
	
	--PVP
	["PVP_BattleTimesReward"] = Resource.PathTool.GetCnfPath("pvp_battle_times_reward") ,
	["PVP_RankReward"] = Resource.PathTool.GetCnfPath("pvp_rank_reward") ,

	--开服基金
	["OpenServerFund"] = Resource.PathTool.GetCnfPath("config_open_server_fund") ,
	["AllThePeopleWelfare"] = Resource.PathTool.GetCnfPath("config_all_the_people_welfare") ,

	--限时活动
	["LimitActivity"] = Resource.PathTool.GetCnfPath("config_limit_activity") ,

	--豪华签到
	["LuxurySignIn"] = Resource.PathTool.GetCnfPath("config_luxury_sign_in") ,
	["LuxurySignInRewardList"] = Resource.PathTool.GetCnfPath("config_luxury_sign_in_reward_list") ,

	--登录奖励
	["LoginReward"] = Resource.PathTool.GetCnfPath("config_login_reward") ,

	--活动公告
	["ActivityNotice"] = Resource.PathTool.GetCnfPath("config_activity_notice") , 
	["ActivityNoticeContent"] = Resource.PathTool.GetCnfPath("config_activity_notice_content") ,

	--称号
	["Title"] = Resource.PathTool.GetCnfPath("config_title") , 

	--深渊
	["EndlessLandCollision"] = Resource.PathTool.GetCnfPath("config_endless_land_collision") ,
	["EndlessLandWave"] = Resource.PathTool.GetCnfPath("config_land_endless_wave") ,
	["EndlessLandKeepKillReward"] = Resource.PathTool.GetCnfPath("config_endless_land_keep_kill_reward") ,
	["EndlessLandShop"] = Resource.PathTool.GetCnfPath("config_endless_land_shop") ,
	["EndlessLandRankReward"] = Resource.PathTool.GetCnfPath("config_endless_land_rank_reward") ,
	["EndlessLandSweep"] = Resource.PathTool.GetCnfPath("config_endless_land_sweep"),
	["EndlessLandTips"] = Resource.PathTool.GetCnfPath("config_endless_land_tips"),
	["EndlessGrade"] = Resource.PathTool.GetCnfPath("config_endless_grade"),
	["EndlessLandBattleCost"] = Resource.PathTool.GetCnfPath("config_endless_land_battle_cost"),

	--怪物攻城
	["AttackCityActivityTime"] = Resource.PathTool.GetCnfPath("config_attack_city_activity_time") ,
	["AttackCityBossReward"] = Resource.PathTool.GetCnfPath("config_attack_city_boss_reward") ,
	["AttackCityEncourage"] = Resource.PathTool.GetCnfPath("config_attack_city_encourage") ,
	["AttackCityMonster"] = Resource.PathTool.GetCnfPath("config_attack_city_monster") ,
	["AttackCityRankReward"] = Resource.PathTool.GetCnfPath("config_attack_city_rank_reward") ,

	--精灵
	["Spirit"] = Resource.PathTool.GetCnfPath("config_spirit") ,
	["SpiritEnhace"] = Resource.PathTool.GetCnfPath("config_spirit_enhance") ,
	["SpiritPokedex"] = Resource.PathTool.GetCnfPath("config_spirit_pokedex") ,
	["SpriteTrain"] = Resource.PathTool.GetCnfPath("config_sprite_train") ,
	["SpriteTalentSkill"] = Resource.PathTool.GetCnfPath("config_sprite_talent_skill") ,
	["SpriteStarUp"] = Resource.PathTool.GetCnfPath("config_sprite_star_up") ,
	["SpriteSpecialProperty"] = Resource.PathTool.GetCnfPath("config_sprite_special_property") ,
	["SpriteQualifications"] = Resource.PathTool.GetCnfPath("config_sprite_qualifications") ,

	--我要变强
	["Stronger"] = Resource.PathTool.GetCnfPath("config_stronger") ,
	["StrongerRecommended"] = Resource.PathTool.GetCnfPath("config_stronger_recommended") ,

	--限时副本
	["Deseno"] = Resource.PathTool.GetCnfPath("config_deseno") ,
	--限时副本预告
	["ActivityAdvanceNotice"] = Resource.PathTool.GetCnfPath("config_activity_advance_notice"),

	--屏蔽字表
	["Shield"] = Resource.PathTool.GetCnfPath("config_shield"),

	--邮件
	["Email"] = Resource.PathTool.GetCnfPath("config_email"),

	--领地
	["LandCity"] = Resource.PathTool.GetCnfPath("config_land_city") ,
	["LandPatrol"] = Resource.PathTool.GetCnfPath("config_land_patrol") ,
	["LandPatrolEvent"] = Resource.PathTool.GetCnfPath("config_land_patrol_event") ,

	--VIP
	["vipTips"] = Resource.PathTool.GetCnfPath("config_vipTips"),
	["Vip"] = Resource.PathTool.GetCnfPath("config_vip"),

	--爬塔
	["TowerChapter"] = Resource.PathTool.GetCnfPath("config_tower_chapter"),
	["TowerGameLevel"] = Resource.PathTool.GetCnfPath("config_tower_gamelevel"),
	["TowerEliteMap"] = Resource.PathTool.GetCnfPath("config_tower_elite_map"),
	["TowerStarGoods"] = Resource.PathTool.GetCnfPath("config_tower_star_goods"),
	["TowerShop"] = Resource.PathTool.GetCnfPath("config_tower_shop"),
	["TowerRandPro"] = Resource.PathTool.GetCnfPath("config_tower_rand_pro"),

	--装备
	["EquipmentRefine"] = Resource.PathTool.GetCnfPath("config_equipment_refine"),
	["EquipmentStrengthenForge"] = Resource.PathTool.GetCnfPath("config_equipment_strengthen_forge"),
	["EquipmentRefineForge"] = Resource.PathTool.GetCnfPath("config_equipment_refine_forging"),
	["EquipmentSuit"] = Resource.PathTool.GetCnfPath("config_equipment_suit"),
	["EquipmentStrenthen"] = Resource.PathTool.GetCnfPath("config_equipment_strengthen"),
	["Equipment"] = Resource.PathTool.GetCnfPath("config_equipment"),
	["EquipmentCompose"] = Resource.PathTool.GetCnfPath("config_equipment_compose"),
	["EquipmentTalent"] = Resource.PathTool.GetCnfPath("config_equipment_talent"),
	["EquipmentStarUp"] = Resource.PathTool.GetCnfPath("config_equipment_star_up"),
	["EquipmentQualityUp"] = Resource.PathTool.GetCnfPath("config_equipment_quality_up"),
	
	--圣器
	["Treasure"] = Resource.PathTool.GetCnfPath("config_treasure"),
	["TreasureStrengthen"] = Resource.PathTool.GetCnfPath("config_treasure_strengthen"),
	["TreasureRefine"] = Resource.PathTool.GetCnfPath("config_treasure_refine"),
	["TreasureStrengthenForge"] = Resource.PathTool.GetCnfPath("config_treasure_strengthen_forge"),
	["TreasureRefineForge"] = Resource.PathTool.GetCnfPath("config_treasure_refine_forging"),
	["TreasureCompose"] = Resource.PathTool.GetCnfPath("config_treasure_compose"),
	["TreasureQualityUpCost"] = Resource.PathTool.GetCnfPath("config_treasure_quality_up_cost"),

	--叛军
	["RebelQuality"] = Resource.PathTool.GetCnfPath("config_rebel_quality"),
	["RebelBattle"] = Resource.PathTool.GetCnfPath("config_rebel_battle"),
	["RebelShop"] = Resource.PathTool.GetCnfPath("config_rebel_shop"),
	["RebelRankAward"] = Resource.PathTool.GetCnfPath("config_rebel_rank_award"),
	["RoleRebel"] = Resource.PathTool.GetCnfPath("config_role_rebel"),
	["RebelExploit"] = Resource.PathTool.GetCnfPath("config_rebel_exploit"),
	["RebelSneer"] = Resource.PathTool.GetCnfPath("config_rebel_sneer"),

	--行为设置
	["MonitorCount"] = Resource.PathTool.GetCnfPath("config_monitor_count"),
	--机器人名字库
	["RobotNameList"] = Resource.PathTool.GetCnfPath("config_robot_name_list"),
	--登陆推送弹框
	["Push"] = Resource.PathTool.GetCnfPath("config_push"),
	--爵位
	["Nobility"] = Resource.PathTool.GetCnfPath("config_nobility"),

	--超值购
	["SuperDiscountBuyReward"] = Resource.PathTool.GetCnfPath("config_super_discount_buy_reward"),
	["SuperDiscountAccumulateReward"] = Resource.PathTool.GetCnfPath("config_super_discount_accumulate_reward"),

	--直购特惠
	["DirectBuyShop"] = Resource.PathTool.GetCnfPath("config_direct_buy_shop"),
	["ExchangeShop"] = Resource.PathTool.GetCnfPath("config_exchange_shop"),

	--七天排行榜
	["SevenDayRankReward"] = Resource.PathTool.GetCnfPath("config_seven_day_rank_reward"),

	-- 今日推荐充值
	["TodayRecommendRecharge"] = Resource.PathTool.GetCnfPath("config_today_recommend_recharge"),

	--节日活动
	["FestivalActivity"] = Resource.PathTool.GetCnfPath("config_festival_activity"),
	["FestivalActivityParam"] = Resource.PathTool.GetCnfPath("config_festival_activity_param"),

	--协助英雄
	["AssistHero"] = Resource.PathTool.GetCnfPath("config_assist_hero"),

	--精准推送
	["AccurateRecommend"] = Resource.PathTool.GetCnfPath("config_accurate_recommend"),
	["AccurateRecommendGift"] = Resource.PathTool.GetCnfPath("config_accurate_recommend_gift"),

	--宝石
	["Gem"] = Resource.PathTool.GetCnfPath("config_gem"),
	["GemSkill"] = Resource.PathTool.GetCnfPath("config_gem_skill"),
	["GemHole"] = Resource.PathTool.GetCnfPath("config_gem_hole"),
	["GemInlayLimit"] = Resource.PathTool.GetCnfPath("config_gem_inlay_limit"),

	--组队
	["TeamFBChapter"] = Resource.PathTool.GetCnfPath("config_team_fb_chapter"),
	["RawGemstoneShop"] = Resource.PathTool.GetCnfPath("config_raw_gemstone_shop"),
	["TeamDungeonLottery"] = Resource.PathTool.GetCnfPath("config_team_dungeon_lottery"),
	["TeamDungeonTaskLink"] = Resource.PathTool.GetCnfPath("config_team_dungeon_task_link"),
	["TeamDungeonTask"] = Resource.PathTool.GetCnfPath("config_team_dungeon_task"),
	["TeamDungeonBossReward"] = Resource.PathTool.GetCnfPath("config_team_dungeon_boss_reward"),
	["TeamNPCData"] = Resource.PathTool.GetCnfPath("config_team_npc_data"),
	["TeamQuestionData"] = Resource.PathTool.GetCnfPath("config_team_question_data"),
	["TeamDungeonQuickChat"] = Resource.PathTool.GetCnfPath("config_team_dungeon_quick_chat"),
	["TeamDungeonLotteryReward"] = Resource.PathTool.GetCnfPath("config_team_dungeon_lottery_reward"),
	["TeamNPCTips"] = Resource.PathTool.GetCnfPath("config_team_npc_tips"),
	["Rule_TeamLottery"] = Resource.PathTool.GetCnfPath("config_rule_team_lottery"),
	["Rule_Team"] = Resource.PathTool.GetCnfPath("config_rule_team"),
	
	--场景
	["Scene"] = Resource.PathTool.GetCnfPath("config_scene"),
	["SceneGather"] = Resource.PathTool.GetCnfPath("config_scene_gather"),

	--主城
	["ConfigSetCity"] = Resource.PathTool.GetCnfPath("config_set_city"),

	--天梯
	["LadderDan"] = Resource.PathTool.GetCnfPath("config_ladder_ban"),
	["LadderKingAward"] = Resource.PathTool.GetCnfPath("config_ladder_king_award"),
	["LadderTask"] = Resource.PathTool.GetCnfPath("config_ladder_task"),
	["LadderCompetitionSeason"] = Resource.PathTool.GetCnfPath("config_ladder_competition_season"),

	--占卜
	["Card"] = Resource.PathTool.GetCnfPath("config_card"),
	["CardLevelUp"] = Resource.PathTool.GetCnfPath("config_card_level_up"),
	["CardBreak"] = Resource.PathTool.GetCnfPath("config_card_break"),
	["DestinyShop"] = Resource.PathTool.GetCnfPath("config_destiny_shop"),

	["CollectionBook"] = Resource.PathTool.GetCnfPath("config_collection_book"),
	["CollectionGroup"] = Resource.PathTool.GetCnfPath("config_collection_group"),
	["DivinationHole"] = Resource.PathTool.GetCnfPath("config_divination_hole"),

	["CardGroupMap"] = Resource.PathTool.GetCnfPath("config_card_group_map"),
	["CollectionInfo"] = Resource.PathTool.GetCnfPath("config_collection_info"),
	["ZhanBuAchievement"] = Resource.PathTool.GetCnfPath("config_zhanbu_achievement"),
	["StarSoulShop"] = Resource.PathTool.GetCnfPath("config_star_soul_shop"),

	["LowCardGroup"] = Resource.PathTool.GetCnfPath("config_low_card_group"),
	["HighCardGroup"] = Resource.PathTool.GetCnfPath("config_high_card_group"),
	["CardHandBook"] = Resource.PathTool.GetCnfPath("config_card_handbook"),
	--隐藏商店
	["HideShop"] = Resource.PathTool.GetCnfPath("config_hide_shop"),

	--推送
	["NewPush"] = Resource.PathTool.GetCnfPath("config_push_new"),

	--在线奖励
	["Onlineaward"] = Resource.PathTool.GetCnfPath("config_onlineaward"),

	--[[限时活动]]
	--圣诞节
	["ChristmasBossLevel"] = Resource.PathTool.GetCnfPath("config_christmas_boss_level"),
	["ChristmasBoss"] = Resource.PathTool.GetCnfPath("config_christmas_boss"),
	["ChristmasScoreExchangeShop"] = Resource.PathTool.GetCnfPath("config_christmas_score_exchange_shop"),
	["ChristmasItemExchangeShop"] = Resource.PathTool.GetCnfPath("config_christmas_item_exchange_shop"),
	["ChristmasSnowBall"] = Resource.PathTool.GetCnfPath("config_christmas_snowball"),

	--回收
	["RecycleOpenLevel"] = Resource.PathTool.GetCnfPath("config_recycle_open_level"),

	--七天登录
	["SevenDayLogin"] = Resource.PathTool.GetCnfPath("serven_day_login"),
	--七天活动
	["SevenDayView"] = Resource.PathTool.GetCnfPath("serven_day_view"),

	--限时活动掉落
	["LimitDrop"] = Resource.PathTool.GetCnfPath("config_limit_drop"),

	--充值回馈
	["RechargeReward"] = Resource.PathTool.GetCnfPath("recharge_reward"),

	--充值额外奖励
	["RechargeExtraReward"] = Resource.PathTool.GetCnfPath("recharge_extra_reward"),

	--累充奖励
	["TotalRechargeReward"] = Resource.PathTool.GetCnfPath("total_recharge_reward"),
	["TotalRechargeUI"] = Resource.PathTool.GetCnfPath("total_recharge_ui"),

	--自定义超值购
	["Custompurchase"] = Resource.PathTool.GetCnfPath("config_custom_purchase"),

	--购买奖励(新)
	["NewSuperDiscountBuyReward"] = Resource.PathTool.GetCnfPath("config_new_super_discount_buy_reward"),
	
	--购买奖励(新)
	["NewSuperDiscountAccumulateReward"] = Resource.PathTool.GetCnfPath("config_new_super_discount_accumulate_reward"),

	--热卖商品
	["HotSaleGoods"] = Resource.PathTool.GetCnfPath("hot_sale_goods"),

	--转盘
	["Turntable"] = Resource.PathTool.GetCnfPath("config_turntable"),
	["TurntableLottery"] = Resource.PathTool.GetCnfPath("config_turntable_lottery"),
	["TurntableRank"] = Resource.PathTool.GetCnfPath("config_turntable_rank"),
	["TurntablePuzzle"] = Resource.PathTool.GetCnfPath("config_turntable_puzzle"),

	--限时产出
	["LimitProduct"] = Resource.PathTool.GetCnfPath("config_limit_product"),

	--水晶商店
	["CrystalShop"] = Resource.PathTool.GetCnfPath("config_crystal_shop"),

	--世界任务等级
	["WorldTaskLevel"] = Resource.PathTool.GetCnfPath("config_world_task_level"),

	--世界等级任务事件
	["WorldLevelTaskEvent"] = Resource.PathTool.GetCnfPath("config_world_level_task_event"),
	["WorldLevelTaskIdList"] = Resource.PathTool.GetCnfPath("config_world_level_task_id_list"),
	["WorldLevelTaskEvent"] = Resource.PathTool.GetCnfPath("config_world_level_task_event"),

	--首次弹窗
	["FirstPush"] = Resource.PathTool.GetCnfPath("config_push_first"),

	--苹果内购商品ID映射表
	["AppleIAPIDMap"] = Resource.PathTool.GetCnfPath("config_apple_iap_id_map"),

	--苹果内购充值上限
	["AppleIAPUpperLimit"] = Resource.PathTool.GetCnfPath("config_apple_iap_upper_limit"),

	--渠道特殊设定
	["ChannelSetting"] = Resource.PathTool.GetCnfPath("config_channel_setting"),
	["ChannelExamineFiles"] = Resource.PathTool.GetCnfPath("config_channel_examine_files"),
	--好友邀请
	["FriendInvite"] = Resource.PathTool.GetCnfPath("config_friend_invite"),
	--好友召回
	["FriendSummon"] = Resource.PathTool.GetCnfPath("config_friend_summon"),
	--绑定身份奖励
	["BindingReward"] = Resource.PathTool.GetCnfPath("config_binding_reward"),
	
	--分享Facebook奖励
	["FacebookShareReward"] = Resource.PathTool.GetCnfPath("config_facebook_share_reward"),

	--新版扭蛋机
	["NDLotteryCost"] = Resource.PathTool.GetCnfPath("nd_lottery_cost"),
	["NDLottery"] = Resource.PathTool.GetCnfPath("nd_lottery"),
	["NDLotteryReward"] = Resource.PathTool.GetCnfPath("nd_lottery_reward"),

	--五星好评
	["FiveStarHighPraise"] = Resource.PathTool.GetCnfPath("config_fivestar_highpraise"),
	
	--UNLOCK
	["UnlockActivityReward"] = Resource.PathTool.GetCnfPath("unlock_activity"),
	["UnlockWatchingVideoReward"] = Resource.PathTool.GetCnfPath("unlock_watching_video_reward"),

	--阵营招募
	["CampRecruit"] = Resource.PathTool.GetCnfPath("config_camp_recruit"),
	["CampRecruitHeroList"] = Resource.PathTool.GetCnfPath("config_camp_recruit_hero_list"),

	--积分赛
	["ScoreMatchActivityTime"] = Resource.PathTool.GetCnfPath("config_score_match_activity_time"),
	["ScoreMatchBattleTask"] = Resource.PathTool.GetCnfPath("config_score_match_battle_task"),
	["ScoreMatchBattleTimesReward"] = Resource.PathTool.GetCnfPath("config_score_match_battle_times_reward"),
	["ScoreMatchRankReward"] = Resource.PathTool.GetCnfPath("config_score_match_rank_reward"),
	["ScoreMatchReward"] = Resource.PathTool.GetCnfPath("config_score_match_reward"),
	["ScoreMatchShop"] = Resource.PathTool.GetCnfPath("config_score_match_shop"),
	["ScoreMatchTimesCost"] = Resource.PathTool.GetCnfPath("config_score_match_times_cost"),
	["ScoreMatchTargetServer"] = Resource.PathTool.GetCnfPath("config_score_match_target_server"),

	--援军助威
	["ReinforcementEncourage"] = Resource.PathTool.GetCnfPath("config_reinforcement_encourage"),

	--资源战
	["ResourceWarChapter"] = Resource.PathTool.GetCnfPath("config_resource_war_chapter"),
	["ResourceWarResource"] = Resource.PathTool.GetCnfPath("config_resource_war_resource"),
	["ResourceWarModelling"] = Resource.PathTool.GetCnfPath("config_resource_war_modelling"),
	["ResourceWarGuildProfit"] = Resource.PathTool.GetCnfPath("config_resource_war_guild_profit"),
	["ResourceWarTechnology"] = Resource.PathTool.GetCnfPath("config_resource_war_technology"),
	["ResourceWarShop"] = Resource.PathTool.GetCnfPath("config_resource_war_shop"),

	--公会战
	["GuildWarStar"] = Resource.PathTool.GetCnfPath("config_guild_war_star"),
	["GuildWarTower"] = Resource.PathTool.GetCnfPath("config_guild_war_tower"),
	["GuildWarVSActivityTime"] = Resource.PathTool.GetCnfPath("config_guild_vs_activity_time"),
	["GuildWarCommon"] = Resource.PathTool.GetCnfPath("config_guild_war_common"),

	--审核主城
	["ExamineCityIcon"] = Resource.PathTool.GetCnfPath("config_examine_cityicon"),

	--跨服排行榜
	["RechargeRank"] = Resource.PathTool.GetCnfPath("config_recharge_rank"),
	["RechargeAward"] = Resource.PathTool.GetCnfPath("config_recharge_award"),

	--GM等级
	["VipGM"] = Resource.PathTool.GetCnfPath("config_vip_gm"),
	--GM商店
	["GMShop"] = Resource.PathTool.GetCnfPath("config_gm_shop"),
}
