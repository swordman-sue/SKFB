
--[[
%% @module: 精灵对象vo
%% @author: swordman sue
%% @created: 2017-01-20
--]]

Scene = Scene or {}

Scene.SpiritVO = Scene.SpiritVO or BaseClass(Scene.BattleObjVO)

function Scene.SpiritVO:__init()
	self.anim_type = RenderObj.AnimType.SKELETON

	--怪物ID
	self.info_id = 0		

	--所属渲染层
	self.render_queue = Scene.RenderQueue.GRQ_AIR	

	--类型
	self.ai_type = AI.Type.SPIRIT	
end

function Scene.SpiritVO:__delete()
end

function Scene.SpiritVO:ParseInfo(sprite_info)	
	self.info_id = sprite_info.info_id

	local config_sprite = CPPGameLib.GetConfig("Spirit", self.info_id)	
	if not config_sprite then
		return
	end

	self.model_type = config_sprite.model_type
	self.model_id = config_sprite.model_id

	self.common_skill_id = config_sprite.common_skill_id
	self.special_skill_id = config_sprite.special_skill_id
	self.auto_skill_id = config_sprite.auto_skill_id
	self.appear_skill_id = config_sprite.appear_skill_id

	--战斗属性
	self.battle_attr:ParseData(sprite_info)
	self.battle_attr.speed = self.battle_attr.speed + config_sprite.init_move_speed

	--血量
	self.hp = self.battle_attr.hp
	self.max_hp = self.battle_attr.hp	
end

function Scene.SpiritVO:ParseSkillList(common_skill_id)
	self.common_skill_id = common_skill_id
	self.skill_list = {self.auto_skill_id}
end

