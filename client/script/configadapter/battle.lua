
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Battle = ConfigAdapter.Battle or {}

--获取怪物列表
function ConfigAdapter.Battle.GetMonsterList(battle_id)
	local config_battle = CPPGameLib.GetConfig("Battle", battle_id)
	local monster_id_list = {}
	for i = 1, 5 do
		local monster_id = config_battle["moster_"..i]
		if monster_id and monster_id > 0 then
			table.insert(monster_id_list, monster_id)
		end
	end
	return monster_id_list
end

--获取怪物列表
function ConfigAdapter.Battle.GetBattleMonsterList(battle_info)
	local battle_monster_list = {}

	if not battle_info then
		return battle_monster_list
	end

    --竞技场机器人
    if battle_info.type == Macros.Game.FBType.ARENA then
        local config_arena_robot = CPPGameLib.GetConfig("ArenaRobot", battle_info.param1, false)
        if config_arena_robot then
            for i, monster_id in ipairs(config_arena_robot.monster_list) do
                if monster_id > 0 then
                    table.insert(battle_monster_list, {index = i - 1, info_id = monster_id, damage = 0, blood_max = 0})
                end
            end
            if #battle_monster_list > 0 then
            	return battle_monster_list
            end            
        end
    end

	local config_battle = CPPGameLib.GetConfig("Battle", battle_info.battle_id)
	if not config_battle then
		return battle_monster_list
	end

	--战役怪物列表
	local hide_monster_list = config_battle.hide_monster_list
	for i = 1, 5 do
		local monster_id = config_battle["moster_"..i]

		if CPPSdkTools.IsInExamine() then
			local safe_monster_list_for_examine = ConfigAdapter.Common.GetSafeMonsterListForExamine() or Macros.Game.SafeMonsterListForExamine

			--审核状态下，指向不侵权的怪物
			monster_id = safe_monster_list_for_examine[MathTool.GetRandom(#safe_monster_list_for_examine)]
		end

		if monster_id and monster_id > 0 then
			table.insert(battle_monster_list, 
				{
					index = i - 1, info_id = monster_id, damage = 0, blood_max = 0, 
					is_hide_mode = hide_monster_list and (hide_monster_list[i] or nil) or nil
				})
		end
	end

	return battle_monster_list
end

--获取怪物总血量
function ConfigAdapter.Battle.GetMonsterTotalHP(battle_id)
	local total_hp = 0

	local monster_id, config_monster
	local config_battle = CPPGameLib.GetConfig("Battle", battle_id)
	for i = 1, 5 do
		monster_id = config_battle["moster_"..i]
		if monster_id and monster_id > 0 then
			config_monster = CPPGameLib.GetConfig("Monster", monster_id, false)
			total_hp = total_hp + (config_monster and config_monster.blood_max or 0)
		end
	end

	return total_hp
end

--获取战力提升入口列表
function ConfigAdapter.Battle.GetPowerPromoteEntrys(role_level)
	local config_power_promote_entrys = CPPGameLib.GetConfig("PowerPromoteEntry", nil, nil, true)

	for _, entry_info in ipairs(config_power_promote_entrys) do
		if entry_info.level_region[1] <= role_level and role_level <= entry_info.level_region[2] then
			return entry_info.system_id_list, entry_info.system_res_list
		end
	end
end

--获取战斗阵营属性加成
function ConfigAdapter.Battle.GetCampPlusPropMap(camp, key)
	local plus_prop_map

	local pro_type, pro_value
	local config_camp = CPPGameLib.GetConfig("Camp", camp, false)
	if config_camp and config_camp[key] then
		for _, prop_info in ipairs(config_camp[key]) do
			pro_type = prop_info[1]
			pro_value = prop_info[2]
			plus_prop_map = plus_prop_map or {}
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + pro_value
		end
	end

	return plus_prop_map
end

--获取战斗QTE评分
function ConfigAdapter.Battle.GetBattleQTEGradeInfo(out_circle_scale,in_circle_scale)
	--参数列表
    local pa_list = ConfigAdapter.Common.GetQTEGradeParameter()
    local perfect_par = MathTool.TransProbCoeffPercentage(pa_list[3])
    local great_par = MathTool.TransProbCoeffPercentage(pa_list[4])
	local grade_info,grade_type
	if math.abs(out_circle_scale - in_circle_scale) <= perfect_par then
		-- grade_info = {grade_type = 2,harm_percent = 120}
		grade_info = {grade_effectid = 100111, harm_percent = 120, grade_type = 2}
	elseif math.abs(out_circle_scale - in_circle_scale) <= great_par then
		grade_info = {grade_effectid = 100112, harm_percent = 110, grade_type = 1}
		-- grade_info = {grade_type = 1,harm_percent = 110}
	end

	--连击伤害系数
	if grade_info and grade_info.grade_type == Macros.Game.QTE_GRADE_TYPE.GRADE_PERFECT then
		--增加连击数
        GlobalVOMgr:AddQTEComboHitTimes()
        --获取战斗QTE连击附加伤害系数
        local harm_coef = ConfigAdapter.Battle.GetBattleQTEComboAddHarmCoef()
        grade_info.harm_percent = math.max(harm_coef,grade_info.harm_percent)
    else
    	--重置连击数
        GlobalVOMgr:ResetQTEComboHitTimes()
	end

	return grade_info
end

--获取战斗QTE连击附加伤害系数
function ConfigAdapter.Battle.GetBattleQTEComboAddHarmCoef()
	local harm_coef
	--连击数
	local combo_times = GlobalVOMgr:GetQTEComboHitTimes()
	if combo_times > 0 then
		local tmp = {{harm_coef = 120},{harm_coef = 130},{harm_coef = 140},{harm_coef = 150},{harm_coef = 160},{harm_coef = 170}}		
		combo_times = math.min(combo_times,#tmp)
		harm_coef = tmp[combo_times].harm_coef
	end
	return harm_coef or 100
end
