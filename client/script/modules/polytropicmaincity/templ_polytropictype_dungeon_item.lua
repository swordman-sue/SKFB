Modules = Modules or {}

Modules.TemplPolytropicTypeDungeonItem = Modules.TemplPolytropicTypeDungeonItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeDungeonItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeDungeonItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(214,400)
    return self.__layout
end

function Modules.TemplPolytropicTypeDungeonItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
   
    local config_system_entry = CPPGameLib.GetConfig("SystemEntry",self.__data.system_id,false)
    if not config_system_entry then
        return
    end

    --按钮
    self.__entry_btn = Util:Button(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
    self.__entry_btn:SetTouchEnabled(false)
    self.__entry_btn:SetScaleFactor(2)
    self.__layout:AddChild(self.__entry_btn)
    PosTool.Center(self.__entry_btn)
    self.__entry_btn:SetBright(true)

    --名字
    local name = Util:Label(config_system_entry.name,Macros.TypefaceSize.headlineII,Macros.Color.Special)
    name:SetAnchorPoint(0.5,0.5)
    self.__layout:AddChild(name)
    PosTool.CenterBottom(name,0,80)

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

    --开启等级
    local open_txt = Util:Name(CPPGameLib.GetString("common_open_lev",config_system_entry.level),Macros.TypefaceSize.headlineII,Macros.Game.QualityType.RED)
    self.__layout:AddChild(open_txt)
    open_txt:SetAnchorPoint(0.5,0)
    PosTool.CenterBottom(open_txt,0,20)
    open_txt:SetVisible(false)
end

--获取
function Modules.TemplPolytropicTypeDungeonItem:GetImgNormal()
    return self.__layout
end

function Modules.TemplPolytropicTypeDungeonItem:IsTouchEnable()
    if not GUI.ScrollItem.IsTouchEnable(self) then
        return false
    end
    return true
end
