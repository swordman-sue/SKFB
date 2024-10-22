
--[[
%% @module: 助战英雄vo
%% @author: swordman sue
%% @created: 2017-05-11
--]]

Scene = Scene or {}

Scene.HeroAssistVO = Scene.HeroAssistVO or BaseClass(Scene.HeroVO)

function Scene.HeroAssistVO:__init()
	self.show_hp_bar = false

	self.battle_attr:DeleteMe()
	self.battle_attr = Scene.HeroAssistBattleAttr.New()
end

function Scene.HeroAssistVO:__delete()
end

function Scene.HeroAssistVO:ParseInfo(info_id, prop_list)
	if not info_id or not prop_list then
		return
	end

	self.info_id = info_id

	local config_hero = CPPGameLib.GetConfig("Hero", self.info_id)
	if not config_hero then
		return
	end

	--基础数据
	self.name = config_hero.name
	self.profession = config_hero.profession
	self.model_type = config_hero.model_type
	self.model_id = config_hero.model_id
	self.model_scale = config_hero.battle_scale or 1
	self.model_scale_inver = 1 / self.model_scale

	--技能数据
	self.skill_list = config_hero.skill_list
	self:ParseSkillList()

	--战斗属性
	self.battle_attr:ParseData(prop_list)
	self.battle_attr.speed = self.battle_attr.speed + config_hero.init_move_speed

	--血量
	self.hp = self.battle_attr.hp
	self.max_hp = self.battle_attr.hp
end