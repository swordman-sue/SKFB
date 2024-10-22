
--
-- @brief pvp(主界面)
-- @author: yjg
-- @date: 2017年6月20日10:19:40
--

Modules = Modules or {}

Modules.PVPView = Modules.PVPView or BaseClass(BaseView)

function Modules.PVPView:__init()
    self.__layout_name = "pvp"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_pvp")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("pvp") ,
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
        	self:CloseManual()
        end,
    }
    self.__open_callback = function()
    	self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PVP)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PVP,true)  
end

function Modules.PVPView:__OpenCallback()   
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end

function Modules.PVPView:__RegistAllEvents()
end

function Modules.PVPView:__Dispose()		
end

function Modules.PVPView:__Server()    
    CallModuleFunc(ModuleType.PVP, "PlayerPVPDataReq")

    self:BindGlobalEvent(Macros.Event.PVPModule.PVP_DATA,function (_, data)
        self:__SetShowUI()
    end)

    self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_ON_FIGHT_RESP,function (_, data)
        self:__SetShowUI()
    end)
    
    self:BindGlobalEvent(Macros.Event.PVPModule.PVP_AWARD,function (_, data)
        self:__SetShowUI()
    end)

    self:BindGlobalEvent(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_TIME_RESP, function(_,system_id,show_txt)
        if Macros.Game.SystemID.PVP == system_id then
            self:Countdown( show_txt )
        end
    end)

    self:BindGlobalEvent(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_END, function(_,system_id,show_txt)
        if Macros.Game.SystemID.PVP == system_id then
            CallModuleFunc(ModuleType.PVP, "PlayerPVPDataReq")
        end
    end)
end

function Modules.PVPView:__Getwidget( ... )
    --居中层
    self.Panel_center = self:GetWidget("Panel_center") 
    --英雄
    self.Panel_hero = self:GetWidget("Panel_hero") 
    --名字
    self.Image_strength = self:GetWidget("Image_strength")

    --挑战
    self.Button_challenge =  Util:WidgetButton( 
                                                    self:GetWidget("Button_challenge") , 
                                                    CPPGameLib.GetString("pvp_challenge") , 
                                                    CPPGameLib.Handler(self,self.OnButtonChallenge)
                                                )

    -- --旗帜
    -- self.Image_flag = self:GetWidget("Image_flag")
    -- --开始匹配
    -- self.Image_matching = self:GetWidget("Image_matching")
    --挑战次数
    self.Text_time = self:GetWidget("Text_time")
    
    --排行
    self.Panel_rank = self:GetWidget("Panel_rank")
    -- self.Panel_rank:SetTouchEnabled(true)
    -- WidgetUnity.SetWidgetListener(self.Panel_rank, nil, nil, function ( ... )
    --     self:OnButtonRank()
    -- end)
    --荣誉榜
    self.Image_honor = self:GetWidget("Image_honor")

    --目标
    self.Panel_target = self:GetWidget("Panel_target") 
    --活动目标
    self.Image_activity = self:GetWidget("Image_activity")
    --每日奖励
    self.Button_everyday = Util:WidgetButton( 
                                                self:GetWidget("Button_everyday") , 
                                                nil , 
                                                CPPGameLib.Handler(self,self.OnButtonEveryday) 
                                            )
    local btn_rank = Resource.PathTool.GetSystemEntryIconPath("btn_rank")
    self.Button_everyday:SetNormalImage( btn_rank , TextureResType.LOCAL )
    self.Button_everyday:SetScale9Enabled( false )

    local Button_everyday = Util:Name( CPPGameLib.GetString("pvp_everyday") , Macros.TypefaceSize.largish )
    Button_everyday:SetAnchorPoint(0.5,0.5)
    Button_everyday:SetPosition(self.Button_everyday:GetContentWidth()/2 , 0)
    self.Button_everyday:AddChild(Button_everyday)

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.everyday_red = Util:Sprite(icon_red_point)
    self.everyday_red:SetPosition( self.Button_everyday:GetContentWidth() - 5  , self.Button_everyday:GetContentHeight() - 10 )
    self.Button_everyday:AddChild( self.everyday_red , 20 )
    

    --每周奖励
    self.Button_weekly = Util:WidgetButton( 
                                                self:GetWidget("Button_weekly") , 
                                                nil , 
                                                CPPGameLib.Handler(self,self.OnButtonWeekly) 
                                            )
    local btn_rank = Resource.PathTool.GetSystemEntryIconPath("btn_rank")
    self.Button_weekly:SetNormalImage( btn_rank , TextureResType.LOCAL )
    self.Button_weekly:SetScale9Enabled( false )

    local Button_weekly = Util:Name( CPPGameLib.GetString("pvp_weekly") , Macros.TypefaceSize.largish )
    Button_weekly:SetAnchorPoint(0.5,0.5)
    Button_weekly:SetPosition(self.Button_weekly:GetContentWidth()/2 , 0)
    self.Button_weekly:AddChild(Button_weekly)

    self.weekly_red = Util:Sprite(icon_red_point)
    self.weekly_red:SetPosition( self.Button_weekly:GetContentWidth() - 5  , self.Button_weekly:GetContentHeight() - 10 )
    self.Button_weekly:AddChild( self.weekly_red , 20 )

    self.Image_integral = {}
    for i = 1 , 3 do
        self.Image_integral[i] = self:GetWidget("Image_integral_"..i)
    end

    self.Text_MyPank = Util:WidgetLabel(self:GetWidget("Text_MyPank"), "" , Macros.TypefaceSize.normal, Macros.Color.white )

    self.Text_MyIntegral = Util:WidgetLabel(self:GetWidget("Text_MyIntegral"), "" , Macros.TypefaceSize.normal, Macros.Color.white )

    self.Image_division = self:GetWidget("Image_division")


    --每周奖励
    self.Button_everyday_0 = Util:WidgetButton( 
                                                self:GetWidget("Button_everyday_0") , 
                                                nil , 
                                                CPPGameLib.Handler(self,self.OnButtonRank) 
                                            )
    local btn_rank = Resource.PathTool.GetSystemEntryIconPath("btn_rank")
    self.Button_everyday_0:SetNormalImage( btn_rank , TextureResType.LOCAL )
    self.Button_everyday_0:SetScale9Enabled( false )

    local worldBoss_honorRank = Util:Name( CPPGameLib.GetString("worldBoss_honorRank") , Macros.TypefaceSize.largish )
    worldBoss_honorRank:SetAnchorPoint(0.5,0.5)
    worldBoss_honorRank:SetPosition(self.Button_everyday_0:GetContentWidth()/2 , 0)
    self.Button_everyday_0:AddChild(worldBoss_honorRank)
    
    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "PVP，大图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_PVP")
        end )
    end )
    Button_rule:SetZOrder(100)
    local common_rule = Util:Name( CPPGameLib.GetString("common_rule") , Macros.TypefaceSize.largish )
    common_rule:SetAnchorPoint(0.5,0.5)
    common_rule:SetPosition(Button_rule:GetContentWidth()/2 , 0)
    Button_rule:AddChild(common_rule)
end

function Modules.PVPView:__ShowUI( ... )
    --信息
    local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

    local bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
    bg_shadow:SetPosition(self.Panel_hero:GetContentWidth()/2 , 0 )
    self.Panel_hero:AddChild(bg_shadow)

    --英雄模型
    local heroModes = LogicGUI.Model.New({ info_id = role_info.head_icon , parent = bg_shadow , model_type = LogicGUI.ModelType.HERO })      
    heroModes:SetPosition(bg_shadow:GetContentWidth()/2 , bg_shadow:GetContentHeight()/2 )
    heroModes:SetScaleFactor(1.2)

    --英雄名字
    self.name = Util:Name( role_info.role_name , Macros.TypefaceSize.normal )
    self.name:SetAnchorPoint( 0.5 , 0.5 )
    self.name:SetPosition( self.Panel_hero:GetContentWidth()/2 , self.Panel_hero:GetContentHeight() + 30 )
    self.Panel_hero:AddChild( self.name )

    -- --英雄称号
    -- local title = Util:Name( "称号" )
    -- title:SetAnchorPoint( 1 , 0.5 )
    -- title:SetPosition( name:GetPositionX() - (name:GetContentWidth()/2) - 3  , name:GetPositionY()  )
    -- self.Panel_hero:AddChild( title )

    --战斗力
    local Fighting = Util:Layout( 50 , 20 )
    Fighting:SetAnchorPoint( 0.5, 0.5 )
    Fighting:SetPosition( self.Image_strength:GetContentWidth()/2 , self.Image_strength:GetContentHeight()/2 - 5 )
    self.Image_strength:AddChild(Fighting)

    --战斗力图样
    local scale = 0.6

    local battle_powerSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "battle_value_big", true)
    local battle_power = Util:Sprite(battle_powerSp)
    battle_power:SetScaleFactor( scale )
    battle_power:SetContentSize( battle_power:GetContentWidth() * scale , battle_power:GetContentHeight() * scale )
    battle_power:SetAnchorPoint( 0 , 0.5)

    local power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "battle_num", true))
    power_font:SetText(tostring(role_info.battle_value))
    power_font:SetScaleFactor( scale )
    power_font:SetContentSize( power_font:GetNode():GetContentWidth() * scale , power_font:GetNode():GetContentHeight() * scale )
    power_font:SetAnchorPoint(0, 0.5)

    Fighting:SetContentSize( battle_power:GetContentWidth() + 5 + power_font:GetNode():GetContentWidth() , power_font:GetNode():GetContentHeight() )
    
    battle_power:SetPosition(0 , Fighting:GetContentHeight()/2)
    Fighting:AddChild(battle_power)

    Fighting:AddChild( power_font:GetNode() )
    PosTool.RightTo( battle_power , power_font , 5 )

    --精灵
    local data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
    self.spirit_model = LogicGUI.Model.New({info_id = data.cur_use_spirit , parent = self.Panel_hero , model_type = LogicGUI.ModelType.SPRITE})      
    self.spirit_model:SetPosition( -50 , self.Panel_hero:GetContentHeight()/2 )

    -- self.spirit_model:SetScaleFactor(0.7)
    -- self.spirit_model:SetTouchRect( 0 - ( (self.spirit_model:GetWidth()/2) * 0.7)  , 0 , 100, 200)      
    -- self.spirit_model:SetTouchFun(nil, nil, function()
    --     LogicGUI.TemplSpriteSet.New()
    -- end)
        
    --精灵设置背景
    local set_bg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "set_bg" ) )
    set_bg:SetAnchorPoint( 0.5 , 0.5 )
    set_bg:SetPosition( self.spirit_model:GetNode():GetPositionX() - 30 , self.spirit_model:GetNode():GetPositionY() + 90 )
    self.Panel_hero:AddChild( set_bg )

    --精灵设置
    local setSprite = Util:Label( CPPGameLib.GetString("pvp_setSprite") , nil , nil , nil , nil , Macros.Color.button_yellow )
    setSprite:SetAnchorPoint( 0.5 , 0.5 )
    setSprite:SetPosition( set_bg:GetContentWidth()/2 , set_bg:GetContentHeight()/2 + 5 )
    set_bg:AddChild( setSprite )

    --精灵设置
    local sprite = Util:Layout( setSprite:GetContentWidth() + 20 , setSprite:GetContentHeight() + 20 , function ( ... )
        -- CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT)
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_SET , false )
    end )
    sprite:SetAnchorPoint( 0.5 , 0.5 )
    setSprite:AddChild( sprite )
    PosTool.Center( sprite )

      --精灵设置
    local sprite = Util:Layout( 100 , 100 , function ( ... )
        -- CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT)
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_SET , false )
    end )
    sprite:SetAnchorPoint( 0.5 , 0.5 )
    self.spirit_model:GetNode():AddChild( sprite )
    PosTool.Center( sprite , 0 , 20 )

    -- --本周胜场
    -- local pvp_weekSuccess = Util:Label( CPPGameLib.GetString("pvp_weekSuccess") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
    -- pvp_weekSuccess:SetAnchorPoint( 0 , 0.5 )
    -- pvp_weekSuccess:SetPosition( 20 , self.Panel_target:GetContentHeight()/2 + 15 )
    -- self.Panel_target:AddChild( pvp_weekSuccess )

    -- self.pvp_weekSuccess = Util:Label( "0/0" , Macros.TypefaceSize.normal , Macros.Color.Special )
    -- self.Panel_target:AddChild( self.pvp_weekSuccess )
    -- PosTool.RightTo( pvp_weekSuccess , self.pvp_weekSuccess )

    local tab = {
                    { zi = CPPGameLib.GetString("pvp_dayParticipation") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
                    { zi = "0/0", ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    self.pvp_dayParticipation , self.pvp_dayParticipationLst = Util:LabCenter(tab)
    self.pvp_dayParticipation:SetAnchorPoint( 0.5 , 0.5 )
    self.pvp_dayParticipation:SetPosition( self.Image_division:GetContentWidth()/2 , self.Image_division:GetContentHeight() + 20 )
    self.Image_division:AddChild( self.pvp_dayParticipation )

    local tab = {
                    { zi = CPPGameLib.GetString("pvp_weekSuccess") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
                    { zi = "0/0", ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    self.pvp_weekSuccess , self.pvp_weekSuccessLst = Util:LabCenter(tab)
    self.pvp_weekSuccess:SetAnchorPoint( 0.5 , 0.5 )
    self.pvp_weekSuccess:SetPosition( self.Image_division:GetContentWidth()/2 , self.Image_division:GetContentHeight()/2 )
    self.Image_division:AddChild( self.pvp_weekSuccess )

    local tab = {
                    { zi = CPPGameLib.GetString("pvp_awardTime") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
                    { zi = "0", ys = Macros.Color.green , dx = Macros.TypefaceSize.normal },
                }
    --今日可获积分奖励剩余场次
    self.pvp_awardTime , self.pvp_awardTimeLst = Util:LabCenter(tab)
    self.pvp_awardTime:SetAnchorPoint( 0.5 , 0.5 )
    self.pvp_awardTime:SetPosition( self.Text_time:GetContentWidth()/2 , self.Text_time:GetContentHeight()/2 )
    self.Text_time:AddChild( self.pvp_awardTime )

    self.rankLst = {}
    self.integralLst = {}

    for i = 1 , 3 do
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..i)
        local com_sp_rank_num = Util:Sprite( sp )
        com_sp_rank_num:SetAnchorPoint( 0.5 , 0.5 )
        com_sp_rank_num:SetPosition( 30  , self.Image_integral[i]:GetContentHeight()/2 )
        self.Image_integral[i]:AddChild( com_sp_rank_num )

        self.rankLst[i] = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.white )
        self.rankLst[i]:SetAnchorPoint( 0 , 0.5 )
        self.rankLst[i]:SetPosition( 70 , com_sp_rank_num:GetPositionY() + 15 )
        self.Image_integral[i]:AddChild( self.rankLst[i] )

        self.integralLst[i] = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.Special )
        self.integralLst[i]:SetAnchorPoint( 0 , 0.5 )
        self.integralLst[i]:SetPosition( 70 , com_sp_rank_num:GetPositionY() - 15 )
        self.Image_integral[i]:AddChild( self.integralLst[i] )
    end

    self.pvp_activityTime = Util:Label( CPPGameLib.GetString("pvp_activityTime" , 0 ) , Macros.TypefaceSize.normal , Macros.Color.golden )
    self.pvp_activityTime:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_center:AddChild( self.pvp_activityTime )
    self.pvp_activityTime:SetPosition( self.Button_challenge:GetPositionX() , self.Button_challenge:GetPositionY() )
    self.pvp_activityTime:SetVisible(false)

end

function Modules.PVPView:__SetShowUI( ... )
    --时间
    local lab , open = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonOpenDesc" , Macros.Game.SystemID.PVP )
    if open == false  then
        self.Button_challenge:SetVisible( false )
        self.pvp_awardTime:SetVisible(false)
        self.pvp_activityTime:SetVisible(true)
        self.pvp_activityTime:SetString( CPPGameLib.GetString("pvp_activityTime") ..  CPPGameLib.GetString("common_colon") .. lab )
    else
        self.Button_challenge:SetVisible( true )
        self.pvp_awardTime:SetVisible(true)
        self.pvp_activityTime:SetVisible(false)
        WidgetUnity.SetWidgetListener( self.Button_challenge , nil, nil, function ( ... )
            self:OnButtonChallenge()
        end, nil, 0.5*1000)
    end
    
    --今天
    local num , max = CallModuleFunc(ModuleType.PVP, "GetDayBattleTimes")
    self.pvp_dayParticipationLst[2]:SetString( num .. "/" .. max )
    self.pvp_dayParticipation:SetPosition( self.Image_division:GetContentWidth()/2 , self.Image_division:GetContentHeight() + 20 )

    --这周
    local num , max = CallModuleFunc(ModuleType.PVP, "GetWeekBattleTimes")
    self.pvp_weekSuccessLst[2]:SetString( num .. "/" .. max )
    self.pvp_weekSuccess:SetPosition( self.Image_division:GetContentWidth()/2 , self.Image_division:GetContentHeight()/2 )

    --次数
    self.pvp_awardTimeLst[2]:SetString( CallModuleFunc(ModuleType.PVP, "GetPVPAwardTime")  )

    local lst , my_rank , my_score = CallModuleFunc(ModuleType.PVP,"GetRankList" )
    for i , v in ipairs( lst ) do
        self.rankLst[i]:SetString( v.role_name )  
        self.integralLst[i]:SetString( CPPGameLib.GetString("pvp_integral_") .. v.score  )
    end

    local text = my_rank
    if my_rank == 0 then
        text = CPPGameLib.GetString( "rebel_meimingci" )
    end
    self.Text_MyPank:SetText( CPPGameLib.GetString("monster_stormcastle_myranktxt") .. text )
    self.Text_MyIntegral:SetText( CPPGameLib.GetString("monster_stormcastle_myintergraltxt") .. my_score )

    --称号
    if self.title then
        if self.title.left then
            self.title.left:RemoveSelf(true)
            self.title.left = nil
        end

        if self.title.right then
            self.title.right:RemoveSelf(true)
            self.title.right = nil
        end

        if self.title.top then
            self.title.top:RemoveSelf(true)
            self.title.top = nil
        end
    end

    self.title = {}

    local titleId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )
    if titleId and titleId ~= 0 then

        local rankSp = Resource.PathTool.GetTitleIconPath( "sp_title_".. titleId , true )
        self.title.top = Util:Sprite( rankSp )
        self.title.top:SetAnchorPoint( 0.5 , 0 )
        self.title.top:SetPosition( self.name:GetPositionX() , self.name:GetPositionY() + (self.name:GetContentHeight()/2) )
        self.Panel_hero:AddChild( self.title.top )
    end

    if titleId_ and titleId_ ~= 0 then

        local rankSp = Resource.PathTool.GetTitleIconPath( "sp_prefix_".. titleId_ , true )
        self.title.left = Util:Sprite( rankSp )
        self.title.left:SetAnchorPoint( 1 , 0.5 )
        self.title.left:SetPosition( self.name:GetPositionX() - (self.name:GetContentWidth()/2) - 3  , self.name:GetPositionY()  )
        self.Panel_hero:AddChild( self.title.left )

        local cnf = CPPGameLib.GetConfig("Title", titleId_ )
        if cnf.postfix == 1 then
            local rankSp = Resource.PathTool.GetTitleIconPath( "sp_suffix_" .. titleId_ )
            self.title.right = Util:Sprite( rankSp )
            self.title.right:SetAnchorPoint( 0 , 0.5 )
            self.title.right:SetPosition( self.name:GetPositionX() + (self.name:GetContentWidth()/2) + 5  , self.name:GetPositionY()  )
            self.Panel_hero:AddChild( self.title.right )
        end
    end

    --精灵
    local data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
    self.spirit_model:SetInfoID(data.cur_use_spirit)

    self:SetRed()
end

--活动倒计时
function Modules.PVPView:Countdown( show_txt )
    --时间
    local lab , open = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonOpenDesc" , Macros.Game.SystemID.PVP )
    if show_txt == CPPGameLib.GetString("maincity_on_fighting") then
        CallModuleFunc(ModuleType.PVP, "PlayerPVPDataReq")
    elseif show_txt ~= "" then
        self.pvp_activityTime:SetString( CPPGameLib.GetString("common_timeStart" , show_txt ) )
    end
end

--设置红点
function Modules.PVPView:SetRed()

    local t1 = CallModuleFunc( ModuleType.PVP , "GetBattleTimesReward" , Macros.Game.PVP_REWARD_TYPE.DAY )
    local num = CallModuleFunc( ModuleType.PVP , "GetDayBattleTimes" )
    local type_ = false
    for i , v in ipairs( t1 ) do
        --图标
        local record = CallModuleFunc( ModuleType.PVP , "GetRecord" , v.type , v.times )
        --可领取
        if num >= v.times and not record then
            type_ = true
            break
        end
    end
    self.everyday_red:SetVisible( type_ )

    local t1 = CallModuleFunc( ModuleType.PVP , "GetBattleTimesReward" , Macros.Game.PVP_REWARD_TYPE.WEEK )
    local num = CallModuleFunc( ModuleType.PVP , "GetWeekBattleTimes" )
    local type_ = false
    for i , v in ipairs( t1 ) do
        --图标
        local record = CallModuleFunc( ModuleType.PVP , "GetRecord" , v.type , v.times )
        --可领取
        if num >= v.times and not record then
            type_ = true
            break
        end
    end
    self.weekly_red:SetVisible( type_ )
end

--挑战
function Modules.PVPView:OnButtonChallenge()
    local id = CallModuleFunc(ModuleType.SPIRIT,"GetCommonKillId" )
    CallModuleFunc(ModuleType.BATTLE, "SearchTargetReq_PVP",id)
    LogicGUI.TemplPvpChallenge.New()
end

--排位
function Modules.PVPView:OnButtonRank()
    print("xy" , "排位")
    LogicGUI.TemplPvpRank.New()
end

--每日奖励
function Modules.PVPView:OnButtonEveryday()
    print("xy" , "每日奖励")
    LogicGUI.TemplPvpAward.New( Macros.Game.PVP_REWARD_TYPE.DAY )
end

--每周奖励
function Modules.PVPView:OnButtonWeekly()
    print("xy" , "每周奖励")
    LogicGUI.TemplPvpAward.New( Macros.Game.PVP_REWARD_TYPE.WEEK )
end