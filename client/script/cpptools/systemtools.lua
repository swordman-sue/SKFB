
--[[
%% @module: 系统接口类
%% @author: swordman sue
%% @created: 2016-07-06
%% @description: 进一步封装CPPSystemTools
--]]

CPPSystemTools = CPPSystemTools or {}

CPPSystemTools.system  = CPPSystemTools.GetSystemType()
CPPSystemTools.android = "android"
CPPSystemTools.apple   = "apple"
CPPSystemTools.win32   = "win32"
CPPSystemTools.wp8     = "wp8"

local PlatTypeMap = 
{
	[CPPSystemTools.android] = 1,
	[CPPSystemTools.apple] = 2,
	[CPPSystemTools.win32] = 3,
}

--获取平台类型(android:1 ios:2 pc:3)
function CPPSystemTools.GetPlatType()
	return PlatTypeMap[CPPSystemTools.system] or 0
end

--是否win32
function CPPSystemTools.IsWin32()
	return CPPSystemTools.system == CPPSystemTools.win32
end

--是否android
function CPPSystemTools.IsAndroid()
	return CPPSystemTools.system == CPPSystemTools.android
end

--是否ios
function CPPSystemTools.IsIOS()
	return CPPSystemTools.system == CPPSystemTools.apple
end

--获取IOS版本
function CPPSystemTools.GetIosVersion()
	local ret = "0"
	if (not CPPSystemTools.IsApptorePackage()) then
		return ret
	end
	local version = string.match(CPPSystemTools.GetDeviceModel(), ".*|(.*)")
	local find_start_pos, find_end_pos = string.find(tostring(version), ".", 0)
	if (find_start_pos ~= nil and version ~= nil) then
		ret = string.sub(version, 0, find_start_pos)
	end
	return ret
end

function CPPSystemTools.CloseFile(content, path)
	local file = io.open(path..".temp", "w")
	if (not file) then
		return
	end
	file:write(content)
	file:close()

	---
	local old_file = io.open(path, "r")
	if (old_file) then
		old_file:close()
		local status = os.rename(path, path..".bak")
		LogTool.LogInfo("%s", tostring(status))
		if (not status) then
			return
		end
	end

	local status = os.rename(path..".temp", path)
	LogTool.LogInfo("%s", tostring(status))
	if (not status) then
		os.rename(path..".bak", path)
		return
	end
	os.remove(path..".bak")
end

function CPPSystemTools.OpenFile(path)
	local content = ""
	local file = io.open(path, "r")
	if (not file) then
		return nil
	end

	for line in file:lines() do
		content = content .. line .. "\n"
	end
	file:close(path)
	return content
end

--获取用户货币类型
function CPPSystemTools.GetCurrencyType()
	local currency_type = CPPSystemTools.AutoCurrencyType() or CPPSystemTools.GetDefCurrencyType()

	--非人民币，检测有效性，若无效，则返回默认货币类型
	if currency_type ~= Macros.Game.CashCurrencyType.CNY then
		local config_recharge = CPPGameLib.GetConfig("Recharge", nil, false, true)
		if config_recharge then
			local _, recharge_info = next(config_recharge)
			if recharge_info and not recharge_info["money_"..currency_type] then
				return CPPSystemTools.GetDefCurrencyType()
			end
		end
	end

	return currency_type
end

--获取默认货币类型
function CPPSystemTools.GetDefCurrencyType()
	return StartConfig.DefCurrencyType or Macros.Game.CashCurrencyType.CNY
end

--自动识别货币类型
function CPPSystemTools.AutoCurrencyType()
	if not StartConfig.AutoCurrencyType then
		return
	end

    return CPPSystemTools.GetCurrencyCode()	
end

--获取国家代码
function CPPSystemTools.GetCountryCodeEx()
	if StartConfig.AutoDistrictLanguage ~= "true" then
		return "CN"
	end

	return CPPSystemTools.GetCountryCode()
end

