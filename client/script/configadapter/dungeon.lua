
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Dungeon = ConfigAdapter.Dungeon or {}

function ConfigAdapter.Dungeon.GetChapterNum(type)
	type = type or Macros.Game.FBType.MAIN_DUNGEON

	local config_name
	if type == Macros.Game.FBType.MAIN_DUNGEON then
		config_name = "MainFuben"

	elseif type == Macros.Game.FBType.HERO_DUNGEON then
		config_name = "HeroFBChapter"

	elseif type == Macros.Game.FBType.DAILY_DUNGEON then
		config_name = "DailyChapter"

	elseif type == Macros.Game.FBType.ELITE_DUNGEON then
		config_name = "EliteFBChapter"
	end

	local config = CPPGameLib.GetConfig(config_name, nil, nil, true)
	return #config
end

function ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, type, show_error)
	type = type or Macros.Game.FBType.MAIN_DUNGEON

	if type == Macros.Game.FBType.MAIN_DUNGEON then
		return CPPGameLib.GetConfig("MainFuben", chapter_index, show_error)

	elseif type == Macros.Game.FBType.HERO_DUNGEON then
		return CPPGameLib.GetConfig("HeroFBChapter", chapter_index, show_error)

	elseif type == Macros.Game.FBType.DAILY_DUNGEON then
		return CPPGameLib.GetConfig("DailyChapter", chapter_index, show_error)

	elseif type == Macros.Game.FBType.ELITE_DUNGEON then
		return CPPGameLib.GetConfig("EliteFBChapter", chapter_index, show_error)
	end
end

function ConfigAdapter.Dungeon.GetGameLevelID(chapter_index, gamelevel_index, type)
	type = type or Macros.Game.FBType.MAIN_DUNGEON

	if type == Macros.Game.FBType.MAIN_DUNGEON then
		config_name = "MainFuben"

	elseif type == Macros.Game.FBType.HERO_DUNGEON then
		config_name = "HeroFBChapter"

	elseif type == Macros.Game.FBType.DAILY_DUNGEON then
		config_name = "DailyChapter"

	elseif type == Macros.Game.FBType.ELITE_DUNGEON then
		config_name = "EliteFBChapter"
	end

	local config_chapter = CPPGameLib.GetConfig(config_name, chapter_index)
	if config_chapter and config_chapter.gamelevel_list then
		return config_chapter.gamelevel_list[gamelevel_index] or 0
	end

	return 0
end

function ConfigAdapter.Dungeon.GetGameLevelList(chapter_index, type)	
	type = type or Macros.Game.FBType.MAIN_DUNGEON

	if type == Macros.Game.FBType.MAIN_DUNGEON then
		config_name = "MainFuben"

	elseif type == Macros.Game.FBType.HERO_DUNGEON then
		config_name = "HeroFBChapter"

	elseif type == Macros.Game.FBType.DAILY_DUNGEON then
		config_name = "DailyChapter"
	elseif type == Macros.Game.FBType.ELITE_DUNGEON then
		config_name = "EliteFBChapter"
	end

	local config_chapter = CPPGameLib.GetConfig(config_name, chapter_index)
	if config_chapter then
		return config_chapter.gamelevel_list
	end
end

--获取章节背景ID
function ConfigAdapter.Dungeon.GetChapterBGID(chapter_index, type)
	local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(chapter_index, type)
	return config_chapter and config_chapter.chapter_bg_id or 0
end

--获取新手副本关联的关卡ID
function ConfigAdapter.Dungeon.GetLeadAssociatedGameLevelID()
	local config_lead_fb = CPPGameLib.GetConfig("LeadFB", 1)	
	return config_lead_fb.associated_gamelevel_id
end

--获取新手副本关卡ID
function ConfigAdapter.Dungeon.GetLeadGameLevelID()
	local config_lead_fb = CPPGameLib.GetConfig("LeadFB", 1)	
	return config_lead_fb.gamelevel_id
end

--获取新手副本战斗英雄列表
function ConfigAdapter.Dungeon.GetLeadBattleHeroList()
	local battle_hero_list = {}

	local key, prop_type, prop_value
	local config_hero
	local config_lead_fb = CPPGameLib.GetConfig("LeadFB", 1)
	for i, hero_id in ipairs(config_lead_fb.hero_list) do
		config_hero = CPPGameLib.GetConfig("Hero", hero_id)

		local hero_info = {}
		hero_info.info_id = hero_id
		hero_info.level = config_hero.init_level
		hero_info.quality_level = config_hero.init_quality
		hero_info.quality_stage = config_hero.init_quality_stage or 0
		local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_id, hero_info.quality_level, hero_info.level ,hero_info.quality_stage)
		for _, prop_type in pairs(Macros.Game.BattleProp) do
			prop_key = Macros.Game.BattlePropKey[prop_type]
			if not basic_prop_map or not basic_prop_map[prop_type] then
				hero_info[prop_key] = 0
			else
				hero_info[prop_key] = basic_prop_map[prop_type]
			end
		end

		key = string.format("hero%d_prop_list", i)
		if config_lead_fb[key] then
			local prop_key
			for _, prop_info in ipairs(config_lead_fb[key]) do
				prop_type = prop_info[1]
				prop_value = prop_info[2]
				prop_key = Macros.Game.BattlePropKey[prop_type]
				hero_info[prop_key] = hero_info[prop_key] or 0
				hero_info[prop_key] = hero_info[prop_key] + prop_value
			end
		end

		table.insert(battle_hero_list, hero_info)
	end

	return battle_hero_list
end

--获取新手副本头像
function ConfigAdapter.Dungeon.GetLeadBattleHeadID()
	local config_lead_fb = CPPGameLib.GetConfig("LeadFB", 1)
	return config_lead_fb.hero_list[1]
end

--获取新手副本敌我名字
function ConfigAdapter.Dungeon.GetLeadBattleNames()
	local config_lead_fb = CPPGameLib.GetConfig("LeadFB", 1)
	return config_lead_fb.my_name, config_lead_fb.enemy_name	
end

--获取日常副本奖励上下文
function ConfigAdapter.Dungeon.GetDailyDungeonRewardContext(chapter_index, gamelevel_index, context, sort_func, compare_func)
	if not context then
		return
	end
	local config_chapter_reward = CPPGameLib.GetConfig("DailyFBReward", chapter_index)
	if not config_chapter_reward then
		return
	end
	local config_gamelevel_reward = config_chapter_reward[gamelevel_index]
	if not config_gamelevel_reward then
		return
	end

	ConfigAdapter.Dungeon.DailyDungeonRewardContextMap = ConfigAdapter.Dungeon.DailyDungeonRewardContextMap or {}
	ConfigAdapter.Dungeon.DailyDungeonRewardContextMap[chapter_index] = ConfigAdapter.Dungeon.DailyDungeonRewardContextMap[chapter_index] or {}	
	ConfigAdapter.Dungeon.DailyDungeonRewardContextMap[chapter_index][gamelevel_index] = ConfigAdapter.Dungeon.DailyDungeonRewardContextMap[chapter_index][gamelevel_index] or {}

	local context_list = ConfigAdapter.Dungeon.DailyDungeonRewardContextMap[chapter_index][gamelevel_index]
	if #context_list <= 0 then
		for context_var, _ in pairs(config_gamelevel_reward) do
			if type(context_var) == "number" then
				table.insert(context_list, context_var)
			end
		end
		table.sort(context_list, sort_func or SortTools.ItemUpperSorter())
	end

	compare_func = compare_func or function(context, context_var)
		return context >= context_var		
	end

	for _, context_var in ipairs(context_list) do
		if compare_func(context, context_var) then
			return context_var
		end
	end
end
	
--获取协助战斗英雄列表
function ConfigAdapter.Dungeon.GetAssistBattleHeroList(gamelevel_id)
	local config_assist_hero = CPPGameLib.GetConfig("AssistHero", gamelevel_id, false)
	if not config_assist_hero then
		return
	end

	local battle_hero_list = {}

	local key, prop_type, prop_value
	local config_hero
	for i, hero_id in ipairs(config_assist_hero.hero_list) do
		config_hero = CPPGameLib.GetConfig("Hero", hero_id)

		local hero_info = {}
		hero_info.info_id = hero_id
		hero_info.level = config_hero.init_level
		hero_info.quality_level = config_hero.init_quality
		hero_info.say_info = config_assist_hero.say_list and config_assist_hero.say_list[i] 
		hero_info.is_assist = true
		hero_info.quality_stage = config_hero.init_quality_stage or 0

		local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
		for _, prop_type in pairs(Macros.Game.BattleProp) do
			prop_key = Macros.Game.BattlePropKey[prop_type]
			if not basic_prop_map or not basic_prop_map[prop_type] then
				hero_info[prop_key] = 0
			else
				hero_info[prop_key] = basic_prop_map[prop_type]
			end
		end

		key = string.format("hero%d_prop_list", i)
		if config_assist_hero[key] then
			local prop_key
			for _, prop_info in ipairs(config_assist_hero[key]) do
				prop_type = prop_info[1]
				prop_value = prop_info[2]
				prop_key = Macros.Game.BattlePropKey[prop_type]
				hero_info[prop_key] = hero_info[prop_key] or 0
				hero_info[prop_key] = hero_info[prop_key] + prop_value
			end
		end

		table.insert(battle_hero_list, hero_info)
	end

	return battle_hero_list
end