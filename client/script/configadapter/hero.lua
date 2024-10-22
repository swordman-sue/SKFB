
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Hero = ConfigAdapter.Hero or {}

--获取属性配置
function ConfigAdapter.Hero.GetPropConfig(profession, quality_level, level, hero_id )
	local config_prop = CPPGameLib.GetConfig("HeroProperty", profession)
	if not config_prop then
		return
	end
	config_prop = config_prop[quality_level]
	if not config_prop then
		LogTool.LogError("[ConfigAdapter.Hero.GetPropConfig] 英雄属性配置中无对应品质等级：%d", quality_level)
		return nil
	end
	config_prop = config_prop[level]	
	if not config_prop then
		LogTool.LogError("[ConfigAdapter.Hero.GetPropConfig] 英雄属性配置中无对应等级：%d", level)
	end

	return config_prop
end

--获取基础信息
function ConfigAdapter.Hero.GetBasicPropMap(info_id, quality_level, level, quality_stage)
	local config_hero = CPPGameLib.GetConfig("Hero", info_id)
	if not config_hero then
		return
	end
	local config_prop = ConfigAdapter.Hero.GetPropConfig(config_hero.profession, quality_level, level, info_id )
	if not config_prop then
		return
	end

	--资质
	local qualifications = ConfigAdapter.Hero.GetHeroQualifications(quality_level,info_id,quality_stage or 0)
	local config_qualifications = CPPGameLib.GetConfig("HeroQualifications",qualifications,false)
	if not config_qualifications then
		return
	end
	
	--资质系数
	local add_rate = 1+MathTool.TransProbCoeff2Decimal(config_qualifications.add_rate)

	--基础属性乘以资质系数
	local prop_map = {}
	prop_map[Macros.Game.BattleProp.ATTACK] = math.floor((config_prop.attack or 0)*add_rate)
	prop_map[Macros.Game.BattleProp.PHYSICAL_DEFENCE] = math.floor((config_prop.physical_defence or 0)*add_rate)
	prop_map[Macros.Game.BattleProp.MAGIC_DEFENCE] = math.floor((config_prop.magic_defence or 0)*add_rate)
	prop_map[Macros.Game.BattleProp.HP] = math.floor((config_prop.blood_max or 0)*add_rate)
	prop_map[Macros.Game.BattleProp.BEAT_BACK_CORRECT] = config_hero.beat_back_correct or 0
	prop_map[Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT] = config_hero.beat_back_defence_correct or 0

	return prop_map
end

--获取升级信息
function ConfigAdapter.Hero:GetUpgradeInfo(quality_level, level)
	local config = CPPGameLib.GetConfig("HeroLevel", quality_level)
	if config then
		return config.level_list[level]
	end
	return nil
end

--X经验可升到多少级
function ConfigAdapter.Hero.GetUpgradeTgtLevelByExp(quality_level, level, now_exp, add_exp,limit_level)
	local tgt_level = level
	local config = CPPGameLib.GetConfig("HeroLevel", quality_level)
	if not config then
		return tgt_level
	end

	now_exp = now_exp + add_exp
	for i = level, #config.level_list do
		now_exp = now_exp - config.level_list[i].exp
		if now_exp >= 0 then
			tgt_level = i + 1
		end
	end	
	tgt_level = math.min(tgt_level,limit_level)
	return tgt_level
end

--升到X级(满经验)，需要多少经验
function ConfigAdapter.Hero.GetUpgradeExpByTgtLevel(quality_level, level, tgt_level, now_exp)
	local add_exp = 0
	local config = CPPGameLib.GetConfig("HeroLevel", quality_level)
	if not config then
		return add_exp
	end

	if level > tgt_level then
		return add_exp
	end

	for i = level, tgt_level do
		add_exp = add_exp + config.level_list[i].exp
	end	
	add_exp = add_exp - now_exp - 1

	return add_exp
end

--获取进阶信息
function ConfigAdapter.Hero.GetBreakInfo(break_level, hero_id)
	local config = CPPGameLib.GetConfig("HeroBreak", hero_id)
	if not config then
		return
	end

	return config.level_list[break_level]	
end

--获取进阶等级上限
function ConfigAdapter.Hero.GetBreakMaxLevel(hero_id)
	local config = CPPGameLib.GetConfig("HeroBreak", hero_id)
	if not config then
		return
	end

	return #config.level_list
end

--获取进阶加成(自身)
function ConfigAdapter.Hero.GetSelfBreakPlusPropMap(hero_id, break_level)
	local config = CPPGameLib.GetConfig("HeroBreak", hero_id)
	if not config then
		return
	end

	break_level = math.min(break_level, #config.level_list)
	config = config.level_list[break_level]
	if not config then
		return
	end

	local plus_prop_map = {}
	for i, prop_type in ipairs(config.self_pro_type_list) do
		plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
		plus_prop_map[prop_type] = plus_prop_map[prop_type] + config.self_pro_value_list[i]
	end
	return plus_prop_map
end

--获取进阶加成(自身前4个)
function ConfigAdapter.Hero.GetSelfBreakPlusBasicPropMap(hero_id, break_level)
	local plus_prop_map = {}

	local config = CPPGameLib.GetConfig("HeroBreak", hero_id)
	if not config then
		return plus_prop_map
	end

	break_level = math.min(break_level, #config.level_list)
	config = config.level_list[break_level]
	if not config then
		return plus_prop_map
	end

	local prop_type

	local function PropMap( type_list , value_list )
		for i = 1, 4 do
			prop_type = type_list[i]
			plus_prop_map[prop_type] = value_list[i]
		end
	end

	PropMap( config.self_pro_type_list , config.self_pro_value_list )

	return plus_prop_map
end

--获取进阶加成(队伍)
function ConfigAdapter.Hero.GetTeamBreakPlusPropMap(hero_id, break_level)
	local config = CPPGameLib.GetConfig("HeroBreak", hero_id)
	if not config then
		return
	end

	break_level = math.min(break_level, #config.level_list)
	config = config.level_list[break_level]
	if not config then
		return
	end
	
	local plus_prop_map = {}

	if config.team_pro_type_list then
		for i, prop_type in ipairs(config.team_pro_type_list) do		
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + config.team_pro_value_list[i]
		end
	end
	return plus_prop_map
end

--获取进阶加成
function ConfigAdapter.Hero.GetBreakPlusPropMap(hero_id, break_level)
	local plus_prop_map = {}
	local self_plus_prop_map = ConfigAdapter.Hero.GetSelfBreakPlusPropMap(hero_id, break_level)
	local team_plus_prop_map = ConfigAdapter.Hero.GetTeamBreakPlusPropMap(hero_id, break_level)

	if self_plus_prop_map then	
		for prop_type, prop_value in pairs(self_plus_prop_map) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value
		end
	end

	if team_plus_prop_map then	
		for prop_type, prop_value in pairs(team_plus_prop_map) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + prop_value
		end
	end
	
	return plus_prop_map
end

--获取进阶升级信息
function ConfigAdapter.Hero.GetBreakLevelupInfo(hero_info)
	local need_level = -1
	local cost_gold = 0
	local cost_obj_list = nil

	if hero_info.break_level >= ConfigAdapter.Hero.GetBreakMaxLevel(hero_info.info_id) then
		return need_level, cost_gold, cost_obj_list
	end

	local config = CPPGameLib.GetConfig("HeroBreakLevelup", hero_info.break_level)
	if config then
		need_level = config.need_hero_level

		cost_obj_list = {}
		cost_gold = config.cost_gold
		if config.cost_item_id > 0 and config.cost_item_num > 0 then
			local obj_info = {}
			obj_info.id = config.cost_item_id
			obj_info.num = config.cost_item_num
			table.insert(cost_obj_list, obj_info)
		end
	end

	return need_level, cost_gold, cost_obj_list, 
	config.cost_same_name_hero_num, config.cost_same_quality_hero_num, config.cost_same_profession_hero_num
end

--获取培养信息
function ConfigAdapter.Hero.GetTrainInfo(pro_type, pro_level)
	local config_train_info = CPPGameLib.GetConfig("HeroTrain", pro_type)
	if config_train_info then
		return config_train_info.pro_level_list[pro_level]
	end
end

--获取培养加成
function ConfigAdapter.Hero.GetTrainPlusPropMap(train_pro_list)
	local plus_prop_map
	if not train_pro_list then
		return plus_prop_map
	end
	plus_prop_map = {}

	local config_train_info
	for _, pro_info in ipairs(train_pro_list) do
		if pro_info.pro_type > 0 then
			--属性加成
			plus_prop_map[pro_info.pro_type] = plus_prop_map[pro_info.pro_type] or 0
			plus_prop_map[pro_info.pro_type] = plus_prop_map[pro_info.pro_type] + pro_info.pro_value

			--额外加成
			config_train_info = ConfigAdapter.Hero.GetTrainInfo(pro_info.pro_type, pro_info.pro_level)
			if config_train_info then
				if pro_info.pro_value < config_train_info.pro_value_max then
					--当前等级属性值未达上限，以上一等级的额外加成为准
					config_train_info = ConfigAdapter.Hero.GetTrainInfo(pro_info.pro_type, pro_info.pro_level - 1)
				end
				if config_train_info then
					for i, pro_type in ipairs(config_train_info.level_up_pro_type_list) do
						plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
						plus_prop_map[pro_type] = plus_prop_map[pro_type] + (config_train_info.level_up_pro_value_list[i] or 0)
					end
				end
			end
		end
	end

	return plus_prop_map
end


--获取潜能等级上限
function ConfigAdapter.Hero.GetPotentialMaxLevel()
	local config_hero_destinys = CPPGameLib.GetConfig("HeroDestiny", nil, nil, true)
	return table.maxn(config_hero_destinys)
end

--获取潜能直接升级概率(万分比)
function ConfigAdapter.Hero.GetPotentialLevelupProb(potential_level, potential_value)
	local prob = 0

	local config = CPPGameLib.GetConfig("HeroDestiny", potential_level)
	if not config then
		return prob
	end

	if config.level_up_rate then
		for _, info in ipairs(config.level_up_rate) do
			if potential_value >= info[1] and potential_value <= info[2] then
				prob = info[3] or 0
				break
			end
		end
	end

	return prob
end

--获取潜能直接升级概率描述
function ConfigAdapter.Hero.GetPotentialLevelupDesc(potential_level, potential_value)
	if not ConfigAdapter.Hero.potential_levelup_desc then
		ConfigAdapter.Hero.potential_levelup_desc = 
		{
			{0, 19},
			{20, 29},
			{30, 39},
			{40, 49},
			{50, 100},
		}
	end

	local desc_index = 1
	-- local prob = ConfigAdapter.Hero.GetPotentialLevelupProb(potential_level, potential_value) / 100
	local config = CPPGameLib.GetConfig("HeroDestiny", potential_level)
	if not config then
		return desc_index
	end
	if config.level_up_rate then
		for cur_index, info in ipairs(config.level_up_rate) do
			if potential_value >= info[1] and potential_value <= info[2] then
				desc_index = cur_index
				break
			end
		end
	end
	print("ldx","desc_index: ",desc_index)
	return CPPGameLib.GetString("hero_potential_probability"..desc_index)
end

--获取潜能加成
function ConfigAdapter.Hero.GetPotentialPlusPropMap(potential_level , hero_id )
	local config = CPPGameLib.GetConfig("HeroDestiny", potential_level,false)
	if not config then
		return
	end

	local plus_prop_map = {}
	if config.pro_type_list then		
		for prop_index, prop_type in pairs(config.pro_type_list) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + (config.pro_value_list[prop_index] or 0)
		end
	end

	return plus_prop_map
end

--获取觉醒加成
function ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_id, awaken_star, awaken_level, awaken_item_list,is_only_pro_type)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_id)
	if not config_hero then
		return
	end
	local special_id = config_hero.init_quality * 10 + config_hero.profession


	local config_hero_awaken = CPPGameLib.GetConfig("HeroAwakening", special_id, false)
	if not config_hero_awaken or not config_hero_awaken[awaken_star] then
		return 
	end

	local config_hero_awaken_info = config_hero_awaken[awaken_star][awaken_level]
	if not config_hero_awaken_info then
		return
	end

	local plus_prop_map = {}

	--加成属性
	if config_hero_awaken_info.pro_type_list then
		for prop_index, prop_type in ipairs(config_hero_awaken_info.pro_type_list) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + (config_hero_awaken_info.pro_value_list[prop_index] or 0)
		end
		if is_only_pro_type then
			return plus_prop_map
		end
	end

	--额外加成属性
	if config_hero_awaken_info.extra_pro_type_list then
		for prop_index, prop_type in ipairs(config_hero_awaken_info.extra_pro_type_list) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + (config_hero_awaken_info.extra_pro_value_list[prop_index] or 0)
		end
	end

	--材料加成属性
	if config_hero_awaken_info.item_pro_type_list then
		for prop_index, prop_type in ipairs(config_hero_awaken_info.item_pro_type_list) do
			plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
			plus_prop_map[prop_type] = plus_prop_map[prop_type] + (config_hero_awaken_info.item_pro_value_list[prop_index] or 0)
		end		
	end

	--当前觉醒道具加成属性
	if awaken_item_list then
		local config_awaken_item
		for _, item_id in ipairs(awaken_item_list) do
			config_awaken_item = CPPGameLib.GetConfig("AwakeningItem", item_id)
			if config_awaken_item and config_awaken_item.pro_type_list then
				for prop_index, pro_type in ipairs(config_awaken_item.pro_type_list) do
					plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
					plus_prop_map[pro_type] = plus_prop_map[pro_type] + (config_awaken_item.pro_value_list[prop_index] or 0)
				end
			end
		end
	end

	return plus_prop_map
end

--获取英雄的技能等级
function ConfigAdapter.Hero.GetPotentialSkillLevel(potential_level)
	local config = CPPGameLib.GetConfig("HeroDestiny", potential_level, false)
	if not config then
		return 1
	end
	return config.skill_level
end

--获取阵位信息
function ConfigAdapter.Hero.GetLineupPosInfo(pos_index)
	local config_lineup = CPPGameLib.GetConfig("HeroLineup", 1)
	return config_lineup.pos_list[pos_index]
end

--获取合击英雄表
function ConfigAdapter.Hero.GetJointHeroMap(hero_id)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_id)
	if not config_hero or not config_hero.joint_hero_list then
		return
	end
	
	local hero_map = {}
	for _, hero_id in ipairs(config_hero.joint_hero_list) do
		hero_map[hero_id] = true
	end
	return hero_map
end

--获取合击英雄名字(目前，joint_hero_list只配1个合击英雄)
function ConfigAdapter.Hero.GetJointHeroID(hero_id)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_id)
	if not config_hero or not config_hero.joint_hero_list then
		return
	end

	local joint_hero_id = config_hero.joint_hero_list[1]
	return joint_hero_id
end

--获取最终的属性表
function ConfigAdapter.Hero.GetFinalPropMap(prop_map, plus_prop_map)
	if not prop_map then
		return
	end

	if plus_prop_map then
		for prop_type, prop_value in pairs(plus_prop_map) do
			prop_map[prop_type] = prop_map[prop_type] or 0
			prop_map[prop_type] = prop_map[prop_type] + prop_value
		end
	end
	
	local sp_prop_type_map = 
	{
		[Macros.Game.BattleProp.ATTACK] = Macros.Game.BattleProp.ATTACK_RATE,
		[Macros.Game.BattleProp.PHYSICAL_DEFENCE] = Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE,
		[Macros.Game.BattleProp.MAGIC_DEFENCE] = Macros.Game.BattleProp.MAGIC_DEFENCE_RATE,
		[Macros.Game.BattleProp.HP] = Macros.Game.BattleProp.HP_RATE,
	}
	for prop_type, sp_prop_type in ipairs(sp_prop_type_map) do
		if prop_map[prop_type] and prop_map[sp_prop_type] then
			prop_map[prop_type] = math.ceil(prop_map[prop_type] * (1 + MathTool.TransProbCoeff2Decimal(prop_map[sp_prop_type])))
		end		
	end	

	return prop_map
end


--获取英雄进化属性
function ConfigAdapter.Hero.GetHeroEvolutionPropMap(quality_level,info_id,level,quality_stage)
	if not quality_level or not info_id or not level or not quality_stage then
		return
	end
	local config_hero = CPPGameLib.GetConfig("Hero",info_id,false)	
	--不能超越品阶上限
	if not config_hero or quality_level > config_hero.quality_limit or (quality_level == config_hero.quality_limit and quality_stage > config_hero.quality_stage_limit) then
		return
	end
	local config_evolution = CPPGameLib.GetConfig("HeroEvolution",quality_level,false)
	if not config_evolution or config_evolution and not config_evolution.stage_list[quality_stage] then
		return
	end
	--基础属性
	local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(info_id, quality_level, level, quality_stage)		

	--进化附加属性
	basic_prop_map = ConfigAdapter.Hero.GetHeroEvolutionPlusPropMap(quality_level,quality_stage,info_id,basic_prop_map)	

	--资质
	basic_prop_map.qualifications = ConfigAdapter.Hero.GetHeroQualifications(quality_level,info_id,quality_stage)

	return basic_prop_map
end

--获取英雄资质
function ConfigAdapter.Hero.GetHeroQualifications(quality_level,info_id,quality_stage)
	local config_hero = CPPGameLib.GetConfig("Hero",info_id,false)	
	local qualifications = config_hero.Qualifications
	--计算资质
	for quality=config_hero.init_quality,quality_level do
		local config_evolution = CPPGameLib.GetConfig("HeroEvolution",quality,false)
		if config_evolution and config_evolution.stage_list then
			--初始品质阶不算第一个
			for stage = quality == config_hero.init_quality and 1 or 0,quality == quality_level and quality_stage or table.maxn(config_evolution.stage_list) do
				if config_evolution.stage_list[stage] then
					qualifications = qualifications + config_evolution.stage_list[stage].add_qualifications
				end
			end
		end
	end
	return qualifications or 0
end

--获取英雄进化附加属性
function ConfigAdapter.Hero.GetHeroEvolutionPlusPropMap(quality_level,quality_stage,info_id,basic_prop_map)
	local basic_prop_map = basic_prop_map or {}
	local config_hero = CPPGameLib.GetConfig("Hero",info_id,false)	
	if not config_hero then
		return
	end
	for cur_quality=config_hero.init_quality,quality_level do
		--进化配置表
		local config_evolution = CPPGameLib.GetConfig("HeroEvolution",cur_quality,false)
		if not config_evolution then
			return
		end
		--进化附加属性
		for stage=cur_quality == config_hero.init_quality and 1 or 0,cur_quality == quality_level and quality_stage or table.maxn(config_evolution.stage_list) do
			if config_evolution.stage_list[stage].pro_type_list then		
				for prop_index, prop_type in pairs(config_evolution.stage_list[stage].pro_type_list) do
					basic_prop_map[prop_type] = basic_prop_map[prop_type] or 0
					basic_prop_map[prop_type] = basic_prop_map[prop_type] + (config_evolution.stage_list[stage].pro_value_list[prop_index] or 0)
				end
			end
		end
	end		
	return basic_prop_map
end
