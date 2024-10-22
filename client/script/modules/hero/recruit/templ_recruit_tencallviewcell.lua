LogicGUI = LogicGUI or {}

LogicGUI.TemplRecruitTenCallViewCell = LogicGUI.TemplRecruitTenCallViewCell or BaseClass(GUI.Template)

function LogicGUI.TemplRecruitTenCallViewCell:__init(panel,hero_id,num)
	self:InitTemplate(GUI.TemplateRegistry.HERO_RECRUIT_TENCALLCELL,panel)
	self:SetData(hero_id,num)
end

function LogicGUI.TemplRecruitTenCallViewCell:SetData(hero_id,num)
	self.__hero_id = hero_id
	self.__hero_num = num
	local item_type,item_config = ObjIDTool.GetInfo(hero_id)
	local quality = item_type == Macros.Game.ObjType.HERO and item_config.init_quality or item_config.quality
	self.Panel_heroModule = self:GetWidget("Panel_heroModule")		
	self.heroName = self:GetWidget("Text_name")	
	self.heroName:SetTextColor(Macros.QualityColor(quality))
	self.heroName:SetText(item_config.name)
	self.heroName:SetOpacity(0)	
end

function LogicGUI.TemplRecruitTenCallViewCell:RunSpecialEffectAction(IsRun)
	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
	    self.__delay_timer = nil
	end
	local item_type,item_config = ObjIDTool.GetInfo(self.__hero_id)
	if not IsRun then
		local effect = nil	
		self.Effect_id, effect = self:__CreateEffect({id = 100023,loop = false}, self.Panel_heroModule,1)
		PosTool.Center(effect)	
		self.__deley_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
				self.__delay_timer = nil
				if item_type == Macros.Game.ObjType.HERO then
					self.__hero_module = LogicGUI.Model.New({info_id = self.__hero_id, parent = self.Panel_heroModule})
					self.__hero_module:SetPosition(2.5,2.5)
					--缩放比例
					self.__hero_module:SetScaleFactor(item_config.ui_scale*0.8 or 0.8)
				else
					--头像
				    local icon = LogicGUI.IconLayer.New(false,false)
				    icon:SetAnchorPoint(0.5,0)  
				    self.Panel_heroModule:AddChild(icon:GetNode())
				    PosTool.CenterBottom(icon,0,30)  
				    icon:SetData({id = self.__hero_id,num = self.__hero_num})
				end				
				CPPActionManager.FadeIn(self.heroName,0.5,nil)			
			end), 0.4)
	else
		self.__hero_module = LogicGUI.Model.New({ info_id = self.__hero_id, parent = self.Panel_heroModule})
		self.__hero_module:SetPosition(2.5,2.5)
		--缩放比例
		self.__hero_module:SetScaleFactor(item_config.ui_scale*0.8 or 0.8)
		self.heroName:SetOpacity(255)
	end
end

function LogicGUI.TemplRecruitTenCallViewCell:__delete()
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end
	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
	    self.__delay_timer = nil
	end
end