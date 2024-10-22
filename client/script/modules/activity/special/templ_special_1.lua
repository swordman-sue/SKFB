--
-- @brief 节日活动
-- @author: yjg
-- @date: 2017年9月13日17:02:30
--

Modules = Modules or {}

Modules.TemplSpecialItem = Modules.TemplSpecialItem or BaseClass(GUI.ScrollItem)

--单充回调
function Modules.TemplSpecialItem:OnOncePay( data )
    print("xy" , "单充回调")
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.take_reward_times < data.cnf.reward_times then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D> ".. num .. " </COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34> ".. num .. " </COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
   if info and info.take_reward_times < data.cnf.reward_times and info.take_reward_times ~= info.context then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function ( ... )
            self:Sell( data )
        end)
    --不可领取
    elseif not info or info.take_reward_times < data.cnf.reward_times then
        self:SetNoGetBtn( CPPGameLib.GetString("common_pay") , function ( ... )
            CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
        end )
    else
        self:SetAlreadyGet()
    end
end

--累充回调
function Modules.TemplSpecialItem:OnAddPpPay( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id , data.type_ )
    local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
    local x = data.cnf.x * addUpAddition
    local num = 0
    if info then
        num = info.context 
    end
    if data.cnf.desc then
        if info and num >= x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )    
    --可以领取
    if info and num >= x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function ( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or num < x then
        self:SetNoGetBtn( CPPGameLib.GetString("common_pay") , function ( ... )
            CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
        end )
    --结束
    elseif info and info.take_reward_times >= data.cnf.reward_times then
        self:SetAlreadyGet()
    end
end

--打折
function Modules.TemplSpecialItem:OnSalePay( data )
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    local diamond_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    local _ , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem" , data.id , data.type_ )
    --可以领取
    if (not info and diamond_num >= data.cnf.x) or ( info and info.take_reward_times < data.cnf.reward_times and diamond_num >= data.cnf.x ) then
        self.button:SetText( CPPGameLib.GetString("rebel_mai") )
        self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_biggreen2" ) )
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        self.button:SetOutline(r, g, b, a, 2)
        WidgetUnity.SetWidgetListener( self.button , nil, nil, function ( ... )

            local max = data.cnf.reward_times
            if info then
                max = data.cnf.reward_times - info.take_reward_times 
            end
            local buy_num =  diamond_num / data.cnf.x 
            if buy_num < max then
                max = buy_num
            end
            local t1 = {}
            table.insert(t1,{ 
                             type = Macros.Game.RolePropType.DIAMOND ,        --货币类型
                             num = data.cnf.x                                --什么事情都没有的单价
                         })

            local info = {}
            --道具id
            info.item_id = data.cnf.reward_item_id_list[1] 
            --价格列表
            info.cost = t1
            -- 最大
            info.max = max
            --图标显示的单价
            info.num = data.cnf.reward_item_num_list[1]
            --func
            info.func = function ( index )
                self:Exchange( data , index )
            end
            GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )

        end, nil, 0.5*1000)
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
    --不可领取
    -- elseif diamond_num < data.cnf.x and (info and info.take_reward_times < data.cnf.reward_times) then
    elseif (not info and diamond_num < data.cnf.x) or ( info and info.take_reward_times < data.cnf.reward_times and diamond_num < data.cnf.x ) then
        self:SetNoGetBtn( CPPGameLib.GetString("vip_get") , function ( ... )
            local tips = LogicGUI.TemplAffirmTips.New( { 
                                                            type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                            content = CPPGameLib.GetString("fund_noDiamond") ,
                                                            cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                            confirm_func = function ( ... )
                                                                CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
                                                            end
                                                        } )
        end )
    else
        self.showittem:SetVisible(false)
        self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW , "sp_activity_sellout" , true ) )
        self.button:SetTouchEnabled(false)
        self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 )
    end
end

--兑换
function Modules.TemplSpecialItem:OnConverPay( data )
    local cnd = CPPGameLib.GetConfig("Item", data.cnf.x )
    if not cnd then return end
    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", data.cnf.x )
    if item_num >= data.cnf.y then
        self.desc:SetText( cnd.name .. " " ..  "<COLOR VALUE=FF6EF12D>".. item_num .. "</COLOR>" .. "/" .. data.cnf.y )
    else
        self.desc:SetText( cnd.name .. " " ..  "<COLOR VALUE=FFFF4C34>".. item_num .. "</COLOR>" .. "/" .. data.cnf.y )
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    local _ , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem" , data.id , data.type_ )
    --可以领取
    if ( not info and item_num >= data.cnf.y) or (info and info.take_reward_times < data.cnf.reward_times and item_num >= data.cnf.y ) then
        self.button:SetText( CPPGameLib.GetString("notice_sell_") )
        self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_biggreen2" ) )
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        self.button:SetOutline(r, g, b, a, 2)
        WidgetUnity.SetWidgetListener( self.button , nil, nil, function ( ... )
            local max = data.cnf.reward_times
            if info then
                max = data.cnf.reward_times - info.take_reward_times 
            end
            diamond_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", data.cnf.x )
            local buy_num =  diamond_num / data.cnf.y 
            if buy_num < max then
                max = buy_num
            end
            local t1 = {}
            table.insert(t1,{ 
                             type_id = data.cnf.x ,        --货币类型
                             num = data.cnf.y                                                     --什么事情都没有的单价
                         })
            local info = {}
            --道具id
            info.item_id = data.cnf.reward_item_id_list[1] 
            --价格列表
            info.cost = t1
            -- 最大
            info.max = max
            --图标显示的单价
            info.num = data.cnf.reward_item_num_list[1]
            --标题
            info.title = CPPGameLib.GetString("special_exchange")
            --func
            info.func = function ( index )
                self:Exchange( data , index )
            end
            GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
        end, nil, 0.5*1000)
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
    --不可领取
    elseif item_num < data.cnf.y or (info and info.take_reward_times < data.cnf.reward_times) then
        self:SetNoGetBtn( CPPGameLib.GetString("notice_sell_") , function ( ... )
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , cnd.name ) )
        end )
    else
        self.showittem:SetVisible(false)
        self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW , "sp_activity_sellout" , true ) )
        self.button:SetTouchEnabled(false)
        self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 )
    end
end

--登录
function Modules.TemplSpecialItem:OnEnter( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context ~= 1 then
        self.button:SetText( CPPGameLib.GetString("manor_get") )
        self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_biggreen2" ) )
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        self.button:SetOutline(r, g, b, a, 2)
        WidgetUnity.SetWidgetListener( self.button , nil, nil, function ( ... )
            print("xy" , "不可领取" )
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("special_register" , data.cnf.x ) )
        end, nil, 0.5*1000)
        Util:GreyButton( self.button )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
    else
        self:SetAlreadyGet()
    end
end

--主线
function Modules.TemplSpecialItem:OnMeanLine( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local chapter_index,gamelevel_index = CallModuleFunc(ModuleType.DUNGEON,"GetCurDungeonLatestChapterAndGameLevelByType",Macros.Game.FBType.MAIN_DUNGEON) 
            GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DUNGEON_GAMELEVEL,{chapter_index,gamelevel_index})
        end )
    else
        self:SetAlreadyGet()
    end
end

--圣器
function Modules.TemplSpecialItem:OnPYX( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.PLUNDEREDTREASURES )
            if openType then
                CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "OpenView", BaseViewType.PLUNDEREDTREASURES)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--竞技场
function Modules.TemplSpecialItem:OnArena( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.ARENA )
            if openType then
                CallModuleFunc(BaseViewType.ARENA,"OpenView",BaseViewType.ARENA)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--深渊
function Modules.TemplSpecialItem:OnChasm( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.CHASM )
            if openType then
                CallModuleFunc(BaseViewType.CHASM,"OpenView",BaseViewType.CHASM)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--召唤
function Modules.TemplSpecialItem:OnCall( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.HERO_RECRUIT )
            if openType then
                CallModuleFunc(ModuleType.HERO , "OpenView", BaseViewType.HERO_RECRUIT)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--开服登录
function Modules.TemplSpecialItem:OpenService( data )
    local _ , server_opTime = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
    local time = server_opTime + ( data.cnf.x * 24 * 60 * 60 ) - ( 24 * 60 * 60  )
    local  year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( time )
    local text = TimeTool.GetTimeFormat( time  , TimeTool.defaul_format[18] )
    self.desc:SetText( string.format( data.cnf.desc , text  ) )
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    --可以领取
    if info and info.context and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context ~= 1 then
        self.button:SetText( CPPGameLib.GetString("manor_get") )
        self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_biggreen2" ) )
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        self.button:SetOutline(r, g, b, a, 2)
        WidgetUnity.SetWidgetListener( self.button , nil, nil, function ( ... )
            print("xy" , "不可领取" )
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("special_register" , data.cnf.x ) )
        end, nil, 0.5*1000)
        Util:GreyButton( self.button )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
    else
        self:SetAlreadyGet()
    end
end

--组队副本通关
function Modules.TemplSpecialItem:OnTeamPlayer( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.LIMIT_TEAM )
            if openType then
                CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--每日通关精英副本
function Modules.TemplSpecialItem:OnExerydayElite( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then

        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.DUNGEON_ELITE )
            if openType then
                GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DUNGEON_ELITE)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计勇者试炼重置
function Modules.TemplSpecialItem:OnOverallTrial( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.TOWER )
            if openType then
                CallModuleFunc(BaseViewType.TOWER,"OpenView",BaseViewType.TOWER)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计攻击叛军
function Modules.TemplSpecialItem:OnOverallRebel( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.REBEL )
            if openType then
                CallModuleFunc(BaseViewType.REBEL,"OpenView",BaseViewType.REBEL)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计挑战无尽深渊
function Modules.TemplSpecialItem:OnOverallAccro( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.CHASM )
            if openType then
                CallModuleFunc(BaseViewType.CHASM,"OpenView",BaseViewType.CHASM)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计挑战王者赛
function Modules.TemplSpecialItem:OnOverallKing( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.LADDER )
            if openType then
                CallModuleFunc(BaseViewType.LADDER,"OpenView",BaseViewType.LADDER)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计占卜翻牌
function Modules.TemplSpecialItem:OnOverallLowAugury( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then

        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.DIVINATION )
            if openType then
                CallModuleFunc(BaseViewType.DIVINATION,"OpenView",BaseViewType.DIVINATION)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计高级占卜翻牌
function Modules.TemplSpecialItem:OnOverallHighAugury( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.DIVINATION )
            if openType then
                CallModuleFunc(BaseViewType.DIVINATION,"OpenView",BaseViewType.DIVINATION)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--卡牌相关活动
function Modules.TemplSpecialItem:OnCardActivity( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.DIVINATION )
            if openType then
                CallModuleFunc(BaseViewType.DIVINATION,"OpenView",BaseViewType.DIVINATION)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--历收藏值最高达到
function Modules.TemplSpecialItem:OnCardCollect( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.DIVINATION )
            if openType then
                CallModuleFunc(BaseViewType.DIVINATION,"OpenView",BaseViewType.DIVINATION)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--活动期间消耗
function Modules.TemplSpecialItem:OnActivityConsume( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            CallModuleFunc(BaseViewType.CHARGE,"OpenView",BaseViewType.CHARGE)
        end )
    else
        self:SetAlreadyGet()
    end
end

--英雄培养
function Modules.TemplSpecialItem:OnHeroTrain( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.HERO_TRAIN )
            if openType then
                CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_PROMOTE, BaseViewSubType.HeroPromote.TRAIN, uid)
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--精灵培养
function Modules.TemplSpecialItem:OnStringTrain( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.SPIRIT )
            if openType then
                CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT )
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计扭蛋次数
function Modules.TemplSpecialItem:OnTomyItems( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.SPIRIT )
            if openType then
                CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT )
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计装备精炼次数
function Modules.TemplSpecialItem:OnEquipRefine( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.SPIRIT )
            if openType then
                CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT )
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end

--累计特殊扭蛋次数
function Modules.TemplSpecialItem:OnNDLottery( data )
    local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", self.__data.id  , data.type_ )
    local num = 0
    if info then
        num = info.context
    end
    if data.cnf.desc then
        if info and info.context >= data.cnf.x then
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FF6EF12D>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FF6EF12D> " )
        else
            -- self.desc:SetText( string.format( data.cnf.desc ,  "<COLOR VALUE=FFFF4C34>".. num .. "</COLOR>"  ) )
            self:SetColor( data.cnf.desc , num , "<COLOR VALUE=FFFF4C34> " )
        end
    end
    self.button:SetVisible(true)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
    --可以领取
    if info and info.context >= data.cnf.x and info.take_reward_times < data.cnf.reward_times then
        self:SetGetBtn( CPPGameLib.GetString("manor_get") , function( ... )
            self:Sell( data )
        end )
    --不可领取
    elseif not info or info.context < data.cnf.x then
        self:SetNoGetBtn( CPPGameLib.GetString("dungeon_go_forward") , function ( ... )
            local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.GASHAPONMACHINE )
            if openType then
                CallModuleFunc(ModuleType.LIMIT_ACTIVITY, "OpenView", BaseViewType.GASHAPONMACHINE )
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_level_open" , assist_info.level ) )
            end
        end )
    else
        self:SetAlreadyGet()
    end
end


--设置已领取
function Modules.TemplSpecialItem:SetAlreadyGet()
    self.showittem:SetVisible(false)
    self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW , "sp_activity_get" , true ) )
    self.button:SetTouchEnabled(false)
    self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 )
end

--设置领取
function Modules.TemplSpecialItem:SetGetBtn( text , fun )
    self.button:SetText( text )
    self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_biggreen2" ) )
    local r, g, b, a = unpack( Macros.Color.button_yellow )
    self.button:SetOutline(r, g, b, a, 2)
    WidgetUnity.SetWidgetListener( self.button , nil, nil, function ( ... )
        print("xy" , "可以领取" )
        if fun then
            fun(  )
        end
    end, nil, 0.5*1000)

    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
end

--设置领取
function Modules.TemplSpecialItem:SetNoGetBtn( text , fun )
    self.button:SetText( text )
    self.button:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_bigred" ) )
    local r, g, b, a = unpack( Macros.Color.button_red )
    self.button:SetOutline(r, g, b, a, 2)
    WidgetUnity.SetWidgetListener( self.button , nil, nil, function ( ... )
        if fun then
            fun()
        end
    end, nil, 0.5*1000)

    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
end

--设置颜色
function Modules.TemplSpecialItem:SetColor( desc , num , color )
    self.desc:SetText( string.format( desc ,  color .. num .. " </COLOR>"  ) )
end



