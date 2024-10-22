--
-- @brief 回收界面模块 
-- @author: yjg
-- @date: 2016年9月14日10:06:04
--

Modules = Modules or {}

Modules.RecycleModule = Modules.RecycleModule or BaseClass(BaseModule)

function Modules.RecycleModule:__init()
    self.setout = {}
    self.resolveLst = {}
end

function Modules.RecycleModule:__delete()
    
end

--分解请求
function Modules.RecycleModule:RecycleDecomposerReq( info )
    -- CPPGameLib.PrintTable("xy",info,"分解请求")
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECYCLE_DECOMPOSE_REQ)
    data.obj_type = info.obj_type
    data.target_list = info.target_list
    GlobalNetAdapter:OnSend(data)
end

--分解回调
function Modules.RecycleModule:RecycleDecomposeResp( info )
    -- CPPGameLib.PrintTable("xy",info,"分解回调")
    local t1 = {}
    --金钱列表
    for i,v in ipairs(self.setout.return_currency_list) do
        local config = CPPGameLib.GetConfig("HuoBi", v.type)
        if v.value ~= 0 then
            table.insert(t1 , { id = config.big_icon , num = v.value })
        end
    end

    --道具列表
    for i,v in ipairs(self.setout.return_object_list) do
        if v.obj_num ~= 0 then
            table.insert(t1 , { id = v.obj_id , num = v.obj_num })
        end
    end

    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self.setout = {}
    self:RemoveAllOnData()
end

--重生申请
function Modules.RecycleModule:RecycleResetReq( info )
    CPPGameLib.PrintTable("xy" , info , "重生申请 ---RecycleResetReq ")
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECYCLE_RESET_REQ)
    data.obj_type = info.obj_type
    data.target_uid = info.target_uid
    GlobalNetAdapter:OnSend(data) 
end

-- 重生回调
function Modules.RecycleModule:RecycleResetResp( info )
    CPPGameLib.PrintTable("xy",info,"重生回调")
    local t1 = {}
    --金钱列表
    for i,v in ipairs(self.setout.return_currency_list) do
        local config = CPPGameLib.GetConfig("HuoBi", v.type)
        if v.value ~= 0 then
            table.insert(t1 , { id = config.big_icon , num = v.value })
        end
    end

    --道具列表
    for i,v in ipairs(self.setout.return_object_list) do
        if v.obj_num ~= 0 then
            table.insert(t1 , { id = v.obj_id , num = v.obj_num })
        end
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self.setout = {}
    self:RemoveAllOnData()
end

-- 分解返还请求
function Modules.RecycleModule:RecycleDecomposeReturnReq( info )
    CPPGameLib.PrintTable("xy",info,"分解返还请求")
    local info = self:GetOnData()
    if #info <= 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("recycle_meixuanfenjie"))
    else
        local t1 = {}
        for i,v in ipairs(info) do
            t1[i] = {}
            t1[i].uid = v.uid
        end
        local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECYCLE_DECOMPOSE_RETURN_REQ)
        data.obj_type = info[1].type
        data.target_list = t1
        GlobalNetAdapter:OnSend(data) 
    end
end

--分解返还响应
function Modules.RecycleModule:RecycleDecomposeReturnResp( info )
    CPPGameLib.PrintTable("xy",info,"分解返还响应")
    self.setout = info

    local cnf = self:GetOnData()
    local type_ , info_ = ObjIDTool.GetInfo(cnf[1].info_id)

    local text = {}
    text[Macros.Game.ObjType.EQUIP] = CPPGameLib.GetString("recycle_equip")
    text[Macros.Game.ObjType.HERO] = CPPGameLib.GetString("recycle_hero")
    text[Macros.Game.ObjType.TREASURE] = CPPGameLib.GetString("recycle_treasure")
    text[Macros.Game.ObjType.FAIRY] = CPPGameLib.GetString("divination_Card")

    local tmp = Modules.TemplResolveEquip.New(info,function ( ... )
        local data = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
        local t1 = {}
        for i,v in ipairs(data) do
            t1[i] = {}
            t1[i].uid = v.uid
        end
        local info_ = {}
        info_.obj_type = info.obj_type
        info_.target_list = t1
        if #t1 <= 0 then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("recycle_meixuanfenjie"))
        else
            self:RemoveAllOnData(info_)
        end
    end , CPPGameLib.GetString("recycle_yulan1") , text[type_]..CPPGameLib.GetString("recycle_heroTips1") )

end

--重生回收请求
function Modules.RecycleModule:RecycleResetReturnReq( info )
    local info = self:GetOnData()
     CPPGameLib.PrintTable("xy" , info , "请求物品列表")  
    if #info <= 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("recycle_meixuanchongsheng"))
    else
        local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RECYCLE_RESET_RETURN_REQ)
        data.obj_type = info[1].type
        data.target_uid = info[1].uid
        GlobalNetAdapter:OnSend(data)   
    end
end

--重生回收响应
function Modules.RecycleModule:RecycleResetReturnResp( info )
    CPPGameLib.PrintTable("xy",info,"重生回收响应")

    -- local data = {}
    -- data.obj_type = info.obj_type
    -- data.return_object_list = info.return_object_list
    -- data.return_currency_list = {}
    -- for i , v in ipairs( info.return_currency_list ) do
    --     table.insert( data.return_currency_list , { type = v.type , value = v.value } )
    -- end
    self.setout = info

   local tmp = Modules.TemplResolveEquip.New(info,function ( ... )
        local data = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
        local uid = nil
        for i,v in ipairs(data) do
            uid = v.uid
        end
        local info_ = {}
        info_.obj_type = info.obj_type
        info_.target_uid = uid

        self:ShowOnData(info_)
        -- CallModuleFunc(ModuleType.RECYCLE, "RecycleResetReq",info_)
    end)


    tmp:SetLoadedCallback( function ( ... )
        -- body
        --设置名字
        tmp:SetName( CPPGameLib.GetString("recycle_yulan2") )
        --设置消耗
        local cnf = self:GetOnData()
        local type_ , info_ = ObjIDTool.GetInfo(cnf[1].info_id)
        tmp:SetConsume( info_.reset_cost_diamond )

        local text = {}
        text[Macros.Game.ObjType.EQUIP] = CPPGameLib.GetString("recycle_equip")
        text[Macros.Game.ObjType.HERO] = CPPGameLib.GetString("recycle_hero")
        text[Macros.Game.ObjType.TREASURE] = CPPGameLib.GetString("recycle_treasure")
        text[Macros.Game.ObjType.FAIRY] = CPPGameLib.GetString("recycle_cardRebirth")

        tmp:SetTips(text[type_]..CPPGameLib.GetString("recycle_equipTips2") )
    end )

end

--物品的列表
function Modules.RecycleModule:DateLst( type , rebirth )
    local list = {}
    --装备
    -- 2705 装备分解那里的装备列表排序 还是按照品质分 是要按照品级分
    -- 按照 宿命→品级从高到低→同品级按精炼从高到低→同精炼按强化从高到低→同强化按ID从小到大排序
    if type == Macros.Game.ObjType.EQUIP then
        local t1 = {}
        t1 = CallModuleFunc(ModuleType.EQUIP, "GetEquipLit")
        for k,v in pairs(t1) do
            if v.equip_heroid == 0 then
                local info = CPPGameLib.GetConfig("Equipment", v.info_id)
                local data = CPPGameLib.GetConfig("EquipmentStrenthen", info.quality)
                table.insert(list, { 
                        info_id = v.info_id,
                        price = data.level_list[v.strengthen_level].cost_gold + info.price,
                        quality = info.quality,
                        name = info.name,
                        strengthen_level = v.strengthen_level,
                        refine_level = v.refine_level,
                        uid = v.uid,
                        taste = v.taste,
                        type = Macros.Game.ObjType.EQUIP,
                 })
            end
        end

        table.sort( list, function ( a , b )
            if a.taste < b.taste then
                return a.taste < b.taste
            elseif a.taste == b.taste then
                if a.refine_level < b.refine_level then
                    return a.refine_level < b.refine_level
                elseif  a.refine_level == b.refine_level then
                    if a.strengthen_level < b.strengthen_level then
                        return a.strengthen_level < b.strengthen_level
                    elseif a.strengthen_level == b.strengthen_level then
                        return a.info_id < b.info_id
                    end
                end
            end
        end )

    --英雄
    elseif type == Macros.Game.ObjType.HERO then
        local t2 = CallModuleFunc(ModuleType.HERO, "GetHeroInfoList")
        for k,v in pairs(t2) do
            local data = CPPGameLib.GetConfig("Hero", v.info_id)

            if v.is_to_battle == 0 and v.quality_level ~= 0 and 
                not v.is_reinforcement and data.type ~= Macros.Game.HeroType.EXP then
                local info = CPPGameLib.GetConfig("HeroLevel", v.quality_level)
                table.insert(list,{
                        info_id = v.info_id,
                        price = info.level_list[v.level].total_exp + v.exp + data.price,
                        break_lv = v.break_level,
                        awaken_star= v.awaken_star,
                        awakening_lv = v.awaken_level,
                        name = data.name,
                        quality = v.quality_level,
                        lv = v.level,
                        uid = v.uid,
                        type = Macros.Game.ObjType.HERO,
                        potential_level = v.potential_level,
                    })
            end
        end

        -- 2017年7月4日21:47:54 第四场修改
        -- 1981 英雄重生排序 按照品质》等级》进阶
        if rebirth then
            table.sort( list, function ( a , b )
                if a.quality > b.quality then
                    return a.quality > b.quality
                elseif a.quality == b.quality then
                    if a.lv > b.lv then
                        return a.lv > b.lv
                    elseif a.lv == b.lv then
                        if a.break_lv > b.break_lv then
                            return a.break_lv > b.break_lv
                        end
                    end
                end
            end )
        else
            table.sort( list, function ( a , b )
                if a.quality < b.quality then
                    return a.quality < b.quality
                elseif a.quality == b.quality then
                    if a.lv < b.lv then
                        return a.lv < b.lv
                    elseif a.lv == b.lv then
                        if a.break_lv > b.break_lv then
                            return a.break_lv > b.break_lv
                        end
                    end
                end
            end )
        end

    --圣器
    elseif type == Macros.Game.ObjType.TREASURE then 
        local t3 = CallModuleFunc(ModuleType.TREASURE, "GetAllTreasureLst")
        for k,v in pairs(t3) do

            if v.hero_infoid == 0 then
                local data = CPPGameLib.GetConfig("Treasure", v.info_id)
                table.insert(list,{
                        info_id = v.info_id,
                        strengthen_level = v.strengthen_level,
                        refine_level = v.refine_level,
                        name = data.name,
                        quality = data.quality,
                        uid = v.uid,
                        type = Macros.Game.ObjType.TREASURE,
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

    --卡牌
    elseif type == Macros.Game.ObjType.FAIRY then

        local t3 = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationLit")
        for k,v in pairs(t3) do
            local cnf = CPPGameLib.GetConfig("Card", v.info_id , false )
            if v.hero_id == 0 and cnf.is_can_decompose ~= 0 then
                table.insert(list,{
                        info_id = v.info_id,
                        strengthen_level = v.level,
                        refine_level = v.break_level,
                        name = v.name,
                        quality = v.quality,
                        uid = v.uid,
                        type = Macros.Game.ObjType.FAIRY,
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

    end
    return list
end

--添加选中
function Modules.RecycleModule:AddOnData( data )
    local t1 = {}
    for i,v in ipairs(self.resolveLst) do
        t1[v.uid or v.info_id] = true
    end

    if not t1[data.uid or data.info_id] then
        table.insert(self.resolveLst,data)
    end

    self:ShowOnData()
end

--删除选中
function Modules.RecycleModule:RemoveOnData( data )
    local t1 = self.resolveLst
    for i,v in ipairs(t1) do
        if v.uid == data.uid then
            table.remove(self.resolveLst,i)
            break
        end
    end
    self:ShowOnData()
end

--删除全部选中
function Modules.RecycleModule:RemoveAllOnData( type_ )

    -- print("skfb" , "删除全部选中============"  , debug.traceback())
    self.resolveLst = {}
    self:ShowOnData( type_ )
end

--显示选中
function Modules.RecycleModule:ShowOnData( type_ )
    local view = self:GetView()
    if view and view:IsOpen() then
        view:ShowOnData( type_ )
    end    
end

function Modules.RecycleModule:GetOnData( ... )
    return self.resolveLst
end


function Modules.RecycleModule:SetOnTab( int )
    self.onTab = int
end

function Modules.RecycleModule:GetOnTab( ... )
    return self.onTab
end



-------------------------------------------------------------------------------------------------------------------------------------
--页签计算
-------------------------------------------------------------------------------------------------------------------------------------
function Modules.RecycleModule:CountTab( ... )
    local t1 =  {
                    {{LogicGUI.TemplHeroResolve, "Panel_resolve"}, 0, Macros.Game.ObjType.HERO},
                    {{LogicGUI.TemplEquipResolve, "Panel_resolve"}, 0, Macros.Game.ObjType.EQUIP},
                    {{LogicGUI.TemplCardResolve, "Panel_resolve"}, 0, Macros.Game.ObjType.FAIRY},
                    {{LogicGUI.TemplHeroRebirthe, "Panel_rebirth"}, 1, Macros.Game.ObjType.HERO},
                    {{LogicGUI.TemplEquipRebirthe, "Panel_rebirth"}, 1, Macros.Game.ObjType.EQUIP},
                    {{LogicGUI.TemplTreasureRebirthe, "Panel_rebirth"}, 1, Macros.Game.ObjType.TREASURE},
                    {{LogicGUI.TemplCardRebirth, "Panel_rebirth"}, 1, Macros.Game.ObjType.FAIRY},
                }

    local t1_text = {
                    CPPGameLib.GetString("recycle_heroResolve"), 
                    CPPGameLib.GetString("recycle_equipResolve"),
                    CPPGameLib.GetString("recycle_cardResolve"),
                    CPPGameLib.GetString("recycle_heroRebirth"),
                    CPPGameLib.GetString("recycle_equipRebirth"),
                    CPPGameLib.GetString("recycle_treasureRebirth"),
                    CPPGameLib.GetString("recycle_cardRebirth"),
                }


    if not GlobalCustomizedLogicAdapterMgr:Call("CheckCardRecycleEnabled") then
        table.remove(t1)
        table.remove(t1_text)
    end

    local cnf = CPPGameLib.GetConfig("RecycleOpenLevel", nil , nil , true )
    local t2 = {}
    for i , v in ipairs( cnf ) do
        if t1[i] and t1_text[i] then
            table.insert( t2 , { lv = v.lv , log = t1[i][1] , text = t1_text[i] , tab_type = t1[i][2] , tab_obj_type = t1[i][3] } )
        end
    end

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local t3 = {}
    local t4 = {}
    self.__resolve_tab_index_map = {}
    self._rebirth_tab_index_map = {}
    for i,v in ipairs( t2 ) do
        if v.lv <= role_level then
            table.insert( t3 , v.log )
            table.insert( t4 , v.text )

            --生成实际选项卡索引
            if v.tab_type == 0 then
                --分解相关
                self.__resolve_tab_index_map[v.tab_obj_type] = #t3
            else
                --重生相关
                self._rebirth_tab_index_map[v.tab_obj_type] = #t3
            end
        end
    end
    return t3 , t4
end

--获取实际选项卡索引
function Modules.RecycleModule:GetTabIndex(obj_type, is_rebirth)
    if is_rebirth then
        return self._rebirth_tab_index_map and self._rebirth_tab_index_map[obj_type] or 1
    else
        return self.__resolve_tab_index_map and self.__resolve_tab_index_map[obj_type] or 1
    end
end

