Modules = Modules or {}
Modules.TemplHandBookHeroIcon = Modules.TemplHandBookHeroIcon or BaseClass(GUI.ScrollItem)

function Modules.TemplHandBookHeroIcon:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEADICON)
end

function Modules.TemplHandBookHeroIcon:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	self:GetNode():SetTouchEnabled(false)
	local info = CPPGameLib.GetConfig("Hero",data.hero_id)
	local panelIcon = self:GetWidget("Panel_head")	
	--底图的数据
	-- local baseMapData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_HANDBOOK, "qualitybg_"..info.init_quality)	
	-- --底图
	-- if not self.__sp_baseMap then
 --      self.__sp_baseMap = SpriteEx.CreateWithImageFile(baseMapData)
 --      panelIcon:AddChild(self.__sp_baseMap)
 --      PosTool.Center(self.__sp_baseMap)
 --    else
 --      self.__sp_baseMap:SetImage(baseMapData)
 --    end
 --    --遮罩
	-- local spiteClipping = SpriteEx.CreateWithImageFile(baseMapData)
	-- self.clipping_node = ClippingNodeEx.Node()
	-- self.clipping_node:SetAlphaTestValue(0)
	-- self.clipping_node:SetStencilMask(spiteClipping)
	-- panelIcon:AddChild(self.clipping_node)
	-- PosTool.Center(self.clipping_node)
    --头像路径
	local icon = LogicGUI.IconLayer.New(true,true,false)
	icon:SetData({id = data.hero_id})
	panelIcon:AddChild(icon:GetNode())
	PosTool.Center(icon)
	-- icon:SetTouchSwallow(true)
	icon:adaptParentContentSize()
    --底框的数据
    local frameData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_HANDBOOK, "quality_"..info.init_quality)
	local frame_image = self:GetWidget("Image_7")
	frame_image:SetImage(frameData)
    --名字
	local hero_name = self:GetWidget("Text_heroname")
	Util:WidgetLabel(hero_name, info.name, Macros.TypefaceSize.slightly,data.IsGray and Macros.Color.Gray or Macros.Color.btn)
	--设灰
	icon:SetGray(data.IsGray)
	frame_image:SetGray(data.IsGray)
end