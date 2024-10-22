LogicGUI = LogicGUI or {}

LogicGUI.TemplRecruitTenCallView = LogicGUI.TemplRecruitTenCallView or BaseClass(GUI.Template)


function LogicGUI.TemplRecruitTenCallView:__init(data,is_special)
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self.__is_special = is_special
		self:createMainUI(data)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_RECRUIT_TENCALLCELL)
	self:InitTemplate(GUI.TemplateRegistry.HERO_RECRUIT_TENCALL)
	
end

function LogicGUI.TemplRecruitTenCallView:createMainUI(data)
	--招募次数
	self.callTimes = ConfigAdapter.Common.GetHighRecruitMoreTimes()
	self.heroList = {}
	for i = 1,self.callTimes do
		table.insert(self.heroList,{info_id = data[i].info_id,num = data[i].num or 1})
	end
	self.__tenCallTime = 0
	--确定按钮
	Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("hero_awaken_buttonText4"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
	self.heroModuleList = {}
	self:GetWidget("Button_sure"):SetVisible(false)
	--招募继续事件
	self._More_Recruit_Continute_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_MORE_RECRUIT_CONTINUTE, function()
		if self.__tenCallTime == self.callTimes then
			self:GetWidget("Button_sure"):SetVisible(true)
		else			
			self:ShowUI()
		end
	end)
	--动态招募
	self:ShowUI()
end

function LogicGUI.TemplRecruitTenCallView:ShowUI()
	self.__tenCallTime = self.__tenCallTime + 1
	local panel = self:GetWidget("Panel_pos"..self.__tenCallTime)
	local item = LogicGUI.TemplRecruitTenCallViewCell.New(panel,self.heroList[self.__tenCallTime].info_id,self.heroList[self.__tenCallTime].num)
	table.insert(self.heroModuleList,item)
	local item_type,item_config = ObjIDTool.GetInfo(self.heroList[self.__tenCallTime].info_id)
	
	local is_run = false
	if item_type == Macros.Game.ObjType.HERO then
		if self.__is_special then
			is_run = item_config.init_quality >= Macros.Game.QualityType.ORANGE
		else
			is_run = item_config.init_quality >= Macros.Game.QualityType.PURPLE
		end
	end
	--是否播放特效
	item:RunSpecialEffectAction(is_run)
	if is_run then			
		if item_config.init_quality >= Macros.Game.QualityType.ORANGE then 	
			GlobalEventSystem:Fire(Macros.Event.HeroModule.HERO_RECRUIT_NOTICE,{item_config.name,item_config.init_quality})	
		end	
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.HERO_SINGLE_RECRUIT_VIEW, false, self.heroList[self.__tenCallTime].info_id,Macros.Game.HERO_RECRUIT.MORE_RECRUIT)
	else
		if self.__delay_timer then
			GlobalTimerQuest:CancelQuest(self.__delay_timer)
		    self.__delay_timer = nil
		end
		if self.__tenCallTime == self.callTimes then			
			self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
				self.__delay_timer = nil
				self:GetWidget("Button_sure"):SetVisible(true)				
			end), 0.5)	
		else
			self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
				self.__delay_timer = nil
				self:ShowUI()				
			end), 0.5)
		end		
	end
	
end

function LogicGUI.TemplRecruitTenCallView:__delete()
	if self.__hero_recruit_tenCalltimer then
		GlobalTimerQuest:CancelQuest(self.__hero_recruit_tenCalltimer)
		self.__hero_recruit_tenCalltimer = nil
	end

	if self.heroModuleList then
		for i,v in ipairs(self.heroModuleList) do
			v:DeleteMe()
		end
		self.heroModuleList = nil
	end

	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
	    self.__delay_timer = nil
	end
end
