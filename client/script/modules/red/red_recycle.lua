
--[[
%% @module: 回收红点
%% @author: yjg
%% @created: 2018年12月24日20:05:43
--]]
Modules.Red.Recycle = Modules.Red.Recycle or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Recycle:__init( ... )
    self.red = nil
	self.red_map = {}
end

--红点统计逻辑
function Modules.Red.Recycle:__Statistics( sub_system_id )
    --可回收装备
    if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.EQUIP then
        self.red_map[Macros.Game.SystemID.EQUIP] = self:EquiupRED() 
    end

    --可回收英雄
    if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.HERO_BAG then
        self.red_map[Macros.Game.SystemID.HERO_BAG] = self:HeroRED() 
    end
end

--处理装备红点数量
function Modules.Red.Recycle:EquiupRED( ... )
    local num = 0

    local type_equip_list
    for i , v in pairs( Macros.Game.EquipType ) do
        type_equip_list = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip", v , true )
        if type_equip_list then
            num = num + math.floor( #type_equip_list / ConfigAdapter.Common.GetRecycleEquipRed() )
            if num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
                break
            end
        end
    end
    
    return num
end

--处理英雄红点数量
-- 2170 英雄分解:背包有3个绿品及绿品以上英雄提示可分解红点
function Modules.Red.Recycle:HeroRED( ... )
    local num = 0

    local hero_info_map = CallModuleFunc(ModuleType.HERO, "GetHeroInfoMap")
    if not hero_info_map then
        return num
    end

    local item_config
    for k , v in pairs( hero_info_map ) do
        item_config = CPPGameLib.GetConfig("Hero", v.info_id , false)

        if v.is_to_battle == Macros.Global.FALSE and item_config and 
            item_config.type ~= Macros.Game.HeroType.EXP and item_config.init_quality >= Macros.Game.QualityType.GREEN then

            num = num + 1
            if num >= ConfigAdapter.Common.GetRecycleHeroRed() and not Modules.Red.BaseRed.IS_SHOW_NUM then
                break
            end
        end
    end

    return math.floor( num / ConfigAdapter.Common.GetRecycleHeroRed() )
end
