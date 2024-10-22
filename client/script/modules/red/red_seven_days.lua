
--[[
%% @module: 七天活动
--]]
Modules.Red.SevenDays = Modules.Red.SevenDays or BaseClass(Modules.Red.BaseRed)

function Modules.Red.SevenDays:__init()
end

--更新
function Modules.Red.SevenDays:Update(now_time, elapse_time)
	--由于七天活动的红点统计较为复杂且在非主城状态下无用，因此过滤
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") then
		return
	end

	Modules.Red.BaseRed.Update(self, now_time, elapse_time)	
end

--红点统计逻辑
function Modules.Red.SevenDays:__Statistics(sub_system_id)
	CallModuleFunc(ModuleType.SEVENDAYS, "CountRedDotNum")
end
