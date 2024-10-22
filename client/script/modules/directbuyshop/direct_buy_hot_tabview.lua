--[[
%% @module: 活动热卖
%% @author: yjg
%% @created: 2018年2月28日15:19:14
--]]

Modules = Modules or {}

Modules.DirectBuyHotTabView = Modules.DirectBuyHotTabView or BaseClass(BaseTabView)

function Modules.DirectBuyHotTabView:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)
end

--选项卡界面创建完成回调
function Modules.DirectBuyHotTabView:__LoadedCallback()
    -- local widget = self:GetWidget("Panel_ScrollView")
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.DirectBuyHotTabView:__OpenCallback()
    self.initial = true

    CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"HotSaleGoodsDataReq")
    
    --商店数据
    self:BindGlobalEvent(Macros.Event.DirectBuyModule.HOT_SALE_GOODS_DATA_RESP,function (_, data)
        self:__SetShowUI( data )
    end)

    -- --商店数据
    -- self:BindGlobalEvent(Macros.Event.DirectBuyModule.TAKE_DIRECT_BUY_GOODS_RESP,function (_, data)
    --     CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"DirectShopDataReq" , true )
    -- end)

    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_SUCCESS,function ( _ , type_ )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotSaleBuyTag",0)
    end)
    
    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_CANCEL,function ( _ , type_ )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotSaleBuyTag",0)
    end)

    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_FAILED,function ( _ , type_ )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetHotSaleBuyTag",0)
    end)

end

--选项卡界面关闭回调
function Modules.DirectBuyHotTabView:__CloseCallback()
    -- if self.__scv_goods_list then
    --     self.__scv_goods_list:DeleteMe()
    --     self.__scv_goods_list = nil
    -- end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.DirectBuyHotTabView:__Dispose()
    if self.__scv_goods_list then
        self.__scv_goods_list:DeleteMe()
        self.__scv_goods_list = nil
    end
end

--选项卡界面关闭回调
function Modules.DirectBuyHotTabView:__Getwidget()
    self.Panel_DirectHotTabScrollView = self:GetWidget("Panel_DirectHotTabScrollView")
    self.Image_1 = self:GetWidget("Image_1")
end

--选项卡界面关闭回调
function Modules.DirectBuyHotTabView:__ShowUI()
    --列表
    local params =  
    {
        item_class = Modules.TemplDirectHotSaleItem,
        item_width = 208,
        item_height = 350,
        view_width = self.Panel_DirectHotTabScrollView:GetContentWidth(),
        row = 1,
        col = 4,
        horizon = true,
        item_space_c = 8,
    } 
    -- 创建参数
    self.__scv_goods_list = GUI.ScrollView.New(params)
    self.Panel_DirectHotTabScrollView:AddChild(self.__scv_goods_list:GetNode())
    PosTool.Center(self.__scv_goods_list:GetNode())

    -- --时间列表
    local info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSaleActivityTime" )

    local star = TimeTool.GetTimeFormat( info.star , TimeTool.defaul_format[19] )
    local over = TimeTool.GetTimeFormat( info.over , TimeTool.defaul_format[19] )

    local direct_buy_time = Util:Name( CPPGameLib.GetString("direct_buy_time" ,  star.."~"..over ) )
    direct_buy_time:SetAnchorPoint( 1 , 0.5 )
    direct_buy_time:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() )
    self.__root_widget:AddChild( direct_buy_time )

end

function Modules.DirectBuyHotTabView:__SetShowUI()
    --创建列表
    local goods_list = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSaleGoodsCnf")    
  
    if self.initial == true then
        self.initial = false
        self.__scv_goods_list:SetDataList( goods_list )
        self.__scv_goods_list:PlayFlyInAction( self.Panel_DirectHotTabScrollView:GetContentWidth() , 0 , 0.1 , 0.3 , function ( ... )
        end )
    else
        self.__scv_goods_list:ItemDataIter(function(i, item_data , item )
                                                if item then
                                                    item:SetData( item_data )
                                                end
                                            end)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--直购特惠商品模板
--------------------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

--副本宝箱信息模板
Modules.TemplDirectHotSaleItem = Modules.TemplDirectHotSaleItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDirectHotSaleItem:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)

    self.__button_diamond_buy = Util:WidgetButton(self:GetWidget("Button_Diamond_Buy"), "", function()
        self:__HandleDiamondBuy()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)

    self.__button_money_buy = Util:WidgetButton(self:GetWidget("Button_Money_Buy"), "", function()
        self:__HandleMoneyBuy()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)

    self.Button_Get = Util:WidgetButton(self:GetWidget("Button_Get"), CPPGameLib.GetString("dungeon_receive") , function()
        self:__HandleGet()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
    self.Button_Get:SetVisible(false)

    local btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.Button_Get , 999)
    BtnEffect_con:SetAnchorPoint(0.5,0.5)
    PosTool.Center( BtnEffect_con )

    self.__text_goodname = Util:WidgetLabel(
                                                self:GetWidget("Text_GoodName"), "", 
                                                Macros.TypefaceSize.popup ,
                                                Macros.Color.keypoint
                                            )
    self.__text_limitnum = Util:WidgetLabel(self:GetWidget("Text_LimitNum"), "", Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    self.__text_limitnum:SetPosition( self.__text_limitnum:GetPositionX() , 159 )
    self.__panel_goodicon = self:GetWidget("Panel_GoodIcon")
    self.__image_sellout = self:GetWidget("Image_Sellout")
end

function Modules.TemplDirectHotSaleItem:__delete()
end

function Modules.TemplDirectHotSaleItem:SetData(info)
    GUI.ScrollItem.SetData(self, info)

    local data = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSaleGoodsCnf" , info.goods_id ) 
    local _ , config_item = ObjIDTool.GetInfo( data.item_id )
    if not config_item then
        return
    end

    --商品名称、限购、icon
    self.__text_goodname:SetText(config_item.name)
    self.__text_limitnum:SetText(CPPGameLib.GetString("direct_buy_shop_limit_buy", data.buy_num_max - data.pick , data.buy_num_max ))

    if not self.icon then
        self.icon = LogicGUI.IconLayer.New()
        self.icon:SetData({id = data.item_id , num = data.item_num })
        self.__panel_goodicon:AddChild(self.icon:GetNode())
        PosTool.Center(self.icon:GetNode())

        --转圈
        local effectid, effect = self:__CreateEffect({res_name = "UI_banjia",loop = true}, self.icon :GetNode() ,2)
        PosTool.Center(effect)
    else
        self.icon:SetData({id = data.item_id , num = data.item_num })
    end

    --折扣图标
    local function handle_discount(btn , num)
        if num then
            -- if self.icon_discount then
            --     self.icon_discount:RemoveSelf(true)
            --     self.icon_discount = nil
            -- end

            local icon_discount = LogicGUI.LconDiscount.New( num )
            icon_discount:SetAnchorPoint(0.5, 0.5)
            btn:AddChild(icon_discount:GetNode() , 999 )
            PosTool.LeftTop(icon_discount , 0 , 0)
            icon_discount:SetScaleFactor( 0.9 )
        end
    end

    local is_sellout = data.pick >= data.buy_num_max

    local is_diamond_buy, is_money_buy

    if not is_sellout then
        --钻石购买按钮
        local cost_type = data.cost_type
        local cost_value = data.cost_value
        if cost_type and cost_value then
            is_diamond_buy = true

            handle_discount(self.__button_diamond_buy , data.diamond )
            if self.lab_cost then
                self.lab_cost:RemoveSelf(true)
                self.lab_cost = nil
            end

            self.lab_cost = Util:LabCenter({
                {tp = Util:GetMoneyIconImgData(cost_type), dx = 0.8},
                {zi = tostring(cost_value), ys = 101, dx = Macros.TypefaceSize.largish, jg = 8 , mb = true },
            })            
            self.lab_cost:SetAnchorPoint(0.5, 0.5)  
            self.__button_diamond_buy:AddChild(self.lab_cost)
            PosTool.Center(self.lab_cost)
        end

        --充值购买按钮
        local config_recharge = CPPGameLib.GetConfig("Recharge", data.recharge_id, false)
        if config_recharge then
            is_money_buy = true

            handle_discount(self.__button_money_buy , data.recharge )
            self:Currency( data.recharge_id )
        end
    end

    -- self.__button_diamond_buy:SetVisible(not is_sellout and is_diamond_buy)
    -- self.__button_money_buy:SetVisible(not is_sellout and is_money_buy)

    -- self.__image_sellout:SetVisible(is_sellout)

    CPPGameLib.PrintTable("xy" , is_sellout , "is_sell/////////////////out")  
    if is_sellout == true then
        self:SetResetTime( data )
    end

    if is_money_buy then
        self.__button_diamond_buy:SetPosition( 102.00 , 104.00 )
    else
        self.__button_diamond_buy:SetPosition( 102.00 , 74.00 )
    end

    if is_diamond_buy then
        self.__button_money_buy:SetPosition( 102.00 , 45.00 )
    else
        self.__button_money_buy:SetPosition( 102.00 , 74.00 )
    end

    self:SetNoMay( data )
end

--设置可以领取
function Modules.TemplDirectHotSaleItem:SetNoMay( data )
    self.Button_Get:SetVisible(false)

    --领取次数
    local take = CallModuleFunc( ModuleType.DIRECT_BUY_SHOP , "GetHotSaleTakeTimes", data.goods_id )
    --购买次数
    local pick = CallModuleFunc( ModuleType.DIRECT_BUY_SHOP , "GetHotSaleGoodsPick", data.goods_id )

    local is_sellout = data.pick >= data.buy_num_max
    local is_diamond_buy, is_money_buy
    if not is_sellout then
        --钻石购买按钮
        local cost_type = data.cost_type
        local cost_value = data.cost_value
        if cost_type and cost_value then
            is_diamond_buy = true
        end

        --充值购买按钮
        local config_recharge = CPPGameLib.GetConfig("Recharge", data.recharge_id, false)
        if config_recharge then
            is_money_buy = true
        end
    end

    self.__button_diamond_buy:SetVisible(not is_sellout and is_diamond_buy)
    self.__button_money_buy:SetVisible(not is_sellout and is_money_buy)
    self.__image_sellout:SetVisible(is_sellout)


    if take ~= pick then
        self.__button_diamond_buy:SetVisible(false)
        self.__button_money_buy:SetVisible(false)
        self.__image_sellout:SetVisible(false)
        self.Button_Get:SetVisible(true)
    end

end

--设置重置时间
function Modules.TemplDirectHotSaleItem:SetResetTime( data )
    if not data.reset_day then
        return 
    end

    local day = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSalePassTime" )
    local day_1 = 0

    for i , v in ipairs( data.reset_day ) do
        if v > day then
            day_1 = v
            break
        end
    end

    day_1 = day_1 - 1

    if day_1 <= 0 then
        return 
    end


    local info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSaleActivityTime" )
    local _, __, ___, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( info.star )
    local star = info.star - ( hour * 60 * 60  ) - ( minute * 60 ) - second
    local _, __, ___, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( star )

    local upTime = star + ( day_1 * 24 * 60 * 60 )
    local _, __, ___, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( upTime )

    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local _, __, ___, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( n_Time )

    local countdown = upTime - n_Time
    if countdown <= 0 then
        return
    end

    if math.floor( countdown/(24*60*60) ) > 0 then
        Util:WidgetLabel( self.__text_limitnum, CPPGameLib.GetString( "hotSale_D" , math.floor( countdown/(24*60*60) ) ) , Macros.TypefaceSize.slightly , Macros.Color.headline )
    elseif math.floor( countdown/(60*60) ) > 0 then
        Util:WidgetLabel( self.__text_limitnum, CPPGameLib.GetString( "hotSale_H" , math.floor( countdown/(60*60) ) ) , Macros.TypefaceSize.slightly , Macros.Color.headline )
    else
        Util:WidgetLabel( self.__text_limitnum, CPPGameLib.GetString( "hotSale_M" , math.floor( countdown/(60) ) ) , Macros.TypefaceSize.slightly , Macros.Color.headline )
    end

    self.__text_limitnum:SetPosition( self.__text_limitnum:GetPositionX() , 150 )
end


--钻石购买
function Modules.TemplDirectHotSaleItem:__HandleDiamondBuy()
    local type = self:Overdue()
    if type == false then
        return 
    end
    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.HOT_SALE )
    local config_shop_goods = CPPGameLib.GetConfig("HotSaleGoods", info.subtype , false )
    if not config_shop_goods then
        return 
    end
    config_shop_goods = config_shop_goods.goods_list[ self.__data.goods_id ]

    local cost_value = config_shop_goods.cost_value_list[1]
    local diamond_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                    content = " ",
                                                    confirm_text = CPPGameLib.GetString("common_confirm") ,
                                                    confirm_func = function ( ... )
                                                        if cost_value <= diamond_num then
                                                            CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"ShopBuyRequest" , Macros.Game.SHOP.DIRECT_HOT_BUY ,  self.__data.goods_id )
                                                        else
                                                            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_diamond_not_enough"))
                                                        end
                                                    end,
                                                    close_func = function ( ... )
                                                        -- body
                                                    end,
                                                } )
    tips:SetLoadedCallback(function()
        local Affirm = Util:LabCenter( { 
                                        { zi = CPPGameLib.GetString("activity_affirmBuy") , ys = Macros.Color.content },
                                    } )
        Affirm:SetPosition(tips.txt_content:GetContentWidth()/2 , tips.txt_content:GetContentHeight()/2 )
        tips.txt_content:AddChild(Affirm)
    end)
end

--充值购买
function Modules.TemplDirectHotSaleItem:__HandleMoneyBuy()
    local type_ = self:Overdue()
    if type_ == false then
        return 
    end

    local time = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSaleBuyTag")
    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    
    if time ~= 0 and math.abs( time - n_Time ) < 20 then   
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_CD" , 20 - (n_Time - time) .. "s" ))
        return 
    end

    CallModuleFunc(ModuleType.CHARGE , "RechargeOrderIDReq" , Macros.Game.RechargeType.HOT_SALE , {recharge_id = self.__data.recharge_id  , goods_id = self.__data.goods_id })
    CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"SetHotSaleBuyTag" , n_Time )
end

--领取
function Modules.TemplDirectHotSaleItem:__HandleGet()
    CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"TakeHotSaleGoodsRechargeRewardReq" , self.__data )
end

--过期
function Modules.TemplDirectHotSaleItem:Overdue()
    local cur_info,oepn_info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen3" )
    local open = true
    if  oepn_info.isOpen ~= "yes" then
        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES,
                                                        content = CPPGameLib.GetString("direct_buy_timeEnd"),
                                                        confirm_func = function ( ... )
                                                            CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"CloseView"  )
                                                        end,
                                                    } )
        open = false
    end
    return open
end

function Modules.TemplDirectHotSaleItem:Currency( info )
    if self.diamond then
        for i = 1 , #self.diamondLab do
            if self.diamondLab[i] then
                self.diamondLab[i]:RemoveSelf(true)
                self.diamondLab[i] = nil
            end
        end

        self.diamond:RemoveSelf(true)
        self.diamond = nil
    end

    self.__button_money_buy:SetText("")
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
    self.__button_money_buy:AddChild( self.diamond )
    PosTool.Center( self.diamond )
end
