
--[[
%% @module: 掉落宝箱vo
%% @author: swordman sue
%% @created: 2016-08-17
--]]

Scene = Scene or {}

Scene.DropBoxVO = Scene.DropBoxVO or BaseClass(Scene.ObjVO)

function Scene.DropBoxVO:__init()
	self.drop_info = nil
	self.render_queue = Scene.RenderQueue.GRQ_AIR
	self.world_star_offset = COCOPoint(64, -96)
end

function Scene.DropBoxVO:__delete()
end
