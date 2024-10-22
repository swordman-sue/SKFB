
--[[
%% @module: 精灵对象vo()
%% @author: swordman sue
%% @created: 2017-01-20
--]]

Scene = Scene or {}

Scene.MtSpiritVO = Scene.MtSpiritVO or BaseClass(Scene.ObjVO)

function Scene.MtSpiritVO:__init()
	self.anim_type = RenderObj.AnimType.SKELETON

	--怪物ID
	self.info_id = 0		

	--所属渲染层
	self.render_queue = Scene.RenderQueue.GRQ_AIR	

	--类型
	self.ai_type = AI.Type.MT_SPIRIT	

	--忽略拖尾特效
	self.ignore_mount_effect = true

	--精灵与英雄的坐标偏移
	self.follow_space = 80
	self.offset_frm_hero = Vector2D.New(-60, -70)
end

function Scene.MtSpiritVO:__delete()
end

function Scene.MtSpiritVO:ParseInfo(sprite_info)	
	self.info_id = sprite_info.info_id

	local config_sprite = CPPGameLib.GetConfig("Spirit", self.info_id)	
	if not config_sprite then
		return
	end
    local config_scene = GlobalVOMgr:GetSceneConfig()

	self.model_type = config_sprite.model_type
	self.model_id = config_sprite.model_id
	self.model_scale = config_sprite.battle_scale * (config_scene and config_scene.model_scale or 1)
	self.speed = config_sprite.init_move_speed
end
