
Modules = Modules or {}

Modules.TemplPolytropicTypeActivityItem = Modules.TemplPolytropicTypeActivityItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeActivityItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeActivityItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(300,130)
    return self.layout
end

function Modules.TemplPolytropicTypeActivityItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.TemplPolytropicTypeActivityItem:__Getwidget()
    --TODO 不够图就随机
    local sp_res_list = {"47","72","84","102"}
    local random = MathTool.GetRandom(1, #sp_res_list)
    --贴图
    local bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON,"sp_prompt_"..sp_res_list[random])--self.__data.system_id
    self.__bottom_bg = Util:Sprite(bg_sp)
    self.layout:AddChild(self.__bottom_bg)
    PosTool.Center(self.__bottom_bg)

    --选中框
    local sp_selected = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON,"sp_prompt_on")
    self.__selected_frame = Util:Sprite(sp_selected)
    self.__bottom_bg:AddChild(self.__selected_frame,3)
    PosTool.Center(self.__selected_frame)
    self.__selected_frame:SetVisible(false) 
end

function Modules.TemplPolytropicTypeActivityItem:__ShowUI()
    local config_system_entry = CPPGameLib.GetConfig("SystemEntry",self.__data.system_id,false)
    if not config_system_entry then
        return
    end
    --名字
    local name = Util:Label(config_system_entry.name,Macros.TypefaceSize.largish,Macros.Color.Special)
    name:SetAnchorPoint(0,0)
    name:SetPosition(10,8)
    self.__bottom_bg:AddChild(name)

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

    --黑色遮罩
    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW,"black1_bg")
    local black1_bg_sp = Util:Sprite9(black1_bg_sp)
    black1_bg_sp:SetContentSize(self.__bottom_bg:GetContentWidth(),self.__bottom_bg:GetContentHeight())
    self.__bottom_bg:AddChild(black1_bg_sp,20)
    PosTool.Center(black1_bg_sp)
    black1_bg_sp:SetVisible(false)

    --锁
    local lock_sp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LIMIT_DUNGEON,"sp_prompt_lock"))
    lock_sp:SetAnchorPoint(0.5,0.5)
    black1_bg_sp:AddChild(lock_sp)
    PosTool.Center(lock_sp,-30,0)
    lock_sp:SetVisible(false)

    --开启等级
    local open_txt = Util:Label(CPPGameLib.GetString("common_open_lev",config_system_entry.level), Macros.TypefaceSize.normal, Macros.Color.btn)
    black1_bg_sp:AddChild(open_txt)
    PosTool.Center(open_txt,30,0)
    open_txt:SetVisible(false)
end

function Modules.TemplPolytropicTypeActivityItem:SetSelected(var)
    self.__selected_frame:SetVisible(var)
end