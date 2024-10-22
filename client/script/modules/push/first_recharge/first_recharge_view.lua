--
-- @brief 首充奖励 
-- @author: yjg
-- @date: 2017年6月29日15:30:00
--

Modules = Modules or {}

Modules.FirstRechargeView = Modules.FirstRechargeView or BaseClass(BaseView)

function Modules.FirstRechargeView:__init()
    self.__layout_name = "templ_first_recharge"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()

        CallModuleFunc(ModuleType.ACTIVITY, "FirstRechargeDataReq")
        self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_FIRST_RECHARGE,function (_, data)
            self:__SetShowUI()
        end)

        self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE,function (_, data)
            self:__SetShowUI()
        end)

        self:BindGlobalEvent(Macros.Event.ChargeModule.CHARGE_DATA_RESP,function (_, data)
            CallModuleFunc(ModuleType.ACTIVITY, "AccumulateRechargeDataReq")
        end)
    end
    
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.FIRST_RECHARGE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.FIRST_RECHARGE,true)
end

function Modules.FirstRechargeView:__Dispose()

    if self.effect_id then
        GlobalEffectMgr:DelEffect(self.effect_id)
        self.effect_id = nil
    end

    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end

end

function Modules.FirstRechargeView:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg")

    --模型
    self.Panel_icon = self:GetWidget("Panel_icon")

    --关闭按钮
    self.Button_close = Util:WidgetButton( 
                                            self:GetWidget("Button_close") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonClose)
                                            )
end

function Modules.FirstRechargeView:__ShowUI()
    local layout = Util:Layout( 475 , 330 )
    layout:SetAnchorPoint( 1 , 0.5 )
    layout:SetPosition( self.Image_bg:GetContentWidth()/2 + 142 + (layout:GetContentWidth()/2) , self.Image_bg:GetContentHeight()/2 - 14 )
    self.Image_bg:AddChild(layout)

    local params =  
    {
        item_class = Modules.FirstRechargeItem,
        item_width = layout:GetContentWidth() ,
        item_height = 150 ,
        row = 4,
        col = 1,
        view_height = layout:GetContentHeight() ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    layout:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode() , 40 , -50 )
    
    --黑底
    local Accumulative = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_rechfirst_Accumulative") )
    Accumulative:SetAnchorPoint( 0.5 , 0.5 )
    Accumulative:SetPosition( self.Image_bg:GetContentWidth()/2 - 270 , 32 )
    self.Image_bg:AddChild( Accumulative )

    --标题
    self.headline = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_text_3" , true ) )
    self.headline:SetVisible(false)
    self.headline:SetAnchorPoint( 0.5 , 0 )
    self.headline:SetPosition( self.Image_bg:GetContentWidth()/2 - 290 , 10 )
    self.Image_bg:AddChild( self.headline , 5 )
end

function Modules.FirstRechargeView:__SetShowUI()
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")
    self.lst_list:SetDataList( t1 )

    self:SetPrice()
    self:SettleGears()
end

function Modules.FirstRechargeView:OnButtonClose()
    if unpack(self.__open_data) == "push" then
        self:CloseManual()
        CallModuleFunc(ModuleType.PUSH,"Transfer",Macros.Game.SystemID.FIRST_RECHARGE)
    else
        self:CloseManual()
        local func = unpack(self.__open_data)
        if func and type(func) == 'function' then
            func()
            func = nil
        end      
    end
end


--档位整理
function Modules.FirstRechargeView:SettleGears()
    self.headline:SetVisible(false)
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")

    table.sort( t1 , function ( a , b )
        return a.context < b.context
    end )

    for i , v in ipairs( t1 ) do
        local record = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeDataRecord" , v.id )
        if not record then
            if i == 1 then
                self:Gears1()
            elseif i == 2 then
                self:Gears2()
            elseif i == 3 then
                self:Gears3()
            elseif i == 4 then
                self:Gears4()
            end
            break
        end
        if i == 4 then
            self:Gears4()
        end
    end

end

--档位1
function Modules.FirstRechargeView:Gears1()

    if self.leftshowUI then
        self.leftshowUI:RemoveSelf(true)
        self.leftshowUI = nil
    end

    local stencil_node = CPPGraphicLabel.Node()
    stencil_node:DrawSolidRect( 0 , 0 , 500 , 500 , 0 , 0 , 0 , 150 )

    self.leftshowUI = ClippingNodeEx.Node()
    self.leftshowUI:SetAlphaTestValue(0)
    self.leftshowUI:SetStencilMask(stencil_node)
    self.leftshowUI:SetAnchorPoint( 0.5 , 0 )
    self.leftshowUI:SetPosition( -160 , -120 )
    self.Panel_icon:AddChild(self.leftshowUI)

    local layou = Util:Layout( 500 , 500 )
    layou:SetAnchorPoint( 0 , 0 )
    self.leftshowUI:AddChild( layou )
    PosTool.Center( layou )

    local _, effect = self:__CreateEffect(
                                            { res_name = "UI_shouchong_juese" , zorder = -1 }, 
                                            layou
                                            )        
    PosTool.Center(effect)

    local heroLst = {}
    local heroPos = {}
    for i = 1 , 3 do
        heroLst[i] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_1_"..i , true ) )
        heroLst[i]:SetOpacity( 0 )
        layou:AddChild( heroLst[i] )
        PosTool.Center( heroLst[i] )
        heroPos[i] = { x = heroLst[i]:GetPositionX() , y = heroLst[i]:GetPositionY() }
    end

    local index = 1
    local function hero( ... )
        for i = 1 , 3 do
            CPPActionManager.FadeOut( heroLst[i] , 0.01 ) 
        end
        CPPActionManager.FadeIn( heroLst[index] , 1 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
            --假装是个定时器
            CPPActionManager.DelayTo( self.leftshowUI , 2 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                CPPActionManager.FadeOut( heroLst[index] , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                    index = index + 1
                    if index > 3 then
                        index = 1
                    end
                    hero()
                end) )
            end ) )
        end ) )
    end
    hero()

    self.headline:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_text_1" , true ) )
    self.headline:SetVisible(true)

    self:SetPrice()
end

--档位2
function Modules.FirstRechargeView:Gears2()
    if self.leftshowUI then
        self.leftshowUI:RemoveSelf(true)
        self.leftshowUI = nil
    end
    
    self.leftshowUI = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_2"))
    self.leftshowUI:SetPosition( 80 , -100 )
    self.leftshowUI:SetAnchorPoint( 0.5 , 0 )
    self.Panel_icon:AddChild( self.leftshowUI , 999 )
    self.Panel_icon:SetZOrder( 1 )
    
    local layou = Util:Layout( 462 , 385 )
    layou:SetAnchorPoint( 0.5 , 0.5 )
    self.leftshowUI:AddChild( layou , -1 )
    PosTool.Center( layou )


    local _, effect = self:__CreateEffect(
                                            { res_name = "UI_shouchong_juese" , zorder = -10 }, 
                                            layou
                                            )   
    PosTool.Center( effect )     

    self.headline:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_text_2" , true ) )
    self.headline:SetVisible(true)

    self:SetPrice()
end

--档位3
function Modules.FirstRechargeView:Gears3()
    if self.leftshowUI then
        self.leftshowUI:RemoveSelf(true)
        self.leftshowUI = nil
    end
    self.leftshowUI = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_3" ))
    self.leftshowUI:SetPosition( 90 , 0 )
    self.leftshowUI:SetAnchorPoint( 0.5 , 0 )
    self.Panel_icon:AddChild( self.leftshowUI , -1 )
    ActionManager:ZoomAnimation(self.leftshowUI,1, 1.02 , 0.98 )
    self.Panel_icon:SetZOrder( 1 )

    local layou = Util:Layout( 462 , 385 )
    layou:SetAnchorPoint( 0.5 , 0.5 )
    self.leftshowUI:AddChild( layou , -1 )
    PosTool.Center( layou )

    local _, effect = self:__CreateEffect(
                                            { res_name = "UI_shouchong_juese" , zorder = -10 }, 
                                            layou
                                            )   
    PosTool.Center( effect )     

    self.headline:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_text_3" , true ) )
    self.headline:SetVisible(true)

    self:SetPrice()
end

--档位4
function Modules.FirstRechargeView:Gears4()
    if self.leftshowUI then
        self.leftshowUI:RemoveSelf(true)
        self.leftshowUI = nil
    end
    
    if self.leftModel then
        self.leftModel:DeleteMe()
        self.leftModel = nil
    end

    self.leftModel = LogicGUI.Model.New({info_id = 50011 , parent = self.Panel_icon , model_type = LogicGUI.ModelType.SPRITE })
    self.leftModel:SetPosition( 60 , 20 )
    self.leftModel:SetScaleFactor( 1.5 )
    self.leftModel:PlayAnimation(RenderObj.ActionIndex.WIN)
    self.Panel_icon:SetZOrder( 999 )
    
    local effect = nil
    self.effect_id , effect = self:__CreateEffect({res_name = "UI_chongzhiyanlong" }, self.leftModel:GetNode() , 9999 )
    effect:SetPosition( self.leftModel:GetNode():GetContentWidth()/2 - 20 , self.leftModel:GetNode():GetContentHeight()/2 + 10 ) 
    effect:SetScaleFactor(1.15)

    CPPActionManager.DelayTo(self.leftModel:GetNode() , 2.2 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        self.leftModel:PlayAnimation(RenderObj.ActionIndex.IDLE)
        self.leftModel:SetScaleFactor( 2.5 )
    end ))

    self.headline:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "first_recharge_text_4" , true ) )
    self.headline:SetVisible(true)

    self:SetPrice()
end

--价钱
function Modules.FirstRechargeView:SetPrice()

    if self.item then
        self.item:RemoveSelf(true)
        self.item = nil
    end

    local t1 , num = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")

    -- 累充数据
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    if max and max > num then
        max = num
    end

    local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
    local tab = nil
    --是否显示首充界面价格单位
    if CPPSdkTools.IsInlandChannel() then
        tab = {
                { zi = CPPGameLib.GetString( "progress_Text" ) , ys = Macros.Color.white , dx = Macros.TypefaceSize.largish },
                { zi = CPPGameLib.GetString( "Currency_CNY" , ( (max or 0) .. "/" .. ( num ) ) ) , ys = Macros.Color.golden , dx = Macros.TypefaceSize.largish },
            }
    else
        tab = {
                { zi = CPPGameLib.GetString( "progress_Text" ) , ys = Macros.Color.white , dx = Macros.TypefaceSize.largish },
                { zi = ( (max or 0) * addUpAddition ) .. "/" .. ( num * addUpAddition ) , ys = Macros.Color.golden , dx = Macros.TypefaceSize.largish },
                { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND), dx = 0.7 , jg = 5},
            }
    end

    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    self.item = Util:LabCenter(tab)
    self.item:SetAnchorPoint( 0.5 , 0.5 )
    self.item:SetPosition( self.Image_bg:GetContentWidth()/2 - 270 , 32  )
    self.Image_bg:AddChild( self.item , 2)
end













--头像
Modules = Modules or {}

Modules.FirstRechargeItem = Modules.FirstRechargeItem or BaseClass(GUI.ScrollItem)

function Modules.FirstRechargeItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.FirstRechargeItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(475 , 150)
    return self.layout
end

function Modules.FirstRechargeItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.FirstRechargeItem:__ShowUI(data)
    --标题
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "red_label")
    local red_label = Util:Sprite( sp )
    red_label:SetAnchorPoint( 0.5 , 1 )
    red_label:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight() )
    self.layout:AddChild( red_label )

    --字体   
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "price_" .. data.context , true )
 
    local price = Util:Sprite( sp )
    price:SetAnchorPoint( 0.5 , 0.5 )
    price:SetPosition( red_label:GetContentWidth()/2 , red_label:GetContentHeight()/2 )
    red_label:AddChild( price )

    --图标
    for i , v in ipairs( data.item ) do
        --头像
        local icon = LogicGUI.IconLayer.New()
        self.layout:AddChild(icon:GetNode())
        local iconData = {id = v.id , num = v.num }
        icon:SetData(iconData)
        icon:SetPosition( (icon:GetNode():GetContentWidth()/2 ) + 40 + ( (i-1) * (icon:GetNode():GetContentWidth() ) ) , self.layout:GetContentHeight()/2 - 20 )
        icon:SetTouchSwallow( false )
        icon:SetScaleFactor( 0.8 )
        if i == 1 then
            local effect_id , effect = self:__CreateEffect({res_name = "UI_taozhuang" }, icon:GetNode() )
            effect:SetPosition( icon:GetNode():GetContentWidth()/2 , icon:GetNode():GetContentHeight()/2 ) 
            -- effect:SetScaleFactor(1.05)
            effect:SetZOrder(999)
        end
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_button" )
    self.btn = Util:Button( sp , function ( ... )
        self:OnBtn( data )
    end)
    self.btn:SetAnchorPoint( 0.5 , 0.5 )
    self.btn:SetPosition( 390 , self.layout:GetContentHeight()/2 - 20 ) 
    self.layout:AddChild( self.btn )
    self.btn:SetScaleFactor( 0.8 )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "draw" , true)
    self.recharge = Util:Sprite( sp )
    self.recharge:SetAnchorPoint( 0.5 , 0.5 )
    self.recharge:SetPosition( self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 ) 
    self.btn:AddChild( self.recharge )

    local BtnEffect_con = nil
    self.btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"},self.recharge,999)
    BtnEffect_con:SetPosition(self.recharge:GetContentWidth()/2 , self.recharge:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)


    local BtnEffect_con = nil
    self.chongzhiEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "UI_chongzhitongyong"},self.btn,999)
    BtnEffect_con:SetPosition(self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.chongzhiEffect_id , false)
    
    self:Transfer( data )
end

--设置可以购买
function Modules.FirstRechargeItem:Transfer(data)
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeDataRecord" , data.id )

    --充值够了
    if max >= data.context then
        --有记录
        if record then
            self:SetNoMay( data )
        else
            self:SetMay( data )
        end
    else
        self:SetMay( data )
    end
end

--设置可以购买
function Modules.FirstRechargeItem:SetMay( data )
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeDataRecord" , data.id )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FIRST_RECHARGE, "draw" , true )
    local btn_button = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_yellow_button" )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
    GlobalEffectMgr:SetEffectVisible(self.chongzhiEffect_id , false)

    --如果充值不够的话
    if data.context > max then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "recharge" , true )
        btn_button = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_button" )
        GlobalEffectMgr:SetEffectVisible(self.chongzhiEffect_id , true)
    else
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true )
    end

    --按钮
    self.recharge:SetImage( sp )
    self.btn:SetNormalImage( btn_button )
end

--设置买过了
function Modules.FirstRechargeItem:SetNoMay(data)
    -- Util:GreyButton( self.btn )
    -- self.recharge:SetGray( true )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "dungeon_get" , true)
    self.btn:SetNormalImage( sp )
    self.btn:SetTouchEnabled(false)
    self.recharge:SetVisible(false)
end

function Modules.FirstRechargeItem:OnBtn(data)
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    if max < self.__data.context  then
        CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE , self.__data.context )
    else
        local info = {}
        info.data_id = self.__data.id
        CallModuleFunc(ModuleType.ACTIVITY, "TakeLimitActivityRewardReq" , info )
    end
end