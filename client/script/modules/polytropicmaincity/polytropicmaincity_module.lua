
--[[
%% @module: 多变的主城
%% @author: ludexian
%% @created: 2018-08-09
%% @description: 负责主城审核相关
--]]

Modules = Modules or {}

Modules.PolytropicMaincityModule = Modules.PolytropicMaincityModule or BaseClass(BaseModule)

--多变类型
Modules.PolytropicMaincityModule.PolytropicType = 
{
    ACTIVITY_VIEW_TYPE = 1,     --活动界面类型
    CHALLENGE_VIEW_TYPE = 2,    --挑战界面类型             
    ARENA_VIEW_TYPE = 3,        --竞技场界面类型
    TOP_RIGHT = 4,
    DUNGEON_VIEW_TYPE = 5,      --主线副本界面类型
    HANDBOOK_VIEW_TYPE = 6,     --图鉴界面类型
    SHOP_VIEW_TYPE = 7,         --商店界面类型
    VIP_VIEW_TYPE = 8,          --Vip界面类型
    WORLDHEART_VIEW_TYPE = 9,   --世界之心界面类型
    SPIRIT_VIEW_TYPE = 10,      --精灵界面类型
    RECOMMEND_VIEW_TYPE = 11,   --推荐阵容界面类型
}
 
function Modules.PolytropicMaincityModule:__init()

end

function Modules.PolytropicMaincityModule:GetModuleEntrysList()
    local ret_entry_list = {}

    local entry_list = self:__GetModuleEntrysList()
    for _, entry_info in ipairs(entry_list) do
        --系统开启检测
        local config_systementry = CPPGameLib.GetConfig("SystemEntry",entry_info.system_id,false)
        if config_systementry and not config_systementry.is_hide_in_examine then            
            table.insert(ret_entry_list, entry_info)
        end
    end
    return ret_entry_list
end

function Modules.PolytropicMaincityModule:__GetModuleEntrysList()
    if not self.__module_entrys_map then
        self.__module_entrys_map = {}
        for id, entry_info in ipairs(CPPGameLib.GetConfig("ExamineCityIcon",nil,nil,true)) do
            entry_info.id = id
            table.insert(self.__module_entrys_map, entry_info)
        end
    end

    return self.__module_entrys_map
end

function Modules.PolytropicMaincityModule:GetPolytropicMaincityBgName()
    return self.__polytropic_maincity_bgname
end

function Modules.PolytropicMaincityModule:SetPolytropicMaincityBgName(res_name)
    self.__polytropic_maincity_bgname = res_name
end

function Modules.PolytropicMaincityModule:__delete()
    
end