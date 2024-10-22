
--[[
%% @module: UI模板基类
%% @author: swordman sue
%% @created: 2016-07-08
--]]

GUI = GUI or {}

GUI.Template = GUI.Template or BaseClass(EventAndTimerWrapper)

--布局方式
GUI.Template.LayoutMode = GUI.Template.LayoutMode or
{
	CODE = 1,			--代码布局(根节点必须是UILayoutEx)
	FROM_CSB = 2,		--从CSD文件读取
	FROM_WIDGET = 3,	--从WIDGET克隆
}

local preload_templates
local preload_texture_atlas
local load_layout

function GUI.Template:__init()
	self.__render_queue = Scene.RenderQueue.GRQ_UI
	self.__layout_mode = GUI.Template.LayoutMode.FROM_CSB

	--{type = BaseView.BGType.SOLID, color = {0, 0, 0, 204}}
	self.__bg_params = nil

	--选项卡参数
	self.__tab_params = nil	

	--选项卡参数
	self.__tab_params_ex = nil	

	--是否自动加载
	self.__is_auto_load = false

    --预加载的模板列表
    self.__preload_template_list = {}

    --预加载的贴图集
    self.__preload_texture_atlas_list = {}	

	--场景切换，是否自动关闭
    self.is_auto_close = true
end

function GUI.Template:__delete(del_from_scene)
	self.__is_loading = false

	--销毁托管资源
	AutoReleaseTool.DelAllObjs(self)

	--销毁特效
	if self.__effect_id_map then
		for effect_id, _ in pairs(self.__effect_id_map) do
			GlobalEffectMgr:DelEffect(effect_id)
		end
		self.__effect_id_map = nil
	end

	--销毁选项卡
	if self.__tab then
		self.__tab:DeleteMe()
		self.__tab = nil
	end

	--销毁背景
	if self.__bg then
		self.__bg:DeleteMe()
		self.__bg = nil
	end

	--移除根节点
	if self.__root_widget then
		self.__root_widget:RemoveSelf(true)		
		self.__root_widget = nil
	end

	--卸载模板
	for _, template_info in pairs(self.__preload_template_list) do
		if GUITemplateMgr then
			GUITemplateMgr:UnLoadAndRemoveModuleLayout(template_info.template_name)
		end
	end		

	--卸载贴图集
	for _, tex_atlas_info in ipairs(self.__preload_texture_atlas_list) do
		Resource.LoadTool.UnloadUITextureAtlas(tex_atlas_info)
	end

	if del_from_scene ~= false then
		if GUITemplateMgr then
			GUITemplateMgr:DelFromScene(self)	
		end	
	end

	if GUITemplateMgr then
	    GUITemplateMgr:HandleViewCloseSound(self.__registry_info)	
	    GUITemplateMgr:HandleTemplateOpenEffectEnd(self)
	end
end

function GUI.Template:InitTemplate(registry_info, parent_node, fill_parent, ...)
	if self.__root_widget then
		LogTool.LogError("[GUI.Template:InitTemplate] 根节点已经存在")		
		return
	end

	local arg_list = {...}

	self.__registry_info = registry_info
	self.__template_name = type(registry_info) == "table" and registry_info[1] or nil

	if self.__is_auto_load then
		self.__is_loading = true

		--预加载贴图集、模板，加载布局数据
		preload_texture_atlas(self, function()
			if not self.__is_loading then
				return
			end
			preload_templates(self, function()
				if not self.__is_loading then
					return
				end
				load_layout(self, registry_info, arg_list, function()
					self:__HandleLoadLayoutFinished(parent_node, fill_parent)
				end)
			end)
		end)
	else
		load_layout(self, registry_info, arg_list, function()
			self:__HandleLoadLayoutFinished(parent_node, fill_parent)
		end)		
	end

    GUITemplateMgr:HandleViewOpenSound(self.__registry_info)	
end

function GUI.Template:GetRegistryInfo()
	return self.__registry_info
end

function GUI.Template:GetName()
	return self.__template_name
end

function GUI.Template:IsLayout()
	return self.__root_widget and not self.__is_loading
end

function GUI.Template:IsOpen()
    if not self:IsLayout() then
        return false
    end
    return self:IsVisible()
end

function GUI.Template:AddChild(child, order, tag)
	order = order or 0
	tag = tag or 0

	child = CPPGameLib.ConvertToCCNode(child)
	self.__root_widget:AddChild(child, order, tag)
end

function GUI.Template:SetData(var)
	self.__data = var
end

function GUI.Template:GetData()
	return self.__data
end

function GUI.Template:GetNode()
	return self.__root_widget
end

function GUI.Template:GetWidget(name, show_error)
	return WidgetUnity.GetWidgetByName(self.__root_widget, name, show_error)
end

function GUI.Template:GetLayoutMode()
	return self.__layout_mode
end

function GUI.Template:SetVisible(var)
	if self.__root_widget then
		self.__root_widget:SetVisible(var)
	end
end

function GUI.Template:SetZOrder(var)
	if self.__root_widget then
		self.__root_widget:SetZOrder(var)
	end
end

function GUI.Template:IsVisible()
	if self.__root_widget then
		return self.__root_widget:IsVisible()
	end
end

--设置背景显隐
function GUI.Template:SetBGVisible(var)
	if self.__bg then
		self.__bg:SetVisible(var)
	end
end

function GUI.Template:SetPosition(x, y)
	if self.__root_widget then
		self.__root_widget:SetPosition(x, y)
	end
end

function GUI.Template:SetOpacity(var)
	if self.__root_widget then
		self.__root_widget:SetOpacity(var)
	end
end

function GUI.Template:GetPosition()
	if self.__root_widget then
		return self.__root_widget:GetPosition()
	end
	return nil
end

function GUI.Template:GetPositionX()
	if self.__root_widget then
		return self.__root_widget:GetPositionX()
	end
	return 0
end

function GUI.Template:GetPositionY()
	if self.__root_widget then
		return self.__root_widget:GetPositionY()
	end
	return 0
end

function GUI.Template:SetTouchEnabled(var)
	if self.__root_widget then
		self.__root_widget:SetTouchEnabled(var)
	end
end

function GUI.Template:SetTouchSwallow(var)
	if self.__root_widget then
		self.__root_widget:SetTouchSwallow(var)
	end
end

function GUI.Template:SetTouchFun(down_func, move_func, up_func, cancel_func)
	if self.__node then
		WidgetUnity.SetWidgetListener(self.__root_widget, down_func, move_func, up_func, cancel_func)
	end
end

function GUI.Template:SetAnchorPoint(x, y)
	if self.__root_widget then
		self.__root_widget:SetAnchorPoint(x, y)
	end
end

function GUI.Template:SetContentSize(w, h)
	if self.__root_widget then
		self.__root_widget:SetContentSize(w, h)
	end
end

function GUI.Template:GetContentSize()
	if self.__root_widget then
		return self.__root_widget:GetContentSize()
	end
end

function GUI.Template:SetScaleFactor(factor)
	if self.__root_widget then
		self.__root_widget:SetScaleFactor(factor)
	end
end

function GUI.Template:GetScaleFactor()
	if self.__root_widget then
		self.__root_widget:GetScaleFactor()
	end
end

function GUI.Template:GetParent()
	if self.__root_widget then
		return self.__root_widget:GetParent()
	end
end

function GUI.Template:SetWidth(w)
	if self.__root_widget then
		return self.__root_widget:SetContentWidth(w)
	end
end

function GUI.Template:GetWidth()
	if self.__root_widget then
		return self.__root_widget:GetContentWidth()
	end
end

function GUI.Template:SetHeight(h)
	if self.__root_widget then
		return self.__root_widget:SetContentHeight(h)
	end
end

function GUI.Template:GetHeight()
	if self.__root_widget then
		return self.__root_widget:GetContentHeight()
	end
end

function GUI.Template:GetRenderQueue()
	return self.__render_queue
end

--开启定时器
function GUI.Template:SetUpdateFunc(func, interval)
	if self.__root_widget then
		if self.__root_widget.SetLuaUpdate then
			self.__root_widget:SetLuaUpdate(func and GlobalCallbackMgr:AddCallBackFunc(func) or 0, interval or 0)
		else
			LogTool.LogError("[GUI.Template:SetUpdateFunc] 根节点找不到SetLuaUpdate方法")
		end
	end
end

--界面置顶
function GUI.Template:MoveToFront()
	if self.__root_widget then
		local parent_node = self.__root_widget:GetParent()
		if parent_node then
			self.__root_widget:SetZOrder(parent_node:GetChildMaxZOrder() + 1)
		end
	end
end

function GUI.Template:PlayActionTimeline(anim_name, loop, end_callback)
	BaseView.PlayActionTimeline(self, anim_name, loop, end_callback)
end

--是否在播放CSB动画
function GUI.Template:IsPlayingActionTimeLine(anim_name)
	return BaseView.IsPlayingActionTimeLine(self, anim_name)
end

--跳过CSB动画
function GUI.Template:SkipActionTimeLine()	
	BaseView.SkipActionTimeLine(self)
end

function GUI.Template:GetActionTimeLine()
	return BaseView.GetActionTimeLine(self)
end

function GUI.Template:GetActionTimeLineDuration()
	return BaseView.GetActionTimeLineDuration(self)
end

function GUI.Template:SetLoadedCallback(callback)
	self.__loaded_callback = callback
end

--是否强制拉伸模式
function GUI.Template:IsForceStretch()
end

--是否忽略IPhone安全区域
function GUI.Template:IsIgnoreIPhoneSafeArea()
end

--刷新布局(根节点尺寸变化后，需调用此函数刷新按照百分比坐标、尺寸布局的子节点)
function GUI.Template:RefreshLayoutComponent()
	if self.__root_widget then
		self.__root_widget:RefreshLayoutComponent()
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--绑定事件(通过BindGlobalEvent绑定的事件，在模板销毁后会自动解绑。否则需要手动解绑)
function GUI.Template:__RegistAllEvents()
end

function GUI.Template:__CreateWidgetFromLuaCode(...)
	--override me	
end

function GUI.Template:__AddPreloadTextureAtlas(tex_atlas_key, language_related)
	local tex_atlas_info = Resource.UITextureAtlas.GetTexAtlasInfo(tex_atlas_key, language_related)
	if tex_atlas_info then
		table.insert(self.__preload_texture_atlas_list, tex_atlas_info)
	end
end

function GUI.Template:__AddPreloadTemplate(registry_info)
	if not registry_info then
		return
	end

	local template_info = {}
	template_info.template_name = registry_info[1]
	template_info.layout_path = Resource.PathTool.GetUILayoutPath(registry_info[2])
	template_info.lua_script_path = registry_info[3]
	table.insert(self.__preload_template_list, template_info)
end

--处理界面加载完成
function GUI.Template:__HandleLoadLayoutFinished(parent_node, fill_parent)
	--初始化背景图
	local function init_bg()
		if not self.__bg and self.__bg_params then
			self.__bg = CommonBG.New(self.__bg_params, self)
			self:AddChild(self.__bg, -1)
			PosTool.Center(self.__bg)

			--若背景图可点击，则设置根节点可穿透
			if self.__bg_params.close_func then
				self.__root_widget:SetTouchSwallow(false)
			end						
		end					
	end

	--初始化选项卡
	local function init_tab()
		if not self.__tab and self.__tab_params then
			self.__tab = GUI.FeignCheckBoxBtn.New(unpack(self.__tab_params))

		elseif not self.__tab and self.__tab_params_ex then
			self.__tab = GUI.FeignCheckBoxOfList.New(self, unpack(self.__tab_params_ex))
		end
	end

	if self.__is_auto_load then
		if not self.__is_loading then
			return
		end
		self.__is_loading = false

		CPPNodeUnity.Retain(self.__root_widget)

		self.__root_widget:RemoveSelf(false)
		self:__AddToParent(parent_node, fill_parent)

		CPPNodeUnity.Release(self.__root_widget)

		--初始化背景图
		init_bg()

		--初始化选项卡
		init_tab()

		--打开回调
		if self.__open_callback then
			self.__open_callback()
		end

		--处理打开效果
		GUITemplateMgr:HandleTemplateOpenEffect(self, function()
			self:FireNextFrame(Macros.Event.View.TEMPLATE_OPEN, self.__template_name)
		end)	
	else
		self:__AddToParent(parent_node, fill_parent)

		--初始化背景图
		init_bg()

		--初始化选项卡
		init_tab()		
	end

    --绑定事件
    self:__RegistAllEvents()

	--加载完成回调
	if self.__loaded_callback then
		self.__loaded_callback()
	end
end

function GUI.Template:__AddToParent(parent_node, fill_parent)
	if parent_node then
		parent_node:AddChild(self.__root_widget)
		if fill_parent then
			self.__root_widget:SetAnchorPoint(0.5, 0.5)
			self.__root_widget:SetSizeType(Widget.SIZETYPE_PERCENT)
			self.__root_widget:SetSizePercent(1.0, 1.0)
			self.__root_widget:SetPositionType(Widget.POSTYPE_PERCENT)
			self.__root_widget:SetPositionPercent(0.5, 0.5)
			self.__root_widget:RefreshLayoutComponent()			
		end		
	else
		GUITemplateMgr:AddToScene(self)
	end
end

--[[
托管自动释放对象
@param obj                          对象(需自动释放的对象，注：1、确保支持GetNode、DeleteMe接口。2、在调用此接口前，确保obj的节点已添加到父容器)
@param obj_key[option]              对象索引
@param priority4autorelease[option] 对象自动销毁的优先级(优先销毁数值小的对象)
@return obj_key                     对象索引
]]
function GUI.Template:__AddAutoReleaseObj(obj, obj_key, priority4autorelease)
	return AutoReleaseTool.AddObj(self, obj, obj_key, priority4autorelease)
end

--[[
获取自动释放对象
@param obj_key      对象索引
]]
function GUI.Template:__GetAutoReleaseObj(obj_key)
	return AutoReleaseTool.GetObj(self, obj_key)
end

--[[
删除自动释放对象
@param obj_key      对象索引
]]
function GUI.Template:__DelAutoReleaseObj(obj_key)
	AutoReleaseTool.DelObj(self, obj_key)
end

--创建特效
function GUI.Template:__CreateEffect(params, parent_node, zorder)
	local effect_id, effect = GlobalEffectMgr:CreateUIEffect(params, parent_node, zorder)	
	if effect_id and effect then
		self.__effect_id_map = self.__effect_id_map or {}
		self.__effect_id_map[effect_id] = true
	end
	return effect_id, effect
end

--销毁特效
function GUI.Template:__DelEffect(effect_id)
	if self.__effect_id_map and self.__effect_id_map[effect_id] then
		self.__effect_id_map[effect_id] = nil
		GlobalEffectMgr:DelEffect(effect_id)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--预加载贴图集
preload_texture_atlas = function(obj_self, finished_callback)
	local texture_atlas_num = #obj_self.__preload_texture_atlas_list
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
load_layout = function(obj_self, registry_info, arg_list, finished_callback)
	local function real_finished_callback()
		obj_self.__root_widget:SetVisible(true)
		finished_callback()
	end

	if obj_self.__layout_mode == GUI.Template.LayoutMode.FROM_CSB then
		if not registry_info then
			LogTool.LogError("[load_layout] 参数registry_info无效")		
			return
		end

		local template_name = registry_info[1]
		obj_self.__layout_name = registry_info[2]
		if not obj_self.__is_auto_load then
			obj_self.__root_widget = GUITemplateMgr:Clone(template_name)
			if not obj_self.__root_widget then
				LogTool.LogError("[load_layout] 模板克隆失败. template_name:%s", template_name)
				return
			end			
		else
			CPPViewHelper.CreateWidgetFromFlatBuffersFileAsync(Resource.PathTool.GetUILayoutPath(obj_self.__layout_name), GlobalCallbackMgr:AddCallBackFunc(function()
				obj_self.__root_widget = CPPViewHelper.GetDFFWidget()
				if not obj_self.__root_widget then
					LogTool.LogError("[load_layout] 布局文件加载失败. layout_name:%s", obj_self.__layout_name)
					return
				end
				real_finished_callback()
			end))
			return
		end

	elseif obj_self.__layout_mode == GUI.Template.LayoutMode.FROM_WIDGET then
		if not registry_info then
			LogTool.LogError("[load_layout] 参数registry_info无效")		
			return
		end

		local src_widget = registry_info
		obj_self.__root_widget = src_widget:Clone()
		if not obj_self.__root_widget then
			LogTool.LogError("[load_layout] widget克隆失败")			
			return
		end

	else
		obj_self.__root_widget = obj_self:__CreateWidgetFromLuaCode(unpack(arg_list))
		if not obj_self.__root_widget then
			LogTool.LogError("[load_layout] 模板创建失败(请检查__CreateWidgetFromLuaCode返回的节点是否有效).")
			return
		end
	end

	real_finished_callback()
end
