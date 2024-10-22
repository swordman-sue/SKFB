
--[[
%% @module: 模块协议注册
%% @author: swordman sue
%% @created: 2016-07-26
--]]

--注册各模块所需协议
function Modules.ModulesMgr:RegistAllProtocols()
	--登陆	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_ACCOUNT_RESP_PB, ModuleType.LOGIN, "LoginAccountResp_PB")

	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_ACCOUNT_RESP, ModuleType.LOGIN, "LoginAccountResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_SELECT_SERVER_RESP, ModuleType.LOGIN, "SelectServerResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_ENTER_GAME_NOTIFY, ModuleType.LOGIN, "EnterGameResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HEART_BEAT, ModuleType.LOGIN, "HeartBeatResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_CHANGE_ROLE_NAME_RESP, ModuleType.LOGIN, "ChangeRoleNameResp") 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_START_GAME_RESP, ModuleType.LOGIN, "StartGameResp") 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_AUTHENTICATION_CODE_RESP, ModuleType.LOGIN, "AuthenticationCodeResp") 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MOBILE_BIND_RESP, ModuleType.LOGIN, "MobileBindResp") 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_CREATE_ROLE_RESP, ModuleType.LOGIN, "CreateRoleResp") 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SDK_LOGIN_RESP, ModuleType.LOGIN, "LoginAccountResp_SDK")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CLIENT_FORCE_UPDATE_RESP, ModuleType.LOGIN, "ClientForceUpdateResp")

	--角色
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ROLE_INFO_NOTIFY, ModuleType.ROLE, "RoleInfoNotifyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ROLE_PROPERTY_NOTIFY, ModuleType.ROLE, "RolePropertyNotifyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ROLE_LEVEL_UPGRADE, ModuleType.ROLE, "RoleLevelUpgradeResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ROLE_PRO_RECOVER_RESP, ModuleType.ROLE, "RoleProRecoverResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUIDE_UPDATE_RESP, ModuleType.ROLE, "UpdateProgressTagResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLAYER_LEVEL_MESSAGE_RESP, ModuleType.ROLE, "PlayerLevelMessageResp")

	--错误通知
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GAME_ERROR_NOTIFY, ModuleType.ERROR_NOTIFY, "GameErrorNotify")	

	--时间同步
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SERVER_TIME_RESP, ModuleType.TIME_SYNC, "ServerTimeResp")	

	--切换
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HEAD_IMAGE_REPLACE_RESP, ModuleType.ROLE, "HeadImageReplaceResp")	

	--聊天
	self:RegistProtocolToModule(Net.ProtocolNo.MSG_S2C_CHAT_RESP, ModuleType.CHAT, "LiaoTianReturn")	
	self:RegistProtocolToModule(Net.ProtocolNo.MSG_S2C_CHAT_NOTIFY, ModuleType.CHAT, "LiaoTianBroadcast")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHAT_RECORD_LIST_RESP, ModuleType.CHAT, "ChatRecordListResp")	

	--查看玩家信息
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ROLE_DETAILS_RESP, ModuleType.CHECKROLEINFO, "RoleDetailsResp")	

	--活动公告
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ACTIVITY_NOTICE_RESP, ModuleType.NOTICE, "ActivityNoticeResp")	
	--系统公告
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SYSTEM_NOTICE, ModuleType.NOTICE, "SystemNoticeResp")

	--兑换码
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_ACTIVITY_GIFT_RESP, ModuleType.ACTIVITY, "TakeActivityGiftResp")	

	--背包
	-- 物品列表响应
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ITEM_LIST_RESP, ModuleType.Pack, "ItemListResp")
	-- 物品使用响应
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ITEM_USE_RESP, ModuleType.Pack, "ItemUseResp")
	-- 物品更新
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ITEM_DATA_NOTIFY, ModuleType.Pack, "ItemDataNotify")
	--翻牌响应
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHOOSE_CARD_RESP, ModuleType.Pack, "ChooseCardResp")
	--物品合成
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ITEM_COMPOSE_RESP, ModuleType.Pack, "ItemComposeResp")
	--物品分解
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ITEM_DECOMPOSE_RESP, ModuleType.Pack, "ItemDecomposeResp")
	
	--装备
	-- 装备列表响应
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_LIST_RESP, ModuleType.EQUIP, "EquipmentListResp")
	-- 装备合成响应
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_COMPOSE_RESP, ModuleType.EQUIP, "EquipmentComposeResp")
	-- 请求装备强化响应: 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_STRENG_RESP, ModuleType.EQUIP, "EquipMentstrengResp")
	-- 装备基本信息通知:
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_INFO_NOTIFY, ModuleType.EQUIP, "EquipMentInfoNotify")
	-- 装备精炼响应:
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_REFINE_RESP, ModuleType.EQUIP, "EquipMentRefineResp")
	-- 装备穿戴响应：
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_PUT_ON_RESP, ModuleType.EQUIP, "EquipMentPutOnResp")
	-- 装备一键强化响应
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_ONE_KEY_ENHANCE_RESP, ModuleType.EQUIP, "EquipmentOneKeyEnhanceResp")
	-- 装备升星
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_STAR_UP_RESP, ModuleType.EQUIP, "EquipmentStarUpResp")
	-- 装备升星
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EQUIPMENT_QUALITY_UP_RESP, ModuleType.EQUIP, "EquipmentQualityUpResp")
	--副本
	self:RegistProtocolToModule(Net.ProtocolNo.MSG_S2C_FB_MAIN_DATA_RESP, ModuleType.DUNGEON, "FBMainDataResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_HERO_DATA_RESP, ModuleType.DUNGEON, "FBHeroDataResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_DAILY_DATA_RESP, ModuleType.DUNGEON, "FBDailyDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_AWARD_GET_RESP, ModuleType.DUNGEON, "FBAwardGetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_CLEAN_RESP, ModuleType.DUNGEON, "FBCleanResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_RESET_RESP, ModuleType.DUNGEON, "FBResetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_ONE_KEY_TAKE_REWARD_RESP, ModuleType.DUNGEON, "FBPassedAllResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2S_TAKE_MAIN_FB_CHAPTER_REWARD_RESP, ModuleType.DUNGEON, "FBPassedBoxRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_ELITE_DATA_RESP, ModuleType.DUNGEON, "FBEliteDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MAP_COMMENT_LIST_RESP, ModuleType.DUNGEON, "MapCommentListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MAP_COMMENT_RESP, ModuleType.DUNGEON, "MapCommentResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MAP_COMMENT_LIKE_RESP, ModuleType.DUNGEON, "MapCommentLikeResp")

	--战斗
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_BATTLE_START_RESP, ModuleType.BATTLE, "BattleStartResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_BATTLE_END_RESP, ModuleType.BATTLE, "BattleEndResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_BATTLE_ASSIST_TARGET_RESP, ModuleType.BATTLE, "BattleAssistTargetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_SEARCH_TARGET_RESP, ModuleType.BATTLE, "SearchTargetResp_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_READY_NOTIFY, ModuleType.BATTLE, "ReadyNotify_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_READY_FINISH_RESP, ModuleType.BATTLE, "ReadyFinishResp_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_BATTLE_START_NOTIFY, ModuleType.BATTLE, "BattleStartNotify_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_BATTLE_OVER_NOTIFY, ModuleType.BATTLE, "BattleOverNotify_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_USE_SKILL_RESP, ModuleType.BATTLE, "UseSkillResp_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_USE_SKILL_NOTIFY, ModuleType.BATTLE, "UseSkillNotify_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_HP_CHANGE_RESP, ModuleType.BATTLE, "HPChangeSyncResp_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_HP_CHANGE_NOTIFY, ModuleType.BATTLE, "HPChangeSyncNotify_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_CREATE_BUFF_RESP, ModuleType.BATTLE, "BuffSyncResp_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_CREATE_BUFF_NOTIFY, ModuleType.BATTLE, "BuffSyncNotify_PVP")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_BATTLE_RESULT_RESP, ModuleType.BATTLE, "BattleEndResp_PVP")

	--英雄
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_BASIC_LIST_RESP, ModuleType.HERO, "HeroBasicListResp")		
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_DETAIL_INFO_RESP, ModuleType.HERO, "HeroDetailInfoResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_GOTO_BATTLE_RESP, ModuleType.HERO, "HeroGotoBattleResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_UPGRADE_RESP, ModuleType.HERO, "HeroUpgradeResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_BREACH_RESP, ModuleType.HERO, "HeroBreakResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_TRAIN_RESP, ModuleType.HERO, "HeroTrainResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_COMPOSE_RESP, ModuleType.HERO, "HeroComposeResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_BASIC_INFO_NOTIFY, ModuleType.HERO, "HeroBasicInfoResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_LINEUP_LIST_RESP, ModuleType.HERO, "HeroLineupListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_RECRUIT_RESP, ModuleType.HERO,"HeroRecruitResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_RECRUITDATA_RESP, ModuleType.HERO, "HeroRecruitDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_COLLECTION_LIST_RESP, ModuleType.HERO, "HeroCollectionResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_DESTINY_RESP, ModuleType.HERO, "HeroPotentialResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REINFORCEMENT_TO_BATTLE_RESP, ModuleType.HERO, "RecinforcementToBattleResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_AWAKEN_RESP, ModuleType.HERO, "HeroAwakenResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_EQUIP_AWAKEN_ITEM_RESP, ModuleType.HERO, "HeroEquipAwakenItemResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_TRAIN_REPLACE_RESP, ModuleType.HERO, "HeroTrainReplaceResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_TRAIN_REWARD_GET_RESP, ModuleType.HERO, "HeroTrainRewardGetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_EVOLUTION_RESP, ModuleType.HERO, "HeroEvolutionResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_VIP_SUMMON_RESP, ModuleType.HERO, "HeroVipSummonResp")
		
	--称号
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ROLE_TITLE_INFO_RESP, ModuleType.TITLE, "RoleTitleInfoResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_USE_TITLE_RESP, ModuleType.TITLE, "UseTitleResp")

	--商城	
	self:RegistProtocolToModule(Net.ProtocolNo.MSG_S2C_SHOP_GOODS_BUY_RESP, ModuleType.SHOP, "ShopBuyResponse")
	self:RegistProtocolToModule(Net.ProtocolNo.MSG_S2C_SHOP_GOODS_BUY_RECORD_RESP, ModuleType.SHOP, "ShopBuyRecordResponse")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SHOP_GOODS_SELL_RESP, ModuleType.SHOP, "ShopGoodsSellResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SHOP_LIMIT_GOODS_LIST_RESP, ModuleType.SHOP, "ShopLimitGoodsListResp")	

	--邮件
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EMAIL_LIST_RESP, ModuleType.EMAIL, "EmailListResponse")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_EMAIL_NEW_NOTIFY, ModuleType.EMAIL, "EmailNewResponse")
	
	--排行榜
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RANK_LIST_RESP, ModuleType.RANK, "RankListResponse")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RANK_LIKE_RESP, ModuleType.RANK, "RankLikeResponse")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RANK_LEAVE_MESSAGE_RESP, ModuleType.RANK, "RankLeaveMessageResponse")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RANK_LEAVE_MESSAGE_LIST_RESP, ModuleType.RANK, "RankLeaveMessageListResponse")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_RANK_LIST_RESP, ModuleType.UNION, "GuildRankListResp")

	--好友
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_LIST_RESP, ModuleType.FRIEND, "FreindListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_BLACK_LIST_RESP, ModuleType.FRIEND, "BlcakListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_APPLY_LIST_RESP, ModuleType.FRIEND, "FriendApplyListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_RECOMMEND_LIST_RESP, ModuleType.FRIEND, "FriendRecommendListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_ADD_RESP, ModuleType.FRIEND, "FriendAddResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_DEL_RESPS, ModuleType.FRIEND, "FriendDelResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_APPLY_REPLY_RESP, ModuleType.FRIEND, "FriendApplyReplyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_BLACK_ADD_RESP, ModuleType.FRIEND, "BlcakAddResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_BLACK_DEL_RESP, ModuleType.FRIEND, "BlackDelResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_ENERGY_GIVE_RESP, ModuleType.FRIEND, "FriendEnergyGiveResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_ENERGY_GET_RESP, ModuleType.FRIEND, "FriendEnergyGetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_DATA_GET_RESP, ModuleType.FRIEND, "FriendDataRefresh")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_TARGET_FIND_RESP, ModuleType.FRIEND, "FriendTargetFindResp")

	--圣器
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_LIST_RESP, ModuleType.TREASURE, "TreasureListResp")   				
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_STRENGTHEN_RESP, ModuleType.TREASURE, "TreasureStrengthenResp")		
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_REFINE_RESP, ModuleType.TREASURE, "TreasureRefineResp")				
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_COMPOSE_RESP, ModuleType.TREASURE, "TreasureComposeResp")			
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_INFO_NOTIFY, ModuleType.TREASURE, "TreasureInfoNotify")				
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_PUT_ON_RESP, ModuleType.TREASURE, "TreasurePutOnResp")				
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_QUALITY_UP_RESP, ModuleType.TREASURE, "TreasureQualityUpResp")				

	--回收
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECYCLE_DECOMPOSE_RESP, ModuleType.RECYCLE, "RecycleDecomposeResp")				
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECYCLE_RESET_RESP, ModuleType.RECYCLE, "RecycleResetResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECYCLE_DECOMPOSE_RETURN_RESP, ModuleType.RECYCLE, "RecycleDecomposeReturnResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECYCLE_RESET_RETURN_RESP, ModuleType.RECYCLE, "RecycleResetReturnResp")				

	--叛军
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REBEL_DATA_RESP, ModuleType.REBEL, "RebelDataResp")				
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REBEL_EXPLOIT_AWARD_RESP, ModuleType.REBEL, "RebelExploitAwardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REBEL_EXPLOIT_AWARD_RECORD_RESP, ModuleType.REBEL, "RebelExploitAwardRecordResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REBEL_SHARE_RESP, ModuleType.REBEL, "RebelShaReResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REBEL_DISCOVER_NOTIFY, ModuleType.REBEL, "RebelDiscoverNotify")

	--爬塔
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FB_TOWER_DATA_RESP, ModuleType.TOWER, "FbTowerDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TOWER_BUY_TIMES_RESP, ModuleType.TOWER, "TowerBuyTimesResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TOWER_RESET_RESP, ModuleType.TOWER, "TowerResetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TOWER_SELECT_PRO_RESP, ModuleType.TOWER, "TowerSelectProResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TOWER_BUY_STAR_GOODS_RESP, ModuleType.TOWER, "TowerBuyStarGoosResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TOWER_ONE_KEY_CLEAN_RESP, ModuleType.TOWER, "TowerOneKeyCleanResp")

	--世界之心
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HEART_OF_WORLD_DATA_RESP, ModuleType.WORLD_HEART, "WorldHeartDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HEART_OF_WORLD_LEVEL_UP_RESP, ModuleType.WORLD_HEART, "WorldHeartLevelUpDataResp")

	--精灵
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPIRIT_ENHANCE_RESP, ModuleType.SPIRIT, "SpiritEnhanceResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPIRIT_USE_RESP, ModuleType.SPIRIT, "SpiritUseResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPIRIT_DATA_RESP, ModuleType.SPIRIT, "SpiritDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPRITE_TRAIN_RESP, ModuleType.SPIRIT, "SpiritTrainResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPRITE_STAR_UP_RESP, ModuleType.SPIRIT, "SpiritStarUpResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPRITE_BLESS_RESP, ModuleType.SPIRIT, "SpiritBlessResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SPRITE_COMPOSE_RESP, ModuleType.SPIRIT, "SpriteComposeResp")

	--领地
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLAYER_LAND_DATA_RESP, ModuleType.MANOR, "PlayerLandDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LAND_PATROL_RESP, ModuleType.MANOR, "LandPatrolResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LAND_GET_PATROL_AWARD_RESP, ModuleType.MANOR, "LandGetPatrolAwardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LAND_SKILL_LEVELUP_RESP, ModuleType.MANOR, "LandSkillLevelupResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LAND_SUPPRESS_RIOT_RESP, ModuleType.MANOR, "LandSuppressRiotResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FRIEND_LAND_LIST_RESP, ModuleType.MANOR, "FriendLandListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LAND_CITY_DETAILS_RESP, ModuleType.MANOR, "LandCityDetailsResp")

	--夺宝
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLUNDEREDTREASURES_DATA_RESP, ModuleType.PLUNDEREDTREASURES, "plunderedTreasuresDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLUNDEREDTREASURES_TARGET_LIST_RESP, ModuleType.PLUNDEREDTREASURES, "plunderedTreasuresTargetListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLUNDEREDTREASURES_FIVETIMES_RESP, ModuleType.PLUNDEREDTREASURES, "plunderedTreasuresFiveTimesResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_DUOBAO_PREGRESS_REWARD_RESP, ModuleType.PLUNDEREDTREASURES, "TakeDuoBaoPregressRewardResp")
	
	--竞技场
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ARENA_DATA_RESP, ModuleType.ARENA, "ArenaTargetListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ARENA_FIGHTREPORTDATA_RESP, ModuleType.ARENA, "ArenaFightReportResp")

	--公会
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_DATE_RESP, ModuleType.UNION, "GuildDateResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_CREATE_RESP, ModuleType.UNION, "GuildCreateResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_LIST_RESP, ModuleType.UNION, "GuildListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_MEMBER_LIST_RESP, ModuleType.UNION, "GuildMemberListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_APPLY_LIST_RESP, ModuleType.UNION, "GuildApplyListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_LEAVEMSG_LIST_RESP, ModuleType.UNION, "GuildLeavemsgListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_DISMISS_RESP, ModuleType.UNION, "GuildDismissResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_APPLY_RESP, ModuleType.UNION, "GuildApplyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_APPLY_CHECK_RESP, ModuleType.UNION, "GuildApplyCheckResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_APPLY_CANCEL_RESP, ModuleType.UNION, "GuildApplyCancelResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_KICKOUT_RESP, ModuleType.UNION, "GuildKickoutResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_LEAVE_RESP, ModuleType.UNION, "GuildLeaveResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_LEAVEM_ESSAGE_RESP, ModuleType.UNION, "GuildLeaveEssageResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_SEARCH_RESP, ModuleType.UNION, "GuildSearchResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_QY_RESP, ModuleType.UNION, "GuildQyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_QS_AWARD_RESP, ModuleType.UNION, "GuildQsAwardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_APPOINT_RESP, ModuleType.UNION, "GuildAppointResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_INFO_MODIFY_RESP, ModuleType.UNION, "GuildInfoModifyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_EVENT_LIST_RESP, ModuleType.UNION, "GuildEventListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_SKILL_LEVEL_UP_RESP, ModuleType.UNION, "GuildSkillLevelUpResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_SKILL_LIST_RESP, ModuleType.UNION, "GuildSkillListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_DATA_RESP, ModuleType.UNION, "GuildFbDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_MONSTER_DATA_RESP, ModuleType.UNION, "GuildMonsterDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_CHAPTER_REWARD_RESP, ModuleType.UNION, "GuildFbChapterRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_MAP_REWARD_RESP, ModuleType.UNION, "GuildFbMapRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_MAP_REWARD_RECORD_RESP, ModuleType.UNION, "GuildFbMapRewardRecordResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_BATTLE_TIMES_BUY_RESP, ModuleType.UNION, "GuildFbBattleTimesBuyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_IMPEACH_RESP, ModuleType.UNION, "GuildImpeachResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_DAMAGE_RECORD_RESP, ModuleType.UNION, "GuildFbDamageRecordResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_FB_CHAPTER_RESET_RESP, ModuleType.UNION, "GuildFbChapterResetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_MONSNTER_DAMAGE_NOTIFY, ModuleType.UNION, "GuildMonsnterDamageNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_MY_MAP_REWARD_RECORD_RESP, ModuleType.UNION, "GuildMyMapRewardRecordResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_IMPEACH_NOTICE, ModuleType.UNION, "GuildImpeachNotice")

	--日常任务
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_DAILY_MISSION_LIST_RESP, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "DaliyMissionListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_DAILY_MISSION_DATA_NOTIFY, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "DaliyMissionDataNotifyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_DAILY_MISSION_AWARD_RESP, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "DaliyMissionAwardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_DAILY_MISSION_POINT_AWARD_RESP, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "DaliyMissionPointAwardResp")
	--成就
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ACHIEVEMENT_LIST_RESP, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "AchievementListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ACHIEVEMENT_DATA_NOTIFY, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "AchievementDataNotifyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ACHIEVEMENT_AWARD_RESP, ModuleType.DALIYTASK_AND_ACHIEVEMENT, "AchievementAwardResp")

	--英雄商店
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SHOP_REFRESH_RESP, ModuleType.HERO_SHOP, "ShopRefreshResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_SHOP_DATA_RESP, ModuleType.HERO_SHOP, "HeroShopDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_AWAKEN_SHOP_DATA_RESP, ModuleType.HERO_SHOP, "AwakenShopDataResp")

	--世界boss
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_DATA_RESP, ModuleType.WORLDBOSS, "WorldBossDataResp") -- 消息说明: 世界BOSS数据响应 消息Id: 22502
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_SELECT_CAMP_RESP, ModuleType.WORLDBOSS, "WorldBossSelectCampResp") -- 消息说明: 选择阵营响应 消息Id: 22504
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_REWARD_RESP, ModuleType.WORLDBOSS, "WorldBossRewardResp") -- 消息说明: 领取奖励响应 消息Id: 22506
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_REWARD_RECORD_RESP, ModuleType.WORLDBOSS, "WorldBossRewardRecordResp") -- 消息说明: 领取奖励记录响应 消息Id: 22508
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_BATTLE_RECORD_RESP, ModuleType.WORLDBOSS, "WorldBossBattleRecordResp") -- 消息说明: 世界BOSS战报响应 消息Id: 22510
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_BATTLE_TIMES_BUY_RESP, ModuleType.WORLDBOSS, "WorldBossBattleTimesBuyResp")-- 消息说明: 世界BOSS挑战次数购买响应 消息Id: 22512
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_DAMAGE_NOTIFY, ModuleType.WORLDBOSS, "WorldBossDamageNotify")-- 消息说明: 世界BOSS挑战次数购买响应 消息Id: 22513
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_ACTIVITY_START_NOTIFY, ModuleType.WORLDBOSS, "WorldBossActivityStartNotify")-- 消息说明: 世界BOSS活动开始 消息Id: 22514
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_BOSS_ACTIVITY_OVER_NOTIFY, ModuleType.WORLDBOSS, "WorldBossActivityOverNotify")-- 消息说明: 世界BOSS活动结束 消息Id: 22515
	--领奖中心
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REWARD_CENTER_LIST_RESP, ModuleType.ACCEPT_PRIZE_CENTER, "RewardCenterListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REWARD_CENTER_GET_RESP, ModuleType.ACCEPT_PRIZE_CENTER, "RewardCenterGetResp") 
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_REWARD_NOTICE, ModuleType.ACCEPT_PRIZE_CENTER, "RewardNoticeNotification") 

	--红点
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RED_POINT_LIST_RESP, ModuleType.RED_MGR, "RedPointListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RED_POINT_NOTIFY, ModuleType.RED_MGR, "RedPointNotify")

	--新一天的通知
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NEW_DAY_NOTIFY, ModuleType.SUNDRY, "NewDayNotifyResp") 

	--活动——签到
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SIGN_IN_DATA_RESP, ModuleType.ACTIVITY, "SignInDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SIGN_IN_RESP, ModuleType.ACTIVITY, "SignInResp")

	--活动——领取体力
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TQT_DATA_RESP, ModuleType.ACTIVITY, "TqtDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TQT_REWARD_RESP, ModuleType.ACTIVITY, "TqtRewardResp")

	--活动——捡钱
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_YCS_DATA_RESP, ModuleType.ACTIVITY, "YcsDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_YCS_REWARD_RESP, ModuleType.ACTIVITY, "YcsRewardResp")

	--活动——七天活动
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SEVEN_DAY_TARGET_LIST_RESP, ModuleType.SEVENDAYS, "SevenDayTargetListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SEVEN_DAY_GET_REWARD_RESP, ModuleType.SEVENDAYS, "SevenDayGetRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SEVEN_DAY_TARGET_DATA_NOTICE, ModuleType.SEVENDAYS, "SevenDayTargetDataNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SEVEN_DAY_COMPLETION_REWARD_RESP, ModuleType.SEVENDAYS, "SevenDayCompletionRewardResp")
	
	--活动——等级商店
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LEVEL_SHOP_ACTIVITY_DATA_RESP, ModuleType.ACTIVITY, "LevelShopDataResp")
	--活动——等级礼包
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LEVEL_GIFT_DATA_RESP, ModuleType.ACTIVITY, "LevelGiftDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_LEVEL_GIFT_REWARD_RESP, ModuleType.ACTIVITY, "TakeLevelGiftRewardResp")
	
	--活动——vip福利
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_VIP_GIFT_DATA_RESP, ModuleType.ACTIVITY, "VipGiftDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_VIP_DAILY_GIFT_GET_RESP, ModuleType.ACTIVITY, "VipDailyGiftGetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_VIP_WEEK_GIFT_BUY_RESP, ModuleType.ACTIVITY, "VipWeekGiftBuyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_TODAY_RECOMMEND_GIFT_RESP, ModuleType.ACTIVITY, "TakeTodayRecommendGiftResp")

	--活动——开服基金
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_OPEN_SERVER_FUND_DATA_RESP, ModuleType.ACTIVITY, "OpenServerFundDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_OPEN_SERVER_FUND_BUY_RESP, ModuleType.ACTIVITY, "OpenServerFundBuyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_OPEN_SERVER_FUND_REWARD_TAKE_RESP, ModuleType.ACTIVITY, "OpenServerFundRewardTakeResp")
	
	--活动——全名福利
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ALL_PEOPLE_WELFARE_TAKE_RESP, ModuleType.ACTIVITY, "AllPeopleWelfareTakeResp")

	--活动——单充
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SINGLE_RECHARGE_DATA_RESP, ModuleType.ACTIVITY, "SingleRechargeDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_LIMIT_ACTIVITY_REWARD_RESP, ModuleType.ACTIVITY, "TakeLimitActivityRewardResp")

	--活动——累充
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ACCUMULATE_RECHARGE_DATA_RESP, ModuleType.ACTIVITY, "AccumulateRechargeDataResp")

	--活动——限时
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LIMIT_RECRUIT_DATA_RESP, ModuleType.ACTIVITY, "LimitRecruitDataResp")

	--活动——首充
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FIRST_RECHARGE_DATA_RESP, ModuleType.ACTIVITY, "FirstRechargeDataResp")

	--活动——壕签
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LUXURY_SIGN_IN_DATA_RESP, ModuleType.CRYSTAL_SHOP, "LuxurySignInDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_LUXURY_SIGN_IN_REWARD_RESP, ModuleType.CRYSTAL_SHOP, "TakeLuxurySignInRewardResp")

	--活动——登录奖励
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOGIN_REWARD_DATA_RESP, ModuleType.ACTIVITY, "LoginRewardDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_LOGIN_REWARD_RESP, ModuleType.ACTIVITY, "TakeLoginRewardResp")

	--充值
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECHARGE_DATA_RESP, ModuleType.CHARGE, "RechargeDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECHARGE_RESP, ModuleType.CHARGE, "RechargeResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MONTH_CARD_DATA_RESP, ModuleType.CHARGE, "MonthCardDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MONTH_CARD_BUY_RESP, ModuleType.CHARGE, "MonthCardBuyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_MONTH_CARD_REWARD_RESP, ModuleType.CHARGE, "TakeMonthCardRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SDK_ORDER_ID_RESP, ModuleType.CHARGE, "RechargeOrderIDResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECHARGE_SUCCESS_REPORT_RESP, ModuleType.CHARGE, "RechargeSuccessReportResp_SDK")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECHARGE_NOTIFY_RESP, ModuleType.CHARGE, "RechargeNotifyResp")	
	
	--pvp
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_TAKE_BATTLE_REWARD_RESP, ModuleType.PVP, "PVPTakeBattleRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLAYER_PVP_DATA_RESP, ModuleType.PVP, "PlayerPVPDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PVP_CANCEL_SEARCH_TARGET_RESP, ModuleType.PVP, "PVPCancelSearchTargetResp")

	--怪物攻城
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_DATA_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_ENTER_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityEnterResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_ENTER_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityEnterNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_LEAVE_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityLeaveResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_LEAVE_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityLeaveNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_GOTO_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityGotoResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_GOTO_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityGotoNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_MONSTER_LIST_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityMonsterListNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_MONSTER_DATA_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityMonsterDataNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_BOX_LIST_NOTICE, ModuleType.MONSTER_STORMCASTLE, "TreasureBoxListNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_BOX_DISAPPEAR_NOTICE, ModuleType.MONSTER_STORMCASTLE, "TreasureBoxDisappearNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_BOX_LOCK_RESP, ModuleType.MONSTER_STORMCASTLE, "TreasureBoxLockResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TREASURE_BOX_PICK_UP_RESP, ModuleType.MONSTER_STORMCASTLE, "TreasureBoxPickUpResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_RELIVE_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityReliveResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_ENCOURAGE_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityEncourageResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_TAKE_BOSS_REWARD_RESP, ModuleType.MONSTER_STORMCASTLE, "AttackCityTakeBossRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_TOP_RANK_LIST_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityTopRankListNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_RANK_UPDATE_NOTICE, ModuleType.MONSTER_STORMCASTLE, "AttackCityRankUpdateNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_ACTIVITY_START_NOTICE, ModuleType.MONSTER_STORMCASTLE, "ActivityStartNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ATTACK_CITY_ACTIVITY_OVER_NOTICE, ModuleType.MONSTER_STORMCASTLE, "ActivityOverNotice")
	
	--深渊
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ENDLESS_LAND_DATA_RESP, ModuleType.CHASM, "EndlessLandDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ENDLESS_LAND_ENTER_RESP, ModuleType.CHASM, "EndlessLandEnterResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ENDLESS_LAND_KILL_MONSTER_RESP, ModuleType.CHASM, "EndlessLandKillMonsterResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ENDLESS_LAND_COLLISION_RESP, ModuleType.CHASM, "EndlessLandCollisionResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ENDLESS_LAND_KEEP_KILL_REWARD_RESP, ModuleType.CHASM, "EndlessLandKeepKillRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ENDLESS_LAND_BATTLE_OVER_RESP, ModuleType.CHASM, "EndlessLandBattleOverResp")

	--次日登陆
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NEXT_DAY_REWARD_DATA_RESP, ModuleType.MORROW, "NextDayRewardDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NEXT_DAY_REWARD_TAKE_RESP, ModuleType.MORROW, "NextDayRewardTakeResp")
	
	--爵位
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NOBILITY_ACTIVATE_RESP, ModuleType.KNIGHTHOOD, "NobilityActivateResp")

	--超值购
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SUPER_DISCOUNT_DATA_RESP, ModuleType.SUPERDISCOUNTVIEW, "SuperDiscountDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SUPER_DISCOUNT_BUY_REWARD_RESP, ModuleType.SUPERDISCOUNTVIEW, "SuperDiscountBuyRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP, ModuleType.SUPERDISCOUNTVIEW, "SuperDiscounitAccumulateRewardResp")

	--直购特惠
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_DIRECT_SHOP_DATA_RESP, ModuleType.DIRECT_BUY_SHOP, "DirectShopDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_DIRECT_BUY_GOODS_RESP, ModuleType.DIRECT_BUY_SHOP, "TakeDirectBuyGoodsResp")

	--精准推送
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ACCURATE_RECOMMEND_DATA_RESP, ModuleType.DIRECT_BUY_SHOP, "AccurateRecommendDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_ACCURATE_RECOMMEND_GIFT_RESP, ModuleType.DIRECT_BUY_SHOP, "TakeAccurateRecommendGiftResp")

	--热卖(走特惠季的模块)
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HOT_SALE_GOODS_DATA_RESP, ModuleType.DIRECT_BUY_SHOP, "HotSaleGoodsDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP, ModuleType.DIRECT_BUY_SHOP, "TakeHotSaleGoodsRechargeRewardResp")

	--冲榜
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_MY_RANK_RESP, ModuleType.RANK, "MYRankResp")	

	--节日活动
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FESTIVAL_ACTIVITY_DATA_RESP, ModuleType.ACTIVITY, "FestivalActivityDataResp")			
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_FESTIVAL_ACTIVITY_REWARD_RESP, ModuleType.ACTIVITY, "TakeFestivalActivityRewardResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_RESP, ModuleType.ACTIVITY, "FestivalActivityGoodsExchangeResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_FESTIVAL_ACTIVITY_RED_POINT_RESP, ModuleType.ACTIVITY, "FestivalActivityRedPointResp")
	
	--宝石
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_LIST_RESP, ModuleType.GEM, "GemListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_COMPOSE_RESP, ModuleType.GEM, "GemComposeResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_INLAY_RESP, ModuleType.GEM, "GemInlayResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_UNLOAD_RESP, ModuleType.GEM, "GemUnloadResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_HOLE_OPEN_RESP, ModuleType.GEM, "GemHoleOpenResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_DATA_NOTIFY, ModuleType.GEM, "GemDataNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GEM_DECOMPOSE_RESP, ModuleType.GEM, "GemDecomposeResp")

	--场景
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_GOTO_RESP, ModuleType.SCENE, "HeroGotoResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_HERO_GOTO_NOTICE, ModuleType.SCENE, "HeroGotoNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ENTER_RESP, ModuleType.SCENE, "EnterSceneResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ENTER_NOTICE, ModuleType.SCENE, "EnterSceneNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_LEAVE_RESP, ModuleType.SCENE, "LeaveSceneResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_LEAVE_NOTICE, ModuleType.SCENE, "LeaveSceneNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_MONSTER_LIST_NOTICE, ModuleType.SCENE, "MonsterListNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_MONSTER_DATA_NOTICE, ModuleType.SCENE, "MonsterDataNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ITEM_LIST_NOTICE, ModuleType.SCENE, "ItemListNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ITEM_DISAPPEAR_NOTICE, ModuleType.SCENE, "ItemDisappearNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ITEM_LOCK_RESP, ModuleType.SCENE, "ItemLockResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ITEM_LOCK_NOTIFY, ModuleType.SCENE, "ItemLockNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ITEM_PICK_UP_RESP, ModuleType.SCENE, "ItemPickupResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCENE_ITEM_PICK_UP_NOTIFY, ModuleType.SCENE, "ItemPickupNotify")

	--天梯
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_KING_MATCH_DATA_RESP, ModuleType.LADDER, "KingMatchDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_KING_MATCH_SEASON_RANK_RESP, ModuleType.LADDER, "KingMatchSeasonRankResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_KING_MATCH_SEARCH_TARGET_RESP, ModuleType.LADDER, "KingMatchSearchTargetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_KING_MATCH_TASK_REWARD_RESP, ModuleType.LADDER, "TakeKingMatchTaskRewardResp")

	--组队	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_ANSWER_QUESTION_RESP, ModuleType.TEAM, "TeamDungeonAnswerQuestionResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY, ModuleType.TEAM, "TeamDungeonAnswerQuestionNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_DIALOG_OVER_RESP, ModuleType.TEAM, "TeamDungeonDialogOverResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_NEXT_NPC_RESP, ModuleType.TEAM, "TeamDungeonNextNPCResp")			
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_NEXT_NPC_NOTIFY, ModuleType.TEAM, "TeamDungeonNextNPCNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_BOSS_TASK_START_RESP, ModuleType.TEAM, "TeamDungeonBossTaskStartResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_NEW_TASK_NOTIFY, ModuleType.TEAM, "TeamDungeonNewTaskNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_TASK_DATA_NOTIFY, ModuleType.TEAM, "TeamDungeonTaskDataNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_LOTTERY_RESP, ModuleType.TEAM, "TeamDungeonLotteryResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_MATCH_RESP, ModuleType.TEAM, "TeamDungeonMatchResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_CREATE_TEAM_RESP, ModuleType.TEAM, "TeamDungeonCreateTeamResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_INVITE_RESP, ModuleType.TEAM, "TeamDungeonInviteResp")	
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_INVITE_NOTIFY, ModuleType.TEAM, "TeamDungeonInviteNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_INVITE_RESP, ModuleType.TEAM, "TeamDungeonAcceptInviteResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_INVITE_NOTIFY, ModuleType.TEAM, "TeamDungeonAcceptInviteNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_RECOMMEND_LIST_RESP, ModuleType.TEAM, "TeamDungeonRecommendListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_CANCEL_MATCH_RESP, ModuleType.TEAM, "TeamDungeonCancelMatchResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_LEAVE_RESP, ModuleType.TEAM, "TeamDungeonLeaveResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_LEAVE_NOTIFY, ModuleType.TEAM, "TeamDungeonLeaveNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_DISMISS_NOTIFY, ModuleType.TEAM, "TeamDungeonDismissNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_DIALOG_OVER_NOITY, ModuleType.TEAM, "TeamDungeonDialogOverNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_TASK_RESP, ModuleType.TEAM, "TeamDungeonAcceptTaskResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_ACCEPT_TASK_NOTIFY, ModuleType.TEAM, "TeamDungeonAcceptTaskNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNDEON_TASK_LINK_FINISH_NOTIFY, ModuleType.TEAM, "TeamDungeonTaskLinkFinishNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2S_TEAM_DUNDEON_TASK_REWARD_NOTIFY, ModuleType.TEAM, "TeamDungeonTaskRewardNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_MATCH_NOTIFY, ModuleType.TEAM, "TeamDungeonMatchNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_CANNEL_MATCH_NOTIFY, ModuleType.TEAM, "TeamDungeonCancelMatchNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_TASK_START_RESP, ModuleType.TEAM, "TeamDungeonTaskStartResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_READY_TEAM_DATA_NOTIFY, ModuleType.TEAM, "TeamDungeonReadyTeamDataNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_RANK_RESP, ModuleType.TEAM, "TeamDungeonRankResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_BOSS_REWARD_NOTIFY, ModuleType.TEAM, "TeamDungeonBossRewardNontify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_BOSS_TASK_START_NOTIFY, ModuleType.TEAM, "TeamDungeonBossTaskStartNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_LOTTERY_RECORD_RESP, ModuleType.TEAM, "TeamDungeonLotteryRecordResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_DATA_RESP, ModuleType.TEAM, "TeamDungeonDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_MATCH_TARGET_NUM_NOTIFY, ModuleType.TEAM, "TeamDungeonMatchTargetNumNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TEAM_DUNGEON_SHORTCUT_CHAT_RESP, ModuleType.TEAM, "TeamDungeonShortCutChatResp")
	
	-- 卡牌
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CARD_LIST_RESP, ModuleType.DIVINATION, "CardListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CARD_LEVEL_UP_RESP, ModuleType.DIVINATION, "CardLevelUpResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CARD_BREAK_RESP, ModuleType.DIVINATION, "CardBreakResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CARD_DATA_NOTICE, ModuleType.DIVINATION, "CardDataNotice")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLAYER_ZHANBU_DATA_RESP, ModuleType.DIVINATION, "PlayerZhanbuDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CARD_REFRESH_RESP, ModuleType.DIVINATION, "CardRefreshResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_OPEN_CARD_RESP, ModuleType.DIVINATION, "OpenCardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_RESP, ModuleType.DIVINATION, "LowCardGroupRefreshTimesUpdateResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CELLECTION_GROUP_ACTIVATE_RESP, ModuleType.DIVINATION, "CellectionGroupActivateResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CELLECTION_GROUP_STAR_UP_RESP, ModuleType.DIVINATION, "CellectionGroupStarUpResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CELLECTION_GROUP_RESET_RESP, ModuleType.DIVINATION, "CellectionGroupRestResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_COLLECTION_BOOK_LIST_RESP, ModuleType.DIVINATION, "CollectionBookListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_COLLECTION_EQUIP_CARD_RESP, ModuleType.DIVINATION, "CollectionEquipCardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_COLLECTION_UNLOAD_CARD_RESP, ModuleType.DIVINATION, "CollectionUnloadCardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ZHAN_BU_ACHIEVEMENT_ACIVATE_RESP, ModuleType.DIVINATION, "ZhanBuAchievementAcivateResp")
	--星魂商店
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_STAR_SOUL_SHOP_DATA_RESP, ModuleType.DIVINATION, "StarSoulShopDataResp")

	--圣诞节
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_DATA_RESP, ModuleType.LIMIT_ACTIVITY, "ChristmasDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_BATTLE_ENTER_RESP, ModuleType.LIMIT_ACTIVITY, "ChristmasBattleEnterResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_BATTLE_LEAVE_RESP, ModuleType.LIMIT_ACTIVITY, "ChristmasBattleLeaveResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_ATTACK_BOSS_RESP, ModuleType.LIMIT_ACTIVITY, "ChristmasAttackBossResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_ATTACK_BOSS_NOTIFY, ModuleType.LIMIT_ACTIVITY, "ChristmasAttackBossNotify")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_TAKE_BOSS_REWARD_RESP, ModuleType.LIMIT_ACTIVITY, "ChristmasTakeBossRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CHRISTMAS_BOSS_DEATH_NOTIFY, ModuleType.LIMIT_ACTIVITY, "ChristmasBossDeathNotify")
	
	--在线奖励
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ONLINE_REWARD_DATA_RESP, ModuleType.ONLINE_AWARD, "OnlineRewardDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_ONLINE_REWARD_RESP, ModuleType.ONLINE_AWARD, "TakeOnlineRewardResp")
	
	--七天奖励
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SEVEN_DAY_LOGIN_DATA_RESP, ModuleType.PUSH_SEVEN, "SevenDayLoginDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_SEVEN_DAY_LOGIN_REWARD_RESP, ModuleType.PUSH_SEVEN, "TakeSevenDayLoginRewardResp")
	
	--充值回馈
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RECHARGE_REWARD_DATA_RESP, ModuleType.FEEDBACK, "RechargeRewardDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_RECHARGE_REWARD_RESP, ModuleType.FEEDBACK, "TakeRechargeRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_RECHARGE_EXTRA_REWARD_RESP, ModuleType.FEEDBACK, "TakeRechargeExtraRewardResp")

	--女王
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TOTAL_RECHARGE_REWARD_DATA_RESP, ModuleType.QUEEN, "TotalRechargeRewardDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_TOTAL_RECHARGE_REWARD_RESP, ModuleType.QUEEN, "TakeTotalRechargeRewardResp")

	--自定义
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NEW_SUPER_DISCOUNT_DATA_RESP, ModuleType.CUSTOM_PURCHASE, "SuperDiscountDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NEW_SUPER_DISCOUNT_BUY_REWARD_RESP, ModuleType.CUSTOM_PURCHASE, "SuperDiscountBuyRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_NEW_SUPER_DISCOUNT_ACCUMULATE_REWARD_RESP, ModuleType.CUSTOM_PURCHASE, "SuperDiscountAccumulateRewardResp")

	--扭蛋机
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ND_LOTTERY_RESP, ModuleType.LIMIT_ACTIVITY, "NDLotteryResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_ND_LOTTERY_RECORD_RESP, ModuleType.LIMIT_ACTIVITY, "NDLotteryRecordResp")

	--转盘
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TURNTABLE_DATA_RESP, ModuleType.TURNTABLE, "TurntableDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TURNTABLE_LOTTERY_RESP, ModuleType.TURNTABLE, "TurntableLotteryResp")

	--水晶商店
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CRYSTAL_SHOP_DATA_RESP, ModuleType.CRYSTAL_SHOP, "CrystalShopDataResp")

	--世界等级任务
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_WORLD_LEVEL_TASK_DATA_RESP, ModuleType.PUSH_WORLDLEVEL, "WorldLevelTaskDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_WORLD_LEVEL_TASK_REWARD_RESP, ModuleType.PUSH_WORLDLEVEL, "TakeWorldLevelTaksRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_RESP, ModuleType.PUSH_WORLDLEVEL, "TakeWorldLevelTaskFinalRewardResp")

	--阵营招募
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CAMP_RECRUIT_DATA_RESP, ModuleType.LIMIT_ACTIVITY, "CampRecruitDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CAMP_RECRUIT_RESP, ModuleType.LIMIT_ACTIVITY, "CampRecruitResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CAMP_RECRUIT_SCORE_TASK_REWARD_RESP, ModuleType.LIMIT_ACTIVITY, "CampRecruitScoreTaskRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_CAMP_RECRUIT_RECORD_RESP, ModuleType.LIMIT_ACTIVITY, "CampRecruitRecordResp")
	
	--积分赛
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCORE_MATCH_DATA_RESP, ModuleType.INTEGRAL_RACE, "ScoreMatchDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCORE_MATCH_SELECT_CAMP_RESP, ModuleType.INTEGRAL_RACE, "ScoreMatchSelectCampResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCORE_MATCH_REFRESH_TARGET_RESP, ModuleType.INTEGRAL_RACE, "ScoreMatchRefreshTargetResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCORE_MATCH_BUY_BATTLE_TIMES_RESP, ModuleType.INTEGRAL_RACE, "ScoreMatchBuyBattleTimesResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_SCORE_MATCH_TAKE_TASK_REWARD_RESP, ModuleType.INTEGRAL_RACE, "ScoreMatchTakeTaskRewardResp")
	
	--资源战
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_PLAYER_RESOURCE_WAR_DATA_RESP, ModuleType.RESOURCE_WAR, "PlayerResourceWarDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_ENEMY_LIST_RESP, ModuleType.RESOURCE_WAR, "ResourceWarEnemyListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_BATTLE_RECORD_RESP, ModuleType.RESOURCE_WAR, "ResourceWarBattleRecordResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_GIVE_UP_OCCUPY_RESP, ModuleType.RESOURCE_WAR, "ResourceWarGiveUpOccupyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_KEEP_OCCUPY_RESP, ModuleType.RESOURCE_WAR, "ResourceWarKeepOccupyResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_CHAPTER_DATA_RESP, ModuleType.RESOURCE_WAR, "ResourceWarChapterDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_SAME_RESOURCE_RESP, ModuleType.RESOURCE_WAR, "ResourceWarSameResourceResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_GUILD_RESOURCE_RESP, ModuleType.RESOURCE_WAR, "ResourceWarGuildResourceResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_RESOURCE_WAR_TECHNOLOGY_UPGRADE_RESP, ModuleType.RESOURCE_WAR, "ResourceWarTechnologyUpgradeResp")

	--公会战
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_ROLE_DATA_RESP, ModuleType.UNION, "GuildWarRoleDataResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_OCCUPY_STAR_LIST_RESP, ModuleType.UNION, "GuildWarOccupyStarListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_DECLARE_WAR_STAR_LIST_RESP, ModuleType.UNION, "GuildWarDeclareWarStarListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_STAR_LIST_RESP, ModuleType.UNION, "GuildWarStarListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_DECLARE_WAR_RESP, ModuleType.UNION, "GuildWarDeclareWarResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_ORGANIZE_DEFENSE_RESP, ModuleType.UNION, "GuildWarOrganizeDefenseResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_EVACUATE_RESP, ModuleType.UNION, "GuildWarEvacuateResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_RESP, ModuleType.UNION, "GuildWarBuyHighStarBattleTimesResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_TAKE_STAR_REWARD_RESP, ModuleType.UNION, "GuildWarTakeStarRewardResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_STAR_DETAIL_RESP, ModuleType.UNION, "GuildWarStarDetailResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_TOWER_LIST_RESP, ModuleType.UNION, "GuildWarTowerListResp")
	self:RegistProtocolToModule(Net.ProtocolNo.S2C_GUILD_WAR_STAR_UPDATE_NOTIFY, ModuleType.UNION, "GuildWarStarUpdateNotify")

	--------------------------------------------------------------------------------------------------------------------------------
	--监听所有协议
	--------------------------------------------------------------------------------------------------------------------------------
	local protocol_register_id
	local protocol_callback = function(protocol_data)
		self:__DispatchProtocols(protocol_data)
	end	
	self.__protocol_regist_list = {}
	for _, protocol_no in pairs(Net.ProtocolNo) do
		protocol_register_id = GlobalNetAdapter:RegisterProtocol(protocol_no, protocol_callback)
		table.insert(self.__protocol_regist_list, protocol_register_id)
	end
end

function Modules.ModulesMgr:UnRegistAllProtocols()
	if self.__protocol_regist_list then
		for _, protocol_register_id in ipairs(self.__protocol_regist_list) do
			GlobalNetAdapter:UnRegisterProtocol(protocol_register_id)
		end
		self.__protocol_regist_list = nil
	end	
	self.__protocol_modules_list = nil
end

function Modules.ModulesMgr:RegistProtocolToModule(protocol_no, module_type, module_func_name)
	if not protocol_no then
		LogTool.LogError("[Modules.ModulesMgr:RegistProtocolToModule] 参数protocol_no无效")		
		return
	end
	if not module_type then
		LogTool.LogError("[Modules.ModulesMgr:RegistProtocolToModule] 参数module_type无效")		
		return
	end
	if not module_func_name then
		LogTool.LogError("[Modules.ModulesMgr:RegistProtocolToModule] 参数module_func_name无效")		
		return
	end
	self.__protocol_modules_list = self.__protocol_modules_list or {}
	self.__protocol_modules_list[protocol_no] = self.__protocol_modules_list[protocol_no] or {}
	table.insert(self.__protocol_modules_list[protocol_no], {type = module_type, func_name = module_func_name})
end

function Modules.ModulesMgr:__DispatchProtocols(protocol_data)
	if not self.__protocol_modules_list or not self.__protocol_modules_list[protocol_data.protocol_no] then
		return
	end

	for _, info in ipairs(self.__protocol_modules_list[protocol_data.protocol_no]) do
		self:Call(info.type, info.func_name, protocol_data)
	end
end

