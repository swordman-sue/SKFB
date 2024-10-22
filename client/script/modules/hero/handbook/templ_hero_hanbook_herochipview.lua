GUI = GUI or {}

GUI.TemplHeroHandBookHeroChipView = GUI.TemplHeroHandBookHeroChipView or BaseClass(GUI.Template)

function GUI.TemplHeroHandBookHeroChipView:__init(id,hero_info)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROCHIPVIEW)
	self:createView(id,hero_info)
end

function GUI.TemplHeroHandBookHeroChipView:__AddToParent()
	
end

function GUI.TemplHeroHandBookHeroChipView:createView(id,hero_info)
	--获取英雄信息
	local heroInfo = CPPGameLib.GetConfig("Hero",id)
	--根据英雄获得对应的碎片信息,包括
	local chipRelationInfo = CPPGameLib.GetConfig("HeroCompound",id)
	--拿英雄碎片拿详细信息
	local chipDetailInfo = CPPGameLib.GetConfig("Item",chipRelationInfo.item_id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_chip"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--头像
	local headPanel = self:GetWidget("Panel_heroChipIcon") 
	local icon =  LogicGUI.IconLayer.New(false,false)
	headPanel:AddChild(icon:GetNode())
	icon:SetData({id = chipRelationInfo.item_id})
	PosTool.Center(icon)
	--名字
	local namePanel = self:GetWidget("Panel_name")  
	local nameChip = Util:Name(chipDetailInfo.name,Macros.TypefaceSize.normal,chipDetailInfo.quality)
	nameChip:SetAnchorPoint(0,0.5)
	namePanel:AddChild(nameChip)
	PosTool.LeftCenter(nameChip)
	--TODO	
	--碎片类型
	local Image_type = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON,"icon_profession_"..heroInfo.profession,true)
	self:GetWidget("Image_type"):SetImage(Image_type)  
	--碎片描述
	Util:WidgetLabel(self:GetWidget("Text_des"),chipDetailInfo.description, Macros.TypefaceSize.normal ,Macros.Color.content,383,45)
	--碎片数量
	Util:WidgetLabel(self:GetWidget("Text_num"),CPPGameLib.GetString("pack_num"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	local chipNum = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",chipRelationInfo.item_id)
	Util:WidgetLabel(self:GetWidget("Text_numCount"),chipNum.."/"..chipRelationInfo.item_num, Macros.TypefaceSize.normal, Macros.Color.content)
	--获取途径
	local ScrollViewPanel = self:GetWidget("Panel_heroChip")
	local params = {
		item_class	= Modules.TemplApproachOfAchievingCell,
	    item_width	= 530,
	    item_height	= 85,
	    row	= 3,
	    col	= 1,
	    item_space_r = 5,
	    view_width = 530,
	    view_height = 258,
	}
	self.ScrollView = GUI.ScrollView.New(params)
	ScrollViewPanel:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)
	self.ScrollViewTable = {}
	self.__approach_of_achieving_list = CallModuleFunc(ModuleType.APPROACH_OF_ACHIEVING,"HandleApproachOfAchievingData",chipRelationInfo.item_id)
	self.ScrollView:SetDataList(self.__approach_of_achieving_list) 
end

function GUI.TemplHeroHandBookHeroChipView:__delete()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end