Modules = Modules or {}

Modules.TemplAcceptPrizeCenterCell = Modules.TemplAcceptPrizeCenterCell or BaseClass(GUI.ScrollItem)

function Modules.TemplAcceptPrizeCenterCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACCEPT_PRIZE_CENTER_CELL)
end

function Modules.TemplAcceptPrizeCenterCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	local reward_config = CPPGameLib.GetConfig("RewardCenter",data.type)
	--标题	
	Util:WidgetLabel(self:GetWidget("Text_name"),reward_config.title,Macros.TypefaceSize.normal,Macros.Color.headline)
	--时间	
	local timeText = self:GetWidget("Text_time")
	Util:WidgetLabel(timeText,"",Macros.TypefaceSize.normal,Macros.Color.keypoint)
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

	local last_time = server_time - data.time

	local min = math.floor(last_time/60)
	if min == 0 then
		min = 1
	end
	if min < 60 then
		timeText:SetText(CPPGameLib.GetString("friend_timeMinText",min))
	elseif min >=60 and min < 1440 then
		timeText:SetText(CPPGameLib.GetString("friend_timeHourText",math.floor(min/60)))
	elseif min >= 1440 and  min < 10080 then
		timeText:SetText(CPPGameLib.GetString("friend_timeDayText",math.floor(min/1440)))
	elseif min >= 10080 and  min < 40320 then
		timeText:SetText(CPPGameLib.GetString("accept_prize_center_timeWeekText"))
	elseif min >= 40320  then
		timeText:SetText(CPPGameLib.GetString("accept_prize_center_timeMonthText"))
	end

	--领取
	Util:WidgetButton(self:GetWidget("Button_get"),CPPGameLib.GetString("accept_prize_center_getReawardText"),function ()
		if #self.__data.dataList ~= 0 then
			local info = {}
			local rewardData = {}
			rewardData.id = self.__data.id
			table.insert(info,rewardData)
			CallModuleFunc(ModuleType.ACCEPT_PRIZE_CENTER,"RewardCenterGetReq",info,self.__index)								
		else
		end
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_red)
	--描述
	if reward_config.reward_type == 16 then	--王者处理		
		local ladder_dan = CPPGameLib.GetConfig("LadderDan",data.value)
		Util:WidgetLabel(self:GetWidget("Text_des"),string.format(reward_config.desc,ladder_dan.name or ""),Macros.TypefaceSize.minimum,Macros.Color.content)
	elseif reward_config.reward_type == 19 or reward_config.reward_type == 20 then	--资源战处理
		local act_value = data.value%10000
		local config_res = CPPGameLib.GetConfig("ResourceWarResource",act_value)
		Util:WidgetLabel(self:GetWidget("Text_des"),string.format(reward_config.desc,config_res.name or ""),Macros.TypefaceSize.minimum,Macros.Color.content)
	elseif reward_config.param_flag == 0 then	--无参数
		Util:WidgetLabel(self:GetWidget("Text_des"),reward_config.desc,Macros.TypefaceSize.minimum,Macros.Color.content)
	elseif reward_config.param_flag == 1 and reward_config.reward_type ~= 16 then	--一个数值
		Util:WidgetLabel(self:GetWidget("Text_des"),string.format(reward_config.desc,data.value),Macros.TypefaceSize.minimum,Macros.Color.content)
	elseif reward_config.param_flag == 2 then	--一个字符串
		if data.type == 2 or data.type == 3 then
			if type(data.value) == 'number' then
				local monster_Config = ObjIDTool.GetBattleRepresentativeMonsterConfig(data.value)
				data.text = monster_Config.name
			end
		end
		Util:WidgetLabel(self:GetWidget("Text_des"),string.format(reward_config.desc,data.text),Macros.TypefaceSize.minimum,Macros.Color.content)
	elseif reward_config.param_flag == 3 then	--先字符串
		Util:WidgetLabel(self:GetWidget("Text_des"),string.format(reward_config.desc,data.text,data.value),Macros.TypefaceSize.minimum,Macros.Color.content)
	elseif reward_config.param_flag == 4 then	--先数值		
		Util:WidgetLabel(self:GetWidget("Text_des"),string.format(reward_config.desc,data.value,data.text),Macros.TypefaceSize.minimum,Macros.Color.content)
	end
	
	--容器
	local Panel_ScrollView = self:GetWidget("Panel_ScrollView")
	local params = {
		item_class	= Modules.TemplAcceptPrizeCenterItemIconCell,
	    item_width	= 80,
	    item_height	= 80,
	    horizon = true,
	    row	= 1,
	    col = 1,
	    item_space_c = 10,
	    view_width = 330,
	    view_height = 80,
	}
	self.ScrollView = GUI.ScrollView.New(params)
	Panel_ScrollView:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)
	self.ScrollView:SetDataList(self.__data.dataList)
end

function Modules.TemplAcceptPrizeCenterCell:__delete()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end	
end

--头像
Modules = Modules or {}

Modules.TemplAcceptPrizeCenterItemIconCell = Modules.TemplAcceptPrizeCenterItemIconCell or BaseClass(GUI.ScrollItem)

function Modules.TemplAcceptPrizeCenterItemIconCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplAcceptPrizeCenterItemIconCell:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,80)
  self.layout:SetClippingEnable(false)
  self.layout:SetTouchEnabled(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TemplAcceptPrizeCenterItemIconCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	 --头像
    self.icon = LogicGUI.IconLayer.New()
    self.layout:AddChild(self.icon:GetNode())
    PosTool.Center(self.icon)  
    self.icon:SetData({id = data.info_id, num = data.item_num})
    self.icon:SetTouchSwallow(false)
    self.icon:adaptParentContentSize()
end