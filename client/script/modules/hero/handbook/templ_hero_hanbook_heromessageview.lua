GUI = GUI or {}

GUI.TemplHeroHandBookMessageView = GUI.TemplHeroHandBookMessageView or BaseClass(GUI.Template)

function GUI.TemplHeroHandBookMessageView:__init(id,hero_info)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROMESSAGEVIEW)

	self:SetTouchEnabled(false)
	self:createView(id,hero_info)
end

function GUI.TemplHeroHandBookMessageView:__AddToParent()

end

function GUI.TemplHeroHandBookMessageView:createView(id,hero_info)
	--获取英雄信息
	local info = CPPGameLib.GetConfig("Hero",id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_info"), Macros.TypefaceSize.popup,Macros.Color.btn)	
	--头像
	local iconPanel = self:GetWidget("Panel_heroIcon") 
	local icon = LogicGUI.IconLayer.New(false,false)
	iconPanel:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = id,obj_info = hero_info})
	--名字
	local txt_heroname = self:GetWidget("Text_heroname")
	txt_heroname:SetText(info.name)
	txt_heroname:SetColor(unpack(Macros.Quality[hero_info and hero_info.quality_level or info.init_quality]))
	txt_heroname:SetStrokeColor(unpack(Macros.Quality_Stroke[hero_info and hero_info.quality_level or info.init_quality]))
	--英雄资质
	local txt_intelligence = self:GetWidget("Text_intelligence")
	local qualifications = hero_info and ConfigAdapter.Hero.GetHeroQualifications(hero_info.quality_level,hero_info.info_id,hero_info.quality_stage) or info.Qualifications
	Util:WidgetLabel(txt_intelligence,CPPGameLib.GetString("hero_intelligence",qualifications), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	txt_intelligence:SetPosition(txt_heroname:GetPositionX()+txt_heroname:GetContentWidth()+10,txt_heroname:GetPositionY())
	--类型
	local Image_type = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON,"icon_profession_"..info.profession,true)
	self:GetWidget("Image_type"):SetImage(Image_type)  
	--英雄描述
	--TODO
	Util:WidgetLabel(self:GetWidget("Text_des"),info.describe, Macros.TypefaceSize.normal ,Macros.Color.content,423,42)
	--去获取按钮
	local Button_achieving = self:GetWidget("Button_achieving")
	WidgetUnity.SetWidgetListener(Button_achieving, nil, nil, function ()
		self:FireNextFrame(Macros.Event.HeroModule.HERO_HAND_BOOK_ADJUST_OPACITY,0)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, id)
	end, nil)
	--获取列表容器
	local listView = self:GetWidget("ListView_heroMessage")
	listView:SetTouchSwallow(false)
	--TODO 判断是否创建模块
	local item = GUI.TemplHeroHandBookMessageViewCell.New(Macros.Game.HeroMessage_Type.SKILL,id,hero_info)
	listView:PushBackCustomItem(item:GetNode())
	if info.type ~= Macros.Game.HeroType.EXP then
		item = GUI.TemplHeroHandBookMessageViewCell.New(Macros.Game.HeroMessage_Type.REINFORCEMENT,id,hero_info)
		listView:PushBackCustomItem(item:GetNode())
	end
	--[[
	item = GUI.TemplHeroHandBookMessageViewCell.New(Macros.Game.HeroMessage_Type.TALENT,id)
	listView:PushBackCustomItem(item:GetNode()) --]]
end

function GUI.TemplHeroHandBookMessageView:__delete()
	
end