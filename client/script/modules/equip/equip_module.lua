--
-- @brief 装备模块 
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--

Modules = Modules or {}

Modules.EquipModule = Modules.EquipModule or BaseClass(BaseModule)

function Modules.EquipModule:__init()
    self.onEquipUid = 0
    self.__equip_map = {}
    self.equipIntensify = {}

    --强化上限 和 精炼上限列表
    self.__lift_max_info_list = {}
    for quality = 1 , 5 do
        --强化信息
        local strengthenInfo = CPPGameLib.GetConfig("EquipmentStrenthen", quality )
        --精炼信息
        local refineInfo = CPPGameLib.GetConfig("EquipmentRefine", quality )
        if strengthenInfo and refineInfo then
            self.__lift_max_info_list[quality] = { strengthenMax = #strengthenInfo.level_list , refineMax = #refineInfo.level_list }
        end
    end

    --升星表整理
    self:SettleEquipmentStarUpCnf()
end

function Modules.EquipModule:__delete()
end

-- 装备列表请求
function Modules.EquipModule:EquipmentItemListReq(info)
    GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.IPMENT_LIST_REQ)) 
end

--装备列表响应: 
function Modules.EquipModule:EquipmentListResp(info)
    --初始化准备列表
    for i,v in ipairs(info.equipment_list) do
        v.flag = 1
        self:UPEquipData(v , true)
    end

    --初始化新增记录表
    local lst = CPPGameLib.SplitEx( Cookies.Get( Cookies.LevelType.ROLE ,Macros.Setting.NewEquip) , "<GH>")
    for k , v in pairs( lst ) do
        self:AddToNewRecordMap( tonumber(v) , true )
    end
end

--装备详情请求: 
function Modules.EquipModule:EquipmentInfoReq(info)
    if not info then return end
end

--装备详情响应: 
function Modules.EquipModule:EquipmentInfoResp(info)
end

--装备合成请求: 
function Modules.EquipModule:EquipmentComposeReq(info)
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_COMPOSE_REQ)
    data.equipment_uid = info
    --发送
    GlobalNetAdapter:OnSend(data)
end

--装备合成响应: 
function Modules.EquipModule:EquipmentComposeResp(info)
    local data = {}
    data.id = info.equipment_uid
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_COMPOUND_TIPS ,false , info.equipment_uid )
end

--请求装备强化: 
function Modules.EquipModule:EquipMentstrengReq( info )
    --战力变化 --装备强化
    CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.EQUIP_INTENSIFY )

    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_STRENG_REQ)
    data.type = info.type
    data.equipment_uid = info.id
    --发送
    GlobalNetAdapter:OnSend(data)
end

--请求装备强化响应:
function Modules.EquipModule:EquipMentstrengResp( info )    
    self:IntensifyWin(info)
end

--消息说明: 装备基本信息通知 消息Id: 20713
function Modules.EquipModule:EquipMentInfoNotify( info )
    self:UPEquipData(info)  
end

--装备精炼请求
function Modules.EquipModule:EquipMentRefineReq(info)   
    if not info then return end
    
    --战力变化 --装备精炼
    CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.EQUIP_REFINE )

    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_REFINE_REQ)
    data.equipment_uid = info.uid

    data.cost_item_list = info.cost_item_list
    -- data.item_id = info.itemid
    -- data.item_num = info.num
    --发送
    GlobalNetAdapter:OnSend(data)
end

--装备精炼响应
function Modules.EquipModule:EquipMentRefineResp(info)
    --战力变化 --装备精炼    
    self:RefineWin(info)
end

--获取当前的强化大师等级
function Modules.EquipModule:GetEquipStrengthenLevelMinAndEquipRefineLevelMin(formation_pos)
    
end

--装备穿戴请求
function Modules.EquipModule:EquipMentPutOnReq(info)
    CallModuleFunc(ModuleType.HERO,"SaveHeroPropertyBeforeChange")

    if not info then return end
    if info.equipment_uid ~= 0 then
        CallModuleFunc(ModuleType.HERO,"SetBeforeStrengthen",info.formation_pos,Macros.Game.ObjType.EQUIP)
    end
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_PUT_ON_REQ)
    data.formation_pos = info.formation_pos
    data.equipment_pos = info.equipment_pos
    data.equipment_uid = info.equipment_uid
    --发送
    GlobalNetAdapter:OnSend(data)
end

--装备穿戴响应
function Modules.EquipModule:EquipMentPutOnResp(info)
    if info.equipment_uid == 0 then
        self:DischargeEquip(info)
    else
        self:CrossEquip(info)
    end
end

--一键强化
function Modules.EquipModule:EquipmentOneKeyEnhanceReq(info)
    --战力变化 --装备精炼
    local pos = info.slot + 1

    CallModuleFunc(ModuleType.HERO,"SetBeforeStrengthen",pos,Macros.Game.ObjType.EQUIP)
    CallModuleFunc(ModuleType.HERO,"SaveHeroPropertyBeforeChange")

    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ)
    data.slot = info.slot
    GlobalNetAdapter:OnSend(data)
end

--一键强化相应
function Modules.EquipModule:EquipmentOneKeyEnhanceResp(info)
    local is_havechangepropertyneedwindwords = CallModuleFunc(ModuleType.HERO,"GetIsHaveChangePropertyNeedWindWords")
    CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true,is_havechangepropertyneedwindwords)
    CallModuleFunc(ModuleType.HERO,"SetMessageTypeAndTranslateInfo",Macros.Game.WIND_WORDS_LINEUP_TYPE.EQUIP_ONEKEY_ENHANCE_MESSAGE,info)
    GlobalEventSystem:Fire(Macros.Event.EquipModule.ONE_KEY_ENHANCE_EQUIP,info)
end


--更新本地装备数据
function Modules.EquipModule:UPEquipData(info, for_init)
    if info.flag == 1 then
        local _ , config_equip = ObjIDTool.GetInfo(info.info_id)
        if config_equip then
            info.quality = config_equip.quality
            info.type = config_equip.item_type
            info.name = config_equip.name
            info.taste = config_equip.taste
            info.talent = config_equip.talent
            info.new = config_equip.new
            info.star = info.star
            info.star_level = info.star_level

            --更新装备
            if self.__equip_map[info.uid] then
                self.__equip_map[info.uid] = info

                if not for_init then
                    self:FireNextFrame(Macros.Event.EquipModule.EQUIP_UP_EQUIP, info)
                end

            --增加装备
            else
                self.__equip_map[info.uid] = info

                if not for_init then
                    self:AddToNewRecordMap(info.uid)
                    self:FireNextFrame(Macros.Event.EquipModule.EQUIP_ADD_EQUIP, info)
                end
            end
        end

    --删除装备
    else
        self.__equip_map[info.uid] = nil  

        if not for_init then
            self:DelFrmNewRecordMap( info.uid )
            self:FireNextFrame(Macros.Event.EquipModule.EQUIP_REMOVE_EQUIP, info)
        end
    end

    --改变装备
    if not for_init then
        self:FireNextFrame(Macros.Event.EquipModule.EQUIP_CHANGE_EQUIP, info)
    end
end

--添加到新增记录表
function Modules.EquipModule:AddToNewRecordMap(id, ignore_save)   
    self.__new_record_map = self.__new_record_map or {}

    local equip_data = self:GetEquip(id)
    if equip_data and equip_data.new then
        self.__new_record_map[id] = self.__new_record_map[id] or {}
        self.__new_record_map[id].type = true

        --保存
        if not ignore_save then
            self:SaveNewRecordMap()
        end
    end
end

--获得新增记录
function Modules.EquipModule:GetNewRecord( id )   
    if not self.__new_record_map then
        return
    end

    if id then
        return self.__new_record_map[id]
    end
end

--获取新增记录表
function Modules.EquipModule:GetNewRecordMap()
    return self.__new_record_map
end

--从新增记录表中删除
function Modules.EquipModule:DelFrmNewRecordMap( id , ignore_save )   
    if not self.__new_record_map then
        return
    end

    if self.__new_record_map[id] then
        self.__new_record_map[id] = nil

        --保存
        if not ignore_save then 
            self:SaveNewRecordMap()
        end
    end    
end

--本地保存
function Modules.EquipModule:SaveNewRecordMap( )   
    if not self.__new_record_map then
        return
    end
    
    local text = ""
    for k , v in pairs( self.__new_record_map ) do
        text = text .. k .. "<GH>"
    end
    Cookies.Set( Cookies.LevelType.ROLE , Macros.Setting.NewEquip , text )
    
    GlobalEventSystem:Fire(Macros.Event.NewModule.PACK_EXTEND , Macros.Game.SystemID.EQUIP )
end

--装备列表
function Modules.EquipModule:GetEquipLit()
    return self.__equip_map
end

--获得装备
function Modules.EquipModule:GetEquip(uid)
    return self.__equip_map[uid]
end

--获取装备强化、精炼的上限
function Modules.EquipModule:GetLiftMaxInfo( quality )
   return self.__lift_max_info_list[quality]
end

--获取装备数量(根据配置ID)
function Modules.EquipModule:GetNumByInfoID(info_id, filter_func)
    local count = 0
    for _, equip_info in pairs(self.__equip_map) do
        if not filter_func then
            if equip_info.info_id == info_id then
                count = count + 1
            end
        else
            if equip_info.info_id == info_id and filter_func(equip_info) then
                count = count + 1
            end
        end
    end 
    return count
end

--获得该类型的装备
function Modules.EquipModule:GetTypeEquip( type , showhero )
    local all = self:GetEquipLit()
    local t1 = {}
    for i,v in pairs(all) do
        local info = CPPGameLib.GetConfig("Equipment", v.info_id)
        if info then
            if not showhero then
                if info.item_type == type and v.equip_heroid == 0 then
                    table.insert(t1,v)
                end
            else
                if info.item_type == type then
                    table.insert(t1,v)
                end
            end
        end
    end
    return t1
end

--获得比身上强的
function Modules.EquipModule:GetBetterEquip( data )
    local type_ , info = ObjIDTool.GetInfo(data.info_id)
    local lst = self:GetTypeEquip(info.item_type)

    if not self.onHeroInfo then
        return 
    end
    local equipdata = self:GetEquip( self.onHeroInfo.equip_uid )

    local t1 = {}
    local t2 = {}
    for i , v in ipairs(lst) do
        local luckCnf = self:GetReinforcement( v.info_id )
        for ii,vv in ipairs(luckCnf) do
            if v.info_id == vv.equipId and v.info_id ~= equipdata.info_id then
                if vv.heroInfoId == self.onHeroInfo.hero_id and v.taste >= data.taste then
                    table.insert( t1 , v )
                    t2[v.uid] = true
                end
            end
        end
    end

    for i,v in ipairs(lst) do
        if v.info_id ~= equipdata.info_id and v.uid ~= data.uid and ( (v.taste > data.taste) or ( (v.taste == data.taste) and v.strengthen_level > data.strengthen_level) ) then
            if not t2[v.uid] then
                table.insert( t1 , v )
            end
        end
    end

    return t1
end

--获得穿着的装备
function Modules.EquipModule:GetCross( id )
    local t1 = {}
    for k,v in pairs(self.__equip_map) do
        if v.equip_heroid == id then
            table.insert(t1,v)
        end
    end

    for i,v in ipairs(t1) do
        local info = CPPGameLib.GetConfig("Equipment", v.info_id)
        v.type = info.item_type
        v.name = info.name
        v.streng = info.streng_level_max
        v.refine = info.refine_level_max
        v.quality = info.quality
        v.suit_id = info.suit_id
    end
    return t1
end

--获得强化属性
-- 2935 装备强化属性面板公式应该是 2017年11月27日16:31:15
function Modules.EquipModule:GetStrengthenPropPlusMap( uid , lv , info_id )
    local equipData = {}

    if uid then
        equipData = self:GetEquip(uid)
    else
        equipData.info_id = info_id
        equipData.strengthen_level = 1 or lv
    end

    --圣器信息
    local equipInfo = CPPGameLib.GetConfig("Equipment", equipData.info_id)


    --当前属性数据
    local t1 = {}
    t1.level = lv or equipData.strengthen_level
    t1.lst = {}
    t1.lst[1] = { type = equipInfo.strengthen_init_pro[1] , value = equipInfo.strengthen_init_pro[2] + ( (t1.level - 1) * equipInfo.strengthen_add_pro) }

    --上一级
    local t2 = {}
    t2.level = lv or equipData.strengthen_level
    t2.level = t2.level - 1
    if t2.level <= 1 then
        t2.level = 1
    end
    t2.lst = {}
    t2.lst[1] = { type = equipInfo.strengthen_init_pro[1] , value = equipInfo.strengthen_init_pro[2] + ( (t2.level - 1) * equipInfo.strengthen_add_pro) }

    --下一级
    local t3 = {}
    t3.level = lv or equipData.strengthen_level
    t3.level = t3.level + 1
    if t3.level > self.__lift_max_info_list[equipInfo.quality].strengthenMax then
        t3.level = lv or equipData.strengthen_level
    end
    t3.lst = {}
    t3.lst[1] = { type = equipInfo.strengthen_init_pro[1] , value = equipInfo.strengthen_init_pro[2] + ( (t3.level - 1) * equipInfo.strengthen_add_pro) }
    
    --下个天赋
    local t4 = {}
    t4.level = lv or equipData.strengthen_level
    t4.talent = equipInfo.talent
    t4.nowLst = {}
    t4.nexLst = {}

    if equipInfo.talent then
        local lst = CPPGameLib.GetConfig("EquipmentTalent",equipInfo.talent)
        lst = lst.level_list

        table.sort( lst, function ( a , b )
            return a.talent_level < b.talent_level
        end )
        for k , v in ipairs( lst ) do
            if v.intensify_level <= t4.level then
                table.insert(t4.nowLst , v )
            else
                table.insert(t4.nexLst , v )
            end
        end
    end

    return t1 , t2 , t3 , t4
end

--获得精炼属性
function Modules.EquipModule:GetRefinePropPlusMap( uid , lv , info_id )
    local equipData = {}

    if uid then
        equipData = self:GetEquip(uid)
    else
        equipData.info_id = info_id
        equipData.refine_level = 0 or lv
    end

    --圣器信息
    local equipInfo = CPPGameLib.GetConfig("Equipment", equipData.info_id)

    --当前属性数据
    local t1 = {}
    t1.level = lv or equipData.refine_level
    t1.lst = {}
    for i ,valueLst in ipairs(equipInfo.refine_pro_type_list) do
        t1.lst[i] = { type = valueLst , value = equipInfo.refine_pro_value_list[i] * t1.level }
    end
    table.sort( t1.lst , function ( a,b )
        return a.type < b.type
    end)

    --上一级
    local t2 = {}
    t2.level = lv or equipData.refine_level
    t2.level = t2.level - 1
    if t2.level <= 1 then
        t2.level = 1
    end
    t2.lst = {}
    for i ,valueLst in ipairs(equipInfo.refine_pro_type_list) do
        t2.lst[i] = { type = valueLst , value = equipInfo.refine_pro_value_list[i] * t2.level }
    end
    table.sort( t2.lst , function ( a,b )
        return a.type < b.type
    end)

    --下一级
    local t3 = {}
    t3.level = lv or equipData.refine_level
    t3.level = t3.level + 1
    if t3.level > self.__lift_max_info_list[equipInfo.quality].strengthenMax then
        t3.level = lv or equipData.refine_level
    end
    t3.lst = {}
    for i ,valueLst in ipairs(equipInfo.refine_pro_type_list) do
        t3.lst[i] = { type = valueLst , value = equipInfo.refine_pro_value_list[i] * t3.level }
    end
    table.sort( t3.lst , function ( a,b )
        return a.type < b.type
    end)

    return t1 , t2 , t3 
end

--获取指定品质对应的精炼上限信息
function Modules.EquipModule:GetMaxRefineInfo(quality)
    local config_refine_info = CPPGameLib.GetConfig("EquipmentRefine", quality)
    if not config_refine_info then
        return
    end

    --从后往前查找第一个符合条件的精炼信息
    local config_refine_lev_info
    local role_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)
    for i = table.maxn(config_refine_info.level_list), 0, -1 do        
        config_refine_lev_info = config_refine_info.level_list[i]
        if role_level >= config_refine_lev_info.need_level then
            return config_refine_lev_info
        end
    end
end

--装备条目显示信息
function Modules.EquipModule:GetLiftPropPlusInfos( data , isInfoId )

    local qianghua = nil
    local jinglian = nil
    if isInfoId then
        qianghua = self:GetStrengthenPropPlusMap(nil, data.strengthen_level , data.info_id) 
        jinglian = self:GetRefinePropPlusMap(nil, data.refine_level ,data.info_id)
    else
        qianghua = self:GetStrengthenPropPlusMap(data.uid) 
        jinglian = self:GetRefinePropPlusMap(data.uid)
    end

    local t1 = {}
    t1[qianghua.lst[1].type] = qianghua.lst[1].value

    for i,v in ipairs(jinglian.lst) do
    if t1[v.type] then
        t1[v.type] = t1[v.type] + v.value
    else
        t1[v.type] = v.value
    end
    end

    local t2 = {}
    for i,v in pairs(t1) do
    table.insert(t2,{ type = i , value = v})
    end

    table.sort( t2, function ( a,b )
        return a.type < b.type
    end)
    return t2
end

--全部的套装部件
function Modules.EquipModule:AllSuit( ... )
    --整张套装表
    local equipment = CPPGameLib.GetConfig("EquipmentSuit", nil, nil , true )
    local t3 = {}
    for id,v in pairs(equipment) do
        t3[id] = {}
        for i,vv in pairs(v.suit_list) do
            local info = CPPGameLib.GetConfig("Equipment", vv)
            table.insert(t3[id] , {id = vv , type = info.item_type , suit_id = id , name = info.name } )
        end
    end

    for k,v in pairs(t3) do
        table.sort(v,function ( a , b )
            return a.type < b.type
        end)
    end
    return t3
end

--获取该套装
function Modules.EquipModule:GetSuit( uid , info)
    --根据uid获取信息
    local data = {}

    if uid then
        data = self:GetEquip(uid)
    else
        data = info
    end

    --获取到装备信息
    local info = CPPGameLib.GetConfig("Equipment", data.info_id)

    --获取身上所有的套装信息
    local suitLst = self:AllSuit()
    --获取该套装信息
    local suit = suitLst[info.suit_id]

    --如果有这个套装的话
    local suitInfo = {}
    if suit then
        local info = CPPGameLib.GetConfig("EquipmentSuit", suit[1].suit_id)
        for i=1,4 do
            if info["pro"..i.."_type_list"] then
                local t1 = {}
                for j=1,#info["pro"..i.."_type_list"] do
                    table.insert(t1 , { type = info["pro"..i.."_type_list"][j] , value = info["pro"..i.."_value_list"][j] ,hero = data.equip_heroid or 0 }  )
                end
                table.insert(suitInfo,t1)
            end
        end
        return suit , suitInfo , info.suit_list
    else
        return false , false
    end
end

--获取当前穿的装备激活的套装数量
function Modules.EquipModule:GetLightUpSuitNum(uid,equip_map)
    --激活装备的位置
    local pos_list = {}
    local light_up_num = 0
    if not uid or not equip_map then
        return 0
    end

    local cur_equipInfo = self:GetEquip(uid)

    --获取到装备信息
    local info = CPPGameLib.GetConfig("Equipment", cur_equipInfo.info_id)

    --获取该套装信息
    local suit_info = CPPGameLib.GetConfig("EquipmentSuit", info.suit_id , false )
    if suit_info then
        for i,equip_id in ipairs(suit_info.suit_list) do
            if equip_map[equip_id] then
                light_up_num = light_up_num + 1
                --获取到装备信息
                local equip_info = CPPGameLib.GetConfig("Equipment", equip_id)
                table.insert(pos_list,equip_info.item_type)
            end
        end
    end
    
    return light_up_num,pos_list,suit_info and suit_info.name or "",info and info.quality or 0
end

--获取当前穿的装备激活的套装数量
function Modules.EquipModule:GetLightUpSuitNumList(equip_map,special_equip_map)
    --激活装备的位置
    local pos_list = {0,0,0,0}   
    if not equip_map or not special_equip_map then
        return pos_list
    end
    for i=1,4 do
        if special_equip_map[i-1] then
            local light_up_num = 0
            --获取到装备信息
            local info = CPPGameLib.GetConfig("Equipment", special_equip_map[i-1].info_id)
            --获取该套装信息
            local suit_info = CPPGameLib.GetConfig("EquipmentSuit", info.suit_id)
            if suit_info then
                for i,equip_id in ipairs(suit_info.suit_list) do
                    if equip_map[equip_id] then
                        light_up_num = light_up_num + 1
                    end
                end
            end 
            pos_list[i] = light_up_num
        end
    end
    
    return pos_list
end

--装备列表排序
function Modules.EquipModule:GetEquipSort( info )
    local list = CPPGameLib.ConvertMap2List(self.__equip_map)

    -- 按照品级从高到低→同品级按精炼从高到低→同精炼按强化从高到低→同强化按ID从小到大排序
    table.sort( list, function ( a , b )
        if a.equip_heroid ~= 0 and b.equip_heroid == 0 then
            return a.equip_heroid ~= 0
        elseif a.equip_heroid ~= 0 and b.equip_heroid ~= 0 then
            if a.taste > b.taste then
                return a.taste > b.taste
            elseif a.taste == b.taste then
                if a.refine_level > b.refine_level then
                    return a.refine_level > b.refine_level
                elseif  a.refine_level == b.refine_level then
                    if a.strengthen_level > b.strengthen_level then
                        return a.strengthen_level > b.strengthen_level
                    elseif a.strengthen_level == b.strengthen_level then
                        return a.info_id < b.info_id
                    end
                end
            end
        elseif a.equip_heroid == 0 and b.equip_heroid == 0 then
            if a.taste > b.taste then
                return a.taste > b.taste
            elseif a.taste == b.taste then
                if a.refine_level > b.refine_level then
                    return a.refine_level > b.refine_level
                elseif  a.refine_level == b.refine_level then
                    if a.strengthen_level > b.strengthen_level then
                        return a.strengthen_level > b.strengthen_level
                    elseif a.strengthen_level == b.strengthen_level then
                        return a.info_id < b.info_id
                    end
                end
            end
        end
    end )

    return list
end

--按照装备英雄的id来找到列表
function Modules.EquipModule:GetHeroEquipSort( hero_id )
    local lst = self:GetEquipSort()
    local t1 = {}
    for i,v in ipairs(lst) do
        local type_ , info = ObjIDTool.GetInfo(v.info_id)
        if v.equip_heroid == hero_id then
            table.insert(t1 , { 
                                name = v.name,
                                info_id = v.info_id,
                                item_type = info.item_type,
                                refine_level = v.refine_level,
                                strengthen_level = v.strengthen_level,
                                uid = v.uid,
                                quality = v.quality,
                                equip_heroid = v.equip_heroid,
                                isStrengthen = self:ToUidGetEquipIsStrengthen(v.uid)
                            })
        end
    end

    table.sort( t1, function ( a , b )
        return a.item_type < b.item_type
    end )

    return t1
end

--按照uid判断装备是否能强化
function Modules.EquipModule:ToUidGetEquipIsStrengthen( uid )
    if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.EQUIP_INTENSIFY) then
        return "false"
    end

    local info = self:GetEquip(uid)
    --按照品质的最高上限
    local qualityMax = self:GetLiftMaxInfo(info.quality).strengthenMax
    --按照玩家等级的最高上限
    local LvMax = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL) * 2
    --玩家持有的金币
    local holdGold = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD)
    --真正的上限
    local max = LvMax
    if LvMax > qualityMax then
        max = qualityMax
    end
    --需要消耗的钱
    local strenConfig = CPPGameLib.GetConfig("EquipmentStrenthen", info.quality)
    local info_ = strenConfig.level_list[info.strengthen_level]
    if not info_ then
        return "false"
    end
    local gold = info_.cost_value_list[1]

    local role_level_now = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if info.strengthen_level < max and holdGold >= gold then
        return "true"
    else
        return "false"
    end
end

--按照uid判断装备是否能精炼
function Modules.EquipModule:ToUidGetEquipIsRefine( uid )
    if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.EQUIP_REFINE) then
        return "false"
    end

            local itemLst = ConfigAdapter.Common.GetRefineItemLst()

    
    local info = self:GetEquip(uid)
    --按照品质的最高上限
    local qualityMax = self:GetLiftMaxInfo(info.quality).refineMax

    --装备精炼材料是否存在
    local is_cost_item_exist
    for _, cost_item_id in ipairs(ConfigAdapter.Common.GetRefineItemLst()) do
        if CallModuleFunc(ModuleType.Pack, "GetNumByItemID", cost_item_id) > 0 then
            is_cost_item_exist = true
            break
        end
    end

    local config = CPPGameLib.GetConfig("EquipmentRefine", info.quality)
    local config_level_info = config.level_list[ info.refine_level ]
    local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if info.refine_level < qualityMax and config_level_info.need_level <= roleLevel and is_cost_item_exist then
        return "true"
    else
        return "false"
    end
end

--计算所有进阶石能让这个东西到几级
function Modules.EquipModule:ToUidGetEquipRefineMax( uid )
    local equipInfo = self:GetEquip( uid )

    --全部石头加起来的经验
    local itemLst = ConfigAdapter.Common.GetRefineItemLst()
    local exp = 0 
    for i , v in ipairs( itemLst ) do
        local itemNum = CallModuleFunc(ModuleType.Pack, "GetNumByItemID",v)
        if itemNum ~= 0 then
            for i = 1 , itemNum do
                local itemInfo = CPPGameLib.GetConfig("Item",v)
                exp = exp + itemInfo.param1
            end
        end
    end


    --精炼上限
    local refineMax = self:GetLiftMaxInfo(equipInfo.quality).refineMax
    local level = 0
    for i = equipInfo.refine_level , refineMax do
        local upExp = self:GetUpgrade( uid , i )
        if exp - upExp >= 0 then
            exp = exp - upExp
            level = level + 1
        else
            break
        end
    end

    --能让这玩意升
    print("xy" , "level" )
    return level 
end

--根据装备的物品id来获得他的属性加成
function Modules.EquipModule:ToInfoIdGetEquipProperty( info_id )
    local cnf = CPPGameLib.GetConfig("Equipment", info_id )
    local t1 = {}
    table.insert( t1 , { type_ = "intensify" , type = cnf.strengthen_init_pro[1] , value = cnf.strengthen_init_pro[2] , add = cnf.strengthen_add_pro } )
    local t2 = {}
    table.insert( t2 , t1 )

    local t3 = {}
    for i,v in ipairs(cnf.refine_pro_type_list) do
        table.insert( t3 , { type_ = "refine" , type = v , value = cnf.refine_pro_value_list[i] , add = cnf.refine_pro_value_list[i] } )
    end
    table.insert( t2 , t3 )

    return t2
end

--强化成功
function Modules.EquipModule:IntensifyWin( info )
    CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)    
    GlobalEventSystem:Fire(Macros.Event.EquipModule.EQUIP_INTENSIFY,info)
end

--精炼成功
function Modules.EquipModule:RefineWin( info )
    CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)
    GlobalEventSystem:Fire(Macros.Event.EquipModule.EQUIP_REFINE,info)
end

--穿好装备
function Modules.EquipModule:CrossEquip( info )    
    CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)
    CallModuleFunc(ModuleType.HERO,"SetMessageTypeAndTranslateInfo",Macros.Game.WIND_WORDS_LINEUP_TYPE.EQUIP_MESSAGE,info)
    GlobalEventSystem:Fire(Macros.Event.EquipModule.EQUIP_CROSS_EQUIP,info)
end

--卸下装备
function Modules.EquipModule:DischargeEquip( info )
    CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)
    GlobalEventSystem:Fire(Macros.Event.EquipModule.EQUIP_DISCHARGE_EQUIP,info)
end

--设置选中的装备
function Modules.EquipModule:SetOnEquipUid( uid )
    self.onEquipUid = uid
end

--获得选中的装备
function Modules.EquipModule:GetOnEquipInfo()
    return self:GetEquip(self.onEquipUid)
end

--设置选中的阵容信息
function Modules.EquipModule:SetOnHeroInfo( info )
    self.onHeroInfo = info
end

--获得装备的宿命
function Modules.EquipModule:GetReinforcement( info_id )
    if not self.luckLst then
        local cnf = CPPGameLib.GetConfig("Hero",nil,nil,true)
        self.luckLst = {}
        for k,v in pairs(cnf) do
            if v.relation_equip_list then
                for i , vv in ipairs(v.relation_equip_list) do
                    for __ , vvv in ipairs(vv) do
                        table.insert( self.luckLst , {  
                                                        heroInfoId = k , 
                                                        equipId = vvv , 
                                                        fateName = v.relation_equip_reinforcementname_list[i] ,
                                                        fateDescribe = v.relation_equip_reinforcementdes_list[i] ,
                                                    } )
                    end
                end
            end
        end
    end

    local t1 = {}
    for i,v in ipairs( self.luckLst ) do
        if info_id == v.equipId then
            table.insert(t1 , v)
        end
    end

    table.sort( t1, function ( a , b )
        return a.heroInfoId < b.heroInfoId
    end )
    
    return t1
end

--修改装备的持有人
function Modules.EquipModule:SetEquipHero( nowid , nexid ,nowEquipUid , nexEquipUid )
    local lst = self:GetEquipSort()
    local t1 = {}
    for i,v in ipairs(lst) do
        if nowEquipUid then
            if v.uid == nowEquipUid then
                v.equip_heroid = 0
            elseif v.uid == nexEquipUid then
                v.equip_heroid = nexid
            end
        else
            if v.equip_heroid == nowid then
                v.equip_heroid = nexid
            end
        end
    end
end

--一键强化
function Modules.EquipModule:OneKeyIntensify( index )

    --该阵位全部装备
    local _,heroInfo,__ = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", index )

    --存起来
    local t1 = {}
    for k,v in pairs(heroInfo) do
        table.insert( t1 , { level = v.strengthen_level , uid = v.uid , quality = v.quality } )
    end

    --我的钱
    local gold = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.GOLD)

    --我的等级
    local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.LEVEL)
    local consume = 0

    if #t1 <= 0 then
        return consume , 0 , "noCross"
    end
    local notype = ""
    local addLevel = 0

    local function Figure( ... )
        --找到强化等级最低的那个
        table.sort(t1 , function( a , b )
            return a.level < b.level
        end )
        --最小
        local min = t1[1]
        --读的表
        local cnf = CPPGameLib.GetConfig("EquipmentStrenthen", min.quality)
        local level = min.level
        --如果强化要求的价钱大于我身上有的钱的话 而且 等级大于上线的话 等级大于角色等级*2的话
        if cnf.level_list[level].cost_value_list[1] > gold or level > self.__lift_max_info_list[min.quality].strengthenMax or level >= (roleLevel*2) then 
            --找我的装备
            for i,v in ipairs(t1) do
                --所有的装备里
                local vCnf = CPPGameLib.GetConfig("EquipmentStrenthen", v.quality).level_list[v.level].cost_value_list[1]
                --如果需要的钱小于我身上的钱的话 等级小于上线的话 
                if vCnf < gold and v.level < self.__lift_max_info_list[v.quality].strengthenMax and level < (roleLevel*2)  then
                    --这个的等级+1
                    t1[i].level = t1[i].level + 1
                    --钱加上
                    consume = consume + vCnf
                    gold = gold - vCnf
                    addLevel = addLevel + 1 

                    --调自己
                    Figure()
                    --跳掉
                    break

                elseif vCnf > gold and consume == 0 then
                    notype = "nogold"
                elseif addLevel == 0 then 
                    notype = "nolevel"
                end
            end
        else
            t1[1].level = t1[1].level + 1
            consume = consume + cnf.level_list[level].cost_value_list[1]
            gold = gold - cnf.level_list[level].cost_value_list[1]
            addLevel = addLevel + 1 
            Figure()
        end
    end
    Figure()

    --一个钱和一个价格列表
    return consume , t1 , notype
end

--一键精炼
function Modules.EquipModule:OneKeyRefine( uid , level )

    local equipInfo = self:GetEquip( uid )

    if level <= 0 then
        local itemLst = ConfigAdapter.Common.GetRefineItemLst()
        local t1 = {}
        for i , v in ipairs( itemLst ) do
            local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID",v )
            if not t1[v] then
                 t1[v] = {}
                 t1[v].id = v
                 t1[v].exp = 0
                 t1[v].num = item_num
            end
        end
        return t1
    end

    --等级升级所需精炼
    local upExp = 0
    for i = equipInfo.refine_level , (level - 1) + equipInfo.refine_level do
        local exp = self:GetUpgrade( uid , i )
        upExp = upExp + exp
    end
    print("xy" , "UpExp----" , upExp )

    --当前精炼经验
    local nowExp = equipInfo.refine_exp
    print("xy" , "nowExp----" , nowExp )
    --所需经验
    local needExp = upExp - nowExp
    print("xy" , "needExp----" , needExp )


    local level_max = self:ToUidGetEquipRefineMax( uid )

    local itemLst = ConfigAdapter.Common.GetRefineItemLst()
    local t1 = {}
    for i , v in ipairs( itemLst ) do
        if not t1[v] then
             t1[v] = {}
             t1[v].id = v
             t1[v].exp = 0
             t1[v].num = 0
        end
        local itemNum = CallModuleFunc(ModuleType.Pack, "GetNumByItemID",v)
        if itemNum ~= 0 then
            local itemInfo = CPPGameLib.GetConfig("Item",v)
            for ii = 1 , itemNum do
                local exp = 0
                for iii , vvv in pairs( t1 ) do
                    exp = exp + vvv.exp
                end

                if level_max ~= 0 and level ~= 0 then
                    if exp < needExp then
                         t1[v].num = t1[v].num + 1
                         t1[v].exp = t1[v].exp + itemInfo.param1
                    else
                         break
                    end
                else
                    t1[v].num = t1[v].num + 1
                    t1[v].exp = t1[v].exp + itemInfo.param1
                end
            end
        end
    end

    return t1
end

--获得升级所需的经验
function Modules.EquipModule:GetUpgrade( uid , lv )
    --获得选中的道具的信息
    local equipInfo = self:GetEquip( uid )

    --装备信息
    equipInfo = CPPGameLib.GetConfig("Equipment", equipInfo.info_id)
    --精炼信息
    local data = CPPGameLib.GetConfig("EquipmentRefine", equipInfo.quality)
    --升级所需经验
    local lv = data.level_list[lv]
    local exp = 0
    if lv then
        exp = lv.exp 
    end
    return exp
end


--用类型取得他最弱的装备的id
function Modules.EquipModule:ToTypeGetWeakestId( item_type )
    local t1 = {}
    for k,v in pairs(CPPGameLib.GetConfig("Equipment", nil, nil , true )) do
        if v.item_type == item_type and v.quality >= Macros.Game.QualityType.GREEN then
            table.insert( t1 , { id = k , taste = v.taste })
        end
    end 

    if #t1 ~= 0 then
        table.sort( t1, function ( a , b )
            return a.taste < b.taste
        end )
        return t1[1].id
    end

end

--获得英雄总数量和上限
function Modules.EquipModule:GetEquipSumAndMax()
    local num = self:GetEquipLit()
    return CPPGameLib.GetTableLength(num) , CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.EQUIP_PACK_MAX)
end