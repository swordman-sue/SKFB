
--[[
%% @module: 怪物对象状态机(多人场景)
%% @author: swordman sue
%% @created: 2017-09-27
--]]

Scene = Scene or {}

Scene.MtMonster = Scene.MtMonster or BaseClass(Scene.Obj)

function Scene.MtMonster:__InitStateMachine()
    Scene.Obj.__InitStateMachine(self)
end
