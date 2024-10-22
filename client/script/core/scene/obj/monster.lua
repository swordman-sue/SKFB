
--[[
%% @module: 怪物对象
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Scene = Scene or {}

Scene.Monster = Scene.Monster or BaseClass(Scene.BattleObj)

function Scene.Monster:__init()	
	self.__obj_type = Scene.ObjType.MONSTER

	--自动战斗(自动释放怒气/合击)
	self.__auto_fight = true

	--加载VO
	self:__LoadVO()	

    GlobalVOMgr:AddMonsterVO(self.__vo.obj_id, self.__vo)    
end

function Scene.Monster:__delete()
	GlobalVOMgr:DeleteMonsterVO(self.__vo.obj_id)	
end

--能否累计怒气
function Scene.Monster:IsAngerAdditive()
	return true
end

--怒气是否充足
function Scene.Monster:IsAngerEnough(is_plus)
    if self.__vo.cost_of_anger_or_joint_skill <= 0 then
        return false
    end
	
	if not is_plus then
	    return GlobalVOMgr:GetAnger(true) >= self.__vo.cost_of_anger_or_joint_skill
	
    elseif self.__vo.skill_id_of_anger_plus_skill or self.__vo.skill_id_of_joint_plus_skill then
	    return GlobalVOMgr:GetAnger(true) >= self.__vo.cost_of_anger_or_joint_skill * 2
	end
end

--怒气处理
function Scene.Monster:DoAngerChange(change_anger)    
    if self.is_real_dead then
        return
    end

    if not self:IsAngerAdditive() then
        return
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoAngerChange", change_anger, true)
end
