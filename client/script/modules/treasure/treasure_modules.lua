--
-- @brief 圣器模块 
-- @author: yjg
-- @date: 2016年9月2日10:13:44
--

Modules = Modules or {}

Modules.TreasureModule = Modules.TreasureModule or BaseClass(BaseModule)

function Modules.TreasureModule:__init()
    self.__treasure_map = {}
    --选中的材料
    self.__selected_strengthen_materials_map = {}

    --强化上限 和 精炼上限列表
    self.__lift_max_info_list = {}
    for quality = 2 , 5 do
        --强化信息
        local strengthenInfo = CPPGameLib.GetConfig("TreasureStrengthen", quality )
        --精炼信息
        local refineInfo = CPPGameLib.GetConfig("TreasureRefine", quality )

        if strengthenInfo and refineInfo then
            self.__lift_max_info_list[quality] = { strengthenMax = #strengthenInfo.level_list , refineMax = table.maxn(refineInfo.level_list) }
        end
    end

    self.__prop_event_handle = self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE,function ( ... )
    end)
    
end

function Modules.TreasureModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--请求圣器列表
function Modules.TreasureModule:TreasureListReq(first)
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_LIST_REQ)
    GlobalNetAdapter:OnSend(data)
end

--请求圣器列表响应
function Modules.TreasureModule:TreasureListResp(info)
    for i,v in ipairs(info.treasure_list) do
        v.flag = 1
        self:UPTreasureData(v, true)
    end

    --初始化新增记录表
    local lst = CPPGameLib.SplitEx( Cookies.Get( Cookies.LevelType.ROLE  ,Macros.Setting.NewTreasure) , "<GH>")
    for k , v in pairs( lst ) do
        self:AddToNewRecordMap( tonumber(v) , true )
    end
end

--请求圣器强化
function Modules.TreasureModule:TreasureStrengthenReq(info)
    --战力变化 --装备精炼
    CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.TREASURE_INTENSIFY )

    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_STRENGTHEN_REQ)
    data.treasure_uid = info.treasure_uid
    data.cost_treasure_list = info.cost_treasure_list
    GlobalNetAdapter:OnSend(data)
end

--圣器强化响应
function Modules.TreasureModule:TreasureStrengthenResp(info)
    self:DeleteAllSelectedStrengthenMaterials()     

    GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_INTENSIFY, info)
end

--圣器精练
function Modules.TreasureModule:TreasureRefineReq(info)
    --战力变化 --装备精炼
    CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.TREASURE_REFINE )
    
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_REFINE_REQ)
    data.treasure_uid = info.treasure_uid
    GlobalNetAdapter:OnSend(data)
end

--圣器精练响应
function Modules.TreasureModule:TreasureRefineResp(info)
    GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_REFINE,info)
end

--圣器合成请求
function Modules.TreasureModule:TreasureComposeReq(treasure_infoid,treasure_num)
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_COMPOSE_REQ)
    data.treasure_info_id = treasure_infoid
    data.treasure_num = treasure_num
    GlobalNetAdapter:OnSend(data)
end

--圣器合成请求响应
function Modules.TreasureModule:TreasureComposeResp(info)
    GlobalEventSystem:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHVIEW,info)
end

--圣器穿戴请求
function Modules.TreasureModule:TreasurePutOnReq(info)
    CallModuleFunc(ModuleType.HERO,"SaveHeroPropertyBeforeChange")
    
    if info.treasure_uid ~= 0 then
        CallModuleFunc(ModuleType.HERO,"SetBeforeStrengthen",info.formation_pos,Macros.Game.ObjType.TREASURE)
    end

    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_PUT_ON_REQ)
    data.formation_pos = info.formation_pos
    data.treasure_pos = info.treasure_pos
    data.treasure_uid = info.treasure_uid
    GlobalNetAdapter:OnSend(data)
end

--圣器穿戴响应
function Modules.TreasureModule:TreasurePutOnResp(info)
    CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)

    if info.treasure_uid == 0 then
        --卸下
        GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_DISCHARGE,info)
    else       
        CallModuleFunc(ModuleType.HERO,"SetMessageTypeAndTranslateInfo",Macros.Game.WIND_WORDS_LINEUP_TYPE.TREASURE_MESSAGE,info)
        --装上
        GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_CROSS,info)
    end   
end

--圣器基本信息通知
function Modules.TreasureModule:TreasureInfoNotify(info)
    self:UPTreasureData(info)    
end

-- 消息说明: 圣器升品请求 消息Id: 21414
function Modules.TreasureModule:TreasureQualityUpReq( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明: 圣器升品请求 消息Id: 21414")
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_QUALITY_UP_REQ)
    data.uid = info.uid
    GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 圣器升品响应 消息Id: 21415
-- "uid__I",       --圣器uid
-- "info_id__I",       --圣器信息ID
-- "enhance_level__H", --强化等级
-- "exp__I",       --圣器经验
-- "refine_level__H",  --精炼等级
function Modules.TreasureModule:TreasureQualityUpResp( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明: 圣器升品响应 消息Id: 21415")
    GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_ADVANCE , info.uid )
    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("treasure_remouldWin") ) 
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--更新圣器数据
function Modules.TreasureModule:UPTreasureData( info , for_init )
    if info.flag == 1 then
        local obj_type , config = ObjIDTool.GetInfo(info.info_id)
        info.quality = config.quality
        info.type = config.type
        info.name = config.name
        info.new = config.new
        
        --更新圣器
        if self.__treasure_map[info.uid] then
            self.__treasure_map[info.uid] = info

            if not for_init then
                self:FireNextFrame(Macros.Event.TreasureModule.TREASURE_UPDATE, info)
            end

        --增加圣器
        else
            self.__treasure_map[info.uid] = info

            if not for_init then
                self:AddToNewRecordMap( info.uid )
                self:FireNextFrame(Macros.Event.TreasureModule.TREASURE_ADDED, info)
            end
        end

    --删除圣器
    else
        self.__treasure_map[info.uid] = nil  

        if not for_init then
            self:DelFrmNewRecordMap( info.uid )
            self:FireNextFrame(Macros.Event.TreasureModule.TREASURE_REMOVE, info)
        end
    end  

    --改变圣器
    if not for_init then
        self:FireNextFrame(Macros.Event.TreasureModule.TREASURE_CHANGE, info)
    end
end


--添加到新增记录表
function Modules.TreasureModule:AddToNewRecordMap(id, ignore_save)   
    self.__new_record_map = self.__new_record_map or {}

    local treasure_data = self:GetTreasure(id)
    if treasure_data and treasure_data.new then
        self.__new_record_map[id] = self.__new_record_map[id] or {}
        self.__new_record_map[id].type = true

        --保存
        if not ignore_save then
            self:SaveNewRecordMap()
        end
    end
end

--获得新增记录
function Modules.TreasureModule:GetNewRecord( id )   
    if not self.__new_record_map then
        return
    end

    if id then
        return self.__new_record_map[id]
    end
end

--获取新增记录表
function Modules.TreasureModule:GetNewRecordMap()
    return self.__new_record_map
end

--从新增记录表中删除
function Modules.TreasureModule:DelFrmNewRecordMap( id )   
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
function Modules.TreasureModule:SaveNewRecordMap( )   
    if not self.__new_record_map then
        return
    end
    
    local text = ""
    for k , v in pairs( self.__new_record_map ) do
        text = text .. k .. "<GH>"
    end
    Cookies.Set( Cookies.LevelType.ROLE , Macros.Setting.NewTreasure , text )
    
    GlobalEventSystem:Fire(Macros.Event.NewModule.PACK_EXTEND , Macros.Game.SystemID.TREASURE )
end

--根据uid获取圣器
function Modules.TreasureModule:GetTreasure( uid )
   return self.__treasure_map[uid]
end

--获取圣器数量(根据配置ID)
function Modules.TreasureModule:GetNumByInfoID(info_id, filter_func)
    local count = 0
    for _, treasure_info in pairs(self.__treasure_map) do
        if not filter_func then
            if treasure_info.info_id == info_id then
                count = count + 1
            end
        else
            if treasure_info.info_id == info_id and filter_func(treasure_info) then
                count = count + 1
            end
        end
    end 
    return count
end

--获取圣器总数和背包上限
function Modules.TreasureModule:GetTreasureSumAndMax()
    return CPPGameLib.GetTableLength(self.__treasure_map), CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.TREASURE_PACK_MAX)
end

--全部圣器列表
function Modules.TreasureModule:GetAllTreasureLst( ignore_dressed )
    --过滤已穿戴圣器
    if ignore_dressed then
        local t1 = {}
        for k , v in pairs( self.__treasure_map ) do
            if v.hero_infoid == 0 then
                table.insert( t1 , v )            
            end
        end
        return t1
    else
        return self.__treasure_map
    end
end

--获取圣器列表
function Modules.TreasureModule:GetTreasureSort()
    local list = CPPGameLib.ConvertMap2List(self.__treasure_map)

    function OnSort(a, b)
        --已穿戴圣器优先
        if a.hero_infoid == 0 and b.hero_infoid ~= 0 then
            return false
        elseif a.hero_infoid ~= 0 and b.hero_infoid == 0 then
            return true
        end

        if a.quality > b.quality then
            --高品质优先
            return a.quality > b.quality
 
        elseif a.quality == b.quality then
            --高强化等级优先
            if a.strengthen_level > b.strengthen_level then
                return a.strengthen_level > b.strengthen_level
            elseif a.strengthen_level == b.strengthen_level then

                --高精炼等级优先
                if a.refine_level > b.refine_level then
                    return a.refine_level > b.refine_level
                elseif a.refine_level == b.refine_level then

                    --小info_id优先
                    return a.info_id < b.info_id
                end
            end
        end
    end
    table.sort(list, OnSort)

    return list
end

--根据指定英雄ID获取圣器列表
function Modules.TreasureModule:GetTreasureSortByHeroID( hero_id )
    local lst = self:GetTreasureSort()
    local t1 = {}
    for i,v in ipairs(lst) do
        if v.hero_infoid == hero_id then
            table.insert(t1 , { 
                                name = v.name,
                                info_id = v.info_id,
                                refine_level = v.refine_level,
                                strengthen_level = v.strengthen_level,
                                uid = v.uid,
                                quality = v.quality,
                                equip_heroid = v.equip_heroid,
                                isStrengthen = self:ToUidGetTreasureIsRefine(v.uid)
                            })
        end
    end
    return t1
end

--获得指定类型的圣器列表
function Modules.TreasureModule:GetTreasureListByType( type )
    local t1 = {}
    for i,v in pairs(self.__treasure_map) do
        local info = CPPGameLib.GetConfig("Treasure", v.info_id)
        if info.type == type and v.hero_infoid == 0 then
            table.insert(t1,v)
        end
    end
    return t1
end

--获得比已穿戴圣器更好的圣器列表
function Modules.TreasureModule:GetBetterTreasureList( data )
    local type_ , info = ObjIDTool.GetInfo(data.info_id)
    local lst = self:GetTreasureListByType(info.type)
    
    if not self.onHeroInfo then
        return 
    end
    local treasuredata = self:GetTreasure( self.onHeroInfo.treasure_uid )

    local t1 = {}
    local t2 = {}
    for i , v in ipairs(lst) do
        local luckCnf = self:GetReinforcement( v.info_id )
        for ii,vv in ipairs(luckCnf) do
            if v.info_id == vv.treasureId and treasuredata.info_id ~= v.info_id then
                if vv.heroInfoId == self.onHeroInfo.hero_id and v.quality >= data.quality then
                    table.insert( t1 , v )
                    t2[v.uid] = true
                end
            end
        end
    end

    for i,v in ipairs(lst) do
        if treasuredata.info_id ~= v.info_id and v.uid ~= data.uid and ( (v.quality > data.quality) or ( (v.quality == data.quality) and v.strengthen_level > data.strengthen_level) ) then
            if not t2[v.uid] then
                table.insert( t1 , v )
            end
        end
    end
    
    return t1
end

--------------------------------------------------------------------------------------------------------------------------------
--强化&精炼相关
--------------------------------------------------------------------------------------------------------------------------------
--获取圣器强化、精炼的上限
function Modules.TreasureModule:GetLiftMaxInfo( quality )
   return self.__lift_max_info_list[quality]
end

--获取强化、精炼属性加成信息
function Modules.TreasureModule:GetLiftPropPlusInfos( data , isInfoId )
    local qianghua = nil
    local jinglian = nil
    if isInfoId then
        qianghua = self:GetStrengthenPropPlusMap(nil, data.strengthen_level , data.info_id) 
        jinglian = self:GetRefinePropPlusMap(nil, data.refine_level ,data.info_id)
    else
        qianghua = self:GetStrengthenPropPlusMap(data.uid) 
        jinglian = self:GetRefinePropPlusMap(data.uid)
    end
    

    if qianghua == 0 or jinglian == 0 then
        return 0
    end

    local t1 = {}
    for i,v in ipairs(qianghua.lst) do
        t1[v.type] = v.value
    end

    for i,v in ipairs(jinglian.lst) do
        if t1[v.type] then
            t1[v.type] = t1[v.type] + v.value 
        else
            t1[v.type] = v.value 
        end
    end

    local t2 = {}
    for k,v in pairs(t1) do
        table.insert(t2,{ type = k , value = v })
    end

    table.sort( t2, function ( a,b )
        return a.type < b.type
    end)

    return t2
end

--------------------------------------------------------------------------------------------------------------------------------
--强化相关
--------------------------------------------------------------------------------------------------------------------------------
--获得强化加成属性
function Modules.TreasureModule:GetStrengthenPropPlusMap( uid , lv , info_id )
    local treasureData = {}

    if uid then
        treasureData = self:GetTreasure(uid)
    else
        treasureData.info_id = info_id
        treasureData.strengthen_level = 1 or lv
    end

    --圣器信息
    local treasure_info = CPPGameLib.GetConfig("Treasure", treasureData.info_id)

    if treasure_info.type == Macros.Game.TreasureType.EXP then
        return 0 , 0 , 0
    end

    --当前属性数据
    local t1 = {}
    t1.level = lv or treasureData.strengthen_level
    t1.lst = {}
    for i ,valueLst in ipairs(treasure_info.strengthen_init_pro_list) do
        t1.lst[i] = { type = valueLst[1] , value = valueLst[2] + (treasure_info.strengthen_add_pro_list[i][2] * (t1.level - 1) ) }
    end
    table.sort( t1.lst , function ( a,b )
        return a.type < b.type
    end)

    --上一级
    local t2 = {}
    t2.level = lv or treasureData.strengthen_level
    t2.level = t2.level - 1
    if t2.level <= 1 then
        t2.level = 1
    end
    t2.lst = {}

    for i ,valueLst in ipairs(treasure_info.strengthen_init_pro_list) do
        t2.lst[i] = { type = valueLst[1] , value = valueLst[2] + (treasure_info.strengthen_add_pro_list[i][2] * (t2.level - 1) ) }
    end
    table.sort( t2.lst , function ( a,b )
        return a.type < b.type
    end)

    --下一级
    local t3 = {}
    t3.level = lv or treasureData.strengthen_level
    t3.level = t3.level + 1
    if t3.level > self.__lift_max_info_list[treasure_info.quality].strengthenMax then
        t3.level = lv or treasureData.strengthen_level
    end
    t3.lst = {}

    for i ,valueLst in ipairs(treasure_info.strengthen_init_pro_list) do
        t3.lst[i] = { type = valueLst[1] , value = valueLst[2] + (treasure_info.strengthen_add_pro_list[i][2] * (t3.level - 1) ) }
    end
    table.sort( t3.lst , function ( a,b )
        return a.type < b.type
    end)

    return t1 , t2 , t3 
end

--按照uid判断圣器是否能强化
function Modules.TreasureModule:ToUidGetTreasureIsStrengthen( uid )
    local info = self:GetTreasure( uid )
    local maxLst = self:GetLiftMaxInfo(info.quality)
    if info.strengthen_level >= maxLst.strengthenMax then
        return "false"
    end

    local num = 0
    for k , v in pairs(self.__treasure_map) do
        if v.type == Macros.Game.TreasureType.EXP then
            num = num + 1
            break
        
        elseif v.uid ~= uid and v.hero_infoid == 0 and v.refine_level <= 0 then
            num = num + 1
            break
        end
    end

    if num == 0 then
        return "false"
    else
        return "true"
    end
end

--获取圣器作为强化材料对应的经验
function Modules.TreasureModule.GetTreasureExp(treasure_info)
    local exp = GlobalCustomizedLogicAdapterMgr:Call("GetTreasureExp", treasure_info)
    if exp then
        return exp
    end
    exp = 0

    if not treasure_info then
        return exp
    end

    --圣器作为材料对应的经验
    local config_treasure = CPPGameLib.GetConfig("Treasure", treasure_info.info_id)
    exp = exp + (config_treasure and config_treasure.as_exp or 0)

    --圣器当前强化等级对应的经验
    local config_strengthen_lev_info = ConfigAdapter.Treasure.GetStrengthenLevInfo(treasure_info.quality, treasure_info.strengthen_level)
    exp = exp + (config_strengthen_lev_info and config_strengthen_lev_info.total_exp or 0)

    --圣器当前强化等级已获得的经验
    exp = exp + treasure_info.strengthen_exp

    return exp
end

--获取强化材料列表
function Modules.TreasureModule:GetStrengthenMaterialList(for_auto_add_materials, need_ret_list, iter_func)
    local material_list = {}

    for uid, v in pairs(self.__treasure_map) do
        --过滤待强化圣器
        if uid ~= self.onTreasureUid and
 
            --过滤已穿戴或已精炼过的圣器
            v.hero_infoid == 0 and v.refine_level == 0 and 
 
            --过滤紫品以上的非经验圣器(用于自动加入强化材料)
           (not for_auto_add_materials or (v.type == Macros.Game.TreasureType.EXP or v.quality <= Macros.Game.QualityType.PURPLE)) then
            
            --创建强化材料
            local material_data = {}
            material_data.uid = uid
            material_data.cost_type = ConfigAdapter.Treasure.GetStrengthenMaterialCostInfo(v)
            material_data.exp = Modules.TreasureModule.GetTreasureExp(v)
            material_data.is_selected = self.__selected_strengthen_materials_map[uid] ~= nil

            if need_ret_list ~= false then
                table.insert(material_list, material_data)
            end

            --执行迭代函数
            if iter_func then
                iter_func(material_data)
            end
        end
    end

    --排序处理
    if need_ret_list ~= false then
        table.sort(material_list, function(data1, data2)
            local treasure1 = self:GetTreasure(data1.uid)
            local treasure2 = self:GetTreasure(data2.uid)
            if not treasure1 then
                return false
            elseif not treasure2 then
                return true
            end

            --经验圣器优先
            if treasure1.type ~= treasure2.type then
                return treasure1.type > treasure2.type

            --低品质优先
            elseif treasure1.quality ~= treasure2.quality then
                return treasure1.quality < treasure2.quality

            --小的info_id优先
            elseif treasure1.info_id ~= treasure2.info_id then
                return treasure1.info_id < treasure2.info_id

            --小的uid优先        
            else
                return treasure1.uid < treasure2.uid
            end
        end)
    end

    return material_list
end

--添加/删除强化材料
function Modules.TreasureModule:SetSelectedStrengthenMaterial( data , type )
    if type == "add" then
        self.__selected_strengthen_materials_map[data.uid] = data
    else
        self.__selected_strengthen_materials_map[data.uid] = nil
    end

    --刷新选中信息
    GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_SELECT)
end

--删除全部强化材料
function Modules.TreasureModule:DeleteAllSelectedStrengthenMaterials()
    self.__selected_strengthen_materials_map = {}
end

--自动加入强化材料
function Modules.TreasureModule:AutoAddStrengthenMaterials()
    --检测数量上限
    local cur_material_num = CPPGameLib.GetTableLength(self.__selected_strengthen_materials_map)
    if cur_material_num >= 5 then
        return
    end

    local add_material_num = 0
    local limit_material_num = 5 - cur_material_num
    for i, v in ipairs(self:GetStrengthenMaterialList(true)) do
        if add_material_num >= limit_material_num then
            break
        end

        if not self.__selected_strengthen_materials_map[v.uid] then
            add_material_num = add_material_num +1
            self:SetSelectedStrengthenMaterial(v, "add")
        end
    end

    if cur_material_num + add_material_num <= 0 then
        return false, true
    end

    return true
end

--获得已选的强化材料列表
function Modules.TreasureModule:GetSelectedStrengthenMaterialMap()
    return self.__selected_strengthen_materials_map
end

--获得已选的强化材料对应的经验、消耗金币
function Modules.TreasureModule:GetSelectedStrengthenMaterialInfo()
    local gold
    local exp = 0
    for i,v in pairs(self.__selected_strengthen_materials_map) do
        exp = exp + v.exp
    end
    gold = exp

    return exp, gold
end

--计算经验升级
function Modules.TreasureModule:CalculateUpgrade( )
    local onInfo = self:GetOnTreasureInfo()

    --强化等级到最高了没
    if ( onInfo.strengthen_level >= self:GetLiftMaxInfo(onInfo.quality).strengthenMax ) then
        return false , 0 , 0 , 0 , 0
    end

    local total_selected_materials_exp = self:GetSelectedStrengthenMaterialInfo()

    --当前的经验值 加上 材料给的经验
    local endExp = onInfo.strengthen_exp + total_selected_materials_exp

    --拿到这一级的升级经验
    local config_strengthen_lev_info = ConfigAdapter.Treasure.GetStrengthenLevInfo(onInfo.quality, onInfo.strengthen_level)
    local nowInfo = CPPGameLib.GetConfig("TreasureStrengthen", onInfo.quality)
    --得到我当前升级的经验 减掉我身上的经验 拿到真正缺的拿点
    local nowExp = config_strengthen_lev_info.exp - onInfo.strengthen_exp 
    local temporary = endExp
    local level = onInfo.strengthen_level

    local function LevelCalculate( ... )
        -- 如果升级的经验 减掉 我有的经验
        if nowInfo.level_list[level] then
            if nowInfo.level_list[level].exp - temporary <= 0 then
                --如果是有的
                if nowInfo.level_list[level] then
                    temporary = temporary - nowInfo.level_list[level].exp
                    level = level + 1
                    LevelCalculate()
                end
            end
        end
    end
    LevelCalculate()

    -- 当前经验 上限经验 , 差额 , 吃完的等级 , 材料给的经验
    return onInfo.strengthen_exp , config_strengthen_lev_info.exp , nowExp , level , total_selected_materials_exp
end

--获取指定品质对应的强化上限信息
function Modules.TreasureModule:GetMaxStrengthenInfo(quality)
    local config_strengthen_info = CPPGameLib.GetConfig("TreasureStrengthen", quality)
    if not config_strengthen_info then
        return
    end

    --从后往前查找第一个符合条件的强化信息
    local config_strengthen_lev_info
    local role_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)
    for i = #config_strengthen_info.level_list, 1, -1 do        
        config_strengthen_lev_info = config_strengthen_info.level_list[i]
        if role_level >= config_strengthen_lev_info.need_level then
            return config_strengthen_lev_info
        end
    end
end

--一键强化
function Modules.TreasureModule:TreasureOneKeyStrengthen(uid, levelup)
    local treasure_info = self:GetTreasure(uid)
    if not treasure_info then
        return
    end

    --计算一键强化到指定等级所需的经验
    local need_exp = 0
    local config_strengthen_lev_info
    for i = treasure_info.strengthen_level, treasure_info.strengthen_level + levelup - 1 do
        config_strengthen_lev_info = ConfigAdapter.Treasure.GetStrengthenLevInfo(treasure_info.quality, i)
        if config_strengthen_lev_info then
            need_exp = need_exp + config_strengthen_lev_info.exp
        end
    end
    need_exp = need_exp - treasure_info.strengthen_exp

    local cost_exp = 0
    local all_material_list = self:GetStrengthenMaterialList(true, nil, function(material_data)
        cost_exp = cost_exp + material_data.exp
    end)

    --可用的材料不足以提升1级
    if need_exp <= 0 then
        return all_material_list, cost_exp, cost_exp
    end

    --查找一键强化到指定等级所需材料
    cost_exp = 0
    local ret_material_list = {}
    for _, material_data in ipairs(all_material_list) do
        if need_exp <= 0 then
            break
        end
        cost_exp = cost_exp + material_data.exp
        need_exp = need_exp - material_data.exp
        table.insert(ret_material_list, material_data)
    end

    return ret_material_list, cost_exp, cost_exp
end

--获取一键强化可提升的等级
function Modules.TreasureModule:GetMaxLevelupForOneKeyStrengthen(uid)
    local treasure_info = self:GetTreasure( uid )

    --计算圣器当前强化等级已升经验与所有圣器材料的经验之和
    local total_exp = treasure_info.strengthen_exp
    self:GetStrengthenMaterialList(true, false, function(material_data)
        total_exp = total_exp + material_data.exp
    end)

    --计算可提升的强化等级
    local level = 0
    local strengthenMax = self:GetLiftMaxInfo(treasure_info.quality).strengthenMax
    local config_strengthen_lev_info
    for i = treasure_info.strengthen_level, strengthenMax do
        if total_exp < 0 then
            break
        end
        config_strengthen_lev_info = ConfigAdapter.Treasure.GetStrengthenLevInfo(treasure_info.quality, i)
        total_exp = total_exp - config_strengthen_lev_info.exp 
        level = level + 1
    end

    return level - 1
end

--------------------------------------------------------------------------------------------------------------------------------
--精炼相关
--------------------------------------------------------------------------------------------------------------------------------
--获得精炼属性
function Modules.TreasureModule:GetRefinePropPlusMap( uid , lv , info_id )
    local treasureData = {}

    if uid then
        treasureData = self:GetTreasure(uid)
    else
        treasureData.info_id = info_id
        treasureData.refine_level = 0 or lv
    end

    --圣器信息
    local treasure_info = CPPGameLib.GetConfig("Treasure", treasureData.info_id)
    
    if treasure_info.type == Macros.Game.TreasureType.EXP then
        return 0 , 0 , 0
    end

    --当前属性数据
    local t1 = {}
    t1.level = lv or treasureData.refine_level
    t1.lst = {}
    for i ,valueLst in ipairs(treasure_info.refine_pro_type_list) do
        t1.lst[i] = { type = valueLst , value = treasure_info.refine_pro_value_list[i] * t1.level }
    end
    table.sort( t1.lst , function ( a,b )
        return a.type < b.type
    end)

    --上一级
    local t2 = {}
    t2.level = lv or treasureData.refine_level
    t2.level = t2.level - 1
    if t2.level <= 1 then
        t2.level = 1
    end
    t2.lst = {}

    for i ,valueLst in ipairs(treasure_info.refine_pro_type_list) do
        t2.lst[i] = { type = valueLst , value = treasure_info.refine_pro_value_list[i] * t2.level }
    end
    table.sort( t2.lst , function ( a,b )
        return a.type < b.type
    end)

    --下一级
    local t3 = {}
    t3.level = lv or treasureData.refine_level
    t3.level = t3.level + 1
    if t3.level > self.__lift_max_info_list[treasure_info.quality].refineMax then
        t3.level = lv or treasureData.refine_level
    end
    t3.lst = {}

    for i ,valueLst in ipairs(treasure_info.refine_pro_type_list) do
        t3.lst[i] = { type = valueLst , value = treasure_info.refine_pro_value_list[i] * t3.level }
    end
    table.sort( t3.lst , function ( a,b )
        return a.type < b.type
    end)
    return t1 , t2 , t3
end

--按照uid判断圣器是否能精炼
function Modules.TreasureModule:ToUidGetTreasureIsRefine( uid )
    local info = self:GetTreasure(uid)
    --精炼道具列表
    local refineLst = CPPGameLib.GetConfig("TreasureRefine", info.quality)
    --精炼道具所需列表
    local refineInfoLst = refineLst.level_list[info.refine_level]
    --按照品质的最高上限
    local max = self:GetLiftMaxInfo(info.quality).refineMax

    --获取材料与消耗货币
    local type_1 = refineInfoLst.cost_treasure_num <= self:GetTreasureRefineSameNameMaterialNum(info.info_id)
    local type_2 = refineInfoLst.cost_item_num <= CallModuleFunc( ModuleType.Pack,"GetNumByItemID" , refineInfoLst.cost_item_id )
    local type_3 = refineInfoLst.cost_value_list[1] <= CallModuleFunc(ModuleType.ROLE,"GetPropValue" , refineInfoLst.cost_type_list[1] )

    local role_level_now = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local IsOpen = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.TREASURE_REFINE )

    if IsOpen and info.refine_level < max and type_1 and type_2 and type_3 and refineInfoLst.need_level <= role_level_now then
        return "true"
    else
        return "false"
    end
end

--获取用于精炼的同名圣器材料数量
function Modules.TreasureModule:GetTreasureRefineSameNameMaterialNum(info_id)
    local count = 0
    for _, treasure_info in pairs(self.__treasure_map) do
        if treasure_info.uid ~= self.onTreasureUid and
            treasure_info.hero_infoid == 0 and
            treasure_info.refine_level == 0 and
            treasure_info.info_id == info_id and 
            treasure_info.strengthen_level == 1 then

            count = count + 1            
        end
    end 
    return count
end

--获取用于精炼的所有材料列表
function Modules.TreasureModule:GetTreasureRefineMaterialList( ... )
    --选中的圣器
    local onInfo = self:GetOnTreasureInfo()
    --精炼道具列表
    local refineLst = CPPGameLib.GetConfig("TreasureRefine", onInfo.quality)
    --精炼道具所需列表
    local refineInfoLst = refineLst.level_list[onInfo.refine_level]

    local t1 = {}
    local num = self:GetTreasureRefineSameNameMaterialNum(onInfo.info_id)
    table.insert( t1 , { id = onInfo.info_id , value = refineInfoLst.cost_treasure_num , own = num } )

    --要吃的材料不是0
    local num = CallModuleFunc( ModuleType.Pack,"GetNumByItemID" , refineInfoLst.cost_item_id )
    table.insert( t1 , { id = refineInfoLst.cost_item_id , value = refineInfoLst.cost_item_num , own = num } )

    --要花掉的钱
    local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , refineInfoLst.cost_type_list[1] )
    table.insert( t1 , { id = refineInfoLst.cost_type_list[1] , value = refineInfoLst.cost_value_list[1] , own = num } )

    return t1
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------
--圣器进化
-----------------------------------------------------------------------------------------------------------------------------------------------------------------
--根据uid找到进化信息
function Modules.TreasureModule:ToUidGetEvolveInfo( uid )
    local treasure_info = self:GetTreasure( uid )
    local treasureCnf = CPPGameLib.GetConfig("Treasure", treasure_info.info_id)
    local t1 = {}
    t1.nowInfo = { 
                    uid = uid ,
                    res_id = treasureCnf.res_id, 
                    name = treasureCnf.name,
                    quality = treasureCnf.quality,
                    info_id = treasure_info.info_id,
                    refine_level = treasure_info.refine_level,
                    strengthen_level = treasure_info.strengthen_level,
                }

    if treasureCnf.next_quality_treasure_id ~= 0 then
        local treasureCnf_ = CPPGameLib.GetConfig("Treasure", treasureCnf.next_quality_treasure_id)

        local strengthen_exp = treasure_info.strengthen_exp
        local data = CPPGameLib.GetConfig("TreasureStrengthen", treasure_info.quality)
        local price = data.level_list[ treasure_info.strengthen_level ]
        strengthen_exp = strengthen_exp + price.total_exp

        local nex_strengthen_level = 1
        for i , v in ipairs( CPPGameLib.GetConfig("TreasureStrengthen", treasureCnf_.quality).level_list ) do
            if v.total_exp > strengthen_exp then
                break
            end
            nex_strengthen_level = v.strengthen_level
        end

        t1.nexInfo = { 
                        uid = uid ,
                        res_id = treasureCnf_.res_id, 
                        name = treasureCnf_.name,
                        quality = treasureCnf_.quality,
                        info_id = treasureCnf.next_quality_treasure_id,
                        refine_level = treasure_info.refine_level,
                        strengthen_level = nex_strengthen_level ,
                    }

        t1.consume = {}
        --消耗物品
        local UpCost = CPPGameLib.GetConfig("TreasureQualityUpCost", treasure_info.refine_level )
        t1.consume.goods = { item_id = treasureCnf.quality_up_cost_item_id , item_num = UpCost.cost_item_num }
        --消耗货币
        t1.consume.currency = {}
        for i , v in ipairs( UpCost.cost_currency_type_list ) do
            table.insert( t1.consume.currency , {type = v , num = UpCost.cost_currency_value_list[i] } )
        end
    end

    return t1
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------
--其他
-----------------------------------------------------------------------------------------------------------------------------------------------------------------
--设置选中的装备
function Modules.TreasureModule:SetOnTreasureUid( uid )
    self.onTreasureUid = uid or self:GetTreasureSort()[1].uid
end

--获得选中的装备
function Modules.TreasureModule:GetOnTreasureInfo()
    return self:GetTreasure(self.onTreasureUid)
end

--设置选中的阵容信息
function Modules.TreasureModule:SetOnHeroInfo( info )
    self.onHeroInfo = info
end

--修改圣器的持有人
function Modules.TreasureModule:SetTreasureHero( nowid , nexid ,nowEquipUid , nexEquipUid )
    local t1 = {}
    for i,v in pairs(self.__treasure_map) do
        if nowEquipUid then
            if v.uid == nowEquipUid then
                v.hero_infoid = 0
            elseif v.uid == nexEquipUid then
                v.hero_infoid = nexid
            end
        else
            if v.hero_infoid == nowid then
                v.hero_infoid = nexid
            end
        end
    end
end

--用类型取得他最弱的装备的id
function Modules.TreasureModule:ToTypeGetWeakestId( item_type )
    for k, v in pairs(CPPGameLib.GetConfig("Treasure", nil, nil , true )) do
        if v.type == item_type and v.quality == Macros.Game.QualityType.BLUE then
            return k
        end
    end 
end

--获得装备的宿命
function Modules.TreasureModule:GetReinforcement( info_id )
    if not self.luckLst then
        local cnf = CPPGameLib.GetConfig( "Hero" ,nil ,nil , true )
        self.luckLst = {}
        for k,v in pairs(cnf) do
            if v.relation_treasure_list then
                for i , vv in ipairs(v.relation_treasure_list) do
                    for __ , vvv in ipairs(vv) do
                        table.insert( self.luckLst , {  
                                                        heroInfoId = k , 
                                                        treasureId = vvv , 
                                                        fateName = v.relation_treasure_reinforcementname_list[i] ,
                                                        fateDescribe = v.relation_treasure_reinforcementdes_list[i] ,
                                                    } )
                    end
                end
            end
        end
    end

    local t1 = {}
    for i,v in ipairs( self.luckLst ) do
        if info_id == v.treasureId then
            table.insert(t1 , v)
        end
    end

    table.sort( t1, function ( a , b )
        return a.heroInfoId < b.heroInfoId
    end )
    
    return t1
end

