--[[
%% @module: 弹道对象vo
%% @author: swordman sue
%% @created: 2016-07-21
--]]

Scene = Scene or {}

Scene.BulletVO = Scene.BulletVO or BaseClass(Scene.BattleObjVO)

function Scene.BulletVO:__init()
	--所属渲染层
	self.render_queue = Scene.RenderQueue.GRQ_AIR	
	
	--弹道ID
	self.info_id = 0
	--终点位置(逻辑坐标)
	self.dst_pos_x = 0
	self.dst_pos_y = 0
	--释放弹道的对象ID
	self.create_obj_id = 0
	--释放弹道的技能ID
	self.create_skill_id = 0
	--能否穿透
	self.can_penetrate = 0
	--移动距离
	self.move_distance = 0
	--碰撞范围(自身前方X像素)
	self.trigger_range = 0
	--攻击范围(自身前后X像素)
	self.attack_range = 0
	--特效ID
	self.effect_id = 0
	--爆炸特效ID
	self.bomb_effect_id = 0
	--是否进行BUFF检测
	self.can_check_buff = false
end

function Scene.BulletVO:__delete()
end

function Scene.BulletVO:ParseInfo(info_id)
	self.info_id = info_id
	
	local config_bullet = CPPGameLib.GetConfig("SkillBullet", info_id)
	if not config_bullet then
		return
	end

	--移动速度
	self.battle_attr.speed = config_bullet.move_speed	

	self.parabola_factor = config_bullet.parabola_factor
	self.can_penetrate = config_bullet.can_penetrate
	self.move_distance = config_bullet.move_distance
	self.trigger_range = config_bullet.trigger_range
	self.attack_range = config_bullet.attack_range
	self.effect_id = config_bullet.effect_id
	self.bomb_effect_id = config_bullet.bomb_effect_id
	self.shake_info = config_bullet.shake_info
end
