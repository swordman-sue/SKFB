--[[
%% @module: 资源战商店
%% @author: yjg
%% @created: 2018年8月14日19:17:32
--]]

Modules = Modules or {} 
Modules.ResourceWarView = Modules.ResourceWarView or BaseClass(BaseView)

function Modules.ResourceWarView:__init()
    self.__layout_name = "union_shop"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("resources_war_shop"),
        show_resource = true ,
        show_diamond = true,
        show_gold = true,
        -- show_stamina = true,
        -- show_energy = true,
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__tab_params_ex = {15 , true}

    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM) 
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM,true)
end

function Modules.ResourceWarView:__OpenCallback( )
    self:__Getwidget()
    self:__Server()
    self:__ShowUI()
end 

function Modules.ResourceWarView:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.ResourceWarView:__Server()

    CallModuleFunc(ModuleType.RESOURCE_WAR , "ShopCnf") 

    CallModuleFunc(ModuleType.RESOURCE_WAR , "ResourceWarBuyNotesRequest") 

    --物品获得
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_SHOP , function (_, up)
        self:SetShowUi( self.index or 1 , up )
    end)

end

function Modules.ResourceWarView:__Getwidget( )
    self.Image_bg = self:GetWidget("Image_bg")

    self.Image_bg2 = self:GetWidget("Image_bg2")

    self.Image_content = self:GetWidget("Image_content")
    
    self.Panel_tab = self:GetWidget("Panel_tab")


    self.Image_time = self:GetWidget("Image_time")
    self.Image_time:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg") )
    self.Image_time:SetContentSize( 303 , 40 )
    self.Image_time:SetVisible(false)

    self.Text_time = self:GetWidget("Text_time")
    self.Text_time:SetPosition( 10 , self.Text_time:GetPositionY() )
end 

function Modules.ResourceWarView:__ShowUI()
    local params =  
    {
        item_class = Modules.ResourceWarViewItem,
        item_width = 463.635,
        item_height = 135.64,
        row = 4,
        col = 2,
        -- item_space_c = 0 ,
        view_height = self.Image_content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    local text = {
            [1] = CPPGameLib.GetString("resources_war_shop") ,
    }

    self.__tab:SetContentSize( self.Panel_tab:GetContentWidth() , self.Panel_tab:GetContentHeight() + 15 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0.5 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  
    self.__tab:SetPosition(self.Panel_tab:GetContentWidth()/2 , self.Panel_tab:GetContentHeight()/2 + 5 )
    self.Panel_tab:AddChild(self.__tab:GetNode())
end

--细胞 
function Modules.ResourceWarView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.white , nil , nil , Macros.Color.button_red )
    name:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(name)
    PosTool.Center( name )

    return layer
end


--选中
function Modules.ResourceWarView:OnCallBack( index , data )

    if self.OnItemEff then
        self.OnItemEff:RemoveSelf(true)
        self.OnItemEff = nil
    end
    local item = self.__tab:GetBtn( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    self.OnItemEff = Util:Sprite( com_sp_cbox_wire )
    self.OnItemEff:SetAnchorPoint( 0.5 , 1 )
    self.OnItemEff:SetPosition( item:GetContentWidth()/2 , 3 )
    item:AddChild( self.OnItemEff )

    self:SetShowUi(index)
end

function Modules.ResourceWarView:SetShowUi( index , up )
    if not up then
        self.index = index
        local info = CallModuleFunc(ModuleType.RESOURCE_WAR, "GetShopCnf" )
        self.lst_list:SetDataList( info ) 
    else
        self.lst_list:ItemIter( function(index, item , data )
            if item then
                item:Transfer( data )
            end
        end )
    end
end


Modules = Modules or {}
Modules.ResourceWarViewItem = Modules.ResourceWarViewItem or BaseClass(GUI.ScrollItem)

function Modules.ResourceWarViewItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM)   
end

function Modules.ResourceWarViewItem:__delete()

end

function Modules.ResourceWarViewItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
    self:__ShowUI(data)
end

function Modules.ResourceWarViewItem:__Getwidget( data )
    --条目
    self.Image_item = self:GetWidget("Image_item") 
    --头像
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --名字
    self.Text_name = self:GetWidget("Text_name") 
    --内容背景
    self.Image_content = self:GetWidget("Image_content")
    self.Image_content:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_num") )
    self.Image_content:SetContentSize( 150 , 35 )
    self.Image_content:SetPosition( self.Image_content:GetPositionX() - 25 , self.Image_content:GetPositionY() )
    --时间
    self.Text_time = self:GetWidget("Text_time")
    --购买按钮
    self.Button_buy = Util:WidgetButton( 
                                            self:GetWidget("Button_buy") , 
                                            CPPGameLib.GetString("union_buy") , 
                                            function ( ... )
                                                self:OnButtonBuy( data )
                                            end,nil , nil , Macros.Color.button_yellow )
end

function Modules.ResourceWarViewItem:__ShowUI(data)
    local type_ , info = ObjIDTool.GetInfo(data.item_id)
    --头像
    local icon = LogicGUI.IconLayer.New(true)
    self.Panel_icon:AddChild(icon:GetNode())
    icon:SetData({id = data.item_id , num = data.item_num })
    PosTool.Center(icon)

    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality or info.init_quality )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2 - 5 )
    self.Text_name:AddChild(name)

    --价格
    local lst = {}
    local posX = {
            [1] = 0-10 , 
            [2] = (self.Image_content:GetContentWidth()/2) + 5
    }
    for i , v in ipairs( data.cost_type_list ) do
        lst[i] = Util:MoneyControl( v , 1.1 , 
                                    data.cost_value_list[i] , 
                                    Macros.TypefaceSize.normal , 
                                    Macros.Color.white , 10 )
        lst[i]:SetAnchorPoint( 0 , 0.5 )
        lst[i]:SetPosition( posX[i] , self.Image_content:GetContentHeight()/2  )
        self.Image_content:AddChild( lst[i] )
    end

    self:Transfer( data )

end

function Modules.ResourceWarViewItem:Transfer( data )
    if self.astrict then
        self.astrict:RemoveSelf(true)
        self.astrict = nil
    end

    local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if roleLevel < data.need_role_level then
        self:SetLv( data )
    else
        self:SetShop( data )
    end

    local buyRecord = CallModuleFunc(ModuleType.RESOURCE_WAR, "GetResourceWarRequest" , data.id )
    if buyRecord and data.buy_num_max ~= 0 and buyRecord.buy_num >= data.buy_num_max then
        self:SetBuy( data )
    end
end


function Modules.ResourceWarViewItem:SetLv( data )

    self.astrict = Util:Label( CPPGameLib.GetString("chasm_lvAstrict" , data.need_role_level ) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    self.astrict:SetAnchorPoint( 1 ,0.5 )
    self.astrict:SetPosition( self.Text_time:GetContentWidth() , self.Text_time:GetContentHeight()/2 )
    self.Text_time:AddChild( self.astrict )

    Util:GreyButton( self.Button_buy )
end

function Modules.ResourceWarViewItem:SetShop( data )
    if data.buy_num_max ~= 0 then
        local buyRecord = CallModuleFunc(ModuleType.RESOURCE_WAR, "GetResourceWarRequest" , data.goods_id )
        local num = 0
        if buyRecord then
            num = buyRecord.buy_num
        end 

        local text = nil
        if data.refresh_type == 1 then
            text = CPPGameLib.GetString("resources_war_shopBuyTips_1" , data.buy_num_max - num )
        elseif data.refresh_type == 2 then
            text = CPPGameLib.GetString("resources_war_shopBuyTips_2" , data.buy_num_max - num )
        end

        if data.buy_num_max - num <= 0 then
            text = CPPGameLib.GetString("shop_limit_buy_num" )
            Util:GreyButton( self.Button_buy )
        end
        self.astrict = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        self.astrict:SetAnchorPoint( 1 ,0.5 )
        self.astrict:SetPosition( self.Text_time:GetContentWidth() , self.Text_time:GetContentHeight()/2 )
        self.Text_time:AddChild( self.astrict )
    end
end

function Modules.ResourceWarViewItem:SetBuy( data )
    self.Button_buy:SetVisible(false)
    self.astrict:SetVisible(false)

    self.getIcon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1",true))
    self.getIcon:SetPosition(self.Button_buy:GetPositionX() , self.Image_item:GetContentHeight()/2 )
    self.Image_item:AddChild(self.getIcon)
end


function Modules.ResourceWarViewItem:OnButtonBuy(data)
    CPPGameLib.PrintTable("xy", data ,"self.__data")   

    local text = ""
    local type_ = 0
    for i , v in ipairs( data.cost_type_list ) do
        local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v )
        if item_num < data.cost_value_list[i] then
            text = CPPGameLib.GetString( "currency_insufficient_" .. v )
            type_ = v
            break
        end
    end

    if text ~= "" then
        GlobalTipMsg.GetInstance():Show( text )
        local config = CPPGameLib.GetConfig("HuoBi", type_ )
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config.big_icon)
    else

        local buyRecord = CallModuleFunc(ModuleType.RESOURCE_WAR, "GetResourceWarRequest" , self.__data.id )

        if not buyRecord then
            buyRecord = {}
            buyRecord.buy_num = 0
        end

        local lst = {}
        local t1 = {}
        for i,v in ipairs(data.cost_type_list ) do
            local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v )

            table.insert( lst , item_num / data.cost_value_list[i] )

            table.insert(t1,{ 
                             type = v ,        --货币类型
                             haveBuyNum = buyRecord.buy_num ,                                --已经买了的个数
                             num = data.cost_value_list[i],                                  --什么事情都没有的单价
                         })
        end
        table.sort( lst, function ( ... )
            return a < b
        end )

        if lst[1] > data.buy_num_max and data.buy_num_max ~= 0 then
            lst[1] = data.buy_num_max - buyRecord.buy_num
        end

        local type_ = Macros.Game.SHOP.RESOURCE_WAR_SHOP

        local info = {}

        --道具id
        info.item_id = data.item_id 
        --商店类型
        info.type = type_ 
        --商品购买id
        info.goods_id = data.goods_id
        --价格列表
        info.cost = t1
        -- 最大
        info.max = lst[1]
        --图标显示的单价
        info.num = data.item_num or 1

        GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
    end
end
