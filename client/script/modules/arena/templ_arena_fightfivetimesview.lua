LogicGUI =LogicGUI or {}

LogicGUI.TemplArenaFightFiveTimesView = LogicGUI.TemplArenaFightFiveTimesView or BaseClass(GUI.Template)

function LogicGUI.TemplArenaFightFiveTimesView:__init(head_icon,fb_type,target_id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
		self.__enemy_icon = head_icon
		self.__fb_type = fb_type
		self.__target_id = target_id
		self:CreateUI()
	end
	-- self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_FIGHT_FIVETIMES_CELL)
	self:InitTemplate(GUI.TemplateRegistry.ARENA_FIGHT_FIVETIMES_VIEW)	
end

function LogicGUI.TemplArenaFightFiveTimesView:CreateUI()	
	--条目容器
	self.__panel_cell_list = {}
	--条目索引
	self.__cur_item_index = 0
	--标题
	Util:WidgetLabel(self:GetWidget("Text_Title"),CPPGameLib.GetString("arena_fight_moretimes"),26,Macros.Color.btn)
	--扫荡进行中
	self.__text_cleaning = self:GetWidget("Text_Cleaning")
	Util:WidgetLabel(self.__text_cleaning, CPPGameLib.GetString("arena_onfighting_text"), Macros.TypefaceSize.normal, Macros.Color.content )
	--按钮容器
	self.__panel_buttons = self:GetWidget("Panel_Buttons")
	self.__panel_buttons:SetVisible(false)
	--再战五次按钮
	Util:WidgetButton(self:GetWidget("Button_Clean"), CPPGameLib.GetString("arena_fight_fivetimes"), function ()
		self:JudgeIsCouldFightAgain(true)
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--扫荡完成按钮
	Util:WidgetButton(self:GetWidget("Button_CleanDone"), CPPGameLib.GetString("arena_fight_fivetimes_done"), function ()
		CallModuleFunc(ModuleType.BATTLE,"StopQuickBattle",CPPGameLib.GetString("arena_onfighting_tips"),function ()
			-- --模型动，气泡动
   --  		self:FireNextFrame(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_ACTION_RESP,true)
			self:DeleteMe()
		end)
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"", function ()
		self.__player_stop_action = true
		CallModuleFunc(ModuleType.BATTLE,"StopQuickBattle",CPPGameLib.GetString("arena_onfighting_tips"),function ()
			-- --模型动，气泡动
   --  		self:FireNextFrame(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_ACTION_RESP,true)
			self:DeleteMe()
		end)		
	end)
	--列表容器
	self.__listview_resultlist = self:GetWidget("ListView_ResultList")
	--绑定回调挑战开始事件
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_START,function ()
		--设置条目数据
		self.__panel_cell_list[self.__cur_item_index]:RunEffect()
	end)
	--绑定回调挑战回调事件
	self:BindGlobalEvent(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_RESP,function (_,is_win, reward_map, card_data)
		--设置条目数据
		self.__panel_cell_list[self.__cur_item_index]:CallBackData(is_win, reward_map, card_data)
	end)
	--绑定升级回调事件
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE,function ()
		self.__is_levelup_change = true
	end)
	--绑定条目回调事件
	self:BindGlobalEvent(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_CELL_RESP,function ()
		if self.__cur_item_index < 5 and (not self.__player_stop_action) then			
			self:JudgeIsCouldFightAgain()
		else
			if self.__player_stop_action then
				self.__player_stop_action = false
			end
			self.__panel_buttons:SetVisible(true)
			self.__text_cleaning:SetVisible(false)
		end              
	end)
	self:CreateItem()
end

function LogicGUI.TemplArenaFightFiveTimesView:CreateItem()	
	self.__panel_buttons:SetVisible(false)
	self.__text_cleaning:SetVisible(true)
	self.__cur_item_index = self.__cur_item_index + 1
	local fight_cell = GUI.TemplArenaFightFiveTimesCell.New(self.__cur_item_index,self.__enemy_icon,self.__fb_type,self.__target_id)
	fight_cell:SetLoadedCallback(function()
		self.__listview_resultlist:PushBackCustomItem(fight_cell:GetNode())
		self.__listview_resultlist:ScrollToBottom()
		table.insert(self.__panel_cell_list,fight_cell)
	end)
end

function LogicGUI.TemplArenaFightFiveTimesView:JudgeIsCouldFightAgain(is_reset)
	if self.__is_levelup_change then
		self.__player_stop_action = true
		CallModuleFunc(ModuleType.BATTLE,"StopQuickBattle",CPPGameLib.GetString("arena_onfighting_tips"),function ()
			-- --模型动，气泡动
  			--  self:FireNextFrame(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_ACTION_RESP,true)
			self:DeleteMe()
		end)
		self.__is_levelup_change = false
		return
	end
	--判断精力
	local enery = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY)
	if enery < ConfigAdapter.Common.GetArenaCostEnery() then
		local str = is_reset and CPPGameLib.GetString("arena_noEnough_stamina_quick_tips1") or CPPGameLib.GetString("arena_noEnough_stamina_quick_tips")
		GlobalTipMsg.GetInstance():Show(str)
		LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.ENERGY )
		self.__panel_buttons:SetVisible(true)
		self.__text_cleaning:SetVisible(false)
		return
	end
	if is_reset then
		self.__listview_resultlist:RemoveAllItems()
		--条目容器
		self.__panel_cell_list = {}
		--条目索引
		self.__cur_item_index = 0
	end
	self:CreateItem()
end