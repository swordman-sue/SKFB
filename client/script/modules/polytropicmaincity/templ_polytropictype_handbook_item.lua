Modules = Modules or {}
Modules.TemplPolytropicTypeHandBookItem = Modules.TemplPolytropicTypeHandBookItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeHandBookItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEADICON)
end

function Modules.TemplPolytropicTypeHandBookItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)


    --按钮
    self.__entry_btn = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
    self.__entry_btn:SetScaleFactor(1)
    self:GetNode():AddChild(self.__entry_btn)
    PosTool.Center(self.__entry_btn,0,20)

    --底框的数据
    local random_quality = MathTool.GetRandom(1, 5)
    local frameData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_HANDBOOK, "quality_"..random_quality)
	local frame_image = self:GetWidget("Image_7")
	frame_image:SetImage(frameData)
    --名字
	local hero_name = self:GetWidget("Text_heroname")
	Util:WidgetLabel(hero_name,self.__data.module_txt, Macros.TypefaceSize.slightly,Macros.Color.btn)	
end