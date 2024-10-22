
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Skill = ConfigAdapter.Skill or {}

--获取技能类型
function ConfigAdapter.Skill.GetSkillType(skill_id, show_error)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id, show_error)
	if config_skill then
		return config_skill.type
	end
	return Macros.Game.SkillType.NORMAL
end

--获取技能等级信息
function ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	skill_lev = skill_lev or 1
	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id, false)
	if skill_type == Macros.Game.SkillType.NEAR or skill_type == Macros.Game.SkillType.JOINT_ATTACHED then
		--远程近身技能、合击配合技能技能等级为1
		skill_lev = 1
	end

	local config_skill = CPPGameLib.GetConfig("SkillLevel", skill_id)
	if not config_skill then
		return
	end
	return config_skill.level_list[skill_lev]
end

--获取精灵技能等级信息
function ConfigAdapter.Skill.GetSpiritSkillLevInfo(skill_id, skill_lev)
	skill_lev = skill_lev or 1
	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id, false)
	if skill_type == Macros.Game.SkillType.SPRITE then
		--怒气技能等级为1
		skill_lev = 1
	end

	local config_skill = CPPGameLib.GetConfig("SkillLevel", skill_id)
	if not config_skill then
		return
	end
	return config_skill.level_list[skill_lev]
end

--获取技能目标逻辑
function ConfigAdapter.Skill.GetSkillTargetLogicInfo(skill_id, ignore_bullet)
	local tgt_logic_info

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if not config_skill then
		return tgt_logic_info
	end

	if config_skill.bullet_id_list and not ignore_bullet then
		--若配了bullet_id_list，则target字段无效
		return tgt_logic_info
	end

	if config_skill.target then
		tgt_logic_info = CPPGameLib.Split(config_skill.target, "|")
		for i = 1, #tgt_logic_info do
			tgt_logic_info[i] = tonumber(tgt_logic_info[i])
		end
	end
	return tgt_logic_info
end

--获取BUFF目标逻辑列表
function ConfigAdapter.Skill.GetBuffTargetLogicInfoList(skill_id, skill_lev)
	local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	if config_skill_lev then
		return config_skill_lev.buff_target
	end
end

--获取可能触发的BUFF列表
function ConfigAdapter.Skill.GetBuffInfoList(skill_id, skill_lev)
	local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	if config_skill_lev then
		return config_skill_lev.buff_list
	end
end

--获取技能弹道信息
function ConfigAdapter.Skill.GetSkillBulletInfo(skill_id, bullet_index)
	bullet_index = bullet_index or 1
	
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if not config_skill then
		return
	end

	local bullet_id = config_skill.bullet_id_list and config_skill.bullet_id_list[bullet_index] or nil
	local bullet_offset = config_skill.bullet_offset_list and config_skill.bullet_offset_list[bullet_index] or nil
	bullet_offset = bullet_offset or {0, 0}
	local bullet_time_list = config_skill.bullet_time_list or {0}

	return bullet_id, bullet_offset, bullet_time_list
end

--获取技能TIPS
function ConfigAdapter.Skill.GetSkillTips(hero_id, skill_id, skill_lev)
	skill_lev = skill_lev or 1

	local config_hero = CPPGameLib.GetConfig("Hero", hero_id)
	if not config_hero then
		return
	end

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if not config_skill then
		return
	end
	local skill_lev_info = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	if not skill_lev_info then
		return
	end

	local tbl = {}
	tbl.des = GUI.RichText.GetColorWord(skill_lev_info.des, Macros.Color.btn_hex)
	if config_skill.type == Macros.Game.SkillType.NORMAL then
		tbl.name = string.format("[%s]", config_skill.name)
	else
		tbl.name = string.format("[%s]    Lv%d", config_skill.name, skill_lev)

		--特殊技能
		if config_skill.type == Macros.Game.SkillType.SPECIAL then
			tbl.cd = string.format("CD:%ds", config_skill.cd_time)

		--合击技能
		elseif config_skill.type == Macros.Game.SkillType.JOINT then
			local joint_hero_id = config_hero.joint_hero_show_list[1]
			local config_joint_hero = CPPGameLib.GetConfig("Hero", joint_hero_id)
			local hero_name = config_joint_hero and string.format(Macros.Quality_RT[config_joint_hero.init_quality], config_joint_hero.name) or ""

			if config_hero.joint_hero_list then
				--主将
				tbl.joint_des = GUI.RichText.GetColorWord(CPPGameLib.GetString("hero_joint_skill_tips1", hero_name), Macros.Color.btn_hex)
			else
				--副将
				tbl.joint_des = GUI.RichText.GetColorWord(CPPGameLib.GetString("hero_joint_skill_tips2", hero_name), Macros.Color.btn_hex)
			end
		end
	end

	return tbl
end

--获取精灵技能TIPS
function ConfigAdapter.Skill.GetSpiritSkillTips(skill_id, skill_lev)
	skill_lev = skill_lev or 1

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if not config_skill then
		return
	end
	local skill_lev_info = ConfigAdapter.Skill.GetSpiritSkillLevInfo(skill_id, skill_lev)
	if not skill_lev_info then
		return
	end
	local tbl = {}
	tbl.des = GUI.RichText.GetColorWord(skill_lev_info.des, Macros.Color.btn_hex)
	if config_skill.type == Macros.Game.SkillType.SPECIAL then
		tbl.name = string.format("[%s]    Lv%d", config_skill.name, skill_lev)
	else		
		tbl.name = string.format("[%s]", config_skill.name)
		--特殊技能
		if config_skill.type == Macros.Game.SkillType.SPECIAL then
			tbl.cd = string.format("CD:%ds", config_skill.cd_time)
		end
	end

	return tbl
end

--是否怒气/合击技能
function ConfigAdapter.Skill.IsAngerOrJointSkill(skill_id, check_plus)
	if not skill_id then
		return false
	end

	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)

	if check_plus then
		return skill_type == Macros.Game.SkillType.ANGER_PLUS or skill_type == Macros.Game.SkillType.JOINT_PLUS
	end
	return skill_type == Macros.Game.SkillType.ANGER or skill_type == Macros.Game.SkillType.JOINT or
	skill_type == Macros.Game.SkillType.ANGER_PLUS or skill_type == Macros.Game.SkillType.JOINT_PLUS
end

--是否合击技能(主将)
function ConfigAdapter.Skill.IsJointSkill(skill_id)
	if not skill_id then
		return false
	end

	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)
	return skill_type == Macros.Game.SkillType.JOINT or skill_type == Macros.Game.SkillType.JOINT_PLUS
end

--是否合击技能(副将)
function ConfigAdapter.Skill.IsJointAttachedSkill(skill_id)
	if not skill_id then
		return false
	end

	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)
	return skill_type == Macros.Game.SkillType.JOINT_ATTACHED	
end

--是否精灵技能
function ConfigAdapter.Skill.IsSpriteSkill(skill_id)
	if not skill_id then
		return false
	end

	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)
	return skill_type == Macros.Game.SkillType.SPRITE	
end

--获得技能列表
function ConfigAdapter.Skill.GetSkillList(info_id, is_monster)
	if not is_monster then
		local _, config_obj = ObjIDTool.GetInfo(info_id)
		if config_obj then
			return config_obj.skill_list
		end
	else
		local config_monster = CPPGameLib.GetConfig("Monster", info_id)		
		if not config_monster then
			return
		end
		if not config_monster.hero_id or config_monster.hero_id <= 0 then
			return config_monster.skill_list
		else
			local config_hero = CPPGameLib.GetConfig("Hero", config_monster.hero_id)
			if config_hero then
				return config_hero.skill_list
			end			
		end
	end
end

--获取技能相关特效列表
function ConfigAdapter.Skill.GetEffectFileMap(skill_id, skill_lev)
	local file_map
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id, false)
	if not config_skill then
		return file_map
	end

	local function record_effect_file_map(effect_id)
		local tmp_file_map = ConfigAdapter.Effect.GetEffectFiles(effect_id)
		if tmp_file_map then
			for file_name, _ in pairs(tmp_file_map) do
				file_map = file_map or {}
				file_map[file_name] = true					
			end
		end
	end

	--技能特效
	local key_list = {"attack_effect_id", "down_attack_effect_id", "attack_scene_effect_id", "down_attack_scene_effect_id", "full_screen_effect_id", "down_full_screen_effect_id", "beattacked_effect_id", "bg_effect_id", "anger_sing_effect_id"}
	for _, key in ipairs(key_list) do
		record_effect_file_map(config_skill[key])
	end	

	--弹道特效
	local config_skill_bullet
	if config_skill.bullet_id_list then
		for _, bullet_id in ipairs(config_skill.bullet_id_list) do
			config_skill_bullet = CPPGameLib.GetConfig("SkillBullet", bullet_id, false)
			if config_skill_bullet then
				record_effect_file_map(config_skill_bullet.effect_id)
			end
		end
	end

	--BUFF特效
	local config_skill_buff
	local config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, skill_lev)
	if config_skill_lev and config_skill_lev.buff_list then
		for _, buff_id in ipairs(config_skill_lev.buff_list) do
			config_skill_buff = CPPGameLib.GetConfig("SkillBuff", buff_id, false)
			if config_skill_buff then
				record_effect_file_map(config_skill_buff.effect_id)
			end
		end		
	end

	return file_map
end

