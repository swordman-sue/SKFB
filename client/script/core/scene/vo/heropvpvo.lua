
--[[
%% @module: 英雄PVP对象vo
%% @author: swordman sue
%% @created: 2017-06-22
--]]

Scene = Scene or {}

Scene.HeroPVPVO = Scene.HeroPVPVO or BaseClass(Scene.HeroVO)

function Scene.HeroPVPVO:__init()
    --类型
    self.ai_type = AI.Type.HERO_PVP    
end