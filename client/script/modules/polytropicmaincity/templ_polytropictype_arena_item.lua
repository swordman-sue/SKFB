Modules = Modules or {}

Modules.TemplPolytropicTypeArenaItem = Modules.TemplPolytropicTypeArenaItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPolytropicTypeArenaItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.ARENA_HEROMESSAGE_CELL)
end

function Modules.TemplPolytropicTypeArenaItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--位置容器
	self.__panel_pos = self:GetWidget("Panel_pos")
	--背景图片
	local image_bg_str = "rank_pillar_others"
	local image_Bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ARENA,image_bg_str)
	self:GetWidget("Image_bg"):SetImage(image_Bg)
	--英雄模型和名字
	self.__panel_forlead = self:GetWidget("Panel_forLead")
	self.__panel_forlead:SetVisible(false)

	--按钮
	self.__entry_btn = Util:Button(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
	self.__entry_btn:SetScaleFactor(2)
	self.__panel_pos:AddChild(self.__entry_btn)
	PosTool.CenterBottom(self.__entry_btn,0,400)
	self.__entry_btn:SetBright(true)

	--系统名字
	local system_name = Util:Label(self.__data.module_txt, Macros.TypefaceSize.headline, Macros.Color.Special)
	self.__panel_pos:AddChild(system_name)
	PosTool.CenterBottom(system_name,0,310)
	
	--隐藏排名信息
	local img_rankmessagebg = self:GetWidget("Image_rankMessageBg")
	img_rankmessagebg:SetVisible(false)
	--气泡
	self.__image_buddle = self:GetWidget("Image_buddle")
	self.__image_buddle:SetVisible(false)	
end

function Modules.TemplPolytropicTypeArenaItem:GetTouchNode()
	return self.__entry_btn
end

--获取位置容器
function Modules.TemplPolytropicTypeArenaItem:GetPanelPos()
	return self.__panel_pos
end
