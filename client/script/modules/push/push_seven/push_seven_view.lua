--
-- @brief 七天累计推送
-- @author: yjg
-- @date: 2018年1月26日09:35:43
--

Modules = Modules or {}

Modules.PushSevenView = Modules.PushSevenView or BaseClass(BaseView)

function Modules.PushSevenView:__init( )
    self.__layout_name = "templ_push_seven"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI( id )
        self:__Server() 
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PUSH_SEVEN)  
end

--数据
function Modules.PushSevenView:__Server()
    local cnf = CallModuleFunc(ModuleType.PUSH_SEVEN , "SevenDayLoginDataReq" )

    --七日登录
    self:BindGlobalEvent(Macros.Event.PushSevenModule.PUSH_SEVEN_DATA , function (_, data)
        self:__SetShowUI()
    end)

    --七天奖励
    self:BindGlobalEvent(Macros.Event.PushSevenModule.PUSH_SEVEN_AWARD , function (_, data)
        self:ShowAward( data )
    end)

    --七天下次
    self:BindGlobalEvent(Macros.Event.PushSevenModule.PUSH_SEVEN_NEX , function (_, index)
        if index == 1 then
            LogicGUI.TemplPushSevenNexTips.New( index , self.Panel_timing , self.Image_icon[index + 1] )
        end
    end)
end

function Modules.PushSevenView:__Getwidget( ... )
    -- 内容背景
    self.Image_morrow = self:GetWidget("Image_morrow") 
    --七天奖励内容
    self.Image_seven_icon = self:GetWidget("Image_seven_icon") 
    self.Image_seven_icon:SetVisible(false)
    --按钮
    self.Button_btn = Util:WidgetButton(  
                                            self:GetWidget("Button_btn") , 
                                            "" , 
                                            CPPGameLib.Handler(self , self.OnButtonBtn) ,
                                            nil,nil,Macros.Color.button_red
                                            ) 
    Util:GreyButton( self.Button_btn )

    --按钮
    self.Button_close = Util:WidgetButton(  
                                            self:GetWidget("Button_close") , 
                                            "" , 
                                            CPPGameLib.Handler(self , self.OnButtonClose) 
                                        )

    -- 节点容器
    self.Panel_timing = self:GetWidget("Panel_timing") 

    --图标
    self.Image_icon = {}
    for i = 1 , 7 do
        self.Image_icon[i] = self:GetWidget("Image_icon_"..i) 
        self.Image_icon[i]:SetImage(Resource.PathTool.GetUIResPath("push/seven/icon_" .. i , true ),TextureResType.LOCAL)
    end

    --装备框
    self.Panel_award = self:GetWidget("Panel_award") 
    
    --装备框文字
    self.Image_seven_text = self:GetWidget("Image_seven_text")
    self.Image_seven_text:SetVisible(false)
end

function Modules.PushSevenView:__ShowUI(  )

    --背景
    self.Image_morrow:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PUSH_SEVEN, "sp_seven_bg" ) )

    --彩带
    self.seven_ribbon = {}
    --特效
    self.seven_effect = {}
    --图标遮罩
    self.seven_icon_hide = {}
    --彩带遮罩
    self.seven_ribbon_hide = {}
    --极品
    self.seven_acura = {}
    --已领取
    self.seven_type = {}

    local sp_seven_ribbon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PUSH_SEVEN, "sp_seven_ribbon" )
   
    local sp_seven_icon_hide = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PUSH_SEVEN, "sp_seven_icon_hide" )
   
    local sp_seven_ribbon_hide = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PUSH_SEVEN, "sp_seven_ribbon_hide" )

    local seven_get = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true )

    for i = 1 , #self.Image_icon do

        self.seven_ribbon[i] = Util:Sprite( sp_seven_ribbon )
        self.seven_ribbon[i]:SetAnchorPoint( 0.5 , 0 )
        self.seven_ribbon[i]:SetPosition( self.Image_icon[i]:GetContentWidth()/2 + 3 , -6 )
        self.Image_icon[i]:AddChild( self.seven_ribbon[i] , 5 )

        local day = Util:Label( CPPGameLib.GetString("activity_dateValue" , CPPGameLib.GetString("common_week" .. i ) ) , Macros.TypefaceSize.slightly , Macros.Color.content )
        day:SetAnchorPoint( 0.5 , 0.5 ) 
        day:SetPosition( self.seven_ribbon[i]:GetContentWidth()/2 - 3 , self.seven_ribbon[i]:GetContentHeight()/2 + 5 )
        self.seven_ribbon[i]:AddChild( day )

        --特效
        self.seven_effect[i] ,effect = self:__CreateEffect({res_name = "UI_qitianleiji"} , self.Image_icon[i])
        PosTool.Center( effect )
        GlobalEffectMgr:SetEffectVisible(self.seven_effect[i] , false )

        --图标遮罩
        self.seven_icon_hide[i] = Util:Sprite( sp_seven_icon_hide )
        self.seven_icon_hide[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.Image_icon[i]:AddChild( self.seven_icon_hide[i] )
        PosTool.Center( self.seven_icon_hide[i] )
        self.seven_icon_hide[i]:SetVisible( false )

        --彩带遮罩
        self.seven_ribbon_hide[i] = Util:Sprite( sp_seven_ribbon_hide )
        self.seven_ribbon_hide[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.seven_ribbon[i]:AddChild( self.seven_ribbon_hide[i] , 5 )
        PosTool.Center( self.seven_ribbon_hide[i] , -3 , 4 )
        self.seven_ribbon_hide[i]:SetVisible( false )

        local cnf = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLoginCnf" , i )
        local icon = LogicGUI.IconLayer.New()
        icon:SetData({ id = cnf.reward[1].id })
        icon:SetAnchorPoint(0.5,0.5)
        self.Image_icon[i]:AddChild(icon:GetNode() , 999 )
        PosTool.Center( icon:GetNode() )
        icon:GetNode() :SetOpacity(0)  

        --极品
        local cnf = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLoginCnf" , i )
        if cnf.best and cnf.best == 1 then
            --极品
            self.seven_acura[i] = Util:Sprite(Resource.PathTool.GetUIResPath("push/seven/seven_acura" , true ) )
            self.seven_acura[i]:SetAnchorPoint( 0 , 1 )  
            self.seven_acura[i]:SetPosition( 1 , self.Image_icon[i]:GetContentHeight() - 2 )
            self.Image_icon[i]:AddChild( self.seven_acura[i] )
        elseif cnf.best and cnf.best == 2 then
            --极品
            self.seven_acura[i] = Util:Sprite(Resource.PathTool.GetUIResPath("push/seven/seven_acura1" , true ) )
            self.seven_acura[i]:SetAnchorPoint( 0 , 1 )  
            self.seven_acura[i]:SetPosition( 1 , self.Image_icon[i]:GetContentHeight() - 2 )
            self.Image_icon[i]:AddChild( self.seven_acura[i] )
        elseif cnf.best and cnf.best == 3 then
            --极品
            self.seven_acura[i] = Util:Sprite(Resource.PathTool.GetUIResPath("push/seven/seven_acura2" , true ) )
            self.seven_acura[i]:SetAnchorPoint( 0 , 1 )  
            self.seven_acura[i]:SetPosition( 1 , self.Image_icon[i]:GetContentHeight() - 2 )
            self.Image_icon[i]:AddChild( self.seven_acura[i] )
        end

        --类型
        self.seven_type[i] = Util:Sprite( seven_get )
        self.seven_type[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.seven_type[i]:SetPosition( self.Image_icon[i]:GetContentWidth()/2 - 15 , self.Image_icon[i]:GetContentHeight()/2 + 15 )
        self.Image_icon[i]:AddChild( self.seven_type[i] , 10 )
        self.seven_type[i]:SetVisible( false )
    end

    self:SetIconHeadline()

    --奖励
    local layout = Util:Layout( self.Panel_award:GetContentWidth() - 80 , self.Panel_award:GetContentHeight() - 40 )
    layout:SetAnchorPoint( 0.5 , 0 )
    layout:SetPosition( self.Panel_award:GetContentWidth()/2 , 15 )  
    self.Panel_award:AddChild( layout )

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "draw" , true)
    self.recharge = Util:Sprite( sp )
    self.recharge:SetAnchorPoint( 0.5 , 0.5 )
    self.recharge:SetPosition( self.Button_btn:GetContentWidth()/2 , self.Button_btn:GetContentHeight()/2 ) 
    self.Button_btn:AddChild( self.recharge )
    self.recharge:SetGray( true )

    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.Button_btn )
    btnEffect_con:SetPosition(self.Button_btn:GetContentWidth()/2 , self.Button_btn:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )

    local tips = Util:Label( CPPGameLib.GetString("pushSevev_tips") , Macros.TypefaceSize.normal , Macros.Color.white )
    tips:SetAnchorPoint( 0.5 , 0.5 ) 
    tips:SetPosition( self.Button_btn:GetPositionX() , self.Button_btn:GetPositionY() - 55 )
    self.Image_morrow:AddChild( tips )

    local cnf = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLoginCnf" )
    local endcnf = cnf[ #cnf ]
    CPPGameLib.PrintTable("xy" , endcnf , "endcnf") 
    if not endcnf.showLst or #endcnf.showLst <= 0 then
        return
    end
end

function Modules.PushSevenView:__SetShowUI()
    local seven_get = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true )
    local sp_seven_end = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_end" , true )
    for i = 1 , #self.Image_icon do
        self.seven_icon_hide[i]:SetVisible( false )
        self.seven_ribbon_hide[i]:SetVisible( false )
        self.seven_type[i]:SetVisible( false )
        GlobalEffectMgr:SetEffectVisible(self.seven_effect[i] , false )

        local type_ = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLogin" , i )
        --未登录
        if type_ == "NoRegister" then
        --登录
        elseif type_ == "Register" then
            GlobalEffectMgr:SetEffectVisible(self.seven_effect[i] , true )
        --已领取
        elseif type_ == "Draw" then
            self.seven_type[i]:SetImage( seven_get )
            self.seven_icon_hide[i]:SetVisible( true )
            self.seven_ribbon_hide[i]:SetVisible( true )
            self.seven_type[i]:SetVisible( true )
            GlobalEffectMgr:SetEffectVisible(self.seven_effect[i] , false )
        --过期
        elseif type_ == "Overdue" then
            self.seven_type[i]:SetImage( sp_seven_end )
            self.seven_icon_hide[i]:SetVisible( true )
            self.seven_ribbon_hide[i]:SetVisible( true )
            self.seven_type[i]:SetVisible( true )
            GlobalEffectMgr:SetEffectVisible(self.seven_effect[i] , false )
        end
    end

    self:__SetBtn()
end

--设置图标标题
function Modules.PushSevenView:SetIconHeadline()


    local index = 0
    for i = 1 , #self.Image_icon do
        local type_ = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLogin" , i )
        if type_ == "Register" or type_ == "Draw" then
            index = index + 1
        end
    end

    local sevenDayViewCnf = CPPGameLib.GetConfig("SevenDayView", index , false )

    if not sevenDayViewCnf then
        sevenDayViewCnf = CPPGameLib.GetConfig("SevenDayView", 999 , false )
    end
    CPPGameLib.PrintTable("xy" , sevenDayViewCnf , "sevenDayViewCnf")  
    --奖励文字
    if sevenDayViewCnf.sp_lab then
        for i , v in ipairs( sevenDayViewCnf.sp_lab ) do
            local sp_lab = Util:Sprite( Resource.PathTool.GetUIResPath("push/seven/"..v , true ) , function( ... )
                -- body
            end )
            sp_lab:SetAnchorPoint( 0.5 , 0.5 )
            self.Panel_award:AddChild( sp_lab )

            --坐标
            if sevenDayViewCnf.sp_lab_pos[i] then
                PosTool.Center( sp_lab , sevenDayViewCnf.sp_lab_pos[i][1] , sevenDayViewCnf.sp_lab_pos[i][2] )
            else
                PosTool.Center( sp_lab )
            end

            --层级
            if sevenDayViewCnf.sp_lab_zod[i] then
                sp_lab:SetZOrder( sevenDayViewCnf.sp_lab_zod[i] )
            end
        end
    end

    --奖励内容
    if sevenDayViewCnf.sp_award then
        for i , v in ipairs( sevenDayViewCnf.sp_award ) do
            local sp_award = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PUSH_SEVEN, v ) )
            sp_award:SetAnchorPoint( 0.5 , 0.5 )
            self.Panel_award:AddChild( sp_award )

            --坐标
            if sevenDayViewCnf.sp_award_pos[i] then
                PosTool.Center( sp_award , sevenDayViewCnf.sp_award_pos[i][1] , sevenDayViewCnf.sp_award_pos[i][2] )
            else
                PosTool.Center( sp_award )
            end

            --层级
            if sevenDayViewCnf.sp_award_zod[i] then
                sp_award:SetZOrder( sevenDayViewCnf.sp_award_zod[i] )
            end
        end
    end

    --特效
    if sevenDayViewCnf.sp_effects then
        for i , v in ipairs( sevenDayViewCnf.sp_effects ) do
            local pos = {}
            pos.x = 0
            pos.y = 0
            --坐标
            if sevenDayViewCnf.sp_effects_pos[i] then
                pos.x = sevenDayViewCnf.sp_effects_pos[i][1]
                pos.y = sevenDayViewCnf.sp_effects_pos[i][2]
            end

            local zorder = nil
            if sevenDayViewCnf.sp_effects_zod[i] then
                zorder = sevenDayViewCnf.sp_effects_zod[i]
            end

            local effect_id , effect = self:__CreateEffect({res_name = v } , self.Panel_award , zorder )
            PosTool.Center(effect , pos.x , pos.y )  
        end
    end

end

function Modules.PushSevenView:__SetBtn()
    local t1 = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetMayDraw" )
    if #t1 > 0 then
        self.Button_btn:SetEnabled( true )
        self.recharge:SetGray( false )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true )
    else
        Util:GreyButton( self.Button_btn )
        self.recharge:SetGray( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    end
end

function Modules.PushSevenView:OnButtonBtn()
    local t1 = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetMayDraw" )
    if #t1 <= 0 then
        -- GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
    else
        local info = {}
        info.day = t1[1].id
        CallModuleFunc(ModuleType.PUSH_SEVEN , "TakeSevenDayLoginRewardReq" , info )
    end
end

function Modules.PushSevenView:OnButtonClose()
    if unpack(self.__open_data) == "push" then 
        self:CloseManual(true)
        CallModuleFunc(ModuleType.PUSH,"Transfer",Macros.Game.SystemID.PUSH_SEVEN)
    else
        self:CloseManual()
    end
end

function Modules.PushSevenView:ShowAward( index )
    if self.seven_type[index] then
        ActionManager.GetInstance():Stamp( self.seven_type[index] , function ( ... )
            CallModuleFunc(ModuleType.PUSH_SEVEN , "SevenDayLoginDataReq" )
        end , 5 , 0.8 , 1 )
    end
end



--
-- @brief 明天获得
-- @author: yjg
-- @date: 2018年3月7日14:58:14
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPushSevenNexTips = LogicGUI.TemplPushSevenNexTips or BaseClass(GUI.Template)

function LogicGUI.TemplPushSevenNexTips:__init( index , bg , icon )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self:InitTemplate(nil)
    self:__Getwidget()
    self:__ShowUI( index , bg , icon )
end

function LogicGUI.TemplPushSevenNexTips:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( Macros.Global.DesignSize.width , Macros.Global.DesignSize.height , function ( ... )
        self:DeleteMe()
    end)
    self.layout:SetClippingEnable(false)
    self.layout:SetAnchorPoint(0.5,0.5)
    return self.layout
end

function LogicGUI.TemplPushSevenNexTips:__delete()
    --背景特效
    if self.effect_id then
        self:__DelEffect(self.effect_id)
        self.effect_id = nil
    end  
end

function LogicGUI.TemplPushSevenNexTips:__Getwidget()
end

function LogicGUI.TemplPushSevenNexTips:__ShowUI( index , bg , icon )


    self.bg_tou = true
    self.bg = Util:Layout( Macros.Global.DesignSize.width , Macros.Global.DesignSize.height , function ( ... )
        if self.bg_tou == true then
            self.bg_tou = false
        else
            return 
        end

        GlobalEffectMgr:SetEffectVisible( self.effect_id , false)

        CPPActionManager.FadeOut( self.bg , 0 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
            local pos = NodeUnity.ConvertPosBetweenTwoContainer( bg , self.layout , COCOPoint( icon:GetPositionX() , icon:GetPositionY() ) )
            ActionManager.GetInstance():RunScaleToAndMoveToEffectEx( self.icon, 0.5 , pos.x , pos.y , 0.1 , function ()
                local effectid,effect = self:__CreateEffect({id = 100028 , loop = false , time_scale = 1 , sacle = 0.2 } , self.layout , 100)
                effect:SetPosition( pos.x , pos.y )

                CPPActionManager.ScaleTo(effect, 0.2, 0.4, GlobalCallbackMgr:AddCallBackFunc(function()
                    CPPActionManager.ScaleTo(effect, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()
                        self.icon:SetVisible(false)
                        self:DeleteMe()
                    end))
                end))
            end)
        end) )

    end)
    self.bg:SetAnchorPoint(0.5,0.5)
    self.layout:AddChild( self.bg )
    PosTool.Center( self.bg )

    local sp = Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_sevev_nexday")
    local bg_sp_sevev_nexday = Util:Sprite( sp )
    bg_sp_sevev_nexday:SetAnchorPoint( 0.5 , 0.5 )
    self.bg:AddChild( bg_sp_sevev_nexday )
    PosTool.Center( bg_sp_sevev_nexday )

    self.effect_id, effect = self:__CreateEffect({id = 100101}, bg_sp_sevev_nexday )
    effect:SetPosition( bg_sp_sevev_nexday:GetContentWidth()/2 , bg_sp_sevev_nexday:GetContentHeight() - 20 )


    local cnf = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLoginCnf" , index + 1 )

    self.icon = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PUSH_SEVEN, "two_dayget_conten" ) )
    self.icon:SetAnchorPoint( 0.5 , 0.5 )
    self.icon:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
    self.layout:AddChild( self.icon )

    local type_ , info_ = ObjIDTool.GetInfo( cnf.reward[1].id )
    CPPGameLib.PrintTable("xy" , info_ , "info_------------")  
    local name = Util:Name( info_.name , Macros.TypefaceSize.normal , info_.init_quality or info_.quality )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( bg_sp_sevev_nexday:GetContentWidth()/2 , 30 )
    bg_sp_sevev_nexday:AddChild( name )

    local hero_recruit_touchscreen_2 = Util:Name( CPPGameLib.GetString("hero_recruit_touchscreen_2") , Macros.TypefaceSize.normal )
    hero_recruit_touchscreen_2:SetPosition( bg_sp_sevev_nexday:GetContentWidth()/2 , -75 )
    bg_sp_sevev_nexday:AddChild( hero_recruit_touchscreen_2 )
end