GUI = GUI or {}

GUI.TemplHeroHandBookTogetherAttack = GUI.TemplHeroHandBookTogetherAttack or BaseClass(GUI.Template)

function GUI.TemplHeroHandBookTogetherAttack:__init(id,hero_info)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROTOGETHERATTACKVIEW)
	self:createMainUI(id)
end

function GUI.TemplHeroHandBookTogetherAttack:__AddToParent()

end

function GUI.TemplHeroHandBookTogetherAttack:createMainUI(id,hero_info)
	local heroInfo = CPPGameLib.GetConfig("Hero",id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_togetherAttacktitle"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--合击技能描述标题
	Util:WidgetLabel(self:GetWidget("Text_attackTogether"),CPPGameLib.GetString("hero_togetherAttackSkillText"), Macros.TypefaceSize.largish, Macros.Color.headline )
	--合击技能描述
	-- local skill_des_rich_text = Util:RichText("",Macros.TypefaceSize.minimum,520,0)
	-- skill_des_rich_text:SetAnchorPoint(0,1)
	-- self:GetNode():AddChild(skill_des_rich_text)
	-- skill_des_rich_text:SetPosition(10,319)
	for i,skillId in ipairs(hero_info and heroInfo.skill_ex_list or heroInfo.skill_list) do
		local skillInfo = CPPGameLib.GetConfig("Skill",skillId)
		local skillLevInfo = ConfigAdapter.Skill.GetSkillLevInfo(skillId)
		if skillInfo.type == Macros.Game.SkillType.JOINT then
			--skill_des_rich_text:SetText()
			local skill_des_rich_text = Util:RichText(CPPGameLib.GetString("hero_togetherAttack_des","["..skillInfo.name.."]：",skillLevInfo.des), Macros.TypefaceSize.minimum ,520,0,Macros.Color.content_hex)
			skill_des_rich_text:SetAnchorPoint(0,1)
			self:GetNode():AddChild(skill_des_rich_text)
			skill_des_rich_text:SetPosition(10,325)
			break
		end
	end
	--合击英雄标题
	Util:WidgetLabel(self:GetWidget("Text_togetherAttackHero"),CPPGameLib.GetString("hero_togetherAttack"), Macros.TypefaceSize.largish,Macros.Color.btn)
	--当前英雄的头像	
	local Panel_heroIcon = self:GetWidget("Panel_heroIcon")
	--监听当前头像
	WidgetUnity.SetWidgetListener(Panel_heroIcon,nil,nil,function ()
		self:FireNextFrame(Macros.Event.HeroModule.HERO_HAND_BOOK_ADJUST_OPACITY,0)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, id)
	end, nil)
	local icon = LogicGUI.IconLayer.New(false,false)
	Panel_heroIcon:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = id,obj_info = hero_info})
	--当前英雄名字
	local Panel_name = self:GetWidget("Panel_name")
	local name = Util:Name(heroInfo.name,18,hero_info and hero_info.quality_level or heroInfo.init_quality)
	Panel_name:AddChild(name)
	PosTool.Center(name)
	--合击英雄
	self:GetWidget("Image_add1"):SetVisible(false)
	self:GetWidget("Image_add2"):SetVisible(false)
	if heroInfo.joint_hero_show_list then
		for i,heroId in ipairs(heroInfo.joint_hero_show_list) do
			--加号可见
			self:GetWidget("Image_add"..i):SetVisible(true)
			--合击英雄头像
			local Panel_heroIcon = self:GetWidget("Panel_helperHeroIcon"..i)
			--监听合击英雄头像
			WidgetUnity.SetWidgetListener(Panel_heroIcon,nil,nil,function ()
				self:FireNextFrame(Macros.Event.HeroModule.HERO_HAND_BOOK_ADJUST_OPACITY,0)
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, heroId)
			end, nil)
			local icon = LogicGUI.IconLayer.New(false,false)
			Panel_heroIcon:AddChild(icon:GetNode())
			PosTool.Center(icon)
			icon:SetData({id = heroId})
			--合击英雄名字
			local heroHelperInfo = CPPGameLib.GetConfig("Hero",heroId)
			local Panel_name = self:GetWidget("Panel_helperHeroName"..i)
			local name = Util:Name(heroHelperInfo.name,18,heroHelperInfo.init_quality)
			Panel_name:AddChild(name)
			PosTool.Center(name)
		end
	end
	--点击英雄头像提示
	Util:WidgetLabel(self:GetWidget("Text_tip"),CPPGameLib.GetString("hero_touchIconTip"), Macros.TypefaceSize.slightly,Macros.Color.content)
end

function GUI.TemplHeroHandBookTogetherAttack:__delete()
	--TODO 删除heroModule
end