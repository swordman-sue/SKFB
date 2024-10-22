
--[[
%% @module: 镜头
%% @author: swordman sue
%% @created: 2016-07-11
注：相机坐标与场景对象的world_pos处于同一坐标系
--]]

Scene = Scene or {}

Scene.Camera = Scene.Camera or BaseClass()

--缓动处理
local smooth

--震动类型
local ShakeType = 
{
	--缩放震动
	ZOOM = 0,
	--上下震动
	OFFSET_VERT = 1,
	--左右震动
	OFFSET_HORZ = 2,
}

local ignore_pos_smooth_delta = 0
local ignore_scale_smooth_delta = 0

function Scene.Camera:__init()
	self.__move_smooth_time = 1
	self.__move_speed = Vector2D.New(0, 0)
	self.__now_pos = Vector2D.New(0, 0)
	self.__pre_pos = Vector2D.New(0, 0)

	self.__scale_speed = 0
	self.__now_scale = 1

	--镜头观察者
	self.__observer = nil

	--震动偏移信息
	self.__shake_delta_info = {}
	self.__shake_delta_info.offset = Vector2D.New(0, 0)
	self.__shake_delta_info.scale = 0

	--是否暂停
	self.__ref_in_pause = RefObj.New()
end

function Scene.Camera:__delete()
	self.__move_speed = nil
	self.__now_pos = nil
	self.__pre_pos = nil
end

function Scene.Camera:Clear()
	self.__now_pos.x = 0 self.__now_pos.y = 0
	self.__move_speed.x = 0 self.__move_speed.y = 0
	self.__target_pos = nil
	self.__target_pos_end_callback = nil

	self.__now_scale = 1
	self.__scale_speed = 0
	self.__target_scale = nil
	self.__target_scale_end_callback = nil

	self.__observer = nil

	--震动信息表
	self.__camera_shake_map = nil

	--震动偏移信息
	self.__shake_delta_info.offset.x = 0 self.__shake_delta_info.offset.y = 0
	self.__shake_delta_info.scale = 0

	self.__fixed_scale = false

	--重置暂停
	self.__ref_in_pause:Reset()

	self:__ReSetRenderGroupPos()
end

--获取镜头中心位置
function Scene.Camera:GetNowPos()
	return self.__now_pos
end

--获取镜头缩放
function Scene.Camera:GetNowScale()
	return self.__now_scale
end

--获取镜头可视大小
function Scene.Camera:GetVisualSize(render_queue, calc_scale)
	render_queue = render_queue or Scene.RenderQueue.GRQ_TERRAIN

	local scale = calc_scale and self:__GetRenderGroupScale(render_queue) or 1

	local map_mode = GlobalScene:GetMapMode()

	--地图覆盖上半屏
	if map_mode == Scene.Map.Mode.SMM_HALF_SCREEN_TOP then
		local _, map_height = GlobalScene:GetMapGroupSize(render_queue)
		return Macros.Global.DesignSize.width / scale, map_height / scale

	--地图覆盖全屏
	else
	    return Macros.Global.DesignSize.width / scale, Macros.Global.DesignSize.height / scale
	end
end

--注册观察者
function Scene.Camera:RegisterObserver(observer)
	self.__observer = observer
end

--注销观察者
function Scene.Camera:UnRegisterObserver()
	self.__observer = nil
end

--更新
function Scene.Camera:Update(now_time, elapse_time)	
	--镜头震动
	if self.__camera_shake_map then
		self:__UpdateShake(elapse_time)
		return
	end

	local need_update

	--镜头缩放
	if self.__target_scale then
		if not self:__UpdateScale(elapse_time) then			
			if self.__target_scale_end_callback then
				self.__target_scale_end_callback()
				self.__target_scale_end_callback = nil
			end
			self.__target_scale = nil
		else
			need_update = true
		end
	end

	--镜头跟随对象
	if self.__observer then
	    local target_pos = Vector2D.New(self.__observer:GetWorldPos())
		if self:__UpdatePos(target_pos, elapse_time) then
			need_update = true
		end

	--镜头跟随坐标	
	elseif self.__target_pos then
		if not self:__UpdatePos(self.__target_pos, elapse_time) then
			if self.__target_pos_end_callback then
				self.__target_pos_end_callback()
				self.__target_pos_end_callback = nil
			end
			self.__target_pos = nil
			self.__lock_max_camera_y = nil
		else
			need_update = true
		end
	end

	if need_update then
		self:__UpdateRenderGroupPos()
	end
end

--直接更新到目标位置
function Scene.Camera:UpdatePosDirect(target_pos)
	target_pos = target_pos or self.__now_pos

	self.__now_pos = self:__CameraBoundHandle(target_pos)
	self.__pre_pos = Vector2D.New(self.__now_pos.x, self.__now_pos.y)
	self.__target_pos = nil
	self.__lock_max_camera_y = nil

	self:__UpdateRenderGroupPos()
end

--缓动更新到目标位置
function Scene.Camera:UpdatePosSmooth(target_pos, speed, end_callback, lock_max_camera_y)
	if math.abs(target_pos.x - self.__now_pos.x) < ignore_pos_smooth_delta and math.abs(target_pos.y - self.__now_pos.y) < ignore_pos_smooth_delta then
		return
	end
	self.__lock_max_camera_y = lock_max_camera_y
	self.__target_pos = target_pos
	self.__target_pos_end_callback = end_callback

	local vec = self.__target_pos - self.__now_pos
	self.__move_smooth_time = math.max(MathTool.EPSINON, vec:Length() / speed)
end

--直接更新到目标缩放
function Scene.Camera:UpdateScaleDirect(target_scale)
	target_scale = target_scale or self.__now_scale

	self.__now_scale = target_scale
	self.__target_scale = nil

	self:__UpdateRenderGroupPos()
end

--缓动更新到目标缩放
function Scene.Camera:UpdateScaleSmooth(target_scale, speed, end_callback)
	if target_scale == self.__now_scale then
		return
	end

	local delta_scale = target_scale - self.__now_scale
	if math.abs(delta_scale) < ignore_scale_smooth_delta then
		return
	end

	self.__target_scale = target_scale
	self.__target_scale_end_callback = end_callback
	self.__target_scale_dir = delta_scale > 0 and 1 or -1
	self.__scale_speed = speed or 0.1
end

--[[
地形、地形跟随层的镜头变焦
@ params：
    震动类型, 震幅, 震频，持续时间
--]]
function Scene.Camera:TerrainShake(type, range, frequency, last_time)
	type = type or ShakeType.ZOOM
	range = range or 5
	frequency = frequency or 0.2
	last_time = last_time or 0.8

	self.__camera_shake_map = self.__camera_shake_map or {}
	if self.__camera_shake_map[type] then
		--同类震动不能并存
		return
	end

	local shake_info = {}
	shake_info.last_time = last_time
	shake_info.pass_time = 0
	shake_info.type = type
	shake_info.range = range
	shake_info.frequency = frequency
	self.__camera_shake_map[shake_info.type] = shake_info
end

function Scene.Camera:Pause()
	self.__ref_in_pause:Retain()
end

function Scene.Camera:Resume()
	self.__ref_in_pause:Release()
end

function Scene.Camera:SetFixedScale(var)
	self.__fixed_scale = var
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--更新镜头位置
function Scene.Camera:__UpdatePos(target_pos, elapse_time)
	if self.__ref_in_pause:IsValid() then
		return
	end

	if math.abs(target_pos.x - self.__now_pos.x) < 1.0 and math.abs(target_pos.y - self.__now_pos.y) < 1.0 then
		return false
	end
	target_pos = smooth(self.__now_pos, target_pos, self.__move_speed, self.__move_smooth_time, elapse_time)
	self.__pre_pos.x = self.__now_pos.x
	self.__pre_pos.y = self.__now_pos.y
	self.__now_pos = self:__CameraBoundHandle(target_pos)

	return true
end

--更新镜头缩放
function Scene.Camera:__UpdateScale(elapse_time)
	if self.__ref_in_pause:IsValid() then
		return
	end
	
	if self.__target_scale == self.__now_scale then
		return false
	end

	local delta = elapse_time * self.__scale_speed * self.__target_scale_dir
	local target_scale = self.__now_scale + delta

	if self.__target_scale_dir > 0 and self.__now_scale < self.__target_scale and target_scale >= self.__target_scale then
		self.__now_scale = self.__target_scale
		return true		
	end
	if self.__target_scale_dir < 0 and self.__now_scale > self.__target_scale and target_scale <= self.__target_scale then
		self.__now_scale = self.__target_scale
		return true
	end

	self.__now_scale = target_scale

	return true
end

--[[
	CameraBoundHandle -> SetCameraPos
	1.修正GRQ_TERRAIN层相机位置
	2.其余层的相机位置，根据GRQ_TERRAIN层相机位置计算
--]]
function Scene.Camera:__CameraBoundHandle(target_pos, render_queue, calc_scale)
	render_queue = render_queue or Scene.RenderQueue.GRQ_TERRAIN

	local now_pos = Vector2D.New(target_pos.x, target_pos.y)

	local visual_width, visual_height = self:GetVisualSize()
	local half_visual_width = visual_width / 2
	local half_visual_height = visual_height / 2	
	local map_terrain_width, map_terrain_height = GlobalScene:GetMapGroupSize(Scene.RenderQueue.GRQ_TERRAIN)
	if calc_scale then		
		local scale = self:__GetRenderGroupScale()
		map_terrain_width = map_terrain_width * scale
		map_terrain_height = map_terrain_height * scale
	end

	if render_queue == Scene.RenderQueue.GRQ_TERRAIN then
		--边界检测
		local min_camera_x = half_visual_width
		local min_camera_y = half_visual_height
		local max_camera_x = map_terrain_width - half_visual_width
		local max_camera_y = map_terrain_height - half_visual_height

		now_pos.x = math.max(now_pos.x, min_camera_x)
		now_pos.x = math.min(now_pos.x, max_camera_x)

		now_pos.y = math.max(now_pos.y, min_camera_y)
		if self.__lock_max_camera_y then
			--锁住最少的镜头Y轴位置
			now_pos.y = math.max(now_pos.y, max_camera_y + self.__lock_max_camera_y)	
			now_pos.y = math.min(now_pos.y, max_camera_y)	
		else			
			now_pos.y = math.min(now_pos.y, max_camera_y)
		end
		return now_pos
	end

	--地形跟随层
	if Scene.Scene.IsFollowTerrain(render_queue) then
		local config_camera = ConfigAdapter.Common.GetBattleCameraInfo()
		now_pos:MultiEx(not self.__fixed_scale and 1 / config_camera.terrain_obj_factor or 1)
		return now_pos
	end

	--地图层 
	local map_group_width, map_group_height = GlobalScene:GetMapGroupSize(render_queue)
	if calc_scale then
		local scale = self:__GetRenderGroupScale(render_queue)
		map_group_width = map_group_width * scale
		map_group_height = map_group_height * scale
	end

	local t_x = 1
	if map_terrain_width - visual_width ~= 0 then
		t_x = (map_group_width - visual_width) / (map_terrain_width - visual_width)
	end
	local t_y = 1
	if map_terrain_height - visual_height ~= 0 then
		t_y = (map_group_height - visual_height) / (map_terrain_height - visual_height)
	end

	now_pos.x = (now_pos.x - half_visual_width) * t_x + half_visual_width
	now_pos.y = (now_pos.y - half_visual_height) * t_y + half_visual_height
	return now_pos
end 

--更新渲染组位置
function Scene.Camera:__UpdateRenderGroupPos()
	local map_group_width, map_group_height = GlobalScene:GetMapGroupSize()

	local terrain_now_pos = Vector2D.New(self.__now_pos.x, self.__now_pos.y)
	terrain_now_pos:MultiEx(self.__now_scale)
	terrain_now_pos = self:__CameraBoundHandle(terrain_now_pos, nil, true)

	for _, render_queue in pairs(Scene.RenderQueue) do
		local offset

		--地形层
		if GlobalScene:HasMapGroup(render_queue) then
			if render_queue == Scene.RenderQueue.GRQ_TERRAIN then
				offset = Vector2D.New(terrain_now_pos.x, terrain_now_pos.y)
			else
				offset = self:__CameraBoundHandle(terrain_now_pos, render_queue, true)
			end

		--地形跟随层
		elseif Scene.Scene.IsFollowTerrain(render_queue) then
			offset = self:__CameraBoundHandle(terrain_now_pos, render_queue, true)
		end

		if offset then
			local scale = self:__GetRenderGroupScale(render_queue)
			local visual_width, visual_height = self:GetVisualSize()
			local map_mode_offset_x, map_mode_offset_y = self:__GetRenderGroupOffsetByMapMode(render_queue)
			offset.x = visual_width / 2 - offset.x
			offset.y = offset.y + visual_height / 2 - map_group_height * scale

			GlobalScene:SetRenderGroupScale(scale + self.__shake_delta_info.scale, render_queue)
			GlobalScene:SetRenderGroupPos(
				offset.x + self.__shake_delta_info.offset.x + map_mode_offset_x, 
				offset.y + self.__shake_delta_info.offset.y + map_mode_offset_y, render_queue)
		end
	end
end

--重置渲染组位置
function Scene.Camera:__ReSetRenderGroupPos()
	local render_group
	for _, render_queue in pairs(Scene.RenderQueue) do

		--地形层、地形跟随层
		if GlobalScene:HasMapGroup(render_queue) or 
			Scene.Scene.IsFollowTerrain(render_queue) then
			local map_mode_offset_x, map_mode_offset_y = self:__GetRenderGroupOffsetByMapMode(render_queue)
			GlobalScene:SetRenderGroupScale(1, render_queue)
			GlobalScene:SetRenderGroupPos(map_mode_offset_x, map_mode_offset_y, render_queue)
		end
	end	
end

--根据地图模式获取渲染组偏移
function Scene.Camera:__GetRenderGroupOffsetByMapMode(render_queue)
	local map_mode = GlobalScene:GetMapMode()
	if map_mode == Scene.Map.Mode.SMM_HALF_SCREEN_TOP then
		local _, map_height = GlobalScene:GetMapGroupSize(render_queue)		
		return 0, Macros.Global.DesignSize.height - map_height
	end
	return 0, 0
end

--镜头震动
function Scene.Camera:__UpdateShake(elapse_time)
	local delta_scale = 0
	local delta_x = 0
	local delta_y = 0
	local finished = false

	--震动处理
	local function handle_shake(type, range, frequency, pass_time)
		local cur_range
		local unit_pass_time = pass_time % frequency
		local half_frequency = frequency / 2
		local quarter_frequency = frequency / 4

		--1/4震频时间，从初始位置变化到最大振幅
		if unit_pass_time >= 0 and unit_pass_time < quarter_frequency then
			cur_range = unit_pass_time / quarter_frequency * range

		--1/4震频时间，从最大振幅还原到初始位置
		elseif unit_pass_time >= quarter_frequency and unit_pass_time < half_frequency then
			cur_range = (half_frequency - unit_pass_time) / quarter_frequency * range

		--1/4震频时间，从初始位置变化到最小振幅
		elseif unit_pass_time >= half_frequency and unit_pass_time < quarter_frequency * 3 then
			cur_range = (half_frequency - unit_pass_time) / quarter_frequency * range

		--1/4震频时间，从最小振幅变化到初始位置
		elseif unit_pass_time >= quarter_frequency * 3 and unit_pass_time < frequency then
			cur_range = (unit_pass_time - frequency) / quarter_frequency * range			
		end

		if type == ShakeType.ZOOM then
			delta_scale = cur_range / 100

		elseif type == ShakeType.OFFSET_HORZ then
			delta_x = cur_range

		elseif type == ShakeType.OFFSET_VERT then
			delta_y = cur_range
		end
	end

	local del_list
	for shake_type, shake_info in pairs(self.__camera_shake_map) do
		shake_info.pass_time = shake_info.pass_time + elapse_time
		handle_shake(shake_info.type, shake_info.range, shake_info.frequency, shake_info.pass_time)		

		if shake_info.pass_time >= shake_info.last_time then
			del_list = del_list or {}
			table.insert(del_list, shake_type)
		end
	end
	if del_list then
		for _, del_type in ipairs(del_list) do
			self.__camera_shake_map[del_type] = nil
		end
		--震动结束
		if CPPGameLib.GetTableLength(self.__camera_shake_map) <= 0 then
			finished = true
			self.__shake_delta_info.offset.x = 0 self.__shake_delta_info.offset.y = 0
			self.__shake_delta_info.scale = 0
			self.__camera_shake_map = nil
		end
	end

	if not finished then
		self.__shake_delta_info.offset.x = delta_x
		self.__shake_delta_info.offset.y = delta_y
		self.__shake_delta_info.scale = delta_scale
	end

	self:UpdatePosDirect()
end

function Scene.Camera:__GetRenderGroupScale(render_queue)
	if self.__fixed_scale then
		return self.__now_scale
	end	

	render_queue = render_queue or Scene.RenderQueue.GRQ_TERRAIN

	local config_camera = ConfigAdapter.Common.GetBattleCameraInfo()

	if Scene.Scene.IsFollowTerrain(render_queue) then
		return self.__now_scale / config_camera.terrain_obj_factor
	end

	local factor_map = ConfigAdapter.Common.GetBattleCameraScaleInfo()
	local factor = factor_map[render_queue]
	if not factor then
		return self.__now_scale
	end
	if factor == 0 then
		return 1
	end

	return 1 + (self.__now_scale - 1) / factor
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
smooth = function(from, to, vel, smooth_time, elapse_time)
	local omega = 2 / smooth_time
	local x = omega * elapse_time
	local exp = 1.0 / (1.0 + x + 0.48*x*x + 0.235*x*x*x)
	-- change = from-to;
	local change = from - to 
	-- temp = (vel+omega*change)*timeDelta			
	local temp = Vector2D.New((vel.x + change.x * omega) * elapse_time, (vel.y + change.y * omega) * elapse_time)
	-- vel = (vel-omega*temp)*exp;	
	local new_vel = Vector2D.New((vel.x - temp.x * omega) * exp, (vel.y - temp.y * omega) * exp)						
	vel.x = new_vel.x;
	vel.y = new_vel.y;
	-- return to+(change+temp)*exp;
	return Vector2D.New(to.x + (change.x + temp.x) * exp, to.y + (change.y + temp.y) * exp);							
end
