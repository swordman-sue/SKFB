--[[
%% @module: 天梯赛荣誉
%% @author: yjg
%% @created: 2017年10月30日17:30:02
--]]

Modules = Modules or {} 
Modules.LadderHonorView = Modules.LadderHonorView or BaseClass(BaseView)

function Modules.LadderHonorView:__init()
    self.__layout_name = "ladder_honor"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_ladder")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("ladder_kingPalace"), 
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
end

function Modules.LadderHonorView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server() 
end	

function Modules.LadderHonorView:__Dispose()
end

function Modules.LadderHonorView:__Server()

    CallModuleFunc(ModuleType.LADDER, "KingMatchSeasonRankReq")

    self:BindGlobalEvent(Macros.Event.LadderModule.LADDER_HONOR , function (_, data)
        self:__SetShowUI()
    end)
    -- self:__SetShowUI()
end

function Modules.LadderHonorView:__Getwidget( )
    --居中层
    self.Panel_center = self:GetWidget("Panel_center")
    --滑动层
    local PageView = self:GetWidget("PageView_honor")
    PageView:SetVisible(false)

    local Pvbg = Util:Layout( PageView:GetContentWidth() , PageView:GetContentHeight() )
    Pvbg:SetPosition( PageView:GetPositionX() , PageView:GetPositionY() )
    Pvbg:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_center:AddChild( Pvbg )

    self.PageView_honor = GUI.PageView.New(
                                        {
                                            view_width = Pvbg:GetContentWidth(),
                                            view_height = Pvbg:GetContentHeight(),
                                            turn_page_func = function( ... )
                                                local index = self.PageView_honor:GetCurPageIndex()
                                                self:Onbox( index + 1 )
                                            end
                                        }
                                    )
    Pvbg:AddChild(self.PageView_honor:GetNode())
    PosTool.Center(self.PageView_honor)

    self.Image_left = self:GetWidget("Image_left")
    self.Image_right = self:GetWidget("Image_right")
end 


function Modules.LadderHonorView:__ShowUI( )
    local ladder_kingPalace_tips = Util:Label( CPPGameLib.GetString("ladder_kingPalace_tips") )
    self.Panel_center:AddChild( ladder_kingPalace_tips )
    PosTool.Center( ladder_kingPalace_tips , 0 , -290 )
end

function Modules.LadderHonorView:__SetShowUI( )
    local lst = CallModuleFunc(ModuleType.LADDER, "GetHistoryMost")

    for i , v in ipairs( lst ) do
        v.page_class = Modules.LadderHonorPage
    end
    self.PageView_honor:SetDataList( lst )

    self.PageView_honor:ForceDoLayout()
    self.PageView_honor:ScrollToPage( #lst - 1 , 0 )
end

function Modules.LadderHonorView:Onbox( index )
    local lst = CallModuleFunc(ModuleType.LADDER, "GetHistoryMost")
    if index <= 1 and index ~= #lst then
        self.Image_left:SetVisible(false)
        self.Image_right:SetVisible(true)
    elseif index >= #lst and index ~= 1 then
        self.Image_left:SetVisible(true)
        self.Image_right:SetVisible(false)
    elseif index == #lst and index == 1 then
        self.Image_left:SetVisible(false)
        self.Image_right:SetVisible(false)
    else
        self.Image_left:SetVisible(true)
        self.Image_right:SetVisible(true)
    end
end

--碎片页面
Modules = Modules or {}

Modules.LadderHonorPage = Modules.LadderHonorPage or BaseClass( GUI.Template )

function Modules.LadderHonorPage:__init( index )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__render_queue = Scene.RenderQueue.GRQ_UI_UP
    self:InitTemplate(nil)  
end

function Modules.LadderHonorPage:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(960,640)
    return self.__layout
end

function Modules.LadderHonorPage:__AddToParent()
end

function Modules.LadderHonorPage:__delete()
end

function Modules.LadderHonorPage:SetData(data)
    self:__ShowUI( data )
end

function Modules.LadderHonorPage:__ShowUI(data)

    local sort , year = CallModuleFunc( ModuleType.LADDER , "GetNowSeason" )
    if data.sort then
        sort = data.sort
        year = CallModuleFunc( ModuleType.LADDER , "GetSeasonVintage" , data.sort )
    end
    local ladder_Season = Util:Label( CPPGameLib.GetString("ladder_Season" , year , sort ) , Macros.TypefaceSize.popup , Macros.Color.Special )
    self.__layout:AddChild( ladder_Season )
    PosTool.Center( ladder_Season , 0 , 270 )

    local pos = {
            [1] = { x = self.__layout:GetContentWidth()/2 , y = self.__layout:GetContentHeight()/2 - 100 },
            [2] = { x = self.__layout:GetContentWidth()/2 - 250 , y = self.__layout:GetContentHeight()/2 - 200 },
            [3] = { x = self.__layout:GetContentWidth()/2 + 250 , y = self.__layout:GetContentHeight()/2 - 200 },
    }

    self.effect = {}
    for i , v in ipairs( pos ) do

        local Effect_con = nil
        self.effect[i] , Effect_con = self:__CreateEffect( { res_name = "UI_wangzhedaiji" } , self.__layout )
        Effect_con:SetPosition(  v.x , v.y + 25 )

        if data and not data.typeNil and data.lst[i] then

            local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_".. i )
            local rank = Util:Sprite( rankSp )
            rank:SetAnchorPoint( 0.5 , 0.5 )
            rank:SetPosition( v.x - 120 , v.y + 220 )
            self.__layout:AddChild( rank )

            local info = data.lst[i]

            --称号
            if info.titleId and info.titleId ~= 0 then
                local rankSp = Resource.PathTool.GetTitleIconPath( "sp_title_".. info.titleId , true )
                title = Util:Sprite( rankSp )
                title:SetAnchorPoint( 0.5 , 0.5 )
                title:SetPosition( v.x , v.y + 300 )
                self.__layout:AddChild( title , 5)
            end

            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER,"sp_ladder_mate_bg" )
            --名字
            local nameBg = Util:Sprite( sp )
            nameBg:SetAnchorPoint( 0.5 , 0.5 )
            nameBg:SetPosition( v.x , v.y + 240 )
            self.__layout:AddChild( nameBg , 5)

            local name = Util:Name( info.name , Macros.TypefaceSize.normal )
            nameBg:AddChild( name )
            PosTool.Center( name )

            --服务器
            local serverBg = Util:Sprite( sp )
            serverBg:SetAnchorPoint( 0.5 , 0.5 )
            serverBg:SetPosition( v.x , v.y + 205 )
            self.__layout:AddChild( serverBg , 5)

            local lst_ = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", info.server)
            local server = Util:Name( "s" .. info.server .. " " .. lst_.name , Macros.TypefaceSize.normal )
            serverBg:AddChild( server )
            PosTool.Center( server )

            --模型
            local __hero_module = LogicGUI.Model.New({info_id = info.icon , parent = self.__layout })
            __hero_module:SetPosition( v.x , v.y )
        else
            local rank_icon = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_chasm_copy" , true ) )
            rank_icon:SetAnchorPoint( 0.5 , 0 )
            rank_icon:SetPosition( v.x , v.y + 100 )
            self.__layout:AddChild( rank_icon )

        end

    end
end


