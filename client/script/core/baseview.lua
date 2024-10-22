
--[[
%% @module: 界面基类
%% @author: swordman sue
%% @created: 2016-07-06
--]]

BaseView = BaseView or BaseClass(EventAndTimerWrapper)

--布局方式
BaseView.LayoutMode = 
{
	CODE = 1,		--代码布局(根节点必须是UILayoutEx)
	FROM_CSB = 2,	--编辑器布局
}

--关闭模式
BaseView.CloseMode = 
{
	VISIBLE = 1,
	DESTROY = 2,
}

--背景类型
BaseView.BGType = 
{
	SOLID = 1,	--纯色
	IMAGE = 2,	--贴图
}

BaseView.ActionTimelineTag = 8888

--私有函数
local release
local close_visible
local close_destroy
local preload_templates
local preload_texture_atlas
local load_layout

function BaseView:__init()
	self.__type = BaseViewType.INVALID
	self.__layout_name = ""
	self.__layout_mode = BaseView.LayoutMode.FROM_CSB
	self.__close_mode = BaseView.CloseMode.DESTROY
	self.__render_queue = Scene.RenderQueue.GRQ_UI

    self.__is_loading = false
	self.__is_visible = false

	--是否强制拉伸模式(多分辨率适配)
	self.__is_force_stretch = false

	--是否忽略IPhone安全区域
	self.__is_ignore_iphone_safe_area = false

	--背景图参数
	--{type = BaseView.BGType.SOLID}
	self.__bg_params = nil

	--标题栏参数
	--[[
		例如：
		title = CPPGameLib.GetString("hero_lineup_title"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		show_level = true,
		close_func = function()
		end
	--]]
	self.__title_bar_params = nil

	--选项卡参数
	self.__tab_params = nil

	--选项卡参数
	self.__tab_params_ex = nil

	--选项卡界面参数
	--[[
		例如：
		{Modules.HeroUpgradeTabView, "Panel_Upgrade"},
		{Modules.HeroBreakTabView, "Panel_Break"},
		{Modules.HeroTrainTabView "Panel_Train"},
		{Modules.HeroPotentialTabView, "Panel_Potential"},	
	--]]
	self.__tab_view_params = nil	

	--界面加载完成回调(此回调函数中创建的对象，需在__Dispose中删除)
	self.__loaded_callback = nil

	--若关闭模式不是DESTROY，则以下俩回调函数中对象的创建与删除必须一一对应
	--界面打开回调
    self.__open_callback = nil
    --界面关闭回调
    self.__close_callback = nil

   	--界面开启参数
  	self.__open_data = nil

    --预加载的模板列表
    self.__preload_template_list = {}

    --预加载的贴图集
    self.__preload_texture_atlas_list = {}

    --预加载的贴图集(动态)
    self.__preload_texture_atlas_list_dyn = {}

    ---------------------------------------------------------------------------
    ---------------------------------------------------------------------------
	--场景切换，是否自动关闭
	self.is_auto_close = true

	--所属模块类型
	self.__bind_module_type = nil

  	--自动销毁信息(作用于销毁模式为Visible的已关闭界面)
	self.__auto_destroy_info = nil

	--打开loading
	self.__open_loading = true
	self.__loading_view_type = BaseViewType.UI_LOADING_II
end

function BaseView:__delete()
	if self:IsOpen() and self.__close_callback then
		self.__close_callback()
	end
	release(self)
end

function BaseView:Open(...)
	if self.__is_loading then
		return
	end

	self.__open_data = {...}	

	-- 界面已经打开
	if self:IsOpen() then
		-- TODO：目前很多界面的open_callback与close_callback、loaded_callback与dispose不匹配，得各个界面作出对应修改
		self:__CheckOpenCallback(true)
		self:MoveToFront()
		return
	end

	-- 界面已经加载完
	if self:IsLayout() then
		self.__is_visible = true
		self:SetVisible(self.__is_visible)

		-- TODO：目前很多界面的open_callback与close_callback、loaded_callback与dispose不匹配，得各个界面作出对应修改
		self:__CheckOpenCallback(true)
		self:MoveToFront()
		return true
	end

	if self.__open_loading then
		self.__is_loading = true

		--打开loading界面
		CallModuleFunc(ModuleType.LOADING, "OpenView", self.__loading_view_type, function()
			if not self.__is_loading then
				return
			end
			self:__OpenReal()
		end)
	else
		self:__OpenReal()
	end

	return true
end

--必须由BaseViewMgr:CloseView调用
function BaseView:Close()
	if self:IsOpen() and self.__close_callback then
		self.__close_callback()
	end

	if self.__close_mode == BaseView.CloseMode.VISIBLE then
		if self.__auto_destroy_info then
			self.__auto_destroy_info.last_time = Macros.Global.NowTime
		end
		close_visible(self)

	elseif self.__close_mode == BaseView.CloseMode.DESTROY then
		close_destroy(self)
	end

	self:FireNextFrame(Macros.Event.View.VIEW_CLOSE, self.__view_id, self.__type)	

	GlobalViewMgr:HandleViewOpenEffectEnd(self)
end

--关闭界面并返回上一个界面
function BaseView:CloseManual(ignore_view_stack, ignore_view_sound)
	if self.__bind_module_type and self.__view_id == self:CallModuleFunc("GetViewID") then
		self:CallModuleFunc("CloseView", self.__view_id, ignore_view_stack, ignore_view_sound)
	else
		GlobalViewMgr:CloseView(self.__view_id, ignore_view_stack, ignore_view_sound)
	end
end

--关闭界面并返回主界面
function BaseView:CloseReturnCity()
	GlobalViewMgr:ClearStackInfo()

	self:CloseManual()
end

function BaseView:IsLayout()
	return self.__root_widget and not self.__is_loading
end

function BaseView:IsOpen()
    if not self:IsLayout() then
        return false
    end
    return self:IsVisible()
end

function BaseView:IsLayoutOrLoading()
	return self:IsLayout() or self.__is_loading
end

function BaseView:IsOpenOrLoading()
	return self:IsOpen() or self.__is_loading
end

function BaseView:GetLayoutName()
	return self.__layout_name
end

function BaseView:GetLayoutMode()
	return self.__layout_mode
end

function BaseView:SetCloseMode(mode)
	self.__close_mode = mode
end

function BaseView:GetCloseMode()
	return self.__close_mode
end

function BaseView:GetNode()
	return self.__root_widget
end

function BaseView:GetWidget(name, show_error)
	return WidgetUnity.GetWidgetByName(self.__root_widget, name, show_error)
end

function BaseView:AddChild(child, order, tag)
	order = order or 0
	tag = tag or 0

    child = CPPGameLib.ConvertToCCNode(child)
	self.__root_widget:AddChild(child, order, tag)
end

function BaseView:SetVisible(var)
	if self.__root_widget then
		self.__root_widget:SetVisible(var)
	end
end

function BaseView:IsVisible()
	if not self.__root_widget then
		return false
	end
	return self.__root_widget:IsVisible()
end

--设置标题
function BaseView:SetTitle(var)
	if self.__title_bar then
		self.__title_bar:SetTitle(var)
	end
end

--获取标题栏
function BaseView:GetTitleBar()
	return self.__title_bar
end

--设置标题栏显隐
function BaseView:SetTitleBarVisible(var)
	if self.__title_bar then
		self.__title_bar:SetVisible(var)
	end
end

--设置标题栏关闭函数
function BaseView:SetTitleBarCloseFunc(close_func)
	if self.__title_bar then
		self.__title_bar:SetCloseFunc(close_func)
	end
end

--获取标题栏关闭函数
function BaseView:GetTitleBarCloseFunc()
	if self.__title_bar then
		return self.__title_bar:GetCloseFunc()
	end
end

--设置背景显隐
function BaseView:SetBGVisible(var)
	if self.__bg then
		self.__bg:SetVisible(var)
	end
end

--重置背景图
function BaseView:ResetBG(bg_params)
	if not self:IsOpen() then
		return
	end

	if self.__bg then
		self.__bg:DeleteMe()
	end
	self.__bg_params = bg_params
	self.__bg = CommonBG.New(self.__bg_params, self)					
	self:AddChild(self.__bg, Macros.Global.z_order_bottom)
	PosTool.Center(self.__bg)

	--若背景图可点击，则设置根节点可穿透
	if self.__bg_params.close_func then
		self.__root_widget:SetTouchSwallow(false)
	end
end

--获取TabView
function BaseView:GetTabView(tab_index)
	if self.__tab_view_map then
		return self.__tab_view_map[tab_index]
	end
end

--获取当前选中的TabView
function BaseView:GetActivatedTabView()
	if self.__tab_view_map then
		for _, tab_view in pairs(self.__tab_view_map) do
			if tab_view:IsOpen() then
				return tab_view, tab_view:GetViewType()
			end
		end
		return nil, self.__tab_view_now:GetViewType()
	end
end

--获取TAB选项按钮
function BaseView:GetTabBtn(tab_index)
	if self.__tab then
		return self.__tab:GetBtn(tab_index)
	end
end

--开启定时器
function BaseView:SetUpdateFunc(func, interval)
	if self.__root_widget then
		if self.__root_widget.SetLuaUpdate then
			self.__root_widget:SetLuaUpdate(func and GlobalCallbackMgr:AddCallBackFunc(func) or 0, interval or 0)
		else
			LogTool.LogError("[BaseView:SetUpdateFunc] 根节点找不到SetLuaUpdate方法")
		end
	end
end

--设置界面ID
function BaseView:SetViewID(var)
	self.__view_id = var
end

function BaseView:GetViewID()
	return self.__view_id
end

--设置界面类型
function BaseView:SetViewType(var)
	self.__type = var
end

--获取界面类型
function BaseView:GetViewType()
	return self.__type
end

--设置解绑标记，避免BaseViewMgr:__DelView中，错误地清空相应模块的界面ID
function BaseView:SetUnbindModuleType(var)
	self.__is_unbind_module_type = var
end

function BaseView:IsUnbindModuleType()
	return self.__is_unbind_module_type
end

--设置所属模块类型
function BaseView:SetBindModuleType(var)
	self.__bind_module_type = var
end

--获取所属模块类型
function BaseView:GetBindModuleType()
	return self.__bind_module_type
end

--调用所属模块的函数
function BaseView:CallModuleFunc(func_name, ...)
	if not self.__bind_module_type then
		LogTool.LogError("[BaseView:CallModuleFunc] self.__bind_module_type为空")
		return
	end
	return CallModuleFunc(self.__bind_module_type, func_name, ...)
end

--检测自动销毁
function BaseView:CheckAutoDestroy()
	if self.__is_loading or 
		self.__close_mode ~= BaseView.CloseMode.VISIBLE or 
		not self.__auto_destroy_info or not self.__auto_destroy_info.triggered then
		return false
	end

	-- 销毁已关闭界面
	if not self:IsOpen() and self:IsLayout() and 
		Macros.Global.NowTime - self.__auto_destroy_info.last_time > self.__auto_destroy_info.max_time then
		release(self)
		return true
	end

	return false
end

function BaseView:GetAutoDestroyInfo()
	return self.__auto_destroy_info
end

function BaseView:FinishAutoDestroy()
	if self.__close_mode ~= BaseView.CloseMode.VISIBLE or
		not self.__auto_destroy_info or not self.__auto_destroy_info.triggered then
		return
	end

	self.__auto_destroy_info.max_time = 0
end

--播放CSB动画
function BaseView:PlayActionTimeline(anim_name, loop, end_callback)
	if self.__layout_mode ~= BaseView.LayoutMode.FROM_CSB then
		return
	end

	if not self:IsOpen() then
		return
	end

	loop = loop or false

	self.__playing_anim_record = self.__playing_anim_record or {}
	if self.__playing_anim_record[anim_name] then
		return
	end
	self.__playing_anim_record[anim_name] = true

	local action
	local function play_action()
		--播放动画
		action:Play(anim_name, loop)

		--播放结束回调
		action:SetAnimationEndListener(anim_name, GlobalCallbackMgr:AddCallBackFunc(function()
			if self.__playing_anim_record then
				self.__playing_anim_record[anim_name] = nil
			end

			if end_callback then
				end_callback()
			end
		end))	

		if self.__skip_action then
			self.__skip_action = false
			self:SkipActionTimeLine()
		end	
	end

	action = self.__root_widget:GetActionByTag(BaseView.ActionTimelineTag)
	if not action then
		--创建动画
		CPPViewHelper.CreateActionTimelineFromFlatBuffersFileAsync(Resource.PathTool.GetUILayoutPath(self.__layout_name), GlobalCallbackMgr:AddCallBackFunc(function()
			if not self:IsOpen() then
				return
			end
			action = CPPViewHelper.GetDFFAction()			
			self.__root_widget:RunAction(action)
			action:SetTag(BaseView.ActionTimelineTag)
			play_action()
		end))
	else
		play_action()
	end	
end

--是否在播放CSB动画
function BaseView:IsPlayingActionTimeLine(anim_name)
	if self.__playing_anim_record then
		return self.__playing_anim_record[anim_name]
	end
end

--跳过CSB动画
function BaseView:SkipActionTimeLine()	
	local action = self:GetActionTimeLine()
	if action then
		local end_frame = action:GetEndFrame()
		action:GotoFrameAndPlay(end_frame - 1, end_frame, false)
	end
	self.__skip_action = true
end

function BaseView:GetActionTimeLine()
	if self.__layout_mode ~= BaseView.LayoutMode.FROM_CSB then
		return
	end

	if not self:IsOpen() then
		return
	end

	local action = self.__root_widget:GetActionByTag(BaseView.ActionTimelineTag)	
	return action
end

function BaseView:GetActionTimeLineDuration()
	local action = self:GetActionTimeLine()	
	if not action then
		return 0
	end

	local duration = (action:GetEndFrame() - action:GetStartFrame()) / CPPSceneManager:GetGameFrame()
	return duration
end

--获取打开参数
function BaseView:GetOpenParams()
	return nil
end

--获取占用的资源表
function BaseView:GetKeepRes()
	local keep_map
	for _, tex_atlas_info in ipairs(self.__preload_texture_atlas_list) do
		keep_map = keep_map or {}
		keep_map[tex_atlas_info.texture_path] = true
	end
	for _, tex_atlas_info in ipairs(self.__preload_texture_atlas_list_dyn) do
		keep_map = keep_map or {}
		keep_map[tex_atlas_info.texture_path] = true
	end
	return keep_map
end

--是否强制拉伸模式
function BaseView:IsForceStretch()
	return self.__is_force_stretch
end

--是否忽略IPhone安全区域
function BaseView:IsIgnoreIPhoneSafeArea()
	return self.__is_ignore_iphone_safe_area
end

--界面置顶
function BaseView:MoveToFront()
	if self.__root_widget then
		local parent_node = self.__root_widget:GetParent()
		if parent_node then
			self.__root_widget:SetZOrder(parent_node:GetChildMaxZOrder() + 1)
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--代码布局处理
function BaseView:__CreateWidgetFromLuaCode()
	--override me
end

--绑定事件(通过BindGlobalEvent绑定的事件，在界面关闭后会自动解绑。否则需要手动解绑)
function BaseView:__RegistAllEvents()
end

--界面销毁回调(释放非托管资源)
function BaseView:__Dispose()
	--override me
end

function BaseView:__AddPreloadTextureAtlas(tex_atlas_key, language_related)
	local tex_atlas_info = Resource.UITextureAtlas.GetTexAtlasInfo(tex_atlas_key, language_related)
	if tex_atlas_info then
		table.insert(self.__preload_texture_atlas_list, tex_atlas_info)
	end
end

function BaseView:__AddPreloadTextureAtlasDyn(tex_atlas_key, language_related)
	local tex_atlas_info = Resource.UITextureAtlas.GetTexAtlasInfo(tex_atlas_key, language_related)
	if tex_atlas_info then
		table.insert(self.__preload_texture_atlas_list_dyn, tex_atlas_info)
	end
end

function BaseView:__AddPreloadTemplate(registry_info)
	if not registry_info then
		return
	end

	local template_info = {}
	template_info.template_name = registry_info[1]
	template_info.layout_path = Resource.PathTool.GetUILayoutPath(registry_info[2])
	template_info.lua_script_path = registry_info[3]
	table.insert(self.__preload_template_list, template_info)
end

function BaseView:__ClearPreLoadTemplateList()
	if self.__preload_template_list then
		self.__preload_template_list = {}
	end
end

function BaseView:__ClearPreLoadTextureAtlas()
	if self.__preload_texture_atlas_list then
		self.__preload_texture_atlas_list = {}
	end
end

function BaseView:__OpenReal()
	self.__is_visible = true
	self.__is_loading = true

	local time = CPPGameEngine:GetNowTime()

	--预加载贴图集、模板，加载布局数据
	preload_texture_atlas(self, function()
		-- LogTool.LogInfo("layout_name:%s load_tex cost %f", self.__layout_name, CPPGameEngine:GetNowTime() - time)
		time = CPPGameEngine:GetNowTime()
		if not self.__is_loading then
			return
		end
		preload_templates(self, function()
			-- LogTool.LogInfo("layout_name:%s load templates cost %f", self.__layout_name, CPPGameEngine:GetNowTime() - time)
			time = CPPGameEngine:GetNowTime()
			if not self.__is_loading then
				return
			end
			load_layout(self, function()
				if self.__open_loading then
					--关闭loading界面
					GlobalViewMgr:CloseViews({[self.__loading_view_type] = true}, nil, true, true)
				end

				LogTool.LogInfo("layout_name:%s load layout cost %f", self.__layout_name, CPPGameEngine:GetNowTime() - time)
				time = CPPGameEngine:GetNowTime()
				if not self.__is_loading then
					return
				end
				self.__is_loading = false

				CPPNodeUnity.Retain(self.__root_widget)

				self.__root_widget:RemoveSelf(false)
				self.__root_widget:SetAnchorPoint(0.5, 0.5)

				--根节点多分辨率适配
				GUI.MultiResolutionAdapter.Adapt(self.__root_widget, self.__is_force_stretch, self.__is_ignore_iphone_safe_area)

				--添加到self.__render_queue所属层
				GlobalScene:AddToRenderGroup(self.__root_widget, self.__render_queue)
				PosTool.Center(self.__root_widget)

				CPPNodeUnity.Release(self.__root_widget)

				--置于顶层
				self:MoveToFront()

				self:SetVisible(self.__is_visible)

				--初始化背景图
				if not self.__bg and self.__bg_params then
					self.__bg = CommonBG.New(self.__bg_params, self)					
					self:AddChild(self.__bg, Macros.Global.z_order_bottom)
					PosTool.Center(self.__bg)

					--若背景图可点击，则设置根节点可穿透
					if self.__bg_params.close_func then
						self.__root_widget:SetTouchSwallow(false)
					end
				end

				--初始化标题栏
				if not self.__title_bar and self.__title_bar_params then
					self.__title_bar = LogicGUI.TitleBarLayer.New(self.__title_bar_params, self.__root_widget)
					self:AddChild(self.__title_bar, Macros.Global.z_order_top)
					PosTool.CenterTop(self.__title_bar)
				end

				--初始化选项卡
				if not self.__tab and self.__tab_params then
					self.__tab = GUI.FeignCheckBoxBtn.New(unpack(self.__tab_params))
					self.__tab:Retain()

				elseif not self.__tab and self.__tab_params_ex then
					self.__tab = GUI.FeignCheckBoxOfList.New(self, unpack(self.__tab_params_ex))
					self.__tab:Retain()
				end

				--初始化选项卡界面
				if self.__tab_view_params then
					local tab_view_widget
					for _, tab_view_param in ipairs(self.__tab_view_params) do
						tab_view_widget = self:GetWidget(tab_view_param[2])
						tab_view_widget:SetVisible(false)						
					end
				end

				--绑定事件
				self:__RegistAllEvents()
		
				--加载完成回调
				if self.__loaded_callback then
					self.__loaded_callback()
				end				

				--检测打开回调
				self:__CheckOpenCallback(true)

				-- LogTool.LogInfo("layout_name:%s load layout2 cost %f", self.__layout_name, CPPGameEngine:GetNowTime() - time)				
			end)
		end)
	end)
end

--检测打开回调
function BaseView:__CheckOpenCallback(force)
	if force then
		self.__check_open_callback = true
	end

	if not self.__check_open_callback then
		return
	end
	self.__check_open_callback = false

	if self:IsOpen() and self.__open_callback then
		self.__open_callback()
	end

	--处理打开效果
	GlobalViewMgr:HandleViewOpenEffect(self, function()
		self:FireNextFrame(Macros.Event.View.VIEW_OPEN, self.__view_id, self.__type)
	end)
end

--选项卡切换处理
function BaseView:__TabChange(tab_index)
	if self.__tab_index == tab_index then
		return
	end
	self.__tab_index = tab_index
	self.__tab_view_map = self.__tab_view_map or {}

	local tab_view = self.__tab_view_map[tab_index]
	local tab_view_param = self.__tab_view_params[tab_index]
	if not tab_view_param then
		LogTool.LogError("[BaseView:__TabChange] tab_index无效")
	end

	if not tab_view and tab_view_param then
		local tab_view_class = tab_view_param[1]
		local tab_view_widget = self:GetWidget(tab_view_param[2])
		tab_view = tab_view_class.New(self, tab_view_widget)
		tab_view:SetViewType(tab_index)

		self.__tab_view_map[tab_index] = tab_view
	end

	if self.__tab_view_now then
		self.__tab_view_now:Close()
	end	
	self.__tab_view_now = tab_view
	if self.__tab_view_now then
		self.__tab_view_now:Open(select(2, unpack(self.__open_data)))
	end
end

--[[
托管自动释放对象
@param obj                          对象(需自动释放的对象，注：1、确保支持GetNode、DeleteMe接口。2、在调用此接口前，确保obj的节点已添加到父容器)
@param obj_key[option]              对象索引
@param priority4autorelease[option] 对象自动销毁的优先级(优先销毁数值小的对象)
@return obj_key                     对象索引
]]
function BaseView:__AddAutoReleaseObj(obj, obj_key, priority4autorelease)
	return AutoReleaseTool.AddObj(self, obj, obj_key, priority4autorelease)
end

--[[
获取自动释放对象
@param obj_key      对象索引
]]
function BaseView:__GetAutoReleaseObj(obj_key)
	return AutoReleaseTool.GetObj(self, obj_key)
end

--[[
删除自动释放对象
@param obj_key      对象索引
]]
function BaseView:__DelAutoReleaseObj(obj_key)
	AutoReleaseTool.DelObj(self, obj_key)
end

--创建特效
function BaseView:__CreateEffect(params, parent_node, zorder)
	local effect_id, effect = GlobalEffectMgr:CreateUIEffect(params, parent_node, zorder)	
	self.__effect_id_map = self.__effect_id_map or {}
	self.__effect_id_map[effect_id] = true
	return effect_id, effect
end

--销毁特效
function BaseView:__DelEffect(effect_id)
	if self.__effect_id_map and self.__effect_id_map[effect_id] then
		self.__effect_id_map[effect_id] = nil
		GlobalEffectMgr:DelEffect(effect_id)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
release = function(obj_self)
	obj_self.__is_visible = false

	if obj_self.__is_loading then
		obj_self.__is_loading = false
		if obj_self.__open_loading then
			--关闭loading界面
			GlobalViewMgr:CloseViews({[obj_self.__loading_view_type] = true}, nil, true, true)			
		end
	end

	if obj_self.__root_widget then
		--解绑事件
		obj_self:UnRegistAllEvents()

		--销毁非托管资源
		obj_self:__Dispose()

		--销毁托管资源
		AutoReleaseTool.DelAllObjs(obj_self)

		--销毁特效
		if obj_self.__effect_id_map then
			for effect_id, _ in pairs(obj_self.__effect_id_map) do
				GlobalEffectMgr:DelEffect(effect_id)
			end
			obj_self.__effect_id_map = nil
		end

		--销毁选项卡界面
		if obj_self.__tab_view_map then
			CPPGameLib.ClearObjList(obj_self.__tab_view_map)
			obj_self.__tab_view_map = nil
		end

		--销毁选项卡
		if obj_self.__tab then
			obj_self.__tab:DeleteMe(false)
			if obj_self.__tab_params or obj_self.__tab_params_ex then
				obj_self.__tab:Release()
			end		
			obj_self.__tab = nil
		end

		--销毁标题栏
		if obj_self.__title_bar then
			obj_self.__title_bar:DeleteMe()
			obj_self.__title_bar = nil
		end

		--销毁背景
		if obj_self.__bg then
			obj_self.__bg:DeleteMe()
			obj_self.__bg = nil
		end

		--移除根节点
		obj_self.__root_widget:RemoveSelf(true)
		obj_self.__root_widget = nil

		--卸载模板
		for _, template_info in pairs(obj_self.__preload_template_list) do
			GUITemplateMgr:UnLoadAndRemoveModuleLayout(template_info.template_name)
		end		

		--卸载贴图集
		for _, tex_atlas_info in ipairs(obj_self.__preload_texture_atlas_list) do
			Resource.LoadTool.UnloadUITextureAtlas(tex_atlas_info)
		end
		for _, tex_atlas_info in ipairs(obj_self.__preload_texture_atlas_list_dyn) do
			Resource.LoadTool.UnloadUITextureAtlas(tex_atlas_info)
		end
		obj_self.__preload_texture_atlas_list_dyn = {}
	end
end

close_visible = function(obj_self)
	obj_self.__is_visible = false
	obj_self:SetVisible(obj_self.__is_visible)
end

close_destroy = function(obj_self)
	release(obj_self)
end

--预加载贴图集
preload_texture_atlas = function(obj_self, finished_callback)
	local texture_atlas_num = #obj_self.__preload_texture_atlas_list
	texture_atlas_num = texture_atlas_num + #obj_self.__preload_texture_atlas_list_dyn
	if texture_atlas_num <= 0 then
		finished_callback()
		return
	end

	local function texture_atlas_finished_callback()
		if not obj_self or not obj_self.__is_loading then
			return
		end

		texture_atlas_num = texture_atlas_num - 1
		if texture_atlas_num <= 0 then
			finished_callback()
		end
	end
	for _, tex_atlas_info in ipairs(obj_self.__preload_texture_atlas_list) do
		Resource.LoadTool.LoadUITextureAtlas(tex_atlas_info, texture_atlas_finished_callback)
	end
	for _, tex_atlas_info in ipairs(obj_self.__preload_texture_atlas_list_dyn) do
		Resource.LoadTool.LoadUITextureAtlas(tex_atlas_info, texture_atlas_finished_callback)
	end
end

--预加载模板
preload_templates = function(obj_self, finished_callback)
	local template_size = #obj_self.__preload_template_list
	if template_size <= 0 then
		finished_callback()
		return
	end

	local function load_template_callback(is_suc, template_name, layout_path)
		if not obj_self or not obj_self.__is_loading then
			return
		end

		if is_suc then
			template_size = template_size - 1
			if template_size <= 0 then
				finished_callback()
			end
		else
			LogTool.LogError("[preload_templates] 模板加载失败. template_name:%s layout_path:%s", template_name, layout_path)
		end
	end
	for _, template_info in pairs(obj_self.__preload_template_list) do
		GUITemplateMgr:AddAndLoadModuleLayout(template_info.template_name, template_info.layout_path, template_info.lua_script_path, load_template_callback)
	end
end

--加载布局文件
load_layout = function(obj_self, finished_callback)
	if obj_self.__layout_mode == BaseView.LayoutMode.FROM_CSB then
		CPPViewHelper.CreateWidgetFromFlatBuffersFileAsync(Resource.PathTool.GetUILayoutPath(obj_self.__layout_name), GlobalCallbackMgr:AddCallBackFunc(function()						
			obj_self.__root_widget = CPPViewHelper.GetDFFWidget()
			if not obj_self.__root_widget then
				LogTool.LogError("[load_layout] 布局文件加载失败. layout_name:%s", obj_self.__layout_name)
				return
			end
			finished_callback()
		end))
	else
		obj_self.__root_widget = obj_self:__CreateWidgetFromLuaCode()
		if not obj_self.__root_widget then
			LogTool.LogError("[load_layout] 布局失败(请检查__CreateWidgetFromLuaCode返回的节点是否有效)")			
			return
		end
		finished_callback()
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--[[
%% @module: 通用背景
%% @author: swordman sue
%% @created: 2016-07-08
--]]

CommonBG = CommonBG or BaseClass(UILayer)

--[[
@ params：
	必须：
    type			[number]BaseView.BGType

    可选：
    color			[table]rgba颜色，默认为{0, 0, 0, 204}
    image_path		[string]贴图路径
    image_size		[cocosize]背景图原尺寸
    image_anchor 	[cocopoint]背景图锚点
    image_pos_func 	[function]背景图定位方法
    is_fixed_size 	[bool]是否固定尺寸
    close_func		[function]关闭回调
--]]
function CommonBG:__init(params, parent_view)
	self.parent_view = parent_view
	self.type = params.type or BaseView.BGType.SOLID
	self.image_size = params.image_size or Macros.Global.BGDefaultSize
	self.image_anchor = params.image_anchor or COCOPoint(0.5, 0.5)
	self.image_pos_func = params.image_pos_func or PosTool.Center
	self.is_fixed_size = params.is_fixed_size

	self:SetTouchEnabled(true)
	self:SetAnchorPoint(0.5, 0.5)
	self:SetContentSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	if params.close_func then
		self:SetTouchFun(nil, nil, params.close_func)
	end

	CPPGameLib.Switch(self.type)
	{
		[BaseView.BGType.SOLID] = function()
			self.color = params.color or {0, 0, 0, 204}

			self.solid_node = CPPGraphicLabel.Node()
			self.solid_node:DrawSolidRect(-Macros.Global.DesignSize.width/2, -Macros.Global.DesignSize.height/2, Macros.Global.DesignSize.width, Macros.Global.DesignSize.height, unpack(self.color))
			self:AddChild(self.solid_node)
			PosTool.Center(self.solid_node)
		end,

		[BaseView.BGType.IMAGE] = function()
			self.image_path = params.image_path
			local image_node = Util:Sprite(self.image_path)
			image_node:SetAnchorPoint(self.image_anchor.x, self.image_anchor.y)
			self:AddChild(image_node)

			self.image_pos_func(image_node)

			if not self.is_fixed_size then
				local scale_x = Macros.Global.DesignSize.width / self.image_size.width
				local scale_y = Macros.Global.DesignSize.height / self.image_size.height
				self:SetScaleFactor(math.max(scale_x, scale_y))
			end
		end,
	}	

	--多分辨率适配
	GUI.MultiResolutionAdapter.AdaptForBG(
		self, self.parent_view:GetNode(), self.parent_view:IsForceStretch(), self.parent_view:IsIgnoreIPhoneSafeArea())
end

function CommonBG:AdjustOpacity(var)
	var = var or 204
	self.solid_node:SetOpacity(var)
end

function CommonBG:AdjustOpacityByAction(var, time)
	var = var or 204
	CPPActionManager.FadeTo(self.solid_node, var, time)
end

function CommonBG:IsFixedSize()
	return self.is_fixed_size
end
