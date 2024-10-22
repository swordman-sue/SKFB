--[[
%% @module: 英雄商店界面
%% @author: yjg
%% @created: 2016年10月25日21:52:49
--]]

Modules = Modules or {} 
Modules.HeroShopView = Modules.HeroShopView or BaseClass(BaseView)

function Modules.HeroShopView:__init()
    self.__layout_name = "hero_shop"    
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("shop_shop"), 
        show_diamond = true,
        show_gold = true,
        show_energy = true,
        show_hero_soul = true,
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

function Modules.HeroShopView:__Dispose()
    if self.__tab then
        self.__tab:DeleteMe()
        self.__tab = nil
    end
    if self.__hero_shop_scrollview then
        self.__hero_shop_scrollview:DeleteMe()
        self.__hero_shop_scrollview = nil
    end
end

function Modules.HeroShopView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end 

function Modules.HeroShopView:__Server( )
    local lst = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopList")
    if not lst then
        CallModuleFunc(ModuleType.HERO_SHOP,"HeroShopDataReq" )
    else
        self:__SetShowUi()
    end

    local lst = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopList")
    if not lst then
        CallModuleFunc(ModuleType.HERO_SHOP,"AwakenShopDataReq" )
    else
        self:__SetShowUi()
    end

    self:BindGlobalEvent(Macros.Event.HeroShopModule.HERO_SHOP, function(_, index)
        print("xy" , "回来了" )
        self:__SetShowUi()
    end)    

    self:BindGlobalEvent(Macros.Event.HeroShopModule.AWAKEN_SHOP, function(_, index)
        self:__SetShowUi()
    end)  
end

--获得组件
function Modules.HeroShopView:__Getwidget( )
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
                                            nil,nil, Macros.Color.button_yellow
                                        )
    --提示
    self.Text_tips = self:GetWidget("Text_tips")
    --免费
    self.Text_freely = self:GetWidget("Text_freely")
    --次数
    self.Text_time = self:GetWidget("Text_time")
    --消耗
    self.Text_consume = self:GetWidget("Text_consume")

    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(1)英雄商店，小图标：" )
        print("xy" , "(2)觉醒商店，小图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
            if self.index == 1 then
               help:SetCell("Rule_HeroShop")
            elseif self.index == 2 then
               help:SetCell("Rule_AwakenShop")
            end
        end )
    end )
    Button_rule:SetZOrder(100)

    --推荐阵容
    local btn_recommendHero = self:GetWidget("Button_recommendHero")
    btn_recommendHero:SetZOrder(100)
    btn_recommendHero:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_lineup"),TextureResType.LOCAL)
    Util:WidgetButton(btn_recommendHero,"",function ()
        --打开推荐阵容
        GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.RECOMMEND_LINEUP)
    end)
    --推荐阵容文字
    Util:WidgetLabel(self:GetWidget("Text_recommendHero"),CPPGameLib.GetString("recommend_lineup_title"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
end

--组件设置
function Modules.HeroShopView:__ShowUI( )
    --免费刷新次数
    local heroShop_freeFlush = Util:Label( CPPGameLib.GetString("heroShop_freeFlush") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    heroShop_freeFlush:SetAnchorPoint( 0 , 0.5 )
    heroShop_freeFlush:SetPosition( 0 , self.Text_freely:GetContentHeight()/2 )
    self.Text_freely:AddChild( heroShop_freeFlush )

    --免费次数
    self.heroShop_freeFlush = Util:Label( "100/100" , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Text_freely:AddChild( self.heroShop_freeFlush )
    PosTool.RightTo( heroShop_freeFlush , self.heroShop_freeFlush )

    --倒计时
    self.countdown = Util:Label( " " , Macros.TypefaceSize.normal , Macros.Color.content )
    self.countdown:SetColor(unpack(Macros.Color.front_color))
    self.Text_freely:AddChild( self.countdown )
    PosTool.RightTo( self.heroShop_freeFlush , self.countdown )

    --免费提示
    self.upTips = Util:Label( CPPGameLib.GetString("heroShop_UsePropFlush" , 0 , 0 ) , Macros.TypefaceSize.normal , Macros.Color.content )
    self.upTips:SetAnchorPoint( 0 , 0.5 )
    self.upTips:SetPosition( 0 , self.Text_tips:GetContentHeight()/2 )
    self.Text_tips:AddChild( self.upTips )

    --剩余次数
    local heroShop_residue = Util:Label( CPPGameLib.GetString("heroShop_residue") ,  Macros.TypefaceSize.normal , Macros.Color.keypoint )
    heroShop_residue:SetAnchorPoint( 0 , 0.5 )
    self.upTips:SetPosition( 0 , self.Text_time:GetContentHeight()/2 )
    self.Text_time:AddChild( heroShop_residue )

    --剩余次数
    self.heroShop_residue = Util:Label( "100/100" , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Text_time:AddChild( self.heroShop_residue )
    PosTool.RightTo( heroShop_residue , self.heroShop_residue )

    --刷新令
    local id = ConfigAdapter.Common.GetHeroShopConsumeGoods()
    local goods_Num = CallModuleFunc(ModuleType.Pack , "GetNumByItemID", id )
    local tab = {
                    { tp = Resource.PathTool.GetItemPath( id ) , dx = 0.5 },
                    { zi = goods_Num , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
                }

    self.icon , self.iconlst = Util:LabCenter( tab )
    self.icon:SetAnchorPoint( 0.5 , 0.5 )
    self.icon:SetPosition( self.Text_consume:GetContentWidth()/2 , self.Text_consume:GetContentHeight()/2 )
    self.Text_consume:AddChild( self.icon )

    --商品列表
    local params =  
    {
        item_class = Modules.TemplHeroShopItem,
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
    local text = { }

    if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_AWAKEN_SHOP) then
        text =  {
            [1] = CPPGameLib.GetString("heroShop_shop") ,
        }
    else
        text =  {
            [1] = CPPGameLib.GetString("heroShop_shop") ,
            [2] = CPPGameLib.GetString("Awaken_shop") ,
        }
    end
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

function Modules.HeroShopView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )
    layer:SetAnchorPoint( 0.5 , 0.5 )

    local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.white , nil , nil , Macros.Color.button_red )
    name:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(name)
    PosTool.Center( name )

    return layer
end

--选中
function Modules.HeroShopView:OnCallBack( index , data )
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

    self.index = index
    self:__SetShowUi()
    local info = nil
    --资源条
    if self.index == 1 then
        info = { show_diamond = true , show_gold = true , show_energy = true , show_hero_soul = true }
    else
        info = { show_diamond = true , show_gold = true , show_energy = true , show_godsoul = true }
    end
    self.__title_bar:SetCurrency( info )
end

--设置界面
function Modules.HeroShopView:__SetShowUi()
    --锁
    local lst = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopList")
    if not lst then
        return
    end

    --设置列表
    local lst = {}
    if self.index == 1 then
        lst = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopList")
    else
        lst = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopList")
    end
    self.__hero_shop_scrollview:SetDataList(lst)

    --免费刷新次数
    local time , num , max = 0 , 0 
    if self.index == 1 then
        time = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopTime")
        num , max = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopFreeRefresh")
    else
        time = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopTime")
        num , max = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopFreeRefresh")
    end

    self.heroShop_freeFlush:SetString( num .."/" .. max )

    -- 倒计时
    self:Countdown( self.index )

    --刷新提示
    local num , name = 0 , 0 
    if self.index == 1 then
        num = ConfigAdapter.Common.GetHeroShopConsumeNum()
        name = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.HERO_SOUL).name
    else
        num = ConfigAdapter.Common.GetAwakenShopCusumeNum()
        name = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.GODSOUL).name
    end
    self.upTips:SetString( CPPGameLib.GetString("heroShop_UsePropFlush" , num , name ) )

    --今天已经刷新的次数
    local num , max = 0 , 0 
    if self.index == 1 then
        max = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_SHOP_UP_MAX)
        num = max - CallModuleFunc(ModuleType.HERO_SHOP , "GetHeroShopCostRefresh")
    else
        max = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.AWAKEN_SHOP_MAX)
        num = max - CallModuleFunc(ModuleType.HERO_SHOP , "GetAwakenShopCostRefresh")
    end
    self.heroShop_residue:SetString( num .. "/" .. max )

    --消耗
    local id , goods_Num = 0 , 0
    id = ConfigAdapter.Common.GetHeroShopConsumeGoods()
    goods_Num = CallModuleFunc(ModuleType.Pack , "GetNumByItemID", id )
    self.iconlst[2]:SetString( goods_Num )

end

--倒计时
function Modules.HeroShopView:Countdown( )
    CPPActionManager.StopAllActions(self.countdown)

    local func = nil
    local function moveAnimate_1( ... )
        local time = nil
        local type_ = nil
        local index = 0
        if self.index == 1 then
            time = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopTime") - 1
            local num , max = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopFreeRefresh")
            type_ = num == max
            index = num
        else
            time = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopTime") - 1
            local num , max = CallModuleFunc(ModuleType.HERO_SHOP,"GetAwakenShopFreeRefresh")
            type_ = num == max
            index = num
        end

        --如果倒计时是0 而且 次数不是满的 
        if time <= 0 and type_ ~= true and not self.TimeType  then
            CPPActionManager.StopAllActions(self.countdown)
            self.TimeType = true
            if self.index == 1 then
                CallModuleFunc(ModuleType.HERO_SHOP,"HeroShopDataReq" )  
            else
                CallModuleFunc(ModuleType.HERO_SHOP,"AwakenShopDataReq" )
            end
        elseif type_ == true then
            self.TimeType = nil
            CPPActionManager.StopAllActions(self.countdown)
            self.countdown:SetString(CPPGameLib.GetString("heroShop_max"))
            self.countdown:SetColor(unpack( Macros.Color.front_color ) )
        else
            self.TimeType = nil
            CPPActionManager.DelayTo( self.countdown , 1 , func )
            self.countdown:SetString( "("..TimeTool.TransTimeStamp("MM:SS",time) ..")" )
            self.countdown:SetColor(unpack( Macros.Color.content ) )
        end
        PosTool.RightTo( self.heroShop_freeFlush , self.countdown )
    end

    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate_1)
    moveAnimate_1()
end

function Modules.HeroShopView:OnButtonFlush( )

-- 当英雄商店刷出上阵英雄同名的碎片或上阵英雄同名的整将，或者与上阵英雄有宿命的整将的时候，弹出确认框
    local function Send( ... )
        --免费
        local freeRefresh_Num = nil
        --道具
        local goods_Num = CallModuleFunc(ModuleType.Pack , "GetNumByItemID", ConfigAdapter.Common.GetHeroShopConsumeGoods() )
        --货币
        local soul_Num = nil

        local info = {}
        info.refresh_type = 0
        info.openIndex = self.index
        if self.index == 1 then
            info.shop_type = Macros.Game.SHOP.SOUL
            freeRefresh_Num = CallModuleFunc(ModuleType.HERO_SHOP , "GetHeroShopFreeRefresh")        
            soul_Num = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").hero_soul
        else
            info.shop_type = Macros.Game.SHOP.AWAKEN_SHOP
            freeRefresh_Num = CallModuleFunc(ModuleType.HERO_SHOP , "GetAwakenShopFreeRefresh")
            soul_Num = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").god_soul
        end

        if freeRefresh_Num ~= 0 then
            print("xy" , "=================freeRefresh_Num" , freeRefresh_Num)
            info.refresh_type = 1
        elseif goods_Num ~= 0 then
            print("xy" , "=================goods_Num" , goods_Num)
            info.refresh_type = 2
        elseif soul_Num ~= 0 and soul_Num >= ConfigAdapter.Common.GetHeroShopConsumeNum() then
            print("xy" , "=================soul_Num" , soul_Num)
            info.refresh_type = 3
        end

        --今天已经刷新的次数
        local num , max = 0 , 0 
        if self.index == 1 then
            max = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_SHOP_UP_MAX)
            num = max - CallModuleFunc(ModuleType.HERO_SHOP , "GetHeroShopCostRefresh")
        else
            max = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.AWAKEN_SHOP_MAX)
            num = max - CallModuleFunc(ModuleType.HERO_SHOP , "GetAwakenShopCostRefresh")
        end

        if num <= 0 and freeRefresh_Num == 0 then

            local type_ , value_ = nil , nil 
            if self.index == 1 then
                type_ , value_ = CallModuleFunc(ModuleType.VIP , "ToTypeGetResetItem" , Macros.Game.VIPType.HERO_SHOP_UP_MAX )
            else
                type_ , value_ = CallModuleFunc(ModuleType.VIP , "ToTypeGetResetItem" , Macros.Game.VIPType.HERO_SHOP_UP_MAX )
            end

            if type_ == true then
                local tips = LogicGUI.TemplAffirmTips.New( { 
                                                                type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                                content = CPPGameLib.GetString("heroShop_surplus") ,
                                                                cancel_text = CPPGameLib.GetString("vip_Isee") ,
                                                                confirm_text = CPPGameLib.GetString("vip_Toimprove") ,
                                                                confirm_func = function ( ... )
                                                                    CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
                                                                end,
                                                                cancel_func = function ( ... )
                                                                end
                                                            } )
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("heroShop_noResidue") )
            end
            return 
        end

        if info.refresh_type ~= 0 then
            CallModuleFunc(ModuleType.HERO_SHOP , "ShopRefreshReq" ,info)
        else
            local id = nil
            if self.index == 1 then
                id = Macros.Game.RolePropType.HERO_SOUL
            else
                id = Macros.Game.RolePropType.GODSOUL
            end

            local cnf = CPPGameLib.GetConfig("HuoBi", id )  
            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, cnf.big_icon)
            -- GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("currency_insufficient_" .. id ) )
        end
    end

    --设置列表
    if self.index == 1 then
        local lst = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopList")

        local text = ""
        for i , v in ipairs( lst ) do

            local cnf = CPPGameLib.GetConfig("HeroShop", v.goods_id) 

            local cnfType , info = ObjIDTool.GetInfo(cnf.item_id)

            if cnfType == Macros.Game.ObjType.HERO and info.type ~= Macros.Game.HeroType.EXP and v.buy_num == 0 then

                --宿命
                local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "ToIdGetFate" , cnf.item_id)
                if type_ then
                    text = CPPGameLib.GetString("heroShop_type2_tips")
                end

                --上阵
                local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "ToIdGetBattle" , cnf.item_id)
                if type_ then
                    text = CPPGameLib.GetString("heroShop_type2_tips")
                end

            elseif cnfType == Macros.Game.ObjType.ITEM  and v.buy_num == 0 then

                --上阵
                local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "ToIdGetBattle" , cnf.item_id)
                if type_ then
                    text = CPPGameLib.GetString("heroShop_type4_tips")
                end

            end

            if text ~= "" then
                break
            end
        end

        local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "GetNowShow"  )

        if text ~= "" and type_ ~= true then
            local tips = LogicGUI.TemplAffirmTips.New( { 
                                                            type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                            content = text ,
                                                            cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                            confirm_func = function ( ... )
                                                               Send() 
                                                            end
                                                        } )
            tips:SetLoadedCallback( function ( ... )
                --选择框
                local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
                local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
                local cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
                
                local common_no_show = Util:Label( CPPGameLib.GetString("common_no_show") , Macros.TypefaceSize.normal , Macros.Color.content )
                
                local layout = Util:Layout( cbx:GetContentWidth() + common_no_show:GetContentWidth() + 5, 50 )
                layout:SetAnchorPoint( 0.5 , 0.5 )
                tips:AddChild( layout )
                PosTool.Center( layout , 0 , -45 )


                cbx:SetAnchorPoint( 0 , 0.5 )
                cbx:SetPosition( 0 , layout:GetContentHeight()/2)
                layout:AddChild(cbx)
                cbx:SetEventListener(
                    GlobalCallbackMgr:AddCallBackFunc(function()
                        CallModuleFunc(ModuleType.HERO_SHOP , "SetNowShow" , true )
                    end),
                    GlobalCallbackMgr:AddCallBackFunc(function()
                        CallModuleFunc(ModuleType.HERO_SHOP , "SetNowShow" , false )
                    end)
                )   
                layout:AddChild(common_no_show)
                PosTool.RightTo( cbx , common_no_show , 5 )
            end )
        else
            Send() 
        end
    else
        print("xy" , "=====================22")
        Send() 
    end
end