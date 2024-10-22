--[[
%% @module: 超值购
%% @author: yjg
%% @created: 2017年8月24日21:35:03
--]]

Modules = Modules or {} 
Modules.SuperDiscountView = Modules.SuperDiscountView or BaseClass(BaseView)

function Modules.SuperDiscountView:__init()
    self.__layout_name = "superdiscount"    
    self.__bg_params = {type = BaseView.BGType.SOLID}

    self.__open_callback = function()
      self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SUPERDISCOUNTVIEW)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SUPERDISCOUNTVIEW,true)
end

function Modules.SuperDiscountView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
    self:__SetShowUI()
end 

function Modules.SuperDiscountView:__Dispose()
end

function Modules.SuperDiscountView:__Server()
    CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "SuperDiscountDataReq")
    --超值购
    self:BindGlobalEvent(Macros.Event.SuperDiscountModule.SUPERDISCOUNT , function (_, data)
        self:__SetShowUI()
    end)
end

function Modules.SuperDiscountView:__Getwidget( )

    --居中层
    self.Image_bg = self:GetWidget("Image_bg")
    --累购好礼
    self.Image_tab = self:GetWidget("Image_tab")
    --礼物列表
    self.Image_icon_bg = {}
    for i = 1 , 4 do
        self.Image_icon_bg[i] = self:GetWidget("Image_icon_bg_"..i)
        self.Image_icon_bg[i]:SetVisible( false )
    end
    --显示列表
    self.Image_item = self:GetWidget("Image_item")
    --描述背景
    self.Image_text_bg = self:GetWidget("Image_text_bg")
    --购买提示
    self.Image_lab = self:GetWidget("Image_lab")
    --对白
    self.Image_lines = self:GetWidget("Image_lines")
    --关闭按钮
    self.Button_close = Util:WidgetButton( 
                                        self:GetWidget("Button_close") , 
                                        nil , 
                                        CPPGameLib.Handler(self,self.OnButtonclose)
                                    )
end 

function Modules.SuperDiscountView:__ShowUI( )
    local cnf = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetSuperDiscountAccumulateReward")
    --左侧列表
    self.box = {}
    for i , v in ipairs( cnf ) do
        if self.Image_icon_bg[i] then
            self.box[i] = {}
            --箱子
            self.box[i].box = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SUPERDISCOUNTVIEW, "sp_box_".. v.box_icon ) )
            self.Image_icon_bg[i]:AddChild( self.box[i].box )
            PosTool.Center( self.box[i].box , -10 )
            self.box[i].box:SetScaleFactor( 0.8 )

            --骨骼
            if v.box_skeleton then
                self.box[i].skeleton , effect = self:__CreateEffect({res_name = v.box_skeleton , type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE } , self.Image_icon_bg[i] , 2)
                effect:SetScaleFactor( 0.8 )
                effect:SetPosition( 40 , 15 )
                GlobalEffectMgr:SetEffectVisible( self.box[i].skeleton , false)
            end

            --特效
            if v.box_effect then
                self.box[i].effect , effect = self:__CreateEffect({res_name = v.box_effect , loop = true} , self.Image_icon_bg[i] )
                effect:SetPosition( self.Image_icon_bg[i]:GetContentWidth()/2 , self.Image_icon_bg[i]:GetContentHeight()/2 )
                GlobalEffectMgr:SetEffectVisible( self.box[i].effect , false)
            end

            --次数
            self.box[i].buyTime = Util:Name( CPPGameLib.GetString( "superdiscoun_buy" , 0 , 0 ) , Macros.TypefaceSize.minimum )
            self.box[i].buyTime:SetPosition( self.Image_icon_bg[i]:GetContentWidth()/2 , 10 )
            self.Image_icon_bg[i]:AddChild( self.box[i].buyTime , 10 )

            --触摸
            local layer = Util:Layout( self.Image_icon_bg[i]:GetContentWidth() , self.Image_icon_bg[i]:GetContentHeight()/2 , function( ... )
                self:OnButtonOpen( v )
            end )
            layer:SetAnchorPoint( 0.5 , 0.5 )
            self.Image_icon_bg[i]:AddChild( layer )
            PosTool.Center( layer )

            --√
            local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
            self.box[i].yes = Util:Sprite( sp_2 )
            self.Image_icon_bg[i]:AddChild( self.box[i].yes )
            PosTool.Center( self.box[i].yes )
            self.box[i].yes:SetVisible( false )

            self.Image_icon_bg[i]:SetVisible(true)
        end
    end

    local lst , money , star , over , pay_id = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetNowTimeAward")
    self.pay_id = pay_id

    local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , self.pay_id )
    --钻石
    self.tips = Util:LabCenter({
        {zi = CPPGameLib.GetString("superDiscount_headline_1") , ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal },
        {zi = text , ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal , jg = 4},
        {zi = CPPGameLib.GetString("superDiscount_headline_2") , ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal , jg = 4},
    })  
    self.tips:SetAnchorPoint( 0 , 0.5 )
    self.tips:SetPosition( 20 , self.Image_text_bg:GetContentHeight()/2 )
    self.Image_text_bg:AddChild( self.tips )


    if not lst then return end
    --图标
    local interval = 450  / #lst
    for k , v in ipairs( lst ) do
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetData({id = v.id , num = v.num})
        icon:SetPosition( 120 + interval * (k - 1) - 10 , self.Image_item:GetContentHeight()/2 )
        self.Image_item:AddChild(icon:GetNode())
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_blue")
    self.Button_buy = Util:Button( 
                                    sp , 
                                    CPPGameLib.Handler(self,self.OnButtonbuy) , 
                                    "" , 
                                    nil , nil , Macros.Color.button_red 
                                    )
    self.Button_buy:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_buy:SetPosition( self.Image_item:GetContentWidth()/2 , -35 )
    self.Image_item:AddChild( self.Button_buy )

    --已购买
    self.buy_2 = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_2" , true) )
    self.buy_2:SetAnchorPoint( 0.5 , 0.5 )
    self.buy_2:SetPosition( self.Button_buy:GetPositionX() ,  self.Button_buy:GetPositionY() )
    self.Image_item:AddChild( self.buy_2 )
    self.buy_2:SetVisible( false )

    -- 每天凌晨0时重置商品
    local SuperDiscount_tips1 = Util:Label( CPPGameLib.GetString("superDiscount_tips1") , Macros.TypefaceSize.minimum )
    SuperDiscount_tips1:SetAnchorPoint( 0.5 , 0.5 )
    SuperDiscount_tips1:SetPosition( self.Image_item:GetContentWidth()/2 , -80 )
    self.Image_item:AddChild( SuperDiscount_tips1 )

    -- 购买超值折扣礼包算入首充
    local SuperDiscount_tips2 = Util:Label( CPPGameLib.GetString("superDiscount_tips2") , Macros.TypefaceSize.minimum )
    SuperDiscount_tips2:SetAnchorPoint( 0.5 , 0.5 )
    SuperDiscount_tips2:SetPosition( self.Image_item:GetContentWidth()/2 ,  SuperDiscount_tips1:GetPositionY() - SuperDiscount_tips2:GetContentHeight() - 5 )
    self.Image_item:AddChild( SuperDiscount_tips2 )

    --开始时间
    local superDiscount_openTime = Util:Label( CPPGameLib.GetString("superDiscount_openTime" , TimeTool.GetTimeFormat( star , TimeTool.defaul_format[20] ) ) , Macros.TypefaceSize.minimum , Macros.Color.white )
    superDiscount_openTime:SetAnchorPoint( 0 , 0.5 )
    superDiscount_openTime:SetPosition( self.Image_bg:GetContentWidth() - 210 , 50 )
    self.Image_bg:AddChild( superDiscount_openTime )

    --结束时间
    local superDiscount_endTime = Util:Label( CPPGameLib.GetString("superDiscount_endTime" , TimeTool.GetTimeFormat( over , TimeTool.defaul_format[20] ) ) , Macros.TypefaceSize.minimum , Macros.Color.white )
    superDiscount_endTime:SetAnchorPoint( 0 , 0.5 )
    superDiscount_endTime:SetPosition( superDiscount_openTime:GetPositionX() , superDiscount_openTime:GetPositionY() - 30 )
    self.Image_bg:AddChild( superDiscount_endTime )

    local tips = Util:RichText( CPPGameLib.GetString("SuperDiscountTips") , Macros.TypefaceSize.slightly , 180 )
    tips:SetAnchorPoint( 0 , 1 )
    tips:SetPosition( self.Button_close:GetPositionX() + 16 , self.Image_lines:GetPositionY() + 40 )
    self.Image_bg:AddChild( tips )

end

function Modules.SuperDiscountView:__SetShowUI( )
    --累计奖励
    local lst , neet_time = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetAccumulateRewad")

    local cnf = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetSuperDiscountAccumulateReward")
    local buyTime = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetCurBuyTime" )
    
    for i , v in ipairs( cnf ) do
        --领取状态
        local type_ = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetRewardTimes" , v.times )
        --已领取
        if type_ == 1 then
            GlobalEffectMgr:SetEffectVisible( self.box[i].skeleton , false)
            GlobalEffectMgr:SetEffectVisible( self.box[i].effect , false)
            self.box[i].yes:SetVisible( true )
            self.box[i].box:SetGray( true )
            self.box[i].box:SetVisible( true )
        --还未领取
        else
            --可领取
            if buyTime >= v.need_buy_times then
                self.box[i].box:SetVisible( false )
                GlobalEffectMgr:SetEffectVisible( self.box[i].skeleton , true)
                GlobalEffectMgr:SetEffectVisible( self.box[i].effect , true)
            end
        end
        self.box[i].buyTime:SetString( CPPGameLib.GetString( "superdiscoun_buy" , buyTime , v.need_buy_times ) )
    end

    -- 按钮状态(0=还没购买, 1=已购买(可领取), 2=已领取)
    local status = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetStatus")
    if status == 1 then
        self.Button_buy:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green") )
        self.Button_buy:SetVisible(true)
        GlobalEffectMgr:SetEffectVisible(self.effect_id , true)
        self.buy_2:SetVisible(false)
        WidgetUnity.SetWidgetListener( self.Button_buy , nil, nil, CPPGameLib.Handler(self,self.OnButtonAward) , nil, 0.5*1000)
        self:Currency( CPPGameLib.GetString("manor_get" ) )
    
   elseif status == 2 then
        GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
        self.Button_buy:SetVisible(false)
        self.buy_2:SetVisible(true)
        self:Currency( )
    else
       CPPGameLib.PrintTable("xy" , _id , "请求物品列表") 
        GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
        self.Button_buy:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue") )
        local lst , money , star , over , _id = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetNowTimeAward")
        self.Button_buy:SetVisible(true)
        self.buy_2:SetVisible(false)
        WidgetUnity.SetWidgetListener( self.Button_buy , nil, nil, CPPGameLib.Handler(self,self.OnButtonbuy) , nil, 0.5*1000)
        self:Currency( _id )
    end

end

--折扣购买
function Modules.SuperDiscountView:OnButtonbuy()
    CallModuleFunc(ModuleType.CHARGE , "RechargeOrderIDReq" , Macros.Game.RechargeType.SUPER_DISCOUNTVIEW , {recharge_id = self.pay_id} )
end

--折扣奖励
function Modules.SuperDiscountView:OnButtonAward()
    CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "SuperDiscountBuyRewardReq")
end

--累计奖励
function Modules.SuperDiscountView:OnButtonOpen( info )
    LogicGUI.TempSuperDiscountAward.New( info )
end

--关闭
function Modules.SuperDiscountView:OnButtonclose()
    if unpack(self.__open_data) == "push" then
        self:CloseManual(true)
        CallModuleFunc(ModuleType.PUSH,"Transfer",Macros.Game.SystemID.SUPERDISCOUNTVIEW)
    else
        self:CloseManual()
    end
end


function Modules.SuperDiscountView:Currency( info )
    if self.diamond then
        self.diamond:RemoveSelf(true)
        self.diamond = nil
    end

    self.Button_buy:SetText("")
    info = info or " "
    if type( info ) == "number" then
        local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , info )
        --钻石
        self.diamond = Util:LabCenter({
            -- {tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.8 },
            {zi = text , mb = true , ys = 101 , dx = Macros.TypefaceSize.popup , jg = 4},
        })  
    else
        -- 钻石
        self.diamond = Util:LabCenter({
            {zi = info , mb = true , ys = 103 , dx = Macros.TypefaceSize.popup , jg = 4},
        })  
    end

    self.diamond:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_buy:AddChild( self.diamond )
    PosTool.Center( self.diamond )
end
