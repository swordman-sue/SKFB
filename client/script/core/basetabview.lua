
--[[
%% @module: 选项卡界面基类
%% @author: swordman sue
%% @created: 2016-08-26
--]]

BaseTabView = BaseTabView or BaseClass(EventAndTimerWrapper)

local preload_templates
local preload_texture_atlas

function BaseTabView:__init(parent_view, tab_view_widget)
	self.__type = 0
	self.__parent_view = parent_view
	self.__root_widget = tab_view_widget
	if not self.__parent_view then
		LogTool.LogError("[BaseTabView:__init] 参数parent_view无效")
		return
	end
	if not self.__root_widget then
		LogTool.LogError("[BaseTabView:__init] 参数sub_widget无效")
		return
	end

	self.__is_loading = false

    --预加载的模板列表
    self.__preload_template_list = {}

    --预加载的贴图集
    self.__preload_texture_atlas_list = {}	

	--选项卡参数
	self.__tab_params = nil	

	--选项卡参数
	self.__tab_params_ex = nil	    
end

function BaseTabView:__delete()	
	self.__is_loading = false
	
	if self:IsOpen() then
		self:__CloseCallback()
	end

	--销毁非托管资源
	self:__Dispose()

	--销毁托管资源
	AutoReleaseTool.DelAllObjs(self)	

	--销毁特效
	self:__DelAllEffects()

	--销毁选项卡
	if self.__tab then
		self.__tab:DeleteMe()
		self.__tab = nil
	end
	
	--卸载模板
	if self.__preload_template_list then
		for _, template_info in pairs(self.__preload_template_list) do
			GUITemplateMgr:UnLoadAndRemoveModuleLayout(template_info.template_name)
		end		
	end

	--卸载贴图集
	if self.__preload_texture_atlas_list then
		for _, tex_atlas_info in ipairs(self.__preload_texture_atlas_list) do
			Resource.LoadTool.UnloadUITextureAtlas(tex_atlas_info)
		end	
	end
end

function BaseTabView:Open(...)
	if self.__is_loading then
		return
	end

	if self:IsOpen() then
		return
	end

	self.__open_data = {...}

	local function callback()
		self:SetVisible(true)
		self:__OpenCallback()
		self:FireNextFrame(Macros.Event.View.TAB_VIEW_OPEN, self.__type)
	end

	if not self.__loaded_finished then		
		self.__is_loading = true

		--预加载贴图集、模板
		preload_texture_atlas(self, function()
			if not self.__is_loading then
				return
			end
			preload_templates(self, function()
				if not self.__is_loading then
					return
				end

				if not self.__tab and self.__tab_params then
					self.__tab = GUI.FeignCheckBoxBtn.New(unpack(self.__tab_params))

				elseif not self.__tab and self.__tab_params_ex then
					self.__tab = GUI.FeignCheckBoxOfList.New(self, unpack(self.__tab_params_ex))
				end		

				self.__is_loading = false
				self.__loaded_finished = true
				self:__LoadedCallback()
				callback()
			end)
		end)
	else
		callback()
	end
end

function BaseTabView:Close()
	self.__is_loading = false

	if self:IsOpen() then
		self:SetVisible(false)
		self:__CloseCallback()
	end

	--切换到其他选项卡时，解绑事件
	self:UnRegistAllEvents()

	self:FireNextFrame(Macros.Event.View.TAB_VIEW_CLOSE, self.__type)
end

function BaseTabView:IsOpen()
	if not self:__LazyCheck() then
		return
	end

	return self.__parent_view:IsOpen() and self.__is_visible
end

function BaseTabView:IsOpenOrLoading()
	return self:IsOpen() or self.__is_loading
end

function BaseTabView:SetViewType(var)
	self.__type = var
end

function BaseTabView:GetViewType(var)
	return self.__type
end

function BaseTabView:CallModuleFunc(func_name, ...)
	if not self:__LazyCheck() then
		return
	end

	return self.__parent_view:CallModuleFunc(func_name, ...)
end

function BaseTabView:GetWidget(name, show_error)
	if not self:__LazyCheck() then
		return
	end

	return WidgetUnity.GetWidgetByName(self.__root_widget, name, show_error)
end

function BaseTabView:GetParent()
	return self.__parent_view
end

function BaseTabView:AddChild(child, order, tag)
	if not self:__LazyCheck() then
		return
	end

	order = order or 0
	tag = tag or 0

    child = CPPGameLib.ConvertToCCNode(child)
	self.__root_widget:AddChild(child, order, tag)
end

function BaseTabView:SetVisible(var)
	if not self:__LazyCheck() then
		return
	end

	if self.__root_widget then
		self.__is_visible = var
		self.__root_widget:SetVisible(var)
	end
end

function BaseTabView:IsVisible()
	if not self:__LazyCheck() then
		return
	end

	return self.__root_widget:IsVisible()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function BaseTabView:__LazyCheck()
	if not self.__parent_view or not self.__root_widget then
		return false
	end
	return true
end

--选项卡界面创建完成回调
function BaseTabView:__LoadedCallback()
	--override me
end

--选项卡界面打开回调
function BaseTabView:__OpenCallback()
	--override me
end

--选项卡界面关闭回调
function BaseTabView:__CloseCallback()
	--override me
end

--选项卡界面销毁回调(释放非托管资源)
function BaseTabView:__Dispose()
	--override me
end

function BaseTabView:__AddPreloadTextureAtlas(tex_atlas_key, language_related)
	local tex_atlas_info = Resource.UITextureAtlas.GetTexAtlasInfo(tex_atlas_key, language_related)
	if tex_atlas_info then
		table.insert(self.__preload_texture_atlas_list, tex_atlas_info)
	end
end

function BaseTabView:__AddPreloadTemplate(registry_info)
	local template_info = {}
	template_info.template_name = registry_info[1]
	template_info.layout_path = Resource.PathTool.GetUILayoutPath(registry_info[2])
	template_info.lua_script_path = registry_info[3]
	table.insert(self.__preload_template_list, template_info)
end

--[[
托管自动释放对象
@param obj                          对象(需自动释放的对象，注：1、确保支持GetNode、DeleteMe接口。2、在调用此接口前，确保obj的节点已添加到父容器)
@param obj_key[option]              对象索引
@param priority4autorelease[option] 对象自动销毁的优先级(优先销毁数值小的对象)
@return obj_key                     对象索引
]]
function BaseTabView:__AddAutoReleaseObj(obj, obj_key, priority4autorelease)
	return AutoReleaseTool.AddObj(self, obj, obj_key, priority4autorelease)
end

--[[
获取自动释放对象
@param obj_key      对象索引
]]
function BaseTabView:__GetAutoReleaseObj(obj_key)
	return AutoReleaseTool.GetObj(self, obj_key)
end

--[[
删除自动释放对象
@param obj_key      对象索引
]]
function BaseTabView:__DelAutoReleaseObj(obj_key)
	AutoReleaseTool.DelObj(self, obj_key)
end

--创建特效
function BaseTabView:__CreateEffect(params, parent_node, zorder)
	local effect_id, effect = GlobalEffectMgr:CreateUIEffect(params, parent_node, zorder)	
	self.__effect_id_map = self.__effect_id_map or {}
	self.__effect_id_map[effect_id] = true
	return effect_id, effect
end

--销毁特效
function BaseTabView:__DelEffect(effect_id)
	if self.__effect_id_map and self.__effect_id_map[effect_id] then
		self.__effect_id_map[effect_id] = nil
		GlobalEffectMgr:DelEffect(effect_id)
	end
end

--销毁所有特效
function BaseTabView:__DelAllEffects()
	if self.__effect_id_map then
		for effect_id, _ in pairs(self.__effect_id_map) do
			GlobalEffectMgr:DelEffect(effect_id)
		end
		self.__effect_id_map = nil
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
