GUI = GUI or {}

GUI.TemplPlunderedTreasuresDetailMessageView = GUI.TemplPlunderedTreasuresDetailMessageView or BaseClass(GUI.Template)

function GUI.TemplPlunderedTreasuresDetailMessageView:__init(treasureId)
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_TREASUREMESSAGE)
	self:createView(treasureId)
end

function GUI.TemplPlunderedTreasuresDetailMessageView:__AddToParent()
	
end

function GUI.TemplPlunderedTreasuresDetailMessageView:createView(treasureId)
	print("xy" , "=createView-----------")
	--获取宝物信息
	local treasureInfo = CPPGameLib.GetConfig("Treasure",treasureId)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_treasureMessgae"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--头像
	local Panel_heroIcon = self:GetWidget("Panel_heroIcon") 
	local icon =  LogicGUI.IconLayer.New(false,false)
	Panel_heroIcon:AddChild(icon:GetNode())
	icon:SetData({id = treasureId})
	PosTool.Center(icon)
	--名字
	Util:WidgetLabel(self:GetWidget("Text_name"),treasureInfo.name, Macros.TypefaceSize.largish,Macros.Quality[treasureInfo.quality],0,0,Macros.Quality_Stroke[treasureInfo.quality])	
	--类型
	Util:WidgetLabel(self:GetWidget("Text_type"),CPPGameLib.GetString("plunderedtreasures_treasure_type_"..treasureInfo.type),Macros.TypefaceSize.normal,Macros.Color.content)		 
	--描述
	Util:WidgetLabel(self:GetWidget("Text_des"),treasureInfo.description, Macros.TypefaceSize.normal,Macros.Color.content,420,44)
	--基本属性
	Util:WidgetLabel(self:GetWidget("Text_baseProperty"),CPPGameLib.GetString("plunderedtreasures_treasure_baseProperty"), Macros.TypefaceSize.largish,Macros.Color.btn)
	--每级强化添加
	Util:WidgetLabel(self:GetWidget("Text_addLevelProperty"),CPPGameLib.GetString("plunderedtreasures_treasure_addLevelProperty"), Macros.TypefaceSize.largish,Macros.Color.btn)
	
	for i=1,2 do
		self:GetWidget("Text_property"..i):SetVisible(false)
		self:GetWidget("Text_propertyValue"..i):SetVisible(false)
		self:GetWidget("Text_addProperty"..i):SetVisible(false)
		self:GetWidget("Text_addPropertyValue"..i):SetVisible(false)
	end

	for i,proInfo in ipairs(treasureInfo.strengthen_init_pro_list) do
		if proInfo[1] ~= 0 then
			self:GetWidget("Text_property"..i):SetVisible(true)
			self:GetWidget("Text_propertyValue"..i):SetVisible(true)
			local is_add_pro = true
			if not treasureInfo.strengthen_add_pro_list then
				is_add_pro = false
			end			
			self:GetWidget("Text_addProperty"..i):SetVisible(is_add_pro)
			self:GetWidget("Text_addPropertyValue"..i):SetVisible(is_add_pro)
			Util:WidgetLabel(self:GetWidget("Text_property"..i),CPPGameLib.GetString("nature"..treasureInfo.strengthen_init_pro_list[i][1])..CPPGameLib.GetString("common_colon"), Macros.TypefaceSize.normal,Macros.Color.keypoint)
			local name,value_str = Util:Nature(treasureInfo.strengthen_init_pro_list[i][1], treasureInfo.strengthen_init_pro_list[i][2], true)
			Util:WidgetLabel(self:GetWidget("Text_propertyValue"..i),value_str, Macros.TypefaceSize.normal,Macros.Color.content)
			--self:GetWidget("Text_propertyValue"..i):SetPosition(self:GetWidget("Text_property"..i):GetPositionX() + self:GetWidget("Text_property"..i):GetContentWidth() + 5,self:GetWidget("Text_propertyValue"..i):GetPositionY())
			if is_add_pro then
			local name,value_str = Util:Nature(treasureInfo.strengthen_add_pro_list[i][1], treasureInfo.strengthen_add_pro_list[i][2], true)
			end
			Util:WidgetLabel(self:GetWidget("Text_addProperty"..i),is_add_pro and (CPPGameLib.GetString("nature"..treasureInfo.strengthen_add_pro_list[i][1])..CPPGameLib.GetString("common_colon")) or "", Macros.TypefaceSize.normal,Macros.Color.keypoint)
			Util:WidgetLabel(self:GetWidget("Text_addPropertyValue"..i),is_add_pro and value_str or "", Macros.TypefaceSize.normal,Macros.Color.content)
			--self:GetWidget("Text_addPropertyValue"..i):SetPosition(self:GetWidget("Text_addProperty"..i):GetPositionX() + self:GetWidget("Text_addProperty"..i):GetContentWidth() + 5,self:GetWidget("Text_addPropertyValue"..i):GetPositionY())
		end
	end
	--获取途径
	Util:WidgetButton(self:GetWidget("Button_achieving"),"",function ()
		GlobalEventSystem:Fire(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW)
		LogicGUI.ApproachOfAchievingLayer.New(treasureId)		
	end)
	
end

function GUI.TemplPlunderedTreasuresDetailMessageView:__delete()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end