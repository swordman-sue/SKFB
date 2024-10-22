
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Equip = ConfigAdapter.Equip or {}

--获取装备初始、强化加成
function ConfigAdapter.Equip.GetStrengthenPlusPropMap(equip_info)
	if not equip_info then
		return
	end

	local config_equip = CPPGameLib.GetConfig("Equipment", equip_info.info_id , false )
	if not config_equip then
		return
	end

	local plus_prop_map = 
	{
		[config_equip.strengthen_init_pro[1]] = config_equip.strengthen_init_pro[2] + config_equip.strengthen_add_pro * (equip_info.strengthen_level - 1)
	}	
	return plus_prop_map
end

--获取装备精炼加成
function ConfigAdapter.Equip.GetRefinePlusPropMap(equip_info)
	if not equip_info then
		return
	end

	local config_equip = CPPGameLib.GetConfig("Equipment", equip_info.info_id , false )
	if not config_equip then
		return
	end

	local refine_pro_type_list = config_equip.refine_pro_type_list
	local refine_pro_value_list = config_equip.refine_pro_value_list

	local plus_prop_map
	for i, v in ipairs(refine_pro_type_list) do
		plus_prop_map = plus_prop_map or {}
		plus_prop_map[v] = plus_prop_map[v] or 0
		plus_prop_map[v] = plus_prop_map[v] + refine_pro_value_list[i] * equip_info.refine_level
	end
	return plus_prop_map
end

--获取套装加成
function ConfigAdapter.Equip.GetSuitPlusPropMap(lineup_equip_map)
	--记录已激活的套装数量信息
	local suit_activated_map

	local config_equip
	for _, equip_info in pairs(lineup_equip_map) do
		config_equip = CPPGameLib.GetConfig("Equipment", equip_info.info_id , false )
		if config_equip and CPPGameLib.GetConfig("EquipmentSuit", config_equip.suit_id, false) then
			suit_activated_map = suit_activated_map or {}
			suit_activated_map[config_equip.suit_id] = suit_activated_map[config_equip.suit_id] or 0
			suit_activated_map[config_equip.suit_id] = suit_activated_map[config_equip.suit_id] + 1
		end
	end

	if not suit_activated_map then
		return
	end

	local plus_prop_map
	local config_suit, prop_type_list, prop_value_list

	for activated_suit_id, activated_num in pairs(suit_activated_map) do
		config_suit = CPPGameLib.GetConfig("EquipmentSuit", activated_suit_id , false )
		for i = 1, activated_num - 1 do
			prop_type_list = config_suit[string.format("pro%d_type_list", i)]
			prop_value_list = config_suit[string.format("pro%d_value_list", i)]

			for prop_index, prop_type in ipairs(prop_type_list) do
				plus_prop_map = plus_prop_map or {}
				plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
				plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value_list[prop_index]
			end
		end
	end

	return plus_prop_map
end

--获取天赋属性加成
function ConfigAdapter.Equip.GetTalentPlusPropMap(equip_info)
	if not equip_info then
		return
	end

	local config_equip = CPPGameLib.GetConfig("Equipment", equip_info.info_id , false)
	if not config_equip then
		return
	end

	local config_equip_talent = CPPGameLib.GetConfig("EquipmentTalent", config_equip.talent, false)
	if not config_equip_talent then
		return
	end

	--根据强化等级查找达到的天赋等级
	local tgt_index
	for index, talent_info in ipairs(config_equip_talent.level_list) do		
		if talent_info.intensify_level > equip_info.strengthen_level then
			tgt_index = index - 1
			break
		end
	end
	tgt_index = tgt_index or #config_equip_talent.level_list

	local talent_level_info = config_equip_talent.level_list[tgt_index]
	if not talent_level_info then
		return
	end

	local plus_prop_map
	for i, v in ipairs(talent_level_info.nature_type) do
		plus_prop_map = plus_prop_map or {}
		plus_prop_map[v] = plus_prop_map[v] or 0
		plus_prop_map[v] = plus_prop_map[v] + talent_level_info.nature_lst[i]
	end
	return plus_prop_map
end

--获取升星加成
function ConfigAdapter.Equip.GetStarUpPlusPropMap(equip_info)
	if not equip_info then
		return
	end

	local config_equip = CPPGameLib.GetConfig("Equipment", equip_info.info_id , false)
	if not config_equip then
		return
	end

	local config_equip_starup = CallModuleFunc(ModuleType.EQUIP,"GetStarUpCnf",config_equip.item_type,config_equip.quality,equip_info.star,equip_info.star_level)
	if not config_equip_starup then
		return
	end

	--根据当前的升星等级来算属性
	local plus_prop_map
	for i,v in ipairs(config_equip_starup.pro_type_list) do
		plus_prop_map = plus_prop_map or {}
		plus_prop_map[v] = plus_prop_map[v] or 0
		plus_prop_map[v] = plus_prop_map[v] + config_equip_starup.pro_value_list[i]
	end
	return plus_prop_map
end

--获取装备强化、精炼、套装、天赋属性加成
function ConfigAdapter.Equip.GetPlusPropMap(lineup_equip_map)
	local plus_prop_map, strengthen_plus_prop_map, refine_plus_prop_map, talent_plus_prop_map

	--套装加成
	plus_prop_map = ConfigAdapter.Equip.GetSuitPlusPropMap(lineup_equip_map)

	for _, equip_info in pairs(lineup_equip_map) do
		--强化加成
		strengthen_plus_prop_map = ConfigAdapter.Equip.GetStrengthenPlusPropMap(equip_info)
		if strengthen_plus_prop_map then
			for prop_type, prop_value in pairs(strengthen_plus_prop_map) do
				plus_prop_map = plus_prop_map or {}
				plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
				plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value		
			end
		end

		--精炼加成
		refine_plus_prop_map = ConfigAdapter.Equip.GetRefinePlusPropMap(equip_info)
		if refine_plus_prop_map then
			for prop_type, prop_value in pairs(refine_plus_prop_map) do
				plus_prop_map = plus_prop_map or {}
				plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
				plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value		
			end
		end

		--天赋加成
		talent_plus_prop_map = ConfigAdapter.Equip.GetTalentPlusPropMap(equip_info)
		if talent_plus_prop_map then
			for prop_type, prop_value in pairs(talent_plus_prop_map) do
				plus_prop_map = plus_prop_map or {}
				plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
				plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value		
			end
		end

		--升星加成
		starup_plus_prop_map = ConfigAdapter.Equip.GetStarUpPlusPropMap(equip_info)
		if starup_plus_prop_map then
			for prop_type, prop_value in pairs(starup_plus_prop_map) do
				plus_prop_map = plus_prop_map or {}
				plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
				plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value		
			end
		end
	end

	return plus_prop_map
end