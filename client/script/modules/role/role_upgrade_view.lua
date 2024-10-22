--
-- @brief 队伍升级 
-- @author: yjg
-- @date: 2016年11月22日09:49:42
--

Modules = Modules or {}

Modules.RoleUpgradeView = Modules.RoleUpgradeView or BaseClass(BaseView)
    
function Modules.RoleUpgradeView:__init()
    self.__layout_name = "teamupdate"  
    self.__render_queue = Scene.RenderQueue.GRQ_UI_UP

    self.__bg_params = {type = BaseView.BGType.SOLID, close_func = function()
    end}
    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ROLEUP, true)    
end

function Modules.RoleUpgradeView:__OpenCallback()
    --检测系统开放
    CallModuleFunc(ModuleType.ROLE, "CheckSystemOpened")

    self.__close_callback_ex,self.__sp_func = unpack(self.__open_data)
    
    self:__Getwidget()
    self:__ShowUI()

    self:__PlayEffect()
end

function Modules.RoleUpgradeView:__RegistAllEvents()
    --监听指引下一个步骤事件
    self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
        --TEMP：临时处理
        local lead_info = CallModuleFunc(ModuleType.LEAD, "GetLeadInfo")
        if lead_info and lead_info.condition and lead_info.condition[1] == Macros.Game.LeadConditionType.SYSTEM_OPENED then
            return
        end

        --新手指引特殊操作检测(战队升级预告前往（第一个）)
        if self.__lead_widget and self.__lead_params then
            CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ROLE_UPDATE_GOTO, self.__lead_widget, self.__lead_params)                                
        end          
    end)
end

function Modules.RoleUpgradeView:__Dispose()
    if self.__close_callback_ex then
        self.__close_callback_ex()
    end
end

function Modules.RoleUpgradeView:__Getwidget( ... )
    --居中层
    self.Panel_BG = self:GetWidget("Panel_BG") 
    self.Panel_BG:SetVisible(false)
    --特效层
    self.Panel_Shield = self:GetWidget("Panel_Shield") 
    --星星
    self.Image_28 = self:GetWidget("Image_28") 
    --特效层
    self.Panel_Shield1 = self:GetWidget("Panel_Shield1") 
    --闪光
    self.Panel_CircleEffect = self:GetWidget("Panel_CircleEffect") 
    --盾
    self.Image_Shield = self:GetWidget("Image_Shield") 
    --字样内容
    self.Panel_88 = self:GetWidget("Panel_88") 

    self.Image_1 = self:GetWidget("Image_1")
    self.Image_1:IgnoreContentAdaptWithSize( true )

    --等级行
    self.Panel_PropContainer1 = self:GetWidget("Panel_PropContainer1") 
    --等级t
    self.Text_CurPropTitle1 = self:GetWidget("Text_CurPropTitle1") 
    --等级v
    self.Text_CurProp1 = self:GetWidget("Text_CurProp1") 
    --下级v
    self.Text_NextProp1 = self:GetWidget("Text_NextProp1") 
    --方向箭头
    self.Panel_47_0 = self:GetWidget("Panel_47_0") 
    self.Image_27 = self:GetWidget("Image_27") 

    --体力行
    self.Panel_PropContainer2 = self:GetWidget("Panel_PropContainer2") 
    --体力t
    self.Text_CurPropTitle2 = self:GetWidget("Text_CurPropTitle2") 
    --体力v
    self.Text_CurProp2 = self:GetWidget("Text_CurProp2") 
    --下级体力v
    self.Text_NextProp2 = self:GetWidget("Text_NextProp2") 
    --方向箭头
    self.Panel_47_0 = self:GetWidget("Panel_47_0") 
    self.Image_27 = self:GetWidget("Image_27") 

    --精力行
    self.Panel_PropContainer3 = self:GetWidget("Panel_PropContainer3") 
    --精力t
    self.Text_CurPropTitle3 = self:GetWidget("Text_CurPropTitle3") 
    --精力v
    self.Text_CurProp3 = self:GetWidget("Text_CurProp3") 
    --下级精力v
    self.Text_NextProp3 = self:GetWidget("Text_NextProp3") 
    --方向箭头
    self.Panel_47_0 = self:GetWidget("Panel_47_0") 
    self.Image_27 = self:GetWidget("Image_27") 

    --彩带
    self.Image_ribbon = self:GetWidget("Image_ribbon") 
    --升级字样
    self.Image_upgrade = self:GetWidget("Image_upgrade") 
    self.Image_upgrade:IgnoreContentAdaptWithSize(true)
    --触摸字样
    self.Text_Continue = self:GetWidget("Text_Continue") 
    --触摸层
    self.Panel_LeadTouch = self:GetWidget("Panel_LeadTouch") 

    local panel_88 = self:GetWidget("Panel_88")
    panel_88:SetBackGroundImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_team_update"), TextureResType.LOCAL)
end

function Modules.RoleUpgradeView:__ShowUI( ... )
    --关闭
    WidgetUnity.SetWidgetListener(self.Panel_BG,nil,nil,function()
        if self.effects == false then
            self:CloseManual()
        end
    end)

    self.__panel_lead = self:GetWidget("Panel_LeadTouch")
    WidgetUnity.SetWidgetListener(self.__panel_lead,nil,nil,function()
        if self.effects == false then
            self:CloseManual()
            --特殊回调
            if self.__sp_func then
                self.__sp_func()
                self.__sp_func = nil
            end
        end
    end)

    local lab_continue = Util:WidgetLabel( self.Text_Continue , CPPGameLib.GetString("battle_result_continue") , Macros.TypefaceSize.tab )
    
    local role_info = self:CallModuleFunc("GetRoleInfo")
    local lv = role_info.level

    self.beforeLv = CPPGameLib.GetConfig("RoleLevel", lv - 1)

    self.lvInfo = CPPGameLib.GetConfig("RoleLevel", lv)

    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")

    --等级
    local beforeLvType = Util:WidgetLabel( self.Text_CurPropTitle1 , CPPGameLib.GetString("squadron_Lv") , Macros.TypefaceSize.tab, Macros.Color.golden , nil,nil , Macros.Color.common_stroke , 2  )

    --之前等级value
    local beforeLvValue = Util:WidgetLabel( self.Text_CurProp1 , self.beforeLv.level , Macros.TypefaceSize.tab , nil , nil , nil , Macros.Color.common_stroke , 2 )

    self.nowLvValue = Util:WidgetLabel( self.Text_NextProp1 , self.lvInfo.level , Macros.TypefaceSize.tab ,Macros.Color.green,nil,nil , Macros.Color.common_stroke , 2  )


    local beforeHPtype = Util:WidgetLabel( self.Text_CurPropTitle2 , CPPGameLib.GetString("squadron_laborer"),  Macros.TypefaceSize.tab, Macros.Color.golden,nil,nil , Macros.Color.common_stroke , 2 )
    --体力value
    local beforeHPvalue = Util:WidgetLabel( self.Text_CurProp2 , userDate.stamina - self.beforeLv.stamina , Macros.TypefaceSize.tab ,nil,nil,nil , Macros.Color.common_stroke , 2 )

    self.nowHPvalue = Util:WidgetLabel( self.Text_NextProp2 , userDate.stamina , Macros.TypefaceSize.tab ,Macros.Color.green,nil,nil , Macros.Color.common_stroke , 2 )

    --精力
    local beforeMPtype = Util:WidgetLabel( self.Text_CurPropTitle3 , CPPGameLib.GetString("squadron_vigor"), Macros.TypefaceSize.tab, Macros.Color.golden,nil,nil , Macros.Color.common_stroke , 2 )
   
    --精力value
    local beforeMPvalue = Util:WidgetLabel( self.Text_CurProp3 , userDate.energy - self.beforeLv.energy , Macros.TypefaceSize.tab ,nil,nil,nil , Macros.Color.common_stroke , 2 )

    self.nowMPvalue = Util:WidgetLabel( self.Text_NextProp3 , userDate.energy , Macros.TypefaceSize.tab ,Macros.Color.green,nil,nil , Macros.Color.common_stroke , 2 )
    

    local cnf = CPPGameLib.GetConfig("UpPrevue", lv)

    if not cnf then return end

    local layou = Util:Layout( 500 , 110 )
    layou:SetAnchorPoint(0.5,0.5)
    self.Panel_88:AddChild(layou)
    PosTool.Center(layou , 0 , - 85 )
    
    local bg_sp_upgrade_dispark = Util:Sprite(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_upgrade_dispark"))

    layou:SetContentSize(#cnf.open_lv * bg_sp_upgrade_dispark:GetContentWidth() + 10 , layou:GetContentHeight() )
    local interval = layou:GetContentWidth() / #cnf.open_lv

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

    self.effects = false

    for i,v in ipairs(cnf.open_lv) do

        local bg_sp_upgrade_dispark = Util:Sprite(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_upgrade_dispark"))
        bg_sp_upgrade_dispark:SetPosition( (bg_sp_upgrade_dispark:GetContentWidth()/2 + 5 ) + interval * (i- 1)  , layou:GetContentHeight()/2 + 10)
        bg_sp_upgrade_dispark:SetAnchorPoint(0.5,0.5)
        layou:AddChild(bg_sp_upgrade_dispark)

        local type_ = Util:Label("["..cnf.system_name[i].."]" , Macros.TypefaceSize.slightly , Macros.Color.itemName )
        type_:SetAnchorPoint( 0 , 1 )
        type_:SetPosition( bg_sp_upgrade_dispark:GetContentWidth()/2 - 50 , bg_sp_upgrade_dispark:GetContentHeight() - 15 )
        bg_sp_upgrade_dispark:AddChild(type_)

        local text_ = Util:Label(cnf.text[i] , Macros.TypefaceSize.slightly, Macros.Color.itemText , bg_sp_upgrade_dispark:GetContentWidth() - 85 , nil )
        text_:SetAnchorPoint( 0 , 1 )
        text_:SetPosition( bg_sp_upgrade_dispark:GetContentWidth()/2 - 50   , type_ :GetPositionY() - type_:GetContentHeight() - 6 )
        bg_sp_upgrade_dispark:AddChild(text_)   

        local xtcnf = CPPGameLib.GetConfig("SystemEntry", cnf.system_type[i] )

        local team_icon = nil
        if xtcnf.team_icon then
            team_icon = Resource.PathTool.GetSystemEntryIconPath(xtcnf.team_icon)
        else
            team_icon = Resource.PathTool.GetSystemEntryIconPath("btn_guild")
        end

        local icon = Util:Sprite( team_icon )
        icon:SetPosition( bg_sp_upgrade_dispark:GetContentWidth()/2 - 80, bg_sp_upgrade_dispark:GetContentHeight()/2 )
        icon:SetScaleFactor(cnf.icon_zoom)
        bg_sp_upgrade_dispark:AddChild( icon )

       if cnf.open_lv[i] > lv then
            local open = Util:Label( CPPGameLib.GetString("reinforcements_lockLevelLimit",cnf.open_lv[i] ) , Macros.TypefaceSize.content, Macros.Color.red,nil,nil , Macros.Color.common_stroke , 2 )
            open:SetAnchorPoint( 0.5 , 0.5 )
            open:SetPosition( bg_sp_upgrade_dispark:GetContentWidth()/2   , bg_sp_upgrade_dispark:GetContentHeight()/2 - 60)
            bg_sp_upgrade_dispark:AddChild(open)
        else
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sprite_open", true)
            local sprite_open = Util:Sprite( sp )
            sprite_open:SetPosition( icon:GetPositionX() - 25 , icon:GetPositionY() + 28 )
            bg_sp_upgrade_dispark:AddChild( sprite_open )

            if v == role_level then
                self.effects = false
                sprite_open:SetVisible(false)
                CPPActionManager.DelayTo(  sprite_open , 0.9 , GlobalCallbackMgr:AddCallBackFunc(function()
                    ActionManager.GetInstance():Stamp( sprite_open , function( ... )
                        local effect_id, effect = self:__CreateEffect({res_name = "UI_shengjigongnengkaiqi" , delay_time = 0.9 , delay_callback = function ( ... )
                            self.effects = false
                        end}, bg_sp_upgrade_dispark )
                        PosTool.Center(effect, 0, 0)  
                    end )
                end))
            end
        
            local open = Util:Label( CPPGameLib.GetString("squadron_opened") , Macros.TypefaceSize.content, Macros.Color.green , nil,nil , Macros.Color.green_stroke , 2 )
            open:SetAnchorPoint( 0.5 , 0.5 )
            open:SetPosition( bg_sp_upgrade_dispark:GetContentWidth()/2   , bg_sp_upgrade_dispark:GetContentHeight()/2 - 60)
            bg_sp_upgrade_dispark:AddChild(open)

            local btn = Util:Layout( bg_sp_upgrade_dispark:GetContentWidth() , bg_sp_upgrade_dispark:GetContentHeight() , function ( ... )               
                if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
                    GUITemplateMgr:Dump()

                    --指引中，关闭主界面外的所有界面
                    GlobalViewMgr:CloseViewsExcludeCity()
                else
                    --非指引中，跳转到对应系统
                    GlobalModulesMgr:OpenSystemModule(cnf.system_type[i], nil, false)
                    --关闭夺宝战五次界面
                    GlobalEventSystem:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW)
                    self:CloseManual()
                end
            end)
            btn:SetAnchorPoint(0.5,0.5)
            bg_sp_upgrade_dispark:AddChild(btn)
            PosTool.Center( btn )

            --新手指引特殊操作检测(战队升级预告前往（第一个）)
            if i == 1 then
                self.__lead_widget = btn
                self.__lead_params = cnf.system_type[i]
            end
        end     
    end
end

function Modules.RoleUpgradeView:__CheckSectionOperateSP()
    --新手指引特殊操作检测(战队升级-继续)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TEAM_UPDATE_CONTINUE, self.Panel_LeadTouch)

    --新手指引特殊操作检测(战队升级预告前往（第一个）)
    if self.__lead_widget and self.__lead_params then
        CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ROLE_UPDATE_GOTO, self.__lead_widget, self.__lead_params)                                
    end
end

function Modules.RoleUpgradeView:__PlayEffect()
    --出场特效
    local effect_id, effect = self:__CreateEffect({id = 100016, loop = false, delay_time = 0.9, delay_callback = function()
        self.Panel_BG:SetVisible(true)
        --皇冠特效
        local effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_CircleEffect"))
        PosTool.CenterBottom(effect, 0, 0)  

        --播放CSB动画
        self:PlayActionTimeline("animation", false, function()
            self:__CheckSectionOperateSP()
        end)
    end}, self:GetWidget("Panel_Shield"))
    PosTool.Center(effect)  
end