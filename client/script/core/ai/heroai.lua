
--[[
%% @module: 英雄AI
%% @author: swordman sue
%% @created: 2016-07-18
--]]

AI = AI or {}

AI.HeroAI = AI.HeroAI or BaseClass(AI.BaseAI)

function AI.HeroAI:__init(params)
	self.__type = AI.Type.HERO
end

function AI.HeroAI:__delete()
	self.__anger_or_joint_common_cd_info = nil
end

--触发怒气/合击技能
function AI.HeroAI:TriggerAngerOrJointSkill(is_plus)
	if self.__anger_or_joint_common_cd_info.in_cd then
		return
	end

	local my_vo = self:__GetMyVO()
	if my_vo.skill_id_of_anger_skill and not is_plus then
		self.__anger_or_joint_skill_triggered = true
		return true

	elseif my_vo.skill_id_of_anger_plus_skill and is_plus then
		self.__anger_or_joint_plus_skill_triggered = true
		return true
	end
end

--触发QTE
function AI.HeroAI:TriggerAngerOrJointQTE()
	self.__anger_or_joint_qte_triggered = true
	self:__ReleaseSkill()
end

function AI.HeroAI:ShutdownAngerOrJointSkill(is_plus)
	if not is_plus then
		self.__anger_or_joint_skill_triggered = nil
	else
		self.__anger_or_joint_plus_skill_triggered = nil
	end
end

--检测怒气/合击技能开关
function AI.HeroAI:IsAngerOrJointSkillTriggered()
	return self.__anger_or_joint_skill_triggered or self.__anger_or_joint_plus_skill_triggered
end

--检测QTE开关
function AI.HeroAI:IsSkillQTETriggered()
	return self.__anger_or_joint_qte_triggered
end

function AI.HeroAI:GetAngerOrJointCommonCDInfo()
	return self.__anger_or_joint_common_cd_info
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function AI.HeroAI:__MyInit()
	if not AI.BaseAI.__MyInit(self) then
		return
	end

	local my_vo = self:__GetMyVO()

	--怒气/合击技能间的公共
	self.__anger_or_joint_common_cd_info = {}
	self.__anger_or_joint_common_cd_info.in_cd = false
	self.__anger_or_joint_common_cd_info.cd_time = my_vo.anger_or_joint_common_cd_time
	self.__anger_or_joint_common_cd_info.cd_elapse_time = 0

	return true
end

--选择技能
function AI.HeroAI:__SelectSkill()
	if self:IsAngerOrJointSkillTriggered() then
		return
	end

	AI.BaseAI.__SelectSkill(self)
end

--释放技能
--[[
释放怒气技能进入QTE状态条件：
	1.英雄
	2.未触发QTE
	3.非自动战斗
]]--
function AI.HeroAI:__ReleaseSkill()
	local me = self:__GetMe()
	local my_vo = self:__GetMyVO()

	--触发怒气/合击技能
	if self:IsAngerOrJointSkillTriggered() then
		self.__next_skill_id = self:__GetAngerOrJointSkillID()
	end

	--重置怒气/合击技能开关，公共CD
	if self.__next_skill_id == my_vo.skill_id_of_anger_skill or 
		self.__next_skill_id == my_vo.skill_id_of_joint_skill then
		self.__anger_or_joint_skill_triggered = nil
		self.__anger_or_joint_plus_skill_triggered = nil
		self.__anger_or_joint_qte_triggered = nil
		self:__SetAngerOrJointCommonCD()
	
	elseif self.__next_skill_id == my_vo.skill_id_of_anger_plus_skill or 
		self.__next_skill_id == my_vo.skill_id_of_joint_plus_skill then
		self.__anger_or_joint_skill_triggered = nil
		self.__anger_or_joint_plus_skill_triggered = nil
		self.__anger_or_joint_qte_triggered = nil
		self:__SetAngerOrJointCommonCD()
	end

	AI.BaseAI.__ReleaseSkill(self)
end

--根据技能搜索目标
function AI.HeroAI:__SearchTarget()
	if self:IsAngerOrJointSkillTriggered() then
		return
	end

	AI.BaseAI.__SearchTarget(self)
end

--攻击检测
function AI.HeroAI:__CheckToAttack()
	local next_skill_id = self:IsAngerOrJointSkillTriggered() and self:__GetAngerOrJointSkillID() or self.__next_skill_id
	if not next_skill_id then
		return false
	end

    local me = self:__GetMe()
    if not me:CanDoAttack(next_skill_id) then
    	return false
    end

	if self:IsAngerOrJointSkillTriggered() then
		if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") then
			--有怒气/合击技能释放中，过滤
			return false
		end
		local first_tgt_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetFirstBattleTgt", self.__obj_id, next_skill_id)
		local first_tgt = GlobalScene:GetObj(first_tgt_id)
		if first_tgt and first_tgt:CanDoBeAttacked() then
			--若当前技能是怒气/合击技能，则不做命中目标逻辑检测(前提是，至少有一个目标)
			self:__ChangeToAttack()
			return true
		end
		return false
	end

	return AI.BaseAI.__CheckToAttack(self)	
end

--更新技能CD
function AI.HeroAI:__UpdateSkillCD(elapse_time)
	AI.BaseAI.__UpdateSkillCD(self, elapse_time)

	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") then
		--怒气/合击技能间的公共
		if self.__anger_or_joint_common_cd_info.in_cd then
			self.__anger_or_joint_common_cd_info.cd_elapse_time = self.__anger_or_joint_common_cd_info.cd_elapse_time + elapse_time
			if self.__anger_or_joint_common_cd_info.cd_elapse_time >= self.__anger_or_joint_common_cd_info.cd_time then
				self.__anger_or_joint_common_cd_info.in_cd = false
			end
		end	
	end
end

--重置怒气/合击技能间公共CD
function AI.HeroAI:__SetAngerOrJointCommonCD()
	self.__anger_or_joint_common_cd_info.in_cd = true
	self.__anger_or_joint_common_cd_info.cd_elapse_time = 0
end

--获取怒气/合击技能ID
function AI.HeroAI:__GetAngerOrJointSkillID()
	if self.__anger_or_joint_plus_skill_triggered then
		return Skill.SkillLogic:GetAngerOrJointSkillID(self.__obj_id, true)

	elseif self.__anger_or_joint_skill_triggered then
		return Skill.SkillLogic:GetAngerOrJointSkillID(self.__obj_id)
	end
end
