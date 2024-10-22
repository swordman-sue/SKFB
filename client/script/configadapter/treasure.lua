
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Treasure = ConfigAdapter.Treasure or {}

--经验圣器列表
local sorted_exp_treasure_info_list

--获取ID最小的经验圣器配置
function ConfigAdapter.Treasure.GetMinTreasureInfoByEXPType()
	if not sorted_exp_treasure_info_list then
		sorted_exp_treasure_info_list = {}

		for info_id, config_info in pairs(CPPGameLib.GetConfig("Treasure", nil, nil, true)) do
			if config_info.type == Macros.Game.TreasureType.EXP then
				config_info.id = info_id
				table.insert(sorted_exp_treasure_info_list, config_info)
			end
		end

		table.sort(sorted_exp_treasure_info_list, SortTools.KeyLowerSorter("id"))
	end

	return sorted_exp_treasure_info_list[1]
end

--获取圣器作为强化材料的消耗类型
function ConfigAdapter.Treasure.GetStrengthenMaterialCostInfo(treasure_info)
	if not treasure_info then
		return
	end

    local config_strengthen = CPPGameLib.GetConfig("TreasureStrengthen", treasure_info.quality)
    if not config_strengthen then
    	return
    end
    local config_strengthen_lev_info = config_strengthen.level_list[treasure_info.strengthen_level]
    if config_strengthen_lev_info then
    	return config_strengthen_lev_info.cost_type_list[1]
    end
end

--获取圣器强化等级信息
function ConfigAdapter.Treasure.GetStrengthenLevInfo(quality, strengthen_level)
    local config_strengthen_info = CPPGameLib.GetConfig("TreasureStrengthen", quality)
    if not config_strengthen_info then
        return
    end

    return config_strengthen_info.level_list[strengthen_level]
end

--获取圣器初始、强化加成
function ConfigAdapter.Treasure.GetStrengthenPlusPropMap(treasure_info)
	if not treasure_info then
		return
	end	

	local config_treasure = CPPGameLib.GetConfig("Treasure", treasure_info.info_id , false )
	if not config_treasure then
		return
	end

	local plus_prop_map
	local add_prop_info, prop_type
	for index, prop_info in ipairs(config_treasure.strengthen_init_pro_list) do
		add_prop_info = config_treasure.strengthen_add_pro_list[index]
		prop_type = prop_info[1]
		plus_prop_map = plus_prop_map or {}
		plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
		plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_info[2] + add_prop_info[2] * (treasure_info.strengthen_level - 1)
	end
	return plus_prop_map
end

--获取圣器精炼加成
function ConfigAdapter.Treasure.GetRefinePlusPropMap(treasure_info)
	if not treasure_info then
		return
	end	

	local config_treasure = CPPGameLib.GetConfig("Treasure", treasure_info.info_id , false )
	if not config_treasure then
		return
	end

	local refine_pro_type_list = config_treasure.refine_pro_type_list
	local refine_pro_value_list = config_treasure.refine_pro_value_list

	local plus_prop_map
	for i, v in ipairs(refine_pro_type_list) do
		plus_prop_map = plus_prop_map or {}
		plus_prop_map[v] = plus_prop_map[v] or 0
		plus_prop_map[v] = plus_prop_map[v] + refine_pro_value_list[i] * treasure_info.refine_level
	end
	return plus_prop_map
end

--获取圣器强化、精炼加成
function ConfigAdapter.Treasure.GetPlusPropMap(lineup_treasure_map)
	local strengthen_plus_prop_map, refine_plus_prop_map, plus_prop_map

	for _, treasure_info in pairs(lineup_treasure_map) do
		--强化加成
		strengthen_plus_prop_map = ConfigAdapter.Treasure.GetStrengthenPlusPropMap(treasure_info)
		for prop_type, prop_value in pairs(strengthen_plus_prop_map) do
			plus_prop_map = plus_prop_map or {}
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value		
		end

		--精炼加成
		refine_plus_prop_map = ConfigAdapter.Treasure.GetRefinePlusPropMap(treasure_info)
		for prop_type, prop_value in pairs(refine_plus_prop_map) do
			plus_prop_map = plus_prop_map or {}
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value		
		end
	end

	return plus_prop_map
end