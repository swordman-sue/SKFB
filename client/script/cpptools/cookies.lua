
--[[
%% @module: 本地缓存
%% @author: swordman sue
%% @created: 2016-12-27
--]]

Cookies = Cookies or {}

Cookies.LevelType = 
{
	--公共相关
	COMMON = "COMMON",
	--角色相关
	ROLE = "ROLE",
}

local function get_key(level_type, key)
	level_type = level_type or Cookies.LevelType.COMMON

	if level_type == Cookies.LevelType.COMMON then
		key = string.format("%s_%s", level_type, key)
	else
		local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
		key = string.format("%s_%d_%s", level_type, role_id, key)
	end

	return key
end

function Cookies.Set(level_type, key, var)	
	key = get_key(level_type, key)

	CPPResManager:SetSetting(key, tostring(var))
end

function Cookies.Get(level_type, key)
	key = get_key(level_type, key)

	return CPPResManager:GetSetting(key)
end
