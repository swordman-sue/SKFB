
--[[
%% @module: PVP英雄AI
%% @author: swordman sue
%% @created: 2017-06-15
--]]

AI = AI or {}

AI.HeroPVPAI = AI.HeroPVPAI or BaseClass(AI.HeroAI)

function AI.HeroPVPAI:__init(params)
	self.__type = AI.Type.HERO_PVP
end

function AI.HeroPVPAI:__delete()
end

--触发怒气/合击技能
function AI.HeroPVPAI:TriggerAngerOrJointSkill(skill_id)
	self.__anger_or_joint_skill_id = skill_id
	return true	
end

function AI.HeroPVPAI:ShutdownAngerOrJointSkill(is_plus)
	if ConfigAdapter.Skill.IsAngerOrJointSkill(self.__anger_or_joint_skill_id, is_plus) then
		self.__anger_or_joint_skill_id = nil
	end
end

--检测怒气/合击技能开关
function AI.HeroPVPAI:IsAngerOrJointSkillTriggered()
	return self.__anger_or_joint_skill_id ~= nil
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--释放技能
function AI.HeroPVPAI:__ReleaseSkill()
	local me = self:__GetMe()
	local my_vo = self:__GetMyVO()

	--触发怒气/合击技能
	if self:IsAngerOrJointSkillTriggered() then
		self.__next_skill_id = self:__GetAngerOrJointSkillID()
	
		--重置怒气/合击技能开关，公共CD
		self:__SetAngerOrJointCommonCD()

		self:ShutdownAngerOrJointSkill()
	end

	AI.BaseAI.__ReleaseSkill(self)
end

--获取怒气/合击技能ID
function AI.HeroPVPAI:__GetAngerOrJointSkillID()
	return self.__anger_or_joint_skill_id
end
