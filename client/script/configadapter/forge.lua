
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Forge = ConfigAdapter.Forge or {}

--获取装备强化大师加成
function ConfigAdapter.Forge.GetEquipStrengthenPlusPropList(strengthen_level)
	if not ConfigAdapter.Forge.EquipmentStrengthenForgeList then
		ConfigAdapter.Forge.EquipmentStrengthenForgeList = {}
		for _, forge_info in pairs(CPPGameLib.GetConfig("EquipmentStrengthenForge", nil, nil , true )) do
			table.insert(ConfigAdapter.Forge.EquipmentStrengthenForgeList, forge_info)
		end
		table.sort(ConfigAdapter.Forge.EquipmentStrengthenForgeList, SortTools.KeyLowerSorter("strengthen_level"))
	end

	local cur_forge_info
	for _, forge_info in ipairs(ConfigAdapter.Forge.EquipmentStrengthenForgeList) do
		if forge_info.strengthen_level > strengthen_level then
			break
		end
		cur_forge_info = forge_info
	end
	if cur_forge_info then
		return cur_forge_info.pro_type_list, cur_forge_info.pro_value_list , cur_forge_info.showLv , cur_forge_info
	end
end

--获取装备精炼大师加成
function ConfigAdapter.Forge.GetEquipRefinePlusPropList(refine_level)
	if not ConfigAdapter.Forge.EquipmentRefineForgeList then
		ConfigAdapter.Forge.EquipmentRefineForgeList = {}
		for _, forge_info in pairs(CPPGameLib.GetConfig("EquipmentRefineForge", nil, nil , true )) do
			table.insert(ConfigAdapter.Forge.EquipmentRefineForgeList, forge_info)
		end
		table.sort(ConfigAdapter.Forge.EquipmentRefineForgeList, SortTools.KeyLowerSorter("refine_level"))
	end

	local cur_forge_info
	for _, forge_info in ipairs(ConfigAdapter.Forge.EquipmentRefineForgeList) do
		if forge_info.refine_level > refine_level then
			break
		end
		cur_forge_info = forge_info
	end
	if cur_forge_info then
		return cur_forge_info.pro_type_list, cur_forge_info.pro_value_list , cur_forge_info.showLv , cur_forge_info
	end	
end

--获取圣器强化大师加成
function ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList(strengthen_level)
	if not ConfigAdapter.Forge.TreasureStrengthenForgeList then
		ConfigAdapter.Forge.TreasureStrengthenForgeList = {}
		for _, forge_info in pairs(CPPGameLib.GetConfig("TreasureStrengthenForge", nil, nil , true )) do
			table.insert(ConfigAdapter.Forge.TreasureStrengthenForgeList, forge_info)
		end
		table.sort(ConfigAdapter.Forge.TreasureStrengthenForgeList, SortTools.KeyLowerSorter("strengthen_level"))
	end

	local cur_forge_info
	for _, forge_info in ipairs(ConfigAdapter.Forge.TreasureStrengthenForgeList) do
		if forge_info.strengthen_level > strengthen_level then
			break
		end
		cur_forge_info = forge_info
	end
	if cur_forge_info then
		return cur_forge_info.pro_type_list, cur_forge_info.pro_value_list , cur_forge_info.showLv , cur_forge_info
	end
end

--获取圣器精炼大师加成
function ConfigAdapter.Forge.GetTreasureRefinePlusPropList(refine_level)
	if not ConfigAdapter.Forge.TreasureRefineForgeList then
		ConfigAdapter.Forge.TreasureRefineForgeList = {}
		for _, forge_info in pairs(CPPGameLib.GetConfig("TreasureRefineForge", nil, nil , true )) do
			table.insert(ConfigAdapter.Forge.TreasureRefineForgeList, forge_info)
		end
		table.sort(ConfigAdapter.Forge.TreasureRefineForgeList, SortTools.KeyLowerSorter("refine_level"))
	end

	local cur_forge_info
	for _, forge_info in ipairs(ConfigAdapter.Forge.TreasureRefineForgeList) do
		if forge_info.refine_level > refine_level then
			break
		end
		cur_forge_info = forge_info
	end
	if cur_forge_info then
		return cur_forge_info.pro_type_list, cur_forge_info.pro_value_list , cur_forge_info.showLv , cur_forge_info
	end	
end

--获取装备锻造大师属性加成
function ConfigAdapter.Forge.GetEquipPlusPropMap(lineup_equip_map)
	if CPPGameLib.GetTableLength(lineup_equip_map) < 4 then
		return
	end

	local equip, equip_strengthen_level_min, equip_refine_level_min
	for _, equip in pairs(lineup_equip_map) do
		--强化等级下限
		if not equip_strengthen_level_min or equip_strengthen_level_min > equip.strengthen_level then
			equip_strengthen_level_min = equip.strengthen_level
		end
		--精炼等级下限
		if not equip_refine_level_min or equip_refine_level_min > equip.refine_level then
			equip_refine_level_min = equip.refine_level
		end
	end

	local plus_prop_map = {}
	local pro_type_list, pro_value_list,equip_strengthen_level,equip_refine_level

	--强化大师加成
	if equip_strengthen_level_min then
		pro_type_list, pro_value_list,equip_strengthen_level = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList(equip_strengthen_level_min)
		for pro_index, pro_type in ipairs(pro_type_list) do			
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + (pro_value_list[pro_index] or 0)
		end
	end

	--精炼大师加成
	if equip_refine_level_min then
		pro_type_list, pro_value_list,equip_refine_level = ConfigAdapter.Forge.GetEquipRefinePlusPropList(equip_refine_level_min)
		for pro_index, pro_type in ipairs(pro_type_list) do			
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + (pro_value_list[pro_index] or 0)
		end
	end

	return plus_prop_map, equip_strengthen_level, equip_refine_level
end

--获取圣器锻造大师属性加成
function ConfigAdapter.Forge.GetTreasurePlusPropMap(lineup_treasure_map)
	if CPPGameLib.GetTableLength(lineup_treasure_map) < 2 then
		return
	end

	local treasure, treasure_strengthen_level_min, treasure_refine_level_min
	for _, treasure in pairs(lineup_treasure_map) do
		--强化等级下限
		if not treasure_strengthen_level_min or treasure_strengthen_level_min > treasure.strengthen_level then
			treasure_strengthen_level_min = treasure.strengthen_level
		end
		--精炼等级下限
		if not treasure_refine_level_min or treasure_refine_level_min > treasure.refine_level then
			treasure_refine_level_min = treasure.refine_level
		end
	end

	local plus_prop_map = {}
	local pro_type_list, pro_value_list,treasure_strengthen_level, treasure_refine_level

	--强化大师加成
	if treasure_strengthen_level_min then
		pro_type_list, pro_value_list,treasure_strengthen_level = ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList(treasure_strengthen_level_min)
		for pro_index, pro_type in ipairs(pro_type_list) do			
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + (pro_value_list[pro_index] or 0)
		end
	end

	--精炼大师加成
	if treasure_refine_level_min then
		pro_type_list, pro_value_list,treasure_refine_level = ConfigAdapter.Forge.GetTreasureRefinePlusPropList(treasure_refine_level_min)
		for pro_index, pro_type in ipairs(pro_type_list) do			
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + (pro_value_list[pro_index] or 0)
		end
	end	

	return plus_prop_map, treasure_strengthen_level, treasure_refine_level
end
