--[[
%% @module: 赛季奖励
%% @author: yjg
%% @created: 2017年10月30日18:19:28
--]]

Modules = Modules or {} 
Modules.LadderSeasonAwardView = Modules.LadderSeasonAwardView or BaseClass(BaseView)

function Modules.LadderSeasonAwardView:__init()
    self.__layout_name = "ladder_season_award"	
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
    self.__tab_params_ex = {6 , true}

    self.__open_callback = function()
      self:__OpenCallback()
    end
end

function Modules.LadderSeasonAwardView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()

    CallModuleFunc(ModuleType.LADDER , "LadderRankReq" )
    self:BindGlobalEvent(Macros.Event.LadderModule.LADDER_RANK , function()
        self:__SetShowUI()
    end)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER , true)
end	

function Modules.LadderSeasonAwardView:__Dispose()
    if self.Effect_Id then
        self:__DelEffect(self.Effect_Id)
        self.Effect_Id = nil
    end 
end

function Modules.LadderSeasonAwardView:__Getwidget( )
    --第一
    self.Panel_first = self:GetWidget("Panel_first")

    --奖励
    self.Panel_award = self:GetWidget("Panel_award")

    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --内容
    self.Image_content = self:GetWidget("Image_content")
    --页签
    self.Image_label = self:GetWidget("Image_label")
end 


function Modules.LadderSeasonAwardView:__ShowUI( )
    --王者组排名
    local params =  
    {
        item_class = Modules.TemplLadderKingRankItem,
        item_width = 783.00 ,
        item_height = 100 ,
        row = 4,
        col = 1,
        item_space_r = 5 , 
        view_height = self.Image_content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.kingRankList = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.kingRankList:GetNode())
    PosTool.Center(self.kingRankList:GetNode())

    local t1 = {
                [1] = { text = CPPGameLib.GetString("ladder_ranking") , x = -330 } , 
                [2] = { text = CPPGameLib.GetString("ladder_playerInfo") , x = -160 } , 
                [3] = { text = CPPGameLib.GetString("ladder_power") , x = 25} , 
                [4] = { text = CPPGameLib.GetString("ladder_played") , x = 125} , 
                [5] = { text = CPPGameLib.GetString("ladder_integral") , x = 225} , 
                [6] = { text = CPPGameLib.GetString("ladder_victory") , x = 325} , 
            }
    for i,v in ipairs( t1 ) do
        local text = Util:Label( v.text , Macros.TypefaceSize.normal , Macros.Color.ladderRank )
        text:SetAnchorPoint( 0.5 , 0.5 )
        text:SetPosition( v.x , 205 )
        self.kingRankList:AddChild(text)
    end

    --赛季奖励
    local params =  
    {
        item_class = Modules.TemplLadderSeasonAwardItem,
        item_width = 783.00 ,
        item_height = 100 ,
        row = 4,
        col = 1,
        item_space_r = 5 , 
        view_height = self.Image_content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.seasonAwardList = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.seasonAwardList:GetNode())
    PosTool.Center(self.seasonAwardList:GetNode())

    local lst = CallModuleFunc(ModuleType.LADDER , "GetRankAwardCnf" )
    self.seasonAwardList:SetDataList( lst )

    local t1 = {
                [1] = { text = CPPGameLib.GetString("ladder_rank") , x = -320 } , 
                [2] = { text = CPPGameLib.GetString("ladder_awardItem") , x = -40 } , 
                [3] = { text = CPPGameLib.GetString("ladder_awardTitle") , x = 280} , 
            }
    for i,v in ipairs( t1 ) do
        local text = Util:Label( v.text , Macros.TypefaceSize.normal , Macros.Color.ladderRank )
        text:SetAnchorPoint( 0.5 , 0.5 )
        text:SetPosition( v.x , 205 )
        self.seasonAwardList:AddChild(text)
    end

    --页签
    local text = { }
        text =  {
            [1] = CPPGameLib.GetString("ladder_king_award") ,
            [2] = CPPGameLib.GetString("ladder_season_award") ,
        }

    self.__tab:SetContentSize( self.Image_label:GetContentWidth() , self.Image_label:GetContentHeight() + 20 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition(self.Image_label:GetContentWidth()/2 , self.Image_label:GetContentHeight()/2 )
    self.Image_label:AddChild(self.__tab:GetNode())

    self.__tab:On( unpack(self.__open_data) or 1 )


    local Effect_con = nil
    self.Effect_Id , Effect_con = self:__CreateEffect( { res_name = "UI_wangzhedaiji" } , self.Panel_first )
    Effect_con:SetPosition( self.Panel_first:GetContentWidth()/2 , self.Panel_first:GetContentHeight()/2 - 115 )

    --领奖提示
    local ladder_award_time = Util:Label( CPPGameLib.GetString("ladder_award_time") , Macros.TypefaceSize.normal , Macros.Color.white )
    ladder_award_time:SetAnchorPoint( 0.5 , 0 )
    ladder_award_time:SetPosition( self.Panel_award:GetContentWidth()/2 , 15 )
    self.Panel_award:AddChild(ladder_award_time)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_now_dan" , true )
    self.now_dan = Util:Sprite( sp )
    self.now_dan:SetAnchorPoint( 0 , 0.5 )
    self.Image_bg:AddChild( self.now_dan )
    self.now_dan:SetPosition( 10 , 22  )

end

function Modules.LadderSeasonAwardView:__SetShowUI( )
    --强者列表
    local lst = CallModuleFunc(ModuleType.LADDER , "GetKingLst" )
    self.kingRankList:SetDataList( lst )
    if #lst <= 0 then
        if not self.ladder_noKing then
            self.ladder_noKing = Util:Label( CPPGameLib.GetString("ladder_noKing") , Macros.TypefaceSize.largish , Macros.Color.Special )
            self.ladder_noKing:SetAnchorPoint( 0.5 , .5 )
            self.ladder_noKing:SetPosition( self.Image_content:GetContentWidth()/2 , self.Image_content:GetContentHeight()/2 )
            self.Image_content:AddChild(self.ladder_noKing , 99)
        else
            self.ladder_noKing:SetVisible( true )
        end
    else
        if self.ladder_noKing then
            self.ladder_noKing:SetVisible( false )
        end
    end

    --玩家信息
    self:SetPlayerInfo()
end

function Modules.LadderSeasonAwardView:Ceil( index , data )
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
function Modules.LadderSeasonAwardView:OnCallBack( index , data )
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

    self:RankAward( index )
end


function Modules.LadderSeasonAwardView:SetPlayerInfo( )
  --称号
    local titleId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )

    local most = CallModuleFunc(ModuleType.LADDER, "GetMost")

    local title = nil
    if most and most.titleId and most.titleId ~= 0 then
        local rankSp = Resource.PathTool.GetTitleIconPath( "sp_title_".. most.titleId , true )
        title = Util:Sprite( rankSp )
        title:SetAnchorPoint( 0.5 , 0.5 )
        title:SetPosition( self.Panel_first:GetContentWidth()/2  , self.Panel_first:GetContentHeight() - (title:GetContentHeight()/2) - 80 )
        self.Panel_first:AddChild( title , 5 )
    end

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER,"sp_ladder_mate_bg" )

    --名字
    local nameBg = Util:Sprite( sp )
    nameBg:SetAnchorPoint( 0.5 , 0.5 )
    nameBg:SetPosition( self.Panel_first:GetContentWidth()/2  , self.Panel_first:GetContentHeight() - 220 )
    self.Panel_first:AddChild( nameBg , 5)

    local name = Util:Label( most.name or "" , Macros.TypefaceSize.normal , Macros.Color.white )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( nameBg:GetContentWidth()/2  , nameBg:GetContentHeight()/2 )
    nameBg:AddChild( name )


    if most and most.server_id then
        --服务器
        local lst_ = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", most.server_id )
        local serverBg = Util:Sprite( sp )
        serverBg:SetAnchorPoint( 0.5 , 0.5 )
        serverBg:SetPosition( self.Panel_first:GetContentWidth()/2  , self.Panel_first:GetContentHeight() - 260 )
        self.Panel_first:AddChild( serverBg , 5 )

        if lst_ then
            local server = Util:Label( "s".. most.server_id .. " " .. lst_.name , Macros.TypefaceSize.normal , Macros.Color.white )
            server:SetAnchorPoint( 0.5 , 0.5 )
            server:SetPosition( serverBg:GetContentWidth()/2  , serverBg:GetContentHeight()/2 )
            serverBg:AddChild( server )
        end
    end

    --模型
    if most.head_id then
        local __hero_module = LogicGUI.Model.New({info_id = most.head_id , parent = self.Panel_first })
        __hero_module:SetPosition( self.Panel_first:GetContentWidth()/2 , 185  )
        
        local config_hero = CPPGameLib.GetConfig("Hero", most.head_id )
        __hero_module:SetScaleFactor( config_hero.show_scale )
    else
        local rank_icon = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_chasm_copy" , true ) )
        rank_icon:SetAnchorPoint( 0.5 , 0.5 )
        rank_icon:SetPosition( self.Panel_first:GetContentWidth()/2 , 300  )
        self.Panel_first:AddChild( rank_icon )
    end

    --战力
    local powerBg = Util:Sprite( sp )
    powerBg:SetAnchorPoint( 0.5 , 0.5 )
    powerBg:SetPosition( self.Panel_first:GetContentWidth()/2  , 130 )
    self.Panel_first:AddChild( powerBg )

    --战斗力
    local maincity_battle_power = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_power" , true) )
    maincity_battle_power:SetAnchorPoint( 0 , 0.5 )
    maincity_battle_power:SetPosition( 0 , powerBg:GetContentHeight()/2 )
    powerBg:AddChild( maincity_battle_power )
    local __power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
    powerBg:AddChild( __power_font:GetNode() )
    PosTool.RightTo( maincity_battle_power , __power_font )
    __power_font:SetText(tostring( most.power ))

    --我的排行
    local rank , star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    if rank == 0 then
        rank = 1
    end

    local dan = Util:Dan( rank , star )
    self.Image_bg:AddChild( dan )
    PosTool.RightTo( self.now_dan , dan  , 5 )
    

    local cnf = CPPGameLib.GetConfig("LadderDan", rank , false )
    if cnf.type >= Macros.Game.LADDER_LEVEL.LEVEL_7 then
        local rank , star = CallModuleFunc(ModuleType.LADDER, "GetMyGrid")

        local text = rank
        if rank == 0 then
            text = CPPGameLib.GetString("rank_reflection")
        end

        local server = Util:Label( CPPGameLib.GetString("ladder_season_rank" , text ) , Macros.TypefaceSize.normal , Macros.Color.Special )
        self.Image_bg:AddChild( server )
        PosTool.RightTo( dan , server  , 5 )
    end


end


function Modules.LadderSeasonAwardView:RankAward( index )
    self.kingRankList:SetVisible(false)
    self.seasonAwardList:SetVisible(false)

    if self.ladder_noKing then
        self.ladder_noKing:SetVisible(false)
    end

    if index == 1 then
        self.kingRankList:SetVisible(true)
        if self.ladder_noKing then
            self.ladder_noKing:SetVisible(true)
        end
    else
        self.seasonAwardList:SetVisible(true)
    end
end



--王者排名
Modules = Modules or {}

Modules.TemplLadderKingRankItem = Modules.TemplLadderKingRankItem or BaseClass(GUI.ScrollItem)

function Modules.TemplLadderKingRankItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplLadderKingRankItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 783.00 , 100.00)
    return self.layout
end

function Modules.TemplLadderKingRankItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)


    CPPGameLib.PrintTable("xy" , data , "请求物品列表")  

    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_season_award")
    local bg = Util:Sprite(sp)
    bg:SetAnchorPoint(0.5,0.5)
    self.layout:AddChild(bg) 
    PosTool.Center( bg ) 

    --排名图标
    if data.rank <= 3 then
        local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
        local icon = Util:Sprite(imageData)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetPosition( 50 , bg:GetContentHeight() / 2)
        bg:AddChild(icon)
    else
        local iconValue = Util:Label(self.__data.rank,Macros.TypefaceSize.popup,Macros.Color.Special)
        iconValue:SetPosition( 50 , bg:GetContentHeight() / 2)
        bg:AddChild(iconValue) 
    end

    local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER,"sp_ladder_division_bright" )
    local icon = Util:Sprite(imageData)
    icon:SetAnchorPoint(0.5,0.5)
    icon:SetPosition( 100 , bg:GetContentHeight() / 2)
    bg:AddChild(icon)

    --玩家头像
    local ownIcon = LogicGUI.RoleIcon.New(false)
    ownIcon:SetAnchorPoint( 0 , 0.5 )
    bg:AddChild(ownIcon:GetNode())
    ownIcon:SetData( data.head_id)
    ownIcon:SetPosition( 130 , bg:GetContentHeight()/2 )

    --名字
    local name = Util:Label( data.name , Macros.TypefaceSize.largish , Macros.Color.white )
    bg:AddChild( name )
    PosTool.RightTo( ownIcon , name , 20 , 15 )

    --服务器
    local lst_ = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", data.server_id )
    if lst_ then 
        local server = Util:Label( "s".. data.server_id .. " " .. lst_.name , Macros.TypefaceSize.normal , Macros.Color.Special )
        bg:AddChild( server )
        PosTool.RightTo( ownIcon , server , 20 , -15 )
    end

    --战力
    local power = Util:Label( data.power , Macros.TypefaceSize.normal , Macros.Color.white )
    bg:AddChild( power )
    power:SetPosition( bg:GetContentWidth()/2 + 25, bg:GetContentHeight()/2 )

    --胜率
    local victory = Util:Label( data.victory , Macros.TypefaceSize.normal , Macros.Color.white )
    bg:AddChild( victory )
    victory:SetPosition( bg:GetContentWidth()/2 + 125, bg:GetContentHeight()/2 )

    --积分
    local integral = Util:Label( data.integral , Macros.TypefaceSize.normal , Macros.Color.white )
    bg:AddChild( integral )
    integral:SetPosition( bg:GetContentWidth()/2 + 225, bg:GetContentHeight()/2 )

    --出战
    local played = Util:Label( data.played .. "%" , Macros.TypefaceSize.normal , Macros.Color.white )
    bg:AddChild( played )
    played:SetPosition( bg:GetContentWidth()/2 + 325, bg:GetContentHeight()/2 )

end




--赛季奖励
Modules = Modules or {}

Modules.TemplLadderSeasonAwardItem = Modules.TemplLadderSeasonAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplLadderSeasonAwardItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplLadderSeasonAwardItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 783.00 , 100.00)
    return self.layout
end

function Modules.TemplLadderSeasonAwardItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER, "sp_ladder_season_award"  )
    local awardBG = Util:Sprite( sp )
    awardBG:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( awardBG )
    PosTool.Center( awardBG )

    local tab = {
                    { tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. data.icon , true )},
                    { tp = Resource.PathTool.GetLadderWordartPath("sp_level_" .. data.icon .. "_" .. data.icon_star , true )},
                }
    local icon = Util:LabCenter(tab)
    icon:SetAnchorPoint( 0.5 , 0.5 )
    icon:SetPosition( awardBG:GetContentWidth() / 2 - 320 , awardBG:GetContentHeight() / 2 )
    awardBG:AddChild( icon )

    if data.max then
        local text = CPPGameLib.GetString("ladder_season_rank_1" , data.max )
        if data.min ~= data.max then
            if data.max ~= 0 then
                text = CPPGameLib.GetString("ladder_season_rank_2" , data.min , data.max )
            else
                text = CPPGameLib.GetString("ladder_season_rank_3" , data.min)
            end
        end

        local ladder_victory = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.white )
        ladder_victory:SetAnchorPoint( 0.5 , 0.5 )
        ladder_victory:SetPosition( icon:GetPositionX() , awardBG:GetContentHeight() / 2 - 30 )
        awardBG:AddChild( ladder_victory )
    end

    local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER,"sp_ladder_division_bright" )
    local bright = Util:Sprite(imageData)
    bright:SetAnchorPoint(0.5,0.5)
    bright:SetPosition( 130 , awardBG:GetContentHeight() / 2)
    awardBG:AddChild(bright)

    local num = #data.award
    if #data.award >= 4 then
        num = 4
    end

    --奖励
    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80,
        item_height = 80,
        row = 1,
        col = num ,
        horizon = true ,
        item_space_c = 2 ,
        view_width = num * 80  ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.layout:AddChild(self.lst_list:GetNode())
    PosTool.Center( self.lst_list:GetNode() , -40 )

    self.lst_list:SetDataList( data.award )


    local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER,"sp_ladder_division_dark" )
    local bright = Util:Sprite(imageData)
    bright:SetAnchorPoint(0.5,0.5)
    bright:SetPosition( awardBG:GetContentWidth()/2 + 180 , awardBG:GetContentHeight() / 2)
    awardBG:AddChild(bright)

    if data.title then
        --称号
        local sp_title = Resource.PathTool.GetTitleIconPath( "sp_title_"..data.title , true )
        self.icon = Util:Sprite( sp_title )
        self.layout:AddChild( self.icon )
        PosTool.Center( self.icon , 280 )
    end

end
