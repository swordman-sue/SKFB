--
-- @brief 装备模块(提升)
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--

Modules = Modules or {}

Modules.EquipModule = Modules.EquipModule or BaseClass(BaseModule)


------------------------------------------------------------------------------------------------------------------------------------
-- 升星
------------------------------------------------------------------------------------------------------------------------------------

--消息说明: 装备升星请求 消息Id: 20718
function Modules.EquipModule:EquipmentStarUpReq(info)
    CPPGameLib.PrintTable("xy" , info , "消息说明: 装备升星请求 消息Id: 20718")  
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_STAR_UP_REQ)
    data.equipment_uid = info.uid
    GlobalNetAdapter:OnSend( data ) 
end

--消息说明: 装备升星请响应 消息Id: 20719
function Modules.EquipModule:EquipmentStarUpResp(info)
    CPPGameLib.PrintTable("xy" , info , "消息说明: 装备升星请响应 消息Id: 20719")  
    GlobalEventSystem:FireNextFrame( Macros.Event.EquipModule.EQUIP_UPSTAR )
end

--获取装备佩戴顺序列表
function Modules.EquipModule:GetEquipAdornOrderLst(  )
    local t1 = {}
    for i = 1 , 5 do
        --按阵位获取全部信息
        local _,equipLst = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", i )
        local t2 = {}
        for j , vv in pairs( equipLst ) do
            local _ , info_ = ObjIDTool.GetInfo(vv.info_id)
            if info_.star_max then
                table.insert( t2 , { 
                                        uid = vv.uid ,    
                                        info_id = vv.info_id,
                                        type = vv.type,
                                        name = vv.name ,
                                        quality = vv.quality ,
                                        equip_heroid = vv.equip_heroid , 
                                        refine_level = vv.refine_level ,
                                        strengthen_level = vv.strengthen_level , 
                                     } )
            end
        end

        table.sort( t2 , function ( a , b )
            return a.type < b.type
        end )   
        table.insert( t1 , t2 )
    end
    return t1
end

--获取装备的升星数据
function Modules.EquipModule:ToUidGetEquipUpStarInfo( uid )
    local info = self:GetEquip( uid )
    local config_equip = CPPGameLib.GetConfig("Equipment", info.info_id , false )

    if not config_equip.star_max then
        return {}
    end

    local t1 = self.StarUpCnf[ info.type ][ config_equip.quality ][ info.star ][ info.star_level ]
    --当前属性
    local now = {}
    now.info = {}
    for i , v in ipairs( t1.pro_type_list ) do
        table.insert( now.info , { type = v , value = t1.pro_value_list[i] } )
    end
    now.desc = t1.activate_pro_des
    now.name = CPPGameLib.GetString("equip_StartLevel" , info.star , info.star_level )

    --当前消耗
    local consume = {}
    consume.prop = { 
                    [1] = {item_id = t1.cost_item_id , item_num = t1.cost_item_num},
                    [2] = {item_id = config_equip.star_up_cost_item_id , item_num = t1.cost_self_num},
                }
    consume.money = { type = Macros.Game.RolePropType.GOLD , num = t1.cost_gold }

    --下级属性
    local nex = {}
    local t2 = nil
    local star = info.star
    local star_level = 0
    if self.StarUpCnf[ info.type ][ config_equip.quality ][ star ][ info.star_level + 1 ] then
        t2 = self.StarUpCnf[ info.type ][ config_equip.quality ][ star ][ info.star_level + 1 ]

        star = star
        star_level = info.star_level + 1
    elseif self.StarUpCnf[ info.type ][ config_equip.quality ][ star + 1 ] then
        t2 = self.StarUpCnf[ info.type ][ config_equip.quality ][ star ][ 0 ]

        star = star + 1
        star_level = 0
    end

    nex.info = {}
    if star < config_equip.star_max then
        nex.name = CPPGameLib.GetString("equip_StartLevel" , star , star_level )
        if t2 then
            for i , v in ipairs( t2.pro_type_list ) do
                table.insert( nex.info , { type = v , value = t2.pro_value_list[i] } )
            end
            nex.desc = t2.activate_pro_des
        end
    end

    local t1 = {}
    t1.nowInfo = now
    t1.nexInfo = nex
    t1.expend = consume

    return t1
end

--整理升星表数据
function Modules.EquipModule:SettleEquipmentStarUpCnf(  )
    if self.StarUpCnf then
        return 
    end
    self.StarUpCnf = {}
    for type = 0 , 3 do
        self.StarUpCnf[ type ] = {}
        local config_equip = CPPGameLib.GetConfig("EquipmentStarUp", type , false )
        for i , v in ipairs( config_equip.sort_lst ) do
            if not self.StarUpCnf[ type ][ v.quality ] then
                self.StarUpCnf[ type ][ v.quality ] = {}
            end
            if not self.StarUpCnf[ type ][ v.quality ][ v.star ] then
                self.StarUpCnf[ type ][ v.quality ][ v.star ] = {}
            end
            if not self.StarUpCnf[ type ][ v.quality ][ v.star ][ v.level ] then
                self.StarUpCnf[ type ][ v.quality ][ v.star ][ v.level ] = v
            end
        end
    end
end

--整理升星表数据
function Modules.EquipModule:GetStarUpCnf( type , quality , star , star_level )
    if self.StarUpCnf[ type ] then
        if self.StarUpCnf[ type ][ quality ] then
            if self.StarUpCnf[ type ][ quality ][ star ] then
                if self.StarUpCnf[ type ][ quality ][ star ][ star_level ] then
                    return self.StarUpCnf[ type ][ quality ][ star ][ star_level ]
                end
            end
        end
    end
end

--装备升星是否开启
function Modules.EquipModule:StarUpIsOpen( )
    local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.EQUIP_UP_STAR )
    return openType
end



------------------------------------------------------------------------------------------------------------------------------------
-- 生品
------------------------------------------------------------------------------------------------------------------------------------
--消息说明: 装备升品请求 消息Id: 20720
function Modules.EquipModule:EquipmentQualityUpReq(info)
    CPPGameLib.PrintTable("xy" , info , "消息说明: 装备升星请求 消息Id: 20718")  
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_QUALITY_UP_REQ)
    data.equipment_uid = info.uid
    GlobalNetAdapter:OnSend( data ) 
end

--消息说明: 装备升品响应 消息Id: 20721
function Modules.EquipModule:EquipmentQualityUpResp(info)
    CPPGameLib.PrintTable("xy" , info , "消息说明: 装备升星请响应 消息Id: 20719")  
    GlobalEventSystem:FireNextFrame( Macros.Event.EquipModule.EQUIP_ADVANCE , info.equipment_uid )
end

--根据uid找到进化信息
function Modules.EquipModule:ToUidGetEvolveInfo( uid )
    local equipInfo = self:GetEquip( uid )
    local equipCnf = CPPGameLib.GetConfig("Equipment", equipInfo.info_id)
    local t1 = {}
    --当前信息
    t1.nowInfo = { 
                    uid = uid ,
                    res_id = equipCnf.res_id, 
                    name = equipCnf.name,
                    quality = equipCnf.quality,
                    info_id = equipInfo.info_id,
                    refine_level = equipInfo.refine_level,
                    strengthen_level = equipInfo.strengthen_level,
                }

    if equipCnf.next_quality_equipment_id ~= 0 then
        local equipCnf_ = CPPGameLib.GetConfig("Equipment", equipCnf.next_quality_equipment_id)

        --强化计算
        local strengthen_exp = 0
        local data = CPPGameLib.GetConfig("EquipmentStrenthen", equipInfo.quality)
        local price = data.level_list[ equipInfo.strengthen_level ]
        strengthen_exp = strengthen_exp + price.total_cost_gold

        local nex_strengthen_level = 1
        for i , v in ipairs( CPPGameLib.GetConfig("EquipmentStrenthen", equipCnf_.quality).level_list ) do
            if v.total_cost_gold > strengthen_exp then
                break
            end
            nex_strengthen_level = v.strength_level
        end

        --精炼计算
        local refine_exp = equipInfo.refine_exp
        local data = CPPGameLib.GetConfig("EquipmentRefine", equipInfo.quality)
        local price = data.level_list[ equipInfo.refine_level ]
        refine_exp = refine_exp + price.total_exp

        local nex_refine_level = 0
        for i , v in ipairs( CPPGameLib.GetConfig("EquipmentRefine", equipCnf_.quality).level_list ) do
            if v.total_exp > refine_exp then
                break
            end
            nex_refine_level = v.refine_level
        end

        --下级信息
        t1.nexInfo = { 
                        uid = uid ,
                        res_id = equipCnf_.res_id, 
                        name = equipCnf_.name,
                        quality = equipCnf_.quality,
                        info_id = equipCnf.next_quality_equipment_id,
                        refine_level = nex_refine_level,
                        strengthen_level = nex_strengthen_level ,
                    }

        t1.consume = {}
        --消耗物品
        local UpCost = CPPGameLib.GetConfig("EquipmentQualityUp", equipCnf.quality )
        t1.consume.goods = { item_id = UpCost.cost_item_id , item_num = UpCost.cost_item_num }
        --消耗货币
        t1.consume.currency = {type = UpCost.cost_currency_type , num = UpCost.cost_currency_value }
    end

    return t1
end
