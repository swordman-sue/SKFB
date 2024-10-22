
--[[
%% @module: 英雄对象vo
%% @author: swordman sue
%% @created: 2016-07-11
--]]

Scene = Scene or {}

Scene.HeroVO = Scene.HeroVO or BaseClass(Scene.BattleObjVO)

function Scene.HeroVO:__init()
	self.anim_type = RenderObj.AnimType.SKELETON

	--英雄ID
	self.info_id = 0	

	--等级
	self.level = 1

	--品质等级
	self.quality_level = 0

	--是否显示血条
	self.show_hp_bar = true	
	self.hp_bar_image_info = {}
	self.hp_bar_image_info.atlas_type = Resource.UITextureAtlasType.COMMON_PROGRESS
	self.hp_bar_image_info.bg_image_name = "prog_bar_bg"
	self.hp_bar_image_info.bar_image_name = "prog_bar_bar_green2"

	--是否显示影子
	self.show_shadow = true

	--类型
	self.ai_type = AI.Type.HERO
end

function Scene.HeroVO:__delete()
	if self.bless_sprite_battle_attr then
		self.bless_sprite_battle_attr:DeleteMe()
		self.bless_sprite_battle_attr = nil
	end
end

function Scene.HeroVO:ParseInfo(hero_info, is_enemy)
	if not hero_info then
		return
	end

	self.info_id = hero_info.info_id	

	self.obj_id = hero_info.obj_id or 0

	self.lineup_index = hero_info.lineup_index or -1

	--等级、品质等级
	self.level = hero_info.level or 1
	self.quality_level = hero_info.quality_level or 0
	self.break_level = hero_info.break_level or 0

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
	self.common_cd_time = config_hero.common_cd_time or ConfigAdapter.Common.GetSkillCommonCD()
	self.anger_or_joint_common_cd_time = config_hero.anger_or_joint_common_cd_time or 4
	self.cost_of_anger_or_joint_skill = config_hero.need_anger or 0

	--技能数据
	self.skill_level = ConfigAdapter.Hero.GetPotentialSkillLevel(hero_info.potential_level)
	self.skill_list = self.quality_level > config_hero.init_quality and config_hero.skill_ex_list or config_hero.skill_list
	self:ParseSkillList()

	--战斗属性
	self.battle_attr:ParseData(hero_info)
	self.battle_attr.speed = self.battle_attr.speed + config_hero.init_move_speed

	--血量
	self.hp = self.battle_attr.hp
	self.max_hp = self.battle_attr.hp

	--喊话信息(协助英雄喊话)
	self.say_info = hero_info.say_info

	--协助英雄标记
	self.is_assist = hero_info.is_assist
	if self.is_assist then
		self.hp_bar_image_info.atlas_type = Resource.UITextureAtlasType.COMMON_PROGRESS
		self.hp_bar_image_info.bg_image_name = "prog_bar_bg"
		self.hp_bar_image_info.bar_image_name = "prog_bar_bar_blue"
	end

	--祝福精灵信息
	if hero_info.bless_sprite_info then
		self.bless_sprite_info = hero_info.bless_sprite_info
		local sprite_attr_info = {}
		Modules.BattleFormula:HandlePlusPropMap(sprite_attr_info, hero_info.bless_sprite_prop_map)
		self.bless_sprite_battle_attr = Scene.ObjBattleAttr.New()
		self.bless_sprite_battle_attr:ParseData(sprite_attr_info)

		CPPGameLib.PrintTable("skfb", hero_info.bless_sprite_prop_map, "bless_sprite_prop_map")
	end

	--初始化公共怒气
	GlobalVOMgr:ChangeAnger(self.battle_attr.anger, is_enemy)
end

