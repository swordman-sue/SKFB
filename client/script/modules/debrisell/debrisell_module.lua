--
-- @brief 出售模块 
-- @author: yjg
-- @date:2016年11月2日20:24:45
--

Modules = Modules or {}

Modules.DebriSellModule = Modules.DebriSellModule or BaseClass(BaseModule)

function Modules.DebriSellModule:__init()
    self.onData = {}

    self.saveFiltration = {}
    self.filtration = {}
    self.filtration[Macros.Game.SELECT_TYPE.EQUIP] = {
                                    [1] = { type = "sell_quality1",value = 1 },
                                    [2] = { type = "sell_quality2",value = 2 },
                                }
    self.filtration[Macros.Game.SELECT_TYPE.HERO] = {
                                    [1] = { type = "sell_quality0",value = 0 },
                                    [2] = { type = "sell_quality1",value = 1 },
                                    [3] = { type = "sell_quality2",value = 2 },
                                }
    self.filtration[Macros.Game.SELECT_TYPE.EQUIP_DEBRIS] = {
                                    [1] = { type = "sell_quality2",value = 2 },
                                    [2] = { type = "sell_quality3",value = 3 },
                                }
    self.filtration[Macros.Game.SELECT_TYPE.HERO_DEBRIS] = {
                                    [1] = { type = "sell_quality2",value = 2 },
                                    [2] = { type = "sell_quality3",value = 3 },
                                }                               
    self.filtration[Macros.Game.SELECT_TYPE.TREASURE] = {
                                    [1] = { type = "sell_quality2",value = 2 },
                                } 
end

function Modules.DebriSellModule:__delete()
end

--获得装备列表
function Modules.DebriSellModule:GetEquipLst()
    local list = {}
    local t1 = {}
    t1 = CallModuleFunc(ModuleType.EQUIP, "GetEquipLit")
    for k,v in pairs(t1) do
        local info = CPPGameLib.GetConfig("Equipment", v.info_id)
        local data = CPPGameLib.GetConfig("EquipmentStrenthen", info.quality)
        if v.equip_heroid == 0 then
            local price = v.enhance_cost_gold + info.price
            table.insert(list, { 
                    info_id = v.info_id,
                    iconType = data.level_list[v.strengthen_level].cost_type_list[1],
                    price = price,
                    quality = info.quality,
                    name = info.name,
                    strengthen_level = v.strengthen_level,
                    refine_level = v.refine_level,
                    item_num = 0,
                    uid = v.uid,
                    type = Macros.Game.ObjType.EQUIP,
                    selectType = Macros.Game.SELECT_TYPE.EQUIP,
             })
        end
    end

    table.sort( list, function ( a , b )
        if a.quality < b.quality then
            return a.quality < b.quality
        elseif a.quality == b.quality then
            if a.strengthen_level < b.strengthen_level then
                return a.strengthen_level < b.strengthen_level
            elseif a.strengthen_level == b.strengthen_level then
                return a.info_id < b.info_id
            end
        end
    end )
    return list
end

--获得装备碎片列表
function Modules.DebriSellModule:GetEquipDebrisLst()
    local list = {}
    local t1 = CallModuleFunc(ModuleType.Pack, "GetAllEquipDebris")
    for i,v in ipairs(t1) do
        local info = CPPGameLib.GetConfig("Item", v.item_id)
        table.insert(list,{
                info_id = v.item_id,
                iconType = Macros.Game.RolePropType.TOWER_PRESTIGE,
                price = info.sell_price * v.item_num ,
                quality = info.quality,
                name = info.name,
                item_num = v.item_num,   
                type = Macros.Game.NormalItemSubType.EQUIPDEBRIS,
                selectType = Macros.Game.SELECT_TYPE.EQUIP_DEBRIS,
        })
    end
    table.sort( list, function ( a , b )
        if a.quality < b.quality then
            return a.quality < b.quality
        elseif a.quality == b.quality then
            return a.info_id < b.info_id
        end
    end )
    return list
end

-- 获得英雄列表
function Modules.DebriSellModule:GetHeroLst()
    local list = {}
    local t2 = CallModuleFunc(ModuleType.HERO, "GetHeroInfoList")
    for k,v in pairs(t2) do
        if v.is_to_battle == 0 then
            local info = CPPGameLib.GetConfig("HeroLevel", v.quality_level)
            local data = CPPGameLib.GetConfig("Hero", v.info_id)
            local price = info.level_list[v.level].cost_value_list[1] + v.exp + data.price
            if price then
                table.insert(list,{
                        info_id = v.info_id,
                        iconType = info.level_list[v.level].cost_type_list[1],
                        price = price or 0 ,
                        name = data.name,
                        quality = v.quality_level,
                        item_num = 0,
                        type = Macros.Game.ObjType.HERO,
                        uid = v.uid,
                        lv = v.level,
                        break_level = v.break_level,
                        selectType = Macros.Game.SELECT_TYPE.HERO,
                    })
            end
        end
    end

    table.sort( list, function ( a , b )
        if a.quality < b.quality then
            return a.quality < b.quality
        elseif a.quality == b.quality then
            return a.price < b.price
        end
    end )

    CPPGameLib.PrintTable("xy" , list , "请求物品列表") 

    return list
end

--获得英雄碎片列表
function Modules.DebriSellModule:GetHeroDebrisLst()
    local list = {}
    local t1 = CallModuleFunc(ModuleType.Pack, "GetAllHeroDebris")
    for k,v in pairs(t1) do
        local info = CPPGameLib.GetConfig("Item", v.item_id)
        table.insert(list, { 
                info_id = v.item_id,
                iconType = Macros.Game.RolePropType.HERO_SOUL,
                price = info.sell_price * v.item_num ,
                quality = info.quality,
                name = info.name,
                item_num = v.item_num,
                type = Macros.Game.NormalItemSubType.HERODEBRIS,
                selectType = Macros.Game.SELECT_TYPE.HERO_DEBRIS,
        })
    end
    table.sort( list, function ( a , b )
        if a.quality < b.quality then
            return a.quality < b.quality
        elseif a.quality == b.quality then
            return a.info_id < b.info_id
        end
    end )
    return list
end

-- 获得圣器列表
function Modules.DebriSellModule:GetTreasureLst()
    local list = {}
    local t1 = CallModuleFunc(ModuleType.TREASURE, "GetAllTreasureLst")
    for k,v in pairs(t1) do
        local info = CPPGameLib.GetConfig("Treasure", v.info_id)
        local data = CPPGameLib.GetConfig("TreasureStrengthen", info.quality)
        if v.hero_infoid == 0 and info.quality < Macros.Game.QualityType.ORANGE then
            local price = data.level_list[v.strengthen_level].cost_value_list[1] + info.price
            table.insert(list, { 
                    info_id = v.info_id,
                    iconType = data.level_list[v.strengthen_level].cost_type_list[1],
                    price = price,
                    quality = info.quality,
                    name = info.name,
                    item_num = 0,
                    uid = v.uid,
                    type = Macros.Game.ObjType.TREASURE,
                    selectType = Macros.Game.SELECT_TYPE.TREASURE,
            }) 
        end
    end
 
    table.sort( list, function ( a , b )
        if a.quality < b.quality then
            return a.quality < b.quality
        elseif a.quality == b.quality then
            return a.info_id < b.info_id
        end
    end )
    return list
end

--设置选中
function Modules.DebriSellModule:SetOnData( data , var )
    local key = data.selectType .. data.info_id .. ( data.uid or "" )

    if var then
        self.onData[ key ] = data
    else
        if self.onData[ key ] then
            self.onData[ key ] = nil
        end
    end
    self:ShowOnData()
end

--获得选中
function Modules.DebriSellModule:GetOnData( data )
    if data then
        local key = data.selectType .. data.info_id .. ( data.uid or "" )
        return self.onData[ key ]
    else
        return self.onData
    end
end

--删除全部选中
function Modules.DebriSellModule:RemoveAllOnData( data )
    self.onData = {}
    self:ShowOnData()
end

--显示选中
function Modules.DebriSellModule:ShowOnData( ... )
    GlobalEventSystem:FireNextFrame(Macros.Event.DebriSellModule.DEDRISELL_UP)
end