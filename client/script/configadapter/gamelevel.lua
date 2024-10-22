
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.GameLevel = ConfigAdapter.GameLevel or {}

--获取首场战役ID
function ConfigAdapter.GameLevel.GetFirstBattleID(gamelevel_id)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel and config_gamelevel.battle_list then
		return config_gamelevel.battle_list[1]
	end
	return 0
end

--获取相应场次对应的战役ID
function ConfigAdapter.GameLevel.GetBattleID(gamelevel_id, battle_index)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel and config_gamelevel.battle_list then
		return config_gamelevel.battle_list[battle_index] or config_gamelevel.battle_list[1]
	end
	return 0
end

--获取战役数量
function ConfigAdapter.GameLevel.GetBattleNum(gamelevel_id)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel and config_gamelevel.battle_list then
		return #config_gamelevel.battle_list
	end
	return 0
end

--获取战役配置
function ConfigAdapter.GameLevel.GetBattleConfig(gamelevel_id, battle_index)
	local battle_id = ConfigAdapter.GameLevel.GetBattleID(gamelevel_id, battle_index)
	if battle_id then
		return CPPGameLib.GetConfig("Battle", battle_id)
	end
end

--获取战役区域左边界坐标(与场景对象的world_pos处于同一坐标系)
function ConfigAdapter.GameLevel.GetBattleOffset(gamelevel_id, battle_index)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel and config_gamelevel.battle_offset_list then
		battle_index = battle_index or 1
		return config_gamelevel.battle_offset_list[battle_index] or config_gamelevel.battle_offset_list[1], 0
	end
	return 0, 0
end

--获取战役区域扩展
function ConfigAdapter.GameLevel.GetBattleMapExtend(gamelevel_id)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel then
		return config_gamelevel.battle_map_extend or 300
	end
	return 0
end

--获取怪物列表
function ConfigAdapter.GameLevel.GetMonsterList(gamelevel_id)
	local monster_list = {}

	local highest_quality = MathTool.MIN_NUM
	local highest_quality_monster_id
	local total_hp = 0

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel and config_gamelevel.battle_list then
		for battle_index, battle_id in ipairs(config_gamelevel.battle_list) do
			local battle_monster_list = ConfigAdapter.Battle.GetMonsterList(battle_id)
			for _, monster_id in ipairs(battle_monster_list) do
				local monster_info = {}
				monster_info.battle_index = battle_index
				monster_info.monster_id = monster_id
				table.insert(monster_list, monster_info)

				local config_monster = CPPGameLib.GetConfig("Monster", monster_id)
				if config_monster then
					--搜索本关中的BOSS
					if config_monster.quality > highest_quality then
						highest_quality = config_monster.quality
						highest_quality_monster_id = monster_id
					end

					--记录总血量
					total_hp = total_hp + config_monster.blood_max
				end
			end
		end
	end

	return monster_list, highest_quality_monster_id, total_hp
end

--获取出生类型
function ConfigAdapter.GameLevel.GetBirthType(gamelevel_id, obj_type)	
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel then
		if obj_type == Scene.ObjType.HERO then
			return config_gamelevel.hero_birth_type
		elseif obj_type == Scene.ObjType.MONSTER then
			return config_gamelevel.monster_birth_type
		end
	end
	return Macros.Game.FBBirthType.STATIC
end

--获取出生点X偏移
function ConfigAdapter.GameLevel.GetBirthPosX(gamelevel_id, obj_type)
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel then
		if obj_type == Scene.ObjType.HERO then
			return config_gamelevel.hero_birth_x or 0

		elseif obj_type == Scene.ObjType.MONSTER then
			return config_gamelevel.monster_birth_x or 0
		end
	end
end

--获取出生点Y坐标
function ConfigAdapter.GameLevel.GetBirthPosY(gamelevel_id, obj_type)
	obj_type = obj_type or Scene.ObjType.HERO

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if config_gamelevel then
		if obj_type == Scene.ObjType.HERO then
			return config_gamelevel.hero_birth_y

		elseif obj_type == Scene.ObjType.MONSTER then
			return config_gamelevel.monster_birth_y
		end
	end
end

--获取可能掉落的对象列表
function ConfigAdapter.GameLevel.GetDropList(gamelevel_id)
	local drop_list = {}

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if not config_gamelevel then
		return drop_list
	end

	if config_gamelevel.drop_obj_id_list then
		for i, obj_id in ipairs(config_gamelevel.drop_obj_id_list) do
			if obj_id ~= 0 then
				local drop_info = {}
				drop_info.id = obj_id
				drop_info.num = config_gamelevel.drop_obj_num_list[i]
				table.insert(drop_list, drop_info)
			end
		end
	end
	
	return drop_list
end

--获取消耗货币表
function ConfigAdapter.GameLevel.GetCostCurrencyMap(gamelevel_id)
	local cost_currency_map = {}

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if not config_gamelevel then
		return cost_currency_map
	end

	--消耗货币列表
	if config_gamelevel.cost_type_list then
		for i, type in ipairs(config_gamelevel.cost_type_list) do
			cost_currency_map[type] = config_gamelevel.cost_value_list[i]
		end
	end

	return cost_currency_map
end

--获取奖励货币表
function ConfigAdapter.GameLevel.GetRewardCurrencyMap(gamelevel_id)
	local reward_currency_map = {}

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if not config_gamelevel then
		return reward_currency_map
	end

	--奖励货币列表
	if config_gamelevel.reward_currency_type_list then
		for i, type in ipairs(config_gamelevel.reward_currency_type_list) do
			reward_currency_map[type] = config_gamelevel.reward_currency_value_list[i]
		end
	end

	return reward_currency_map
end

--获取奖励道具表
function ConfigAdapter.GameLevel.GetRewardObjList(gamelevel_id)
	local reward_obj_list = {}

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if not config_gamelevel then
		return reward_obj_list
	end

	--奖励道具列表
	if config_gamelevel.reward_item_id_list then
		for i, obj_id in ipairs(config_gamelevel.reward_item_id_list) do
			if obj_id ~= 0 then
				table.insert(reward_obj_list, {id = obj_id, num = config_gamelevel.reward_item_num_min_list[i]})
			end
		end
	end	

	return reward_obj_list
end

--获取可能掉落的对象列表
function ConfigAdapter.GameLevel.GetHeroDungeonDropList(gamelevel_id)
	local drop_list = {}

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", gamelevel_id)
	if not config_gamelevel then
		return drop_list
	end

	if config_gamelevel.reward_item_id_list then
		for i, obj_id in ipairs(config_gamelevel.reward_item_id_list) do
			if obj_id ~= 0 then
				local drop_info = {}
				drop_info.id = obj_id
				if config_gamelevel.reward_item_num_min_list[i] ~= config_gamelevel.reward_item_num_max_list[i] then
					drop_info.min_num = config_gamelevel.reward_item_num_min_list[i]
					drop_info.max_num = config_gamelevel.reward_item_num_max_list[i]
				else
					drop_info.num = config_gamelevel.reward_item_num_min_list[i]
				end
				table.insert(drop_list, drop_info)
			end
		end
	end
	
	return drop_list
end
