
--[[
%% @module: 首充红点
--]]
Modules.Red.FirstCharge = Modules.Red.FirstCharge or BaseClass( Modules.Red.BaseRed )

function Modules.Red.FirstCharge:__init( ... )
end

--红点统计逻辑
function Modules.Red.FirstCharge:__Statistics( )
    local firstRecharge = CallModuleFunc(ModuleType.CHARGE , "GetFirstRechargeRed")
    if firstRecharge == 0 then
        self.red = 0
    else
        local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
        if max and max ~= 0 then
            self.red = 0
        else
            self.red = 1
        end
    end
end