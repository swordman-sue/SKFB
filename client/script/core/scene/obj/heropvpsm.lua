
--[[
%% @module: PVP英雄对象状态机
%% @author: swordman sue
%% @created: 2017-06-16
--]]

Scene = Scene or {}

Scene.HeroPVP = Scene.HeroPVP or BaseClass(Scene.BattleObj)

function Scene.HeroPVP:__InitStateMachine()
	Scene.BattleObj.__InitStateMachine(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--死亡状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.HeroPVP:__EnterStateDead(state_name)
	Scene.BattleObj.__EnterStateDead(self, state_name)

	GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.HERO_DEAD, self.__vo.obj_id)
end

