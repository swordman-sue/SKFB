
--[[
%% @module: 技能逻辑
%% @author: swordman sue
%% @created: 2016-09-08
--]]

Skill = Skill or {}

Skill.SkillLogic = Skill.SkillLogic or {}

--技能特殊逻辑类型
Skill.SpSkillLogicType = 
{
	ATTACK = 0,
	BEATTACKED = 1,
}

local check_in_range

--根据技能命中目标逻辑，搜索目标和追击点
function Skill.SkillLogic:SearchTgtAndChaseForAI(attacker_obj_id, skill_id)
	local is_need_tgt = true
	local is_need_chase = true
	local tgt_obj_id, chase_world_pos

	local me = GlobalScene:GetObj(attacker_obj_id)
    if not skill_id or not me or not me:CanDoAttack(skill_id) then
    	return is_need_tgt, is_need_chase, tgt_obj_id, chase_world_pos
    end

	local first_tgt_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", attacker_obj_id, skill_id)

	local bullet_id, bullet_offset = ConfigAdapter.Skill.GetSkillBulletInfo(skill_id)
	if bullet_id then
		--弹道
		local config_bullet = CPPGameLib.GetConfig("SkillBullet", bullet_id)
		if config_bullet then
			tgt_obj_id = first_tgt_id
			chase_world_pos = self:__FindChasePos(attacker_obj_id, tgt_obj_id, bullet_offset[1], bullet_offset[1] + config_bullet.move_distance)		
		end
	else
		--命中目标逻辑
		local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(skill_id)
		if tgt_logic_info then
			local tgt_type = tgt_logic_info[1] 

			--1 近战目标(1|类型|抬手距离|伤害距离，0=单体；1=群体)
			if tgt_type == Macros.Game.SkillTgtLogic.NEAR then
				if tgt_logic_info[2] == 0 then
					tgt_obj_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstPNTgt", attacker_obj_id, skill_id) or first_tgt_id
				else
					tgt_obj_id = first_tgt_id
				end
				local trigger_range = tgt_logic_info[3] ~= 0 and tgt_logic_info[3] or ConfigAdapter.Common.GetTriggerRangeOfNearSkill()
				chase_world_pos = self:__FindChasePos(attacker_obj_id, tgt_obj_id, 0, trigger_range)

			--2 全体随机N个(2|N)
			elseif tgt_type == Macros.Game.SkillTgtLogic.RAND_IN_ALL then
				is_need_tgt = false
				is_need_chase = false
						
			--3 范围(3|范围左边界|范围右边界，目标进入范围内释放)
			elseif tgt_type == Macros.Game.SkillTgtLogic.RANGE then
				tgt_obj_id = first_tgt_id
				chase_world_pos = self:__FindChasePos(attacker_obj_id, tgt_obj_id, tgt_logic_info[2], tgt_logic_info[3])

			--4 全体(4)
			elseif tgt_type == Macros.Game.SkillTgtLogic.ALL then
				tgt_obj_id = first_tgt_id
				is_need_chase = false

			--[[
			5 特殊目标（5|选取目标|范围，范围配0表示特定目标）
			--]]
			elseif tgt_type == Macros.Game.SkillTgtLogic.SPECIAL then
				local sp_tgt_vo_list = self:__GetSpecialBattleTgtMapOfSkillTgtLogic(attacker_obj_id, skill_id)
				tgt_obj_id = sp_tgt_vo_list[1] and sp_tgt_vo_list[1].obj_id or nil
				is_need_chase = false
			end			
		end
	end

	return is_need_tgt, is_need_chase, tgt_obj_id, chase_world_pos
end

--根据命中目标逻辑，检测能否攻击
function Skill.SkillLogic:CheckToAttackForAI(attacker_obj_id, tgt_obj_id, skill_id, skill_lev)
	local attacker = GlobalScene:GetObj(attacker_obj_id)
    if not attacker or not attacker:CanDoAttack(skill_id) then
    	return false
    end

	local bullet_id, bullet_offset = ConfigAdapter.Skill.GetSkillBulletInfo(skill_id)
	if bullet_id then		
		--弹道		
		local tgt = GlobalScene:GetObj(tgt_obj_id)
		if not tgt or not tgt:CanDoBeAttacked() then
			return false
		end

		local config_bullet = CPPGameLib.GetConfig("SkillBullet", bullet_id)
		if not config_bullet then
			return false
		end
		if not check_in_range(attacker_obj_id, tgt:GetWorldPosVec(), bullet_offset[1], bullet_offset[1] + config_bullet.move_distance) then
			return false
		end
	else
		local tgt = GlobalScene:GetObj(tgt_obj_id)
		local tgt_vo = tgt and tgt:GetVO() or nil		

		--命中目标逻辑
		local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(skill_id)
		if not tgt_logic_info then
			return false
		end

		local tgt_type = tgt_logic_info[1] 
		if tgt_type == Macros.Game.SkillTgtLogic.NEAR or 
			tgt_type == Macros.Game.SkillTgtLogic.RANGE or 
			tgt_type == Macros.Game.SkillTgtLogic.SPECIAL or 
			tgt_type == Macros.Game.SkillTgtLogic.ALL then
			if not tgt or not tgt:CanDoBeAttacked() then
				return false
			end
		end

		--1 近战目标(1|类型|抬手距离|伤害距离，0=单体；1=群体)
		if tgt_type == Macros.Game.SkillTgtLogic.NEAR then
			local trigger_range = tgt_logic_info[3] ~= 0 and tgt_logic_info[3] or ConfigAdapter.Common.GetTriggerRangeOfNearSkill()
			local attacker_world_pos = attacker:GetWorldPosVec()
			if not attacker_world_pos:IsNearHorz(tgt:GetWorldPosVec(), trigger_range) then
				return false
			end

			--近战单体攻击检测
			if tgt_logic_info[2] == 0 and (attacker:IsHero() or attacker:IsMonster() or attacker:IsHeroPVP() or attacker:IsMonsterPVP()) then								
				if tgt_obj_id ~= CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstPNTgt", attacker_obj_id, skill_id) then
					return false
				end

				--添加待攻击近战碰撞对记录
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "AddToAttackPNObjsRecord", tgt_obj_id, attacker_obj_id, skill_id, skill_lev)
			end

		--3 范围(3|范围左边界|范围右边界，目标进入范围内释放)
		elseif tgt_type == Macros.Game.SkillTgtLogic.RANGE then
			if not check_in_range(attacker_obj_id, tgt:GetWorldPosVec(), tgt_logic_info[2], tgt_logic_info[3]) then
				return false
			end
		end
	end

	return true
end

--根据技能命中目标逻辑，搜索所有目标
function Skill.SkillLogic:SearchAllTgtBySkill(attacker_obj_id, skill_id, ignore_bullet, external_data, ignore_check_can_dobeattacked)
    local target_id_map

	local bullet_id, bullet_offset = ConfigAdapter.Skill.GetSkillBulletInfo(skill_id)
	if bullet_id and not ignore_bullet then
		--已配置弹道，过滤技能命中目标
		return target_id_map
	end

	local attacker = GlobalScene:GetObj(attacker_obj_id)
	if not attacker then
		return target_id_map
	end	

	local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(skill_id, ignore_bullet)
	if tgt_logic_info then
		local tgt_type = tgt_logic_info[1]
		--1 近战目标(1|类型|抬手距离|伤害距离，0=单体；1=群体)
		if tgt_type == Macros.Game.SkillTgtLogic.NEAR then
			local attacker_world_pos = attacker:GetWorldPosVec()
			local attack_range = tgt_logic_info[4] ~= 0 and tgt_logic_info[4] or ConfigAdapter.Common.GetCollisionRangeOfNearSkill()

			--单体
			if tgt_logic_info[2] == 0 then
				local target_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstPNTgt", attacker_obj_id, skill_id) or CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", attacker_obj_id, skill_id)
				local target = GlobalScene:GetObj(target_id)
				if target and (ignore_check_can_dobeattacked or target:CanDoBeAttacked()) and 
					attacker_world_pos:IsNearHorz(target:GetWorldPosVec(), attack_range) then
					target_id_map = target_id_map or {}
					target_id_map[target_id] = true
				end				

			--群体
			else
				local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, not ignore_check_can_dobeattacked)
				for _, vo in ipairs(target_list) do
					local target = GlobalScene:GetObj(vo.obj_id)
					if target and attacker_world_pos:IsNearHorz(target:GetWorldPosVec(), attack_range) then
						target_id_map = target_id_map or {}
						target_id_map[vo.obj_id] = true
					end
				end
			end

		--2 全体随机N个(2|N)
		elseif tgt_type == Macros.Game.SkillTgtLogic.RAND_IN_ALL then
			local target
			local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, not ignore_check_can_dobeattacked)
			target_list = MathTool.SelectRandomItemsFrmList(target_list, tgt_logic_info[2])
			for _, vo in ipairs(target_list) do
				target_id_map = target_id_map or {}
				target_id_map[vo.obj_id] = true
			end
		
		--3 范围(3|范围左边界|范围右边界，目标进入范围内释放)
		elseif tgt_type == Macros.Game.SkillTgtLogic.RANGE then
			local target, target_world_pos
			local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, not ignore_check_can_dobeattacked)
			for _, vo in ipairs(target_list) do
				target = GlobalScene:GetObj(vo.obj_id)
				if target and check_in_range(attacker_obj_id, target:GetWorldPosVec(), tgt_logic_info[2], tgt_logic_info[3]) then
					target_id_map = target_id_map or {}					
					target_id_map[vo.obj_id] = true
				end
			end
		
		--4 全体(4)
		elseif tgt_type == Macros.Game.SkillTgtLogic.ALL then
			local target
			local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, not ignore_check_can_dobeattacked)
			for _, vo in ipairs(target_list) do
				target = GlobalScene:GetObj(vo.obj_id)
				if target then
					target_id_map = target_id_map or {}
					target_id_map[vo.obj_id] = true
				end
			end

		--[[		
			5 特殊目标（5|选取目标|范围，范围配0表示特定目标）
	    --]]			
		elseif tgt_type == Macros.Game.SkillTgtLogic.SPECIAL then
			local target
			local target_list = self:__GetSpecialBattleTgtMapOfSkillTgtLogic(attacker_obj_id, skill_id, ignore_bullet, external_data)
			for _, target_vo in ipairs(target_list) do
				target = GlobalScene:GetObj(target_vo.obj_id)
				if target and (ignore_check_can_dobeattacked or target:CanDoBeAttacked(skill_id)) then
					target_id_map = target_id_map or {}
					target_id_map[target_vo.obj_id] = true
				end
			end			
		end
	end

	return target_id_map
end

--根据技能弹道逻辑，搜索所有目标
function Skill.SkillLogic:SearchAllTgtBySkillBullet(attacker_obj_id, skill_id)
    local target_id_map

	local bullet_id, bullet_offset = ConfigAdapter.Skill.GetSkillBulletInfo(skill_id)
	if not bullet_id then
		--无配置弹道，过滤弹道命中目标
		return target_id_map
	end    

	--弹道
	local config_bullet = CPPGameLib.GetConfig("SkillBullet", bullet_id)
	if not config_bullet then
		return target_id_map
	end

	--抛物线弹道(由释放弹道的技能命中目标逻辑决定)
	if config_bullet.parabola_factor then
		target_id_map = self:SearchAllTgtBySkill(attacker_obj_id, skill_id, true)
		return target_id_map
	end

	local range1 = bullet_offset[1]
	local range2 = bullet_offset[1] + config_bullet.move_distance

	local target
	local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, true)	
	if config_bullet.can_penetrate == Macros.Global.TRUE then
		--可穿透，搜索射程内所有目标
		for _, tgt_vo in ipairs(target_list) do
			target = GlobalScene:GetObj(tgt_vo.obj_id)
			if target and check_in_range(attacker_obj_id, target:GetWorldPosVec(), range1, range2) then
				target_id_map = target_id_map or {}
				target_id_map[tgt_vo.obj_id] = true
			end		
		end
	else
		--不可穿透，搜索射程内首个目标以及爆炸范围内所有目标
		local first_tgt_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", attacker_obj_id, skill_id)
		first_tgt = GlobalScene:GetObj(first_tgt_id)
		if first_tgt and first_tgt:CanDoBeAttacked() and
			check_in_range(attacker_obj_id, first_tgt:GetWorldPosVec(), range1, range2) then
			
			target_id_map = {}
			target_id_map[first_tgt_id] = true
			for _, tgt_vo in ipairs(target_list) do
				target = GlobalScene:GetObj(tgt_vo.obj_id)
				if target and check_in_range_ex(first_tgt:GetWorldPosVec(), target:GetWorldPosVec(), -config_bullet.attack_range, config_bullet.attack_range) then
					target_id_map[tgt_vo.obj_id] = true
				end
			end			
		end
	end

	return target_id_map
end

--根据技能命中目标逻辑/弹道逻辑，搜索所有目标
function Skill.SkillLogic:SearchAllTgt(attacker_obj_id, skill_id)
    local target_id_map = 
    	self:SearchAllTgtBySkill(attacker_obj_id, skill_id) or 
    	self:SearchAllTgtBySkillBullet(attacker_obj_id, skill_id)

    return target_id_map
end

--根据BUFF命中目标逻辑，搜索所有目标
function Skill.SkillLogic:SearchAllTgtByBuff(attacker_obj_id, skill_id, by_skill_tgt_logic, bless_sprite_buff_data, external_data)
	local target_id_map_list = {}

	local buff_tgt_logic_info_list 
	if not bless_sprite_buff_data then
		buff_tgt_logic_info_list = ConfigAdapter.Skill.GetBuffTargetLogicInfoList(skill_id)
	else
		buff_tgt_logic_info_list = bless_sprite_buff_data.buff_target
	end
	
	if buff_tgt_logic_info_list then
		for i, buff_tgt_logic_info in ipairs(buff_tgt_logic_info_list) do
			local target
			local target_id_map = {}

			local tgt_type = buff_tgt_logic_info[1]

			--根据技能命中目标逻辑
			if tgt_type == Macros.Game.BuffTgtLogic.BY_SKILL_TGT_LOGIC and by_skill_tgt_logic then
				target_id_map = self:SearchAllTgtBySkill(attacker_obj_id, skill_id, nil, external_data, true)

			--根据BUFF命中目标逻辑
			elseif tgt_type ~= Macros.Game.BuffTgtLogic.BY_SKILL_TGT_LOGIC and not by_skill_tgt_logic then
				--[[
				特殊敌方目标/特殊敌方范围（2|选取目标|范围，范围配0表示特定目标）
			    选取目标：
			        0 = 最近敌人
			        1 = 最远敌人
			        2 = 血量最少敌人
				--]]
				if tgt_type == Macros.Game.BuffTgtLogic.SP_ENEMY then
					local sp_enemy_id

					local sp_tgt_type = buff_tgt_logic_info[2]
					if sp_tgt_type == 0 or sp_tgt_type == 1 then
						local func_name = sp_tgt_type == 0 and "GetFirstEnemy" or "GetLastEnemy"
						sp_enemy_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, func_name, attacker_obj_id)

					elseif sp_tgt_type == 2 then
						local enemy_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyList", attacker_obj_id)
						if enemy_list then
							local hp_percent
							local min_hp_percent = MathTool.MAX_NUM
							for _, enemy_vo in ipairs(enemy_list) do
								target = GlobalScene:GetObj(enemy_vo.obj_id)
								if target then
									hp_percent = enemy_vo.hp / enemy_vo.max_hp									
									if hp_percent > 0 and hp_percent < min_hp_percent then
										sp_enemy_id = enemy_vo.obj_id
										min_hp_percent = hp_percent
									end
								end
							end
						end					
					end

					if sp_enemy_id then
						--以特殊敌人为目标
						target_id_map[sp_enemy_id] = true

						local sp_enemy = GlobalScene:GetObj(sp_enemy_id)

						--以特殊敌人特定范围内的其他敌人为目标
						local sp_enemy_range = buff_tgt_logic_info[3] * 0.5
						if sp_enemy and sp_enemy_range > 0 then
							local sp_enemy_pos = sp_enemy:GetWorldPosVec()

							local enemy, enemy_pos
							local enemy_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetEnemyList", attacker_obj_id)
							for _, enemy_vo in ipairs(enemy_list) do
								enemy = GlobalScene:GetObj(enemy_vo.obj_id)
								if enemy and enemy_vo.obj_id ~= sp_enemy_id then
									enemy_pos = enemy:GetWorldPosVec()
									if sp_enemy_pos:IsNearHorz(enemy_pos, sp_enemy_range) then
										target_id_map[enemy_vo.obj_id] = true
									end
								end
							end
						end
					end				

				--[[
				特殊我方范围（3|选取目标|范围，范围配0表示特定目标）
			    选取目标：
			        0 = 自身
			        1 = 最前队友
			        2 = 最后队友
			        3 = 血量最少队友
				--]]
				elseif tgt_type == Macros.Game.BuffTgtLogic.SP_TEAMMATE then
					local sp_teammate_id

					local sp_tgt_type = buff_tgt_logic_info[2]
					if sp_tgt_type == 0 then
						sp_teammate_id = attacker_obj_id

					elseif sp_tgt_type == 1 or sp_tgt_type == 2 then
						local func_name = sp_tgt_type == 1 and "GetFirstTeammate" or "GetLastTeammate"
						sp_teammate_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, func_name, attacker_obj_id)

					elseif sp_tgt_type == 3 then
						local teammate_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", attacker_obj_id)
						if teammate_list then
							local hp_percent
							local min_hp_percent = MathTool.MAX_NUM
							for _, teammate_vo in ipairs(teammate_list) do
								target = GlobalScene:GetObj(teammate_vo.obj_id)
								if target then	
									hp_percent = teammate_vo.hp / teammate_vo.max_hp
									if hp_percent > 0 and hp_percent < min_hp_percent then															
										sp_teammate_id = teammate_vo.obj_id
										min_hp_percent = hp_percent
									end
								end
							end
						end					
					end

					if sp_teammate_id then
						--以特殊队友为目标
						target_id_map[sp_teammate_id] = true

						local sp_teammate = GlobalScene:GetObj(sp_teammate_id)

						--以特殊队友特定范围内的其他队友为目标
						local sp_teammate_range = (buff_tgt_logic_info[3] or 0) * 0.5
						if sp_teammate and sp_teammate_range > 0 then
							local sp_teammate_pos = sp_teammate:GetWorldPosVec()

							local teammate, teammate_pos
							local teammate_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", attacker_obj_id)
							for _, teammate_vo in ipairs(teammate_list) do
								teammate = GlobalScene:GetObj(teammate_vo.obj_id)
								if teammate and teammate_vo.obj_id ~= sp_teammate_id then
									teammate_pos = teammate:GetWorldPosVec()
									if sp_teammate_pos:IsNearHorz(teammate_pos, sp_teammate_range) then
										target_id_map[teammate_vo.obj_id] = true
									end
								end
							end
						end
					end								

				--己方全体（4）
				elseif tgt_type == Macros.Game.BuffTgtLogic.ALL_TEAMMATE then
					local teammate_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", attacker_obj_id)
					for _, vo in ipairs(teammate_list) do
						target = GlobalScene:GetObj(vo.obj_id)
						if target then
							target_id_map[vo.obj_id] = true						
						end
					end

				--不指定目标的图腾BUFF（5）
				elseif tgt_type == Macros.Game.BuffTgtLogic.BUFF_TOTEMS_FOR_SPRITE then
					local sp_enemy_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstEnemy", attacker_obj_id)
					if sp_enemy_id then
						target_id_map[sp_enemy_id] = true
					end
				end
			end

			--记录第i个BUFF对应的目标列表
			table.insert(target_id_map_list, target_id_map)
		end
	end

	return target_id_map_list
end

--BUFF检测
function Skill.SkillLogic:CheckBuff(attacker_obj_id, skill_id, by_skill_tgt_logic, bless_sprite_buff_data, external_data)	
	local buff_info
	local buff_info_list
	if not bless_sprite_buff_data then
		buff_info_list = ConfigAdapter.Skill.GetBuffInfoList(skill_id)
	else
		buff_info_list = bless_sprite_buff_data.buff_list
	end
	if not buff_info_list then
		return
	end

	local attacker = GlobalScene:GetObj(attacker_obj_id)
	if not attacker then
		return
	end

	local is_pvp = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon")

	local target
	local target_id_map_list = self:SearchAllTgtByBuff(attacker_obj_id, skill_id, by_skill_tgt_logic, bless_sprite_buff_data, external_data)	
	for i, target_id_map in ipairs(target_id_map_list) do		
		buff_info = buff_info_list[i]
		if buff_info then
			for tgt_id, _ in pairs(target_id_map) do
				target = GlobalScene:GetObj(tgt_id)
				if target and MathTool.CheckProbability(buff_info[1]) then
					if is_pvp then
						if attacker:IsHeroPVP(true) then					        
							--PVP副本中，请求同步己方释放的BUFF
							CallModuleFunc(ModuleType.BATTLE, "BuffSyncReq_PVP", buff_info[2], attacker_obj_id, tgt_id, bless_sprite_buff_data and Macros.Global.TRUE or Macros.Global.FALSE)						
						end
					else
						GlobalBuffMgr:CreateBuff(buff_info[2], attacker:GetVO(), target:GetVO(), bless_sprite_buff_data ~= nil, external_data)					
					end
				end
			end
		end
	end
end

--计算攻击场景特效坐标
function Skill.SkillLogic:CalcAttackSceneEffectPos(attacker_obj_id, skill_id, target_id_map, external_data)
	local attacker = GlobalScene:GetObj(attacker_obj_id)
	if not attacker then
		return
	end

	local _
	local effect_x, effect_y

	local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(skill_id)
	if tgt_logic_info then
		local tgt_type = tgt_logic_info[1]

		--1(攻击者 + 伤害距离) / 2
		if tgt_type == Macros.Game.SkillTgtLogic.NEAR then
			if not target_id_map then
				--找不到目标的坐标处理
				local attacker_world_pos = attacker:GetWorldPosVec()
				effect_x = attacker_world_pos.x + tgt_logic_info[4] * attacker:GetDir()
				_, effect_y = GlobalScene:WorldToPixel(nil, attacker_world_pos.y)
			end						

		--3(范围左边界 + 范围右边界) / 2
		elseif tgt_type == Macros.Game.SkillTgtLogic.RANGE then
			local attacker_world_pos = attacker:GetWorldPosVec()
			local trigger_range_min, trigger_range_max
			tgt_logic_info[2] = tgt_logic_info[2] * attacker:GetDir()
			tgt_logic_info[3] = tgt_logic_info[3] * attacker:GetDir()
			trigger_range_min = math.min(tgt_logic_info[2], tgt_logic_info[3])
			trigger_range_max = math.max(tgt_logic_info[2], tgt_logic_info[3])			

			effect_x = attacker_world_pos.x + (trigger_range_min + trigger_range_max)/2
			_, effect_y = GlobalScene:WorldToPixel(nil, attacker_world_pos.y)
		end		
	end

	if not effect_x and not effect_y then
		--(目标左边界 + 目标右边界) / 2
		local mid_world_pos_x, mid_world_pos_y = Skill.SkillLogic:CalcMiddlePosForTargets(target_id_map)
		if mid_world_pos_x and mid_world_pos_y then
			effect_x, effect_y = GlobalScene:WorldToPixel(mid_world_pos_x, mid_world_pos_y)
		end

		--精灵技能的场景特效坐标，以选取点为准
		if external_data then
			effect_x = external_data.x
			if not effect_y then
				local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
				_, effect_y = GlobalScene:WorldToPixel(0, CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY"))
			end
		end		
	end

	return effect_x, effect_y	
end

--计算目标左右边界的中心点
function Skill.SkillLogic:CalcMiddlePosForTargets(target_id_map)
	local target_num = CPPGameLib.GetTableLength(target_id_map)
	if target_num > 0 then
		if target_num == 1 then
			local target_id = next(target_id_map)
			local target = GlobalScene:GetObj(target_id)
			if target then
				return target:GetWorldPos()
			end
		else	
			local min_x, max_x, min_y, max_y
			min_x = MathTool.MAX_NUM
			min_y = MathTool.MAX_NUM
			max_x = MathTool.MIN_NUM
			max_y = MathTool.MIN_NUM

			local target, target_world_pos
			for target_id, _ in pairs(target_id_map) do
				target = GlobalScene:GetObj(target_id)
				if target then
					target_world_pos = target:GetWorldPosVec()
					min_x = target_world_pos.x < min_x and target_world_pos.x or min_x
					max_x = target_world_pos.x > max_x and target_world_pos.x or max_x
					min_y = target_world_pos.y < min_y and target_world_pos.y or min_y
					max_y = target_world_pos.y > max_y and target_world_pos.y or max_y
				end
			end

			return (min_x + max_x) / 2, (min_y + max_y) / 2
		end
	end
end

--处理受击、伤害、治疗....
function Skill.SkillLogic:HandleBeAttacked(beattacked_info_list)
	local obj, hp_change_type, hp_change, config_skill
	local has_do_beattacked_record = {}
	local has_play_treatment_effect_record = {}

	for _, info in ipairs(beattacked_info_list) do
		obj = GlobalScene:GetObj(info.obj_id)
		if obj then
			config_skill = CPPGameLib.GetConfig("Skill", info.skill_id)

			--伤害/治疗计算
			hp_change_type, hp_change, damage_type = Modules.BattleFormula:Calc(info.attacker_obj_id, info.obj_id, info.skill_id, info.skill_lev, info.damage_time_index)

			--受击处理
			if hp_change_type == Modules.BattleFormula.HPChangeType.DAMAGE and not has_do_beattacked_record[info.obj_id] then
				obj:DoBeAttacked(info.attacker_obj_id, info.skill_id, info.skill_lev, info.damage_time_index)
				has_do_beattacked_record[info.obj_id] = true
			end

			--血量变化
			if hp_change_type ~= nil then
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoHpChange", info.obj_id, hp_change, damage_type)
			    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HPChangeSyncReq", Modules.BattleFormula.HPChangeSrc.SKILL, info.attacker_obj_id, info.obj_id, hp_change, damage_type)
			end

			--怒气变化
			if config_skill and config_skill.defender_anger_profit then
				obj:DoAngerChange(config_skill.defender_anger_profit)
			end

			--治疗特效
			if hp_change_type == Modules.BattleFormula.HPChangeType.TREATMENT and not has_play_treatment_effect_record[info.obj_id] then
				obj:PlayBeAttackedEffect(info.skill_id)
				obj:PlayBeAttackedSound(info.skill_id)
				has_play_treatment_effect_record[info.obj_id] = true
			end			
		end
	end
end

--获取合击怪物表
local function get_joint_monster_map(info_id)
	local config_monster = CPPGameLib.GetConfig("Monster", info_id)
	if not config_monster or not config_monster.joint_monster_list then
		return
	end
	
	local monster_map = {}
	for _, info_id in ipairs(config_monster.joint_monster_list) do
		monster_map[info_id] = true
	end
	return monster_map
end

--获取合击对象表
local function get_joint_obj_map(attacker_obj_id)
	local attacker_obj = GlobalScene:GetObj(attacker_obj_id)
	if not attacker_obj then
		return
	end

	local attacker_vo = attacker_obj:GetVO()
	if attacker_obj:IsHeroRelevant() then
		return ConfigAdapter.Hero.GetJointHeroMap(attacker_vo.info_id)
	end

	local config_monster = CPPGameLib.GetConfig("Monster", attacker_vo.info_id)
	if config_monster then
		if config_monster.hero_id and config_monster.hero_id > 0 then
			return ConfigAdapter.Hero.GetJointHeroMap(config_monster.hero_id)
		else
			return get_joint_monster_map(attacker_vo.info_id)
		end
	end	
end

--检测合击技能释放条件
function Skill.SkillLogic:CheckJointSkillCondition(attacker_obj_id)
	local attacker_obj = GlobalScene:GetObj(attacker_obj_id)
	if not attacker_obj then
		return false
	end

	local attacker_obj_vo = attacker_obj:GetVO()

	--检测合击对象
	local joint_obj_map = get_joint_obj_map(attacker_obj_id)
	if not joint_obj_map then
		return false
	end

	local joint_info_id
	local joint_obj_num = 0
	local teammate_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", attacker_obj_id)
	if teammate_list then
		local teammate_obj
		for _, teammate_vo in ipairs(teammate_list) do
			teammate_obj = GlobalScene:GetObj(teammate_vo.obj_id)
			joint_info_id = ObjIDTool.GetBattleInfoID(teammate_vo.obj_id, false)	

			--副将处于死亡、受限类BUFF(眩晕、冰冻、石化、沉默、放逐)，则不满足合击技能条件
			if joint_obj_map[joint_info_id] and 
				teammate_obj and
				not teammate_obj:IsDead() and 
				not teammate_obj:IsForbidAttack() and
				not teammate_obj:IsForbidSpecialSkill() and
				not teammate_obj:IsForbidAngerOrJointSkill() then
				joint_obj_num = joint_obj_num + 1
			end			
		end
	end	

	return joint_obj_num >= CPPGameLib.GetTableLength(joint_obj_map)	
end

--获取合击技能的副将
function Skill.SkillLogic:GetAttachedObjsOfJointSkill(attacker_obj_id)
	local attacker_obj = GlobalScene:GetObj(attacker_obj_id)
	if not attacker_obj then
		return
	end

	local attacker_obj_vo = attacker_obj:GetVO()

	--检测合击对象
	local joint_obj_map = get_joint_obj_map(attacker_obj_id)
	if not joint_obj_map then
		return false
	end

	local joint_info_id
	local joint_attached_obj_map
	local teammate_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateList", attacker_obj_id)
	if teammate_list then
		local teammate_obj
		for _, teammate_vo in ipairs(teammate_list) do
			teammate_obj = GlobalScene:GetObj(teammate_vo.obj_id)
			if teammate_obj then
				joint_info_id = ObjIDTool.GetBattleInfoID(teammate_vo.obj_id, false)
				
				if joint_obj_map[joint_info_id] and 
					teammate_obj and not teammate_obj:IsDead() and 
					teammate_vo:GetJointAttachedSkillID() > 0 then

					joint_attached_obj_map = joint_attached_obj_map or {}
					joint_attached_obj_map[teammate_vo.obj_id] = true
				end			
			end
		end
	end	
	return joint_attached_obj_map
end

--获取攻击动作索引
function Skill.SkillLogic:GetAttackActIndex(skill_id)
	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if config_skill and config_skill.action_index then
		return config_skill.action_index
	end

	local act_index
	
	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)
	if skill_type == Macros.Game.SkillType.NORMAL then
		act_index = RenderObj.ActionIndex.ATTACK

	elseif skill_type == Macros.Game.SkillType.NEAR then
		act_index = RenderObj.ActionIndex.ATTACK_NEAR

	elseif skill_type == Macros.Game.SkillType.SPECIAL then
		act_index = RenderObj.ActionIndex.SP_SKILL

	elseif skill_type == Macros.Game.SkillType.ANGER or
		skill_type == Macros.Game.SkillType.ANGER_PLUS then
		act_index = RenderObj.ActionIndex.ANGER_SKILL

	elseif skill_type == Macros.Game.SkillType.JOINT or 
		skill_type == Macros.Game.SkillType.JOINT_PLUS or
		skill_type == Macros.Game.SkillType.JOINT_ATTACHED then
		act_index = RenderObj.ActionIndex.JOINT_SKILL
	end

	return act_index
end

--触发弹道
function Skill.SkillLogic:TriggerBullet(create_obj_id, skill_id, skill_lev, bullet_index, bullet_num)
	local create_obj = GlobalScene:GetObj(create_obj_id)
	if not create_obj then
		return
	end

	local create_obj_pos = create_obj:GetWorldPosVec()
	local dir = create_obj:GetDir()

	local bullet_id, bullet_offset = ConfigAdapter.Skill.GetSkillBulletInfo(skill_id, bullet_index)
	if not bullet_id then
		return
	end

	local config_bullet = CPPGameLib.GetConfig("SkillBullet", bullet_id)
	if not config_bullet then
		return
	end

	local do_once_check_buff
	local buff_tgt_logic_info_list = ConfigAdapter.Skill.GetBuffTargetLogicInfoList(skill_id, skill_lev)
	if buff_tgt_logic_info_list and buff_tgt_logic_info_list[1] then
		local tgt_type = buff_tgt_logic_info_list[1][1]
		do_once_check_buff = tgt_type ~= Macros.Game.BuffTgtLogic.BY_SKILL_TGT_LOGIC
	end

	--创建弹道
	local function create_bullet_func(parabola_tgt_id, can_check_buff)
		local vo = Scene.BulletVO.New()
		vo:ParseInfo(bullet_id)
		vo.create_obj_id = create_obj_id
		vo.create_skill_id = skill_id
		vo.create_skill_lev = skill_lev
		vo.org_pos_x, vo.org_pos_y = GlobalScene:WorldToLogic(create_obj_pos.x + bullet_offset[1] * dir, create_obj_pos.y + bullet_offset[2])
		vo.dst_pos_x, vo.dst_pos_y = GlobalScene:WorldToLogic(create_obj_pos.x + (bullet_offset[1] + vo.move_distance * ( 4 / 3 ) ) * dir, create_obj_pos.y + bullet_offset[2])
		if vo.parabola_factor and parabola_tgt_id then
			vo.parabola_tgt_id = parabola_tgt_id
		else
			vo.parabola_factor = nil
		end
		vo.can_check_buff = can_check_buff
		vo.do_once_check_buff = do_once_check_buff
		GlobalScene:CreateBullet(vo)	
	end

	if config_bullet.parabola_factor then
		--抛物线弹道
		--搜索目标(由释放弹道的技能命中目标逻辑决定)
		local target_id_map = create_obj:GetAttackTargets()
		if target_id_map then
			local can_check_buff
			local target_index = 0
			local target_num = CPPGameLib.GetTableLength(target_id_map)
			for tgt_id, _ in pairs(target_id_map) do
				target_index = target_index + 1
				if target_index == target_num then
					can_check_buff = true
				end
				create_bullet_func(tgt_id, can_check_buff)
			end
		end
	else
		--非抛物线弹道
		create_bullet_func(nil, bullet_index == bullet_num)
	end
end

--[[
	创建技能特殊逻辑对象
--]]
function Skill.SkillLogic:CreateSpecialSkillLogic(battle_obj, skill_id, type)
	type = type or Skill.SpSkillLogicType.ATTACK

	if not Skill.SkillLogic.SpecialSkillClassMap then
		Skill.SkillLogic.SpecialSkillClassMap = 
		{	
			--怒气技能
			[Macros.Game.SkillType.ANGER] = Skill.SpecialSkillAngerJoint,

			--真.怒气技能
			[Macros.Game.SkillType.ANGER_PLUS] = Skill.SpecialSkillAngerJoint,

			--合击技能(主将)
			[Macros.Game.SkillType.JOINT] = Skill.SpecialSkillAngerJoint,

			--真.合击技能
			[Macros.Game.SkillType.JOINT_PLUS] = Skill.SpecialSkillAngerJoint,

			--合击技能(副将)
			[Macros.Game.SkillType.JOINT_ATTACHED] = Skill.SpecialSkillAngerJoint,

			--精灵技能
			[Macros.Game.SkillType.SPRITE] = Skill.SpecialSkillSprite,
		}
	end

	local skill_type = ConfigAdapter.Skill.GetSkillType(skill_id)
	local sp_skill_class = Skill.SkillLogic.SpecialSkillClassMap[skill_type] or Skill.SpecialSkillBase
	return sp_skill_class.New(battle_obj, type)
end

--[[
	生成震动信息列表
--]]
function Skill.SkillLogic:GenShakeInfoList(config_shake_info_list, for_bullet)
	local shake_info_list
	
	if config_shake_info_list then
		for _, config_shake_info in ipairs(config_shake_info_list) do
			local shake_info = {}
			if for_bullet then
				shake_info.start_time = Macros.Global.NowTime
				shake_info.end_time = shake_info.start_time + config_shake_info[1]
				shake_info.type = config_shake_info[2]
				shake_info.range = config_shake_info[3]
				shake_info.frequency = config_shake_info[4]
				shake_info.pass_time = shake_info.start_time
			else
				shake_info.start_time = config_shake_info[1]
				shake_info.end_time = config_shake_info[2]
				shake_info.type = config_shake_info[3]
				shake_info.range = config_shake_info[4]
				shake_info.frequency = config_shake_info[5]
				shake_info.pass_time = 0
			end
			shake_info.triggered = false

			shake_info_list = shake_info_list or {}
			table.insert(shake_info_list, shake_info)
		end
	end

	return shake_info_list
end

--[[
	检测震动
--]]
function Skill.SkillLogic:CheckShake(shake_info_list, elapse_time)
	if shake_info_list then
		for _, shake_info in ipairs(shake_info_list) do
			if not shake_info.triggered then
				shake_info.pass_time = shake_info.pass_time + elapse_time
				if shake_info.pass_time >= shake_info.start_time then
					shake_info.triggered = true
					GlobalScene:CameraShake(shake_info.type, shake_info.range, shake_info.frequency, shake_info.end_time - shake_info.start_time)
				end
			end
		end
	end
end

--[[
	检测怒气/合击技能的自动释放
--]]
function Skill.SkillLogic:CheckAutoReleaseAngerOrJointSkill(is_enemy)
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllObjsEnterRegion") then
		return
	end

	local is_pvp = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon")

	local function handle_func(obj_map, for_check)
		local obj
		local has_trigger
		for obj_id, vo in pairs(obj_map) do
			obj = GlobalScene:GetObj(obj_id)
			if obj and not vo.ai_list then
				--检测
				if for_check then
					if not obj:IsAngerEnough() then
						--清空怒气/合击待释放标记
						obj:ShutdownAngerOrJointSkill()
					end
					if not obj:IsAngerEnough(true) then
						--清空强化版怒气/合击待释放标记
						obj:ShutdownAngerOrJointSkill(true)
					end						
					if obj:IsAngerOrJointSkillTriggered() then						
						--触发中
						has_trigger = true				
					end

				--触发
				elseif self:CheckIsCouldReleaseAngerOrJointSkill(vo, is_enemy) then
					has_trigger = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "TriggerAngerOrJointSkill", obj_id, obj:IsAngerEnough(true))
					if has_trigger then
						break
					end
				end
			end
		end
		return has_trigger
	end

	if not is_enemy then
		--英雄
		if not handle_func(GlobalVOMgr:GetHeroMap(), true) then
			if not is_pvp and CallModuleFunc(ModuleType.BATTLE, "GetAutoFight") then
				--非PVP，自动释放怒气/合击技能
				handle_func(GlobalVOMgr:GetHeroMap())
			end
		end
	else
		--怪物
		if not handle_func(GlobalVOMgr:GetMonsterMap(), true) then
			if not is_pvp then
				--非PVP或PVP中打机器人，自动释放怒气/合击技能
				handle_func(GlobalVOMgr:GetMonsterMap())
			end
		end
	end
end

--获取怒气/合击技能ID
function Skill.SkillLogic:GetAngerOrJointSkillID(obj_id, is_plus)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj or obj:IsDead() then
		return
	end
	local vo = obj:GetVO()

	if is_plus then
		if vo.skill_id_of_joint_plus_skill and Skill.SkillLogic:CheckJointSkillCondition(obj_id) then
			--真.合击技能
			return vo.skill_id_of_joint_plus_skill
		else
			--真.怒气技能
			return vo.skill_id_of_anger_plus_skill
		end
	else
		if vo.skill_id_of_joint_skill and Skill.SkillLogic:CheckJointSkillCondition(obj_id) then
			--合击技能
			return vo.skill_id_of_joint_skill
		else
			--怒气技能
			return vo.skill_id_of_anger_skill
		end
	end
end

--检测精灵祝福BUFF
function Skill.SkillLogic:CheckSpriteBlessBuff(trigger_mode, data, obj_vo)
	local bless_sprite_info = obj_vo.bless_sprite_info
	if not bless_sprite_info or bless_sprite_info.id <= 0 then
		return
	end

	local config_sprite_sp_prop = ConfigAdapter.Spirit.GetSpecialPropertyInfo(bless_sprite_info.id, bless_sprite_info.star)
	if not config_sprite_sp_prop or not config_sprite_sp_prop.bless_skill_trigger_mode then
		return
	end

	if bless_sprite_info.next_check_time and bless_sprite_info.next_check_time > Macros.Global.NowTime then
		--CD中
		return
	end

	for _, trigger_mode_info in ipairs(config_sprite_sp_prop.bless_skill_trigger_mode) do
		if trigger_mode_info[1] == trigger_mode then
			local is_triggered, skill_id_for_skill_tgt_logic

			CPPGameLib.Switch(trigger_mode)
			{
				--1|技能类型|触发几率=使用对应类型且作用于敌方的技能时触发	
				[Macros.Game.SpiritBlessSkillTriggerMode.ATTACK_WITH_SKILL] = function()
					local config_skill = CPPGameLib.GetConfig("Skill", data.skill_id)
					if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.ENEMY and config_skill.type == trigger_mode_info[2] and MathTool.CheckProbability(trigger_mode_info[3]) then
						is_triggered = true
						skill_id_for_skill_tgt_logic = data.skill_id
					end
				end,

				--2|技能类型|触发几率=受到对应类型且作用于敌方的技能时触发	
				[Macros.Game.SpiritBlessSkillTriggerMode.BE_ATTACKED_BY_SKILL] = function()
					local config_skill = CPPGameLib.GetConfig("Skill", data.skill_id)
					if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.ENEMY and config_skill.type == trigger_mode_info[2] and MathTool.CheckProbability(trigger_mode_info[3]) then
						is_triggered = true
					end
				end,

				--3|技能类型|触发几率=使用对应类型且作用于友方的技能时触发
				[Macros.Game.SpiritBlessSkillTriggerMode.TREAT_WITH_SKILL] = function()
					local config_skill = CPPGameLib.GetConfig("Skill", data.skill_id)
					if config_skill and config_skill.camp == Macros.Game.SkillTgtCamp.FRIEND and config_skill.type == trigger_mode_info[2] and MathTool.CheckProbability(trigger_mode_info[3]) then
						is_triggered = true
						skill_id_for_skill_tgt_logic = data.skill_id						
					end
				end,

				--4|buff类型|触发几率=受到对应类型的buff命中时触发
				[Macros.Game.SpiritBlessSkillTriggerMode.BE_ATTACHED_BY_BUFF] = function()
					local config_buff = CPPGameLib.GetConfig("SkillBuff", data.buff_id)
					if config_buff and config_buff.type == trigger_mode_info[2] and MathTool.CheckProbability(trigger_mode_info[3]) then
						is_triggered = true
					end
				end,

				--5|触发几率=回血时触发
				[Macros.Game.SpiritBlessSkillTriggerMode.BE_TREATED] = function()
					if MathTool.CheckProbability(trigger_mode_info[2]) then
						is_triggered = true
					end
				end,				
			}

			--触发BUFF
			if is_triggered then
				print("skfb", string.format("触发精灵祝福BUFF 祝福精灵ID:%d 祝福精灵星级:%d", bless_sprite_info.id, bless_sprite_info.star))
				CPPGameLib.PrintTable("skfb", config_sprite_sp_prop.buff_list, "buff_list")

				bless_sprite_info.next_check_time = Macros.Global.NowTime + (config_sprite_sp_prop.cd_time or 0)

				local bless_sprite_buff_data = 
				{
					buff_list = config_sprite_sp_prop.buff_list,
					buff_target = config_sprite_sp_prop.buff_target
				}
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCheckBuffCurFrame", obj_vo.obj_id, nil, nil, bless_sprite_buff_data)
				if skill_id_for_skill_tgt_logic then
					CallModuleFunc(ModuleType.SP_LOGIC_MGR, "RecordCheckBuffCurFrame", obj_vo.obj_id, skill_id_for_skill_tgt_logic, true, bless_sprite_buff_data)
				end
				break
			end
		end
	end
end

function Skill.SkillLogic:CheckIsCouldReleaseAngerOrJointSkill(vo, is_enemy)
	local can_trigger
	if vo.profession == Macros.Game.HeroProfession.ASSIST then
		--治疗类怒气技能自动释放条件：剩余总血量/存活英雄的总血量>配置值
		local radio_value = ConfigAdapter.Common.GetReleaseHealingSkillHPRadioValue()
		if (is_enemy and GlobalVOMgr:CalcMonsterCurHPRatio() < radio_value) or GlobalVOMgr:CalcHeroCurHPRatio() < radio_value then
			can_trigger = true
		end
	else
		--自动释放怒气/合击技能
		can_trigger = true
	end	

	return can_trigger
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--查找追击点
function Skill.SkillLogic:__FindChasePos(obj_id, tgt_id, range1, range2)
	local tgt = GlobalScene:GetObj(tgt_id)
	if not tgt then
		return nil
	end
	local tgt_world_pos = tgt:GetWorldPosVec()

	local me = GlobalScene:GetObj(obj_id)
	local my_world_pos = me:GetWorldPosVec()

	if check_in_range(obj_id, tgt_world_pos, range1, range2) then
		--在触发范围内，无须追击
		return my_world_pos
	end

	--根据目标位置计算方向，避免错位导致无法找到追击点
	local dir = MathTool.CalcHorzDirBetweenPoints(my_world_pos, tgt_world_pos)
	range1 = range1 * dir
	range2 = range2 * dir
	local trigger_range_min = math.min(range1, range2)
	local trigger_range_max = math.max(range1, range2)

	--目标在右侧
	if dir == MathTool.HORZ_RIGHT then
		if tgt_world_pos.x < my_world_pos.x + trigger_range_min then
			--目标处于近身无效范围内
			return nil
		end
		return Vector2D.New(tgt_world_pos.x - trigger_range_max, my_world_pos.y)

	--目标在左侧
	else
		if tgt_world_pos.x > my_world_pos.x + trigger_range_max then
			--目标处于近身无效范围内
			return nil
		end
		return Vector2D.New(tgt_world_pos.x - trigger_range_min, my_world_pos.y)
	end
end

--获取技能目标逻辑中的特殊目标列表
--[[
	5 特殊目标（5|选取目标|范围，范围配0表示特定目标）
	    1 生命值最低(5|1)
	    2 最后排1个(5|2)
	    3 职业(5|3|职业类型)
	    4 最前排1个(5|4)
		5 随机范围(5|5|随机点类型|范围)
		  随机点类型：0当前镜头内随机，1施法者到对面边界内随机，2外部输入随机点
--]]
function Skill.SkillLogic:__GetSpecialBattleTgtMapOfSkillTgtLogic(attacker_obj_id, skill_id, ignore_bullet, external_data)
	local target
	local sp_target_id, sp_target_pos
	local sp_target_id_list = {}

	local tgt_logic_info = ConfigAdapter.Skill.GetSkillTargetLogicInfo(skill_id, ignore_bullet)
	if tgt_logic_info then
		--生命值最低(5|1)
		if tgt_logic_info[2] == 1 then
			local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, true)
			local hp_percent
			local min_hp_percent = MathTool.MAX_NUM
			for _, tgt_vo in ipairs(target_list) do
				hp_percent = tgt_vo.hp / tgt_vo.max_hp
				if hp_percent > 0 and hp_percent < min_hp_percent then
					sp_target_id = tgt_vo.obj_id
					min_hp_percent = hp_percent
				end
			end

		--最后排1个(5|2)
		elseif tgt_logic_info[2] == 2 then
			sp_target_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetLastBattleTgt", attacker_obj_id, skill_id)

		--职业(5|3|职业类型)
		elseif tgt_logic_info[2] == 3 then
			local target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, true)			
			for _, tgt_vo in ipairs(target_list) do
				if tgt_vo.profession == tgt_logic_info[3] then
					table.insert(sp_target_id_list, tgt_vo)
				end
			end
			return sp_target_id_list
		
		--最前排1个(5|4)
		elseif tgt_logic_info[2] == 4 then
			sp_target_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", attacker_obj_id, skill_id)

		--随机范围(5|5|随机点类型)
		--随机点类型：0当前镜头内随机，1施法者到对面边界内随机，2外部输入随机点
		elseif tgt_logic_info[2] == 5 then
			local random_point_type = tgt_logic_info[3]

			--镜头内随机
			if random_point_type == 0 then
				local camera_bounding = GlobalScene:GetCameraBounding()
				local random_x = MathTool.GetRandom(camera_bounding.org.x, camera_bounding.org.x + camera_bounding.size.width - 1)
				sp_target_pos = Vector2D.New(random_x, 0)

			--施法者到对面边界内随机
			elseif random_point_type == 1 then
				local camera_bounding = GlobalScene:GetCameraBounding()
				local random_x

				local attacker = GlobalScene:GetObj(attacker_obj_id)
				if attacker then
					local attacker_pos = attacker:GetWorldPosVec()
					local attacker_dir = attacker:GetDir()
					random_x = MathTool.GetRandom(
						attacker_dir == MathTool.HORZ_RIGHT and attacker_pos.x or camera_bounding.org.x, 
						attacker_dir == MathTool.HORZ_RIGHT and (camera_bounding.org.x + camera_bounding.size.width - 1) or attacker_pos.x)
				else
					random_x = MathTool.GetRandom(camera_bounding.org.x, camera_bounding.org.x + camera_bounding.size.width - 1)
				end
				sp_target_pos = Vector2D.New(random_x, 0)

			--外部输入随机点
			else
				sp_target_pos = external_data
			end
		end
	end

	local sp_target = GlobalScene:GetObj(sp_target_id)
	if sp_target then
		sp_target_pos = sp_target:GetWorldPosVec()

		--以特定对象为目标
		table.insert(sp_target_id_list, sp_target:GetVO())
	end

	if sp_target_pos then
		--以特定对象特定范围内的其他对象为目标
		local sp_range = tgt_logic_info[#tgt_logic_info]
		if sp_range and sp_range > 0 then
			sp_range = sp_range * 0.5

			local ot_target, ot_target_pos
			local ot_target_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetBattleTgtList", attacker_obj_id, skill_id, true)
			for _, ot_tgt_vo in ipairs(ot_target_list) do
				ot_target = GlobalScene:GetObj(ot_tgt_vo.obj_id)				
				if ot_target and ot_tgt_vo.obj_id ~= sp_target_id then
					ot_target_pos = ot_target:GetWorldPosVec()
					if sp_target_pos:IsNearHorz(ot_target_pos, sp_range) then
						table.insert(sp_target_id_list, ot_tgt_vo)
					end
				end
			end
		end		
	end

	return sp_target_id_list
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
check_in_range = function(attacker_obj_id, tgt_world_pos, range1, range2, error_delta)
	local attacker = GlobalScene:GetObj(attacker_obj_id)
	if not attacker then
		return false
	end	
	local my_world_pos = attacker:GetWorldPosVec()

	--范围检测误差范围
	error_delta = error_delta or 0

	range1 = range1 * attacker:GetDir()
	range2 = range2 * attacker:GetDir()
	local trigger_range_min = math.min(range1, range2)
	local trigger_range_max = math.max(range1, range2)
	if tgt_world_pos.x < my_world_pos.x + trigger_range_min - error_delta or tgt_world_pos.x > my_world_pos.x + trigger_range_max + error_delta then
		return false
	end
	return true
end

check_in_range_ex = function(my_world_pos, world_pos, range1, range2)
	local trigger_range_min = math.min(range1, range2)
	local trigger_range_max = math.max(range1, range2)
	if world_pos.x < my_world_pos.x + trigger_range_min or world_pos.x > my_world_pos.x + trigger_range_max then
		return false
	end
	return true
end
