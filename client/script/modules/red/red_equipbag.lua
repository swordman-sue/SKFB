
--[[
%% @module: 装备红点
%% @author: yjg
%% @created: 2018年12月21日10:12:41
--]]
Modules.Red.EquipBAG = Modules.Red.EquipBAG or BaseClass( Modules.Red.BaseRed )

function Modules.Red.EquipBAG:__init( ... )
	self.red = nil
end

function Modules.Red.EquipBAG:DeleteMe( ... )

end

--红点统计逻辑
function Modules.Red.EquipBAG:__Statistics( sub_system_id )
	local lst = CallModuleFunc(ModuleType.Pack, "GetAllEquipDebris")
    self.red = 0
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