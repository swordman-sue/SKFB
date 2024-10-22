
--[[
%% @module: 女王红点
%% @author: yjg
%% @created: 2018年12月21日17:45:00
--]]
Modules.Red.Queen = Modules.Red.Queen or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Queen:__init( ... )
end

--红点统计逻辑
function Modules.Red.Queen:__Statistics( sub_system_id )
    self.red = CallModuleFunc(ModuleType.QUEEN, "CheckCanReward", true)
end