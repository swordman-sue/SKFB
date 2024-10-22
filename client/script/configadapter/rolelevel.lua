
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.RoleLevel = ConfigAdapter.RoleLevel or {}

--获取当前等级经验上限
function ConfigAdapter.RoleLevel.GetMaxExp(level)
	local config = CPPGameLib.GetConfig("RoleLevel", level)
	return config and config.exp or 0
end

--获取当前等级精力上限
function ConfigAdapter.RoleLevel.GetMaxEnergy(level)
	local config = CPPGameLib.GetConfig("RoleLevel", level)
	return config and config.energy_max or 0
end

--获取当前等级体力上限
function ConfigAdapter.RoleLevel.GetMaxStamina(level)
	local config = CPPGameLib.GetConfig("RoleLevel", level)
	return config and config.stamina_max or 0
end

--获取当前等级通缉令上限
function ConfigAdapter.RoleLevel.GetMaxWanted(level)
	local config = CPPGameLib.GetConfig("RoleLevel", level)
	return config and config.wanted_max or 0
end

