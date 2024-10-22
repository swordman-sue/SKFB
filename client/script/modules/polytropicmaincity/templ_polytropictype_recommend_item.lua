Modules = Modules or {}

Modules.TemplPolytropicTypeRecommendItem = Modules.TemplPolytropicTypeRecommendItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeRecommendItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeRecommendItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(225,88)
    return self.__layout
end

function Modules.TemplPolytropicTypeRecommendItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
   
    --背景
    local sp_btn = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,"btn_btn_activity_no")
    self.__sp_btn = Util:Sprite(sp_btn)
    self.__sp_btn:SetAnchorPoint(0,0.5)
    self.__layout:AddChild(self.__sp_btn)
    PosTool.LeftCenter(self.__sp_btn)

    --按钮
    local system_icon = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
    system_icon:SetScaleFactor(0.7)
    system_icon:SetAnchorPoint(0,0.5)
    self.__sp_btn:AddChild(system_icon)
    PosTool.LeftCenter(system_icon,0,0)

    --文字
    local label = Util:Label(self.__data.module_txt or "", Macros.TypefaceSize.headline, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
    label:SetAnchorPoint(0,0.5)
    self.__sp_btn:AddChild(label)
    PosTool.LeftCenter(label,85,0)
end

function Modules.TemplPolytropicTypeRecommendItem:SetSelected(var)
    local sp_btn = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,var and "btn_btn_activity_on" or "btn_btn_activity_no")
    self.__sp_btn:SetImage(sp_btn)
end


