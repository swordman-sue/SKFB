Modules = Modules or {}

Modules.TemplPolytropicTypeChallengeItem = Modules.TemplPolytropicTypeChallengeItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeChallengeItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeChallengeItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(302,415)
    return self.__layout
end

function Modules.TemplPolytropicTypeChallengeItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)

    --TODO 不够图就随机
    local sp_res_list = {"1001","1002","1003","1004","1005","1006","1007","1008","1009","1010"}
    local random = MathTool.GetRandom(1, #sp_res_list)
    --贴图
    local bg_sp = Resource.PathTool.GetUIResPath("challenge_bg/"..sp_res_list[random])
    self.__bottom_bg = Util:Sprite(bg_sp)
    self.__bottom_bg:SetAnchorPoint(0.5,1)
    self.__layout:AddChild(self.__bottom_bg)
    PosTool.CenterTop(self.__bottom_bg,0,-30)
    
    --框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHALLENGE,"red")
    local frame = Util:Sprite(sp)
    self.__layout:AddChild(frame)
    PosTool.Center(frame)
    
    local config_system_entry = CPPGameLib.GetConfig("SystemEntry",self.__data.system_id,false)
    if not config_system_entry then
        return
    end

    --名字
    local name = Util:Label(config_system_entry.name,Macros.TypefaceSize.headlineII,Macros.Color.Special)
    name:SetAnchorPoint(0.5,0.5)
    frame:AddChild(name)
    PosTool.CenterBottom(name,0,60)

    --黑色遮罩
    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW,"black1_bg")
    local black1_bg_sp = Util:Sprite9(black1_bg_sp)
    black1_bg_sp:SetContentSize(self.__bottom_bg:GetContentWidth()+10,self.__bottom_bg:GetContentHeight()+10)
    self.__bottom_bg:AddChild(black1_bg_sp)
    PosTool.Center(black1_bg_sp,-3,0)
    black1_bg_sp:SetVisible(false)

    --锁
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHALLENGE, "challenge_lock")
    local lock_sp = Util:Sprite(sp)
    lock_sp:SetAnchorPoint(0.5,0.5)
    self.__layout:AddChild(lock_sp)
    PosTool.Center(lock_sp,0,20)
    lock_sp:SetVisible(false)

    --开启等级
    local open_txt = Util:Name(CPPGameLib.GetString("common_open_lev",config_system_entry.level),Macros.TypefaceSize.minimum)
    lock_sp:AddChild(open_txt)
    open_txt:SetAnchorPoint(0,0.5)
    PosTool.Center(open_txt,-5,0)
    open_txt:SetVisible(false)
end
