
--[[
%% @module: 日志工具
%% @author: swordman sue
%% @created: 2016-07-07
--]]
LogTool = LogTool or {}

print_sys = print
print = function (print_tag, ...)
	if not StartConfig.IsTest then
		return
	end

	if not StartConfig.PrintTag or print_tag ~= StartConfig.PrintTag then
		return
	end

	local arg_count = select('#', ...)
	if arg_count <= 0 then
		return
	end

	local str = ""
	local arg
	for i = 1, arg_count do
		arg = select(i, ...)
		str = str .. tostring(arg) .. " "
	end
    CPPGameLib.LuaLog(str)
end

local handle_params = function(...)
	if select("#", ...) == 1 then
		return ...
	else
		return string.format(...)
	end
end

function LogTool.LogInfo(...)
	if not StartConfig.IsTest then
		return
	end

	local content = handle_params(...)
    CPPGameLib.LuaLogInfo(content)
end

function LogTool.LogError(...)
	if not StartConfig.IsTest then
		return
	end

	local content = handle_params(...)
	-- error(content)
    CPPGameLib.LuaLogError(content .. "\n" .. debug.traceback())
end

function LogTool:Update()
	self:CommitLuaError()
end

--[[
缓存报错日志
--]]
function LogTool:CacheLuaError(key, err_str)
	if not key or not err_str or err_str == "" then
		return
	end

	self.lua_error_str = err_str

	self.lua_error_record = self.lua_error_record or {}
	if self.lua_error_record[key] then
		return
	end
	self.lua_error_record[key] = true

	print(8, "CacheLuaError\n", err_str)

	self.lua_error_tbl = self.lua_error_tbl or {}
	table.insert(self.lua_error_tbl, err_str)

	self.need_commit_lua_error = true	
end

--[[
上传报错日志
--]]
function LogTool:CommitLuaError()
	if not self.need_commit_lua_error then
		return false
	end	
	self.need_commit_lua_error = false

	self.lua_error_str = nil
	local lua_error_tbl = self.lua_error_tbl
	self.lua_error_tbl = nil

	--[[
	TODO：添加上传报错日志的协议
	local real_err_str
	local plat_name = UserVo.Instance:GetPlatName()
	local mainrole_vo = GameVoManager.Instance:GetMainRoleVo()
	local user_id = mainrole_vo.user_id and mainrole_vo.user_id:AsKey() or "null"
	local os = SystemTools:GetOSVersion()
    local protocol = ProtocolRulesMgr.Instance:GetPrototype(10522)
    for _, err_str in ipairs(lua_error_tbl) do
    	real_err_str = string.format("[plat_name:%s]-[user_id:%s]-[os:%s]\n %s", plat_name, user_id, os, err_str)
	    protocol.error_str = real_err_str	    
   	 	BaseController.SendProtocol(nil, GameNet.Instance:GetLoginNetId(), protocol)
   	end
   	--]]

   	return true
end

function LogTool:GetLastLuaError()
	return self.lua_error_str
end

function LogTool:ClearLastLuaError()
	self.lua_error_str = nil
end

