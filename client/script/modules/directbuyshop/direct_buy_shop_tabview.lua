--[[
%% @module: 直购特惠
%% @author: yjg
%% @created: 2017年9月1日10:37:53
--]]

Modules = Modules or {}

Modules.DirectBuyTabView = Modules.DirectBuyTabView or BaseClass(BaseTabView)

function Modules.DirectBuyTabView:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)
end

--选项卡界面创建完成回调
function Modules.DirectBuyTabView:__LoadedCallback()
    -- local widget = self:GetWidget("Panel_ScrollView")
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.DirectBuyTabView:__OpenCallback()
    CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"DirectShopDataReq")
    --商店数据
    self:BindGlobalEvent(Macros.Event.DirectBuyModule.DIRECT_SHOP_DATA_RESP,function (_, data)
        self:__SetShowUI( data )
    end)

    --商店数据
    self:BindGlobalEvent(Macros.Event.DirectBuyModule.TAKE_DIRECT_BUY_GOODS_RESP,function (_, data)
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"DirectShopDataReq" , true )
    end)

    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_SUCCESS,function ( _ , type_ )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetDirectBuyBuyTag",0)
    end)
    
    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_CANCEL,function ( _ , type_ )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetDirectBuyBuyTag",0)
    end)

    --取消标记
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_FAILED,function ( _ , type_ )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetDirectBuyBuyTag",0)
    end)

end

--选项卡界面关闭回调
function Modules.DirectBuyTabView:__CloseCallback()
    -- if self.__scv_goods_list then
    --     self.__scv_goods_list:DeleteMe()
    --     self.__scv_goods_list = nil
    -- end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.DirectBuyTabView:__Dispose()
    if self.__scv_goods_list then
        self.__scv_goods_list:DeleteMe()
        self.__scv_goods_list = nil
    end
end

--选项卡界面关闭回调
function Modules.DirectBuyTabView:__Getwidget()
    self.Panel_DirectBuyScrollView = self:GetWidget("Panel_DirectBuyScrollView")

    self.Panel_1 = self:GetWidget("Panel_1")
    self.Panel_1:SetVisible(false)

    self.Text_DirectBuyRefreshTime = self:GetWidget("Text_DirectBuyRefreshTime")
    self.Text_DirectBuyRefreshTime:SetVisible(false)

    self.Image_1 = self:GetWidget("Image_1")
    self.Image_1:SetVisible(false)

    self.Text_DirectConvertRefreshTime = self:GetWidget("Text_DirectConvertRefreshTime")
    self.Text_DirectConvertRefreshTime:SetVisible(false)
end

--选项卡界面关闭回调
function Modules.DirectBuyTabView:__ShowUI()
    --列表
    local params =  
    {
        item_class = Modules.TemplDirectBuyItem,
        item_width = 208,
        item_height = 350,
        view_width = self.Panel_DirectBuyScrollView:GetContentWidth(),
        row = 1,
        col = 4,
        horizon = true,
        item_space_c = 8,
    }
    -- 创建参数
    self.__scv_goods_list = GUI.ScrollView.New(params)
    self.Panel_DirectBuyScrollView:AddChild(self.__scv_goods_list:GetNode())
    PosTool.Center(self.__scv_goods_list:GetNode())

    --刷新时间
    self.Text_DirectBuyRefreshTime = Util:WidgetLabel(   
                                                        self.Text_DirectBuyRefreshTime , 
                                                        CPPGameLib.GetString("direct_buy_shop_upTime" , CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyUpTime")  ), 
                                                        Macros.TypefaceSize.largish , 
                                                        Macros.Color.Special
                                                    )
    self.Text_DirectBuyRefreshTime:SetVisible( false )

    self.bg_time = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, "bg_time") )
    self.bg_time:SetAnchorPoint( 0 , 0.5 )
    self.bg_time:SetContentSize( self.Text_DirectBuyRefreshTime:GetContentWidth() + 20 ,  self.Panel_1:GetContentHeight() )
    self.bg_time:SetPosition( 0 , self.Panel_1:GetContentHeight()/2 )
    self.Panel_1:AddChild( self.bg_time , -1 )

    --时间列表
    local timeLst = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen1" )
    local direct_buy_time = Util:Name( CPPGameLib.GetString("direct_buy_time" , timeLst ) , Macros.TypefaceSize.normal )
    direct_buy_time:SetAnchorPoint( 1 , 0.5 )
    direct_buy_time:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() )
    self.__root_widget:AddChild( direct_buy_time )
    direct_buy_time:SetVisible( false )

    local effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Panel_DirectBuyScrollView , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Panel_DirectBuyScrollView:GetContentWidth() + 10 , self.Panel_DirectBuyScrollView:GetContentHeight()/2 )

    local effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Panel_DirectBuyScrollView , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( - 10 , self.Panel_DirectBuyScrollView:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )

end

function Modules.DirectBuyTabView:__SetShowUI()
    --创建列表
    local goods_list = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetBuyGoodsList")
    self.__scv_goods_list:SetDataList( goods_list )
    
    -- self.__scv_goods_list:PlayFlyInAction( self.Panel_DirectBuyScrollView:GetContentWidth() , 0 , 0.1 , 0.3 , function ( ... )
    -- end )
end

--------------------------------------------------------------------------------------------------------------------------------
--直购特惠商品模板
--------------------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

--副本宝箱信息模板
Modules.TemplDirectBuyItem = Modules.TemplDirectBuyItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDirectBuyItem:__init(data)
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
                                            Macros.TypefaceSize.popup  , 
                                            Macros.Color.keypoint
                                            )

    self.__text_limitnum = Util:WidgetLabel(self:GetWidget("Text_LimitNum"), "", Macros.TypefaceSize.slightly , Macros.Color.keypoint )

    self.__panel_goodicon = self:GetWidget("Panel_GoodIcon")
    self.__image_sellout = self:GetWidget("Image_Sellout")
end

function Modules.TemplDirectBuyItem:__delete()
end

function Modules.TemplDirectBuyItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local config_shop_goods = CPPGameLib.GetConfig("DirectBuyShop", self.__data.goods_id)
    if not config_shop_goods then
        return
    end

    local _ , config_item = ObjIDTool.GetInfo( config_shop_goods.item_id )
    if not config_item then
        return
    end

    local is_sellout = self.__data.buy_num >= config_shop_goods.buy_num_max
    local is_diamond_buy, is_money_buy

    --商品名称、限购、icon
    self.__text_goodname:SetText(config_item.name)
    self.__text_limitnum:SetText(CPPGameLib.GetString("direct_buy_shop_limit_buy" .. config_shop_goods.refresh_type , config_shop_goods.buy_num_max - self.__data.buy_num, config_shop_goods.buy_num_max))

    CPPGameLib.PrintTable("xy" , config_shop_goods , "config_recharge")  

    local icon = LogicGUI.IconLayer.New()
    icon:SetData({id = config_shop_goods.item_id , num = config_shop_goods.item_num })
    self.__panel_goodicon:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())

    --转圈
    local effectid, effect = self:__CreateEffect({res_name = "UI_banjia",loop = true}, icon:GetNode() ,2)
    PosTool.Center(effect)

    --折扣图标
    local function handle_discount(btn , num)
        if num then
            local icon_discount = LogicGUI.LconDiscount.New( num )
            icon_discount:SetAnchorPoint(0.5, 0.5)
            btn:AddChild(icon_discount:GetNode() , 999 )
            PosTool.LeftTop(icon_discount , 0 , 0)
            icon_discount:SetScaleFactor( 0.9 )
        end
    end

    if not is_sellout then
        --钻石购买按钮
        local cost_type = config_shop_goods.cost_type_list and config_shop_goods.cost_type_list[1] or nil
        local cost_value = config_shop_goods.cost_value_list and config_shop_goods.cost_value_list[1] or nil
        if cost_type and cost_value then
            is_diamond_buy = true

            handle_discount(self.__button_diamond_buy , config_shop_goods.discount )

            local lab_cost = Util:LabCenter({
                {tp = Util:GetMoneyIconImgData(cost_type), dx = 0.8},
                {zi = tostring(cost_value), ys = 101, dx = Macros.TypefaceSize.largish, jg = 8 , mb = true },
            })            
            lab_cost:SetAnchorPoint(0.5, 0.5)  
            self.__button_diamond_buy:AddChild(lab_cost)
            PosTool.Center(lab_cost)
        end

        --充值购买按钮
        local config_recharge = CPPGameLib.GetConfig("Recharge", config_shop_goods.recharge_id, false)
        if config_recharge then
            is_money_buy = true

            handle_discount(self.__button_money_buy , config_shop_goods.discount_rmb )

            self:Currency( config_shop_goods.recharge_id )
        end
    end

    self.__button_diamond_buy:SetVisible(not is_sellout and is_diamond_buy)
    self.__button_money_buy:SetVisible(not is_sellout and is_money_buy)

    self.__image_sellout:SetVisible(is_sellout)

    if is_sellout then
        self.__text_limitnum:SetText(CPPGameLib.GetString("direct_buy_shop_limit_refresh" .. config_shop_goods.refresh_type ))
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
function Modules.TemplDirectBuyItem:SetNoMay( data )
    local type_ = CallModuleFunc( ModuleType.DIRECT_BUY_SHOP , "GetBuyCanTakeGoodsList", data.goods_id )
    if type_ then
        self.__button_diamond_buy:SetVisible(false)
        self.__button_money_buy:SetVisible(false)
        self.__image_sellout:SetVisible(false)
        self.Button_Get:SetVisible(true)
    end
end

--钻石购买
function Modules.TemplDirectBuyItem:__HandleDiamondBuy()
    local type = self:Overdue()
    if type == false then
        return 
    end
    local config_shop_goods = CPPGameLib.GetConfig("DirectBuyShop", self.__data.goods_id)
    local cost_value = config_shop_goods.cost_value_list[1]
    local diamond_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                        content = " ",
                                                        confirm_text = CPPGameLib.GetString("common_confirm") ,
                                                        confirm_func = function ( ... )
                                                            if cost_value <= diamond_num then
                                                                CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"ShopBuyRequest" , Macros.Game.SHOP.DIRECT_BUY ,  self.__data.goods_id )
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
function Modules.TemplDirectBuyItem:__HandleMoneyBuy()
    local type = self:Overdue()
    if type == false then
        return 
    end

    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local time = CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyBuyTag" )
    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if time ~= 0 and math.abs( time - n_Time ) < 20 then      
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_CD" , 20 - (n_Time - time) .. "s" ))
        return 
    end

    local config_shop_goods = CPPGameLib.GetConfig("DirectBuyShop", self.__data.goods_id)
    print("xy" , "__HandleMoneyBuy" , config_shop_goods.recharge_id, self.__data.goods_id)
    CallModuleFunc(ModuleType.RECHARGE , "HandleRecharge" , Macros.Game.RechargeType.DIRECT_BUY , {recharge_id = config_shop_goods.recharge_id, goods_id = self.__data.goods_id})
    
    CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"SetDirectBuyBuyTag" , n_Time )
end

--领取
function Modules.TemplDirectBuyItem:__HandleGet()
    CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"TakeDirectBuyGoodsReq" , self.__data.goods_id )
end

--过期
function Modules.TemplDirectBuyItem:Overdue()
    local cur_info,oepn_info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen1" )
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

function Modules.TemplDirectBuyItem:Currency( info )
    if self.diamond then
        self.diamond:RemoveSelf(true)
        self.diamond = nil

        for i = 1 , #self.diamondLab do
            self.diamondLab[i]:RemoveSelf(true)
            self.diamondLab[i] = nil
        end
    end

    self.__button_money_buy:SetText("")
    if type( info ) == "number" then
        local text = CallModuleFunc(ModuleType.SUNDRY, "Currency" , info )
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
