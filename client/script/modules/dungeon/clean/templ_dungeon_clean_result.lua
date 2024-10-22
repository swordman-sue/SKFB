
Modules = Modules or {}

--副本扫荡累计结果条目
Modules.TemplDungeonCleanResult = Modules.TemplDungeonCleanResult or BaseClass(GUI.Template)

local CleanResult = 
{
	NORMAL = 0,
	FIND_REBEL = 1,
	HERO_BAG_FULL = 2,
	EQUIP_BAG_FULL = 3,
}

function Modules.TemplDungeonCleanResult:__init(data, clean_times_req, is_level_change)
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__clean_times_req = clean_times_req
	self.__clean_times_index = 0
	self.__drop_data_list = {}
	self.__is_auto_load = true
	self.__level_change = is_level_change
	self.is_auto_close = false
	
	self.__open_callback = function()
		Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("dungeon_clean_title"), Macros.TypefaceSize.popup, Macros.Color.btn)

		WidgetUnity.SetWidgetListener(self:GetWidget("Button_close"), nil, nil, function()
			self.__is_on_close = true
			if self.__is_on_sendmessage then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_clean_tips"))
				return
			end
			CallModuleFunc(ModuleType.DUNGEON, "CloseCleanView")
		end)

		self.__panel_btns = self:GetWidget("Panel_Buttons")

		--扫荡
		local str_clean_times = CPPGameLib.GetString("dungeon_clean_times", ConfigAdapter.Common.GetMainDungeonCleanDefaultTimes())
		self.__btn_clean = Util:WidgetButton(self:GetWidget("Button_Clean"), str_clean_times, function()
			CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelClean", self.__data.param1, self.__data.param2, self.__data.fb_type)
		end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_red)

		--扫荡完成 
		self.__btn_clean_done = Util:WidgetButton(self:GetWidget("Button_CleanDone"), CPPGameLib.GetString("dungeon_clean_done"), function()
			CallModuleFunc(ModuleType.DUNGEON, "CloseCleanView")
		end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)

		--扫荡中
		self.__txt_cleaning = Util:WidgetLabel(self:GetWidget("Text_Cleaning"), CPPGameLib.GetString("dungeon_cleaning"), Macros.TypefaceSize.normal, Macros.Color.content)

		--扫荡奖励
		self.__list_result = self:GetWidget("ListView_ResultList")
		self.__list_result:SetItemsMargin(4)

		--设置相关数据
		self:__InitData(data)
	end

	self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_CLEAN_RESULT_ITEM)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_CLEAN_RESULT_TOTAL_ITEM)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CLEAN)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CLEAN,true)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_CLEAN_RESULT)
end

function Modules.TemplDungeonCleanResult:__delete()	
	CPPActionManager.StopAllActions(self.__btn_clean_done)
	
	self:__StopShowTimer()	
	self:__UnRegistAllEvents()
	self:__UpdateGameLevelData()
end

function Modules.TemplDungeonCleanResult:__InitData(data, clean_times_req, is_reset_clean_up)
	self:SetData(data)
	--如果是首次扫荡或上次扫荡结果为（正常、背包满），则重新刷结果
	if self.__data and is_reset_clean_up then
		self.__clean_times_req = clean_times_req
		self.__clean_times_index = 0
		self.__drop_data_list = {}
		self.__list_result:RemoveAllItems()
	end
	if self.__data.fb_type == Macros.Game.FBType.DAILY_DUNGEON then
		self.__data.drop_item_list = {}
		local config_daliy = CPPGameLib.GetConfig("DailyFBReward",self.__data.param1)
		if config_daliy then
			local daliy_list = config_daliy[self.__data.param2]
			if daliy_list then
				for daliy_key,v in pairs(daliy_list) do
					if type(daliy_key) == 'number' then
						if v.is_sweep_reward and v.is_sweep_reward == Macros.Global.TRUE then
							for index,cur_item_id in ipairs(v.item_id_list) do
								table.insert(self.__data.drop_item_list,{item_id = cur_item_id,item_num = v.item_num_list[index] or 0})
							end
							break
						end
					end
				end
			end
		end		
	end
	table.insert(self.__drop_data_list,self.__data.drop_item_list)
	self.__panel_btns:SetVisible(false)
	self.__txt_cleaning:SetVisible(true)	

    --self:__StartShowTimer()
    self:__ShowResult()
end

function Modules.TemplDungeonCleanResult:__ShowResult()
	self.__is_on_sendmessage = false
	self.__clean_times_index = self.__clean_times_index + 1
	local _gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(self.__data.param1, self.__data.param2, self.__data.fb_type)
	--单次结果
	local result_item = Modules.TemplDungeonCleanResultItem.New(
	{
		clean_times_index = self.__clean_times_index,
		gamelevel_id = _gamelevel_id,
		is_level_up = self.__level_change,
		drop_item_list = self.__data.drop_item_list,
		fb_type = self.__data.fb_type,
	})
	self.__list_result:PushBackCustomItem(result_item:GetNode())
	self.__list_result:ScrollToBottom()

	--判断背包是否已满
	local gamelevel_info = CPPGameLib.GetConfig("GameLevel",_gamelevel_id)
	local is_herobag_no_full,is_equipbag_no_full,is_treasurebag_no_full = true,true,true 
	if gamelevel_info.need_hero_bag_num then
		CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.HERO, gamelevel_info.need_hero_bag_num,false,function ( sum )
        	is_herobag_no_full = sum >= gamelevel_info.need_hero_bag_num
        	if not is_herobag_no_full then
        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_chapter_clean_herobagfull_tips"))
        	end
    	end)
	end	
	if gamelevel_info.need_equipment_bag_num then
		CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.EQUIP, gamelevel_info.need_equipment_bag_num,true,function ( sum )
        	is_equipbag_no_full = sum >= gamelevel_info.need_equipment_bag_num
        	if not is_equipbag_no_full then
        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_chapter_clean_equipbagfull_tips"))
        	end
    	end)
	end		
	if gamelevel_info.need_treasure_bag_num then
		CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.TREASURE, gamelevel_info.need_treasure_bag_num,true,function ( sum )
        	is_treasurebag_no_full = sum >= gamelevel_info.need_treasure_bag_num
        	if not is_treasurebag_no_full then
        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_chapter_clean_treasurebagfull_tips"))
        	end
    	end)
	end	
	--是否已满
	local is_no_full = is_herobag_no_full and is_equipbag_no_full and is_treasurebag_no_full or false
	if not is_no_full then
		self:__TotalResult(_gamelevel_id)
		return
	end

	--升级中断
	if self.__level_change then		
		self:__TotalResult(_gamelevel_id)
		self.__level_change = false
		return
	end

	--背包空间满
	if self.__data.result == CleanResult.HERO_BAG_FULL or 
		self.__data.result == CleanResult.EQUIP_BAG_FULL then
		self:__TotalResult(_gamelevel_id)

	--遇到叛军			
	elseif self.__data.result == CleanResult.FIND_REBEL then
		CallModuleFunc(ModuleType.REBEL, "ShowRebelLayer", function(is_attack)
			if is_attack then
				--立即攻击
				CallModuleFunc(ModuleType.DUNGEON, "CloseCleanView")
			else
				CallModuleFunc(ModuleType.DUNGEON, "IsSureFramePopUp",false)
				--延迟攻击
				if self.__clean_times_index < self.__clean_times_req then
					CPPActionManager.DelayTo(self.__btn_clean_done, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()						
						if not self.__is_on_close then	
							self.__is_on_sendmessage = true						
							CallModuleFunc(ModuleType.DUNGEON, "FBCleanReq",nil,self.__data.param1, self.__data.param2,self.__data.fb_type)
						else
							self:__TotalResult(_gamelevel_id)
						end
					end))	
				else
					CPPActionManager.DelayTo(self.__btn_clean_done, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
						self:__TotalResult(_gamelevel_id)
					end))
				end
			end
		end)	

	--可继续扫荡
	else
		if self.__clean_times_index < self.__clean_times_req then
			CPPActionManager.DelayTo(self.__btn_clean_done, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()							
				if not self.__is_on_close then
					self.__is_on_sendmessage = true	
					CallModuleFunc(ModuleType.DUNGEON, "FBCleanReq",nil,self.__data.param1, self.__data.param2,self.__data.fb_type)
				else
					self:__TotalResult(_gamelevel_id)
				end
			end))	
		else
			CPPActionManager.DelayTo(self.__btn_clean_done, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
				self:__TotalResult(_gamelevel_id)
			end))				
		end					
	end		
end

function Modules.TemplDungeonCleanResult:__TotalResult(_gamelevel_id)
	local result_total_item = Modules.TemplDungeonCleanResultTotalItem.New(
	{
		clean_times = self.__clean_times_index,
		gamelevel_id = _gamelevel_id,
		is_level_up = self.__level_change,
		drop_data_list = self.__drop_data_list,
		fb_type = self.__data.fb_type,
	})		
	self.__list_result:PushBackCustomItem(result_total_item:GetNode())
	self.__list_result:ScrollToBottom()
	self.__panel_btns:SetVisible(true)
	self.__txt_cleaning:SetVisible(false)
	self.__btn_clean:SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	if self.__data.fb_type == Macros.Game.FBType.DAILY_DUNGEON then
		self.__btn_clean_done:SetAnchorPoint(0.5,0.5)
		PosTool.Center(self.__btn_clean_done)
	end
	local clean_times, times_by_stamina, times_by_battle_times = CallModuleFunc(ModuleType.DUNGEON, "GetGameLevelCanCleanTimes",self.__data.param1 , self.__data.param2 , self.__data.fb_type  )
	
	if times_by_battle_times > 0 then
		local str_clean_times = CPPGameLib.GetString("dungeon_clean_times", math.min(times_by_battle_times,10) )
		self.__btn_clean:SetText(str_clean_times)
		WidgetUnity.SetWidgetListener(self.__btn_clean, nil, nil, function()			
			local is_clean_again = CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelClean", self.__data.param1, self.__data.param2, self.__data.fb_type)
			if is_clean_again then
				self.__is_on_sendmessage = true
				self.__is_on_close = false
			end
		end)
	else
		self.__btn_clean:SetText(CPPGameLib.GetString("dungeon_reset"))
		WidgetUnity.SetWidgetListener(self.__btn_clean, nil, nil, function()
			--关闭扫荡界面
			CallModuleFunc(ModuleType.DUNGEON, "CloseCleanView")
			local lst = CallModuleFunc(ModuleType.DUNGEON, "GetGameLevelInfo", self.__data.param1, self.__data.param2, self.__data.fb_type)
			local gold , may = CallModuleFunc(ModuleType.DUNGEON, "GetRestTimes", lst ,self.__data.fb_type)
			if may ~= 0 then
				--重置次数
				self:Feasible(lst)
			else
				--没有次数
				self:NoFeasible(lst)	
			end
		end)	
	end
	--背包已满提示
	if self.__data.result == CleanResult.HERO_BAG_FULL then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_clean_hero_bag_full"))
		return
	end
	if self.__data.result == CleanResult.EQUIP_BAG_FULL then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_clean_equip_bag_full"))
		return
	end
end

function Modules.TemplDungeonCleanResult:__StartShowTimer()
	self:__StopShowTimer()

    self.__show_result_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
    	function()
    		self:__ShowResult()
		end), 0.5, -1)
end

function Modules.TemplDungeonCleanResult:__StopShowTimer()
	if self.__show_result_timer then
		GlobalTimerQuest:CancelQuest(self.__show_result_timer)
		self.__show_result_timer = nil
	end
end

function Modules.TemplDungeonCleanResult:__RegistAllEvents()
	--升级中断
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function(_,data)
		self.__level_change = true
	end)
	--扫荡回调
	self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_CLEAN_RESP, function(_,resp_data, clean_times_req, is_reset_clean_up)
		self:__InitData(resp_data, clean_times_req, is_reset_clean_up)
	end)
	-- 重置
    self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_MAINLINE_RESET,function (__,data)
    	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("dungeon_resetSucceed") )
    	CallModuleFunc(ModuleType.DUNGEON, "CloseCleanView")
    end)
end

function Modules.TemplDungeonCleanResult:__UnRegistAllEvents()
end

function Modules.TemplDungeonCleanResult:Feasible( gamelevel_info )
	local gold , may = CallModuleFunc(ModuleType.DUNGEON, "GetRestTimes", gamelevel_info,self.__data.fb_type)
	local userGold = CallModuleFunc(ModuleType.ROLE , "GetPropValue", Macros.Game.RolePropType.DIAMOND )
	local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= " ",
													cancel_text = CPPGameLib.GetString("common_cancel") ,
													confirm_func = function ( ... )
														if gold <= userGold then
															local info = {}
															info.map_type = self.__data.fb_type
															info.param1 = self.__data.param1
															info.param2 = self.__data.param2
															CallModuleFunc(ModuleType.DUNGEON, "FBResetReq",info)
														else
															GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("currency_insufficient_" .. Macros.Game.RolePropType.DIAMOND) )
														end
													end,
													cancel_func = function ( ... )
													end,
												} )

	tips:SetLoadedCallback( function ( ... )
		local text = 
			{
				{ zi = CPPGameLib.GetString("dungeon_resetTips",gold), ys = Macros.Color.content , dx = Macros.TypefaceSize.largish,jg = -70},
				{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND),dx = 0.8,jg = -140},
			}
		local Affirm = Util:LabCenter(text)
		Affirm:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 40 )
		tips:GetNode():AddChild(Affirm)

		local arena_today_buyTimes = Util:Label( CPPGameLib.GetString("tower_buyTimes",may) , Macros.TypefaceSize.normal , Macros.Color.red )
		arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
		arena_today_buyTimes:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 40 )
		tips:GetNode():AddChild(arena_today_buyTimes)
	end )

end

function Modules.TemplDungeonCleanResult:NoFeasible(gamelevel_info)
	local gold , may = CallModuleFunc(ModuleType.DUNGEON, "GetRestTimes" , gamelevel_info,self.__data.fb_type)
	local userGold = CallModuleFunc(ModuleType.ROLE , "GetPropValue", Macros.Game.RolePropType.DIAMOND )

	local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= " ",
													confirm_text = CPPGameLib.GetString("common_pay") ,
													confirm_func = function ( ... )
														CallModuleFunc(ModuleType.RECHARGE, "OpenView", BaseViewType.RECHARGE)
													end,
													cancel_text = CPPGameLib.GetString("common_noPay") ,
													cancel_func = function ( ... )
														print("----2")
													end,
												} )

	tips:SetLoadedCallback( function ( ... )
		local userVip = CallModuleFunc(ModuleType.ROLE , "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL )
		local dungeon_resetMax = Util:Label( CPPGameLib.GetString("dungeon_resetMax") , nil , Macros.Color.content )
		dungeon_resetMax:SetPosition( tips.txt_content:GetContentWidth()/2 , (tips.txt_content:GetContentHeight()/2) + (dungeon_resetMax:GetContentHeight()/2 ) + 2 )
		tips.txt_content:AddChild(dungeon_resetMax)

		local dungeon_resetVipTips = Util:Label( CallModuleFunc(ModuleType.DUNGEON, "GetRestNewVipInfo") , nil , Macros.Color.content )
		dungeon_resetVipTips:SetPosition( tips.txt_content:GetContentWidth()/2 , (tips.txt_content:GetContentHeight()/2) - (dungeon_resetVipTips:GetContentHeight()/2) - 2 )
		tips.txt_content:AddChild(dungeon_resetVipTips)
	end)
end

function  Modules.TemplDungeonCleanResult:__UpdateGameLevelData()
	self:FireNextFrame(Macros.Event.DungeonModule.UPDATE_DUNGEON_GAMELEVEL_DATA,self.__data.param2)
end