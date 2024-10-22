
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Spirit = ConfigAdapter.Spirit or {}

local get_spirit_plus_prop_map

--获取世界之心属性加成
function ConfigAdapter.Spirit.GetWorldHeartPlusPropMap(stage, level)
	local plus_prop_map

	local config_stage, config_elve, level_num, prop_type
	for stage_index = 1, stage do
		config_stage = CPPGameLib.GetConfig("HeartOfWorld", stage_index)
		if config_stage then
			level_num = stage_index == stage and level or #config_stage.elves_list
			for level_index = 1, level_num do
				config_elve = config_stage.elves_list[level_index]
				for prop_index,prop_type in ipairs(config_elve.add_pro_type) do
					if prop_type > 0 then
						plus_prop_map = plus_prop_map or {}
						plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
						plus_prop_map[prop_type] = plus_prop_map[prop_type] + config_elve.add_pro_value[prop_index]
					end
				end				
			end
		end
	end

	return plus_prop_map
end

--获取精灵图鉴配置
function ConfigAdapter.Spirit.GetPokedexInfo(pokedex_id, pokedex_lev)
	local config_pokedex = CPPGameLib.GetConfig("SpiritPokedex", pokedex_id)
	if config_pokedex and config_pokedex.pokedex_list then
		return config_pokedex.pokedex_list[pokedex_lev]
	end
end

--获取精灵强化信息
function ConfigAdapter.Spirit.GetEnhaceInfo(spirit_id, enhance_level)
	local config_enhace = CPPGameLib.GetConfig("SpiritEnhace", spirit_id, false)
	if config_enhace and config_enhace.enhance_list then
		return config_enhace.enhance_list[enhance_level]
	end	
end

--获取精灵培养信息
function ConfigAdapter.Spirit.GetTrainInfo(spirit_id, train_stage)
	local config_train = CPPGameLib.GetConfig("SpriteTrain", spirit_id, false)
	if not config_train or not config_train.stage_list then
		return
	end	
	return config_train.stage_list[train_stage]
end

--获取精灵天赋技能信息
function ConfigAdapter.Spirit.GetTalentInfo(spirit_id, skill_lv)
	local config_talent = CPPGameLib.GetConfig("SpriteTalentSkill", spirit_id)
	if not config_talent or not config_talent.skill_list then
		return
	end	
	return config_talent.skill_list[skill_lv]
end

--获取精灵资质
function ConfigAdapter.Spirit.GetQualificationInfo(spirit_id, star)
	star = star or 0

	local config_spirit = CPPGameLib.GetConfig("Spirit", spirit_id)
	if not config_spirit then
		return
	end

	local qualifications = config_spirit.qualifications[star + 1] or 0
	local config_qualifications = CPPGameLib.GetConfig("SpriteQualifications", qualifications, false)
	return config_qualifications
end

--获取精灵特性
function ConfigAdapter.Spirit.GetSpecialPropertyInfo(spirit_id, star)
	star = star or 0

	local config_spirit_sp_prop = CPPGameLib.GetConfig("SpriteSpecialProperty", spirit_id)
	if not config_spirit_sp_prop then
		return
	end
	return config_spirit_sp_prop.star_list[star] 
end

--获取当前精灵祝福开放数量以及下一个名额开放等级
function ConfigAdapter.Spirit.GetCurBlessNumAndNextOpenLevel()
	local bless_sprite_num_max = 0 
	local next_openlevel
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local rolelevel_info = CPPGameLib.GetConfig("RoleLevel",role_level)
	if rolelevel_info and rolelevel_info.bless_sprite_num_max then
		bless_sprite_num_max = rolelevel_info.bless_sprite_num_max
	end
	--遍历寻找下一个
	for _,cur_rolelevel_info in ipairs(CPPGameLib.GetConfig("RoleLevel",nil,nil,true)) do
		if cur_rolelevel_info.bless_sprite_num_max > bless_sprite_num_max then
			next_openlevel = cur_rolelevel_info.level
			break
		end
	end
	return bless_sprite_num_max,next_openlevel
end

--获取精灵强化加成
function ConfigAdapter.Spirit.GetEnhacePlusPropMap(spirit_id, enhance_level, star, for_heros_plus_prop_map)
	local plus_prop_map

	local qualifications_info = ConfigAdapter.Spirit.GetQualificationInfo(spirit_id, star)
	local pro_addition_rate = qualifications_info and MathTool.TransProbCoeff2Decimal(qualifications_info.add_rate) or 0

	local enhace_info = ConfigAdapter.Spirit.GetEnhaceInfo(spirit_id, enhance_level)
	if enhace_info then
		for i, pro_type in ipairs(enhace_info.pro_type_list) do
			if for_heros_plus_prop_map then
				for_heros_plus_prop_map[pro_type] = for_heros_plus_prop_map[pro_type] or 0
				for_heros_plus_prop_map[pro_type] = for_heros_plus_prop_map[pro_type] + enhace_info.pro_value_list[i] * (1 + pro_addition_rate)
			else
				plus_prop_map = plus_prop_map or {}
				plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
				plus_prop_map[pro_type] = plus_prop_map[pro_type] + enhace_info.pro_value_list[i] * (1 + pro_addition_rate)
			end
		end
	end

	return plus_prop_map
end

--获取精灵培养加成
function ConfigAdapter.Spirit.GetTrainPlusPropMap(spirit_id, train_stage, train_times, for_heros_plus_prop_map)
	local plus_prop_map

	local train_info = ConfigAdapter.Spirit.GetTrainInfo(spirit_id, train_stage)
	if not train_info then
		return
	end

	local function record_func(pro_type, pro_value)
		if for_heros_plus_prop_map then
			for_heros_plus_prop_map[pro_type] = for_heros_plus_prop_map[pro_type] or 0
			for_heros_plus_prop_map[pro_type] = for_heros_plus_prop_map[pro_type] + pro_value
		else
			plus_prop_map = plus_prop_map or {}
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + pro_value
		end
	end

	for i, pro_type in ipairs(train_info.total_pro_type_list) do
		record_func(pro_type, train_info.total_pro_value_list[i])
	end

	for i, pro_type in ipairs(train_info.pro_type_list) do
		record_func(pro_type, train_info.pro_value_list[i] * train_times)
	end

	return plus_prop_map
end

--获取天赋属性加成
function ConfigAdapter.Spirit.GetTalentPlusPropMap(spirit_data, for_battle_heros)
    if not spirit_data then
    	return
	end

	--全体出战英雄属性加成
	local all_heros_plus_prop_map
	--全体精灵属性加成
	local all_spirits_plus_prop_map

	local train_info, talent_info
	for _, spirit_info in ipairs(spirit_data.spirit_list) do
		train_info = ConfigAdapter.Spirit.GetTrainInfo(spirit_info.id, spirit_info.train_stage)
		if train_info then
			talent_info = ConfigAdapter.Spirit.GetTalentInfo(spirit_info.id, train_info.talent_level or 1)
			if talent_info then
				--全体精灵属性加成
				if talent_info.spirit_pro_type_list and talent_info.spirit_pro_value_list then
					all_spirits_plus_prop_map = all_spirits_plus_prop_map or {}
					for i, pro_type in ipairs(talent_info.spirit_pro_type_list) do
						Modules.BattleFormula:AddPlusPropMap(all_spirits_plus_prop_map, pro_type, talent_info.spirit_pro_value_list[i])												
					end
				end

				--全体出战英雄属性加成
				if for_battle_heros then
					all_heros_plus_prop_map = all_heros_plus_prop_map or {}
					if talent_info.hero_pro_type_list and talent_info.hero_pro_value_list then
						for i, pro_type in ipairs(talent_info.hero_pro_type_list) do
							Modules.BattleFormula:AddPlusPropMap(all_heros_plus_prop_map, pro_type, talent_info.hero_pro_value_list[i])												
						end
					end
				end
			end
		end
	end

	return all_heros_plus_prop_map, all_spirits_plus_prop_map
end

--[[
获取精灵属性加成
精灵总属性 = 精灵强化属性 * ( 1 + 精灵资质加成万分比 ) + 培养属性 + 所有精灵天赋“全体精灵属性”
出战属性加成 = 精灵总属性 * 属性转换率万分比 + 图鉴属性 + 所有精灵天赋“全体出战英雄属性”
祝福属性加成 = 精灵总属性 * 属性转换率万分比 + 特性属性 + 图鉴属性 + 所有精灵天赋“全体出战英雄属性”
--]]
--获取精灵属性加成/精灵总属性
function ConfigAdapter.Spirit.GetPropMap(spirit_data, spirit_id, do_pro_conversion)
	local plus_prop_map

	if not spirit_data then
		return plus_prop_map
	end

	local ignore_talent
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", spirit_id or spirit_data.cur_use_spirit)
	if not spirit_info and spirit_id then
		--处理未获得精灵的属性显示问题
		spirit_info = 
		{
			id = spirit_id,
			enhance_level = 0,
			train_stage = 1,
			train_times = 0,
			star = 0,
		}
		ignore_talent = true
	end

	plus_prop_map = get_spirit_plus_prop_map(spirit_data, spirit_info, nil, nil, do_pro_conversion, ignore_talent)
	return plus_prop_map
end

--获取己方精灵出战/祝福属性加成、精灵总属性
function ConfigAdapter.Spirit.GetPropMapEx(spirit_data, spirit_id, for_battle_heros, for_battle_bless, calc_spirit_props)
	local plus_prop_map, spirit_prop_map

	if not spirit_data then
		return plus_prop_map
	end

	local spirit_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", spirit_id or spirit_data.cur_use_spirit)
	if not spirit_info and spirit_id then
		--处理未获得精灵的属性显示问题
		spirit_info = 
		{
			id = spirit_id,
			enhance_level = 0,
			train_stage = 1,
			train_times = 0,
			star = 0,
		}
	end
	plus_prop_map, spirit_prop_map = get_spirit_plus_prop_map(spirit_data, spirit_info, for_battle_heros, for_battle_bless, true, nil, calc_spirit_props)
	return plus_prop_map, spirit_prop_map
end

--获取敌方精灵总属性
function ConfigAdapter.Spirit.GetPropMapForEnemy(spirit_data, spirit_info)
	local plus_prop_map

	if not spirit_data then
		return plus_prop_map
	end

	plus_prop_map = get_spirit_plus_prop_map(spirit_data, spirit_info)
	return plus_prop_map
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_spirit_plus_prop_map = function(spirit_data, spirit_info, for_battle_heros, for_battle_bless, do_pro_conversion, ignore_talent, calc_spirit_props)
	local plus_prop_map, spirit_prop_map
	local all_heros_plus_prop_map, all_spirits_plus_prop_map

	if not ignore_talent then
		--获取天赋属性加成
		all_heros_plus_prop_map, all_spirits_plus_prop_map = ConfigAdapter.Spirit.GetTalentPlusPropMap(spirit_data, for_battle_heros)
	end

	if spirit_info and spirit_info.id > 0 then
		local sub_plus_prop_map = {}

		--强化加成
		ConfigAdapter.Spirit.GetEnhacePlusPropMap(spirit_info.id, spirit_info.enhance_level, spirit_info.star, sub_plus_prop_map)
		-- CPPGameLib.PrintTable("skfb", sub_plus_prop_map, "强化加成后")
	
		--培养加成
		ConfigAdapter.Spirit.GetTrainPlusPropMap(spirit_info.id, spirit_info.train_stage, spirit_info.train_times, sub_plus_prop_map)	
		-- CPPGameLib.PrintTable("skfb", sub_plus_prop_map, "培养加成后")

		--天赋-全体精灵属性加成
		if all_spirits_plus_prop_map then
			for pro_type, pro_value in pairs(all_spirits_plus_prop_map) do
				Modules.BattleFormula:AddPlusPropMap(sub_plus_prop_map, pro_type, pro_value)				
			end
		end
		-- CPPGameLib.PrintTable("skfb", sub_plus_prop_map, "天赋-全体精灵属性加成后")

		--属性转换
		local pro_conversion_rate
		if do_pro_conversion then
			local train_info = ConfigAdapter.Spirit.GetTrainInfo(spirit_info.id, spirit_info.train_stage)
			pro_conversion_rate = train_info and MathTool.TransProbCoeff2Decimal(train_info.pro_conversion_rate) or 1
		else
			pro_conversion_rate = 1
		end
		-- print("skfb", "属性转换率: ", pro_conversion_rate)

		for pro_type, pro_value in pairs(sub_plus_prop_map) do
			plus_prop_map = plus_prop_map or {}
			Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, pro_value * pro_conversion_rate)

			--计算精灵总属性
			if calc_spirit_props then
				spirit_prop_map = spirit_prop_map or {}				
				Modules.BattleFormula:AddPlusPropMap(spirit_prop_map, pro_type, pro_value)
			end			
		end

		--祝福特性属性加成
		if for_battle_bless then
			local sp_prop_info = ConfigAdapter.Spirit.GetSpecialPropertyInfo(spirit_info.id, spirit_info.star)
			if sp_prop_info then
				sub_plus_prop_map = Modules.BattleFormula:GenPlusPropMap(sp_prop_info.bless_pro_type_list, sp_prop_info.bless_pro_value_list)
				if sub_plus_prop_map then
					for pro_type, pro_value in pairs(sub_plus_prop_map) do
						plus_prop_map = plus_prop_map or {}
						Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, pro_value)
					end				
				end
			end
		end
	end
	
	if for_battle_heros then
		--图鉴加成
		if spirit_data.pokedex_list then
			local config_pokedex_info
			for _, pokedex_data in ipairs(spirit_data.pokedex_list) do
				config_pokedex_info = ConfigAdapter.Spirit.GetPokedexInfo(pokedex_data.id, pokedex_data.level)
				if config_pokedex_info then
					plus_prop_map = plus_prop_map or {}
					Modules.BattleFormula:AddPlusPropMap(plus_prop_map, config_pokedex_info.pro_type, config_pokedex_info.pro_value)
				end
			end
		end

		--天赋-全体出战英雄属性加成
		if all_heros_plus_prop_map then
			for pro_type, pro_value in pairs(all_heros_plus_prop_map) do
				plus_prop_map = plus_prop_map or {}
				Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, pro_value)				
			end
		end
	end

	return plus_prop_map, spirit_prop_map
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
