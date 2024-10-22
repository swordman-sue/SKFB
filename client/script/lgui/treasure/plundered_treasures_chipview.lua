GUI = GUI or {}

GUI.TemplPlunderedTreasureChipView = GUI.TemplPlunderedTreasureChipView or BaseClass(GUI.Template)

function GUI.TemplPlunderedTreasureChipView:__init(pieceId)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROCHIPVIEW)
	self:createView(pieceId)
end

function GUI.TemplPlunderedTreasureChipView:__AddToParent()
	
end

function GUI.TemplPlunderedTreasureChipView:createView(pieceId)
	--获取圣器碎片信息
	local treasureInfo = CPPGameLib.GetConfig("Item",pieceId)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("info_debris"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--头像
	local headPanel = self:GetWidget("Panel_heroChipIcon") 
	local icon =  LogicGUI.IconLayer.New(false,false)
	headPanel:AddChild(icon:GetNode())
	icon:SetData({id = pieceId})
	PosTool.Center(icon)	
	--名字
	local namePanel = self:GetWidget("Panel_name")  
	local nameChip = Util:Name(treasureInfo.name,Macros.TypefaceSize.normal,treasureInfo.quality)
	nameChip:SetAnchorPoint(0,0.5)
	namePanel:AddChild(nameChip)
	PosTool.LeftCenter(nameChip)
	namePanel:SetPosition(99,342)
	--TODO	
	--碎片类型
	self:GetWidget("Image_type"):SetVisible(false)
	--碎片描述
	Util:WidgetLabel(self:GetWidget("Text_des"),treasureInfo.description, Macros.TypefaceSize.normal,Macros.Color.content,383,45)
	--碎片数量
	Util:WidgetLabel(self:GetWidget("Text_num"),CPPGameLib.GetString("plunderedtreasures_treasure_haveNum"), Macros.TypefaceSize.normal,Macros.Color.keypoint)
	local chipNum = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",pieceId)
	Util:WidgetLabel(self:GetWidget("Text_numCount"),chipNum,Macros.TypefaceSize.normal,Macros.Color.content)
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
	    touchable = true,
    	touch_callback = function (index,data)
    		-- body
    	end,
	}
	self.ScrollView = GUI.ScrollView.New(params)
	ScrollViewPanel:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)
	self.ScrollViewTable = {}
	self.__approach_of_achieving_list = CallModuleFunc(ModuleType.APPROACH_OF_ACHIEVING,"HandleApproachOfAchievingData",pieceId)
	self.ScrollView:SetDataList(self.__approach_of_achieving_list)
end

function GUI.TemplPlunderedTreasureChipView:__delete()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end