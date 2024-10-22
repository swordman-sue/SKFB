--[[
%% @module: 占卜命运商店
%% @author: yjg
%% @created: 2017年11月16日20:31:59
--]]

Modules = Modules or {} 
Modules.DivinationDestinyShopView = Modules.DivinationDestinyShopView or BaseClass(BaseView)

function Modules.DivinationDestinyShopView:__init()
    self.__layout_name = "divination_destiny_shop"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination_destinystarShop"), 
        show_diamond = true,
        show_gold = true,
        show_destiny_coin = true,
        show_star_soul = true,
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__tab_params_ex = {6 , true}

    self.__open_callback = function()
      self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
    -- self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_HERO_SHOP_ITEM)    
end

function Modules.DivinationDestinyShopView:__Dispose()
end

function Modules.DivinationDestinyShopView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end 

function Modules.DivinationDestinyShopView:__Server( )
    --命运商店
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DESTINY , function (_, data)
        self:__SetShowUI()
    end)
end

--获得组件
function Modules.DivinationDestinyShopView:__Getwidget( )

    --居中层
    self.Panel_store = self:GetWidget("Panel_store")
    --大背景
    self.Image_bigBg = self:GetWidget("Image_bigBg")
    --二背景
    self.Image_smallBg = self:GetWidget("Image_smallBg")
    --列表背景
    self.Image_scrollviewDeepBg = self:GetWidget("Image_scrollviewDeepBg")
    --列表容器
    self.PageView_1 = self:GetWidget("PageView_1")
    --→
    self.Image_front = self:GetWidget("Image_front")
    --←
    self.Image_back = self:GetWidget("Image_back")
    --标签页
    self.Panel_tab = self:GetWidget("Panel_tab")

end

--组件设置
function Modules.DivinationDestinyShopView:__ShowUI( )
    --页签
    local text =  {
            [1] = CPPGameLib.GetString("divination_basicsCard") ,
            [2] = CPPGameLib.GetString("divination_purpleCard") ,
            [3] = CPPGameLib.GetString("divination_orangeCard") ,
        }

    self.__tab:SetContentSize( self.Panel_tab:GetContentWidth() , self.Panel_tab:GetContentHeight() + 25 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition(self.Panel_tab:GetContentWidth()/2 , self.Panel_tab:GetContentHeight()/2 + 15 )
    self.Panel_tab:AddChild(self.__tab:GetNode())

    self.__tab:On( unpack(self.__open_data) or 1 )

    local tips = Util:Label( CPPGameLib.GetString("divination_destinyCoin") ..  CPPGameLib.GetString("common_colon") , nil , Macros.Color.content )
    tips:SetAnchorPoint( 0 , 0.5 )
    tips:SetPosition( 10 , 30 )
    self.Image_smallBg:AddChild( tips )

    local destinyCoin = Util:MoneyControl( Macros.Game.RolePropType.DESTINY_COIN )
    self.Image_smallBg:AddChild( destinyCoin )
    PosTool.RightTo( tips , destinyCoin )

    self.destinyCoin = Util:Label( 0 , nil , Macros.Color.content )
    self.Image_smallBg:AddChild( self.destinyCoin )
    PosTool.RightTo( destinyCoin , self.destinyCoin , 10 )

    self:__SetShowUI()
end

function Modules.DivinationDestinyShopView:__SetShowUI( )
    local money = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.DESTINY_COIN )
    self.destinyCoin:SetString( money )
end

function Modules.DivinationDestinyShopView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )
    layer:SetAnchorPoint( 0.5 , 0.5 )

    local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.white , nil , nil , Macros.Color.button_yellow )
    name:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(name)
    PosTool.Center( name )

    return layer
end

--选中
function Modules.DivinationDestinyShopView:OnCallBack( index , data )
    if self.OnItemEff then
        self.OnItemEff:RemoveSelf(true)
        self.OnItemEff = nil
    end
    local item = self.__tab:GetBtn( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    self.OnItemEff = Util:Sprite( com_sp_cbox_wire )
    self.OnItemEff:SetAnchorPoint( 0.5 , 1 )
    self.OnItemEff:SetPosition( item:GetContentWidth()/2 , 2 )
    item:AddChild( self.OnItemEff )

    local lst = CallModuleFunc(ModuleType.DIVINATION,"GetDivinationDestinyShopLst" , index )
    self:SetCell( Util:SegmentationTab(lst , 6) )
end

--设置细胞
function Modules.DivinationDestinyShopView:SetCell( data )
    if self.packIcon then
        for i = 1 , #self.packIcon do
            self.packIcon[i]:DeleteMe()
            self.packIcon[i] = nil
        end
        self.PageView_1:RemoveAllPages()
    end

    self.packIcon = {}
    local index = 0
    for i,v in ipairs(data) do
        local layer = Util:Layout( self.PageView_1:GetContentWidth() , self.PageView_1:GetContentHeight() )

        local interval = layer:GetContentWidth() / 3 
        local xIndex = 0
        local y = layer:GetContentHeight() - 197 

        for i,vv in ipairs(v) do
            xIndex = xIndex + 1
            if xIndex > 3 then
                xIndex = 1
                y = 0
            end
            index = index + 1
            self.packIcon[index] = GUI.TemplDivinationDestinyShopCell.New(vv)
            self.packIcon[index]:SetPosition( (xIndex-1) * interval , y)
            self.packIcon[index]:GetNode():SetOpacity(0)
            layer:AddChild( self.packIcon[index]:GetNode() )
        end
        self.PageView_1:AddPage(layer)
    end
    self:ShowAction()
end

function  Modules.DivinationDestinyShopView:ShowAction()

    print("xy" , "self.touch" , self.touch )
    local index = 0
    local func = nil
    local function moveAnimate( ... )
        index = index + 1
        if self.packIcon[index] then
            CPPActionManager.FadeIn(self.packIcon[index]:GetNode(),0.05,func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end










GUI = GUI or {}
GUI.TemplDivinationDestinyShopCell = GUI.TemplDivinationDestinyShopCell or BaseClass(GUI.Template)
function GUI.TemplDivinationDestinyShopCell:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)

    self.data = data
    self:__Getwidget()
    self:__ShowUI(data)
end

function GUI.TemplDivinationDestinyShopCell:__AddToParent()

end

function GUI.TemplDivinationDestinyShopCell:__Getwidget()
    --背景
    self.Image_2 = self:GetWidget("Image_2") 
    --名字
    self.Panel_name = self:GetWidget("Panel_name") 
    
    --图标
    self.Panel_head = self:GetWidget("Panel_head") 
    --原价
    self.Text_couldBuyTimes = self:GetWidget("Text_couldBuyTimes") 
    self.Text_couldBuyTimes:SetText(" ")
    --价格背景
    self.Image_coinBg = self:GetWidget("Image_coinBg") 
    self.Image_coinBg:SetVisible(false)

    --货币图标
    self.Image_coin = self:GetWidget("Image_coin") 
    self.Image_coin:SetVisible(false)

    --价钱
    self.Text_coinNum = self:GetWidget("Text_coinNum") 
    self.Text_coinNum:SetVisible(false)

    --VIP图片
    self.Image_vip = self:GetWidget("Image_vip") 
    self.Image_vip:SetVisible(false)
        --vip字样
        self.Text_vip = self:GetWidget("Text_vip") 
    --原价容器X2
    self.Panel_before = self:GetWidget("Panel_before") 
    self.Panel_before:SetVisible(false)
        self.Text_before = self:GetWidget("Text_before") 
        self.Text_beforeNum = self:GetWidget("Text_beforeNum") 
        self.Image_beforeCoin = self:GetWidget("Image_beforeCoin") 
        self.Image_55_0 = self:GetWidget("Image_55_0") 
    --价格容器X2
    self.Panel_after = self:GetWidget("Panel_after") 
    self.Panel_after:SetVisible(false)
        self.Text_after = self:GetWidget("Text_after") 
        self.Text_afterNum = self:GetWidget("Text_afterNum") 
        self.Image_afterCoin = self:GetWidget("Image_afterCoin") 
    --已购买
    self.Image_HaveBuy = self:GetWidget("Image_HaveBuy") 
    self.Image_HaveBuy:SetVisible(false)
end

function GUI.TemplDivinationDestinyShopCell:__ShowUI(data)
    --头像
    local icon = LogicGUI.IconLayer.New( false )
    self:GetWidget("Panel_head"):AddChild(icon:GetNode())
    PosTool.Center(icon)
    icon:SetData({id = data.object_id , num = data.object_num })

    --价格
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "short_price_bg")
    if #data.cost_type_list == 1 then 
        local layer = Util:Sprite(sp)
        layer:SetPosition( self.Image_2:GetContentWidth()/2 , self.Image_coinBg:GetPositionY() )
        self.Image_2:AddChild(layer)

        --图标
        local consume = Util:MoneyControl(data.cost_type_list[1] , 0.8)
        consume:SetPosition(0 ,layer:GetContentHeight()/2)
        consume:SetAnchorPoint(0,0.5)
        layer:AddChild(consume)

        --数量
        local consumeNum = Util:Label(data.cost_value_list[1] , Macros.TypefaceSize.normal)
        layer:AddChild(consumeNum)
        PosTool.RightTo( consume , consumeNum )
    else
        local interval = self.Image_2:GetContentWidth() / #data.cost_type_list
        local tp = Util:Sprite(sp)
        local layer = Util:Layout( tp:GetContentWidth() * #data.cost_type_list + 25  , tp:GetContentHeight() )
        layer:SetAnchorPoint( 0.5 , 0.5 )
        layer:SetPosition( self.Image_2:GetContentWidth()/2 , self.Image_coinBg:GetPositionY() )
        self.Image_2:AddChild(layer)

        local layout = {}
        for i,v in ipairs(data.cost_type_list) do
            layout[i] = Util:Sprite(sp)
            local x = 0
            if layout[i-1] then
                x = layout[i-1]:GetContentWidth() + 25 
            end

            layout[i]:SetPosition( x ,  layer:GetContentHeight()/2 )
            layout[i]:SetAnchorPoint( 0 , 0.5 )
            layer:AddChild(layout[i])

            --图标
            local consume = Util:MoneyControl(data.cost_type_list[i] , 0.8)
            consume:SetPosition(0  ,layout[i]:GetContentHeight()/2)
            consume:SetAnchorPoint(0,0.5)
            layout[i]:AddChild(consume)

            --数量
            local consumeNum = Util:Label(data.cost_value_list[i] , Macros.TypefaceSize.normal )
            layout[i]:AddChild(consumeNum)
            PosTool.RightTo( consume , consumeNum )

        end
    end

    local type,info = ObjIDTool.GetInfo(data.object_id)
    if not info then return end
    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.largish , info.quality or info.init_quality)
    name:SetPosition(self.Panel_name:GetContentWidth()/2  , self.Panel_name:GetContentHeight() /2 )
    name:SetAnchorPoint(0.5,0.5)
    self.Panel_name:AddChild(name)

    local btn = Util:Layout( self.Image_2:GetContentWidth() , self.Image_2:GetContentHeight() , function ( ... )
        self:Transition(data)
    end)
    btn:SetPosition(self.Image_2:GetContentWidth()/2 , self.Image_2:GetContentHeight()/2 )
    btn:SetAnchorPoint(0.5,0.5)
    self.Image_2:AddChild(btn)
end

function GUI.TemplDivinationDestinyShopCell:Transition(data)

    CPPGameLib.PrintTable("xy" , data , "Transition")  

    local t1 = {}

    table.insert(t1,{ 
                     type = data.cost_type_list[1] ,
                     num = data.cost_value_list[1] 
                 })

    local money = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.DESTINY_COIN )

    local info = {}
    info.item_id = data.object_id 
    -- 商店类型
    -- info.type = Macros.Game.SHOP.TOWER
    -- 商品购买id
    info.goods_id = data.id
    -- 价格列表
    info.cost = t1
    -- 最大
    info.max = math.floor( (money or 0) / data.cost_value_list[1] ) 
    -- 单个数量
    info.num = 1
    --标题
    info.title = CPPGameLib.GetString("special_exchange")
    --func
    info.func = function ( num )
        CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.FAIRY , num , true , function ( sum )
            if sum >= num then
                local money = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.DESTINY_COIN )
                local sum = data.cost_value_list[1] * num
                if money >= sum then
                    local data = {}
                    data.param1 = info.goods_id
                    data.param2 = num
                    CallModuleFunc(ModuleType.DIVINATION,"DestinyBuyRequest" , data )
                else
                    local txt = "currency_insufficient_".. Macros.Game.RolePropType.DESTINY_COIN
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
                    CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.DESTINY_COIN )
                end
            end
        end )
    end

    GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
end

function GUI.TemplDivinationDestinyShopCell:GetData( ... )
    return self.data
end