
--[[
%% @module: 英雄镜像vo(用于夺宝、竞技场的敌方英雄)
%% @author: swordman sue
%% @created: 2016-10-18
--]]

Scene = Scene or {}

Scene.HeroMirrorVO = Scene.HeroMirrorVO or BaseClass(Scene.HeroVO)

function Scene.HeroMirrorVO:__init()
	self.hp_bar_image_info.bar_image_name = "prog_bar_bar_red"	

	self.is_hero_mirror = true
end

function Scene.HeroMirrorVO:__delete()
end

function Scene.HeroMirrorVO:ParseInfo(hero_info)
	if not hero_info then
		return
	end

	Scene.HeroVO.ParseInfo(self, hero_info, true)
end