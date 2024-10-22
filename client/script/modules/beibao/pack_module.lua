--
-- @brief 物品背包模块 
-- @author: yjg
-- @date: 2016年8月9日17:23:47
--

Modules = Modules or {}

Modules.PackModule = Modules.PackModule or BaseClass(BaseModule)

function Modules.PackModule:__init()
    self.commonGoods = {}
    self.awakenGoods = {}

    self.equipDebris = {}
    self.heroDebris = {}
end

function Modules.PackModule:__delete()
end
   
-- 请求物品列表
function Modules.PackModule:ItemListReq( info )
    GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ITEM_LIST_REQ))    
end

--物品列表
function Modules.PackModule:ItemListResp( info )
    --初始化物品列表
    for i,v in ipairs(info.item_list) do
        v.flag = 1
        v.use_num = v.item_num
        self:ItemDataNotify(v, true)
    end

    --初始化新增记录表
    local lst = CPPGameLib.SplitEx( Cookies.Get( Cookies.LevelType.ROLE ,Macros.Setting.NewPack) , "<GH>")
    for k , v in pairs( lst ) do
        self:AddToNewRecordMap( tonumber(v) , true )
    end
end

-- 物品更新
function Modules.PackModule:ItemDataNotify( info , for_init )
    --更新普通道具
    self:UpdateCommonGoods( info, for_init )
    
    local obj_type , config_info = ObjIDTool.GetInfo( info.item_id )
    if obj_type == Macros.Game.ObjType.ITEM then        
        if config_info and config_info.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS then
            --更新装备碎片
            self:UpdateEquipDebris( info, for_init )

        elseif config_info and config_info.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
            --更新英雄碎片
            self:UpdateHeroDebris( info, for_init )

        elseif config_info and config_info.sub_type == Macros.Game.NormalItemSubType.TREASURE_DEBRIS then    
            --更新圣器碎片
        end

    elseif obj_type == Macros.Game.ObjType.AWAKEN_TOOL then
        --更新觉醒道具
        self:UpdateAwakenGoods( info, for_init )
    end

    --处理物品改变导致的提示(红点....)
    if not for_init and info.use_num > 0 then
        info.item_num = info.use_num
        self:__HandleItemNotify(info)
    end    
end

--翻牌请求
function Modules.PackModule:ChooseCardReq(fp_type)
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHOOSE_CARD_REQ)
    protocol.fp_type = fp_type
    GlobalNetAdapter:OnSend(protocol)
end

--翻牌请求响应
function Modules.PackModule:ChooseCardResp(info )
    self:FireNextFrame(Macros.Event.PackModule.GET_CHOOSECARD_DATA,info)
end

--物品合成请求
function Modules.PackModule:ItemComposeReq(item_id)
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ITEM_COMPOSE_REQ)
    protocol.item_id = item_id
    GlobalNetAdapter:OnSend(protocol)
end

--物品合成响应
function Modules.PackModule:ItemComposeResp(data)
    GlobalEventSystem:FireNextFrame(Macros.Event.HeroModule.HERO_AWAKEN_PROP_COMPOSE_RESP,data.item_id)
end

--物品分解请求
function Modules.PackModule:ItemDecomposeReq(info)
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ITEM_DECOMPOSE_REQ)
    protocol.item_id = info.item_id
    protocol.item_num = info.item_num
    GlobalNetAdapter:OnSend(protocol)   
end

--物品分解响应
function Modules.PackModule:ItemDecomposeResp(info )
    local type,item_info = ObjIDTool.GetInfo(info.item_id)
    --觉醒道具
    if type == Macros.Game.ObjType.AWAKEN_TOOL then
        local t1 = {}
        table.insert(t1,{type = Macros.Game.RolePropType.GODSOUL , num = item_info.god_soul * info.item_num})
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    end
end

--发送使用
function Modules.PackModule:ItemUseReq( info )
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ITEM_USE_REQ)
    data.item_id = info.item_id
    data.use_num = info.item_num
    data.select_gift_id = info.select_gift_id
    GlobalNetAdapter:OnSend(data)

    --设置礼包使用中标记
    local config_item = CPPGameLib.GetConfig("Item", info.item_id)    
    if config_item and config_item.type == Macros.Game.ItemBigType.GIFT then
        self.__is_using_gift = true
    end
end

--使用获得
function Modules.PackModule:ItemUseResp( info )
    local cnf = CPPGameLib.GetConfig("Item", info.item_id)
    if not cnf then return end

    if info.item_id == Macros.Game.SpecialItem.MUSCLE  then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("pack_getStamina",cnf.param1 * info.use_num))
    elseif info.item_id == Macros.Game.SpecialItem.LOW_AVOIDWAR or info.item_id == Macros.Game.SpecialItem.HIGH_AVOIDWAR then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_UseAvoidWarItemSuccess"))
    elseif info.item_id == Macros.Game.SpecialItem.VIGOR then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("pack_getVigor",cnf.param1 * info.use_num) )
    elseif info.item_id == Macros.Game.SpecialItem.WANTED then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("pack_manted",cnf.param1 * info.use_num) )
    elseif cnf.type == Macros.Game.ItemBigType.GIFT and info.use_num ~= 1 and cnf.sub_type ~= Macros.Game.GiftItemSubType.MCQ then
        local t1 = {}
        for i,v in ipairs(info.item_list) do
            table.insert(t1,{id = v.item_id , num = v.item_num})
        end

        for i,v in ipairs(info.money_list) do
            table.insert(t1,{type = v.type , num = v.value})
        end
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS_1 , false , t1 )
    else
        local t1 = {}
        for i,v in ipairs(info.money_list) do
            table.insert(t1,{type = v.type , num = v.value})
        end
        for i,v in ipairs(info.item_list) do
            table.insert(t1,{id = v.item_id , num = v.item_num})
        end
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    end

    --移除礼包使用中标记
    if cnf.type == Macros.Game.ItemBigType.GIFT then
        self.__is_using_gift = false
    end

    --处理物品改变导致的提示(红点....)
    for i,v in ipairs( info.item_list ) do
        self:__HandleItemNotify(v)
    end

    GlobalEventSystem:FireNextFrame(Macros.Event.PackModule.USE_ITEM_SUCCESS, info)
end

--添加到新增记录表
function Modules.PackModule:AddToNewRecordMap( id , ignore_save )   
    self.__new_record_map = self.__new_record_map or {}

    local item_config = CPPGameLib.GetConfig("Item", id , false)
    if item_config and item_config.new then
        self.__new_record_map[id] = self.__new_record_map[id] or {}
        self.__new_record_map[id].type = true

        --保存
        if not ignore_save then
            self:SaveNewRecordMap()
        end
    end
end

--获得新增记录
function Modules.PackModule:GetNewRecord( id )   
    if not self.__new_record_map then
        return
    end

    if id then
        return self.__new_record_map[id]
    end
end

--获取新增记录表
function Modules.PackModule:GetNewRecordMap()
    return self.__new_record_map
end

--从新增记录表中删除
function Modules.PackModule:DelFrmNewRecordMap( id , ignore_save )  
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

--保存新增记录表到文件
function Modules.PackModule:SaveNewRecordMap( )   
    if not self.__new_record_map then
        return
    end

    local text = ""
    for k , v in pairs( self.__new_record_map ) do
        text = text .. k .. "<GH>"
    end
    Cookies.Set( Cookies.LevelType.ROLE , Macros.Setting.NewPack , text )

    self:FireNextFrame(Macros.Event.NewModule.PACK_EXTEND, Macros.Game.SystemID.Pack)
end

--是否使用礼包中
function Modules.PackModule:IsUsingGift()
    return self.__is_using_gift
end

--处理物品改变导致的提示(红点....)
function Modules.PackModule:__HandleItemNotify( data )
    if not data then
        return
    end

    if self.__is_using_gift then
        return
    end

    local item_config = CPPGameLib.GetConfig("Item", data.item_id, false)
    if not item_config then return end
    
    --如果是需要在主城推送使用的物品
    if item_config.is_pop_maincity then
        CallModuleFunc(ModuleType.MAIN_CITY, "InsertPopUseData", data)
    end

    --世界之心契约
    if data.item_id == 10020 then
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.WORLD_HEART)

    --英雄进阶材料
    elseif data.item_id == ConfigAdapter.Common.GetHeroBreakPropId() then
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.HERO_LINEUP)

    --装备精炼材料
    elseif item_config.sub_type == Macros.Game.NormalItemSubType.REFINE then   
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.EQUIP_DRESS)

    --卡牌进阶石
    elseif data.item_id == ConfigAdapter.Common.GetDivinationBreakPropId() then  
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.DIVINATION_DRESS)

    --圣器碎片
    elseif item_config.sub_type == Macros.Game.NormalItemSubType.TREASURE_DEBRIS then   
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.PLUNDEREDTREASURES)

    --精灵培养材料
    elseif item_config.sub_type == Macros.Game.NormalItemSubType.SPIRIT_TRAIN then   
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.SPIRIT_SUBLIMATE, true)
 
    --精灵升华材料
    elseif item_config.sub_type == Macros.Game.NormalItemSubType.SPIRIT_SUBLIMATE then   
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.SPIRIT_SUBLIMATE)
  
    --精灵强化材料
    elseif item_config.sub_type == Macros.Game.NormalItemSubType.SPIRIT_INTENSIFY then   
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.SPIRIT_ENHANCE)
    
    --雪球
    elseif CPPGameLib.GetConfig("ChristmasSnowBall", data.item_id, false) then
        --统计红点
        CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.CHRISTMAS_ACTIVITY)
    end
end
