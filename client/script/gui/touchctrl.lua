
GUI = GUI or {}

-- 可拖动触控控制类
-- parent   	: 父节点
-- move_node 	: 移动节点
-- view_x		: 显示区域坐标
-- view_y		: 显示区域坐标
-- view_width	: 显示区域宽
-- view_height	: 显示区域高
GUI.TouchCtrl = GUI.TouchCtrl or BaseClass()

-- 计算速度
local function __CalcSpeed(s, a)
	local t = math.sqrt(2 * s / a)
	return a * t
end

function GUI.TouchCtrl:__init(parent, scissor_node, move_node, view_x, view_y, view_width, view_height)
	self.parent = parent
	self.parent:SetTouchEnabled(true)
	self.parent:SetTouchSwallow(false)

	self.__scissor_node = scissor_node

	self.__move_node   = move_node
	self.__move_node:SetTouchEnabled(false)
	self.__move_node:SetTouchSwallow(false)

	self.scroll_x    = 0			-- 当前滚动位置
	self.scroll_y    = 0			-- 当前滚动位置
	self.diff_x      = 0			-- 拖动距离差
	self.diff_y      = 0			-- 拖动距离差
	self.speed_x     = 0			-- 自动滑行速度
	self.speed_y     = 0			-- 自动滑行速度
	self.touch_time  = 0			-- 上次按下时间
	self.interval    = 0			-- 拖动时帧时间间隔
	self.out_x       = 0			-- 越界大小
	self.out_y       = 0			-- 越界大小
	self.acc         = 500			-- 加速阻力加速度
	self.node_width  = 10
	self.node_height = 10
	self.is_just_stop_move = false
	self.touch_up_move_dis = 32		-- 松开后移动距离

	-- 检测是否移动用参数
	self.touch_down_x = 0
	self.touch_down_y = 0
	self.touch_move_x = 0
	self.touch_move_y = 0

	-- 设置可视区域
	self:SetViewSize(view_x, view_y, view_width, view_height)

	-- 下按事件
	local function OnTouchDown(x, y)
		--TODO：待优化
		if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			return
		end

		self.touch_down_x = x
		self.touch_down_y = y
		self.touch_move_x = x
		self.touch_move_y = y

		-- 高速移动刚刚点停止标记
		if math.abs(self.speed_x) > 600 or math.abs(self.speed_y) > 600 then
			self.is_just_stop_move = true
		else
			self.is_just_stop_move = false
		end

		self.touch_x = x
		self.touch_y = y
		self.speed_x = 0
		self.speed_y = 0
		self.diff_x = 0
		self.diff_y = 0
		self.touch_time = CPPGameEngine:GetNowTimeMilli()
		self.touch_move_time_delta_list = {}
		self.touch_move_dis_list = {}

		-- 按下设置移动参数，避免按下不移动松开导致静止问题
		if self.node_width > self.__view_width then
			if self.scroll_x < self.scroll_x_min then
				self.out_x = self.scroll_x_min - self.scroll_x
			elseif self.scroll_x > self.scroll_x_max then
				self.out_x = self.scroll_x_max - self.scroll_x
			else
				self.out_x = 0
			end
		end
 		if self.node_height > self.__view_height then
			if self.scroll_y < self.scroll_y_min then
				self.out_y = self.scroll_y_min - self.scroll_y
			elseif self.scroll_y > self.scroll_y_max then
				self.out_y = self.scroll_y_max - self.scroll_y
			else
				self.out_y = 0
			end
		end

		CPPActionManager.StopAllActions(self.__move_node:GetNode())
	end

	-- 触屏移动
	local function OnTouchMove(x, y)
		--TODO：待优化
		if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			return
		end

		self.touch_move_x = x
		self.touch_move_y = y

		-- 记录帧时差
		local tm = CPPGameEngine:GetNowTimeMilli()
		self.interval = tm - self.touch_time
		if self.interval == 0 then
			self.interval = 1 / CPPSceneManager:GetGameFrame()
		end
		self.touch_time = tm

		local change = false

		-- 拖动x位置变化
		if self.node_width > self.__view_width then
			self.diff_x = x - self.touch_x
			self.scroll_x = self.scroll_x + self.diff_x
			self.touch_x = x
			self:__FixPosX()
			change = true
		end

		-- 拖动y位置变化
 		if self.node_height > self.__view_height then
			self.diff_y = y - self.touch_y
			self.scroll_y = self.scroll_y + self.diff_y
			self.touch_y = y
			self:__FixPosY()
			change = true
 		end

		-- 设置位置
		if change then
	 		self:__GatherTouchMove()
			self:__SetPosition()
		end
	end

	-- 松开事件
	local function OnTouchUp(x, y)
		--TODO：待优化
		if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			return
		end
		
		if self.touchup_callback then
			self.touchup_callback()
		else
			-- x轴回弹或继续滑动
			if self.out_x ~= 0 then
				self:__CheckXMoveEnd()
			else
				if self.node_width > self.__view_width then
					self.speed_x = self:__CalculateTouchMoveVelocity()
				end
			end

			-- y轴回弹或继续滑动
			if self.out_y ~= 0 then
				self:__CheckYMoveEnd()
			else 
		 		if self.node_height > self.__view_height then
					local _
					_, self.speed_y = self:__CalculateTouchMoveVelocity()
				end
			end
		end

		self.diff_x = 0
		self.diff_y = 0
	end

	-- 减速
	local function Speeddown(speed, acc, interval)
		local var = acc * interval
		if speed > 0 then
			speed = speed - var
			if speed <= 0 then
				speed = 0
			end
		end
		if speed < 0 then
			speed = speed + var
			if speed >= 0 then
				speed = 0
			end
		end
		return speed
	end

	--更新回调
	local function OnUpdate(interval)
		if self.__action_locate_info then
			local cur_x, cur_y = self.__move_node:GetPosition()
			self.scroll_x = self.scroll_x + (cur_x - self.__action_locate_info.x)
			self.scroll_y = self.scroll_y + (cur_y - self.__action_locate_info.y)
			self.__action_locate_info.x = cur_x
			self.__action_locate_info.y = cur_y
			if self.scroll_callback then
				self.scroll_callback()
			end
			return
		end

		-- 松手后滑动位置
		local change = false
		if self.speed_x ~= 0 then
			self.scroll_x = self.scroll_x + self.speed_x * interval
			self:__FixPosX2()
			change = true
		end
		if self.speed_y ~= 0 then
			self.scroll_y = self.scroll_y + self.speed_y * interval
			self:__FixPosY2()
			change = true
		end

		if self.speed_x ~= 0 then
			local speed = self.speed_x
			self.speed_x = Speeddown(self.speed_x, self.acc, interval)
		end
		if self.speed_y ~= 0 then
			local speed = self.speed_y
			self.speed_y = Speeddown(self.speed_y, self.acc, interval)	
		end

		-- 设置位置
		if change then
			self:__SetPosition()
		end				
	end

	self.parent:SetTouchFun(OnTouchDown, OnTouchMove, OnTouchUp)
	self.parent:SetUpdateFunc(OnUpdate)
end

function GUI.TouchCtrl:__delete()
	self.parent:SetTouchFun(0, 0, 0)
	self.parent:SetUpdateFunc(0)
end

-- 定位拖动坐标
function GUI.TouchCtrl:LocatePos(x, y, force)
	local change = false
	if self.node_width > self.__view_width or force then
		self.scroll_x = x
		self:__FixPosX(0)
		change = true
	end

	if self.node_height > self.__view_height or force then
		self.scroll_y = y
		self:__FixPosY(0)
		change = true
	end

	if change then
		self:__SetPosition(force)
	end
end

-- 定位拖动坐标
function GUI.TouchCtrl:ActionLocatePos(offset_x, offset_y, move_time, end_callback)
	local function OnActionEnd()
		if self.__action_locate_info then
			self.scroll_x = self.__action_locate_info.s_x + offset_x
			self.scroll_y = self.__action_locate_info.s_y + offset_y		
			self.__action_locate_info = nil
			self:__SetPosition()	
		end
			
		if end_callback then
			end_callback()
		end
	end
	local action_end_callback = GlobalCallbackMgr:AddCallBackFunc(OnActionEnd)
	CPPActionManager.MoveBy(self.__move_node:GetNode(), move_time or 0.5, offset_x, offset_y, action_end_callback)

	self.__action_locate_info = {}
	self.__action_locate_info.s_x = self.scroll_x
	self.__action_locate_info.s_y = self.scroll_y
	self.__action_locate_info.x, self.__action_locate_info.y = self.__move_node:GetPosition()
end

-- 定位拖动比例(x,y 范围 0-1)
function GUI.TouchCtrl:LocatePosScale(x, y)
	local change = false
	if self.node_width > self.__view_width then
		local len = math.abs(self.scroll_x_max - self.scroll_x_min)
		self.scroll_x = self.scroll_x_min + len * x
		self:__FixPosX(0)
		change = true
	end

	if self.node_height > self.__view_height then
		local len = math.abs(self.scroll_y_max - self.scroll_y_min)
		self.scroll_y = self.scroll_y_min + len * y
		self:__FixPosY(0)
		change = true
	end

	if change then
		self:__SetPosition()
	end
end

-- 定位到最前位置
function GUI.TouchCtrl:LocateToTop()
	local is_change = false
	if self.node_width > self.__view_width then
		self.scroll_x = self.scroll_x_max
		is_change = true
	end
 	if self.node_height > self.__view_height then
		self.scroll_y = self.scroll_y_min
		is_change = true
	end
	if is_change then
		self:__SetPosition()
	end
end

-- 判断是否已经定位到最前位置
function GUI.TouchCtrl:IsLocateToTop(item_width,item_height)
	local is_locate = true
	if self.node_width > self.__view_width then
		is_locate = self.scroll_x <= (self.scroll_x_max + item_width)
	end
 	if self.node_height > self.__view_height then
		is_locate = self.scroll_y <= (self.scroll_y_min + item_height)
	end
	return is_locate
end

-- 定位到最后位置
function GUI.TouchCtrl:LocateToLast()
	local is_change = false
	if self.node_width > self.__view_width then
		self.scroll_x = self.scroll_x_min
		is_change = true
	end
 	if self.node_height > self.__view_height then
		self.scroll_y = self.scroll_y_max
		is_change = true
	end
	if is_change then
		self:__SetPosition()
	end
end

-- 判断是否已经定位到最前位置
function GUI.TouchCtrl:IsLocateToLast(item_width,item_height)
	local is_locate = true
	if self.node_width > self.__view_width then
		is_locate = self.scroll_x >= (self.scroll_x_min - item_width)
	end
 	if self.node_height > self.__view_height then
		is_locate = self.scroll_y >= (self.scroll_y_max - item_height)
	end
	return is_locate
end

-- 设置移动节点尺寸
-- node_width	: 移动节点宽
-- node_height	: 移动节点高
function GUI.TouchCtrl:SetSize(node_width, node_height, not_set_pos)
	if self.node_width == node_width and self.node_height == node_height then
		return
	end

	self.node_width   = node_width
	self.node_height  = node_height
	self.scroll_x_min = self.__view_width - node_width	-- 最小滚动值
	self.scroll_y_min = 0	-- 最小滚动值
	self.scroll_x_max = 0	-- 最大滚动值
	self.scroll_y_max = node_height - self.__view_height	-- 最大滚动值

	if self.__view_width > node_width then
		self.scroll_x_max = self.scroll_x_min
	end
	if self.__view_height > node_height then
		self.scroll_y_max = self.scroll_y_min
	end

	if not_set_pos == nil then
		self.scroll_x = 0
		self.scroll_y = 0
		self:__SetPosition()
	end
	if self.scroll_bar_x ~= nil then
		self.scroll_bar_x:SetSize(self.node_width, self.__view_width, not_set_pos)
	end
	if self.scroll_bar_y ~= nil then
		self.scroll_bar_y:SetSize(self.node_height, self.__view_height, not_set_pos)
	end
end

-- 设置可视区域
function GUI.TouchCtrl:SetViewSize(view_x, view_y, view_width, view_height)
	self.__view_width  = view_width
	self.__view_height = view_height
	self.__move_bounce_x = self.__view_width / 2
	self.__move_bounce_y = self.__view_height / 2

	self.parent:SetTouchRect(view_x, view_y, self.__view_width, self.__view_height)	
	self.__scissor_node:SetContentSize(self.__view_width, self.__view_height)
	PosTool.Center(self.__move_node, self.scroll_x, self.scroll_y)
end

-- 设置裁剪区域
function GUI.TouchCtrl:SetScissorSize(width, height)
	self.__scissor_node:SetContentSize(width, height)
	PosTool.Center(self.__move_node, self.scroll_x, self.scroll_y)
end

-- 是否拖动过了
function GUI.TouchCtrl:IsMove()
	if self.is_just_stop_move then
		return true
	end
	local move_fast = math.abs(self.diff_x) > 50 or math.abs(self.diff_y) > 50
	local move_more = math.abs(self.touch_down_x-self.touch_move_x) > 60 or math.abs(self.touch_down_y-self.touch_move_y) > 60
	return move_fast or move_more
end

function GUI.TouchCtrl:GetScrollX()
	return self.scroll_x
end

function GUI.TouchCtrl:GetScrollY()
	return self.scroll_y
end

function GUI.TouchCtrl:GetScrollMinX()
	return self.scroll_x_min
end

function GUI.TouchCtrl:GetScrollMaxX()
	return self.scroll_x_max
end

function GUI.TouchCtrl:GetScrollMinY()
	return self.scroll_y_min
end

function GUI.TouchCtrl:GetScrollMaxY()
	return self.scroll_y_max
end

function GUI.TouchCtrl:GetNodeWidth()
	return self.node_width
end

function GUI.TouchCtrl:GetNodeHeight()
	return self.node_height
end

-- 获取滚动条刻度位置(0-1)横向
function GUI.TouchCtrl:GetLocatePosScaleX()
	local scale_x = 0
	local len = math.abs(self.scroll_x_max - self.scroll_x_min)
	if len > 0 then
		scale_x = (self.scroll_x - self.scroll_x_min) / len
	end
	return scale_x
end

-- 获取滚动条刻度位置(0-1)纵向
function GUI.TouchCtrl:GetLocatePosScaleY()
	local scale_y = 0
	local len = math.abs(self.scroll_y_max - self.scroll_y_min)
	if len > 0 then
		scale_y = (self.scroll_y - self.scroll_y_min) / len
	end
	return scale_y
end

function GUI.TouchCtrl:SetMoveOffx(move_off_x)
	self.__move_bounce_x = move_off_x
end

function GUI.TouchCtrl:SetMoveOffy(move_off_y)
	self.__move_bounce_y = move_off_y
end

function GUI.TouchCtrl:SetScrollCallBack(scroll_callback)
	self.scroll_callback = scroll_callback
end

function GUI.TouchCtrl:SetTouchUpCallback(touchup_callback)
	self.touchup_callback = touchup_callback
end

function GUI.TouchCtrl:SetFixPosXCallback(fixpos_callback)
	self.fixpos_callback = fixpos_callback
end

--设置滚动速度上限
function GUI.TouchCtrl:SetMaxScrollSpeed(speed_x, speed_y)
	self.__max_speed_x = speed_x
	self.__max_speed_y = speed_y
end

--用于SpecialScrollView调整
function GUI.TouchCtrl:AdjustForSpecialScrollView(is_horizon)
    if is_horizon then
        self.node_width = MathTool.CompareFloat(self.node_width, self.__view_width) and (self.node_width + 1) or self.node_width
    else
        self.node_height = MathTool.CompareFloat(self.node_height, self.__view_height) and (self.node_height + 1) or self.node_height
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
-- 设置面板位置
function GUI.TouchCtrl:__SetPosition(force)
	if self.scroll_bar_y ~= nil then
		local scale = self.scroll_y / (self.scroll_y_max-self.scroll_y_min)
		self.scroll_bar_y:SetCursorPos(scale)
	end
	if self.scroll_bar_x ~= nil then
		local scale = -self.scroll_x / (self.scroll_x_max-self.scroll_x_min)
		self.scroll_bar_x:SetCursorPos(scale)
	end
	PosTool.Center(self.__move_node, self.scroll_x, self.scroll_y)

	if self.scroll_callback then
		self.scroll_callback(force)
	end
end

-- x位置软修正(预留回弹空间)
function GUI.TouchCtrl:__FixPosX(off)	
	off = off or self.__move_bounce_x
	if self.fixpos_callback then
		self.scroll_x = self.fixpos_callback(self.scroll_x,self.diff_x)
		return 
	end		
	if self.scroll_x < self.scroll_x_min then
		if self.scroll_x < self.scroll_x_min - off then
			self.scroll_x = self.scroll_x_min - off
			self.speed_x = 0
		end
		self.out_x = self.scroll_x_min - self.scroll_x
	
	elseif self.scroll_x > self.scroll_x_max then
		if self.scroll_x > self.scroll_x_max + off then
			self.scroll_x = self.scroll_x_max + off
			self.speed_x = 0
		end
		self.out_x = self.scroll_x_max - self.scroll_x
	
	else
		self.out_x = 0
	end
end

-- 强制修正x位置
function GUI.TouchCtrl:__FixPosX2()
	if self.scroll_x < self.scroll_x_min and self.speed_x < 0 then
		self.scroll_x = self.scroll_x_min
		self.speed_x = 0
	
	elseif self.scroll_x > self.scroll_x_max and self.speed_x > 0 then
		self.scroll_x = self.scroll_x_max
		self.speed_x = 0
	end
end

-- y位置软修正(预留回弹空间)
function GUI.TouchCtrl:__FixPosY(off)
	off = off or self.__move_bounce_y
	if self.scroll_y < self.scroll_y_min then
		if self.scroll_y < self.scroll_y_min - off then
			self.scroll_y = self.scroll_y_min - off
			self.speed_y = 0
		end
		self.out_y = self.scroll_y_min - self.scroll_y

	elseif self.scroll_y > self.scroll_y_max then
		if self.scroll_y > self.scroll_y_max + off then
			self.scroll_y = self.scroll_y_max + off
			self.speed_y = 0
		end
		self.out_y = self.scroll_y_max - self.scroll_y
	
	else
		self.out_y = 0
	end
end

-- 强制修正y位置
function GUI.TouchCtrl:__FixPosY2()
	if self.scroll_y < self.scroll_y_min and self.speed_y < 0 then
		self.scroll_y = self.scroll_y_min
		self.speed_y = 0
	
	elseif self.scroll_y > self.scroll_y_max and self.speed_y > 0 then
		self.scroll_y = self.scroll_y_max
		self.speed_y = 0
	end
end

-- x轴移动结束检测（判断是否需要回弹）
function GUI.TouchCtrl:__CheckXMoveEnd()
	if self.out_x < 0 then
		self.speed_x = -__CalcSpeed(math.abs(self.out_x), self.acc)
	elseif self.out_x > 0 then
		self.speed_x = __CalcSpeed(math.abs(self.out_x), self.acc)
	end
	self.out_x = 0
end

-- Y轴移动结束检测（判断是否需要回弹）
function GUI.TouchCtrl:__CheckYMoveEnd()
	if self.out_y < 0 then
		self.speed_y = -__CalcSpeed(math.abs(self.out_y), self.acc)
	elseif self.out_y > 0 then
		self.speed_y = __CalcSpeed(math.abs(self.out_y), self.acc)
	end
	self.out_y = 0
end

-- 统计最近几个移动信息
function GUI.TouchCtrl:__GatherTouchMove()
	if #self.touch_move_dis_list >= 5 then
		table.remove(self.touch_move_dis_list, 1)
		table.remove(self.touch_move_time_delta_list, 1)
	end

	table.insert(self.touch_move_dis_list, {x = self.diff_x, y = self.diff_y})
	table.insert(self.touch_move_time_delta_list, self.interval / 1000)
end

-- 计算松开后的移动速度
function GUI.TouchCtrl:__CalculateTouchMoveVelocity()
	local total_time = 0
	if self.touch_move_time_delta_list then
		for _, time_delta in ipairs(self.touch_move_time_delta_list) do
			total_time = total_time + time_delta
		end
	end
	if total_time == 0 or total_time >= 0.5 then
		return 0, 0
	end

	local dis_x = 0
	local dis_y = 0
	for _, dis_info in ipairs(self.touch_move_dis_list) do
		dis_x = dis_x + dis_info.x
		dis_y = dis_y + dis_info.y
	end

	local speed_x = dis_x / total_time
	local speed_y = dis_y / total_time

	if self.__max_speed_x then
		if speed_x > 0 then
			speed_x = math.min(speed_x, self.__max_speed_x)
		else
			speed_x = math.max(speed_x, -self.__max_speed_x)
		end
	end

	if self.__max_speed_y then
		if speed_y > 0 then
			speed_y = math.min(speed_y, self.__max_speed_y)
		else
			speed_y = math.max(speed_y, -self.__max_speed_y)
		end
	end

	return speed_x, speed_y
end
