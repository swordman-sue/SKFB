
--[[
%% @module: 热推商店
%% @author: ldx
%% @created: 2017年9月14日21:19:35
--]]

Modules = Modules or {}

Modules.DirectBuyHotRecommentTabview = Modules.DirectBuyHotRecommentTabview or BaseClass(BaseTabView)

function Modules.DirectBuyHotRecommentTabview:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)
end

--选项卡界面创建完成回调
function Modules.DirectBuyHotRecommentTabview:__LoadedCallback()
    self:__Getwidget()
    CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"AccurateRecommendDataReq")
end

--选项卡界面打开回调
function Modules.DirectBuyHotRecommentTabview:__OpenCallback()
    --商店购买记录
    self:BindGlobalEvent(Macros.Event.DirectBuyModule.DIRECT_HOT_RECOMMENT_SHOP_DATA_RESP,function (_, accurate_recommend_goods_list,is_req)
        self.__accurate_recommend_goods_list = accurate_recommend_goods_list
        if not is_req then
            for i,gift_info in ipairs(self.__shop_gift_list) do
                if self.__accurate_recommend_goods_list[gift_info.gift_id] then
                    gift_info.can_take_times = self.__accurate_recommend_goods_list[gift_info.gift_id].can_take_times
                    gift_info.buy_num = self.__accurate_recommend_goods_list[gift_info.gift_id].buy_times
                end
            end
            self.__hotrecomment_scrollView:SetDataList(self.__shop_gift_list)
        else
           self:__SetShowUI() 
        end        
    end)

    --领取精准推送商品奖励响应
    self:BindGlobalEvent(Macros.Event.DirectBuyModule.TAKE_ACCURATE_RECOMMEND_GIFT_RESP,function (_, goods_id)
        for i,gift_info in ipairs(self.__shop_gift_list) do
            if gift_info.gift_id == goods_id then
                gift_info.can_take_times = self.__accurate_recommend_goods_list[goods_id].can_take_times
                gift_info.buy_num = self.__accurate_recommend_goods_list[goods_id].buy_times
                break
            end
        end
        self.__hotrecomment_scrollView:SetDataList(self.__shop_gift_list)
    end)

    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_SUCCESS,function (_, recharge_type)
        if recharge_type == Macros.Game.RechargeType.ACCURATE_RECOMMEND then
            CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotRecommentBuyTag",0)
        end
    end)
    
    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_CANCEL,function (_, recharge_type)
        if recharge_type == Macros.Game.RechargeType.ACCURATE_RECOMMEND then
            CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotRecommentBuyTag",0)
        end
    end)

    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_FAILED,function (_,recharge_type)
        if recharge_type == Macros.Game.RechargeType.ACCURATE_RECOMMEND then
            CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotRecommentBuyTag",0)
        end
    end)

end

--选项卡界面关闭回调
function Modules.DirectBuyHotRecommentTabview:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.DirectBuyHotRecommentTabview:__Dispose()
    if self.__hotrecomment_scrollView then
        self.__hotrecomment_scrollView:DeleteMe()
        self.__hotrecomment_scrollView = nil
    end
end

--选项卡界面关闭回调
function Modules.DirectBuyHotRecommentTabview:__Getwidget()
    self.__grid_type_map = 
    {
        ["weapon_equipment_uid"] = Macros.Game.EquipType.WEAPON,
        ["hat_equipment_uid"] = Macros.Game.EquipType.HELMET,
        ["belt_equipment_uid"] = Macros.Game.EquipType.BELT,
        ["clothes_equipment_uid"] = Macros.Game.EquipType.CLOTH,
        ["attack_treasure_uid"] = Macros.Game.TreasureType.EMBLEM,
        ["defence_treasure_uid"] = Macros.Game.TreasureType.GAD,
    }
    self:RecommentItemList()
    --商品刷新时间背景
    self.__image_refrashtimebg = self:GetWidget("Image_RefrashTimeBg")
    self.__image_refrashtimebg:SetVisible(false)
    --商品刷新时间
    self.__text_refrashtime = self:GetWidget("Text_RefrashTime")
    Util:WidgetLabel(self.__text_refrashtime, "", Macros.TypefaceSize.largish, Macros.Color.Special, 0, 0)
    --活动时间
    self.__text_activitytime = self:GetWidget("Text_ActivityTime")
    local str = self:CallModuleFunc("GetDirectBuyShopOpen2") 
    local activitytime_str = CPPGameLib.GetString("direct_buy_time",str)
    Util:WidgetLabel(self.__text_activitytime,activitytime_str, Macros.TypefaceSize.normal, Macros.Color.btn,0, 0,Macros.Color.common_stroke)
    --列表容器    
    self.__panel_hotrecomment_scrollView = self:GetWidget("Panel_HotRecommentScrollView")

    --列表
    local params =  
    {
        item_class = Modules.DirectBuyHotRecommentItem,
        item_width = 208,
        item_height = 350,
        view_width = self.__panel_hotrecomment_scrollView:GetContentWidth(),
        row = 1,
        col = 4,
        horizon = true,
        item_space_c = 8,
    }
    -- 创建参数
    self.__hotrecomment_scrollView = GUI.ScrollView.New(params)
    self.__panel_hotrecomment_scrollView:AddChild(self.__hotrecomment_scrollView:GetNode())
    PosTool.Center(self.__hotrecomment_scrollView:GetNode())

end

function Modules.DirectBuyHotRecommentTabview:__SetShowUI()   
    --设置记录
    for cur_gift_id,gift_record_info in pairs(self.__accurate_recommend_goods_list) do
        local is_in_list
        for index,gift_info in ipairs(self.__shop_gift_list) do 
            if gift_info.gift_id == cur_gift_id then
                local gift_config = CPPGameLib.GetConfig("AccurateRecommendGift",cur_gift_id) 
                gift_info.buy_num = gift_record_info.buy_times
                gift_info.can_take_times = gift_record_info.can_take_times
                gift_info.is_reach_limit = gift_info.buy_num >= gift_config.buy_max_num 
                is_in_list = true
                break
            end                          
        end
        --插入数据
        if not is_in_list then
            local gift_info = CPPGameLib.GetConfig("AccurateRecommendGift",cur_gift_id)           
            local data = {}
            data.gift_id = cur_gift_id
            data.percent = 0 
            data.buy_num = gift_record_info.buy_times
            data.is_reach_limit = data.buy_num >= gift_info.buy_max_num  
            data.can_take_times = gift_record_info.can_take_times
            data.sort_weight = 0
            table.insert(self.__shop_gift_list,data)
        end
    end  
    --列表排序
    table.sort(self.__shop_gift_list,function (data1,data2)
        if data1.can_take_times ~= data2.can_take_times then
            return data1.can_take_times > data2.can_take_times
        else
            if data1.is_reach_limit and data2.is_reach_limit then
                return data1.gift_id < data2.gift_id
            elseif data1.is_reach_limit and not data2.is_reach_limit then
                return false
            elseif not data1.is_reach_limit and data2.is_reach_limit then
                return true
            else
                if data1.sort_weight ~= data2.sort_weight then
                    return data1.sort_weight > data2.sort_weight
                else
                    if data1.percent ~= data2.percent then
                        return data1.percent > data2.percent
                    else
                        return data1.gift_id < data2.gift_id
                    end
                end
            end
        end
    end)  
    --标记
    for i,gift_info in ipairs(self.__shop_gift_list) do
        gift_info.index = i
    end
    --保留前4个
    for i=1,#self.__shop_gift_list - 4 do
        table.remove(self.__shop_gift_list)
    end
    self.__hotrecomment_scrollView:SetDataList(self.__shop_gift_list)

    local actionType = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetActionType")
    if not actionType then
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetActionType" , true )
        self.__hotrecomment_scrollView:PlayFlyInAction( self.__panel_hotrecomment_scrollView:GetContentWidth() , 0 , 0.1 , 0.3 , function ( ... )
        end )
    end
end

--推荐物品队列
function Modules.DirectBuyHotRecommentTabview:RecommentItemList()  
    --角色等级
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    --先找符合等级的礼包
    local cur_recomment_info
    for i,recomment_info in ipairs(CPPGameLib.GetConfig("AccurateRecommend",nil,nil,true)) do
        if role_level >= recomment_info.lv_min and role_level <= recomment_info.lv_max then
            cur_recomment_info = recomment_info
            break
        end
    end   
    --找出最弱鸡的英雄
    local lineup_list = CallModuleFunc(ModuleType.HERO,"GetLineupList")
    local cur_battle_value,weak_index
    for i, lineup_info in ipairs(lineup_list) do
        if lineup_info.uid ~= 0 then
            --战力
            local hero_detail_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", i)
            local battle_value = CallModuleFunc(ModuleType.POWER, "CalcMonsterPowerEx",hero_detail_info)
            if not cur_battle_value or (cur_battle_value and battle_value < cur_battle_value) then
                weak_index = i
                cur_battle_value = battle_value
            end
        end
    end
    --英雄信息
    local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",lineup_list[weak_index].uid)
    --配置英雄信息
    local hero_config = CPPGameLib.GetConfig("Hero",hero_info.info_id)   
    --英雄队列里阵位的装备信息
    local tbl_all,tbl_equip,tbl_treasure,tbl_special_equip,tbl_special_treasure = CallModuleFunc(ModuleType.HERO,"GetLineupEquipInfo",weak_index)
    --构造推荐系统队列
    local recommended_list = {}
    for recommended_id,info in ipairs(CPPGameLib.GetConfig("Stronger", nil, nil , true )) do
        if hero_info.quality_level >= info.open_quality and role_level >= info.open_level then
            local data = {}
            --各项进度
            local cur_value,limit_value
            if info.system_id == Macros.Game.SystemID.HERO_UPGRADE then
                --升级
                cur_value = hero_info.level
                limit_value = role_level                
            elseif info.system_id == Macros.Game.SystemID.HERO_BREAK then
                --进阶
                cur_value = hero_info.break_level
                local break_info
                for i=0,table.maxn(CPPGameLib.GetConfig("HeroBreakLevelup",nil,nil,true)) do
                    local cur_info = CPPGameLib.GetConfig("HeroBreakLevelup",i)
                    if cur_info.need_hero_level > role_level then
                        break
                    end
                    break_info = cur_info
                end
                limit_value = break_info and (break_info.break_level + 1) or 0
                limit_value = math.min(limit_value,table.maxn(CPPGameLib.GetConfig("HeroBreakLevelup",nil,nil,true)))
            elseif info.system_id == Macros.Game.SystemID.HERO_TRAIN then
                --培养
                local train_least_level
                for i,train_info in ipairs(hero_info.train_pro_list) do
                    train_least_level = train_least_level or train_info.pro_level
                    if train_least_level > train_info.pro_level then
                        train_least_level = train_info.pro_level
                    end
                end    
                cur_value = train_least_level   
                local train_config = CPPGameLib.GetConfig("HeroTrain",1)
                local train_info
                for i=0,table.maxn(train_config.pro_level_list) do
                    local cur_info = train_config.pro_level_list[i]
                    if cur_info.need_hero_level > role_level then
                        break
                    end
                    train_info = cur_info
                end
                limit_value = train_info and (train_info.pro_level + 1) or 0
                limit_value = math.min(limit_value,table.maxn(train_config.pro_level_list))
            elseif info.system_id == Macros.Game.SystemID.HERO_POTENTIAL then
                --潜能
                cur_value = hero_info.potential_level    
                local potential_info
                for i=0,table.maxn(CPPGameLib.GetConfig("HeroDestiny",nil,nil,true)) do
                    local cur_info = CPPGameLib.GetConfig("HeroDestiny",i)
                    if cur_info.need_level > role_level then
                        break
                    end
                    potential_info = cur_info
                end
                limit_value = potential_info and (potential_info.level + 1) or 0
                limit_value = math.min(limit_value,table.maxn(CPPGameLib.GetConfig("HeroDestiny",nil,nil,true)))
            elseif info.system_id == Macros.Game.SystemID.HERO_AWAKEN then
                --觉醒
                local special_id = hero_config.init_quality * 10 + hero_config.profession
                local awaken_config = CPPGameLib.GetConfig("HeroAwakening", special_id)
                local outside_break
                local awaken_star = 0
                local awaken_level = 0
                for star=0,Macros.Game.HERO_AWAKE_STAR_MAX do
                    for level=0,Macros.Game.HERO_AWAKE_LEVEL_MAX do
                        if awaken_config[star][level] then
                            if awaken_config[star][level].need_hero_level > role_level then
                                outside_break = true
                                break
                            end
                            awaken_star = star
                            awaken_level = level
                        end
                    end
                    if outside_break then
                        break
                    end
                end
                cur_value = hero_info.awaken_star* 10 + hero_info.awaken_level
                limit_value = awaken_star* 10 + awaken_level
            elseif info.system_id == Macros.Game.SystemID.EQUIP_INTENSIFY then
                --装备强化
                data.cur_equip_grid_type = cur_equip_grid_type
                data.cur_equip_uid = cur_equip_uid
                local total_strengthen_level = 0
                for _,equip_info in pairs(tbl_equip) do
                    total_strengthen_level = total_strengthen_level + equip_info.strengthen_level
                end
                cur_value = math.floor(total_strengthen_level/4)            
                limit_value = role_level*2
            elseif info.system_id == Macros.Game.SystemID.EQUIP_REFINE then
                --装备精炼
                data.cur_equip_grid_type = cur_equip_grid_type
                data.cur_equip_uid = cur_equip_uid
                local total_refine_level = 0
                for _,equip_info in pairs(tbl_equip) do
                    total_refine_level = total_refine_level or 0
                    total_refine_level = total_refine_level + equip_info.refine_level
                end
                cur_value = math.floor(total_refine_level/4) 
                local config_equip_info = CPPGameLib.GetConfig("EquipmentRefine",1)
                local equip_refine_info
                for i=0,table.maxn(config_equip_info.level_list) do
                    local cur_info = config_equip_info.level_list[i]
                    if cur_info.need_level > role_level then
                        break
                    end
                    equip_refine_info = cur_info
                end
                limit_value = equip_refine_info and (equip_refine_info.refine_level + 1) or 0
                limit_value = math.min(limit_value,table.maxn(config_equip_info.level_list))
            elseif info.system_id == Macros.Game.SystemID.TREASURE_INTENSIFY then
                --圣器强化
                data.cur_treasure_grid_type = cur_treasure_grid_type
                data.cur_treasure_uid = cur_treasure_uid
                local total_strengthen_level = 0
                for _,treasure_info in pairs(tbl_treasure) do
                    total_strengthen_level = total_strengthen_level or 0
                    total_strengthen_level = total_strengthen_level + treasure_info.strengthen_level
                end
                cur_value = math.floor(total_strengthen_level/2)    
                local config_treasure_info = CPPGameLib.GetConfig("TreasureStrengthen",2)
                local treaure_strengthen_info
                for i,cur_info in ipairs(config_treasure_info.level_list) do
                    if cur_info.need_level > role_level then
                        break
                    end
                    treaure_strengthen_info = cur_info
                end         
                limit_value = treaure_strengthen_info and (treaure_strengthen_info.strengthen_level + 1) or 0
                limit_value = math.min(limit_value,#config_treasure_info.level_list)
            elseif info.system_id == Macros.Game.SystemID.TREASURE_REFINE then
                --圣器精炼
                data.cur_treasure_grid_type = cur_treasure_grid_type
                data.cur_treasure_uid = cur_treasure_uid
                local total_refine_level = 0
                for _,treasure_info in pairs(tbl_treasure) do
                    total_refine_level = total_refine_level or 0
                    total_refine_level = total_refine_level + treasure_info.refine_level
                end
                cur_value = math.floor(total_refine_level/2)    
                local config_treasure_info = CPPGameLib.GetConfig("TreasureRefine",2)
                local treaure_refine_info
                for i=0,table.maxn(config_treasure_info.level_list) do
                    local cur_info = config_treasure_info.level_list[i]
                    if cur_info.need_level > role_level then
                        break
                    end
                    treaure_refine_info = cur_info
                end         
                limit_value = treaure_refine_info and (treaure_refine_info.refine_level + 1) or 0
                limit_value = math.min(limit_value,table.maxn(config_treasure_info.level_list))
            elseif info.system_id == Macros.Game.SystemID.SPIRIT_ENHANCE then
                --精灵强化
                cur_value = 0 
                limit_value = 0
                local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
                if spirit_data.cur_use_spirit ~= 0 then
                    if spirit_data.spirit_info_map[spirit_data.cur_use_spirit] then
                        cur_value = spirit_data.spirit_info_map[spirit_data.cur_use_spirit].enhance_level
                    end
                    local config_spirit_info = CPPGameLib.GetConfig("SpiritEnhace",spirit_data.cur_use_spirit)
                    local spirit_enhance_info
                    for i=0,table.maxn(config_spirit_info.enhance_list) do
                        local cur_info = config_spirit_info.enhance_list[i]
                        if cur_info.need_level > role_level then
                            break
                        end
                        spirit_enhance_info = cur_info
                    end
                    limit_value = spirit_enhance_info.enhance_level
                end               
            elseif info.system_id == Macros.Game.SystemID.SPIRIT_SUBLIMATE then
                --精灵升华
                cur_value = 0 
                limit_value = 1
                local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
                if spirit_data.cur_use_spirit ~= 0 then
                    if spirit_data.spirit_info_map[spirit_data.cur_use_spirit] then
                        cur_value = spirit_data.spirit_info_map[spirit_data.cur_use_spirit].train_stage
                    end
                    local config_spirit_enhance_info = CPPGameLib.GetConfig("SpiritEnhace",spirit_data.cur_use_spirit)
                    local spirit_enhance_info
                    for i=0,table.maxn(config_spirit_enhance_info.enhance_list) do
                        local cur_info = config_spirit_enhance_info.enhance_list[i]
                        if cur_info.need_level > role_level then
                            break
                        end
                        spirit_enhance_info = cur_info
                    end
                    local limit_enhance_level = spirit_enhance_info.enhance_level
                    local config_spirit_sublimate_info = CPPGameLib.GetConfig("SpriteTrain",spirit_data.cur_use_spirit)
                    local spirit_sublimate_info
                    for i=1,table.maxn(config_spirit_sublimate_info.stage_list) do
                        local cur_info = config_spirit_sublimate_info.stage_list[i]
                        if cur_info.need_enhance_level > limit_enhance_level then
                            break
                        end
                        spirit_sublimate_info = cur_info
                    end
                    limit_value = spirit_sublimate_info.stage
                end                
            end        
            data.percent = limit_value == 0 and 0 or (cur_value/limit_value)
            data.percent = math.min(data.percent,1)
            data.limit_value = limit_value
            recommended_list[info.system_id] = data
        end
    end
    self.__shop_gift_list = {}
    --礼包数据
    for i,gift_id in ipairs(cur_recomment_info.gift_list) do
        --礼包信息
        local gift_info = CPPGameLib.GetConfig("AccurateRecommendGift",gift_id)
        local data = {}
        data.gift_id = gift_id
        data.percent = 0
        data.buy_num = 0
        data.is_reach_limit = data.buy_num >= gift_info.buy_max_num    
        data.can_take_times = 0
        if recommended_list[gift_info.system_id] then
            if recommended_list[gift_info.system_id].limit_value ~= 0 then
                data.percent = 1 - recommended_list[gift_info.system_id].percent
            end
        end
        data.sort_weight = gift_info.system_id and gift_info.weight * data.percent or MathTool.MAX_NUM
        table.insert(self.__shop_gift_list,data)
    end 
end

--------------------------------------------------------------------------------------------------------------------------------
--热荐特惠商品模板
--------------------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

--副本宝箱信息模板
Modules.DirectBuyHotRecommentItem = Modules.DirectBuyHotRecommentItem or BaseClass(GUI.ScrollItem)

function Modules.DirectBuyHotRecommentItem:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)

    self.__button_diamond_buy = Util:WidgetButton(self:GetWidget("Button_Diamond_Buy"), "", function()
        self:__HandleDiamondBuy()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
    self.__button_diamond_buy:SetVisible(false)

    self.__button_money_buy = Util:WidgetButton(self:GetWidget("Button_Money_Buy"), "", function()        
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
    self.__button_money_buy:SetVisible(false)
   
    self.__button_get = Util:WidgetButton(self:GetWidget("Button_Get"),"" , function()
        self:__HandleJudgeWhich()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
    self.__button_get:SetNormalImage(sp)

    self.__text_goodname = Util:WidgetLabel(
                                                self:GetWidget("Text_GoodName"), "", 
                                                Macros.TypefaceSize.popup , 
                                                Macros.Color.keypoint
                                            )
    self.__text_limitnum = Util:WidgetLabel(self:GetWidget("Text_LimitNum"), "", Macros.TypefaceSize.slightly , Macros.Color.keypoint )

    self.__panel_goodicon = self:GetWidget("Panel_GoodIcon")
    self.__image_sellout = self:GetWidget("Image_Sellout")
end

function Modules.DirectBuyHotRecommentItem:__delete()
    if self.__button_zoubian_effectid then
        self:__DelEffect(self.__button_zoubian_effectid)
        self.__button_zoubian_effectid = nil
    end
end

function Modules.DirectBuyHotRecommentItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local config_shop_goods = CPPGameLib.GetConfig("AccurateRecommendGift", self.__data.gift_id)
    if not config_shop_goods then
        return
    end
    local _ , config_item = ObjIDTool.GetInfo( config_shop_goods.item_id )
    if not config_item then
        return
    end

    --折扣图标
    local function handle_discount(btn,num,str,offset_x,offset_y)
        if num then
            local icon_discount = LogicGUI.LconDiscount.New( num,str)
            icon_discount:SetAnchorPoint(0.5, 0.5)
            btn:AddChild(icon_discount:GetNode() , 999 )
            PosTool.LeftTop(icon_discount , offset_x and offset_x or -0 ,offset_y and offset_y or 0 )
            icon_discount:SetScaleFactor( 0.9 )
        end
    end

    local is_sellout = self.__data.buy_num >= config_shop_goods.buy_max_num

    --商品名称、限购、icon
    self.__text_goodname:SetText(config_item.name)
    self.__text_limitnum:SetText(CPPGameLib.GetString("direct_buy_shop_limit_buy", config_shop_goods.buy_max_num - self.__data.buy_num, config_shop_goods.buy_max_num))

    local icon = LogicGUI.IconLayer.New()
    icon:SetData({id = config_shop_goods.item_id , num = config_shop_goods.item_num })
    self.__panel_goodicon:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())
    --添加标签
    handle_discount(self.__panel_goodicon,self.__data.index <= 3 and 6 or 7,self.__data.index <= 3 and CPPGameLib.GetString("direct_buy_hot_need")
             or CPPGameLib.GetString("direct_buy_need"),-2,7)

    --转圈
    local effectid, effect = self:__CreateEffect({res_name = "UI_banjia",loop = true}, icon:GetNode() ,2)
    PosTool.Center(effect)

    if not is_sellout and not self.__lab_cost then
        --充值购买按钮
        local config_recharge = CPPGameLib.GetConfig("Recharge", config_shop_goods.recharge_id, false)
        if config_recharge then

            handle_discount(self.__button_get , config_shop_goods.discount_rmb )
            --删除字体
            -- self.__lab_cost = Util:LabCenter({
            -- {tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "rmb") , dx = 1 },
            -- {zi = tostring(config_recharge.money), ys = 101, dx = Macros.TypefaceSize.largish, jg = 8 , mb = true },
            -- })            
            -- self.__lab_cost:SetAnchorPoint(0.5, 0.5)
            -- self.__button_get:AddChild(self.__lab_cost)
            -- PosTool.Center(self.__lab_cost)     
            self:Currency( config_shop_goods.recharge_id )     
        end
    end

    if is_sellout and self.__data.can_take_times == 0 then
        self.__image_sellout:SetVisible(is_sellout)
        self.__button_get:SetVisible(false)
    else
        self.__image_sellout:SetVisible(false)
    end    
    self.__button_get:SetText(self.__data.can_take_times > 0 and CPPGameLib.GetString("dungeon_receive") or "")  
    if self.__data.can_take_times > 0 then
        if self.__lab_cost then
            self.__lab_cost:SetVisible(false)
        end
        self.__button_zoubian_effectid,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_get)
        PosTool.Center(effect)
    end  
end

--判断选择
function Modules.DirectBuyHotRecommentItem:__HandleJudgeWhich()
    if self.__data.can_take_times > 0 then
        self:__HandleGet()
    else
        self:__HandleMoneyBuy()
    end
end

--领取
function Modules.DirectBuyHotRecommentItem:__HandleGet()
    CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"TakeAccurateRecommendGiftReq" , self.__data.gift_id )
end

--充值购买
function Modules.DirectBuyHotRecommentItem:__HandleMoneyBuy()
    local cur_info,open_info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen2" )
    if open_info.isOpen ~= "yes" then
        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES,
                                                        content = CPPGameLib.GetString("direct_buy_timeEnd"),
                                                        confirm_func = function ( ... )
                                                            CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"CloseView"  )
                                                        end,
                                                    } )
        return
    end
    --检查标记
    local time = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotRecommentBuyTag")
    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if time ~= 0 and math.abs( time - n_Time ) < 20 then       
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_CD" , 20 - (n_Time - time) .. "s" ))
        return
    end
    local config_shop_goods = CPPGameLib.GetConfig("AccurateRecommendGift", self.__data.gift_id)
    CallModuleFunc(ModuleType.CHARGE , "RechargeOrderIDReq" , Macros.Game.RechargeType.ACCURATE_RECOMMEND , {recharge_id = config_shop_goods.recharge_id, goods_id = self.__data.gift_id})
    


    CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotRecommentBuyTag", n_Time )
end

function Modules.DirectBuyHotRecommentItem:Currency( info )
    if self.diamond then
        self.diamond:RemoveSelf(true)
        self.diamond = nil

        for i = 1 , #self.diamondLab do
            self.diamondLab[i]:RemoveSelf(true)
            self.diamondLab[i] = nil
        end
    end

    self.__button_get:SetText("")
    if type( info ) == "number" then
        local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , info )
        --钻石
        self.diamond , self.diamondLab = Util:LabCenter({
            -- {zi = CPPGameLib.GetString("monthCard_rechargeTips_1") , mb = true , ys = 101 , dx = Macros.TypefaceSize.normal , jg = 4},
            -- {tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.7 , jg = 4},
            {zi = text , mb = true , ys = 101 , dx = Macros.TypefaceSize.normal},
        })  
    else
        --钻石
        self.diamond , self.diamondLab = Util:LabCenter({
            {zi = info , mb = true , ys = 101 , dx = Macros.TypefaceSize.normal},
        })  
    end

    self.diamond:SetAnchorPoint( 0.5 , 0.5 )
    self.__button_get:AddChild( self.diamond )
    PosTool.Center( self.diamond )
end
