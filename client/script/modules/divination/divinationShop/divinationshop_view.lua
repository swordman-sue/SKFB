--[[
%% @module: 占卜星魂商店
%% @author: yjg
%% @created: 2017年11月14日21:07:50
--]]

Modules = Modules or {} 
Modules.DivinationShopView = Modules.DivinationShopView or BaseClass(BaseView)

function Modules.DivinationShopView:__init()
    self.__layout_name = "hero_shop"    
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination_starShop"), 
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

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_HERO_SHOP_ITEM)    
end

function Modules.DivinationShopView:__Dispose()
end

function Modules.DivinationShopView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end 

function Modules.DivinationShopView:__Server( )
    CallModuleFunc(ModuleType.DIVINATION , "StarSoulShopDataReq" )
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_STAR_SHOP , function ( ... )
        self:__SetShowUI()
    end )
end

--获得组件
function Modules.DivinationShopView:__Getwidget( )
    --背景容器
    self.Panel_bg = self:GetWidget("Panel_bg")
    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --页签
    self.Panel_tab = self:GetWidget("Panel_tab")
    --背景2
    self.Image_bg2 = self:GetWidget("Image_bg2")
    --内容版
    self.Image_content = self:GetWidget("Image_content")
    --刷新
    self.Button_flush = Util:WidgetButton( 
                                            self:GetWidget("Button_flush") , 
                                            CPPGameLib.GetString("heroShop_up") ,
                                            CPPGameLib.Handler(self,self.OnButtonFlush) ,
                                            nil,nil,Macros.Color.button_yellow
                                        )
    --提示
    self.Text_tips = self:GetWidget("Text_tips")
    --免费
    self.Text_freely = self:GetWidget("Text_freely")
    --次数
    self.Text_time = self:GetWidget("Text_time")
    --消耗
    self.Text_consume = self:GetWidget("Text_consume")

    self.Button_recommendHero = self:GetWidget("Button_recommendHero")
    self.Button_recommendHero:SetVisible(false)
    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") )
    Button_rule:SetVisible(false)
end

--组件设置
function Modules.DivinationShopView:__ShowUI( )

    --免费刷新次数
    local heroShop_freeFlush = Util:Label( CPPGameLib.GetString("divination_remainTime") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    heroShop_freeFlush:SetAnchorPoint( 0 , 0.5 )
    heroShop_freeFlush:SetPosition( 0 , self.Text_tips:GetContentHeight()/2 )
    self.Text_tips:AddChild( heroShop_freeFlush )

    --免费次数
    self.heroShop_freeFlush = Util:Label( "00100" , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Text_tips:AddChild( self.heroShop_freeFlush )
    PosTool.RightTo( heroShop_freeFlush , self.heroShop_freeFlush )


    local tab = {

                    { zi = CPPGameLib.GetString("divination_consume") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
                    { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.CARD_CURRENCY) , dx = 0.75 , jg = 3 },
                    { zi = ConfigAdapter.Common.GetStarShopRefreshConsume() , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
                    { zi = CPPGameLib.GetString("divination_consume_") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal , jg = 3 },
                }
    local tips = Util:LabCenter(tab)
    tips:SetAnchorPoint( 0 , 0.5 )
    tips:SetPosition( 0 , self.Text_freely:GetContentHeight()/2 )
    self.Text_freely:AddChild( tips )


    local divination_autoUPShop = Util:Label( CPPGameLib.GetString("divination_autoUPShop") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    divination_autoUPShop:SetAnchorPoint( 0 , 0.5 )
    divination_autoUPShop:SetPosition( 520 , self.Text_tips:GetPositionY() )
    self.Image_bg2:AddChild( divination_autoUPShop )

    --倒计时
    self.countdown = Util:Label( "00:00:00" , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Image_bg2:AddChild( self.countdown )
    PosTool.RightTo( divination_autoUPShop , self.countdown )

    --商品列表
    local params =  
    {
        item_class = Modules.TemplDivinationShopItem,
        item_width = 458.74,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        view_width = self.Image_content:GetContentWidth() - 10 ,
        view_height = self.Image_content:GetContentHeight() - 10,
    }
    -- 创建参数
    self.__hero_shop_scrollview = GUI.ScrollView.New(params)
    self.__hero_shop_scrollview:SetTouchEnabled(false)
    self.Image_content:AddChild(self.__hero_shop_scrollview:GetNode())
    PosTool.Center(self.__hero_shop_scrollview:GetNode())

    --页签
    local text =  {
            [1] = CPPGameLib.GetString("divination_starShop") ,
        }

    self.__tab:SetContentSize( self.Panel_tab:GetContentWidth() , self.Panel_tab:GetContentHeight() + 15 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition(self.Panel_tab:GetContentWidth()/2 , self.Panel_tab:GetContentHeight()/2 + 5 )
    self.Panel_tab:AddChild(self.__tab:GetNode())

    self.__tab:On( unpack(self.__open_data) or 1 )
end

function Modules.DivinationShopView:Ceil( index , data )
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
function Modules.DivinationShopView:OnCallBack( index , data )
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
end

--设置界面
function Modules.DivinationShopView:__SetShowUI()
    local lst = CallModuleFunc(ModuleType.DIVINATION , "GetStarSoulShopLst" )
    self.__hero_shop_scrollview:SetDataList(lst)

    local num = CallModuleFunc(ModuleType.DIVINATION , "GetStarSoulShopUpTimes" )
    self.heroShop_freeFlush:SetString( ConfigAdapter.Common.GetStarShopRefreshMax() - num .. "/" .. ConfigAdapter.Common.GetStarShopRefreshMax() )

    self:Divination()
end

function Modules.DivinationShopView:OnButtonFlush()
    local num = CallModuleFunc(ModuleType.DIVINATION , "GetStarSoulShopUpTimes" )
    if ConfigAdapter.Common.GetStarShopRefreshMax() - num == 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_notRemainTime"))
        return 
    end

    local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.CARD_CURRENCY )
    if ConfigAdapter.Common.GetStarShopRefreshConsume() > item_num then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("currency_insufficient_" .. Macros.Game.RolePropType.CARD_CURRENCY ))
        CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.CARD_CURRENCY )
        return
    end

    local info = {}
    info.refresh_type = 2
    CallModuleFunc(ModuleType.DIVINATION , "DestinyStarShopRefreshReq" , info )
end

-- CD时间
function Modules.DivinationShopView:Divination()
    --结束的时间
    local time_1 = CallModuleFunc(ModuleType.DIVINATION, "GetStarSoulShopTime" ) + ( ConfigAdapter.Common.GetStarShopRefreshTime() * 60 )

    local num = CallModuleFunc(ModuleType.DIVINATION , "GetStarSoulShopUpTimes" )
    if ConfigAdapter.Common.GetStarShopRefreshMax() - num == 0 then
        self.countdown:SetString( "" )
        return 
    end

    CPPActionManager.StopAllActions(self.Panel_bg)
    local func = nil
    local function moveAnimate()
        local time = time_1 - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if time <= 0 then
            local info = {}
            info.refresh_type = 1
            CallModuleFunc(ModuleType.DIVINATION , "DestinyStarShopRefreshReq" , info )
        else
            local endTime = TimeTool.TransTimeStamp("HH:MM:SS" , time )
            self.countdown:SetString( endTime )
            CPPActionManager.DelayTo(self.Panel_bg , 1 , func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end






--
-- @brief 星魂商店条目 
-- @author: yjg
-- @date: 2017年11月30日20:55:05
--

Modules = Modules or {}

Modules.TemplDivinationShopItem = Modules.TemplDivinationShopItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDivinationShopItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_HERO_SHOP_ITEM) 
    self:__Getwidget()
end

function Modules.TemplDivinationShopItem:__delete()
end

function Modules.TemplDivinationShopItem:SetData(data)
     GUI.ScrollItem.SetData(self, data)   
    self.data = data
    self:__ShowUI(self.data)
end

function Modules.TemplDivinationShopItem:SetViewIndex(var)
    GUI.ScrollItem.SetViewIndex(self, var)

    --新手指引特殊操作检测(英雄商店-购买商品（商品索引，从1开始）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_SHOP_BUY, self.Button_buy, self:GetViewIndex())    
end

function Modules.TemplDivinationShopItem:__AddToParent()
end

function Modules.TemplDivinationShopItem:__Getwidget( ... )
    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --名称
    self.Text_name = self:GetWidget("Text_name")
    --购买
    self.Button_buy = Util:WidgetButton( 
                                            self:GetWidget("Button_buy") , 
                                            CPPGameLib.GetString("tower_shop") ,
                                            CPPGameLib.Handler(self,self.OnButtonGain) ,
                                            nil,nil,Macros.Color.button_yellow
                                            )
    --价钱
    self.Panel_2 = self:GetWidget("Panel_2")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    self.Panel_icon:SetPosition(self.Panel_icon:GetPositionX() ,self.Panel_icon:GetPositionY() )
end

function Modules.TemplDivinationShopItem:__ShowUI( data )

    --图标    
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = data.cnf.item_id , num = data.cnf.item_num})
    self.Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)

    --名字   
    local type_ , info = ObjIDTool.GetInfo( data.cnf.item_id )
    self.name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality or info.init_quality)
    self.name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    self.name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(self.name)

    --消耗
    self.consumeType = data.cnf.cost_type_list[1]
    self.consumeNum = data.cnf.cost_value_list[1]

    self.consume = Util:MoneyControl(data.cnf.cost_type_list[1] , 0.8 )
    self.consume:SetPosition( 0 , self.Panel_2:GetContentHeight()/2)
    self.consume:SetAnchorPoint(0,0.5)
    self.Panel_2:AddChild(self.consume)

    self.consumeValue = Util:Label( data.cnf.cost_value_list[1] , Macros.TypefaceSize.slightly , Macros.Color.lilyWhite )
    self.consumeValue:SetAnchorPoint( 1 , 0.5 )
    self.consumeValue:SetPosition( self.Panel_2:GetContentWidth() - 5 , self.Panel_2:GetContentHeight()/2 )
    self.Panel_2:AddChild(self.consumeValue)

    --按钮
    if data.buy_num ~= 0 then
        self.Button_buy:SetText(CPPGameLib.GetString("shop_albuy"))
        Util:GreyButton( self.Button_buy )
    else
        self.Button_buy:SetEnabled(true)
        self.Button_buy:SetText(CPPGameLib.GetString("shop_sure_Buy"))     
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        local stroke_width = 2
        self.Button_buy:SetOutline(r, g, b, a, stroke_width) 
    end

    if type_ == Macros.Game.ObjType.FAIRY then
        local type_ = CallModuleFunc(ModuleType.DIVINATION , "ToStarSoulShopIdGetFateType" , data.cnf.item_id )

        if type_ then
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "fate" , true )
            self.tobattle = Util:Sprite( sp )
            self.tobattle:SetPosition( 23 , self.icon_layer:GetNode():GetContentHeight() - 5 )
            self.icon_layer:GetNode():AddChild(self.tobattle,999)
        end

    end


end

--购买
function Modules.TemplDivinationShopItem:OnButtonGain()
    local haveConsumeNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.consumeType)
    self.data.consumeType = self.consumeType
    self.data.haveConsumeNum = haveConsumeNum
    self.data.consumeNum = self.consumeNum
    CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.FAIRY , self.__data.cnf.item_num , true , function ( sum )
        if sum >= self.__data.cnf.item_num then

            if haveConsumeNum < self.consumeNum then
                local txt = "currency_insufficient_"..self.consumeType
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
                CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", self.consumeType )
                return
            end   

            local info = {}
            info.type = Macros.Game.SHOP.DIVINATION_PACK
            info.param1 = self.data.cnf.goods_id
            info.param2 = 1
            CallModuleFunc(ModuleType.DIVINATION , "DestinyStarBuyRequest",info)

        end
    end )
end
