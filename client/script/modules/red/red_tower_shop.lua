
--[[
%% @module: 爬塔商店红点
%% @author: yjg
%% @created: 2018年12月21日18:19:02
--]]
Modules.Red.TowerShop = Modules.Red.TowerShop or BaseClass( Modules.Red.BaseRed )

function Modules.Red.TowerShop:__init( ... )
end

--红点统计逻辑
function Modules.Red.TowerShop:__Statistics( sub_system_id )
    self.red = CallModuleFunc(ModuleType.TOWER , "GetNowBuyNum")
end