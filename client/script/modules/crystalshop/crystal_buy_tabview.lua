--[[
%% @module: 水晶商店购买
%% @author: yjg
%% @created: 2018年5月9日10:45:59
--]]

Modules = Modules or {}

Modules.CrystalBuyTabView = Modules.CrystalBuyTabView or BaseClass(BaseTabView)

function Modules.CrystalBuyTabView:__init()
    -- self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIRECT_BUY_ITEM)
end

--选项卡界面创建完成回调
function Modules.CrystalBuyTabView:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.CrystalBuyTabView:__OpenCallback()
    local goods_list = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"GetGoodsList")  
    if not goods_list or #goods_list == 0 then
        CallModuleFunc(ModuleType.CRYSTAL_SHOP,"CrystalShopDataReq")
    else
        self:__SetShowUI()
    end

    self:BindGlobalEvent(Macros.Event.CrystalShopModule.CRYSTAL_BUY_DATA,function (_, data)
        self:__SetShowUI()
    end)
end

--选项卡界面关闭回调
function Modules.CrystalBuyTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.CrystalBuyTabView:__Dispose()
    if self.__scv_goods_list then
        self.__scv_goods_list:DeleteMe()
        self.__scv_goods_list = nil
    end
end

--选项卡界面关闭回调
function Modules.CrystalBuyTabView:__Getwidget()
    self.Panel_DirectBuyScrollView = self:GetWidget("Panel_DirectBuyScrollView")
    self.Panel_DirectBuyScrollView:SetContentSize( 900 , 380 )
    PosTool.MoveBy( self.Panel_DirectBuyScrollView , 0 , 20 )

    self.Image_1 = self:GetWidget("Image_1")
    self.Image_1:SetVisible(false)

    self.Panel_1 = self:GetWidget("Panel_1")
    self.Panel_1:SetVisible(false)
end

--选项卡界面关闭回调
function Modules.CrystalBuyTabView:__ShowUI()
    --列表
    local params =  
    {
        item_class = Modules.TemplCrystalBuyItem,
        item_width = 410  ,
        item_height = 126,
        view_height = self.Panel_DirectBuyScrollView:GetContentHeight() ,
        row = 4,
        col = 2,
    } 
    -- 创建参数
    self.__scv_goods_list = GUI.ScrollView.New(params)
    self.Panel_DirectBuyScrollView:AddChild(self.__scv_goods_list:GetNode())
    PosTool.Center(self.__scv_goods_list:GetNode())

    --拥有水晶
    local t1 = {
                 { zi = CPPGameLib.GetString("crystalShop_hold") , ys = Macros.Color.Special },
                 { tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.CRYSTAL ) , dx = 0.8 , jg = -5 },
                 { zi = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.CRYSTAL) , jg = 5 },
                }
    local crystalShop_hold = nil
    crystalShop_hold , self.crystalShop_hold = Util:LabCenter( t1 )
    crystalShop_hold:SetAnchorPoint( 0 , 0.5 )
    crystalShop_hold:SetPosition( 60 , -12 )
    self.Panel_DirectBuyScrollView:AddChild( crystalShop_hold )

    --去获取
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_achieving",true)
    local btn = Util:Button( sp , CPPGameLib.Handler(self,self.OnGoGet) )
    btn:SetAnchorPoint( 0 , 0.5 )
    btn:SetPosition(280 , -12 )
    self.Panel_DirectBuyScrollView:AddChild(btn)

    --剩余刷新次数
    local t1 = {
                 { zi = CPPGameLib.GetString("crystalShop_Upitem") , ys = Macros.Color.Special },
                 { zi = "00/00" , jg = 5 },
                }
    local crystalShop_Upitem = nil
    crystalShop_Upitem , self.crystalShop_Upitem = Util:LabCenter( t1 )
    crystalShop_Upitem:SetAnchorPoint( 0 , 0.5 )
    crystalShop_Upitem:SetPosition(self.Panel_DirectBuyScrollView:GetContentWidth()/2 + 200 , -12 )
    self.Panel_DirectBuyScrollView:AddChild( crystalShop_Upitem )

    --刷新
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_divina_rebirth")
    local refreshBtn = Util:Button( sp , CPPGameLib.Handler(self,self.OnrRfreshBtn) )
    refreshBtn:SetAnchorPoint( 0 , 0.5 )
    refreshBtn:SetPosition(900 , 20 )
    self.Panel_DirectBuyScrollView:AddChild(refreshBtn)
end

function Modules.CrystalBuyTabView:__SetShowUI()
    --创建列表
    local goods_list = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"GetGoodsList")    
    self.__scv_goods_list:SetDataList( goods_list )

    -- self.__scv_goods_list:ItemDataIter(function(i, item_data , item )
    --                                         if item then
    --                                             item:SetData( item_data )
    --                                         end
    --                                     end)
    --有用水晶
    self.crystalShop_hold[3]:SetString( CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.CRYSTAL) )
    
    --剩余次数
    local num , max = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"GetRefreshTimes") 
    self.crystalShop_Upitem[2]:SetString(  max - num .. "/" .. max )
end

--去获取
function Modules.CrystalBuyTabView:OnGoGet()
    GlobalEventSystem:FireNextFrame(Macros.Event.CrystalShopModule.CRYSTAL_FLIP_OVER , 2 )
end

--刷新
function Modules.CrystalBuyTabView:OnrRfreshBtn()
    local num , max = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"GetRefreshTimes") 
    if num ~= max then
        CallModuleFunc(ModuleType.CRYSTAL_SHOP,"CrystalShopRefreshReq")  
    else
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("divination_notRemainTime") )
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--水晶条目
--------------------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

Modules.TemplCrystalBuyItem = Modules.TemplCrystalBuyItem or BaseClass(GUI.ScrollItem)

function Modules.TemplCrystalBuyItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplCrystalBuyItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(410,126)
    self.__layout:SetTouchEnabled(false)
    self.__layout:SetClippingEnable(false)
    return self.__layout
end

function Modules.TemplCrystalBuyItem:__delete()
end

function Modules.TemplCrystalBuyItem:SetData(info)
    GUI.ScrollItem.SetData(self, info)
    self:__ShowUI( info )
end

function Modules.TemplCrystalBuyItem:__ShowUI( info ) 
    --条目背景
    local bg_item_w = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_item_w")
    local bg_item = Util:Sprite9( bg_item_w )
    bg_item:SetContentSize( 400 , 123 )
    self.__layout:AddChild( bg_item )
    PosTool.Center( bg_item )

    --图标
    local icon = LogicGUI.IconLayer.New()
    icon:SetData( { id = info.item_id , num = info.item_num } )
    bg_item:AddChild( icon:GetNode() )
    icon:SetAnchorPoint( 0 , 0.5 )
    icon:SetPosition( 20 , bg_item:GetContentHeight()/2 )

    local type_ , cnf = ObjIDTool.GetInfo( info.item_id )
    local name = Util:Name( cnf.name , nil , cnf.init_quality or cnf.quality )
    name:SetAnchorPoint( 0 , 1 )
    name:SetPosition( icon:GetNode():GetPositionX() + icon:GetNode():GetContentWidth() + 10 , bg_item:GetContentHeight() - 15 )
    bg_item:AddChild( name )

    --描述背景
    local sp_item_text = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, "sp_item_text")
    local item_text = Util:Sprite9( sp_item_text )
    item_text:SetContentSize( 140 , 57 )
    item_text:SetAnchorPoint( 0 , 0 )
    item_text:SetPosition( icon:GetNode():GetPositionX() + icon:GetNode():GetContentWidth() + 10 , 18 )
    bg_item:AddChild( item_text )

    --消耗
    for i , v in ipairs( info.cost ) do
        local money = Util:MoneyControl( v.id , 0.6 , v.value , nil , nil , 10 )
        money:SetAnchorPoint( 0 , 1 )
        money:SetPosition( 10 , ( item_text:GetContentHeight() - 7 ) - ( 25 * (i-1) ) )
        item_text:AddChild( money )
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 CPPGameLib.GetString("shop_sure_Buy") , 
                                 Macros.TypefaceSize.largish , 
                                 Macros.Color.btn , Macros.Color.button_yellow
                                 )
    self.btn:SetAnchorPoint( 0 , 0.5 )
    self.btn:SetPosition( bg_item:GetContentWidth()/2 + 57 , bg_item:GetContentHeight()/2 )
    bg_item:AddChild(self.btn) 

    self:SetBtn()
end

function Modules.TemplCrystalBuyItem:SetBtn()
    local info = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"ToIdGetGoodsNum",self.__data.id) 
    --如果最大数量不为0
    if self.__data.buy_num_max ~= 0 then
        if info.buy_num >= self.__data.buy_num_max then
            self.btn:SetText( CPPGameLib.GetString("shop_albuy") )
            Util:GreyButton( self.btn )
        end
    else
        self.btn:SetText( CPPGameLib.GetString("shop_sure_Buy") )
        self.btn:SetTextColor(unpack(Macros.Color.btn))
        local r, g, b, a = unpack(Macros.Color.button_yellow)
        self.btn:SetOutline(r, g, b, a, 2)
    end
end

function Modules.TemplCrystalBuyItem:OnBtn()
    local type_ = nil
    for i , v in ipairs( self.__data.cost ) do
        local value = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v.id )
        if value < v.value then
            type_ = v.id
            break
        end
    end

    if type_ ~= nil then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , CPPGameLib.GetString("currency_get_"..type_) ) )   
        return
    end
    CallModuleFunc(ModuleType.CRYSTAL_SHOP,"CrystalShopBuyReq",self.__data.id) 
end