--
-- @brief 物品背包模块 (碎片)
-- @author: yjg
-- @date: 2018年12月22日11:23:10
--

Modules.PackModule = Modules.PackModule or BaseClass(BaseModule)

--------------------------------------------------------------------------------------------------------------------------------------------
--装备碎片
--------------------------------------------------------------------------------------------------------------------------------------------

--更新装备碎片
function Modules.PackModule:UpdateEquipDebris( data, for_init )
    local info = self.equipDebris[data.item_id]
    if info then
        local item_num = info.item_num + data.use_num 

        --更新
        if item_num > 0 then
            info.item_num = item_num

        --删除
        else
            self.equipDebris[data.item_id] = nil

        end

    --增加
    else
        self:AddEquipDebris( data )
        info = self.equipDebris[data.item_id]
    end

    if not for_init then
        self:FireNextFrame( Macros.Event.EquipModule.EQUIP_UP_DEBRIS , info )
    end
end

--新增装备碎片
function Modules.PackModule:AddEquipDebris( data )
    self.equipDebris[data.item_id] = {}
    self.equipDebris[data.item_id].item_id = data.item_id
    self.equipDebris[data.item_id].item_num = data.use_num

    local type_ , info = ObjIDTool.GetInfo( data.item_id )
    self.equipDebris[data.item_id].item_name = info.name
    self.equipDebris[data.item_id].quality = info.quality
    self.equipDebris[data.item_id].target_id = info.target_id

    local equipment = CPPGameLib.GetConfig("Equipment", info.target_id)
    self.equipDebris[data.item_id].taste = equipment.taste

    local equipmentCnf  = CPPGameLib.GetConfig("EquipmentCompose", info.target_id)
    self.equipDebris[data.item_id].target_num = equipmentCnf.item_num
end

--获得装备碎片
function Modules.PackModule:GetAllEquipDebris( ... )
    local t2 = {}
    local t3 = {}
    for k,v in pairs( self.equipDebris ) do
        if v.item_num >= v.target_num then
            table.insert( t2 , v )
        else
            table.insert( t3 , v )
        end
    end

    local function OnSort( a , b )
        if a.taste > b.taste then
            return a.taste > b.taste
        elseif a.taste == b.taste then
            if a.item_num > b.item_num then
                return a.item_num > b.item_num
            elseif a.item_num == b.item_num then
                return a.item_id > b.item_id
            end
        end  
    end

    table.sort( t2 , OnSort )
    table.sort( t3 , OnSort )

    for i,v in ipairs( t3 ) do
        table.insert( t2 , v )
    end

    return t2
end

--按id获得装备碎片
function Modules.PackModule:GetEquipDebris( id )
    return self.equipDebris[id]
end

--按照装备ID获得其碎片和上限
function Modules.PackModule:ToEquipIdGetDebris( id )
    local num , max = 0 , 0 
    local heroCnf = CPPGameLib.GetConfig("EquipmentCompose", id , false )
    if heroCnf then
        max = heroCnf.item_num
        local debris = self:GetEquipDebris( heroCnf.item_id )
        if debris then
            num = debris.item_num
        end
    end
    return num , max 
end

--------------------------------------------------------------------------------------------------------------------------------------------
--英雄碎片
--------------------------------------------------------------------------------------------------------------------------------------------
--更新英雄碎片
function Modules.PackModule:UpdateHeroDebris( data, for_init )
    local info = self.heroDebris[data.item_id]
    if info then
        local item_num = info.item_num + data.use_num 

        --更新
        if item_num > 0 then
            info.item_num = item_num

        --删除
        else
            self.heroDebris[data.item_id] = nil
        end

    --增加
    else
        self:AddHeroDebris( data )
        info = self.heroDebris[data.item_id]
    end

    if not for_init then
        self:FireNextFrame( Macros.Event.HeroModule.HERO_UP_DEBRIS , info )
    end
end

--新增装备碎片
function Modules.PackModule:AddHeroDebris( data )
    local good_data = {}
    self.heroDebris[data.item_id] = good_data

    good_data.item_id = data.item_id
    good_data.item_num = data.use_num

    local type_ , info = ObjIDTool.GetInfo( data.item_id )
    good_data.item_name = info.name
    good_data.quality = info.quality
    good_data.target_id = info.target_id

    local heroCompoundCnf = CPPGameLib.GetConfig("HeroCompound", info.target_id)
    good_data.target_num = heroCompoundCnf.item_num
end

--获得英雄碎片
function Modules.PackModule:GetAllHeroDebris( ... )
    local t2 = {}
    local t3 = {}
    for k,v in pairs( self.heroDebris ) do
        if v.item_num >= v.target_num then
            table.insert( t2 , v )
        else
            table.insert( t3 , v )
        end
    end

    local function OnSort( a , b )
        if a.quality > b.quality then
            return a.quality > b.quality
        elseif a.quality == b.quality then
            if a.item_num > b.item_num then
                return a.item_num > b.item_num
            elseif a.item_num == b.item_num then
                return a.item_id > b.item_id
            end
        end  
    end

    table.sort( t2 , OnSort )
    table.sort( t3 , OnSort )

    for i,v in ipairs( t3 ) do
        table.insert( t2 , v )
    end

    return t2
end

--按id获得英雄碎片
function Modules.PackModule:GetHeroDebris( id )
    return self.heroDebris[id]
end

--按照英雄ID获得其碎片和上限
function Modules.PackModule:ToHeroIdGetDebris( id )
    local num , max = 0 , 0 
    local heroCnf = CPPGameLib.GetConfig("HeroCompound", id , false )
    if heroCnf then
        max = heroCnf.item_num
        local debris = self:GetHeroDebris( heroCnf.item_id )
        if debris then
            num = debris.item_num
        end
    end
    return num , max 
end
