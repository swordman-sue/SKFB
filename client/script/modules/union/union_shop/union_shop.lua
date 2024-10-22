--
-- @brief 公会商店
-- @author: yjg
-- @date: 2016年10月22日11:11:53
--

Modules = Modules or {}

Modules.UnionShopView = Modules.UnionShopView or BaseClass(BaseView)
function Modules.UnionShopView:__init()
    self.__layout_name = "union_shop"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("union_shop"),
        show_contribution = true ,
        show_diamond = true,
        show_gold = true,
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__tab_params_ex = {15 , true}
        
    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM) 
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION)
end

function Modules.UnionShopView:__Dispose()
    if self.screen then
        GlobalTimerQuest:CancelQuest(self.screen)
        self.screen = nil
    end       
end

function Modules.UnionShopView:__OpenCallback()    
    
    self:__Getwidget()
    local unionData = CallModuleFunc(ModuleType.UNION , "UnionData" )
    if not unionData then
        CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "union" )
    else
        self:__ShowUI()
        self:__Server()
    end
    --公会信息
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_DATE , function (_, data)
        self:__ShowUI()
        self:__Server()
    end)
end

--数据
function Modules.UnionShopView:__Server()
    local lst = CallModuleFunc(ModuleType.UNION , "GetBuyRecord" )
    if CPPGameLib.GetTableLength(lst) <= 0 then
        local lst = {
                [1] = Macros.Game.SHOP.UNIONPROP ,
                [2] = Macros.Game.SHOP.UNIONAWARDPROP ,
        }
        for i,v in ipairs( lst ) do
            CallModuleFunc(ModuleType.UNION , "GetMyBuyRecordReq" , v )
        end
    end

    local index = 0
    --购买记录
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_BUY_RECORD , function (_, data)
        self.__tab:On(1)
    end)

    --购买派发
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_BUY , function (_, type)
        if type == Macros.Game.SHOP.UNIONPROP then
            self.__tab:On( 1 )
        elseif type == Macros.Game.SHOP.UNIONAWARDPROP then
            self.__tab:On( 2 )
        end
    end)
end

function Modules.UnionShopView:__Getwidget( ... )

    self.Image_bg = self:GetWidget("Image_bg")

    self.Image_bg2 = self:GetWidget("Image_bg2")

    self.Image_content = self:GetWidget("Image_content")
    
    self.Panel_tab = self:GetWidget("Panel_tab")

    self.Image_time = self:GetWidget("Image_time")
    
    self.Text_time = self:GetWidget("Text_time")
end

function Modules.UnionShopView:__ShowUI( ... )

    local params =  
    {
        item_class = Modules.UnionShopItem,
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
            -- [2] = CPPGameLib.GetString("union_deseno") ,
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

    self.Image_time:SetVisible(false)
    -- self.Text_time = Util:WidgetLabel(self.Text_time , CPPGameLib.GetString("union_UpTimeLimitTime" , 0) , Macros.TypefaceSize.normal , Macros.Color.green ) 
end

--细胞 
function Modules.UnionShopView:Ceil( index , data )
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
function Modules.UnionShopView:OnCallBack( index , data )

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

function Modules.UnionShopView:SetShowUi( index )
    self.index = index
    self:SetRed()
    local lst = {
            [1] = "GetPropShop",
            -- [2] = "GetDesenoShop",
            [2] = "GetAwardShop",
    }
    self.lst_list:SetDataList( CallModuleFunc( ModuleType.UNION , lst[index] ) )
    -- if index == 2 then
    --     self.Image_time:SetVisible(true)
    -- else
    --     self.Image_time:SetVisible(false)
    -- end
end

function Modules.UnionShopView:Countdown( index )
    if self.countdownType == true then
        return 
    end
    self.countdownType = true
    local sTime = CallModuleFunc(ModuleType.UNION , "GetDesenoTime" )

    local func = nil
    local function moveAnimate( ... )
        local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        --当前服务器时间
        local endTime = sTime - serverTime

        if endTime <= 0 then
            self.desenoKsy = true
            CPPActionManager.StopAllActions(self.Image_bg)
            CallModuleFunc(ModuleType.UNION , "ShopLimitGoodsListReq" , Macros.Game.SHOP.UNIONTIMEPROP )
        else
            self.desenoKsy = false
            self.Text_time:SetText( CPPGameLib.GetString("union_UpTimeLimitTime" , TimeTool.TransTimeStamp("HH:MM:SS" , endTime ) ) )
            CPPActionManager.DelayTo(self.Image_bg , 1 , func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

function Modules.UnionShopView:SetRed( index )
    if not self.red then
        local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
        self.red = {}
        for i = 2 , 2 do
            local item = self.__tab:GetBtn(i)
            self.red[i] = Util:Sprite( icon_red_point )
            self.red[i]:SetAnchorPoint( 0.5 , 0.5 )
            self.red[i]:SetPosition( item:GetContentWidth() - 5 , item:GetContentHeight() - 5 )
            self.red[i]:SetVisible(false)
            item:AddChild( self.red[i] )
        end
    end

    local lst , redLet = CallModuleFunc(ModuleType.UNION, "GetUnionRedLst" )
   
    -- if lst[ Macros.Game.UNION_RED.DESENO_SHOP ] and self.index ~= 2 then
    --     self.red[2]:SetVisible(true)
    -- elseif self.index == 2 then
    --     lst[ Macros.Game.UNION_RED.DESENO_SHOP ] = nil
    --     self.red[2]:SetVisible(false)
    --     local t1 = redLet
    --     for i,v in ipairs(t1) do
    --         if v.type == Macros.Game.UNION_RED.DESENO_SHOP then
    --             table.remove(redLet , i )
    --         end
    --     end
    -- end

    local type_ = CallModuleFunc(ModuleType.UNION, "GetBuyRecordType" )
    CPPGameLib.PrintTable("xy" , lst , "GetBuyRecordType")
    if lst[ Macros.Game.UNION_RED.AWARD ] and type_ then
        self.red[2]:SetVisible(true)
    else
        self.red[2]:SetVisible(false)
    end
end








Modules = Modules or {}
Modules.UnionShopItem = Modules.UnionShopItem or BaseClass(GUI.ScrollItem)

function Modules.UnionShopItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.UNION_SHOP_ITEM)   
end

function Modules.UnionShopItem:__delete()

end

function Modules.UnionShopItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
    self:__ShowUI(data)
end

function Modules.UnionShopItem:__Getwidget( data )
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
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1" , true)
    self.buy = Util:Sprite( sp )
    self.buy:SetAnchorPoint( 0.5 , 0.5 )
    self.buy:SetPosition( self.Button_buy:GetPositionX() , self.Image_item:GetContentHeight()/2 )
    self.Image_item:AddChild( self.buy )
    self.buy:SetVisible(false)
end

function Modules.UnionShopItem:__ShowUI(data)
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
    
    local lst = {}
    local posX = {
            [1] = 5 , 
            [2] = (self.Image_content:GetContentWidth()/2) + 5
    }
    for i,v in ipairs( data.cost_type_list ) do
        lst[i] = Util:MoneyControl( v , 0.8 , 
                                    data.cost_value_list[i] , 
                                    Macros.TypefaceSize.normal , 
                                    Macros.Color.content , 2 )
        lst[i]:SetAnchorPoint( 0 , 0.5 )
        lst[i]:SetPosition( posX[i] , self.Image_content:GetContentHeight() - lst[i]:GetContentHeight() + 10 )
        self.Image_content:AddChild( lst[i] )
    end

    local text , color = self:ShowTips( data )
    if data.type ~= Macros.Game.SHOP.UNIONTIMEPROP then
        print("xy" , "==========1")
        if text ~= CPPGameLib.GetString( "shop_limit_buy_num" ) then
            --持有数量
            Util:WidgetLabel( self.Text_time , text , Macros.TypefaceSize.normal , color )
        else
            self.Button_buy:SetVisible(false)
            self.buy:SetVisible(true)
        end
    else
        if data.buy_num_max <= data.buyRecord then
            text = CPPGameLib.GetString( "shop_limit_buy_num" )
        else
            -- "军团剩余%s件"
            text = CPPGameLib.GetString( "union_DesenoShop" , data.buy_num_max - data.buy_num )

            local text_ , color_ = self:ShowTips( data )
            if text_ ~= "" then
                text_ = CPPGameLib.GetString( "shop_today_couldBuyTimes" , 1 - data.buyRecord )
            end

            if text_ ~= CPPGameLib.GetString( "shop_limit_buy_num" ) then
                --持有数量
                Util:WidgetLabel( self.Text_time , text_ , Macros.TypefaceSize.normal , color )
            else
                self.Button_buy:SetVisible(false)
                self.buy:SetVisible(true)
            end

            local discount = LogicGUI.LconDiscount.New( data.discount )
            discount:SetAnchorPoint( 0.5 , 0.5 )
            discount:SetPosition( 20 , icon:GetNode():GetContentHeight() )
            icon:AddChild( discount:GetNode() , 50 )

        end

        local time = Util:Label( 
                                    text , 
                                    Macros.TypefaceSize.normal , 
                                    Macros.Color.keypoint
                                )
        time:SetAnchorPoint(0,0.5)
        time:SetPosition( 5 , lst[1]:GetPositionY() - time:GetContentHeight() - 8 )
        self.Image_content:AddChild( time )
    end

end

function Modules.UnionShopItem:OnButtonBuy(data)
    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local unionLevel = CallModuleFunc(ModuleType.UNION,"GetUnionLevel")

    local text = ""

    for i,v in ipairs( data.cost_type_list ) do
        local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v )
        if item_num < data.cost_value_list[i] then
            text = CPPGameLib.GetString( "currency_insufficient_" .. v )
            break
        end
    end


    if data.need_role_level > level then
        text = CPPGameLib.GetString( "union_noLevel" , data.need_role_level )
    elseif data.need_guild_level > unionLevel then
        text = CPPGameLib.GetString( "union_noCorps" , data.need_guild_level )
    elseif data.buy_type == 0 then
        if data.buy_num_max <= data.buyRecord then
            text = CPPGameLib.GetString( "shop_today_can_notbuy" )
        end
    elseif data.buy_type == 1 then
        if data.buy_num_max <= data.buyRecord then
            text = CPPGameLib.GetString( "shop_limit_buy_num" )
        end
    end

    if text ~= "" then
        GlobalTipMsg.GetInstance():Show( text )
    else
        local lst = {}
        local t1 = {}
        for i,v in ipairs(data.cost_type_list ) do
            local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v )

            table.insert( lst , item_num / data.cost_value_list[i] )

            table.insert(t1,{ 
                             type = v ,        --货币类型
                             haveBuyNum = data.buyRecord,                                --已经买了的个数
                             num = data.cost_value_list[i],                                                     --什么事情都没有的单价
                         })
        end
        table.sort( lst, function ( ... )
            return a < b
        end )

        if lst[1] > data.buy_num_max and data.buy_num_max ~= 0 then
            lst[1] = data.buy_num_max - data.buyRecord
        end

        local info = {}
        --道具id
        info.item_id = data.item_id 
        --商店类型
        info.type = data.type 
        --商品购买id
        info.goods_id = data.goods_id
        --价格列表
        info.cost = t1
        -- 最大
        info.max = lst[1]
        GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
    end



end

function Modules.UnionShopItem:ShowTips( data )

    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local unionLevel = CallModuleFunc(ModuleType.UNION,"GetUnionLevel")

    local text = ""
    local color = Macros.Color.keypoint

    if data.need_role_level > level then

        color = Macros.Color.red_color
        text = CPPGameLib.GetString( "union_noLevel" , data.need_role_level )

    elseif data.need_guild_level > unionLevel then

        color = Macros.Color.red_color
        text = CPPGameLib.GetString( "union_noCorps" , data.need_guild_level )

    else
        if data.buy_type == 0 then
            
            if data.buy_num_max <= data.buyRecord then
                color = Macros.Color.red_color
                text = CPPGameLib.GetString( "shop_today_can_notbuy" )
            else
                text = CPPGameLib.GetString( "shop_today_couldBuyTimes" , data.buy_num_max - data.buyRecord )
            end

        elseif data.buy_type == 1 then

            if data.buy_num_max <= data.buyRecord then
                color = Macros.Color.red_color
                text = CPPGameLib.GetString( "shop_limit_buy_num" )
            else
                text = CPPGameLib.GetString( "shop_can_buymax" , data.buy_num_max - data.buyRecord )
            end
        end
    end

    return text , color
end