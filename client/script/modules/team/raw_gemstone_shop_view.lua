--[[
%% @module: 原石商店界面
%% @author: ldx
%% @created: 2017-09-25
--]]

Modules = Modules or {}

Modules.RawGemStoneShopView = Modules.RawGemStoneShopView or BaseClass(BaseView)

function Modules.RawGemStoneShopView:__init()
    self.__layout_name = "union_shop"   
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("team_shop_name"), 
        show_gem = true,
        show_diamond = true,
        show_gold = true,
        show_power = true,
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__tab_params_ex = {15 , true}

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM) 
end

function Modules.RawGemStoneShopView:__RegistAllEvents()
    --匹配成功关闭界面
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function(_,is_match)
        if is_match then
            self:CloseManual()
        end
    end)
    --组队副本商店购买记录响应
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_SHOP_RECORD_RESP, function()
        local info = self:GetDataListByIndex(self.__selected_index)
        self.__raw_gemstone_scrollview:SetDataList(info)
    end)
end

function Modules.RawGemStoneShopView:__Dispose()
    if self.__raw_gemstone_scrollview then
        self.__raw_gemstone_scrollview:DeleteMe()
        self.__raw_gemstone_scrollview = nil
    end
end

function Modules.RawGemStoneShopView:__OpenCallback()  
    --背景
    self.__image_content = self:GetWidget("Image_content")  
    --时间背景
    self.__image_time = self:GetWidget("Image_time")
    self.__image_time:SetVisible(false)
    --页签控件
    self.__panel_tab = self:GetWidget("Panel_tab")
    self:__ShowUI()
end

function Modules.RawGemStoneShopView:__ShowUI()
    local params =  
    {
        item_class = Modules.RawGemStoneShopItem,
        item_width = 463,
        item_height = 135,
        row = 4,
        col = 2,
        -- item_space_c = 0 ,
        view_height = self.__image_content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.__raw_gemstone_scrollview = GUI.ScrollView.New(params)
    self.__image_content:AddChild(self.__raw_gemstone_scrollview:GetNode())
    PosTool.Center(self.__raw_gemstone_scrollview:GetNode())

    local text_list = {
        [1] = CPPGameLib.GetString("union_prop") ,    
        [2] = CPPGameLib.GetString("union_award") ,      
    }

    self.__tab:SetContentSize( self.__panel_tab:GetContentWidth() , self.__panel_tab:GetContentHeight() + 15 )
    self.__tab:SetData( text_list )
    self.__tab:SetItemAlign( 0.5 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  
    self.__tab:SetPosition(self.__panel_tab:GetContentWidth()/2 , self.__panel_tab:GetContentHeight()/2 + 5 )
    self.__panel_tab:AddChild(self.__tab:GetNode())
end

--细胞 
function Modules.RawGemStoneShopView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.btn , nil , nil , Macros.Color.button_red )
    name:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(name)
    PosTool.Center( name )

    return layer
end

--选中
function Modules.RawGemStoneShopView:OnCallBack( index , data )

    if self.__onitem_eff then
        self.__onitem_eff:RemoveSelf(true)
        self.__onitem_eff = nil
    end
    local item = self.__tab:GetBtn( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    self.__onitem_eff = Util:Sprite( com_sp_cbox_wire )
    self.__onitem_eff:SetAnchorPoint( 0.5 , 1 )
    self.__onitem_eff:SetPosition( item:GetContentWidth()/2 , 3 )
    item:AddChild( self.__onitem_eff )
    --选中index
    self.__selected_index = index
    --获取购买记录
    if CallModuleFunc(ModuleType.TEAM,"GetBuyRecordByShopType",index == 1 and Macros.Game.SHOP.GEM_PROP or Macros.Game.SHOP.GEM_REWARD) then
        local info = self:GetDataListByIndex(index)
        self.__raw_gemstone_scrollview:SetDataList(info)
    else
        CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",index == 1 and Macros.Game.SHOP.GEM_PROP or Macros.Game.SHOP.GEM_REWARD)
    end    
end

--获取配置表信息
function Modules.RawGemStoneShopView:GetDataListByIndex(index)
    if not self.__total_datalist then
        self.__total_datalist = {}
        for cur_goods_id,raw_info in pairs(CPPGameLib.GetConfig("RawGemstoneShop",nil,nil,true)) do
            self.__total_datalist[raw_info.goods_type] = self.__total_datalist[raw_info.goods_type] or {}
            local data = {goods_id = cur_goods_id}
            table.insert(self.__total_datalist[raw_info.goods_type],data)
        end
    end
    return self.__total_datalist[index] or {}
end

Modules = Modules or {}
Modules.RawGemStoneShopItem = Modules.RawGemStoneShopItem or BaseClass(GUI.ScrollItem)

function Modules.RawGemStoneShopItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM)   
end

function Modules.RawGemStoneShopItem:__delete()

end

function Modules.RawGemStoneShopItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.RawGemStoneShopItem:__Getwidget()
    --条目
    self.__image_item = self:GetWidget("Image_item") 
    --头像
    self.__panel_icon = self:GetWidget("Panel_icon") 
    --名字
    self.__text_name = self:GetWidget("Text_name") 
    --内容背景
    self.__image_content = self:GetWidget("Image_content")
    --时间
    self.__text_time = self:GetWidget("Text_time")
    --购买按钮
    self.__btn_buy = Util:WidgetButton( 
                                            self:GetWidget("Button_buy") , 
                                            CPPGameLib.GetString("union_buy") , 
                                            function ()
                                                self:OnButtonBuy()
                                            end,Macros.TypefaceSize.button , Macros.Color.btn, Macros.Color.button_yellow )
end

function Modules.RawGemStoneShopItem:__ShowUI()
    --配置信息
    self.__goods_config = CPPGameLib.GetConfig("RawGemstoneShop",self.__data.goods_id)
    --物品配置
    local type_ , info = ObjIDTool.GetInfo(self.__goods_config.item_id)
    --头像
    local icon = LogicGUI.IconLayer.New(true)
    self.__panel_icon:AddChild(icon:GetNode())
    icon:SetData({id = self.__goods_config.item_id , num = self.__goods_config.item_num })
    PosTool.Center(icon)

    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality or info.init_quality )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.__text_name:GetContentHeight()/2)
    self.__text_name:AddChild(name)

    --价格
    local lst = {}
    local posX = {
            [1] = 5 , 
            [2] = (self.__image_content:GetContentWidth()/2) + 5
    }
    for i , v in ipairs( self.__goods_config.cost_type_list ) do
        lst[i] = Util:MoneyControl( v , 0.8 , 
                                    self.__goods_config.cost_value_list[i] , 
                                    Macros.TypefaceSize.normal , 
                                    Macros.Color.content , 2 )
        lst[i]:SetAnchorPoint( 0 , 0.5 )
        lst[i]:SetPosition( posX[i] , self.__image_content:GetContentHeight() - lst[i]:GetContentHeight() + 10 )
        self.__image_content:AddChild( lst[i] )
    end

    if self.__goods_config.goods_type == Macros.Game.RAW_GEMSTONE_SHOP.SHOP then
        self:SetShop()
    else
        self:SetAward()
    end
    local shop_type = Macros.Game.SHOP.GEM_PROP
    if self.__goods_config.goods_type == Macros.Game.RAW_GEMSTONE_SHOP.REWARD then
        shop_type = Macros.Game.SHOP.GEM_REWARD
    end
    local buy_num = CallModuleFunc(ModuleType.TEAM, "GetBuyRecordByShopTypeAndGoodsId" , shop_type,self.__data.goods_id )
    if self.__goods_config.buy_num_max ~= 0 and buy_num >= self.__goods_config.buy_num_max then
        self.__btn_buy:SetVisible(false)
        self.__limit_conditional:SetVisible(false)
        self.__have_buy_image = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1",true))
        self.__have_buy_image:SetPosition(self.__btn_buy:GetPositionX() , self.__image_item:GetContentHeight()/2 )
        self.__image_item:AddChild(self.__have_buy_image)
    end
end

function Modules.RawGemStoneShopItem:SetShop()
    if self.__limit_conditional then
        self.__limit_conditional:SetVisible(self.__goods_config.buy_num_max ~= 0)
    end
    local shop_type = Macros.Game.SHOP.GEM_PROP
    if self.__goods_config.goods_type == Macros.Game.RAW_GEMSTONE_SHOP.REWARD then
        shop_type = Macros.Game.SHOP.GEM_REWARD
    end
    --是否有数量限制
    if self.__goods_config.buy_num_max ~= 0 then
        local buy_num = CallModuleFunc(ModuleType.TEAM, "GetBuyRecordByShopTypeAndGoodsId" ,shop_type, self.__data.goods_id )
        local text = CPPGameLib.GetString("team_buy_max_times" , self.__goods_config.buy_num_max - buy_num )
        if self.__goods_config.buy_num_max - buy_num <= 0 then
            text = CPPGameLib.GetString("shop_today_can_notbuy" )
            Util:GreyButton( self.__btn_buy )
        end
        if not self.__limit_conditional then
            self:CreateLabel()
        end
        self.__limit_conditional:SetString(text)
    end
end

function Modules.RawGemStoneShopItem:CreateLabel()
    self.__limit_conditional = Util:Label("", Macros.TypefaceSize.normal , Macros.Color.keypoint )
    self.__limit_conditional:SetAnchorPoint( 1 ,0.5 )
    self.__limit_conditional:SetPosition( self.__text_time:GetContentWidth() , self.__text_time:GetContentHeight()/2 )
    self.__text_time:AddChild( self.__limit_conditional )
end

function Modules.RawGemStoneShopItem:SetAward()
    if self.__limit_conditional then
        self.__limit_conditional:SetVisible(self.__goods_config.need_pass_dungeonid ~= 0)
    end
    if self.__goods_config.need_pass_dungeonid ~= 0 then
        if not self.__limit_conditional then
            self:CreateLabel()
        end
        local fb_config = CPPGameLib.GetConfig("TeamFBChapter",self.__goods_config.need_pass_dungeonid)
        self.__limit_conditional:SetString(CPPGameLib.GetString("team_buy_limit_conditional",fb_config and fb_config.dungeon_name or ""))
        local is_could_buy = CallModuleFunc(ModuleType.TEAM, "GetIsPassDungeonRecord",self.__goods_config.need_pass_dungeonid)
        if not is_could_buy then
            Util:GreyButton( self.__btn_buy )
            self.__limit_conditional:SetColor( unpack( Macros.Color.red_color ) )
        else
            self.__btn_buy:SetEnabled( true )
            self.__limit_conditional:SetColor( unpack( Macros.Color.keypoint ) )
        end
    end  
end

function Modules.RawGemStoneShopItem:OnButtonBuy() 

    local text = ""
    for i , prop_type in ipairs( self.__goods_config.cost_type_list ) do
        local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , prop_type )
        if item_num < self.__goods_config.cost_value_list[i] then
            text = CPPGameLib.GetString("currency_insufficient_"..prop_type)
            break
        end
    end

    local shop_type = Macros.Game.SHOP.GEM_PROP
    if self.__goods_config.goods_type == Macros.Game.RAW_GEMSTONE_SHOP.REWARD then
        shop_type = Macros.Game.SHOP.GEM_REWARD
    end

    if text ~= "" then
        GlobalTipMsg.GetInstance():Show( text )
    else

        local buy_num = CallModuleFunc(ModuleType.TEAM, "GetBuyRecordByShopTypeAndGoodsId" ,shop_type,self.__data.goods_id)

        local lst = {}
        local t1 = {}
        for i,prop_type in ipairs(self.__goods_config.cost_type_list ) do
            local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , prop_type )

            table.insert( lst , item_num / self.__goods_config.cost_value_list[i] )

            table.insert(t1,{ 
                             type = prop_type ,        --货币类型
                             haveBuyNum = buy_num ,                                --已经买了的个数
                             num = self.__goods_config.cost_value_list[i],         --什么事情都没有的单价
                         })
        end
        table.sort( lst, function ( ... )
            return a < b
        end )

        if lst[1] > self.__goods_config.buy_num_max and self.__goods_config.buy_num_max ~= 0 then
            lst[1] = self.__goods_config.buy_num_max - buy_num
        end

        local info = {}

        --道具id
        info.item_id = self.__goods_config.item_id 
        --商店类型
        info.type = shop_type 
        --商品购买id
        info.goods_id = self.__data.goods_id
        --价格列表
        info.cost = t1
        -- 最大
        info.max = lst[1]
        --图标显示的单价
        info.num = self.__goods_config.item_num or 1

        GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
    end
end