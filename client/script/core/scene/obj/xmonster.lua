
--[[
%% @module: X怪物对象
%% @author: swordman sue
%% @created: 2017-05-15
%% @description: 在原地创建一个x怪物，释放技能后立即销毁
--]]

Scene = Scene or {}

Scene.XMonster = Scene.XMonster or BaseClass(Scene.BattleObj)

function Scene.XMonster:__init()	
	self.__obj_type = Scene.ObjType.X_MONSTER

	--加载VO
	self:__LoadVO()	
end

function Scene.XMonster:__delete()	
end

function Scene.XMonster:__EnterStateAttack(state_name)
	Scene.BattleObj.__EnterStateAttack(self, state_name)

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_SKILL_RELEASE, self.__s_attack_state.skill_id)
end
