
require "modules/modulesdefine"

--[[
%% @module: 模块管理器
%% @author: swordman sue
%% @created: 2016-07-10
%% @description: 创建和管理模块，调用模块函数
--]]

Modules.ModulesMgr = Modules.ModulesMgr or BaseClass(EventAndTimerWrapper)

local get_module

CallModuleFunc = function(module_type, func_name, ...)
	if GlobalModulesMgr then
		return GlobalModulesMgr:Call(module_type, func_name, ...)
	end
end

CallSystemFunc = function(system_id, func_name, ...)
	if GlobalModulesMgr then
		return GlobalModulesMgr:CallSystemFunc(system_id, func_name, ...)
	end
end

function Modules.ModulesMgr.CreateInstance()
	if not GlobalModulesMgr then
		GlobalModulesMgr = Modules.ModulesMgr.New()
	end
end

function Modules.ModulesMgr.DelInstance()
	if GlobalModulesMgr then
		GlobalModulesMgr:DeleteMe()
		GlobalModulesMgr = nil
	end
end

function Modules.ModulesMgr:__init()
	self.__modules_map = {}
	self.__common_modules_map = {}
	self:RegistAllProtocols()

	GlobalRunner:AddRunObj(self, Runner.Priority.RP_MODULE_MGR)	
end

function Modules.ModulesMgr:__delete()
	self:UnRegistAllProtocols()

	self:Dump(true)

    GlobalRunner:RemoveRunObj(self)	
end

function Modules.ModulesMgr:Dump(cleanup)
	self.__in_dumping = true

	--销毁系统模块
	for _, module_obj in pairs(self.__modules_map) do
		module_obj:DeleteMe()
	end
	self.__modules_map = cleanup ~= true and {} or nil

	--销毁公共模块
	local del_list = {}
	for module_type, module_obj in pairs(self.__common_modules_map) do
		table.insert(del_list, {obj = module_obj, order = Modules.ComModulesRegistry[module_type].order})
	end
	self.__common_modules_map = cleanup ~= true and {} or nil	
	table.sort(del_list, SortTools.KeyUpperSorter("order"))
	for _, del_info in ipairs(del_list) do
		del_info.obj:DeleteMe()
	end

	self.__in_dumping = false
	GlobalEventSystem:PauseFireEvent(false)
end

--调用模块函数
function Modules.ModulesMgr:Call(module_type, func_name, ...)
	if self.__in_dumping then
		return
	end

	if not module_type then
		return
	end

	local module_obj = get_module(self, module_type)
	if not module_obj then
		LogTool.LogError("[Modules.ModulesMgr:Call] 找不到对应的模块(module_type:%s)", module_type and module_type or "nil")
		return
	end
	if not module_obj[func_name] then
		LogTool.LogError("[Modules.ModulesMgr:Call] 找不到对应的模块函数(module_type:%s, func_name:%s)", module_type, func_name and func_name or "nil")
		return
	end
	return module_obj[func_name](module_obj, ...)	
end

--调用系统函数
function Modules.ModulesMgr:CallSystemFunc(system_id, func_name, ...)
	local config_system_entry = CPPGameLib.GetConfig("SystemEntry", system_id)
	if not config_system_entry then
		return
	end

	return self:Call(config_system_entry.module_type, func_name, ...)
end

function Modules.ModulesMgr:Update(now_time, elapse_time)
	for _, module_obj in pairs(self.__common_modules_map) do
		if module_obj:CanUpdate(elapse_time) then
			module_obj:Update(now_time, elapse_time)
		end
	end

	for _, module_obj in pairs(self.__modules_map) do
		if module_obj:CanUpdate(elapse_time) then
			module_obj:Update(now_time, elapse_time)
		end
	end
end

--检测系统模块是否开启(system_id请参考配置x系统开启.xml)
function Modules.ModulesMgr:IsOpen(system_id, show_tips)
	local config_system_entry = CPPGameLib.GetConfig("SystemEntry", system_id, false)
	if not config_system_entry then
		return false
	end

	local open_sys_map = CPPSdkTools.GetOpenSysMapInExamine()
	if open_sys_map and open_sys_map[system_id] then
		--审核中，若配置了系统开启列表，则据此判断是否开启
		return true
	end

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	if config_system_entry.level and config_system_entry.level > role_level then
		if show_tips then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", config_system_entry.level))
		end
		return false, config_system_entry.prepare_level <= role_level, config_system_entry.level
	end
	
	return true
end

--系统模块跳转
function Modules.ModulesMgr:OpenSystemModule(system_id, extra_view_params, check_open)
	if check_open ~= false and not self:IsOpen(system_id, true) then
		return
	end

	local config_system_entry = CPPGameLib.GetConfig("SystemEntry", system_id)
	if not config_system_entry then
		return
	end

	--配置中的界面参数
	local view_params = {}
	if config_system_entry.view_params then
		view_params = CPPGameLib.CopyTblShallow(config_system_entry.view_params)
	end

	--额外的界面参数
	if extra_view_params then
		view_params = view_params or {}
		local param_index = #view_params
		for i = 1, table.maxn(extra_view_params) do
			param_index = param_index + 1
			view_params[param_index] = extra_view_params[i]
		end
	end

	local module_type = ModuleType[config_system_entry.module_type]
	local view_type = BaseViewType[config_system_entry.view_type or config_system_entry.module_type]

	--检测是否重复进入同一个界面
	local cur_view_info = GlobalViewMgr:GetCurViewInfo()
	if cur_view_info and cur_view_info.module_type == module_type and cur_view_info.view_type == view_type then
		--对比界面参数
		local is_params_same = true
		if #cur_view_info.view_params ~= #view_params then
			is_params_same = false
		else
			for i = 1, #cur_view_info.view_params do
				if cur_view_info.view_params[i] ~= view_params[i] then
					is_params_same = false
				end
			end
		end

		if is_params_same then
			--界面参数一致，提示
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("on_cur_system", config_system_entry.name))
			return
		else
			--界面参数不一致，先关闭，后打开
			local cur_view = GlobalViewMgr:GetViewByType(cur_view_info.view_type)
			if cur_view then
				cur_view:CloseManual(true, true)
			end
		end
	end

	CallModuleFunc(module_type, "OpenView", view_type, unpack(view_params))
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_module = function(obj_self, module_type)
	if not module_type then
		LogTool.LogError("[get_module] 参数module_type无效")
		return
	end

	if obj_self.__modules_map[module_type] then
		return obj_self.__modules_map[module_type]
	end

	if obj_self.__common_modules_map[module_type] then
		return obj_self.__common_modules_map[module_type]
	end	

	local module_registry_info = Modules.ComModulesRegistry[module_type] or Modules.ModulesRegistry[module_type]
	if not module_registry_info then
		LogTool.LogError("[get_module] 找不到模块注册信息(module_type:%s)", module_type)
		return
	end

	RequireToolBatch:LoadFilesSync({file_list = module_registry_info.file_list})

	local module_class = Modules[module_registry_info.class_name]
	if not module_class then
		LogTool.LogError("[get_module] 找不到模块类(module_type:%s, class_name:%s)", module_type, module_registry_info.class_name)
		return
	end

	local module_obj = module_class.New(module_type)
	if Modules.ComModulesRegistry[module_type] then
		--公共模块
		obj_self.__common_modules_map[module_type] = module_obj
	else
		--系统模块
		obj_self.__modules_map[module_type] = module_obj
	end
	
	return module_obj
end

