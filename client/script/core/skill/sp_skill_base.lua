
--[[
%% @module: 技能特殊逻辑基类
%% @author: swordman sue
%% @created: 2016-11-25
--]]

Skill = Skill or {}

Skill.SpecialSkillBase = Skill.SpecialSkillBase or BaseClass()

function Skill.SpecialSkillBase:__init(battle_obj, type)
	self.__battle_obj = battle_obj
	self.__vo = self.__battle_obj:GetVO()

	if type == Skill.SpSkillLogicType.ATTACK then
		self.__s_attack_state = self.__battle_obj.__s_attack_state

	elseif type == Skill.SpSkillLogicType.BEATTACKED then
		self.__s_beattacked_state = self.__battle_obj.__s_beattacked_state
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--攻击状态
--------------------------------------------------------------------------------------------------------------------------------
function Skill.SpecialSkillBase:DoAttack(skill_id, skill_lev)		
	--处理怒气改变
	self.__s_attack_state.has_handle_anger_change = false
	self:HandleAngerChange()

	--进入攻击状态
	self.__battle_obj:ChangeState("attack", true)
end

function Skill.SpecialSkillBase:EnterStateAttack(state_name)
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
	self.__s_attack_state.shake_list = Skill.SkillLogic:GenShakeInfoList(config_skill and config_skill.shake_info)

	--进入不可打断的攻击状态，移除待攻击近战碰撞对记录
	if self.__battle_obj:IsInUnInterruptableAttack() then
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DelToAttackPNObjsRecord", self.__vo.obj_id)
	end
end

function Skill.SpecialSkillBase:UpdateStateAttack(state_name, elapse_time)
	Skill.SkillLogic:CheckShake(self.__s_attack_state.shake_list, elapse_time)
end

function Skill.SpecialSkillBase:QuitStateAttack(state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--受击状态
--------------------------------------------------------------------------------------------------------------------------------
function Skill.SpecialSkillBase:DoBeAttacked(attacker_obj_id, skill_id, skill_lev, damage_time_index)
	--受击音效
	self.__battle_obj:PlayBeAttackedSound(skill_id)

	--播放受击特效
	self.__battle_obj:PlayBeAttackedEffect(skill_id)	

	--播放怪物受击特效
	if self.__battle_obj:IsMonster() then
		self.__battle_obj:PlayBeAttackedEffectEx(self.__vo.beattacked_effect_id)
	end

	--检测精灵祝福BUFF
	local data = {}
	data.skill_id = skill_id
	data.skill_lev = skill_lev
	Skill.SkillLogic:CheckSpriteBlessBuff(Macros.Game.SpiritBlessSkillTriggerMode.BE_ATTACKED_BY_SKILL, data, self.__vo)

	--击退
	self.__battle_obj:DoRepel(attacker_obj_id, skill_id, skill_lev)
end

function Skill.SpecialSkillBase:EnterStateBeAttacked(state_name)
end

function Skill.SpecialSkillBase:UpdateStateBeAttacked(state_name, elapse_time)
	if self.__s_beattacked_state and self.__s_beattacked_state.is_anim_end then
		--恢复受击者状态
		if self.__battle_obj.is_real_dead then
			self.__battle_obj:DoDead()
		else
			self.__battle_obj:DoIdle()
		end
	end
end

function Skill.SpecialSkillBase:QuitStateBeAttacked(state_name)
end

--处理怒气改变
function Skill.SpecialSkillBase:HandleAngerChange(for_do_damage)
	if self.__s_attack_state.has_handle_anger_change then
		return
	end

	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_attack_state.skill_id)
	if not config_skill or not config_skill.attacker_anger_profit then
		return
	end

	local flag = (not self.__vo:IsProfessionNear() and config_skill.type == Macros.Game.SkillType.NEAR) or 
	(self.__vo:IsProfessionNear() and (config_skill.type == Macros.Game.SkillType.NORMAL or config_skill.type == Macros.Game.SkillType.SPECIAL))

	if not for_do_damage then
		--一般情况下，进入战斗状态后触发怒气加成
		if not flag then
			self.__s_attack_state.has_handle_anger_change = true
			self.__battle_obj:DoAngerChange(config_skill.attacker_anger_profit)
		end
	else
		--近战职业的普通、特殊技能，远程职业的近身技能，对敌人造成伤害后触发怒气加成
		if flag then
			self.__s_attack_state.has_handle_anger_change = true
			self.__battle_obj:DoAngerChange(config_skill.attacker_anger_profit)
		end
	end
end

function Skill.SpecialSkillBase:ResumeExpand()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--处理受击动作
function Skill.SpecialSkillBase:__HandleBeAttackedAnim()
	local act_index = RenderObj.ActionIndex.HIT
	local config_skill = CPPGameLib.GetConfig("Skill", self.__s_beattacked_state.skill_id)
	if config_skill then
		local damage_anim_list = config_skill.damage_anim_list
		local damage_anim_index = damage_anim_list and damage_anim_list[self.__s_beattacked_state.damage_time_index] or nil
		if damage_anim_index then
			--伤害触发的动作
			act_index = damage_anim_index

		elseif config_skill.float_info then
			--击飞动作
			act_index = RenderObj.ActionIndex.HIT_FLY
		end
	end

	local attacker = GlobalScene:GetObj(self.__s_beattacked_state.attacker_obj_id)
	if not attacker or not attacker:IsInAngerOrJointSkill() then
		local body_anim_play_info = self.__battle_obj:GetBodyAnimPlayInfo()
		body_anim_play_info.end_callback = function()
			if not self.__s_beattacked_state then
				return
			end
			self.__s_beattacked_state.is_anim_end = true
		end	
	end
	self.__battle_obj:PlayAnimation(act_index, false)
end