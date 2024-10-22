
--[[
%% @module: 在线奖励
%% @author: yjg
%% @created: 2018年12月21日17:28:36
--]]
Modules.Red.Onlineaward = Modules.Red.Onlineaward or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Onlineaward:__init( ... )
end

--红点统计逻辑
function Modules.Red.Onlineaward:__Statistics( )
    self.red = CallModuleFunc(ModuleType.ONLINE_AWARD, "CheckOnlineReward", true)
end