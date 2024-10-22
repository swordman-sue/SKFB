
--[[
%% @module: 滚动列表
%% @author: swordman sue
%% @description: 
	负责管理和布局滚动项
	节点结构说明:
	  self -> parent_node -> scissor_node -> move_node -> { scroll_item ... }
注：需手动释放(DeleteMe)
--]]

GUI = GUI or {}

GUI.ScrollView = GUI.ScrollView or BaseClass(Layer)

GUI.ScrollView.global_cache_id = 0
GUI.ScrollView.global_cache_map = {}

--脏位条目延迟删除时间
local dirty_item_del_time = 2

--[[
@ params：
	必须：
	item_class			[table]滚动项类(继承GUI.ScrollItem，需重写SetData函数)
    item_width			[number]滚动项宽
    item_height			[number]滚动项高
    row					[number]可视行
    col					[number]可视列

    可选：
    horizon 			[bool]是否横向排列(默认false)
    item_space_r		[number]滚动项行间隔(默认0)
    item_space_c		[number]滚动项列间隔(默认0)
    filter_func			[function]筛选函数(返回true则通过，否则过滤)
    view_width 			[number]可视区域宽
    view_height 		[number]可视区域高
    update_move_func 	[function]移动更新回调
    structre_params 	[table]构造参数
    is_dynamic_create 	[bool]是否动态创建

    注：由触碰节点(默认为根节点)监听触碰事件，可重写GetTouchNode函数重定义触碰节点
    selectable			[bool]是否可选(类似单选框，可重写SetSelected函数重定义选中逻辑)
    select_callback		[function]选中回调
    select_filter_func 	[function]选中过滤(返回true则通过，否则过滤)
    select_filter_func_callback 	[function]选中过滤后的回调
    touchable 			[bool]是否可点击
    touch_callback 		[bool]点击回调
--]]
function GUI.ScrollView:__init(params)
	self.__item_class = params.item_class
	self.__row = params.row
	self.__col = params.col

	self.__item_map = {}
	self.__data_list = {}
	self.__dirty_item_map = nil

	--横向排列
	self.__horizon = params.horizon ~= nil and params.horizon or false	
	--筛选函数
	self.__filter_func = params.filter_func or nil

	self.__org_item_size = COCOSize(params.item_width, params.item_height)

	--根据行/列间隔调整尺寸
	self.__item_space_c = params.item_space_c or 0
	self.__item_space_r = params.item_space_r or 0
	self.__item_width = self.__org_item_size.width + self.__item_space_c
	self.__item_height = self.__org_item_size.height + self.__item_space_r

	--可视区域
	self.__view_width = self.__col * self.__item_width - self.__item_space_c
	self.__view_height = self.__row * self.__item_height - self.__item_space_r
	self.__half_view_width = self.__view_width / 2
	self.__half_view_height = self.__view_height / 2
	self.__view_x = -self.__half_view_width
	self.__view_y = -self.__half_view_height	

	--点击/选择相关
	self.__selectable = params.selectable
	self.__select_callback = params.select_callback
	self.__select_filter_func = params.select_filter_func
	self.__select_filter_func_callback = params.select_filter_func_callback
	self.__touchable = params.touchable
	self.__touch_callback = params.touch_callback

	--滚动回调
	self.__move_callback = params.update_move_func

	--构造参数
	self.__structre_params = params.structre_params	

	--是否动态创建
	self.__is_dynamic_create = params.is_dynamic_create == nil and true or params.is_dynamic_create

	--移动节点的父节点(设置可视区域用, 滚动条等控件须是兄弟目录)
	self.__parent_node = Layer.New()
	self:AddChild(self.__parent_node)

	self.__scissor_node = UILayer.New()
	self.__scissor_node:SetAnchorPoint(0.5, 0.5)
	self.__scissor_node:SetClippingEnable(true)
	self.__parent_node:AddChild(self.__scissor_node)

	--创建移动节点(目标对象列表挂在其下面)
	self.__move_node = UILayer.New()
	self.__move_node:SetPosition(0, 0)
	self.__scissor_node:AddChild(self.__move_node)

	--创建拖动控件
	local move_node_offsetx = 0
	local move_node_offsety = 0
	if params.view_width or params.view_height then
		self.__view_width = params.view_width or self.__view_width
		self.__view_height = params.view_height or self.__view_height
		self.__half_view_width = self.__view_width / 2
		self.__half_view_height = self.__view_height / 2
		self.__view_x = -self.__half_view_width
		self.__view_y = -self.__half_view_height
	end
	self.__touch_controller = GUI.TouchCtrl.New(
		self.__parent_node, self.__scissor_node, self.__move_node, self.__view_x, self.__view_y, self.__view_width, self.__view_height)

	self.__touch_controller:SetMaxScrollSpeed(1200, 1200)

	--设置更新回调
	self:SetUpdateFunc(function(elapse_time)
		self:Update(elapse_time)
	end)

	--设置滚动回调
	self.__touch_controller:SetScrollCallBack(function(force)
		self:__ScrollCallBack(force)
	end)

	if StartConfig.IsDebug then
	    GUI.ScrollView.global_cache_id = GUI.ScrollView.global_cache_id + 1
	    self.__global_cache_id = GUI.ScrollView.global_cache_id
	    GUI.ScrollView.global_cache_map[self.__global_cache_id] = debug.traceback()    	
	end
end

function GUI.ScrollView:__delete()
	if StartConfig.IsDebug then
	    GUI.ScrollView.global_cache_map[self.__global_cache_id] = nil
	end

	self:__ClearItems(true)

	if self.__touch_controller then
		self.__touch_controller:DeleteMe()
		self.__touch_controller = nil
	end

	if self.__flyin_action_timer then
		GlobalTimerQuest:CancelQuest(self.__flyin_action_timer)
		self.__flyin_action_timer = nil
	end

	if self.__item_action_timer then
		GlobalTimerQuest:CancelQuest(self.__item_action_timer)
		self.__item_action_timer = nil
	end
end

function GUI.ScrollView.PrintCache()
	print(8, "---------------GUI.ScrollView:PrintCache---------------")
	for _, info in pairs(GUI.ScrollView.global_cache_map) do
		LogTool.LogInfo(info)
	end
end

function GUI.ScrollView:Update(elapse_time)
	--延时显示列表项
	if self.__show_items_delay_time and self.__show_items_delay_time <= Macros.Global.NowTimeMilli then
		self:__ShowItems()
	end

	--清空脏位表中的条目
	self:__ClearDirtyItems(elapse_time)
end

function GUI.ScrollView:GetItemWidth()
	return self.__item_width
end

function GUI.ScrollView:GetItemHeight()
	return self.__item_height
end

--获取scrollView控件的宽
function GUI.ScrollView:GetViewWidth()
	return self.__view_width
end

--获取scrollView控件的高
function GUI.ScrollView:GetViewHeight()
	return self.__view_height
end

--获取可视行数
function GUI.ScrollView:GetRowNum()
	return self.__row
end

--获取可视列数
function GUI.ScrollView:GetColNum()
	return self.__col
end

--修改裁剪区域
function GUI.ScrollView:SetScissorSize(width, height)
	if width or height then
		width = width or self.__view_width
		height = height or self.__view_height
		self.__touch_controller:SetScissorSize(width, height)
	end	
end

--设置数据列表
function GUI.ScrollView:SetDataList(data_list)
	self:__ClearItems()

	self:RefreshLayout(data_list)
	self:LocateToTop()
end

--刷新列表
function GUI.ScrollView:RefreshLayout(data_list)
	self:__InitDataList(data_list)
	self:__SetTouchSize()
	self:__ShowItems()
end

--插入滚动项
function GUI.ScrollView:AttachItem(data, index, do_layout)
	if not data then
		return
	end

	if #self.__data_list == 0 then
		index = nil
	end
	if index and (index <= 0 or index > #self.__data_list + 1) then
		LogTool.LogError("[GUI.ScrollView:AttachItem] 参数index无效")
		return
	end
	if index == #self.__data_list + 1 then
		--当作插入到尾部
		index = nil		
	end

	--插入项
	if index then
		--更新数据表
		table.insert(self.__data_list, index, data)

		--更新条目表
		local item
		for i = table.maxn(self.__item_map), index, -1 do
			item = self.__item_map[i]
			if item then
				self.__item_map[i] = nil
				self.__item_map[i + 1] = item
				item:SetIndex(i + 1)
			end
		end
	else
		--更新数据表
		table.insert(self.__data_list, data)
	end

	--清空脏位表
	self.__dirty_item_map = nil

	--刷新列表
	if do_layout ~= false then
		self:RefreshLayout()
	end
end

--移除滚动项
function GUI.ScrollView:DetachItem(index, do_layout)
	if index <= 0 or index > #self.__data_list then
		return
	end

	--更新数据表
	table.remove(self.__data_list, index)
	
	--更新条目表
	local item = self.__item_map[index]
	self.__item_map[index] = nil
	if item then
		item:DeleteMe()
	end
	for i = index + 1, table.maxn(self.__item_map) do
		item = self.__item_map[i]
		if item then
			self.__item_map[i] = nil
			self.__item_map[i - 1] = item
			item:SetIndex(i - 1)
		end
	end

	--清空脏位表
	self.__dirty_item_map = nil

	--刷新列表
	if do_layout ~= false then
		self:RefreshLayout()
	end
end

--将某项放到最后
function GUI.ScrollView:MoveItemToLast(index)
	if index <= 0 or index > #self.__data_list then
		return
	end

	local data = self.__data_list[index]
	self:DetachItem(index, false)
	self:AttachItem(data)
end

--将某项移动到某处
function GUI.ScrollView:MoveItemTo(index, to_index)
	if index <= 0 or index > #self.__data_list then
		return
	end
	if to_index <= 0 or to_index > #self.__data_list then
		return
	end
	if index == to_index then
		return
	end

	local data = self.__data_list[index]
	self:DetachItem(index, false)

	if index < to_index then
		self:AttachItem(data, to_index - 1)
	else
		self:AttachItem(data, to_index)
	end
end

function GUI.ScrollView:GetItem(item_index, auto_create)
	if not item_index then
		return nil
	end

	local item
	if auto_create then
		item = self:__CreateItem(item_index)
	else
		item = self.__item_map[item_index]
	end
	return item
end

function GUI.ScrollView:GetItemData(item_index)
	if not self.__data_list then
		return
	end

	return self.__data_list[item_index]
end

function GUI.ScrollView:GetItemCount()
	return #self.__data_list
end

--让滚动容器根据item序号滚动定位到某个item的位置
function GUI.ScrollView:LocateItem(index, force)
	index = math.max(1, index)
	index = math.min(index, #self.__data_list)

	--第一个条目的位置
	local pos_x_1, pos_y_1 = self:__CalcPositionByIndex(1)

	--目的条目的位置
	local pos_x, pos_y = self:__CalcPositionByIndex(index)

	self:LocatePos(pos_x_1 - pos_x, pos_y_1 - pos_y, force)
end

--条目迭代器(已创建条目)
function GUI.ScrollView:ItemIter(iter_func)
	if not iter_func then
		return
	end

	local item
	for index, data in ipairs(self.__data_list) do
		item = self:GetItem(index)
		if item then
			if iter_func(index, item, item:GetData()) then
				break
			end
		end
	end
end

--条目数据迭代器(所有条目数据)
function GUI.ScrollView:ItemDataIter(iter_func)
	if not iter_func then
		return
	end

	for index, data in ipairs(self.__data_list) do
		if iter_func(index, data, self:GetItem(index)) then
			break
		end
	end
end

--选中某条目
function GUI.ScrollView:SelectItem(index, is_need_callback)
	if not index then
		return
	end

	if self.__shield_selectitem then
		return
	end

	if is_need_callback == nil then
		is_need_callback = true
	end

	local select_changed = self:__IsSelectedChanged(index)
	if select_changed then
		local selected_item = self:GetItem(index, true)
		if not selected_item then
			return
		end

		if self.__select_filter_func then
			--选择条件过滤
			if not self.__select_filter_func(index, selected_item:GetData()) then
				if self.__select_filter_func_callback then
					self.__select_filter_func_callback(index,selected_item:GetData())
				end
				return
			end
		end

	    if selected_item then
	    	--选择下一个
	        selected_item:SetSelected(true)
	    end		

		local last_selected_item = self:GetItem(self.__selected_item_index) 
	    if last_selected_item then
	    	--反选上一个
	        last_selected_item:SetSelected(false)
	    end

	    --记录选中条目索引
	    self.__selected_item_index = index

	    --选中回调
	    if self.__select_callback and is_need_callback then
	        self.__select_callback(self.__selected_item_index, selected_item:GetData())
	    end
	end
end

--选中&定位某条目
function GUI.ScrollView:SelectAndLocateItem(index)
	self:SelectItem(index)
	self:LocateItem(index)
end

--取消选中条目
function GUI.ScrollView:UnSelectItem()
	local selected_item = self:GetItem(self.__selected_item_index)
	if selected_item then
		selected_item:SetSelected(false)
	end
	self.__selected_item_index = nil
end

--获取选中条目
function GUI.ScrollView:GetSelectedItem()
	return self:GetItem(self.__selected_item_index)
end

--设置屏蔽选中条目
function GUI.ScrollView:SetShieldSelectedItem(is_shield)
	self.__shield_selectitem = is_shield
end

--点击某条目
function GUI.ScrollView:TouchItem(index)
	if not index then
		return
	end 

    local touched_item = self:GetItem(index, true)
    if not touched_item then
    	return
    end

    if self.__touch_callback then
        self.__touch_callback(index, touched_item:GetData())
    end
end

function GUI.ScrollView:GetTouchCtrl()
	return self.__touch_controller
end

--是否拖动了
function GUI.ScrollView:IsMove()
	if self.__touch_controller then
		return self.__touch_controller:IsMove()
	end
	return false
end

--滚动到最前位置
function GUI.ScrollView:LocateToTop()
	if self.__touch_controller ~= nil then
		self.__touch_controller:LocateToTop()
	end
end

--是否滚动到最前位置
function GUI.ScrollView:IsLocateToTop(item_width,item_height)	
	local cur_item_width = item_width and item_width or self.__org_item_size.width/2
	local cur_item_height = item_height and item_height or self.__org_item_size.height/2
	if self.__touch_controller ~= nil then
		return self.__touch_controller:IsLocateToTop(cur_item_width,cur_item_height)
	end
end

--滚动到最后位置
function GUI.ScrollView:LocateToLast()
	if self.__touch_controller ~= nil then
		self.__touch_controller:LocateToLast()
	end
end

--是否滚动到最后位置
function GUI.ScrollView:IsLocateToLast(item_width,item_height)
	local cur_item_width = item_width and item_width or self.__org_item_size.width/2
	local cur_item_height = item_height and item_height or self.__org_item_size.height/2
	if self.__touch_controller ~= nil then
		return self.__touch_controller:IsLocateToLast(cur_item_width,cur_item_height)
	end
end

--定位到某位置
function GUI.ScrollView:LocatePos(x, y, force)
	if self.__touch_controller then
		self.__touch_controller:LocatePos(x, y, force)
	end
end

--定位到某位置(带动作效果)
function GUI.ScrollView:ActionLocatePos(offset_x, offset_y, move_time, end_callback)
	if self.__touch_controller then
		self.__touch_controller:ActionLocatePos(offset_x, offset_y, move_time, end_callback)
	end
end

function GUI.ScrollView:GetScrollX()
	if self.__touch_controller then
		return self.__touch_controller:GetScrollX()
	end
	return 0
end

function GUI.ScrollView:GetScrollY()
	if self.__touch_controller then
		return self.__touch_controller:GetScrollY()
	end
	return 0
end

function GUI.ScrollView:GetScrollMinX()
	if self.__touch_controller then
		return self.__touch_controller:GetScrollMinX()
	end
	return 0
end

function GUI.ScrollView:GetScrollMaxX()
	if self.__touch_controller then
		return self.__touch_controller:GetScrollMaxX()
	end
	return 0
end

function GUI.ScrollView:GetScrollMinY()
	if self.__touch_controller then
		return self.__touch_controller:GetScrollMinY()
	end
	return 0
end

function GUI.ScrollView:GetScrollMaxY()
	if self.__touch_controller then
		return self.__touch_controller:GetScrollMaxY()
	end
	return 0
end

function GUI.ScrollView:SetTouchEnabled(var)
	if self.__parent_node then
		self.__parent_node:SetTouchEnabled(var)
	end
end

function GUI.ScrollView:SetTouchSwallow(var)
	if self.__parent_node then
		self.__parent_node:SetTouchSwallow(var)
	end
end

--播放条目飞入效果
function GUI.ScrollView:PlayFlyInAction(offset_x, offset_y, flyin_interval, flyin_time, finished_callback)
	flyin_interval = flyin_interval or 0.1
	flyin_time = flyin_time or 0.3

	if self.__flyin_action_timer then
		return
	end

	self.__is_in_flyaction = true
	self:SetShieldSelectedItem(true)
	self:SetTouchEnabled(false)

	local item, item_num
	item_num = self.__horizon and self.__col or self.__row
	for i = 1, item_num do
		item = self:GetItem(i)
		if item then
			if not offset_x and not self.__horizon then
				offset_x = -item:GetWidth()
			elseif not offset_y and self.__horizon then
				offset_y = -item:GetHeight()
			end
			offset_x = offset_x or 0
			offset_y = offset_y or 0

			item:SetPosition(item:GetPositionX() + offset_x, item:GetPositionY() + offset_y)
		end
	end

	local flyin_index = 1
	local function FlyinFinishedFunc()
		self.__is_in_flyaction = false			
		self:SetShieldSelectedItem(false)
		self:SetTouchEnabled(true)

		if finished_callback then
			finished_callback()
		end
	end

	self.__flyin_action_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
	function()
		local flyin_finished_func_id
		if flyin_index == self.__row then
			flyin_finished_func_id = GlobalCallbackMgr:AddCallBackFunc(FlyinFinishedFunc)
			GlobalTimerQuest:CancelQuest(self.__flyin_action_timer)
			self.__flyin_action_timer = nil
		end

		item = self:GetItem(flyin_index)
		if item then
			CPPActionManager.MoveBy(item:GetNode(), flyin_time, -offset_x, -offset_y, flyin_finished_func_id)
			item:SetOffsetX(0)
		end
		flyin_index = flyin_index + 1

	end), flyin_interval, -1)	
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function GUI.ScrollView:__CreateItem(index)
	if self.__item_map[index] then
		return self.__item_map[index]
	end

	local data = self.__data_list[index]
	if not data then
		LogTool.LogError("[GUI.ScrollView:__CreateItem] 列表条目找不到对应数据")
		return
	end

	if type(data) ~= "table" then
		LogTool.LogError("[GUI.ScrollView:__CreateItem] 列表条目数据类型不合法 type:%s", type(data))
		return
	end

	--创建项
	local item
	local structre_params = data.structre_params or self.__structre_params
	if structre_params then
		item = self.__item_class.New(unpack(structre_params))
	else	
		item = self.__item_class.New()
	end
	
	if not item.SetData then
		item:DeleteMe()
		LogTool.LogError("[GUI.ScrollView:__CreateItem] 列表条目找不到SetData方法")
		return
	end	

	item:SetIndex(index)

	item:SetAnchorPoint(0.5, 0.5)

	--如果布局文件中根节点的尺寸与设定值不一致，则重新设置
	local org_size = item:GetContentSize()
	if not org_size:Equals(self.__org_item_size) then
		item:SetContentSize(self.__org_item_size.width, self.__org_item_size.height)
		now_size = item:GetContentSize()
		item:RefreshLayoutComponent()
	end

	--设置数据
	item:SetData(data)

	--设置触碰过滤器
	if not self.__item_touch_filter then
		self.__item_touch_filter = function()
			return self:IsMove()
		end
	end
	item:SetTouchFilter(self.__item_touch_filter)

	--设置触碰事件
	if self.__selectable then
		local touch_node = item:GetTouchNode()
		touch_node:SetTouchEnabled(true)
		touch_node:SetTouchSwallow(false)
		WidgetUnity.SetWidgetListener(touch_node,function ()
			item:TouchBegin()
		end, function ()
			item:TouchMoved()
		end, function()
			item:TouchEnd()
			if not item:IsTouchEnable() then
				return
			end

			self:SelectItem(item:GetIndex())
		end,function ()
			item:TouchCancel()
		end)
	elseif self.__touchable then
		local touch_node = item:GetTouchNode()
		touch_node:SetTouchEnabled(true)
		touch_node:SetTouchSwallow(false)
		WidgetUnity.SetWidgetListener(touch_node, nil, nil, function()
			if not item:IsTouchEnable() then
				return
			end

			self:TouchItem(item:GetIndex())
		end)
	end	

	self.__item_map[index] = item
	self.__move_node:AddChild(item:GetNode(),item:GetNode():GetZOrder())	

	return item
end

--显示列表项
function GUI.ScrollView:__ShowItems()
	if self.__is_in_flyaction then
		return
	end

	--清除延时显示列表项的标识
	self.__show_items_delay_time = nil

	--记录到脏位表，延时删除		
	if self.__is_dynamic_create then
		self.__dirty_item_map = self.__dirty_item_map or {}

		for index, item in pairs(self.__item_map) do
			item:SetVisible(false)

			if not self.__dirty_item_map[index] then
				self.__dirty_item_map[index] = Macros.Global.NowTime + dirty_item_del_time
			end
		end
	end

	local item, item_check
	local pos_x, pos_y
	local min_x = -self.__half_view_width - self:GetScrollX() - self.__item_width / 2
	local max_x = min_x + self.__view_width + self.__item_width
	local max_y = self.__half_view_height - self:GetScrollY() + self.__item_height / 2
	local min_y = max_y - self.__view_height - self.__item_height
	for view_index, index in ipairs(self.__view_data_list) do
		pos_x, pos_y = self:__CalcPositionByIndex(view_index)

		--可视化检测
		in_view_box = false
		in_view_offset = 100
		if self.__horizon then
			if pos_x >= min_x - in_view_offset and pos_x <= max_x + in_view_offset then
				in_view_box = true
			end
		else
			if pos_y >= min_y - in_view_offset and pos_y <= max_y + in_view_offset then
				in_view_box = true
			end
		end
		if in_view_box or not self.__is_dynamic_create then
			--处于可视范围
			item_check = self:GetItem(index)
			item = self:GetItem(index, true)
			if item then
				item:SetVisible(true)
				item:SetPosition(pos_x + item:GetOffsetX(), pos_y + item:GetOffsetY())
				item:SetViewIndex(view_index)

				--重新进入可视范围，更新选中标记
				if item and not item_check then
					if index == self.__selected_item_index then
						item:SetSelected(true)
					end
				end
			end

			--从脏位表移除
			if self.__dirty_item_map then
				self.__dirty_item_map[index] = nil
			end
		end
	end
end

--清空脏位表中的条目
function GUI.ScrollView:__ClearDirtyItems(elapse_time)
	if not self.__dirty_item_map then
		return
	end

	self.__dirty_elapse_time = self.__dirty_elapse_time or 0
	self.__dirty_elapse_time = self.__dirty_elapse_time + elapse_time
	if self.__dirty_elapse_time >= 0.5 then
		self.__dirty_elapse_time = nil

		local beg_time = CPPGameEngine:GetNowTime()
		local limit_time = 1 / CPPSceneManager:GetGameFrame() * 0.5

		local item
		for index, time_to_del in pairs(self.__dirty_item_map) do			
			item = self.__item_map[index]
			if item and Macros.Global.NowTime >= time_to_del then
				item:DeleteMe()
				self.__item_map[index] = nil
				self.__dirty_item_map[index] = nil

				if CPPGameEngine:GetNowTime() - beg_time > limit_time then
					--超时处理
					return
				end
			end
		end
	end
end

--清空列表项
function GUI.ScrollView:__ClearItems(cleanup)
	self.__selected_item_index = nil
	self.__dirty_item_map = nil

	if self.__item_map then
		for _, item in pairs(self.__item_map) do
			item:DeleteMe()
		end

		if cleanup then
			self.__item_map = nil
		else
			self.__item_map = {}
		end
	end
end

--初始化数据表
function GUI.ScrollView:__InitDataList(data_list)
	if data_list then
		self.__data_list = CPPGameLib.CopyListShallow(data_list)
	end

	self.__view_data_num = 0
	self.__view_data_list = {}
	for index, data in ipairs(self.__data_list) do
		if not self.__filter_func or self.__filter_func(data) then
			self.__view_data_num = self.__view_data_num + 1
			table.insert(self.__view_data_list, index)
		end
	end
end

--设置移动节点尺寸
function GUI.ScrollView:__SetTouchSize()
	if self.__touch_controller then
		local node_w = 0
		local node_h = 0
		if self.__horizon then
			node_w = self.__item_width * math.ceil(self.__view_data_num / self.__row) - self.__item_space_c
			node_h = self.__row * self.__item_height - self.__item_space_r
		else
			node_w = self.__col * self.__item_width - self.__item_space_c
			node_h = self.__item_height * math.ceil(self.__view_data_num / self.__col) - self.__item_space_r
		end
		self.__touch_controller:SetSize(node_w, node_h)
	end
end

--列表滚动回调
function GUI.ScrollView:__ScrollCallBack(force)
	if force then
		self:__ShowItems()

	elseif not self.__show_items_delay_time then
		self.__show_items_delay_time = Macros.Global.NowTimeMilli + 100
	end

	if self.__move_callback then
		self.__move_callback()
	end
end

--根据次序计算坐标
function GUI.ScrollView:__CalcPositionByIndex(i)
	--计算页面偏移
	local i_page = math.floor((i-1) / (self.__row*self.__col))
	local pg_off_x = 0
	local pg_off_y = 0

	if self.__horizon then
		pg_off_x = self.__item_width*self.__col * i_page
		pg_off_y = 0
	else
		pg_off_x = 0
		pg_off_y = -self.__item_height*self.__row * i_page
	end

	--排除页偏移后得到的二级索引是i_2
	local i_2 = (i-1) % (self.__row*self.__col)

	--得到最终坐标(页偏移+(-半屏)+半单位+行/列坐标)
	pos_x = pg_off_x - self.__half_view_width + self.__org_item_size.width/2 + (i_2%self.__col) * self.__item_width
	pos_y = pg_off_y + self.__half_view_height - self.__org_item_size.height/2 - math.floor(i_2/self.__col) * self.__item_height

	return pos_x, pos_y
end

--返回选择结果
function GUI.ScrollView:__IsSelectedChanged(index)
	return not self.__selected_item_index or self.__selected_item_index ~= index
end
