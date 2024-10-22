GUI = GUI or {}

GUI.TemplHeroHandBookMessageViewCell = GUI.TemplHeroHandBookMessageViewCell or BaseClass(GUI.Template)

function GUI.TemplHeroHandBookMessageViewCell:__init(panelType,id,hero_info)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROMESSAGECELL)

	self:SetTouchEnabled(false)	
	self:SetPanelData(panelType,id,hero_info)
end

function GUI.TemplHeroHandBookMessageViewCell:__AddToParent()
	
end

function GUI.TemplHeroHandBookMessageViewCell:SetPanelData(panelType,id,hero_info)
	local info = CPPGameLib.GetConfig("Hero",id)
	local panelHeight = 0
	--顶部题目背景
	self.Top = self:GetWidget("Image_titleBg")
	--设置标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_handbook_messageTitle_"..panelType), Macros.TypefaceSize.largish,Macros.Color.btn)
	--隐藏所有的技能
	self.Panel_Table = {}
	--普攻
	self.Panel_Type_1 = self:GetWidget("Panel_attack")
	self.Panel_Type_1:SetVisible(false)
	--技能
	self.Panel_Type_3 = self:GetWidget("Panel_skill")
	self.Panel_Type_3:SetVisible(false)
	--怒气
	self.Panel_Type_4 = self:GetWidget("Panel_angry")
	self.Panel_Type_4:SetVisible(false)
	--合击
	self.Panel_Type_5 = self:GetWidget("Panel_together")
	self.Panel_Type_5:SetVisible(false)
	--根据类型来创建
	if panelType == Macros.Game.HeroMessage_Type.SKILL then	
		panelHeight = 40
		for i,v in ipairs(hero_info and info.skill_ex_list or info.skill_list) do 
			local skillInfo = CPPGameLib.GetConfig("Skill",v)
			--CPPGameLib.PrintTable("ldx",skillInfo,"skillInfo: ")	
			local skillLevInfo = ConfigAdapter.Skill.GetSkillLevInfo(v)
			if skillInfo then
				if skillInfo.type ~= Macros.Game.SkillType.NEAR and skillInfo.type ~= Macros.Game.SkillType.JOINT_ATTACHED and 
					skillInfo.type ~= Macros.Game.SkillType.ANGER_PLUS and skillInfo.type ~= Macros.Game.SkillType.JOINT_PLUS then
					self.Panel_Table[skillInfo.type] = panelHeight
					self["Panel_Type_"..skillInfo.type]:SetVisible(true)
					local skill_desc = skillLevInfo and skillLevInfo.des or ""
					local richText = Util:RichText("["..skillInfo.name.."]".." "..skill_desc,Macros.TypefaceSize.slightly,440,0,Macros.Color.content_hex)
					richText:SetAnchorPoint(0,1)
					richText:SetPosition(48,30)
					self["Panel_Type_"..skillInfo.type]:AddChild(richText)	
					panelHeight = math.max(panelHeight + 45,panelHeight + richText:GetContentHeight() + 12)	
					if i == #info.skill_list then	
						panelHeight = panelHeight + 2				
					end
				end	
			end		
		end	
		--设置容器最终大小
		self:GetNode():SetContentSize(543,panelHeight)
		--重新设置顶部控件位置
		PosTool.LeftTop(self.Top,1,0)
		--重新设置各个文字位置
		for i=1,5 do
			if self.Panel_Table[i] then
				PosTool.LeftTop(self["Panel_Type_"..i],38,-self.Panel_Table[i])
				--self["Panel_Type_"..i]:SetPosition(36,)
			end
		end			
	elseif panelType == Macros.Game.HeroMessage_Type.REINFORCEMENT then
		panelHeight = 42
		local reinforcement_num = 0
		local ShowReinforcementname = function (reinforcementname_list,reinforcementdes_list)
			if reinforcementname_list then
				for i,reinforcementname in ipairs(reinforcementname_list) do
					reinforcement_num = reinforcement_num + 1
					local lab = "["..reinforcementname.."]".." "..reinforcementdes_list[i]
					local reinforcement =  LabelEx.CreateWithString(lab,Util.FontType,18)
					reinforcement:SetAnchorPoint(0,1)
					reinforcement:SetTextWidthAndHeight(440,0)			
					reinforcement:SetColor(unpack(Macros.Color.content))
					reinforcement:SetTag(reinforcement_num)
					self:GetNode():AddChild(reinforcement)
					self.Panel_Table[reinforcement_num] = panelHeight
					panelHeight = panelHeight + reinforcement:GetContentHeight() + 5			
				end	
			end
		end
		ShowReinforcementname(info.relation_hero_reinforcementname_list,info.relation_hero_reinforcementdes_list)
		ShowReinforcementname(hero_info and info.relation_equip_reinforcementname_ex_list or info.relation_equip_reinforcementname_list,
								hero_info and info.relation_equip_reinforcementdes_ex_list or info.relation_equip_reinforcementdes_list)
		ShowReinforcementname(hero_info and info.relation_treasure_reinforcementname_ex_list or info.relation_treasure_reinforcementname_list,
								hero_info and info.relation_treasure_reinforcementdes_ex_list or info.relation_treasure_reinforcementdes_list)
		ShowReinforcementname(info.relation_card_reinforcementname_list,info.relation_card_reinforcementdes_list)
		self:GetNode():SetContentSize(543,panelHeight)	
		--重新设置顶部控件位置
		PosTool.LeftTop(self.Top,1,0)
		--重新设置各个文字位置		
		for i=1,reinforcement_num do
			local label = self:GetNode():GetChildByTag(i)			
			PosTool.LeftTop(label,60, - self.Panel_Table[i])
		end	
	elseif panelType == Macros.Game.HeroMessage_Type.TALENT then
	else
	end
end
