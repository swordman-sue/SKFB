--[[
%% @module: 水晶充值
%% @author: yjg
%% @created: 2018年5月9日10:47:59
--]]

Modules = Modules or {}

Modules.CrystalRechargeTabView = Modules.CrystalRechargeTabView or BaseClass(BaseTabView)

function Modules.CrystalRechargeTabView:__init()
end

--选项卡界面创建完成回调
function Modules.CrystalRechargeTabView:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.CrystalRechargeTabView:__OpenCallback()
    -- CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"DirectShopDataReq")
    if not CallModuleFunc(ModuleType.CRYSTAL_SHOP, "GetLuxurySignInType") then
        CallModuleFunc(ModuleType.CRYSTAL_SHOP, "LuxurySignInDataReq") 
    end
    self:__SetShowUI()
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_LUXURY,function (_, data)
        self:__SetShowUI()
    end)
end

--选项卡界面关闭回调
function Modules.CrystalRechargeTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.CrystalRechargeTabView:__Dispose()
    if self.__scv_goods_list then
        self.__scv_goods_list:DeleteMe()
        self.__scv_goods_list = nil
    end
end

--选项卡界面关闭回调
function Modules.CrystalRechargeTabView:__Getwidget()
    self.Image_RefrashTimeBg = self:GetWidget("Image_RefrashTimeBg")
    self.Image_RefrashTimeBg:SetVisible(false)

    self.Text_ActivityTime = self:GetWidget("Text_ActivityTime")
    self.Text_ActivityTime:SetVisible(false)

    self.Panel_HotRecommentScrollView = self:GetWidget("Panel_HotRecommentScrollView")
    self.Panel_HotRecommentScrollView:SetContentSize( 856.00 , 410.00 )
    PosTool.MoveBy( self.Panel_HotRecommentScrollView , 0 , 5 )

    -- self.Panel_HotRecommentScrollView:SetBackGroundColor(0, 0, 0)
    -- self.Panel_HotRecommentScrollView:SetBackGroundColorType(1)
    -- self.Panel_HotRecommentScrollView:SetBackGroundColorOpacity(120)
end

--选项卡界面关闭回调
function Modules.CrystalRechargeTabView:__ShowUI()

    local layout = Util:Layout( 600 , self.Panel_HotRecommentScrollView:GetContentHeight() )
    layout:SetAnchorPoint(1,0.5)
    layout:SetPosition( self.Panel_HotRecommentScrollView:GetContentWidth() , self.Panel_HotRecommentScrollView:GetContentHeight()/2 )
    self.Panel_HotRecommentScrollView:AddChild( layout )

    --列表
    local params =  
    {
        item_class = Modules.TemplCrystalRechargeItem,
        item_width = 600,
        item_height = 126,
        row = 4,
        col = 1,
        view_height = layout:GetContentHeight(),
    }
    -- 创建参数
    self.__scv_goods_list = GUI.ScrollView.New(params)
    layout:AddChild(self.__scv_goods_list:GetNode())
    PosTool.Center(self.__scv_goods_list:GetNode())

    local sprite = Util:Sprite( Resource.PathTool.GetHalfHeadPath( 50001 ) )
    sprite:SetAnchorPoint( 0 , 0 )
    sprite:SetPosition( -100 , 0 )
    self.Panel_HotRecommentScrollView:AddChild( sprite , -1 )
    ActionManager:ZoomAnimation( sprite , 2 , 1.01 , 1 )

    local crystalShop_Tips = Util:Label( CPPGameLib.GetString("crystalShop_Tips") )
    crystalShop_Tips:SetAnchorPoint( 0.5 , 0.5 )
    crystalShop_Tips:SetPosition( layout:GetContentWidth()/2 , 10 )
    layout:AddChild( crystalShop_Tips )
end

function Modules.CrystalRechargeTabView:__SetShowUI()
    -- 创建列表
    local goods_list = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"GetLuxuryechargeCnf")
    self.__scv_goods_list:SetDataList( goods_list )
end

function Modules.CrystalRechargeTabView:__GetShowType()
    return Macros.Game.SystemID.LUXURY
end

--------------------------------------------------------------------------------------------------------------------------------
--水晶条目
--------------------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

Modules.TemplCrystalRechargeItem = Modules.TemplCrystalRechargeItem or BaseClass(GUI.ScrollItem)

function Modules.TemplCrystalRechargeItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplCrystalRechargeItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(600,126)
    return self.__layout
end

function Modules.TemplCrystalRechargeItem:__delete()
end

function Modules.TemplCrystalRechargeItem:SetData(info)
    GUI.ScrollItem.SetData(self, info)
    self:__ShowUI( info )
end

function Modules.TemplCrystalRechargeItem:__ShowUI( info ) 
    --条目背景
    local bg_item_w = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_item_w")
    self.bg_item = Util:Sprite9( bg_item_w )
    self.bg_item:SetContentSize( 600 , 123 )
    self.__layout:AddChild( self.bg_item )
    PosTool.Center( self.bg_item )

    --图标
    for i , v in ipairs( info.item ) do
        local icon = LogicGUI.IconLayer.New()
        icon:SetData( {id = v.id , num = v.num } )
        self.bg_item:AddChild( icon:GetNode() )
        icon:SetAnchorPoint( 0 , 0.5 )
        icon:SetPosition( 20 + ( (icon:GetNode():GetContentWidth() + 10) * ( i - 1 ) ) , self.bg_item:GetContentHeight()/2 )
    end

    --提示
    local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , info.money )


    local t1 = {
                    {zi = CPPGameLib.GetString("recharge_advancedOneActivityTips_1") , ys = Macros.Color.pack_color , dx = Macros.TypefaceSize.largish },
                    {zi = text , ys = Macros.Color.headline , dx = Macros.TypefaceSize.largish , jg = 4},
                    {zi = CPPGameLib.GetString("recharge_advancedOneActivityTips_2") , ys = Macros.Color.pack_color  , dx = Macros.TypefaceSize.largish , jg = 4},
                }

    local tips = Util:LabCenter( t1 )  

    tips:SetAnchorPoint( 0.5 , 0.5 )
    tips:SetPosition( 480 , self.bg_item:GetContentHeight()/2 + 35 )
    self.bg_item:AddChild(tips)

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 CPPGameLib.GetString("shop_sure_Buy") , 
                                 Macros.TypefaceSize.largish , 
                                 Macros.Color.btn , Macros.Color.button_yellow
                                 )
    self.btn:SetAnchorPoint( 0.5 , 0.5 )
    self.btn:SetPosition( 480 , self.bg_item:GetContentHeight()/2 - 15 )
    self.bg_item:AddChild(self.btn) 

    self:Transfer( info )
end

function Modules.TemplCrystalRechargeItem:Transfer(data)
    if data.status == 0 then
        self:SetNoCondition( data )
    elseif data.status == 1 then
        self:SetMay( data )
    elseif data.status == 2 then
        self:SetNoMay( data )
    end
end

--设置未达到领取条件
function Modules.TemplCrystalRechargeItem:SetNoCondition()
    --按钮
    if self.btn then
        self.btn:RemoveSelf(true)
        self.btn = nil
    end
    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnNoBtn) , 
                                 CPPGameLib.GetString("shop_pay_money") , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow
                                 )
    self.btn:SetAnchorPoint( 0.5 , 0.5 )
    self.btn:SetPosition( 480 , self.bg_item:GetContentHeight()/2 - 15 )
    self.bg_item:AddChild(self.btn) 
end

--设置可以领取
function Modules.TemplCrystalRechargeItem:SetMay( data )
    --按钮
    if self.btn then
        self.btn:RemoveSelf(true)
        self.btn = nil
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 CPPGameLib.GetString("activity_vipRecharge") , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow
                                 )
    self.btn:SetAnchorPoint( 0.5 , 0.5 )
    self.btn:SetPosition( 480 , self.bg_item:GetContentHeight()/2 - 15 )
    self.bg_item:AddChild(self.btn)  

    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.btn )
    btnEffect_con:SetPosition(self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )
end

--设置已领取
function Modules.TemplCrystalRechargeItem:SetNoMay(data)
    --按钮
    if self.btn then
        self.btn:RemoveSelf(true)
        self.btn = nil
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
    self.btn = Util:Button( sp , CPPGameLib.Handler(self, self.OnNoBtn) )
    self.btn:SetAnchorPoint( 0.5 , 0.5 )
    self.btn:SetPosition( 480 , self.bg_item:GetContentHeight()/2 - 15 )
    self.bg_item:AddChild(self.btn) 
    self.btn:SetTouchEnabled(false)
end

--购买回调
function Modules.TemplCrystalRechargeItem:OnBtn(data)
    CallModuleFunc(ModuleType.CRYSTAL_SHOP, "TakeLuxurySignInRewardReq" , self.__data ) 
end

--不可购买回调
function Modules.TemplCrystalRechargeItem:OnNoBtn(data)
    CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
end