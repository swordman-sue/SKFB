
--[[
%% @module: 领地红点
--]]
Modules.Red.Manor = Modules.Red.Manor or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Manor:__init( ... )
end

--红点统计逻辑
function Modules.Red.Manor:__Statistics( sub_system_id )
	self.red = 0

	for id,v in ipairs(CPPGameLib.GetConfig("LandCity", nil, nil , true )) do
		local lst = CallModuleFunc(ModuleType.MANOR, "GetCityType", id)
		if lst and lst.patrol.type_ == "NotPatrol" then
			self.red = self.red + 1
			if self.red > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
	end
end
