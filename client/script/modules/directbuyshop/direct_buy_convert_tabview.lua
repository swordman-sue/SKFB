
--[[
%% @module: 兑换特惠9报废
%% @author: yjg
%% @created: 2017年9月1日10:37:53
--]]

Modules = Modules or {}

Modules.DirectBuyConvertTabview = Modules.DirectBuyConvertTabview or BaseClass(BaseTabView)

function Modules.DirectBuyConvertTabview:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)
end

--选项卡界面创建完成回调
function Modules.DirectBuyConvertTabview:__LoadedCallback()
    -- local widget = self:GetWidget("Panel_ScrollView")
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.DirectBuyConvertTabview:__OpenCallback()

    if not CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetConvertRecordLst") then
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"DirectNotesRequest")
    else
        self:__SetShowUI( )
    end
    --商店数据
    self:BindGlobalEvent(Macros.Event.DirectBuyModule.CONVERT_DATA_RESP,function (_, data)
        self:__SetShowUI( )
    end)
end

--选项卡界面关闭回调
function Modules.DirectBuyConvertTabview:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.DirectBuyConvertTabview:__Dispose()
    if self.__scv_goods_list then
        self.__scv_goods_list:DeleteMe()
        self.__scv_goods_list = nil
    end
end

--选项卡界面关闭回调
function Modules.DirectBuyConvertTabview:__Getwidget()
    self.Panel_DirectConvertScrollView = self:GetWidget("Panel_DirectConvertScrollView")
    self.Image_1 = self:GetWidget("Image_1")
    self.Text_DirectConvertRefreshTime = self:GetWidget("Text_DirectConvertRefreshTime")
end

--选项卡界面关闭回调
function Modules.DirectBuyConvertTabview:__ShowUI()
    --列表
    local params =  
    {
        item_class = Modules.DirectBuyConvertItem,
        item_width = 208,
        item_height = 350,
        view_width = self.Panel_DirectConvertScrollView:GetContentWidth(),
        row = 1,
        col = 4,
        horizon = true,
        item_space_c = 8,
    }
    -- 创建参数
    self.__scv_goods_list = GUI.ScrollView.New(params)
    self.Panel_DirectConvertScrollView:AddChild(self.__scv_goods_list:GetNode())
    PosTool.Center(self.__scv_goods_list:GetNode())
    self.__scv_goods_list:PlayFlyInAction( self.Panel_DirectConvertScrollView:GetContentWidth() , 0 , 0.1 , 0.3 , function ( ... )
    end )
        
    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", 10027 )
    local _ , config_item = ObjIDTool.GetInfo( 10027 )
    local lab_cost = nil
    lab_cost , self.lab_cost = Util:LabCenter({
        {tp = Resource.PathTool.GetItemPath( config_item.res_id ), dx = 0.5},
        {zi = tostring(item_num), ys = 101, dx = Macros.TypefaceSize.largish, jg = 8 , mb = true },
    })            
    lab_cost:SetAnchorPoint(0, 0.5)  
    self.Text_DirectConvertRefreshTime:AddChild(lab_cost)
    lab_cost:SetPosition( 0 , self.Text_DirectConvertRefreshTime:GetContentHeight()/2 )

    --获取按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_achieving",true)
    local btn = Util:Button( sp , CPPGameLib.Handler(self,self.Onbtn_no) )
    btn:SetAnchorPoint(1,0.5)
    btn:SetPosition( self.Image_1:GetContentWidth() - 10 , self.Image_1:GetContentHeight()/2 )
    self.Image_1:AddChild(btn)
end

function Modules.DirectBuyConvertTabview:__SetShowUI()
    --创建列表
    local goods_list = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetConvertGoodsList")
    self.__scv_goods_list:SetDataList( goods_list )

    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", 10027 )
    self.lab_cost[2]:SetString( item_num )

end

function Modules.DirectBuyConvertTabview:Onbtn_no()
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, 10027)
end





















--------------------------------------------------------------------------------------------------------------------------------
--直购特惠商品模板
--------------------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

--副本宝箱信息模板
Modules.DirectBuyConvertItem = Modules.DirectBuyConvertItem or BaseClass(GUI.ScrollItem)

function Modules.DirectBuyConvertItem:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)

    self.__button_diamond_buy = Util:WidgetButton(self:GetWidget("Button_Diamond_Buy"), "", function()
        self:__HandleDiamondBuy()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)

    self.__button_money_buy = Util:WidgetButton(self:GetWidget("Button_Money_Buy"), "", function()
        self:__HandleMoneyBuy()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)

    self.Button_Get = Util:WidgetButton(self:GetWidget("Button_Get"), CPPGameLib.GetString("dungeon_receive") , function()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
    self.Button_Get:SetVisible(false)

    self.__text_goodname = Util:WidgetLabel(self:GetWidget("Text_GoodName"), "", Macros.TypefaceSize.largish  , Macros.Color.keypoint)
    self.__text_limitnum = Util:WidgetLabel(self:GetWidget("Text_LimitNum"), "", Macros.TypefaceSize.normal , Macros.Color.keypoint)

    self.__panel_goodicon = self:GetWidget("Panel_GoodIcon")
    self.__image_sellout = self:GetWidget("Image_Sellout")
end

function Modules.DirectBuyConvertItem:__delete()
end

function Modules.DirectBuyConvertItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)


    local config_shop_goods = CPPGameLib.GetConfig("ExchangeShop", self.__data.goods_id)
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
    self.__text_limitnum:SetText(CPPGameLib.GetString("direct_buy_shop_limit_buy", config_shop_goods.buy_num_max - self.__data.buy_num, config_shop_goods.buy_num_max))

    local icon = LogicGUI.IconLayer.New()
    icon:SetData({id = config_shop_goods.item_id, num = config_shop_goods.item_num})
    self.__panel_goodicon:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())

    --转圈
    local effectid, effect = self:__CreateEffect({res_name = "UI_banjia",loop = true}, icon:GetNode() ,2)
    PosTool.Center(effect)
    
    --折扣图标
    local function handle_discount(btn)
        if config_shop_goods.discount then
            local icon_discount = LogicGUI.LconDiscount.New(config_shop_goods.discount)
            icon_discount:SetAnchorPoint(0.5, 0.5)
            btn:AddChild(icon_discount)
            PosTool.LeftTop(icon_discount)
        end
    end

    if not is_sellout then

        --需要消耗的类型
        if config_shop_goods.cost_type_list then
            --钻石购买按钮
            local cost_type = config_shop_goods.cost_type_list[1]
            local cost_value = config_shop_goods.cost_value_list[1]
            if cost_type and cost_value then
                is_diamond_buy = true

                handle_discount(self.__button_diamond_buy)

                local lab_cost = Util:LabCenter({
                    {tp = Util:GetMoneyIconImgData(cost_type), dx = 0.8},
                    {zi = tostring(cost_value), ys = 101, dx = Macros.TypefaceSize.largish, jg = 8 , mb = true },
                })            
                lab_cost:SetAnchorPoint(0.5 , 0.5)  
                self.__button_diamond_buy:AddChild(lab_cost)
                PosTool.Center(lab_cost)
            end
        end

        --需要消耗的物品
        if config_shop_goods.cost_item_id_list then
            --物品购买按钮
            local cost_type = config_shop_goods.cost_item_id_list[1]
            local cost_value = config_shop_goods.cost_item_num_list[1]
            if cost_type and cost_value then
                is_money_buy = true

                handle_discount(self.__button_money_buy)

                local _ , config_item = ObjIDTool.GetInfo( cost_type )

                local lab_cost = Util:LabCenter({
                    {tp = Resource.PathTool.GetItemPath(config_item.res_id) , dx = 0.5},
                    {zi = tostring(cost_value), ys = 101, dx = Macros.TypefaceSize.largish, jg = 8 , mb = true },
                })            
                lab_cost:SetAnchorPoint(0.5 , 0.5)  
                self.__button_money_buy:AddChild(lab_cost)
                PosTool.Center(lab_cost)
            end
        end
    end

    self.__button_diamond_buy:SetVisible(not is_sellout and is_diamond_buy)
    self.__button_money_buy:SetVisible(not is_sellout and is_money_buy)
    self.__image_sellout:SetVisible(is_sellout)

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
end

--？？？购买
function Modules.DirectBuyConvertItem:__HandleDiamondBuy()
    print("xy" , "__HandleDiamondBuy" )
    local config_shop_goods = CPPGameLib.GetConfig("ExchangeShop", self.__data.goods_id)
    local _ , config_item = ObjIDTool.GetInfo( config_shop_goods.item_id )
    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", config_shop_goods.cost_item_id_list[1])
    if item_num >= config_shop_goods.cost_item_num_list[1] then
        CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"ConvertShopBuyRequest" , self.__data.goods_id )
    else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("direct_buy_insufficient",config_item.name))
    end
end

--物品购买
function Modules.DirectBuyConvertItem:__HandleMoneyBuy()
    print("xy" , "__HandleMoneyBuy" )
    local config_shop_goods = CPPGameLib.GetConfig("ExchangeShop", self.__data.goods_id)
    local _ , config_item = ObjIDTool.GetInfo( config_shop_goods.cost_item_id_list[1] )

    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", config_shop_goods.cost_item_id_list[1])
    if item_num >= config_shop_goods.cost_item_num_list[1] then
        CallModuleFunc( ModuleType.DIRECT_BUY_SHOP,"ConvertShopBuyRequest" , self.__data.goods_id )
    else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("direct_buy_insufficient",config_item.name))
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, 10027)
    end
end