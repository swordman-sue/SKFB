
--[[
%% @module: 英雄背包红点
--]]
Modules.Red.HeroBag = Modules.Red.HeroBag or BaseClass( Modules.Red.BaseRed )

function Modules.Red.HeroBag:__init( ... )
end

--红点统计逻辑
function Modules.Red.HeroBag:__Statistics( sub_system_id )
    self.red = 0

	local lst = CallModuleFunc(ModuleType.Pack, "GetAllHeroDebris")
    if not lst then
        return 
    end
    for i,v in ipairs(lst) do
        --持有的          需要的
        if v.item_num >= v.target_num then
            self.red = self.red + 1
            if not Modules.Red.BaseRed.IS_SHOW_NUM then
            	break
            end
        end
    end
    return self.red
end