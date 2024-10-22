
--[[
%% @module: 公会红点
%% @author: yjg
%% @created: 2018年12月24日20:20:16
--]]
Modules.Red.Union = Modules.Red.Union or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Union:__init( ... )
end

--红点统计逻辑
function Modules.Red.Union:__Statistics( sub_system_id )
    self.red = 0
    local unionData = CallModuleFunc(ModuleType.UNION , "UnionData")
    if not unionData or not unionData.red_point_list then
        return 
    end

    local lst = CallModuleFunc(ModuleType.UNION "GetUnionRedLst" )
    --商店的第二个红点 如果有限时
    if lst[Macros.Game.UNION_RED.DESENO_SHOP] then
        self.red = 1
        return
    end

    if lst[Macros.Game.UNION_RED.AWARD] then
        self.red = 1
        return
    end
end
