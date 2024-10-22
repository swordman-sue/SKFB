
require "core/baseviewdefine"

--[[
%% @module: 模块界面管理器
%% @author: swordman sue
%% @created: 2016-07-10
%% @description: 管理模块界面的加载和释放
--]]

BaseViewMgr = BaseViewMgr or BaseClass(EventAndTimerWrapper)

BaseViewMgr.identify_count = BaseViewMgr.identify_count or 0

--私有函数
local check_filter_view

function BaseViewMgr.CreateInstance()
	if not GlobalViewMgr then
		GlobalViewMgr = BaseViewMgr.New()
	end
end

function BaseViewMgr.DelInstance()
	if GlobalViewMgr then
		GlobalViewMgr:DeleteMe()
		GlobalViewMgr = nil
	end
end

function BaseViewMgr:__init()
	self.__view_map = {}
	self.__view_info_stack = {}
	self.__view_stack_enabled = true

	GlobalRunner:AddRunObj(self, Runner.Priority.RP_VIEW_MGR, 4)
end

function BaseViewMgr:__delete()
	for _, view in pairs(self.__view_map) do
		view:DeleteMe()
	end
	self.__view_map = nil

	self.__cur_view_info = nil
	self.__view_info_stack = nil

    GlobalRunner:RemoveRunObj(self)
end

function BaseViewMgr:PrintCache()
	print(8, "---------------BaseViewMgr:PrintCache---------------")
	for view_id, view in pairs(self.__view_map) do
		print(8, view:GetViewType())
	end
end

function BaseViewMgr:RegistAllEvents()
	self:BindGlobalEvent(Macros.Event.View.VIEW_OPEN, function(_, view_id, view_type)
		self:__CheckOnlyCityViewOpen(view_type)
		self:__HandleViewOpenEventForViewStack(view_id, view_type)
	end)
end

function BaseViewMgr:OpenView(view_type, module_type, ...)
	if not view_type then
		LogTool.LogError("[BaseViewMgr:OpenView] 界面类型为空")
		return
	end

	local view_registry_info = BaseViewMgr.ViewsRegistry[view_type]
	if not view_registry_info then
		LogTool.LogError("[BaseViewMgr:OpenView] 找不到界面类型(view_type:%s)对应的注册信息", view_type)
		return
	end

	--加载相应文件
	GlobalRequireToolBatch:LoadFilesSync({file_list = view_registry_info.file_list})

	local view_class = Modules[view_registry_info.class_name]
	if not view_class then
		LogTool.LogError("[BaseViewMgr:OpenView] 找不到对应的界面类(class_name:%s)", view_registry_info.class_name)
		return
	end

	local view_id
	local view = self:GetViewByModuleAndViewType(module_type, view_type)
	if not view then
		--搜索不到已有界面，则创建新界面
		view_id = self:__GenViewID()
		view = view_class.New(...)
		view:SetViewID(view_id)
		view:SetViewType(view_type)
		view:SetBindModuleType(module_type)
	else
		--搜索到已有界面，重新打开(针对关闭模式为BaseView.CloseMode.VISIBLE的界面)
		view_id = view:GetViewID()
		view:SetUnbindModuleType(false)
	end

	--界面栈处理(打开界面)
	self:__HandleOpenViewForViewStack(view_id, view_type, module_type, ...)

	--处理界面打开音效
	self:__HandleViewOpenSound(view_type, ...)
	
	view:Open(...)
	self:__AddView(view_id, view)

	StatisticsTool.GetInstance():SystemOperate(module_type, view_type)
end

--关闭界面
function BaseViewMgr:CloseView(view_id, ignore_view_stack, ignore_view_sound)
	local view = self:GetView(view_id)
	if not view then
		return
	end

	local view_type = view:GetViewType()

	--界面栈处理(关闭界面)
	if self:__HandleCloseViewForViewStack(view_id, view_type, ignore_view_stack) then
		return
	end

	view:Close()
	self:__DelView(view_id, view:GetCloseMode() == BaseView.CloseMode.DESTROY)

	if not ignore_view_stack then
		self:__CheckNoViewOpen(view_type)
	end

	--处理界面关闭音效
	self.__ignore_view_sound = ignore_view_sound
	self:__HandleViewCloseSound(view_type)		
end

--关闭界面
function BaseViewMgr:CloseViews(view_type_map, close_mode, ignore_view_stack, ignore_view_sound)
	--关闭界面
	local close_list
	for view_id, view in pairs(self.__view_map) do
		if view_type_map[view:GetViewType()] then
			close_list = close_list or {}
			table.insert(close_list, view)
		end
	end

	if close_list then
		for _, view in ipairs(close_list) do
			if close_mode then
				view:SetCloseMode(close_mode)
			end
			view:CloseManual(ignore_view_stack, ignore_view_sound)
		end

		if not ignore_view_stack then
			self:__CheckNoViewOpen()
		end
	end	
end

--关闭主界面外的所有界面
function BaseViewMgr:CloseViewsExcludeCity()
	local close_list
	local view_type
	for view_id, view in pairs(self.__view_map) do
		view_type = view:GetViewType()
		if view_type ~= BaseViewType.MAIN_CITY and view.is_auto_close and view_type ~= BaseViewType.POLYTROPIC_MAINCITY then
			close_list = close_list or {}
			table.insert(close_list, view)
		end
	end	

	if close_list then
		for _, view in ipairs(close_list) do
			view:CloseManual(true, true)
		end

		self:__CheckNoViewOpen()		
	end

	--清空界面栈相关
	self:ClearStackInfo()	
end

--获取界面
function BaseViewMgr:GetView(view_id, show_err)
	if not view_id then
		return
	end

	local view = self.__view_map[view_id]
	if not view and false ~= show_err then
		LogTool.LogError("[BaseViewMgr:GetView] 找不到对应的界面对象(view_id:%d)", view_id)
	end
	return view
end

--获取界面(根据界面类型)
function BaseViewMgr:GetViewByType(type)
	for _, view in pairs(self.__view_map) do
		if view:GetViewType() == type then
			return view
		end
	end
	return nil
end

--获取界面(根据布局文件名)
function BaseViewMgr:GetViewByLayoutName(layout_name)
	for _, view in pairs(self.__view_map) do
		if view:GetLayoutName() == layout_name then
			return view
		end
	end
	return nil
end

--获取界面(根据模块类型、界面类型)
function BaseViewMgr:GetViewByModuleAndViewType(module_type, view_type)
	for _, view in pairs(self.__view_map) do
		if view:GetBindModuleType() == module_type and view:GetViewType() == view_type then
			return view
		end
	end
end

--更新
function BaseViewMgr:Update(now_time, elapse_time)
	self.__ignore_view_sound = false

	local del_list
	for view_id, view in pairs(self.__view_map) do				
		if view:CheckAutoDestroy() then
			del_list = del_list or {}
			table.insert(del_list, view_id)
		end
	end

	if del_list then
		local view
		for _, view_id in ipairs(del_list) do
			self:__DelView(view_id, true)
		end
	end
end

--[[
切场景相关销毁
--]]
function BaseViewMgr:Dump(is_clear_stack)
	self.__in_dumping = true

	--关闭界面
	local close_list
	for view_id, view in pairs(self.__view_map) do
		if view.is_auto_close then
			close_list = close_list or {}
			table.insert(close_list, view)
		end
	end
	if close_list then
		for _, view in ipairs(close_list) do
			view:CloseManual(true, true)
		end
	end

	--清空界面栈相关
	if is_clear_stack then
		self:ClearStackInfo()
	end

	self.__in_dumping = false	
end

--是否只有主界面打开
function BaseViewMgr:IsOnlyCityViewOpen(filter_type)
	local flag = false

	local view_type
	for _, view in pairs(self.__view_map) do
		view_type = view:GetViewType()

		if view_type ~= filter_type and not check_filter_view(view_type) then
			if view_type == BaseViewType.MAIN_CITY or view_type == BaseViewType.POLYTROPIC_MAINCITY then
				if view:IsLayoutOrLoading() then
					flag = true
				end
				
			elseif not BaseViewMgr.NonFullScreenViewRegistry[view_type] then
				if view:IsOpenOrLoading() then
					return false
				end
			end
		end
	end

	return flag
end

--界面是否打开
function BaseViewMgr:IsViewOpen(view_id)
	local view = self:GetView(view_id, false)
	return view and view:IsOpen()
end

--界面是否打开
function BaseViewMgr:IsViewOpenByType(view_type)
	local view = self:GetViewByType(view_type)
	return view and view:IsOpen()
end

--获取当前界面信息
function BaseViewMgr:GetCurViewInfo()
	return self.__cur_view_info
end

--获取当前界面类型
function BaseViewMgr:GetCurViewInfoModuleType()
	return self.__cur_view_info and self.__cur_view_info.module_type
end

--获取栈顶界面信息
function BaseViewMgr:GetTopStackInfo()
	if self.__view_info_stack then
		return self.__view_info_stack[#self.__view_info_stack]
	end
end

--清空界面栈相关
function BaseViewMgr:ClearStackInfo()
	self.__cur_view_info = nil
	self.__view_info_stack = {}
end

--是否处于界面栈过滤表
function BaseViewMgr:IsInViewStackFilter(view_type)
	return BaseViewMgr.ViewStackFilterMap[view_type]
end

--设置界面栈是否开启
function BaseViewMgr:SetViewStackEnabled(var)
	self.__view_stack_enabled = var
end

--处理界面打开时的缩放效果
function BaseViewMgr:HandleViewOpenEffect(view, finished_callback)
	if not view then
		return
	end

	self:__LazyInitOpenEffectConfig()

	local function do_finished_callback()
		if finished_callback then
			finished_callback()
		end
	end

	local view_type = view:GetViewType()

	-- if not self:IsOnlyCityViewOpen(view_type) then
	-- 	--若有其他界面打开着，过滤
	-- 	do_finished_callback()
	-- 	return
	-- end

	if not self.__open_effect_config or not self.__open_effect_config[view_type] then
		--不需打开效果的界面，过滤
		do_finished_callback()
		return
	end

	if self.__in_view_open_effecting then
		--打开效果中，过滤
		do_finished_callback()
		return
	end

	ActionManager.GetInstance():ViewOrTemplateOpenEffect(view, function()
		self.__view_in_open_effecting = nil
		self.__in_view_open_effecting = false
		self:FireNextFrame(Macros.Event.View.VIEW_EFFECT_FINISHED)

		--完成回调
		do_finished_callback()
	end)

	self.__view_in_open_effecting = view
	self.__in_view_open_effecting = true	
end

--处理界面打开时的缩放效果(结束)
function BaseViewMgr:HandleViewOpenEffectEnd(view)
	if self.__view_in_open_effecting == view then
		self.__in_view_open_effecting = nil
		self.__in_view_open_effecting = false
	end
end

--是否界面打开效果中
function BaseViewMgr:IsInViewOpenEffect()
	return self.__in_view_open_effecting
end

--获取界面音效信息
function BaseViewMgr:GetViewSoundInfo(view_type)
	self:__LazyInitViewSoundConfig()

	return self.__view_sound_config and self.__view_sound_config[view_type]
end

--获取占用的资源表
function BaseViewMgr:GetKeepRes()
	local keep_res_map
	local tmp_keep_res_map
	for _, view in pairs(self.__view_map) do
		tmp_keep_res_map = view:GetKeepRes()
		if tmp_keep_res_map then
			for texture_path, _ in pairs(tmp_keep_res_map) do
				keep_res_map = keep_res_map or {}
				keep_res_map[texture_path] = true
			end
		end
	end
	return CPPGameLib.ConvertMap2List(keep_res_map, true)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function BaseViewMgr:__GenViewID()
	BaseViewMgr.identify_count = BaseViewMgr.identify_count + 1
	return BaseViewMgr.identify_count
end

function BaseViewMgr:__AddView(view_id, view)
	if not view_id then
		LogTool.LogError("[BaseViewMgr:__AddView] 参数view_id无效")
		return
	end
	if not view then
		LogTool.LogError("[BaseViewMgr:__AddView] 参数view无效")
		return
	end

	local module_type = view:GetBindModuleType()
	if module_type then
		CallModuleFunc(module_type, "SetViewID", view_id)	
	end
	if not self.__view_map[view_id] then
		self.__view_map[view_id] = view
	end
end

function BaseViewMgr:__DelView(view_id, cleanup)
	if not view_id then
		LogTool.LogError("[BaseViewMgr:__DelView] 参数view_id无效")
		return
	end
	if not self.__view_map[view_id] then
		LogTool.LogError("[BaseViewMgr:__AddView] 对应的界面不存在view_id:%d", view_id)
		return
	end	

	local view = self.__view_map[view_id]

	local last_view_type = view:GetViewType()
	local module_type = view:GetBindModuleType()
	if module_type and not view:IsUnbindModuleType() then
		CallModuleFunc(module_type, "SetViewID", nil)
	end

	if cleanup then
		view:DeleteMe()
		self.__view_map[view_id] = nil
	end

	self:__CheckOnlyCityViewOpen(last_view_type)
end

--检测只有主界面打开
function BaseViewMgr:__CheckOnlyCityViewOpen(last_view_type)
	if check_filter_view(last_view_type) then
		return
	end

	if self:IsOnlyCityViewOpen() then
		GlobalEventSystem:Fire(Macros.Event.View.ONLY_CITY_VIEW_OPEN)
	else
		GlobalEventSystem:Fire(Macros.Event.View.NOT_ONLY_CITY_VIEW_OPEN)
	end
end

--检测所有界面都关闭
function BaseViewMgr:__CheckNoViewOpen(last_view_type)
	if check_filter_view(last_view_type) then
		return
	end

	local no_view_open = true
	for _, view in pairs(self.__view_map) do
		if not check_filter_view(view:GetViewType()) and view:IsOpen() then
			no_view_open = false
		end
	end
	if no_view_open then
		GlobalEventSystem:Fire(Macros.Event.View.NO_VIEW_OPEN, last_view_type)
	end
end

--界面栈处理
function BaseViewMgr:__HandleOpenViewForViewStack(view_id, view_type, module_type, ...)
	if not self.__view_stack_enabled then
		return
	end

	if BaseViewMgr.ViewStackFilterMap[view_type] then
		return
	end

	--将上一个界面信息压入栈
	if self.__cur_view_info then
		local cur_view = self:GetView(self.__cur_view_info.view_id)
		local view_params = cur_view and cur_view:GetOpenParams() or nil
		self.__cur_view_info.view_params = view_params or self.__cur_view_info.view_params
		table.insert(self.__view_info_stack, self.__cur_view_info)
	end

	--记录当前界面信息
	self.__cur_view_info = {}
	self.__cur_view_info.view_id = view_id
	self.__cur_view_info.view_type = view_type
	self.__cur_view_info.module_type = module_type
	self.__cur_view_info.view_params = {...}

	--从栈底往上搜索，若有相同类型的界面，则回退到该位置的前一个位置
	local ret_index
	for index, view_info in ipairs(self.__view_info_stack) do
		if view_info.module_type == module_type and view_info.view_type == view_type then
			ret_index = index - 1
		end
	end
	if ret_index then		
		for i = #self.__view_info_stack, 1, -1 do
			if i > ret_index then
				table.remove(self.__view_info_stack, i)
			else
				break
			end
		end
	end
end

--界面栈处理
function BaseViewMgr:__HandleCloseViewForViewStack(view_id, view_type, ignore_view_stack)
	if not self.__view_stack_enabled then
		return
	end

	if BaseViewMgr.ViewStackFilterMap[view_type] then
		return
	end

	if self.__cur_view_info and self.__cur_view_info.view_id == view_id then
		if ignore_view_stack ~= true then
			local view_info = table.remove(self.__view_info_stack)
			if view_info then
				--若当前界面是全屏界面 或 栈顶界面已关闭
				if not BaseViewMgr.NonFullScreenViewRegistry[view_type] or not self:IsViewOpen(view_info.view_id) then					
					--清空当前界面信息
					self.__cur_view_info = nil

					--恢复栈顶界面
					CallModuleFunc(view_info.module_type, "OpenView", view_info.view_type, unpack(view_info.view_params))

					if not BaseViewMgr.NonFullScreenViewRegistry[view_info.view_type] then
						--若栈顶界面是全屏界面，则先恢复栈顶界面，再关闭当前界面
						return true
					end					
					--若栈顶界面是非全屏界面，则立即关闭当前界面
					return

				--若当前界面是非全屏界面
				else
					--恢复栈顶界面信息
					self.__cur_view_info = view_info
					return
				end
			end
		end

		--清空当前界面信息
		self.__cur_view_info = nil		
	end
end

--界面打开事件处理
function BaseViewMgr:__HandleViewOpenEventForViewStack(view_id, view_type)
	if not self.__view_stack_enabled then
		return
	end
	
	if BaseViewMgr.ViewStackFilterMap[view_type] then
		return
	end

	if BaseViewMgr.NonFullScreenViewRegistry[view_type] then
		return
	end

	--当前界面打开后，关闭/隐藏栈底界面
	if self.__cur_view_info and self.__cur_view_info.view_id == view_id then
		local close_list
		for view_id, view in pairs(self.__view_map) do
			if self.__cur_view_info.view_id ~= view_id and not BaseViewMgr.ViewStackFilterMap[view:GetViewType()] then
				close_list = close_list or {}
				table.insert(close_list, view)
			end
		end
		if close_list then
			for _, view in ipairs(close_list) do
				--关闭
				view:CloseManual(true, true)
			end
		end
	end
end

--初始化界面打开效果表
function BaseViewMgr:__LazyInitOpenEffectConfig()
	if not GameConfig.ViewOpenEffect then
		return
	end
	
	if not self.__open_effect_config then
		self.__open_effect_config = {}
		local view_type
		for _, info in pairs(GameConfig.ViewOpenEffect) do
			if info.type == 1 then
				view_type = BaseViewType[info.type_name]
				if view_type then
					self.__open_effect_config[view_type] = true
				end
			end
		end
	end
end

--初始化界面音效表
function BaseViewMgr:__LazyInitViewSoundConfig()
	if not GameConfig.ViewSound then
		return
	end
	
	if not self.__view_sound_config then
		self.__view_sound_config = {}
		local view_type
		for _, info in pairs(GameConfig.ViewSound) do
			if not info.type or info.type == 0 then
				view_type = BaseViewType[info.sub_type]
				if view_type then
					self.__view_sound_config[view_type] = info
				end
			end
		end
	end
end

--处理界面打开音效
function BaseViewMgr:__HandleViewOpenSound(view_type, ...)
	if not view_type then
		return
	end

	local sub_view_type = select(1, ...)
	if sub_view_type and BaseViewMgr.SubViewSoundFilterMap[sub_view_type] then
		return
	end

	self:__LazyInitViewSoundConfig()

	local info = self.__view_sound_config and self.__view_sound_config[view_type] or nil
	if not info then
		return
	end

	if info.open_bgm then
		GlobalSoundMgr:PlayBGM(info.open_bgm, nil, info.volume, true)
		return
	end

	if info.open_sound then
		GlobalSoundMgr:PlayEffectByConfigID(info.open_sound)
	end
end

--处理界面关闭音效
function BaseViewMgr:__HandleViewCloseSound(view_type)
	if not view_type then
		return
	end

	if self.__ignore_view_sound then
		return
	end

	self:__LazyInitViewSoundConfig()

	local info = self.__view_sound_config and self.__view_sound_config[view_type] or nil
	if not info then
		return
	end

	if self.__cur_view_info then
		return
	end

	if info.close_sound then
		GlobalSoundMgr:PlayEffectByConfigID(info.close_sound)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
check_filter_view = function(view_type)
	if CallModuleFunc(ModuleType.LOADING, "IsLoadingViewType", view_type) then
		return true
	end

	if view_type == BaseViewType.RES_UPDATE or
		view_type == BaseViewType.LOGIN or
		view_type == BaseViewType.LOGIN_SERVER_LIST then
		return true
	end
end
