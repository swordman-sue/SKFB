
--[[
%% @module: 翻页列表
%% @description: 
	负责管理翻页项
注：需手动释放(DeleteMe)
--]]

GUI = GUI or {}

GUI.PageView = GUI.PageView or BaseClass(Layer)

GUI.PageView.EVENT_TYPE = {
	TURN_PAGE_ENDED = 0,	--翻页结束
	TURNING_PAGE = 1,		--正在翻页
	AUTO_TURNING_PAGE = 2,	--自动翻页
}

--脏位条目延迟删除时间
GUI.PageView.DEL_PAGETIME = 2 	
--更新脏位表的时间
GUI.PageView.UPDATE_DIRTYTIME = 1 

--[[
@ params：
	必须：
    view_width 			[number]可视区域宽
    view_height 		[number]可视区域高   
    page_class			[table]页面类
    create_func			[function]自己创建滚动项方法，需return创建的滚动项

    可选：   
    first_scroll_page	[number]首次需要滚动到的页面(不填默认为第一个页面)
    update_move_func 	[function]移动更新回调
    structre_params 	[table]构造参数
    is_dynamic_create 	[bool]是否动态创建
    turn_page_func 		[function]翻页回调
	filter_func			[function]筛选函数(返回true则通过，否则过滤)
--]]

function GUI.PageView:__init(params)
	self.__pageview = UIPageViewEx.Create()
	self.__pageview:SetAnchorPoint(0.5,0.5)
	self.__pageview:SetContentSize(params.view_width, params.view_height)
	self:AddChild(self.__pageview)
	PosTool.Center(self.__pageview)

	self.__page_map = {}
	self.__page_data_list = {}
	self.__dirty_page_map = nil

	self.__page_class = params.page_class
	self.__create_func = params.create_func

	self.__move_callback = params.update_move_func

	self.__structre_params = params.structre_params

	self.__is_dynamic_create = params.is_dynamic_create == nil and true or params.is_dynamic_create

	--筛选函数
	self.__filter_func = params.filter_func or nil

	--页区域的一半
	self.__half_page_width = params.view_width / 2
	self.__half_page_height = params.view_height / 2
	--可视区域
	self.__view_width = params.view_width

	--翻页回调
	self.__turn_page_func = params.turn_page_func

	--首次需要滚动到的页面
	self.__first_scroll_page = params.first_scroll_page or 0

	--设置更新回调
	self:SetUpdateFunc(function(elapse_time)
		self:Update(elapse_time)
	end)

	--翻页监听
	local function pageViewEvent(event_type)		
		if event_type == GUI.PageView.EVENT_TYPE.TURN_PAGE_ENDED then
        	--翻页结束
        	if self.__turn_page_func then
        		self.__turn_page_func()
        	end

        	self:__ShowPages()

       	elseif event_type == GUI.PageView.EVENT_TYPE.TURNING_PAGE then
       		if self.__is_dynamic_create then
	       		--正在翻页
	       		self:__TurningPageCallBack()
	       	end

	    elseif event_type == GUI.PageView.EVENT_TYPE.AUTO_TURNING_PAGE then
	    	if self.__is_dynamic_create and not self.__is_init_scroll then
	       		--正在翻页
	       		self:__TurningPageCallBack()
	       	end
       	end
    end
	WidgetUnity.SetPageViewListener(self.__pageview, pageViewEvent, true)
end

function GUI.PageView:__delete()
	self:__ClearPages(true)
end

--设置数据列表
function GUI.PageView:SetDataList(data_list, create_num)
	self:__ClearPages()

	if not data_list and create_num then
		data_list = {}
		for i = 1, create_num do
			table.insert(data_list, {page_index = i})
		end
	end

	self:__InitDataList(data_list)
	self:__ShowPages(true)
end

--更新
function GUI.PageView:Update(elapse_time)
	self.__dirty_elapse_time = self.__dirty_elapse_time or 0
	self.__dirty_elapse_time = self.__dirty_elapse_time + elapse_time
	if self.__dirty_elapse_time >= GUI.PageView.UPDATE_DIRTYTIME then
		self.__dirty_elapse_time = nil
		--清空脏位表
		self:__ClearDirtyPageMap()
	end
end

--整理结构
function GUI.PageView:ForceDoLayout()
	if self.__pageview then
		self.__pageview:ForceDoLayout()
	end
end

--滚到对应的页面
function GUI.PageView:ScrollToPage(page,is_init)
	--是否初始滚动
	self.__is_init_scroll = is_init
	if self.__pageview then	
		self.__pageview:ScrollToPage(page)
	end
end

function GUI.PageView:ScrollToPageByTime(page,pass_time)
	if self.__pageview then
		self.__pageview:ScrollToPageByTime(page,pass_time)
	end
end

function GUI.PageView:GetScollX()
	local inner_container_x = 0
	if self.__pageview then
		inner_container_x = self.__pageview:GetInnerContainerPosition()
	end
	return inner_container_x
end

--获取页面数据(索引从1开始)
function GUI.PageView:GetPageData(index)
	return self.__page_data_list[index]
end

--获取当前内容页面Index(索引从1开始)
function GUI.PageView:GetCurContentPageIndex()
	if not self.__view_index_to_index_map then
		return 1
	end

	local cur_page_index = self:GetCurPageIndex()
	return self.__view_index_to_index_map[cur_page_index + 1]	
end

--获取当前内容页面
function GUI.PageView:GetCurContentPage()
	local cur_content_page_index = self:GetCurContentPageIndex()
	return self.__page_map[cur_content_page_index]
end

--获取内容页面
function GUI.PageView:GetContentPage(index)
	return self.__page_map[index]
end

--获取页面Index(索引从0开始)
function GUI.PageView:GetCurPageIndex()
	if self.__pageview then
		return self.__pageview:GetCurPageIndex()
	end
	return 0
end

--获取当前页面
function GUI.PageView:GetCurPage()
	if self.__pageview then
		return self.__pageview:GetCurPage()
	end
end

--获取页面(索引从0开始)
function GUI.PageView:GetPage(index)
	if self.__pageview then
		return self.__pageview:GetPage(index)
	end
end

--获取页面数量
function GUI.PageView:GetPageNum()
	if self.__pageview then
		return self.__pageview:GetPageNum()
	end
	return 0
end

--清除所有页面
function GUI.PageView:RemoveAllPages()
	if self.__pageview then
		self.__pageview:RemoveAllPages()
	end
end

function GUI.PageView:SetTouchEnabled(enabled)
	if self.__pageview then
		self.__pageview:SetTouchEnabled(enabled)
	end
end

--内容页迭代器(已创建内容页)
function GUI.PageView:PageIter(iter_func)
	if not iter_func then
		return
	end

	local page, page_node
	for index, data in ipairs(self.__page_data_list) do
		page = self:GetContentPage(index)
		if page then
			if iter_func(index, page, data) then
				break
			end
		end
	end
end

--内容页数据迭代器(所有内容页数据)
function GUI.PageView:PageDataIter(iter_func)
	if not iter_func then
		return
	end

	local page
	for index, data in ipairs(self.__page_data_list) do
		page = self:GetContentPage(index)
		if iter_func(index, data, page) then
			break
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------

--初始化数据表
function GUI.PageView:__InitDataList(data_list)
	if data_list then
		self.__page_data_list = CPPGameLib.CopyListShallow(data_list)
	end

	self.__view_page_data_list = {}
	self.__view_index_to_index_map = {}

	if self.__page_data_list then
		for index, data in ipairs(self.__page_data_list) do
			if not self.__filter_func or self.__filter_func(data) then
				table.insert(self.__view_page_data_list, index)
				self.__view_index_to_index_map[#self.__view_page_data_list] = index
			end
		end
	end
end

--显示页面
function GUI.PageView:__ShowPages(is_init)
	local now_time = CPPGameEngine:GetNowTime()

	--记录到脏位表，延时删除		
	if self.__is_dynamic_create then
		self.__dirty_page_map = self.__dirty_page_map or {}

		for index, content_page in pairs(self.__page_map) do
			content_page:SetVisible(false)

			self.__dirty_page_map[index] = self.__dirty_page_map[index] or now_time
		end
	end

	self.__visual_page_map = {}
	local bm_page
	local in_view_box
	local min_x = -self.__half_page_width - self:GetScollX()
	local max_x = min_x + self.__view_width * 2
	for view_index, index in ipairs(self.__view_page_data_list) do
		--容器页面
		bm_page = self.__pageview:GetPage(view_index - 1)
		if not bm_page then
			bm_page = Util:Layout(1, 1)
			self.__pageview:AddPage(bm_page)
			PosTool.Center(bm_page)
		end

		in_view_box = self:__CheckPageVisible(view_index, min_x, max_x)

		if (is_init and self.__first_scroll_page == (view_index - 1)) or in_view_box or not self.__is_dynamic_create then
			self.__visual_page_map[index] = true

			--内容页面
			local data = self.__page_data_list[index]
			local content_page = self.__page_map[index]
			if data and not content_page then
				local page_class = data.page_class or self.__page_class
				if page_class then
					local structre_params = data.structre_params or self.__structre_params
					if structre_params then
						content_page = page_class.New(unpack(structre_params))
					else
						content_page = page_class.New()
					end				
					content_page:SetData(data)					
					bm_page:AddChild(content_page:GetNode())

				elseif self.__create_func then
					content_page = self.__create_func(view_page_index, data)
					if content_page then
						if content_page.GetNode then
							bm_page:AddChild(content_page:GetNode())
						else
							bm_page:AddChild(content_page)
						end
					end
				end
				self.__page_map[index] = content_page
			end

			if content_page then
				content_page:SetVisible(true)
			end

			--从脏位表移除
			if self.__dirty_page_map then
				self.__dirty_page_map[index] = nil
			end
		end
	end

	if is_init and self.__first_scroll_page ~= 0 then	
		self:ForceDoLayout()
		self:ScrollToPage(self.__first_scroll_page,true)
	end
end

--清空脏位表
function GUI.PageView:__ClearDirtyPageMap()
	if self.__dirty_page_map then
		local page
		local beg_time = CPPGameEngine:GetNowTime()
		local limit_time = 1 / CPPSceneManager:GetGameFrame() * 0.5

		for index, mark_time in pairs(self.__dirty_page_map) do
			if beg_time - mark_time >= GUI.PageView.DEL_PAGETIME then
				page = self.__page_map[index]
				if page then
					if page.DeleteMe then
						page:DeleteMe()
					elseif page.RemoveSelf then
						page:RemoveSelf(true)
					end
					self.__page_map[index] = nil
					self.__dirty_page_map[index] = nil
				end

				if CPPGameEngine:GetNowTime() - beg_time > limit_time then
					--超时处理
					return
				end
			end
		end
	end
end

--清空列表项
function GUI.PageView:__ClearPages(cleanup)
	self.__dirty_page_map = nil
	if self.__page_map then
		for _, page in pairs(self.__page_map) do
			if page.DeleteMe then
				page:DeleteMe()
			elseif page.RemoveSelf then
				page:RemoveSelf(true)
			end
		end

		if cleanup then
			self.__page_map = nil
		else
			self.__page_map = {}
		end
	end

	if self.__pageview then
		self.__pageview:RemoveAllPages()
	end
end

--检测内容页可视化
function GUI.PageView:__CheckPageVisible(view_index, min_x, max_x)
	local page_min_x = (view_index - 1) * self.__view_width
	local page_max_x = page_min_x + self.__view_width
	return (page_min_x >= min_x and page_min_x <= max_x) or (page_max_x >= min_x and page_max_x <= max_x)
end

--翻页回调
function GUI.PageView:__TurningPageCallBack()
	self:__ShowPages()

	--移动回调
	if self.__move_callback then
		self.__move_callback()
	end
end	