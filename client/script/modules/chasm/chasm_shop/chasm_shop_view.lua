--[[
%% @module: 无尽深渊
%% @author: yjg
%% @created: 2017年7月17日17:31:09
--]]

Modules = Modules or {} 
Modules.ChasmShopView = Modules.ChasmShopView or BaseClass(BaseView)

function Modules.ChasmShopView:__init()
    self.__layout_name = "union_shop"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("chasm_shop"),
        show_endless = true ,
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

function Modules.ChasmShopView:__OpenCallback( )
    self:__Getwidget()
    self:__Server()
    self:__ShowUI()
end 

function Modules.ChasmShopView:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.ChasmShopView:__Server()

    CallModuleFunc(ModuleType.CHASM , "ShopCnf") 

    CallModuleFunc(ModuleType.CHASM , "ChasmBuyNotesRequest") 
    --物品获得
    self:BindGlobalEvent(Macros.Event.ChasmModule.CHASM_SHOP , function (_, text)
        self:SetShowUi( self.index or 1 )
    end)
end

function Modules.ChasmShopView:__Getwidget( )
    self.Image_bg = self:GetWidget("Image_bg")

    self.Image_bg2 = self:GetWidget("Image_bg2")

    self.Image_content = self:GetWidget("Image_content")
    
    self.Panel_tab = self:GetWidget("Panel_tab")


    self.Image_time = self:GetWidget("Image_time")
    self.Image_time:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg") )
    self.Image_time:SetContentSize( 303 , 40 )

    self.Text_time = self:GetWidget("Text_time")
    self.Text_time:SetPosition( 10 , self.Text_time:GetPositionY() )
end 

function Modules.ChasmShopView:__ShowUI()
    local params =  
    {
        item_class = Modules.ChasmShopViewItem,
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
            [1] = CPPGameLib.GetString("union_prop") ,
            [2] = CPPGameLib.GetString("union_award") ,           
    }

    self.__tab:SetContentSize( self.Panel_tab:GetContentWidth() , self.Panel_tab:GetContentHeight() + 15 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0.5 )


    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  
    self.__tab:SetPosition(self.Panel_tab:GetContentWidth()/2 , self.Panel_tab:GetContentHeight()/2 + 5 )
    self.Panel_tab:AddChild(self.__tab:GetNode())

    local num = CallModuleFunc(ModuleType.CHASM, "GetKeepKillNum" )
    self.Text_time = Util:WidgetLabel(self.Text_time , CPPGameLib.GetString("chasm_myKillRecord" , num ) , Macros.TypefaceSize.normal , Macros.Color.green ) 
end

--细胞 
function Modules.ChasmShopView:Ceil( index , data )
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
function Modules.ChasmShopView:OnCallBack( index , data )

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

function Modules.ChasmShopView:SetShowUi( index )
    self.index = index
    if index == Macros.Game.CHASM_SHOP.SHOP then
        self.Image_time:SetVisible(false)

        local info = CallModuleFunc(ModuleType.CHASM, "GetShopCnf" )
        self.lst_list:SetDataList( info ) 
    else
        self.Image_time:SetVisible(true)

        local info = CallModuleFunc(ModuleType.CHASM, "GetAwardCnf" )
        self.lst_list:SetDataList( info ) 
    end

end

Modules = Modules or {}
Modules.ChasmShopViewItem = Modules.ChasmShopViewItem or BaseClass(GUI.ScrollItem)

function Modules.ChasmShopViewItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM)   
end

function Modules.ChasmShopViewItem:__delete()

end

function Modules.ChasmShopViewItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
    self:__ShowUI(data)
end

function Modules.ChasmShopViewItem:__Getwidget( data )
    --条目
    self.Image_item = self:GetWidget("Image_item") 
    --头像
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --名字
    self.Text_name = self:GetWidget("Text_name") 
    --内容背景
    self.Image_content = self:GetWidget("Image_content")
    --时间
    self.Text_time = self:GetWidget("Text_time")
    --购买按钮
    self.Button_buy = Util:WidgetButton( 
                                            self:GetWidget("Button_buy") , 
                                            CPPGameLib.GetString("union_buy") , 
                                            function ( ... )
                                                self:OnButtonBuy( data )
                                            end,nil , nil , Macros.Color.button_yellow )
    --新手指引特殊操作检测(深渊商店-购买（商品ID）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.CHASM_SHOP_BUY, self.Button_buy, data.id)
end

function Modules.ChasmShopViewItem:__ShowUI(data)
    local type_ , info = ObjIDTool.GetInfo(data.item_id)
    --头像
    local icon = LogicGUI.IconLayer.New(true)
    self.Panel_icon:AddChild(icon:GetNode())
    icon:SetData({id = data.item_id , num = data.item_num })
    PosTool.Center(icon)

    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality or info.init_quality )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2)
    self.Text_name:AddChild(name)

    --价格
    local lst = {}
    local posX = {
            [1] = 5 , 
            [2] = (self.Image_content:GetContentWidth()/2) + 5
    }
    for i , v in ipairs( data.cost_type_list ) do
        lst[i] = Util:MoneyControl( v , 0.8 , 
                                    data.cost_value_list[i] , 
                                    Macros.TypefaceSize.normal , 
                                    Macros.Color.content , 2 )
        lst[i]:SetAnchorPoint( 0 , 0.5 )
        lst[i]:SetPosition( posX[i] , self.Image_content:GetContentHeight() - lst[i]:GetContentHeight() + 10 )
        self.Image_content:AddChild( lst[i] )
    end

    self:Transfer( data )

end

function Modules.ChasmShopViewItem:Transfer( data )
    if self.astrict then
        self.astrict:RemoveSelf(true)
        self.astrict = nil
    end

    if data.goods_type == Macros.Game.CHASM_SHOP.SHOP then
        self:SetShop( data )
    else
        self:SetAward( data )
    end

    local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if roleLevel < data.need_role_level then
        self:SetLv( data )
    end

    local buyRecord = CallModuleFunc(ModuleType.CHASM, "GetChasmRequest" , data.id )
    if buyRecord and data.buy_num_max ~= 0 and buyRecord.buy_num >= data.buy_num_max then
        self:SetBuy( data )
    end
end


function Modules.ChasmShopViewItem:SetLv( data )

    self.astrict = Util:Label( CPPGameLib.GetString("chasm_lvAstrict" , data.need_role_level ) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    self.astrict:SetAnchorPoint( 1 ,0.5 )
    self.astrict:SetPosition( self.Text_time:GetContentWidth() , self.Text_time:GetContentHeight()/2 )
    self.Text_time:AddChild( self.astrict )

    Util:GreyButton( self.Button_buy )
end

function Modules.ChasmShopViewItem:SetShop( data )

    if data.buy_num_max ~= 0 then
        local buyRecord = CallModuleFunc(ModuleType.CHASM, "GetChasmRequest" , data.id )
        local num = 0
        if buyRecord then
            num = buyRecord.buy_num
        end 

        local text = CPPGameLib.GetString("chasm_shopAstrict" , data.buy_num_max - num )
        if data.buy_num_max - num <= 0 then
            text = CPPGameLib.GetString("shop_today_can_notbuy" )
            Util:GreyButton( self.Button_buy )
        end
        self.astrict = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        self.astrict:SetAnchorPoint( 1 ,0.5 )
        self.astrict:SetPosition( self.Text_time:GetContentWidth() , self.Text_time:GetContentHeight()/2 )
        self.Text_time:AddChild( self.astrict )
    end
end

function Modules.ChasmShopViewItem:SetAward( data )

    if data.need_kill_monster_num ~= 0 then
        self.astrict = Util:Label( CPPGameLib.GetString("chasm_awardAstrict" , data.need_kill_monster_num ) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        self.astrict:SetAnchorPoint( 1 ,0.5 )
        self.astrict:SetPosition( self.Text_time:GetContentWidth() , self.Text_time:GetContentHeight()/2 )
        self.Text_time:AddChild( self.astrict )
    end

    local num = CallModuleFunc(ModuleType.CHASM, "GetKeepKillNum" )

    if num < data.need_kill_monster_num then
        Util:GreyButton( self.Button_buy )
        self.astrict:SetColor( unpack( Macros.Color.red_color ) )
    else
        self.Button_buy:SetEnabled( true )
        self.astrict:SetColor( unpack( Macros.Color.keypoint ) )
    end
end

function Modules.ChasmShopViewItem:SetBuy( data )
    self.Button_buy:SetVisible(false)
    self.astrict:SetVisible(false)

    self.getIcon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1",true))
    self.getIcon:SetPosition(self.Button_buy:GetPositionX() , self.Image_item:GetContentHeight()/2 )
    self.Image_item:AddChild(self.getIcon)
end


function Modules.ChasmShopViewItem:OnButtonBuy(data)
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

        local buyRecord = CallModuleFunc(ModuleType.CHASM, "GetChasmRequest" , self.__data.id )

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

        local type_ = Macros.Game.SHOP.CHASM_SHOP
        if data.goods_type == Macros.Game.CHASM_SHOP.AWARD then
            type_ = Macros.Game.SHOP.CHASM_AWARD
        end


        local info = {}

        --道具id
        info.item_id = data.item_id 
        --商店类型
        info.type = type_ 
        --商品购买id
        info.goods_id = data.id
        --价格列表
        info.cost = t1
        -- 最大
        info.max = lst[1]
        --图标显示的单价
        info.num = data.item_num or 1

        GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
    end
end
