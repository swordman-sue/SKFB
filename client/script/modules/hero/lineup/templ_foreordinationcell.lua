GUI = GUI or {}
GUI.TemplForeordinationCell = GUI.TemplForeordinationCell or BaseClass(GUI.Template)

function GUI.TemplForeordinationCell:__init(info,onFightHeroList,table)
	self.__is_auto_load = true
	self.__open_callback = function ( )
		self:createMainUI(info,onFightHeroList,table)
	end
	self:InitTemplate(GUI.TemplateRegistry.HERO_FOREORDINATIONCELL)	
end

function GUI.TemplForeordinationCell:__AddToParent()

end
function GUI.TemplForeordinationCell:createMainUI(info,onFightHeroList,equipTable)
	if not info or not onFightHeroList then
		return
	end
	print("info.info_idinfo.info_id: ",info.info_id)
	local heroInfo = CPPGameLib.GetConfig("Hero",info.info_id)
	local PanelTop = self:GetWidget("Panel_20")
	local PanelName = self:GetWidget("Panel_19")
	local lable = nil
	if info.break_level ~= 0 then 
		lable = Util:Name(heroInfo.name.." "..info.break_level,20,info.quality_level)
	else
		lable = Util:Name(heroInfo.name,20,info.quality_level)
	end
	PanelName:AddChild(lable)
	PosTool.Center(lable)
	local PanelText = self:GetWidget("Panel_16")
	local panelHeight = 0 
	local totalNum = 0
	self.sortTable = {}
	self.MakeWorkNum = 0
	--与人的宿命
	if heroInfo.relation_hero_list then
		for i,relation_hero_list in ipairs(heroInfo.relation_hero_list) do	
			totalNum = 	totalNum + 1
			--宿命名字
			local lab = "["..heroInfo.relation_hero_reinforcementname_list[i].."]"
			self["labelName_"..i] = LabelEx.CreateWithString(lab,Util.FontType,20)
			self["labelName_"..i]:SetTextWidthAndHeight(100,0)			
			self["labelName_"..i]:SetAnchorPoint(0,1)
			PanelText:AddChild(self["labelName_"..i])
			local IsMakeWork = true
			CPPGameLib.PrintTable(relation_hero_list,"relation_hero_list")
			self["heroName_"..i] = heroInfo.relation_hero_reinforcementdes_list[i]
			for heroIndex,heroId in ipairs(relation_hero_list) do
				print("heroIndex,heroId: ",heroIndex,heroId)				
				if not onFightHeroList[heroId] then
					IsMakeWork = false
					break
				end
			end
			if IsMakeWork then			
				table.insert(self.sortTable,totalNum - self.MakeWorkNum,totalNum)
				self.MakeWorkNum = self.MakeWorkNum + 1
			else
				table.insert(self.sortTable,1,totalNum)
			end		
		end
	end
	--人的属性加成
	--[[
	for i,relation_hero_pro_type_list in ipairs(heroInfo.relation_hero_pro_type_list) do
		for typeIndex,type in ipairs(relation_hero_pro_type_list) do
			if typeIndex ~= #relation_hero_pro_type_list then
				self["heroName_"..i] = self["heroName_"..i]..CPPGameLib.GetString("nature"..type)..CPPGameLib.GetString("enhanced_Text")..heroInfo.relation_hero_pro_value_list[i][typeIndex]..","
			else
				self["heroName_"..i] = self["heroName_"..i]..CPPGameLib.GetString("nature"..type)..CPPGameLib.GetString("enhanced_Text")..heroInfo.relation_hero_pro_value_list[i][typeIndex]
			end
		end
	end	--]]
	--装备的宿命
	local equipStartNum = totalNum
	--[[
	for i,equipId in ipairs(heroInfo.relation_equip_list) do
		totalNum = 	totalNum + 1
		--宿命名字
		local lab = "["..heroInfo.relation_equip_reinforcementname_list[i].."]"
		self["labelName_"..totalNum] = LabelEx.CreateWithString(lab,Util.FontType,20)
		self["labelName_"..totalNum]:SetTextWidthAndHeight(100,0)			
		self["labelName_"..totalNum]:SetAnchorPoint(0,1)
		PanelText:AddChild(self["labelName_"..totalNum])
		local equipInfo = CPPGameLib.GetConfig("Equipment",equipId)
		self["heroName_"..totalNum] = heroInfo.relation_equip_reinforcementdes_list[i]
		if equipTable[equipId] then			
			table.insert(self.sortTable,totalNum - self.MakeWorkNum,totalNum)
			self.MakeWorkNum = self.MakeWorkNum + 1
		else
			table.insert(self.sortTable,1,totalNum)
		end
	end --]]
	if heroInfo.relation_equip_list then
		for i,relation_equip_list in ipairs(heroInfo.relation_equip_list) do	
			totalNum = 	totalNum + 1
			--宿命名字
			local lab = "["..heroInfo.relation_equip_reinforcementname_list[i].."]"
			self["labelName_"..totalNum] = LabelEx.CreateWithString(lab,Util.FontType,20)
			self["labelName_"..totalNum]:SetTextWidthAndHeight(100,0)			
			self["labelName_"..totalNum]:SetAnchorPoint(0,1)
			PanelText:AddChild(self["labelName_"..totalNum])
			local IsMakeWork = true
			self["heroName_"..totalNum] = heroInfo.relation_equip_reinforcementdes_list[i]
			for equipIndex,equipId in ipairs(relation_equip_list) do				
				if not equipTable[equipId] then
					IsMakeWork = false
					break
				end
			end
			if IsMakeWork then			
				table.insert(self.sortTable,totalNum - self.MakeWorkNum,totalNum)
				self.MakeWorkNum = self.MakeWorkNum + 1
			else
				table.insert(self.sortTable,1,totalNum)
			end		
		end
	end
	--装备属性加成
	--[[
	for i,type in ipairs(heroInfo.relation_equip_pro_type_list) do
		equipStartNum = equipStartNum + 1
		self["heroName_"..equipStartNum] = self["heroName_"..equipStartNum]..CPPGameLib.GetString("nature"..type)..CPPGameLib.GetString("enhanced_Text")..heroInfo.relation_equip_pro_value_list[i]
	end --]]
	--圣器的宿命
	local treasureStartNum = totalNum
	--[[
	for i,treasureId in ipairs(heroInfo.relation_treasure_list) do
		totalNum = 	totalNum + 1
		local lab = "["..heroInfo.relation_treasure_reinforcementname_list[i].."]"
		self["labelName_"..totalNum] = LabelEx.CreateWithString(lab,Util.FontType,20)
		self["labelName_"..totalNum]:SetTextWidthAndHeight(100,0)			
		self["labelName_"..totalNum]:SetAnchorPoint(0,1)
		PanelText:AddChild(self["labelName_"..totalNum])
		self["heroName_"..totalNum] = CPPGameLib.GetString("equip_SomeThing")
		local treasureInfo = CPPGameLib.GetConfig("Treasure",treasureId)
		self["heroName_"..totalNum] = self["heroName_"..totalNum]..treasureInfo.name..","
		if equipTable[treasureId] then
			table.insert(self.sortTable,totalNum - self.MakeWorkNum,totalNum)
			self.MakeWorkNum = self.MakeWorkNum + 1			
		else
			table.insert(self.sortTable,1,totalNum)
		end
	end --]]
	if heroInfo.relation_treasure_list then
		for i,relation_treasure_list in ipairs(heroInfo.relation_treasure_list) do	
			totalNum = 	totalNum + 1
			--宿命名字
			local lab = "["..heroInfo.relation_treasure_reinforcementname_list[i].."]"
			self["labelName_"..totalNum] = LabelEx.CreateWithString(lab,Util.FontType,20)
			self["labelName_"..totalNum]:SetTextWidthAndHeight(100,0)			
			self["labelName_"..totalNum]:SetAnchorPoint(0,1)
			PanelText:AddChild(self["labelName_"..totalNum])
			local IsMakeWork = true
			self["heroName_"..totalNum] = heroInfo.relation_treasure_reinforcementdes_list[i]
			for treasureIndex,treasureId in ipairs(relation_treasure_list) do				
				if not equipTable[treasureId] then
					IsMakeWork = false
					break
				end
			end
			if IsMakeWork then			
				table.insert(self.sortTable,totalNum - self.MakeWorkNum,totalNum)
				self.MakeWorkNum = self.MakeWorkNum + 1
			else
				table.insert(self.sortTable,1,totalNum)
			end		
		end
	end
	--圣器属性加成
	--[[
	for i,type in ipairs(heroInfo.relation_treasure_pro_type_list) do
		treasureStartNum = treasureStartNum + 1
		self["heroName_"..treasureStartNum] = self["heroName_"..treasureStartNum]..CPPGameLib.GetString("nature"..type)..CPPGameLib.GetString("enhanced_Text")..heroInfo.relation_treasure_pro_value_list[i]
	end --]]
	--给文字上颜色并排序
	for i=1,totalNum do
		local label = LabelEx.CreateWithString(self["heroName_"..self.sortTable[i]],Util.FontType,20)
		label:SetTextWidthAndHeight(180,0)					
		label:SetAnchorPoint(0,1)
		PanelText:AddChild(label)
		panelHeight = panelHeight + label:GetContentHeight() + 5
		self["labelName_"..self.sortTable[i]]:SetPosition(12,panelHeight)
		label:SetPosition(130,panelHeight)
		if i >= (totalNum - self.MakeWorkNum + 1) then
			self["labelName_"..self.sortTable[i]]:SetColor(unpack(Macros.Color.orange))			
			label:SetColor(unpack(Macros.Color.orange))			
		else
			self["labelName_"..self.sortTable[i]]:SetColor(unpack(Macros.Color.keypoint))			
			label:SetColor(unpack(Macros.Color.keypoint))	
		end		
	end
	PanelText:SetContentSize(330,panelHeight)
	self:GetNode():SetContentSize(350,panelHeight+45)
	PosTool.CenterBottom(PanelText)
	PosTool.CenterTop(PanelTop,0,-5)
end

function GUI.TemplForeordinationCell:__delete()
	
end