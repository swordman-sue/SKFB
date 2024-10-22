
--[[
%% @module: 奖励中心红点
%% @author: ldx
%% @created: 2019年1月04日
--]]
Modules.Red.AcceptPrizeCenter = Modules.Red.AcceptPrizeCenter or BaseClass( Modules.Red.BaseRed )

function Modules.Red.AcceptPrizeCenter:__init( ... )
end

--红点统计逻辑
function Modules.Red.AcceptPrizeCenter:__Statistics( sub_system_id )
    local reward_list = CallModuleFunc(ModuleType.ACCEPT_PRIZE_CENTER , "GetRewardCenterList")
    if not reward_list then
        self.red = 0
        return
    end

    self.red = #reward_list
end