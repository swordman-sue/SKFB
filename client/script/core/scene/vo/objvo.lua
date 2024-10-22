
--[[
%% @module: 场景对象vo
%% @author: swordman sue
%% @created: 2016-07-11
--]]

Scene = Scene or {}

Scene.ObjVO = Scene.ObjVO or BaseClass()

function Scene.ObjVO:__init()
	self.obj_id = 0
	--初始位置(逻辑坐标)
	self.org_pos_x = 0
	self.org_pos_y = 0
	--模型类型(hero、monster)
	self.model_type = ""
	--模型ID
	self.model_id = 0
	self.model_scale = 1
	self.model_scale_inver = 1
	--朝向
	self.dir = MathTool.HORZ_RIGHT
	--动画类型
	self.anim_type = RenderObj.AnimType.UNKNOWN
	--所属渲染层
	self.render_queue = Scene.RenderQueue.GRQ_BUILD_AND_PLAYER	
end

function Scene.ObjVO:__delete()
end


