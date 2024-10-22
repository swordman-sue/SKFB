
--[[
%% @module: 怪物PVP对象vo
%% @author: swordman sue
%% @created: 2017-06-22
--]]

Scene = Scene or {}

Scene.MonsterPVPVO = Scene.MonsterPVPVO or BaseClass(Scene.HeroVO)

function Scene.MonsterPVPVO:__init()
	--是否显示血条
	self.show_hp_bar = true
	self.hp_bar_image_info = {}
	self.hp_bar_image_info.atlas_type = Resource.UITextureAtlasType.COMMON_PROGRESS
	self.hp_bar_image_info.bg_image_name = "prog_bar_bg"
	self.hp_bar_image_info.bar_image_name = "prog_bar_bar_red"
end

function Scene.MonsterPVPVO:ParseInfo(hero_info)
	if not hero_info then
		return
	end

	Scene.HeroVO.ParseInfo(self, hero_info, true)
end
