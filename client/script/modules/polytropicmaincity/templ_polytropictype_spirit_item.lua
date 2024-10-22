Modules = Modules or {}

Modules.TemplPolytropicTypeSpiritItem = Modules.TemplPolytropicTypeSpiritItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeSpiritItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeSpiritItem:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(90,80)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplPolytropicTypeSpiritItem:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	--选中框
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
	self.__image_selectedframe = Util:Sprite(sp)
	self.__layout:AddChild(self.__image_selectedframe,2)
	self.__image_selectedframe:SetVisible(false)
	PosTool.Center(self.__image_selectedframe)

	--图标
	local spirit_icon = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
	spirit_icon:SetScaleFactor(0.7)
	self.__layout:AddChild(spirit_icon)
	PosTool.Center(spirit_icon)
end

function Modules.TemplPolytropicTypeSpiritItem:SetSelected(var)
	self.__image_selectedframe:SetVisible(var)
end
