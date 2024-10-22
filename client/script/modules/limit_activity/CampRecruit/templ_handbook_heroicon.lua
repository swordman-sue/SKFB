Modules = Modules or {}
Modules.TemplCampRecruitHandBookHeroIcon = Modules.TemplCampRecruitHandBookHeroIcon or BaseClass(GUI.ScrollItem)

function Modules.TemplCampRecruitHandBookHeroIcon:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEADICON)
end

function Modules.TemplCampRecruitHandBookHeroIcon:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	local item_type,item_info = ObjIDTool.GetInfo(self.__data.reward_id)	
	local quality
	if item_type == Macros.Game.ObjType.HERO then
		quality = item_info.init_quality
	else
		quality = item_info.quality
	end
	local panelIcon = self:GetWidget("Panel_head")	
    --头像路径
	local icon = LogicGUI.IconLayer.New()
	icon:SetData({id = self.__data.reward_id,num = self.__data.reward_num})
	panelIcon:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:adaptParentContentSize()
    --底框的数据
    local frameData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_HANDBOOK, "quality_"..quality)
	local frame_image = self:GetWidget("Image_7")
	frame_image:SetImage(frameData)
    --名字
	local hero_name = self:GetWidget("Text_heroname")
	Util:WidgetLabel(hero_name, item_info.name, Macros.TypefaceSize.slightly,Macros.Color.btn)
	--热点英雄
	if self.__data.is_hot_hero then
	    local hot_hero_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_CHALLENGE, "hot",true)
		local hot_hero = Util:Sprite(hot_hero_sp)
		self:GetNode():AddChild(hot_hero)
		hot_hero:SetAnchorPoint(0,1)
		PosTool.LeftTop(hot_hero)
	end
end