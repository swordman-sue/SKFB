Modules = Modules or {}

Modules.TemplStrongerSystemCell = Modules.TemplStrongerSystemCell or BaseClass(GUI.ScrollItem)

function Modules.TemplStrongerSystemCell:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_STRONGER_SYSTEM_CELL)  
end

function Modules.TemplStrongerSystemCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	--系统图标
	local image_systemicon = self:GetWidget("Image_systemIcon")
	local sp = Resource.PathTool.GetSystemEntryIconPath(self.__data.icon)
	image_systemicon:IgnoreContentAdaptWithSize(true)
	image_systemicon:SetImage(sp,TextureResType.LOCAL)
	--推荐图标
	self:GetWidget("Image_recommendTag"):SetVisible(self.__data.is_recommend)
	--系统名字
	Util:WidgetLabel(self:GetWidget("Text_systemName"),self.__data.text, Macros.TypefaceSize.popup, Macros.Color.keypoint)
	--进度条	
	self.__loading_bar = self:GetWidget("LoadingBar")
	self.__loading_bar:SetPercent(self.__data.percent)
	--进度条文字
	Util:WidgetLabel(self:GetWidget("Text_progressNum"),self.__data.cur_value.."/"..self.__data.limit_value..self.__data.link_txt, Macros.TypefaceSize.normal, Macros.Color.btn)
end

