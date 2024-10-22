
--[[
%% @module: 模块事件注册
%% @author: swordman sue
%% @created: 2016-07-14
--]]

--注册各模块所需事件
function Modules.ModulesMgr:RegistAllEvents()
	--时间同步
	self:__RegistEventToModule(Macros.Event.Net.SERVER_CONNECTED, ModuleType.TIME_SYNC, "HandleServerConnected")
	self:__RegistEventToModule(Macros.Event.Net.SERVER_DISCONNECTED, ModuleType.TIME_SYNC, "HandleServerDisconnected")

	--登陆
	self:__RegistEventToModule(Macros.Event.Net.SERVER_CONNECTED, ModuleType.LOGIN, "HandleServerConnected")
	self:__RegistEventToModule(Macros.Event.Net.SERVER_DISCONNECTED, ModuleType.LOGIN, "HandleServerDisconnected")
	self:__RegistEventToModule(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, ModuleType.LOGIN, "HandleErrorNotify")	

	--主界面
	self:__RegistEventToModule(Macros.Event.View.ONLY_CITY_VIEW_OPEN, ModuleType.MAIN_CITY, "HandleOnlyCityViewOpen")
	self:__RegistEventToModule(Macros.Event.View.NOT_ONLY_CITY_VIEW_OPEN, ModuleType.MAIN_CITY, "HandleNotOnlyCityViewOpen")
	self:__RegistEventToModule(Macros.Event.RoleModule.LEVEL_CHANGE, ModuleType.MAIN_CITY, "HandleRoleLevelChange")
	self:__RegistEventToModule(Macros.Event.RoleModule.PROP_CHANGE, ModuleType.MAIN_CITY, "HandleRolePropChange")
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_LINEUP_UPDATE, ModuleType.MAIN_CITY, "HandleHeroLineupUpdate")
	self:__RegistEventToModule(Macros.Event.RoleModule.ROLE_NAME_CHANGED, ModuleType.MAIN_CITY, "HandleRoleNameChanged")
	self:__RegistEventToModule(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_TIME_RESP, ModuleType.MAIN_CITY, "HandleLimitActivityRefreshTime")	
	self:__RegistEventToModule(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_END, ModuleType.MAIN_CITY, "HandleLimitActivityEnd")	
	self:__RegistEventToModule(Macros.Event.View.VIEW_CLOSE, ModuleType.MAIN_CITY, "HandleViewCloseEvent")	

	--副本
	self:__RegistEventToModule(Macros.Event.BattleModule.BATTLE_END_RETURN_RELEVANT_VIEW, ModuleType.DUNGEON, "HandleBattleEndReturnRelevantView")	
	self:__RegistEventToModule(Macros.Event.View.MAINCITY_VISIBLE, ModuleType.DUNGEON, "HandleMaincityVisibleEvent")	

	--新手指引
	self:__RegistEventToModule(Macros.Event.BattleModule.BATTLE_START, ModuleType.LEAD, "HandleBattleStart")
	self:__RegistEventToModule(Macros.Event.SceneObj.MONSTER_DEAD, ModuleType.LEAD, "HandleMonsterDead")
	self:__RegistEventToModule(Macros.Event.RoleModule.ROLE_NAME_CHANGED, ModuleType.LEAD, "HandleRoleNameChanged")
	self:__RegistEventToModule(Macros.Event.DungeonModule.DUNGEON_PASS_GAME_LEVEL, ModuleType.LEAD, "HandlePassGameLevel")
	self:__RegistEventToModule(Macros.Event.DungeonModule.TOWER_GAMELEVEL_PASSED_FIRST_TIME, ModuleType.LEAD, "HandlePassTowerGameLevelFirstTime")
	self:__RegistEventToModule(Macros.Event.SystemModule.SYSTEM_OPENED, ModuleType.LEAD, "HandleSystemOpened")
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_LINEUP_POS_OPENED, ModuleType.LEAD, "HandleHeroLineupPosOpened")
	self:__RegistEventToModule(Macros.Event.HeroModule.REINFORCEMENT_LINEUP_POS_OPENED, ModuleType.LEAD, "HandleReinforcementLineupPosOpened")
	self:__RegistEventToModule(Macros.Event.SceneObj.COMMON_ANGER_CHANGE, ModuleType.LEAD, "HandleCommonAngerChange")
	self:__RegistEventToModule(Macros.Event.BattleModule.BATTLE_END, ModuleType.LEAD, "HandleBattleEnd")
	self:__RegistEventToModule(Macros.Event.DungeonModule.HERO_ADVENT_POPUP, ModuleType.LEAD, "HandleHeroAdventViewOpened")
	self:__RegistEventToModule(Macros.Event.ChasmModule.CHASM_FIRST_LOSE_LEAD, ModuleType.LEAD, "HandleChasmFirstChallengeLose")
	self:__RegistEventToModule(Macros.Event.Net.ON_SEND_PROTOCOL, ModuleType.LEAD, "HandleOnSendProtocol")
	self:__RegistEventToModule(Macros.Event.View.VIEW_OPEN, ModuleType.LEAD, "HandleEventForViewOpen")
	self:__RegistEventToModule(Macros.Event.View.TAB_VIEW_OPEN, ModuleType.LEAD, "HandleEventForTabViewOpen")
	self:__RegistEventToModule(Macros.Event.View.TEMPLATE_OPEN, ModuleType.LEAD, "HandleEventForTemplateOpen")
	self:__RegistEventToModule(Macros.Event.View.MAINCITY_VISIBLE, ModuleType.LEAD, "HandleMaincityVisibleEvent")
	self:__RegistEventToModule(Macros.Event.View.VIEW_EFFECT_FINISHED, ModuleType.LEAD, "HandleEventForViewOrTemplateEffectFinished")
	self:__RegistEventToModule(Macros.Event.View.TEMPLATE_EFFECT_FINISHED, ModuleType.LEAD, "HandleEventForViewOrTemplateEffectFinished")

	--剧情	
	self:__RegistEventToModule(Macros.Event.BattleModule.BATTLE_START, ModuleType.STORY, "HandleBattleStart")
	self:__RegistEventToModule(Macros.Event.SceneObj.MONSTER_DEAD, ModuleType.STORY, "HandleMonsterDead")
	self:__RegistEventToModule(Macros.Event.RoleModule.ROLE_NAME_CHANGED, ModuleType.STORY, "HandleRoleNameChanged")
	self:__RegistEventToModule(Macros.Event.DungeonModule.DUNGEON_PASS_GAME_LEVEL, ModuleType.STORY, "HandlePassGameLevel")
	self:__RegistEventToModule(Macros.Event.DungeonModule.TOWER_GAMELEVEL_PASSED_FIRST_TIME, ModuleType.STORY, "HandlePassTowerGameLevelFirstTime")
	self:__RegistEventToModule(Macros.Event.SystemModule.SYSTEM_OPENED, ModuleType.STORY, "HandleSystemOpened")
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_LINEUP_POS_OPENED, ModuleType.STORY, "HandleHeroLineupPosOpened")
	self:__RegistEventToModule(Macros.Event.HeroModule.REINFORCEMENT_LINEUP_POS_OPENED, ModuleType.STORY, "HandleReinforcementLineupPosOpened")
	self:__RegistEventToModule(Macros.Event.SceneObj.COMMON_ANGER_CHANGE, ModuleType.STORY, "HandleCommonAngerChange")
	self:__RegistEventToModule(Macros.Event.DungeonModule.HERO_ADVENT_POPUP, ModuleType.STORY, "HandleHeroAdventViewOpened")
	self:__RegistEventToModule(Macros.Event.ChasmModule.CHASM_FIRST_LOSE_LEAD, ModuleType.STORY, "HandleChasmFirstChallengeLose")
	self:__RegistEventToModule(Macros.Event.BattleModule.BATTLE_END, ModuleType.STORY, "HandleBattleEnd")

	--英雄
	self:__RegistEventToModule(Macros.Event.RoleModule.LEVEL_CHANGE, ModuleType.HERO, "HandleRoleLevelChange")	

	--英雄商店
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.HERO_SHOP, "HandleNewDayNotify")

	--战斗
	self:__RegistEventToModule(Macros.Event.RoleModule.PROP_CHANGE, ModuleType.BATTLE, "HandleRolePropChange")

	--特殊逻辑管理器
	self:__RegistEventToModule(Macros.Event.View.NO_VIEW_OPEN, ModuleType.SP_LOGIC_MGR, "HandleNoViewOpen")

	--红点管理器
	self:__RegistEventToModule(Macros.Event.RoleModule.LEVEL_CHANGE, ModuleType.RED_MGR, "HandleRoleLevelChange")	
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.RED_MGR, "HandleNewDayNotify")
	self:__RegistEventToModule(Macros.Event.View.MAINCITY_VISIBLE, ModuleType.RED_MGR, "HandleMaincityVisibleEvent")	
	self:__RegistEventToModule(Macros.Event.View.VIEW_CLOSE, ModuleType.RED_MGR, "HandleViewCloseEvent")	
	self:__RegistEventToModule(Macros.Event.EquipModule.EQUIP_ADD_EQUIP, ModuleType.RED_MGR, "HandleAddEquipEvent")	
	self:__RegistEventToModule(Macros.Event.EquipModule.EQUIP_UP_EQUIP, ModuleType.RED_MGR, "HandleUpdateEquipEvent")	
	self:__RegistEventToModule(Macros.Event.EquipModule.EQUIP_REMOVE_EQUIP, ModuleType.RED_MGR, "HandleRemoveEquipEvent")	
	self:__RegistEventToModule(Macros.Event.EquipModule.EQUIP_CHANGE_EQUIP, ModuleType.RED_MGR, "HandleEquipChangeEvent")	
	self:__RegistEventToModule(Macros.Event.EquipModule.EQUIP_UP_DEBRIS, ModuleType.RED_MGR, "HandleUpdateEquipDebris")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_ADD, ModuleType.RED_MGR, "HandleAddHeroEvent")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_UPDATE, ModuleType.RED_MGR, "HandleUpdateHeroEvent")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_REMOVE, ModuleType.RED_MGR, "HandleRemoveHeroEvent")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_CHANGE, ModuleType.RED_MGR, "HandleChangeHeroEvent")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_UP_DEBRIS, ModuleType.RED_MGR, "HandleUpdateHeroDebris")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_ON_OR_OFF_FIGHT, ModuleType.RED_MGR, "HandleHeroOnOrOffFightEvent")	
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_LINEUP_UPDATE, ModuleType.RED_MGR, "HandleHeroLineupUpdateEvent")	
	self:__RegistEventToModule(Macros.Event.TreasureModule.TREASURE_CHANGE, ModuleType.RED_MGR, "HandleChangeTreasure")	
	self:__RegistEventToModule(Macros.Event.DungeonModule.MAIN_DUNGEON_DATA_RESP, ModuleType.RED_MGR, "HandleMainDungeonDataResp")	
	self:__RegistEventToModule(Macros.Event.DungeonModule.TAKE_MAIN_FB_CHAPTER_REWARD, ModuleType.RED_MGR, "HandleTakeMainDungeonChapterReward")	
	self:__RegistEventToModule(Macros.Event.DungeonModule.DAILY_DUNGEON_DATA_RESP, ModuleType.RED_MGR, "HandleDailyDungeonDataResp")	
	self:__RegistEventToModule(Macros.Event.Net.SERVER_DISCONNECTED, ModuleType.RED_MGR, "HandleServerDisconnected")
	self:__RegistEventToModule(Macros.Event.SystemModule.SYSTEM_OPENED, ModuleType.RED_MGR, "HandleSystemOpened")

	--公告
	self:__RegistEventToModule(Macros.Event.HeroModule.HERO_RECRUIT_NOTICE, ModuleType.NOTICE, "HandleHeroRecruitNotice")	
	self:__RegistEventToModule(Macros.Event.GameState.LOGIN_ACCOUNT_SUCCESS, ModuleType.NOTICE, "HandleLoginAccountSuccess")	
	
	--VIP
	self:__RegistEventToModule(Macros.Event.VIPModule.VIP_BUY, ModuleType.VIP, "ShopBuyRecordRequestRqsp")

	--组队
	self:__RegistEventToModule(Macros.Event.SceneObj.MT_MONSTER_KILLED, ModuleType.TEAM, "SetTeamDungeonFightMonsterNum")
	self:__RegistEventToModule(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_BATTLE_START, ModuleType.TEAM, "SetTeamDungeonBossBattleStartData")
	self:__RegistEventToModule(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, ModuleType.TEAM, "HandleErrorNotify")

	--商城模块
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.SHOP, "HandleNewDayNotify")

	--活动
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.ACTIVITY, "HandleNewDayNotify")
	self:__RegistEventToModule(Macros.Event.RoleModule.PROP_CHANGE, ModuleType.ACTIVITY, "HandleRolePropChange")
	self:__RegistEventToModule(Macros.Event.RoleModule.LEVEL_CHANGE, ModuleType.ACTIVITY, "HandleRoleLevelChange")

	--七天活动
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.SEVENDAYS, "HandleNewDayNotify")

	--直购特惠
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.DIRECT_BUY_SHOP, "HandleNewDayNotify")

	--限时副本汇总
	self:__RegistEventToModule(Macros.Event.GameState.LOGIN_ACCOUNT_SUCCESS, ModuleType.LIMIT_DUNGEON, "HandleLoginAccountSuccess")	
	self:__RegistEventToModule(Macros.Event.RoleModule.LEVEL_CHANGE, ModuleType.LIMIT_DUNGEON, "HandleRoleLevelChange")	
	self:__RegistEventToModule(Macros.Event.View.MAINCITY_VISIBLE, ModuleType.LIMIT_DUNGEON, "HandleMaincityVisibleEvent")	

	--叛军
	self:__RegistEventToModule(Macros.Event.SundryModule.NEW_DAY_NOTIFY, ModuleType.REBEL, "HandleNewDayNotify")

	--怪物攻城
	self:__RegistEventToModule(Macros.Event.View.MAINCITY_VISIBLE, ModuleType.MONSTER_STORMCASTLE, "HandleMaincityVisibleEvent")	
	self:__RegistEventToModule(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_TIME_RESP, ModuleType.MONSTER_STORMCASTLE, "HandleRefreshActivityTime")	

	self:__RegistEventToModule(Macros.Event.GameState.LOGIN_ACCOUNT_SUCCESS, ModuleType.NOTICE, "ActivityNoticeReq")

	--------------------------------------------------------------------------------------------------------------------------------
	--监听所有事件
	--------------------------------------------------------------------------------------------------------------------------------
	local event_callback = function(_event_name, ...)
		self:__DispatchEvents(_event_name, ...)
	end
	for _, sub_event in pairs(Macros.Event) do
		for _, event_name in pairs(sub_event) do
			self:BindGlobalEvent(event_name, event_callback)
		end
	end
end

function Modules.ModulesMgr:UnRegistAllEvents()
	EventAndTimerWrapper.UnRegistAllEvents(self)

	self.__event_modules_list = nil
end

function Modules.ModulesMgr:__RegistEventToModule(event_name, module_type, module_func_name)
	if not event_name then
		LogTool.LogError("[Modules.ModulesMgr:__RegistEventToModule] 参数event_name无效")		
		return
	end
	if not module_type then
		LogTool.LogError("[Modules.ModulesMgr:__RegistEventToModule] 参数module_type无效")		
		return
	end
	if not module_func_name then
		LogTool.LogError("[Modules.ModulesMgr:__RegistEventToModule] 参数module_func_name无效")		
		return
	end

	self.__event_modules_list = self.__event_modules_list or {}
	self.__event_modules_list[event_name] = self.__event_modules_list[event_name] or {}
	table.insert(self.__event_modules_list[event_name], {type = module_type, func_name = module_func_name})
end

function Modules.ModulesMgr:__DispatchEvents(event_name, ...)
	if not self.__event_modules_list or not self.__event_modules_list[event_name] then
		return
	end

	for _, info in ipairs(self.__event_modules_list[event_name]) do
		self:Call(info.type, info.func_name, ...)
	end
end
