
--[[
%% @module: PVP英雄对象
%% @author: swordman sue
%% @created: 2017-06-16
--]]

Scene = Scene or {}

Scene.HeroPVP = Scene.HeroPVP or BaseClass(Scene.BattleObj)

function Scene.HeroPVP:__init()	
	self.__obj_type = Scene.ObjType.HERO_PVP

	--加载VO
	self:__LoadVO()

    GlobalVOMgr:AddHeroVO(self.__vo.obj_id, self.__vo)
end

function Scene.HeroPVP:__delete()
	GlobalVOMgr:DeleteHeroVO(self.__vo.obj_id)
end

--能否累计怒气
function Scene.HeroPVP:IsAngerAdditive()
    return Scene.Hero.IsAngerAdditive(self)
end

--怒气是否充足
function Scene.HeroPVP:IsAngerEnough(is_plus)
    return Scene.Hero.IsAngerEnough(self, is_plus)
end

--怒气处理
function Scene.HeroPVP:DoAngerChange(change_anger)    
    Scene.Hero.DoAngerChange(self, change_anger)   
end

--设置怒气/合击技能释放开关
function Scene.HeroPVP:TriggerAngerOrJointSkill(skill_id)
    local ai = GlobalAIMgr:GetAI(self.__ai_key)
    if ai then
        return ai:TriggerAngerOrJointSkill(skill_id)
    end
end

--设置自动战斗(自动释放怒气/合击)
function Scene.HeroPVP:SetAutoFight(var)
end

--是否自动战斗
function Scene.HeroPVP:IsAutoFight()
end
