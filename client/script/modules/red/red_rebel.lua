
--[[
%% @module: 女王红点
%% @author: yjg
%% @created: 2018年12月21日17:45:00
--]]
Modules.Red.Rebel = Modules.Red.Rebel or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Rebel:__init( ... )
end

--红点统计逻辑
function Modules.Red.Rebel:__Statistics( sub_system_id )
    self.red = 0

    local rebel = CallModuleFunc(ModuleType.REBEL, "GetRebelInfo")
    if not rebel then
        return
    end

    --当前可领取的奖励数量
    self.red = CallModuleFunc(ModuleType.REBEL , "GetNoeAwardRed")

    --获得叛军发现列表
    local find_list = CallModuleFunc(ModuleType.REBEL , "GetRebelFindList")    
    if find_list then
        self.red = self.red + #find_list
    end
end
