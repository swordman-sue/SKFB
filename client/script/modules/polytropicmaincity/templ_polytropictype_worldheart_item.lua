Modules = Modules or {}

Modules.TemplPolytropicTypeWorldHeartItem = Modules.TemplPolytropicTypeWorldHeartItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeWorldHeartItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPolytropicTypeWorldHeartItem:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(120,120)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplPolytropicTypeWorldHeartItem:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	--精灵球底
	local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "bg_spirit_crystal")
	self.__spirit_bg = Util:Sprite(image_data)
	self.__layout:AddChild(self.__spirit_bg)
	PosTool.Center(self.__spirit_bg)
	
	--精灵球
	self.__spirit_ball = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
	self.__spirit_ball:SetScaleFactor(0.7)
	self.__layout:AddChild(self.__spirit_ball)
	PosTool.Center(self.__spirit_ball)

	--精灵球名字
	self.__spirit_name = Util:Label(self.__data.module_txt or "", Macros.TypefaceSize.largish, Macros.Color.blinkcontent, 0, 0, Macros.Color.common_stroke)
	self.__spirit_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
	self.__layout:AddChild(self.__spirit_name)
	PosTool.Center(self.__spirit_name,0,-20)	
end

function Modules.TemplPolytropicTypeWorldHeartItem:SetSelected(var)
	if var then
		if not self.__selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "selected_frame")
		    self.__selected_image = Util:Sprite(sp)
		    self.__layout:AddChild(self.__selected_image, 1)
		    PosTool.Center(self.__selected_image)
		end
	else
		if self.__selected_image then
			self.__selected_image:RemoveSelf(true)
			self.__selected_image = nil
		end
	end
end