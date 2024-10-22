
--[[
%% @module: 地图对象
%% @author: swordman sue
%% @created: 2016-07-11
--]]

Scene = Scene or {}

Scene.Map = Scene.Map or BaseClass()

Scene.Map.SliceUnitSize = COCOSize(256, 256)
Scene.Map.TileUnitSize = COCOSize(32, 32)

Scene.Map.Mode = 
{
	--地图覆盖全屏
	SMM_FULL_SCREEN = 1,

	--地图覆盖上半屏
	SMM_HALF_SCREEN_TOP = 2,
}

local get_map_render
local get_slice_info
local cache_slice

function Scene.Map:__init()
	self.__map_render_list = {}
	self.__map_slice_map = {}
	self.__map_mode = Scene.Map.Mode.SMM_FULL_SCREEN
end

function Scene.Map:__delete()
	self:Unload(true)
end

--TODO：处理地图块动态加载/卸载	
--加载地图
function Scene.Map:Load(map_id)
	self:Unload()

	self.__loaded_num = 0
	self.__need_load_num = 0

	GlobalRequireToolBatch:LoadFiles({
		file_list = {Resource.PathTool.GetMapCnfPath(map_id)}, 
		callback = function()
			self.__config_map = CPPGameLib.GetConfig("Map", map_id)

			--屏幕适配处理
			for _, group_info in pairs(self.__config_map.group_list) do
				if group_info.org_size[1] < Macros.Global.DesignSize.width then
					group_info.scale = Macros.Global.DesignSize.width / group_info.org_size[1]
					group_info.org_size[1] = Macros.Global.DesignSize.width
				end
			end

			self:LoadSlices()
		end})
end

-- TEMP：临时处理地图预加载失败导致的地图创建失败问题
function Scene.Map:LoadForRemedy()
	self:CreateSlices()
end

--加载地图块
function Scene.Map:LoadSlices()
	local real_slice_path
	for render_queue, group_info in pairs(self.__config_map.group_list) do
		for count = 1, group_info.repeat_count do
			for _, slice_name in pairs(group_info.slice_list) do
				self.__need_load_num = self.__need_load_num + 1

				real_slice_path = Resource.PathTool.GetMapSlicePath(group_info.slice_path, slice_name)							
				CPPViewHelper.LoadAsyncImageResource(real_slice_path, GlobalCallbackMgr:AddCallBackFunc(function()
					self.__loaded_num = self.__loaded_num + 1
					if self.__loaded_num == self.__need_load_num then
						self.__is_loaded = true
						self:CreateSlices()
					end
				end))
			end
		end
	end
end

--创建地图块
function Scene.Map:CreateSlices()
	if not self.__config_map then
		return
	end

	if self.__is_created_slices then
		return
	end
	self.__is_created_slices = true

	local slice, slice_x, slice_y, slice_posx, slice_posy
	local real_slice_path
	local map_render

	for render_queue, group_info in pairs(self.__config_map.group_list) do
		for count = 1, group_info.repeat_count do
			for _, slice_name in pairs(group_info.slice_list) do
				real_slice_path = Resource.PathTool.GetMapSlicePath(group_info.slice_path, slice_name)
				map_render = get_map_render(self, render_queue)
				slice_x, slice_y, slice_posx, slice_posy = get_slice_info(slice_name, group_info.org_size, count)
				slice = SpriteEx.CreateWithImageFile(real_slice_path)
				slice:SetAnchorPoint(0, 0)
				slice:SetPosition(slice_posx, slice_posy)
				slice:SetScaleXFactor(group_info.scale or 1)
				map_render:AddChild(slice)

				--缓存地图块
				cache_slice(self, render_queue, slice, slice_x, slice_y)
			end
		end
	end
end

--卸载地图
function Scene.Map:Unload(cleanup)
	if not self.__is_loaded then
		return
	end
	self.__is_loaded = false
	self.__is_created_slices = false

	if self.__config_map then		
		GlobalRequireToolBatch:UnloadFiles({file_list = {Resource.PathTool.GetMapCnfPath(self.__config_map.map_id)}})
		self.__config_map = nil
	end

	for _, map_render in pairs(self.__map_render_list) do
		map_render:DeleteMe()
	end

	if cleanup then
		self.__map_render_list = nil	
		self.__map_slice_map = nil
	else
		self.__map_render_list = {}
		self.__map_slice_map = {}
	end
end

--更新
function Scene.Map:Update(now_time, elapse_time)
end


--设置加载完成标识
function Scene.Map:SetLoaded(var)
	self.__is_loaded = var
end

--获取加载完成标识
function Scene.Map:IsLoaded()
	return self.__is_loaded
end

--获取加载进度信息
function Scene.Map:GetLoadInfo()
	return self.__loaded_num or 0, self.__need_load_num or 0
end

--获取地图模式
function Scene.Map:GetMode()
	return self.__map_mode
end

--是否存在渲染组对应的地图
function Scene.Map:HasGroup(render_queue)
	if not self.__config_map then
		return false
	end
	return self.__config_map.group_list[render_queue] ~= nil
end

--获取各渲染组对应的地图数据
function Scene.Map:GetGroupList()
	if self.__config_map then
		return self.__config_map.group_list
	end
end

--获取渲染组对应的地图尺寸
function Scene.Map:GetGroupSize(render_queue)
	if self.__config_map then
		local group_info = self.__config_map.group_list[render_queue] or self.__config_map.group_list[Scene.RenderQueue.GRQ_TERRAIN]
		if group_info then
			return group_info.org_size[1] * group_info.repeat_count, group_info.org_size[2]
		end
	end	
	return 0, 0
end

--置灰地图块
function Scene.Map:SetGray(var)
	for _, render_queue_map in pairs(self.__map_slice_map) do
		for _, x_map in pairs(render_queue_map) do
			for _, slice in pairs(x_map) do
				slice:SetGray(var)
			end
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_map_render = function(obj_self, render_queue)
	local map_render = obj_self.__map_render_list[render_queue]
	if not map_render then
		map_render = Layer.New()
		obj_self.__map_render_list[render_queue] = map_render
		GlobalScene:AddToRenderGroup(map_render, render_queue)
	end
	return map_render
end

get_slice_info = function(slice_name, group_org_size, repeat_count)
	local slice_num = tonumber(slice_name)
	local slice_x = math.floor(slice_num / 100)
	local slice_y = slice_num % 100
	return slice_x, slice_y, (slice_x - 1) * Scene.Map.SliceUnitSize.width + group_org_size[1] * (repeat_count - 1), (slice_y - 1) * Scene.Map.SliceUnitSize.height
end

cache_slice = function(obj_self, render_queue, slice, slice_x, slice_y)
	obj_self.__map_slice_map[render_queue] = obj_self.__map_slice_map[render_queue] or {}
	obj_self.__map_slice_map[render_queue][slice_x] = obj_self.__map_slice_map[render_queue][slice_x] or {}
	obj_self.__map_slice_map[render_queue][slice_x][slice_y] = slice 	
end