--[[
%% @module: 天梯赛VS
%% @author: yjg
%% @created: 2017年10月30日17:30:02
--]]

Template = Template or {} 
Template.LadderVSView = Template.LadderVSView or BaseClass(GUI.Template)

function Template.LadderVSView:__init()

    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function ()
        self:__Getwidget()
        self:__ShowUI()
        self:__Server()
    end 
    self:InitTemplate(GUI.TemplateRegistry.TEMP_LADDER_VS) 

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER , true )  
end

function Template.LadderVSView:__Server( )
    
    CallModuleFunc(ModuleType.LADDER , "KingMatchSearchTargetReq" )

    self:BindGlobalEvent(Macros.Event.LadderModule.LADDER_VS , function()
        CPPActionManager.DelayTo( self.Panel_center , MathTool.GetRandom( 3 , 5 ) , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
            self:SetRivalInfo(  )
        end) )
    end)
end	

function Template.LadderVSView:__Dispose()
    if self.Effect_Id then
        self:__DelEffect(self.Effect_Id)
        self.Effect_Id = nil
    end 
    if self.Effect_Id_ then
        self:__DelEffect(self.Effect_Id_)
        self.Effect_Id_ = nil
    end 
    if self.wangzhepipei then
        self:__DelEffect(self.wangzhepipei)
        self.wangzhepipei = nil
    end 
end

function Template.LadderVSView:__Getwidget( )
    --居中层
    self.Panel_center = self:GetWidget("Panel_center")

    --图标层
    self.Panel_icon_Tier = self:GetWidget("Panel_icon_Tier")

    --我的模型
    self.Pane_my_Tier = self:GetWidget("Pane_my_Tier")

    --他的模型
    self.Panel_he_Tier = self:GetWidget("Panel_he_Tier")

end 


function Template.LadderVSView:__ShowUI( )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_matching" , true )
    self.icon_bg = Util:Sprite( sp )
    self.icon_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_icon_Tier:AddChild( self.icon_bg )
    PosTool.Center( self.icon_bg )

    local Effect_con = nil
    self.Effect_Id , Effect_con = self:__CreateEffect( { res_name = "UI_wangzhedaiji" } , self.Pane_my_Tier )
    Effect_con:SetPosition( self.Pane_my_Tier:GetContentWidth()/2 , self.Pane_my_Tier:GetContentHeight()/2 - 70 )

    local Effect_con = nil
    self.Effect_Id_ , Effect_con = self:__CreateEffect( { res_name = "UI_wangzhedaiji2" } , self.Panel_he_Tier )
    Effect_con:SetPosition( self.Panel_he_Tier:GetContentWidth()/2 , self.Panel_he_Tier:GetContentHeight()/2 - 70 )

    --玩家信息
    self:SetPlayerInfo()

    --随机信息
    self:RandomHeroInfo()

end

--玩家信息
function Template.LadderVSView:SetPlayerInfo( )

    --区名
    local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
    
    --名字
    local my_name = CallModuleFunc(ModuleType.ROLE, "GetName")
    local name = Util:Label( CPPGameLib.GetString("common_district" , role_info.server_id ) .. " " .. my_name , Macros.TypefaceSize.normal , Macros.Color.ladderVs_1 )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( self.Pane_my_Tier:GetContentWidth()/2  , self.Pane_my_Tier:GetContentHeight() - 95 )
    self.Pane_my_Tier:AddChild( name )

    --模型
    local my_head_id = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
    local __hero_module = LogicGUI.Model.New({info_id = my_head_id , parent = self.Pane_my_Tier  , dir = MathTool.HORZ_RIGHT })
    __hero_module:SetPosition( self.Pane_my_Tier:GetContentWidth()/2 , 100  )


    local t1 = {}
    -- --称号
    -- local titleId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )
    --段位
    local rank , star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    --战力
    local power = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.BATTLE_VALUE)

    t1 = { titleId = titleId  , rank = rank , star = star , power = power , tier = self.Pane_my_Tier }

    self:SetGeneralInfo( t1 )
end


--随机信息
function Template.LadderVSView:RandomHeroInfo()
    --模型
    local my_head_id = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
    self.__hero_module = LogicGUI.Model.New({info_id = my_head_id , parent = self.Panel_he_Tier , dir = MathTool.HORZ_LEFT })
    self.__hero_module:SetPosition( self.Panel_he_Tier:GetContentWidth()/2 , 90  )

    local func = nil
    local function moveAnimate( ... )
        local icon_ , name_ = CallModuleFunc(ModuleType.PVP, "GetRandomInfo")
        self.__hero_module:SetInfoID( icon_ )

        local config_hero = CPPGameLib.GetConfig("Hero", icon_ )
        self.__hero_module:SetScaleFactor( config_hero.show_scale )

        CPPActionManager.DelayTo( self.Panel_he_Tier , 0.001 , func)
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

--对手信息
function Template.LadderVSView:SetRivalInfo( )

    local info = CallModuleFunc(ModuleType.LADDER , "GetMyOpponent" )
    if not info.role_name then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("ladder_noOpponent"))

        self.__hero_module:SetInfoID(  )
        CPPActionManager.DelayTo( self.Panel_center , 3 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
            self:DeleteMe()
        end) )
        
        return
    end
    
    self.icon_bg:SetVisible(false)
    CPPActionManager.StopAllActions(self.Panel_he_Tier)

    --名字
    local name = Util:Label( CPPGameLib.GetString("common_district" , info.server ) .. " " .. info.role_name , Macros.TypefaceSize.normal , Macros.Color.ladderVs_2 )
    name:SetPosition( self.Panel_he_Tier:GetContentWidth()/2  , self.Panel_he_Tier:GetContentHeight() - 95 )
    self.Panel_he_Tier:AddChild( name )

    --模型
    local my_head_id = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
    self.__hero_module:SetInfoID( info.head_img_id )

    local config_hero = CPPGameLib.GetConfig("Hero", info.head_img_id )
    self.__hero_module:SetScaleFactor( config_hero.show_scale )
        
    local t1 = {}
    t1 = { titleId = info.title_id  , rank = info.stage_level , star = info.star , power = info.power , tier = self.Panel_he_Tier }

    self:SetGeneralInfo( t1 )


    local Effect_con = nil
    self.wangzhepipei , Effect_con = self:__CreateEffect( { id = 100088 } , self.Panel_icon_Tier )
    PosTool.Center( Effect_con )

    CPPActionManager.DelayTo( self.Panel_center , 2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
        
        local checkBattle = CallModuleFunc(ModuleType.LADDER , "CheckBattleCondition" )
        if checkBattle then
            CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.LADDER , info.role_id)
        end

    end) )
end


--设置通用信息 战力 称号 段位
function Template.LadderVSView:SetGeneralInfo( data )
    --称号
    -- if data.titleId and data.titleId ~= 0 then
        -- local rankSp = Resource.PathTool.GetTitleIconPath( "sp_title_".. 2000 , true )
        -- title = Util:Sprite( rankSp )
        -- title:SetAnchorPoint( 0.5 , 0.5 )
        -- title:SetPosition( data.tier:GetContentWidth()/2  , data.tier:GetContentHeight() - (title:GetContentHeight()/2) + 10 )
        -- data.tier:AddChild( title )
    -- end

    --段位
    if data.rank then
        if data.rank == 0 then
            data.rank = 1
        end
        local dan = Util:Dan( data.rank , data.star )
        dan:SetAnchorPoint( 0.5 , 0.5 )
        data.tier:AddChild( dan )
        dan:SetPosition( data.tier:GetContentWidth()/2 , 50  )
    end


    --战斗力
    if data.power then
        local maincity_battle_power = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_power" , true) )

        
        local __power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
        __power_font:SetText(tostring( data.power ))

        local layer = Util:Layout( maincity_battle_power:GetContentWidth() + __power_font:GetNode():GetContentWidth() + 5 , 10 )
        layer:SetPosition( data.tier:GetContentWidth()/2 , 15 )
        layer:SetAnchorPoint( 0.5 , 0.5 )
        data.tier:AddChild( layer )

        maincity_battle_power:SetAnchorPoint( 0 , 0.5)
        maincity_battle_power:SetPosition( 0 , layer:GetContentHeight()/2 )
        layer:AddChild( maincity_battle_power )

        layer:AddChild( __power_font:GetNode() )
        PosTool.RightTo( maincity_battle_power , __power_font )
    end


end

--匹配
function Template.LadderVSView:OnButtonMate( )
    print("xy" , "匹配")
end
