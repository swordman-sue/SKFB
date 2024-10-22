
--[[
%% @module: 女王红点
%% @author: yjg
%% @created: 2018年12月21日17:45:00
--]]
Modules.Red.Shop = Modules.Red.Shop or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Shop:__init( ... )
end

--红点统计逻辑
function Modules.Red.Shop:__Statistics( sub_system_id )
    local vip_info = CallModuleFunc(ModuleType.SHOP , "GetPurchasableGiftList")
    self.red = #vip_info
end