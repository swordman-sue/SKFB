Modules = Modules or {}

Modules.TemplPolytropicTypeShopItem = Modules.TemplPolytropicTypeShopItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeShopItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeShopItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(285,214)
    return self.__layout
end

function Modules.TemplPolytropicTypeShopItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
   
    local config_system_entry = CPPGameLib.GetConfig("SystemEntry",self.__data.system_id,false)
    if not config_system_entry then
        return
    end

    --背景
    local sp_shop_btn = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SHOP_FAILARMY, "sp_shop_btn")
    local shop_btn = Util:Sprite(sp_shop_btn)
    self.__layout:AddChild( shop_btn )
    PosTool.Center(shop_btn)

    --按钮
    self.__entry_btn = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
    self.__entry_btn:SetScaleFactor(1.5)
    self.__layout:AddChild(self.__entry_btn)
    PosTool.Center(self.__entry_btn,0,-10)

    --名字
    local name = Util:Label(config_system_entry.name,Macros.TypefaceSize.headlineII,Macros.Color.golden,0,0,Macros.Color.common_stroke,2)
    name:SetAnchorPoint(0.5,1)
    self.__layout:AddChild(name)
    PosTool.CenterTop(name,0,-20)

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

    --开启等级
    --背景
    local bg_hero_name3 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_name3" )
    local hero_name = Util:Sprite( bg_hero_name3 )
    hero_name:SetPosition( shop_btn:GetContentWidth()/2 , shop_btn:GetContentHeight()/2 )
    self.__layout:AddChild( hero_name )    
    hero_name:SetVisible(false)
    --文字
    local system_foreshow_openlevel_text = Util:Label(CPPGameLib.GetString("system_foreshow_openlevel_text",config_system_entry.level), Macros.TypefaceSize.normal , Macros.Color.Special )
    system_foreshow_openlevel_text:SetAnchorPoint(0.5,0.5)
    self.__layout:AddChild(system_foreshow_openlevel_text)
    PosTool.Center(system_foreshow_openlevel_text) 
    system_foreshow_openlevel_text:SetVisible(false)
end

