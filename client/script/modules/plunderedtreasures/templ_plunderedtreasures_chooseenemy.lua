Modules = Modules or {}

Modules.TemplPlunderedTreasuresChooseenemyCell = Modules.TemplPlunderedTreasuresChooseenemyCell or BaseClass(GUI.ScrollItem)

function Modules.TemplPlunderedTreasuresChooseenemyCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_FIGHTENEMY)
end

function Modules.TemplPlunderedTreasuresChooseenemyCell:SetData(data)
	--CPPGameLib.PrintTable("ldx",data,"TemplPlunderedTreasuresChooseenemyCellTemplPlunderedTreasuresChooseenemyCell ")
	self.__data = data
	--玩家名字
	Util:WidgetLabel(self:GetWidget("Text_playerName"),data.role_name,Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--玩家等级
	Util:WidgetLabel(self:GetWidget("Text_level"),CPPGameLib.GetString("plunderedtreasures_playerLevel",data.level),Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--玩家战力
	Util:WidgetLabel(self:GetWidget("Text_battleValue"),CPPGameLib.GetString("plunderedtreasures_playerBattleValue"),Macros.TypefaceSize.normal, Macros.Color.content)
	--玩家战力数值
	Util:WidgetLabel(self:GetWidget("Text_battleValueNum"),data.fighting_power,Macros.TypefaceSize.normal, Macros.Color.content)
	--夺取概率
	self.Text_probability = self:GetWidget("Text_probability")	
	Util:WidgetLabel(self.Text_probability,"",Macros.TypefaceSize.normal)
	local treasureInfo = CPPGameLib.GetConfig("Treasure",data.treasure_info_id)
	local dropInfo = CPPGameLib.GetConfig("DuobaoDrop",treasureInfo.quality)	
	--是否解除免战状态回调
	self.__plunderedTreasuresOpenWarEvent = self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_OPENWAR, function(_,type)
	 	if self.IsCurCellCallFight then
		 	CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresDataReq")
	      	if type == 1 then
	      		self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW)
	      	
	      		local checkBattle = CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"CheckBattleCondition" , Macros.Game.FBType.PLUNDERED_TREASURES )
	      		if checkBattle then
		      		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.PLUNDERED_TREASURES,data.role_id,1,data.treasure_info_id,data.treasure_item_id)
	      		end

	      	elseif type == 2 then
	      		self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW)
	      		CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
	      		CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "OpenView", BaseViewType.QUICK_PLUNDEREDTREASURES,self.__data)		
	      	else
	      		CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
	      		CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresFiveTimesReq")
	      	end
	      	self.IsCurCellCallFight = false
        end
    end)
    --三个按钮
    self.PlayerFightButton = self:GetWidget("Button_center")
    self.RobotFightButton = self:GetWidget("Button_bottom")
    self.RobotFightFiveTimesButton = self:GetWidget("Button_up")
    self.RobotFightButton:SetVisible(false)
	self.RobotFightFiveTimesButton:SetVisible(false)
	self.PlayerFightButton:SetVisible(false)
	--检查是否开启夺宝5次功能
	local message_list = ConfigAdapter.Common.GetPlunderedTreasureFiveTimesMessageList()
	local is_open,is__pre_open = self:CheckIsOpenPlunderedTreasuresFiveTimes(message_list)	
	--抢夺玩家
	self.PlayerFightButton:SetTouchSwallow(false)
	Util:WidgetButton(self.PlayerFightButton,CPPGameLib.GetString("plunderedtreasures_playerPlunder"),function ()
		self:FightButtonCallBack(1)
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_red )
	--抢夺机器人一次	
	self.RobotFightButton:SetTouchSwallow(false)
	Util:WidgetButton(self.RobotFightButton,CPPGameLib.GetString("plunderedtreasures_playerPlunder"),function ()
		self:FightButtonCallBack(2)
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_yellow )
	--抢夺机器人五次	
	self.RobotFightFiveTimesButton:SetTouchSwallow(false)
	Util:WidgetButton(self.RobotFightFiveTimesButton,CPPGameLib.GetString("plunderedtreasures_playerPlunderFiveTimes"),function ()
		if not is_open and is__pre_open then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_train_times_conditional",message_list[2],message_list[3]))
			return
		end
		self:FightButtonCallBack(3)	
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_red )
	--英雄头像列表
	local heroIconListView = self:GetWidget("ListView_playerHeroList")
	for i,heroId in ipairs(data.hero_list) do
		if heroId ~= 0 then
			local layout = Util:Layout()
			layout:SetTouchEnabled(true)
			layout:SetContentSize(80,80)
			layout:SetTouchSwallow(false)
			local icon = LogicGUI.IconLayer.New(false,false)
			layout:AddChild(icon:GetNode())
			PosTool.Center(icon)
			icon:SetData({id = heroId})
			icon:adaptParentContentSize()
			heroIconListView:PushBackCustomItem(layout)
			WidgetUnity.SetWidgetListener(layout,function ()
				self.__could_callback = true
			end,function ()
				self.__could_callback = false
			end,function ()
				if self.__could_callback then
					CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK_DETAIL,heroId)
				end
			end,nil)
		end
	end
	--根据类型来判断
	if data.target_type == Modules.PlunderedTreasuresModule.TargetType.PLAYER then
		self.Text_probability:SetText(dropInfo.user_rate_desc)
		self.Text_probability:SetColor(unpack(Macros.Quality[dropInfo.user_rate_color]))
		self.PlayerFightButton:SetVisible(true)
	else
		self.Text_probability:SetText(dropInfo.rate_desc)
		self.Text_probability:SetColor(unpack(Macros.Quality[dropInfo.rate_color]))		
		self.RobotFightButton:SetVisible(true)
		self.RobotFightFiveTimesButton:SetVisible(true)
		if not is_open and is__pre_open then
			self.RobotFightFiveTimesButton:SetBright(false)
			self.RobotFightFiveTimesButton:SetTextColor(unpack(Macros.Color.forbidden))
			local r, g, b, a = unpack( Macros.Color.forbidden_stroke )
			self.RobotFightFiveTimesButton:SetOutline(r, g, b, a, 2)
		elseif not is_open and not is__pre_open  then
			self.RobotFightFiveTimesButton:SetVisible(false)
			self.RobotFightButton:SetPosition(self.PlayerFightButton:GetPositionX(),self.PlayerFightButton:GetPositionY())
		end				
	end
end

function Modules.TemplPlunderedTreasuresChooseenemyCell:CheckIsOpenPlunderedTreasuresFiveTimes(message_list)
	local is_open,is__pre_open = false
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local vip_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)
	if role_level >= message_list[3] or vip_level >= message_list[2] then
		is_open = true
	end
	if role_level >= message_list[1] then
		is__pre_open = true
	end
	return is_open,is__pre_open
end

--判断是否在免战
function Modules.TemplPlunderedTreasuresChooseenemyCell:CheckIsAvoidWar()
	local overTime = CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"GetAvoidWarOverTime")
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	return overTime > severTime 
end

--挑战按钮回调
function Modules.TemplPlunderedTreasuresChooseenemyCell:FightButtonCallBack(type)
	local eneryNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY)
	if eneryNum < ConfigAdapter.Common.GetPlunderedTreasureCostEnery() then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_eneryNoEnoughTips"))
		LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.ENERGY )
		return
	end

	--设置选中的沙包
	CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"SetOnPlaye",self.__data)

	local IsAvoidWar = self:CheckIsAvoidWar()
	if IsAvoidWar then
		self.IsCurCellCallFight = true
		LogicGUI.TemplPlunderedTreasuresRelieveStateTipView.New(type)
	else
		if type == 1 then
			self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW)
		
			local checkBattle = CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"CheckBattleCondition" , Macros.Game.FBType.PLUNDERED_TREASURES )
      		if checkBattle then
				CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.PLUNDERED_TREASURES,self.__data.role_id,1,self.__data.treasure_info_id,self.__data.treasure_item_id)
			end
			
		elseif type == 2 then
			self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW)
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "OpenView", BaseViewType.QUICK_PLUNDEREDTREASURES,self.__data)
		else
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresFiveTimesReq")
		end		
	end	
end

--新手指引特殊操作检测
function Modules.TemplPlunderedTreasuresChooseenemyCell:CheckSectionOperateSP()
	--圣器争夺-敌人列表-抢夺（第一个机器人）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_ENEMYLIST_GRAB, self.RobotFightButton)				
end

function Modules.TemplPlunderedTreasuresChooseenemyCell:__delete()

end
