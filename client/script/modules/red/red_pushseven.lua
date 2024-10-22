
--[[
%% @module: 七天登录
%% @author: yjg
%% @created: 2018年12月21日17:45:00
--]]
Modules.Red.PushSeven = Modules.Red.PushSeven or BaseClass( Modules.Red.BaseRed )

function Modules.Red.PushSeven:__init( ... )
end

--红点统计逻辑
function Modules.Red.PushSeven:__Statistics( sub_system_id )
	local allRed = CallModuleFunc(ModuleType.PUSH_SEVEN, "GetMayDraw")
	self.red = #allRed
end