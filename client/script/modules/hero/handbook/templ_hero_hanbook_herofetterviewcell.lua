GUI = GUI or {}

GUI.TemplHeroHandBookHeroFetterViewCell = GUI.TemplHeroHandBookHeroFetterViewCell or BaseClass(GUI.ScrollItem)

function GUI.TemplHeroHandBookHeroFetterViewCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROFETTERVIEWCELL)
end

function GUI.TemplHeroHandBookHeroFetterViewCell:SetData(data)
	--头像
	local headPanel = self:GetWidget("Panel_itemIcon") 
	local icon = LogicGUI.IconLayer.New(false,false)
	headPanel:AddChild(icon:GetNode())
	icon:SetData({id = data.id})
	PosTool.Center(icon)
	icon:adaptParentContentSize()
	--名字
	local namePanel = self:GetWidget("Panel_name")  
	local nameChip = Util:Name(data.name,Macros.TypefaceSize.normal,data.quality)
	nameChip:SetAnchorPoint(0,0.5)
	namePanel:AddChild(nameChip)
	PosTool.LeftCenter(nameChip)
	--已拥有图片
	local alreadyHavedNum = ObjIDTool.GetOwnNum(data.id)
	self:GetWidget("Image_alreadyHaved"):SetVisible(alreadyHavedNum > 0)
	--描述
	Util:WidgetLabel(self:GetWidget("Text_des"),data.des ,Macros.TypefaceSize.minimum,Macros.Color.content,340,40)
	--获取按钮
	local Button_achieving = self:GetWidget("Button_achieving")
	WidgetUnity.SetWidgetListener(Button_achieving, nil, nil, function ()
		self:FireNextFrame(Macros.Event.HeroModule.HERO_HAND_BOOK_ADJUST_OPACITY,0)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, data.id)
	end, nil)
end

function GUI.TemplHeroHandBookHeroFetterViewCell:EnglishCustomization()
	--背景
	local Image_bg = self:GetWidget("Image_bg")
	Image_bg:SetContentSize( Image_bg:GetContentWidth() , Image_bg:GetContentHeight() + 20 )

	--图标
	local headPanel = self:GetWidget("Panel_itemIcon") 
	PosTool.MoveBy(headPanel, 0 , 20 )

	--名字
	local namePanel = self:GetWidget("Panel_name")  
	PosTool.MoveBy(namePanel, 0 , 20 )	

	--已拥有图片
	local Image_alreadyHaved = self:GetWidget("Image_alreadyHaved")
	PosTool.MoveBy(Image_alreadyHaved, 0 , 20 )

	--描述
	local Text_des = self:GetWidget("Text_des")
	Text_des:SetContentSize( Text_des:GetContentWidth() , Text_des:GetContentHeight() + 20 )
	PosTool.MoveBy(Text_des, 0 , 10 )

	--获取按钮
	local Button_achieving = self:GetWidget("Button_achieving")
	PosTool.MoveBy(Button_achieving, 0 , 20 )

end