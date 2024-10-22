LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresReportView = LogicGUI.TemplPlunderedTreasuresReportView or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresReportView:__init(data)
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(data)
	end	
	self:__AddPreloadTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_REPORT)
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_REPORTVIEW)
end

function LogicGUI.TemplPlunderedTreasuresReportView:InitUI(data)
	self.__data = data
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_reportTitleText"), Macros.TypefaceSize.popup , Macros.Color.btn)	
	--关闭界面的函数
	local function closeView()
		self:DeleteMe()
		if data.flag == 1 then
			local data = {getPiece = true}
			self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHVIEW,data)
			self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW,true,true)
		end
	end
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		closeView()
	end)
	--扫荡完成
	self.__button_cleandone = self:GetWidget("Button_CleanDone")
	self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_cleandone)
	PosTool.Center(effect)
	Util:WidgetButton(self.__button_cleandone, CPPGameLib.GetString("plunderedtreasures_reportSaoDangFinish"),function ()
		closeView()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)	
	self.__button_cleandone:SetVisible(false)
	--扫荡中
	Util:WidgetLabel(self:GetWidget("Text_Cleaning"),CPPGameLib.GetString("plunderedtreasures_reportCleaning"), Macros.TypefaceSize.largish,Macros.Color.content)
	
	local ScrollViewPanel = self:GetWidget("Panel_reportScrollView")
	local params = {
		item_class	= Modules.TemplPlunderedTreasuresReportCell,
	    item_width	= 422,
	    item_height	= 149,
	    row	= 3,
	    col	= 1,
	    view_width = 422,
	    view_height = 347,
	}
	self.ScrollView = GUI.ScrollView.New(params)
	ScrollViewPanel:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)
	self.resetData = {}
	for i,itemInfo in ipairs(data.item_list) do
		local fiveTimesData = {}
		fiveTimesData.times = i
		fiveTimesData.item_id = itemInfo.item_id
		fiveTimesData.item_num = itemInfo.item_num
		fiveTimesData.treasure_item_id = data.treasure_item_id
		if data.flag == 1 then
			if i == #data.item_list then
				fiveTimesData.flag = 1
			else
				fiveTimesData.flag = 0
			end
		else
			fiveTimesData.flag = 0
		end				
		table.insert(self.resetData,fiveTimesData)
	end	
	self.__countTime = 0
	self.__realTimes = #data.item_list
	--[[
	for i=1,self.__realTimes do
		local item = self.ScrollView:GetItem(i)
		item:GetNode():SetPosition(-item:GetNode():GetContentWidth(),item:GetNode():GetPositionY())
	end --]]
	if not self.__ShowReportEvent then
		self.__ShowReportEvent = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
			function()
			self:showAction()
			end), 0.5, -1)
	end
	self:showAction()
end

function LogicGUI.TemplPlunderedTreasuresReportView:showAction()
	self.__countTime = self.__countTime + 1
	self.ScrollView:AttachItem(self.resetData[self.__countTime])	
	self.ScrollView:LocateToLast()
	--[[
	local item = self.ScrollView:GetItem(self.__countTime)
	CPPActionManager.MoveBy(item:GetNode(),0.3,item:GetNode():GetContentWidth(),0,nil) --]]
	if self.__countTime == self.__realTimes then		
		self.__button_cleandone:SetVisible(true)
		self:GetWidget("Text_Cleaning"):SetVisible(false)
		if self.__ShowReportEvent then
			GlobalTimerQuest:CancelQuest(self.__ShowReportEvent)
			self.__ShowReportEvent = nil
		end
		if self.__data.flag == 0 and self.__countTime < 5 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_treasure_clean_tips"))
		end
	end
end

function LogicGUI.TemplPlunderedTreasuresReportView:__delete()
	if self.__ShowReportEvent then
		GlobalTimerQuest:CancelQuest(self.__ShowReportEvent)
		self.__ShowReportEvent = nil
	end
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end