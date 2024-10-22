
--[[
%% @module: 场景物品对象状态机(多人场景)
%% @author: swordman sue
%% @created: 2017-09-28
--]]

Scene = Scene or {}

Scene.MtItem = Scene.MtItem or BaseClass(Scene.Obj)

function Scene.MtItem:__InitStateMachine()
    Scene.Obj.__InitStateMachine(self)
end
