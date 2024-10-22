--
-- @brief 物品背包模块 (物品)
-- @author: yjg
-- @date: 2018年12月25日18:37:56
--

Modules.PackModule = Modules.PackModule or BaseClass(BaseModule)

--------------------------------------------------------------------------------------------------------------------------------------------
--普通道具
--------------------------------------------------------------------------------------------------------------------------------------------

--更新普通道具
function Modules.PackModule:UpdateCommonGoods( data , for_init )
    --减少
    if data.flag == 0 then
        data.use_num = 0 - data.use_num
    end

    local is_update = true
    local info = self.commonGoods[data.item_id]
    if info then
        local item_num = info.item_num + data.use_num 

        --更新
        if item_num > 0 then
            info.item_num = item_num

        --删除
        else
            self.commonGoods[data.item_id] = nil
            if not for_init then
                self:DelFrmNewRecordMap(data.item_id)
            end
        end

    --增加
    else
        is_update = false
        self:AddCommonGoods(data)
        if not for_init then
            self:AddToNewRecordMap(data.item_id)
        end
        info = self.commonGoods[data.item_id]
    end

    --物品更新事件
    if not for_init then
        local obj_type , _ = ObjIDTool.GetInfo(data.item_id)
        if obj_type == Macros.Game.ObjType.ITEM then
            self:FireNextFrame( Macros.Event.PackModule.UPDATE_GOODS , info , is_update , Macros.Game.ObjType.ITEM )
        end
    end
end

--新增普通道具
function Modules.PackModule:AddCommonGoods(data, for_init)
    local good_data = {}
    self.commonGoods[data.item_id] = good_data

    good_data.item_id = data.item_id
    good_data.item_num = data.use_num

    local obj_type, info = ObjIDTool.GetInfo( data.item_id )
    good_data.obj_type = obj_type
    good_data.type = info.type
    good_data.sub_type = info.sub_type
    good_data.name = info.name
    good_data.quality = info.quality
    good_data.systemid = info.systemid
    good_data.goforward = info.goforward
    good_data.is_can_use = info.is_can_use
    good_data.description = info.description
    good_data.param1 = info.param1
    good_data.param2 = info.param2
end

--获得普通道具
function Modules.PackModule:GetAllCommonGoods( ... )
    local t1 = {}
    local t2 = {}
    local t3 = {}
    for k, v in pairs( self.commonGoods ) do
        if v.item_num > 0 and 
            v.obj_type == Macros.Game.ObjType.ITEM and
            v.sub_type ~= Macros.Game.NormalItemSubType.HERODEBRIS and
            v.sub_type ~= Macros.Game.NormalItemSubType.EQUIPDEBRIS and 
            v.sub_type ~= Macros.Game.NormalItemSubType.TREASURE_DEBRIS then
            
            if v.type == Macros.Game.ItemBigType.GIFT then
                table.insert(t2,v)
            elseif v.sub_type == Macros.Game.NormalItemSubType.SPIRIT_DEBRIS then
                table.insert(t3,v)
            else
                table.insert(t1,v)
            end            
        end
    end

    function PackSort(a,b)
        if a.quality > b.quality then
            return  a.quality > b.quality
        elseif a.quality == b.quality then
            if a.item_id > b.item_id then
                return a.item_id > b.item_id
            end
        end
    end

    table.sort(t1,PackSort)
    table.sort(t2,PackSort)
    table.sort(t3,PackSort)

    for i , v in ipairs(t1) do
        table.insert( t3 , v )
    end

    for i , v in ipairs(t3) do
        table.insert( t2 , v )
    end

    return t2
end

--按id获得道具
function Modules.PackModule:GetCommonGoods( id )
    return self.commonGoods[id]
end

--获得物品数量(根据ID)
function Modules.PackModule:GetNumByItemID( id )
    local goods = self:GetCommonGoods(id)

    if goods then
        local num = goods.item_num
        local type_ , info_ = ObjIDTool.GetInfo(id)
        if obj_type == Macros.Game.ObjType.ITEM and info_.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
            num = goods.item_num * info_.param2
        end
        return num
    end
    return 0
end

--获得物品数量(根据类型)
function Modules.PackModule:GetNumByItemType( type_1 , type_2 )
    type_1 = type_1 or  Macros.Game.ItemBigType.NORMAL
    local num = 0
    for k , v in pairs( self.commonGoods ) do
        local type_ , info_ = ObjIDTool.GetInfo( v.item_id )
        if info_.type == type_1 and info_.sub_type == type_2 then
            num = num + v.item_num
        end
    end
    return num
end

--根据ID获取过期时间
function Modules.PackModule:ItemValidTime( id )
    local type_ , info_ = ObjIDTool.GetInfo( id )
    local finally = info_.expiry_date

    if finally then
        
        local _ , openClothingTime =  CallModuleFunc(ModuleType.ROLE , "GetOpenClothing" )
        local end_time = nil
        local len = string.len( tostring( finally ) )

        -- --如果不是指定时间
        if len ~= 8 then
            end_time = openClothingTime + ( finally * 24 * 60 * 60 )

            local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( end_time )
            end_time = end_time - ( hour * 60 * 60)
            end_time = end_time - ( minute * 60 )
            end_time = end_time - ( second )
        else

            local year = string.sub( tostring(finally) , 0 , 4 )
            local month = string.sub( tostring(finally) , 5 , 6 )
            local day = string.sub( tostring(finally) , 7 , 8 )

            end_time = TimeTool.GetTimeStampFromDate( tonumber(year) , tonumber(month) , tonumber(day) )
        end

        -- --因为是第二天的零点消失 于是多加24小时
        end_time = end_time + ( 24 * 60 * 60 )

        local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( serverTime )
        serverTime = serverTime - ( hour * 60 * 60)
        serverTime = serverTime - ( minute * 60 )
        serverTime = serverTime - ( second )

        end_time = end_time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

        local day = math.floor( end_time / (24 * 60 * 60) )
        local hour = math.floor( end_time / (60 * 60) )
        local minute = math.floor( end_time / (60) )

        local now = nil
        if day > 0 then
            now =  CPPGameLib.GetString("title_residue" , day .. CPPGameLib.GetString("common_day") ) 
        elseif hour > 0 then
            now = CPPGameLib.GetString("title_residue" , hour .. CPPGameLib.GetString("common_hour_") ) 
        elseif minute > 0 then
            now = CPPGameLib.GetString("title_residue" , minute .. CPPGameLib.GetString("common_minute_") ) 
        else
            now = CPPGameLib.GetString("onlineawardoverdue") 
            end_time = 0
        end

        return now , end_time
    end
end


--------------------------------------------------------------------------------------------------------------------------------------------
--觉醒道具
--------------------------------------------------------------------------------------------------------------------------------------------

--更新普通道具
function Modules.PackModule:UpdateAwakenGoods( data , for_init )
    local is_update = true
    local info = self.awakenGoods[data.item_id]
    if info then
        local item_num = info.item_num + data.use_num 

        --更新
        if item_num > 0 then
            info.item_num = item_num

        --删除
        else
            self.awakenGoods[data.item_id] = nil
        end

    --增加
    else
        is_update = false
        self:AddAwakenGoods( data )
        info = self.awakenGoods[data.item_id]
    end

    --物品更新事件
    if not for_init then
        self:FireNextFrame( Macros.Event.PackModule.UPDATE_GOODS , info , is_update , Macros.Game.ObjType.AWAKEN_TOOL )
    end
end

--新增普通道具
function Modules.PackModule:AddAwakenGoods( data )
    self.awakenGoods[data.item_id] = {}
    self.awakenGoods[data.item_id].item_id = data.item_id
    self.awakenGoods[data.item_id].item_num = data.use_num

    local type_ , info = ObjIDTool.GetInfo( data.item_id )

    self.awakenGoods[data.item_id].name = info.name
    self.awakenGoods[data.item_id].quality = info.quality
    self.awakenGoods[data.item_id].description = info.description
end

--获得装备碎片
function Modules.PackModule:GetAllAwakenGoods( ... )
    local t1 = {}
    for i , v in pairs( self.awakenGoods ) do
        table.insert( t1 , v )
    end

    table.sort( t1 , function ( a , b)
        if a.quality ~= b.quality then
            return  a.quality > b.quality
        else
            return  a.item_id < b.item_id
        end
    end)
    return t1
end

--按id获得道具
function Modules.PackModule:GetAwakenGoods( id )
    return self.awakenGoods[id]
end
