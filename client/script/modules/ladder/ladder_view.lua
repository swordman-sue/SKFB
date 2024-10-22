--[[
%% @module: 天梯赛
%% @author: yjg
%% @created: 2017年10月28日18:32:51
--]]

Modules = Modules or {} 
Modules.LadderView = Modules.LadderView or BaseClass(BaseView)

function Modules.LadderView:__init()
    self.__layout_name = "ladder"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_ladder")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("ladder"), 
      -- show_diamond = true,
      -- show_gold = true,
      -- show_stamina = true,
      -- show_energy = true,
      close_func = function()
        self:CloseManual()
      end,
    }

    self.__open_callback = function()
      self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER , true )  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER_COMMON)  
end

function Modules.LadderView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
    self:__FirstWin()
end	

function Modules.LadderView:__Dispose()
    if self.Effect_Id then
        self:__DelEffect(self.Effect_Id)
        self.Effect_Id = nil
    end 
    if self.effect_id then
        self:__DelEffect(self.effect_id)
        self.effect_id = nil
    end  
    if self.Effect_Id_1 then
        self:__DelEffect(self.Effect_Id_1)
        self.Effect_Id_1 = nil
    end  
end

function Modules.LadderView:__Server()
    CallModuleFunc( ModuleType.LADDER , "KingMatchDataReq" )
    self:BindGlobalEvent(Macros.Event.LadderModule.LADDER , function()
        self:SetShowUI()
    end)

    --红点刷新
    self:BindGlobalEvent(Macros.Event.LadderModule.LADDER_TASK , function()
        self:SetRed( )
    end)

end

function Modules.LadderView:__Getwidget( )

    --居中层
    self.Panel_center = self:GetWidget("Panel_center")

    --UI层
    self.Panel_UI_Tier = self:GetWidget("Panel_UI_Tier")
    
        --任务
        self.Button_task = Util:WidgetButton( 
                                                self:GetWidget("Button_task") , 
                                                "" , 
                                                CPPGameLib.Handler(self,self.OnButtonTask) ,
                                                nil , nil , Macros.Color.button_yellow 
                                            )

        local ladder_task = Util:RichText( string.format( Macros.Color.city_headline_rt, CPPGameLib.GetString("ladder_task") ) , Macros.TypefaceSize.largish )
        ladder_task:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_task:AddChild( ladder_task )
        PosTool.Center( ladder_task , 0 , -30 )

        --排行榜1
        self.Button_rank_1 = Util:WidgetButton( 
                                                self:GetWidget("Button_rank_1") , 
                                                "" , 
                                                CPPGameLib.Handler(self,self.OnButtonRank1) ,
                                                nil , nil , Macros.Color.button_yellow 
                                            )
        local ladder_rank1 = Util:RichText( string.format( Macros.Color.city_headline_rt, CPPGameLib.GetString("ladder_rank1") ) , Macros.TypefaceSize.largish )
        ladder_rank1:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_rank_1:AddChild( ladder_rank1 )
        PosTool.Center( ladder_rank1 , 0 , -30 )
        --开始
        self.Button_start = Util:WidgetButton( 
                                                self:GetWidget("Button_start") , 
                                                "" , 
                                                CPPGameLib.Handler(self,self.OnButtonStart) ,
                                                nil , nil , Macros.Color.button_yellow 
                                            )

        --图标背景
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_btn_king" , true )
        local icon_bg = Util:Sprite( sp )
        icon_bg:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_start:AddChild( icon_bg , 1 )
        PosTool.Center( icon_bg , 0 , 23)

        --图标背景
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_btn_test" )
        local icon_bg = Util:Sprite( sp )
        icon_bg:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_start:AddChild( icon_bg , 1 )
        PosTool.Center( icon_bg , 0 , -8 )

        local ladder_start_1 = Util:Label( CPPGameLib.GetString("ladder_start_1") , Macros.TypefaceSize.minimum )
        ladder_start_1:SetAnchorPoint( 0.5 , 0.5 )
        icon_bg:AddChild( ladder_start_1 )
        PosTool.Center( ladder_start_1)

        --荣耀
        self.Button_glory = Util:WidgetButton( 
                                                self:GetWidget("Button_glory") , 
                                                "" , 
                                                CPPGameLib.Handler(self,self.OnButtonGlory) ,
                                                nil , nil , Macros.Color.button_yellow 
                                            )

        local ladder_glory = Util:RichText( string.format( Macros.Color.city_headline_rt, CPPGameLib.GetString("ladder_glory") ) , Macros.TypefaceSize.largish )
        ladder_glory:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_glory:AddChild( ladder_glory )
        PosTool.Center( ladder_glory , 0 , -30 )

        --排行榜2
        self.Button_rank_2 = Util:WidgetButton( 
                                                self:GetWidget("Button_rank_2") , 
                                                "" , 
                                                CPPGameLib.Handler(self,self.OnButtonRank2) ,
                                                nil , nil , Macros.Color.button_yellow 
                                            )
        local ladder_rank2 = Util:RichText( string.format( Macros.Color.city_headline_rt, CPPGameLib.GetString("ladder_rank2") ) , Macros.TypefaceSize.largish )
        ladder_rank2:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_rank_2:AddChild( ladder_rank2 )
        PosTool.Center( ladder_rank2 , 0 , -30 )

    --图标层
    self.Panel_icon_Tier = self:GetWidget("Panel_icon_Tier")

    --模型层
    self.Panel_Mode_Tier = self:GetWidget("Panel_Mode_Tier")

    --成绩层
    self.Panel_grade_Tier = self:GetWidget("Panel_grade_Tier")

        --评分
        self.Panel_grade = self:GetWidget("Panel_grade")
        --奖励1
        self.Panel_award_1 = self:GetWidget("Panel_award_1")
        --奖励2
        self.Panel_award_2 = self:GetWidget("Panel_award_2")
end 


function Modules.LadderView:__ShowUI( )
    --赛季
    self.ladder_Season = Util:Label( CPPGameLib.GetString("ladder_Season" , 0 , 0 ) , Macros.TypefaceSize.popup , Macros.Color.Special )
    self.ladder_Season:SetPosition( self.Panel_center:GetContentWidth()/2 , self.Panel_center:GetContentHeight()/2 + 270 )
    self.Panel_center:AddChild( self.ladder_Season )

    --赛季结束
    self.ladder_SeasonEnd = Util:Label( " " , Macros.TypefaceSize.normal , Macros.Color.red )
    self.ladder_SeasonEnd:SetPosition( self.ladder_Season:GetPositionX() , self.ladder_Season:GetPositionY() - self.ladder_SeasonEnd:GetContentHeight() )
    self.Panel_center:AddChild( self.ladder_SeasonEnd )

    --赛季倒计时
    self.ladder_notOpen = Util:Label( "" )
    self.ladder_notOpen:SetAnchorPoint( 0.5 , 0.5 )
    self.ladder_notOpen:SetPosition( self.ladder_Season:GetPositionX() , self.ladder_Season:GetPositionY() - 50 )
    self.Panel_center:AddChild( self.ladder_notOpen )

    local Effect_con = nil
    self.Effect_Id , Effect_con = self:__CreateEffect( { res_name = "UI_wangzhedaiji" } , self.Panel_Mode_Tier )
    Effect_con:SetPosition( self.Panel_Mode_Tier:GetContentWidth()/2 , self.Panel_Mode_Tier:GetContentHeight()/2 - 100 )

    local Effect_con = nil
    self.Effect_Id_1 , Effect_con = self:__CreateEffect( { res_name = "UI_wangzhezhujiemian" } , self.Panel_icon_Tier )
    Effect_con:SetPosition( self.Panel_icon_Tier:GetContentWidth()/2 , self.Panel_icon_Tier:GetContentHeight()/2 )

    --图标背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_ladder_icon_bg")
    self.icon_bg = Util:Sprite( sp )
    self.icon_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_icon_Tier:AddChild( self.icon_bg , 1)
    PosTool.Center( self.icon_bg )

    --图标背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_ladder_ribbon")
    self.ribbon = Util:Sprite( sp )
    self.ribbon:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_icon_Tier:AddChild( self.ribbon , 2)
    PosTool.Center( self.ribbon , 0 , -78 )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_more_award" , true)
    self.btn_confirm = Util:Button( sp , function()
        self:OnButtonPromotion(  )
    end)
    self.btn_confirm:SetAnchorPoint( 1 , 0.5 )
    self.btn_confirm:SetPosition( self.Panel_grade_Tier:GetContentWidth() - 20 , self.btn_confirm:GetContentHeight() )
    self.Panel_grade_Tier:AddChild( self.btn_confirm )

    local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
    if time < ConfigAdapter.Common.GetLadderEnterTime() then
        local access = Util:Label( CPPGameLib.GetString("ladder_access_2") )
        access:SetAnchorPoint( 0.5 , 0.5 )
        access:SetPosition( self.Button_start:GetPositionX() , self.Button_start:GetPositionY() + 60 )
        self.Panel_UI_Tier:AddChild( access )

        WidgetUnity.SetWidgetListener(self.Button_start, nil, nil, function( ... )
            -- local text = TimeTool.TransTimeStamp( CPPGameLib.GetString("time_fate") , ConfigAdapter.Common.GetLadderEnterTime() - time)
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("ladder_access_1"))
        end, nil, 0.5*1000)
    end

    --玩家信息
    self:SetPlayerInfo()
end


function Modules.LadderView:SetShowUI( )
    --当前赛季
    local season , year = CallModuleFunc( ModuleType.LADDER , "GetNowSeason" )

    CPPGameLib.PrintTable("xy" , season , "GetNowSeason")  

    CPPActionManager.StopAllActions(self.ladder_SeasonEnd)
    --赛季倒计时
    local info = CallModuleFunc( ModuleType.LADDER , "GetSeasonInfo" )
    CPPGameLib.PrintTable("xy" , info , "GetSeasonInfo")  

    if not info.countDown or info.countDown <= 0 then
        local func = nil
        local function moveAnimate( ... )
            local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
            local endTime = info.closeTime - serverTime
            if endTime <= 0 then
                self.ladder_SeasonEnd:SetString( CPPGameLib.GetString("ladder_SeasonEnd") )
                -- CallModuleFunc( ModuleType.LADDER , "KingMatchDataReq" )                
            else
                local text = ""
                if endTime >= ( 24 * 60 * 60  ) then
                    local sky =  endTime/( 24 * 60 * 60 ) 
                    endTime = endTime - ( math.floor(sky) * 24 * 60 * 60  )
                    local hour = endTime / 60 / 60
                    text = CPPGameLib.GetString("time_reciprocal_day", math.floor(sky) , math.floor( hour) ) 
                else

                    local hour = endTime/( 60 * 60 ) 
                    endTime = endTime - ( math.floor( hour ) * 60 * 60 )
                    local minute = endTime / 60 

                    if math.floor( hour ) == 0 then
                        text = math.ceil( minute ) .. CPPGameLib.GetString("common_minute_")
                    else
                        text = CPPGameLib.GetString("time_reciprocal_hour", math.floor( hour ) , math.ceil( minute ) ) 
                    end

                end
                self.ladder_SeasonEnd:SetString( CPPGameLib.GetString("ladder_SeasonProceed") .. text )
                CPPActionManager.DelayTo( self.ladder_SeasonEnd , 1 , func)
            end
        end
        func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
        moveAnimate()
    else
        if info.season ~= 1 then
            self.ladder_SeasonEnd:SetString( CPPGameLib.GetString("ladder_SeasonEnd") )
            -- season = season - 1
        else
            self.ladder_SeasonEnd:SetString( " " )
        end
        self:SeasonGrade_2()

    end
    self.ladder_Season:SetString( CPPGameLib.GetString("ladder_Season" , year , season ) )

    --段位图标
    self:SetIconInfo()

    --成绩
    self:SetGradeInfo()
end


--玩家信息
function Modules.LadderView:SetPlayerInfo( )
    --称号
    local titleId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )

    local title = nil
    if titleId and titleId ~= 0 then
        local rankSp = Resource.PathTool.GetTitleIconPath( "sp_title_".. titleId , true )
        title = Util:Sprite( rankSp )
        title:SetAnchorPoint( 0.5 , 0.5 )
        title:SetPosition( self.Panel_Mode_Tier:GetContentWidth()/2  , self.Panel_Mode_Tier:GetContentHeight() - (title:GetContentHeight()/2) - 20 )
        self.Panel_Mode_Tier:AddChild( title , 5)
    end

    --名字
    local my_name = CallModuleFunc(ModuleType.ROLE, "GetName")
    local name = Util:Name( my_name , Macros.TypefaceSize.normal )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( self.Panel_Mode_Tier:GetContentWidth()/2  , self.Panel_Mode_Tier:GetContentHeight() - 120 )
    self.Panel_Mode_Tier:AddChild( name , 5 )

    --模型
    local my_head_id = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
    local config_hero = CPPGameLib.GetConfig("Hero", my_head_id )
    
    local __hero_module = LogicGUI.Model.New({info_id = my_head_id , parent = self.Panel_Mode_Tier })
    __hero_module:SetPosition( self.Panel_Mode_Tier:GetContentWidth()/2 , 70  )
    __hero_module:SetScaleFactor( config_hero.show_scale )


    local maincity_battle_power = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_power" , true) )
    
    local my_combat_value = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.BATTLE_VALUE)
    local __power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
    __power_font:SetText(tostring( my_combat_value ))

    local layer = Util:Layout( maincity_battle_power:GetContentWidth() + __power_font:GetNode():GetContentWidth() + 5 , 10 )
    layer:SetPosition( self.Panel_Mode_Tier:GetContentWidth()/2 , 25 )
    layer:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_Mode_Tier:AddChild( layer )

    maincity_battle_power:SetAnchorPoint( 0 , 0.5)
    maincity_battle_power:SetPosition( 0 , layer:GetContentHeight()/2 )
    layer:AddChild( maincity_battle_power )

    layer:AddChild( __power_font:GetNode() )
    PosTool.RightTo( maincity_battle_power , __power_font )
        
    self:SetRed()
end


--设置红点
function Modules.LadderView:SetRed( )
    --任务
    local index = CallModuleFunc(ModuleType.LADDER, "GetTaskRed")


    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    if index ~= 0 then
        if not self.icon_red_point then
            self.icon_red_point = Util:Sprite(icon_red_point)
            self.icon_red_point:SetPosition(self.Button_task:GetContentWidth() - 10 , self.Button_task:GetContentHeight() - 10 )
            self.Button_task:AddChild(self.icon_red_point , 20) 
        else
            self.icon_red_point:SetVisible(true)
        end
    else
        if self.icon_red_point then
            self.icon_red_point:SetVisible(false)
        end
    end
end

--段位图标
function Modules.LadderView:SetIconInfo( )
    local rank , star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    if rank == 0 then
        rank = 1
    end

    local config_level = CPPGameLib.GetConfig("LadderDan", rank , false )

    --图标
    local Icon = Util:Sprite( Resource.PathTool.GetLadderIconPath( config_level.icon ) , function ( ... )
        self:OnButtonPromotion(  )
    end )
    Icon:SetAnchorPoint( 0.5 , 0.5 )
    self.icon_bg:AddChild( Icon )
    PosTool.Center( Icon )

    if config_level.type - 1 ~= 0 then
        local effect = nil
        self.effect_id , effect = self:__CreateEffect({res_name = "UI_duanweisaoguang" .. config_level.type - 1 }, self.icon_bg )
        effect:SetPosition( self.icon_bg:GetContentWidth()/2 , self.icon_bg:GetContentHeight()/2 )
    end

    local rank , star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    if rank == 0 then
        rank = 1
    end

    local radius = 100
    local beg_degree = 180 / (config_level.promotion_aequum + 1)
    local step_degree = 180 / (config_level.promotion_aequum + 1)
    local x, y, degree
    for i = 1, config_level.promotion_aequum do
        local sp_star = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_star"))
        sp_star:SetGray(1 + config_level.promotion_aequum - i > star)
        degree = beg_degree + (i - 1) * step_degree

        x = (self.icon_bg:GetContentWidth()/2) + math.cos(math.rad(degree)) * radius
        y = (self.icon_bg:GetContentHeight()/2) + math.sin(math.rad(degree)) * radius

        sp_star:SetPosition(x, y)

        self.icon_bg:AddChild(sp_star)       
    end

    local dan = Util:Dan( rank , star )
    dan:SetAnchorPoint( 0.5 , 0.5 )
    self.ribbon:AddChild( dan )
    PosTool.Center( dan , 0 , -10 )

    --我的排名
    local rank , value = CallModuleFunc(ModuleType.LADDER, "GetMyGrid")
    if rank ~= 0 then
        local pvp_integral_ = Util:Label( CPPGameLib.GetString("pvp_integral_") .. value , Macros.TypefaceSize.largish , Macros.Color.Special )
        pvp_integral_:SetPosition( self.ribbon:GetContentWidth()/2 , -45 )
        self.ribbon:AddChild( pvp_integral_ )
    end 

end

--玩家成绩&奖励
function Modules.LadderView:SetGradeInfo( )
    -- --赛季成绩
    self.ladder_season_grade = Util:Label( CPPGameLib.GetString("ladder_season_grade") , Macros.TypefaceSize.slightly , Macros.Color.ladder )
    self.ladder_season_grade:SetAnchorPoint( 0.5 , 1 )
    self.ladder_season_grade:SetPosition( self.Panel_grade:GetContentWidth()/2 , self.Panel_grade:GetContentHeight() - 4 )
    self.Panel_grade:AddChild( self.ladder_season_grade )
    self:SeasonGrade()


    local lst = CallModuleFunc(ModuleType.LADDER, "GetSeasonAward")
    local text = ""
    if lst.season then
        text = CPPGameLib.GetString("ladder_rank_award")
    elseif lst.noSeason then
        text = CPPGameLib.GetString("ladder_norank_award")
    end
    -- --赛季奖励
    self.ladder_rank_award = Util:Label( text , Macros.TypefaceSize.slightly , Macros.Color.ladder )
    self.ladder_rank_award:SetAnchorPoint( 0.5 , 1 )
    self.ladder_rank_award:SetPosition( self.Panel_award_1:GetContentWidth()/2 , self.Panel_award_1:GetContentHeight() - 4 )
    self.Panel_award_1:AddChild( self.ladder_rank_award )
    self:RankAward()


    local lst = CallModuleFunc(ModuleType.LADDER, "GetIsHaveWinning")
    local text = ""
    if lst.winning then
        text = CPPGameLib.GetString("ladder_first_award")
    elseif lst.promotion then
        text = CPPGameLib.GetString("ladder_promotionAward")
    else
        self.Panel_award_2:SetVisible( false )
    end
    -- --首胜奖励 or 晋级奖励
    self.ladder_first_award = Util:Label( text , Macros.TypefaceSize.slightly , Macros.Color.ladder )
    self.ladder_first_award:SetAnchorPoint( 0.5 , 1 )
    self.ladder_first_award:SetPosition( self.Panel_award_2:GetContentWidth()/2 , self.Panel_award_2:GetContentHeight() - 4 )
    self.Panel_award_2:AddChild( self.ladder_first_award )
    self:FirstAward()
end

-- --赛季成绩
function Modules.LadderView:SeasonGrade( )
    local max , odds = CallModuleFunc(ModuleType.LADDER, "GetMyWinning")

    -- --出战
    local tab = {
                 { zi = CPPGameLib.GetString("ladder_SG_played") , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.white },
                 { zi = max , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.white },
                }
    local ladder_SG_played = Util:LabCenter(tab)
    ladder_SG_played:SetAnchorPoint( 0 , 0.5 )
    ladder_SG_played:SetPosition( 10 , self.Panel_grade:GetContentHeight()/2 + 10 )
    self.Panel_grade:AddChild( ladder_SG_played )

    -- --胜率
    local tab = {
                 { zi = CPPGameLib.GetString("ladder_SG_victory") , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.white },
                 { zi = odds .. "%" , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.white },
                }
    local ladder_SG_hide = Util:LabCenter(tab)
    ladder_SG_hide:SetAnchorPoint( 1 , 0.5 )
    ladder_SG_hide:SetPosition( self.Panel_grade:GetContentWidth() - 10 , ladder_SG_played:GetPositionY() )
    self.Panel_grade:AddChild( ladder_SG_hide )

    local rank , value = CallModuleFunc(ModuleType.LADDER, "GetMyGrid")
    local text = CPPGameLib.GetString("ladder_Rank_Tips" , rank , value )

    if rank == 0 then
        text = CPPGameLib.GetString("ladder_noRank_Tips")
    end 

    --排行
    local tab = {
                  { zi = CPPGameLib.GetString("ladder_SG_rank") , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.white },
                  { zi = text , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.white },
                }
    local ladder_SG_rank = Util:LabCenter(tab)
    ladder_SG_rank:SetAnchorPoint( 0 , 0.5 )
    ladder_SG_rank:SetPosition( 10 , self.Panel_grade:GetContentHeight()/2 - 25 )
    self.Panel_grade:AddChild( ladder_SG_rank )
end

--未开启状态
function Modules.LadderView:SeasonGrade_2( )
    local lst = CallModuleFunc(ModuleType.LADDER, "GetSeasonInfo")

    self.ladder_notOpen:SetString( CPPGameLib.GetString("ladder_notOpen", lst.season , lst.openTime_) )

    local func = nil
    local function moveAnimate( ... )
        local runTime = lst.openTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if runTime <= 0 then
            CallModuleFunc( ModuleType.LADDER , "KingMatchDataReq" )
            self.ladder_notOpen:SetString( "" )
        else
            CPPActionManager.DelayTo(self.ladder_notOpen , 1 , func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

-- --排行奖励
function Modules.LadderView:RankAward( )

    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80,
        item_height = 80,
        row = 1,
        col = 4 ,
        horizon = true ,
        item_space_c = 0 ,
        view_width = self.Panel_award_1:GetContentWidth() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_award_1:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode() , 0 , -12 )

    local lst = CallModuleFunc(ModuleType.LADDER, "GetSeasonAward")
    self.lst_list:SetDataList( lst.lst )
end

-- --首胜奖励
function Modules.LadderView:FirstAward( )
    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80,
        item_height = 80,
        row = 1,
        col = 4,
        horizon = true ,
        view_width = self.Panel_award_2:GetContentWidth() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_award_2:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode() , 0 , -12 )

    local lst = CallModuleFunc(ModuleType.LADDER, "GetIsHaveWinning")
    self.lst_list:SetDataList( lst.lst )

    if lst.max then
        local sp_conclude_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_conclude", true)
        local sp_conclude = Util:Sprite( sp_conclude_ )
        sp_conclude:SetAnchorPoint( 0.5 , 0.5 )
        sp_conclude:SetPosition( self.Panel_award_2:GetContentWidth() - 10 , self.Panel_award_2:GetContentHeight() - 20 )
        self.Panel_award_2:AddChild( sp_conclude )
    end
end

--任务
function Modules.LadderView:OnButtonTask( )
    print("xy" , "任务")
    LogicGUI.TempLadderTask.New()
end

--规则
function Modules.LadderView:OnButtonRank1( )
    print("xy" , "规则")
    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
    help:SetLoadedCallback( function ( ... )
       help:SetCell("Rule_Ladder")
    end )
end

--开始
function Modules.LadderView:OnButtonStart( )
    print("xy" , "开始")

    self:SkipMatching()
    -- CallModuleFunc(ModuleType.LADDER , "OpenView" , BaseViewType.LADDER_VS )
end

--荣耀
function Modules.LadderView:OnButtonGlory( )
    print("xy" , "荣耀")
    CallModuleFunc(ModuleType.LADDER , "OpenView" , BaseViewType.LADDER_HONOR_VIEW )
end

--排行榜2
function Modules.LadderView:OnButtonRank2( )
    print("xy" , "排行榜2")
    CallModuleFunc(ModuleType.LADDER , "OpenView" , BaseViewType.LADDER_SEASON_AWARD_VIEW )
end

--晋级
function Modules.LadderView:OnButtonPromotion( )
    print("xy" , "晋级奖励")
    LogicGUI.TempLadderPromotion.New()
end

--荣耀
function Modules.LadderView:SkipMatching( )
    local seasonInfo = CallModuleFunc(ModuleType.LADDER , "GetSeasonInfo" )
    if seasonInfo and seasonInfo.season and (seasonInfo.proceed and seasonInfo.proceed ~= false) then
        Template.LadderVSView.New()
    else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("ladder_noOpponentTime"))
    end
end

--首胜奖励
function Modules.LadderView:__FirstWin()
    local data = CallModuleFunc( ModuleType.LADDER , "GetWinningAward" )
    CallModuleFunc( ModuleType.LADDER , "SetWinningAward" )
    if data and data.is_first_win == 1 then
        local cnf = CPPGameLib.GetConfig("LadderDan", data.level , false )
        if cnf then
            local t1 = {}
            for i,v in ipairs(cnf.first_award_id) do
                table.insert(t1,{id = v , num = cnf.first_award_num[i] })
            end
            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 , nil , nil , 100072 )
        end
    end
end
