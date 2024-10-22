LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresOneKeyView = LogicGUI.TemplPlunderedTreasuresOneKeyView or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresOneKeyView:__init(data)
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(data)
	end	
	self:__AddPreloadTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_REPORT)
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_REPORTVIEW)
end

function LogicGUI.TemplPlunderedTreasuresOneKeyView:__RegistAllEvents()
	--使用物品回调
	self:BindGlobalEvent(Macros.Event.PackModule.USE_ITEM_SUCCESS, function(_,info)
		if info.item_id == Macros.Game.SpecialItem.VIGOR then
			self:CreatePlunderedSpecialMessage(CPPGameLib.GetString("plunderedtreasures_onekey_plundered_auto_cost_tips"))
			--请求扫荡
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresFiveTimesReq",{treasure_id = self.__data.treasure_id,treasure_piece_id = self.__treasure_piece_id})
		end
	end)
	--夺宝回调
	self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_FIVETIMES, function(_,info)
		self.__count_time = self.__count_time + 1
		self.__times_data = {}
		self.__times_data.times = self.__count_time
		self.__times_data.item_id = info.item_list[1].item_id
		self.__times_data.item_num = info.item_list[1].item_num
		self.__times_data.treasure_item_id = self.__treasure_piece_id
		self.__times_data.flag = info.flag
		self:ShowPlunderedMessage()
	end)
end

function LogicGUI.TemplPlunderedTreasuresOneKeyView:InitUI(data)
	self.__data = data
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_reportTitleText"), Macros.TypefaceSize.popup , Macros.Color.btn)	
	--关闭界面的函数
	local function closeView()
		self:DeleteMe()
		self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHVIEW)
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
	self.__txt_cleaning = self:GetWidget("Text_Cleaning")
	Util:WidgetLabel(self.__txt_cleaning,CPPGameLib.GetString("plunderedtreasures_reportCleaning"), Macros.TypefaceSize.largish,Macros.Color.content)
	
	local scrollview_panel = self:GetWidget("Panel_reportScrollView")
	scrollview_panel:SetVisible(false)

	self.__panel_list = {}
	--列表容器
	self.__panel_listview = self:GetWidget("ListView")
	self.__panel_listview:SetVisible(true)
				
	self.__count_time = 0
	local is_have_plunderedfinish
	is_have_plunderedfinish,self.__treasure_piece_id = self:JudgeIsHavePlunderedFinish()
	print("ldx","self.__treasure_piece_idself.__treasure_piece_id: ",self.__treasure_piece_id,self.__data.treasure_id)
	--请求扫荡
	if not is_have_plunderedfinish then
		CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresFiveTimesReq",{treasure_id = self.__data.treasure_id,treasure_piece_id = self.__treasure_piece_id})
	end
end

function LogicGUI.TemplPlunderedTreasuresOneKeyView:ShowPlunderedMessage()
	local is_have_plunderedfinish
	is_have_plunderedfinish,self.__treasure_piece_id = self:JudgeIsHavePlunderedFinish()
	--判断是否已经夺取完毕
	if is_have_plunderedfinish then
		self:CreatePlunderedMessage(true)
		local type,config_info = ObjIDTool.GetInfo(self.__data.treasure_id)
		local treasure_name = string.format(Macros.Quality_RT[config_info.quality or 1],config_info.name or "")
		self:CreatePlunderedSpecialMessage(CPPGameLib.GetString("plunderedtreasures_onekey_plundered_congratrulation_tips",treasure_name))
	else
		--判断精力是否足够
		if ConfigAdapter.Common.GetPlunderedTreasureCostEnery() > CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY) then						
			--是否自动用药
			if self.__data.is_auto_use then
				--精力数量
				local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", Macros.Game.SpecialItem.VIGOR)
				if item_num and item_num ~= 0 then
					self:CreatePlunderedMessage(false,true,true)
					local info = {}
					info.item_id = Macros.Game.SpecialItem.VIGOR
					info.item_num = 1
					info.select_gift_id = 0
					CallModuleFunc(ModuleType.Pack, "ItemUseReq",info)
				else	
					self:CreatePlunderedMessage(false,true)			
					self:CreatePlunderedSpecialMessage(CPPGameLib.GetString("plunderedtreasures_onekey_plundered_auto_stop_tips"))			
				end
			else
				self:CreatePlunderedMessage(false,true)
				self:CreatePlunderedSpecialMessage(CPPGameLib.GetString("plunderedtreasures_onekey_plundered_auto_stop_tips"))				
			end
		else
			self:CreatePlunderedMessage()			
		end
	end
end


--创建新扫荡信息
function LogicGUI.TemplPlunderedTreasuresOneKeyView:CreatePlunderedMessage(is_finish,is_no_energy,is_pass)
	local panel = GUI.TemplPlunderedTreasuresOneKeyCell.New(self.__times_data)
	table.insert(self.__panel_list,panel)
	self.__panel_listview:PushBackCustomItem(panel:GetNode())
	self.__panel_listview:ScrollToBottom()
	if not is_pass then
		if is_finish or is_no_energy then
			--扫荡完成
			self.__button_cleandone:SetVisible(true)
			self.__txt_cleaning:SetVisible(false)
			print("ldx","is_finishis_finishis_finishis_finish ",is_finish)
		elseif not is_no_energy then
			CPPActionManager.DelayTo(self.__button_cleandone, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
				print("ldx","self.__count_time:  ",self.__count_time)
				CPPActionManager.StopAllActions(self.__button_cleandone)
				--请求扫荡
				CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresFiveTimesReq",{treasure_id = self.__data.treasure_id,treasure_piece_id = self.__treasure_piece_id})
			end))		
		end
	end
end

--创建新扫荡特殊信息
function LogicGUI.TemplPlunderedTreasuresOneKeyView:CreatePlunderedSpecialMessage(lab)
	local layout = Util:Layout(422, 35)
	local rich_txt = Util:RichText(lab, Macros.TypefaceSize.largish, 0, 0, Macros.Color.content_hex)
	layout:AddChild(rich_txt)
	PosTool.Center(rich_txt)
	self.__panel_listview:PushBackCustomItem(layout)
	self.__panel_listview:ScrollToBottom()
end

function LogicGUI.TemplPlunderedTreasuresOneKeyView:__delete()
	if self.__show_report_event then
		GlobalTimerQuest:CancelQuest(self.__show_report_event)
		self.__show_report_event = nil
	end
	--删除控件
	for i,panel in ipairs(self.__panel_list) do
		panel:DeleteMe()
	end
end

--判断是否已经夺取完毕
function LogicGUI.TemplPlunderedTreasuresOneKeyView:JudgeIsHavePlunderedFinish()	
	local info = CPPGameLib.GetConfig("TreasureCompose",self.__data.treasure_id)
	local allHave = true
	local treasure_piece_id
	for i,item_id in ipairs(info.cost_item_list) do
		local item_info = CallModuleFunc(ModuleType.Pack,"GetCommonGoods",item_id)
		if not item_info then
			allHave = false
			treasure_piece_id = item_id
			break
		end				
	end
	print("ldx","")
	return allHave,treasure_piece_id
end