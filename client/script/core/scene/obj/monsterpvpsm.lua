
--[[
%% @module: PVP怪物对象状态机
%% @author: swordman sue
%% @created: 2017-06-16
--]]

Scene = Scene or {}

Scene.MonsterPVP = Scene.MonsterPVP or BaseClass(Scene.BattleObj)

function Scene.MonsterPVP:__InitStateMachine()
	Scene.BattleObj.__InitStateMachine(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--死亡状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.MonsterPVP:__EnterStateDead(state_name)
	Scene.BattleObj.__EnterStateDead(self, state_name)

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.MONSTER_DEAD, self.__vo.obj_id)
end
