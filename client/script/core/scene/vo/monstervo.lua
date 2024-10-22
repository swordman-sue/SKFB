
--[[
%% @module: 怪物对象vo
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Scene = Scene or {}

Scene.MonsterVO = Scene.MonsterVO or BaseClass(Scene.BattleObjVO)

function Scene.MonsterVO:__init()
	self.anim_type = RenderObj.AnimType.SKELETON

	--怪物ID
	self.info_id = 0		

	--是否显示血条
	self.show_hp_bar = true
	self.hp_bar_image_info = {}
	self.hp_bar_image_info.atlas_type = Resource.UITextureAtlasType.COMMON_PROGRESS
	self.hp_bar_image_info.bg_image_name = "prog_bar_bg"
	self.hp_bar_image_info.bar_image_name = "prog_bar_bar_red"

	--是否显示影子
	self.show_shadow = true

	--类型
	self.ai_type = AI.Type.MONSTER	
end

function Scene.MonsterVO:__delete()
end

function Scene.MonsterVO:ParseInfo(monster_info)
	self.info_id = monster_info.info_id
	self.index = monster_info.index

	local config_monster = CPPGameLib.GetConfig("Monster", self.info_id)	
	if not config_monster then
		return
	end

	--基础数据
	self.name = config_monster.name 
	if not config_monster.hero_id or config_monster.hero_id <= 0 then
		self.model_type = config_monster.model_type
		self.model_id = config_monster.model_id
		self.profession = config_monster.profession
		self.common_cd_time = config_monster.common_cd_time or ConfigAdapter.Common.GetSkillCommonCD()
		self.anger_or_joint_common_cd_time = config_monster.anger_or_joint_common_cd_time or 4		
		self.skill_list = config_monster.skill_list
		self.cost_of_anger_or_joint_skill = config_monster.need_anger or 0
		if config_monster.hide_blood_bar then
			self.show_hp_bar = config_monster.hide_blood_bar == Macros.Global.FALSE
		end
	else
		local config_hero = CPPGameLib.GetConfig("Hero", config_monster.hero_id)
		if config_hero then
			self.model_type = config_hero.model_type
			self.model_id = config_hero.model_id
			self.profession = config_hero.profession
			self.common_cd_time = config_hero.common_cd_time or ConfigAdapter.Common.GetSkillCommonCD()
			self.anger_or_joint_common_cd_time = config_hero.anger_or_joint_common_cd_time or 4		
			self.skill_list = config_hero.skill_list
			self.cost_of_anger_or_joint_skill = config_hero.need_anger or 0
		end
	end
	self.quality_level = config_monster.quality or 0

	--出生后是否隐藏
	self.is_hide_mode = monster_info.is_hide_mode

	--特殊AI列表
	self.ai_list = config_monster.ai

	--战斗缩放系数
	self.model_scale = config_monster.battle_scale or 1
	self.model_scale_inver = 1 / self.model_scale

	--特殊受击特效
	self.beattacked_effect_id = config_monster.beattacked_effect_id

	--大体型怪物与边界的间隔
	self.boundary_interval = config_monster.boundary_interval

	--免疫控制类技能
	--[[
		不可移动、会攻击的怪物：用于大型BOSS
		受到英雄普攻/特殊攻击时，会掉血但不会进入击退/受击状态，同时会击退攻击英雄并使其受到伤害，
		受怒气/合击技能伤害时，会播放受击动作，但不接受怒气/合击技能对于位置的改变（x轴的移动和y轴的移动），
		不接受控制性BUFF影响（石化、冰冻、击飞等），但接受BUFF、DEBUFF影响（强化、减防等）
	--]]
	self.immune_ctrl_skill = config_monster.immune_ctrl_skill == Macros.Global.TRUE

	--免疫控制类技能
	--[[
		不可移动、不会攻击的怪物：用于日常副本，
		受到英雄普攻/特殊攻击时，会掉血但不会进入击退/受击状态，同时会击退攻击英雄但不会使其受到伤害，
		受怒气/合击技能伤害时，会播放受击动作，但不接受怒气/合击技能对于位置的改变（x轴的移动和y轴的移动），
		不接受控制性BUFF影响（石化、冰冻、击飞等），但接受BUFF、DEBUFF影响（强化、减防等）
	--]]
	self.immune_ctrl_skill2 = config_monster.immune_ctrl_skill2 == Macros.Global.TRUE

	--技能数据
	self:ParseSkillList()

	--战斗属性
	self.battle_attr:ParseData(monster_info)
	self.battle_attr.speed = self.battle_attr.speed + config_monster.init_move_speed

	--血量
	self.hp = self.battle_attr.hp
	self.max_hp = self.battle_attr.hp
	self.hp = math.max(0, self.max_hp - monster_info.damage)		

	--初始化公共怒气
	if not self.is_x_monster then
		GlobalVOMgr:ChangeAnger(self.battle_attr.anger, true)
	end
end
