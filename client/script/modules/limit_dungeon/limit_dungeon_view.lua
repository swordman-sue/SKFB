
--[[
%% @module: 限时副本汇总(组队、世界BOSS、PVP、怪物攻城....)
--]]

Modules = Modules or {}

Modules.LimitDungeonView = Modules.LimitDungeonView or BaseClass(BaseView)

function Modules.LimitDungeonView:__init()
    self.__layout_name = "prompt"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_prompt")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("prompt"),
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LIMIT_DUNGEON)  
end

function Modules.LimitDungeonView:__OpenCallback()
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.LimitDungeonView:__Dispose()
end

function Modules.LimitDungeonView:__RegistAllEvents()
    --红点更新
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE , function ( _ , system_id)
        self.lst_list:ItemIter(function(index, item, data)
            if data.id == system_id then
                item:UpdateRedDot()
                return true
            end
        end)
    end)    
end

function Modules.LimitDungeonView:__Getwidget()
    --框
    self.Image_frame = self:GetWidget("Image_frame") 
    self.Image_frame:SetImage( Resource.PathTool.GetPromptPath("sp_activity_frame") , TextureResType.LOCAL )
    --道具列表
    self.Panel_icon = self:GetWidget("Panel_icon")
    --场景
    self.Panel_scene = self:GetWidget("Panel_scene")
    --说明底
    self.Image_explain = self:GetWidget("Image_explain")
    --名字
    self.Image_name = self:GetWidget("Image_name")
    --前往
    self.Button_go = Util:WidgetButton( 
                                        self:GetWidget("Button_go") , 
                                        CPPGameLib.GetString("prompt_go") , 
                                        CPPGameLib.Handler(self,self.OnButtonGo),
                                        nil,nil,Macros.Color.button_yellow
                                    )
    --列表
    self.Panel_lst = self:GetWidget("Panel_lst") 
end

function Modules.LimitDungeonView:__ShowUI( ... )
    --活动介绍
    local prompt_introduce = Util:Name( CPPGameLib.GetString("prompt_introduce") , Macros.TypefaceSize.normal , 100 )
    prompt_introduce:SetAnchorPoint( 0 , 0.5 )
    prompt_introduce:SetPosition( 15 , self.Image_explain:GetContentHeight()/2 )
    self.Image_explain:AddChild( prompt_introduce )

    self.iconLst = {}
    for i = 1 , 5 do
        self.iconLst[i] = LogicGUI.IconLayer.New()
        self.iconLst[i]:SetPosition( 50 + (85*(i-1)) , self.Panel_icon:GetContentHeight()/2  )
        self.Panel_icon:AddChild( self.iconLst[i]:GetNode() )
    end

    --选择列表
    local params =  
    {
        item_class = Modules.LimitDungeonTtem,
        item_width = self.Panel_lst:GetContentWidth() ,
        item_height = 130 ,
        row = 4,
        col = 1,
        item_space_c = 3,
        selectable = true , 
        select_callback = function( index , itemData )
            self.index = index
            self:__SetShowUI( itemData )
        end,
        view_height = self.Panel_lst:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    local list = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonList")

    self.lst_list:SetDataList(list)

    self.lst_list:SelectItem( unpack(self.__open_data) or 1 )
end

function Modules.LimitDungeonView:__SetShowUI( data )

    CPPGameLib.PrintTable("xy" , data ,"__SetShowUI")

    self.data = data

    --设置选中的场景
    local prompt = Resource.PathTool.GetPromptPath("sp_prompt_" .. data.id )
    local promptwh = Util:Sprite( prompt )
    local promptsp = Util:Sprite9( prompt , {promptwh:GetContentWidth() , promptwh:GetContentHeight()} )
    promptsp:SetContentSize( self.Panel_scene:GetContentWidth() , self.Panel_scene:GetContentHeight() )   
    self.Panel_scene:AddChild( promptsp )
    PosTool.Center( promptsp )

    if not self.timeLst then
        self.timeLst = {}
    end

    for i = 1 , #self.timeLst do
        self.timeLst[i]:RemoveSelf(true)
        self.timeLst[i] = nil
    end

    local timeTextLst = CPPGameLib.SplitEx( data.time_text , "<GH>")  
    local y = 440
    for i,v in ipairs( timeTextLst ) do
        self.timeLst[i] = Util:Name( v , Macros.TypefaceSize.normal )
        self.timeLst[i]:SetAnchorPoint( 0 , 0.5 )
        self.Image_frame:AddChild( self.timeLst[i] )
        if self.timeLst[i-1] then
            y = self.timeLst[i-1]:GetPositionY() - (self.timeLst[i-1]:GetContentHeight()/2) - 15
        end 
        self.timeLst[i]:SetPosition( 25 , y )
    end

    if not self.prompt_introduce then
        self.prompt_introduce = {}
    end

    for i = 1 , #self.prompt_introduce do
        self.prompt_introduce[i]:RemoveSelf(true)
        self.prompt_introduce[i] = nil
    end

    local introduceLst = CPPGameLib.SplitEx( data.introduce or "" , "<GH>")  
    local y = -5
    for i,v in ipairs( introduceLst ) do
        self.prompt_introduce[i] = Util:Name( v , Macros.TypefaceSize.normal )
        self.prompt_introduce[i]:SetAnchorPoint( 0 , 1 )
        self.Image_explain:AddChild( self.prompt_introduce[i] )
        if self.prompt_introduce[i-1] then
            y = self.prompt_introduce[i-1]:GetPositionY() - self.prompt_introduce[i-1]:GetContentHeight() - 5
        end 
        self.prompt_introduce[i]:SetPosition( 15 , y )
    end

    for i = 1 , 5 do
        if data.reward_icon[i] then
            self.iconLst[i]:SetData(  {id = data.reward_icon[i] } )
            self.iconLst[i]:SetVisible(true)
        else
            self.iconLst[i]:SetVisible(false)
        end
    end

    self.Image_name:SetImage( Resource.PathTool.GetPromptPath("sp_prompt_text_"..data.id) , TextureResType.LOCAL )
    self.Image_name:IgnoreContentAdaptWithSize(true)

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if data.level > role_level then
        if data.level > role_level then
            self.Button_go:SetText( CPPGameLib.GetString("prompt_noLv") )
        else
            self.Button_go:SetText( CPPGameLib.GetString("prompt_noOpen") )
        end

        if data.id == Macros.Game.SystemID.PVP then
            local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
            if time < ConfigAdapter.Common.GetPVPOpenFate() then
                self.Button_go:SetText( CPPGameLib.GetString("system_foreshow_maincity_text" , " ") )
                return 
            end
        end

        Util:GreyButton( self.Button_go )
    else

        if data.id == Macros.Game.SystemID.PVP then
            local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
            if time < ConfigAdapter.Common.GetPVPOpenFate() then
                self.Button_go:SetText( CPPGameLib.GetString("system_foreshow_maincity_text" , " ") )
                Util:GreyButton( self.Button_go )
                return 
            end
        end
        self.Button_go:SetEnabled(true)
        self.Button_go:SetText(CPPGameLib.GetString("prompt_go"))     
        local r, g, b, a = unpack(Macros.Color.button_yellow)
        local stroke_width = 2
        self.Button_go:SetOutline(r, g, b, a, stroke_width) 
    end
end

function Modules.LimitDungeonView:OnButtonGo(  )
    if self.data.id == Macros.Game.SystemID.PVP then
        local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
        if time < ConfigAdapter.Common.GetPVPOpenFate() then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("system_foreshow_not_yet_open") )
            return 
        end
    end

    local cnf = CPPGameLib.GetConfig("SystemEntry", self.data.id)
    local num = nil
    if cnf.view_params and #cnf.view_params ~= 0 then
        num = unpack(cnf.view_params)
    end

    if self.data.id == Macros.Game.SystemID.LIMIT_TEAM then    
        CallModuleFunc(ModuleType.TEAM, "SetEnterTeamHallFrmPromptView", true)
        CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1) 
    else
        CallModuleFunc( cnf.module_type, "OpenView", cnf.view_type or cnf.module_type , num )
    end
end

--获取打开参数
function Modules.LimitDungeonView:GetOpenParams()
    if self.index then
        return {self.index}
    end
end


Modules = Modules or {}

Modules.LimitDungeonTtem = Modules.LimitDungeonTtem or BaseClass(GUI.ScrollItem)

function Modules.LimitDungeonTtem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.LimitDungeonTtem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 298.00 , 130 )
    return self.layout
end

function Modules.LimitDungeonTtem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI( data )
end

function Modules.LimitDungeonTtem:__Getwidget()
    local no_select = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON, "sp_prompt_".. self.__data.id )
    self.btn = Util:Sprite( no_select )
    self.layout:AddChild( self.btn )
    PosTool.Center( self.btn )

    local sp_prompt_on_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON, "sp_prompt_on" )
    self.prompt = Util:Sprite( sp_prompt_on_ )
    self.btn:AddChild( self.prompt , 3 )
    PosTool.Center( self.prompt )
    self.prompt:SetVisible(false)
end

function Modules.LimitDungeonTtem:__ShowUI( data )
    local config_system_entry = CPPGameLib.GetConfig("SystemEntry", data.id, false)
    --名字
    local name = Util:Label( config_system_entry.name , Macros.TypefaceSize.largish , Macros.Color.Special )
    name:SetAnchorPoint( 0 , 0 )
    name:SetPosition( 10 , 8 )
    self.btn:AddChild( name )

    --状态
    local sp_activity_open_ = Resource.PathTool.GetPromptPath("sp_activity_open" )
    self.sp_activity_open_ = Util:Sprite( sp_activity_open_ )
    self.sp_activity_open_:SetAnchorPoint( 0 , 1 )
    self.sp_activity_open_:SetPosition( 2 , self.btn:GetContentHeight() - 3 )
    self.btn:AddChild( self.sp_activity_open_ )

    --红点
    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite(icon_red_point)
    self.icon_red_point:SetPosition( self.btn:GetContentWidth() - 10 , self.btn:GetContentHeight() - 10 )
    self.btn:AddChild( self.icon_red_point , 20 )
    self:UpdateRedDot()

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if data.level > role_level then

        local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
        local black1_bg_sp = Util:Sprite9(black1_bg_sp)
        black1_bg_sp:SetPosition( self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )
        black1_bg_sp:SetContentSize( self.btn:GetContentWidth() , self.btn:GetContentHeight() )
        self.btn:AddChild( black1_bg_sp , 20 )

        if self.lock then
            self.lock:RemoveSelf(true)
            self.lock = nil
        end

        local tab = {
                        { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON, "sp_prompt_lock") , dx = 0.9 },
                        { zi = CPPGameLib.GetString("common_open_lev" , "  " .. data.level .. " " ) , dx = Macros.TypefaceSize.normal , x = 0 , y = -5 },
                    }
                    
        if data.id == Macros.Game.SystemID.PVP then
            local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
            if time < ConfigAdapter.Common.GetPVPOpenFate() then
                tab = {
                            { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON, "sp_prompt_lock") , dx = 0.9 },
                            { zi = CPPGameLib.GetString("system_foreshow_maincity_text" , " " ) , ys = Macros.Color.red , dx = Macros.TypefaceSize.normal , x = 0 , y = -5 },
                        }
            end
        end

        --用作好几个组件拼起来居中显示的强迫症控件 2.0版
        self.lock = Util:LabCenter(tab)
        self.lock:SetAnchorPoint( 0.5 , 0.5 )
        black1_bg_sp:AddChild( self.lock )
        PosTool.Center( self.lock )

    else

        if data.id == Macros.Game.SystemID.PVP then
            local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
            if time < ConfigAdapter.Common.GetPVPOpenFate() then
                local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
                local black1_bg_sp = Util:Sprite9(black1_bg_sp)
                black1_bg_sp:SetPosition( self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )
                black1_bg_sp:SetContentSize( self.btn:GetContentWidth() , self.btn:GetContentHeight() )
                self.btn:AddChild( black1_bg_sp , 20 )

                if self.lock then
                    self.lock:RemoveSelf(true)
                    self.lock = nil
                end
                local tab = {
                                { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON, "sp_prompt_lock") , dx = 0.9 },
                                { zi = CPPGameLib.GetString("system_foreshow_maincity_text" , " " ) , ys = Macros.Color.red , dx = Macros.TypefaceSize.normal , x = 0 , y = -5 },
                            }
                --用作好几个组件拼起来居中显示的强迫症控件 2.0版
                self.lock = Util:LabCenter(tab)
                self.lock:SetAnchorPoint( 0.5 , 0.5 )
                black1_bg_sp:AddChild( self.lock )
                PosTool.Center( self.lock )
            end
        end

    end

    local lab , open = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonOpenDesc" , data.id )
    self.sp_activity_open_:SetVisible( open )
end

function Modules.LimitDungeonTtem:SetSelected( var )
    self.prompt:SetVisible(var)
end

function Modules.LimitDungeonTtem:UpdateRedDot()
    self.icon_red_point:SetVisible(CallModuleFunc(ModuleType.RED_MGR , "GetRed", self.__data.id ) > 0)
end