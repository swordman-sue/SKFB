
--[[
%% @module: 英雄对象状态机(多人场景)
%% @author: swordman sue
%% @created: 2017-09-27
--]]

Scene = Scene or {}

Scene.MtHero = Scene.MtHero or BaseClass(Scene.Obj)

function Scene.MtHero:__InitStateMachine()
    Scene.Obj.__InitStateMachine(self)
end
