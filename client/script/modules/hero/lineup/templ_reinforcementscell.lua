GUI = GUI or {}
GUI.TemplReinforcementsCell = GUI.TemplReinforcementsCell or BaseClass(GUI.Template)

function GUI.TemplReinforcementsCell:__init(uid,onFightHeroList,table,divination_type_list)
	self:InitTemplate(GUI.TemplateRegistry.HERO_REINFORCEMENTS_CELL)	
	self:createMainUI(uid,onFightHeroList,table,divination_type_list)
end

function GUI.TemplReinforcementsCell:__AddToParent()

end
function GUI.TemplReinforcementsCell:createMainUI(uid,onFightHeroList,equipTable,divination_type_list)
	if not uid or not onFightHeroList then
		return
	end
	local heroInfo = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",uid)
	local config_hero = CPPGameLib.GetConfig("Hero",heroInfo.info_id)
	--名字
	local text_title = self:GetWidget("Text_title")
	Util:WidgetLabel(text_title,"", Macros.TypefaceSize.popup, Macros.Quality[heroInfo.quality_level], 0, 0, Macros.Quality_Stroke[heroInfo.quality_level])	
	if heroInfo.break_level > 0 then
		text_title:SetText(CPPGameLib.GetString("common_add6", config_hero.name,heroInfo.break_level))
	else
		text_title:SetText(config_hero.name)
	end
	--宿命容器
	local Panel_reinforcements = self:GetWidget("Panel_reinforcements")	
	local totalNum = 0	

	print("skfb", heroInfo.info_id, config_hero.relation_hero_list, config_hero.relation_equip_list, config_hero.relation_treasure_list)

	--与人的宿命
	if config_hero.relation_hero_list then
		for i,relation_hero_list in ipairs(config_hero.relation_hero_list) do	
			local insertRelationHeroList = {}
			totalNum = 	totalNum + 1
			--宿命名字
			local IsMakeWork = true
			for heroIndex,heroId in ipairs(relation_hero_list) do				
				if not onFightHeroList[heroId] then
					IsMakeWork = false
					break
				end
			end
			insertRelationHeroList = CPPGameLib.CopyTblShallow(relation_hero_list)
			table.insert(insertRelationHeroList,1,heroInfo.info_id)		
			self["ReinforcementsCell_"..totalNum] = GUI.TemplHeroReinforcementsInfo.New(config_hero.relation_hero_reinforcementname_list[i],insertRelationHeroList,config_hero.relation_hero_reinforcementdes_list[i],IsMakeWork,equipTable)
			self["ReinforcementsCell_"..totalNum]:SetAnchorPoint(0,0)
			Panel_reinforcements:AddChild(self["ReinforcementsCell_"..totalNum]:GetNode())	
		end
	end
	--品阶是否改变
	local is_quality_change = heroInfo.quality_level > config_hero.init_quality
	--与装备的宿命
	local cur_relation_equip_list = is_quality_change and config_hero.relation_equip_ex_list or config_hero.relation_equip_list
	local relation_equip_reinforcementname_list = is_quality_change and config_hero.relation_equip_reinforcementname_ex_list or config_hero.relation_equip_reinforcementname_list
	local relation_equip_reinforcementdes_list = is_quality_change and config_hero.relation_equip_reinforcementdes_ex_list or config_hero.relation_equip_reinforcementdes_list
	if cur_relation_equip_list then
		for i,relation_equip_list in ipairs(cur_relation_equip_list) do	
			local insertRelationEquipList = {}
			totalNum = 	totalNum + 1
			local IsMakeWork = false
			for equipIndex,equipId in ipairs(relation_equip_list) do				
				if equipTable[equipId] then
					IsMakeWork = true
					break
				end
			end
			insertRelationEquipList = CPPGameLib.CopyTblShallow(relation_equip_list)
			table.insert(insertRelationEquipList,1,heroInfo.info_id)
			local count_num = 3
			--判断
			while #insertRelationEquipList >= count_num do	
				table.insert(insertRelationEquipList,count_num,"text")	
				count_num = count_num + 2
			end			
			self["ReinforcementsCell_"..totalNum] = GUI.TemplHeroReinforcementsInfo.New(relation_equip_reinforcementname_list[i],insertRelationEquipList,relation_equip_reinforcementdes_list[i],IsMakeWork,equipTable)
			self["ReinforcementsCell_"..totalNum]:SetAnchorPoint(0,0)
			Panel_reinforcements:AddChild(self["ReinforcementsCell_"..totalNum]:GetNode())		
		end
	end
	--与圣器的宿命
	local cur_relation_treasure_list = is_quality_change and config_hero.relation_treasure_ex_list or config_hero.relation_treasure_list
	local relation_treasure_reinforcementname_list = is_quality_change and config_hero.relation_treasure_reinforcementname_ex_list or config_hero.relation_treasure_reinforcementname_list
	local relation_treasure_reinforcementdes_list = is_quality_change and config_hero.relation_treasure_reinforcementdes_ex_list or config_hero.relation_treasure_reinforcementdes_list
	if cur_relation_treasure_list then
		for i,relation_treasure_list in ipairs(cur_relation_treasure_list) do	
			local insertRelationTreasureList = {}
			totalNum = totalNum + 1
			local IsMakeWork = false
			for treasureIndex,treasureId in ipairs(relation_treasure_list) do				
				if equipTable[treasureId] then
					IsMakeWork = true
					break
				end
			end
			insertRelationTreasureList = CPPGameLib.CopyTblShallow(relation_treasure_list)
			table.insert(insertRelationTreasureList,1,heroInfo.info_id)
			local count_num = 3
			--判断
			while #insertRelationTreasureList >= count_num do	
				table.insert(insertRelationTreasureList,count_num,"text")	
				count_num = count_num + 2
			end	
			self["ReinforcementsCell_"..totalNum] = GUI.TemplHeroReinforcementsInfo.New(relation_treasure_reinforcementname_list[i],insertRelationTreasureList,relation_treasure_reinforcementdes_list[i],IsMakeWork,equipTable)
			self["ReinforcementsCell_"..totalNum]:SetAnchorPoint(0,0)
			Panel_reinforcements:AddChild(self["ReinforcementsCell_"..totalNum]:GetNode())	
		end
	end
	--与卡牌的宿命
	if config_hero.relation_card_list then
		for i,relation_card_list in ipairs(config_hero.relation_card_list) do	
			local insertRelationCardList = {}
			totalNum = totalNum + 1
			local IsMakeWork = true
			for _,card_type in ipairs(relation_card_list) do				
				if not divination_type_list[card_type] then
					IsMakeWork = false
					break
				end
			end
			insertRelationCardList = CPPGameLib.CopyTblShallow(relation_card_list)
			table.insert(insertRelationCardList,1,heroInfo.info_id)	
			self["ReinforcementsCell_"..totalNum] = GUI.TemplHeroReinforcementsInfo.New(config_hero.relation_card_reinforcementname_list[i],insertRelationCardList,config_hero.relation_card_reinforcementdes_list[i],IsMakeWork,equipTable,divination_type_list,config_hero.recommend_card_list[i])
			self["ReinforcementsCell_"..totalNum]:SetAnchorPoint(0,0)
			Panel_reinforcements:AddChild(self["ReinforcementsCell_"..totalNum]:GetNode())	
		end
	end
	if self.ReinforcementsCell_1 then
		local panelHeight = self.ReinforcementsCell_1:GetNode():GetContentHeight() * totalNum
		Panel_reinforcements:SetContentSize(485,panelHeight)
		self:GetNode():SetContentSize(485,panelHeight+42)
	end
	Panel_reinforcements:SetPosition(0,0)
	--PosTool.LeftTop(Panel_reinforcements,0,-35)
	PosTool.LeftTop(self:GetWidget("Image_Top"),0,0)	
	for i=1,totalNum do
		--PosTool.LeftTop(self["ReinforcementsCell_"..i],0,-(panelHeight - (totalNum-i)*self["ReinforcementsCell_"..i]:GetNode():GetContentHeight()))
		self["ReinforcementsCell_"..i]:SetPosition(0,(totalNum-i)*self["ReinforcementsCell_"..i]:GetNode():GetContentHeight())	
		if i == totalNum then
			--分割线
			self["ReinforcementsCell_"..i]:GetWidget("Image_devLine"):SetVisible(false)
		end			
	end	
end

function GUI.TemplReinforcementsCell:__delete()
	
end