
--[[
%% @module: PVP怪物对象
%% @author: swordman sue
%% @created: 2017-06-16
--]]

Scene = Scene or {}

Scene.MonsterPVP = Scene.MonsterPVP or BaseClass(Scene.BattleObj)

function Scene.MonsterPVP:__init()	
	self.__obj_type = Scene.ObjType.MONSTER_PVP

	--加载VO
	self:__LoadVO()	

    GlobalVOMgr:AddMonsterVO(self.__vo.obj_id, self.__vo)    
end

function Scene.MonsterPVP:__delete()
	GlobalVOMgr:DeleteMonsterVO(self.__vo.obj_id)	
end

--能否累计怒气
function Scene.MonsterPVP:IsAngerAdditive()
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon") then
		return Scene.Monster.IsAngerAdditive(self)
	end
	return false
end

--怒气是否充足
function Scene.MonsterPVP:IsAngerEnough(is_plus)
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon") then
		return Scene.Monster.IsAngerEnough(self, is_plus)
	end
    return true	
end

--怒气处理
function Scene.MonsterPVP:DoAngerChange(change_anger)    
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon") then
		return Scene.Monster.DoAngerChange(self, change_anger)
	end
end

--设置怒气/合击技能释放开关
function Scene.MonsterPVP:TriggerAngerOrJointSkill(skill_id)
	if self:IsInAngerOrJointSkill() then
		return
	end

    local ai = GlobalAIMgr:GetAI(self.__ai_key)
    if ai then
        return ai:TriggerAngerOrJointSkill(skill_id)
    end
end

--设置自动战斗(自动释放怒气/合击)
function Scene.MonsterPVP:SetAutoFight(var)
end

--是否自动战斗
function Scene.MonsterPVP:IsAutoFight()
end
